/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/filefn.h"
#include "wx/image.h"
#include "wx/dcmemory.h"
#include "wx/app.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#if (GTK_MINOR_VERSION > 0)
#include <gdk/gdkrgb.h>
#endif

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

extern GtkWidget *wxRootWindow;

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

    wxImage image( bitmap );
    if (!image.Ok()) return FALSE;

    m_bitmap = gdk_pixmap_new( wxRootWindow->window, image.GetWidth(), image.GetHeight(), 1 );
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

    GdkVisual *visual = gdk_window_get_visual( wxRootWindow->window );
    wxASSERT( visual );
    
    int bpp = visual->depth;
    if ((bpp == 16) && (visual->red_mask != 0xf800)) bpp = 15;
    if (bpp == 15)
    {
        red = red & 0xf8;
        green = green & 0xf8;
        blue = blue & 0xf8;
    }
    if (bpp == 16)
    {
        red = red & 0xf8;
        green = green & 0xfc;
        blue = blue & 0xf8;
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

    m_bitmap = gdk_pixmap_new( wxRootWindow->window, bitmap.GetWidth(), bitmap.GetHeight(), 1 );

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
    m_mask = (wxMask *) NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
    m_palette = (wxPalette *) NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap) gdk_pixmap_unref( m_pixmap );
    if (m_bitmap) gdk_bitmap_unref( m_bitmap );
    if (m_mask) delete m_mask;
    if (m_palette) delete m_palette;
}

//-----------------------------------------------------------------------------

#define M_BMPDATA ((wxBitmapRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap()
{
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( int width, int height, int depth )
{
    Create( width, height, depth );

    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

bool wxBitmap::Create( int width, int height, int depth )
{
    UnRef();

    wxCHECK_MSG( (width > 0) && (height > 0), FALSE, wxT("invalid bitmap size") )

    GdkVisual *visual = gdk_window_get_visual( wxRootWindow->window );
    wxASSERT( visual );

    if (depth == -1) depth = visual->depth;

    wxCHECK_MSG( (depth == visual->depth) ||
                 (depth == 1), FALSE, wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();
    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    if (depth == 1)
    {
        M_BMPDATA->m_bitmap = gdk_pixmap_new( wxRootWindow->window, width, height, 1 );
        M_BMPDATA->m_bpp = 1;
    }
    else
    {
        M_BMPDATA->m_pixmap = gdk_pixmap_new( wxRootWindow->window, width, height, depth );
        M_BMPDATA->m_bpp = visual->depth;
    }

    return Ok();
}

bool wxBitmap::CreateFromXpm( const char **bits )
{
    wxCHECK_MSG( bits != NULL, FALSE, wxT("invalid bitmap data") )

    GdkVisual *visual = gdk_window_get_visual( wxRootWindow->window );
    wxASSERT( visual );
    
    m_refData = new wxBitmapRefData();

    GdkBitmap *mask = (GdkBitmap*) NULL;

    M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm_d( wxRootWindow->window, &mask, NULL, (gchar **) bits );

    wxCHECK_MSG( M_BMPDATA->m_pixmap, FALSE, wxT("couldn't create pixmap") );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask();
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

    M_BMPDATA->m_bpp = visual->depth;  // ?
    
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);

    return TRUE;
}

extern GtkWidget *wxRootWindow;

bool wxBitmap::CreateFromImage( const wxImage& image, int depth )
{
    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") )
    wxCHECK_MSG( depth == -1 || depth == 1, FALSE, wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();
      
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);

    // ------
    // convertion to mono bitmap:
    // ------
    if (depth == 1)
    {
        int width = image.GetWidth();
        int height = image.GetHeight();

        SetHeight( height );
        SetWidth( width );

        SetBitmap( gdk_pixmap_new( wxRootWindow->window, width, height, 1 ) );

        SetDepth( 1 );

        GdkVisual *visual = gdk_window_get_visual( wxRootWindow->window );
        wxASSERT( visual );

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
            mask->m_bitmap = gdk_pixmap_new( wxRootWindow->window, width, height, 1 );

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
    }
    
    // ------
    // convertion to colour bitmap:
    // ------
    else
    {
        int width = image.GetWidth();
        int height = image.GetHeight();

        SetHeight( height );
        SetWidth( width );

        SetPixmap( gdk_pixmap_new( wxRootWindow->window, width, height, -1 ) );

        // Retrieve depth

        GdkVisual *visual = gdk_window_get_visual( wxRootWindow->window );
        wxASSERT( visual );

        int bpp = visual->depth;

        SetDepth( bpp );

        if ((bpp == 16) && (visual->red_mask != 0xf800)) bpp = 15;
        if (bpp < 8) bpp = 8;

#if (GTK_MINOR_VERSION > 0)

        if (!image.HasMask() && (bpp > 8))
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

#endif

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
            mask->m_bitmap = gdk_pixmap_new( wxRootWindow->window, width, height, 1 );

            SetMask( mask );
        }

        // Render

        enum byte_order { RGB, RBG, BRG, BGR, GRB, GBR };
        byte_order b_o = RGB;

        if (bpp >= 24)
        {
            if ((visual->red_mask > visual->green_mask) && (visual->green_mask > visual->blue_mask))      b_o = RGB;
            else if ((visual->red_mask > visual->blue_mask) && (visual->blue_mask > visual->green_mask))  b_o = RGB;
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
                case 15:
                    {
                        guint32 pixel = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
                        gdk_image_put_pixel( data_image, x, y, pixel );
                        break;
                    }
                case 16:
                    {
                        guint32 pixel = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
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
    }

    return TRUE;
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;
    
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    GdkImage *gdk_image = (GdkImage*) NULL;
    if (GetPixmap())
    {
        gdk_image = gdk_image_get( GetPixmap(),
            0, 0,
            GetWidth(), GetHeight() );
    } else
    if (GetBitmap())
    {
        gdk_image = gdk_image_get( GetBitmap(),
            0, 0,
            GetWidth(), GetHeight() );
    } else
    {
        wxFAIL_MSG( wxT("Ill-formed bitmap") );
    }

    wxCHECK_MSG( gdk_image, wxNullImage, wxT("couldn't create image") );
    
    image.Create( GetWidth(), GetHeight() );
    char unsigned *data = image.GetData();

    if (!data)
    {
        gdk_image_destroy( gdk_image );
        wxFAIL_MSG( wxT("couldn't create image") );
        return wxNullImage;
    }

    GdkImage *gdk_image_mask = (GdkImage*) NULL;
    if (GetMask())
    {
        gdk_image_mask = gdk_image_get( GetMask()->GetBitmap(),
            0, 0,
            GetWidth(), GetHeight() );

        image.SetMaskColour( 16, 16, 16 );  // anything unlikely and dividable
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

        if (visual == NULL) visual = gdk_window_get_visual( wxRootWindow->window );
        bpp = visual->depth;
        if (bpp == 16) bpp = visual->red_prec + visual->green_prec + visual->blue_prec;
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
                    data[pos] = 16;
                    data[pos+1] = 16;
                    data[pos+2] = 16;
                }
            }

            pos += 3;
        }
    }

    gdk_image_destroy( gdk_image );
    if (gdk_image_mask) gdk_image_destroy( gdk_image_mask );    

    return image;
}

