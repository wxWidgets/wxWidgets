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

#if defined(__WIN95__) && (!defined(__GNUWIN32_OLD__) || defined(__MINGW32__))
    #include <commctrl.h>
#endif

#ifndef _WIN32_IE
    // minimal set of features by default
    #define _WIN32_IE 0x0200
#endif

// VZ: normally, the trick with subclassing the tooltip control and processing
//     TTM_WINDOWFROMPOINT should work but, somehow, it doesn't. I leave the
//     code here for now (but it's not compiled) in case we need it later.
//
//     For now I use an ugly workaround and process TTN_NEEDTEXT directly in
//     radio button wnd proc - fixing TTM_WINDOWFROMPOINT code would be nice
//     because it would then work for all controls, not only radioboxes but for
//     now I don't understand what's wrong with it...
#define wxUSE_TTM_WINDOWFROMPOINT   0

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// the tooltip parent window
WXHWND wxToolTip::ms_hwndTT = (WXHWND)NULL;

#if wxUSE_TTM_WINDOWFROMPOINT

// the tooltip window proc
static WNDPROC gs_wndprocToolTip = (WNDPROC)NULL;

#endif // wxUSE_TTM_WINDOWFROMPOINT

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// a wrapper around TOOLINFO Win32 structure
#ifdef __VISUALC__
    #pragma warning( disable : 4097 ) // we inherit from a typedef - so what?
#endif

class wxToolInfo : public TOOLINFO
{
public:
    wxToolInfo(HWND hwnd)
    {
        // initialize all members
        ::ZeroMemory(this, sizeof(TOOLINFO));

        // the structure TOOLINFO has been extended with a 4 byte field in
        // version 4.70 of comctl32.dll and if we compile on a newer machine
        // but run on one with the old version of comctl32, nothing will work
        // because the library will detect that we rely on a more recent
        // version of it. So we always use the old size - if we ever start
        // using our lParam member, we'd have to check for comctl32 version
        // during run-time
#if _WIN32_IE >= 0x0300
        cbSize = sizeof(TOOLINFO) - sizeof(LPARAM);
#else // old headers
        cbSize = sizeof(TOOLINFO);
#endif // compile-time comctl32.dll version

        uFlags = TTF_IDISHWND;
        uId = (UINT)hwnd;
    }
};

#ifdef __VISUALC__
    #pragma warning( default : 4097 )
#endif

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
    (void)SendTooltipMessage((WXHWND)hwnd, msg, wParam, (void *)lParam);
}

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_TTM_WINDOWFROMPOINT

// ----------------------------------------------------------------------------
// window proc for our tooltip control
// ----------------------------------------------------------------------------

LRESULT APIENTRY wxToolTipWndProc(HWND hwndTT,
                                  UINT msg,
                                  WPARAM wParam,
                                  LPARAM lParam)
{
    if ( msg == TTM_WINDOWFROMPOINT )
    {
        LPPOINT ppt = (LPPOINT)lParam;

        // the window on which event occured
        HWND hwnd = ::WindowFromPoint(*ppt);

        OutputDebugString("TTM_WINDOWFROMPOINT: ");
        OutputDebugString(wxString::Format("0x%08x => ", hwnd));

        // return a HWND corresponding to a wxWindow because only wxWindows are
        // associated with tooltips using TTM_ADDTOOL
        wxWindow *win = wxGetWindowFromHWND((WXHWND)hwnd);

        if ( win )
        {
            hwnd = GetHwndOf(win);
            OutputDebugString(wxString::Format("0x%08x\r\n", hwnd));

#if 0
            // modify the point too!
            RECT rect;
            GetWindowRect(hwnd, &rect);

            ppt->x = (rect.right - rect.left) / 2;
            ppt->y = (rect.bottom - rect.top) / 2;
#endif // 0
            return (LRESULT)hwnd;
        }
        else
        {
            OutputDebugString("no window\r\n");
        }
    }

    return ::CallWindowProc(CASTWNDPROC gs_wndprocToolTip, hwndTT, msg, wParam, lParam);
}

#endif // wxUSE_TTM_WINDOWFROMPOINT

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------

void wxToolTip::Enable(bool flag)
{
    SendTooltipMessageToAll(ms_hwndTT, TTM_ACTIVATE, flag, 0);
}

void wxToolTip::SetDelay(long milliseconds)
{
    SendTooltipMessageToAll(ms_hwndTT, TTM_SETDELAYTIME,
                            TTDT_INITIAL, milliseconds);
}

// ---------------------------------------------------------------------------
// implementation helpers
// ---------------------------------------------------------------------------

