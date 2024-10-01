#include "TaskSchedulerManager.h"
#include <comdef.h>
#include <iostream>
#include <fstream>
#include <taskschd.h>  // Task Scheduler API
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")



// TaskSchedulerManager sýnýfýndaki GetTaskDetails fonksiyonu belirli bir görevin detaylarýný alýr
std::wstring TaskSchedulerManager::GetTaskDetails(const std::wstring& taskName)
{
    std::wstring taskDetails = L""; // Görev detaylarýný tutacak deðiþken

    // COM baþlatma: Task Scheduler API kullanmak için COM baþlatýlýr
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        return L"Error initializing COM library."; 
    }

    ITaskService* pService = NULL; // ITaskService arabirimi Task Scheduler'a baðlanmak için kullanýlýr
    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
    if (FAILED(hr)) {
        CoUninitialize();
        return L"Error: Could not create an instance of ITaskService."; 
    }

    // Task Scheduler servisine baðlanýr
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr)) {
        pService->Release();
        CoUninitialize();
        return L"Error: Could not connect to Task Scheduler service."; 
    }

    // Kök klasör alýnýr
    ITaskFolder* pFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pFolder); // Kök klasör (root folder) istenir
    if (FAILED(hr)) {
        pService->Release();
        CoUninitialize();
        return L"Error: Could not get the root folder.";  
    }

    // Belirli bir görev alýnýr
    IRegisteredTask* pTask = NULL;
    hr = pFolder->GetTask(_bstr_t(taskName.c_str()), &pTask);
    if (SUCCEEDED(hr) && pTask != NULL)
    {
        // Görev tanýmý alýnýr
        ITaskDefinition* pTaskDef = NULL;
        hr = pTask->get_Definition(&pTaskDef);
        if (SUCCEEDED(hr) && pTaskDef != NULL)
        {
            IRegistrationInfo* pRegInfo = NULL;
            hr = pTaskDef->get_RegistrationInfo(&pRegInfo);

            // Kayýt bilgisi (author, URI, oluþturulma zamaný gibi) alýnýyor
            if (SUCCEEDED(hr) && pRegInfo != NULL)
            {
                // Görev ID'si
                _bstr_t taskId;
                pRegInfo->get_URI(taskId.GetAddress());

                // Görevi oluþturan kullanýcý bilgisi
                _bstr_t author;
                pRegInfo->get_Author(author.GetAddress());

                // Görevin oluþturulma tarihi
                _bstr_t dateCreated;
                pRegInfo->get_Date(dateCreated.GetAddress());

                // Detaylarý string'e ekleme
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

    // Kullanýlan COM nesnelerini serbest býrak
    pFolder->Release();
    pService->Release();
    CoUninitialize();

    return taskDetails; // Görev detaylarý döndürülür
}
