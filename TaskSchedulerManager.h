#ifndef TASKSCHEDULERMANAGER_H
#define TASKSCHEDULERMANAGER_H

#include <string>
#include <Windows.h>
#include <taskschd.h>

class TaskSchedulerManager
{
public:
    void ListTasks();  // G�revleri listeleyen fonksiyon
    std::wstring GetTaskDetails(const std::wstring& taskName);  // G�rev detaylar�n� alan fonksiyon
};

#endif
