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
    #include "wx/app.h"
    #include "wx/palette.h"
    #include "wx/icon.h"
    #include "wx/math.h"
    #include "wx/image.h"
#endif

#include "wx/rawbmp.h"

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern GtkWidget *wxGetRootWindow();

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask()
{
    m_bitmap = (GdkBitmap *) NULL;
}

wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
    m_bitmap = (GdkBitmap *) NULL;
    Create( bitmap, colour );
}

#if wxUSE_PALETTE
wxMask::wxMask( const wxBitmap& bitmap, int paletteIndex )
{
    m_bitmap = (GdkBitmap *) NULL;
    Create( bitmap, paletteIndex );
}
#endif // wxUSE_PALETTE

wxMask::wxMask( const wxBitmap& bitmap )
{
    m_bitmap = (GdkBitmap *) NULL;
    Create( bitmap );
}

wxMask::~wxMask()
{
    if (m_bitmap)
        g_object_unref (m_bitmap);
}

bool wxMask::Create( const wxBitmap& bitmap,
                     const wxColour& colour )
{
    if (m_bitmap)
    {
        g_object_unref (m_bitmap);
        m_bitmap = (GdkBitmap*) NULL;
    }

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
}

#if wxUSE_PALETTE
bool wxMask::Create( const wxBitmap& bitmap, int paletteIndex )
{
    unsigned char r,g,b;
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, false, wxT("Cannot create mask from bitmap without palette") );

    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}
#endif // wxUSE_PALETTE

bool wxMask::Create( const wxBitmap& bitmap )
{
    if (m_bitmap)
    {
        g_object_unref (m_bitmap);
        m_bitmap = (GdkBitmap*) NULL;
    }

    if (!bitmap.Ok()) return false;

    wxCHECK_MSG( bitmap.GetDepth() == 1, false, wxT("Cannot create mask from colour bitmap") );

    m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, bitmap.GetWidth(), bitmap.GetHeight(), 1 );

    if (!m_bitmap) return false;

    GdkGC *gc = gdk_gc_new( m_bitmap );
    gdk_gc_set_function(gc, GDK_COPY_INVERT);
    gdk_draw_drawable(m_bitmap, gc, bitmap.GetPixmap(), 0, 0, 0, 0, bitmap.GetWidth(), bitmap.GetHeight());
    g_object_unref (gc);

    return true;
}

GdkBitmap *wxMask::GetBitmap() const
{
    return m_bitmap;
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
public:
    wxBitmapRefData();
    virtual ~wxBitmapRefData();

    GdkPixmap      *m_pixmap;
    GdkPixbuf      *m_pixbuf;
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
    wxPalette      *m_palette;
};

wxBitmapRefData::wxBitmapRefData()
{
    m_pixmap = (GdkPixmap *) NULL;
    m_pixbuf = (GdkPixbuf *) NULL;
    m_mask = (wxMask *) NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
    m_palette = (wxPalette *) NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap)
        g_object_unref (m_pixmap);
    if (m_pixbuf)
        g_object_unref (m_pixbuf);
    delete m_mask;
#if wxUSE_PALETTE
    delete m_palette;
#endif // wxUSE_PALETTE
}

//-----------------------------------------------------------------------------

#define M_BMPDATA wx_static_cast(wxBitmapRefData*, m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap()
{
}

wxBitmap::wxBitmap( int width, int height, int depth )
{
    Create( width, height, depth );
}

bool wxBitmap::Create( int width, int height, int depth )
{
    UnRef();

    if ( width <= 0 || height <= 0 )
    {
        return false;
    }

    if (depth == 32)
    {
        SetPixbuf(gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width, height));
        M_BMPDATA->m_bpp = 32;
    }
    else
    {
        if (depth != 1)
        {
            const GdkVisual* visual = wxTheApp->GetGdkVisual();
            if (depth == -1)
                depth = visual->depth;

            wxCHECK_MSG(depth == visual->depth, false, wxT("invalid bitmap depth"));
        }

        SetPixmap(gdk_pixmap_new(wxGetRootWindow()->window, width, height, depth));
    }

    return Ok();
}

