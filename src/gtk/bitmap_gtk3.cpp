/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/image.h"
    #include "wx/colour.h"
#endif

#include "wx/rawbmp.h"

#include "wx/gtk/private/object.h"
#include "wx/gtk/private/gtk2-compat.h"

#include <gtk/gtk.h>

extern GtkWidget *wxGetRootWindow();

#ifdef __WXGTK30__
static void PixmapToPixbuf(cairo_surface_t* pixmap, GdkPixbuf* pixbuf, int w, int h)
{
    pixbuf = gdk_pixbuf_get_from_surface( pixmap, 0, 0, w, h );
}
#else
static void PixmapToPixbuf(GdkPixmap* pixmap, GdkPixbuf* pixbuf, int w, int h)
{
    gdk_pixbuf_get_from_drawable(pixbuf, pixmap, NULL, 0, 0, 0, 0, w, h);
    if (gdk_drawable_get_depth(pixmap) == 1)
    {
        // invert to match XBM convention
        guchar* p = gdk_pixbuf_get_pixels(pixbuf);
        const int inc = 3 + int(gdk_pixbuf_get_has_alpha(pixbuf) != 0);
        const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - w * inc;
        for (int y = h; y; y--, p += rowpad)
            for (int x = w; x; x--, p += inc)
            {
                // pixels are either (0,0,0) or (0xff,0xff,0xff)
                p[0] = ~p[0];
                p[1] = ~p[1];
                p[2] = ~p[2];
            }
    }
}
#endif

#ifdef __WXGTK30__
static void MaskToAlpha(cairo_surface_t* mask, GdkPixbuf* pixbuf, int w, int h)
{
    GdkPixbuf* mask_pixbuf = gdk_pixbuf_get_from_surface( mask, 0, 0, w, h );
    guchar* p = gdk_pixbuf_get_pixels(pixbuf) + 3;
    const guchar* mask_data = gdk_pixbuf_get_pixels(mask_pixbuf);
    const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - w * 4;
    const int mask_rowpad = gdk_pixbuf_get_rowstride(mask_pixbuf) - w * 3;
    for (int y = h; y; y--, p += rowpad, mask_data += mask_rowpad)
    {
        for (int x = w; x; x--, p += 4, mask_data += 3)
        {
            *p = 255;
            // no need to test all 3 components,
            //   pixels are either (0,0,0) or (0xff,0xff,0xff)
            if (mask_data[0] == 0)
                *p = 0;
        }
    }
    g_object_unref(mask_pixbuf);
}
#else
static void MaskToAlpha(GdkPixmap* mask, GdkPixbuf* pixbuf, int w, int h)
{
    GdkPixbuf* mask_pixbuf = gdk_pixbuf_get_from_drawable(
        NULL, mask, NULL, 0, 0, 0, 0, w, h);
    guchar* p = gdk_pixbuf_get_pixels(pixbuf) + 3;
    const guchar* mask_data = gdk_pixbuf_get_pixels(mask_pixbuf);
    const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - w * 4;
    const int mask_rowpad = gdk_pixbuf_get_rowstride(mask_pixbuf) - w * 3;
    for (int y = h; y; y--, p += rowpad, mask_data += mask_rowpad)
    {
        for (int x = w; x; x--, p += 4, mask_data += 3)
        {
            *p = 255;
            // no need to test all 3 components,
            //   pixels are either (0,0,0) or (0xff,0xff,0xff)
            if (mask_data[0] == 0)
                *p = 0;
        }
    }
    g_object_unref(mask_pixbuf);
}
#endif

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask, wxMaskBase)

wxMask::wxMask()
{
    m_bitmap = NULL;
}

wxMask::wxMask(const wxMask& mask)
{
    if ( !mask.m_bitmap )
    {
        m_bitmap = NULL;
        return;
    }

    gint w, h;

    // create a copy of an existing mask
#ifdef __WXGTK30__
    // I don't know if there is a simpler way to copy cairo_image_surface. (JC)
    w = cairo_image_surface_get_width( mask.m_bitmap );
    h = cairo_image_surface_get_height( mask.m_bitmap );

    cairo_format_t format;
    int stride;
    unsigned char *src_data;
    unsigned char *data;  

    stride = cairo_image_surface_get_stride( mask.m_bitmap );
    src_data = cairo_image_surface_get_data( mask.m_bitmap );
    format = cairo_image_surface_get_format( mask.m_bitmap );

    // Caller is in charge of allocation of memory.(JC)
    data = (unsigned char *)malloc( stride * h );
    memcpy( data, src_data, stride * h );

    m_bitmap = cairo_image_surface_create_for_data( data, format, w, h, stride );
#else
    gdk_drawable_get_size(mask.m_bitmap, &w, &h);
    m_bitmap = gdk_pixmap_new(mask.m_bitmap, w, h, 1);

    wxGtkObject<GdkGC> gc(gdk_gc_new(m_bitmap));
    gdk_draw_drawable(m_bitmap, gc, mask.m_bitmap, 0, 0, 0, 0, -1, -1);
#endif
}

wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
    m_bitmap = NULL;
    InitFromColour(bitmap, colour);
}

#if wxUSE_PALETTE
wxMask::wxMask( const wxBitmap& bitmap, int paletteIndex )
{
    m_bitmap = NULL;
    Create( bitmap, paletteIndex );
}
#endif // wxUSE_PALETTE

wxMask::wxMask( const wxBitmap& bitmap )
{
    m_bitmap = NULL;
    InitFromMonoBitmap(bitmap);
}

wxMask::~wxMask()
{
    if (m_bitmap)
        g_object_unref (m_bitmap);
}

void wxMask::FreeData()
{
    if (m_bitmap)
    {
        g_object_unref (m_bitmap);
        m_bitmap = NULL;
    }
}

bool wxMask::InitFromColour(const wxBitmap& bitmap, const wxColour& colour)
{
#ifdef __WXGTK30__
    wxFAIL_MSG("Not implemented for GTK+3.0 backend");
#else
    const int w = bitmap.GetWidth();
    const int h = bitmap.GetHeight();

    // create mask as XBM format bitmap

    // one bit per pixel, each row starts on a byte boundary
    const size_t out_size = size_t((w + 7) / 8) * unsigned(h);
    wxByte* out = new wxByte[out_size];
    // set bits are unmasked
    memset(out, 0xff, out_size);
    unsigned bit_index = 0;
    if (bitmap.HasPixbuf())
    {
        const wxByte r_mask = colour.Red();
        const wxByte g_mask = colour.Green();
        const wxByte b_mask = colour.Blue();
        GdkPixbuf* pixbuf = bitmap.GetPixbuf();
        const wxByte* in = gdk_pixbuf_get_pixels(pixbuf);
        const int inc = 3 + int(gdk_pixbuf_get_has_alpha(pixbuf) != 0);
        const int rowpadding = gdk_pixbuf_get_rowstride(pixbuf) - inc * w;
        for (int y = 0; y < h; y++, in += rowpadding)
        {
            for (int x = 0; x < w; x++, in += inc, bit_index++)
                if (in[0] == r_mask && in[1] == g_mask && in[2] == b_mask)
                    out[bit_index >> 3] ^= 1 << (bit_index & 7);
            // move index to next byte boundary
            bit_index = (bit_index + 7) & ~7u;
        }
    }
    else
    {
        GdkImage* image = gdk_drawable_get_image(bitmap.GetPixmap(), 0, 0, w, h);
        GdkColormap* colormap = gdk_image_get_colormap(image);
        guint32 mask_pixel;
        if (colormap == NULL)
            // mono bitmap, white is pixel value 0
            mask_pixel = guint32(colour.Red() != 255 || colour.Green() != 255 || colour.Blue() != 255);
        else
        {
            wxColor c(colour);
            c.CalcPixel(colormap);
            mask_pixel = c.GetPixel();
        }
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++, bit_index++)
                if (gdk_image_get_pixel(image, x, y) == mask_pixel)
                    out[bit_index >> 3] ^= 1 << (bit_index & 7);
            bit_index = (bit_index + 7) & ~7u;
        }
        g_object_unref(image);
    }

    m_bitmap = gdk_bitmap_create_from_data(wxGetRootWindow()->window, (char*)out, w, h);

    delete[] out;
    return true;
#endif
}

