///////////////////////////////////////////////////////////////////////////////
// Name:        msw/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// stubs for missing functions under MicroWindows
// ----------------------------------------------------------------------------

#ifdef __WXMICROWIN__

static inline bool IsIconic(HWND WXUNUSED(hwnd)) { return FALSE; }
static inline bool IsZoomed(HWND WXUNUSED(hwnd)) { return FALSE; }

#endif // __WXMICROWIN__

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
wxWindowList wxModelessWindows;

// ============================================================================
// wxTopLevelWindowMSW implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;
}

bool wxTopLevelWindowMSW::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // init our fields
    Init();

    m_windowStyle = style;

    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    return TRUE;
}

wxTopLevelWindowMSW::~wxTopLevelWindowMSW()
{
    wxTopLevelWindows.DeleteObject(this);

    if ( wxModelessWindows.Find(this) )
        wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if ( wxTheApp && (wxTopLevelWindows.Number() == 0) )
    {
        wxTheApp->SetTopWindow(NULL);

        if ( wxTheApp->GetExitOnFrameDelete() )
        {
            ::PostQuitMessage(0);
        }
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW geometry
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::DoSetClientSize(int width, int height)
{
    HWND hWnd = GetHwnd();

    RECT rectClient;
    ::GetClientRect(hWnd, &rectClient);

    RECT rectTotal;
    ::GetWindowRect(hWnd, &rectTotal);

    // Find the difference between the entire window (title bar and all)
    // and the client area; add this to the new client size to move the
    // window
    width += rectTotal.right - rectTotal.left - rectClient.right;
    height += rectTotal.bottom - rectTotal.top - rectClient.bottom;

    // note that calling GetClientAreaOrigin() takes the toolbar into account
    wxPoint pt = GetClientAreaOrigin();
    width += pt.x;
    height += pt.y;

    if ( !::MoveWindow(hWnd, rectTotal.left, rectTotal.top,
                       width, height, TRUE /* redraw */) )
    {
        wxLogLastError(_T("MoveWindow"));
    }

    wxSizeEvent event(wxSize(width, height), m_windowId);
    event.SetEventObject(this);
    (void)GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW showing
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::DoShowWindow(int nShowCmd)
{
    ::ShowWindow(GetHwnd(), nShowCmd);

    m_iconized = nShowCmd == SW_MINIMIZE;
}

bool wxTopLevelWindowMSW::Show(bool show)
{
    // don't use wxWindow version as we want to call DoShowWindow() ourselves
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    int nShowCmd;
    if ( show )
    {
        if ( m_maximizeOnShow )
        {
            // show and maximize
            nShowCmd = SW_MAXIMIZE;

            m_maximizeOnShow = FALSE;
        }
        else // just show
        {
            nShowCmd = SW_SHOW;
        }
    }
    else // hide
    {
        nShowCmd = SW_HIDE;
    }

    DoShowWindow(nShowCmd);

    if ( show )
    {
        ::BringWindowToTop(GetHwnd());

        wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
    }
    else // hide
    {
        // Try to highlight the correct window (the parent)
        if ( GetParent() )
        {
            HWND hWndParent = GetHwndOf(GetParent());
            if (hWndParent)
                ::BringWindowToTop(hWndParent);
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::Maximize(bool maximize)
{
    if ( IsShown() )
    {
        // just maximize it directly
        DoShowWindow(maximize ? SW_MAXIMIZE : SW_RESTORE);
    }
    else // hidden
    {
        // we can't maximize the hidden frame because it shows it as well, so
        // just remember that we should do it later in this case
        m_maximizeOnShow = TRUE;
    }
}

bool wxTopLevelWindowMSW::IsMaximized() const
{
    return ::IsZoomed(GetHwnd()) != 0;
}

void wxTopLevelWindowMSW::Iconize(bool iconize)
{
    DoShowWindow(iconize ? SW_MINIMIZE : SW_RESTORE);
}

bool wxTopLevelWindowMSW::IsIconized() const
{
    // also update the current state
    ((wxTopLevelWindowMSW *)this)->m_iconized = ::IsIconic(GetHwnd()) != 0;

    return m_iconized;
}

void wxTopLevelWindowMSW::Restore()
{
    DoShowWindow(SW_RESTORE);
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowMSW::SetIcon(const wxIcon& icon)
{
    // this sets m_icon
    wxTopLevelWindowBase::SetIcon(icon);

#if defined(__WIN95__) && !defined(__WXMICROWIN__)
    if ( m_icon.Ok() )
    {
        ::SendMessage(GetHwnd(), WM_SETICON,
                      (WPARAM)TRUE, (LPARAM)GetHiconOf(m_icon));
    }
#endif // __WIN95__
}
