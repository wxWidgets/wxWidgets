/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "gdk/gdkprivate.h"
#include "gdk/gdkx.h"

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

wxBitmapRefData::wxBitmapRefData(void)
{
  m_pixmap = (GdkPixmap *) NULL;
  m_bitmap = (GdkBitmap *) NULL;
  m_mask = (wxMask *) NULL;
  m_width = 0;
  m_height = 0;
  m_bpp = 0;
  m_palette = (wxPalette *) NULL;
}

wxBitmapRefData::~wxBitmapRefData(void)
{
  if (m_pixmap) gdk_pixmap_unref( m_pixmap );
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

  GdkBitmap *mask = NULL;
  
  M_BMPDATA->m_pixmap = 
    gdk_pixmap_create_from_xpm_d( (GdkWindow*) &gdk_root_parent, &mask, NULL, (gchar **) bits );
    
  if (mask)
  {
    M_BMPDATA->m_mask = new wxMask();
    M_BMPDATA->m_mask->m_bitmap = mask;
  }
  
  gdk_window_get_size( M_BMPDATA->m_pixmap, &(M_BMPDATA->m_width), &(M_BMPDATA->m_height) );
  
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
  return (m_refData != NULL);
}
  
int wxBitmap::GetHeight(void) const
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return -1;
  }

  return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth(void) const
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return -1;
  }
  
  return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth(void) const
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return -1;
  }
  
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
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return (wxMask *) NULL;
  }
  
  return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return;
  }
  
  if (M_BMPDATA->m_mask) delete M_BMPDATA->m_mask;
  
  M_BMPDATA->m_mask = mask;
}

bool wxBitmap::SaveFile( const wxString &WXUNUSED(name), int WXUNUSED(type), 
  wxPalette *WXUNUSED(palette) )
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return FALSE;
  }
  
  return FALSE;
}

bool wxBitmap::LoadFile( const wxString &WXUNUSED(name), int WXUNUSED(type) )
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return FALSE;
  }
  
  return FALSE;
}
        
wxPalette *wxBitmap::GetPalette(void) const
{
  if (!Ok()) return (wxPalette *) NULL;
  return M_BMPDATA->m_palette;
}

GdkPixmap *wxBitmap::GetPixmap(void) const
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return (GdkPixmap *) NULL;
  }
  
  return M_BMPDATA->m_pixmap;
}
  
GdkBitmap *wxBitmap::GetBitmap(void) const
{
  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return (GdkBitmap *) NULL;
  }
  
  return M_BMPDATA->m_bitmap;
}
  
