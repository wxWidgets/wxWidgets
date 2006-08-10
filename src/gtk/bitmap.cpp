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
    #include "wx/dcmemory.h"
    #include "wx/palette.h"
    #include "wx/icon.h"
    #include "wx/math.h"
    #include "wx/image.h"
#endif

#include "wx/filefn.h"

#include "wx/rawbmp.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <gdk/gdkimage.h>

extern void gdk_wx_draw_bitmap     (GdkDrawable  *drawable,
                          GdkGC               *gc,
                          GdkDrawable  *src,
                          gint                xsrc,
                          gint                ysrc,
                          gint                xdest,
                          gint                ydest,
                          gint                width,
                          gint                height);

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

    wxImage image = bitmap.ConvertToImage();
    if (!image.Ok()) return false;

    m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, image.GetWidth(), image.GetHeight(), 1 );
    GdkGC *gc = gdk_gc_new( m_bitmap );

    GdkColor color;
    color.red = 65000;
    color.green = 65000;
    color.blue = 65000;
    color.pixel = 1;
    gdk_gc_set_foreground( gc, &color );
    gdk_gc_set_fill( gc, GDK_SOLID );
    gdk_draw_rectangle( m_bitmap, gc, TRUE, 0, 0, image.GetWidth(), image.GetHeight() );

    unsigned char *data = image.GetData();
    int index = 0;

    unsigned char red = colour.Red();
    unsigned char green = colour.Green();
    unsigned char blue = colour.Blue();

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    int bpp = visual->depth;
    if ((bpp == 16) && (visual->red_mask != 0xf800))
        bpp = 15;
    if (bpp == 15)
    {
        red = red & 0xf8;
        green = green & 0xf8;
        blue = blue & 0xf8;
    }
    else if (bpp == 16)
    {
        red = red & 0xf8;
        green = green & 0xfc;
        blue = blue & 0xf8;
    }
    else if (bpp == 12)
    {
        red = red & 0xf0;
        green = green & 0xf0;
        blue = blue & 0xf0;
    }

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.pixel = 0;
    gdk_gc_set_foreground( gc, &color );

    for (int j = 0; j < image.GetHeight(); j++)
    {
        int start_x = -1;
        int i;
        for (i = 0; i < image.GetWidth(); i++)
        {
            if ((data[index] == red) &&
                (data[index+1] == green) &&
                (data[index+2] == blue))
            {
                if (start_x == -1)
                start_x = i;
            }
            else
            {
                if (start_x != -1)
                {
                    gdk_draw_line( m_bitmap, gc, start_x, j, i-1, j );
                    start_x = -1;
                }
            }
            index += 3;
        }
        if (start_x != -1)
            gdk_draw_line( m_bitmap, gc, start_x, j, i, j );
    }

    g_object_unref (gc);

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

    gdk_wx_draw_bitmap( m_bitmap, gc, bitmap.GetPixmap(), 0, 0, 0, 0, bitmap.GetWidth(), bitmap.GetHeight() );

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
    ~wxBitmapRefData();

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

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    if (depth == -1)
        depth = visual->depth;

    wxCHECK_MSG( (depth == visual->depth) || (depth == 1) || (depth == 32), false,
                    wxT("invalid bitmap depth") );

    m_refData = new wxBitmapRefData();
    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    M_BMPDATA->m_bpp = depth;
    if (depth == 32)
    {
        M_BMPDATA->m_pixbuf = gdk_pixbuf_new( GDK_COLORSPACE_RGB, true,
                                              8, width, height);
    }
    else
    {
        M_BMPDATA->m_pixmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, depth );
    }

    return Ok();
}

bool wxBitmap::CreateFromXpm( const char **bits )
{
    UnRef();

    wxCHECK_MSG( bits != NULL, false, wxT("invalid bitmap data") );

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    m_refData = new wxBitmapRefData();

    GdkBitmap *mask = (GdkBitmap*) NULL;

    M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm_d( wxGetRootWindow()->window, &mask, NULL, (gchar **) bits );

    wxCHECK_MSG( M_BMPDATA->m_pixmap, false, wxT("couldn't create pixmap") );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask();
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    gdk_drawable_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

    M_BMPDATA->m_bpp = visual->depth;  // Can we get a different depth from create_from_xpm_d() ?

    return true;
}

