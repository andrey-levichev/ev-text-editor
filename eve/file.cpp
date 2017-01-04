#include <file.h>

// File

#ifndef PLATFORM_WINDOWS
const int INVALID_HANDLE_VALUE = -1;
#endif

File::File() :
    _handle(INVALID_HANDLE_VALUE)
{    
}

File::File(const String& fileName, FileMode openMode) :
    _handle(INVALID_HANDLE_VALUE)
{
    if (!open(fileName, openMode))
        throw Exception(STR("failed to open file"));
}

File::~File()
{
    close();
}

bool File::open(const String& fileName, FileMode openMode)
{
    if (_handle != INVALID_HANDLE_VALUE)
        throw Exception(STR("file already open"));
    
    int mode;
    
#ifdef PLATFORM_WINDOWS
    switch (openMode)
    {
    case FILE_MODE_CREATE_ALWAYS:
        mode = CREATE_ALWAYS;
        break;
    case FILE_MODE_CREATE_NEW:
        mode = CREATE_NEW;
        break;
    case FILE_MODE_OPEN_ALWAYS:
        mode = OPEN_ALWAYS;
        break;
    default:
    case FILE_MODE_OPEN_EXISTING:
        mode = OPEN_EXISTING;
        break;
    case FILE_MODE_TRUNCATE_EXISTING:
        mode = TRUNCATE_EXISTING;
        break;
    }
    
    _handle = CreateFile(fileName.str(), GENERIC_READ | GENERIC_WRITE,
        0, nullptr, mode, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
    switch (openMode)
    {
    case FILE_MODE_CREATE_ALWAYS:
        mode = O_CREAT | O_TRUNC;
        break;
    case FILE_MODE_CREATE_NEW:
        mode = O_CREAT | O_EXCL;
        break;
    case FILE_MODE_OPEN_ALWAYS:
        mode = O_CREAT;
        break;
    default:
    case FILE_MODE_OPEN_EXISTING:
        mode = 0;
        break;
    case FILE_MODE_TRUNCATE_EXISTING:
        mode = O_TRUNC;
        break;
    }
    
    _handle = ::open(fileName.str(), O_RDWR | mode, 
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#endif
    
    return _handle != INVALID_HANDLE_VALUE;
}

void File::close()
{
    if (_handle != INVALID_HANDLE_VALUE)
    {
#ifdef PLATFORM_WINDOWS
        ASSERT_NO_EXCEPTION(CloseHandle(_handle));
#else
        ASSERT(::close(_handle) == 0);
#endif
        _handle = INVALID_HANDLE_VALUE;
    }        
}

ByteArray File::readBytes()
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = size(), bytesRead;
    ByteArray bytes(bytesSize);
    
    if (ReadFile(_handle, bytes.values(), bytesSize, &bytesRead, nullptr))
#else
    ssize_t bytesSize = size(), bytesRead;
    ByteArray bytes(bytesSize);

    if ((bytesRead = read(_handle, bytes.values(), bytesSize)) >= 0)
#endif
    {
        if (bytesSize != bytesRead)
            throw Exception(STR("failed to read entire file"));
    }
    else
        throw Exception(STR("failed to read file"));

    return bytes;
}

void File::writeBytes(const ByteArray& bytes)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    
#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = bytes.size(), bytesWritten;
    
    if (WriteFile(_handle, bytes.values(), bytesSize, &bytesWritten, nullptr))
#else
    ssize_t bytesSize = bytes.size(), bytesWritten;

    if ((bytesWritten = write(_handle, bytes.values(), bytesSize)) >= 0)
#endif
    {
        if (bytesSize != bytesWritten)
            throw Exception(STR("failed to write entire file"));
    }
    else
        throw Exception(STR("failed to write file"));
}

String File::readString()
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

#ifdef PLATFORM_WINDOWS
    DWORD charsSize = size() / sizeof(char_t);
    DWORD bytesSize = charsSize * sizeof(char_t), bytesRead;
    Array<char_t> chars(charsSize + 1);
    
    if (ReadFile(_handle, chars.values(), bytesSize, &bytesRead, nullptr))
#else
    ssize_t charsSize = size() / sizeof(char_t);
    ssize_t bytesSize = charsSize * sizeof(char_t), bytesRead;
    Array<char_t> chars(charsSize + 1);
    
    if ((bytesRead = read(_handle, chars.values(), bytesSize)) >= 0)
#endif
    {
        if (bytesSize != bytesRead)
            throw Exception(STR("failed to read entire file"));
    }
    else
        throw Exception(STR("failed to read file"));
            
    chars[charsSize] = 0;
    return String::acquire(chars.release());
}

void File::writeString(const String& str)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = str.length() * sizeof(char_t), bytesWritten;
    
    if (WriteFile(_handle, str.str(), bytesSize, &bytesWritten, nullptr))
#else
    ssize_t bytesSize = str.length() * sizeof(char_t), bytesWritten;
    
    if ((bytesWritten = write(_handle, str.str(), bytesSize)) >= 0)
#endif
    {
        if (bytesSize != bytesWritten)
            throw Exception(STR("failed to write entire file"));
    }
    else
        throw Exception(STR("failed to write file"));
}

int64_t File::size() const
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

#ifdef PLATFORM_WINDOWS
    int64_t sz;
    ASSERT(GetFileSizeEx(_handle, reinterpret_cast<LARGE_INTEGER*>(&sz)));
    return sz;
#else
    struct stat64 st;
    ASSERT(fstat64(_handle, &st) == 0);
    return st.st_size;
#endif
}
