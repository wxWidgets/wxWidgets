///////////////////////////////////////////////////////////////////////////////
// Name:        msw/tooltip.cpp
// Purpose:     wxToolTip class implementation for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.01.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/tooltip.h"
#include "wx/msw/private.h"

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__)
#include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// a simple wrapper around TOOLINFO Win32 structure
class wxToolInfo : public TOOLINFO
{
public:
    wxToolInfo(wxWindow *win)
    {
        // initialize all members
        ::ZeroMemory(this, sizeof(TOOLINFO));

        cbSize = sizeof(TOOLINFO);
        uFlags = TTF_IDISHWND;
        uId = (UINT)win->GetHWND();
    }
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// send a message to the tooltip control
inline LRESULT SendTooltipMessage(WXHWND hwnd,
                                  UINT msg,
                                  WPARAM wParam,
                                  void *lParam)
{
    return hwnd ? ::SendMessage((HWND)hwnd, msg, wParam, (LPARAM)lParam)
                : 0;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// "semiglobal" functions - these methods work with the tooltip control which
// is shared among all the wxToolTips of the same frame
// ----------------------------------------------------------------------------

// create the tooltip ctrl for our parent frame if it doesn't exist yet
WXHWND wxToolTip::GetToolTipCtrl()
{
    wxWindow *parent = m_window;
    while ( parent && !parent->IsKindOf(CLASSINFO(wxFrame)) )
    {
        parent = parent->GetParent();
    }

    wxCHECK_MSG( parent, 0, "can't create tooltip control outside a frame" );

    wxFrame *frame = (wxFrame *)parent;
    HWND hwndTT = (HWND)frame->GetToolTipCtrl();
    if ( !hwndTT )
    {
        hwndTT = ::CreateWindow(TOOLTIPS_CLASS,
                                (LPSTR)NULL,
                                TTS_ALWAYSTIP,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                (HWND)frame->GetHWND(), (HMENU)NULL,
                                wxGetInstance(), NULL);

        if ( hwndTT )
        {
            frame->SetToolTipCtrl((WXHWND)hwndTT);
        }
        else
        {
            wxLogSysError(_("Can not create tooltip control"));
        }
    }

    return (WXHWND)hwndTT;
}

void wxToolTip::Enable(bool flag)
{
    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_ACTIVATE, flag, 0);
}

void wxToolTip::RelayEvent(WXMSG *msg)
{
    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_RELAYEVENT, 0, msg);
}

void wxToolTip::SetDelay(long milliseconds)
{
    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_SETDELAYTIME,
                             TTDT_INITIAL, (void *)milliseconds);
}

// ----------------------------------------------------------------------------
// ctor & dtor
// ----------------------------------------------------------------------------

wxToolTip::wxToolTip(const wxString &tip)
         : m_text(tip)
{
    m_window = NULL;
}

wxToolTip::~wxToolTip()
{
    // there is no need to Remove() this tool - it will be done automatically
    // anyhow
}

// ----------------------------------------------------------------------------
// others
// ----------------------------------------------------------------------------

void wxToolTip::Remove()
{
    // remove this tool from the tooltip control
    if ( m_window )
    {
        wxToolInfo ti(m_window);
        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_DELTOOL, 0, &ti);
    }
}

void wxToolTip::SetWindow(wxWindow *win)
{
    Remove();

    m_window = win;

    if ( m_window )
    {
        wxToolInfo ti(m_window);

        // as we store our text anyhow, it seems useless to waste system memory
        // by asking the tooltip ctrl to remember it too - instead it will send
        // us TTN_NEEDTEXT (via WM_NOTIFY) when it is about to be shown
        ti.hwnd = (HWND)m_window->GetHWND();
        ti.lpszText = LPSTR_TEXTCALLBACK;
        // instead of: ti.lpszText = (char *)m_text.c_str();

        if ( !SendTooltipMessage(GetToolTipCtrl(), TTM_ADDTOOL, 0, &ti) )
        {
            wxLogSysError(_("Failed to create the tooltip '%s'"),
                          m_text.c_str());
        }
    }
}

void wxToolTip::SetTip(const wxString& tip)
{
    m_text = tip;

    if ( m_window )
    {
        // update it immediately
        wxToolInfo ti(m_window);
        ti.lpszText = (char *)m_text.c_str();

        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, 0, &ti);
    }
}