bool wxBitmap::CreateFromXpm( const char **bits )
{
    UnRef();

    wxCHECK_MSG( bits != NULL, false, wxT("invalid bitmap data") );

    GdkBitmap *mask = (GdkBitmap*) NULL;
    SetPixmap(gdk_pixmap_create_from_xpm_d(wxGetRootWindow()->window, &mask, NULL, (gchar**)bits));

    wxCHECK_MSG( M_BMPDATA->m_pixmap, false, wxT("couldn't create pixmap") );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask;
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    return true;
}

wxBitmap wxBitmap::Rescale( int clipx, int clipy, int clipwidth, int clipheight, int newx, int newy )
{
    wxBitmap bmp;

    wxCHECK_MSG(Ok(), bmp, wxT("invalid bitmap"));

    if (newy==M_BMPDATA->m_width && newy==M_BMPDATA->m_height)
        return *this;

    int width = wxMax(newx, 1);
    int height = wxMax(newy, 1);
    width = wxMin(width, clipwidth);
    height = wxMin(height, clipheight);

    if (HasPixbuf())
    {
        bmp.SetDepth(GetDepth());
        bmp.SetPixbuf(gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                     true, //gdk_pixbuf_get_has_alpha(GetPixbuf()),
                                     8, width, height));
        gdk_pixbuf_scale(GetPixbuf(), bmp.GetPixbuf(),
                         0, 0, width, height,
                         clipx, clipy,
                         (double)newx/GetWidth(), (double)newy/GetHeight(),
                         GDK_INTERP_BILINEAR);
    }
    else
    {
        GdkImage* img = gdk_drawable_get_image(GetPixmap(), 0, 0, GetWidth(), GetHeight());

        wxCHECK_MSG(img, bmp, wxT("couldn't create image"));

        GdkGC *gc = NULL;
        GdkPixmap *dstpix = NULL;
        char *dst = NULL;
        long dstbyteperline = 0;

        if (GetDepth() != 1)
        {
            GdkVisual *visual = gdk_drawable_get_visual( GetPixmap() );
            if (visual == NULL)
                visual = wxTheApp->GetGdkVisual();

            bmp = wxBitmap(width, height, visual->depth);
            dstpix = bmp.GetPixmap();
            gc = gdk_gc_new( dstpix );
        }
        else
        {
            dstbyteperline = (width + 7) / 8;
            dst = (char*) malloc(dstbyteperline*height);
        }

        // be careful to use the right scaling factor
        float scx = (float)M_BMPDATA->m_width/(float)newx;
        float scy = (float)M_BMPDATA->m_height/(float)newy;
        // prepare accel-tables
        int *tablex = (int *)calloc(width,sizeof(int));
        int *tabley = (int *)calloc(height,sizeof(int));

        // accel table filled with clipped values
        for (int x = 0; x < width; x++)
            tablex[x] = (int) (scx * (x+clipx));
        for (int y = 0; y < height; y++)
            tabley[y] = (int) (scy * (y+clipy));

        // Main rescaling routine starts here
        for (int h = 0; h < height; h++)
        {
            char outbyte = 0;
            int old_x = -1;
            guint32 old_pixval = 0;

            for (int w = 0; w < width; w++)
            {
                guint32 pixval;
                int x = tablex[w];
                if (x == old_x)
                    pixval = old_pixval;
                else
                {
                    pixval = gdk_image_get_pixel( img, x, tabley[h] );
                    old_pixval = pixval;
                    old_x = x;
                }

                if ( dst )
                {
                    if (!pixval)
                    {
                        char bit=1;
                        char shift = bit << (w % 8);
                        outbyte |= shift;
                    }

                    if ((w+1)%8==0)
                    {
                        dst[h*dstbyteperline+w/8] = outbyte;
                        outbyte = 0;
                    }
                }
                else
                {
                    GdkColor col;
                    col.pixel = pixval;
                    gdk_gc_set_foreground( gc, &col );
                    gdk_draw_point( dstpix, gc, w, h);
                }
            }

            // do not forget the last byte
            if ( dst && (width % 8 != 0) )
                dst[h*dstbyteperline+width/8] = outbyte;
        }

        g_object_unref (img);
        if (gc) g_object_unref (gc);

        if ( dst )
        {
            bmp = wxBitmap( (const char *)dst, width, height, 1 );
            free( dst );
        }

        if (GetMask())
        {
            dstbyteperline = (width + 7) / 8;
            dst = (char*) malloc(dstbyteperline*height);
            img = gdk_drawable_get_image(GetMask()->GetBitmap(), 0, 0, GetWidth(), GetHeight());

            for (int h = 0; h < height; h++)
            {
                char outbyte = 0;
                int old_x = -1;
                guint32 old_pixval = 0;

                for (int w = 0; w < width; w++)
                {
                    guint32 pixval;
                    int x = tablex[w];
                    if (x == old_x)
                        pixval = old_pixval;
                    else
                    {
                        pixval = gdk_image_get_pixel( img, x, tabley[h] );
                        old_pixval = pixval;
                        old_x = x;
                    }

                    if (pixval)
                    {
                        char bit=1;
                        char shift = bit << (w % 8);
                        outbyte |= shift;
                    }

                    if ((w+1)%8 == 0)
                    {
                        dst[h*dstbyteperline+w/8] = outbyte;
                        outbyte = 0;
                    }
                }

                // do not forget the last byte
                if (width % 8 != 0)
                    dst[h*dstbyteperline+width/8] = outbyte;
            }
            wxMask* mask = new wxMask;
            mask->m_bitmap = gdk_bitmap_create_from_data( wxGetRootWindow()->window, (gchar *) dst, width, height );
            bmp.SetMask(mask);

            free( dst );
            g_object_unref (img);
        }

        free( tablex );
        free( tabley );
    }

    return bmp;
}

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG( image.Ok(), false, wxT("invalid image") );
    wxCHECK_MSG( depth == -1 || depth == 1, false, wxT("invalid bitmap depth") );

    if (image.GetWidth() <= 0 || image.GetHeight() <= 0)
        return false;

    if (depth == 1)
        return CreateFromImageAsPixmap(image, depth);

    if (image.HasAlpha())
        return CreateFromImageAsPixbuf(image);

    return CreateFromImageAsPixmap(image, depth);
}