wxBitmap::wxBitmap( const wxBitmap& bmp )
{
    Ref( bmp );

    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( const wxString &filename, int type )
{
    LoadFile( filename, type );

    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( const char bits[], int width, int height, int WXUNUSED(depth))
{
    m_refData = new wxBitmapRefData();

    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_bitmap =
      gdk_bitmap_create_from_data( wxRootWindow->window, (gchar *) bits, width, height );
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    M_BMPDATA->m_bpp = 1;

    wxCHECK_RET( M_BMPDATA->m_bitmap, wxT("couldn't create bitmap") );

    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList) wxTheBitmapList->DeleteObject(this);
}

wxBitmap& wxBitmap::operator = ( const wxBitmap& bmp )
{
    if (*this == bmp) return (*this);
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
    return (m_refData != NULL);
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

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, M_BMPDATA->m_bpp );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

    if (ret.GetPixmap())
    {
        GdkGC *gc = gdk_gc_new( ret.GetPixmap() );
        gdk_draw_pixmap( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
        gdk_gc_destroy( gc );
    }
    else
    {
        GdkGC *gc = gdk_gc_new( ret.GetBitmap() );
        gdk_wx_draw_bitmap( ret.GetBitmap(), gc, GetBitmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
        gdk_gc_destroy( gc );
    }

    if (GetMask())
    {
        wxMask *mask = new wxMask;
        mask->m_bitmap = gdk_pixmap_new( wxRootWindow->window, rect.width, rect.height, 1 );

        GdkGC *gc = gdk_gc_new( mask->m_bitmap );
        gdk_wx_draw_bitmap( mask->m_bitmap, gc, M_BMPDATA->m_mask->m_bitmap, 0, 0, rect.x, rect.y, rect.width, rect.height );
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
        wxImage image( *this );
        if (image.Ok()) return image.SaveFile( name, type );
    }

    return FALSE;
}

bool wxBitmap::LoadFile( const wxString &name, int type )
{
    UnRef();

    if (!wxFileExists(name)) return FALSE;

    GdkVisual *visual = gdk_window_get_visual( wxRootWindow->window );
    wxASSERT( visual );
    
    if (type == wxBITMAP_TYPE_XPM)
    {
        m_refData = new wxBitmapRefData();

        GdkBitmap *mask = (GdkBitmap*) NULL;

        M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm( wxRootWindow->window, &mask, NULL, name.fn_str() );

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
        if (!image.LoadFile( name, type )) return FALSE;
        if (image.Ok()) *this = image.ConvertToBitmap();
        else return FALSE;
    }

    return TRUE;
}

wxPalette *wxBitmap::GetPalette() const
{
    if (!Ok()) return (wxPalette *) NULL;

    return M_BMPDATA->m_palette;
}

void wxBitmap::SetHeight( int height )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bpp = depth;
}

void wxBitmap::SetPixmap( GdkPixmap *pixmap )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_pixmap = pixmap;
}

void wxBitmap::SetBitmap( GdkPixmap *bitmap )
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bitmap = bitmap;
}

GdkPixmap *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( Ok(), (GdkPixmap *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixmap;
}

GdkBitmap *wxBitmap::GetBitmap() const
{
    wxCHECK_MSG( Ok(), (GdkBitmap *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_bitmap;
}