bool wxMask::InitFromMonoBitmap(const wxBitmap& bitmap)
{
    if (!bitmap.IsOk()) return false;

    wxCHECK_MSG( bitmap.GetDepth() == 1, false, wxT("Cannot create mask from colour bitmap") );

    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

#ifdef __WXGTK30__
    // I don't know if there is a simpler way to copy cairo_image_surface. (JC)
    cairo_format_t format;
    int stride;
    unsigned char *src_data;
    unsigned char *data;  

    stride = cairo_image_surface_get_stride( bitmap.GetPixmap() );
    src_data = cairo_image_surface_get_data( bitmap.GetPixmap() );
    format = cairo_image_surface_get_format( bitmap.GetPixmap() );

    // Caller is in charge of allocation of memory.(JC)
    data = (unsigned char *)malloc( stride * h );
    memcpy( data, src_data, stride * h );

    m_bitmap = cairo_image_surface_create_for_data( data, format, w, h, stride );
#else
    m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, w, h, 1 );

    if (!m_bitmap) return false;

    wxGtkObject<GdkGC> gc(gdk_gc_new( m_bitmap ));
    gdk_gc_set_function(gc, GDK_COPY_INVERT);
    gdk_draw_drawable(m_bitmap, gc, bitmap.GetPixmap(), 0, 0, 0, 0, w, h);
#endif

    return true;
}

#ifdef __WXGTK30__
cairo_surface_t *wxMask::GetBitmap() const
#else
GdkBitmap *wxMask::GetBitmap() const
#endif
{
    return m_bitmap;
}

//-----------------------------------------------------------------------------
// wxBitmapRefData
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxGDIRefData
{
public:
    wxBitmapRefData(int width, int height, int depth);
    virtual ~wxBitmapRefData();

    virtual bool IsOk() const;

#ifdef __WXGTK30__
    cairo_surface_t      *m_pixmap;
#else
    GdkPixmap      *m_pixmap;
#endif
    GdkPixbuf      *m_pixbuf;
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
    bool m_alphaRequested;

private:
    // We don't provide a copy ctor as copying m_pixmap and m_pixbuf properly
    // is expensive and we don't want to do it implicitly (and possibly
    // accidentally). wxBitmap::CloneGDIRefData() which does need to do it does
    // it explicitly itself.
    wxDECLARE_NO_COPY_CLASS(wxBitmapRefData);
};

wxBitmapRefData::wxBitmapRefData(int width, int height, int depth)
{
    m_pixmap = NULL;
    m_pixbuf = NULL;
    m_mask = NULL;
    m_width = width;
    m_height = height;
    m_bpp = depth;
#ifdef __WXGTK20__
    if (m_bpp < 0)
        m_bpp = gdk_drawable_get_depth(wxGetRootWindow()->window);
#endif
    m_alphaRequested = depth == 32;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap)
        g_object_unref (m_pixmap);
    if (m_pixbuf)
        g_object_unref (m_pixbuf);
    delete m_mask;
}

bool wxBitmapRefData::IsOk() const
{
    return m_bpp != 0;
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

#define M_BMPDATA static_cast<wxBitmapRefData*>(m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxASSERT(depth == 1);
    if (width > 0 && height > 0 && depth == 1)
    {
#ifdef __WXGTK30__
        // NB: Keep in mind that Cairo images must have a rowstride of 4 bytes, so you will need to
        //     align your data properly
        int stride = cairo_format_stride_for_width( CAIRO_FORMAT_A1, width );
        // FIXME Pass in CAIRO_FORMAT_A1 is probably not right. (JC)
        SetPixmap(cairo_image_surface_create_for_data((unsigned char*)bits, CAIRO_FORMAT_A1, width, height, stride));
#else
        SetPixmap(gdk_bitmap_create_from_data(gtk_widget_get_window(wxGetRootWindow()), bits, width, height));
#endif
    }
}

wxBitmap::wxBitmap(const char* const* bits)
{
    wxCHECK2_MSG(bits != NULL, return, wxT("invalid bitmap data"));

#ifdef __WXGTK30__
    SetPixbuf(gdk_pixbuf_new_from_xpm_data( (const char**)bits ));
#else
    GdkBitmap* mask = NULL;
    SetPixmap(gdk_pixmap_create_from_xpm_d(gtk_widget_get_window(wxGetRootWindow()), &mask, NULL, const_cast<char**>(bits)));
    if (!M_BMPDATA)
        return;

    if (M_BMPDATA->m_pixmap != NULL && mask != NULL)
    {
        M_BMPDATA->m_mask = new wxMask;
        M_BMPDATA->m_mask->m_bitmap = mask;
    }
#endif
}

wxBitmap::~wxBitmap()
{
}

bool wxBitmap::Create( int width, int height, int depth )
{
    UnRef();
    wxCHECK_MSG(width >= 0 && height >= 0, false, "invalid bitmap size");
    m_refData = new wxBitmapRefData(width, height, depth);
    return true;
}

