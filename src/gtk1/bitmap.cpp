/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "bitmap.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#include "wx/bitmap.h"
#include "wx/palette.h"
#include "wx/icon.h"
#include "wx/filefn.h"
#include "wx/image.h"
#include "wx/dcmemory.h"
#include "wx/app.h"

#ifdef __WXGTK20__
    // need this to get gdk_image_new_bitmap()
    #define GDK_ENABLE_BROKEN
#endif

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#ifdef __WXGTK20__
    #include <gdk/gdkimage.h>
#else // GTK+ 1.2
    #include <gdk/gdkrgb.h>
#endif // GTK+ 2.0/1.2

#include <math.h>

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

wxMask::wxMask( const wxBitmap& bitmap, int paletteIndex )
{
    m_bitmap = (GdkBitmap *) NULL;
    Create( bitmap, paletteIndex );
}

wxMask::wxMask( const wxBitmap& bitmap )
{
    m_bitmap = (GdkBitmap *) NULL;
    Create( bitmap );
}

wxMask::~wxMask()
{
    if (m_bitmap)
        gdk_bitmap_unref( m_bitmap );
}

bool wxMask::Create( const wxBitmap& bitmap,
                     const wxColour& colour )
{
    if (m_bitmap)
    {
        gdk_bitmap_unref( m_bitmap );
        m_bitmap = (GdkBitmap*) NULL;
    }

    wxImage image = bitmap.ConvertToImage();
    if (!image.Ok()) return FALSE;

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

    gdk_gc_unref( gc );

    return TRUE;
}

bool wxMask::Create( const wxBitmap& bitmap, int paletteIndex )
{
    unsigned char r,g,b;
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, FALSE, wxT("Cannot create mask from bitmap without palette") );

    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}

bool wxMask::Create( const wxBitmap& bitmap )
{
    if (m_bitmap)
    {
        gdk_bitmap_unref( m_bitmap );
        m_bitmap = (GdkBitmap*) NULL;
    }

    if (!bitmap.Ok()) return FALSE;

    wxCHECK_MSG( bitmap.GetBitmap(), FALSE, wxT("Cannot create mask from colour bitmap") );

    m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, bitmap.GetWidth(), bitmap.GetHeight(), 1 );

    if (!m_bitmap) return FALSE;

    GdkGC *gc = gdk_gc_new( m_bitmap );

    gdk_wx_draw_bitmap( m_bitmap, gc, bitmap.GetBitmap(), 0, 0, 0, 0, bitmap.GetWidth(), bitmap.GetHeight() );

    gdk_gc_unref( gc );

    return TRUE;
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
    GdkBitmap      *m_bitmap;
#ifdef __WXGTK20__
    GdkPixbuf      *m_pixbuf;
#endif
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
    wxPalette      *m_palette;
};

wxBitmapRefData::wxBitmapRefData()
{
    m_pixmap = (GdkPixmap *) NULL;
    m_bitmap = (GdkBitmap *) NULL;
#ifdef __WXGTK20__
    m_pixbuf = (GdkPixbuf *) NULL;
#endif
    m_mask = (wxMask *) NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
    m_palette = (wxPalette *) NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap)
        gdk_pixmap_unref( m_pixmap );
    if (m_bitmap)
        gdk_bitmap_unref( m_bitmap );
#ifdef __WXGTK20__
    if (m_pixbuf)
        gdk_pixbuf_unref( m_pixbuf );
#endif
    delete m_mask;
    delete m_palette;
}

//-----------------------------------------------------------------------------

#define M_BMPDATA ((wxBitmapRefData *)m_refData)

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

    wxCHECK_MSG( (depth == visual->depth) || (depth == 1), FALSE,
                    wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();
    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    if (depth == 1)
    {
        M_BMPDATA->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 );
        M_BMPDATA->m_bpp = 1;
    }
    else
    {
        M_BMPDATA->m_pixmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, depth );
        M_BMPDATA->m_bpp = visual->depth;
    }

    return Ok();
}

