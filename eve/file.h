#ifndef FILE_INCLUDED
#define FILE_INCLUDED

#include <foundation.h>

// File

#ifndef PLATFORM_WINDOWS
#define INVALID_HANDLE_VALUE -1
#endif

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
        
    ~File();

    bool isOpen() const;
    
    bool open(const String& fileName, 
        FileMode openMode = FILE_MODE_OPEN_EXISTING);
        
    void close();
    
    template<typename _Type>
    Array<_Type> read()
    {
        if (_handle == INVALID_HANDLE_VALUE)
            throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
        DWORD bytesSize = size(), bytesRead;
#else
        ssize_t bytesSize = size(), bytesRead;
#endif
        if (bytesSize % sizeof(_Type) != 0)
            throw Exception(STR("file size must be a multiple of type size"));

        Array<_Type> data(bytesSize / sizeof(_Type));

#ifdef PLATFORM_WINDOWS
        if (ReadFile(_handle, data.values(), bytesSize, &bytesRead, NULL))
#else
        if ((bytesRead = ::read(_handle, data.values(), bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesRead)
                throw Exception(STR("failed to read entire file"));
        }
        else
            throw Exception(STR("failed to read file"));

        return data;
    }

    template<typename _Type>
    void write(const Array<_Type>& data)
    {
        if (_handle == INVALID_HANDLE_VALUE)
            throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
        DWORD bytesSize = data.size() * sizeof(_Type), bytesWritten;

        if (WriteFile(_handle, data.values(), bytesSize, &bytesWritten, NULL))
#else
        ssize_t bytesSize = data.size() * sizeof(_Type), bytesWritten;

        if ((bytesWritten = ::write(_handle, data.values(), bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesWritten)
                throw Exception(STR("failed to write entire file"));
        }
        else
            throw Exception(STR("failed to write file"));
    }
    
    String readString();
    void writeString(const String& str);
    
    int64_t size() const;
   
private:
    File(const File&);
    File& operator=(const File&);

protected:
#ifdef PLATFORM_WINDOWS
    HANDLE _handle;
#else
    int _handle;
#endif
};

#endif
