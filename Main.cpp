#include "FileSystemWatcher.h"

int main()
{
	FileSystemWatcher watcher(L"C:\\Windows\\System32\\Tasks");  // İzlenecek dizin
    watcher.StartWatching();  // Görev değişikliklerini izlemeye başla

    return 0;
}
