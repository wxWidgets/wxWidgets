/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2002 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/dcmemory.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxDC)

wxMemoryDC::wxMemoryDC(void)
{
  m_ok = false;
};

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
{
  m_ok = false;
};

wxMemoryDC::~wxMemoryDC(void)
{
}

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
}

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
}

