/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
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
  m_cmap = gdk_colormap_get_system();
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
{
  m_ok = FALSE;
  m_cmap = gdk_colormap_get_system();
}

wxMemoryDC::~wxMemoryDC(void)
{
}

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
  m_selected = bitmap;
  if (m_selected.Ok())
  {
    if (m_selected.GetPixmap())
    {
      m_window = m_selected.GetPixmap();
    }
    else
    {
      m_window = m_selected.GetBitmap();
    }
    
    SetUpDC();
    
    m_isDrawable = FALSE;
  }
  else
  {
    m_ok = FALSE;
    m_window = (GdkWindow *) NULL;
  }
}

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
  }
}


