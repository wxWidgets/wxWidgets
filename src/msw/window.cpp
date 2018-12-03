/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/window.cpp
// Purpose:     wxWindowMSW
// Author:      Julian Smart
// Modified by: VZ on 13.05.99: no more Default(), MSWOnXXX() reorganisation
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/window.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/msw/missing.h"
    #include "wx/accel.h"
    #include "wx/menu.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/dialog.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/layout.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/listbox.h"
    #include "wx/button.h"
    #include "wx/msgdlg.h"
    #include "wx/settings.h"
    #include "wx/statbox.h"
    #include "wx/sizer.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
    #include "wx/menuitem.h"
    #include "wx/module.h"
    #include "wx/vector.h"
#endif

#if wxUSE_OWNER_DRAWN && !defined(__WXUNIVERSAL__)
    #include "wx/ownerdrw.h"
#endif

#include "wx/hashmap.h"
#include "wx/evtloop.h"
#include "wx/popupwin.h"
#include "wx/power.h"
#include "wx/scopeguard.h"
#include "wx/sysopt.h"

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
    #include <ole2.h>
    #include <oleacc.h>
    #ifndef OBJID_CLIENT
        #define OBJID_CLIENT 0xFFFFFFFC
    #endif
#endif

#include "wx/msw/private.h"
#include "wx/msw/private/keyboard.h"
#include "wx/msw/private/winstyle.h"
#include "wx/msw/dcclient.h"
#include "wx/msw/seh.h"
#include "wx/private/textmeasure.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#if wxUSE_RADIOBOX
    #include "wx/radiobox.h"
#endif // wxUSE_RADIOBOX

#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif // wxUSE_SPINCTRL

#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/dynlib.h"

#include <string.h>

#include <shellapi.h>
#include <mmsystem.h>

#include <windowsx.h>

#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h"
#endif

#if wxUSE_DYNLIB_CLASS
    #define HAVE_TRACKMOUSEEVENT
#endif // everything needed for TrackMouseEvent()

#ifndef MAPVK_VK_TO_CHAR
    // Contrary to MS claims that this is present starting with Win2k, it is
    // missing from the SDK released for Windows 5.2 build 3790, aka XP 64-bit
    // and also from tdm32-gcc-5.1.0.
    #define MAPVK_VK_TO_CHAR 2
#endif

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE
extern wxMenu *wxCurrentPopupMenu;
#endif

#if wxUSE_POPUPWIN
extern wxPopupWindow* wxCurrentPopupWindow;
#endif // wxUSE_POPUPWIN

#if wxUSE_UXTHEME
// This is a hack used by the owner-drawn wxButton implementation to ensure
// that the brush used for erasing its background is correctly aligned with the
// control.
wxWindowMSW *wxWindowBeingErased = NULL;
#endif // wxUSE_UXTHEME

namespace
{

// true if we had already created the std colour map, used by
// wxGetStdColourMap() and wxWindow::OnSysColourChanged()           (FIXME-MT)
bool gs_hasStdCmap = false;

// last mouse event information we need to filter out the duplicates
struct MouseEventInfoDummy
{
    // mouse position (in screen coordinates)
    wxPoint pos;

    // last mouse event type
    wxEventType type;
} gs_lastMouseEvent;

// hash containing the registered handlers for the custom messages
WX_DECLARE_HASH_MAP(int, wxWindow::MSWMessageHandler,
                    wxIntegerHash, wxIntegerEqual,
                    MSWMessageHandlers);

MSWMessageHandlers gs_messageHandlers;

// hash containing all our windows, it uses HWND keys and wxWindow* values
WX_DECLARE_HASH_MAP(HWND, wxWindow *,
                    wxPointerHash, wxPointerEqual,
                    WindowHandles);

WindowHandles gs_windowHandles;

#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK

// temporary override for WM_ERASEBKGND processing: we don't store this in
// wxWindow itself as we don't need it during most of the time so don't
// increase the size of all window objects unnecessarily
WX_DECLARE_HASH_MAP(wxWindow *, wxWindow *,
                    wxPointerHash, wxPointerEqual,
                    EraseBgHooks);

EraseBgHooks gs_eraseBgHooks;

#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK

// If this variable is strictly positive, EVT_CHAR_HOOK is not generated for
// Escape key presses as it can't be intercepted because it's needed by some
// currently shown window, e.g. IME entry.
//
// This is currently global as we allow using UI from the main thread only
// anyhow but could be replaced with a thread-specific value in the future if
// needed.
int gs_modalEntryWindowCount = 0;

// Indicates whether we are currently processing WM_CAPTURECHANGED message.
bool gs_insideCaptureChanged = false;

} // anonymous namespace

#ifdef WM_GESTURE

namespace
{

// Class used to dynamically load gestures related API functions.
class GestureFuncs
{
public:
    // Must be called before using any other methods of this class (and they
    // can't be used if this one returns false).
    static bool IsOk()
    {
        if ( !ms_gestureSymbolsLoaded )
        {
            ms_gestureSymbolsLoaded = true;
            LoadGestureSymbols();
        }

        return ms_pfnGetGestureInfo &&
                ms_pfnCloseGestureInfoHandle &&
                    ms_pfnSetGestureConfig;
    }

    typedef BOOL (WINAPI *GetGestureInfo_t)(HGESTUREINFO, PGESTUREINFO);

    static GetGestureInfo_t GetGestureInfo()
    {
        return ms_pfnGetGestureInfo;
    }

    typedef BOOL (WINAPI *CloseGestureInfoHandle_t)(HGESTUREINFO);

    static CloseGestureInfoHandle_t CloseGestureInfoHandle()
    {
        return ms_pfnCloseGestureInfoHandle;
    }

    typedef BOOL
        (WINAPI *SetGestureConfig_t)(HWND, DWORD, UINT, PGESTURECONFIG, UINT);

    static SetGestureConfig_t SetGestureConfig()
    {
        return ms_pfnSetGestureConfig;
    }

private:
    static void LoadGestureSymbols()
    {
        wxLoadedDLL dll(wxS("user32.dll"));

        wxDL_INIT_FUNC(ms_pfn, GetGestureInfo, dll);
        wxDL_INIT_FUNC(ms_pfn, CloseGestureInfoHandle, dll);
        wxDL_INIT_FUNC(ms_pfn, SetGestureConfig, dll);
    }

    static GetGestureInfo_t ms_pfnGetGestureInfo;
    static CloseGestureInfoHandle_t ms_pfnCloseGestureInfoHandle;
    static SetGestureConfig_t ms_pfnSetGestureConfig;

    static bool ms_gestureSymbolsLoaded;
};

GestureFuncs::GetGestureInfo_t
    GestureFuncs::ms_pfnGetGestureInfo = NULL;
GestureFuncs::CloseGestureInfoHandle_t
    GestureFuncs::ms_pfnCloseGestureInfoHandle = NULL;
GestureFuncs::SetGestureConfig_t
    GestureFuncs::ms_pfnSetGestureConfig = NULL;

bool GestureFuncs::ms_gestureSymbolsLoaded = false;

} // anonymous namespace

#endif // WM_GESTURE

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// the window proc for all our windows
LRESULT WXDLLEXPORT APIENTRY
wxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#if wxDEBUG_LEVEL >= 2
const wxChar *wxGetMessageName(int message);

inline
void wxTraceMSWMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // The casts to size_t allow to avoid having different code for Win32 and
    // Win64 as size_t is of the right size in both cases, unlike int or long.
    wxLogTrace("winmsg",
               wxT("Processing %s(hWnd=%p, wParam=%zx, lParam=%zx)"),
               wxGetMessageName(message),
               hWnd,
               static_cast<size_t>(wParam),
               static_cast<size_t>(lParam));
}
#endif  // wxDEBUG_LEVEL >= 2

void wxRemoveHandleAssociation(wxWindowMSW *win);
extern void wxAssociateWinWithHandle(HWND hWnd, wxWindowMSW *win);

// get the text metrics for the current font
static TEXTMETRIC wxGetTextMetrics(const wxWindowMSW *win);

