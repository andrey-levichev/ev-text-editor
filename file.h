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

enum TextEncoding
{
    TEXT_ENCODING_UTF8,
    TEXT_ENCODING_UTF16_LE,
    TEXT_ENCODING_UTF16_BE
};

class File
{
public:
    File();
    File(const String& fileName,
        FileMode openMode = FILE_MODE_READ);

    ~File();

    bool isOpen() const;

    bool open(const String& fileName,
        FileMode openMode = FILE_MODE_READ);

    void close();

    template<typename _Type>
    Buffer<_Type> read()
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

        Buffer<_Type> data(bytesSize / sizeof(_Type));

#ifdef PLATFORM_WINDOWS
        LARGE_INTEGER offset = { { 0 } };
        BOOL rc = SetFilePointerEx(_handle, offset, NULL, FILE_BEGIN);
        ASSERT(rc);

        if (ReadFile(_handle, data.values(), bytesSize, &bytesRead, NULL))
#else
        int rc = lseek(_handle, 0, SEEK_SET);
        ASSERT(rc == 0);

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
    void write(const Buffer<_Type>& data)
    {
        write(data.size(), data.values());
    }

    template<typename _Type>
    void write(int size, const _Type* data)
    {
        ASSERT((data && size > 0) || (!data && size == 0));

        if (_handle == INVALID_HANDLE_VALUE)
            throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
        DWORD bytesSize = size * sizeof(_Type), bytesWritten;

        LARGE_INTEGER offset = { { 0 } };
        BOOL rc = SetFilePointerEx(_handle, offset, NULL, FILE_BEGIN);
        ASSERT(rc);

        if (WriteFile(_handle, data, bytesSize, &bytesWritten, NULL))
#else
        ssize_t bytesSize = size * sizeof(_Type), bytesWritten;

        int rc = lseek(_handle, 0, SEEK_SET);
        ASSERT(rc == 0);

        if ((bytesWritten = ::write(_handle, data, bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesWritten)
                throw Exception(STR("failed to write entire file"));
        }
        else
            throw Exception(STR("failed to write file"));
    }

    String readString(TextEncoding& encoding, bool& bom, bool& crLf);
    void writeString(const String& str, TextEncoding encoding, bool bom, bool crLf);

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
