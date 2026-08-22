///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/tooltip.cpp
// Purpose:     wxToolTip class implementation for MSW
// Author:      Vadim Zeitlin
// Created:     31.01.99
// Copyright:   (c) 1999 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_TOOLTIPS

#include "wx/tooltip.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/app.h"
    #include "wx/control.h"
    #include "wx/module.h"
    #include "wx/toplevel.h"
#endif

#include "wx/tokenzr.h"
#include "wx/vector.h"
#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/winui.h"
    #include "wx/winui/private/tlwhostmsw.h"
    #include <atomic>
    #define wxMSWWinUITooltipLog(...) wxWinUIDebugLog(__VA_ARGS__)
#else
    #define wxMSWWinUITooltipLog(...) ((void)0)
#endif

#ifndef TTTOOLINFO_V1_SIZE
    #define TTTOOLINFO_V1_SIZE 0x28
#endif

#ifndef TTF_TRANSPARENT
    #define TTF_TRANSPARENT 0x0100
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
WXHWND wxToolTip::ms_hwndTT = nullptr;

// Requested width for new logical tooltips. Keep zero as the public/system
// default sentinel: native TTM sizing computes its effective value locally.
int wxToolTip::ms_maxWidth = 0;

#if defined(__WXWINUI__) && wxUSE_WINUI3
namespace
{

std::atomic<unsigned long long> gs_nextWinUIToolTipIdentity{0};

unsigned long long wxNextWinUIToolTipIdentity()
{
    unsigned long long identity =
        gs_nextWinUIToolTipIdentity.fetch_add(
            1, std::memory_order_relaxed) + 1;
    if ( identity == 0 )
    {
        identity =
            gs_nextWinUIToolTipIdentity.fetch_add(
                1, std::memory_order_relaxed) + 1;
    }
    return identity;
}

} // anonymous namespace
#endif

#if wxUSE_TTM_WINDOWFROMPOINT

// the tooltip window proc
static WNDPROC gs_wndprocToolTip = nullptr;

#endif // wxUSE_TTM_WINDOWFROMPOINT

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// This is simply a wrapper for vector<HWND> but defined as a class to hide the
// details from the public header.
class wxToolTipOtherWindows : public wxVector<WXHWND>
{
};

// a wrapper around TOOLINFO Win32 structure
#ifdef __VISUALC__
    #pragma warning( disable : 4097 ) // we inherit from a typedef - so what?
#endif

class wxToolInfo : public TOOLINFO
{
public:
    wxToolInfo(HWND hwndOwner, unsigned int id, const wxRect& rc)
    {
        // initialize all members
        wxZeroMemory(*this);

        // the structure TOOLINFO has been extended with a 4 byte field in
        // version 4.70 of comctl32.dll and another one in 5.01 but we don't
        // use these extended fields so use the old struct size to ensure that
        // the tooltips work on old (Windows 95) systems too
        cbSize = TTTOOLINFO_V1_SIZE;

        hwnd = hwndOwner;

        if (rc.IsEmpty())
        {
            uFlags = TTF_IDISHWND;
            uId = (UINT_PTR)hwndOwner;
        }
        else
        {
            // this tooltip must be shown only if the mouse hovers a specific rect
            // of the hwnd parameter!
            rect.left = rc.GetLeft();
            rect.right = rc.GetRight();
            rect.top = rc.GetTop();
            rect.bottom = rc.GetBottom();

            // note that not setting TTF_IDISHWND from the uFlags member means that the
            // ti.uId field should not contain the HWND but rather as MSDN says an
            // "Application-defined identifier of the tool"; this is used internally by
            // Windows to distinguish the different tooltips attached to the same window
            uId = id;
        }

        // we use TTF_TRANSPARENT to fix a problem which arises at least with
        // the text controls but may presumably happen with other controls
        // which display the tooltip at mouse position: it can start flashing
        // then as the control gets "focus lost" events and dismisses the
        // tooltip which then reappears because mouse remains hovering over the
        // control, see SF patch 1821229
        uFlags |= TTF_TRANSPARENT;
        // we use TTF_SUBCLASS to avoid the need for the rest of the code
        // to handle all mouse move messages and relay them to wxToolTip
        // (see https://github.com/wxWidgets/wxWidgets/pull/24482)
        uFlags |= TTF_SUBCLASS;
    }
};

