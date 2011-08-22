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

static void MaskToAlpha(cairo_surface_t* mask, GdkPixbuf* pixbuf, int w, int h)
{
    GdkPixbuf* mask_pixbuf = gdk_pixbuf_get_from_surface( mask, 0, 0, w, h );
    guchar* p = gdk_pixbuf_get_pixels(pixbuf) + 3;
    const guchar* mask_data = gdk_pixbuf_get_pixels(mask_pixbuf);
    const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - w * 4;
    const int mask_rowpad = gdk_pixbuf_get_rowstride(mask_pixbuf) - w * 4;
    for (int y = h; y; y--, p += rowpad, mask_data += mask_rowpad)
    {
        for (int x = w; x; x--, p += 4, mask_data += 4)
        {
            *p = 255;
            // no need to test all 3 components,
            //   pixels are either (0,0,0) or (0xff,0xff,0xff)
            if (mask_data[3] == 255)
                *p = 0;
        }
    }
    g_object_unref(mask_pixbuf);
}

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
        cairo_surface_destroy (m_bitmap);
}

void wxMask::FreeData()
{
    if (m_bitmap)
    {
        cairo_surface_destroy (m_bitmap);
        m_bitmap = NULL;
    }
}

bool wxMask::InitFromColour(const wxBitmap& bitmap, const wxColour& colour)
{
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
        int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, w);
        m_bitmap = cairo_image_surface_create_for_data((unsigned char*)out, CAIRO_FORMAT_RGB24,
                w, h, stride);

        delete[] out;
    }
    else
    {
        cairo_content_t content = cairo_surface_get_content(bitmap.GetPixmap());
        cairo_surface_t* surface = cairo_surface_create_similar(bitmap.GetPixmap(),
                content, w, h);
        cairo_t* cr = cairo_create( surface );
        cairo_set_source_rgb( cr, colour.Red()/255.0, colour.Green()/255.0, colour.Blue()/255.0);
        cairo_rectangle( cr, 0, 0, w, h);
        cairo_clip( cr );
        cairo_fill( cr );
        cairo_destroy( cr );    

        m_bitmap = surface;
    }

    return true;
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

cairo_surface_t *wxMask::GetBitmap() const
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

    cairo_surface_t      *m_pixmap;
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
#ifndef __WXGTK30__
    if (m_bpp < 0)
        m_bpp = gdk_drawable_get_depth(wxGetRootWindow()->window);