wxBitmap wxBitmap::Rescale( int clipx, int clipy, int clipwidth, int clipheight, int newx, int newy )
{
    wxCHECK_MSG( Ok(), wxNullBitmap, wxT("invalid bitmap") );

    if (newy==M_BMPDATA->m_width && newy==M_BMPDATA->m_height)
        return *this;

    int width = wxMax(newx, 1);
    int height = wxMax(newy, 1);
    width = wxMin(width, clipwidth);
    height = wxMin(height, clipheight);

    wxBitmap bmp;

    if (HasPixbuf())
    {
        bmp.SetWidth(width);
        bmp.SetHeight(height);
        bmp.SetDepth(GetDepth());
        bmp.SetPixbuf(gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                     gdk_pixbuf_get_has_alpha(GetPixbuf()),
                                     8, width, height));
        gdk_pixbuf_scale(GetPixbuf(), bmp.GetPixbuf(),
                         0, 0, width, height,
                         clipx, clipy,
                         (double)newx/GetWidth(), (double)newy/GetHeight(),
                         GDK_INTERP_BILINEAR);
    }
    else
    {
        GdkImage *img = (GdkImage*) NULL;
        if (GetPixmap())
            img = gdk_image_get( GetPixmap(), 0, 0, GetWidth(), GetHeight() );
        else
            wxFAIL_MSG( wxT("Ill-formed bitmap") );

        wxCHECK_MSG( img, wxNullBitmap, wxT("couldn't create image") );

        int bpp = -1;


        GdkGC *gc = NULL;
        GdkPixmap *dstpix = NULL;
        if (GetDepth() != 1)
        {
            GdkVisual *visual = gdk_drawable_get_visual( GetPixmap() );
            if (visual == NULL)
                visual = wxTheApp->GetGdkVisual();

            bpp = visual->depth;
            bmp = wxBitmap(width,height,bpp);
            dstpix = bmp.GetPixmap();
            gc = gdk_gc_new( dstpix );
        }

        char *dst = NULL;
        long dstbyteperline = 0;

        if (GetDepth() == 1)
        {
            bpp = 1;
            dstbyteperline = width/8*M_BMPDATA->m_bpp;
            if (width*M_BMPDATA->m_bpp % 8 != 0)
                dstbyteperline++;
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
            dstbyteperline = width/8;
            if (width % 8 != 0)
                dstbyteperline++;
            dst = (char*) malloc(dstbyteperline*height);
            img = gdk_image_get( GetMask()->GetBitmap(), 0, 0, GetWidth(), GetHeight() );

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

    m_refData = new wxBitmapRefData();

    if (depth == 1)
    {
        return CreateFromImageAsBitmap(image);
    }
    else
    {
        if (image.HasAlpha())
            return CreateFromImageAsPixbuf(image);

        return CreateFromImageAsPixmap(image);
    }
}

// conversion to mono bitmap:
bool wxBitmap::CreateFromImageAsBitmap(const wxImage& img)
{
    // convert alpha channel to mask, if it is present:
    wxImage image(img);
    image.ConvertAlphaToMask();

    int width = image.GetWidth();
    int height = image.GetHeight();

    SetHeight( height );
    SetWidth( width );

    SetPixmap( gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 ) );

    SetDepth( 1 );

    // Create picture image

    GdkGC* data_gc = gdk_gc_new(M_BMPDATA->m_pixmap);
    GdkColor color;
    color.pixel = 1;
    gdk_gc_set_foreground(data_gc, &color);
    gdk_draw_rectangle(M_BMPDATA->m_pixmap, data_gc, true, 0, 0, width, height);
    GdkImage* data_image = gdk_drawable_get_image(M_BMPDATA->m_pixmap, 0, 0, width, height);

    // Create mask image

    GdkImage *mask_image = (GdkImage*) NULL;
    GdkGC* mask_gc = NULL;

    if (image.HasMask())
    {
        wxMask *mask = new wxMask();
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 );
        mask_gc = gdk_gc_new(mask->m_bitmap);
        gdk_gc_set_foreground(mask_gc, &color);
        gdk_draw_rectangle(mask->m_bitmap, mask_gc, true, 0, 0, width, height);
        mask_image = gdk_drawable_get_image(mask->m_bitmap, 0, 0, width, height);

        SetMask( mask );
    }

    int r_mask = image.GetMaskRed();
    int g_mask = image.GetMaskGreen();
    int b_mask = image.GetMaskBlue();

    unsigned char* data = image.GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r = data[index];
            index++;
            int g = data[index];
            index++;
            int b = data[index];
            index++;

            if (mask_image != NULL)
            {
                if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                    gdk_image_put_pixel( mask_image, x, y, 0 );
            }

            if ((r == 255) && (b == 255) && (g == 255))
                gdk_image_put_pixel( data_image, x, y, 0 );

        } // for
    }  // for

    // Blit picture

    gdk_draw_image( GetPixmap(), data_gc, data_image, 0, 0, 0, 0, width, height );

    g_object_unref (data_image);
    g_object_unref (data_gc);

    // Blit mask

    if (mask_image != NULL)
    {
        gdk_draw_image( GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

        g_object_unref (mask_image);
        g_object_unref (mask_gc);
    }

    return true;
}