wxBitmap::wxBitmap( const wxImage &image )
{
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
  
  if (!image.Ok()) return;

  m_refData = new wxBitmapRefData();

  M_BMPDATA->m_height = image.GetHeight();
  M_BMPDATA->m_width = image.GetWidth();
  int width = image.GetWidth();
  int height = image.GetHeight();
  
  // Create picture
  
  GdkImage *data_image = 
    gdk_image_new( GDK_IMAGE_FASTEST, gdk_visual_get_system(), width, height );
  
  M_BMPDATA->m_pixmap = 
    gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, -1 );

  // Create mask
  
  GdkImage *mask_image = (GdkImage*) NULL;
  
  if (image.HasMask())
  {
    unsigned char *mask_data = (unsigned char*)malloc( ((width >> 3)+8) * height );
  
    mask_image =  gdk_image_new_bitmap( gdk_visual_get_system(), mask_data, width, height );
  
    M_BMPDATA->m_mask = new wxMask();
    M_BMPDATA->m_mask->m_bitmap = gdk_pixmap_new( (GdkWindow*)&gdk_root_parent, width, height, 1 );
  }
  
  // Retrieve depth  
  
  M_BMPDATA->m_bpp = data_image->depth;
  
  int render_depth = 8;
  if (M_BMPDATA->m_bpp > 8) render_depth = M_BMPDATA->m_bpp;
  
  // Render
  
  int r_mask = image.GetMaskRed();
  int g_mask = image.GetMaskGreen();
  int b_mask = image.GetMaskBlue();
  
  unsigned char* data = image.GetData();
  
  int index = 0;
  for (int y = 0; y < height; y++)
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
	  if ((r == r_mask) && (b = b_mask) && (g = g_mask))
	    gdk_image_put_pixel( mask_image, x, y, 0 );
	  else
	    gdk_image_put_pixel( mask_image, x, y, 1 );
	}
	
	switch (render_depth)
	{
	  case 8:
	  {
	    GdkColormap *cmap = gtk_widget_get_default_colormap();
            GdkColor *colors = cmap->colors;
            int max = 3 * (65536);
            int index = -1;

            for (int i = 0; i < cmap->size; i++)
            {
              int rdiff = (r << 8) - colors[i].red;
              int gdiff = (g << 8) - colors[i].green;
              int bdiff = (b << 8) - colors[i].blue;
              int sum = ABS (rdiff) + ABS (gdiff) + ABS (bdiff);
              if (sum < max) { index = i; max = sum; }
            }
	    
	    gdk_image_put_pixel( data_image, x, y, index );
	    
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
	  case 24:
	  {
	    break;
	  }
	  case 32:
	  {
	    break;
	  }
	  default: break;
	}
      }
    
  // Blit picture
  
  GdkGC *data_gc = gdk_gc_new( M_BMPDATA->m_pixmap );
  
  gdk_draw_image( M_BMPDATA->m_pixmap, data_gc, data_image, 0, 0, 0, 0, width, height );
  
  gdk_image_destroy( data_image );
  gdk_gc_unref( data_gc );
  
  // Blit mask
  
  if (image.HasMask())
  {
    GdkGC *mask_gc = gdk_gc_new( M_BMPDATA->m_mask->m_bitmap );
  
    gdk_draw_image( M_BMPDATA->m_mask->m_bitmap, mask_gc, mask_image, 0, 0, 0, 0, width, height );
  
    gdk_image_destroy( mask_image );
    gdk_gc_unref( mask_gc );
  }
   
}

wxImage wxBitmap::ConvertToImage() const
{
  wxImage image;

  if (!Ok())
  {
    wxFAIL_MSG( "invalid bitmap" );
    return image;
  }
  
  GdkImage *gdk_image = gdk_image_get( M_BMPDATA->m_pixmap, 0, 0, M_BMPDATA->m_width, M_BMPDATA->m_height );
  
  if (!gdk_image) return image;
  
  image.Create( M_BMPDATA->m_width, M_BMPDATA->m_height );
  char unsigned *data = image.GetData();
  
  int bpp = gdk_image->bpp;
  GdkColormap *cmap = gtk_widget_get_default_colormap();
  
  long pos = 0;
  for (int j = 0; j < M_BMPDATA->m_height; j++)
  {
    for (int i = 0; i < M_BMPDATA->m_width; i++)
    {
      int pixel = gdk_image_get_pixel( gdk_image, i, j );
      if (bpp <= 8)
      {
        data[pos] = cmap->colors[pixel].red >> 8;
        data[pos+1] = cmap->colors[pixel].green >> 8;
        data[pos+2] = cmap->colors[pixel].blue >> 8;
      } else if (bpp == 15)
      {
        data[pos] = (pixel >> 7) & 0xf8;
        data[pos+1] = (pixel >> 3) & 0xf8;
        data[pos+2] = (pixel << 3) & 0xf8;
      } else if (bpp == 16)
      {
        data[pos] = (pixel >> 8) & 0xf8;
        data[pos+1] = (pixel >> 3) & 0xfc;
        data[pos+2] = (pixel << 3) & 0xf8;
      } else
      {
        data[pos] = (pixel >> 16) & 0xff;
        data[pos+1] = (pixel >> 8) & 0xff;
        data[pos+2] = pixel & 0xff;
      }
      
      pos += 3;
    }
  }
  
  gdk_image_destroy( gdk_image );
  
  return image;
}



