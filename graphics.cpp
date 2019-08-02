#include <graphics.h>

#ifdef PLATFORM_WINDOWS

__DrawingFactory::__DrawingFactory()
{
    ASSERT_COM_SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_ptr));
}

__RenderTarget::__RenderTarget(__DrawingFactory& factory, HWND window)
{
    RECT rect;
    BOOL rc = GetClientRect(window, &rect);
    ASSERT(rc);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);
    ASSERT_COM_SUCCEEDED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
                                                         D2D1::HwndRenderTargetProperties(window, size), &_ptr));
}

__SolidBrush::__SolidBrush(__RenderTarget& renderTarget, const D2D1_COLOR_F& color)
{
    ASSERT_COM_SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &_ptr));
}

__TextFactory::__TextFactory()
{
    ASSERT_COM_SUCCEEDED(
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&_ptr)));
}

__TextFormat::__TextFormat(__TextFactory& factory, const String& fontName, DWRITE_FONT_WEIGHT fontWeight,
                           DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize)
{
    WCHAR locale[LOCALE_NAME_MAX_LENGTH];
    int rc = GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH);
    ASSERT(rc > 0);

    ASSERT_COM_SUCCEEDED(factory->CreateTextFormat(reinterpret_cast<const WCHAR*>(fontName.chars()), NULL, fontWeight,
                                                   fontStyle, fontStretch, fontSize, locale, &_ptr));
}

__TextLayout::__TextLayout(__TextFactory& factory, const String& text, __TextFormat& textFormat, float width,
                           float height, bool legacyFontMeasuring)
{
    if (legacyFontMeasuring)
    {
        ASSERT_COM_SUCCEEDED(factory->CreateGdiCompatibleTextLayout(reinterpret_cast<const WCHAR*>(text.chars()),
                                                                    text.length(), textFormat, width, height, 1, NULL,
                                                                    false, &_ptr));
    }
    else
    {
        ASSERT_COM_SUCCEEDED(factory->CreateTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(),
                                                       textFormat, width, height, &_ptr));
    }
}

__EllipsisTrimmingSign::__EllipsisTrimmingSign(__TextFactory& factory, __TextFormat& textFormat)
{
    ASSERT_COM_SUCCEEDED(factory->CreateEllipsisTrimmingSign(textFormat, &_ptr));
}

__ImagingFactory::__ImagingFactory()
{
    ASSERT_COM_SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                          __uuidof(IWICImagingFactory), reinterpret_cast<void**>(&_ptr)));
}

__BitmapDecoder::__BitmapDecoder(__ImagingFactory& factory, const String& fileName)
{
    ASSERT_COM_SUCCEEDED(factory->CreateDecoderFromFilename(reinterpret_cast<const WCHAR*>(fileName.chars()), NULL,
                                                            GENERIC_READ, WICDecodeMetadataCacheOnLoad, &_ptr));
}

__BitmapFrameDecode::__BitmapFrameDecode(__BitmapDecoder& decoder)
{
    ASSERT_COM_SUCCEEDED(decoder->GetFrame(0, &_ptr));
}

__FormatConverter::__FormatConverter(__ImagingFactory& factory, __BitmapFrameDecode& frame)
{
    ASSERT_COM_SUCCEEDED(factory->CreateFormatConverter(&_ptr));
    ASSERT_COM_SUCCEEDED(_ptr->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0,
                                          WICBitmapPaletteTypeMedianCut));
}

__Bitmap::__Bitmap(__RenderTarget& renderTarget, __ImagingFactory& imagingFactory, const String& fileName)
{
    __BitmapDecoder bitmapDecoder(imagingFactory, fileName.chars());
    __BitmapFrameDecode frame(bitmapDecoder);
    __FormatConverter formatConverter(imagingFactory, frame);

    ASSERT_COM_SUCCEEDED(renderTarget->CreateBitmapFromWicBitmap(formatConverter, &_ptr));
}

#endif

Size TextBlock::getSize() const
{
#ifdef PLATFORM_WINDOWS
    DWRITE_TEXT_METRICS textMetrics;
    ASSERT_COM_SUCCEEDED(_textLayout->GetMetrics(&textMetrics));
    return { textMetrics.width, textMetrics.height };
#else
    Size size = { 0, 0 };
    return size;
#endif
}

#ifdef PLATFORM_WINDOWS
Graphics::Graphics(uintptr_t window) : _renderTarget(_drawingFactory, reinterpret_cast<HWND>(window))
#else
Graphics::Graphics(uintptr_t window)
#endif
{
}