bool wxBitmap::CreateFromXpm( const char **bits )
{
    UnRef();

    wxCHECK_MSG( bits != NULL, FALSE, wxT("invalid bitmap data") )

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    m_refData = new wxBitmapRefData();

    GdkBitmap *mask = (GdkBitmap*) NULL;

    M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm_d( wxGetRootWindow()->window, &mask, NULL, (gchar **) bits );

    wxCHECK_MSG( M_BMPDATA->m_pixmap, FALSE, wxT("couldn't create pixmap") );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask();
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

    M_BMPDATA->m_bpp = visual->depth;  // Can we get a different depth from create_from_xpm_d() ?

    return TRUE;
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

#ifdef __WXGTK20__
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
#endif // __WXGTK20__
    {
        GdkImage *img = (GdkImage*) NULL;
        if (GetPixmap())
            img = gdk_image_get( GetPixmap(), 0, 0, GetWidth(), GetHeight() );
        else if (GetBitmap())
            img = gdk_image_get( GetBitmap(), 0, 0, GetWidth(), GetHeight() );
        else
            wxFAIL_MSG( wxT("Ill-formed bitmap") );

        wxCHECK_MSG( img, wxNullBitmap, wxT("couldn't create image") );

        int bpp = -1;

        
        GdkGC *gc = NULL;
        GdkPixmap *dstpix = NULL;
        if (GetPixmap())
        {
            GdkVisual *visual = gdk_window_get_visual( GetPixmap() );
            if (visual == NULL)
                visual = wxTheApp->GetGdkVisual();

            bpp = visual->depth;
            bmp = wxBitmap(width,height,bpp);
            dstpix = bmp.GetPixmap();
            gc = gdk_gc_new( dstpix );
        }

        char *dst = NULL;
        long dstbyteperline = 0;
        
        if (GetBitmap())
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
                    
                if (bpp == 1)
                {
                    if (!pixval)
                    {
                        char bit=1;
                        char shift = bit << w % 8;
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
            if ((bpp == 1) && (width % 8 != 0))
                dst[h*dstbyteperline+width/8] = outbyte;
        }
        
        gdk_image_destroy( img );
        if (gc) gdk_gc_unref( gc );

        if (bpp == 1)
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
                        char shift = bit << w % 8;
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
            gdk_image_destroy( img );
        }

        free( tablex );
        free( tabley );
    }
    
    return bmp; 
}

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") )
    wxCHECK_MSG( depth == -1 || depth == 1, FALSE, wxT("invalid bitmap depth") )

    if (image.GetWidth() <= 0 || image.GetHeight() <= 0)
        return false;
    
    m_refData = new wxBitmapRefData();

    if (depth == 1)
    {
        return CreateFromImageAsBitmap(image);
    }
    else
    {
#ifdef __WXGTK20__
        if (image.HasAlpha())
            return CreateFromImageAsPixbuf(image);
#endif
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

    SetBitmap( gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 ) );

    SetDepth( 1 );

    GdkVisual *visual = wxTheApp->GetGdkVisual();

    // Create picture image

    unsigned char *data_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

    GdkImage *data_image =
        gdk_image_new_bitmap( visual, data_data, width, height );

    // Create mask image

    GdkImage *mask_image = (GdkImage*) NULL;

    if (image.HasMask())
    {
        unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

        mask_image =  gdk_image_new_bitmap( visual, mask_data, width, height );

        wxMask *mask = new wxMask();
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 );

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

            if (image.HasMask())
            {
                if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                    gdk_image_put_pixel( mask_image, x, y, 1 );
                else
                    gdk_image_put_pixel( mask_image, x, y, 0 );
            }

            if ((r == 255) && (b == 255) && (g == 255))
                gdk_image_put_pixel( data_image, x, y, 1 );
            else
                gdk_image_put_pixel( data_image, x, y, 0 );

        } // for
    }  // for

    // Blit picture

    GdkGC *data_gc = gdk_gc_new( GetBitmap() );

    gdk_draw_image( GetBitmap(), data_gc, data_image, 0, 0, 0, 0, width, height );

    gdk_image_destroy( data_image );
    gdk_gc_unref( data_gc );

    // Blit mask

    if (image.HasMask())
    {
        GdkGC *mask_gc = gdk_gc_new( GetMask()->GetBitmap() );

        gdk_draw_image( GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

        gdk_image_destroy( mask_image );
        gdk_gc_unref( mask_gc );
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

    if ((bpp == 16) && (visual->red_mask != 0xf800))
        bpp = 15;
    else if (bpp < 8)
        bpp = 8;

    // We handle 8-bit bitmaps ourselves using the colour cube, 12-bit
    // visuals are not supported by GDK so we do these ourselves, too.
    // 15-bit and 16-bit should actually work and 24-bit certainly does.
#ifdef __sgi
    if (!image.HasMask() && (bpp > 16))
#else
    if (!image.HasMask() && (bpp > 12))
#endif
    {
        static bool s_hasInitialized = FALSE;

        if (!s_hasInitialized)
        {
            gdk_rgb_init();
            s_hasInitialized = TRUE;
        }

        GdkGC *gc = gdk_gc_new( GetPixmap() );

        gdk_draw_rgb_image( GetPixmap(),
                            gc,
                            0, 0,
                            width, height,
                            GDK_RGB_DITHER_NONE,
                            image.GetData(),
                            width*3 );

        gdk_gc_unref( gc );
        return TRUE;
    }

    // Create picture image

    GdkImage *data_image =
        gdk_image_new( GDK_IMAGE_FASTEST, visual, width, height );

    // Create mask image

    GdkImage *mask_image = (GdkImage*) NULL;

    if (image.HasMask())
    {
        unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );

        mask_image =  gdk_image_new_bitmap( visual, mask_data, width, height );

        wxMask *mask = new wxMask();
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, width, height, 1 );

        SetMask( mask );
    }

    // Render

    enum byte_order { RGB, RBG, BRG, BGR, GRB, GBR };
    byte_order b_o = RGB;

    if (bpp > 8)
    {
        if ((visual->red_mask > visual->green_mask) && (visual->green_mask > visual->blue_mask))      b_o = RGB;
        else if ((visual->red_mask > visual->blue_mask) && (visual->blue_mask > visual->green_mask))  b_o = RBG;
        else if ((visual->blue_mask > visual->red_mask) && (visual->red_mask > visual->green_mask))   b_o = BRG;
        else if ((visual->blue_mask > visual->green_mask) && (visual->green_mask > visual->red_mask)) b_o = BGR;
        else if ((visual->green_mask > visual->red_mask) && (visual->red_mask > visual->blue_mask))   b_o = GRB;
        else if ((visual->green_mask > visual->blue_mask) && (visual->blue_mask > visual->red_mask))  b_o = GBR;
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

            if (image.HasMask())
            {
                if ((r == r_mask) && (b == b_mask) && (g == g_mask))
                    gdk_image_put_pixel( mask_image, x, y, 1 );
                else
                    gdk_image_put_pixel( mask_image, x, y, 0 );
            }

            switch (bpp)
            {
                case 8:
                {
                    int pixel = -1;
                    if (wxTheApp->m_colorCube)
                    {
                        pixel = wxTheApp->m_colorCube[ ((r & 0xf8) << 7) + ((g & 0xf8) << 2) + ((b & 0xf8) >> 3) ];
                    }
                    else
                    {
                        GdkColormap *cmap = gtk_widget_get_default_colormap();
                        GdkColor *colors = cmap->colors;
                        int max = 3 * (65536);

                        for (int i = 0; i < cmap->size; i++)
                        {
                            int rdiff = (r << 8) - colors[i].red;
                            int gdiff = (g << 8) - colors[i].green;
                            int bdiff = (b << 8) - colors[i].blue;
                            int sum = ABS (rdiff) + ABS (gdiff) + ABS (bdiff);
                            if (sum < max) { pixel = i; max = sum; }
                        }
                    }

                    gdk_image_put_pixel( data_image, x, y, pixel );

                    break;
                }
                case 12:  // SGI only
                {
                    guint32 pixel = 0;
                    switch (b_o)
                    {
                        case RGB: pixel = ((r & 0xf0) << 4) | (g & 0xf0) | ((b & 0xf0) >> 4); break;
                        case RBG: pixel = ((r & 0xf0) << 4) | (b & 0xf0) | ((g & 0xf0) >> 4); break;
                        case GRB: pixel = ((g & 0xf0) << 4) | (r & 0xf0) | ((b & 0xf0) >> 4); break;
                        case GBR: pixel = ((g & 0xf0) << 4) | (b & 0xf0) | ((r & 0xf0) >> 4); break;
                        case BRG: pixel = ((b & 0xf0) << 4) | (r & 0xf0) | ((g & 0xf0) >> 4); break;
                        case BGR: pixel = ((b & 0xf0) << 4) | (g & 0xf0) | ((r & 0xf0) >> 4); break;
                    }
                    gdk_image_put_pixel( data_image, x, y, pixel );
                    break;
                }
                case 15:
                {
                    guint32 pixel = 0;
                    switch (b_o)
                    {
                        case RGB: pixel = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3); break;
                        case RBG: pixel = ((r & 0xf8) << 7) | ((b & 0xf8) << 2) | ((g & 0xf8) >> 3); break;
                        case GRB: pixel = ((g & 0xf8) << 7) | ((r & 0xf8) << 2) | ((b & 0xf8) >> 3); break;
                        case GBR: pixel = ((g & 0xf8) << 7) | ((b & 0xf8) << 2) | ((r & 0xf8) >> 3); break;
                        case BRG: pixel = ((b & 0xf8) << 7) | ((r & 0xf8) << 2) | ((g & 0xf8) >> 3); break;
                        case BGR: pixel = ((b & 0xf8) << 7) | ((g & 0xf8) << 2) | ((r & 0xf8) >> 3); break;
                    }
                    gdk_image_put_pixel( data_image, x, y, pixel );
                    break;
                }
                case 16:
                {
                    // I actually don't know if for 16-bit displays, it is alway the green
                    // component or the second component which has 6 bits.
                    guint32 pixel = 0;
                    switch (b_o)
                    {
                        case RGB: pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3); break;
                        case RBG: pixel = ((r & 0xf8) << 8) | ((b & 0xfc) << 3) | ((g & 0xf8) >> 3); break;
                        case GRB: pixel = ((g & 0xf8) << 8) | ((r & 0xfc) << 3) | ((b & 0xf8) >> 3); break;
                        case GBR: pixel = ((g & 0xf8) << 8) | ((b & 0xfc) << 3) | ((r & 0xf8) >> 3); break;
                        case BRG: pixel = ((b & 0xf8) << 8) | ((r & 0xfc) << 3) | ((g & 0xf8) >> 3); break;
                        case BGR: pixel = ((b & 0xf8) << 8) | ((g & 0xfc) << 3) | ((r & 0xf8) >> 3); break;
                    }
                    gdk_image_put_pixel( data_image, x, y, pixel );
                    break;
                }
                case 32:
                case 24:
                {
                    guint32 pixel = 0;
                    switch (b_o)
                    {
                        case RGB: pixel = (r << 16) | (g << 8) | b; break;
                        case RBG: pixel = (r << 16) | (b << 8) | g; break;
                        case BRG: pixel = (b << 16) | (r << 8) | g; break;
                        case BGR: pixel = (b << 16) | (g << 8) | r; break;
                        case GRB: pixel = (g << 16) | (r << 8) | b; break;
                        case GBR: pixel = (g << 16) | (b << 8) | r; break;
                    }
                    gdk_image_put_pixel( data_image, x, y, pixel );
                }
                default: break;
            }
        } // for
    }  // for

    // Blit picture

    GdkGC *data_gc = gdk_gc_new( GetPixmap() );

    gdk_draw_image( GetPixmap(), data_gc, data_image, 0, 0, 0, 0, width, height );

    gdk_image_destroy( data_image );
    gdk_gc_unref( data_gc );

    // Blit mask

    if (image.HasMask())
    {
        GdkGC *mask_gc = gdk_gc_new( GetMask()->GetBitmap() );

        gdk_draw_image( GetMask()->GetBitmap(), mask_gc, mask_image, 0, 0, 0, 0, width, height );

        gdk_image_destroy( mask_image );
        gdk_gc_unref( mask_gc );
    }

    return true;
}