// conversion to colour bitmap:
bool wxBitmap::CreateFromImageAsPixmap(const wxImage& img)
{
    // convert alpha channel to mask, if it is present:
    wxImage image(img);
    image.ConvertAlphaToMask();

    int width = image.GetWidth();
    int height = image.GetHeight();

    SetHeight( height );
    SetWidth( width );

    SetPixmap( gdk_pixmap_new( wxGetRootWindow()->window, width, height, -1 ) );

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    int bpp = visual->depth;

    SetDepth( bpp );

    GdkGC *gc = gdk_gc_new( GetPixmap() );

    gdk_draw_rgb_image( GetPixmap(),
                        gc,
                        0, 0,
                        width, height,
                        GDK_RGB_DITHER_NONE,
                        image.GetData(),
                        width*3 );

    g_object_unref (gc);

    // Create mask image

    if (!image.HasMask())
        return true;

    wxMask *mask = new wxMask();
    mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 );
    GdkGC* mask_gc = gdk_gc_new(mask->m_bitmap);
    GdkColor color;
    color.pixel = 1;
    gdk_gc_set_foreground(mask_gc, &color);
    gdk_draw_rectangle(mask->m_bitmap, mask_gc, true, 0, 0, width, height);
    GdkImage* mask_image = gdk_drawable_get_image(mask->m_bitmap, 0, 0, width, height);

    SetMask( mask );

    int r_mask = image.GetMaskRed();
    int g_mask = image.GetMaskGreen();
    int b_mask = image.GetMaskBlue();

    unsigned char* data = image.GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int r = data[index];
            index++;
            int g = data[index];
            index++;
            int b = data[index];
            index++;

            if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                gdk_image_put_pixel( mask_image, x, y, 0 );
        } // for
    }  // for

    // Blit mask

    gdk_draw_image( GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

    g_object_unref (mask_image);
    g_object_unref (mask_gc);

    return true;
}

