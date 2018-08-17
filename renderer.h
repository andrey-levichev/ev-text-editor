#ifndef RENDERER_INCLUDED
#define RENDERER_INCLUDED

#include <foundation.h>
#include <windows.h>
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

using Color = uint32_t;

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

// ComPtr

template<typename _Interface>
class ComPtr
{
public:
    ComPtr(const ComPtr<_Interface>& other) :
        _ptr(other._ptr)
    {
        _ptr->AddRef();
    }

    ~ComPtr()
    {
        if (_ptr)
            _ptr->Release();
    }

    ComPtr& operator=(const ComPtr& other)
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
    ComPtr() :
        _ptr(NULL)
    {
    }

protected:
    _Interface* _ptr;
};

// forward declarations

class Renderer;

// DrawingFactory

class DrawingFactory : public ComPtr<ID2D1Factory>
{
public:
    DrawingFactory();
};

// RenderTarget

class RenderTarget : public ComPtr<ID2D1HwndRenderTarget>
{
public:
    RenderTarget(DrawingFactory& factory, HWND window);
};

// SolidBrush

class SolidBrush : public ComPtr<ID2D1SolidColorBrush>
{
public:
    SolidBrush(RenderTarget& renderTarget, const D2D1_COLOR_F& color);
};

// TextFactory

class TextFactory : public ComPtr<IDWriteFactory>
{
public:
    TextFactory();
};

// TextFormat

class TextFormat : public ComPtr<IDWriteTextFormat>
{
public:
    TextFormat(TextFactory& factory, const String& fontName,
               DWRITE_FONT_WEIGHT fontWeight,
               DWRITE_FONT_STYLE fontStyle,
               DWRITE_FONT_STRETCH fontStretch,
               float fontSize, const String& locale);
};

// TextLayout

class TextLayout : public ComPtr<IDWriteTextLayout>
{
public:
    TextLayout(TextFactory& factory, const String& text,
               TextFormat& textFormat, float width, float height, bool legacyFontMeasuring);
};

// EllipsisTrimmingSign

class EllipsisTrimmingSign : public ComPtr<IDWriteInlineObject>
{
public:
    EllipsisTrimmingSign(TextFactory& factory, TextFormat& textFormat);
};

// ImagingFactory

class ImagingFactory : public ComPtr<IWICImagingFactory>
{
public:
    ImagingFactory();
};

// BitmapDecoder

class BitmapDecoder : public ComPtr<IWICBitmapDecoder>
{
public:
    BitmapDecoder(ImagingFactory& factory, const String& fileName);
};

// BitmapFrameDecode

class BitmapFrameDecode : public ComPtr<IWICBitmapFrameDecode>
{
public:
    BitmapFrameDecode(BitmapDecoder& decoder);
};

// FormatConverter

class FormatConverter : public ComPtr<IWICFormatConverter>
{
public:
    FormatConverter(ImagingFactory& factory, BitmapFrameDecode& frame);
};

// Bitmap

class Bitmap : public ComPtr<ID2D1Bitmap>
{
public:
    Bitmap(RenderTarget& renderTarget,
           ImagingFactory& imagingFactory, const String& fileName);
};

// TextBlock

class TextBlock
{
public:
    TextBlock(TextFactory& textFactory, const String& font, float fontSize, bool bold,
              const String& text, const Size& size) :
        _textFormat(textFactory, font.chars(),
            bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, STR("en-us")),
        _textLayout(textFactory, text, _textFormat, size.width, size.height, true)
    {

    }

    Size getTextSize() const;

private:
    TextFormat _textFormat;
    TextLayout _textLayout;
    friend class Renderer;
};

// Image

class Image
{
public:
    Image(RenderTarget& renderTarget,
            ImagingFactory& imagingFactory, const String& fileName) :
        _bitmap(renderTarget, imagingFactory, fileName)
    {
    }

private:
    Bitmap _bitmap;
    friend class Renderer;
};

// Renderer

class Renderer
{
public:
    Renderer(HWND window);

    void beginDraw();
    void endDraw();

    void drawLine(const Point& p1, const Point& p2, Color color, float width = 1);
    void drawHorizontalLine(float y, float x1, float x2, Color color, float width = 1);
    void drawVerticalLine(float x, float y1, float y2, Color color, float width = 1);

    void drawRectangle(const Rect& rect, Color color, float borderWidth = 1);
    void drawRoundedRectangle(const Rect& rect, Color color,
                              float cornerRadius, float borderWidth = 1);

    void fillRectangle(const Rect& rect, Color color);
    void fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius);

    void drawText(const String& font, float fontSize, bool bold,
                  const Rect& rect, TextAlignment textAlignment, ParagraphAlignment paragraphAlignment,
                  Color color, const String& text);

    void drawTextBlock(const TextBlock& textBlock, const Point& pos, Color color);

    Unique<TextBlock> createTextBlock(const String& font, float fontSize,
                                         bool bold, const String& text, const Size& size)
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
    DrawingFactory _drawingFactory;
    ImagingFactory _imagingFactory;
    TextFactory _textFactory;
    RenderTarget _renderTarget;
};

#endif
