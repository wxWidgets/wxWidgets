/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/utils.h"
#endif

#include "wx/os2/private.h"

#include "wx/dcmemory.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)
#endif

/*
 * Memory DC
 *
 */

wxMemoryDC::wxMemoryDC(void)
{
  // TODO:
/*
  m_hDC = (WXHDC) ::CreateCompatibleDC((HDC) NULL);
  m_ok = (m_hDC != 0);
  m_bOwnsDC = TRUE;

  SetBrush(*wxWHITE_BRUSH);
  SetPen(*wxBLACK_PEN);

  // the background mode is only used for text background
  // and is set in DrawText() to OPAQUE as required, other-
  // wise always TRANSPARENT, RR
  ::SetBkMode( GetHdc(), TRANSPARENT );
*/
}

wxMemoryDC::wxMemoryDC(wxDC *old_dc)
{
  // TODO:
/*
  old_dc->BeginDrawing();

  m_hDC = (WXHDC) ::CreateCompatibleDC((HDC) old_dc->GetHDC());
  m_ok = (m_hDC != 0);

  old_dc->EndDrawing();

  SetBrush(*wxWHITE_BRUSH);
  SetPen(*wxBLACK_PEN);

  // the background mode is only used for text background
  // and is set in DrawText() to OPAQUE as required, other-
  // wise always TRANSPARENT, RR
  ::SetBkMode( GetHdc(), TRANSPARENT );
*/
}

wxMemoryDC::~wxMemoryDC(void)
{
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
   // TODO:
};

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
  if (!m_selectedBitmap.Ok())
  {
    *width = 0; *height = 0;
    return;
  }
  *width = m_selectedBitmap.GetWidth();
  *height = m_selectedBitmap.GetHeight();
};


