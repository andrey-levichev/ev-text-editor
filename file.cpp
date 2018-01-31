#include <file.h>

// File

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

bool File::isOpen() const
{
    return _handle != INVALID_HANDLE_VALUE;
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

bool File::open(const String& fileName, FileMode openMode)
{
    if (_handle != INVALID_HANDLE_VALUE)
        throw Exception(STR("file already open"));

#ifdef PLATFORM_WINDOWS
    DWORD access = 0, disposition = OPEN_EXISTING;

    if ((openMode & FILE_MODE_READ) != 0)
        access |= GENERIC_READ;
    if ((openMode & FILE_MODE_WRITE) != 0)
        access |= GENERIC_WRITE;
    if (access == 0)
        throw Exception(STR("invalid file mode"));

    if ((openMode & FILE_MODE_CREATE) != 0)
        disposition = OPEN_ALWAYS;
    if ((openMode & FILE_MODE_NEW_ONLY) != 0)
        disposition = CREATE_NEW;
    if ((openMode & FILE_MODE_TRUNCATE) != 0)
        disposition = CREATE_ALWAYS;

    _handle = CreateFile(reinterpret_cast<LPCTSTR>(fileName.chars()),
        access, 0, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);
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

    _handle = ::open(fileName.chars(), mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
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
    DWORD bytesSize = size < 0 ? this->size() : size,  bytesRead;
#else
    ssize_t bytesSize = size < 0 ? this->size() : size, bytesRead;
#endif
    ByteBuffer data(bytesSize);

#ifdef PLATFORM_WINDOWS
    if (ReadFile(_handle, data.values(), bytesSize, &bytesRead, NULL))
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
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = size, bytesRead;

    if (ReadFile(_handle, data, bytesSize, &bytesRead, NULL))
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
    ASSERT((data && size > 0) || (!data && size == 0));

    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    DWORD bytesSize = size, bytesWritten;

    if (WriteFile(_handle, data, bytesSize, &bytesWritten, NULL))
#else
    ssize_t bytesWritten;

    if ((bytesWritten = ::write(_handle, data, bytesSize)) >= 0)
#endif
    {
        if (bytesSize != bytesWritten)
            throw Exception(STR("failed to write all data"));
    }
    else
        throw Exception(STR("failed to write file"));
}

String File::readString(TextEncoding& encoding, bool& bom, bool& crLf)
{
    ByteBuffer bytes = read();
    int bomOffset = 0;

    if (bytes.size() >= 2 && bytes[0] == 0xfe && bytes[1] == 0xff)
    {
        encoding = TEXT_ENCODING_UTF16_BE;
        bom = true;
        bomOffset = 2;
    }
    else if (bytes.size() >= 2 && bytes[0] == 0xff && bytes[1] == 0xfe)
    {
        encoding = TEXT_ENCODING_UTF16_LE;
        bom = true;
        bomOffset = 2;
    }
    else if (bytes.size() >= 3 && bytes[0] == 0xef &&
        bytes[1] == 0xbb && bytes[2] == 0xbf)
    {
        encoding = TEXT_ENCODING_UTF8;
        bom = true;
        bomOffset = 3;
    }
    else
    {
        encoding = TEXT_ENCODING_UTF8;
        bom = false;
    }

    if (encoding != TEXT_ENCODING_UTF8 && bytes.size() % 2 != 0)
        throw Exception(STR("text in UTF-16 encoding has odd number of bytes"));

#ifdef ARCH_LITTLE_ENDIAN
    if (encoding == TEXT_ENCODING_UTF16_BE)
        swapBytes(reinterpret_cast<uint16_t*>(bytes.values()), bytes.size() / 2);
#else
    if (encoding == TEXT_ENCODING_UTF16_LE)
        swapBytes(reinterpret_cast<uint16_t*>(bytes.values()), bytes.size() / 2);
#endif

    byte_t* p = bytes.values() + bomOffset;
    byte_t* e = bytes.values() + bytes.size();
    int len = 0;
    unichar_t ch;

    while (p < e)
    {
        if (encoding == TEXT_ENCODING_UTF8)
            p += utf8CharToUnicode(reinterpret_cast<char*>(p), ch);
        else
            p += utf16CharToUnicode(reinterpret_cast<char16_t*>(p), ch) * 2;

        if (ch >= 0x20 || ch == '\n' || ch == '\t')
            len += UTF_CHAR_LENGTH(ch);
    }

    String str;
    str.ensureCapacity(len + 1);

    p = bytes.values() + bomOffset;
    crLf = false;

    while (p < e)
    {
        if (encoding == TEXT_ENCODING_UTF8)
            p += utf8CharToUnicode(reinterpret_cast<char*>(p), ch);
        else
            p += utf16CharToUnicode(reinterpret_cast<char16_t*>(p), ch) * 2;

        if (ch >= 0x20)
            str += ch;
        else
        {
            if (ch == '\r')
                crLf = true;
            else if (ch == '\n' || ch == '\t')
                str += ch;
        }
    }

    ASSERT(str.length() == len);
    return str;
}

void File::writeString(const String& str, TextEncoding encoding, bool bom, bool crLf)
{
    const char_t* p = str.chars();
    const char_t* e = p + str.length();
    int len = bom ? (encoding == TEXT_ENCODING_UTF8 ? 3 : 2) : 0;
    unichar_t ch;

    while (p < e)
    {
        p += UTF_CHAR_TO_UNICODE(p, ch);

        if (encoding == TEXT_ENCODING_UTF8)
        {
            if (ch == '\n' && crLf)
                ++len;
            len += utf8CharLength(ch);
        }
        else
        {
            if (ch == '\n' && crLf)
                len += 2;
            len += utf16CharLength(ch) * 2;
        }
    }

    p = str.chars();

    ByteBuffer bytes;
    bytes.resize(len);
    int i = 0;

    if (bom)
    {
        if (encoding == TEXT_ENCODING_UTF8)
        {
            bytes[i++] = 0xef;
            bytes[i++] = 0xbb;
            bytes[i++] = 0xbf;
        }
        else
        {
            char16_t bomch = 0xfeff;
            bytes[i++] = *(reinterpret_cast<byte_t*>(&bomch));
            bytes[i++] = *(reinterpret_cast<byte_t*>(&bomch) + 1);
        }
    }

    while (p < e)
    {
        p += UTF_CHAR_TO_UNICODE(p, ch);

        if (ch == '\n' && crLf)
        {
            if (encoding == TEXT_ENCODING_UTF8)
                bytes[i++] = '\r';
            else
            {
                char16_t lfch = '\r';
                bytes[i++] = *(reinterpret_cast<byte_t*>(&lfch));
                bytes[i++] = *(reinterpret_cast<byte_t*>(&lfch) + 1);
            }
        }

        byte_t s[4];
        int n;

        if (encoding == TEXT_ENCODING_UTF8)
            n = unicodeCharToUtf8(ch, reinterpret_cast<char*>(s));
        else
            n = unicodeCharToUtf16(ch, reinterpret_cast<char16_t*>(s)) * 2;

        for (int j = 0; j < n; ++j)
            bytes[i++] = s[j];
    }

#ifdef ARCH_LITTLE_ENDIAN
    if (encoding == TEXT_ENCODING_UTF16_BE)
        swapBytes(reinterpret_cast<uint16_t*>(bytes.values()), bytes.size() / 2);
#else
    if (encoding == TEXT_ENCODING_UTF16_LE)
        swapBytes(reinterpret_cast<uint16_t*>(bytes.values()), bytes.size() / 2);
#endif

    ASSERT(bytes.size() == len);
    write(bytes);
}

