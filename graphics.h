#ifndef GRAPHICS_INCLUDED
#define GRAPHICS_INCLUDED

#include <foundation.h>

// Point

struct Point
{
    float x = 0, y = 0;
};

// Rect

struct Rect
{
    float left = 0;
    float top = 0;
    float right = 0;
    float bottom = 0;
};

// Size

struct Size
{
    float width = 0, height = 0;
};

// Color

typedef uint32_t Color;

// TextAlignment

enum TextAlignment
{
    TEXT_ALIGNMENT_LEFT,
    TEXT_ALIGNMENT_RIGHT,
    TEXT_ALIGNMENT_CENTER,
    TEXT_ALIGNMENT_JUSTIFIED
};

// ParagraphAlignment

enum ParagraphAlignment
{
    PARAGRAPH_ALIGNMENT_TOP,
    PARAGRAPH_ALIGNMENT_BOTTOM,
    PARAGRAPH_ALIGNMENT_CENTER
};

#ifdef PLATFORM_WINDOWS

// __ComPtr

template<typename _Interface>
class __ComPtr
{
public:
    __ComPtr(const __ComPtr<_Interface>& other) : _ptr(other._ptr)
    {
        _ptr->AddRef();
    }

    __ComPtr(__ComPtr<_Interface>&& other) : _ptr(other._ptr)
    {
        other._ptr = nullptr;
    }

    ~__ComPtr()
    {
        if (_ptr)
            _ptr->Release();
    }

    __ComPtr& operator=(const __ComPtr& other)
    {
        if (_ptr)
            _ptr->Release();

        _ptr = other._ptr;
        _ptr->AddRef();

        return *this;
    }

    __ComPtr& operator=(__ComPtr&& other)
    {
        if (_ptr)
            _ptr->Release();

        _ptr = other._ptr;
        other._ptr = nullptr;

        return *this;
    }

    operator _Interface*() const
    {
        return _ptr;
    }

    _Interface* operator->() const
    {
        return _ptr;
    }

protected:
    __ComPtr() : _ptr(nullptr)
    {
    }

protected:
    _Interface* _ptr;
};

// __DrawingFactory

class __DrawingFactory : public __ComPtr<ID2D1Factory>
{
public:
    __DrawingFactory();
};

// __RenderTarget

class __RenderTarget : public __ComPtr<ID2D1HwndRenderTarget>
{
public:
    __RenderTarget(__DrawingFactory& factory, HWND window);
};

// __SolidBrush

class __SolidBrush : public __ComPtr<ID2D1SolidColorBrush>
{
public:
    __SolidBrush(__RenderTarget& renderTarget, const D2D1_COLOR_F& color);
};

// __TextFactory

class __TextFactory : public __ComPtr<IDWriteFactory>
{
public:
    __TextFactory();
};

// __TextFormat

class __TextFormat : public __ComPtr<IDWriteTextFormat>
{
public:
    __TextFormat(__TextFactory& factory, const String& fontName, DWRITE_FONT_WEIGHT fontWeight,
                 DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize);

    void textAlignment(TextAlignment alignment);
    void paragraphAlignment(ParagraphAlignment alignment);
    void wordWrap(bool wrap);
};

// __TextLayout

class __TextLayout : public __ComPtr<IDWriteTextLayout>
{
public:
    __TextLayout(__TextFactory& factory, const String& text, __TextFormat& textFormat, float width, float height);
};

// __EllipsisTrimmingSign

class __EllipsisTrimmingSign : public __ComPtr<IDWriteInlineObject>
{
public:
    __EllipsisTrimmingSign(__TextFactory& factory, __TextFormat& textFormat);
};

// __ImagingFactory

class __ImagingFactory : public __ComPtr<IWICImagingFactory>
{
public:
    __ImagingFactory();
};

// __BitmapDecoder

class __BitmapDecoder : public __ComPtr<IWICBitmapDecoder>
{
public:
    __BitmapDecoder(__ImagingFactory& factory, const String& fileName);
};

// __BitmapFrameDecode

class __BitmapFrameDecoder : public __ComPtr<IWICBitmapFrameDecode>
{
public:
    __BitmapFrameDecoder(__BitmapDecoder& decoder);
};

// __FormatConverter

class __FormatConverter : public __ComPtr<IWICFormatConverter>
{
public:
    __FormatConverter(__ImagingFactory& factory, __BitmapFrameDecode& frame);
};

// __Bitmap

class __Bitmap : public __ComPtr<ID2D1Bitmap>
{
public:
    __Bitmap(__RenderTarget& renderTarget, __ImagingFactory& imagingFactory, const String& fileName);
};

#endif

// TextBlock

