#ifndef TASKSCHEDULERMANAGER_H
#define TASKSCHEDULERMANAGER_H

#include <string>
#include <Windows.h>
#include <taskschd.h>

class TaskSchedulerManager
{
public:
    void ListTasks();  // Görevleri listeleyen fonksiyon
    std::wstring GetTaskDetails(const std::wstring& taskName);  // Görev detaylarýný alan fonksiyon
};

#endif
