/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dcclient.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/log.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
    IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
    IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static PAINTSTRUCT g_paintStruct;

#ifdef __WXDEBUG__
    // a global variable which we check to verify that wxPaintDC are only
    // created in resopnse to WM_PAINT message - doing this from elsewhere is a
    // common programming error among wxWindows programmers and might lead to
    // very subtle and difficult to debug refresh/repaint bugs.
    extern bool g_isPainting = FALSE;
#endif // __WXDEBUG__

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxWindowDC
// ----------------------------------------------------------------------------

wxWindowDC::wxWindowDC()
{
  m_canvas = NULL;
}

wxWindowDC::wxWindowDC(wxWindow *the_canvas)
{
  m_canvas = the_canvas;
  m_hDC = (WXHDC) ::GetWindowDC(GetWinHwnd(the_canvas) );
  m_hDCCount++;

  SetBackground(wxBrush(m_canvas->GetBackgroundColour(), wxSOLID));
}

wxWindowDC::~wxWindowDC()
{
  if (m_canvas && m_hDC)
  {
    SelectOldObjects(m_hDC);

    ::ReleaseDC(GetWinHwnd(m_canvas), GetHdc());
    m_hDC = 0;
  }

  m_hDCCount--;
}

// ----------------------------------------------------------------------------
// wxClientDC
// ----------------------------------------------------------------------------

wxClientDC::wxClientDC()
{
  m_canvas = NULL;
}

wxClientDC::wxClientDC(wxWindow *the_canvas)
{
  m_canvas = the_canvas;
  m_hDC = (WXHDC) ::GetDC(GetWinHwnd(the_canvas));

  SetBackground(wxBrush(m_canvas->GetBackgroundColour(), wxSOLID));
}

wxClientDC::~wxClientDC()
{
  if ( m_canvas && GetHdc() )
  {
    SelectOldObjects(m_hDC);

    ::ReleaseDC(GetWinHwnd(m_canvas), GetHdc());
    m_hDC = 0;
  }
}

// ----------------------------------------------------------------------------
// wxPaintDC
// ----------------------------------------------------------------------------

// TODO (VZ) I have still some doubts about this hack and I still think that we
//           should store pairs of (hwnd, hdc) and not just the DC - what if
//           BeginPaint() was called on other window? It seems to work like
//           this, but to be sure about it we'd need to store hwnd too...

WXHDC  wxPaintDC::ms_PaintHDC = 0;
size_t wxPaintDC::ms_PaintCount = 0; // count of ms_PaintHDC usage

wxPaintDC::wxPaintDC()
{
  m_canvas = NULL;
}

wxPaintDC::wxPaintDC(wxWindow *canvas)
{
  wxCHECK_RET( canvas, "NULL canvas in wxPaintDC ctor" );
  wxCHECK_RET( g_isPainting,
               _T("wxPaintDC may be created only in EVT_PAINT handler!") );

  m_canvas = canvas;

  // Don't call Begin/EndPaint if it's already been called: for example, if
  // calling a base class OnPaint.
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

  SetBackground(wxBrush(m_canvas->GetBackgroundColour(), wxSOLID));
}

wxPaintDC::~wxPaintDC()
{
  if ( m_hDC ) {
    if ( !--ms_PaintCount ) {
      ::EndPaint((HWND)m_canvas->GetHWND(), &g_paintStruct);
      m_hDCCount--;
      m_hDC = (WXHDC) NULL;
      ms_PaintHDC = (WXHDC) NULL;
    }
    //else: ms_PaintHDC still in use
  }
}