bool wxBitmap::CreateFromImageAsPixmap(const wxImage& image, int depth)
{
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    if (depth == 1)
    {
        // create XBM format bitmap

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
        SetPixmap(gdk_bitmap_create_from_data(wxGetRootWindow()->window, (char*)out, w, h));
        delete[] out;
    }
    else
    {
        SetPixmap(gdk_pixmap_new(wxGetRootWindow()->window, w, h, depth));
        GdkGC* gc = gdk_gc_new(M_BMPDATA->m_pixmap);
        gdk_draw_rgb_image(
            M_BMPDATA->m_pixmap, gc,
            0, 0, w, h,
            GDK_RGB_DITHER_NONE, image.GetData(), w * 3);
        g_object_unref(gc);
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
        mask->m_bitmap = gdk_bitmap_create_from_data(M_BMPDATA->m_pixmap, (char*)out, w, h);
        SetMask(mask);
        delete[] out;
    }
    return true;
}

bool wxBitmap::CreateFromImageAsPixbuf(const wxImage& image)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                       true, //image.HasAlpha(),
                                       8 /* bits per sample */,
                                       width, height);
    if (!pixbuf)
        return false;

    wxASSERT( image.HasAlpha() ); // for now
    wxASSERT( gdk_pixbuf_get_n_channels(pixbuf) == 4 );
    wxASSERT( gdk_pixbuf_get_width(pixbuf) == width );
    wxASSERT( gdk_pixbuf_get_height(pixbuf) == height );

    SetDepth(32);
    SetPixbuf(pixbuf);

    // Copy the data:
    unsigned char *in = image.GetData();
    unsigned char *out = gdk_pixbuf_get_pixels(pixbuf);
    unsigned char *alpha = image.GetAlpha();

    int rowinc = gdk_pixbuf_get_rowstride(pixbuf) - 4 * width;

    for (int y = 0; y < height; y++, out += rowinc)
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
    wxImage image;

    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    const int w = GetWidth();
    const int h = GetHeight();
    image.Create(w, h);
    unsigned char *data = image.GetData();

    wxCHECK_MSG(data != NULL, wxNullImage, wxT("couldn't create image") );

    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = GetPixbuf();
        wxASSERT( gdk_pixbuf_get_has_alpha(pixbuf) );

        image.SetAlpha();

        unsigned char *alpha = image.GetAlpha();
        unsigned char *in = gdk_pixbuf_get_pixels(pixbuf);
        unsigned char *out = data;
        int rowinc = gdk_pixbuf_get_rowstride(pixbuf) - 4 * w;

        for (int y = 0; y < h; y++, in += rowinc)
        {
            for (int x = 0; x < w; x++, in += 4, out += 3, alpha++)
            {
                out[0] = in[0];
                out[1] = in[1];
                out[2] = in[2];
                *alpha = in[3];
            }
        }
    }
    else
    {
        GdkPixmap* pixmap = GetPixmap();
        GdkPixmap* pixmap_invert = NULL;
        if (GetDepth() == 1)
        {
            // mono bitmaps are inverted, i.e. 0 is white
            pixmap_invert = gdk_pixmap_new(pixmap, w, h, 1);
            GdkGC* gc = gdk_gc_new(pixmap_invert);
            gdk_gc_set_function(gc, GDK_COPY_INVERT);
            gdk_draw_drawable(pixmap_invert, gc, pixmap, 0, 0, 0, 0, w, h);
            g_object_unref(gc);
            pixmap = pixmap_invert;
        }
        // create a pixbuf which shares data with the wxImage
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(
            data, GDK_COLORSPACE_RGB, false, 8, w, h, 3 * w, NULL, NULL);

        gdk_pixbuf_get_from_drawable(pixbuf, pixmap, NULL, 0, 0, 0, 0, w, h);

        g_object_unref(pixbuf);
        if (pixmap_invert != NULL)
            g_object_unref(pixmap_invert);

        if (GetMask())
        {
            // the colour used as transparent one in wxImage and the one it is
            // replaced with when it really occurs in the bitmap
            const int MASK_RED = 1;
            const int MASK_GREEN = 2;
            const int MASK_BLUE = 3;
            const int MASK_BLUE_REPLACEMENT = 2;

            image.SetMaskColour(MASK_RED, MASK_GREEN, MASK_BLUE);
            GdkImage* image_mask = gdk_drawable_get_image(GetMask()->GetBitmap(), 0, 0, w, h);

            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++, data += 3)
                {
                    if (gdk_image_get_pixel(image_mask, x, y) == 0)
                    {
                        data[0] = MASK_RED;
                        data[1] = MASK_GREEN;
                        data[2] = MASK_BLUE;
                    }
                    else if (data[0] == MASK_RED && data[1] == MASK_GREEN && data[2] == MASK_BLUE)
                    {
                        data[2] = MASK_BLUE_REPLACEMENT;
                    }
                }
            }
            g_object_unref(image_mask);
        }
    }

    return image;
}