// wrapper around BringWindowToTop() API
static inline void wxBringWindowToTop(HWND hwnd)
{
    // raise top level parent to top of z order
    if (!::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
    {
        wxLogLastError(wxT("SetWindowPos"));
    }
}

// ensure that all our parent windows have WS_EX_CONTROLPARENT style
static void EnsureParentHasControlParentStyle(wxWindow *parent)
{
    /*
       If we have WS_EX_CONTROLPARENT flag we absolutely *must* set it for our
       parent as well as otherwise several Win32 functions using
       GetNextDlgTabItem() to iterate over all controls such as
       IsDialogMessage() or DefDlgProc() would enter an infinite loop: indeed,
       all of them iterate over all the controls starting from the currently
       focused one and stop iterating when they get back to the focus but
       unless all parents have WS_EX_CONTROLPARENT bit set, they would never
       get back to the initial (focused) window: as we do have this style,
       GetNextDlgTabItem() will leave this window and continue in its parent,
       but if the parent doesn't have it, it wouldn't recurse inside it later
       on and so wouldn't have a chance of getting back to this window either.
     */
    while ( parent && !parent->IsTopLevel() )
    {
        // force the parent to have this style
        wxMSWWinExStyleUpdater(GetHwndOf(parent)).TurnOn(WS_EX_CONTROLPARENT);

        parent = parent->GetParent();
    }
}

// GetCursorPos can return an error, so use this function
// instead.
// Error observed when using Remote Desktop to connect to XP.
void wxGetCursorPosMSW(POINT* pt)
{
    if (!GetCursorPos(pt))
    {
        DWORD pos = GetMessagePos();
        // the coordinates may be negative in multi-monitor systems
        pt->x = GET_X_LPARAM(pos);
        pt->y = GET_Y_LPARAM(pos);
    }
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

// in wxUniv/MSW this class is abstract because it doesn't have DoPopupMenu()
// method
#ifdef __WXUNIVERSAL__
    wxIMPLEMENT_ABSTRACT_CLASS(wxWindowMSW, wxWindowBase);
#endif // __WXUNIVERSAL__

wxBEGIN_EVENT_TABLE(wxWindowMSW, wxWindowBase)
    EVT_SYS_COLOUR_CHANGED(wxWindowMSW::OnSysColourChanged)
wxEND_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxWindow utility functions
// ---------------------------------------------------------------------------

// Find an item given the MS Windows id
wxWindow *wxWindowMSW::FindItem(long id, WXHWND hWnd) const
{
    // First check for the control itself and its Windows-level children which
    // are mapped to the same wxWindow at wx level.
    wxWindow *wnd = MSWFindItem(id, hWnd);
    if ( wnd )
        return wnd;

    // Then check wx level children.
    wxWindowList::compatibility_iterator current = GetChildren().GetFirst();
    while (current)
    {
        wxWindow *childWin = current->GetData();

        wnd = childWin->FindItem(id, hWnd);
        if ( wnd )
            return wnd;

        current = current->GetNext();
    }

    return NULL;
}

// Find an item given the MS Windows handle
wxWindow *wxWindowMSW::FindItemByHWND(WXHWND hWnd, bool controlOnly) const
{
    wxWindowList::compatibility_iterator current = GetChildren().GetFirst();
    while (current)
    {
        wxWindow *parent = current->GetData();

        // Do a recursive search.
        wxWindow *wnd = parent->FindItemByHWND(hWnd);
        if ( wnd )
            return wnd;

        if ( !controlOnly
#if wxUSE_CONTROLS
                || wxDynamicCast(parent, wxControl)
#endif // wxUSE_CONTROLS
           )
        {
            wxWindow *item = current->GetData();
            if ( item->GetHWND() == hWnd )
                return item;
            else
            {
                if ( item->ContainsHWND(hWnd) )
                    return item;
            }
        }

        current = current->GetNext();
    }
    return NULL;
}

// Default command handler
bool wxWindowMSW::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    return false;
}

// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindowMSW::Init()
{
    // MSW specific
    m_oldWndProc = NULL;
    m_mouseInWindow = false;
    m_lastKeydownProcessed = false;

    m_hWnd = 0;

    m_xThumbSize = 0;
    m_yThumbSize = 0;

#if wxUSE_DEFERRED_SIZING
    m_hDWP = 0;
    m_pendingPosition = wxDefaultPosition;
    m_pendingSize = wxDefaultSize;
#endif // wxUSE_DEFERRED_SIZING

}

// Destructor
wxWindowMSW::~wxWindowMSW()
{
    SendDestroyEvent();

    // VS: destroy children first and _then_ detach *this from its parent.
    //     If we did it the other way around, children wouldn't be able
    //     find their parent frame.
    DestroyChildren();

    if ( m_hWnd )
    {
        // VZ: test temp removed to understand what really happens here
        //if (::IsWindow(GetHwnd()))
        {
            if ( !::DestroyWindow(GetHwnd()) )
            {
                wxLogLastError(wxT("DestroyWindow"));
            }
        }

        // remove hWnd <-> wxWindow association
        wxRemoveHandleAssociation(this);
    }

}

const wxChar *wxWindowMSW::GetMSWClassName(long style)
{
    return wxApp::GetRegisteredClassName
                  (
                    wxT("wxWindow"),
                    COLOR_BTNFACE,
                    0, // no special extra style
                    (style & wxFULL_REPAINT_ON_RESIZE) ? wxApp::RegClass_Default
                                                       : wxApp::RegClass_ReturnNR
                  );
}

// real construction (Init() must have been called before!)
bool wxWindowMSW::CreateUsingMSWClass(const wxChar* classname,
                                      wxWindow *parent,
                                      wxWindowID id,
                                      const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxString& name)
{
    wxCHECK_MSG( parent, false, wxT("can't create wxWindow without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    parent->AddChild(this);

    WXDWORD exstyle;
    DWORD msflags = MSWGetCreateWindowFlags(&exstyle);

#ifdef __WXUNIVERSAL__
    // no borders, we draw them ourselves
    exstyle &= ~(WS_EX_DLGMODALFRAME |
                 WS_EX_STATICEDGE |
                 WS_EX_CLIENTEDGE |
                 WS_EX_WINDOWEDGE);
    msflags &= ~WS_BORDER;
#endif // wxUniversal

    if ( IsShown() )
    {
        msflags |= WS_VISIBLE;
    }

    if ( !MSWCreate(classname, NULL, pos, size, msflags, exstyle) )
        return false;

    InheritAttributes();

    return true;
}

void wxWindowMSW::SetId(wxWindowID winid)
{
    wxWindowBase::SetId(winid);

    // Also update the ID used at the Windows level to avoid nasty surprises
    // when we can't find the control when handling messages for it after
    // changing its ID because Windows still uses the old one.
    if ( GetHwnd() )
    {
        ::SetLastError(0);

        if ( !::SetWindowLong(GetHwnd(), GWL_ID, winid) )
        {
            const DWORD err = ::GetLastError();
            if ( err )
                wxLogApiError(wxT("SetWindowLong(GWL_ID)"), err);
        }
    }
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindowMSW::SetFocus()
{
    HWND hWnd = GetHwnd();
    wxCHECK_RET( hWnd, wxT("can't set focus to invalid window") );

    ::SetLastError(0);

    if ( !::SetFocus(hWnd) )
    {
        // was there really an error?
        DWORD dwRes = ::GetLastError();
        if ( dwRes )
        {
            HWND hwndFocus = ::GetFocus();
            if ( hwndFocus != hWnd )
            {
                wxLogApiError(wxT("SetFocus"), dwRes);
            }
        }
    }
}

void wxWindowMSW::SetFocusFromKbd()
{
    // when the focus is given to the control with DLGC_HASSETSEL style from
    // keyboard its contents should be entirely selected: this is what
    // ::IsDialogMessage() does and so we should do it as well to provide the
    // same LNF as the native programs
    if ( ::SendMessage(GetHwnd(), WM_GETDLGCODE, 0, 0) & DLGC_HASSETSEL )
    {
        ::SendMessage(GetHwnd(), EM_SETSEL, 0, -1);
    }

    // do this after (maybe) setting the selection as like this when
    // wxEVT_SET_FOCUS handler is called, the selection would have been already
    // set correctly -- this may be important
    wxWindowBase::SetFocusFromKbd();
}

// Get the window with the focus
wxWindow *wxWindowBase::DoFindFocus()
{
    HWND hWnd = ::GetFocus();
    if ( hWnd )
    {
        return wxGetWindowFromHWND((WXHWND)hWnd);
    }

    return NULL;
}

void wxWindowMSW::DoEnable( bool enable )
{
    MSWEnableHWND(GetHwnd(), enable);
}

bool wxWindowMSW::MSWEnableHWND(WXHWND hWnd, bool enable)
{
    if ( !hWnd )
        return false;

    // If disabling focused control, we move focus to the next one, as if the
    // user pressed Tab. That's because we can't keep focus on a disabled
    // control, Tab-navigation would stop working then.
    if ( !enable && ::GetFocus() == hWnd )
        Navigate();

    return ::EnableWindow(hWnd, (BOOL)enable) != 0;
}

bool wxWindowMSW::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return false;

    HWND hWnd = GetHwnd();

    // we could be called before the underlying window is created (this is
    // actually useful to prevent it from being initially shown), e.g.
    //
    //      wxFoo *foo = new wxFoo;
    //      foo->Hide();
    //      foo->Create(parent, ...);
    //
    // should work without errors
    if ( hWnd )
    {
        ::ShowWindow(hWnd, show ? SW_SHOW : SW_HIDE);
    }

    if ( IsFrozen() )
    {
        // DoFreeze/DoThaw don't do anything if the window is not shown, so
        // we have to call them from here now
        if ( show )
            DoFreeze();
        else
            DoThaw();
    }

    return true;
}

bool
wxWindowMSW::MSWShowWithEffect(bool show,
                               wxShowEffect effect,
                               unsigned timeout)
{
#if wxUSE_DYNLIB_CLASS
    if ( effect == wxSHOW_EFFECT_NONE ||
            (GetParent() && !GetParent()->IsShownOnScreen()) )
        return Show(show);

    if ( !wxWindowBase::Show(show) )
        return false;

    // Show() has a side effect of sending a WM_SIZE to the window, which helps
    // ensuring that it's laid out correctly, but AnimateWindow() doesn't do
    // this so send the event ourselves
    SendSizeEvent();

    // prepare to use AnimateWindow()

    if ( !timeout )
        timeout = 200; // this is the default animation timeout, per MSDN

    DWORD dwFlags = show ? 0 : AW_HIDE;

    switch ( effect )
    {
        case wxSHOW_EFFECT_ROLL_TO_LEFT:
            dwFlags |= AW_HOR_NEGATIVE;
            break;

        case wxSHOW_EFFECT_ROLL_TO_RIGHT:
            dwFlags |= AW_HOR_POSITIVE;
            break;

        case wxSHOW_EFFECT_ROLL_TO_TOP:
            dwFlags |= AW_VER_NEGATIVE;
            break;

        case wxSHOW_EFFECT_ROLL_TO_BOTTOM:
            dwFlags |= AW_VER_POSITIVE;
            break;

        case wxSHOW_EFFECT_SLIDE_TO_LEFT:
            dwFlags |= AW_SLIDE | AW_HOR_NEGATIVE;
            break;

        case wxSHOW_EFFECT_SLIDE_TO_RIGHT:
            dwFlags |= AW_SLIDE | AW_HOR_POSITIVE;
            break;

        case wxSHOW_EFFECT_SLIDE_TO_TOP:
            dwFlags |= AW_SLIDE | AW_VER_NEGATIVE;
            break;

        case wxSHOW_EFFECT_SLIDE_TO_BOTTOM:
            dwFlags |= AW_SLIDE | AW_VER_POSITIVE;
            break;

        case wxSHOW_EFFECT_BLEND:
            dwFlags |= AW_BLEND;
            break;

        case wxSHOW_EFFECT_EXPAND:
            dwFlags |= AW_CENTER;
            break;


        case wxSHOW_EFFECT_MAX:
            wxFAIL_MSG( wxT("invalid window show effect") );
            return false;

        default:
            wxFAIL_MSG( wxT("unknown window show effect") );
            return false;
    }

    if ( !::AnimateWindow(GetHwnd(), timeout, dwFlags) )
    {
        wxLogLastError(wxT("AnimateWindow"));

        return false;
    }

    return true;
#else    // wxUSE_DYNLIB_CLASS
    return Show(show);
#endif
}

// Raise the window to the top of the Z order
void wxWindowMSW::Raise()
{
    wxBringWindowToTop(GetHwnd());
}

// Lower the window to the bottom of the Z order
void wxWindowMSW::Lower()
{
    ::SetWindowPos(GetHwnd(), HWND_BOTTOM, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void wxWindowMSW::DoCaptureMouse()
{
    HWND hWnd = GetHwnd();
    if ( hWnd )
    {
        ::SetCapture(hWnd);
    }
}

void wxWindowMSW::DoReleaseMouse()
{
    if ( !::ReleaseCapture() )
    {
        wxLogLastError(wxT("ReleaseCapture"));
    }
}

/* static */ wxWindow *wxWindowBase::GetCapture()
{
    // When we receive WM_CAPTURECHANGED message, ::GetCapture() still returns
    // the HWND that is losing the mouse capture. But as we must not release
    // the capture for it (it's going to happen anyhow), pretend that there is
    // no capture any more.
    if ( gs_insideCaptureChanged )
        return NULL;

    HWND hwnd = ::GetCapture();
    return hwnd ? wxFindWinFromHandle(hwnd) : NULL;
}

bool wxWindowMSW::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return false;
    }

    HWND hWnd = GetHwnd();
    if ( hWnd != 0 )
    {
        // note the use of GetFont() instead of m_font: our own font could have
        // just been reset and in this case we need to change the font used by
        // the native window to the default for this class, i.e. exactly what
        // GetFont() returns
        WXHANDLE hFont = GetFont().GetResourceHandle();

        wxASSERT_MSG( hFont, wxT("should have valid font") );

        ::SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
    }

    return true;
}

bool wxWindowMSW::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return false;
    }

    // don't "overwrite" busy cursor
    if ( wxIsBusy() )
        return true;

    if ( m_cursor.IsOk() )
    {
        // normally we should change the cursor only if it's over this window
        // but we should do it always if we capture the mouse currently
        bool set = HasCapture();
        if ( !set )
        {
            HWND hWnd = GetHwnd();

            POINT point;
            ::wxGetCursorPosMSW(&point);

            RECT rect = wxGetWindowRect(hWnd);

            set = ::PtInRect(&rect, point) != 0;
        }

        if ( set )
        {
            ::SetCursor(GetHcursorOf(m_cursor));
        }
        //else: will be set later when the mouse enters this window
    }
    else // Invalid cursor: this means reset to the default one.
    {
        // To revert to the correct cursor we need to find the window currently
        // under the cursor and ask it to set its cursor itself as only it
        // knows what it is.
        POINT pt;
        wxGetCursorPosMSW(&pt);

        const wxWindowMSW* win = wxFindWindowAtPoint(wxPoint(pt.x, pt.y));
        if ( !win )
            win = this;

        ::SendMessage(GetHwndOf(win), WM_SETCURSOR,
                      (WPARAM)GetHwndOf(win),
                      MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
    }

    return true;
}

void wxWindowMSW::WarpPointer(int x, int y)
{
    ClientToScreen(&x, &y);

    if ( !::SetCursorPos(x, y) )
    {
        wxLogLastError(wxT("SetCursorPos"));
    }
}

bool wxWindowMSW::EnableTouchEvents(int eventsMask)
{
#ifdef WM_GESTURE
    if ( GestureFuncs::IsOk() )
    {
        // Static struct used when we need to use just a single configuration.
        GESTURECONFIG config = {0, 0, 0};

        GESTURECONFIG* ptrConfigs = &config;
        UINT numConfigs = 1;

        // This is used only if we need to allocate the configurations
        // dynamically.
        wxVector<GESTURECONFIG> configs;

        // There are two simple cases: enabling or disabling all gestures.
        if ( eventsMask == wxTOUCH_NONE )
        {
            config.dwBlock = GC_ALLGESTURES;
        }
        else if ( eventsMask == wxTOUCH_ALL_GESTURES )
        {
            config.dwWant = GC_ALLGESTURES;
        }
        else // Need to enable the individual gestures
        {
            int wantedPan = 0;
            switch ( eventsMask & wxTOUCH_PAN_GESTURES )
            {
                case wxTOUCH_VERTICAL_PAN_GESTURE:
                    wantedPan = GC_PAN_WITH_SINGLE_FINGER_VERTICALLY;
                    break;

                case wxTOUCH_HORIZONTAL_PAN_GESTURE:
                    wantedPan = GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY;
                    break;

                case wxTOUCH_PAN_GESTURES:
                    wantedPan = GC_PAN;
                    break;

                case 0:
                    // This is the only other possibility and wantedPan is
                    // already initialized to 0 anyhow, so don't do anything,
                    // just list it for completeness.
                    break;
            }

            if ( wantedPan )
            {
                eventsMask &= ~wxTOUCH_PAN_GESTURES;

                config.dwID = GID_PAN;
                config.dwWant = wantedPan;
                configs.push_back(config);
            }

            if ( eventsMask & wxTOUCH_ZOOM_GESTURE )
            {
                eventsMask &= ~wxTOUCH_ZOOM_GESTURE;

                config.dwID = GID_ZOOM;
                config.dwWant = GC_ZOOM;
                configs.push_back(config);
            }

            if ( eventsMask & wxTOUCH_ROTATE_GESTURE )
            {
                eventsMask &= ~wxTOUCH_ROTATE_GESTURE;

                config.dwID = GID_ROTATE;
                config.dwWant = GC_ROTATE;
                configs.push_back(config);
            }

            if ( eventsMask & wxTOUCH_PRESS_GESTURES )
            {
                eventsMask &= ~wxTOUCH_PRESS_GESTURES;

                config.dwID = GID_TWOFINGERTAP;
                config.dwWant = GC_TWOFINGERTAP;
                configs.push_back(config);

                config.dwID = GID_PRESSANDTAP;
                config.dwWant = GC_PRESSANDTAP;
                configs.push_back(config);
            }

            // As we clear all the known bits if they're set in the code above,
            // there should be nothing left.
            wxCHECK_MSG( eventsMask == 0, false,
                         wxS("Unknown touch event mask bit specified") );

            ptrConfigs = &configs[0];
        }

        if ( !GestureFuncs::SetGestureConfig()
             (
                m_hWnd,
                0,                      // Reserved, must be always 0.
                numConfigs,             // Number of gesture configurations.
                ptrConfigs,             // Pointer to the first one.
                sizeof(GESTURECONFIG)   // Size of each configuration.
             )
           )
        {
            wxLogLastError("SetGestureConfig");
            return false;
        }

        return true;
    }
#endif // WM_GESTURE

    return wxWindowBase::EnableTouchEvents(eventsMask);
}

void wxWindowMSW::MSWUpdateUIState(int action, int state)
{
    // we send WM_CHANGEUISTATE so if nothing needs changing then the system
    // won't send WM_UPDATEUISTATE
    ::SendMessage(GetHwnd(), WM_CHANGEUISTATE, MAKEWPARAM(action, state), 0);
}

// ---------------------------------------------------------------------------
// scrolling stuff
// ---------------------------------------------------------------------------

namespace
{

inline int GetScrollPosition(HWND hWnd, int wOrient)
{
    WinStruct<SCROLLINFO> scrollInfo;
    scrollInfo.cbSize = sizeof(SCROLLINFO);
    scrollInfo.fMask = SIF_POS;
    ::GetScrollInfo(hWnd, wOrient, &scrollInfo );

    return scrollInfo.nPos;
}

inline UINT WXOrientToSB(int orient)
{
    return orient == wxHORIZONTAL ? SB_HORZ : SB_VERT;
}

} // anonymous namespace

int wxWindowMSW::GetScrollPos(int orient) const
{
    HWND hWnd = GetHwnd();
    wxCHECK_MSG( hWnd, 0, wxT("no HWND in GetScrollPos") );

    return GetScrollPosition(hWnd, WXOrientToSB(orient));
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindowMSW::GetScrollRange(int orient) const
{
    int maxPos;
    HWND hWnd = GetHwnd();
    if ( !hWnd )
        return 0;
    WinStruct<SCROLLINFO> scrollInfo;
    scrollInfo.fMask = SIF_RANGE;
    if ( !::GetScrollInfo(hWnd, WXOrientToSB(orient), &scrollInfo) )
    {
        // Most of the time this is not really an error, since the return
        // value can also be zero when there is no scrollbar yet.
        // wxLogLastError(wxT("GetScrollInfo"));
    }
    maxPos = scrollInfo.nMax;

    // undo "range - 1" done in SetScrollbar()
    return maxPos + 1;
}

int wxWindowMSW::GetScrollThumb(int orient) const
{
    return orient == wxHORIZONTAL ? m_xThumbSize : m_yThumbSize;
}

void wxWindowMSW::SetScrollPos(int orient, int pos, bool refresh)
{
    HWND hWnd = GetHwnd();
    wxCHECK_RET( hWnd, wxT("SetScrollPos: no HWND") );

    WinStruct<SCROLLINFO> info;
    info.nPage = 0;
    info.nMin = 0;
    info.nPos = pos;
    info.fMask = SIF_POS;
    if ( HasFlag(wxALWAYS_SHOW_SB) )
    {
        // disable scrollbar instead of removing it then
        info.fMask |= SIF_DISABLENOSCROLL;
    }

    ::SetScrollInfo(hWnd, WXOrientToSB(orient), &info, refresh);
}

// New function that will replace some of the above.
void wxWindowMSW::SetScrollbar(int orient,
                               int pos,
                               int pageSize,
                               int range,
                               bool refresh)
{
#if wxUSE_DEFERRED_SIZING
    // Work around not documented, but reliably happening, at least under
    // Windows 7, but with changing the scrollbars in the middle of a deferred
    // positioning operation: the child windows of a window whose position is
    // deferred after changing the scrollbar get offset compared to their
    // correct position, somehow.
    //
    // Note that this scenario happens all the time with wxScrolledWindow as
    // its HandleOnSize(), which calls AdjustScrollbars() and hence this method
    // indirectly, gets called from WM_SIZE handler, which begins deferring
    // window positions, and calls Layout() which continues doing it after the
    // scrollbar update. So one way of reproducing the bug is to have a window
    // with children, such as wxStaticBox, inside a wxScrolledWindow whose size
    // gets changed.
    //
    // Fix this simply by "flushing" the pending windows positions and starting
    // a new deferring operation.
    if ( m_hDWP )
    {
        // Do reposition the children already moved.
        EndRepositioningChildren();

        // And restart another deferred positioning operation as any currently
        // existing ChildrenRepositioningGuard objects would be confused if we
        // just removed the HDWP from under them.
        //
        // Unfortunately we have to ignore BeginRepositioningChildren() return
        // value here, there is not much that we can do if it fails (but this
        // should never happen anyhow).
        BeginRepositioningChildren();
    }
#endif // wxUSE_DEFERRED_SIZING

    // We have to set the variables here to make them valid in events
    // triggered by ::SetScrollInfo()
    *(orient == wxHORIZONTAL ? &m_xThumbSize : &m_yThumbSize) = pageSize;

    HWND hwnd = GetHwnd();
    if ( !hwnd )
        return;

    WinStruct<SCROLLINFO> info;
    if ( range != -1 )
    {
        info.nPage = pageSize;
        info.nMin = 0;              // range is nMax - nMin + 1
        info.nMax = range - 1;      //  as both nMax and nMax are inclusive
        info.nPos = pos;

        // We normally also reenable scrollbar in case it had been previously
        // disabled by specifying SIF_DISABLENOSCROLL below but we should only
        // do this if it has valid range, otherwise it would be enabled but not
        // do anything.
        if ( range >= pageSize )
        {
            ::EnableScrollBar(hwnd, WXOrientToSB(orient), ESB_ENABLE_BOTH);
        }
    }
    //else: leave all the fields to be 0

    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    if ( HasFlag(wxALWAYS_SHOW_SB) || range == -1 )
    {
        // disable scrollbar instead of removing it then
        info.fMask |= SIF_DISABLENOSCROLL;
    }

    ::SetScrollInfo(hwnd, WXOrientToSB(orient), &info, refresh);
}

void wxWindowMSW::ScrollWindow(int dx, int dy, const wxRect *prect)
{
    if ( !dx && !dy )
        return;

    RECT rect;
    RECT *pr;
    if ( prect )
    {
        wxCopyRectToRECT(*prect, rect);
        pr = &rect;
    }
    else
    {
        pr = NULL;

    }

    ::ScrollWindow(GetHwnd(), dx, dy, pr, pr);
}

static bool ScrollVertically(HWND hwnd, int kind, int count)
{
    int posStart = GetScrollPosition(hwnd, SB_VERT);

    int pos = posStart;
    for ( int n = 0; n < count; n++ )
    {
        ::SendMessage(hwnd, WM_VSCROLL, kind, 0);

        int posNew = GetScrollPosition(hwnd, SB_VERT);
        if ( posNew == pos )
        {
            // don't bother to continue, we're already at top/bottom
            break;
        }

        pos = posNew;
    }

    return pos != posStart;
}

bool wxWindowMSW::ScrollLines(int lines)
{
    bool down = lines > 0;

    return ScrollVertically(GetHwnd(),
                            down ? SB_LINEDOWN : SB_LINEUP,
                            down ? lines : -lines);
}

bool wxWindowMSW::ScrollPages(int pages)
{
    bool down = pages > 0;

    return ScrollVertically(GetHwnd(),
                            down ? SB_PAGEDOWN : SB_PAGEUP,
                            down ? pages : -pages);
}

// ----------------------------------------------------------------------------
// RTL support
// ----------------------------------------------------------------------------

void wxWindowMSW::SetLayoutDirection(wxLayoutDirection dir)
{
    if ( wxUpdateLayoutDirection(GetHwnd(), dir) )
    {
        // Update layout: whether we have children or are drawing something, we
        // need to redo it with the new layout.
        SendSizeEvent();
        Refresh();
    }
}

wxLayoutDirection wxWindowMSW::GetLayoutDirection() const
{
    wxCHECK_MSG( GetHwnd(), wxLayout_Default, wxT("invalid window") );

    return wxHasWindowExStyle(this, WS_EX_LAYOUTRTL) ? wxLayout_RightToLeft
                                                     : wxLayout_LeftToRight;
}

wxCoord
wxWindowMSW::AdjustForLayoutDirection(wxCoord x,
                                      wxCoord WXUNUSED(width),
                                      wxCoord WXUNUSED(widthTotal)) const
{
    // Win32 mirrors the coordinates of RTL windows automatically, so don't
    // redo it ourselves
    return x;
}

// ---------------------------------------------------------------------------
// subclassing
// ---------------------------------------------------------------------------

void wxWindowMSW::SubclassWin(WXHWND hWnd)
{
    wxASSERT_MSG( !m_oldWndProc, wxT("subclassing window twice?") );

    HWND hwnd = (HWND)hWnd;
    wxCHECK_RET( ::IsWindow(hwnd), wxT("invalid HWND in SubclassWin") );

    SetHWND(hWnd);

    wxAssociateWinWithHandle(hwnd, this);

    m_oldWndProc = wxGetWindowProc((HWND)hWnd);

    // we don't need to subclass the window of our own class (in the Windows
    // sense of the word)
    if ( !wxCheckWindowWndProc(hWnd) )
    {
        wxSetWindowProc(hwnd, wxWndProc);

        // If the window didn't use our window proc during its creation, the
        // code in HandleCreate() hasn't been executed, so do it here.
        if ( wxHasWindowExStyle(this, WS_EX_CONTROLPARENT) )
            EnsureParentHasControlParentStyle(GetParent());
    }
    else
    {
        // don't bother restoring it either: this also makes it easy to
        // implement IsOfStandardClass() method which returns true for the
        // standard controls and false for the wxWidgets own windows as it can
        // simply check m_oldWndProc
        m_oldWndProc = NULL;
    }

    // we're officially created now, send the event
    wxWindowCreateEvent event((wxWindow *)this);
    (void)HandleWindowEvent(event);
}

void wxWindowMSW::UnsubclassWin()
{
    wxRemoveHandleAssociation(this);

    // Restore old Window proc
    HWND hwnd = GetHwnd();
    if ( hwnd )
    {
        SetHWND(0);

        wxCHECK_RET( ::IsWindow(hwnd), wxT("invalid HWND in UnsubclassWin") );

        if ( m_oldWndProc )
        {
            if ( !wxCheckWindowWndProc((WXHWND)hwnd) )
            {
                wxSetWindowProc(hwnd, m_oldWndProc);
            }

            m_oldWndProc = NULL;
        }
    }
}

void wxWindowMSW::AssociateHandle(WXWidget handle)
{
    if ( m_hWnd )
    {
      if ( !::DestroyWindow(GetHwnd()) )
      {
        wxLogLastError(wxT("DestroyWindow"));
      }
    }

    WXHWND wxhwnd = (WXHWND)handle;

    // this also calls SetHWND(wxhwnd)
    SubclassWin(wxhwnd);
}

void wxWindowMSW::DissociateHandle()
{
    // this also calls SetHWND(0) for us
    UnsubclassWin();
}


bool wxCheckWindowWndProc(WXHWND hWnd, WXWNDPROC WXUNUSED(wndProc))
{
    const wxString str(wxGetWindowClass(hWnd));

    // TODO: get rid of wxTLWHiddenParent special case (currently it's not
    //       registered by wxApp but using ad hoc code in msw/toplevel.cpp);
    //       there is also a hidden window class used by sockets &c
    return wxApp::IsRegisteredClassName(str) || str == wxT("wxTLWHiddenParent");
}

// ----------------------------------------------------------------------------
// Style handling
// ----------------------------------------------------------------------------

void wxWindowMSW::SetWindowStyleFlag(long flags)
{
    long flagsOld = GetWindowStyleFlag();
    if ( flags == flagsOld )
        return;

    // update the internal variable
    wxWindowBase::SetWindowStyleFlag(flags);

    // and the real window flags
    MSWUpdateStyle(flagsOld, GetExtraStyle());
}

void wxWindowMSW::SetExtraStyle(long exflags)
{
    long exflagsOld = GetExtraStyle();
    if ( exflags == exflagsOld )
        return;

    // update the internal variable
    wxWindowBase::SetExtraStyle(exflags);

    // and the real window flags
    MSWUpdateStyle(GetWindowStyleFlag(), exflagsOld);
}

void wxWindowMSW::MSWUpdateStyle(long flagsOld, long exflagsOld)
{
    // now update the Windows style as well if needed - and if the window had
    // been already created
    if ( !GetHwnd() )
        return;

    // we may need to call SetWindowPos() when we change some styles
    bool callSWP = false;

    WXDWORD exstyle;
    long style = MSWGetStyle(GetWindowStyleFlag(), &exstyle);

    // this is quite a horrible hack but we need it because MSWGetStyle()
    // doesn't take exflags as parameter but uses GetExtraStyle() internally
    // and so we have to modify the window exflags temporarily to get the
    // correct exstyleOld
    long exflagsNew = GetExtraStyle();
    wxWindowBase::SetExtraStyle(exflagsOld);

    WXDWORD exstyleOld;
    long styleOld = MSWGetStyle(flagsOld, &exstyleOld);

    wxWindowBase::SetExtraStyle(exflagsNew);


    if ( style != styleOld )
    {
        // some flags (e.g. WS_VISIBLE or WS_DISABLED) should not be changed by
        // this function so instead of simply setting the style to the new
        // value we clear the bits which were set in styleOld but are set in
        // the new one and set the ones which were not set before
        wxMSWWinStyleUpdater updateStyle(GetHwnd());
        updateStyle.TurnOff(styleOld).TurnOn(style);

        // we need to call SetWindowPos() if any of the styles affecting the
        // frame appearance have changed
        callSWP = ((styleOld ^ style ) & (WS_BORDER |
                                      WS_THICKFRAME |
                                      WS_CAPTION |
                                      WS_DLGFRAME |
                                      WS_MAXIMIZEBOX |
                                      WS_MINIMIZEBOX |
                                      WS_SYSMENU) ) != 0;
    }

    // There is one extra complication with the extended style: we must never
    // reset WS_EX_CONTROLPARENT because it may break the invariant that the
    // parent of any window with this style bit set has it as well. We enforce
    // this invariant elsewhere and must not clear it here to avoid the fatal
    // problems (hangs) which happen if we break it, so ensure it is preserved.
    if ( exstyleOld & WS_EX_CONTROLPARENT )
        exstyle |= WS_EX_CONTROLPARENT;

    wxMSWWinExStyleUpdater updateExStyle(GetHwnd());
    if ( updateExStyle.TurnOff(exstyleOld).TurnOn(exstyle).Apply() )
    {
        // ex style changes don't take effect without calling SetWindowPos
        callSWP = true;
    }

    if ( callSWP )
    {
        // we must call SetWindowPos() to flush the cached extended style and
        // also to make the change to wxSTAY_ON_TOP style take effect: just
        // setting the style simply doesn't work
        if ( !::SetWindowPos(GetHwnd(),
                             updateExStyle.IsOn(WS_EX_TOPMOST) ? HWND_TOPMOST
                                                               : HWND_NOTOPMOST,
                             0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_FRAMECHANGED) )
        {
            wxLogLastError(wxT("SetWindowPos"));
        }
    }
}

wxBorder wxWindowMSW::GetDefaultBorderForControl() const
{
    return wxBORDER_THEME;
}

wxBorder wxWindowMSW::GetDefaultBorder() const
{
    return wxWindowBase::GetDefaultBorder();
}

// Translate wxBORDER_THEME (and other border styles if necessary) to the value
// that makes most sense for this Windows environment
wxBorder wxWindowMSW::TranslateBorder(wxBorder border) const
{
#if wxUSE_UXTHEME
    if (border == wxBORDER_THEME)
    {
        if (CanApplyThemeBorder())
        {
            if ( wxUxThemeIsActive() )
                return wxBORDER_THEME;
        }
        return wxBORDER_SUNKEN;
    }
#endif
    return border;
}


WXDWORD wxWindowMSW::MSWGetStyle(long flags, WXDWORD *exstyle) const
{
    // translate common wxWidgets styles to Windows ones

    // most of windows are child ones, those which are not (such as
    // wxTopLevelWindow) should remove WS_CHILD in their MSWGetStyle()
    WXDWORD style = WS_CHILD;

    // using this flag results in very significant reduction in flicker,
    // especially with controls inside the static boxes (as the interior of the
    // box is not redrawn twice), but sometimes results in redraw problems, so
    // optionally allow the old code to continue to use it provided a special
    // system option is turned on
    if ( !wxSystemOptions::GetOptionInt(wxT("msw.window.no-clip-children"))
            || (flags & wxCLIP_CHILDREN) )
        style |= WS_CLIPCHILDREN;

    // it doesn't seem useful to use WS_CLIPSIBLINGS here as we officially
    // don't support overlapping windows and it only makes sense for them and,
    // presumably, gives the system some extra work (to manage more clipping
    // regions), so avoid it altogether


    if ( flags & wxVSCROLL )
        style |= WS_VSCROLL;

    if ( flags & wxHSCROLL )
        style |= WS_HSCROLL;

    const wxBorder border = TranslateBorder(GetBorder(flags));

    // After translation, border is now optimized for the specific version of Windows
    // and theme engine presence.

    // WS_BORDER is only required for wxBORDER_SIMPLE
    if ( border == wxBORDER_SIMPLE )
        style |= WS_BORDER;

    // now deal with ext style if the caller wants it
    if ( exstyle )
    {
        *exstyle = 0;

        if ( flags & wxTRANSPARENT_WINDOW )
            *exstyle |= WS_EX_TRANSPARENT;

        switch ( border )
        {
            default:
            case wxBORDER_DEFAULT:
                wxFAIL_MSG( wxT("unknown border style") );
                wxFALLTHROUGH;

            case wxBORDER_NONE:
            case wxBORDER_SIMPLE:
            case wxBORDER_THEME:
                break;

            case wxBORDER_STATIC:
                *exstyle |= WS_EX_STATICEDGE;
                break;

            case wxBORDER_RAISED:
                *exstyle |= WS_EX_DLGMODALFRAME;
                break;

            case wxBORDER_SUNKEN:
                *exstyle |= WS_EX_CLIENTEDGE;
                style &= ~WS_BORDER;
                break;

//            case wxBORDER_DOUBLE:
//                *exstyle |= WS_EX_DLGMODALFRAME;
//                break;
        }

        // wxUniv doesn't use Windows dialog navigation functions at all
#if !defined(__WXUNIVERSAL__)
        // to make the dialog navigation work with the nested panels we must
        // use this style (top level windows such as dialogs don't need it)
        if ( (flags & wxTAB_TRAVERSAL) && !IsTopLevel() )
        {
            *exstyle |= WS_EX_CONTROLPARENT;
        }
#endif // __WXUNIVERSAL__
    }

    return style;
}

// Setup background and foreground colours correctly
void wxWindowMSW::SetupColours()
{
    if ( GetParent() )
        SetBackgroundColour(GetParent()->GetBackgroundColour());
}

bool wxWindowMSW::IsMouseInWindow() const
{
    // get the mouse position
    POINT pt;
    wxGetCursorPosMSW(&pt);

    // find the window which currently has the cursor and go up the window
    // chain until we find this window - or exhaust it
    HWND hwnd = ::WindowFromPoint(pt);
    while ( hwnd && (hwnd != GetHwnd()) )
        hwnd = ::GetParent(hwnd);

    return hwnd != NULL;
}

void wxWindowMSW::OnInternalIdle()
{
#ifndef HAVE_TRACKMOUSEEVENT
    // Check if we need to send a LEAVE event
    if ( m_mouseInWindow )
    {
        // note that we should generate the leave event whether the window has
        // or doesn't have mouse capture
        if ( !IsMouseInWindow() )
        {
            GenerateMouseLeave();
        }
    }
#endif // !HAVE_TRACKMOUSEEVENT

    wxWindowBase::OnInternalIdle();
}

// Set this window to be the child of 'parent'.
bool wxWindowMSW::Reparent(wxWindowBase *parent)
{
    if ( !wxWindowBase::Reparent(parent) )
        return false;

    HWND hWndChild = GetHwnd();
    HWND hWndParent = GetParent() ? GetWinHwnd(GetParent()) : (HWND)0;

    ::SetParent(hWndChild, hWndParent);

    if ( wxHasWindowExStyle(this, WS_EX_CONTROLPARENT) )
    {
        EnsureParentHasControlParentStyle(GetParent());
    }

    return true;
}

static inline void SendSetRedraw(HWND hwnd, bool on)
{
    ::SendMessage(hwnd, WM_SETREDRAW, (WPARAM)on, 0);
}

void wxWindowMSW::DoFreeze()
{
    if ( !IsShown() )
        return; // no point in freezing hidden window

    SendSetRedraw(GetHwnd(), false);
}

void wxWindowMSW::DoThaw()
{
    if ( !IsShown() )
        return; // hidden windows aren't frozen by DoFreeze

    SendSetRedraw(GetHwnd(), true);

    // we need to refresh everything or otherwise the invalidated area
    // is not going to be repainted
    Refresh();
}

void wxWindowMSW::Refresh(bool eraseBack, const wxRect *rect)
{
    HWND hWnd = GetHwnd();
    if ( hWnd )
    {
        RECT mswRect;
        const RECT *pRect;
        if ( rect )
        {
            wxCopyRectToRECT(*rect, mswRect);
            pRect = &mswRect;
        }
        else
        {
            pRect = NULL;
        }

        UINT flags = RDW_INVALIDATE | RDW_ALLCHILDREN;
        if ( eraseBack )
            flags |= RDW_ERASE;

        ::RedrawWindow(hWnd, pRect, NULL, flags);
    }
}

void wxWindowMSW::Update()
{
    if ( !::UpdateWindow(GetHwnd()) )
    {
        wxLogLastError(wxT("UpdateWindow"));
    }

    // just calling UpdateWindow() is not enough, what we did in our WM_PAINT
    // handler needs to be really drawn right now
    (void)::GdiFlush();
}

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------

#if wxUSE_STATBOX

// we need to lower the sibling static boxes so controls contained within can be
// a drop target
static void AdjustStaticBoxZOrder(wxWindow *parent)
{
    // no sibling static boxes if we have no parent (ie TLW)
    if ( !parent )
        return;

    for ( wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxStaticBox *statbox = wxDynamicCast(node->GetData(), wxStaticBox);
        if ( statbox )
        {
            ::SetWindowPos(GetHwndOf(statbox), HWND_BOTTOM, 0, 0, 0, 0,
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

#else // !wxUSE_STATBOX

static inline void AdjustStaticBoxZOrder(wxWindow * WXUNUSED(parent))
{
}

#endif // wxUSE_STATBOX/!wxUSE_STATBOX

#if wxUSE_DRAG_AND_DROP

void wxWindowMSW::SetDropTarget(wxDropTarget *pDropTarget)
{
    if ( m_dropTarget != 0 ) {
        m_dropTarget->Revoke(m_hWnd);
        delete m_dropTarget;
    }

    m_dropTarget = pDropTarget;
    if ( m_dropTarget != 0 )
    {
        AdjustStaticBoxZOrder(GetParent());
        m_dropTarget->Register(m_hWnd);
    }
}

// old-style file manager drag&drop support: we retain the old-style
// DragAcceptFiles in parallel with SetDropTarget.
void wxWindowMSW::DragAcceptFiles(bool accept)
{
    HWND hWnd = GetHwnd();
    if ( hWnd )
    {
        AdjustStaticBoxZOrder(GetParent());
        ::DragAcceptFiles(hWnd, (BOOL)accept);
    }
}
#endif // wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowMSW::DoSetToolTip(wxToolTip *tooltip)
{
    wxWindowBase::DoSetToolTip(tooltip);

    if ( m_tooltip )
        m_tooltip->SetWindow((wxWindow *)this);
}

#endif // wxUSE_TOOLTIPS

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindowMSW::IsSizeDeferred() const
{
#if wxUSE_DEFERRED_SIZING
    if ( m_pendingPosition != wxDefaultPosition ||
         m_pendingSize     != wxDefaultSize )
        return true;
#endif // wxUSE_DEFERRED_SIZING

    return false;
}

// Get total size
void wxWindowMSW::DoGetSize(int *x, int *y) const
{
#if wxUSE_DEFERRED_SIZING
    // if SetSize() had been called at wx level but not realized at Windows
    // level yet (i.e. EndDeferWindowPos() not called), we still should return
    // the new and not the old position to the other wx code
    if ( m_pendingSize != wxDefaultSize )
    {
        if ( x )
            *x = m_pendingSize.x;
        if ( y )
            *y = m_pendingSize.y;
    }
    else // use current size
#endif // wxUSE_DEFERRED_SIZING
    {
        RECT rect = wxGetWindowRect(GetHwnd());

        if ( x )
            *x = rect.right - rect.left;
        if ( y )
            *y = rect.bottom - rect.top;
    }
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowMSW::DoGetClientSize(int *x, int *y) const
{
#if wxUSE_DEFERRED_SIZING
    if ( m_pendingSize != wxDefaultSize )
    {
        // we need to calculate the client size corresponding to pending size
        //
        // FIXME: Unfortunately this doesn't work correctly for the maximized
        //        top level windows, the returned values are too small (e.g.
        //        under Windows 7 on a 1600*1200 screen with task bar on the
        //        right the pending size for a maximized window is 1538*1200
        //        and WM_NCCALCSIZE returns 1528*1172 even though the correct
        //        client size of such window is 1538*1182). No idea how to fix
        //        it though, setting WS_MAXIMIZE in GWL_STYLE before calling
        //        WM_NCCALCSIZE doesn't help and AdjustWindowRectEx() doesn't
        //        work in this direction neither. So we just have to live with
        //        the slightly wrong results and relayout the window when it
        //        gets finally shown in its maximized state (see #11762).
        RECT rect;
        rect.left = m_pendingPosition.x;
        rect.top = m_pendingPosition.y;
        rect.right = rect.left + m_pendingSize.x;
        rect.bottom = rect.top + m_pendingSize.y;

        ::SendMessage(GetHwnd(), WM_NCCALCSIZE, FALSE, (LPARAM)&rect);

        if ( x )
            *x = rect.right - rect.left;
        if ( y )
            *y = rect.bottom - rect.top;
    }
    else
#endif // wxUSE_DEFERRED_SIZING
    {
        RECT rect = wxGetClientRect(GetHwnd());

        if ( x )
            *x = rect.right;
        if ( y )
            *y = rect.bottom;
    }

    // The size of the client window can't be negative but ::GetClientRect()
    // can return negative size for an extremely small (1x1) window with
    // borders so ensure that we correct it here as having negative sizes is
    // completely unexpected.
    if ( x && *x < 0 )
        *x = 0;
    if ( y && *y < 0 )
        *y = 0;
}

void wxWindowMSW::DoGetPosition(int *x, int *y) const
{
    wxWindow * const parent = GetParent();

    wxPoint pos;
#if wxUSE_DEFERRED_SIZING
    if ( m_pendingPosition != wxDefaultPosition )
    {
        pos = m_pendingPosition;
    }
    else // use current position
#endif // wxUSE_DEFERRED_SIZING
    {
        RECT rect = wxGetWindowRect(GetHwnd());

        // we do the adjustments with respect to the parent only for the "real"
        // children, not for the dialogs/frames
        if ( !IsTopLevel() )
        {
            // In RTL mode, we want the logical left x-coordinate,
            // which would be the physical right x-coordinate.
            ::MapWindowPoints(NULL, parent ? GetHwndOf(parent) : HWND_DESKTOP,
                              (LPPOINT)&rect, 2);
        }

        pos.x = rect.left;
        pos.y = rect.top;
    }

    // we also must adjust by the client area offset: a control which is just
    // under a toolbar could be at (0, 30) in Windows but at (0, 0) in wx
    if ( parent && !IsTopLevel() )
    {
        const wxPoint pt(parent->GetClientAreaOrigin());
        pos.x -= pt.x;
        pos.y -= pt.y;
    }

    if ( x )
        *x = pos.x;
    if ( y )
        *y = pos.y;
}

void wxWindowMSW::DoScreenToClient(int *x, int *y) const
{
    POINT pt;
    if ( x )
        pt.x = *x;
    if ( y )
        pt.y = *y;

    ::ScreenToClient(GetHwnd(), &pt);

    if ( x )
        *x = pt.x;
    if ( y )
        *y = pt.y;
}

void wxWindowMSW::DoClientToScreen(int *x, int *y) const
{
    POINT pt;
    if ( x )
        pt.x = *x;
    if ( y )
        pt.y = *y;

    ::ClientToScreen(GetHwnd(), &pt);

    if ( x )
        *x = pt.x;
    if ( y )
        *y = pt.y;
}

bool
wxWindowMSW::DoMoveSibling(WXHWND hwnd, int x, int y, int width, int height)
{
    // toplevel window's coordinates are mirrored if the TLW is a child of another
    // RTL window and changing width without moving the position would enlarge the
    // window in the wrong direction, so we need to adjust for it
    if ( IsTopLevel() )
    {
        // note that this may be different from GetParent() for wxDialogs
        HWND tlwParent = ::GetParent((HWND)hwnd);
        if ( tlwParent && (::GetWindowLong(tlwParent, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0 )
        {
            RECT old;
            ::GetWindowRect((HWND) hwnd, &old);
            if ( old.left == x && old.right - old.left != width )
            {
                x -= width - (old.right - old.left);
            }
            // else: not a simple resize
        }
    }

#if wxUSE_DEFERRED_SIZING
    else if ( MSWIsPositionDirectlySupported(x, y) )
    {
        // if our parent had prepared a defer window handle for us, use it
        wxWindowMSW * const parent = GetParent();

        HDWP hdwp = parent ? (HDWP)parent->m_hDWP : NULL;
        if ( hdwp )
        {
            hdwp = ::DeferWindowPos(hdwp, (HWND)hwnd, NULL, x, y, width, height,
                                    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
            if ( !hdwp )
            {
                wxLogLastError(wxT("DeferWindowPos"));
            }
        }

        if ( parent )
        {
            // hdwp must be updated as it may have been changed
            parent->m_hDWP = (WXHANDLE)hdwp;
        }

        if ( hdwp )
        {
            // did deferred move, remember new coordinates of the window as they're
            // different from what Windows would return for it
            return true;
        }

        // otherwise (or if deferring failed) move the window in place immediately
    }
#endif // wxUSE_DEFERRED_SIZING

    MSWMoveWindowToAnyPosition(hwnd, x, y, width, height, IsShown());

    // if wxUSE_DEFERRED_SIZING, indicates that we didn't use deferred move,
    // ignored otherwise
    return false;
}

void wxWindowMSW::MSWMoveWindowToAnyPosition(WXHWND hwnd, int x, int y, int width, int height, bool bRepaint)
{
    bool scroll = GetParent() && !MSWIsPositionDirectlySupported(x, y);

    if ( scroll )
    {
        // scroll to the actual position (looks like there is no need to Freeze() the parent)
        ::ScrollWindow(GetHwndOf(GetParent()), -x, -y, NULL, NULL);
    }

    // move to relative coordinates
    if ( !::MoveWindow(hwnd, (scroll ? 0 : x), (scroll ? 0 : y), width, height, bRepaint) )
    {
        wxLogLastError(wxT("MoveWindow"));
    }

    if ( scroll )
    {
        // scroll back
        ::ScrollWindow(GetHwndOf(GetParent()), x, y, NULL, NULL);
    }
}

void wxWindowMSW::DoMoveWindow(int x, int y, int width, int height)
{
    // TODO: is this consistent with other platforms?
    // Still, negative width or height shouldn't be allowed
    if (width < 0)
        width = 0;
    if (height < 0)
        height = 0;

    if ( DoMoveSibling(m_hWnd, x, y, width, height) )
    {
#if wxUSE_DEFERRED_SIZING
        m_pendingPosition = wxPoint(x, y);
        m_pendingSize = wxSize(width, height);
    }
    else // window was moved immediately, without deferring it
    {
        m_pendingPosition = wxDefaultPosition;
        m_pendingSize = wxDefaultSize;
#endif // wxUSE_DEFERRED_SIZING
    }
}

// set the size of the window: if the dimensions are positive, just use them,
// but if any of them is equal to -1, it means that we must find the value for
// it ourselves (unless sizeFlags contains wxSIZE_ALLOW_MINUS_ONE flag, in
// which case -1 is a valid value for x and y)
//
// If sizeFlags contains wxSIZE_AUTO_WIDTH/HEIGHT flags (default), we calculate
// the width/height to best suit our contents, otherwise we reuse the current
// width/height
void wxWindowMSW::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // get the current size and position...
    int currentX, currentY;
    int currentW, currentH;

    GetPosition(&currentX, &currentY);
    GetSize(&currentW, &currentH);

    if ( x == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        x = currentX;
    if ( y == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        y = currentY;

    wxSize size = wxDefaultSize;
    if ( width == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
        {
            size = GetBestSize();
            width = size.x;
        }
        else
        {
            // just take the current one
            width = currentW;
        }
    }

    if ( height == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
        {
            if ( size.x == wxDefaultCoord )
            {
                size = GetBestSize();
            }
            //else: already called GetBestSize() above

            height = size.y;
        }
        else
        {
            // just take the current one
            height = currentH;
        }
    }

    // ... and don't do anything (avoiding flicker) if it's already ok unless
    // we're forced to resize the window
    if ( !(sizeFlags & wxSIZE_FORCE) )
    {
        if ( width == currentW && height == currentH )
        {
            // We need to send wxSizeEvent ourselves because Windows won't do
            // it if the size doesn't change.
            if ( sizeFlags & wxSIZE_FORCE_EVENT )
            {
                wxSizeEvent event( wxSize(width,height), GetId() );
                event.SetEventObject( this );
                HandleWindowEvent( event );
            }

            // Still call DoMoveWindow() below if we need to change the
            // position, otherwise we're done.
            if ( x == currentX && y == currentY )
                return;
        }
    }

    AdjustForParentClientOrigin(x, y, sizeFlags);

    DoMoveWindow(x, y, width, height);
}

void wxWindowMSW::DoSetClientSize(int width, int height)
{
    // setting the client size is less obvious than it could have been
    // because in the result of changing the total size the window scrollbar
    // may [dis]appear and/or its menubar may [un]wrap (and AdjustWindowRect()
    // doesn't take neither into account) and so the client size will not be
    // correct as the difference between the total and client size changes --
    // so we keep changing it until we get it right
    //
    // normally this loop shouldn't take more than 3 iterations (usually 1 but
    // if scrollbars [dis]appear as the result of the first call, then 2 and it
    // may become 3 if the window had 0 size originally and so we didn't
    // calculate the scrollbar correction correctly during the first iteration)
    // but just to be on the safe side we check for it instead of making it an
    // "infinite" loop (i.e. leaving break inside as the only way to get out)
    for ( int i = 0; i < 4; i++ )
    {
        RECT rectClient;
        ::GetClientRect(GetHwnd(), &rectClient);

        // if the size is already ok, stop here (NB: rectClient.left = top = 0)
        if ( (rectClient.right == width || width == wxDefaultCoord) &&
             (rectClient.bottom == height || height == wxDefaultCoord) )
        {
            break;
        }

        // Find the difference between the entire window (title bar and all)
        // and the client area; add this to the new client size to move the
        // window
        RECT rectWin;
        ::GetWindowRect(GetHwnd(), &rectWin);

        const int widthWin = rectWin.right - rectWin.left,
                  heightWin = rectWin.bottom - rectWin.top;

        if ( IsTopLevel() )
        {
            // toplevel window's coordinates are mirrored if the TLW is a child of another
            // RTL window and changing width without moving the position would enlarge the
            // window in the wrong direction, so we need to adjust for it

            // note that this may be different from GetParent() for wxDialogs
            HWND tlwParent = ::GetParent(GetHwnd());
            if ( tlwParent && (::GetWindowLong(tlwParent, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0 )
            {
                const int diffWidth = width - (rectClient.right - rectClient.left);
                rectWin.left -= diffWidth;
                rectWin.right -= diffWidth;
            }
        }
        else
        {
            // MoveWindow positions the child windows relative to the parent, so
            // adjust if necessary
            wxWindow *parent = GetParent();
            if ( parent )
            {
                ::ScreenToClient(GetHwndOf(parent), (POINT *)&rectWin);
            }
        }

        // don't call DoMoveWindow() because we want to move window immediately
        // and not defer it here as otherwise the value returned by
        // GetClient/WindowRect() wouldn't change as the window wouldn't be
        // really resized
        MSWMoveWindowToAnyPosition(GetHwnd(), rectWin.left, rectWin.top,
                                   width + widthWin - rectClient.right,
                                   height + heightWin - rectClient.bottom, true);
    }
}

wxSize wxWindowMSW::DoGetBorderSize() const
{
    wxCoord border;
    switch ( GetBorder() )
    {
        case wxBORDER_STATIC:
        case wxBORDER_SIMPLE:
            border = 1;
            break;

        case wxBORDER_SUNKEN:
        case wxBORDER_THEME:
            border = 2;
            break;

        case wxBORDER_RAISED:
            border = 3;
            break;

        default:
            wxFAIL_MSG( wxT("unknown border style") );
            wxFALLTHROUGH;

        case wxBORDER_NONE:
            border = 0;
    }

    return 2*wxSize(border, border);
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindowMSW::GetCharHeight() const
{
    return wxGetTextMetrics(this).tmHeight;
}

int wxWindowMSW::GetCharWidth() const
{
    // +1 is needed because Windows apparently adds it when calculating the
    // dialog units size in pixels
#if wxDIALOG_UNIT_COMPATIBILITY
    return wxGetTextMetrics(this).tmAveCharWidth;
#else
    return wxGetTextMetrics(this).tmAveCharWidth + 1;
#endif
}

void wxWindowMSW::DoGetTextExtent(const wxString& string,
                                  int *x, int *y,
                                  int *descent,
                                  int *externalLeading,
                                  const wxFont *fontToUse) const
{
    // ensure we work with a valid font
    wxFont font;
    if ( !fontToUse || !fontToUse->IsOk() )
        font = GetFont();
    else
        font = *fontToUse;

    wxCHECK_RET( font.IsOk(), wxT("invalid font in GetTextExtent()") );

    const wxWindow* win = static_cast<const wxWindow*>(this);
    wxTextMeasure txm(win, &font);
    txm.GetTextExtent(string, x, y, descent, externalLeading);
}

// ---------------------------------------------------------------------------
// popup menu
// ---------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE

// yield for WM_COMMAND events only, i.e. process all WM_COMMANDs in the queue
// immediately, without waiting for the next event loop iteration
//
// NB: this function should probably be made public later as it can almost
//     surely replace wxYield() elsewhere as well
static void wxYieldForCommandsOnly()
{
    // peek all WM_COMMANDs (it will always return WM_QUIT too but we don't
    // want to process it here)
    MSG msg;
    while ( ::PeekMessage(&msg, (HWND)0, WM_COMMAND, WM_COMMAND, PM_REMOVE) )
    {
        if ( msg.message == WM_QUIT )
        {
            // if we retrieved a WM_QUIT, insert back into the message queue.
            ::PostQuitMessage(0);
            break;
        }

        // luckily (as we don't have access to wxEventLoopImpl method from here
        // anyhow...) we don't need to pre process WM_COMMANDs so dispatch it
        // immediately
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

bool wxWindowMSW::DoPopupMenu(wxMenu *menu, int x, int y)
{
    wxPoint pt;
    if ( x == wxDefaultCoord && y == wxDefaultCoord )
    {
        pt = wxGetMousePosition();
    }
    else
    {
        pt = ClientToScreen(wxPoint(x, y));
    }

    // using TPM_RECURSE allows us to show a popup menu while another menu
    // is opened which can be useful and is supported by the other
    // platforms, so allow it under Windows too
    UINT flags = TPM_RIGHTBUTTON | TPM_RECURSE;

    ::TrackPopupMenu(GetHmenuOf(menu), flags, pt.x, pt.y, 0, GetHwnd(), NULL);

    // we need to do it right now as otherwise the events are never going to be
    // sent to wxCurrentPopupMenu from HandleCommand()
    //
    // note that even eliminating (ugly) wxCurrentPopupMenu global wouldn't
    // help and we'd still need wxYieldForCommandsOnly() as the menu may be
    // destroyed as soon as we return (it can be a local variable in the caller
    // for example) and so we do need to process the event immediately
    wxYieldForCommandsOnly();

    return true;
}

#endif // wxUSE_MENUS_NATIVE

// ---------------------------------------------------------------------------
// menu events
// ---------------------------------------------------------------------------

#if wxUSE_MENUS && !defined(__WXUNIVERSAL__)

bool
wxWindowMSW::HandleMenuSelect(WXWORD nItem, WXWORD flags, WXHMENU hMenu)
{
    // Ignore the special messages generated when the menu is closed (this is
    // the only case when the flags are set to -1), in particular don't clear
    // the help string in the status bar when this happens as it had just been
    // restored by the base class code.
    if ( !hMenu && flags == 0xffff )
        return false;

    // sign extend to int from unsigned short we get from Windows
    int item = (signed short)nItem;

    // WM_MENUSELECT is generated for both normal items and menus, including
    // the top level menus of the menu bar, which can't be represented using
    // any valid identifier in wxMenuEvent so use an otherwise unused value for
    // them
    if ( flags & (MF_POPUP | MF_SEPARATOR) )
        item = wxID_NONE;

    wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, item);
    if ( wxMenu::ProcessMenuEvent(MSWFindMenuFromHMENU(hMenu), event, this) )
        return true;

    // by default, i.e. if the event wasn't handled above, clear the status bar
    // text when an item which can't have any associated help string in wx API
    // is selected
    if ( item == wxID_NONE )
    {
        wxFrame *frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
        if ( frame )
            frame->DoGiveHelp(wxEmptyString, true);
    }

    return false;
}

bool
wxWindowMSW::DoSendMenuOpenCloseEvent(wxEventType evtType, wxMenu* menu)
{
    wxMenuEvent event(evtType, menu && !menu->IsAttached() ? wxID_ANY : 0, menu);

    return wxMenu::ProcessMenuEvent(menu, event, this);
}

bool wxWindowMSW::HandleMenuPopup(wxEventType evtType, WXHMENU hMenu)
{
    wxMenu* const menu = MSWFindMenuFromHMENU(hMenu);

    return DoSendMenuOpenCloseEvent(evtType, menu);
}

wxMenu* wxWindowMSW::MSWFindMenuFromHMENU(WXHMENU hMenu)
{
    if ( wxCurrentPopupMenu && wxCurrentPopupMenu->GetHMenu() == hMenu )
        return wxCurrentPopupMenu;

    return NULL;
}

#endif // wxUSE_MENUS && !defined(__WXUNIVERSAL__)

// ===========================================================================
// pre/post message processing
// ===========================================================================

WXLRESULT wxWindowMSW::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    WXLRESULT rc;
    if ( m_oldWndProc )
        rc = ::CallWindowProc(m_oldWndProc, GetHwnd(), nMsg, wParam, lParam);
    else
        rc = ::DefWindowProc(GetHwnd(), nMsg, wParam, lParam);

    // Special hack used by wxTextEntry auto-completion only: this event is
    // sent after the normal keyboard processing so that its handler could use
    // the updated contents of the text control, after taking the key that was
    // pressed into account.
    if ( nMsg == WM_CHAR )
    {
        wxKeyEvent event(CreateCharEvent(wxEVT_AFTER_CHAR, wParam, lParam));
        HandleWindowEvent(event);
    }

    return rc;
}

bool wxWindowMSW::MSWProcessMessage(WXMSG* pMsg)
{
    // wxUniversal implements tab traversal itself
#ifndef __WXUNIVERSAL__
    // Notice that we check for both wxTAB_TRAVERSAL and WS_EX_CONTROLPARENT
    // being set here. While normally the latter should always be set if the
    // former is, doing it like this also works if there is ever a bug that
    // results in wxTAB_TRAVERSAL being set but not WS_EX_CONTROLPARENT as we
    // must not call IsDialogMessage() then, it would simply hang (see #15458).
    if ( m_hWnd &&
            HasFlag(wxTAB_TRAVERSAL) &&
                wxHasWindowExStyle(this, WS_EX_CONTROLPARENT) )
    {
        // intercept dialog navigation keys
        MSG *msg = (MSG *)pMsg;

        // here we try to do all the job which ::IsDialogMessage() usually does
        // internally
        if ( msg->message == WM_KEYDOWN )
        {
            bool bCtrlDown = wxIsCtrlDown();
            bool bShiftDown = wxIsShiftDown();

            // WM_GETDLGCODE: ask the control if it wants the key for itself,
            // don't process it if it's the case (except for Ctrl-Tab/Enter
            // combinations which are always processed)
            LONG lDlgCode = ::SendMessage(msg->hwnd, WM_GETDLGCODE, 0, 0);

            // surprisingly, DLGC_WANTALLKEYS bit mask doesn't contain the
            // DLGC_WANTTAB nor DLGC_WANTARROWS bits although, logically,
            // it, of course, implies them
            if ( lDlgCode & DLGC_WANTALLKEYS )
            {
                lDlgCode |= DLGC_WANTTAB | DLGC_WANTARROWS;
            }

            bool bForward = true,
                 bWindowChange = false,
                 bFromTab = false;

            // should we process this message specially?
            bool bProcess = true;
            switch ( msg->wParam )
            {
                case VK_TAB:
                    if ( (lDlgCode & DLGC_WANTTAB) && !bCtrlDown )
                    {
                        // let the control have the TAB
                        bProcess = false;
                    }
                    else // use it for navigation
                    {
                        // Ctrl-Tab cycles thru notebook pages
                        bWindowChange = bCtrlDown;
                        bForward = !bShiftDown;
                        bFromTab = true;
                    }
                    break;

                case VK_UP:
                case VK_LEFT:
                    if ( (lDlgCode & DLGC_WANTARROWS) || bCtrlDown )
                        bProcess = false;
                    else
                        bForward = false;
                    break;

                case VK_DOWN:
                case VK_RIGHT:
                    if ( (lDlgCode & DLGC_WANTARROWS) || bCtrlDown )
                        bProcess = false;
                    break;

                case VK_PRIOR:
                    bForward = false;
                    wxFALLTHROUGH;

                case VK_NEXT:
                    // we treat PageUp/Dn as arrows because chances are that
                    // a control which needs arrows also needs them for
                    // navigation (e.g. wxTextCtrl, wxListCtrl, ...)
                    if ( (lDlgCode & DLGC_WANTARROWS) && !bCtrlDown )
                        bProcess = false;
                    else // OTOH Ctrl-PageUp/Dn works as [Shift-]Ctrl-Tab
                        bWindowChange = true;
                    break;

                case VK_RETURN:
                    {
#if wxUSE_BUTTON
                        // currently active button should get enter press even
                        // if there is a default button elsewhere so check if
                        // this window is a button first
                        wxButton *btn = NULL;
                        if ( lDlgCode & DLGC_DEFPUSHBUTTON )
                        {
                            // let IsDialogMessage() handle this for all
                            // buttons except the owner-drawn ones which it
                            // just seems to ignore
                            long style = ::GetWindowLong(msg->hwnd, GWL_STYLE);
                            if ( (style & BS_OWNERDRAW) == BS_OWNERDRAW )
                            {
                                btn = wxDynamicCast
                                      (
                                        wxFindWinFromHandle(msg->hwnd),
                                        wxButton
                                      );
                            }
                        }
                        else // not a button itself, do we have default button?
                        {
                            // check if this window or any of its ancestors
                            // wants the message for itself (we always reserve
                            // Ctrl-Enter for dialog navigation though)
                            wxWindow *win = this;
                            if ( !bCtrlDown )
                            {
                                // this will contain the dialog code of this
                                // window and all of its parent windows in turn
                                LONG lDlgCode2 = lDlgCode;

                                while ( win )
                                {
                                    if ( lDlgCode2 & DLGC_WANTMESSAGE )
                                    {
                                        // as it wants to process Enter itself,
                                        // don't call IsDialogMessage() which
                                        // would consume it
                                        return false;
                                    }

                                    // don't propagate keyboard messages beyond
                                    // the first top level window parent
                                    if ( win->IsTopLevel() )
                                        break;

                                    win = win->GetParent();

                                    lDlgCode2 = ::SendMessage
                                                  (
                                                    GetHwndOf(win),
                                                    WM_GETDLGCODE,
                                                    0,
                                                    0
                                                  );
                                }
                            }

                            btn = MSWGetDefaultButtonFor(win);
                        }

                        if ( MSWClickButtonIfPossible(btn) )
                            return true;

                        // This "Return" key press won't be actually used for
                        // navigation so don't generate wxNavigationKeyEvent
                        // for it but still pass it to IsDialogMessage() as it
                        // may handle it in some other way (e.g. by playing the
                        // default error sound).
                        bProcess = false;

#endif // wxUSE_BUTTON

                    }
                    break;

                default:
                    bProcess = false;
            }

            if ( bProcess )
            {
                wxNavigationKeyEvent event;
                event.SetDirection(bForward);
                event.SetWindowChange(bWindowChange);
                event.SetFromTab(bFromTab);
                event.SetEventObject(this);

                if ( HandleWindowEvent(event) )
                {
                    // as we don't call IsDialogMessage(), which would take of
                    // this by default, we need to manually send this message
                    // so that controls can change their UI state if needed
                    MSWUpdateUIState(UIS_CLEAR, UISF_HIDEFOCUS);

                    return true;
                }
            }
        }

        if ( MSWSafeIsDialogMessage(msg) )
        {
            // IsDialogMessage() did something...
            return true;
        }
    }
#endif // __WXUNIVERSAL__

#if wxUSE_TOOLTIPS
    if ( m_tooltip )
    {
        // relay mouse move events to the tooltip control
        MSG *msg = (MSG *)pMsg;
        if ( msg->message == WM_MOUSEMOVE )
            wxToolTip::RelayEvent(pMsg);
    }
#endif // wxUSE_TOOLTIPS

    return false;
}

bool wxWindowMSW::MSWTranslateMessage(WXMSG* pMsg)
{
#if wxUSE_ACCEL && !defined(__WXUNIVERSAL__)
    return m_acceleratorTable.Translate(this, pMsg);
#else
    (void) pMsg;
    return false;
#endif // wxUSE_ACCEL
}

bool wxWindowMSW::MSWShouldPreProcessMessage(WXMSG* WXUNUSED(msg))
{
    // We don't have any reason to not preprocess messages at this level.
    return true;
}

#ifndef __WXUNIVERSAL__

bool wxWindowMSW::MSWSafeIsDialogMessage(WXMSG* msg)
{
    // don't let IsDialogMessage() get VK_ESCAPE as it _always_ eats the
    // message even when there is no cancel button and when the message is
    // needed by the control itself: in particular, it prevents the tree in
    // place edit control from being closed with Escape in a dialog
    if ( msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE )
    {
        return false;
    }

    // ::IsDialogMessage() is broken and may sometimes hang the application by
    // going into an infinite loop when it tries to find the control to give
    // focus to when Alt-<key> is pressed, so we try to detect [some of] the
    // situations when this may happen and not call it then
    if ( msg->message == WM_SYSCHAR )
    {
        HWND hwndFocus = ::GetFocus();

        // if the currently focused window itself has WS_EX_CONTROLPARENT style,
        // ::IsDialogMessage() will also enter an infinite loop, because it will
        // recursively check the child windows but not the window itself and so if
        // none of the children accepts focus it loops forever (as it only stops
        // when it gets back to the window it started from)
        //
        // while it is very unusual that a window with WS_EX_CONTROLPARENT
        // style has the focus, it can happen. One such possibility is if
        // all windows are either toplevel, wxDialog, wxPanel or static
        // controls and no window can actually accept keyboard input.
        if ( ::GetWindowLong(hwndFocus, GWL_EXSTYLE) & WS_EX_CONTROLPARENT )
        {
            // pessimistic by default
            bool canSafelyCallIsDlgMsg = false;
            for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                  node;
                  node = node->GetNext() )
            {
                wxWindow * const win = node->GetData();
                if ( win->CanAcceptFocus() &&
                        !wxHasWindowExStyle(win, WS_EX_CONTROLPARENT) )
                {
                    // it shouldn't hang...
                    canSafelyCallIsDlgMsg = true;

                    break;
                }
            }

            if ( !canSafelyCallIsDlgMsg )
                return false;
        }

        // ::IsDialogMessage() can enter in an infinite loop when the
        // currently focused window is disabled or hidden and its
        // parent has WS_EX_CONTROLPARENT style, so don't call it in
        // this case
        while ( hwndFocus )
        {
            if ( !::IsWindowEnabled(hwndFocus) ||
                    !::IsWindowVisible(hwndFocus) )
            {
                // it would enter an infinite loop if we do this!
                return false;
            }

            if ( !(::GetWindowLong(hwndFocus, GWL_STYLE) & WS_CHILD) )
            {
                // it's a top level window, don't go further -- e.g. even
                // if the parent of a dialog is disabled, this doesn't
                // break navigation inside the dialog
                break;
            }

            hwndFocus = ::GetParent(hwndFocus);
        }
    }

    return ::IsDialogMessage(GetHwnd(), msg) != 0;
}

#endif // __WXUNIVERSAL__

/* static */
wxButton* wxWindowMSW::MSWGetDefaultButtonFor(wxWindow* win)
{
#if wxUSE_BUTTON
    win = wxGetTopLevelParent(win);

    wxTopLevelWindow *const tlw = wxDynamicCast(win, wxTopLevelWindow);
    if ( tlw )
        return wxDynamicCast(tlw->GetDefaultItem(), wxButton);
#endif // wxUSE_BUTTON

    return NULL;
}

/* static */
bool wxWindowMSW::MSWClickButtonIfPossible(wxButton* btn)
{
#if wxUSE_BUTTON
    if ( btn && btn->IsEnabled() && btn->IsShownOnScreen() )
    {
        btn->MSWCommand(BN_CLICKED, 0 /* unused */);
        return true;
    }
#endif // wxUSE_BUTTON

    wxUnusedVar(btn);

    return false;
}

// ---------------------------------------------------------------------------
// message params unpackers
// ---------------------------------------------------------------------------

void wxWindowMSW::UnpackCommand(WXWPARAM wParam, WXLPARAM lParam,
                             WORD *id, WXHWND *hwnd, WORD *cmd)
{
    *id = LOWORD(wParam);
    *hwnd = (WXHWND)lParam;
    *cmd = HIWORD(wParam);
}

void wxWindowMSW::UnpackActivate(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *state, WXWORD *minimized, WXHWND *hwnd)
{
    *state = LOWORD(wParam);
    *minimized = HIWORD(wParam);
    *hwnd = (WXHWND)lParam;
}

void wxWindowMSW::UnpackScroll(WXWPARAM wParam, WXLPARAM lParam,
                            WXWORD *code, WXWORD *pos, WXHWND *hwnd)
{
    *code = LOWORD(wParam);
    *pos = HIWORD(wParam);
    *hwnd = (WXHWND)lParam;
}

void wxWindowMSW::UnpackCtlColor(WXWPARAM wParam, WXLPARAM lParam,
                                 WXHDC *hdc, WXHWND *hwnd)
{
    *hwnd = (WXHWND)lParam;
    *hdc = (WXHDC)wParam;
}

void wxWindowMSW::UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                                WXWORD *item, WXWORD *flags, WXHMENU *hmenu)
{
    *item = (WXWORD)wParam;
    *flags = HIWORD(wParam);
    *hmenu = (WXHMENU)lParam;
}

// ---------------------------------------------------------------------------
// Main wxWidgets window proc and the window proc for wxWindow
// ---------------------------------------------------------------------------

// Hook for new window just as it's being created, when the window isn't yet
// associated with the handle
static wxWindowMSW *gs_winBeingCreated = NULL;

// implementation of wxWindowCreationHook class: it just sets gs_winBeingCreated to the
// window being created and insures that it's always unset back later
wxWindowCreationHook::wxWindowCreationHook(wxWindowMSW *winBeingCreated)
{
    gs_winBeingCreated = winBeingCreated;
}

wxWindowCreationHook::~wxWindowCreationHook()
{
    gs_winBeingCreated = NULL;
}

// Main window proc
LRESULT WXDLLEXPORT APIENTRY
wxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // trace all messages: useful for the debugging but noticeably slows down
    // the code so don't do it by default
#if wxDEBUG_LEVEL >= 2
    // We have to do this inside a helper function as wxLogTrace() constructs
    // an object internally, but objects can't be used in functions using __try
    // (expanded from wxSEH_TRY below) with MSVC.
    wxTraceMSWMessage(hWnd, message, wParam, lParam);
#endif // wxDEBUG_LEVEL >= 2

    wxWindowMSW *wnd = wxFindWinFromHandle(hWnd);

    // when we get the first message for the HWND we just created, we associate
    // it with wxWindow stored in gs_winBeingCreated
    if ( !wnd && gs_winBeingCreated )
    {
        wxAssociateWinWithHandle(hWnd, gs_winBeingCreated);
        wnd = gs_winBeingCreated;
        gs_winBeingCreated = NULL;
        wnd->SetHWND((WXHWND)hWnd);
    }

    LRESULT rc;

    // We have to catch unhandled Win32 exceptions here because otherwise they
    // would be simply lost if we're called from a kernel callback (as it
    // happens when we process WM_PAINT, for example under WOW64: the 32 bit
    // exceptions can't pass through the 64 bit kernel in this case and so are
    // mostly just suppressed, although the exact behaviour differs across
    // Windows versions, see the "Remarks" section of WindowProc documentation
    // at https://msdn.microsoft.com/en-us/library/ms633573.aspx
    wxSEH_TRY
    {
        if ( wnd && wxGUIEventLoop::AllowProcessing(wnd) )
            rc = wnd->MSWWindowProc(message, wParam, lParam);
        else
            rc = ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    wxSEH_HANDLE(0)

    return rc;
}

bool
wxWindowMSW::MSWHandleMessage(WXLRESULT *result,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
    // did we process the message?
    bool processed = false;

    // the return value
    union
    {
        bool        allow;
        WXLRESULT   result;
        WXHBRUSH    hBrush;
    } rc;

    // for most messages we should return 0 when we do process the message
    rc.result = 0;

    switch ( message )
    {
        case WM_CREATE:
            {
                bool mayCreate;
                processed = HandleCreate((WXLPCREATESTRUCT)lParam, &mayCreate);
                if ( processed )
                {
                    // return 0 to allow window creation
                    rc.result = mayCreate ? 0 : -1;
                }
            }
            break;

        case WM_DESTROY:
            // never set processed to true and *always* pass WM_DESTROY to
            // DefWindowProc() as Windows may do some internal cleanup when
            // processing it and failing to pass the message along may cause
            // memory and resource leaks!
            (void)HandleDestroy();
            break;

        case WM_SIZE:
            processed = HandleSize(LOWORD(lParam), HIWORD(lParam), wParam);
            break;

        case WM_MOVE:
            processed = HandleMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;

        case WM_MOVING:
            {
                LPRECT pRect = (LPRECT)lParam;
                wxRect rect;
                rect.SetLeft(pRect->left);
                rect.SetTop(pRect->top);
                rect.SetRight(pRect->right);
                rect.SetBottom(pRect->bottom);
                processed = HandleMoving(rect);
                if (processed) {
                    pRect->left = rect.GetLeft();
                    pRect->top = rect.GetTop();
                    pRect->right = rect.GetRight();
                    pRect->bottom = rect.GetBottom();
                }
            }
            break;

        case WM_ENTERSIZEMOVE:
            {
                processed = HandleEnterSizeMove();
            }
            break;

        case WM_EXITSIZEMOVE:
            {
                processed = HandleExitSizeMove();
            }
            break;

        case WM_SIZING:
            {
                LPRECT pRect = (LPRECT)lParam;
                wxRect rect;
                rect.SetLeft(pRect->left);
                rect.SetTop(pRect->top);
                rect.SetRight(pRect->right);
                rect.SetBottom(pRect->bottom);
                processed = HandleSizing(rect);
                if (processed) {
                    pRect->left = rect.GetLeft();
                    pRect->top = rect.GetTop();
                    pRect->right = rect.GetRight();
                    pRect->bottom = rect.GetBottom();
                }
            }
            break;

        case WM_ACTIVATEAPP:
            // This implicitly sends a wxEVT_ACTIVATE_APP event
            wxTheApp->SetActive(wParam != 0, FindFocus());
            break;

        case WM_ACTIVATE:
            {
                WXWORD state, minimized;
                WXHWND hwnd;
                UnpackActivate(wParam, lParam, &state, &minimized, &hwnd);

                processed = HandleActivate(state, minimized != 0, (WXHWND)hwnd);
            }
            break;

        case WM_SETFOCUS:
            processed = HandleSetFocus((WXHWND)wParam);
            break;

        case WM_KILLFOCUS:
            processed = HandleKillFocus((WXHWND)wParam);
            break;

        case WM_PRINTCLIENT:
            processed = HandlePrintClient((WXHDC)wParam);
            break;

        case WM_PAINT:
            if ( wParam )
            {
                wxPaintDCEx dc((wxWindow *)this, (WXHDC)wParam);

                processed = HandlePaint();
            }
            else // no DC given
            {
                processed = HandlePaint();
            }
            break;

        case WM_CLOSE:
#ifdef __WXUNIVERSAL__
            // Universal uses its own wxFrame/wxDialog, so we don't receive
            // close events unless we have this.
            Close();
#endif // __WXUNIVERSAL__

            // don't let the DefWindowProc() destroy our window - we'll do it
            // ourselves in ~wxWindow
            processed = true;
            rc.result = TRUE;
            break;

        case WM_SHOWWINDOW:
            processed = HandleShow(wParam != 0, (int)lParam);
            break;

        case WM_MOUSEMOVE:
            processed = HandleMouseMove(GET_X_LPARAM(lParam),
                                        GET_Y_LPARAM(lParam),
                                        wParam);
            break;

#ifdef HAVE_TRACKMOUSEEVENT
        case WM_MOUSELEAVE:
            // filter out excess WM_MOUSELEAVE events sent after PopupMenu()
            // (on XP at least)
            if ( m_mouseInWindow )
            {
                GenerateMouseLeave();
            }

            // always pass processed back as false, this allows the window
            // manager to process the message too.  This is needed to
            // ensure windows XP themes work properly as the mouse moves
            // over widgets like buttons. So don't set processed to true here.
            break;
#endif // HAVE_TRACKMOUSEEVENT

#if wxUSE_MOUSEWHEEL
        case WM_MOUSEWHEEL:
            processed = HandleMouseWheel(wxMOUSE_WHEEL_VERTICAL, wParam, lParam);
            break;

        case WM_MOUSEHWHEEL:
            processed = HandleMouseWheel(wxMOUSE_WHEEL_HORIZONTAL, wParam, lParam);
            break;
#endif // wxUSE_MOUSEWHEEL

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
            {
                int x = GET_X_LPARAM(lParam),
                    y = GET_Y_LPARAM(lParam);

                wxWindowMSW *win = this;

                processed = win->HandleMouseEvent(message, x, y, wParam);

                // if the app didn't eat the event, handle it in the default
                // way, that is by giving this window the focus
                if ( !processed )
                {
                    // for the standard classes their WndProc sets the focus to
                    // them anyhow and doing it from here results in some weird
                    // problems, so don't do it for them (unnecessary anyhow)
                    if ( !win->IsOfStandardClass() )
                    {
                        if ( message == WM_LBUTTONDOWN && win->IsFocusable() )
                            win->SetFocus();
                    }
                }
            }
            break;


        case WM_COMMAND:
            {
                WORD id, cmd;
                WXHWND hwnd;
                UnpackCommand(wParam, lParam, &id, &hwnd, &cmd);

                processed = HandleCommand(id, cmd, hwnd);
            }
            break;

        case WM_NOTIFY:
            processed = HandleNotify((int)wParam, lParam, &rc.result);
            break;

            // for these messages we must return true if process the message
#ifdef WM_DRAWITEM
        case WM_DRAWITEM:
            processed = MSWOnDrawItem(wParam, (WXDRAWITEMSTRUCT *)lParam);
            if ( processed )
                rc.result = TRUE;
            break;

        case WM_MEASUREITEM:
            processed = MSWOnMeasureItem(wParam, (WXMEASUREITEMSTRUCT *)lParam);
            if ( processed )
                rc.result = TRUE;
            break;
#endif // defined(WM_DRAWITEM)

        case WM_GETDLGCODE:
            if ( !IsOfStandardClass() || HasFlag(wxWANTS_CHARS) )
            {
                // Get current input processing flags to retain flags like DLGC_HASSETSEL, etc.
                rc.result = MSWDefWindowProc(WM_GETDLGCODE, 0, 0);
                // we always want to get the char events
                rc.result |= DLGC_WANTCHARS;

                if ( HasFlag(wxWANTS_CHARS) )
                {
                    // in fact, we want everything
                    rc.result |= DLGC_WANTARROWS |
                                 DLGC_WANTTAB |
                                 DLGC_WANTALLKEYS;
                }

                // Message is marked as processed so MSWDefWindowProc() will not be called once more.
                processed = true;
            }
            //else: get the dlg code from the DefWindowProc()
            break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            // Generate the key down event in any case.
            m_lastKeydownProcessed = HandleKeyDown((WORD) wParam, lParam);
            if ( m_lastKeydownProcessed )
            {
                // If it was processed by an event handler, we stop here,
                // notably we intentionally don't generate char event then.
                processed = true;
            }
            else // key down event not handled
            {
                // Examine the event to decide whether we need to generate a
                // char event for it ourselves or let Windows do it. Window
                // mostly only does it for the keys which produce printable
                // characters (although there are exceptions, e.g. VK_ESCAPE or
                // VK_BACK (but not VK_DELETE)) while we do it for all keys
                // except the modifier ones (the wisdom of this is debatable
                // but by now this decision is enshrined forever due to
                // backwards compatibility).
                switch ( wParam )
                {
                    // No wxEVT_CHAR events are generated for these keys at all.
                    case VK_SHIFT:
                    case VK_CONTROL:
                    case VK_MENU:
                    case VK_CAPITAL:
                    case VK_NUMLOCK:
                    case VK_SCROLL:

                    // Windows will send us WM_CHAR for these ones so we'll
                    // generate wxEVT_CHAR for them later when we get it.
                    case VK_ESCAPE:
                    case VK_SPACE:
                    case VK_RETURN:
                    case VK_BACK:
                    case VK_TAB:
                    case VK_ADD:
                    case VK_SUBTRACT:
                    case VK_MULTIPLY:
                    case VK_DIVIDE:
                    case VK_DECIMAL:
                    case VK_NUMPAD0:
                    case VK_NUMPAD1:
                    case VK_NUMPAD2:
                    case VK_NUMPAD3:
                    case VK_NUMPAD4:
                    case VK_NUMPAD5:
                    case VK_NUMPAD6:
                    case VK_NUMPAD7:
                    case VK_NUMPAD8:
                    case VK_NUMPAD9:
                    case VK_OEM_1:
                    case VK_OEM_2:
                    case VK_OEM_3:
                    case VK_OEM_4:
                    case VK_OEM_5:
                    case VK_OEM_6:
                    case VK_OEM_7:
                    case VK_OEM_8:
                    case VK_OEM_102:
                    case VK_OEM_PLUS:
                    case VK_OEM_COMMA:
                    case VK_OEM_MINUS:
                    case VK_OEM_PERIOD:
                        break;

                    default:
                        if ( (wParam >= '0' && wParam <= '9') ||
                                (wParam >= 'A' && wParam <= 'Z') )
                        {
                            // We'll get WM_CHAR for those later too.
                            break;
                        }

                        // But for the rest we won't get WM_CHAR later so we do
                        // need to generate the event right now.
                        wxKeyEvent event(wxEVT_CHAR);
                        InitAnyKeyEvent(event, wParam, lParam);

                        // Set the "extended" bit in lParam because we want to
                        // generate CHAR events with WXK_HOME and not
                        // WXK_NUMPAD_HOME even if the "Home" key on numpad was
                        // pressed.
                        event.m_keyCode = wxMSWKeyboard::VKToWX
                                          (
                                            wParam,
                                            lParam | (KF_EXTENDED << 16)
                                          );

                        // Don't produce events without any valid character
                        // code (even if this shouldn't normally happen...).
                        if ( event.m_keyCode != WXK_NONE )
                            processed = HandleWindowEvent(event);
                }
            }
            break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
            processed = HandleKeyUp((WORD) wParam, lParam);
            break;

        case WM_SYSCHAR:
        case WM_CHAR: // Always an ASCII character
            if ( m_lastKeydownProcessed )
            {
                // The key was handled in the EVT_KEY_DOWN and handling
                // a key in an EVT_KEY_DOWN handler is meant, by
                // design, to prevent EVT_CHARs from happening
                m_lastKeydownProcessed = false;
                processed = true;
            }
            else
            {
                processed = HandleChar((WORD)wParam, lParam);
            }
            break;

        case WM_IME_STARTCOMPOSITION:
            // IME popup needs Escape as it should undo the changes in its
            // entry window instead of e.g. closing the dialog for which the
            // IME is used (and losing all the changes in the IME window).
            gs_modalEntryWindowCount++;
            break;

        case WM_IME_ENDCOMPOSITION:
            gs_modalEntryWindowCount--;
            break;

#if wxUSE_HOTKEY
        case WM_HOTKEY:
            processed = HandleHotKey(wParam, lParam);
            break;
#endif // wxUSE_HOTKEY

        case WM_CUT:
        case WM_COPY:
        case WM_PASTE:
            processed = HandleClipboardEvent(message);
            break;

        case WM_HSCROLL:
        case WM_VSCROLL:
            {
                WXWORD code, pos;
                WXHWND hwnd;
                UnpackScroll(wParam, lParam, &code, &pos, &hwnd);

                processed = MSWOnScroll(message == WM_HSCROLL ? wxHORIZONTAL
                                                              : wxVERTICAL,
                                        code, pos, hwnd);
            }
            break;

#ifdef WM_GESTURE
        case WM_GESTURE:
        {
            if ( !GestureFuncs::IsOk() )
                break;

            HGESTUREINFO hGestureInfo = reinterpret_cast<HGESTUREINFO>(lParam);

            WinStruct<GESTUREINFO> gestureInfo;
            if ( !GestureFuncs::GetGestureInfo()(hGestureInfo, &gestureInfo) )
            {
                wxLogLastError("GetGestureInfo");
                break;
            }

            if ( gestureInfo.hwndTarget != GetHWND() )
            {
                wxLogDebug("This is Not the window targeted by this gesture!");
            }

            const wxPoint pt = ScreenToClient
                               (
                                    wxPoint(gestureInfo.ptsLocation.x,
                                            gestureInfo.ptsLocation.y)
                               );

            // dwID field is used to determine the type of gesture
            switch ( gestureInfo.dwID )
            {
                case GID_PAN:
                    // Point contains the current position of the pan.
                    processed = HandlePanGesture(pt, gestureInfo.dwFlags);
                    break;

                case GID_ZOOM:
                    // Point is the mid-point of 2 fingers and ullArgument
                    // contains the distance between the fingers in its lower
                    // half
                    processed = HandleZoomGesture
                                (
                                    pt,
                                    static_cast<DWORD>(gestureInfo.ullArguments),
                                    gestureInfo.dwFlags
                                );
                    break;

                case GID_ROTATE:
                    // Point is the center point of rotation and ullArguments
                    // contains the angle of rotation
                    processed = HandleRotateGesture
                                (
                                    pt,
                                    static_cast<DWORD>(gestureInfo.ullArguments),
                                    gestureInfo.dwFlags
                                );
                    break;

                case GID_TWOFINGERTAP:
                    processed = HandleTwoFingerTap(pt, gestureInfo.dwFlags);
                    break;

                case GID_PRESSANDTAP:
                    processed = HandlePressAndTap(pt, gestureInfo.dwFlags);
                    break;
            }

            if ( processed )
            {
                // If processed, we must call this to avoid memory leaks
                if ( !GestureFuncs::CloseGestureInfoHandle()(hGestureInfo) )
                {
                    wxLogLastError("CloseGestureInfoHandle");
                }
            }
        }
        break;
#endif // WM_GESTURE

        // CTLCOLOR messages are sent by children to query the parent for their
        // colors
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            {
                WXHDC hdc;
                WXHWND hwnd;
                UnpackCtlColor(wParam, lParam, &hdc, &hwnd);

                processed = HandleCtlColor(&rc.hBrush, (WXHDC)hdc, (WXHWND)hwnd);
            }
            break;

        case WM_SYSCOLORCHANGE:
            // the return value for this message is ignored
            processed = HandleSysColorChange();
            break;

        case WM_DISPLAYCHANGE:
            processed = HandleDisplayChange();
            break;

        case WM_PALETTECHANGED:
            processed = HandlePaletteChanged((WXHWND)wParam);
            break;

        case WM_CAPTURECHANGED:
            processed = HandleCaptureChanged((WXHWND)lParam);
            break;

        case WM_SETTINGCHANGE:
            processed = HandleSettingChange(wParam, lParam);
            break;

        case WM_QUERYNEWPALETTE:
            processed = HandleQueryNewPalette();
            break;

        case WM_ERASEBKGND:
            {
#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK
                // check if an override was configured for this window
                EraseBgHooks::const_iterator it = gs_eraseBgHooks.find(this);
                if ( it != gs_eraseBgHooks.end() )
                    processed = it->second->MSWEraseBgHook((WXHDC)wParam);
                else
#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK
                    processed = HandleEraseBkgnd((WXHDC)wParam);
            }

            if ( processed )
            {
                // we processed the message, i.e. erased the background
                rc.result = TRUE;
            }
            break;

        case WM_DROPFILES:
            processed = HandleDropFiles(wParam);
            break;

        case WM_INITDIALOG:
            processed = HandleInitDialog((WXHWND)wParam);

            if ( processed )
            {
                // we never set focus from here
                rc.result = FALSE;
            }
            break;

        case WM_QUERYENDSESSION:
            processed = HandleQueryEndSession(lParam, &rc.allow);
            break;

        case WM_ENDSESSION:
            processed = HandleEndSession(wParam != 0, lParam);
            break;

        case WM_GETMINMAXINFO:
            processed = HandleGetMinMaxInfo((MINMAXINFO*)lParam);
            break;

        case WM_SETCURSOR:
            processed = HandleSetCursor((WXHWND)wParam,
                                        LOWORD(lParam),     // hit test
                                        HIWORD(lParam));    // mouse msg

            if ( processed )
            {
                // returning TRUE stops the DefWindowProc() from further
                // processing this message - exactly what we need because we've
                // just set the cursor.
                rc.result = TRUE;
            }
            break;

#if wxUSE_ACCESSIBILITY
        case WM_GETOBJECT:
            {
                //WPARAM dwFlags = (WPARAM) (DWORD) wParam;
                LPARAM dwObjId = (LPARAM) (DWORD) lParam;

                if (dwObjId == (LPARAM)OBJID_CLIENT && GetOrCreateAccessible())
                {
                    processed = true;
                    rc.result = LresultFromObject(IID_IAccessible, wParam, (IUnknown*) GetAccessible()->GetIAccessible());
                }
                break;
            }
#endif

        case WM_HELP:
            {
                // by default, WM_HELP is propagated by DefWindowProc() upwards
                // to the window parent but as we do it ourselves already
                // (wxHelpEvent is derived from wxCommandEvent), we don't want
                // to get the other events if we process this message at all
                processed = true;

                // WM_HELP doesn't use lParam under CE
                HELPINFO* info = (HELPINFO*) lParam;
                if ( info->iContextType == HELPINFO_WINDOW )
                {
                    wxHelpEvent helpEvent
                                (
                                    wxEVT_HELP,
                                    GetId(),
                                    wxPoint(info->MousePos.x, info->MousePos.y)
                                );

                    helpEvent.SetEventObject(this);
                    HandleWindowEvent(helpEvent);
                }
                else if ( info->iContextType == HELPINFO_MENUITEM )
                {
                    wxHelpEvent helpEvent(wxEVT_HELP, info->iCtrlId);
                    helpEvent.SetEventObject(this);
                    HandleWindowEvent(helpEvent);

                }
                else // unknown help event?
                {
                    processed = false;
                }
            }
            break;

        case WM_CONTEXTMENU:
            {
                // As with WM_HELP above, we need to avoid duplicate events due
                // to wxContextMenuEvent being a (propagatable) wxCommandEvent
                // at wx level but WM_CONTEXTMENU also being propagated upwards
                // by DefWindowProc(). Unlike WM_HELP, we still need to pass
                // this one to DefWindowProc() as it sometimes does useful
                // things with it, e.g. displays the default context menu in
                // EDIT controls. So we do let the default processing to take
                // place but set this flag before calling into DefWindowProc()
                // and don't do anything if we're called from inside it.
                static bool s_propagatedByDefWndProc = false;
                if ( s_propagatedByDefWndProc )
                {
                    // We could also return false from here, it shouldn't
                    // matter, the important thing is to not send any events.
                    // But returning true prevents the message from bubbling up
                    // even further upwards and so seems to be better.
                    processed = true;
                    break;
                }

                // we don't convert from screen to client coordinates as
                // the event may be handled by a parent window
                wxPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

                wxContextMenuEvent evtCtx(wxEVT_CONTEXT_MENU, GetId(), pt);

                // we could have got an event from our child, reflect it back
                // to it if this is the case
                wxWindowMSW *win = NULL;
                WXHWND hWnd = (WXHWND)wParam;
                if ( hWnd != m_hWnd )
                {
                    win = FindItemByHWND(hWnd);
                }

                if ( !win )
                    win = this;

                evtCtx.SetEventObject(win);
                processed = win->HandleWindowEvent(evtCtx);

                if ( !processed )
                {
                    // Temporarily set the flag before calling out.
                    s_propagatedByDefWndProc = true;
                    wxON_BLOCK_EXIT_SET(s_propagatedByDefWndProc, false);

                    // Now do whatever the default handling does, which could
                    // be nothing at all -- but we can't know this, so we still
                    // need to call it.
                    win->MSWDefWindowProc(message, wParam, lParam);

                    // And finally pretend that we processed the message in any
                    // case because otherwise DefWindowProc() that we're called
                    // from would pass the message to our parent resulting in
                    // duplicate events. As it is, we ensure that only one
                    // wxWindow ever gets this message for any given click.
                    processed = true;
                }
            }
            break;

#if wxUSE_MENUS && !defined(__WXUNIVERSAL__)
        case WM_MENUCHAR:
            // we're only interested in our own menus, not MF_SYSMENU
            if ( HIWORD(wParam) == MF_POPUP )
            {
                // handle menu chars for ownerdrawn menu items
                int i = HandleMenuChar(toupper(LOWORD(wParam)), lParam);
                if ( i != wxNOT_FOUND )
                {
                    rc.result = MAKELRESULT(i, MNC_EXECUTE);
                    processed = true;
                }
            }
            break;

        case WM_INITMENUPOPUP:
            processed = HandleMenuPopup(wxEVT_MENU_OPEN, (WXHMENU)wParam);
            break;

        case WM_MENUSELECT:
            {
                WXWORD item, flags;
                WXHMENU hmenu;
                UnpackMenuSelect(wParam, lParam, &item, &flags, &hmenu);

                processed = HandleMenuSelect(item, flags, hmenu);
            }
            break;

        case WM_UNINITMENUPOPUP:
            processed = HandleMenuPopup(wxEVT_MENU_CLOSE, (WXHMENU)wParam);
            break;
#endif // wxUSE_MENUS && !defined(__WXUNIVERSAL__)

        case WM_POWERBROADCAST:
            {
                bool vetoed;
                processed = HandlePower(wParam, lParam, &vetoed);
                rc.result = processed && vetoed ? BROADCAST_QUERY_DENY : TRUE;
            }
            break;

        case WM_NCACTIVATE:
            // When we're losing activation to our own popup window, we want to
            // retain the "active" appearance of the title bar, as dropping
            // down a combobox popup shouldn't deactivate the window containing
            // the combobox, for example. Explicitly calling DefWindowProc() to
            // draw the window as active seems to be the only way of achieving
            // this (thanks to Barmak Shemirani for suggesting it at
            // https://stackoverflow.com/a/52808753/15275).
            if ( !wParam &&
                    wxCurrentPopupWindow &&
                        wxCurrentPopupWindow->MSWGetOwner() == this )
            {
                rc.result = MSWDefWindowProc(message, TRUE, lParam);
                processed = true;
            }
            break;

#if wxUSE_UXTHEME
        // If we want the default themed border then we need to draw it ourselves
        case WM_NCCALCSIZE:
            {
                const wxBorder border = TranslateBorder(GetBorder());
                if (wxUxThemeIsActive() && border == wxBORDER_THEME)
                {
                    // first ask the widget to calculate the border size
                    rc.result = MSWDefWindowProc(message, wParam, lParam);
                    processed = true;

                    // now alter the client size making room for drawing a
                    // themed border
                    RECT *rect;
                    NCCALCSIZE_PARAMS *csparam = NULL;
                    if ( wParam )
                    {
                        csparam = (NCCALCSIZE_PARAMS *)lParam;
                        rect = &csparam->rgrc[0];
                    }
                    else
                    {
                        rect = (RECT *)lParam;
                    }

                    wxUxThemeHandle hTheme((const wxWindow *)this, L"EDIT");
                    RECT rcClient = { 0, 0, 0, 0 };
                    wxClientDC dc((wxWindow *)this);
                    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();

                    if ( ::GetThemeBackgroundContentRect
                                (
                                 hTheme,
                                 GetHdcOf(*impl),
                                 EP_EDITTEXT,
                                 ETS_NORMAL,
                                 rect,
                                 &rcClient) == S_OK )
                    {
                        InflateRect(&rcClient, -1, -1);
                        if (wParam)
                            csparam->rgrc[0] = rcClient;
                        else
                            *((RECT*)lParam) = rcClient;

                        // WVR_REDRAW triggers a bug whereby child windows are moved up and left,
                        // so don't use.
                        // rc.result = WVR_REDRAW;
                    }
                }
            }
            break;

        case WM_NCPAINT:
            {
                const wxBorder border = TranslateBorder(GetBorder());
                if (wxUxThemeIsActive() && border == wxBORDER_THEME)
                {
                    // first ask the widget to paint its non-client area, such as scrollbars, etc.
                    rc.result = MSWDefWindowProc(message, wParam, lParam);
                    processed = true;

                    wxUxThemeHandle hTheme((const wxWindow *)this, L"EDIT");
                    wxWindowDC dc((wxWindow *)this);
                    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();

                    // Clip the DC so that you only draw on the non-client area
                    RECT rcBorder;
                    wxCopyRectToRECT(GetSize(), rcBorder);

                    RECT rcClient;
                    ::GetThemeBackgroundContentRect(
                        hTheme, GetHdcOf(*impl), EP_EDITTEXT, ETS_NORMAL, &rcBorder, &rcClient);
                    InflateRect(&rcClient, -1, -1);

                    ::ExcludeClipRect(GetHdcOf(*impl), rcClient.left, rcClient.top,
                                      rcClient.right, rcClient.bottom);

                    // Make sure the background is in a proper state
                    if (::IsThemeBackgroundPartiallyTransparent(hTheme, EP_EDITTEXT, ETS_NORMAL))
                    {
                        ::DrawThemeParentBackground(GetHwnd(), GetHdcOf(*impl), &rcBorder);
                    }

                    // Draw the border
                    int nState;
                    if ( !IsEnabled() )
                        nState = ETS_DISABLED;
                    // should we check this?
                    //else if ( ::GetWindowLong(GetHwnd(), GWL_STYLE) & ES_READONLY)
                    //    nState = ETS_READONLY;
                    else
                        nState = ETS_NORMAL;
                    ::DrawThemeBackground(hTheme, GetHdcOf(*impl), EP_EDITTEXT, nState, &rcBorder, NULL);
                }
            }
            break;

#endif // wxUSE_UXTHEME

        default:
            // try a custom message handler
            const MSWMessageHandlers::const_iterator
                i = gs_messageHandlers.find(message);
            if ( i != gs_messageHandlers.end() )
            {
                processed = (*i->second)(this, message, wParam, lParam);
            }
    }

    if ( !processed )
        return false;

    *result = rc.result;

    return true;
}

WXLRESULT wxWindowMSW::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    WXLRESULT result;
    if ( !MSWHandleMessage(&result, message, wParam, lParam) )
    {
#if wxDEBUG_LEVEL >= 2
        wxLogTrace("winmsg", wxT("Forwarding %s to DefWindowProc."),
                   wxGetMessageName(message));
#endif // wxDEBUG_LEVEL >= 2
        result = MSWDefWindowProc(message, wParam, lParam);
    }

    return result;
}

// ----------------------------------------------------------------------------
// wxWindow <-> HWND map
// ----------------------------------------------------------------------------

wxWindow *wxFindWinFromHandle(HWND hwnd)
{
    WindowHandles::const_iterator i = gs_windowHandles.find(hwnd);
    return i == gs_windowHandles.end() ? NULL : i->second;
}

void wxAssociateWinWithHandle(HWND hwnd, wxWindowMSW *win)
{
    // adding NULL hwnd is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( hwnd != (HWND)NULL,
                 wxT("attempt to add a NULL hwnd to window list ignored") );

#if wxDEBUG_LEVEL
    WindowHandles::const_iterator i = gs_windowHandles.find(hwnd);
    if ( i != gs_windowHandles.end() )
    {
        if ( i->second != win )
        {
            wxFAIL_MSG(
                wxString::Format(
                    wxT("HWND %p already associated with another window (%s)"),
                    hwnd, win->GetClassInfo()->GetClassName()
                )
            );
        }
        //else: this actually happens currently because we associate the window
        //      with its HWND during creation (if we create it) and also when
        //      SubclassWin() is called later, this is ok
    }
#endif // wxDEBUG_LEVEL

    gs_windowHandles[hwnd] = (wxWindow *)win;
}

void wxRemoveHandleAssociation(wxWindowMSW *win)
{
    gs_windowHandles.erase(GetHwndOf(win));
}

// ----------------------------------------------------------------------------
// various MSW speciic class dependent functions
// ----------------------------------------------------------------------------

// Default destroyer - override if you destroy it in some other way
// (e.g. with MDI child windows)
void wxWindowMSW::MSWDestroyWindow()
{
}

void wxWindowMSW::MSWGetCreateWindowCoords(const wxPoint& pos,
                                           const wxSize& size,
                                           int& x, int& y,
                                           int& w, int& h) const
{
    // CW_USEDEFAULT can't be used for child windows so just position them at
    // the origin by default
    x = pos.x == wxDefaultCoord ? 0 : pos.x;
    y = pos.y == wxDefaultCoord ? 0 : pos.y;

    AdjustForParentClientOrigin(x, y);

    // We don't have any clearly good choice for the size by default neither
    // but we must use something non-zero.
    w = WidthDefault(size.x);
    h = HeightDefault(size.y);

    /*
      NB: there used to be some code here which set the initial size of the
          window to the client size of the parent if no explicit size was
          specified. This was wrong because wxWidgets programs often assume
          that they get a WM_SIZE (EVT_SIZE) upon creation, however this broke
          it. To see why, you should understand that Windows sends WM_SIZE from
          inside ::CreateWindow() anyhow. However, ::CreateWindow() is called
          from some base class ctor and so this WM_SIZE is not processed in the
          real class' OnSize() (because it's not fully constructed yet and the
          event goes to some base class OnSize() instead). So the WM_SIZE we
          rely on is the one sent when the parent frame resizes its children
          but here is the problem: if the child already has just the right
          size, nothing will happen as both wxWidgets and Windows check for
          this and ignore any attempts to change the window size to the size it
          already has - so no WM_SIZE would be sent.
     */
}

WXHWND wxWindowMSW::MSWGetParent() const
{
    return m_parent ? m_parent->GetHWND() : WXHWND(NULL);
}

bool wxWindowMSW::MSWCreate(const wxChar *wclass,
                            const wxChar *title,
                            const wxPoint& pos,
                            const wxSize& size,
                            WXDWORD style,
                            WXDWORD extendedStyle)
{
    // check a common bug in the user code: if the window is created with a
    // non-default ctor and Create() is called too, we'd create 2 HWND for a
    // single wxWindow object and this results in all sorts of trouble,
    // especially for wxTLWs
    wxCHECK_MSG( !m_hWnd, true, "window can't be recreated" );

    // this can happen if this function is called using the return value of
    // wxApp::GetRegisteredClassName() which failed
    wxCHECK_MSG( wclass, false, "failed to register window class?" );


    // choose the position/size for the new window
    int x, y, w, h;
    (void)MSWGetCreateWindowCoords(pos, size, x, y, w, h);

    // controlId is menu handle for the top level windows, so set it to 0
    // unless we're creating a child window
    int controlId = style & WS_CHILD ? GetId() : 0;

    // do create the window
    wxWindowCreationHook hook(this);

    m_hWnd = MSWCreateWindowAtAnyPosition
             (
              extendedStyle,
              wclass,
              title ? title : m_windowName.t_str(),
              style,
              x, y, w, h,
              MSWGetParent(),
              controlId
             );

    if ( !m_hWnd )
    {
        return false;
    }

    SubclassWin(m_hWnd);

    return true;
}

WXHWND wxWindowMSW::MSWCreateWindowAtAnyPosition(WXDWORD exStyle, const wxChar* clName,
                                                 const wxChar* title, WXDWORD style,
                                                 int x, int y, int width, int height,
                                                 WXHWND parent, wxWindowID id)
{
    WXHWND hWnd = ::CreateWindowEx(exStyle, clName, title, style, x, y, width, height,
                                   parent, (HMENU)wxUIntToPtr(id), wxGetInstance(),
                                   NULL); // no extra data

    if ( !hWnd )
    {
        wxLogLastError(wxString::Format
        (
            wxT("CreateWindowEx(\"%s\", flags=%08lx, ex=%08lx)"),
            clName, style, exStyle
        ));
    }
    else if ( !IsTopLevel() && !MSWIsPositionDirectlySupported(x, y) )
    {
        // fix position if limited by Short range
        MSWMoveWindowToAnyPosition(hWnd, x, y, width, height, IsShown());
    }

    return hWnd;
}

// ===========================================================================
// MSW message handlers
// ===========================================================================

// ---------------------------------------------------------------------------
// WM_NOTIFY
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    HWND hWnd = hdr->hwndFrom;
    wxWindow *win = wxFindWinFromHandle(hWnd);

    // if the control is one of our windows, let it handle the message itself
    if ( win )
    {
        return win->MSWOnNotify(idCtrl, lParam, result);
    }

    // VZ: why did we do it? normally this is unnecessary and, besides, it
    //     breaks the message processing for the toolbars because the tooltip
    //     notifications were being forwarded to the toolbar child controls
    //     (if it had any) before being passed to the toolbar itself, so in my
    //     example the tooltip for the combobox was always shown instead of the
    //     correct button tooltips
#if 0
    // try all our children
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *child = node->GetData();
        if ( child->MSWOnNotify(idCtrl, lParam, result) )
        {
            return true;
        }

        node = node->GetNext();
    }
#endif // 0

    // by default, handle it ourselves
    return MSWOnNotify(idCtrl, lParam, result);
}

#if wxUSE_TOOLTIPS

bool wxWindowMSW::HandleTooltipNotify(WXUINT code,
                                      WXLPARAM lParam,
                                      const wxString& ttip)
{
    // I don't know why it happens, but the versions of comctl32.dll starting
    // from 4.70 sometimes send TTN_NEEDTEXTW even to ANSI programs (normally,
    // this message is supposed to be sent to Unicode programs only) -- hence
    // we need to handle it as well, otherwise no tooltips will be shown in
    // this case
    if ( !(code == (WXUINT) TTN_NEEDTEXTA || code == (WXUINT) TTN_NEEDTEXTW)
            || ttip.empty() )
    {
        // not a tooltip message or no tooltip to show anyhow
        return false;
    }

    LPTOOLTIPTEXT ttText = (LPTOOLTIPTEXT)lParam;

    // We don't want to use the szText buffer because it has a limit of 80
    // bytes and this is not enough, especially for Unicode build where it
    // limits the tooltip string length to only 40 characters
    //
    // The best would be, of course, to not impose any length limitations at
    // all but then the buffer would have to be dynamic and someone would have
    // to free it and we don't have the tooltip owner object here any more, so
    // for now use our own static buffer with a higher fixed max length.
    //
    // Note that using a static buffer should not be a problem as only a single
    // tooltip can be shown at the same time anyhow.
#if !wxUSE_UNICODE
    if ( code == (WXUINT) TTN_NEEDTEXTW )
    {
        // We need to convert tooltip from multi byte to Unicode on the fly.
        static wchar_t buf[513];

        // Truncate tooltip length if needed as otherwise we might not have
        // enough space for it in the buffer and MultiByteToWideChar() would
        // return an error
        size_t tipLength = wxMin(ttip.length(), WXSIZEOF(buf) - 1);

        // Convert to WideChar without adding the NULL character. The NULL
        // character is added afterwards (this is more efficient).
        int len = ::MultiByteToWideChar
                    (
                        CP_ACP,
                        0,                      // no flags
                        ttip.t_str(),
                        tipLength,
                        buf,
                        WXSIZEOF(buf) - 1
                    );

        if ( !len )
        {
            wxLogLastError(wxT("MultiByteToWideChar()"));
        }

        buf[len] = L'\0';
        ttText->lpszText = (LPSTR) buf;
    }
    else // TTN_NEEDTEXTA
#endif // !wxUSE_UNICODE
    {
        // we get here if we got TTN_NEEDTEXTA (only happens in ANSI build) or
        // if we got TTN_NEEDTEXTW in Unicode build: in this case we just have
        // to copy the string we have into the buffer
        static wxChar buf[513];
        wxStrlcpy(buf, ttip.c_str(), WXSIZEOF(buf));
        ttText->lpszText = buf;
    }

    return true;
}

#endif // wxUSE_TOOLTIPS

bool wxWindowMSW::MSWOnNotify(int WXUNUSED(idCtrl),
                              WXLPARAM lParam,
                              WXLPARAM* WXUNUSED(result))
{
#if wxUSE_TOOLTIPS
    if ( m_tooltip )
    {
        NMHDR* hdr = (NMHDR *)lParam;
        if ( HandleTooltipNotify(hdr->code, lParam, m_tooltip->GetTip()))
        {
            // processed
            return true;
        }
    }
#else
    wxUnusedVar(lParam);
#endif // wxUSE_TOOLTIPS

    return false;
}

// ---------------------------------------------------------------------------
// end session messages
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleQueryEndSession(long logOff, bool *mayEnd)
{
    wxCloseEvent event(wxEVT_QUERY_END_SESSION, wxID_ANY);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(true);
    event.SetLoggingOff(logOff == (long)ENDSESSION_LOGOFF);

    bool rc = wxTheApp->ProcessEvent(event);

    if ( rc )
    {
        // we may end only if the app didn't veto session closing (double
        // negation...)
        *mayEnd = !event.GetVeto();
    }

    return rc;
}

bool wxWindowMSW::HandleEndSession(bool endSession, long logOff)
{
    // do nothing if the session isn't ending
    if ( !endSession )
        return false;

    // only send once
    if ( (this != wxTheApp->GetTopWindow()) )
        return false;

    wxCloseEvent event(wxEVT_END_SESSION, wxID_ANY);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(false);
    event.SetLoggingOff((logOff & ENDSESSION_LOGOFF) != 0);

    return wxTheApp->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// window creation/destruction
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleCreate(WXLPCREATESTRUCT cs,
                               bool *mayCreate)
{
    if ( ((CREATESTRUCT *)cs)->dwExStyle & WS_EX_CONTROLPARENT )
        EnsureParentHasControlParentStyle(GetParent());

    *mayCreate = true;

    return true;
}

bool wxWindowMSW::HandleDestroy()
{
    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget != NULL )
    {
        m_dropTarget->Revoke(m_hWnd);

        wxDELETE(m_dropTarget);
    }
#endif // wxUSE_DRAG_AND_DROP

    // WM_DESTROY handled
    return true;
}

// ---------------------------------------------------------------------------
// activation/focus
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleActivate(int state,
                                 bool minimized,
                                 WXHWND WXUNUSED(activate))
{
    if ( minimized )
    {
        // Getting activation event when the window is minimized, as happens
        // e.g. when the window task bar icon is clicked, is unexpected and
        // managed to even break the logic in wx itself (see #17128), so just
        // don't do it as there doesn't seem to be any need to be notified
        // about the activation of the window icon in the task bar in practice.
        return false;
    }

    wxActivateEvent event(wxEVT_ACTIVATE,
                          (state == WA_ACTIVE) || (state == WA_CLICKACTIVE),
                          m_windowId,
                          state == WA_CLICKACTIVE
                            ? wxActivateEvent::Reason_Mouse
                            : wxActivateEvent::Reason_Unknown);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleSetFocus(WXHWND hwnd)
{
    // Strangly enough, some controls get set focus events when they are being
    // deleted, even if they already had focus before.
    if ( m_isBeingDeleted )
    {
        return false;
    }

    if ( ContainsHWND(hwnd) )
    {
        // If another subwindow of this window already had focus before, this
        // window should already have focus at wx level, no need for another
        // event.
        return false;
    }

    // notify the parent keeping track of focus for the kbd navigation
    // purposes that we got it
    wxChildFocusEvent eventFocus((wxWindow *)this);
    (void)HandleWindowEvent(eventFocus);

#if wxUSE_CARET
    // Deal with caret
    if ( m_caret )
    {
        m_caret->OnSetFocus();
    }
#endif // wxUSE_CARET

    wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
    event.SetEventObject(this);

    // wxFindWinFromHandle() may return NULL, it is ok
    event.SetWindow(wxFindWinFromHandle(hwnd));

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleKillFocus(WXHWND hwnd)
{
    // Don't send the event when in the process of being deleted.  This can
    // only cause problems if the event handler tries to access the object.
    if ( m_isBeingDeleted )
    {
        return false;
    }

    if ( ContainsHWND(hwnd) )
    {
        // If the focus switches to another HWND which is part of the same
        // wxWindow, we must not generate a wxEVT_KILL_FOCUS.
        return false;
    }

#if wxUSE_CARET
    // Deal with caret
    if ( m_caret )
    {
        m_caret->OnKillFocus();
    }
#endif // wxUSE_CARET

    wxFocusEvent event(wxEVT_KILL_FOCUS, m_windowId);
    event.SetEventObject(this);

    // wxFindWinFromHandle() may return NULL, it is ok
    event.SetWindow(wxFindWinFromHandle(hwnd));

    return HandleWindowEvent(event);
}

// ---------------------------------------------------------------------------
// labels
// ---------------------------------------------------------------------------

void wxWindowMSW::SetLabel( const wxString& label)
{
    SetWindowText(GetHwnd(), label.c_str());
}

wxString wxWindowMSW::GetLabel() const
{
    return wxGetWindowText(GetHWND());
}

// ---------------------------------------------------------------------------
// miscellaneous
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleShow(bool show, int WXUNUSED(status))
{
    wxShowEvent event(GetId(), show);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleInitDialog(WXHWND WXUNUSED(hWndFocus))
{
    wxInitDialogEvent event(GetId());
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleDropFiles(WXWPARAM wParam)
{
    HDROP hFilesInfo = (HDROP) wParam;

    // Get the total number of files dropped
    UINT gwFilesDropped = ::DragQueryFile
                            (
                                (HDROP)hFilesInfo,
                                (UINT)-1,
                                (LPTSTR)0,
                                (UINT)0
                            );

    wxString *files = new wxString[gwFilesDropped];
    for ( UINT wIndex = 0; wIndex < gwFilesDropped; wIndex++ )
    {
        // first get the needed buffer length (+1 for terminating NUL)
        size_t len = ::DragQueryFile(hFilesInfo, wIndex, NULL, 0) + 1;

        // and now get the file name
        ::DragQueryFile(hFilesInfo, wIndex,
                        wxStringBuffer(files[wIndex], len), len);
    }

    wxDropFilesEvent event(wxEVT_DROP_FILES, gwFilesDropped, files);
    event.SetEventObject(this);

    POINT dropPoint;
    DragQueryPoint(hFilesInfo, (LPPOINT) &dropPoint);
    event.m_pos.x = dropPoint.x;
    event.m_pos.y = dropPoint.y;

    DragFinish(hFilesInfo);

    return HandleWindowEvent(event);
}


bool wxWindowMSW::HandleSetCursor(WXHWND WXUNUSED(hWnd),
                                  short nHitTest,
                                  int WXUNUSED(mouseMsg))
{
    // the logic is as follows:
    //  0. if we're busy, set the busy cursor (even for non client elements)
    //  1. don't set custom cursor for non client area of enabled windows
    //  2. ask user EVT_SET_CURSOR handler for the cursor
    //  3. if still no cursor but we're in a TLW, set the global cursor

    HCURSOR hcursor = 0;

    // Check for "business" is complicated by the fact that modal dialogs shown
    // while busy cursor is in effect shouldn't show it as they are active and
    // accept input from the user, unlike all the other windows.
    bool isBusy = false;
    if ( wxIsBusy() )
    {
        wxDialog* const
            dlg = wxDynamicCast(wxGetTopLevelParent((wxWindow *)this), wxDialog);
        if ( !dlg || !dlg->IsModal() )
            isBusy = true;
    }

    if ( isBusy )
    {
        hcursor = wxGetCurrentBusyCursor();
    }
    else // not busy
    {
        if ( nHitTest != HTCLIENT )
            return false;

        // first ask the user code - it may wish to set the cursor in some very
        // specific way (for example, depending on the current position)
        POINT pt;
        wxGetCursorPosMSW(&pt);

        int x = pt.x,
            y = pt.y;
        ScreenToClient(&x, &y);
        wxSetCursorEvent event(x, y);
        event.SetId(GetId());
        event.SetEventObject(this);

        bool processedEvtSetCursor = HandleWindowEvent(event);
        if ( processedEvtSetCursor && event.HasCursor() )
        {
            hcursor = GetHcursorOf(event.GetCursor());
        }

        if ( !hcursor )
        {
            // the test for processedEvtSetCursor is here to prevent using
            // m_cursor if the user code caught EVT_SET_CURSOR() and returned
            // nothing from it - this is a way to say that our cursor shouldn't
            // be used for this point
            if ( !processedEvtSetCursor && m_cursor.IsOk() )
            {
                hcursor = GetHcursorOf(m_cursor);
            }

            if ( !hcursor && !GetParent() )
            {
                const wxCursor *cursor = wxGetGlobalCursor();
                if ( cursor && cursor->IsOk() )
                {
                    hcursor = GetHcursorOf(*cursor);
                }
            }
        }
    }


    if ( hcursor )
    {
        ::SetCursor(hcursor);

        // cursor set, stop here
        return true;
    }

    // pass up the window chain
    return false;
}

bool wxWindowMSW::HandlePower(WXWPARAM wParam,
                              WXLPARAM WXUNUSED(lParam),
                              bool *vetoed)
{
    wxEventType evtType;
    switch ( wParam )
    {
        case PBT_APMQUERYSUSPEND:
            evtType = wxEVT_POWER_SUSPENDING;
            break;

        case PBT_APMQUERYSUSPENDFAILED:
            evtType = wxEVT_POWER_SUSPEND_CANCEL;
            break;

        case PBT_APMSUSPEND:
            evtType = wxEVT_POWER_SUSPENDED;
            break;

        case PBT_APMRESUMESUSPEND:
            evtType = wxEVT_POWER_RESUME;
            break;

        default:
            wxLogDebug(wxT("Unknown WM_POWERBROADCAST(%d) event"), wParam);
            wxFALLTHROUGH;

        // these messages are currently not mapped to wx events
        case PBT_APMQUERYSTANDBY:
        case PBT_APMQUERYSTANDBYFAILED:
        case PBT_APMSTANDBY:
        case PBT_APMRESUMESTANDBY:
        case PBT_APMBATTERYLOW:
        case PBT_APMPOWERSTATUSCHANGE:
        case PBT_APMOEMEVENT:
        case PBT_APMRESUMECRITICAL:
        case PBT_APMRESUMEAUTOMATIC:
            evtType = wxEVT_NULL;
            break;
    }

    // don't handle unknown messages
    if ( evtType == wxEVT_NULL )
        return false;

    // TODO: notify about PBTF_APMRESUMEFROMFAILURE in case of resume events?

    wxPowerEvent event(evtType);
    if ( !HandleWindowEvent(event) )
        return false;

    *vetoed = event.IsVetoed();

    return true;
}

bool wxWindowMSW::IsDoubleBuffered() const
{
    for ( const wxWindowMSW *win = this; win; win = win->GetParent() )
    {
        if ( wxHasWindowExStyle(win, WS_EX_COMPOSITED) )
            return true;

        if ( win->IsTopLevel() )
            break;
    }

    return false;
}

void wxWindowMSW::SetDoubleBuffered(bool on)
{
    wxMSWWinExStyleUpdater(GetHwnd()).TurnOnOrOff(on, WS_EX_COMPOSITED);
}

// ---------------------------------------------------------------------------
// owner drawn stuff
// ---------------------------------------------------------------------------

#if (wxUSE_OWNER_DRAWN && wxUSE_MENUS_NATIVE) || \
        (wxUSE_CONTROLS && !defined(__WXUNIVERSAL__))
    #define WXUNUSED_UNLESS_ODRAWN(param) param
#else
    #define WXUNUSED_UNLESS_ODRAWN(param)
#endif

bool
wxWindowMSW::MSWOnDrawItem(int WXUNUSED_UNLESS_ODRAWN(id),
                           WXDRAWITEMSTRUCT * WXUNUSED_UNLESS_ODRAWN(itemStruct))
{
#if wxUSE_OWNER_DRAWN

#if wxUSE_MENUS_NATIVE
    // is it a menu item?
    DRAWITEMSTRUCT *pDrawStruct = (DRAWITEMSTRUCT *)itemStruct;
    if ( id == 0 && pDrawStruct->CtlType == ODT_MENU )
    {
        wxMenuItem *pMenuItem = (wxMenuItem *)(pDrawStruct->itemData);

        // see comment before the same test in MSWOnMeasureItem() below
        if ( !pMenuItem )
            return false;

        wxCHECK_MSG( wxDynamicCast(pMenuItem, wxMenuItem),
                         false, wxT("MSWOnDrawItem: bad wxMenuItem pointer") );

        // prepare to call OnDrawItem(): notice using of wxDCTemp to prevent
        // the DC from being released
        wxDCTemp dc((WXHDC)pDrawStruct->hDC);
        wxRect rect(pDrawStruct->rcItem.left, pDrawStruct->rcItem.top,
                    pDrawStruct->rcItem.right - pDrawStruct->rcItem.left,
                    pDrawStruct->rcItem.bottom - pDrawStruct->rcItem.top);

        return pMenuItem->OnDrawItem
               (
                dc,
                rect,
                (wxOwnerDrawn::wxODAction)pDrawStruct->itemAction,
                (wxOwnerDrawn::wxODStatus)pDrawStruct->itemState
               );
    }
#endif // wxUSE_MENUS_NATIVE

#endif // USE_OWNER_DRAWN

#if wxUSE_CONTROLS && !defined(__WXUNIVERSAL__)

#if wxUSE_OWNER_DRAWN
    wxControl *item = wxDynamicCast(FindItem(id), wxControl);
#else // !wxUSE_OWNER_DRAWN
    // we may still have owner-drawn buttons internally because we have to make
    // them owner-drawn to support colour change
    wxControl *item =
#                     if wxUSE_BUTTON
                         wxDynamicCast(FindItem(id), wxButton)
#                     else
                         NULL
#                     endif
                    ;
#endif // USE_OWNER_DRAWN

    if ( item )
    {
        return item->MSWOnDraw(itemStruct);
    }

#endif // wxUSE_CONTROLS

    return false;
}

bool
wxWindowMSW::MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *itemStruct)
{
#if wxUSE_OWNER_DRAWN && wxUSE_MENUS_NATIVE
    // is it a menu item?
    MEASUREITEMSTRUCT *pMeasureStruct = (MEASUREITEMSTRUCT *)itemStruct;
    if ( id == 0 && pMeasureStruct->CtlType == ODT_MENU )
    {
        wxMenuItem *pMenuItem = (wxMenuItem *)(pMeasureStruct->itemData);

        // according to Carsten Fuchs the pointer may be NULL under XP if an
        // MDI child frame is initially maximized, see this for more info:
        // http://article.gmane.org/gmane.comp.lib.wxwidgets.general/27745
        //
        // so silently ignore it instead of asserting
        if ( !pMenuItem )
            return false;

        wxCHECK_MSG( wxDynamicCast(pMenuItem, wxMenuItem),
                        false, wxT("MSWOnMeasureItem: bad wxMenuItem pointer") );

        size_t w, h;
        bool rc = pMenuItem->OnMeasureItem(&w, &h);

        pMeasureStruct->itemWidth = w;
        pMeasureStruct->itemHeight = h;

        return rc;
    }

    wxControl *item = wxDynamicCast(FindItem(id), wxControl);
    if ( item )
    {
        return item->MSWOnMeasure(itemStruct);
    }
#else
    wxUnusedVar(id);
    wxUnusedVar(itemStruct);
#endif // wxUSE_OWNER_DRAWN && wxUSE_MENUS_NATIVE

    return false;
}

// ---------------------------------------------------------------------------
// colours and palettes
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleSysColorChange()
{
    wxSysColourChangedEvent event;
    event.SetEventObject(this);

    (void)HandleWindowEvent(event);

    // always let the system carry on the default processing to allow the
    // native controls to react to the colours update
    return false;
}

bool wxWindowMSW::HandleDisplayChange()
{
    wxDisplayChangedEvent event;
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleCtlColor(WXHBRUSH *brush, WXHDC hDC, WXHWND hWnd)
{
#if !wxUSE_CONTROLS || defined(__WXUNIVERSAL__)
    wxUnusedVar(hDC);
    wxUnusedVar(hWnd);
#else
    wxControl *item = wxDynamicCast(FindItemByHWND(hWnd, true), wxControl);

    if ( item )
        *brush = item->MSWControlColor(hDC, hWnd);
    else
#endif // wxUSE_CONTROLS
        *brush = NULL;

    return *brush != NULL;
}

bool wxWindowMSW::HandlePaletteChanged(WXHWND hWndPalChange)
{
#if wxUSE_PALETTE
    // same as below except we don't respond to our own messages
    if ( hWndPalChange != GetHWND() )
    {
        // check to see if we our our parents have a custom palette
        wxWindowMSW *win = this;
        while ( win && !win->HasCustomPalette() )
        {
            win = win->GetParent();
        }

        if ( win && win->HasCustomPalette() )
        {
            // realize the palette to see whether redrawing is needed
            HDC hdc = ::GetDC((HWND) hWndPalChange);
            win->m_palette.SetHPALETTE((WXHPALETTE)
                    ::SelectPalette(hdc, GetHpaletteOf(win->m_palette), FALSE));

            int result = ::RealizePalette(hdc);

            // restore the palette (before releasing the DC)
            win->m_palette.SetHPALETTE((WXHPALETTE)
                    ::SelectPalette(hdc, GetHpaletteOf(win->m_palette), FALSE));
            ::RealizePalette(hdc);
            ::ReleaseDC((HWND) hWndPalChange, hdc);

            // now check for the need to redraw
            if (result > 0)
                ::InvalidateRect((HWND) hWndPalChange, NULL, TRUE);
        }

    }
#endif // wxUSE_PALETTE

    wxPaletteChangedEvent event(GetId());
    event.SetEventObject(this);
    event.SetChangedWindow(wxFindWinFromHandle(hWndPalChange));

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleCaptureChanged(WXHWND hWndGainedCapture)
{
    // Ensure that wxWindow::GetCapture() returns NULL if called from the event
    // handlers invoked below. This is necessary to avoid wrongly calling
    // ReleaseMouse() when we're already losing the mouse capture anyhow.
    gs_insideCaptureChanged = true;
    wxON_BLOCK_EXIT_SET(gs_insideCaptureChanged, false);

    // notify windows on the capture stack about lost capture
    // (see http://sourceforge.net/tracker/index.php?func=detail&aid=1153662&group_id=9863&atid=109863):
    wxWindowBase::NotifyCaptureLost();

    wxWindow *win = wxFindWinFromHandle(hWndGainedCapture);
    wxMouseCaptureChangedEvent event(GetId(), win);
    event.SetEventObject(this);
    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleSettingChange(WXWPARAM wParam, WXLPARAM lParam)
{
    // despite MSDN saying "(This message cannot be sent directly to a window.)"
    // we need to send this to child windows (it is only sent to top-level
    // windows) so {list,tree}ctrls can adjust their font size if necessary
    // this is exactly how explorer does it to enable the font size changes

    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        // top-level windows already get this message from the system
        wxWindow *win = node->GetData();
        if ( !win->IsTopLevel() )
        {
            ::SendMessage(GetHwndOf(win), WM_SETTINGCHANGE, wParam, lParam);
        }

        node = node->GetNext();
    }

    // let the system handle it
    return false;
}

bool wxWindowMSW::HandleQueryNewPalette()
{

#if wxUSE_PALETTE
    // check to see if we our our parents have a custom palette
    wxWindowMSW *win = this;
    while (!win->HasCustomPalette() && win->GetParent()) win = win->GetParent();
    if (win->HasCustomPalette()) {
        /* realize the palette to see whether redrawing is needed */
        HDC hdc = ::GetDC((HWND) GetHWND());
        win->m_palette.SetHPALETTE( (WXHPALETTE)
             ::SelectPalette(hdc, (HPALETTE) win->m_palette.GetHPALETTE(), FALSE) );

        int result = ::RealizePalette(hdc);
        /* restore the palette (before releasing the DC) */
        win->m_palette.SetHPALETTE( (WXHPALETTE)
             ::SelectPalette(hdc, (HPALETTE) win->m_palette.GetHPALETTE(), TRUE) );
        ::RealizePalette(hdc);
        ::ReleaseDC((HWND) GetHWND(), hdc);
        /* now check for the need to redraw */
        if (result > 0)
            ::InvalidateRect((HWND) GetHWND(), NULL, TRUE);
        }
#endif // wxUSE_PALETTE

    wxQueryNewPaletteEvent event(GetId());
    event.SetEventObject(this);

    return HandleWindowEvent(event) && event.GetPaletteRealized();
}

// Responds to colour changes: passes event on to children.
void wxWindowMSW::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
    // the top level window also reset the standard colour map as it might have
    // changed (there is no need to do it for the non top level windows as we
    // only have to do it once)
    if ( IsTopLevel() )
    {
        // FIXME-MT
        gs_hasStdCmap = false;
    }
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while ( node )
    {
        // Only propagate to non-top-level windows because Windows already
        // sends this event to all top-level ones
        wxWindow *win = node->GetData();
        if ( !win->IsTopLevel() )
        {
            // we need to send the real WM_SYSCOLORCHANGE and not just trigger
            // EVT_SYS_COLOUR_CHANGED call because the latter wouldn't work for
            // the standard controls
            ::SendMessage(GetHwndOf(win), WM_SYSCOLORCHANGE, 0, 0);
        }

        node = node->GetNext();
    }
}

extern wxCOLORMAP *wxGetStdColourMap()
{
    static COLORREF s_stdColours[wxSTD_COL_MAX];
    static wxCOLORMAP s_cmap[wxSTD_COL_MAX];

    if ( !gs_hasStdCmap )
    {
        static bool s_coloursInit = false;

        if ( !s_coloursInit )
        {
            // When a bitmap is loaded, the RGB values can change (apparently
            // because Windows adjusts them to care for the old programs always
            // using 0xc0c0c0 while the transparent colour for the new Windows
            // versions is different). But we do this adjustment ourselves so
            // we want to avoid Windows' "help" and for this we need to have a
            // reference bitmap which can tell us what the RGB values change
            // to.
            wxLogNull logNo; // suppress error if we couldn't load the bitmap
            wxBitmap stdColourBitmap(wxT("wxBITMAP_STD_COLOURS"));
            if ( stdColourBitmap.IsOk() )
            {
                // the pixels in the bitmap must correspond to wxSTD_COL_XXX!
                wxASSERT_MSG( stdColourBitmap.GetWidth() == wxSTD_COL_MAX,
                              wxT("forgot to update wxBITMAP_STD_COLOURS!") );

                wxMemoryDC memDC;
                memDC.SelectObject(stdColourBitmap);

                wxColour colour;
                for ( size_t i = 0; i < WXSIZEOF(s_stdColours); i++ )
                {
                    memDC.GetPixel(i, 0, &colour);
                    s_stdColours[i] = wxColourToRGB(colour);
                }
            }
            else // wxBITMAP_STD_COLOURS couldn't be loaded
            {
                s_stdColours[0] = RGB(000,000,000);     // black
                s_stdColours[1] = RGB(128,128,128);     // dark grey
                s_stdColours[2] = RGB(192,192,192);     // light grey
                s_stdColours[3] = RGB(255,255,255);     // white
                //s_stdColours[4] = RGB(000,000,255);     // blue
                //s_stdColours[5] = RGB(255,000,255);     // magenta
            }

            s_coloursInit = true;
        }

        gs_hasStdCmap = true;

        // create the colour map
#define INIT_CMAP_ENTRY(col) \
            s_cmap[wxSTD_COL_##col].from = s_stdColours[wxSTD_COL_##col]; \
            s_cmap[wxSTD_COL_##col].to = ::GetSysColor(COLOR_##col)

        INIT_CMAP_ENTRY(BTNTEXT);
        INIT_CMAP_ENTRY(BTNSHADOW);
        INIT_CMAP_ENTRY(BTNFACE);
        INIT_CMAP_ENTRY(BTNHIGHLIGHT);

#undef INIT_CMAP_ENTRY
    }

    return s_cmap;
}

#if wxUSE_UXTHEME && !defined(TMT_FILLCOLOR)
    #define TMT_FILLCOLOR       3802
    #define TMT_TEXTCOLOR       3803
    #define TMT_BORDERCOLOR     3801
#endif

wxColour wxWindowMSW::MSWGetThemeColour(const wchar_t *themeName,
                                        int themePart,
                                        int themeState,
                                        MSWThemeColour themeColour,
                                        wxSystemColour fallback) const
{
#if wxUSE_UXTHEME
    if ( wxUxThemeIsActive() )
    {
        int themeProperty = 0;

        // TODO: Convert this into a table? Sure would be faster.
        switch ( themeColour )
        {
            case ThemeColourBackground:
                themeProperty = TMT_FILLCOLOR;
                break;
            case ThemeColourText:
                themeProperty = TMT_TEXTCOLOR;
                break;
            case ThemeColourBorder:
                themeProperty = TMT_BORDERCOLOR;
                break;
            default:
                wxFAIL_MSG(wxT("unsupported theme colour"));
        };

        wxUxThemeHandle hTheme((const wxWindow *)this, themeName);
        COLORREF col;
        HRESULT hr = ::GetThemeColor
                            (
                                hTheme,
                                themePart,
                                themeState,
                                themeProperty,
                                &col
                            );

        if ( SUCCEEDED(hr) )
            return wxRGBToColour(col);

        wxLogApiError(
            wxString::Format(
                "GetThemeColor(%s, %i, %i, %i)",
                themeName, themePart, themeState, themeProperty),
            hr);
    }
#else
    wxUnusedVar(themeName);
    wxUnusedVar(themePart);
    wxUnusedVar(themeState);
    wxUnusedVar(themeColour);
#endif
    return wxSystemSettings::GetColour(fallback);
}

// ---------------------------------------------------------------------------
// painting
// ---------------------------------------------------------------------------

// this variable is used to check that a paint event handler which processed
// the event did create a wxPaintDC inside its code and called BeginPaint() to
// validate the invalidated window area as otherwise we'd keep getting an
// endless stream of WM_PAINT messages for this window resulting in a lot of
// difficult to debug problems (e.g. impossibility to repaint other windows,
// lack of timer and idle events and so on)
extern bool wxDidCreatePaintDC;
bool wxDidCreatePaintDC = false;

bool wxWindowMSW::HandlePaint()
{
    HRGN hRegion = ::CreateRectRgn(0, 0, 0, 0); // Dummy call to get a handle
    if ( !hRegion )
    {
        wxLogLastError(wxT("CreateRectRgn"));
    }
    if ( ::GetUpdateRgn(GetHwnd(), hRegion, FALSE) == ERROR )
    {
        wxLogLastError(wxT("GetUpdateRgn"));
    }

    m_updateRegion = wxRegion((WXHRGN) hRegion);

    wxDidCreatePaintDC = false;

    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);

    bool processed = HandleWindowEvent(event);

    if ( wxDidCreatePaintDC && !processed )
    {
        // Event handler did paint something as wxPaintDC object was created
        // but then it must have skipped the event to indicate that default
        // handling should still take place, so call MSWDefWindowProc() right
        // now. It's important to do it before EndPaint() call below as that
        // would validate the window and MSWDefWindowProc(WM_PAINT) wouldn't do
        // anything if called after it.
        OnPaint(event);
    }

    // note that we must generate NC event after the normal one as otherwise
    // BeginPaint() will happily overwrite our decorations with the background
    // colour
    wxNcPaintEvent eventNc(m_windowId);
    eventNc.SetEventObject(this);
    HandleWindowEvent(eventNc);

    // don't keep an HRGN we don't need any longer (GetUpdateRegion() can only
    // be called from inside the event handlers called above)
    m_updateRegion.Clear();

    wxPaintDCImpl::EndPaint((wxWindow *)this);

    // It doesn't matter whether the event was actually processed or not here,
    // what matters is whether we already painted, and hence validated, the
    // window or not. If we did, either the event was processed or we called
    // OnPaint() above, so we should return true. If we did not, even the event
    // was processed, we must still call MSWDefWindowProc() to ensure that the
    // window is validated, i.e. to avoid the problem described in the comment
    // before wxDidCreatePaintDC definition above.
    return wxDidCreatePaintDC;
}

// Can be called from an application's OnPaint handler
void wxWindowMSW::OnPaint(wxPaintEvent& event)
{
#ifdef __WXUNIVERSAL__
    event.Skip();
#else
    HDC hDC = (HDC) wxPaintDCImpl::FindDCInCache((wxWindow*) event.GetEventObject());
    if (hDC != 0)
    {
        MSWDefWindowProc(WM_PAINT, (WPARAM) hDC, 0);
    }
#endif
}

bool wxWindowMSW::HandleEraseBkgnd(WXHDC hdc)
{
    if ( IsBeingDeleted() )
    {
        // We can get WM_ERASEBKGND after starting the destruction of our top
        // level parent. Handling it in this case is unnecessary and can be
        // actually harmful as e.g. wxStaticBox::GetClientSize() doesn't work
        // without a valid TLW parent (because it uses dialog units internally
        // which use the dialog font), so just don't do anything then.
        return false;
    }

    switch ( GetBackgroundStyle() )
    {
        case wxBG_STYLE_ERASE:
        case wxBG_STYLE_COLOUR:
            // we need to generate an erase background event
            {
                wxDCTemp dc(hdc, GetClientSize());
                wxDCTempImpl *impl = (wxDCTempImpl*) dc.GetImpl();

                impl->SetHDC(hdc);
                impl->SetWindow((wxWindow *)this);

                wxEraseEvent event(m_windowId, &dc);
                event.SetEventObject(this);
                bool rc = HandleWindowEvent(event);

                // must be called manually as ~wxDC doesn't do anything for
                // wxDCTemp
                impl->SelectOldObjects(hdc);

                if ( rc )
                {
                    // background erased by the user-defined handler
                    return true;
                }
            }
            wxFALLTHROUGH;

        case wxBG_STYLE_SYSTEM:
            if ( !DoEraseBackground(hdc) )
            {
                // let the default processing to take place if we didn't erase
                // the background ourselves
                return false;
            }
            break;

        case wxBG_STYLE_PAINT:
        case wxBG_STYLE_TRANSPARENT:
            // no need to do anything here at all, background will be entirely
            // redrawn in WM_PAINT handler
            break;

        default:
            wxFAIL_MSG( "unknown background style" );
    }

    return true;
}

#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK

bool wxWindowMSW::MSWHasEraseBgHook() const
{
    return gs_eraseBgHooks.find(const_cast<wxWindowMSW *>(this))
                != gs_eraseBgHooks.end();
}

void wxWindowMSW::MSWSetEraseBgHook(wxWindow *child)
{
    if ( child )
    {
        if ( !gs_eraseBgHooks.insert(
                EraseBgHooks::value_type(this, child)).second )
        {
            wxFAIL_MSG( wxT("Setting erase background hook twice?") );
        }
    }
    else // reset the hook
    {
        if ( gs_eraseBgHooks.erase(this) != 1 )
        {
            wxFAIL_MSG( wxT("Resetting erase background which was not set?") );
        }
    }
}

#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK

bool wxWindowMSW::DoEraseBackground(WXHDC hDC)
{
    HBRUSH hbr = (HBRUSH)MSWGetBgBrush(hDC);
    if ( !hbr )
        return false;

    // erase just the client area of the window, this is important for the
    // frames to avoid drawing over the toolbar part of the window (you might
    // think using WS_CLIPCHILDREN would prevent this from happening, but it
    // clearly doesn't)
    RECT rc;
    wxCopyRectToRECT(GetClientRect(), rc);
    ::FillRect((HDC)hDC, &rc, hbr);

    return true;
}

WXHBRUSH
wxWindowMSW::MSWGetBgBrushForChild(WXHDC hDC, wxWindowMSW *child)
{
    // Test for the custom background brush first.
    WXHBRUSH hbrush = MSWGetCustomBgBrush();
    if ( hbrush )
    {
        // We assume that this is either a stipple or hatched brush and not a
        // solid one as otherwise it would have been enough to set the
        // background colour and such brushes need to be positioned correctly
        // in order to align when different windows are painted, so do it here.
        RECT rc;
        ::GetWindowRect(GetHwndOf(child), &rc);

        // It is important to pass both points to MapWindowPoints() as in
        // addition to converting them to our coordinate system, this function
        // will also exchange the left and right coordinates if this window
        // uses RTL layout, which is exactly what we need here as the child
        // window origin is its _right_ top corner in this case and not the
        // left one.
        ::MapWindowPoints(NULL, GetHwnd(), (POINT *)&rc, 2);

        int x = rc.left,
            y = rc.top;
        MSWAdjustBrushOrg(&x, &y);

        if ( !::SetBrushOrgEx((HDC)hDC, -x, -y, NULL) )
        {
            wxLogLastError(wxT("SetBrushOrgEx(bg brush)"));
        }

        return hbrush;
    }

    // Otherwise see if we have a custom background colour.
    if ( m_hasBgCol )
    {
        wxBrush *
            brush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour());

        return (WXHBRUSH)GetHbrushOf(*brush);
    }

    return 0;
}

WXHBRUSH wxWindowMSW::MSWGetBgBrush(WXHDC hDC)
{
    // Use the special wxWindowBeingErased variable if it is set as the child
    // being erased.
    wxWindowMSW * const child =
#if wxUSE_UXTHEME
                                wxWindowBeingErased ? wxWindowBeingErased :
#endif
                                this;

    for ( wxWindowMSW *win = this; win; win = win->GetParent() )
    {
        WXHBRUSH hBrush = win->MSWGetBgBrushForChild(hDC, child);
        if ( hBrush )
            return hBrush;

        // don't use the parent background if we're not transparent
        if ( !win->HasTransparentBackground() )
            break;

        // background is not inherited beyond top level windows
        if ( win->IsTopLevel() )
            break;
    }

    return 0;
}

bool wxWindowMSW::HandlePrintClient(WXHDC hDC)
{
    // we receive this message when DrawThemeParentBackground() is
    // called from def window proc of several controls under XP and we
    // must draw properly themed background here
    //
    // note that naively I'd expect filling the client rect with the
    // brush returned by MSWGetBgBrush() work -- but for some reason it
    // doesn't and we have to call parents MSWPrintChild() which is
    // supposed to call DrawThemeBackground() with appropriate params
    //
    // also note that in this case lParam == PRF_CLIENT but we're
    // clearly expected to paint the background and nothing else!

    if ( IsTopLevel() || InheritsBackgroundColour() )
        return false;

    // sometimes we don't want the parent to handle it at all, instead
    // return whatever value this window wants
    if ( !MSWShouldPropagatePrintChild() )
        return MSWPrintChild(hDC, (wxWindow *)this);

    for ( wxWindow *win = GetParent(); win; win = win->GetParent() )
    {
        if ( win->MSWPrintChild(hDC, (wxWindow *)this) )
            return true;

        if ( win->IsTopLevel() || win->InheritsBackgroundColour() )
            break;
    }

    return false;
}

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleMinimize()
{
    wxIconizeEvent event(m_windowId);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleMaximize()
{
    wxMaximizeEvent event(m_windowId);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleMove(int x, int y)
{
    wxPoint point(x,y);
    wxMoveEvent event(point, m_windowId);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleMoving(wxRect& rect)
{
    wxMoveEvent event(rect, m_windowId);
    event.SetEventObject(this);

    bool rc = HandleWindowEvent(event);
    if (rc)
        rect = event.GetRect();
    return rc;
}

bool wxWindowMSW::HandleEnterSizeMove()
{
    wxMoveEvent event(wxPoint(0,0), m_windowId);
    event.SetEventType(wxEVT_MOVE_START);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleExitSizeMove()
{
    wxMoveEvent event(wxPoint(0,0), m_windowId);
    event.SetEventType(wxEVT_MOVE_END);
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

#if wxUSE_DEFERRED_SIZING

bool wxWindowMSW::BeginRepositioningChildren()
{
    int numChildren = 0;
    for ( HWND child = ::GetWindow(GetHwndOf(this), GW_CHILD);
          child;
          child = ::GetWindow(child, GW_HWNDNEXT) )
    {
        numChildren ++;
    }

    // Nothing is gained by deferring the repositioning of a single child.
    if ( numChildren < 2 )
        return false;

    // Protect against valid m_hDWP being overwritten
    if ( m_hDWP )
        return false;

    m_hDWP = (WXHANDLE)::BeginDeferWindowPos(numChildren);
    if ( !m_hDWP )
    {
        wxLogLastError(wxT("BeginDeferWindowPos"));
        return false;
    }

    // Return true to indicate that EndDeferWindowPos() should be called.
    return true;
}

void wxWindowMSW::EndRepositioningChildren()
{
    wxASSERT_MSG( m_hDWP, wxS("Shouldn't be called") );

    // reset m_hDWP to NULL so that child windows don't try to use our
    // m_hDWP after we call EndDeferWindowPos() on it (this shouldn't
    // happen anyhow normally but who knows what weird flow of control we
    // may have depending on what the users EVT_SIZE handler does...)
    HDWP hDWP = (HDWP)m_hDWP;
    m_hDWP = NULL;

    // do put all child controls in place at once
    if ( !::EndDeferWindowPos(hDWP) )
    {
        wxLogLastError(wxT("EndDeferWindowPos"));
    }

    // Reset our children's pending pos/size values.
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindowMSW * const child = node->GetData();
        child->MSWEndDeferWindowPos();
    }
}

#endif // wxUSE_DEFERRED_SIZING

bool wxWindowMSW::HandleSize(int WXUNUSED(w), int WXUNUSED(h), WXUINT wParam)
{
    // when we resize this window, its children are probably going to be
    // repositioned as well, prepare to use DeferWindowPos() for them
    ChildrenRepositioningGuard repositionGuard(this);

    // update this window size
    bool processed = false;
    switch ( wParam )
    {
        default:
            wxFAIL_MSG( wxT("unexpected WM_SIZE parameter") );
            // fall through nevertheless
            wxFALLTHROUGH;

        case SIZE_MAXHIDE:
            wxFALLTHROUGH;
        case SIZE_MAXSHOW:
            // we're not interested in these messages at all
            break;

        case SIZE_MINIMIZED:
            processed = HandleMinimize();
            break;

        case SIZE_MAXIMIZED:
            /* processed = */ HandleMaximize();
            // fall through to send a normal size event as well
            wxFALLTHROUGH;

        case SIZE_RESTORED:
            // don't use w and h parameters as they specify the client size
            // while according to the docs EVT_SIZE handler is supposed to
            // receive the total size
            wxSizeEvent event(GetSize(), m_windowId);
            event.SetEventObject(this);

            processed = HandleWindowEvent(event);
            break;
    }

    return processed;
}

bool wxWindowMSW::HandleSizing(wxRect& rect)
{
    wxSizeEvent event(rect, m_windowId);
    event.SetEventObject(this);

    bool rc = HandleWindowEvent(event);
    if (rc)
        rect = event.GetRect();
    return rc;
}

bool wxWindowMSW::HandleGetMinMaxInfo(void *mmInfo)
{
    MINMAXINFO *info = (MINMAXINFO *)mmInfo;

    bool rc = false;

    int minWidth = GetMinWidth(),
        minHeight = GetMinHeight(),
        maxWidth = GetMaxWidth(),
        maxHeight = GetMaxHeight();

    if ( minWidth != wxDefaultCoord )
    {
        info->ptMinTrackSize.x = minWidth;
        rc = true;
    }

    if ( minHeight != wxDefaultCoord )
    {
        info->ptMinTrackSize.y = minHeight;
        rc = true;
    }

    if ( maxWidth != wxDefaultCoord )
    {
        info->ptMaxTrackSize.x = maxWidth;
        rc = true;
    }

    if ( maxHeight != wxDefaultCoord )
    {
        info->ptMaxTrackSize.y = maxHeight;
        rc = true;
    }

    return rc;
}

// ---------------------------------------------------------------------------
// command messages
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleCommand(WXWORD id_, WXWORD cmd, WXHWND control)
{
    // sign extend to int from short before comparing with the other int ids
    int id = (signed short)id_;

#if wxUSE_MENUS_NATIVE
    if ( !cmd && wxCurrentPopupMenu )
    {
        wxMenu *popupMenu = wxCurrentPopupMenu;
        wxCurrentPopupMenu = NULL;

        return popupMenu->MSWCommand(cmd, id);
    }
#endif // wxUSE_MENUS_NATIVE

    wxWindow *win = NULL;

    // first try to find it from HWND - this works even with the broken
    // programs using the same ids for different controls
    if ( control )
    {
        win = wxFindWinFromHandle(control);
    }

    // try the id
    if ( !win )
    {
        win = FindItem(id, control);
    }

    if ( win )
    {
        return win->MSWCommand(cmd, id);
    }

    // the messages sent from the in-place edit control used by the treectrl
    // for label editing have id == 0, but they should _not_ be treated as menu
    // messages (they are EN_XXX ones, in fact) so don't translate anything
    // coming from a control to wxEVT_MENU
    if ( !control )
    {
        wxCommandEvent event(wxEVT_MENU, id);
        event.SetEventObject(this);
        event.SetInt(id);

        return HandleWindowEvent(event);
    }
    else
    {
#if wxUSE_SPINCTRL && !defined(__WXUNIVERSAL__)
        // the text ctrl which is logically part of wxSpinCtrl sends WM_COMMAND
        // notifications to its parent which we want to reflect back to
        // wxSpinCtrl
        wxSpinCtrl *spin = wxSpinCtrl::GetSpinForTextCtrl(control);
        if ( spin && spin->ProcessTextCommand(cmd, id) )
            return true;
#endif // wxUSE_SPINCTRL

    }

    return false;
}

// ---------------------------------------------------------------------------
// mouse events
// ---------------------------------------------------------------------------

void wxWindowMSW::InitMouseEvent(wxMouseEvent& event,
                                 int x, int y,
                                 WXUINT flags)
{
    // our client coords are not quite the same as Windows ones
    wxPoint pt = GetClientAreaOrigin();
    event.m_x = x - pt.x;
    event.m_y = y - pt.y;

    event.m_shiftDown = (flags & MK_SHIFT) != 0;
    event.m_controlDown = (flags & MK_CONTROL) != 0;
    event.m_leftDown = (flags & MK_LBUTTON) != 0;
    event.m_middleDown = (flags & MK_MBUTTON) != 0;
    event.m_rightDown = (flags & MK_RBUTTON) != 0;
    event.m_aux1Down = (flags & MK_XBUTTON1) != 0;
    event.m_aux2Down = (flags & MK_XBUTTON2) != 0;
    event.m_altDown = ::wxIsAltDown();

    event.SetTimestamp(::GetMessageTime());

    event.SetEventObject(this);
    event.SetId(GetId());

    gs_lastMouseEvent.pos = ClientToScreen(wxPoint(x, y));
    gs_lastMouseEvent.type = event.GetEventType();
}

bool wxWindowMSW::HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags)
{
    // the mouse events take consecutive IDs from WM_MOUSEFIRST to
    // WM_MOUSELAST, so it's enough to subtract WM_MOUSEMOVE == WM_MOUSEFIRST
    // from the message id and take the value in the table to get wxWin event
    // id
    static const wxEventType eventsMouse[] =
    {
        wxEVT_MOTION,
        wxEVT_LEFT_DOWN,
        wxEVT_LEFT_UP,
        wxEVT_LEFT_DCLICK,
        wxEVT_RIGHT_DOWN,
        wxEVT_RIGHT_UP,
        wxEVT_RIGHT_DCLICK,
        wxEVT_MIDDLE_DOWN,
        wxEVT_MIDDLE_UP,
        wxEVT_MIDDLE_DCLICK,
        0, // this one is for wxEVT_MOTION which is not used here
        wxEVT_AUX1_DOWN,
        wxEVT_AUX1_UP,
        wxEVT_AUX1_DCLICK,
        wxEVT_AUX2_DOWN,
        wxEVT_AUX2_UP,
        wxEVT_AUX2_DCLICK
    };

    // the same messages are used for both auxiliary mouse buttons so we need
    // to adjust the index manually
    switch ( msg )
    {
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
            if (HIWORD(flags) == XBUTTON2)
                msg += wxEVT_AUX2_DOWN - wxEVT_AUX1_DOWN;
    }

    wxMouseEvent event(eventsMouse[msg - WM_MOUSEMOVE]);
    InitMouseEvent(event, x, y, flags);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleMouseMove(int x, int y, WXUINT flags)
{
    if ( !m_mouseInWindow )
    {
        // it would be wrong to assume that just because we get a mouse move
        // event that the mouse is inside the window: although this is usually
        // true, it is not if we had captured the mouse, so we need to check
        // the mouse coordinates here
        if ( !HasCapture() || IsMouseInWindow() )
        {
            // Generate an ENTER event
            m_mouseInWindow = true;

#ifdef HAVE_TRACKMOUSEEVENT
            typedef BOOL (WINAPI *_TrackMouseEvent_t)(LPTRACKMOUSEEVENT);
            static _TrackMouseEvent_t s_pfn_TrackMouseEvent;
            static bool s_initDone = false;
            if ( !s_initDone )
            {
                // see comment in wxApp::GetComCtl32Version() explaining the
                // use of wxLoadedDLL
                wxLoadedDLL dllComCtl32(wxT("comctl32.dll"));
                if ( dllComCtl32.IsLoaded() )
                {
                    s_pfn_TrackMouseEvent = (_TrackMouseEvent_t)
                        dllComCtl32.RawGetSymbol(wxT("_TrackMouseEvent"));
                }

                s_initDone = true;
            }

            if ( s_pfn_TrackMouseEvent )
            {
                WinStruct<TRACKMOUSEEVENT> trackinfo;

                trackinfo.dwFlags = TME_LEAVE;
                trackinfo.hwndTrack = GetHwnd();

                (*s_pfn_TrackMouseEvent)(&trackinfo);
            }
#endif // HAVE_TRACKMOUSEEVENT

            wxMouseEvent event(wxEVT_ENTER_WINDOW);
            InitMouseEvent(event, x, y, flags);

            (void)HandleWindowEvent(event);
        }
    }
#ifdef HAVE_TRACKMOUSEEVENT
    else // mouse not in window
    {
        // Check if we need to send a LEAVE event
        // Windows doesn't send WM_MOUSELEAVE if the mouse has been captured so
        // send it here if we are using native mouse leave tracking
        if ( HasCapture() && !IsMouseInWindow() )
        {
            GenerateMouseLeave();
        }
    }
#endif // HAVE_TRACKMOUSEEVENT

    // Windows often generates mouse events even if mouse position hasn't
    // changed (http://article.gmane.org/gmane.comp.lib.wxwidgets.devel/66576)
    //
    // Filter this out as it can result in unexpected behaviour compared to
    // other platforms
    if ( gs_lastMouseEvent.type == wxEVT_RIGHT_DOWN ||
         gs_lastMouseEvent.type == wxEVT_LEFT_DOWN ||
         gs_lastMouseEvent.type == wxEVT_MIDDLE_DOWN ||
         gs_lastMouseEvent.type == wxEVT_MOTION )
    {
        if ( ClientToScreen(wxPoint(x, y)) == gs_lastMouseEvent.pos )
        {
            gs_lastMouseEvent.type = wxEVT_MOTION;

            return false;
        }
    }

    return HandleMouseEvent(WM_MOUSEMOVE, x, y, flags);
}


bool
wxWindowMSW::HandleMouseWheel(wxMouseWheelAxis axis,
                              WXWPARAM wParam, WXLPARAM lParam)
{
#if wxUSE_MOUSEWHEEL
    // notice that WM_MOUSEWHEEL position is in screen coords (as it's
    // forwarded up to the parent by DefWindowProc()) and not in the client
    // ones as all the other messages, translate them to the client coords for
    // consistency -- but do it using Windows function and not our own one
    // because InitMouseEvent() expects coordinates in Windows client
    // coordinates and not wx ones (the difference being the height of the
    // toolbar, if any).
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(GetHwnd(), &pt);

    wxMouseEvent event(wxEVT_MOUSEWHEEL);
    InitMouseEvent(event, pt.x, pt.y, LOWORD(wParam));
    event.m_wheelRotation = (short)HIWORD(wParam);
    event.m_wheelDelta = WHEEL_DELTA;
    event.m_wheelAxis = axis;

    static int s_linesPerRotation = -1;
    if ( s_linesPerRotation == -1 )
    {
        if ( !::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0,
                                     &s_linesPerRotation, 0))
        {
            // this is not supposed to happen
            wxLogLastError(wxT("SystemParametersInfo(GETWHEELSCROLLLINES)"));

            // the default is 3, so use it if SystemParametersInfo() failed
            s_linesPerRotation = 3;
        }
    }

    static int s_columnsPerRotation = -1;
    if ( s_columnsPerRotation == -1 )
    {
        if ( !::SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0,
                                     &s_columnsPerRotation, 0))
        {
            // this setting is not supported on Windows 2000/XP, so use the value of 1
            // http://msdn.microsoft.com/en-us/library/ms997498.aspx
            s_columnsPerRotation = 1;
        }
    }

    event.m_linesPerAction = s_linesPerRotation;
    event.m_columnsPerAction = s_columnsPerRotation;
    return HandleWindowEvent(event);

#else // !wxUSE_MOUSEWHEEL
    wxUnusedVar(wParam);
    wxUnusedVar(lParam);

    return false;
#endif // wxUSE_MOUSEWHEEL/!wxUSE_MOUSEWHEEL
}

void wxWindowMSW::GenerateMouseLeave()
{
    m_mouseInWindow = false;

    int state = 0;
    if ( wxIsShiftDown() )
        state |= MK_SHIFT;
    if ( wxIsCtrlDown() )
        state |= MK_CONTROL;

    // Only the high-order bit should be tested
    if ( GetKeyState( VK_LBUTTON ) & (1<<15) )
        state |= MK_LBUTTON;
    if ( GetKeyState( VK_MBUTTON ) & (1<<15) )
        state |= MK_MBUTTON;
    if ( GetKeyState( VK_RBUTTON ) & (1<<15) )
        state |= MK_RBUTTON;

    POINT pt;
    wxGetCursorPosMSW(&pt);

    // we need to have client coordinates here for symmetry with
    // wxEVT_ENTER_WINDOW
    RECT rect = wxGetWindowRect(GetHwnd());
    pt.x -= rect.left;
    pt.y -= rect.top;

    wxMouseEvent event(wxEVT_LEAVE_WINDOW);
    InitMouseEvent(event, pt.x, pt.y, state);

    (void)HandleWindowEvent(event);
}

#ifdef WM_GESTURE
// ---------------------------------------------------------------------------
// Gesture events
// ---------------------------------------------------------------------------

bool wxWindowMSW::InitGestureEvent(wxGestureEvent& event,
                                   const wxPoint& pt,
                                   WXDWORD flags)
{
    event.SetEventObject(this);
    event.SetTimestamp(::GetMessageTime());
    event.SetPosition(pt);

    if ( flags & GF_BEGIN )
        event.SetGestureStart();

    if ( flags & GF_END )
        event.SetGestureEnd();

    return (flags & GF_BEGIN) != 0;
}

bool wxWindowMSW::HandlePanGesture(const wxPoint& pt, WXDWORD flags)
{
    // wxEVT_GESTURE_PAN
    wxPanGestureEvent event(GetId());

    // This is used to calculate the pan delta.
    static wxPoint s_previousLocation;

    // If the gesture has just started, store the current point to determine
    // the pan delta later on.
    if ( InitGestureEvent(event, pt, flags) )
    {
        s_previousLocation = pt;
    }

    // Determine the horizontal and vertical changes
    event.SetDelta(pt - s_previousLocation);

    // Update the last gesture event point
    s_previousLocation = pt;

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleZoomGesture(const wxPoint& pt,
                                    WXDWORD fingerDistance,
                                    WXDWORD flags)
{
    // wxEVT_GESTURE_ZOOM
    wxZoomGestureEvent event(GetId());

    // These are used to calculate the center of the zoom and zoom factor
    static wxPoint s_previousLocation;
    static int s_intialFingerDistance;

    // This flag indicates that the gesture has just started, store the current
    // point and distance between the fingers for future calculations.
    if ( InitGestureEvent(event, pt, flags) )
    {
        s_previousLocation = pt;
        s_intialFingerDistance = fingerDistance;
    }

    // Calculate center point of the zoom. Human beings are not very good at
    // moving two fingers at exactly the same rate outwards/inwards and there
    // is usually some error, which can cause the center to shift slightly. So,
    // it is recommended to take the average of center of fingers in the
    // current and last positions.
    const wxPoint ptCenter = (s_previousLocation + pt)/2;

    const double zoomFactor = (double) fingerDistance / s_intialFingerDistance;

    event.SetZoomFactor(zoomFactor);

    event.SetPosition(ptCenter);

    // Update gesture event point
    s_previousLocation = pt;

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleRotateGesture(const wxPoint& pt,
                                      WXDWORD angleArgument,
                                      WXDWORD flags)
{
    // wxEVT_GESTURE_ROTATE
    wxRotateGestureEvent event(GetId());

    if ( InitGestureEvent(event, pt, flags) )
    {
        event.SetRotationAngle(angleArgument);
    }
    else // Not the first event.
    {
        // Use angleArgument to obtain the cumulative angle since the gesture
        // was first started. This angle is in radians and MSW returns negative
        // angle for clockwise rotation and positive otherwise, so, multiply
        // angle by -1 for positive angle for clockwise and negative in case of
        // counterclockwise.
        double angle = -GID_ROTATE_ANGLE_FROM_ARGUMENT(angleArgument);

        // If the rotation is anti-clockwise convert the angle to its
        // corresponding positive value in a clockwise sense.
        if ( angle < 0 )
        {
            angle += 2 * M_PI;
        }

        // Set the angle
        event.SetRotationAngle(angle);
    }

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleTwoFingerTap(const wxPoint& pt, WXDWORD flags)
{
    // wxEVT_TWO_FINGER_TAP
    wxTwoFingerTapEvent event(GetId());

    InitGestureEvent(event, pt, flags);

    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandlePressAndTap(const wxPoint& pt, WXDWORD flags)
{
    wxPressAndTapEvent event(GetId());

    InitGestureEvent(event, pt, flags);

    return HandleWindowEvent(event);
}
#endif // WM_GESTURE

// ---------------------------------------------------------------------------
// keyboard handling
// ---------------------------------------------------------------------------

namespace
{

// Implementation of InitAnyKeyEvent() which can also be used when there is no
// associated window: this can happen for the wxEVT_CHAR_HOOK events created by
// the global keyboard hook (e.g. the event might have happened in a non-wx
// window).
void
MSWInitAnyKeyEvent(wxKeyEvent& event,
                   WXWPARAM wParam,
                   WXLPARAM lParam,
                   const wxWindowBase *win /* may be NULL */)
{
    if ( win )
    {
        event.SetId(win->GetId());
        event.SetEventObject(const_cast<wxWindowBase *>(win));
    }
    else // No associated window.
    {
        // Use wxID_ANY for compatibility with the old code even if wxID_NONE
        // would arguably make more sense.
        event.SetId(wxID_ANY);
    }

    event.m_shiftDown = wxIsShiftDown();
    event.m_controlDown = wxIsCtrlDown();
    event.m_altDown = (HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN;

    event.m_rawCode = (wxUint32) wParam;
    event.m_rawFlags = (wxUint32) lParam;
    event.SetTimestamp(::GetMessageTime());
}

} // anonymous namespace

void
wxWindowMSW::InitAnyKeyEvent(wxKeyEvent& event,
                             WXWPARAM wParam,
                             WXLPARAM lParam) const
{
    MSWInitAnyKeyEvent(event, wParam, lParam, this);
}

wxKeyEvent
wxWindowMSW::CreateKeyEvent(wxEventType evType,
                            WXWPARAM wParam,
                            WXLPARAM lParam) const
{
    // Catch any attempts to use this with WM_CHAR, it wouldn't work because
    // wParam is supposed to be a virtual key and not a character here.
    wxASSERT_MSG( evType != wxEVT_CHAR && evType != wxEVT_CHAR_HOOK,
                    "CreateKeyEvent() can't be used for char events" );

    wxKeyEvent event(evType);
    InitAnyKeyEvent(event, wParam, lParam);

    event.m_keyCode = wxMSWKeyboard::VKToWX
                                     (
                                        wParam,
                                        lParam
#if wxUSE_UNICODE
                                        , &event.m_uniChar
#endif // wxUSE_UNICODE
                                     );

    return event;
}

wxKeyEvent
wxWindowMSW::CreateCharEvent(wxEventType evType,
                             WXWPARAM wParam,
                             WXLPARAM lParam) const
{
    wxKeyEvent event(evType);
    InitAnyKeyEvent(event, wParam, lParam);

#if wxUSE_UNICODE
    // TODO: wParam uses UTF-16 so this is incorrect for characters outside of
    //       the BMP, we should use WM_UNICHAR to handle them.
    event.m_uniChar = wParam;
#endif // wxUSE_UNICODE

    // Set non-Unicode key code too for compatibility if possible.
    if ( wParam < 0x80 )
    {
        // It's an ASCII character, no need to translate it.
        event.m_keyCode = wParam;
    }
    else
    {
        // Check if this key can be represented (as a single character) in the
        // current locale.
        const wchar_t wc = wParam;
        char ch;
        if ( wxConvLibc.FromWChar(&ch, 1, &wc, 1) != wxCONV_FAILED )
        {
            // For compatibility continue to provide the key code in this field
            // even though using GetUnicodeKey() is recommended now.
            event.m_keyCode = static_cast<unsigned char>(ch);
        }
        //else: Key can't be represented in the current locale, leave m_keyCode
        //      as WXK_NONE and use GetUnicodeKey() to access the character.
    }

    // the alphanumeric keys produced by pressing AltGr+something on European
    // keyboards have both Ctrl and Alt modifiers which may confuse the user
    // code as, normally, keys with Ctrl and/or Alt don't result in anything
    // alphanumeric, so pretend that there are no modifiers at all (the
    // KEY_DOWN event would still have the correct modifiers if they're really
    // needed)
    if ( event.m_controlDown && event.m_altDown &&
            (event.m_keyCode >= 32 && event.m_keyCode < 256) )
    {
        event.m_controlDown =
        event.m_altDown = false;
    }

    return event;
}

// isASCII is true only when we're called from WM_CHAR handler and not from
// WM_KEYDOWN one
bool wxWindowMSW::HandleChar(WXWPARAM wParam, WXLPARAM lParam)
{
    wxKeyEvent event(CreateCharEvent(wxEVT_CHAR, wParam, lParam));
    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleKeyDown(WXWPARAM wParam, WXLPARAM lParam)
{
    wxKeyEvent event(CreateKeyEvent(wxEVT_KEY_DOWN, wParam, lParam));
    return HandleWindowEvent(event);
}

bool wxWindowMSW::HandleKeyUp(WXWPARAM wParam, WXLPARAM lParam)
{
    wxKeyEvent event(CreateKeyEvent(wxEVT_KEY_UP, wParam, lParam));
    return HandleWindowEvent(event);
}

#if wxUSE_MENUS
int wxWindowMSW::HandleMenuChar(int chAccel,
                                WXLPARAM lParam)
{
    const HMENU hmenu = (HMENU)lParam;

    WinStruct<MENUITEMINFO> mii;

    // use MIIM_FTYPE to know if the item is ownerdrawn or not
    mii.fMask = MIIM_FTYPE | MIIM_DATA;

    // find if we have this letter in any owner drawn item
    const int count = ::GetMenuItemCount(hmenu);
    for ( int i = 0; i < count; i++ )
    {
        if ( ::GetMenuItemInfo(hmenu, i, TRUE, &mii) )
        {
            if ( mii.fType == MFT_OWNERDRAW )
            {
                //  dwItemData member of the MENUITEMINFO is a
                //  pointer to the associated wxMenuItem -- see the
                //  menu creation code
                wxMenuItem *item = (wxMenuItem*)mii.dwItemData;

                const wxString label(item->GetItemLabel());
                const wxChar *p = wxStrchr(label.t_str(), wxT('&'));
                while ( p++ )
                {
                    if ( *p == wxT('&') )
                    {
                        // this is not the accel char, find the real one
                        p = wxStrchr(p + 1, wxT('&'));
                    }
                    else // got the accel char
                    {
                        // FIXME-UNICODE: this comparison doesn't risk to work
                        // for non ASCII accelerator characters I'm afraid, but
                        // what can we do?
                        if ( (wchar_t)wxToupper(*p) == (wchar_t)chAccel )
                        {
                            return i;
                        }
                        else
                        {
                            // this one doesn't match
                            break;
                        }
                    }
                }
            }
        }
        else // failed to get the menu text?
        {
            // it's not fatal, so don't show error, but still log it
            wxLogLastError(wxT("GetMenuItemInfo"));
        }
    }
    return wxNOT_FOUND;
}

#endif // wxUSE_MENUS

bool wxWindowMSW::HandleClipboardEvent(WXUINT nMsg)
{
    const wxEventType type = nMsg == WM_CUT       ? wxEVT_TEXT_CUT
                           : nMsg == WM_COPY      ? wxEVT_TEXT_COPY
                           : /* nMsg == WM_PASTE */ wxEVT_TEXT_PASTE;
    wxClipboardTextEvent evt(type, GetId());

    evt.SetEventObject(this);

    return HandleWindowEvent(evt);
}


// ---------------------------------------------------------------------------
// scrolling
// ---------------------------------------------------------------------------

bool wxWindowMSW::MSWOnScroll(int orientation, WXWORD wParam,
                              WXWORD pos, WXHWND control)
{
    if ( control && control != m_hWnd ) // Prevent infinite recursion
    {
        wxWindow *child = wxFindWinFromHandle(control);
        if ( child )
            return child->MSWOnScroll(orientation, wParam, pos, control);
    }

    wxScrollWinEvent event;
    event.SetPosition(pos);
    event.SetOrientation(orientation);
    event.SetEventObject(this);

    switch ( wParam )
    {
    case SB_TOP:
        event.SetEventType(wxEVT_SCROLLWIN_TOP);
        break;

    case SB_BOTTOM:
        event.SetEventType(wxEVT_SCROLLWIN_BOTTOM);
        break;

    case SB_LINEUP:
        event.SetEventType(wxEVT_SCROLLWIN_LINEUP);
        break;

    case SB_LINEDOWN:
        event.SetEventType(wxEVT_SCROLLWIN_LINEDOWN);
        break;

    case SB_PAGEUP:
        event.SetEventType(wxEVT_SCROLLWIN_PAGEUP);
        break;

    case SB_PAGEDOWN:
        event.SetEventType(wxEVT_SCROLLWIN_PAGEDOWN);
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        // under Win32, the scrollbar range and position are 32 bit integers,
        // but WM_[HV]SCROLL only carry the low 16 bits of them, so we must
        // explicitly query the scrollbar for the correct position (this must
        // be done only for these two SB_ events as they are the only one
        // carrying the scrollbar position)
        {
            WinStruct<SCROLLINFO> scrollInfo;
            scrollInfo.fMask = SIF_TRACKPOS;

            if ( !::GetScrollInfo(GetHwnd(),
                                  WXOrientToSB(orientation),
                                  &scrollInfo) )
            {
                // Not necessarily an error, if there are no scrollbars yet.
                // wxLogLastError(wxT("GetScrollInfo"));
            }

            event.SetPosition(scrollInfo.nTrackPos);
        }

        event.SetEventType( wParam == SB_THUMBPOSITION
                                ? wxEVT_SCROLLWIN_THUMBRELEASE
                                : wxEVT_SCROLLWIN_THUMBTRACK );
        break;

    default:
        return false;
    }

    return HandleWindowEvent(event);
}

// ----------------------------------------------------------------------------
// custom message handlers
// ----------------------------------------------------------------------------

/* static */ bool
wxWindowMSW::MSWRegisterMessageHandler(int msg, MSWMessageHandler handler)
{
    wxCHECK_MSG( gs_messageHandlers.find(msg) == gs_messageHandlers.end(),
                 false, wxT("registering handler for the same message twice") );

    gs_messageHandlers[msg] = handler;
    return true;
}

/* static */ void
wxWindowMSW::MSWUnregisterMessageHandler(int msg, MSWMessageHandler handler)
{
    const MSWMessageHandlers::iterator i = gs_messageHandlers.find(msg);
    wxCHECK_RET( i != gs_messageHandlers.end() && i->second == handler,
                 wxT("unregistering non-registered handler?") );

    gs_messageHandlers.erase(i);
}

// ===========================================================================
// global functions
// ===========================================================================

void wxGetCharSize(WXHWND wnd, int *x, int *y, const wxFont& the_font)
{
    TEXTMETRIC tm;
    HDC dc = ::GetDC((HWND) wnd);
    HFONT was = 0;

    //    the_font.UseResource();
    //    the_font.RealizeResource();
    HFONT fnt = (HFONT)the_font.GetResourceHandle(); // const_cast
    if ( fnt )
        was = (HFONT) SelectObject(dc,fnt);

    GetTextMetrics(dc, &tm);
    if ( fnt && was )
    {
        SelectObject(dc,was);
    }
    ReleaseDC((HWND)wnd, dc);

    if ( x )
        *x = tm.tmAveCharWidth;
    if ( y )
        *y = tm.tmHeight + tm.tmExternalLeading;

    //   the_font.ReleaseResource();
}

// ----------------------------------------------------------------------------
// keyboard codes
// ----------------------------------------------------------------------------

namespace wxMSWKeyboard
{

namespace
{

// use the "extended" bit of lParam to distinguish extended keys from normal
// keys as the same virtual key code is sent for both by Windows
inline
int ChooseNormalOrExtended(int lParam, int keyNormal, int keyExtended)
{
    // except that if lParam is 0, it means we don't have real lParam from
    // WM_KEYDOWN but are just translating just a VK constant (e.g. done from
    // msw/treectrl.cpp when processing TVN_KEYDOWN) -- then assume this is a
    // non-numpad (hence extended) key as this is a more common case
    return !lParam || (HIWORD(lParam) & KF_EXTENDED) ? keyExtended : keyNormal;
}

// this array contains the Windows virtual key codes which map one to one to
// WXK_xxx constants and is used in wxMSWKeyboard::VKToWX/WXToVK() below
//
// note that keys having a normal and numpad version (e.g. WXK_HOME and
// WXK_NUMPAD_HOME) are not included in this table as the mapping is not 1-to-1
const struct wxKeyMapping
{
    int vk;
    wxKeyCode wxk;
} gs_specialKeys[] =
{
    { VK_CANCEL,        WXK_CANCEL },
    { VK_BACK,          WXK_BACK },
    { VK_TAB,           WXK_TAB },
    { VK_CLEAR,         WXK_CLEAR },
    { VK_SHIFT,         WXK_SHIFT },
    { VK_CONTROL,       WXK_CONTROL },
    { VK_MENU ,         WXK_ALT },
    { VK_PAUSE,         WXK_PAUSE },
    { VK_CAPITAL,       WXK_CAPITAL },
    { VK_SPACE,         WXK_SPACE },
    { VK_ESCAPE,        WXK_ESCAPE },
    { VK_SELECT,        WXK_SELECT },
    { VK_PRINT,         WXK_PRINT },
    { VK_EXECUTE,       WXK_EXECUTE },
    { VK_SNAPSHOT,      WXK_SNAPSHOT },
    { VK_HELP,          WXK_HELP },

    { VK_NUMPAD0,       WXK_NUMPAD0 },
    { VK_NUMPAD1,       WXK_NUMPAD1 },
    { VK_NUMPAD2,       WXK_NUMPAD2 },
    { VK_NUMPAD3,       WXK_NUMPAD3 },
    { VK_NUMPAD4,       WXK_NUMPAD4 },
    { VK_NUMPAD5,       WXK_NUMPAD5 },
    { VK_NUMPAD6,       WXK_NUMPAD6 },
    { VK_NUMPAD7,       WXK_NUMPAD7 },
    { VK_NUMPAD8,       WXK_NUMPAD8 },
    { VK_NUMPAD9,       WXK_NUMPAD9 },
    { VK_MULTIPLY,      WXK_NUMPAD_MULTIPLY },
    { VK_ADD,           WXK_NUMPAD_ADD },
    { VK_SUBTRACT,      WXK_NUMPAD_SUBTRACT },
    { VK_DECIMAL,       WXK_NUMPAD_DECIMAL },
    { VK_DIVIDE,        WXK_NUMPAD_DIVIDE },

    { VK_F1,            WXK_F1 },
    { VK_F2,            WXK_F2 },
    { VK_F3,            WXK_F3 },
    { VK_F4,            WXK_F4 },
    { VK_F5,            WXK_F5 },
    { VK_F6,            WXK_F6 },
    { VK_F7,            WXK_F7 },
    { VK_F8,            WXK_F8 },
    { VK_F9,            WXK_F9 },
    { VK_F10,           WXK_F10 },
    { VK_F11,           WXK_F11 },
    { VK_F12,           WXK_F12 },
    { VK_F13,           WXK_F13 },
    { VK_F14,           WXK_F14 },
    { VK_F15,           WXK_F15 },
    { VK_F16,           WXK_F16 },
    { VK_F17,           WXK_F17 },
    { VK_F18,           WXK_F18 },
    { VK_F19,           WXK_F19 },
    { VK_F20,           WXK_F20 },
    { VK_F21,           WXK_F21 },
    { VK_F22,           WXK_F22 },
    { VK_F23,           WXK_F23 },
    { VK_F24,           WXK_F24 },

    { VK_NUMLOCK,       WXK_NUMLOCK },
    { VK_SCROLL,        WXK_SCROLL },

    { VK_LWIN,          WXK_WINDOWS_LEFT },
    { VK_RWIN,          WXK_WINDOWS_RIGHT },
    { VK_APPS,          WXK_WINDOWS_MENU },

    { VK_BROWSER_BACK,        WXK_BROWSER_BACK },
    { VK_BROWSER_FORWARD,     WXK_BROWSER_FORWARD },
    { VK_BROWSER_REFRESH,     WXK_BROWSER_REFRESH },
    { VK_BROWSER_STOP,        WXK_BROWSER_STOP },
    { VK_BROWSER_SEARCH,      WXK_BROWSER_SEARCH },
    { VK_BROWSER_FAVORITES,   WXK_BROWSER_FAVORITES },
    { VK_BROWSER_HOME,        WXK_BROWSER_HOME },
    { VK_VOLUME_MUTE,         WXK_VOLUME_MUTE },
    { VK_VOLUME_DOWN,         WXK_VOLUME_DOWN },
    { VK_VOLUME_UP,           WXK_VOLUME_UP },
    { VK_MEDIA_NEXT_TRACK,    WXK_MEDIA_NEXT_TRACK },
    { VK_MEDIA_PREV_TRACK,    WXK_MEDIA_PREV_TRACK },
    { VK_MEDIA_STOP,          WXK_MEDIA_STOP },
    { VK_MEDIA_PLAY_PAUSE,    WXK_MEDIA_PLAY_PAUSE },
    { VK_LAUNCH_MAIL,         WXK_LAUNCH_MAIL },
    { VK_LAUNCH_APP1,         WXK_LAUNCH_APP1 },
    { VK_LAUNCH_APP2,         WXK_LAUNCH_APP2 },
};

} // anonymous namespace

int VKToWX(WXWORD vk, WXLPARAM lParam, wchar_t *uc)
{
    int wxk;

    // check the table first
    for ( size_t n = 0; n < WXSIZEOF(gs_specialKeys); n++ )
    {
        if ( gs_specialKeys[n].vk == vk )
        {
            wxk = gs_specialKeys[n].wxk;
            if ( wxk < WXK_START )
            {
                // Unicode code for this key is the same as its ASCII code.
                if ( uc )
                    *uc = wxk;
            }

            return wxk;
        }
    }

    // keys requiring special handling
    switch ( vk )
    {
        case VK_OEM_1:
        case VK_OEM_PLUS:
        case VK_OEM_COMMA:
        case VK_OEM_MINUS:
        case VK_OEM_PERIOD:
        case VK_OEM_2:
        case VK_OEM_3:
        case VK_OEM_4:
        case VK_OEM_5:
        case VK_OEM_6:
        case VK_OEM_7:
        case VK_OEM_8:
        case VK_OEM_102:
            // MapVirtualKey() returns 0 if it fails to convert the virtual
            // key which nicely corresponds to our WXK_NONE.
            wxk = ::MapVirtualKey(vk, MAPVK_VK_TO_CHAR);

            if ( HIWORD(wxk) & 0x8000 )
            {
                // It's a dead key and we don't return anything at all for them
                // as we simply don't have any way to indicate the difference
                // between e.g. a normal "'" and "'" as a dead key -- and
                // generating the same events for them just doesn't seem like a
                // good idea.
                wxk = WXK_NONE;
            }

            // In any case return this as a Unicode character value.
            if ( uc )
                *uc = wxk;

            // For compatibility with the old non-Unicode code we continue
            // returning key codes for Latin-1 characters directly
            // (normally it would really only make sense to do it for the
            // ASCII characters, not Latin-1 ones).
            if ( wxk > 255 )
            {
                // But for anything beyond this we can only return the key
                // value as a real Unicode character, not a wxKeyCode
                // because this enum values clash with Unicode characters
                // (e.g. WXK_LBUTTON also happens to be U+012C a.k.a.
                // "LATIN CAPITAL LETTER I WITH BREVE").
                wxk = WXK_NONE;
            }
            break;

        // handle extended keys
        case VK_PRIOR:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_PAGEUP, WXK_PAGEUP);
            break;

        case VK_NEXT:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_PAGEDOWN, WXK_PAGEDOWN);
            break;

        case VK_END:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_END, WXK_END);
            break;

        case VK_HOME:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_HOME, WXK_HOME);
            break;

        case VK_LEFT:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_LEFT, WXK_LEFT);
            break;

        case VK_UP:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_UP, WXK_UP);
            break;

        case VK_RIGHT:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_RIGHT, WXK_RIGHT);
            break;

        case VK_DOWN:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_DOWN, WXK_DOWN);
            break;

        case VK_INSERT:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_INSERT, WXK_INSERT);
            break;

        case VK_DELETE:
            wxk = ChooseNormalOrExtended(lParam, WXK_NUMPAD_DELETE, WXK_DELETE);

            if ( uc )
                *uc = WXK_DELETE;
            break;

        case VK_RETURN:
            // don't use ChooseNormalOrExtended() here as the keys are reversed
            // here: numpad enter is the extended one
            wxk = HIWORD(lParam) & KF_EXTENDED ? WXK_NUMPAD_ENTER : WXK_RETURN;

            if ( uc )
                *uc = WXK_RETURN;
            break;

        default:
            if ( (vk >= '0' && vk <= '9') || (vk >= 'A' && vk <= 'Z') )
            {
                // A simple alphanumeric key and the values of them coincide in
                // Windows and wx for both ASCII and Unicode codes.
                wxk = vk;
            }
            else // Something we simply don't know about at all.
            {
                wxk = WXK_NONE;
            }

            if ( uc )
                *uc = vk;
    }

    return wxk;
}

WXWORD WXToVK(int wxk, bool *isExtended)
{
    // check the table first
    for ( size_t n = 0; n < WXSIZEOF(gs_specialKeys); n++ )
    {
        if ( gs_specialKeys[n].wxk == wxk )
        {
            // All extended keys (i.e. non-numpad versions of the keys that
            // exist both in the numpad and outside of it) are dealt with
            // below.
            if ( isExtended )
                *isExtended = false;

            return gs_specialKeys[n].vk;
        }
    }

    // and then check for special keys not included in the table
    bool extended = false;
    WXWORD vk;
    switch ( wxk )
    {
        case WXK_PAGEUP:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_PAGEUP:
            vk = VK_PRIOR;
            break;

        case WXK_PAGEDOWN:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_PAGEDOWN:
            vk = VK_NEXT;
            break;

        case WXK_END:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_END:
            vk = VK_END;
            break;

        case WXK_HOME:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_HOME:
            vk = VK_HOME;
            break;

        case WXK_LEFT:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_LEFT:
            vk = VK_LEFT;
            break;

        case WXK_UP:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_UP:
            vk = VK_UP;
            break;

        case WXK_RIGHT:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_RIGHT:
            vk = VK_RIGHT;
            break;

        case WXK_DOWN:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_DOWN:
            vk = VK_DOWN;
            break;

        case WXK_INSERT:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_INSERT:
            vk = VK_INSERT;
            break;

        case WXK_DELETE:
            extended = true;
            wxFALLTHROUGH;
        case WXK_NUMPAD_DELETE:
            vk = VK_DELETE;
            break;

        default:
            // check to see if its one of the OEM key codes.
            BYTE vks = LOBYTE(VkKeyScan(wxk));
            if ( vks != 0xff )
            {
                vk = vks;
            }
            else
            {
                vk = (WXWORD)wxk;
            }
            break;
    }

    if ( isExtended )
        *isExtended = extended;

    return vk;
}

} // namespace wxMSWKeyboard

// small helper for wxGetKeyState() and wxGetMouseState()
static inline bool wxIsKeyDown(WXWORD vk)
{
    if ( vk == VK_LBUTTON || vk == VK_RBUTTON )
    {
        if ( ::GetSystemMetrics(SM_SWAPBUTTON) )
        {
            if ( vk == VK_LBUTTON )
                vk = VK_RBUTTON;
            else // vk == VK_RBUTTON
                vk = VK_LBUTTON;
        }
    }

    // the low order bit indicates whether the key was pressed since the last
    // call and the high order one indicates whether it is down right now and
    // we only want that one
    return (GetAsyncKeyState(vk) & (1<<15)) != 0;
}

bool wxGetKeyState(wxKeyCode key)
{
    // although this does work under Windows, it is not supported under other
    // platforms so don't allow it, you must use wxGetMouseState() instead
    wxASSERT_MSG( key != VK_LBUTTON &&
                    key != VK_RBUTTON &&
                        key != VK_MBUTTON,
                    wxT("can't use wxGetKeyState() for mouse buttons") );

    const WXWORD vk = wxMSWKeyboard::WXToVK(key);

    // if the requested key is a LED key, return true if the led is pressed
    if ( key == WXK_NUMLOCK || key == WXK_CAPITAL || key == WXK_SCROLL )
    {
        // low order bit means LED is highlighted and high order one means the
        // key is down; for compatibility with the other ports return true if
        // either one is set
        return GetKeyState(vk) != 0;

    }
    else // normal key
    {
        return wxIsKeyDown(vk);
    }
}


wxMouseState wxGetMouseState()
{
    wxMouseState ms;
    POINT pt;
    wxGetCursorPosMSW(&pt);

    ms.SetX(pt.x);
    ms.SetY(pt.y);
    ms.SetLeftDown(wxIsKeyDown(VK_LBUTTON));
    ms.SetMiddleDown(wxIsKeyDown(VK_MBUTTON));
    ms.SetRightDown(wxIsKeyDown(VK_RBUTTON));
    ms.SetAux1Down(wxIsKeyDown(VK_XBUTTON1));
    ms.SetAux2Down(wxIsKeyDown(VK_XBUTTON2));

    ms.SetControlDown(wxIsCtrlDown ());
    ms.SetShiftDown  (wxIsShiftDown());
    ms.SetAltDown    (wxIsAltDown  ());
//    ms.SetMetaDown();

    return ms;
}


wxWindow *wxGetActiveWindow()
{
    HWND hWnd = GetActiveWindow();
    if ( hWnd != 0 )
    {
        return wxFindWinFromHandle(hWnd);
    }
    return NULL;
}

extern wxWindow *wxGetWindowFromHWND(WXHWND hWnd)
{
    HWND hwnd = (HWND)hWnd;

    // For a radiobutton, we get the radiobox from GWL_USERDATA (which is set
    // by code in msw/radiobox.cpp), for all the others we just search up the
    // window hierarchy
    wxWindow *win = NULL;
    if ( hwnd )
    {
        win = wxFindWinFromHandle(hwnd);
        if ( !win )
        {
#if wxUSE_RADIOBOX && !defined(__WXUNIVERSAL__)
            // native radiobuttons return DLGC_RADIOBUTTON here and for any
            // wxWindow class which overrides WM_GETDLGCODE processing to
            // do it as well, win would be already non NULL
            if ( ::SendMessage(hwnd, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON )
            {
                win = wxRadioBox::GetFromRadioButtonHWND(hwnd);
            }
            //else: it's a wxRadioButton, not a radiobutton from wxRadioBox
#endif // wxUSE_RADIOBOX

            // spin control text buddy window should be mapped to spin ctrl
            // itself so try it too
#if wxUSE_SPINCTRL && !defined(__WXUNIVERSAL__)
            if ( !win )
            {
                win = wxSpinCtrl::GetSpinForTextCtrl((WXHWND)hwnd);
            }
#endif // wxUSE_SPINCTRL
        }
    }

    while ( hwnd && !win )
    {
        // this is a really ugly hack needed to avoid mistakenly returning the
        // parent frame wxWindow for the find/replace modeless dialog HWND -
        // this, in turn, is needed to call IsDialogMessage() from
        // wxApp::ProcessMessage() as for this we must return NULL from here
        //
        // FIXME: this is clearly not the best way to do it but I think we'll
        //        need to change HWND <-> wxWindow code more heavily than I can
        //        do it now to fix it
        if ( ::GetWindow(hwnd, GW_OWNER) )
        {
            // it's a dialog box, don't go upwards
            break;
        }

        hwnd = ::GetParent(hwnd);
        win = wxFindWinFromHandle(hwnd);
    }

    return win;
}

// Windows keyboard hook. Allows interception of e.g. F1, ESCAPE
// in active frames and dialogs, regardless of where the focus is.
static HHOOK wxTheKeyboardHook = 0;

LRESULT APIENTRY
wxKeyboardHook(int nCode, WXWPARAM wParam, WXLPARAM lParam)
{
    DWORD hiWord = HIWORD(lParam);
    if ( nCode != HC_NOREMOVE && ((hiWord & KF_UP) == 0) )
    {
        wchar_t uc = 0;
        int id = wxMSWKeyboard::VKToWX(wParam, lParam, &uc);

        // Don't intercept keyboard entry (notably Escape) if a modal window
        // (not managed by wx, e.g. IME one) is currently opened as more often
        // than not it needs all the keys for itself.
        //
        // Also don't catch it if a window currently captures the mouse as
        // Escape is normally used to release the mouse capture and if you
        // really need to catch all the keys in the window that has mouse
        // capture it can be easily done in its own EVT_CHAR handler as it is
        // certain to have focus while it has the capture.
        if ( !gs_modalEntryWindowCount && !::GetCapture() )
        {
            if ( id != WXK_NONE
#if wxUSE_UNICODE
                    || static_cast<int>(uc) != WXK_NONE
#endif // wxUSE_UNICODE
                    )
            {
                wxWindow const* win = wxWindow::DoFindFocus();
                if ( !win )
                {
                    // Even if the focus got lost somehow, still send the event
                    // to the top level parent to allow a wxDialog to always
                    // close on Escape.
                    win = wxGetActiveWindow();
                }

                wxKeyEvent event(wxEVT_CHAR_HOOK);
                MSWInitAnyKeyEvent(event, wParam, lParam, win);

                event.m_keyCode = id;
#if wxUSE_UNICODE
                event.m_uniChar = uc;
#endif // wxUSE_UNICODE

                wxEvtHandler * const handler = win ? win->GetEventHandler()
                                                   : wxTheApp;

                if ( handler && handler->ProcessEvent(event) )
                {
                    if ( !event.IsNextEventAllowed() )
                    {
                        // Stop processing of this event.
                        return 1;
                    }
                }
            }
        }
    }

    return (int)CallNextHookEx(wxTheKeyboardHook, nCode, wParam, lParam);
}

void wxSetKeyboardHook(bool doIt)
{
    if ( doIt )
    {
        wxTheKeyboardHook = ::SetWindowsHookEx
                              (
                                WH_KEYBOARD,
                                wxKeyboardHook,
                                NULL,   // must be NULL for process hook
                                ::GetCurrentThreadId()
                              );
        if ( !wxTheKeyboardHook )
        {
            wxLogLastError(wxT("SetWindowsHookEx(wxKeyboardHook)"));
        }
    }
    else // uninstall
    {
        if ( wxTheKeyboardHook )
            ::UnhookWindowsHookEx(wxTheKeyboardHook);
    }
}

#if wxDEBUG_LEVEL >= 2
const wxChar *wxGetMessageName(int message)
{
    switch ( message )
    {
        case 0x0000: return wxT("WM_NULL");
        case 0x0001: return wxT("WM_CREATE");
        case 0x0002: return wxT("WM_DESTROY");
        case 0x0003: return wxT("WM_MOVE");
        case 0x0005: return wxT("WM_SIZE");
        case 0x0006: return wxT("WM_ACTIVATE");
        case 0x0007: return wxT("WM_SETFOCUS");
        case 0x0008: return wxT("WM_KILLFOCUS");
        case 0x000A: return wxT("WM_ENABLE");
        case 0x000B: return wxT("WM_SETREDRAW");
        case 0x000C: return wxT("WM_SETTEXT");
        case 0x000D: return wxT("WM_GETTEXT");
        case 0x000E: return wxT("WM_GETTEXTLENGTH");
        case 0x000F: return wxT("WM_PAINT");
        case 0x0010: return wxT("WM_CLOSE");
        case 0x0011: return wxT("WM_QUERYENDSESSION");
        case 0x0012: return wxT("WM_QUIT");
        case 0x0013: return wxT("WM_QUERYOPEN");
        case 0x0014: return wxT("WM_ERASEBKGND");
        case 0x0015: return wxT("WM_SYSCOLORCHANGE");
        case 0x0016: return wxT("WM_ENDSESSION");
        case 0x0017: return wxT("WM_SYSTEMERROR");
        case 0x0018: return wxT("WM_SHOWWINDOW");
        case 0x0019: return wxT("WM_CTLCOLOR");
        case 0x001A: return wxT("WM_WININICHANGE");
        case 0x001B: return wxT("WM_DEVMODECHANGE");
        case 0x001C: return wxT("WM_ACTIVATEAPP");
        case 0x001D: return wxT("WM_FONTCHANGE");
        case 0x001E: return wxT("WM_TIMECHANGE");
        case 0x001F: return wxT("WM_CANCELMODE");
        case 0x0020: return wxT("WM_SETCURSOR");
        case 0x0021: return wxT("WM_MOUSEACTIVATE");
        case 0x0022: return wxT("WM_CHILDACTIVATE");
        case 0x0023: return wxT("WM_QUEUESYNC");
        case 0x0024: return wxT("WM_GETMINMAXINFO");
        case 0x0026: return wxT("WM_PAINTICON");
        case 0x0027: return wxT("WM_ICONERASEBKGND");
        case 0x0028: return wxT("WM_NEXTDLGCTL");
        case 0x002A: return wxT("WM_SPOOLERSTATUS");
        case 0x002B: return wxT("WM_DRAWITEM");
        case 0x002C: return wxT("WM_MEASUREITEM");
        case 0x002D: return wxT("WM_DELETEITEM");
        case 0x002E: return wxT("WM_VKEYTOITEM");
        case 0x002F: return wxT("WM_CHARTOITEM");
        case 0x0030: return wxT("WM_SETFONT");
        case 0x0031: return wxT("WM_GETFONT");
        case 0x0037: return wxT("WM_QUERYDRAGICON");
        case 0x0039: return wxT("WM_COMPAREITEM");
        case 0x0041: return wxT("WM_COMPACTING");
        case 0x0044: return wxT("WM_COMMNOTIFY");
        case 0x0046: return wxT("WM_WINDOWPOSCHANGING");
        case 0x0047: return wxT("WM_WINDOWPOSCHANGED");
        case 0x0048: return wxT("WM_POWER");

        case 0x004A: return wxT("WM_COPYDATA");
        case 0x004B: return wxT("WM_CANCELJOURNAL");
        case 0x004E: return wxT("WM_NOTIFY");
        case 0x0050: return wxT("WM_INPUTLANGCHANGEREQUEST");
        case 0x0051: return wxT("WM_INPUTLANGCHANGE");
        case 0x0052: return wxT("WM_TCARD");
        case 0x0053: return wxT("WM_HELP");
        case 0x0054: return wxT("WM_USERCHANGED");
        case 0x0055: return wxT("WM_NOTIFYFORMAT");
        case 0x007B: return wxT("WM_CONTEXTMENU");
        case 0x007C: return wxT("WM_STYLECHANGING");
        case 0x007D: return wxT("WM_STYLECHANGED");
        case 0x007E: return wxT("WM_DISPLAYCHANGE");
        case 0x007F: return wxT("WM_GETICON");
        case 0x0080: return wxT("WM_SETICON");

        case 0x0081: return wxT("WM_NCCREATE");
        case 0x0082: return wxT("WM_NCDESTROY");
        case 0x0083: return wxT("WM_NCCALCSIZE");
        case 0x0084: return wxT("WM_NCHITTEST");
        case 0x0085: return wxT("WM_NCPAINT");
        case 0x0086: return wxT("WM_NCACTIVATE");
        case 0x0087: return wxT("WM_GETDLGCODE");
        case 0x00A0: return wxT("WM_NCMOUSEMOVE");
        case 0x00A1: return wxT("WM_NCLBUTTONDOWN");
        case 0x00A2: return wxT("WM_NCLBUTTONUP");
        case 0x00A3: return wxT("WM_NCLBUTTONDBLCLK");
        case 0x00A4: return wxT("WM_NCRBUTTONDOWN");
        case 0x00A5: return wxT("WM_NCRBUTTONUP");
        case 0x00A6: return wxT("WM_NCRBUTTONDBLCLK");
        case 0x00A7: return wxT("WM_NCMBUTTONDOWN");
        case 0x00A8: return wxT("WM_NCMBUTTONUP");
        case 0x00A9: return wxT("WM_NCMBUTTONDBLCLK");

        case 0x00B0: return wxT("EM_GETSEL");
        case 0x00B1: return wxT("EM_SETSEL");
        case 0x00B2: return wxT("EM_GETRECT");
        case 0x00B3: return wxT("EM_SETRECT");
        case 0x00B4: return wxT("EM_SETRECTNP");
        case 0x00B5: return wxT("EM_SCROLL");
        case 0x00B6: return wxT("EM_LINESCROLL");
        case 0x00B7: return wxT("EM_SCROLLCARET");
        case 0x00B8: return wxT("EM_GETMODIFY");
        case 0x00B9: return wxT("EM_SETMODIFY");
        case 0x00BA: return wxT("EM_GETLINECOUNT");
        case 0x00BB: return wxT("EM_LINEINDEX");
        case 0x00BC: return wxT("EM_SETHANDLE");
        case 0x00BD: return wxT("EM_GETHANDLE");
        case 0x00BE: return wxT("EM_GETTHUMB");
        case 0x00C1: return wxT("EM_LINELENGTH");
        case 0x00C2: return wxT("EM_REPLACESEL");
        case 0x00C4: return wxT("EM_GETLINE");
        case 0x00C5: return wxT("EM_LIMITTEXT/EM_SETLIMITTEXT"); /* ;win40 Name change */
        case 0x00C6: return wxT("EM_CANUNDO");
        case 0x00C7: return wxT("EM_UNDO");
        case 0x00C8: return wxT("EM_FMTLINES");
        case 0x00C9: return wxT("EM_LINEFROMCHAR");
        case 0x00CB: return wxT("EM_SETTABSTOPS");
        case 0x00CC: return wxT("EM_SETPASSWORDCHAR");
        case 0x00CD: return wxT("EM_EMPTYUNDOBUFFER");
        case 0x00CE: return wxT("EM_GETFIRSTVISIBLELINE");
        case 0x00CF: return wxT("EM_SETREADONLY");
        case 0x00D0: return wxT("EM_SETWORDBREAKPROC");
        case 0x00D1: return wxT("EM_GETWORDBREAKPROC");
        case 0x00D2: return wxT("EM_GETPASSWORDCHAR");
        case 0x00D3: return wxT("EM_SETMARGINS");
        case 0x00D4: return wxT("EM_GETMARGINS");
        case 0x00D5: return wxT("EM_GETLIMITTEXT");
        case 0x00D6: return wxT("EM_POSFROMCHAR");
        case 0x00D7: return wxT("EM_CHARFROMPOS");
        case 0x00D8: return wxT("EM_SETIMESTATUS");
        case 0x00D9: return wxT("EM_GETIMESTATUS");

        case 0x0100: return wxT("WM_KEYDOWN");
        case 0x0101: return wxT("WM_KEYUP");
        case 0x0102: return wxT("WM_CHAR");
        case 0x0103: return wxT("WM_DEADCHAR");
        case 0x0104: return wxT("WM_SYSKEYDOWN");
        case 0x0105: return wxT("WM_SYSKEYUP");
        case 0x0106: return wxT("WM_SYSCHAR");
        case 0x0107: return wxT("WM_SYSDEADCHAR");
        case 0x0108: return wxT("WM_KEYLAST");

        case 0x010D: return wxT("WM_IME_STARTCOMPOSITION");
        case 0x010E: return wxT("WM_IME_ENDCOMPOSITION");
        case 0x010F: return wxT("WM_IME_COMPOSITION");

        case 0x0110: return wxT("WM_INITDIALOG");
        case 0x0111: return wxT("WM_COMMAND");
        case 0x0112: return wxT("WM_SYSCOMMAND");
        case 0x0113: return wxT("WM_TIMER");
        case 0x0114: return wxT("WM_HSCROLL");
        case 0x0115: return wxT("WM_VSCROLL");
        case 0x0116: return wxT("WM_INITMENU");
        case 0x0117: return wxT("WM_INITMENUPOPUP");
        case 0x011F: return wxT("WM_MENUSELECT");
        case 0x0120: return wxT("WM_MENUCHAR");
        case 0x0121: return wxT("WM_ENTERIDLE");

        case 0x0127: return wxT("WM_CHANGEUISTATE");
        case 0x0128: return wxT("WM_UPDATEUISTATE");
        case 0x0129: return wxT("WM_QUERYUISTATE");

        case 0x0132: return wxT("WM_CTLCOLORMSGBOX");
        case 0x0133: return wxT("WM_CTLCOLOREDIT");
        case 0x0134: return wxT("WM_CTLCOLORLISTBOX");
        case 0x0135: return wxT("WM_CTLCOLORBTN");
        case 0x0136: return wxT("WM_CTLCOLORDLG");
        case 0x0137: return wxT("WM_CTLCOLORSCROLLBAR");
        case 0x0138: return wxT("WM_CTLCOLORSTATIC");
        case 0x01E1: return wxT("MN_GETHMENU");

        case 0x0200: return wxT("WM_MOUSEMOVE");
        case 0x0201: return wxT("WM_LBUTTONDOWN");
        case 0x0202: return wxT("WM_LBUTTONUP");
        case 0x0203: return wxT("WM_LBUTTONDBLCLK");
        case 0x0204: return wxT("WM_RBUTTONDOWN");
        case 0x0205: return wxT("WM_RBUTTONUP");
        case 0x0206: return wxT("WM_RBUTTONDBLCLK");
        case 0x0207: return wxT("WM_MBUTTONDOWN");
        case 0x0208: return wxT("WM_MBUTTONUP");
        case 0x0209: return wxT("WM_MBUTTONDBLCLK");
        case 0x020A: return wxT("WM_MOUSEWHEEL");
        case 0x020B: return wxT("WM_XBUTTONDOWN");
        case 0x020C: return wxT("WM_XBUTTONUP");
        case 0x020D: return wxT("WM_XBUTTONDBLCLK");
        case 0x0210: return wxT("WM_PARENTNOTIFY");
        case 0x0211: return wxT("WM_ENTERMENULOOP");
        case 0x0212: return wxT("WM_EXITMENULOOP");

        case 0x0213: return wxT("WM_NEXTMENU");
        case 0x0214: return wxT("WM_SIZING");
        case 0x0215: return wxT("WM_CAPTURECHANGED");
        case 0x0216: return wxT("WM_MOVING");
        case 0x0218: return wxT("WM_POWERBROADCAST");
        case 0x0219: return wxT("WM_DEVICECHANGE");

        case 0x0220: return wxT("WM_MDICREATE");
        case 0x0221: return wxT("WM_MDIDESTROY");
        case 0x0222: return wxT("WM_MDIACTIVATE");
        case 0x0223: return wxT("WM_MDIRESTORE");
        case 0x0224: return wxT("WM_MDINEXT");
        case 0x0225: return wxT("WM_MDIMAXIMIZE");
        case 0x0226: return wxT("WM_MDITILE");
        case 0x0227: return wxT("WM_MDICASCADE");
        case 0x0228: return wxT("WM_MDIICONARRANGE");
        case 0x0229: return wxT("WM_MDIGETACTIVE");
        case 0x0230: return wxT("WM_MDISETMENU");
        case 0x0233: return wxT("WM_DROPFILES");

        case 0x0281: return wxT("WM_IME_SETCONTEXT");
        case 0x0282: return wxT("WM_IME_NOTIFY");
        case 0x0283: return wxT("WM_IME_CONTROL");
        case 0x0284: return wxT("WM_IME_COMPOSITIONFULL");
        case 0x0285: return wxT("WM_IME_SELECT");
        case 0x0286: return wxT("WM_IME_CHAR");
        case 0x0290: return wxT("WM_IME_KEYDOWN");
        case 0x0291: return wxT("WM_IME_KEYUP");

        case 0x02A0: return wxT("WM_NCMOUSEHOVER");
        case 0x02A1: return wxT("WM_MOUSEHOVER");
        case 0x02A2: return wxT("WM_NCMOUSELEAVE");
        case 0x02A3: return wxT("WM_MOUSELEAVE");

        case 0x0300: return wxT("WM_CUT");
        case 0x0301: return wxT("WM_COPY");
        case 0x0302: return wxT("WM_PASTE");
        case 0x0303: return wxT("WM_CLEAR");
        case 0x0304: return wxT("WM_UNDO");
        case 0x0305: return wxT("WM_RENDERFORMAT");
        case 0x0306: return wxT("WM_RENDERALLFORMATS");
        case 0x0307: return wxT("WM_DESTROYCLIPBOARD");
        case 0x0308: return wxT("WM_DRAWCLIPBOARD");
        case 0x0309: return wxT("WM_PAINTCLIPBOARD");
        case 0x030A: return wxT("WM_VSCROLLCLIPBOARD");
        case 0x030B: return wxT("WM_SIZECLIPBOARD");
        case 0x030C: return wxT("WM_ASKCBFORMATNAME");
        case 0x030D: return wxT("WM_CHANGECBCHAIN");
        case 0x030E: return wxT("WM_HSCROLLCLIPBOARD");
        case 0x030F: return wxT("WM_QUERYNEWPALETTE");
        case 0x0310: return wxT("WM_PALETTEISCHANGING");
        case 0x0311: return wxT("WM_PALETTECHANGED");
        case 0x0312: return wxT("WM_HOTKEY");

        case 0x0317: return wxT("WM_PRINT");
        case 0x0318: return wxT("WM_PRINTCLIENT");

        // common controls messages - although they're not strictly speaking
        // standard, it's nice to decode them nevertheless

        // listview
        case 0x1000 + 0: return wxT("LVM_GETBKCOLOR");
        case 0x1000 + 1: return wxT("LVM_SETBKCOLOR");
        case 0x1000 + 2: return wxT("LVM_GETIMAGELIST");
        case 0x1000 + 3: return wxT("LVM_SETIMAGELIST");
        case 0x1000 + 4: return wxT("LVM_GETITEMCOUNT");
        case 0x1000 + 5: return wxT("LVM_GETITEMA");
        case 0x1000 + 75: return wxT("LVM_GETITEMW");
        case 0x1000 + 6: return wxT("LVM_SETITEMA");
        case 0x1000 + 76: return wxT("LVM_SETITEMW");
        case 0x1000 + 7: return wxT("LVM_INSERTITEMA");
        case 0x1000 + 77: return wxT("LVM_INSERTITEMW");
        case 0x1000 + 8: return wxT("LVM_DELETEITEM");
        case 0x1000 + 9: return wxT("LVM_DELETEALLITEMS");
        case 0x1000 + 10: return wxT("LVM_GETCALLBACKMASK");
        case 0x1000 + 11: return wxT("LVM_SETCALLBACKMASK");
        case 0x1000 + 12: return wxT("LVM_GETNEXTITEM");
        case 0x1000 + 13: return wxT("LVM_FINDITEMA");
        case 0x1000 + 83: return wxT("LVM_FINDITEMW");
        case 0x1000 + 14: return wxT("LVM_GETITEMRECT");
        case 0x1000 + 15: return wxT("LVM_SETITEMPOSITION");
        case 0x1000 + 16: return wxT("LVM_GETITEMPOSITION");
        case 0x1000 + 17: return wxT("LVM_GETSTRINGWIDTHA");
        case 0x1000 + 87: return wxT("LVM_GETSTRINGWIDTHW");
        case 0x1000 + 18: return wxT("LVM_HITTEST");
        case 0x1000 + 19: return wxT("LVM_ENSUREVISIBLE");
        case 0x1000 + 20: return wxT("LVM_SCROLL");
        case 0x1000 + 21: return wxT("LVM_REDRAWITEMS");
        case 0x1000 + 22: return wxT("LVM_ARRANGE");
        case 0x1000 + 23: return wxT("LVM_EDITLABELA");
        case 0x1000 + 118: return wxT("LVM_EDITLABELW");
        case 0x1000 + 24: return wxT("LVM_GETEDITCONTROL");
        case 0x1000 + 25: return wxT("LVM_GETCOLUMNA");
        case 0x1000 + 95: return wxT("LVM_GETCOLUMNW");
        case 0x1000 + 26: return wxT("LVM_SETCOLUMNA");
        case 0x1000 + 96: return wxT("LVM_SETCOLUMNW");
        case 0x1000 + 27: return wxT("LVM_INSERTCOLUMNA");
        case 0x1000 + 97: return wxT("LVM_INSERTCOLUMNW");
        case 0x1000 + 28: return wxT("LVM_DELETECOLUMN");
        case 0x1000 + 29: return wxT("LVM_GETCOLUMNWIDTH");
        case 0x1000 + 30: return wxT("LVM_SETCOLUMNWIDTH");
        case 0x1000 + 31: return wxT("LVM_GETHEADER");
        case 0x1000 + 33: return wxT("LVM_CREATEDRAGIMAGE");
        case 0x1000 + 34: return wxT("LVM_GETVIEWRECT");
        case 0x1000 + 35: return wxT("LVM_GETTEXTCOLOR");
        case 0x1000 + 36: return wxT("LVM_SETTEXTCOLOR");
        case 0x1000 + 37: return wxT("LVM_GETTEXTBKCOLOR");
        case 0x1000 + 38: return wxT("LVM_SETTEXTBKCOLOR");
        case 0x1000 + 39: return wxT("LVM_GETTOPINDEX");
        case 0x1000 + 40: return wxT("LVM_GETCOUNTPERPAGE");
        case 0x1000 + 41: return wxT("LVM_GETORIGIN");
        case 0x1000 + 42: return wxT("LVM_UPDATE");
        case 0x1000 + 43: return wxT("LVM_SETITEMSTATE");
        case 0x1000 + 44: return wxT("LVM_GETITEMSTATE");
        case 0x1000 + 45: return wxT("LVM_GETITEMTEXTA");
        case 0x1000 + 115: return wxT("LVM_GETITEMTEXTW");
        case 0x1000 + 46: return wxT("LVM_SETITEMTEXTA");
        case 0x1000 + 116: return wxT("LVM_SETITEMTEXTW");
        case 0x1000 + 47: return wxT("LVM_SETITEMCOUNT");
        case 0x1000 + 48: return wxT("LVM_SORTITEMS");
        case 0x1000 + 49: return wxT("LVM_SETITEMPOSITION32");
        case 0x1000 + 50: return wxT("LVM_GETSELECTEDCOUNT");
        case 0x1000 + 51: return wxT("LVM_GETITEMSPACING");
        case 0x1000 + 52: return wxT("LVM_GETISEARCHSTRINGA");
        case 0x1000 + 117: return wxT("LVM_GETISEARCHSTRINGW");
        case 0x1000 + 53: return wxT("LVM_SETICONSPACING");
        case 0x1000 + 54: return wxT("LVM_SETEXTENDEDLISTVIEWSTYLE");
        case 0x1000 + 55: return wxT("LVM_GETEXTENDEDLISTVIEWSTYLE");
        case 0x1000 + 56: return wxT("LVM_GETSUBITEMRECT");
        case 0x1000 + 57: return wxT("LVM_SUBITEMHITTEST");
        case 0x1000 + 58: return wxT("LVM_SETCOLUMNORDERARRAY");
        case 0x1000 + 59: return wxT("LVM_GETCOLUMNORDERARRAY");
        case 0x1000 + 60: return wxT("LVM_SETHOTITEM");
        case 0x1000 + 61: return wxT("LVM_GETHOTITEM");
        case 0x1000 + 62: return wxT("LVM_SETHOTCURSOR");
        case 0x1000 + 63: return wxT("LVM_GETHOTCURSOR");
        case 0x1000 + 64: return wxT("LVM_APPROXIMATEVIEWRECT");
        case 0x1000 + 65: return wxT("LVM_SETWORKAREA");

        // tree view
        case 0x1100 + 0: return wxT("TVM_INSERTITEMA");
        case 0x1100 + 50: return wxT("TVM_INSERTITEMW");
        case 0x1100 + 1: return wxT("TVM_DELETEITEM");
        case 0x1100 + 2: return wxT("TVM_EXPAND");
        case 0x1100 + 4: return wxT("TVM_GETITEMRECT");
        case 0x1100 + 5: return wxT("TVM_GETCOUNT");
        case 0x1100 + 6: return wxT("TVM_GETINDENT");
        case 0x1100 + 7: return wxT("TVM_SETINDENT");
        case 0x1100 + 8: return wxT("TVM_GETIMAGELIST");
        case 0x1100 + 9: return wxT("TVM_SETIMAGELIST");
        case 0x1100 + 10: return wxT("TVM_GETNEXTITEM");
        case 0x1100 + 11: return wxT("TVM_SELECTITEM");
        case 0x1100 + 12: return wxT("TVM_GETITEMA");
        case 0x1100 + 62: return wxT("TVM_GETITEMW");
        case 0x1100 + 13: return wxT("TVM_SETITEMA");
        case 0x1100 + 63: return wxT("TVM_SETITEMW");
        case 0x1100 + 14: return wxT("TVM_EDITLABELA");
        case 0x1100 + 65: return wxT("TVM_EDITLABELW");
        case 0x1100 + 15: return wxT("TVM_GETEDITCONTROL");
        case 0x1100 + 16: return wxT("TVM_GETVISIBLECOUNT");
        case 0x1100 + 17: return wxT("TVM_HITTEST");
        case 0x1100 + 18: return wxT("TVM_CREATEDRAGIMAGE");
        case 0x1100 + 19: return wxT("TVM_SORTCHILDREN");
        case 0x1100 + 20: return wxT("TVM_ENSUREVISIBLE");
        case 0x1100 + 21: return wxT("TVM_SORTCHILDRENCB");
        case 0x1100 + 22: return wxT("TVM_ENDEDITLABELNOW");
        case 0x1100 + 23: return wxT("TVM_GETISEARCHSTRINGA");
        case 0x1100 + 64: return wxT("TVM_GETISEARCHSTRINGW");
        case 0x1100 + 24: return wxT("TVM_SETTOOLTIPS");
        case 0x1100 + 25: return wxT("TVM_GETTOOLTIPS");

        // header
        case 0x1200 + 0: return wxT("HDM_GETITEMCOUNT");
        case 0x1200 + 1: return wxT("HDM_INSERTITEMA");
        case 0x1200 + 10: return wxT("HDM_INSERTITEMW");
        case 0x1200 + 2: return wxT("HDM_DELETEITEM");
        case 0x1200 + 3: return wxT("HDM_GETITEMA");
        case 0x1200 + 11: return wxT("HDM_GETITEMW");
        case 0x1200 + 4: return wxT("HDM_SETITEMA");
        case 0x1200 + 12: return wxT("HDM_SETITEMW");
        case 0x1200 + 5: return wxT("HDM_LAYOUT");
        case 0x1200 + 6: return wxT("HDM_HITTEST");
        case 0x1200 + 7: return wxT("HDM_GETITEMRECT");
        case 0x1200 + 8: return wxT("HDM_SETIMAGELIST");
        case 0x1200 + 9: return wxT("HDM_GETIMAGELIST");
        case 0x1200 + 15: return wxT("HDM_ORDERTOINDEX");
        case 0x1200 + 16: return wxT("HDM_CREATEDRAGIMAGE");
        case 0x1200 + 17: return wxT("HDM_GETORDERARRAY");
        case 0x1200 + 18: return wxT("HDM_SETORDERARRAY");
        case 0x1200 + 19: return wxT("HDM_SETHOTDIVIDER");

        // tab control
        case 0x1300 + 2: return wxT("TCM_GETIMAGELIST");
        case 0x1300 + 3: return wxT("TCM_SETIMAGELIST");
        case 0x1300 + 4: return wxT("TCM_GETITEMCOUNT");
        case 0x1300 + 5: return wxT("TCM_GETITEMA");
        case 0x1300 + 60: return wxT("TCM_GETITEMW");
        case 0x1300 + 6: return wxT("TCM_SETITEMA");
        case 0x1300 + 61: return wxT("TCM_SETITEMW");
        case 0x1300 + 7: return wxT("TCM_INSERTITEMA");
        case 0x1300 + 62: return wxT("TCM_INSERTITEMW");
        case 0x1300 + 8: return wxT("TCM_DELETEITEM");
        case 0x1300 + 9: return wxT("TCM_DELETEALLITEMS");
        case 0x1300 + 10: return wxT("TCM_GETITEMRECT");
        case 0x1300 + 11: return wxT("TCM_GETCURSEL");
        case 0x1300 + 12: return wxT("TCM_SETCURSEL");
        case 0x1300 + 13: return wxT("TCM_HITTEST");
        case 0x1300 + 14: return wxT("TCM_SETITEMEXTRA");
        case 0x1300 + 40: return wxT("TCM_ADJUSTRECT");
        case 0x1300 + 41: return wxT("TCM_SETITEMSIZE");
        case 0x1300 + 42: return wxT("TCM_REMOVEIMAGE");
        case 0x1300 + 43: return wxT("TCM_SETPADDING");
        case 0x1300 + 44: return wxT("TCM_GETROWCOUNT");
        case 0x1300 + 45: return wxT("TCM_GETTOOLTIPS");
        case 0x1300 + 46: return wxT("TCM_SETTOOLTIPS");
        case 0x1300 + 47: return wxT("TCM_GETCURFOCUS");
        case 0x1300 + 48: return wxT("TCM_SETCURFOCUS");
        case 0x1300 + 49: return wxT("TCM_SETMINTABWIDTH");
        case 0x1300 + 50: return wxT("TCM_DESELECTALL");

        // toolbar
        case WM_USER+1: return wxT("TB_ENABLEBUTTON");
        case WM_USER+2: return wxT("TB_CHECKBUTTON");
        case WM_USER+3: return wxT("TB_PRESSBUTTON");
        case WM_USER+4: return wxT("TB_HIDEBUTTON");
        case WM_USER+5: return wxT("TB_INDETERMINATE");
        case WM_USER+9: return wxT("TB_ISBUTTONENABLED");
        case WM_USER+10: return wxT("TB_ISBUTTONCHECKED");
        case WM_USER+11: return wxT("TB_ISBUTTONPRESSED");
        case WM_USER+12: return wxT("TB_ISBUTTONHIDDEN");
        case WM_USER+13: return wxT("TB_ISBUTTONINDETERMINATE");
        case WM_USER+17: return wxT("TB_SETSTATE");
        case WM_USER+18: return wxT("TB_GETSTATE");
        case WM_USER+19: return wxT("TB_ADDBITMAP");
        case WM_USER+20: return wxT("TB_ADDBUTTONS");
        case WM_USER+21: return wxT("TB_INSERTBUTTON");
        case WM_USER+22: return wxT("TB_DELETEBUTTON");
        case WM_USER+23: return wxT("TB_GETBUTTON");
        case WM_USER+24: return wxT("TB_BUTTONCOUNT");
        case WM_USER+25: return wxT("TB_COMMANDTOINDEX");
        case WM_USER+26: return wxT("TB_SAVERESTOREA");
        case WM_USER+76: return wxT("TB_SAVERESTOREW");
        case WM_USER+27: return wxT("TB_CUSTOMIZE");
        case WM_USER+28: return wxT("TB_ADDSTRINGA");
        case WM_USER+77: return wxT("TB_ADDSTRINGW");
        case WM_USER+29: return wxT("TB_GETITEMRECT");
        case WM_USER+30: return wxT("TB_BUTTONSTRUCTSIZE");
        case WM_USER+31: return wxT("TB_SETBUTTONSIZE");
        case WM_USER+32: return wxT("TB_SETBITMAPSIZE");
        case WM_USER+33: return wxT("TB_AUTOSIZE");
        case WM_USER+35: return wxT("TB_GETTOOLTIPS");
        case WM_USER+36: return wxT("TB_SETTOOLTIPS");
        case WM_USER+37: return wxT("TB_SETPARENT");
        case WM_USER+39: return wxT("TB_SETROWS");
        case WM_USER+40: return wxT("TB_GETROWS");
        case WM_USER+42: return wxT("TB_SETCMDID");
        case WM_USER+43: return wxT("TB_CHANGEBITMAP");
        case WM_USER+44: return wxT("TB_GETBITMAP");
        case WM_USER+45: return wxT("TB_GETBUTTONTEXTA");
        case WM_USER+75: return wxT("TB_GETBUTTONTEXTW");
        case WM_USER+46: return wxT("TB_REPLACEBITMAP");
        case WM_USER+47: return wxT("TB_SETINDENT");
        case WM_USER+48: return wxT("TB_SETIMAGELIST");
        case WM_USER+49: return wxT("TB_GETIMAGELIST");
        case WM_USER+50: return wxT("TB_LOADIMAGES");
        case WM_USER+51: return wxT("TB_GETRECT");
        case WM_USER+52: return wxT("TB_SETHOTIMAGELIST");
        case WM_USER+53: return wxT("TB_GETHOTIMAGELIST");
        case WM_USER+54: return wxT("TB_SETDISABLEDIMAGELIST");
        case WM_USER+55: return wxT("TB_GETDISABLEDIMAGELIST");
        case WM_USER+56: return wxT("TB_SETSTYLE");
        case WM_USER+57: return wxT("TB_GETSTYLE");
        case WM_USER+58: return wxT("TB_GETBUTTONSIZE");
        case WM_USER+59: return wxT("TB_SETBUTTONWIDTH");
        case WM_USER+60: return wxT("TB_SETMAXTEXTROWS");
        case WM_USER+61: return wxT("TB_GETTEXTROWS");
        case WM_USER+41: return wxT("TB_GETBITMAPFLAGS");

        default:
            static wxString s_szBuf;
            s_szBuf.Printf(wxT("<unknown message = %d>"), message);
            return s_szBuf.c_str();
    }
}
#endif // wxDEBUG_LEVEL >= 2

static TEXTMETRIC wxGetTextMetrics(const wxWindowMSW *win)
{
    // prepare the DC
    TEXTMETRIC tm;
    HWND hwnd = GetHwndOf(win);
    HDC hdc = ::GetDC(hwnd);

#if !wxDIALOG_UNIT_COMPATIBILITY
    // and select the current font into it
    HFONT hfont = GetHfontOf(win->GetFont());
    if ( hfont )
    {
        hfont = (HFONT)::SelectObject(hdc, hfont);
    }
#endif

    // finally retrieve the text metrics from it
    GetTextMetrics(hdc, &tm);

#if !wxDIALOG_UNIT_COMPATIBILITY
    // and clean up
    if ( hfont )
    {
        (void)::SelectObject(hdc, hfont);
    }
#endif

    ::ReleaseDC(hwnd, hdc);

    return tm;
}

// Find the wxWindow at the current mouse position, returning the mouse
// position.
wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    return wxFindWindowAtPoint(pt);
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    POINT pt2;
    pt2.x = pt.x;
    pt2.y = pt.y;

    HWND hWnd = ::WindowFromPoint(pt2);
    if ( hWnd )
    {
        // WindowFromPoint() ignores the disabled children but we're supposed
        // to take them into account, so check if we have a child at this
        // coordinate using ChildWindowFromPointEx().
        for ( ;; )
        {
            pt2.x = pt.x;
            pt2.y = pt.y;
            ::ScreenToClient(hWnd, &pt2);
            HWND child = ::ChildWindowFromPointEx(hWnd, pt2, CWP_SKIPINVISIBLE);
            if ( child == hWnd || !child )
                break;

            // ChildWindowFromPointEx() only examines the immediate children
            // but we want to get the deepest (top in Z-order) one, so continue
            // iterating for as long as it finds anything.
            hWnd = child;
        }
    }

    return wxGetWindowFromHWND((WXHWND)hWnd);
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    POINT pt;
    wxGetCursorPosMSW(&pt);

    return wxPoint(pt.x, pt.y);
}

#if wxUSE_HOTKEY

bool wxWindowMSW::RegisterHotKey(int hotkeyId, int modifiers, int keycode)
{
    UINT win_modifiers=0;
    if ( modifiers & wxMOD_ALT )
        win_modifiers |= MOD_ALT;
    if ( modifiers & wxMOD_SHIFT )
        win_modifiers |= MOD_SHIFT;
    if ( modifiers & wxMOD_CONTROL )
        win_modifiers |= MOD_CONTROL;
    if ( modifiers & wxMOD_WIN )
        win_modifiers |= MOD_WIN;

    if ( !::RegisterHotKey(GetHwnd(), hotkeyId, win_modifiers, keycode) )
    {
        wxLogLastError(wxT("RegisterHotKey"));

        return false;
    }

    return true;
}

bool wxWindowMSW::UnregisterHotKey(int hotkeyId)
{
    if ( !::UnregisterHotKey(GetHwnd(), hotkeyId) )
    {
        wxLogLastError(wxT("UnregisterHotKey"));

        return false;
    }

    return true;
}

bool wxWindowMSW::HandleHotKey(WXWPARAM wParam, WXLPARAM lParam)
{
    int win_modifiers = LOWORD(lParam);

    wxKeyEvent event(CreateKeyEvent(wxEVT_HOTKEY, HIWORD(lParam)));
    event.SetId(wParam);
    event.m_shiftDown = (win_modifiers & MOD_SHIFT) != 0;
    event.m_controlDown = (win_modifiers & MOD_CONTROL) != 0;
    event.m_altDown = (win_modifiers & MOD_ALT) != 0;
    event.m_metaDown = (win_modifiers & MOD_WIN) != 0;

    return HandleWindowEvent(event);
}

#endif // wxUSE_HOTKEY

// this class installs a message hook which really wakes up our idle processing
// each time a message is handled, even if we're sitting inside a local modal
// loop (e.g. a menu is opened or scrollbar is being dragged or even inside
// ::MessageBox()) and so don't control message dispatching otherwise
class wxIdleWakeUpModule : public wxModule
{
public:
    virtual bool OnInit() wxOVERRIDE
    {
        ms_hMsgHookProc = ::SetWindowsHookEx
                            (
                             WH_GETMESSAGE,
                             &wxIdleWakeUpModule::MsgHookProc,
                             NULL,
                             GetCurrentThreadId()
                            );

        if ( !ms_hMsgHookProc )
        {
            wxLogLastError(wxT("SetWindowsHookEx(WH_GETMESSAGE)"));

            return false;
        }

        return true;
    }

    virtual void OnExit() wxOVERRIDE
    {
        ::UnhookWindowsHookEx(wxIdleWakeUpModule::ms_hMsgHookProc);
    }

    static LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        // Don't process idle events unless the message is going to be really
        // handled, i.e. removed from the queue, as it seems wrong to do it
        // just because someone called PeekMessage(PM_NOREMOVE).
        if ( wParam == PM_REMOVE )
            wxTheApp->MSWProcessPendingEventsIfNeeded();

        return CallNextHookEx(ms_hMsgHookProc, nCode, wParam, lParam);
    }

private:
    static HHOOK ms_hMsgHookProc;

    wxDECLARE_DYNAMIC_CLASS(wxIdleWakeUpModule);
};

HHOOK wxIdleWakeUpModule::ms_hMsgHookProc = 0;

wxIMPLEMENT_DYNAMIC_CLASS(wxIdleWakeUpModule, wxModule);
