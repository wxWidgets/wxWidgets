/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#endif

#include "wx/msw/private.h"

#include "wx/dcscreen.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
    m_hDC = (WXHDC) ::GetDC((HWND) NULL);

    // the background mode is only used for text background and is set in
    // DrawText() to OPAQUE as required, otherwise always TRANSPARENT
    ::SetBkMode( GetHdc(), TRANSPARENT );
}

void wxScreenDC::DoGetSize(int *width, int *height) const
{
    // skip wxWindowDC version because it doesn't work without a valid m_canvas
    // (which we don't have)
    wxDC::DoGetSize(width, height);
}