// Takes care of deleting ToolTip control window when shutting down the library.
class wxToolTipModule : public wxModule
{
public:
    wxToolTipModule()
    {
    }

    virtual bool OnInit() override
    {
        return true;
    }

    virtual void OnExit() override
    {
        wxToolTip::DeleteToolTipCtrl();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxToolTipModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxToolTipModule, wxModule);

#ifdef __VISUALC__
    #pragma warning( default : 4097 )
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// send a message to the tooltip control if it exists
//
// NB: wParam is always 0 for the TTM_XXX messages we use
static inline LRESULT SendTooltipMessage(WXHWND hwnd, UINT msg, void *lParam)
{
    return hwnd ? ::SendMessage((HWND)hwnd, msg, 0, (LPARAM)lParam) : 0;
}

// send a message to all existing tooltip controls
static inline void
SendTooltipMessageToAll(WXHWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( hwnd )
        ::SendMessage((HWND)hwnd, msg, wParam, lParam);
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

        // the window on which event occurred
        HWND hwnd = ::WindowFromPoint(*ppt);

        OutputDebugString("TTM_WINDOWFROMPOINT: ");
        OutputDebugString(wxString::Format("0x%08x => ", hwnd));

        // return a HWND corresponding to a wxWindow because only wxWidgets are
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
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // XAML dependency-property writes are re-entrant. If an application
    // callback issued a newer Enable() transaction, that nested call already
    // published the native state too and this stale outer call must stop.
    if ( !wxWinUISetToolTipsEnabled(flag) )
        return;
#endif

    // Make sure the tooltip has been created
    (void) GetToolTipCtrl();

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Creating the native tooltip control can also dispatch window messages.
    // Preserve last-writer-wins if one of them toggled the global policy.
    if ( wxWinUIAreToolTipsEnabled() != flag )
        return;
#endif

    SendTooltipMessageToAll(ms_hwndTT, TTM_ACTIVATE, flag, 0);
}

void wxToolTip::SetDelay(long milliseconds)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Do not partially emulate this for XAML slots. ToolTip::Opened fires
    // after presentation, so closing there and reopening from a timer causes
    // visible/UIA flicker. Replacing ToolTipService activation with pointer
    // handlers would also lose focus and long-press triggers and could not
    // preserve reshow semantics. Keep all three settings native-only until
    // WinUI exposes a cancellable opening/timing surface.
#endif

    // Make sure the tooltip has been created
    (void) GetToolTipCtrl();

    SendTooltipMessageToAll(ms_hwndTT, TTM_SETDELAYTIME,
                            TTDT_INITIAL, milliseconds);
}

void wxToolTip::SetAutoPop(long milliseconds)
{
    SendTooltipMessageToAll(ms_hwndTT, TTM_SETDELAYTIME,
                            TTDT_AUTOPOP, milliseconds);
}

void wxToolTip::SetReshow(long milliseconds)
{
    SendTooltipMessageToAll(ms_hwndTT, TTM_SETDELAYTIME,
                            TTDT_RESHOW, milliseconds);
}

void wxToolTip::SetMaxWidth(int width)
{
    wxASSERT_MSG( width == -1 || width >= 0, wxT("invalid width value") );

    ms_maxWidth = width;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxWinUISetToolTipMaxWidth(width);
#endif
}

void wxToolTip::DeleteToolTipCtrl()
{
    if ( ms_hwndTT )
    {
        ::DestroyWindow((HWND)ms_hwndTT);
        ms_hwndTT = nullptr;
    }
}

// ---------------------------------------------------------------------------
// implementation helpers
// ---------------------------------------------------------------------------

