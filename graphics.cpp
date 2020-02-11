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

    ASSERT_COM_SUCCEEDED(factory->CreateTextFormat(reinterpret_cast<const WCHAR*>(fontName.chars()), nullptr, fontWeight,
                                                   fontStyle, fontStretch, fontSize, locale, &_ptr));
}

void __TextFormat::textAlignment(TextAlignment alignment)
{
    DWRITE_TEXT_ALIGNMENT txtAlign;
    switch (alignment)
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

    ASSERT_COM_SUCCEEDED(_ptr->SetTextAlignment(txtAlign));
}

void __TextFormat::paragraphAlignment(ParagraphAlignment alignment)
{
    DWRITE_PARAGRAPH_ALIGNMENT paraAlign;
    switch (alignment)
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

    ASSERT_COM_SUCCEEDED(_ptr->SetParagraphAlignment(paraAlign));
}

void __TextFormat::wordWrap(bool wrap)
{
    ASSERT_COM_SUCCEEDED(_ptr->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP));
}

__TextLayout::__TextLayout(__TextFactory& factory, const String& text, __TextFormat& textFormat, float width, float height)
{
    ASSERT_COM_SUCCEEDED(factory->CreateTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(),
                                                   textFormat, width, height, &_ptr));
}

__EllipsisTrimmingSign::__EllipsisTrimmingSign(__TextFactory& factory, __TextFormat& textFormat)
{
    ASSERT_COM_SUCCEEDED(factory->CreateEllipsisTrimmingSign(textFormat, &_ptr));
}

__ImagingFactory::__ImagingFactory()
{
    ASSERT_COM_SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                          __uuidof(IWICImagingFactory), reinterpret_cast<void**>(&_ptr)));
}

__BitmapDecoder::__BitmapDecoder(__ImagingFactory& factory, const String& fileName)
{
    ASSERT_COM_SUCCEEDED(factory->CreateDecoderFromFilename(reinterpret_cast<const WCHAR*>(fileName.chars()), nullptr,
                                                            GENERIC_READ, WICDecodeMetadataCacheOnLoad, &_ptr));
}

__BitmapFrameDecode::__BitmapFrameDecode(__BitmapDecoder& decoder)
{
    ASSERT_COM_SUCCEEDED(decoder->GetFrame(0, &_ptr));
}

__FormatConverter::__FormatConverter(__ImagingFactory& factory, __BitmapFrameDecode& frame)
{
    ASSERT_COM_SUCCEEDED(factory->CreateFormatConverter(&_ptr));
    ASSERT_COM_SUCCEEDED(_ptr->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
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

Size TextBlock::size() const
{
#ifdef PLATFORM_WINDOWS
    DWRITE_TEXT_METRICS textMetrics;
    ASSERT_COM_SUCCEEDED(_textLayout->GetMetrics(&textMetrics));
    return { textMetrics.width, textMetrics.height };
#else
    return Size();
#endif
}

void TextBlock::textAlignment(TextAlignment alignment)
{
#ifdef PLATFORM_WINDOWS
    _textFormat.textAlignment(alignment);
#endif
}

void TextBlock::paragraphAlignment(ParagraphAlignment alignment)
{
#ifdef PLATFORM_WINDOWS
    _textFormat.paragraphAlignment(alignment);
#endif
}

void TextBlock::wordWrap(bool wrap)
{
#ifdef PLATFORM_WINDOWS
    _textFormat.wordWrap(wrap);
#endif
}

Size Image::size() const
{
#ifdef PLATFORM_WINDOWS
    D2D1_SIZE_F size = _bitmap->GetSize();
    return { size.width, size.height };
#else
    return Size();
#endif
}

#ifdef PLATFORM_WINDOWS
Graphics::Graphics(uintptr_t window) : _renderTarget(_drawingFactory, reinterpret_cast<HWND>(window))
#else
Graphics::Graphics(uintptr_t window)
#endif
{
}

void Graphics::beginDraw(uintptr_t context)
{
#if defined(PLATFORM_WINDOWS)
    _renderTarget->BeginDraw();
    _renderTarget->SetTransform(D2D1::IdentityMatrix());
#elif defined(PLATFORM_LINUX)
    _context = context;
#endif
}

void Graphics::endDraw()
{
#if defined(PLATFORM_WINDOWS)
    ASSERT_COM_SUCCEEDED(_renderTarget->EndDraw());
#elif defined(PLATFORM_LINUX)
    _context = 0;
#endif
}

void Graphics::clear(Color color)
{
#if defined(PLATFORM_WINDOWS)
    _renderTarget->Clear(D2D1::ColorF(color));
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);
    cairo_paint(reinterpret_cast<cairo_t*>(_context));
#endif
}

void Graphics::drawLine(const Point& p1, const Point& p2, Color color, float width)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ p1.x, p1.y }, { p2.x, p2.y }, brush, width);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_move_to(cr, p1.x, p1.y);
    cairo_line_to(cr, p2.x, p2.y);
    cairo_set_line_width(cr, width);
    cairo_stroke(cr);
#endif
}

void Graphics::drawHorizontalLine(float y, float x1, float x2, Color color, float width)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x1, y }, { x2, y }, brush, width);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_move_to(cr, x1, y);
    cairo_line_to(cr, x2, y);
    cairo_set_line_width(cr, width);
    cairo_stroke(cr);
