/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
   #include "wx/string.h"
   #include "wx/window.h"
   #include "wx/msw/private.h"
#endif

#include "wx/dcscreen.h"


#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)
#endif

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC(void)
{
  m_hDC = (WXHDC) ::GetDC((HWND) NULL);
  m_hDCCount ++;

  // the background mode is only used for text background
  // and is set in DrawText() to OPAQUE as required, other-
  // wise always TRANSPARENT, RR
  ::SetBkMode( GetHdc(), TRANSPARENT );
}

wxScreenDC::~wxScreenDC(void)
{
  SelectOldObjects(m_hDC);
  ::ReleaseDC((HWND) NULL, (HDC) m_hDC);
  m_hDC = 0;
  m_hDCCount --;
}