// create the tooltip ctrl for our parent frame if it doesn't exist yet
/* static */
WXHWND wxToolTip::GetToolTipCtrl()
{
    wxMSWWinUITooltipLog("wxToolTip::GetToolTipCtrl enter existing=%p",
                         static_cast<void *>(ms_hwndTT));
    if ( !ms_hwndTT )
    {
        WXDWORD exflags = 0;
        if ( wxApp::MSWGetDefaultLayout() == wxLayout_RightToLeft )
        {
            exflags |= WS_EX_LAYOUTRTL;
        }

        // we want to show the tooltips always (even when the window is not
        // active) and we don't want to strip "&"s from them
        ms_hwndTT = (WXHWND)::CreateWindowEx(exflags,
                                             TOOLTIPS_CLASS,
                                             nullptr,
                                             TTS_ALWAYSTIP | TTS_NOPREFIX,
                                             CW_USEDEFAULT, CW_USEDEFAULT,
                                             CW_USEDEFAULT, CW_USEDEFAULT,
                                             nullptr, nullptr,
                                              wxGetInstance(),
                                              nullptr);
        wxMSWWinUITooltipLog("wxToolTip::GetToolTipCtrl CreateWindowEx result=%p",
                             static_cast<void *>(ms_hwndTT));
       if ( ms_hwndTT )
       {
           HWND hwnd = (HWND)ms_hwndTT;
           wxMSWDarkMode::AllowForWindow(hwnd);
           SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

#if wxUSE_TTM_WINDOWFROMPOINT
           // subclass the newly created control
           gs_wndprocToolTip = wxSetWindowProc(hwnd, wxToolTipWndProc);
#endif // wxUSE_TTM_WINDOWFROMPOINT
       }
    }

    wxMSWWinUITooltipLog("wxToolTip::GetToolTipCtrl leave hwnd=%p",
                         static_cast<void *>(ms_hwndTT));
    return ms_hwndTT;
}

/* static */
void wxToolTip::UpdateVisibility()
{
    wxToolInfo ti(nullptr, 0, wxRect());
    ti.uFlags = 0;

    if ( !SendTooltipMessage(ms_hwndTT, TTM_GETCURRENTTOOL, &ti) )
        return;

    wxWindow* const associatedWindow = wxFindWinFromHandle(ti.hwnd);
    if ( !associatedWindow )
        return;

    bool hideTT = false;
    if ( !associatedWindow->IsShownOnScreen() )
    {
        // If the associated window or its parent is hidden, the tooltip
        // shouldn't remain shown.
        hideTT = true;
    }
    else
    {
        // Even if it's not hidden, it could also be iconized.
        wxTopLevelWindow* const
            frame = wxDynamicCast(wxGetTopLevelParent(associatedWindow), wxTopLevelWindow);

        if ( frame && frame->IsIconized() )
            hideTT = true;
    }

    if ( hideTT )
        ::ShowWindow(ms_hwndTT, SW_HIDE);
}

// ----------------------------------------------------------------------------
// ctor & dtor
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxToolTip, wxObject);

wxToolTip::wxToolTip(const wxString &tip)
         : m_text(tip)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    m_winuiMaxWidthAtCreation = ms_maxWidth;
    m_winuiIdentity = wxNextWinUIToolTipIdentity();
#endif
    wxMSWWinUITooltipLog("wxToolTip::ctor text this=%p tipLen=%lu",
                         static_cast<void *>(this),
                         static_cast<unsigned long>(tip.length()));
    m_window = nullptr;
    m_others = nullptr;

    // make sure m_rect.IsEmpty() == true
    m_rect.SetWidth(0);
    m_rect.SetHeight(0);

    // since m_rect is not valid, m_id is ignored by wxToolInfo ctor...
    m_id = 0;
}

wxToolTip::wxToolTip(wxWindow* win, unsigned int id, const wxString &tip, const wxRect& rc)
         : m_text(tip), m_rect(rc), m_id(id)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    m_winuiMaxWidthAtCreation = ms_maxWidth;
    m_winuiIdentity = wxNextWinUIToolTipIdentity();
#endif
    wxMSWWinUITooltipLog("wxToolTip::ctor window this=%p win=%p hwnd=%p id=%u tipLen=%lu",
                         static_cast<void *>(this),
                         static_cast<void *>(win),
                         win ? reinterpret_cast<void *>(win->GetHWND()) : nullptr,
                         id,
                         static_cast<unsigned long>(tip.length()));
    m_window = nullptr;
    m_others = nullptr;

    SetWindow(win);
}