void Graphics::beginDraw()
{
#ifdef PLATFORM_WINDOWS
    _renderTarget->BeginDraw();
    _renderTarget->SetTransform(D2D1::IdentityMatrix());
    _renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
#endif
}

void Graphics::endDraw()
{
#ifdef PLATFORM_WINDOWS
    ASSERT_COM_SUCCEEDED(_renderTarget->EndDraw());
#endif
}

void Graphics::drawLine(const Point& p1, const Point& p2, Color color, float width)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, brush, width);
#endif
}

void Graphics::drawHorizontalLine(float y, float x1, float x2, Color color, float width)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x1, y }, { x2, y }, brush, width);
#endif
}

void Graphics::drawVerticalLine(float x, float y1, float y2, Color color, float width)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x, y1 }, { x, y2 }, brush, width);
#endif
}

void Graphics::drawRectangle(const Rect& rect, Color color, float borderWidth)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush, borderWidth);
#endif
}

void Graphics::drawRoundedRectangle(const Rect& rect, Color color, float cornerRadius, float borderWidth)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRoundedRectangle(
        { { rect.left, rect.top, rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush, borderWidth);
#endif
}

void Graphics::fillRectangle(const Rect& rect, Color color)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush);
#endif
}

void Graphics::fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRoundedRectangle(
        { { rect.left, rect.top, rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush);
#endif
}

void Graphics::drawText(const String& font, float fontSize, const String& text, const Rect& rect, Color color,
                        TextAlignment textAlignment, ParagraphAlignment paragraphAlignment, bool bold, bool wrapLines)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    __TextFormat textFormat(_textFactory, font.chars(), bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize);

    DWRITE_TEXT_ALIGNMENT txtAlign;
    switch (textAlignment)
    {
    default:
    case TEXT_ALIGNMENT_LEFT:
        txtAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;
    case TEXT_ALIGNMENT_RIGHT:
        txtAlign = DWRITE_TEXT_ALIGNMENT_TRAILING;
        break;
    case TEXT_ALIGNMENT_CENTER:
        txtAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
        break;
    case TEXT_ALIGNMENT_JUSTIFIED:
        txtAlign = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
    }

    DWRITE_PARAGRAPH_ALIGNMENT paraAlign;
    switch (paragraphAlignment)
    {
    default:
    case PARAGRAPH_ALIGNMENT_TOP:
        paraAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        break;
    case PARAGRAPH_ALIGNMENT_BOTTOM:
        paraAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
        break;
    case PARAGRAPH_ALIGNMENT_CENTER:
        paraAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
        break;
    }

    ASSERT_COM_SUCCEEDED(textFormat->SetTextAlignment(txtAlign));
    ASSERT_COM_SUCCEEDED(textFormat->SetParagraphAlignment(paraAlign));
    ASSERT_COM_SUCCEEDED(
        textFormat->SetWordWrapping(wrapLines ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP));

    _renderTarget->DrawText(reinterpret_cast<const WCHAR*>(text.chars()), text.length(), textFormat,
                            { rect.left, rect.top, rect.right, rect.bottom }, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP,
                            DWRITE_MEASURING_MODE_GDI_CLASSIC);
#endif
}

void Graphics::drawText(const TextBlock& textBlock, const Point& pos, Color color)
{
#ifdef PLATFORM_WINDOWS
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawTextLayout({ pos.x, pos.y }, textBlock._textLayout, brush);
#endif
}

void Graphics::drawImage(const Image& image, const Point& pos, const Size* size)
{
#ifdef PLATFORM_WINDOWS
    D2D1_RECT_F rect;

    if (size)
        rect = { pos.x, pos.y, pos.x + size->width, pos.y + size->height };
    else
    {
        D2D1_SIZE_F size = image._bitmap->GetSize();
        rect = { pos.x, pos.y, pos.x + size.width, pos.y + size.height };
    }

    _renderTarget->DrawBitmap(image._bitmap, rect);
#endif
}

void Graphics::resize(int width, int height)
{
#ifdef PLATFORM_WINDOWS
    ASSERT_COM_SUCCEEDED(_renderTarget->Resize({ static_cast<UINT32>(width), static_cast<UINT32>(height) }));
#endif
}

Size Graphics::getSize()
{
#ifdef PLATFORM_WINDOWS
    D2D1_SIZE_F size = _renderTarget->GetSize();
    return { size.width, size.height };
#else
    Size size = { 0, 0 };
    return size;
#endif
}
