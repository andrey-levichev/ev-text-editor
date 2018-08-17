#include <renderer.h>
#include <math.h>

DrawingFactory::DrawingFactory()
{
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_ptr)))
        throw Exception(STR("failed to create Direct2D factory"));
}

RenderTarget::RenderTarget(DrawingFactory& factory, HWND window)
{
    RECT rc;
    GetClientRect(window, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    if (FAILED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
               D2D1::HwndRenderTargetProperties(window, size), &_ptr)))
        throw Exception(STR("failed to create render target"));
}

SolidBrush::SolidBrush(RenderTarget& renderTarget, const D2D1_COLOR_F& color)
{
    if (FAILED(renderTarget->CreateSolidColorBrush(color, &_ptr)))
        throw Exception(STR("failed to create brush"));
}

TextFactory::TextFactory()
{
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                   __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&_ptr))))
        throw Exception(STR("failed to create DirectWrite factory"));
}

TextFormat::TextFormat(TextFactory& factory, const String& fontName,
                       DWRITE_FONT_WEIGHT fontWeight,
                       DWRITE_FONT_STYLE fontStyle,
                       DWRITE_FONT_STRETCH fontStretch,
                       float fontSize, const String& locale)
{
    if (FAILED(factory->CreateTextFormat(reinterpret_cast<const WCHAR*>(fontName.chars()), NULL,
                                         fontWeight, fontStyle, fontStretch, fontSize,
                                         reinterpret_cast<const WCHAR*>(locale.chars()), &_ptr)))
        throw Exception(STR("failed to create text format"));
}

TextLayout::TextLayout(TextFactory& factory, const String& text,
                       TextFormat& textFormat, float width, float height, bool legacyFontMeasuring)
{
    HRESULT hr;

    if (legacyFontMeasuring)
        hr = factory->CreateGdiCompatibleTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(),
                textFormat, width, height, 1, NULL, false, &_ptr);
    else
        hr = factory->CreateTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(),
            textFormat, width, height, &_ptr);

    if (FAILED(hr))
        throw Exception(STR("failed to create text layout"));
}

EllipsisTrimmingSign::EllipsisTrimmingSign(TextFactory& factory, TextFormat& textFormat)
{
    if (FAILED(factory->CreateEllipsisTrimmingSign(textFormat, &_ptr)))
        throw Exception(STR("failed to create trimming sign"));
}

ImagingFactory::ImagingFactory()
{
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory,
                                NULL, CLSCTX_INPROC_SERVER,
                                __uuidof(IWICImagingFactory), reinterpret_cast<void**>(&_ptr))))
        throw Exception(STR("Failed to create WIC imaging factory"));
}

BitmapDecoder::BitmapDecoder(ImagingFactory& factory, const String& fileName)
{
    if (FAILED(factory->CreateDecoderFromFilename(reinterpret_cast<const WCHAR*>(fileName.chars()),
               NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &_ptr)))
        throw Exception(STR("Failed to create WIC decoder"));
}

BitmapFrameDecode::BitmapFrameDecode(BitmapDecoder& decoder)
{
    if (FAILED(decoder->GetFrame(0, &_ptr)))
        throw Exception(STR("Failed to load bitmap frame"));
}

FormatConverter::FormatConverter(ImagingFactory& factory, BitmapFrameDecode& frame)
{
    if (FAILED(factory->CreateFormatConverter(&_ptr)))
        throw Exception(STR("Failed to create WIC format converter"));

    if (FAILED(_ptr->Initialize(frame, GUID_WICPixelFormat32bppPBGRA,
                                      WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeMedianCut)))
        throw Exception(STR("Failed to initialize WIC format converter"));
}

Bitmap::Bitmap(RenderTarget& renderTarget,
               ImagingFactory& imagingFactory, const String& fileName)
{
    BitmapDecoder bitmapDecoder(imagingFactory, fileName.chars());
    BitmapFrameDecode frame(bitmapDecoder);
    FormatConverter formatConverter(imagingFactory, frame);

    if (FAILED(renderTarget->CreateBitmapFromWicBitmap(formatConverter, &_ptr)))
        throw Exception(STR("Failed to create D2D bitmap"));
}

Size TextBlock::getTextSize() const
{
    DWRITE_TEXT_METRICS textMetrics;
	_textLayout->GetMetrics(&textMetrics);
    return { textMetrics.width, textMetrics.height };
}

Renderer::Renderer(HWND window) :
    _renderTarget(_drawingFactory, window)
{
}