wxToolTip::~wxToolTip()
{
    wxMSWWinUITooltipLog("wxToolTip::dtor enter this=%p window=%p",
                         static_cast<void *>(this),
                         static_cast<void *>(m_window));
    // the tooltip has to be removed before deleting. Otherwise, if it is visible
    // while being deleted, there will be a delay before it goes away.
    Remove();

    delete m_others;
    wxMSWWinUITooltipLog("wxToolTip::dtor leave this=%p",
                         static_cast<void *>(this));
}

// ----------------------------------------------------------------------------
// others
// ----------------------------------------------------------------------------

/* static */
void wxToolTip::Remove(WXHWND hWnd, unsigned int id, const wxRect& rc)
{
    wxToolInfo ti((HWND)hWnd, id, rc);

    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_DELTOOL, &ti);
}

void wxToolTip::DoRemove(WXHWND hWnd)
{
    if ( m_window && hWnd == m_window->GetHWND() )
    {
        // Remove the tooltip from the main window.
        Remove(hWnd, m_id, m_rect);
    }
    else
    {
        // Not really sure what to pass to remove in this case...
        Remove(hWnd, 0, wxRect());
    }
}

void wxToolTip::Remove()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( m_isWinUIHost )
        return;
#endif
    DoForAllWindows(&wxToolTip::DoRemove);
}

void wxToolTip::AddOtherWindow(WXHWND hWnd)
{
    if ( !m_others )
        m_others = new wxToolTipOtherWindows;

    m_others->push_back(hWnd);

    DoAddHWND(hWnd);
}

void wxToolTip::DoAddHWND(WXHWND hWnd)
{
    HWND hwnd = (HWND)hWnd;
    wxMSWWinUITooltipLog("wxToolTip::DoAddHWND enter this=%p hwnd=%p window=%p tipLen=%lu",
                         static_cast<void *>(this),
                         static_cast<void *>(hwnd),
                         static_cast<void *>(m_window),
                         static_cast<unsigned long>(m_text.length()));

    wxToolInfo ti(hwnd, m_id, m_rect);

    // another possibility would be to specify LPSTR_TEXTCALLBACK here as we
    // store the tooltip text ourselves anyhow, and provide it in response to
    // TTN_NEEDTEXT (sent via WM_NOTIFY), but then we would be limited to 79
    // character tooltips as this is the size of the szText buffer in
    // NMTTDISPINFO struct -- and setting the tooltip here we can have tooltips
    // of any length
    ti.hwnd = hwnd;
    ti.lpszText = wxMSW_CONV_LPTSTR(m_text);

    const bool added = SendTooltipMessage(GetToolTipCtrl(), TTM_ADDTOOL, &ti) != 0;
    wxMSWWinUITooltipLog("wxToolTip::DoAddHWND first TTM_ADDTOOL hwnd=%p added=%d",
                         static_cast<void *>(hwnd),
                         added ? 1 : 0);
    if ( !added )
    {
        wxLogDebug(wxT("Failed to create the tooltip '%s'"), m_text);

        return;
    }

#ifdef TTM_SETMAXTIPWIDTH
    if ( !AdjustMaxWidth() )
#endif // TTM_SETMAXTIPWIDTH
    {
        // replace the '\n's with spaces because otherwise they appear as
        // unprintable characters in the tooltip string
        m_text.Replace(wxT("\n"), wxT(" "));
        ti.lpszText = wxMSW_CONV_LPTSTR(m_text);

        const bool addedAfterReplace =
            SendTooltipMessage(GetToolTipCtrl(), TTM_ADDTOOL, &ti) != 0;
        wxMSWWinUITooltipLog("wxToolTip::DoAddHWND second TTM_ADDTOOL hwnd=%p added=%d",
                             static_cast<void *>(hwnd),
                             addedAfterReplace ? 1 : 0);
        if ( !addedAfterReplace )
        {
            wxLogDebug(wxT("Failed to create the tooltip '%s'"), m_text);
        }
    }

    wxMSWWinUITooltipLog("wxToolTip::DoAddHWND leave this=%p hwnd=%p",
                         static_cast<void *>(this),
                         static_cast<void *>(hwnd));
}

