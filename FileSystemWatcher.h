#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <string>

// FileSystemWatcher sýnýfý, bir dizindeki dosya deðiþikliklerini izlemek için kullanýlýr
class FileSystemWatcher
{
public:
    // Constructor: Ýzlenecek dizin yolu alýnarak sýnýf baþlatýlýr
    FileSystemWatcher(const std::wstring& dir);

    // Ýzlemeyi baþlatan fonksiyon
    void StartWatching();

private:
    // Ýzlenen dizin yolu
    std::wstring directory;

    // Dizin izleme iþlemini gerçekleþtiren fonksiyon
    void WatchDirectory();
};

#endif
