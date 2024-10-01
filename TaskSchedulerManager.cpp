#include "TaskSchedulerManager.h"
#include <comdef.h>
#include <iostream>
#include <fstream>
#include <taskschd.h>  // Task Scheduler API
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")



// TaskSchedulerManager s�n�f�ndaki GetTaskDetails fonksiyonu belirli bir g�revin detaylar�n� al�r
std::wstring TaskSchedulerManager::GetTaskDetails(const std::wstring& taskName)
{
    std::wstring taskDetails = L""; // G�rev detaylar�n� tutacak de�i�ken

    // COM ba�latma: Task Scheduler API kullanmak i�in COM ba�lat�l�r
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        return L"Error initializing COM library."; 
    }

    ITaskService* pService = NULL; // ITaskService arabirimi Task Scheduler'a ba�lanmak i�in kullan�l�r
    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
    if (FAILED(hr)) {
        CoUninitialize();
        return L"Error: Could not create an instance of ITaskService."; 
    }

    // Task Scheduler servisine ba�lan�r
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr)) {
        pService->Release();
        CoUninitialize();
        return L"Error: Could not connect to Task Scheduler service."; 
    }

    // K�k klas�r al�n�r
    ITaskFolder* pFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pFolder); // K�k klas�r (root folder) istenir
    if (FAILED(hr)) {
        pService->Release();
        CoUninitialize();
        return L"Error: Could not get the root folder.";  
    }

    // Belirli bir g�rev al�n�r
    IRegisteredTask* pTask = NULL;
    hr = pFolder->GetTask(_bstr_t(taskName.c_str()), &pTask);
    if (SUCCEEDED(hr) && pTask != NULL)
    {
        // G�rev tan�m� al�n�r
        ITaskDefinition* pTaskDef = NULL;
        hr = pTask->get_Definition(&pTaskDef);
        if (SUCCEEDED(hr) && pTaskDef != NULL)
        {
            IRegistrationInfo* pRegInfo = NULL;
            hr = pTaskDef->get_RegistrationInfo(&pRegInfo);

            // Kay�t bilgisi (author, URI, olu�turulma zaman� gibi) al�n�yor
            if (SUCCEEDED(hr) && pRegInfo != NULL)
            {
                // G�rev ID'si
                _bstr_t taskId;
                pRegInfo->get_URI(taskId.GetAddress());

                // G�revi olu�turan kullan�c� bilgisi
                _bstr_t author;
                pRegInfo->get_Author(author.GetAddress());

                // G�revin olu�turulma tarihi
                _bstr_t dateCreated;
                pRegInfo->get_Date(dateCreated.GetAddress());

                // Detaylar� string'e ekleme
                taskDetails = L"Task Name: " + taskName + L"\n" +
                    L"Task ID: " + std::wstring(taskId) + L"\n" +
                    L"Created by: " + std::wstring(author) + L"\n" +
                    L"Creation Date: " + std::wstring(dateCreated) + L"\n";

                pRegInfo->Release();
            }
            pTaskDef->Release(); 
        }
        pTask->Release();
    }
    else {
        
        taskDetails = L"Error: Could not retrieve task details for " + taskName;
    }

    // Kullan�lan COM nesnelerini serbest b�rak
    pFolder->Release();
    pService->Release();
    CoUninitialize();

    return taskDetails; // G�rev detaylar� d�nd�r�l�r
}