#endif
}

void Graphics::drawVerticalLine(float x, float y1, float y2, Color color, float width)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawLine({ x, y1 }, { x, y2 }, brush, width);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_move_to(cr, x, y1);
    cairo_line_to(cr, x, y2);
    cairo_set_line_width(cr, width);
    cairo_stroke(cr);
#endif
}

void Graphics::drawRectangle(const Rect& rect, Color color, float borderWidth)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush, borderWidth);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_rectangle(cr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    cairo_set_line_width(cr, borderWidth);
    cairo_stroke(cr);
#endif
}

void Graphics::drawRoundedRectangle(const Rect& rect, Color color, float cornerRadius, float borderWidth)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawRoundedRectangle(
        { { rect.left, rect.top, rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush, borderWidth);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    double x = rect.left, y = rect.top;
    double width = rect.right - rect.left;
    double height = rect.bottom - rect.top;
    double radius = cornerRadius, degrees = M_PI / 180.0;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);

    cairo_set_line_width(cr, borderWidth);
    cairo_stroke(cr);
#endif
}

void Graphics::fillRectangle(const Rect& rect, Color color)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRectangle({ rect.left, rect.top, rect.right, rect.bottom }, brush);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_rectangle(cr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    cairo_fill(cr);
#endif
}

void Graphics::fillRoundedRectangle(const Rect& rect, Color color, float cornerRadius)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->FillRoundedRectangle(
        { { rect.left, rect.top, rect.right, rect.bottom }, cornerRadius, cornerRadius }, brush);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    double x = rect.left, y = rect.top;
    double width = rect.right - rect.left;
    double height = rect.bottom - rect.top;
    double radius = cornerRadius, degrees = M_PI / 180.0;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);

    cairo_fill(cr);
#endif
}

void Graphics::setAntialias(bool on)
{
#if defined(PLATFORM_WINDOWS)
    _renderTarget->SetAntialiasMode(on ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairo_set_antialias(reinterpret_cast<cairo_t*>(_context),
        on ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
#endif
}

void Graphics::setClip(const Rect& rect)
{
#if defined(PLATFORM_WINDOWS)
    _renderTarget->PushAxisAlignedClip(
        { rect.left, rect.top, rect.right, rect.bottom }, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_rectangle(cr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    cairo_clip(cr);
#endif
}

void Graphics::cancelClip()
{
#if defined(PLATFORM_WINDOWS)
    _renderTarget->PopAxisAlignedClip();
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairo_reset_clip(reinterpret_cast<cairo_t*>(_context));
#endif
}

void Graphics::drawText(const String& font, float fontSize, const String& text, const Rect& rect, Color color,
                        TextAlignment textAlignment, ParagraphAlignment paragraphAlignment, bool bold, bool wordWrap)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    __TextFormat textFormat(_textFactory, font.chars(), bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize);

    textFormat.textAlignment(textAlignment);
    textFormat.paragraphAlignment(paragraphAlignment);
    textFormat.wordWrap(wordWrap);

    _renderTarget->DrawText(reinterpret_cast<const WCHAR*>(text.chars()), text.length(), textFormat,
                            { rect.left, rect.top, rect.right, rect.bottom }, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP,
                            DWRITE_MEASURING_MODE_NATURAL);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_select_font_face(cr, font.chars(), CAIRO_FONT_SLANT_NORMAL,
        bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, fontSize);

    cairo_move_to(cr, rect.left, rect.top);
    cairo_show_text(cr, text.chars());
#endif
}

void Graphics::drawText(const TextBlock& textBlock, const Point& pos, Color color)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawTextLayout({ pos.x, pos.y }, textBlock._textLayout, brush);
#elif defined(PLATFORM_LINUX)
#endif
}

void Graphics::drawImage(const Image& image, const Point& pos, const Size* size)
{
#if defined(PLATFORM_WINDOWS)
    D2D1_RECT_F rect;

    if (size)
        rect = { pos.x, pos.y, pos.x + size->width, pos.y + size->height };
    else
    {
        D2D1_SIZE_F size = image._bitmap->GetSize();
        rect = { pos.x, pos.y, pos.x + size.width, pos.y + size.height };
    }

    _renderTarget->DrawBitmap(image._bitmap, rect);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_set_source_surface(cr, image._image, pos.x, pos.y);
    cairo_paint(cr);
#endif
}

void Graphics::resize(int width, int height)
{
#if defined(PLATFORM_WINDOWS)
    ASSERT_COM_SUCCEEDED(_renderTarget->Resize({ static_cast<UINT32>(width), static_cast<UINT32>(height) }));
#elif defined(PLATFORM_LINUX)
#endif
}

Size Graphics::size() const
{
#if defined(PLATFORM_WINDOWS)
    D2D1_SIZE_F size = _renderTarget->GetSize();
    return { size.width, size.height };
#elif defined(PLATFORM_LINUX)
    return Size();
#else
    return Size();
#endif
}

#ifdef PLATFORM_LINUX

void Graphics::cairoSetColor(Color color)
{
    cairo_set_source_rgb(reinterpret_cast<cairo_t*>(_context),
        ((color & 0xff0000) >> 16) / 255.0, ((color & 0xff00) >> 8) / 255.0, (color & 0xff) / 255.0);
}

#endif
