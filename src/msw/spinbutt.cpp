/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "spinbutt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Can't resolve reference to CreateUpDownControl in
// TWIN32, but could probably use normal CreateWindow instead.

#if wxUSE_SPINBTN

#if defined(__WIN95__) && !defined(__TWIN32__)

#include "wx/spinbutt.h"
#include "wx/msw/private.h"

#if !defined(__GNUWIN32__) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
    #include <commctrl.h>
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
    IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent);
#endif

// ----------------------------------------------------------------------------
// wxSpinButton
// ----------------------------------------------------------------------------

bool wxSpinButton::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    // basic initialization
    InitBase();

    m_windowId = (id == -1) ? NewControlId() : id;

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    SetName(name);

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    m_windowStyle = style;

    SetParent(parent);

    // get the right size for the control
    if ( width <= 0 || height <= 0 )
    {
        wxSize size = DoGetBestSize();
        if ( width <= 0 )
            width = size.x;
        if ( height <= 0 )
            height = size.y;
    }

    if ( x < 0 )
        x = 0;
    if ( y < 0 )
        y = 0;

    // translate the styles
    DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP |
                   UDS_SETBUDDYINT; // it doesn't harm if we don't have buddy

    if ( m_windowStyle & wxSP_HORIZONTAL )
        wstyle |= UDS_HORZ;
    if ( m_windowStyle & wxSP_ARROW_KEYS )
        wstyle |= UDS_ARROWKEYS;
    if ( m_windowStyle & wxSP_WRAP )
        wstyle |= UDS_WRAP;

    // create the UpDown control.
    m_hWnd = (WXHWND)CreateUpDownControl
                     (
                       wstyle,
                       x, y, width, height,
                       GetHwndOf(parent),
                       m_windowId,
                       wxGetInstance(),
                       NULL, // no buddy
                       m_max, m_min,
                       m_min // initial position
                     );

    if ( !m_hWnd )
    {
        wxLogLastError("CreateUpDownControl");

        return FALSE;
    }

    if ( parent )
    {
        parent->AddChild(this);
    }

    SubclassWin(m_hWnd);

    return TRUE;
}

wxSpinButton::~wxSpinButton()
{
}

// ----------------------------------------------------------------------------
// size calculation
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestSize() const
{
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
}

// ----------------------------------------------------------------------------
// Attributes
// ----------------------------------------------------------------------------

int wxSpinButton::GetValue() const
{
    return (short)LOWORD(::SendMessage(GetHwnd(), UDM_GETPOS, 0, 0));
}

void wxSpinButton::SetValue(int val)
{
    ::SendMessage(GetHwnd(), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) val, 0));
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    wxSpinButtonBase::SetRange(minVal, maxVal);
    ::SendMessage(GetHwnd(), UDM_SETRANGE, 0,
                   (LPARAM) MAKELONG((short)maxVal, (short)minVal));
}

bool wxSpinButton::MSWOnScroll(int orientation, WXWORD wParam,
                               WXWORD pos, WXHWND control)
{
    wxCHECK_MSG( control, FALSE, wxT("scrolling what?") )

    if ( wParam != SB_THUMBPOSITION )
    {
        // probable SB_ENDSCROLL - we don't react to it
        return FALSE;
    }

    wxSpinEvent event(wxEVT_SCROLL_THUMBTRACK, m_windowId);
    event.SetPosition((short)pos);    // cast is important for negative values!
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxSpinButton::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
#ifndef __GNUWIN32__
#if defined(__BORLANDC__) || defined(__WATCOMC__)
    LPNM_UPDOWN lpnmud = (LPNM_UPDOWN)lParam;
#elif defined(__VISUALC__) && (__VISUALC__ >= 1000) && (__VISUALC__ < 1020)
    LPNM_UPDOWN lpnmud = (LPNM_UPDOWN)lParam;
#else
    LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;
#endif

    wxSpinEvent event(lpnmud->iDelta > 0 ? wxEVT_SCROLL_LINEUP
                                         : wxEVT_SCROLL_LINEDOWN,
                      m_windowId);
    event.SetPosition(lpnmud->iPos + lpnmud->iDelta);
    event.SetEventObject(this);

    bool processed = GetEventHandler()->ProcessEvent(event);

    *result = event.IsAllowed() ? 0 : 1;

    return processed;
#else // GnuWin32
    return FALSE;
#endif
}

bool wxSpinButton::MSWCommand(WXUINT cmd, WXWORD id)
{
    // No command messages
    return FALSE;
}

#endif // __WIN95__

#endif
    // wxUSE_SPINCTN

