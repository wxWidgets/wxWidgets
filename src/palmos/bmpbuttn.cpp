/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcmemory.h"
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define BUTTON_HEIGHT_FACTOR (EDIT_CONTROL_FACTOR * 1.1)

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id,
    const wxBitmap& bitmap,
    const wxPoint& pos,
    const wxSize& size, long style,
    const wxValidator& wxVALIDATOR_PARAM(validator),
    const wxString& name)
{
    return false;
}

#define FOCUS_MARGIN 3

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top,
    int right, int bottom, bool sel )
{
}

void wxBitmapButton::DrawButtonFocus( WXHDC dc, int left, int top, int right,
    int bottom, bool WXUNUSED(sel) )
{
}

void wxBitmapButton::DrawButtonDisable( WXHDC dc, int left, int top, int right,
    int bottom, bool with_marg )
{
}

wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize best;
    best.x = 0;
    best.y = 0;

    return best;
}

#endif // wxUSE_BMPBUTTON