class TextBlock
{
public:
    Size size() const;
    void textAlignment(TextAlignment alignment);
    void paragraphAlignment(ParagraphAlignment alignment);

#if defined(PLATFORM_WINDOWS)

private:
    TextBlock(__TextFactory& textFactory, const String& font,
        float fontSize, bool bold, const String& text, const Size& size, bool wrap);

    __TextFormat _textFormat;
    __TextLayout _textLayout;

#elif defined(PLATFORM_LINUX)

private:
    TextBlock(cairo_t* cr, const String& font,
        float fontSize, bool bold, const String& text, const Size& size, bool wrap);

    PangoFontDescription* _fontDesc;
    PangoLayout* _layout;

public:
    TextBlock(const TextBlock& other)
    {
        _fontDesc = pango_font_description_copy(other._fontDesc);
        _layout = pango_layout_copy(other._layout);
    }

    TextBlock& operator=(const TextBlock& other)
    {
        if (_fontDesc)
            pango_font_description_free(_fontDesc);

        if (_layout)
            g_object_unref(_layout);

        _fontDesc = pango_font_description_copy(other._fontDesc);
        _layout = pango_layout_copy(other._layout);
        return *this;
    }

    ~TextBlock();

#else

private:
    TextBlock()
    {
    }

#endif

    friend class Graphics;
};

// Image

class Image
{
public:
    Size size() const;

#if defined(PLATFORM_WINDOWS)

private:
    Image(__RenderTarget& renderTarget, __ImagingFactory& imagingFactory, const String& fileName) :
        _bitmap(renderTarget, imagingFactory, fileName)
    {
    }

    __Bitmap _bitmap;

#elif defined(PLATFORM_LINUX)

private:
    Image(const String& fileName)
    {
        _image = cairo_image_surface_create_from_png(fileName.chars());
    }

    cairo_surface_t* _image = nullptr;

public:
    Image(const Image& other)
    {
        _image = cairo_surface_reference(other._image);
    }

    Image& operator=(const Image& other)
    {
        if (_image)
            cairo_surface_destroy(_image);

        _image = cairo_surface_reference(other._image);
        return *this;
    }

    ~Image()
    {
        cairo_surface_destroy(_image);
    }

#else

private:
    Image()
    {
    }

#endif

    friend class Graphics;
};

// Graphics

class Graphics
{
public:
    Graphics(uintptr_t window = 0);

    void beginDraw(uintptr_t context = 0);
    void endDraw();
    void clear(Color color = 0xffffff);

    void drawLine(const Point& p1, const Point& p2, Color color, float width = 1);
    void drawHorizontalLine(float y, float x1, float x2, Color color, float width = 1);
    void drawVerticalLine(float x, float y1, float y2, Color color, float width = 1);

    void drawRectangle(const Rect& rect, Color color, float borderWidth = 1);
    void drawRoundedRectangle(const Rect& rect, Color color, float cornerRadius, float borderWidth = 1);

    void fillRectangle(const Rect& rect, Color color);
    void fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius);

    void setAntialias(bool on);
    void setClip(const Rect& rect);
    void resetClip();

    void drawText(const String& font, float fontSize, const String& text, const Rect& rect, Color color = 0,
                  TextAlignment textAlignment = TEXT_ALIGNMENT_LEFT,
                  ParagraphAlignment paragraphAlignment = PARAGRAPH_ALIGNMENT_TOP, bool bold = false,
                  bool wrap = false);

    void drawText(const TextBlock& textBlock, const Point& pos, Color color = 0);

    TextBlock createTextBlock(const String& font, float fontSize,
        const String& text, const Size& size, bool bold = false, bool wrap = true)
    {
#if defined(PLATFORM_WINDOWS)
        return TextBlock(_textFactory, font, fontSize, bold, text, size, wrap);
#elif defined(PLATFORM_LINUX)
        ASSERT(_context);
        return TextBlock(reinterpret_cast<cairo_t*>(_context), font, fontSize, bold, text, size, wrap);
#else
        return TextBlock();
#endif
    }

    void drawImage(const Image& image, const Point& pos, const Size& size = Size());

    Image createImage(const String& fileName)
    {
#ifdef PLATFORM_WINDOWS
        return Image(_renderTarget, _imagingFactory, fileName);
#else
        return Image(fileName);
#endif
    }

    void resize(int width, int height);
    Size size() const;

#if defined(PLATFORM_WINDOWS)

private:
    __DrawingFactory _drawingFactory;
    __ImagingFactory _imagingFactory;
    __TextFactory _textFactory;
    __RenderTarget _renderTarget;

#elif defined(PLATFORM_LINUX)

private:
    void cairoSetColor(Color color);

private:
    uintptr_t _context = 0;
    Size _size;

#endif
};

#endif
