/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcmemory.h"
#endif

#include "wx/dcmemory.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxWindowDC)

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
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
  m_bitmap = bitmap;
  if (m_bitmap.Ok())
  {
    m_pixmap = m_bitmap.GetPixmap();
  }
  else
  {
    m_ok = FALSE;
  };
};

void wxMemoryDC::GetSize( int *width, int *height ) const
{
  if (m_bitmap.Ok())
  {
    if (width) (*width) = m_bitmap.GetWidth();
    if (height) (*height) = m_bitmap.GetHeight();
  }
  else
  {
    if (width) (*width) = 0;
    if (height) (*height) = 0;
  };
};


