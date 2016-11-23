#ifndef FILE_INCLUDED
#define FILE_INCLUDED

#include <foundation.h>

// File

enum FileMode
{
    FILE_MODE_CREATE_ALWAYS, // create or truncate if exists
    FILE_MODE_CREATE_NEW, // create new or fail if exists
    FILE_MODE_OPEN_ALWAYS, // open if exists or create 
    FILE_MODE_OPEN_EXISTING, // open if exists or fail
    FILE_MODE_TRUNCATE_EXISTING // open existing and truncate or fail
};

class File
{
public:
    File();
    File(const String& fileName,
        FileMode openMode = FILE_MODE_OPEN_EXISTING);
        
    File(const File&) = delete;
    
    ~File();
    
    File& operator=(const File&) = delete;
    
    bool open(const String& fileName, 
        FileMode openMode = FILE_MODE_OPEN_EXISTING);
        
    void close();
    
    Array<uint8_t> readBytes();
    void writeBytes(const Array<uint8_t>& bytes);
    
    String readString();
    void writeString(const String& str);
    
    int64_t size() const;
    
private:
#ifdef PLATFORM_WINDOWS
    HANDLE _handle;
#else
    int _handle;
#endif
};

#endif
