/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/dcclient.cpp
// Purpose:     wxClientDC class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif

// ----------------------------------------------------------------------------
// array/list types
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxPaintDCInfo
{
    wxPaintDCInfo(wxWindow *win, wxDC *dc)
    {
        handle = win->GetWinHandle();
        hdc    = dc->GetHDC();
        count  = 1;
    }

    WXWINHANDLE handle;       // window for this DC
    WXHDC       hdc;        // the DC handle
    size_t     count;      // usage count
};

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxArrayDCInfo);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)
IMPLEMENT_CLASS(wxPaintDCEx, wxPaintDC)

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__
    int g_isPainting = 0;
#endif // __WXDEBUG__

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxWindowDC
// ----------------------------------------------------------------------------

wxWindowDC::wxWindowDC()
{
}

wxWindowDC::wxWindowDC(wxWindow *canvas)
{
}

void wxWindowDC::InitDC()
{
}

void wxWindowDC::DoGetSize(int *width, int *height) const
{
}

// ----------------------------------------------------------------------------
// wxClientDC
// ----------------------------------------------------------------------------

wxClientDC::wxClientDC()
{
}

wxClientDC::wxClientDC(wxWindow *canvas)
{
}

void wxClientDC::InitDC()
{
}

wxClientDC::~wxClientDC()
{
}

void wxClientDC::DoGetSize(int *width, int *height) const
{
}

// ----------------------------------------------------------------------------
// wxPaintDC
// ----------------------------------------------------------------------------

wxArrayDCInfo wxPaintDC::ms_cache;

wxPaintDC::wxPaintDC()
{
}

wxPaintDC::wxPaintDC(wxWindow *canvas)
{
}

wxPaintDC::~wxPaintDC()
{
}

wxPaintDCInfo *wxPaintDC::FindInCache(size_t *index) const
{
    return NULL;
}

WXHDC wxPaintDC::FindDCInCache(wxWindow* win)
{
    return 0;
}

/*
 * wxPaintDCEx
 */

wxPaintDCEx::wxPaintDCEx(wxWindow *canvas, WXHDC dc) : saveState(0)
{
}

wxPaintDCEx::~wxPaintDCEx()
{
}