void Renderer::beginDraw()
{
    _renderTarget->BeginDraw();
    _renderTarget->SetTransform(D2D1::IdentityMatrix());
    _renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void Renderer::endDraw()
{
    _renderTarget->EndDraw();
}

void Renderer::drawLine(const Point& p1, const Point& p2, Color color, float width)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, brush, width);
}

void Renderer::drawHorizontalLine(float y, float x1, float x2, Color color, float width)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x1, y }, { x2, y }, brush, width);
}

void Renderer::drawVerticalLine(float x, float y1, float y2, Color color, float width)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x, y1 }, { x, y2 }, brush, width);
}

void Renderer::drawRectangle(const Rect& rect, Color color, float borderWidth)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush, borderWidth);
}

void Renderer::drawRoundedRectangle(const Rect& rect, Color color,
                                    float cornerRadius, float borderWidth)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRoundedRectangle({ { rect.left, rect.top,
        rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush, borderWidth);
}

void Renderer::fillRectangle(const Rect& rect, Color color)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush);
}

void Renderer::fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRoundedRectangle({ { rect.left, rect.top,
        rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush);
}

void Renderer::drawText(const String& font, float fontSize, bool bold,
                        const Rect& rect, TextAlignment textAlignment, ParagraphAlignment paragraphAlignment,
                        Color color, const String& text)
{
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    TextFormat textFormat(_textFactory, font.chars(),
                          bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
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

    _renderTarget->DrawText(reinterpret_cast<const WCHAR*>(text.chars()), text.length(), textFormat,
        { rect.left, rect.top, rect.right, rect.bottom }, brush,
        D2D1_DRAW_TEXT_OPTIONS_CLIP, DWRITE_MEASURING_MODE_GDI_CLASSIC);
}

void Renderer::drawTextBlock(const TextBlock& textBlock, const Point& pos, Color color)
{
#if 0
    SolidBrush brush(_renderTarget, D2D1::ColorF(D2D1::ColorF::AliceBlue));

    DWRITE_TEXT_METRICS textMetrics;
	textBlock._textLayout->GetMetrics(&textMetrics);

    DWRITE_OVERHANG_METRICS overhangMetrics;
	textBlock._textLayout->GetOverhangMetrics(&overhangMetrics);

    D2D1_RECT_F rect = { pos.x, pos.y, pos.x + textMetrics.layoutWidth, pos.y + textMetrics.layoutHeight };
    _renderTarget->FillRectangle(rect, brush);

	D2D1_RECT_F overhangRect = { rect.left - overhangMetrics.left, rect.top - overhangMetrics.top,
			rect.right + overhangMetrics.right, rect.bottom + overhangMetrics.bottom };
	brush->SetColor(D2D1::ColorF((D2D1::ColorF::Azure)));
    _renderTarget->FillRectangle(overhangRect, brush);

	D2D1_RECT_F textRect = { rect.left + textMetrics.left, rect.top + textMetrics.top,
			rect.left + textMetrics.left + textMetrics.width,
			rect.top + textMetrics.top + textMetrics.height	};
    brush->SetColor(D2D1::ColorF((D2D1::ColorF::BlanchedAlmond)));
    _renderTarget->FillRectangle(textRect, brush);

    brush->SetColor(D2D1::ColorF(color));
#else
    SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawTextLayout({ pos.x, pos.y }, textBlock._textLayout, brush);
#endif
}

void Renderer::drawImage(const Image& image, const Point& pos, const Size* size)
{
    D2D1_RECT_F rect;

    if (size)
        rect = { pos.x, pos.y, pos.x + size->width, pos.y + size->height };
    else
    {
#ifdef COMPILER_GCC
        D2D1_SIZE_F size;
        image._bitmap->GetSize(size);
#else
        D2D1_SIZE_F size = image._bitmap->GetSize();
#endif
        rect = { pos.x, pos.y, pos.x + size.width, pos.y + size.height };
    }

    _renderTarget->DrawBitmap(image._bitmap, rect);
}

void Renderer::resize(int width, int height)
{
    _renderTarget->Resize({ static_cast<UINT32>(width), static_cast<UINT32>(height) });
}

Size Renderer::getSize()
{
#ifdef COMPILER_GCC
    D2D1_SIZE_F size;
    _renderTarget->GetSize(size);
#else
    D2D1_SIZE_F size = _renderTarget->GetSize();
#endif

    return { size.width, size.height };
}
