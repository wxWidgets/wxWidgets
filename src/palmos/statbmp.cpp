/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/statbmp.cpp
// Purpose:     wxStaticBitmap
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

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBMP

#include "wx/statbmp.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/window.h"
#endif

#include "wx/palmos/private.h"

// ---------------------------------------------------------------------------
// macors
// ---------------------------------------------------------------------------

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxStaticBitmap
// ---------------------------------------------------------------------------

static wxGDIImage* ConvertImage( const wxGDIImage& bitmap )
{
    return NULL;
}

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxGDIImage& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    return false;
}

wxBorder wxStaticBitmap::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

WXDWORD wxStaticBitmap::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    return 0;
}

bool wxStaticBitmap::ImageIsOk() const
{
    return false;
}

void wxStaticBitmap::Free()
{
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    return wxSize(0,0);
}

void wxStaticBitmap::SetImage( const wxGDIImage* image )
{
}

void wxStaticBitmap::SetImageNoCopy( wxGDIImage* image)
{
}

// We need this or the control can never be moved e.g. in Dialog Editor.
WXLRESULT wxStaticBitmap::MSWWindowProc(WXUINT nMsg,
                                   WXWPARAM wParam,
                                   WXLPARAM lParam)
{
    return false;
}

#endif // wxUSE_STATBMP