bool wxBitmap::CreateFromImageAsPixbuf(const wxImage& image)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                       image.HasAlpha(),
                                       8 /* bits per sample */,
                                       width, height);
    if (!pixbuf)
        return false;

    wxASSERT( image.HasAlpha() ); // for now
    wxASSERT( gdk_pixbuf_get_n_channels(pixbuf) == 4 );
    wxASSERT( gdk_pixbuf_get_width(pixbuf) == width );
    wxASSERT( gdk_pixbuf_get_height(pixbuf) == height );

    M_BMPDATA->m_pixbuf = pixbuf;
    SetHeight(height);
    SetWidth(width);
    SetDepth(wxTheApp->GetGdkVisual()->depth);

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

    image.Create(GetWidth(), GetHeight());
    unsigned char *data = image.GetData();

    if (!data)
    {
        wxFAIL_MSG( wxT("couldn't create image") );
        return wxNullImage;
    }

    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = GetPixbuf();
        wxASSERT( gdk_pixbuf_get_has_alpha(pixbuf) );

        int w = GetWidth();
        int h = GetHeight();

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
        // the colour used as transparent one in wxImage and the one it is
        // replaced with when it really occurs in the bitmap
        static const int MASK_RED = 1;
        static const int MASK_GREEN = 2;
        static const int MASK_BLUE = 3;
        static const int MASK_BLUE_REPLACEMENT = 2;

        GdkImage *gdk_image = (GdkImage*) NULL;

        if (HasPixmap())
        {
            gdk_image = gdk_image_get( GetPixmap(),
                                       0, 0,
                                       GetWidth(), GetHeight() );
        }
        else
        {
            wxFAIL_MSG( wxT("Ill-formed bitmap") );
        }

        wxCHECK_MSG( gdk_image, wxNullImage, wxT("couldn't create image") );

        GdkImage *gdk_image_mask = (GdkImage*) NULL;
        if (GetMask())
        {
            gdk_image_mask = gdk_image_get( GetMask()->GetBitmap(),
                                            0, 0,
                                            GetWidth(), GetHeight() );

            image.SetMaskColour( MASK_RED, MASK_GREEN, MASK_BLUE );
        }

        int bpp = -1;
        int red_shift_right = 0;
        int green_shift_right = 0;
        int blue_shift_right = 0;
        int red_shift_left = 0;
        int green_shift_left = 0;
        int blue_shift_left = 0;
        bool use_shift = false;

        if (GetDepth() != 1)
        {
            GdkVisual *visual = gdk_drawable_get_visual( GetPixmap() );
            if (visual == NULL)
                visual = wxTheApp->GetGdkVisual();

            bpp = visual->depth;
            if (bpp == 16)
                bpp = visual->red_prec + visual->green_prec + visual->blue_prec;
            red_shift_right = visual->red_shift;
            red_shift_left = 8-visual->red_prec;
            green_shift_right = visual->green_shift;
            green_shift_left = 8-visual->green_prec;
            blue_shift_right = visual->blue_shift;
            blue_shift_left = 8-visual->blue_prec;

            use_shift = (visual->type == GDK_VISUAL_TRUE_COLOR) || (visual->type == GDK_VISUAL_DIRECT_COLOR);
        }
        else
        {
            bpp = 1;
        }


        GdkColormap *cmap = gtk_widget_get_default_colormap();

        long pos = 0;
        for (int j = 0; j < GetHeight(); j++)
        {
            for (int i = 0; i < GetWidth(); i++)
            {
                wxUint32 pixel = gdk_image_get_pixel( gdk_image, i, j );
                if (bpp == 1)
                {
                    if (pixel == 0)
                    {
                        data[pos]   = 0;
                        data[pos+1] = 0;
                        data[pos+2] = 0;
                    }
                    else
                    {
                        data[pos]   = 255;
                        data[pos+1] = 255;
                        data[pos+2] = 255;
                    }
                }
                else if (use_shift)
                {
                    data[pos] =   (pixel >> red_shift_right)   << red_shift_left;
                    data[pos+1] = (pixel >> green_shift_right) << green_shift_left;
                    data[pos+2] = (pixel >> blue_shift_right)  << blue_shift_left;
                }
                else if (cmap->colors)
                {
                    data[pos] =   cmap->colors[pixel].red   >> 8;
                    data[pos+1] = cmap->colors[pixel].green >> 8;
                    data[pos+2] = cmap->colors[pixel].blue  >> 8;
                }
                else
                {
                    wxFAIL_MSG( wxT("Image conversion failed. Unknown visual type.") );
                }

                if (gdk_image_mask)
                {
                    int mask_pixel = gdk_image_get_pixel( gdk_image_mask, i, j );
                    if (mask_pixel == 0)
                    {
                        data[pos] = MASK_RED;
                        data[pos+1] = MASK_GREEN;
                        data[pos+2] = MASK_BLUE;
                    }
                    else if ( data[pos] == MASK_RED &&
                                data[pos+1] == MASK_GREEN &&
                                    data[pos+2] == MASK_BLUE )
                    {
                        data[pos+2] = MASK_BLUE_REPLACEMENT;
                    }
                }

                pos += 3;
            }
        }

        g_object_unref (gdk_image);
        if (gdk_image_mask) g_object_unref (gdk_image_mask);
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
        m_refData = new wxBitmapRefData();

        M_BMPDATA->m_mask = (wxMask *) NULL;
        M_BMPDATA->m_pixmap = gdk_bitmap_create_from_data
                              (
                                wxGetRootWindow()->window,
                                (gchar *) bits,
                                width,
                                height
                              );
        M_BMPDATA->m_width = width;
        M_BMPDATA->m_height = height;
        M_BMPDATA->m_bpp = 1;

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
    return true;
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, M_BMPDATA->m_bpp );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           gdk_pixbuf_get_has_alpha(GetPixbuf()),
                                           8, rect.width, rect.height);
        ret.SetPixbuf(pixbuf);
        gdk_pixbuf_copy_area(GetPixbuf(),
                             rect.x, rect.y, rect.width, rect.height,
                             pixbuf, 0, 0);
    }
    else
    {
        if (ret.GetDepth() != 1)
        {
            GdkGC *gc = gdk_gc_new( ret.GetPixmap() );
            gdk_draw_drawable( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
            g_object_unref (gc);
        }
        else
        {
            GdkGC *gc = gdk_gc_new( ret.GetPixmap() );
            GdkColor col;
            col.pixel = 0xFFFFFF;
            gdk_gc_set_foreground( gc, &col );
            col.pixel = 0;
            gdk_gc_set_background( gc, &col );
            gdk_wx_draw_bitmap( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
            g_object_unref (gc);
        }
    }

    if (GetMask())
    {
        wxMask *mask = new wxMask;
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, rect.width, rect.height, 1 );

        GdkGC *gc = gdk_gc_new( mask->m_bitmap );
        GdkColor col;
        col.pixel = 0xFFFFFF;
        gdk_gc_set_foreground( gc, &col );
        col.pixel = 0;
        gdk_gc_set_background( gc, &col );
        gdk_wx_draw_bitmap( mask->m_bitmap, gc, M_BMPDATA->m_mask->m_bitmap, rect.x, rect.y, 0, 0, rect.width, rect.height );
        g_object_unref (gc);

        ret.SetMask( mask );
    }

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, wxBitmapType type, const wxPalette *WXUNUSED(palette) ) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    // Try to save the bitmap via wxImage handlers:
    {
        wxImage image = ConvertToImage();
        if (image.Ok()) return image.SaveFile( name, type );
    }

    return false;
}

