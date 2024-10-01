#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <string>

// FileSystemWatcher s�n�f�, bir dizindeki dosya de�i�ikliklerini izlemek i�in kullan�l�r
class FileSystemWatcher
{
public:
    // Constructor: �zlenecek dizin yolu al�narak s�n�f ba�lat�l�r
    FileSystemWatcher(const std::wstring& dir);

    // �zlemeyi ba�latan fonksiyon
    void StartWatching();

private:
    // �zlenen dizin yolu
    std::wstring directory;

    // Dizin izleme i�lemini ger�ekle�tiren fonksiyon
    void WatchDirectory();
};

#endif
