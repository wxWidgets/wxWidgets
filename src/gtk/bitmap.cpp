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
#include "wx/icon.h"
#include "gdk/gdkprivate.h"

#ifdef USE_GDK_IMLIB

#include "../gdk_imlib/gdk_imlib.h"
#include "gdk/gdkx.h"        // GDK_DISPLAY
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#endif

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask(void)
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

wxMask::~wxMask(void)
{
#ifdef USE_GDK_IMLIB
  // do not delete the mask, gdk_imlib does it for you
#else
  if (m_bitmap) gdk_bitmap_unref( m_bitmap );
#endif  
}

GdkBitmap *wxMask::GetBitmap(void) const
{
  return m_bitmap;
}
  
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
#ifdef USE_GDK_IMLIB
    GdkImlibImage  *m_image;
#endif
    wxPalette      *m_palette;
};

wxBitmapRefData::wxBitmapRefData(void)
{
  m_pixmap = (GdkPixmap *) NULL;
  m_bitmap = (GdkBitmap *) NULL;
  m_mask = (wxMask *) NULL;
  m_width = 0;
  m_height = 0;
  m_bpp = 0;
  m_palette = (wxPalette *) NULL;
#ifdef USE_GDK_IMLIB
  m_image = (GdkImlibImage *) NULL;
#endif
}

wxBitmapRefData::~wxBitmapRefData(void)
{
#ifdef USE_GDK_IMLIB
  if (m_pixmap) gdk_imlib_free_pixmap( m_pixmap );
  if (m_image) gdk_imlib_kill_image( m_image );
#else
  if (m_pixmap) gdk_pixmap_unref( m_pixmap );
#endif
  if (m_bitmap) gdk_bitmap_unref( m_bitmap );
  if (m_mask) delete m_mask;
  if (m_palette) delete m_palette;
}

//-----------------------------------------------------------------------------

#define M_BMPDATA ((wxBitmapRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap(void)
{
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}
  
