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


//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask(void)
{
};

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap), const wxColour& WXUNUSED(colour) )
{
};

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap), int WXUNUSED(paletteIndex) )
{
};

wxMask::wxMask( const wxBitmap& WXUNUSED(bitmap) )
{
};

wxMask::~wxMask(void)
{
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
  public:
  
    wxBitmapRefData(void);
    ~wxBitmapRefData(void);
  
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
    wxPalette      *m_palette;
};

wxBitmapRefData::wxBitmapRefData(void)
{
  m_mask = NULL;
  m_width = 0;
  m_height = 0;
  m_bpp = 0;
  m_palette = NULL;
};

wxBitmapRefData::~wxBitmapRefData(void)
{
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
  
wxBitmap::wxBitmap( int width, int height, int depth )
{
  m_refData = new wxBitmapRefData();
  M_BMPDATA->m_mask = NULL;
  M_BMPDATA->m_width = width;
  M_BMPDATA->m_height = height;
  M_BMPDATA->m_bpp = depth;
   
  if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);
};

wxBitmap::wxBitmap( char **WXUNUSED(bits) )
{
  m_refData = new wxBitmapRefData();

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

wxBitmap::wxBitmap( const wxString &filename, int type )
{
  LoadFile( filename, type );
};

wxBitmap::wxBitmap( const char WXUNUSED(bits)[], int width, int height, int WXUNUSED(depth))
{
  m_refData = new wxBitmapRefData();

  M_BMPDATA->m_mask = NULL;
  M_BMPDATA->m_width = width;
  M_BMPDATA->m_height = height;
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

void wxBitmap::SetHeight( int height )
{
  if (!Ok()) return;
  M_BMPDATA->m_height = height;
};

void wxBitmap::SetWidth( int width )
{
  if (!Ok()) return;
  M_BMPDATA->m_width = width;
};

void wxBitmap::SetDepth( int depth )
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

void wxBitmap::Resize( int WXUNUSED(height), int WXUNUSED(width) )
{
  if (!Ok()) return;
  
};

bool wxBitmap::SaveFile( const wxString &WXUNUSED(name), int WXUNUSED(type), 
  wxPalette *WXUNUSED(palette) )
{
  return FALSE;
};

bool wxBitmap::LoadFile( const wxString &WXUNUSED(name), int WXUNUSED(type) )
{
  return FALSE;
};
        
wxPalette *wxBitmap::GetPalette(void) const
{
  if (!Ok()) return NULL;
  return M_BMPDATA->m_palette;
};