wxBitmap::wxBitmap( const wxString &filename, wxBitmapType type )
{
    LoadFile( filename, type );
}

wxBitmap::wxBitmap( const char bits[], int width, int height, int WXUNUSED(depth))
{
    if ( width > 0 && height > 0 )
    {
        SetPixmap(gdk_bitmap_create_from_data(wxGetRootWindow()->window, bits, width, height));

        wxASSERT_MSG( M_BMPDATA->m_pixmap, wxT("couldn't create bitmap") );
    }
}

wxBitmap::~wxBitmap()
{
}

bool wxBitmap::operator == ( const wxBitmap& bmp ) const
{
    return m_refData == bmp.m_refData;
}

bool wxBitmap::operator != ( const wxBitmap& bmp ) const
{
    return m_refData != bmp.m_refData;
}

bool wxBitmap::Ok() const
{
    return (m_refData != NULL) &&
           (
              M_BMPDATA->m_pixbuf ||
              M_BMPDATA->m_pixmap
           );
}

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( Ok(), (wxMask *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );

    if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;

    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    *this = icon;
    return Ok();
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxBitmap ret;

    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 ret, wxT("invalid bitmap or bitmap region") );

    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           true, //gdk_pixbuf_get_has_alpha(GetPixbuf()),
                                           8, rect.width, rect.height);
        ret.SetPixbuf(pixbuf);
        ret.SetDepth(M_BMPDATA->m_bpp);
        gdk_pixbuf_copy_area(GetPixbuf(),
                             rect.x, rect.y, rect.width, rect.height,
                             pixbuf, 0, 0);
    }
    else
    {
        ret = wxBitmap(rect.width, rect.height, M_BMPDATA->m_bpp);
        GdkGC *gc = gdk_gc_new( ret.GetPixmap() );
        gdk_draw_drawable( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
        g_object_unref (gc);
    }

    if (GetMask())
    {
        wxMask *mask = new wxMask;
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, rect.width, rect.height, 1 );

        GdkGC *gc = gdk_gc_new( mask->m_bitmap );
        gdk_draw_drawable(mask->m_bitmap, gc, M_BMPDATA->m_mask->m_bitmap, rect.x, rect.y, 0, 0, rect.width, rect.height);
        g_object_unref (gc);

        ret.SetMask( mask );
    }

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, wxBitmapType type, const wxPalette *WXUNUSED(palette) ) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    // Try to save the bitmap via wxImage handlers:
    wxImage image = ConvertToImage();
    return image.Ok() && image.SaveFile(name, type);
}