wxBitmap::wxBitmap( int width, int height, int depth )
{
  m_refData = new wxBitmapRefData();
  M_BMPDATA->m_mask = (wxMask *) NULL;
  M_BMPDATA->m_pixmap = 
    gdk_pixmap_new( (GdkWindow*) &gdk_root_parent, width, height, depth );
  M_BMPDATA->m_width = width;
  M_BMPDATA->m_height = height;
  M_BMPDATA->m_bpp = depth;
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( char **bits )
{
  m_refData = new wxBitmapRefData();

#ifndef USE_GDK_IMLIB

  GdkBitmap *mask = NULL;
  
  M_BMPDATA->m_pixmap = 
    gdk_pixmap_create_from_xpm_d( (GdkWindow*) &gdk_root_parent, &mask, NULL, (gchar **) bits );
    
  if (mask)
  {
    M_BMPDATA->m_mask = new wxMask();
    M_BMPDATA->m_mask->m_bitmap = mask;
  }
  
  gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  
#else

  M_BMPDATA->m_image = gdk_imlib_create_image_from_xpm_data( bits );
  Render();
  
#endif
				  
  M_BMPDATA->m_bpp = 24; // ?
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}
  
wxBitmap::wxBitmap( const wxBitmap& bmp )
{
  Ref( bmp );
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}
  
wxBitmap::wxBitmap( const wxBitmap* bmp )
{
  if (bmp) Ref( *bmp );
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap( const wxString &filename, int type )
{
  LoadFile( filename, type );
  
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}

// CMB 15/5/98: add constructor for xbm bitmaps
wxBitmap::wxBitmap( const char bits[], int width, int height, int WXUNUSED(depth))
{
  m_refData = new wxBitmapRefData();

  M_BMPDATA->m_mask = (wxMask *) NULL;
  M_BMPDATA->m_bitmap = 
    gdk_bitmap_create_from_data( (GdkWindow*) &gdk_root_parent, (gchar *) bits, width, height );
  M_BMPDATA->m_width = width;
  M_BMPDATA->m_height = height;
  M_BMPDATA->m_bpp = 1;

  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
}
  
wxBitmap::~wxBitmap(void)
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
  wxASSERT_MSG( m_refData != NULL, "invalid bitmap" );
  return (m_refData != NULL);
}
  
int wxBitmap::GetHeight(void) const
{
  if (!Ok()) return 0;
  return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth(void) const
{
  if (!Ok()) return 0;
  return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth(void) const
{
  if (!Ok()) return 0;
  return M_BMPDATA->m_bpp;
}

void wxBitmap::SetHeight( int height )
{
  if (!Ok()) return;
  
  wxFAIL_MSG( "wxBitmap::SetHeight not implemented" );
  
  M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
  if (!Ok()) return;
  
  wxFAIL_MSG( "wxBitmap::SetWidth not implemented" );
  
  M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
  if (!Ok()) return;
  
  wxFAIL_MSG( "wxBitmap::SetDepth not implemented" );
  
  M_BMPDATA->m_bpp = depth;
}

wxMask *wxBitmap::GetMask(void) const
{
  if (!Ok()) return (wxMask *) NULL;
  
  return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
  if (!Ok()) return;
  
  if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;
  
  M_BMPDATA->m_mask = mask;
}

void wxBitmap::Resize( int height, int width )
{
  if (!Ok()) return;
  
#ifdef USE_GDK_IMLIB
  
  if (M_BMPDATA->m_bitmap) return;  // not supported for bitmaps
  
  if (!M_BMPDATA->m_image) RecreateImage();
  
  if (M_BMPDATA->m_pixmap) gdk_imlib_free_pixmap( M_BMPDATA->m_pixmap );
  if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;
  
  GdkImlibImage* image = gdk_imlib_clone_scaled_image( M_BMPDATA->m_image, height, width );
  gdk_imlib_destroy_image( M_BMPDATA->m_image );
  M_BMPDATA->m_image = image;
  M_BMPDATA->m_height = height;
  M_BMPDATA->m_width = width;
  
  Render();
  
#else
  
  wxFAIL_MSG( "wxBitmap::Resize not implemented without GdkImlib" );
  
#endif
}

bool wxBitmap::SaveFile( const wxString &name, int WXUNUSED(type), 
  wxPalette *WXUNUSED(palette) )
{
#ifdef USE_GDK_IMLIB

  if (!Ok()) return FALSE;
  
  if (!M_BMPDATA->m_image) RecreateImage();
  
  return gdk_imlib_save_image( M_BMPDATA->m_image, WXSTRINGCAST name, (GdkImlibSaveInfo *) NULL );

#else
  
  wxFAIL_MSG( "wxBitmap::SaveFile not implemented without GdkImlib" );
  
#endif

  return FALSE;
}

bool wxBitmap::LoadFile( const wxString &name, int WXUNUSED(type) )
{
#ifdef USE_GDK_IMLIB

  UnRef();
  m_refData = new wxBitmapRefData();
  
  M_BMPDATA->m_image = gdk_imlib_load_image( WXSTRINGCAST name );

  if (!M_BMPDATA->m_image)
  {
    UnRef();
    return FALSE;
  }

  Render();
				  
  gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  M_BMPDATA->m_bpp = 24; // ?
  
  return TRUE;
  
#else
  
  wxFAIL_MSG( "wxBitmap::LoadFile not implemented without GdkImlib" );
  
#endif

  return FALSE;
}
        
wxPalette *wxBitmap::GetPalette(void) const
{
  if (!Ok()) return (wxPalette *) NULL;
  return M_BMPDATA->m_palette;
}

GdkPixmap *wxBitmap::GetPixmap(void) const
{
  if (!Ok()) return (GdkPixmap *) NULL;
  
//  if (!M_BMPDATA->m_image) RecreateImage();
  
  return M_BMPDATA->m_pixmap;
}
  
GdkBitmap *wxBitmap::GetBitmap(void) const
{
  if (!Ok()) return (GdkBitmap *) NULL;
  
  return M_BMPDATA->m_bitmap;
}
  
void wxBitmap::DestroyImage(void)
{
  if (!Ok()) return;
  
  if (M_BMPDATA->m_image)
  {
    gdk_imlib_destroy_image( M_BMPDATA->m_image );
    M_BMPDATA->m_image = (GdkImlibImage *) NULL;
  }
}

void wxBitmap::RecreateImage(void)
{
  if (!Ok()) return;
  
#ifdef USE_GDK_IMLIB

  DestroyImage();
  
  wxCHECK_RET( M_BMPDATA->m_pixmap != NULL, "invalid bitmap" );
  
  long size = (long)(M_BMPDATA->m_width)*(long)(M_BMPDATA->m_height)*(long)3;
  unsigned char *data = new unsigned char[size];
  for (long i = 0; i < size; i++) data[i] = 100;
  
  GdkImage *image = gdk_image_get( M_BMPDATA->m_pixmap, 0, 0, M_BMPDATA->m_width, M_BMPDATA->m_height );
  
  long pos = 0;
  for (int j = 0; j < M_BMPDATA->m_height; j++)
  {
    for (int i = 0; i < M_BMPDATA->m_width; i++)
    {
      XColor xcol;
      xcol.pixel = gdk_image_get_pixel( image, i, j );
      Colormap cm = ((GdkColormapPrivate*)gdk_imlib_get_colormap())->xcolormap;
      XQueryColor( gdk_display, cm, &xcol );
      
      data[pos] = xcol.red;
      data[pos+1] = xcol.green;
      data[pos+2] = xcol.blue;
      pos += 3;
    }
  }
  
  wxCHECK_RET( M_BMPDATA->m_pixmap != NULL, "invalid bitmap" );
  
  M_BMPDATA->m_image = gdk_imlib_create_image_from_data( 
     data, (unsigned char*)NULL, M_BMPDATA->m_width, M_BMPDATA->m_height );
  
  delete[] data;
  
  gdk_image_destroy( image );
  
  Render();
  
#else
  
  wxFAIL_MSG( "wxBitmap::RecreateImage not implemented without GdkImlib" );
  
#endif
}

void wxBitmap::Render(void)
{
  if (!Ok()) return;
  
#ifdef USE_GDK_IMLIB

  if (!M_BMPDATA->m_image) RecreateImage();
  
  if (M_BMPDATA->m_pixmap)
  { 
    gdk_imlib_free_pixmap( M_BMPDATA->m_pixmap );
    M_BMPDATA->m_pixmap = (GdkPixmap*) NULL;
  }
  if (M_BMPDATA->m_mask)
  {
    delete M_BMPDATA->m_mask;
    M_BMPDATA->m_mask = (wxMask*) NULL;
  }
  
  gdk_imlib_render( M_BMPDATA->m_image, M_BMPDATA->m_image->rgb_width, M_BMPDATA->m_image->rgb_height );
  M_BMPDATA->m_width = M_BMPDATA->m_image->rgb_width;
  M_BMPDATA->m_height = M_BMPDATA->m_image->rgb_height;
  M_BMPDATA->m_pixmap = gdk_imlib_move_image( M_BMPDATA->m_image );
  
  wxCHECK_RET( M_BMPDATA->m_pixmap != NULL, "pixmap rendering failed" ) 
  
  GdkBitmap *mask = gdk_imlib_move_mask( M_BMPDATA->m_image );
  if (mask)
  {
    M_BMPDATA->m_mask = new wxMask();
    M_BMPDATA->m_mask->m_bitmap = mask;
  }
  
#else
  
  wxFAIL_MSG( "wxBitmap::Render not implemented without GdkImlib" );
  
#endif
}