#if wxUSE_IMAGE

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG( image.IsOk(), false, wxT("invalid image") );
    wxCHECK_MSG( depth == -1 || depth == 1, false, wxT("invalid bitmap depth") );

    if (image.GetWidth() <= 0 || image.GetHeight() <= 0)
        return false;

    // create pixbuf if image has alpha and requested depth is compatible
    if (image.HasAlpha() && (depth == -1 || depth == 32))
        return CreateFromImageAsPixbuf(image);

    // otherwise create pixmap, if alpha is present it will be converted to mask
    return CreateFromImageAsPixmap(image, depth);
}

bool wxBitmap::CreateFromImageAsPixmap(const wxImage& image, int depth)
{
    const int w = image.GetWidth();
    const int h = image.GetHeight();

    if (depth == 1)
    {
        // create XBM format bitmap
#ifdef __WXGTK30__
        // I don't know if there is a simpler way to copy cairo_image_surface. (JC)
        cairo_format_t format;
        int stride;
        unsigned char *src_data;
        unsigned char *data;  

        stride = cairo_format_stride_for_width( CAIRO_FORMAT_A1, w );
        src_data = image.GetData();

        // Caller is in charge of allocation of memory.(JC)
        data = (unsigned char *)malloc( stride * h );
        memcpy( data, src_data, stride * h );

        SetPixmap(cairo_image_surface_create_for_data( data, CAIRO_FORMAT_A1, w, h, stride ));
#else
        // one bit per pixel, each row starts on a byte boundary
        const size_t out_size = size_t((w + 7) / 8) * unsigned(h);
        wxByte* out = new wxByte[out_size];
        // set bits are black
        memset(out, 0xff, out_size);
        const wxByte* in = image.GetData();
        unsigned bit_index = 0;
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++, in += 3, bit_index++)
                if (in[0] == 255 && in[1] == 255 && in[2] == 255)
                    out[bit_index >> 3] ^= 1 << (bit_index & 7);
            // move index to next byte boundary
            bit_index = (bit_index + 7) & ~7u;
        }
        SetPixmap(gdk_bitmap_create_from_data(gtk_widget_get_window(wxGetRootWindow()), (char*)out, w, h));
        delete[] out;

        if (!M_BMPDATA)     // SetPixmap may have failed
            return false;
#endif

    }
    else
    {
#ifdef __WXGTK30__
        // I don't know if there is a simpler way to copy cairo_image_surface. (JC)
        cairo_format_t format;
        int stride;
        unsigned char *src_data;
        unsigned char *data;  

        stride = cairo_format_stride_for_width( CAIRO_FORMAT_RGB24, w );
        src_data = image.GetData();

        // Caller is in charge of allocation of memory.(JC)
        data = (unsigned char *)malloc( stride * h );
        memcpy( data, src_data, stride * h );

        SetPixmap(cairo_image_surface_create_for_data( data, CAIRO_FORMAT_RGB24, w, h, stride ));
#else
        SetPixmap(gdk_pixmap_new(wxGetRootWindow()->window, w, h, depth));
        if (!M_BMPDATA)
            return false;

        wxGtkObject<GdkGC> gc(gdk_gc_new(M_BMPDATA->m_pixmap));
        gdk_draw_rgb_image(
            M_BMPDATA->m_pixmap, gc,
            0, 0, w, h,
            GDK_RGB_DITHER_NONE, image.GetData(), w * 3);
#endif
    }

    const wxByte* alpha = image.GetAlpha();
    if (alpha != NULL || image.HasMask())
    {
        // create mask as XBM format bitmap

        const size_t out_size = size_t((w + 7) / 8) * unsigned(h);
        wxByte* out = new wxByte[out_size];
        memset(out, 0xff, out_size);
        unsigned bit_index = 0;
        if (alpha != NULL)
        {
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++, bit_index++)
                    if (*alpha++ < wxIMAGE_ALPHA_THRESHOLD)
                        out[bit_index >> 3] ^= 1 << (bit_index & 7);
                bit_index = (bit_index + 7) & ~7u;
            }
        }
        else
        {
            const wxByte r_mask = image.GetMaskRed();
            const wxByte g_mask = image.GetMaskGreen();
            const wxByte b_mask = image.GetMaskBlue();
            const wxByte* in = image.GetData();
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++, in += 3, bit_index++)
                    if (in[0] == r_mask && in[1] == g_mask && in[2] == b_mask)
                        out[bit_index >> 3] ^= 1 << (bit_index & 7);
                bit_index = (bit_index + 7) & ~7u;
            }
        }
        wxMask* mask = new wxMask;