bool wxBitmap::LoadFile( const wxString &name, wxBitmapType type )
{
    UnRef();

    if (type == wxBITMAP_TYPE_XPM)
    {
        GdkBitmap *mask = (GdkBitmap*) NULL;
        SetPixmap(gdk_pixmap_create_from_xpm(wxGetRootWindow()->window, &mask, NULL, name.fn_str()));

        if (mask)
        {
            M_BMPDATA->m_mask = new wxMask;
            M_BMPDATA->m_mask->m_bitmap = mask;
        }
    }
    else // try if wxImage can load it
    {
        wxImage image;
        if (image.LoadFile(name, type) && image.Ok())
            *this = wxBitmap(image);
    }

    return Ok();
}

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    if (!Ok())
        return (wxPalette *) NULL;

    return M_BMPDATA->m_palette;
}

void wxBitmap::SetPalette(const wxPalette& WXUNUSED(palette))
{
    // TODO
}
#endif // wxUSE_PALETTE

void wxBitmap::SetHeight( int height )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData;

    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData;

    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData;

    M_BMPDATA->m_bpp = depth;
}

void wxBitmap::SetPixmap( GdkPixmap *pixmap )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData;

    wxASSERT(M_BMPDATA->m_pixmap == NULL);
    M_BMPDATA->m_pixmap = pixmap;
    gdk_drawable_get_size(pixmap, &M_BMPDATA->m_width, &M_BMPDATA->m_height);
    M_BMPDATA->m_bpp = gdk_drawable_get_depth(pixmap);
    PurgeOtherRepresentations(Pixmap);
}

GdkPixmap *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( Ok(), (GdkPixmap *) NULL, wxT("invalid bitmap") );

    // create the pixmap on the fly if we use Pixbuf representation:
    if (M_BMPDATA->m_pixmap == NULL)
    {
        delete M_BMPDATA->m_mask;
        M_BMPDATA->m_mask = new wxMask;
        gdk_pixbuf_render_pixmap_and_mask(M_BMPDATA->m_pixbuf,
                                          &M_BMPDATA->m_pixmap,
                                          &M_BMPDATA->m_mask->m_bitmap,
                                          128 /*threshold*/);
    }

    return M_BMPDATA->m_pixmap;
}

bool wxBitmap::HasPixmap() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixmap != NULL;
}

