
# Scheduled Task Activity Logger
 [![AGPL License](https://img.shields.io/badge/C++-Project-yellow)](https://dotnet.microsoft.com/en-us/languages/csharp) 
 
![windows-scheduled-tasks-whatsup-gold31adc1fab39b4771b621fbb5c977138c](https://github.com/user-attachments/assets/a975fbc1-f0a7-4edd-9220-b20727ba646b)



This project is designed to monitor and log scheduled task activities on the Windows operating system. The application operates in silent mode, running in the background without any user interface, and detects events related to the creation, modification, and deletion of scheduled tasks. Detailed logs are created for each event.

## Requirements:

* Development Environment: Microsoft Visual Studio or any other C++ IDE.
* Windows API: Used for interacting with the Windows operating system.
* Win32 API: Used for monitoring scheduled tasks and event logs.
* The compiler must run in administrator mode.

## Project Goals
* Silent Mode Operation: The application runs in the background without displaying any user interface.
No icons are shown in the taskbar or notification area.

* Monitoring and Logging Scheduled Tasks:
Tracks and logs task creation, modification, and deletion events.
Logs include the type of action, time, user, and the affected task.


## How It Works

* Directory Monitoring
When the program starts, the FileSystemWatcher class is initialized with a target directory. The StartWatching function begins continuous monitoring of this directory, looking for changes such as file additions, modifications, and deletions.

* Handling Events

File Addition: When a new task file is added to the directory, the system retrieves the task details through the TaskSchedulerManager class and logs this information. Additionally, the task details are cached in memory for quick access.

File Modification: If an existing task file is modified, the program checks if the modification event is valid (i.e., not a duplicate within a short time frame). The new task details are fetched, logged, and the cache is updated with the latest information.

File Deletion: When a task file is deleted, the system logs the deletion event along with the task details (if previously cached). The task is then removed from the cache.

Duplicate Event Prevention:To avoid multiple log entries for the same modification event, a cooldown mechanism is implemented. Each task's last modification timestamp is tracked, and if the same task is modified again within a defined interval (e.g., 2 seconds), the event is ignored.

* Logging
All significant events—whether a task is added, modified, or deleted—are logged into a task_changes_log.txt file. The logs include task details such as the task name, ID, creator, creation time, and the timestamp of the event (addition, modification, or deletion).

* Task Details
The TaskSchedulerManager class is responsible for retrieving detailed information about tasks. This includes details such as the task name, task ID, creation date, and the user who created the task. These details are written to both the cache and the log file when a task is added or modified.


## Example Workflow:
* A new task file is added to the monitored directory.
* The program logs the addition event with detailed task information.
* If the task is later modified, the program fetches the updated details and logs the changes.
* If the task is deleted, the deletion event is logged along with any cached information about the task.


For example, I scheduled Task-2 in the task scheduler. Then I edited Task1- which I created before the program ran. Then I scheduled Task-3 and edited Task-2. Finally I deleted Task-1, Task-2 and Task-3 respectively.

![Ekran görüntüsü 2024-09-13 095709](https://github.com/user-attachments/assets/f886eb58-095e-4281-a801-64480d971980)


## Lessons Learned
* File System Event Handling: I learned that file system events often trigger multiple times for a single action, requiring careful filtering to avoid redundant operations.

* Task Caching: Implementing a cache for task details improved performance and reduced unnecessary data access, highlighting the importance of efficient data management.

* Concurrency Management: Working with real-time file changes taught me the need for synchronization and handling concurrent events without causing race conditions.

* Windows API Experience: Using ReadDirectoryChangesW and other Windows API functions provided valuable insights into system-level programming and file monitoring.

* Logging and Debugging: Proper logging with timestamps helped track and troubleshoot issues more effectively, emphasizing the value of detailed logs in real-time applications.

* Error Handling: I learned the importance of robust error handling, especially when dealing with system resources and potential edge cases like rapid file changes or file locks.




## Conclusion  
This project provided valuable insights into real-time file monitoring and task management. By leveraging the Windows API, I was able to create a system that efficiently tracks and logs file changes, while handling concurrency and avoiding redundant operations. Implementing task caching and robust logging improved both performance and troubleshooting capabilities. The experience gained in error handling, Windows API usage, and file system event management will be beneficial in future projects that involve system-level programming and real-time data processing. Overall, the project was a successful demonstration of integrating low-level system features into a practical application.

