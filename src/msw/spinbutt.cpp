/////////////////////////////////////////////////////////////////////////////
// Name:        msw/spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "spinbutt.h"
    #pragma implementation "spinbutbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/app.h"
#endif

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent)

#if defined(__WIN95__)

#include "wx/msw/private.h"

#if defined(__WIN95__) && !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)

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
    DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | /*  WS_CLIPSIBLINGS | */
                   UDS_NOTHOUSANDS | // never useful, sometimes harmful
                   UDS_SETBUDDYINT;  // it doesn't harm if we don't have buddy

    if ( m_windowStyle & wxCLIP_SIBLINGS )
        wstyle |= WS_CLIPSIBLINGS;
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
        wxLogLastError(wxT("CreateUpDownControl"));

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
#ifdef UDM_GETPOS32
    if ( wxTheApp->GetComCtl32Version() >= 580 )
    {
        // use the full 32 bit range if available
        return ::SendMessage(GetHwnd(), UDM_GETPOS32, 0, 0);
    }
#endif // UDM_GETPOS32

    // we're limited to 16 bit
    return (short)LOWORD(::SendMessage(GetHwnd(), UDM_GETPOS, 0, 0));
}

void wxSpinButton::SetValue(int val)
{
    // wxSpinButtonBase::SetValue(val); -- no, it is pure virtual

#ifdef UDM_SETPOS32
    if ( wxTheApp->GetComCtl32Version() >= 580 )
    {
        // use the full 32 bit range if available
        ::SendMessage(GetHwnd(), UDM_SETPOS32, 0, val);
    }
    else // we're limited to 16 bit
#endif // UDM_SETPOS32
    {
        ::SendMessage(GetHwnd(), UDM_SETPOS, 0, MAKELONG((short) val, 0));
    }
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    wxSpinButtonBase::SetRange(minVal, maxVal);

#ifdef UDM_SETRANGE32
    if ( wxTheApp->GetComCtl32Version() >= 471 )
    {
        // use the full 32 bit range if available
        ::SendMessage(GetHwnd(), UDM_SETRANGE32, minVal, maxVal);
    }
    else // we're limited to 16 bit
#endif // UDM_SETRANGE32
    {
        ::SendMessage(GetHwnd(), UDM_SETRANGE, 0,
                      (LPARAM) MAKELONG((short)maxVal, (short)minVal));
    }
}

bool wxSpinButton::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
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

bool wxSpinButton::MSWOnNotify(int WXUNUSED(idCtrl), WXLPARAM lParam, WXLPARAM *result)
{
    NM_UPDOWN *lpnmud = (NM_UPDOWN *)lParam;

    if (lpnmud->hdr.hwndFrom != GetHwnd()) // make sure it is the right control
        return FALSE;

    wxSpinEvent event(lpnmud->iDelta > 0 ? wxEVT_SCROLL_LINEUP
                                         : wxEVT_SCROLL_LINEDOWN,
                      m_windowId);
    event.SetPosition(lpnmud->iPos + lpnmud->iDelta);
    event.SetEventObject(this);

    bool processed = GetEventHandler()->ProcessEvent(event);

    *result = event.IsAllowed() ? 0 : 1;

    return processed;
}

bool wxSpinButton::MSWCommand(WXUINT WXUNUSED(cmd), WXWORD WXUNUSED(id))
{
    // No command messages
    return FALSE;
}

#endif // __WIN95__

#endif
    // wxUSE_SPINCTN