void wxToolTip::SetWindow(wxWindow *win)
{
    wxMSWWinUITooltipLog("wxToolTip::SetWindow enter this=%p oldWindow=%p newWindow=%p newHwnd=%p",
                         static_cast<void *>(this),
                         static_cast<void *>(m_window),
                         static_cast<void *>(win),
                         win ? reinterpret_cast<void *>(win->GetHWND()) : nullptr);
    Remove();
    wxMSWWinUITooltipLog("wxToolTip::SetWindow after Remove this=%p",
                         static_cast<void *>(this));

    m_window = win;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    m_isWinUIHost = false;

    if ( m_window && wxWinUIIsHostWindow(m_window) )
    {
        // Keep GetWindow()/GetToolTip() coherent, but let the shared island
        // state adapter own the visual tooltip.  Registering the covered HWND
        // with the native tooltip control would create a duplicate.
        m_isWinUIHost = true;
        wxWinUITLWHostNotifySlotState(m_window);
        return;
    }
#endif

    // add the window itself
    if ( m_window )
    {
        DoAddHWND(m_window->GetHWND());
    }
    wxMSWWinUITooltipLog("wxToolTip::SetWindow after main hwnd this=%p window=%p",
                         static_cast<void *>(this),
                         static_cast<void *>(m_window));
#if !defined(__WXUNIVERSAL__)
    // and all of its subcontrols (e.g. radio buttons in a radiobox) as well
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
                // maybe it's a child of parent of the control, in fact?
                // (radiobuttons are subcontrols, i.e. children of the radiobox
                // for wxWidgets but are its siblings at Windows level)
                hwnd = GetDlgItem(GetHwndOf(m_window->GetParent()), id);
            }

            // must have it by now!
            wxASSERT_MSG( hwnd, wxT("no hwnd for subcontrol?") );

            AddOtherWindow((WXHWND)hwnd);
        }
    }
#endif // !defined(__WXUNIVERSAL__)
    wxMSWWinUITooltipLog("wxToolTip::SetWindow leave this=%p window=%p",
                         static_cast<void *>(this),
                         static_cast<void *>(m_window));
}

void wxToolTip::SetRect(const wxRect& rc)
{
    m_rect = rc;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( m_isWinUIHost )
        return;
#endif

    if ( m_window )
    {
        wxToolInfo ti(GetHwndOf(m_window), m_id, m_rect);
        (void)SendTooltipMessage(GetToolTipCtrl(), TTM_NEWTOOLRECT, &ti);
    }
}

void wxToolTip::SetTip(const wxString& tip)
{
    wxMSWWinUITooltipLog("wxToolTip::SetTip enter this=%p tipLen=%lu window=%p",
                         static_cast<void *>(this),
                         static_cast<unsigned long>(tip.length()),
                         static_cast<void *>(m_window));
    m_text = tip;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( m_isWinUIHost )
    {
        // SetTip() is public and bypasses wxWindow::SetToolTip(); explicitly
        // dirty the slot so this canonical object's new text reaches XAML.
        wxWinUITLWHostNotifySlotState(m_window);
        return;
    }
#endif

#ifdef TTM_SETMAXTIPWIDTH
    if ( !AdjustMaxWidth() )
#endif // TTM_SETMAXTIPWIDTH
    {
        // replace the '\n's with spaces because otherwise they appear as
        // unprintable characters in the tooltip string
        m_text.Replace(wxT("\n"), wxT(" "));
    }

    DoForAllWindows(&wxToolTip::DoSetTip);
    wxMSWWinUITooltipLog("wxToolTip::SetTip leave this=%p",
                         static_cast<void *>(this));
}

