/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/bitmap.h"
#include "gdk/gdkprivate.h"

#ifdef USE_GDK_IMLIB
#include "../gdk_imlib/gdk_imlib.h"
#endif

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask(void)
{
  m_bitmap = NULL;
};

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap), const wxColour& WXUNUSED(colour) )
{
};

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap), const int WXUNUSED(paletteIndex) )
{
};

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap) )
{
};

wxMask::~wxMask(void)
{
#ifdef USE_GDK_IMLIB
  // do not delete the mask, gdk_imlib does it for you
#else
  if (m_bitmap) gdk_bitmap_unref( m_bitmap );
#endif  
};

GdkBitmap *wxMask::GetBitmap(void) const
{
  return m_bitmap;
};
  
//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

// CMB 20/5/98: added m_bitmap for GdkBitmaps
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

wxBitmapRefData::wxBitmapRefData(void)
{
  m_pixmap = NULL;
  m_bitmap = NULL;
  m_mask = NULL;
  m_width = 0;
  m_height = 0;
  m_bpp = 0;
  m_palette = NULL;
};

wxBitmapRefData::~wxBitmapRefData(void)
{
#ifdef USE_GDK_IMLIB
  if (m_pixmap) gdk_imlib_free_pixmap( m_pixmap );
#else
  if (m_pixmap) gdk_pixmap_unref( m_pixmap );
  if (m_bitmap) gdk_bitmap_unref( m_bitmap );
#endif
  if (m_mask) delete m_mask;
  if (m_palette) delete m_palette;
};

//-----------------------------------------------------------------------------

#define M_BMPDATA ((wxBitmapRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap(void)
{
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
};
  
wxBitmap::wxBitmap( const int width, const int height, const int depth )
{
  m_refData = new wxBitmapRefData();
  M_BMPDATA->m_mask = NULL;
  M_BMPDATA->m_pixmap = 
    gdk_pixmap_new( (GdkWindow*) &gdk_root_parent, width, height, depth );
  gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  M_BMPDATA->m_bpp = depth;
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
};

wxBitmap::wxBitmap( char **bits )
{
  m_refData = new wxBitmapRefData();
  
  GdkBitmap *mask = NULL;

#ifndef USE_GDK_IMLIB
  M_BMPDATA->m_pixmap = 
    gdk_pixmap_create_from_xpm_d( (GdkWindow*) &gdk_root_parent, &mask, NULL, (gchar **) bits );
#else
  M_BMPDATA->m_pixmap = NULL;
  int res = gdk_imlib_data_to_pixmap( bits, &M_BMPDATA->m_pixmap, &mask );
#endif
  
  if (mask)
  {
    M_BMPDATA->m_mask = new wxMask();
    M_BMPDATA->m_mask->m_bitmap = mask;
  };
				  
  gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  M_BMPDATA->m_bpp = 24; // ?
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
};
  
wxBitmap::wxBitmap( const wxBitmap& bmp )
{
  Ref( bmp );
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
};
  
wxBitmap::wxBitmap( const wxBitmap* bmp )
{
  if (bmp) Ref( *bmp );
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
};

wxBitmap::wxBitmap( const wxString &filename, const int type )
{
  LoadFile( filename, type );
};

// CMB 15/5/98: add constructor for xbm bitmaps
wxBitmap::wxBitmap( const char bits[], const int width, const int height, const int WXUNUSED(depth))
{
  m_refData = new wxBitmapRefData();

  M_BMPDATA->m_mask = NULL;
  M_BMPDATA->m_bitmap = 
    gdk_bitmap_create_from_data( (GdkWindow*) &gdk_root_parent, (gchar *) bits, width, height );
  gdk_window_get_size( M_BMPDATA->m_bitmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  M_BMPDATA->m_bpp = 1;

  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}
  
wxBitmap::~wxBitmap(void)
{
  if (wxTheBitmapList) wxTheBitmapList->DeleteObject(this);
};
  
wxBitmap& wxBitmap::operator = ( const wxBitmap& bmp )
{
  if (*this == bmp) return (*this); 
  Ref( bmp ); 
  return *this; 
};
  
bool wxBitmap::operator == ( const wxBitmap& bmp )
{
  return m_refData == bmp.m_refData; 
};
  
bool wxBitmap::operator != ( const wxBitmap& bmp )
{
  return m_refData != bmp.m_refData; 
};
  
bool wxBitmap::Ok(void) const
{
  return m_refData != NULL;
};
  
int wxBitmap::GetHeight(void) const
{
  if (!Ok()) return 0;
  return M_BMPDATA->m_height;
};

int wxBitmap::GetWidth(void) const
{
  if (!Ok()) return 0;
  return M_BMPDATA->m_width;
};

int wxBitmap::GetDepth(void) const
{
  if (!Ok()) return 0;
  return M_BMPDATA->m_bpp;
};

void wxBitmap::SetHeight( const int height )
{
  if (!Ok()) return;
  M_BMPDATA->m_height = height;
};

void wxBitmap::SetWidth( const int width )
{
  if (!Ok()) return;
  M_BMPDATA->m_width = width;
};

void wxBitmap::SetDepth( const int depth )
{
  if (!Ok()) return;
  M_BMPDATA->m_bpp = depth;
};

wxMask *wxBitmap::GetMask(void) const
{
  if (!Ok()) return NULL;
  return M_BMPDATA->m_mask;
};

void wxBitmap::SetMask( wxMask *mask )
{
  if (!Ok()) return;
  if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;
  M_BMPDATA->m_mask = mask;
};

bool wxBitmap::SaveFile( const wxString &WXUNUSED(name), const int WXUNUSED(type), 
  wxPalette *WXUNUSED(palette) )
{
  return FALSE;
};

bool wxBitmap::LoadFile( const wxString &name, const int WXUNUSED(type) )
{
#ifdef USE_GDK_IMLIB

  UnRef();
  m_refData = new wxBitmapRefData();
  M_BMPDATA->m_mask = NULL;

  GdkBitmap *mask = NULL;
  
  int res = gdk_imlib_load_file_to_pixmap( WXSTRINGCAST name, &M_BMPDATA->m_pixmap, &mask );

  if (res != 1)
  {
    UnRef();
    return FALSE;
  };

  if (mask)
  {
    M_BMPDATA->m_mask = new wxMask();
    M_BMPDATA->m_mask->m_bitmap = mask;
  }
				  
  gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  M_BMPDATA->m_bpp = 24; // ?
  
  return TRUE;
#endif

  return FALSE;
};
        
wxPalette *wxBitmap::GetPalette(void) const
{
  if (!Ok()) return NULL;
  return M_BMPDATA->m_palette;
};

GdkPixmap *wxBitmap::GetPixmap(void) const
{
  if (!Ok()) return NULL;
  return M_BMPDATA->m_pixmap;
};
  
GdkBitmap *wxBitmap::GetBitmap(void) const
{
  if (!Ok()) return NULL;
  return M_BMPDATA->m_bitmap;
};
  
