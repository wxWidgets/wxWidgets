///////////////////////////////////////////////////////////////////////////////
// Name:        msw/tooltip.cpp
// Purpose:     wxToolTip class implementation for MSW
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_TOOLTIPS

#include "wx/tooltip.h"
#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// the tooltip parent window
WXHWND wxToolTip::hwndTT = (WXHWND)NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------


// a simple wrapper around TOOLINFO Win32 structure
class wxToolInfo // define a TOOLINFO for OS/2 here : public TOOLINFO
{
public:
    wxToolInfo(wxWindow *win)
    {
        // initialize all members
//        ::ZeroMemory(this, sizeof(TOOLINFO));

        cbSize = sizeof(this);
        uFlags = 0; // TTF_IDISHWND;
        uId = (UINT)win->GetHWND();
    }
    size_t        cbSize;
    ULONG         uFlags;
    UINT          uId;
    HWND          hwnd;
    char*         lpszText;
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// send a message to the tooltip control
inline MRESULT SendTooltipMessage(WXHWND hwnd,
                                  UINT msg,
                                  MPARAM wParam,
                                  MPARAM lParam)
{
//    return hwnd ? ::SendMessage((HWND)hwnd, msg, wParam, (MPARAM)lParam)
//                : 0;
    return (MRESULT)0;
}

// send a message to all existing tooltip controls
static void SendTooltipMessageToAll(WXHWND hwnd,
                                    UINT msg,
                                    MPARAM wParam,
                                    MPARAM lParam)
{
   if ( hwnd )
     (void)SendTooltipMessage((WXHWND)hwnd, msg, wParam, lParam);
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------

void wxToolTip::Enable(bool flag)
{
//    SendTooltipMessageToAll((WXHWND)hwndTT,TTM_ACTIVATE, flag, 0);
}

void wxToolTip::SetDelay(long milliseconds)
{
//    SendTooltipMessageToAll((WXHWND)hwndTT,TTM_SETDELAYTIME, TTDT_INITIAL, milliseconds);
}

// ---------------------------------------------------------------------------
// implementation helpers
// ---------------------------------------------------------------------------

// create the tooltip ctrl for our parent frame if it doesn't exist yet
WXHWND wxToolTip::GetToolTipCtrl()
{
// TODO:
/*
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
*/
    return (WXHWND)0;
}

void wxToolTip::RelayEvent(WXMSG *msg)
{
//  (void)SendTooltipMessage(GetToolTipCtrl(), TTM_RELAYEVENT, 0, msg);
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
//      (void)SendTooltipMessage(GetToolTipCtrl(), TTM_DELTOOL, 0, &ti);
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
//        ti.lpszText = LPSTR_TEXTCALLBACK;
        // instead of: ti.lpszText = (char *)m_text.c_str();

// TODO:
/*
        if ( !SendTooltipMessage(GetToolTipCtrl(), TTM_ADDTOOL, 0, &ti) )
        {
            wxLogSysError(_("Failed to create the tooltip '%s'"),
                          m_text.c_str());
        }
*/
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

//      (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, 0, &ti);
    }
}

#endif // wxUSE_TOOLTIPS
