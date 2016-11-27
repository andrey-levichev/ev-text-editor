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
    
    ByteArray readBytes();
    void writeBytes(const ByteArray& bytes);
    
    String readString();
    void writeString(const String& str);
    
    int64_t size() const;
    
protected:
#ifdef PLATFORM_WINDOWS
    HANDLE _handle;
#else
    int _handle;
#endif
};

#endif