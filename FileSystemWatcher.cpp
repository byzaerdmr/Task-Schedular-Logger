#include "FileSystemWatcher.h"
#include "TaskSchedulerManager.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <windows.h>
#include <chrono>

// FileSystemWatcher s�n�f�n�n constructor'�, izlenecek dizini al�r.
FileSystemWatcher::FileSystemWatcher(const std::wstring& dir) : directory(dir) {}

// �nceki g�rev detaylar�n� saklamak i�in bir map yap�s�
std::unordered_map<std::wstring, std::wstring> taskCache; // G�rev isimlerini ve detaylar�n� �nbelle�e almak i�in kullan�l�r


void FileSystemWatcher::StartWatching()
{
    std::wcout << L"Monitoring the task scheduler..." << std::endl;
    WatchDirectory(); // Dizini izlemeye ba�lar
}

// Dizini izlemek i�in kullan�lan fonksiyon
void FileSystemWatcher::WatchDirectory()
{
    // �zlenecek dizini a�ar
    HANDLE hDir = CreateFileW(directory.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    
    if (hDir == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error: Unable to open directory for watching." << std::endl;
        return;
    }

    // De�i�iklikler i�in buffer ve di�er de�i�kenler
    char buffer[1024];
    DWORD bytesReturned;

    TaskSchedulerManager taskManager; 

    // G�rev ekleme ve d�zenleme olaylar�n�n zaman�n� saklamak i�in harita
    const std::chrono::seconds ignoreInterval(3); // Ayn� dosya i�in 3 saniyelik tekrar kontrol engeli
    std::unordered_map<std::wstring, std::chrono::time_point<std::chrono::steady_clock>> lastModificationTimes;

    while (TRUE)
    {
        // Dizinde yap�lan de�i�iklikleri okur
        if (ReadDirectoryChangesW(hDir, &buffer, sizeof(buffer), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned, NULL, NULL))
        {
            // De�i�iklik detaylar�n� i�eren yap�
            FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
            std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));

            // ��lemi belirle: Ekleme mi, Silme mi, D�zenleme mi?
            if (pNotify->Action == FILE_ACTION_ADDED)
            {
                // G�rev eklendi�inde yap�lacak i�lemler
                std::wstring taskDetails = taskManager.GetTaskDetails(fileName);

                // G�rev detaylar�n� �nbelle�e ekle
                taskCache[fileName] = taskDetails;

               
                std::wofstream logFile("task_changes_log.txt", std::ios::app);
                if (logFile.is_open())
                {
                    logFile << L"Task added: " << fileName << std::endl;
                    logFile << taskDetails << std::endl; // G�rev detaylar�n� loga ekler
                    logFile.close();
                }

                // Eklenen dosya i�in zaman kaydet
                lastModificationTimes[fileName] = std::chrono::steady_clock::now();
            }



            else if (pNotify->Action == FILE_ACTION_REMOVED)
            {
                // G�rev silindi�inde yap�lacak i�lemler
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
                        logFile << taskCache[fileName] << std::endl; // Silinen g�revi logla
                        taskCache.erase(fileName);  // Silinen g�revi �nbellekten kald�r
                    }
                    logFile.close();
                }

                // Dosya silindi�i i�in son d�zenleme zaman�n� da kald�r
                lastModificationTimes.erase(fileName);
            }
            else if (pNotify->Action == FILE_ACTION_MODIFIED)
            {
                // Ayn� dosya k�sa bir s�re �nce zaten d�zenlenmi� mi kontrol et
                auto it = lastModificationTimes.find(fileName);
                if (it != lastModificationTimes.end())
                {
                    auto timeSinceLastModification = std::chrono::steady_clock::now() - it->second;

                    // E�er 2 saniyeden k�sa bir s�re ge�tiyse bu d�zenlemeyi atla
                    if (timeSinceLastModification < ignoreInterval)
                    {
                        continue;
                    }
                }

                // G�rev d�zenlendi
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

                // �nbellekteki g�revi g�ncelle
                taskCache[fileName] = updatedTaskDetails;

                lastModificationTimes[fileName] = std::chrono::steady_clock::now();
            }
        }
    }

    // Dizin izleyici kapan�r
    CloseHandle(hDir);
}
