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
#include "gdk/gdkprivate.h"
#include "gdk/gdkx.h"
#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask()
{
    m_bitmap = (GdkBitmap *) NULL;
}

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap), const wxColour& WXUNUSED(colour) )
{
}

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap), int WXUNUSED(paletteIndex) )
{
}

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap) )
{
}

wxMask::~wxMask()
{
    if (m_bitmap) gdk_bitmap_unref( m_bitmap );
}

GdkBitmap *wxMask::GetBitmap(void) const
{
    return m_bitmap;
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
public:
  wxBitmapRefData(void);
  ~wxBitmapRefData(void);

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
    wxCHECK_RET( (width > 0) && (height > 0), "invalid bitmap size" )
    wxCHECK_RET( (depth > 0) || (depth == -1), "invalid bitmap depth" )

    m_refData = new wxBitmapRefData();

    GdkWindow *parent = (GdkWindow*) &gdk_root_parent;

    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_pixmap = gdk_pixmap_new( parent, width, height, depth );
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;

    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( const char **bits )
{
    wxCHECK_RET( bits != NULL, "invalid bitmap data" )

    m_refData = new wxBitmapRefData();

    GdkBitmap *mask = (GdkBitmap*) NULL;
    GdkWindow *parent = (GdkWindow*) &gdk_root_parent;

    M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm_d( parent, &mask, NULL, (gchar **) bits );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask();
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

    M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;  // ?
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( char **bits )
{
    wxCHECK_RET( bits != NULL, "invalid bitmap data" )

    m_refData = new wxBitmapRefData();

    GdkBitmap *mask = (GdkBitmap*) NULL;
    GdkWindow *parent = (GdkWindow*) &gdk_root_parent;

    M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm_d( parent, &mask, NULL, (gchar **) bits );

    wxCHECK_RET( M_BMPDATA->m_pixmap, "couldn't create pixmap" );

    if (mask)
    {
        M_BMPDATA->m_mask = new wxMask();
        M_BMPDATA->m_mask->m_bitmap = mask;
    }

    gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );

    M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;  // ?
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
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

    wxCHECK_RET( M_BMPDATA->m_bitmap, "couldn't create bitmap" );

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

bool wxBitmap::Ok(void) const
{
    return (m_refData != NULL);
}

int wxBitmap::GetHeight(void) const
{
    wxCHECK_MSG( Ok(), -1, "invalid bitmap" );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth(void) const
{
    wxCHECK_MSG( Ok(), -1, "invalid bitmap" );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth(void) const
{
    wxCHECK_MSG( Ok(), -1, "invalid bitmap" );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask(void) const
{
    wxCHECK_MSG( Ok(), (wxMask *) NULL, "invalid bitmap" );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
    wxCHECK_RET( Ok(), "invalid bitmap" );

    if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;

    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::SaveFile( const wxString &name, int type, wxPalette *WXUNUSED(palette) )
{
    wxCHECK_MSG( Ok(), FALSE, "invalid bitmap" );

    if (type == wxBITMAP_TYPE_PNG)
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

        M_BMPDATA->m_pixmap = gdk_pixmap_create_from_xpm( parent, &mask, NULL, name );

        if (mask)
        {
           M_BMPDATA->m_mask = new wxMask();
           M_BMPDATA->m_mask->m_bitmap = mask;
        }

        gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
        M_BMPDATA->m_bpp = gdk_window_get_visual( parent )->depth;
    }
    else if (type == wxBITMAP_TYPE_PNG)
    {
        wxImage image;
        image.LoadFile( name, type );
        if (image.Ok()) *this = image.ConvertToBitmap();
    }
    else if (type == wxBITMAP_TYPE_BMP)
    {
        wxImage image;
        image.LoadFile( name, type );
        if (image.Ok()) *this = image.ConvertToBitmap();
    }
    else
        return FALSE;

    return TRUE;
}

wxPalette *wxBitmap::GetPalette(void) const
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

GdkPixmap *wxBitmap::GetPixmap(void) const
{
    wxCHECK_MSG( Ok(), (GdkPixmap *) NULL, "invalid bitmap" );

    return M_BMPDATA->m_pixmap;
}

GdkBitmap *wxBitmap::GetBitmap(void) const
{
    wxCHECK_MSG( Ok(), (GdkBitmap *) NULL, "invalid bitmap" );

    return M_BMPDATA->m_bitmap;
}


