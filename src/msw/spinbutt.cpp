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

#if defined(__WIN95__) && !defined(__TWIN32__)

#include "wx/spinbutt.h"
#include "wx/msw/private.h"

#if !defined(__GNUWIN32__) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
    #include <commctrl.h>
#endif

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
    IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent);
#endif

bool wxSpinButton::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
  wxSystemSettings settings;
  m_backgroundColour = parent->GetBackgroundColour() ;
  m_foregroundColour = parent->GetForegroundColour() ;

  SetName(name);

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  SetParent(parent);

  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  InitBase();

  m_windowId = (id == -1) ? NewControlId() : id;

  DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP;

  if ( m_windowStyle & wxSP_HORIZONTAL )
    wstyle |= UDS_HORZ;
  if ( m_windowStyle & wxSP_ARROW_KEYS )
    wstyle |= UDS_ARROWKEYS;
  if ( m_windowStyle & wxSP_WRAP )
    wstyle |= UDS_WRAP;

  // Create the ListView control.
  HWND hWndListControl = CreateUpDownControl(wstyle,
    x, y, width, height,
    (HWND) parent->GetHWND(),
    m_windowId,
    wxGetInstance(),
    0,
    m_min, m_max, m_min);

  m_hWnd = (WXHWND) hWndListControl;
  if (parent) parent->AddChild(this);

  // TODO: have this for all controls.
  if ( !m_hWnd )
    return FALSE;

  SubclassWin((WXHWND) m_hWnd);

  return TRUE;
}

wxSpinButton::~wxSpinButton()
{
}

// Attributes
////////////////////////////////////////////////////////////////////////////

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
    wxCHECK_MSG( control, FALSE, _T("scrolling what?") )

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
    LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;

    wxSpinEvent event(lpnmud->iDelta > 0 ? wxEVT_SCROLL_LINEUP
                                         : wxEVT_SCROLL_LINEDOWN,
                      m_windowId);
    event.SetPosition(lpnmud->iPos + lpnmud->iDelta);
    event.SetEventObject(this);

    bool processed = GetEventHandler()->ProcessEvent(event);

    *result = event.IsAllowed() ? 0 : 1;

    return processed;
#else
    return FALSE;
#endif
}

bool wxSpinButton::MSWCommand(WXUINT cmd, WXWORD id)
{
    // No command messages
    return FALSE;
}

#endif // __WIN95__
