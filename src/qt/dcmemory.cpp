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

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)

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
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
  m_selected = bitmap;
  if (m_selected.Ok())
  {
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


