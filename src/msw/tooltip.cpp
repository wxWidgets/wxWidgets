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

// send a message to all existing tooltip controls
static void SendTooltipMessageToAll(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // NB: it might be somewhat easier to maintain a list of all existing
    //     wxToolTip controls (put them there in ctor, delete from the list
    //     in dtor) - may be it's worth doing it this way? OTOH, typical
    //     application won't have many top level windows, so iterating over all
    //     of them shouldnt' take much time neither...

    // iterate over all top level windows and send message to the tooltip
    // control of each and every of them (or more precisely to all dialogs and
    // frames)
    wxDialog *dialog = NULL;
    wxFrame *frame = NULL;

    wxNode *node = wxTopLevelWindows.First();
    while ( node )
    {
        wxWindow *win = (wxWindow *)node->Data();

        node = node->Next();

        if ( win->IsKindOf(CLASSINFO(wxFrame)) )
        {
            frame = (wxFrame *)win;
            dialog = NULL;
        }
        else if ( win->IsKindOf(CLASSINFO(wxDialog)) )
        {
            dialog = (wxDialog *)win;
            frame = NULL;
        }
        else
        {
            // skip this strange top level window
            continue;
        }

        wxASSERT_MSG( dialog || frame, "logic error" );

        WXHWND hwndTT = frame ? frame->GetToolTipCtrl()
                              : dialog->GetToolTipCtrl();
        if ( hwndTT )
        {
            (void)SendTooltipMessage(hwndTT, msg, wParam, (void *)lParam);
        }
    }
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------

void wxToolTip::Enable(bool flag)
{
    SendTooltipMessageToAll(TTM_ACTIVATE, flag, 0);
}

void wxToolTip::SetDelay(long milliseconds)
{
    SendTooltipMessageToAll(TTM_SETDELAYTIME, TTDT_INITIAL, milliseconds);
}

// ---------------------------------------------------------------------------
// implementation helpers
// ---------------------------------------------------------------------------

// create the tooltip ctrl for our parent frame if it doesn't exist yet
WXHWND wxToolTip::GetToolTipCtrl()
{
    // find either parent dialog or parent frame - tooltip controls are managed
    // by these 2 classes only (it doesn't make sense to create one tooltip per
    // each and every wxWindow)
    wxFrame *frame = NULL;
    wxDialog *dialog = NULL;

    wxWindow *parent = m_window;
    while ( parent )
    {
        if ( parent->IsKindOf(CLASSINFO(wxFrame)) )
        {
            frame = (wxFrame *)parent;

            break;
        }
        else if ( parent->IsKindOf(CLASSINFO(wxDialog)) )
        {
            dialog = (wxDialog *)parent;

            break;
        }

        parent = parent->GetParent();
    }

    wxCHECK_MSG( frame || dialog, 0,
                 "can't create tooltip control outside a frame or a dialog" );

    HWND hwndTT = (HWND)(frame ? frame->GetToolTipCtrl()
                                 : dialog->GetToolTipCtrl());
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
            if ( frame )
                frame->SetToolTipCtrl((WXHWND)hwndTT);
            else
                dialog->SetToolTipCtrl((WXHWND)hwndTT);
        }
        else
        {
            wxLogSysError(_("Can not create tooltip control"));
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
        ti.lpszText = (char *)m_text.c_str();

        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, 0, &ti);
    }
}

#endif // wxUSE_TOOLTIPS