#ifdef __WXGTK30__
        // I don't know if there is a simpler way to copy cairo_image_surface. (JC)
        int stride;
        unsigned char *src_data;
        unsigned char *data;  

        // FIXME: CAIRO_FORMAT_A1 is good here? (JC)
        stride = cairo_format_stride_for_width( CAIRO_FORMAT_A1, w );

        // Caller is in charge of allocation of memory.(JC)
        data = (unsigned char *)malloc( stride * h );
        memcpy( data, out, stride * h );

        mask->m_bitmap = cairo_image_surface_create_for_data( data, CAIRO_FORMAT_A1, w, h, stride );
#else
        mask->m_bitmap = gdk_bitmap_create_from_data(M_BMPDATA->m_pixmap, (char*)out, w, h);
#endif
        SetMask(mask);
        delete[] out;
    }
    return IsOk();
}

bool wxBitmap::CreateFromImageAsPixbuf(const wxImage& image)
{
    wxASSERT(image.HasAlpha());

    int width = image.GetWidth();
    int height = image.GetHeight();

    Create(width, height, 32);
    GdkPixbuf* pixbuf = GetPixbuf();
    if (!pixbuf)
        return false;

    // Copy the data:
    const unsigned char* in = image.GetData();
    unsigned char *out = gdk_pixbuf_get_pixels(pixbuf);
    unsigned char *alpha = image.GetAlpha();

    int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - 4 * width;

    for (int y = 0; y < height; y++, out += rowpad)
    {
        for (int x = 0; x < width; x++, alpha++, out += 4, in += 3)
        {
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
            out[3] = *alpha;
        }
    }

    return true;
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( IsOk(), wxNullImage, wxT("invalid bitmap") );

    const int w = GetWidth();
    const int h = GetHeight();
    wxImage image(w, h, false);
    unsigned char *data = image.GetData();

    wxCHECK_MSG(data != NULL, wxNullImage, wxT("couldn't create image") );

    // prefer pixbuf if available, it will preserve alpha and should be quicker
    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = GetPixbuf();
        unsigned char* alpha = NULL;
        if (gdk_pixbuf_get_has_alpha(pixbuf))
        {
            image.SetAlpha();
            alpha = image.GetAlpha();
        }
        const unsigned char* in = gdk_pixbuf_get_pixels(pixbuf);
        unsigned char *out = data;
        const int inc = 3 + int(alpha != NULL);
        const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - inc * w;

        for (int y = 0; y < h; y++, in += rowpad)
        {
            for (int x = 0; x < w; x++, in += inc, out += 3)
            {
                out[0] = in[0];
                out[1] = in[1];
                out[2] = in[2];
                if (alpha != NULL)
                    *alpha++ = in[3];
            }
        }
    }
    else
    {
#ifdef __WXGTK30__
        wxFAIL_MSG("Not implemented in wxGTK3");
#else
        GdkPixmap* pixmap = GetPixmap();
        GdkPixmap* pixmap_invert = NULL;
        if (GetDepth() == 1)
        {
            // mono bitmaps are inverted, i.e. 0 is white
            pixmap_invert = gdk_pixmap_new(pixmap, w, h, 1);
            wxGtkObject<GdkGC> gc(gdk_gc_new(pixmap_invert));
            gdk_gc_set_function(gc, GDK_COPY_INVERT);
            gdk_draw_drawable(pixmap_invert, gc, pixmap, 0, 0, 0, 0, w, h);
            pixmap = pixmap_invert;
        }
        // create a pixbuf which shares data with the wxImage
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(
            data, GDK_COLORSPACE_RGB, false, 8, w, h, 3 * w, NULL, NULL);

        gdk_pixbuf_get_from_drawable(pixbuf, pixmap, NULL, 0, 0, 0, 0, w, h);

        g_object_unref(pixbuf);
        if (pixmap_invert != NULL)
            g_object_unref(pixmap_invert);
#endif
    }
    // convert mask, unless there is already alpha
    if (GetMask() && !image.HasAlpha())
    {
        // we hard code the mask colour for now but we could also make an
        // effort (and waste time) to choose a colour not present in the
        // image already to avoid having to fudge the pixels below --
        // whether it's worth to do it is unclear however
        const int MASK_RED = 1;
        const int MASK_GREEN = 2;
        const int MASK_BLUE = 3;
        const int MASK_BLUE_REPLACEMENT = 2;

        image.SetMaskColour(MASK_RED, MASK_GREEN, MASK_BLUE);
#ifdef __WXGTK30__
        GdkPixbuf* image_mask = gdk_pixbuf_get_from_surface(GetMask()->GetBitmap(), 0, 0, w, h);
#else
        GdkImage* image_mask = gdk_drawable_get_image(GetMask()->GetBitmap(), 0, 0, w, h);
#endif

        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++, data += 3)
            {
#ifdef __WXGTK30__
                guchar *p = gdk_pixbuf_get_pixels(image_mask);
                // FIXME The following line is probably wrong. (JC)
                if (*(p+x+y*w) == 0)
#else
                if (gdk_image_get_pixel(image_mask, x, y) == 0)
#endif
                {
                    data[0] = MASK_RED;
                    data[1] = MASK_GREEN;
                    data[2] = MASK_BLUE;
                }
                else if (data[0] == MASK_RED && data[1] == MASK_GREEN && data[2] == MASK_BLUE)
                {
                    // we have to fudge the colour a bit to prevent
                    // this pixel from appearing transparent
                    data[2] = MASK_BLUE_REPLACEMENT;
                }
            }
        }
        g_object_unref(image_mask);
    }

    return image;
}

