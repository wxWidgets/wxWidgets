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

#if wxUSE_TOOLTIPS

#include "wx/tooltip.h"
#include "wx/msw/private.h"

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
#include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// the tooltip parent window
WXHWND wxToolTip::hwndTT = NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------


// a simple wrapper around TOOLINFO Win32 structure
#pragma warning( disable : 4097 )
class wxToolInfo : public TOOLINFO
{
public:
    wxToolInfo(wxWindow *win)
    {
        // initialize all members
#if __GNUWIN32__ && !defined(wxUSE_NORLANDER_HEADERS)
        memset(this, 0, sizeof(TOOLINFO));
#else
        ::ZeroMemory(this, sizeof(TOOLINFO));
#endif

        cbSize = sizeof(TOOLINFO);
        uFlags = TTF_IDISHWND;
        uId = (UINT)win->GetHWND();
    }
};
#pragma warning( default : 4097 )

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

// send a message to all existing tooltip controls
static void SendTooltipMessageToAll(WXHWND hwnd,
                                    UINT msg,
                                    WPARAM wParam,
                                    LPARAM lParam)
{
   if ( hwnd )
     (void)SendTooltipMessage((WXHWND)hwnd, msg, wParam, (void *)lParam);
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------



void wxToolTip::Enable(bool flag)
{
    SendTooltipMessageToAll((WXHWND)hwndTT,TTM_ACTIVATE, flag, 0);
}

void wxToolTip::SetDelay(long milliseconds)
{
    SendTooltipMessageToAll((WXHWND)hwndTT,TTM_SETDELAYTIME, TTDT_INITIAL, milliseconds);
}

// ---------------------------------------------------------------------------
// implementation helpers
// ---------------------------------------------------------------------------

// create the tooltip ctrl for our parent frame if it doesn't exist yet
WXHWND wxToolTip::GetToolTipCtrl()
{
    if ( !hwndTT )
    {
        hwndTT = (WXHWND)::CreateWindow(TOOLTIPS_CLASS,
                                (LPSTR)NULL,
                                TTS_ALWAYSTIP,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                NULL, (HMENU)NULL,
                                wxGetInstance(),
                                NULL);
       if ( hwndTT )
       {
           SetWindowPos((HWND)hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
       }

    }

    return (WXHWND)hwndTT;
}

void wxToolTip::RelayEvent(WXMSG *msg)
{
    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_RELAYEVENT, 0, msg);
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
        ti.lpszText = (wxChar *)m_text.c_str();

        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, 0, &ti);
    }
}

#endif // wxUSE_TOOLTIPS
