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

bool File::open(const String& fileName, FileMode openMode)
{
    if (_handle != INVALID_HANDLE_VALUE)
        throw Exception(STR("file already open"));

    int mode;

#ifdef PLATFORM_WINDOWS
    switch (openMode)
    {
    case FILE_MODE_CREATE:
        mode = CREATE_ALWAYS;
        break;
    case FILE_MODE_CREATE_NEW:
        mode = CREATE_NEW;
        break;
    case FILE_MODE_OPEN:
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

    _handle = CreateFile(reinterpret_cast<LPCTSTR>(fileName.chars()),
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        mode, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    switch (openMode)
    {
    case FILE_MODE_CREATE:
        mode = O_CREAT | O_TRUNC;
        break;
    case FILE_MODE_CREATE_NEW:
        mode = O_CREAT | O_EXCL;
        break;
    case FILE_MODE_OPEN:
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

    _handle = ::open(fileName.chars(), O_RDWR | mode,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
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

String File::readString(TextEncoding& encoding, bool& bom, bool& crLf)
{
    ByteBuffer bytes = read<byte_t>();
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

        if (charIsPrint(ch) || ch == '\n' || ch == '\t')
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

        if (charIsPrint(ch))
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

int64_t File::size() const
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not open"));

#ifdef PLATFORM_WINDOWS
    int64_t sz;
    BOOL rc = GetFileSizeEx(_handle, reinterpret_cast<LARGE_INTEGER*>(&sz));
    ASSERT(rc);
    return sz;
#else
    struct stat st;
    int rc = fstat(_handle, &st);
    ASSERT(rc == 0);
    return st.st_size;
#endif
}