// create the tooltip ctrl for our parent frame if it doesn't exist yet
WXHWND wxToolTip::GetToolTipCtrl()
{
    if ( !ms_hwndTT )
    {
        ms_hwndTT = (WXHWND)::CreateWindow(TOOLTIPS_CLASS,
                                           (LPCTSTR)NULL,
                                           TTS_ALWAYSTIP,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           NULL, (HMENU)NULL,
                                           wxGetInstance(),
                                           NULL);
       if ( ms_hwndTT )
       {
           HWND hwnd = (HWND)ms_hwndTT;
           SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

#if wxUSE_TTM_WINDOWFROMPOINT
           // subclass the newly created control
           gs_wndprocToolTip = (WNDPROC)::GetWindowLong(hwnd, GWL_WNDPROC);
           ::SetWindowLong(hwnd, GWL_WNDPROC, (long)wxToolTipWndProc);
#endif // wxUSE_TTM_WINDOWFROMPOINT
       }
    }

    return ms_hwndTT;
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
        wxToolInfo ti(GetHwndOf(m_window));
        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_DELTOOL, 0, &ti);
    }
}

void wxToolTip::Add(WXHWND hWnd)
{
    HWND hwnd = (HWND)hWnd;

    wxToolInfo ti(hwnd);

    // as we store our text anyhow, it seems useless to waste system memory
    // by asking the tooltip ctrl to remember it too - instead it will send
    // us TTN_NEEDTEXT (via WM_NOTIFY) when it is about to be shown
    ti.hwnd = hwnd;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    // instead of: ti.lpszText = (char *)m_text.c_str();

    if ( !SendTooltipMessage(GetToolTipCtrl(), TTM_ADDTOOL, 0, &ti) )
    {
        wxLogDebug(_T("Failed to create the tooltip '%s'"), m_text.c_str());
    }
    else
    {
        // check for multiline toopltip
        int index = m_text.Find(_T('\n'));

        if ( index != wxNOT_FOUND )
        {
#if _WIN32_IE >= 0x0300
            if ( wxTheApp->GetComCtl32Version() >= 470 )
            {
                // use TTM_SETMAXWIDTH to make tooltip multiline using the
                // extent of its first line as max value
                HFONT hfont = (HFONT)SendTooltipMessage(GetToolTipCtrl(),
                                                        WM_GETFONT,
                                                        0, 0);
                if ( !hfont )
                {
                    hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
                    if ( !hfont )
                    {
                        wxLogLastError(wxT("GetStockObject(DEFAULT_GUI_FONT)"));
                    }
                }

                HDC hdc = CreateCompatibleDC(NULL);
                if ( !hdc )
                {
                    wxLogLastError(wxT("CreateCompatibleDC(NULL)"));
                }

                if ( !SelectObject(hdc, hfont) )
                {
                    wxLogLastError(wxT("SelectObject(hfont)"));
                }

                SIZE sz;
                if ( !GetTextExtentPoint(hdc, m_text, index, &sz) )
                {
                    wxLogLastError(wxT("GetTextExtentPoint"));
                }

                DeleteDC(hdc);

                SendTooltipMessage(GetToolTipCtrl(), TTM_SETMAXTIPWIDTH,
                                   0, (void *)sz.cx);
            }
#endif // comctl32.dll >= 4.70

            // replace the '\n's with spaces because otherwise they appear as
            // unprintable characters in the tooltip string
            m_text.Replace(_T("\n"), _T(" "));
        }
    }
}

void wxToolTip::SetWindow(wxWindow *win)
{
    Remove();

    m_window = win;

    // add the window itself
    if ( m_window )
    {
        Add(m_window->GetHWND());
    }

    // and all of its subcontrols (e.g. radiobuttons in a radiobox) as well
    wxControl *control = wxDynamicCast(m_window, wxControl);
    if ( control )
    {
        const wxArrayLong& subcontrols = control->GetSubcontrols();
        size_t count = subcontrols.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            int id = subcontrols[n];
            HWND hwnd = GetDlgItem(GetHwndOf(m_window), id);
            if ( !hwnd )
            {
                // may be it's a child of parent of the control, in fact?
                // (radiobuttons are subcontrols, i.e. children of the radiobox
                // for wxWindows but are its siblings at Windows level)
                hwnd = GetDlgItem(GetHwndOf(m_window->GetParent()), id);
            }

            // must have it by now!
            wxASSERT_MSG( hwnd, _T("no hwnd for subcontrol?") );

            Add((WXHWND)hwnd);
        }
    }

    // VZ: it's ugly to do it here, but I don't want any major changes right
    //     now, later we will probably want to have wxWindow::OnGotToolTip() or
    //     something like this where the derived class can do such things
    //     itself instead of wxToolTip "knowing" about them all
    wxComboBox *combo = wxDynamicCast(control, wxComboBox);
    if ( combo )
    {
        WXHWND hwndComboEdit = combo->GetEditHWND();
        if ( hwndComboEdit )
        {
            Add(hwndComboEdit);
        }
        //else: it's ok for a combo to be read only, of course
    }
}

void wxToolTip::SetTip(const wxString& tip)
{
    m_text = tip;

    if ( m_window )
    {
        // update it immediately
        wxToolInfo ti(GetHwndOf(m_window));
        ti.lpszText = (wxChar *)m_text.c_str();

        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, 0, &ti);
    }
}

#endif // wxUSE_TOOLTIPS