#ifdef __WXGTK20__
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
#endif // __WXGTK20__

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

#ifdef __WXGTK20__
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
#endif // __WXGTK20__
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
        else if (GetBitmap())
        {
            gdk_image = gdk_image_get( GetBitmap(),
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
        bool use_shift = FALSE;

        if (GetPixmap())
        {
            GdkVisual *visual = gdk_window_get_visual( GetPixmap() );
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
        if (GetBitmap())
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

        gdk_image_destroy( gdk_image );
        if (gdk_image_mask) gdk_image_destroy( gdk_image_mask );
    }

    return image;
}

wxBitmap::wxBitmap( const wxBitmap& bmp )
        : wxGDIObject()
{
    Ref( bmp );
}

wxBitmap::wxBitmap( const wxString &filename, int type )
{
    LoadFile( filename, type );
}

wxBitmap::wxBitmap( const char bits[], int width, int height, int WXUNUSED(depth))
{
    if ( width > 0 && height > 0 )
    {
        m_refData = new wxBitmapRefData();

        M_BMPDATA->m_mask = (wxMask *) NULL;
        M_BMPDATA->m_bitmap = gdk_bitmap_create_from_data
                              (
                                wxGetRootWindow()->window,
                                (gchar *) bits,
                                width,
                                height
                              );
        M_BMPDATA->m_width = width;
        M_BMPDATA->m_height = height;
        M_BMPDATA->m_bpp = 1;

        wxASSERT_MSG( M_BMPDATA->m_bitmap, wxT("couldn't create bitmap") );
    }
}

wxBitmap::~wxBitmap()
{
}

wxBitmap& wxBitmap::operator = ( const wxBitmap& bmp )
{
    if ( m_refData != bmp.m_refData )
        Ref( bmp );

    return *this;
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
#ifdef __WXGTK20__
              M_BMPDATA->m_pixbuf ||
#endif
              M_BMPDATA->m_bitmap || M_BMPDATA->m_pixmap 
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
    return TRUE;
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, M_BMPDATA->m_bpp );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

#ifdef __WXGTK20__
    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           gdk_pixbuf_get_has_alpha(GetPixbuf()),
                                           8, GetWidth(), GetHeight());
        ret.SetPixbuf(pixbuf);
        gdk_pixbuf_copy_area(GetPixbuf(),
                             rect.x, rect.y, rect.width, rect.height,
                             pixbuf, 0, 0);
    }
    else
