#include <graphics.h>

#ifdef PLATFORM_WINDOWS

// __DrawingFactory

__DrawingFactory::__DrawingFactory()
{
    ASSERT_COM_SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_ptr));
}

// __RenderTarget

__RenderTarget::__RenderTarget(__DrawingFactory& factory, HWND window)
{
    RECT rect;
    BOOL rc = GetClientRect(window, &rect);
    ASSERT(rc);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);
    ASSERT_COM_SUCCEEDED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
                                                         D2D1::HwndRenderTargetProperties(window, size), &_ptr));
}

// __SolidBrush

__SolidBrush::__SolidBrush(__RenderTarget& renderTarget, const D2D1_COLOR_F& color)
{
    ASSERT_COM_SUCCEEDED(renderTarget->CreateSolidColorBrush(color, &_ptr));
}

// __TextFactory

__TextFactory::__TextFactory()
{
    ASSERT_COM_SUCCEEDED(
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&_ptr)));
}

// __TextFormat

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

// __TextLayout

__TextLayout::__TextLayout(__TextFactory& factory, const String& text, __TextFormat& textFormat, float width, float height)
{
    ASSERT_COM_SUCCEEDED(factory->CreateTextLayout(reinterpret_cast<const WCHAR*>(text.chars()), text.length(),
                                                   textFormat, width, height, &_ptr));
}

// __EllipsisTrimmingSign

__EllipsisTrimmingSign::__EllipsisTrimmingSign(__TextFactory& factory, __TextFormat& textFormat)
{
    ASSERT_COM_SUCCEEDED(factory->CreateEllipsisTrimmingSign(textFormat, &_ptr));
}

// __ImagingFactory

__ImagingFactory::__ImagingFactory()
{
    ASSERT_COM_SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                          __uuidof(IWICImagingFactory), reinterpret_cast<void**>(&_ptr)));
}

// __BitmapDecoder

__BitmapDecoder::__BitmapDecoder(__ImagingFactory& factory, const String& fileName)
{
    ASSERT_COM_SUCCEEDED(factory->CreateDecoderFromFilename(reinterpret_cast<const WCHAR*>(fileName.chars()), nullptr,
                                                            GENERIC_READ, WICDecodeMetadataCacheOnLoad, &_ptr));
}

// __BitmapFrameDecoder

__BitmapFrameDecoder::__BitmapFrameDecoder(__BitmapDecoder& decoder)
{
    ASSERT_COM_SUCCEEDED(decoder->GetFrame(0, &_ptr));
}

// __FormatConverter

__FormatConverter::__FormatConverter(__ImagingFactory& factory, __BitmapFrameDecoder& frame)
{
    ASSERT_COM_SUCCEEDED(factory->CreateFormatConverter(&_ptr));
    ASSERT_COM_SUCCEEDED(_ptr->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0,
                                          WICBitmapPaletteTypeMedianCut));
}

// __Bitmap

__Bitmap::__Bitmap(__RenderTarget& renderTarget, __ImagingFactory& imagingFactory, const String& fileName)
{
    __BitmapDecoder bitmapDecoder(imagingFactory, fileName.chars());
    __BitmapFrameDecoder frame(bitmapDecoder);
    __FormatConverter formatConverter(imagingFactory, frame);

    ASSERT_COM_SUCCEEDED(renderTarget->CreateBitmapFromWicBitmap(formatConverter, &_ptr));
}

#endif

// TextBlock

#if defined(PLATFORM_WINDOWS)

TextBlock::TextBlock(__TextFactory& textFactory, const String& font,
        float fontSize, bool bold, const String& text, const Size& size, bool wrap) :
    _textFormat(textFactory, font.chars(), bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize),
    _textLayout(textFactory, text, _textFormat, size.width, size.height)
{
    _textFormat.wordWrap(wrap);
}

#elif defined(PLATFORM_LINUX)

