#include "FileSystemWatcher.h"
#include "TaskSchedulerManager.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <windows.h>
#include <chrono>

// FileSystemWatcher sýnýfýnýn constructor'ý, izlenecek dizini alýr.
FileSystemWatcher::FileSystemWatcher(const std::wstring& dir) : directory(dir) {}

// Önceki görev detaylarýný saklamak için bir map yapýsý
std::unordered_map<std::wstring, std::wstring> taskCache; // Görev isimlerini ve detaylarýný önbelleðe almak için kullanýlýr


void FileSystemWatcher::StartWatching()
{
    std::wcout << L"Monitoring the task scheduler..." << std::endl;
    WatchDirectory(); // Dizini izlemeye baþlar
}

// Dizini izlemek için kullanýlan fonksiyon
void FileSystemWatcher::WatchDirectory()
{
    // Ýzlenecek dizini açar
    HANDLE hDir = CreateFileW(directory.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    
    if (hDir == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: Unable to open directory for watching." << std::endl;
        return;
    }

    // Deðiþiklikler için buffer ve diðer deðiþkenler
    char buffer[1024];
    DWORD bytesReturned;

    TaskSchedulerManager taskManager; 

    // Görev ekleme ve düzenleme olaylarýnýn zamanýný saklamak için harita
    const std::chrono::seconds ignoreInterval(3); // Ayný dosya için 3 saniyelik tekrar kontrol engeli
    std::unordered_map<std::wstring, std::chrono::time_point<std::chrono::steady_clock>> lastModificationTimes;

    while (TRUE)
    {
        // Dizinde yapýlan deðiþiklikleri okur
        if (ReadDirectoryChangesW(hDir, &buffer, sizeof(buffer), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned, NULL, NULL))
        {
            // Deðiþiklik detaylarýný içeren yapý
            FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
            std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));

            // Ýþlemi belirle: Ekleme mi, Silme mi, Düzenleme mi?
            if (pNotify->Action == FILE_ACTION_ADDED)
            {
                // Görev eklendiðinde yapýlacak iþlemler
                std::wstring taskDetails = taskManager.GetTaskDetails(fileName);

                // Görev detaylarýný önbelleðe ekle
                taskCache[fileName] = taskDetails;

               
                std::wofstream logFile("task_changes_log.txt", std::ios::app);
                if (logFile.is_open())
                {
                    logFile << L"Task added: " << fileName << std::endl;
                    logFile << taskDetails << std::endl; // Görev detaylarýný loga ekler
                    logFile.close();
                }

                // Eklenen dosya için zaman kaydet
                lastModificationTimes[fileName] = std::chrono::steady_clock::now();
            }



            else if (pNotify->Action == FILE_ACTION_REMOVED)
            {
                // Görev silindiðinde yapýlacak iþlemler
                std::wofstream logFile("task_changes_log.txt", std::ios::app);
                if (logFile.is_open())
                {
                    
                    SYSTEMTIME st;
                    GetLocalTime(&st);

                    logFile << L"Task deleted: " << fileName << std::endl;
                    logFile << L"Deleted at: "
                        << st.wYear << L"/" << st.wMonth << L"/" << st.wDay << L" "
                        << st.wHour << L":" << st.wMinute << L":" << st.wSecond << std::endl;

                    if (taskCache.find(fileName) != taskCache.end())
                    {
                        logFile << taskCache[fileName] << std::endl; // Silinen görevi logla
                        taskCache.erase(fileName);  // Silinen görevi önbellekten kaldýr
                    }
                    logFile.close();
                }

                // Dosya silindiði için son düzenleme zamanýný da kaldýr
                lastModificationTimes.erase(fileName);
            }
            else if (pNotify->Action == FILE_ACTION_MODIFIED)
            {
                // Ayný dosya kýsa bir süre önce zaten düzenlenmiþ mi kontrol et
                auto it = lastModificationTimes.find(fileName);
                if (it != lastModificationTimes.end())
                {
                    auto timeSinceLastModification = std::chrono::steady_clock::now() - it->second;

                    // Eðer 2 saniyeden kýsa bir süre geçtiyse bu düzenlemeyi atla
                    if (timeSinceLastModification < ignoreInterval)
                    {
                        continue;
                    }
                }

                // Görev düzenlendi
                std::wstring updatedTaskDetails = taskManager.GetTaskDetails(fileName);

               
                std::wofstream logFile("task_changes_log.txt", std::ios::app);
                if (logFile.is_open())
                {
                    SYSTEMTIME st;
                    GetLocalTime(&st);

                    logFile << L"Task modified: " << fileName << std::endl;
                    logFile << L"Modified at: "
                        << st.wYear << L"/" << st.wMonth << L"/" << st.wDay << L" "
                        << st.wHour << L":" << st.wMinute << L":" << st.wSecond << std::endl;

                    logFile << L"" << updatedTaskDetails << std::endl;
                    logFile.close();
                }

                // Önbellekteki görevi güncelle
                taskCache[fileName] = updatedTaskDetails;

                lastModificationTimes[fileName] = std::chrono::steady_clock::now();
            }
        }
    }

    // Dizin izleyici kapanýr
    CloseHandle(hDir);
}
