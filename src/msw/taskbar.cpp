/////////////////////////////////////////////////////////////////////////
// File:        taskbar.cpp
// Purpose:     Implements wxTaskBarIcon class for manipulating icons on
//              the Windows task bar.
// Author:      Julian Smart
// Modified by:
// Created:     24/3/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "taskbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/window.h"
#include "wx/frame.h"
#include "wx/utils.h"
#include "wx/menu.h"
#endif

#if defined(__WIN95__)

#include "wx/msw/private.h"
#include "wx/msw/winundef.h"

#include <string.h>
#include "wx/taskbar.h"

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#endif

#ifdef __WXWINCE__
    #include <winreg.h>
    #include <shellapi.h>
#endif

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxTaskBarIconList);

LRESULT APIENTRY _EXPORT
wxTaskBarIconWindowProc( HWND hWnd, unsigned msg, UINT wParam, LONG lParam );

wxChar *wxTaskBarWindowClass = (wxChar*) wxT("wxTaskBarWindowClass");

wxTaskBarIconList wxTaskBarIcon::sm_taskBarIcons;

// initialized on demand
UINT   gs_msgTaskbar = 0;
UINT   gs_msgRestartTaskbar = 0;

#if WXWIN_COMPATIBILITY_2_4
BEGIN_EVENT_TABLE(wxTaskBarIcon, wxEvtHandler)
    EVT_TASKBAR_MOVE         (wxTaskBarIcon::_OnMouseMove)
    EVT_TASKBAR_LEFT_DOWN    (wxTaskBarIcon::_OnLButtonDown)
    EVT_TASKBAR_LEFT_UP      (wxTaskBarIcon::_OnLButtonUp)
    EVT_TASKBAR_RIGHT_DOWN   (wxTaskBarIcon::_OnRButtonDown)
    EVT_TASKBAR_RIGHT_UP     (wxTaskBarIcon::_OnRButtonUp)
    EVT_TASKBAR_LEFT_DCLICK  (wxTaskBarIcon::_OnLButtonDClick)
    EVT_TASKBAR_RIGHT_DCLICK (wxTaskBarIcon::_OnRButtonDClick)
END_EVENT_TABLE()
#endif


IMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// NotifyIconData: wrapper around NOTIFYICONDATA
// ----------------------------------------------------------------------------

struct NotifyIconData : public NOTIFYICONDATA
{
    NotifyIconData(WXHWND hwnd)
    {
        memset(this, 0, sizeof(NOTIFYICONDATA));
        cbSize = sizeof(NOTIFYICONDATA);
        hWnd = (HWND) hwnd;
        uCallbackMessage = gs_msgTaskbar;
        uFlags = NIF_MESSAGE;

        // we use the same id for all taskbar icons as we don't need it to
        // distinguish between them
        uID = 99;
    }
};

// ----------------------------------------------------------------------------
// wxTaskBarIcon
// ----------------------------------------------------------------------------

wxTaskBarIcon::wxTaskBarIcon()
{
    m_hWnd = 0;
    m_iconAdded = false;

    AddObject(this);

    if (RegisterWindowClass())
        m_hWnd = CreateTaskBarWindow();
}

wxTaskBarIcon::~wxTaskBarIcon()
{
    RemoveObject(this);

    if (m_iconAdded)
    {
        RemoveIcon();
    }

    if (m_hWnd)
    {
        ::DestroyWindow((HWND) m_hWnd);
        m_hWnd = 0;
    }
}

// Operations
bool wxTaskBarIcon::SetIcon(const wxIcon& icon, const wxString& tooltip)
{
    if (!IsOk())
        return false;

    m_icon = icon;
    m_strTooltip = tooltip;

    NotifyIconData notifyData(m_hWnd);

    if (icon.Ok())
    {
        notifyData.uFlags |= NIF_ICON;
        notifyData.hIcon = GetHiconOf(icon);
    }

    if ( !tooltip.empty() )
    {
        notifyData.uFlags |= NIF_TIP;
//        lstrcpyn(notifyData.szTip, tooltip.c_str(), WXSIZEOF(notifyData.szTip));
        wxStrncpy(notifyData.szTip, tooltip.c_str(), WXSIZEOF(notifyData.szTip));
    }

    bool ok = Shell_NotifyIcon(m_iconAdded ? NIM_MODIFY
                                           : NIM_ADD, &notifyData) != 0;

    if ( !m_iconAdded && ok )
        m_iconAdded = true;

    return ok;
}

bool wxTaskBarIcon::RemoveIcon()
{
    if (!m_iconAdded)
        return false;

    m_iconAdded = false;

    NotifyIconData notifyData(m_hWnd);

    return Shell_NotifyIcon(NIM_DELETE, &notifyData) != 0;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
    static bool s_inPopup = false;

    if (s_inPopup)
        return false;

    s_inPopup = true;

    bool        rval = false;
    wxWindow*   win;
    int         x, y;
    wxGetMousePosition(&x, &y);

    // is wxFrame the best window type to use???
    win = new wxFrame(NULL, -1, wxEmptyString, wxPoint(x,y), wxSize(-1,-1), 0);
    win->PushEventHandler(this);

    // Remove from record of top-level windows, or will confuse wxWindows
    // if we try to exit right now.
    wxTopLevelWindows.DeleteObject(win);

    menu->UpdateUI();

    // Work around a WIN32 bug
    ::SetForegroundWindow ((HWND) win->GetHWND ());

    rval = win->PopupMenu(menu, 0, 0);

    // Work around a WIN32 bug
    ::PostMessage ((HWND) win->GetHWND(),WM_NULL,0,0L);

    win->PopEventHandler(false);
    win->Destroy();
    delete win;

    s_inPopup = false;

    return rval;
}

