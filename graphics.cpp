#include <graphics.h>

__DrawingFactory::__DrawingFactory()
{
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_ptr)))
        throw Exception(STR("failed to create Direct2D factory"));
}

__RenderTarget::__RenderTarget(__DrawingFactory& factory, HWND window)
{
    RECT rc;
    GetClientRect(window, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    if (FAILED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
                                               D2D1::HwndRenderTargetProperties(window, size), &_ptr)))
        throw Exception(STR("failed to create render target"));
}

__SolidBrush::__SolidBrush(__RenderTarget& renderTarget, const D2D1_COLOR_F& color)
{
    if (FAILED(renderTarget->CreateSolidColorBrush(color, &_ptr)))
        throw Exception(STR("failed to create brush"));
}

__TextFactory::__TextFactory()
{
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(&_ptr))))
        throw Exception(STR("failed to create DirectWrite factory"));
}

__TextFormat::__TextFormat(__TextFactory& factory, const String& fontName, DWRITE_FONT_WEIGHT fontWeight,
                           DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize,
                           const String& locale)
{
    if (FAILED(factory->CreateTextFormat(reinterpret_cast<const WCHAR*>(fontName.chars()), NULL, fontWeight, fontStyle,
                                         fontStretch, fontSize, reinterpret_cast<const WCHAR*>(locale.chars()), &_ptr)))
        throw Exception(STR("failed to create text format"));
}

__TextLayout::__TextLayout(__TextFactory& factory, const String& text, __TextFormat& textFormat, float width,
                           float height, bool legacyFontMeasuring)
{
    HRESULT hr;

    if (legacyFontMeasuring)
        hr = factory->CreateGdiCompatibleTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(),
                                                    textFormat, width, height, 1, NULL, false, &_ptr);
    else
        hr = factory->CreateTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(), textFormat, width,
                                       height, &_ptr);

    if (FAILED(hr))
        throw Exception(STR("failed to create text layout"));
}

__EllipsisTrimmingSign::__EllipsisTrimmingSign(__TextFactory& factory, __TextFormat& textFormat)
{
    if (FAILED(factory->CreateEllipsisTrimmingSign(textFormat, &_ptr)))
        throw Exception(STR("failed to create trimming sign"));
}

__ImagingFactory::__ImagingFactory()
{
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory),
                                reinterpret_cast<void**>(&_ptr))))
        throw Exception(STR("Failed to create WIC imaging factory"));
}

__BitmapDecoder::__BitmapDecoder(__ImagingFactory& factory, const String& fileName)
{
    if (FAILED(factory->CreateDecoderFromFilename(reinterpret_cast<const WCHAR*>(fileName.chars()), NULL, GENERIC_READ,
                                                  WICDecodeMetadataCacheOnLoad, &_ptr)))
        throw Exception(STR("Failed to create WIC decoder"));
}

__BitmapFrameDecode::__BitmapFrameDecode(__BitmapDecoder& decoder)
{
    if (FAILED(decoder->GetFrame(0, &_ptr)))
        throw Exception(STR("Failed to load bitmap frame"));
}

__FormatConverter::__FormatConverter(__ImagingFactory& factory, __BitmapFrameDecode& frame)
{
    if (FAILED(factory->CreateFormatConverter(&_ptr)))
        throw Exception(STR("Failed to create WIC format converter"));

    if (FAILED(_ptr->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0,
                                WICBitmapPaletteTypeMedianCut)))
        throw Exception(STR("Failed to initialize WIC format converter"));
}

__Bitmap::__Bitmap(__RenderTarget& renderTarget, __ImagingFactory& imagingFactory, const String& fileName)
{
    __BitmapDecoder bitmapDecoder(imagingFactory, fileName.chars());
    __BitmapFrameDecode frame(bitmapDecoder);
    __FormatConverter formatConverter(imagingFactory, frame);

    if (FAILED(renderTarget->CreateBitmapFromWicBitmap(formatConverter, &_ptr)))
        throw Exception(STR("Failed to create D2D bitmap"));
}

Size __TextBlockD2d::getTextSize() const
{
    DWRITE_TEXT_METRICS textMetrics;
    _textLayout->GetMetrics(&textMetrics);
    return { textMetrics.width, textMetrics.height };
}

Graphics::Graphics(HWND window) : _renderTarget(_drawingFactory, window)
{
}

void Graphics::beginDraw()
{
    _renderTarget->BeginDraw();
    _renderTarget->SetTransform(D2D1::IdentityMatrix());
    _renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void Graphics::endDraw()
{
    _renderTarget->EndDraw();
}

void Graphics::drawLine(const Point& p1, const Point& p2, Color color, float width)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, brush, width);
}

void Graphics::drawHorizontalLine(float y, float x1, float x2, Color color, float width)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x1, y }, { x2, y }, brush, width);
}

void Graphics::drawVerticalLine(float x, float y1, float y2, Color color, float width)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x, y1 }, { x, y2 }, brush, width);
}

void Graphics::drawRectangle(const Rect& rect, Color color, float borderWidth)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush, borderWidth);
}

void Graphics::drawRoundedRectangle(const Rect& rect, Color color, float cornerRadius, float borderWidth)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRoundedRectangle(
        { { rect.left, rect.top, rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush, borderWidth);
}

void Graphics::fillRectangle(const Rect& rect, Color color)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush);
}

void Graphics::fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRoundedRectangle(
        { { rect.left, rect.top, rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush);
}

void Graphics::drawText(const String& font, float fontSize, const String& text, const Rect& rect, Color color,
                        TextAlignment textAlignment, ParagraphAlignment paragraphAlignment, bool bold, bool wrapLines)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    __TextFormat textFormat(_textFactory, font.chars(), bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, STR("en-us"));

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

    textFormat->SetTextAlignment(txtAlign);
    textFormat->SetParagraphAlignment(paraAlign);
    textFormat->SetWordWrapping(wrapLines ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

    _renderTarget->DrawText(reinterpret_cast<const WCHAR*>(text.chars()), text.length(), textFormat,
                            { rect.left, rect.top, rect.right, rect.bottom }, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP,
                            DWRITE_MEASURING_MODE_GDI_CLASSIC);
}

void Graphics::drawTextBlock(const TextBlock& textBlock, const Point& pos, Color color)
{
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    const __TextBlockD2d& textBlockD2d = dynamic_cast<const __TextBlockD2d&>(textBlock);

    _renderTarget->DrawTextLayout({ pos.x, pos.y }, textBlockD2d._textLayout, brush);
}

void Graphics::drawImage(const Image& image, const Point& pos, const Size* size)
{
    D2D1_RECT_F rect;
    const __ImageD2d& imageD2d = dynamic_cast<const __ImageD2d&>(image);

    if (size)
        rect = { pos.x, pos.y, pos.x + size->width, pos.y + size->height };
    else
    {
        D2D1_SIZE_F size = imageD2d._bitmap->GetSize();
        rect = { pos.x, pos.y, pos.x + size.width, pos.y + size.height };
    }

    _renderTarget->DrawBitmap(imageD2d._bitmap, rect);
}

void Graphics::resize(int width, int height)
{
    _renderTarget->Resize({ static_cast<UINT32>(width), static_cast<UINT32>(height) });
}

Size Graphics::getSize()
{
    D2D1_SIZE_F size = _renderTarget->GetSize();
    return { size.width, size.height };
}