void wxToolTip::DoSetTip(WXHWND hWnd)
{
    wxMSWWinUITooltipLog("wxToolTip::DoSetTip enter this=%p hwnd=%p tipLen=%lu",
                         static_cast<void *>(this),
                         static_cast<void *>(hWnd),
                         static_cast<unsigned long>(m_text.length()));
    // update the tip text shown by the control
    wxToolInfo ti((HWND)hWnd, m_id, m_rect);

    // for some reason, changing the tooltip text directly results in
    // repaint of the controls under it, see #10520 -- but this doesn't
    // happen if we reset it first
    ti.lpszText = const_cast<wxChar *>(wxT(""));
    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, &ti);

    ti.lpszText = wxMSW_CONV_LPTSTR(m_text);
    (void)SendTooltipMessage(GetToolTipCtrl(), TTM_UPDATETIPTEXT, &ti);
    wxMSWWinUITooltipLog("wxToolTip::DoSetTip leave this=%p hwnd=%p",
                         static_cast<void *>(this),
                         static_cast<void *>(hWnd));
}

bool wxToolTip::AdjustMaxWidth()
{
    // use TTM_SETMAXTIPWIDTH to make tooltip multiline using the
    // extent of its first line as max value
    HFONT hfont = (HFONT)
        SendTooltipMessage(GetToolTipCtrl(), WM_GETFONT, nullptr);

    if ( !hfont )
    {
        hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if ( !hfont )
        {
            wxLogLastError(wxT("GetStockObject(DEFAULT_GUI_FONT)"));
        }
    }

    MemoryHDC hdc;
    if ( !hdc )
    {
        wxLogLastError(wxT("CreateCompatibleDC(nullptr)"));
    }

    if ( !SelectObject(hdc, hfont) )
    {
        wxLogLastError(wxT("SelectObject(hfont)"));
    }

    // find the width of the widest line
    int maxWidth = 0;
    wxStringTokenizer tokenizer(m_text, wxT("\n"));
    while ( tokenizer.HasMoreTokens() )
    {
        const wxString token = tokenizer.GetNextToken();

        SIZE sz{};
        if ( !::GetTextExtentPoint32(hdc, token.t_str(),
                                     token.length(), &sz) )
        {
            wxLogLastError(wxT("GetTextExtentPoint32"));
            continue;
        }

        if ( sz.cx > maxWidth )
            maxWidth = sz.cx;
    }

    // Limit size to the configured policy, if set. Keep the public sentinel
    // itself unchanged: WinUI tooltips snapshot SetMaxWidth() at construction,
    // and a native tooltip must not silently turn policy 0 into an explicit
    // 400px XAML limit for every later object.
    int effectiveMaxWidth = ms_maxWidth;
    if ( effectiveMaxWidth == 0 )
    {
        // this is more or less arbitrary but seems to work well
        static const int DEFAULT_MAX_WIDTH = 400;

        effectiveMaxWidth = wxGetClientDisplayRect().width / 2;

        if ( effectiveMaxWidth > DEFAULT_MAX_WIDTH )
            effectiveMaxWidth = DEFAULT_MAX_WIDTH;
    }

    if ( effectiveMaxWidth != -1 && maxWidth > effectiveMaxWidth )
        maxWidth = effectiveMaxWidth;

    // only set a new width if it is bigger than the current setting:
    // otherwise adding a tooltip with shorter line(s) than a previous
    // one would result in breaking the longer lines unnecessarily as
    // all our tooltips share the same maximal width
    if ( maxWidth > SendTooltipMessage(GetToolTipCtrl(),
                                       TTM_GETMAXTIPWIDTH, nullptr) )
    {
        SendTooltipMessage(GetToolTipCtrl(), TTM_SETMAXTIPWIDTH,
                           wxUIntToPtr(maxWidth));
    }

    return true;
}

void wxToolTip::DoForAllWindows(void (wxToolTip::*func)(WXHWND))
{
    if ( m_window )
    {
        (this->*func)(m_window->GetHWND());
    }

    if ( m_others )
    {
        for ( wxToolTipOtherWindows::const_iterator it = m_others->begin();
              it != m_others->end();
              ++it )
        {
            (this->*func)(*it);
        }
    }
}

#endif // wxUSE_TOOLTIPS