bool wxBitmap::LoadFile( const wxString &name, wxBitmapType type )
{
    UnRef();

    if (!wxFileExists(name))
        return false;

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    if (type == wxBITMAP_TYPE_XPM)
    {
        m_refData = new wxBitmapRefData();

        GdkBitmap *mask = (GdkBitmap*) NULL;

        M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm
                              (
                                wxGetRootWindow()->window,
                                &mask,
                                NULL,
                                name.fn_str()
                              );

        if (mask)
        {
           M_BMPDATA->m_mask = new wxMask();
           M_BMPDATA->m_mask->m_bitmap = mask;
        }

        gdk_drawable_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

        M_BMPDATA->m_bpp = visual->depth;
    }
    else // try if wxImage can load it
    {
        wxImage image;
        if ( !image.LoadFile( name, type ) || !image.Ok() )
            return false;

        *this = wxBitmap(image);
    }

    return true;
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
        m_refData = new wxBitmapRefData();

    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData();

    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bpp = depth;
}

void wxBitmap::SetPixmap( GdkPixmap *pixmap )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData();

    M_BMPDATA->m_pixmap = pixmap;
    PurgeOtherRepresentations(Pixmap);
}

GdkPixmap *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( Ok(), (GdkPixmap *) NULL, wxT("invalid bitmap") );

    // create the pixmap on the fly if we use Pixbuf representation:
    if (HasPixbuf() && !HasPixmap())
    {
        delete M_BMPDATA->m_mask;
        M_BMPDATA->m_mask = new wxMask();
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

    if (HasPixmap() && !HasPixbuf())
    {
        int width = GetWidth();
        int height = GetHeight();

        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           GetMask() != NULL,
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
        m_refData = new wxBitmapRefData();

    M_BMPDATA->m_pixbuf = pixbuf;
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

#if 0
    if (gdk_pixbuf_get_has_alpha( pixbuf ))
        wxPrintf( wxT("Has alpha\n") );
    else
        wxPrintf( wxT("No alpha.\n") );
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
