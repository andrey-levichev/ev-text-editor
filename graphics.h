#ifndef RENDERER_INCLUDED
#define RENDERER_INCLUDED

#include <foundation.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

// Point

struct Point
{
    float x, y;
};

// Rect

struct Rect
{
    float left;
    float top;
    float right;
    float bottom;
};

// Size

struct Size
{
    float width, height;
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

// __ComPtr

template<typename _Interface>
class __ComPtr
{
public:
    __ComPtr(const __ComPtr<_Interface>& other) : _ptr(other._ptr)
    {
        _ptr->AddRef();
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

    operator _Interface*() const
    {
        return _ptr;
    }

    _Interface* operator->() const
    {
        return _ptr;
    }

protected:
    __ComPtr() : _ptr(NULL)
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
                 DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize, const String& locale);
};

// __TextLayout

class __TextLayout : public __ComPtr<IDWriteTextLayout>
{
public:
    __TextLayout(__TextFactory& factory, const String& text, __TextFormat& textFormat, float width, float height,
                 bool legacyFontMeasuring);
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

class __BitmapFrameDecode : public __ComPtr<IWICBitmapFrameDecode>
{
public:
    __BitmapFrameDecode(__BitmapDecoder& decoder);
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

// TextBlock

class TextBlock
{
public:
    TextBlock(__TextFactory& textFactory, const String& font, float fontSize, bool bold, const String& text,
              const Size& size) :
        _textFormat(textFactory, font.chars(), bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, STR("en-us")),
        _textLayout(textFactory, text, _textFormat, size.width, size.height, true)
    {
    }

    Size getTextSize() const;

private:
    __TextFormat _textFormat;
    __TextLayout _textLayout;

    friend class Graphics;
};

// Image

class Image
{
public:
    Image(__RenderTarget& renderTarget, __ImagingFactory& imagingFactory, const String& fileName) :
        _bitmap(renderTarget, imagingFactory, fileName)
    {
    }

private:
    __Bitmap _bitmap;

    friend class Graphics;
};

// Graphics

class Graphics
{
public:
    Graphics(HWND window);

    void beginDraw();
    void endDraw();

    void drawLine(const Point& p1, const Point& p2, Color color, float width = 1);
    void drawHorizontalLine(float y, float x1, float x2, Color color, float width = 1);
    void drawVerticalLine(float x, float y1, float y2, Color color, float width = 1);

    void drawRectangle(const Rect& rect, Color color, float borderWidth = 1);
    void drawRoundedRectangle(const Rect& rect, Color color, float cornerRadius, float borderWidth = 1);

    void fillRectangle(const Rect& rect, Color color);
    void fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius);

    void drawText(const String& font, float fontSize, const String& text, const Rect& rect, Color color = 0x000000,
                  TextAlignment textAlignment = TEXT_ALIGNMENT_LEFT,
                  ParagraphAlignment paragraphAlignment = PARAGRAPH_ALIGNMENT_TOP, bool bold = false,
                  bool wrapLines = false);

    void drawTextBlock(const TextBlock& textBlock, const Point& pos, Color color);

    Unique<TextBlock> createTextBlock(const String& font, float fontSize, bool bold, const String& text,
                                      const Size& size)
    {
        return createUnique<TextBlock>(_textFactory, font, fontSize, bold, text, size);
    }

    void drawImage(const Image& image, const Point& pos, const Size* size = NULL);

    Unique<Image> createImage(const String& fileName)
    {
        return createUnique<Image>(_renderTarget, _imagingFactory, fileName);
    }

    void resize(int width, int height);
    Size getSize();

private:
    __DrawingFactory _drawingFactory;
    __ImagingFactory _imagingFactory;
    __TextFactory _textFactory;
    __RenderTarget _renderTarget;
};

#endif