#if WXWIN_COMPATIBILITY_2_4
// Overridables
void wxTaskBarIcon::OnMouseMove(wxEvent&)         {}
void wxTaskBarIcon::OnLButtonDown(wxEvent&)       {}
void wxTaskBarIcon::OnLButtonUp(wxEvent&)         {}
void wxTaskBarIcon::OnRButtonDown(wxEvent&)       {}
void wxTaskBarIcon::OnRButtonUp(wxEvent&)         {}
void wxTaskBarIcon::OnLButtonDClick(wxEvent&)     {}
void wxTaskBarIcon::OnRButtonDClick(wxEvent&)     {}

void wxTaskBarIcon::_OnMouseMove(wxEvent& e)      { OnMouseMove(e);     }
void wxTaskBarIcon::_OnLButtonDown(wxEvent& e)    { OnLButtonDown(e);   }
void wxTaskBarIcon::_OnLButtonUp(wxEvent& e)      { OnLButtonUp(e);     }
void wxTaskBarIcon::_OnRButtonDown(wxEvent& e)    { OnRButtonDown(e);   }
void wxTaskBarIcon::_OnRButtonUp(wxEvent& e)      { OnRButtonUp(e);     }
void wxTaskBarIcon::_OnLButtonDClick(wxEvent& e)  { OnLButtonDClick(e); }
void wxTaskBarIcon::_OnRButtonDClick(wxEvent& e)  { OnRButtonDClick(e); }
#endif

wxTaskBarIcon* wxTaskBarIcon::FindObjectForHWND(WXHWND hWnd)
{
    wxTaskBarIconList::compatibility_iterator node = sm_taskBarIcons.GetFirst();
    while (node)
    {
        wxTaskBarIcon *obj = node->GetData();
        if (obj->GetHWND() == hWnd)
            return obj;
        node = node->GetNext();
    }
    return NULL;
}

void wxTaskBarIcon::AddObject(wxTaskBarIcon* obj)
{
    sm_taskBarIcons.Append(obj);
}

void wxTaskBarIcon::RemoveObject(wxTaskBarIcon* obj)
{
    sm_taskBarIcons.DeleteObject(obj);
}

bool wxTaskBarIcon::RegisterWindowClass()
{
    static bool s_registered = false;

    if ( s_registered )
        return true;

    // Taskbar restart msg will be sent to us if the icon needs to be redrawn
    gs_msgRestartTaskbar = RegisterWindowMessage(wxT("TaskbarCreated"));

    // Also register the taskbar message here
    gs_msgTaskbar = ::RegisterWindowMessage(wxT("wxTaskBarIconMessage"));

    // set up and register window class
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) wxTaskBarIconWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = wxGetInstance();
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = wxTaskBarWindowClass;

    if ( !::RegisterClass(&wc) )
    {
        wxLogLastError(_T("RegisterClass(taskbar icon)"));

        return false;
    }

    s_registered = true;

    return true;
}

WXHWND wxTaskBarIcon::CreateTaskBarWindow()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    HWND hWnd = CreateWindowEx (0, wxTaskBarWindowClass,
            wxT("wxTaskBarWindow"),
            WS_OVERLAPPED,
            0,
            0,
            10,
            10,
            NULL,
            (HMENU) 0,
            hInstance,
            NULL);

    return (WXHWND) hWnd;
}

// ----------------------------------------------------------------------------
// wxTaskBarIcon window proc
// ----------------------------------------------------------------------------

long wxTaskBarIcon::WindowProc(WXHWND hWnd,
                               unsigned int msg,
                               unsigned int wParam,
                               long lParam)
{
    wxEventType eventType = 0;

    if (msg == gs_msgRestartTaskbar)   // does the icon need to be redrawn?
    {
        m_iconAdded = false;
        SetIcon(m_icon, m_strTooltip);
    }

    if (msg != gs_msgTaskbar)
        return DefWindowProc((HWND) hWnd, msg, wParam, lParam);

    switch (lParam)
    {
        case WM_LBUTTONDOWN:
            eventType = wxEVT_TASKBAR_LEFT_DOWN;
            break;

        case WM_LBUTTONUP:
            eventType = wxEVT_TASKBAR_LEFT_UP;
            break;

        case WM_RBUTTONDOWN:
            eventType = wxEVT_TASKBAR_RIGHT_DOWN;
            break;

        case WM_RBUTTONUP:
            eventType = wxEVT_TASKBAR_RIGHT_UP;
            break;

        case WM_LBUTTONDBLCLK:
            eventType = wxEVT_TASKBAR_LEFT_DCLICK;
            break;

        case WM_RBUTTONDBLCLK:
            eventType = wxEVT_TASKBAR_RIGHT_DCLICK;
            break;

        case WM_MOUSEMOVE:
            eventType = wxEVT_TASKBAR_MOVE;
            break;

        default:
            break;
    }

    if (eventType)
    {
        wxTaskBarIconEvent event(eventType, this);

        ProcessEvent(event);
    }

    return 0;
}

LRESULT APIENTRY _EXPORT
wxTaskBarIconWindowProc(HWND hWnd, unsigned msg, UINT wParam, LONG lParam)
{
    wxTaskBarIcon *obj = wxTaskBarIcon::FindObjectForHWND((WXHWND) hWnd);
    if (obj)
        return obj->WindowProc((WXHWND) hWnd, msg, wParam, lParam);
    else
        return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif
    // __WIN95__