#endif
    m_alphaRequested = depth == 32;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap)
        cairo_surface_destroy (m_pixmap);
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
        // Caller is in charge of allocation of memory.(JC)
        int stride = cairo_format_stride_for_width( CAIRO_FORMAT_RGB24, width );
        unsigned char *image_data = new unsigned char[stride * height];
        memset(image_data, 0, stride*height);
        unsigned int *data = (unsigned int *)image_data;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int move = x % 8;
                int bit = ( (*bits) & (1 << move ) ) >> move;
                
                if (bit == 1) 
                {
                    *data = 0;
                }
                else
                {
                    *data = (255 << 16 | 255 << 8 | 255);
                }
                ++data;

                if ((x+1)%8 == 0)
                {
                    ++bits;
                }
            }
        }

        SetPixmap(cairo_image_surface_create_for_data( image_data, CAIRO_FORMAT_RGB24, width, height, stride ));
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
        int stride = cairo_format_stride_for_width( CAIRO_FORMAT_RGB24, w );
        unsigned char *src_data = image.GetData();

        int rowpad = stride - 4 * w;
        // Caller is in charge of allocation of memory.(JC)
        unsigned char *data = (unsigned char *)malloc( stride * h );
        unsigned char *image_data = data;

        for (int y = 0; y < h; y++, data += rowpad)
        {
            for (int x = 0; x < w; x++, data += 4, src_data += 3)
            {
                // The order matters!
                data[3] = 0;
                data[2] = src_data[0];
                data[1] = src_data[1];
                data[0] = src_data[2];
            }
        }

        SetPixmap(cairo_image_surface_create_for_data( image_data, CAIRO_FORMAT_RGB24, w, h, stride ));
    }

    const wxByte* alpha = image.GetAlpha();
    if (alpha != NULL || image.HasMask())
    {
        // create mask as CAIRO_FORMAT_A1 image surface

        int stride = cairo_format_stride_for_width( CAIRO_FORMAT_A1, w );
        unsigned char *data = (unsigned char *)malloc( stride * h );
        memset(data, 0x00, stride*h);
        unsigned char *data_start = data;
        int byte_index = 0;
        int rowpad = stride - size_t((w+7)/8);

        if (alpha != NULL)
        {
            for (int y = 0; y < h; y++, byte_index += rowpad)
            {
                for (int x = 0; x < w; x++)
                {
                    if (*alpha++ < wxIMAGE_ALPHA_THRESHOLD)
                    {
                        int move = x % 8;
                        data[byte_index] |= (1 << move);
                    }
                    if ((x+1)%8 == 0)
                    {
                        // data[byte_index] ^= 0xff;
                        byte_index++;
                    }
                }
                if (w%8 != 0) 
                {
                    byte_index++;
                }
            }
        }
        else
        {
            const wxByte r_mask = image.GetMaskRed();
            const wxByte g_mask = image.GetMaskGreen();
            const wxByte b_mask = image.GetMaskBlue();
            const wxByte* in = image.GetData();

            for (int y = 0; y < h; y++, byte_index += rowpad)
            {
                for (int x = 0; x < w; x++, in += 3)
                {
                    if (in[0] == r_mask && in[1] == g_mask && in[2] == b_mask)
                    {
                        int move = x % 8;
                        data[byte_index] |= (1 << move);
                    }
                    if ((x+1)%8 == 0)
                    {
                        // data[byte_index] ^= 0xff;
                        byte_index++;
                    }
                }
                if (w%8 != 0) 
                {
                    byte_index++;
                }
            }
        }

        wxMask* mask = new wxMask;
        mask->m_bitmap = cairo_image_surface_create_for_data( data_start, CAIRO_FORMAT_A1, w, h, stride );
        SetMask(mask);
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
        GdkPixbuf* pixbuf = gdk_pixbuf_get_from_surface(GetPixmap(), 0, 0, w, h);

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
        GdkPixbuf* image_mask = gdk_pixbuf_get_from_surface(GetMask()->GetBitmap(), 0, 0, w, h);

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
        cairo_format_t format = cairo_image_surface_get_format(bmpData->m_pixmap);
        cairo_surface_t *surface = cairo_image_surface_create(format, w, h);
        cairo_t *cr = cairo_create(surface);
        cairo_set_source_surface(cr, bmpData->m_pixmap, 0, 0);
        cairo_rectangle(cr, rect.x, rect.y, w, h);
        cairo_clip(cr);
        cairo_paint(cr);
        cairo_destroy(cr);

        newRef->m_pixmap = surface;
        /* newRef->m_pixmap = cairo_surface_create_for_rectangle(bmpData->m_pixmap, rect.x, rect.y, w, h); */
    }
    if (bmpData->m_mask && bmpData->m_mask->m_bitmap)
    {
#ifdef __WXGTK30__
        newRef->m_mask = new wxMask;
        newRef->m_mask->m_bitmap = cairo_surface_create_for_rectangle(bmpData->m_mask->m_bitmap, rect.x, rect.y, w, h);
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
    
    cairo_format_t fmt = cairo_image_surface_get_format( pixmap );

    switch (fmt) {
    case CAIRO_FORMAT_A1:
        bmpData->m_bpp = 1;
        break;
    case CAIRO_FORMAT_A8:
        bmpData->m_bpp = 8;
        break;
    case CAIRO_FORMAT_RGB24:
        bmpData->m_bpp = 24;
        break;
    case CAIRO_FORMAT_ARGB32:
        bmpData->m_bpp = 32;
        break;
    case CAIRO_FORMAT_INVALID:
        bmpData->m_bpp = 0;
        break;
    }
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

        int w = bmpData->m_width;
        int h = bmpData->m_height;
        int stride = cairo_format_stride_for_width( CAIRO_FORMAT_RGB24, w );
        unsigned char *src_data;
        unsigned char *data;  

        src_data = gdk_pixbuf_get_pixels(bmpData->m_pixbuf);

        int rowpad = stride - 4 * w;
        // Caller is in charge of allocation of memory.(JC)
        data = (unsigned char *)malloc( stride * h );

        for (int y = 0; y < h; y++, data += rowpad)
        {
            for (int x = 0; x < w; x++, data += 4, src_data += 3)
            {
                data[3] = 0;
                data[2] = src_data[0];
                data[1] = src_data[1];
                data[0] = src_data[2];
            }
        }

        bmpData->m_pixmap = cairo_image_surface_create_for_data( data, CAIRO_FORMAT_RGB24, w, h, stride );
    }
    else
    {
        bmpData->m_pixmap = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
            bmpData->m_width, bmpData->m_height);
    }
    return bmpData->m_pixmap;
}

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
    cairo_surface_t* mask = NULL;
    if (bmpData->m_mask)
        mask = bmpData->m_mask->m_bitmap;
    const bool useAlpha = bmpData->m_alphaRequested || mask;
    bmpData->m_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, useAlpha, 8, w, h);
    if (bmpData->m_pixmap)
    {
        g_object_unref(bmpData->m_pixbuf);
        bmpData->m_pixbuf = gdk_pixbuf_get_from_surface( bmpData->m_pixmap, 0, 0, w, h );
    }
    if (mask) 
    {
        if (!gdk_pixbuf_get_has_alpha(bmpData->m_pixbuf)) 
        {
            GdkPixbuf *new_pixbuf = gdk_pixbuf_add_alpha(bmpData->m_pixbuf, FALSE, 0, 0, 0);
            g_object_unref(bmpData->m_pixbuf);
            bmpData->m_pixbuf = new_pixbuf;

        }
        MaskToAlpha(mask, bmpData->m_pixbuf, w, h);
    }
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
        cairo_content_t content = cairo_surface_get_content(oldRef->m_pixmap);
        newRef->m_pixmap = cairo_surface_create_similar(
                oldRef->m_pixmap, content, oldRef->m_width, oldRef->m_height);
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
