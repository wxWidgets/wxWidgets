/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#ifdef __GNUG__
#pragma implementation
#pragma implementation "dcclient.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include "wx/dcclient.h"
#include "wx/log.h"

#include <windows.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxDC)
#endif

wxClientDC::wxClientDC(void)
{
  m_canvas = NULL;
}

wxClientDC::wxClientDC(wxWindow *the_canvas)
{
  m_canvas = the_canvas;
//  BeginDrawing();
  m_hDC = (WXHDC) ::GetDC((HWND) the_canvas->GetHWND());
}

wxClientDC::~wxClientDC(void)
{
//  EndDrawing();

  if (m_canvas && (HDC) m_hDC)
  {
    SelectOldObjects(m_hDC);

    ::ReleaseDC((HWND) m_canvas->GetHWND(), (HDC) m_hDC);
	m_hDC = 0;
  }
}

wxWindowDC::wxWindowDC(void)
{
  m_canvas = NULL;
}

wxWindowDC::wxWindowDC(wxWindow *the_canvas)
{
  m_canvas = the_canvas;
//  m_hDC = (WXHDC) ::GetDCEx((HWND) the_canvas->GetHWND(), NULL, DCX_WINDOW);
  m_hDC = (WXHDC) ::GetWindowDC((HWND) the_canvas->GetHWND() );
  m_hDCCount ++;
}

wxWindowDC::~wxWindowDC(void)
{
  if (m_canvas && m_hDC)
  {
    SelectOldObjects(m_hDC);

    ::ReleaseDC((HWND) m_canvas->GetHWND(), (HDC) m_hDC);
	m_hDC = 0;
  }
  m_hDCCount --;
}

wxPaintDC::wxPaintDC(void)
{
  m_canvas = NULL;
}

static PAINTSTRUCT g_paintStruct;

// Don't call Begin/EndPaint if it's already been called:
// for example, if calling a base class OnPaint.

WXHDC wxPaintDC::ms_PaintHDC = 0;
uint  wxPaintDC::ms_PaintCount = 0; // count of ms_PaintHDC usage

wxPaintDC::wxPaintDC(wxWindow *canvas)
{
  wxCHECK_RET( canvas, "NULL canvas in wxPaintDC ctor" );

  m_canvas = canvas;
  if ( ms_PaintCount > 0 ) {
    // it means that we've already called BeginPaint and so we must just
    // reuse the same HDC (BeginPaint shouldn't be called more than once)
    wxASSERT( ms_PaintHDC );

    m_hDC = ms_PaintHDC;
    ms_PaintCount++;
  }
  else {
    ms_PaintHDC =
    m_hDC = (WXHDC)::BeginPaint((HWND)m_canvas->GetHWND(), &g_paintStruct);
    ms_PaintCount = 1;
    m_hDCCount++;
  }
}

wxPaintDC::~wxPaintDC()
{
  if ( m_hDC ) {
    if ( !--ms_PaintCount ) {
      ::EndPaint((HWND)m_canvas->GetHWND(), &g_paintStruct);
      m_hDCCount--;
      m_hDC = NULL;
      ms_PaintHDC = NULL;
    }
    //else: ms_PaintHDC still in use
  }
}

