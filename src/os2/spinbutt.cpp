/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "spinbutt.h"
    #pragma implementation "spinbutbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#if wxUSE_SPINBTN

// Can't resolve reference to CreateUpDownControl in
// TWIN32, but could probably use normal CreateWindow instead.


#include "wx/spinbutt.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent)

#include "wx/os2/private.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)

bool wxSpinButton::Create(
  wxWindow*                         parent
, wxWindowID                        id
, const wxPoint&                    pos
, const wxSize&                     size
, long                              style
, const wxString&                   name
)
{
    SetName(name);

    m_windowStyle = style;

    SetParent(parent);

    m_windowId = (id == -1) ? NewControlId() : id;

    // TODO create spin button
    return FALSE;
}

wxSpinButton::~wxSpinButton()
{
}

// ----------------------------------------------------------------------------
// size calculation
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestSize() const
{
    // TODO:
/*
    if ( (GetWindowStyle() & wxSP_VERTICAL) != 0 )
    {
        // vertical control
        return wxSize(GetSystemMetrics(SM_CXVSCROLL),
                      2*GetSystemMetrics(SM_CYVSCROLL));
    }
    else
    {
        // horizontal control
        return wxSize(2*GetSystemMetrics(SM_CXHSCROLL),
                      GetSystemMetrics(SM_CYHSCROLL));
    }
*/
    return wxSize(0, 0);
}

// ----------------------------------------------------------------------------
// Attributes
// ----------------------------------------------------------------------------

int wxSpinButton::GetValue() const
{
    // TODO
    return 0;
}

void wxSpinButton::SetValue(int val)
{
    // TODO
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    // TODO
}

bool wxSpinButton::OS2OnScroll(int orientation, WXWORD wParam,
                               WXWORD pos, WXHWND control)
{
    wxCHECK_MSG( control, FALSE, wxT("scrolling what?") )
// TODO:
/*
    if ( wParam != SB_THUMBPOSITION )
    {
        // probable SB_ENDSCROLL - we don't react to it
        return FALSE;
    }

    wxSpinEvent event(wxEVT_SCROLL_THUMBTRACK, m_windowId);
    event.SetPosition((short)pos);    // cast is important for negative values!
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
*/
    return FALSE;
}

bool wxSpinButton::OS2OnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    // TODO:
/*
    LPNM_UPDOWN lpnmud = (LPNM_UPDOWN)lParam;

    wxSpinEvent event(lpnmud->iDelta > 0 ? wxEVT_SCROLL_LINEUP
                                         : wxEVT_SCROLL_LINEDOWN,
                      m_windowId);
    event.SetPosition(lpnmud->iPos + lpnmud->iDelta);
    event.SetEventObject(this);

    bool processed = GetEventHandler()->ProcessEvent(event);

    *result = event.IsAllowed() ? 0 : 1;

    return processed;
*/
    return FALSE;
}

bool wxSpinButton::OS2Command(WXUINT cmd, WXWORD id)
{
    // No command messages
    return FALSE;
}

#endif //wxUSE_SPINBTN