GdkPixbuf *wxBitmap::GetPixbuf() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    if (M_BMPDATA->m_pixbuf == NULL)
    {
        int width = GetWidth();
        int height = GetHeight();

        // always create the alpha channel so raw bitmap access will work
        // correctly
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           true, // GetMask() != NULL,
                                           8, width, height);
        M_BMPDATA->m_pixbuf =
            gdk_pixbuf_get_from_drawable(pixbuf, M_BMPDATA->m_pixmap, NULL,
                                         0, 0, 0, 0, width, height);

        // apply the mask to created pixbuf:
        if (M_BMPDATA->m_pixbuf && M_BMPDATA->m_mask)
        {
            GdkPixbuf *pmask =
                gdk_pixbuf_get_from_drawable(NULL,
                                             M_BMPDATA->m_mask->GetBitmap(),
                                             NULL,
                                             0, 0, 0, 0, width, height);
            if (pmask)
            {
                guchar *bmp = gdk_pixbuf_get_pixels(pixbuf);
                guchar *mask = gdk_pixbuf_get_pixels(pmask);
                int bmprowinc = gdk_pixbuf_get_rowstride(pixbuf) - 4 * width;
                int maskrowinc = gdk_pixbuf_get_rowstride(pmask) - 3 * width;

                for (int y = 0; y < height;
                     y++, bmp += bmprowinc, mask += maskrowinc)
                {
                    for (int x = 0; x < width; x++, bmp += 4, mask += 3)
                    {
                        if (mask[0] == 0 /*black pixel*/)
                            bmp[3] = 0;
                    }
                }

                g_object_unref (pmask);
            }
        }
    }

    return M_BMPDATA->m_pixbuf;
}

bool wxBitmap::HasPixbuf() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixbuf != NULL;
}

void wxBitmap::SetPixbuf( GdkPixbuf *pixbuf )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData;

    wxASSERT(M_BMPDATA->m_pixbuf == NULL);
    M_BMPDATA->m_pixbuf = pixbuf;
    M_BMPDATA->m_width = gdk_pixbuf_get_width(pixbuf);
    M_BMPDATA->m_height = gdk_pixbuf_get_height(pixbuf);
    PurgeOtherRepresentations(Pixbuf);
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

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if (bpp != 32)
        return NULL;

    GdkPixbuf *pixbuf = GetPixbuf();
    if (!pixbuf)
        return NULL;

    if (!gdk_pixbuf_get_has_alpha( pixbuf ))
        return NULL;

#if 0
    if (gdk_pixbuf_get_has_alpha( pixbuf ))
        wxPrintf( wxT("Has alpha, %d channels\n"), gdk_pixbuf_get_n_channels(pixbuf) );
    else
        wxPrintf( wxT("No alpha, %d channels.\n"), gdk_pixbuf_get_n_channels(pixbuf) );
#endif

    data.m_height = gdk_pixbuf_get_height( pixbuf );
    data.m_width = gdk_pixbuf_get_width( pixbuf );
    data.m_stride = gdk_pixbuf_get_rowstride( pixbuf );

    return gdk_pixbuf_get_pixels( pixbuf );
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(data))
{
}


bool wxBitmap::HasAlpha() const
{
    return HasPixbuf();
}

void wxBitmap::UseAlpha()
{
    GetPixbuf();
}

//-----------------------------------------------------------------------------
// wxBitmapHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler,wxBitmapHandlerBase)

wxBitmapHandler::~wxBitmapHandler()
{
}

bool wxBitmapHandler::Create(wxBitmap * WXUNUSED(bitmap),
                             void * WXUNUSED(data),
                             long WXUNUSED(type),
                             int WXUNUSED(width),
                             int WXUNUSED(height),
                             int WXUNUSED(depth))
{
    wxFAIL_MSG( _T("not implemented") );

    return false;
}

bool wxBitmapHandler::LoadFile(wxBitmap * WXUNUSED(bitmap),
                               const wxString& WXUNUSED(name),
                               long WXUNUSED(flags),
                               int WXUNUSED(desiredWidth),
                               int WXUNUSED(desiredHeight))
{
    wxFAIL_MSG( _T("not implemented") );

    return false;
}

bool wxBitmapHandler::SaveFile(const wxBitmap * WXUNUSED(bitmap),
                               const wxString& WXUNUSED(name),
                               int WXUNUSED(type),
                               const wxPalette * WXUNUSED(palette))
{
    wxFAIL_MSG( _T("not implemented") );

    return false;
}

/* static */ void wxBitmap::InitStandardHandlers()
{
    // TODO: Insert handler based on GdkPixbufs handler later
}
