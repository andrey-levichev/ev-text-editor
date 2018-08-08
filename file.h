#ifndef FILE_INCLUDED
#define FILE_INCLUDED

#include <foundation.h>

// File

#ifndef PLATFORM_WINDOWS
#define INVALID_HANDLE_VALUE -1
#endif

enum FileMode
{
    FILE_MODE_READ = 1,
    FILE_MODE_WRITE = 2,
    FILE_MODE_CREATE = 4,
    FILE_MODE_NEW_ONLY = 8,
    FILE_MODE_TRUNCATE = 16,
    FILE_MODE_APPEND = 32
};

enum FilePosition
{
    FILE_POSITION_START,
    FILE_POSITION_CURRENT,
    FILE_POSITION_END
};

class File
{
public:
    File();
    File(const String& filename, int openMode = FILE_MODE_READ);
    ~File();

    bool isOpen() const;
    bool isExecutable() const;
    int64_t size() const;

    bool open(const String& filename, int openMode = FILE_MODE_READ);
    void close();

    int64_t setPosition(int64_t offset, FilePosition position = FILE_POSITION_START);

    ByteBuffer read(int size = -1);
    void read(int size, void* data);

    void write(const ByteBuffer& data);
    void write(int size, const void* data);

public:
    static bool exists(const String& filename);
    static void remove(const String& filename);

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
