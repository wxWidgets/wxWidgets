/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcmemory.h"
#endif

#include "wx/dcmemory.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxPaintDC)

wxMemoryDC::wxMemoryDC(void)
{
  m_ok = FALSE;
};

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
{
  m_ok = FALSE;
};

wxMemoryDC::~wxMemoryDC(void)
{
	if ( m_selected.Ok() )
	{
		wxBitmapRefData * bmap = (wxBitmapRefData*) (m_selected.GetRefData()) ;
		UnlockPixels( GetGWorldPixMap(  (CGrafPtr) bmap->m_hBitmap ) ) ;
	}
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
	if ( m_selected.Ok() )
	{
		wxBitmapRefData * bmap = (wxBitmapRefData*) (m_selected.GetRefData()) ;
		UnlockPixels( GetGWorldPixMap(  (CGrafPtr) bmap->m_hBitmap ) ) ;
	}
  m_selected = bitmap;
  if (m_selected.Ok())
  {
		wxBitmapRefData * bmap = (wxBitmapRefData*) (m_selected.GetRefData()) ;
		if ( bmap->m_hBitmap )
		{
			m_macPort = (GrafPtr) bmap->m_hBitmap ;
			LockPixels( GetGWorldPixMap(  (CGrafPtr)  m_macPort ) ) ;
			wxMask * mask = bitmap.GetMask() ;
			if ( mask )
			{
				m_macMask = mask->GetMaskBitmap() ;
			}
			MacSetupPort() ;
 			m_ok = TRUE ;
			// SetBackground(wxBrush(*wxWHITE, wxSOLID));
 		}
 		else
 		{
	    m_ok = FALSE;
 		}
  }
  else
  {
    m_ok = FALSE;
  };
};

void wxMemoryDC::GetSize( int *width, int *height ) const
{
  if (m_selected.Ok())
  {
    if (width) (*width) = m_selected.GetWidth();
    if (height) (*height) = m_selected.GetHeight();
  }
  else
  {
    if (width) (*width) = 0;
    if (height) (*height) = 0;
  };
};


