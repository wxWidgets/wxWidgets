/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcmemory.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/utils.h"
#endif

#include "wx/dcmemory.h"

#include <windows.h>
#include "wx/msw/winundef.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)
#endif

/*
 * Memory DC
 *
 */

wxMemoryDC::wxMemoryDC(void)
{
  m_hDC = (WXHDC) ::CreateCompatibleDC((HDC) NULL);
  m_ok = (m_hDC != 0);
  m_bOwnsDC = TRUE;

  SetBrush(*wxWHITE_BRUSH);
  SetPen(*wxBLACK_PEN);
}

wxMemoryDC::wxMemoryDC(wxDC *old_dc)
{
  old_dc->BeginDrawing();

  m_hDC = (WXHDC) ::CreateCompatibleDC((HDC) old_dc->GetHDC());
  m_ok = (m_hDC != 0);

  old_dc->EndDrawing();

  SetBrush(*wxWHITE_BRUSH);
  SetPen(*wxBLACK_PEN);
}

wxMemoryDC::~wxMemoryDC(void)
{
}

void wxMemoryDC::SelectObject(const wxBitmap& bitmap)
{
  // Select old bitmap out of the device context
  if (m_oldBitmap)
  {
    ::SelectObject((HDC) m_hDC, (HBITMAP) m_oldBitmap);
    if (m_selectedBitmap.Ok())
    {
      m_selectedBitmap.SetSelectedInto(NULL);
      m_selectedBitmap = wxNullBitmap;
    }
  }

  // Do own check for whether the bitmap is already selected into
  // a device context
  if (bitmap.GetSelectedInto() && (bitmap.GetSelectedInto() != this))
  {
    wxFatalError(_T("Error in wxMemoryDC::SelectObject\nBitmap is selected in another wxMemoryDC.\nDelete the first wxMemoryDC or use SelectObject(NULL)"));
    return;
  }

  // Check if the bitmap has the correct depth for this device context
//  if (bitmap.Ok() && (bitmap.GetDepth() != GetDepth()))
  // JACS 11/12/98: disabling this since the Forty Thieves sample
  // shows this not working properly. In fact, if loading from a resource,
  // the depth should become the screen depth, so why was it being called?
//  if (0)
//  {
//      // Make a new bitmap that has the correct depth.
//      wxBitmap newBitmap = bitmap.GetBitmapForDC(* this);
//
//      m_selectedBitmap = newBitmap ;
//  }
//  else
//  {
      m_selectedBitmap = bitmap;
//  }

  if (!m_selectedBitmap.Ok())
    return;

  m_selectedBitmap.SetSelectedInto(this);
  HBITMAP bm = (HBITMAP) ::SelectObject((HDC) m_hDC, (HBITMAP) m_selectedBitmap.GetHBITMAP());

  if (bm == ERROR)
  {
    wxFatalError(_T("Error in wxMemoryDC::SelectObject\nBitmap may not be loaded, or may be selected in another wxMemoryDC.\nDelete the first wxMemoryDC to deselect bitmap."));
  }
  else if (!m_oldBitmap)
    m_oldBitmap = (WXHBITMAP) bm;
}

void wxMemoryDC::DoGetSize(int *width, int *height) const
{
  if (!m_selectedBitmap.Ok())
  {
    *width = 0; *height = 0;
    return;
  }
  *width = m_selectedBitmap.GetWidth();
  *height = m_selectedBitmap.GetHeight();
}