#endif // wxUSE_IMAGE

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
    wxCHECK_RET( IsOk(), wxT("invalid bitmap") );

    AllocExclusive();
    delete M_BMPDATA->m_mask;
    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    *this = icon;
    return IsOk();
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxBitmap ret;

    wxCHECK_MSG(IsOk(), ret, wxT("invalid bitmap"));

    const int w = rect.width;
    const int h = rect.height;
    const wxBitmapRefData* bmpData = M_BMPDATA;

    wxCHECK_MSG(rect.x >= 0 && rect.y >= 0 &&
                rect.x + w <= bmpData->m_width &&
                rect.y + h <= bmpData->m_height,
                ret, wxT("invalid bitmap region"));

    wxBitmapRefData * const newRef = new wxBitmapRefData(w, h, bmpData->m_bpp);
    ret.m_refData = newRef;

    if (bmpData->m_pixbuf)
    {
        GdkPixbuf* pixbuf =
            gdk_pixbuf_new_subpixbuf(bmpData->m_pixbuf, rect.x, rect.y, w, h);
        newRef->m_pixbuf = gdk_pixbuf_copy(pixbuf);
        g_object_unref(pixbuf);
    }
    if (bmpData->m_pixmap)
    {
#ifdef __WXGTK30__
        wxFAIL_MSG("Not yet implemented in wxGTK3");
#else
        newRef->m_pixmap = gdk_pixmap_new(bmpData->m_pixmap, w, h, -1);
        GdkGC* gc = gdk_gc_new(newRef->m_pixmap);
        gdk_draw_drawable(
            newRef->m_pixmap, gc, bmpData->m_pixmap, rect.x, rect.y, 0, 0, w, h);
        g_object_unref(gc);
#endif
    }
    if (bmpData->m_mask && bmpData->m_mask->m_bitmap)
    {
#ifdef __WXGTK30__
        wxFAIL_MSG("Not yet implemented in wxGTK3");
#else
        GdkPixmap* sub_mask = gdk_pixmap_new(bmpData->m_mask->m_bitmap, w, h, 1);
        newRef->m_mask = new wxMask;
        newRef->m_mask->m_bitmap = sub_mask;
        GdkGC* gc = gdk_gc_new(sub_mask);
        gdk_draw_drawable(
            sub_mask, gc, bmpData->m_mask->m_bitmap, rect.x, rect.y, 0, 0, w, h);
        g_object_unref(gc);
#endif
    }

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, wxBitmapType type, const wxPalette *WXUNUSED(palette) ) const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

#if wxUSE_IMAGE
    wxImage image = ConvertToImage();
    if (image.IsOk() && image.SaveFile(name, type))
        return true;
#endif
    const char* type_name = NULL;
    switch (type)
    {
        case wxBITMAP_TYPE_BMP:  type_name = "bmp";  break;
        case wxBITMAP_TYPE_ICO:  type_name = "ico";  break;
        case wxBITMAP_TYPE_JPEG: type_name = "jpeg"; break;
        case wxBITMAP_TYPE_PNG:  type_name = "png";  break;
        default: break;
    }
    return type_name &&
        gdk_pixbuf_save(GetPixbuf(), name.fn_str(), type_name, NULL, NULL);
}

