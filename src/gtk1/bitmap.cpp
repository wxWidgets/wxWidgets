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

#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>

// in GTK+ 1.3 gdk_root_parent was renamed into gdk_parent_root
#ifdef __WXGTK13__
    #define gdk_root_parent gdk_parent_root
#else // GTK+ <= 1.2
    // need to get the declaration of gdk_root_parent from private header
    #include <gdk/gdkx.h>
#endif // GTK+ 1.3/1.2

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

    GdkWindow *parent = (GdkWindow*) &gdk_root_parent;
    m_bitmap = gdk_pixmap_new( parent, image.GetWidth(), image.GetHeight(), 1 );
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

    GdkVisual *visual = gdk_visual_get_system();
    int bpp = visual->depth;
    if ((bpp == 16) && (visual->red_mask != 0xf800)) bpp = 15;
    if (bpp == 15)
    {
        red = red & 0xf8;
        blue = blue & 0xf8;
        green = green & 0xf8;
    }
    if (bpp == 16)
    {
        red = red & 0xf8;
        blue = blue & 0xfc;
        green = green & 0xf8;
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

bool wxMask::Create( const wxBitmap& WXUNUSED(bitmap),
                     int WXUNUSED(paletteIndex) )
{
    if (m_bitmap)
    {
        gdk_bitmap_unref( m_bitmap );
        m_bitmap = (GdkBitmap*) NULL;
    }

    wxFAIL_MSG( wxT("not implemented") );

    return FALSE;
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

    m_bitmap = gdk_pixmap_new( (GdkWindow*) &gdk_root_parent, bitmap.GetWidth(), bitmap.GetHeight(), 1 );

    if (!m_bitmap) return FALSE;

    GdkGC *gc = gdk_gc_new( m_bitmap );

    gdk_draw_bitmap( m_bitmap, gc, bitmap.GetBitmap(), 0, 0, 0, 0, bitmap.GetWidth(), bitmap.GetHeight() );

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
    wxCHECK_RET( (width > 0) && (height > 0), wxT("invalid bitmap size") )

    GdkWindow *parent = (GdkWindow*) &gdk_root_parent;
    if (depth == -1) depth = gdk_window_get_visual( parent )->depth;

    wxCHECK_RET( (depth ==  gdk_window_get_visual( parent )->depth) ||
                 (depth == 1), wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();
    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    if (depth == 1)
    {
        M_BMPDATA->m_bitmap = gdk_pixmap_new( parent, width, height, 1 );
        M_BMPDATA->m_bpp = 1;
    }
    else
    {
        M_BMPDATA->m_pixmap = gdk_pixmap_new( parent, width, height, depth );
        M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;
    }

    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

bool wxBitmap::CreateFromXpm( const char **bits )
{
    wxCHECK_MSG( bits != NULL, FALSE, wxT("invalid bitmap data") )

    m_refData = new wxBitmapRefData();

    GdkBitmap *mask = (GdkBitmap*) NULL;
    GdkWindow *parent = (GdkWindow*) &gdk_root_parent;

    M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm_d( parent, &mask, NULL, (gchar **) bits );

    wxCHECK_MSG( M_BMPDATA->m_pixmap, FALSE, wxT("couldn't create pixmap") );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask();
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

    M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;  // ?
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);

    return TRUE;
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
      gdk_bitmap_create_from_data( (GdkWindow*) &gdk_root_parent, (gchar *) bits, width, height );
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

bool wxBitmap::operator == ( const wxBitmap& bmp )
{
    return m_refData == bmp.m_refData;
}

bool wxBitmap::operator != ( const wxBitmap& bmp )
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
        gdk_draw_bitmap( ret.GetBitmap(), gc, GetBitmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
        gdk_gc_destroy( gc );
    }

    if (GetMask())
    {
        wxMask *mask = new wxMask;
        GdkWindow *parent = (GdkWindow*) &gdk_root_parent;
        mask->m_bitmap = gdk_pixmap_new( parent, rect.width, rect.height, 1 );

        GdkGC *gc = gdk_gc_new( mask->m_bitmap );
        gdk_draw_bitmap( mask->m_bitmap, gc, M_BMPDATA->m_mask->m_bitmap, 0, 0, rect.x, rect.y, rect.width, rect.height );
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

    if (type == wxBITMAP_TYPE_XPM)
    {
        m_refData = new wxBitmapRefData();

        GdkBitmap *mask = (GdkBitmap*) NULL;
        GdkWindow *parent = (GdkWindow*) &gdk_root_parent;

        M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm( parent, &mask, NULL, name.fn_str() );

        if (mask)
        {
           M_BMPDATA->m_mask = new wxMask();
           M_BMPDATA->m_mask->m_bitmap = mask;
        }

        gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
        M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;
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