TextBlock::TextBlock(cairo_t* cr, const String& font,
    float fontSize, bool bold, const String& text, const Size& size, bool wrap)
{
    _fontDesc = pango_font_description_new();
    pango_font_description_set_family(_fontDesc, font.chars());
    pango_font_description_set_weight(_fontDesc, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
    pango_font_description_set_absolute_size(_fontDesc, fontSize * PANGO_SCALE);

    _layout = pango_cairo_create_layout(cr);
    pango_layout_set_font_description(_layout, _fontDesc);
    pango_layout_set_text(_layout, text.chars(), -1);

    pango_layout_set_ellipsize(_layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_width(_layout, PANGO_SCALE * size.width);
    pango_layout_set_height(_layout, PANGO_SCALE * size.height);
}

TextBlock::~TextBlock()
{
    g_object_unref(_layout);
    pango_font_description_free(_fontDesc);
}

#endif

Size TextBlock::size() const
{
#if defined(PLATFORM_WINDOWS)
    DWRITE_TEXT_METRICS textMetrics;
    ASSERT_COM_SUCCEEDED(_textLayout->GetMetrics(&textMetrics));
    return { textMetrics.width, textMetrics.height };
#elif defined(PLATFORM_LINUX)
    int width, height;

    pango_layout_get_pixel_size(_layout, &width, &height);
    return { static_cast<float>(width), static_cast<float>(height) };
#else
    return Size();
#endif
}

void TextBlock::textAlignment(TextAlignment alignment)
{
#if defined(PLATFORM_WINDOWS)
    _textFormat.textAlignment(alignment);
#elif defined(PLATFORM_LINUX)
    switch (alignment)
    {
    case TEXT_ALIGNMENT_LEFT:
        pango_layout_set_alignment(_layout, PANGO_ALIGN_LEFT);
        break;

    case TEXT_ALIGNMENT_RIGHT:
        pango_layout_set_alignment(_layout, PANGO_ALIGN_RIGHT);
        break;

    case TEXT_ALIGNMENT_CENTER:
        pango_layout_set_alignment(_layout, PANGO_ALIGN_CENTER);
        break;

    case TEXT_ALIGNMENT_JUSTIFIED:
        pango_layout_set_justify(_layout, true);
        break;
    }
#endif
}

void TextBlock::paragraphAlignment(ParagraphAlignment alignment)
{
#if defined(PLATFORM_WINDOWS)
    _textFormat.paragraphAlignment(alignment);
#elif defined(PLATFORM_LINUX)
#endif
}

// Image

Size Image::size() const
{
#if defined(PLATFORM_WINDOWS)
    D2D1_SIZE_F size = _bitmap->GetSize();
    return { size.width, size.height };
#elif defined(PLATFORM_LINUX)
    return { static_cast<float>(cairo_image_surface_get_width(_image)),
        static_cast<float>(cairo_image_surface_get_height(_image)) };
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

// Graphics

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

    cairo_save(cr);
    cairo_rectangle(cr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    cairo_clip(cr);
#endif
}

void Graphics::resetClip()
{
#if defined(PLATFORM_WINDOWS)
    _renderTarget->PopAxisAlignedClip();
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairo_restore(reinterpret_cast<cairo_t*>(_context));
#endif
}

void Graphics::drawText(const String& font, float fontSize, const String& text, const Rect& rect, Color color,
                        TextAlignment textAlignment, ParagraphAlignment paragraphAlignment, bool bold, bool wrap)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    __TextFormat textFormat(_textFactory, font.chars(), bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
                            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize);

    textFormat.textAlignment(textAlignment);
    textFormat.paragraphAlignment(paragraphAlignment);
    textFormat.wordWrap(wrap);

    _renderTarget->DrawText(reinterpret_cast<const WCHAR*>(text.chars()), text.length(), textFormat,
                            { rect.left, rect.top, rect.right, rect.bottom }, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP,
                            DWRITE_MEASURING_MODE_NATURAL);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);

    PangoFontDescription* fontDesc = pango_font_description_new();
    pango_font_description_set_family(fontDesc, font.chars());
    pango_font_description_set_weight(fontDesc, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
    pango_font_description_set_absolute_size(fontDesc, fontSize * PANGO_SCALE);

    PangoLayout* layout = pango_cairo_create_layout(cr);
    pango_layout_set_font_description(layout, fontDesc);
    pango_layout_set_text(layout, text.chars(), -1);

    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_width(layout, PANGO_SCALE * (rect.right - rect.left));
    if (wrap)
        pango_layout_set_height(layout, PANGO_SCALE * (rect.bottom - rect.top));

    switch (textAlignment)
    {
    case TEXT_ALIGNMENT_LEFT:
        pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
        break;

    case TEXT_ALIGNMENT_RIGHT:
        pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
        break;

    case TEXT_ALIGNMENT_CENTER:
        pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
        break;

    case TEXT_ALIGNMENT_JUSTIFIED:
        pango_layout_set_justify(layout, true);
        break;
    }

    cairo_move_to(cr, rect.left, rect.top);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
    pango_font_description_free(fontDesc);
#endif
}

void Graphics::drawText(const TextBlock& textBlock, const Point& pos, Color color)
{
#if defined(PLATFORM_WINDOWS)
    __SolidBrush brush(_renderTarget, D2D1::ColorF(color));
    _renderTarget->DrawTextLayout({ pos.x, pos.y }, textBlock._textLayout, brush);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairoSetColor(color);

    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);
    cairo_move_to(cr, pos.x, pos.y);
    pango_cairo_show_layout(cr, textBlock._layout);
#endif
}

void Graphics::drawImage(const Image& image, const Point& pos, const Size& size)
{
#if defined(PLATFORM_WINDOWS)
    D2D1_RECT_F rect;

    if (size.width > 0 && size.height > 0)
        rect = { pos.x, pos.y, pos.x + size.width, pos.y + size.height };
    else
    {
        Size imgSize = image.size();
        rect = { pos.x, pos.y, pos.x + imgSize.width, pos.y + imgSize.height };
    }

    _renderTarget->DrawBitmap(image._bitmap, rect);
#elif defined(PLATFORM_LINUX)
    ASSERT(_context);
    cairo_t* cr = reinterpret_cast<cairo_t*>(_context);

    if (size.width > 0 && size.height > 0)
    {
        Size imgSize = image.size();
        cairo_scale(cr, size.width / imgSize.width, size.height / imgSize.height);
    }

    cairo_set_source_surface(cr, image._image, pos.x, pos.y);
    cairo_paint(cr);
#endif
}

void Graphics::resize(int width, int height)
{
#if defined(PLATFORM_WINDOWS)
    ASSERT_COM_SUCCEEDED(_renderTarget->Resize({ static_cast<UINT32>(width), static_cast<UINT32>(height) }));
#elif defined(PLATFORM_LINUX)
    _size.width = width;
    _size.height = height;
#endif
}

Size Graphics::size() const
{
#if defined(PLATFORM_WINDOWS)
    D2D1_SIZE_F size = _renderTarget->GetSize();
    return { size.width, size.height };
#elif defined(PLATFORM_LINUX)
    return _size;
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