bool wxBitmap::LoadFile( const wxString &name, wxBitmapType type )
{
#if wxUSE_IMAGE
    wxImage image;
    if (image.LoadFile(name, type) && image.IsOk())
        *this = wxBitmap(image);
    else
#endif
    {
        wxUnusedVar(type); // The type is detected automatically by GDK.

        UnRef();
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(name.fn_str(), NULL);
        if (pixbuf)
            SetPixbuf(pixbuf);
    }

    return IsOk();
}

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    return NULL;
}

void wxBitmap::SetPalette(const wxPalette& WXUNUSED(palette))
{
    // TODO
}
#endif // wxUSE_PALETTE

void wxBitmap::SetHeight( int height )
{
    AllocExclusive();
    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
    AllocExclusive();
    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
    AllocExclusive();
    M_BMPDATA->m_bpp = depth;
}

#ifdef __WXGTK30__
void wxBitmap::SetPixmap( cairo_surface_t *pixmap )
{
    UnRef();

    if (!pixmap)
        return;

    int w, h;
    // I don't know if there is a simpler way to copy cairo_image_surface. (JC)
    w = cairo_image_surface_get_width( pixmap );
    h = cairo_image_surface_get_height( pixmap );

    wxBitmapRefData* bmpData = new wxBitmapRefData(w, h, 0);
    m_refData = bmpData;
    bmpData->m_pixmap = pixmap;
}

cairo_surface_t *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    wxBitmapRefData* bmpData = M_BMPDATA;
    if (bmpData->m_pixmap)
        return bmpData->m_pixmap;

    if (bmpData->m_pixbuf)
    {
        cairo_surface_t** mask_pixmap = NULL;
        if (gdk_pixbuf_get_has_alpha(bmpData->m_pixbuf))
        {
            // make new mask from alpha
            delete bmpData->m_mask;
            bmpData->m_mask = new wxMask;
            mask_pixmap = &bmpData->m_mask->m_bitmap;
        }
        wxFAIL_MSG("Not implemented in wxGTK3");
        // gdk_pixbuf_render_pixmap_and_mask(
        //     bmpData->m_pixbuf, &bmpData->m_pixmap, mask_pixmap, 128);
    }
    else
    {
        wxFAIL_MSG("Not implemented in wxGTK3");
        // bmpData->m_pixmap = gdk_pixmap_new(wxGetRootWindow()->window,
        //     bmpData->m_width, bmpData->m_height, bmpData->m_bpp == 1 ? 1 : -1);
    }
    return bmpData->m_pixmap;
}
#else
void wxBitmap::SetPixmap( GdkPixmap *pixmap )
{
    UnRef();

    if (!pixmap)
        return;

    int w, h;
    gdk_drawable_get_size(pixmap, &w, &h);
    wxBitmapRefData* bmpData = new wxBitmapRefData(w, h, 0);
    m_refData = bmpData;
    bmpData->m_pixmap = pixmap;
    bmpData->m_bpp = gdk_drawable_get_depth(pixmap);
}

GdkPixmap *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    wxBitmapRefData* bmpData = M_BMPDATA;
    if (bmpData->m_pixmap)
        return bmpData->m_pixmap;

    if (bmpData->m_pixbuf)
    {
        GdkPixmap** mask_pixmap = NULL;
        if (gdk_pixbuf_get_has_alpha(bmpData->m_pixbuf))
        {
            // make new mask from alpha
            delete bmpData->m_mask;
            bmpData->m_mask = new wxMask;
            mask_pixmap = &bmpData->m_mask->m_bitmap;
        }
        gdk_pixbuf_render_pixmap_and_mask(
            bmpData->m_pixbuf, &bmpData->m_pixmap, mask_pixmap, 128);
    }
    else
    {
        bmpData->m_pixmap = gdk_pixmap_new(wxGetRootWindow()->window,
            bmpData->m_width, bmpData->m_height, bmpData->m_bpp == 1 ? 1 : -1);
    }
    return bmpData->m_pixmap;
}
#endif

bool wxBitmap::HasPixmap() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixmap != NULL;
}

