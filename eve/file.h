#ifndef FILE_INCLUDED
#define FILE_INCLUDED

#include <foundation.h>

// File

enum FileMode
{
    FILE_MODE_CREATE_ALWAYS,
    FILE_MODE_CREATE_NEW,
    FILE_MODE_OPEN_ALWAYS,
    FILE_MODE_OPEN_EXISTING,
    FILE_MODE_TRUNCATE_EXISTING
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
    
    ByteBuffer readBytes();
    void writeBytes(const ByteBuffer& bytes);
    
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