#endif // __WXGTK20__
    {
        if (ret.GetPixmap())
        {
            GdkGC *gc = gdk_gc_new( ret.GetPixmap() );
            gdk_draw_pixmap( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
            gdk_gc_destroy( gc );
        }
        else
        {
            GdkGC *gc = gdk_gc_new( ret.GetBitmap() );
            GdkColor col;
            col.pixel = 0xFFFFFF;
            gdk_gc_set_foreground( gc, &col );
            col.pixel = 0;
            gdk_gc_set_background( gc, &col );
            gdk_wx_draw_bitmap( ret.GetBitmap(), gc, GetBitmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
            gdk_gc_destroy( gc );
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
        gdk_gc_destroy( gc );

        ret.SetMask( mask );
    }

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, int type, wxPalette *WXUNUSED(palette) )
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid bitmap") );

    // Try to save the bitmap via wxImage handlers:
    {
        wxImage image = ConvertToImage();
        if (image.Ok()) return image.SaveFile( name, type );
    }

    return FALSE;
}

bool wxBitmap::LoadFile( const wxString &name, int type )
{
    UnRef();

    if (!wxFileExists(name))
        return FALSE;

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

        gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

        M_BMPDATA->m_bpp = visual->depth;
    }
    else // try if wxImage can load it
    {
        wxImage image;
        if ( !image.LoadFile( name, type ) || !image.Ok() )
            return FALSE;

        *this = wxBitmap(image);
    }

    return TRUE;
}

