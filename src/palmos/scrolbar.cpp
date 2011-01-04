/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/scrolbar.cpp
// Purpose:     wxScrollBar
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

#if wxUSE_SCROLLBAR

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/settings.h"
#endif

#include "wx/palmos/private.h"

// Scrollbar
bool wxScrollBar::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& wxVALIDATOR_PARAM(validator),
           const wxString& name)
{
    return false;
}

wxScrollBar::~wxScrollBar(void)
{
}

bool wxScrollBar::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
                              WXWORD pos, WXHWND WXUNUSED(control))
{
    return false;
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
}

int wxScrollBar::GetThumbPosition(void) const
{
}

void wxScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize,
    bool refresh)
{
}


void wxScrollBar::Command(wxCommandEvent& event)
{
}

wxSize wxScrollBar::DoGetBestSize() const
{
    return wxSize(0,0);
}

#endif // wxUSE_SCROLLBAR