GdkPixbuf *wxBitmap::GetPixbuf() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    wxBitmapRefData* bmpData = M_BMPDATA;
    if (bmpData->m_pixbuf)
        return bmpData->m_pixbuf;

    const int w = bmpData->m_width;
    const int h = bmpData->m_height;
#ifdef __WXGTK30__
    cairo_surface_t* mask = NULL;
#else
    GdkPixmap* mask = NULL;
#endif
    if (bmpData->m_mask)
        mask = bmpData->m_mask->m_bitmap;
    const bool useAlpha = bmpData->m_alphaRequested || mask;
    bmpData->m_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, useAlpha, 8, w, h);
    if (bmpData->m_pixmap)
        PixmapToPixbuf(bmpData->m_pixmap, bmpData->m_pixbuf, w, h);
    if (mask)
        MaskToAlpha(mask, bmpData->m_pixbuf, w, h);
    return bmpData->m_pixbuf;
}

bool wxBitmap::HasPixbuf() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixbuf != NULL;
}

void wxBitmap::SetPixbuf(GdkPixbuf* pixbuf)
{
    UnRef();

    if (!pixbuf)
        return;

    int depth = -1;
    if (gdk_pixbuf_get_has_alpha(pixbuf))
        depth = 32;
    m_refData = new wxBitmapRefData(
        gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf), depth);

    M_BMPDATA->m_pixbuf = pixbuf;
}

void wxBitmap::PurgeOtherRepresentations(wxBitmap::Representation keep)
{
    if (keep == Pixmap && HasPixbuf())
    {
        g_object_unref (M_BMPDATA->m_pixbuf);
        M_BMPDATA->m_pixbuf = NULL;
    }
    if (keep == Pixbuf && HasPixmap())
    {
        g_object_unref (M_BMPDATA->m_pixmap);
        M_BMPDATA->m_pixmap = NULL;
    }
}

#ifdef wxHAS_RAW_BITMAP
void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    void* bits = NULL;
    GdkPixbuf *pixbuf = GetPixbuf();
    const bool hasAlpha = HasAlpha();

    // allow access if bpp is valid and matches existence of alpha
    if ( pixbuf && ((bpp == 24 && !hasAlpha) || (bpp == 32 && hasAlpha)) )
    {
        data.m_height = gdk_pixbuf_get_height( pixbuf );
        data.m_width = gdk_pixbuf_get_width( pixbuf );
        data.m_stride = gdk_pixbuf_get_rowstride( pixbuf );
        bits = gdk_pixbuf_get_pixels(pixbuf);
    }
    return bits;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(data))
{
}
#endif // wxHAS_RAW_BITMAP

bool wxBitmap::HasAlpha() const
{
    const wxBitmapRefData* bmpData = M_BMPDATA;
    return bmpData && (bmpData->m_alphaRequested ||
        (bmpData->m_pixbuf && gdk_pixbuf_get_has_alpha(bmpData->m_pixbuf)));
}

wxGDIRefData* wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData(0, 0, 0);
}

wxGDIRefData* wxBitmap::CloneGDIRefData(const wxGDIRefData* data) const
{
    const wxBitmapRefData* oldRef = static_cast<const wxBitmapRefData*>(data);
    wxBitmapRefData * const newRef = new wxBitmapRefData(oldRef->m_width,
                                                         oldRef->m_height,
                                                         oldRef->m_bpp);
    if (oldRef->m_pixmap != NULL)
    {
#ifdef __WXGTK30__
        wxFAIL_MSG("Not implemented");
#else
        newRef->m_pixmap = gdk_pixmap_new(
            oldRef->m_pixmap, oldRef->m_width, oldRef->m_height,
            // use pixmap depth, m_bpp may not match
            gdk_drawable_get_depth(oldRef->m_pixmap));
        wxGtkObject<GdkGC> gc(gdk_gc_new(newRef->m_pixmap));
        gdk_draw_drawable(
            newRef->m_pixmap, gc, oldRef->m_pixmap, 0, 0, 0, 0, -1, -1);
#endif
    }
    if (oldRef->m_pixbuf != NULL)
    {
        newRef->m_pixbuf = gdk_pixbuf_copy(oldRef->m_pixbuf);
    }
    if (oldRef->m_mask != NULL)
    {
        newRef->m_mask = new wxMask(*oldRef->m_mask);
    }

    return newRef;
}

/* static */ void wxBitmap::InitStandardHandlers()
{
    // TODO: Insert handler based on GdkPixbufs handler later
}