wxPalette *wxBitmap::GetPalette() const
{
    if (!Ok())
        return (wxPalette *) NULL;

    return M_BMPDATA->m_palette;
}

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
#ifdef __WXGTK20__
    PurgeOtherRepresentations(Pixmap);
#endif
}

void wxBitmap::SetBitmap( GdkPixmap *bitmap )
{
    if (!m_refData)
        m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bitmap = bitmap;
#ifdef __WXGTK20__
    PurgeOtherRepresentations(Pixmap);
#endif
}

GdkPixmap *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( Ok(), (GdkPixmap *) NULL, wxT("invalid bitmap") );

#ifdef __WXGTK20__
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
#endif // __WXGTK20__

    return M_BMPDATA->m_pixmap;
}

bool wxBitmap::HasPixmap() const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixmap != NULL;
}

GdkBitmap *wxBitmap::GetBitmap() const
{
    wxCHECK_MSG( Ok(), (GdkBitmap *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_bitmap;
}

#ifdef __WXGTK20__
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
                    
                gdk_pixbuf_unref(pmask);
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
        gdk_pixbuf_unref( M_BMPDATA->m_pixbuf );
        M_BMPDATA->m_pixbuf = NULL;
    }
    if (keep == Pixbuf && HasPixmap())
    {
        gdk_pixmap_unref( M_BMPDATA->m_pixmap );
        M_BMPDATA->m_pixmap = NULL;
    }
}

#endif // __WXGTK20__
