#include <file.h>

// File

File::File() : _handle(INVALID_HANDLE_VALUE)
{
}

File::File(const String& filename, int openMode) : _handle(INVALID_HANDLE_VALUE)
{
    if (!open(filename, openMode))
        throw Exception(STR("failed to open file"));
}

File::~File()
{
    try
    {
        close();
    }
    catch (Exception& ex)
    {
        reportError(ex.message());
    }
    catch (...)
    {
        reportError(STR("unknown error"));
    }
}

bool File::isOpen() const
{
    return _handle != INVALID_HANDLE_VALUE;
}

bool File::isExecutable() const
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    return false;
#else
    struct stat st;
    int rc = fstat(_handle, &st);
    ASSERT(rc == 0);
    return (st.st_mode & S_IXUSR) != 0;
#endif
}

int64_t File::size() const
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER sz;
    BOOL rc = GetFileSizeEx(_handle, &sz);
    ASSERT(rc);
    return sz.QuadPart;
#else
    struct stat st;
    int rc = fstat(_handle, &st);
    ASSERT(rc == 0);
    return st.st_size;
#endif
}

bool File::open(const String& filename, int openMode)
{
    if (_handle != INVALID_HANDLE_VALUE)
        throw Exception(STR("file already open"));

#ifdef PLATFORM_WINDOWS
    DWORD access = 0, disposition = OPEN_EXISTING;

    if ((openMode & FILE_MODE_READ) != 0)
        access |= GENERIC_READ;
    if ((openMode & FILE_MODE_WRITE) != 0)
        access |= GENERIC_WRITE;
    if ((openMode & FILE_MODE_APPEND) != 0)
        access = FILE_APPEND_DATA;

    if (access == 0)
        throw Exception(STR("invalid file mode"));

    if ((openMode & FILE_MODE_CREATE) != 0)
        disposition = OPEN_ALWAYS;
    if ((openMode & FILE_MODE_NEW_ONLY) != 0)
        disposition = CREATE_NEW;
    if ((openMode & FILE_MODE_TRUNCATE) != 0)
        disposition = CREATE_ALWAYS;

    _handle = CreateFile(reinterpret_cast<LPCTSTR>(filename.chars()), access, 0, nullptr, disposition,
                         FILE_ATTRIBUTE_NORMAL, nullptr);
#else
    int mode = 0;

    if ((openMode & FILE_MODE_READ) != 0 && (openMode & FILE_MODE_WRITE) != 0)
        mode = O_RDWR;
    else if ((openMode & FILE_MODE_READ) != 0)
        mode = O_RDONLY;
    else if ((openMode & FILE_MODE_WRITE) != 0)
        mode = O_WRONLY;
    else
        throw Exception(STR("invalid file mode"));

    if ((openMode & FILE_MODE_CREATE) != 0)
        mode |= O_CREAT;
    if ((openMode & FILE_MODE_NEW_ONLY) != 0)
        mode |= O_EXCL;
    if ((openMode & FILE_MODE_APPEND) != 0)
        mode |= O_APPEND;
    if ((openMode & FILE_MODE_TRUNCATE) != 0)
        mode |= O_TRUNC;

    _handle = ::open(filename.chars(), mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#endif

    return _handle != INVALID_HANDLE_VALUE;
}

void File::close()
{
    if (_handle != INVALID_HANDLE_VALUE)
    {
#ifdef PLATFORM_WINDOWS
        BOOL rc = CloseHandle(_handle);
        ASSERT(rc);
#else
        int rc = ::close(_handle);
        ASSERT(rc == 0);
#endif
        _handle = INVALID_HANDLE_VALUE;
    }
}

int64_t File::setPosition(int64_t offset, FilePosition position)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER off, newOff;
    off.QuadPart = offset;

    DWORD pos;
    switch (position)
    {
    case FILE_POSITION_START:
        pos = FILE_BEGIN;
        break;
    case FILE_POSITION_CURRENT:
        pos = FILE_CURRENT;
        break;
    case FILE_POSITION_END:
        pos = FILE_END;
        break;
    default:
        throw Exception(STR("invalid position"));
    }

    BOOL rc = SetFilePointerEx(_handle, off, &newOff, pos);
    ASSERT(rc);

    return newOff.QuadPart;
#else
    int pos;
    switch (position)
    {
    case FILE_POSITION_START:
        pos = SEEK_SET;
        break;
    case FILE_POSITION_CURRENT:
        pos = SEEK_CUR;
        break;
    case FILE_POSITION_END:
        pos = SEEK_END;
        break;
    default:
        throw Exception(STR("invalid position"));
    }

    int newOff = lseek(_handle, offset, pos);
    ASSERT(newOff >= 0);

    return newOff;
#endif
}

ByteBuffer File::read(int size)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = size < 0 ? this->size() : size, bytesRead;
#else
    ssize_t bytesSize = size < 0 ? this->size() : size, bytesRead;
#endif
    ByteBuffer data(bytesSize);

#ifdef PLATFORM_WINDOWS
    if (ReadFile(_handle, data.values(), bytesSize, &bytesRead, nullptr))
#else
    if ((bytesRead = ::read(_handle, data.values(), bytesSize)) >= 0)
#endif
    {
        if (bytesRead < bytesSize)
            data.resize(bytesRead);
    }
    else
        throw Exception(STR("failed to read file"));

    return data;
}

void File::read(int size, void* data)
{
    ASSERT(size >= 0 && data);

    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = size, bytesRead;

    if (ReadFile(_handle, data, bytesSize, &bytesRead, nullptr))
#else
    ssize_t bytesSize = size, bytesRead;

    if ((bytesRead = ::read(_handle, data, bytesSize)) >= 0)
#endif
    {
        if (bytesRead != bytesSize)
            throw Exception(STR("failed to read all data"));
    }
    else
        throw Exception(STR("failed to read file"));
}

void File::write(const ByteBuffer& data)
{
    write(data.size(), data.values());
}

void File::write(int size, const void* data)
{
    ASSERT(data ? size >= 0 : size == 0);

    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = size, bytesWritten;

    if (WriteFile(_handle, data, bytesSize, &bytesWritten, nullptr))
#else
    ssize_t bytesSize = size, bytesWritten;

    if ((bytesWritten = ::write(_handle, data, bytesSize)) >= 0)
#endif
    {
        if (bytesSize != bytesWritten)
            throw Exception(STR("failed to write all data"));
    }
    else
        throw Exception(STR("failed to write file"));
}

bool File::exists(const String& filename)
{
#ifdef PLATFORM_WINDOWS
    return GetFileAttributes(reinterpret_cast<LPCTSTR>(filename.chars())) != INVALID_FILE_ATTRIBUTES;
#else
    return access(filename.chars(), F_OK) == 0;
#endif
}

void File::remove(const String& filename)
{
#ifdef PLATFORM_WINDOWS
    BOOL rc = DeleteFile(reinterpret_cast<LPCTSTR>(filename.chars()));
    if (!rc)
#else
    int rc = unlink(filename.chars());
    if (rc != 0)
#endif
        throw Exception(STR("failed to delete file"));
}
