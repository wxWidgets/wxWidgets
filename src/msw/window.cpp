/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/windows.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by: VZ on 13.05.99: no more Default(), MSWOnXXX() reorganisation
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "window.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <windows.h>
    #include "wx/msw/winundef.h"
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/setup.h"
    #include "wx/menu.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
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
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
    #include <ole2.h>
    #include <oleacc.h>
    #ifndef WM_GETOBJECT
        #define WM_GETOBJECT 0x003D
    #endif
    #ifndef OBJID_CLIENT
        #define OBJID_CLIENT 0xFFFFFFFC
    #endif
#endif

#include "wx/menuitem.h"
#include "wx/log.h"

#include "wx/msw/private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif // wxUSE_SPINCTRL

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/textctrl.h"
#include "wx/notebook.h"

#include <string.h>

#if (!defined(__GNUWIN32_OLD__) && !defined(__WXMICROWIN__)) || defined(__CYGWIN10__)
    #include <shellapi.h>
    #include <mmsystem.h>
#endif

#ifdef __WIN32__
    #include <windowsx.h>
#endif

#if (!defined(__GNUWIN32_OLD__) && !defined(__WXMICROWIN__)) || defined(__CYGWIN10__)
    #ifdef __WIN95__
        #include <commctrl.h>
    #endif
#elif !defined(__WXMICROWIN__) // broken compiler
    #include "wx/msw/gnuwin32/extra.h"
#endif

#if defined(__GNUG__)
#include "wx/msw/missing.h"
#endif

// ----------------------------------------------------------------------------
// standard constants not available with all compilers/headers
// ----------------------------------------------------------------------------

// This didn't appear in mingw until 2.95.2
#ifndef SIF_TRACKPOS
#define SIF_TRACKPOS 16
#endif

#if wxUSE_MOUSEWHEEL
    #ifndef WM_MOUSEWHEEL
        #define WM_MOUSEWHEEL           0x020A
    #endif
    #ifndef WHEEL_DELTA
        #define WHEEL_DELTA             120
    #endif
    #ifndef SPI_GETWHEELSCROLLLINES
        #define SPI_GETWHEELSCROLLLINES 104
    #endif
#endif // wxUSE_MOUSEWHEEL

#ifndef VK_OEM_1
    #define VK_OEM_1        0xBA
    #define VK_OEM_2        0xBF
    #define VK_OEM_3        0xC0
    #define VK_OEM_4        0xDB
    #define VK_OEM_5        0xDC
    #define VK_OEM_6        0xDD
    #define VK_OEM_7        0xDE
#endif

#ifndef VK_OEM_COMMA
    #define VK_OEM_PLUS     0xBB
    #define VK_OEM_COMMA    0xBC
    #define VK_OEM_MINUS    0xBD
    #define VK_OEM_PERIOD   0xBE
#endif

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// the last Windows message we got (FIXME-MT)
extern MSG s_currentMsg;

#if wxUSE_MENUS_NATIVE
wxMenu *wxCurrentPopupMenu = NULL;
#endif // wxUSE_MENUS_NATIVE

extern const wxChar *wxCanvasClassName;

// true if we had already created the std colour map, used by
// wxGetStdColourMap() and wxWindow::OnSysColourChanged()           (FIXME-MT)
static bool gs_hasStdCmap = FALSE;

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// the window proc for all our windows
#ifdef __DIGITALMARS__
extern "C" LRESULT WXDLLEXPORT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);
#else
LRESULT WXDLLEXPORT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);
#endif
                                   

#ifdef  __WXDEBUG__
    const char *wxGetMessageName(int message);
#endif  //__WXDEBUG__

void wxRemoveHandleAssociation(wxWindowMSW *win);
#ifdef __DIGITALMARS__
extern "C" void wxAssociateWinWithHandle(HWND hWnd, wxWindowMSW *win);
#else
extern void wxAssociateWinWithHandle(HWND hWnd, wxWindowMSW *win);
#endif
wxWindow *wxFindWinFromHandle(WXHWND hWnd);

// this magical function is used to translate VK_APPS key presses to right
// mouse clicks
static void TranslateKbdEventToMouse(wxWindowMSW *win,
                                     int *x, int *y, WPARAM *flags);

// get the text metrics for the current font
static TEXTMETRIC wxGetTextMetrics(const wxWindowMSW *win);

// find the window for the mouse event at the specified position
static wxWindowMSW *FindWindowForMouseEvent(wxWindowMSW *win, int *x, int *y); //TW:REQ:Univ

// wrapper around BringWindowToTop() API
static inline void wxBringWindowToTop(HWND hwnd)
{
#ifdef __WXMICROWIN__
    // It seems that MicroWindows brings the _parent_ of the window to the top,
    // which can be the wrong one.

    // activate (set focus to) specified window
    ::SetFocus(hwnd);
#endif

    // raise top level parent to top of z order
    if (!::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
    {
        wxLogLastError(_T("SetWindowPos"));
    }
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

// in wxUniv/MSW this class is abstract because it doesn't have DoPopupMenu()
// method
#ifdef __WXUNIVERSAL__
    IMPLEMENT_ABSTRACT_CLASS(wxWindowMSW, wxWindowBase)
#else // __WXMSW__
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif // __WXUNIVERSAL__/__WXMSW__

BEGIN_EVENT_TABLE(wxWindowMSW, wxWindowBase)
    EVT_ERASE_BACKGROUND(wxWindowMSW::OnEraseBackground)
    EVT_SYS_COLOUR_CHANGED(wxWindowMSW::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindowMSW::OnInitDialog)
    EVT_IDLE(wxWindowMSW::OnIdle)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxWindow utility functions
// ---------------------------------------------------------------------------

// Find an item given the MS Windows id
wxWindow *wxWindowMSW::FindItem(long id) const
{
#if wxUSE_CONTROLS
    wxControl *item = wxDynamicCastThis(wxControl);
    if ( item )
    {
        // is it we or one of our "internal" children?
        if ( item->GetId() == id
#ifndef __WXUNIVERSAL__
                || (item->GetSubcontrols().Index(id) != wxNOT_FOUND)
#endif // __WXUNIVERSAL__
           )
        {
            return item;
        }
    }
#endif // wxUSE_CONTROLS

    wxWindowList::Node *current = GetChildren().GetFirst();
    while (current)
    {
        wxWindow *childWin = current->GetData();

        wxWindow *wnd = childWin->FindItem(id);
        if ( wnd )
            return wnd;

        current = current->GetNext();
    }

    return NULL;
}

// Find an item given the MS Windows handle
wxWindow *wxWindowMSW::FindItemByHWND(WXHWND hWnd, bool controlOnly) const
{
    wxWindowList::Node *current = GetChildren().GetFirst();
    while (current)
    {
        wxWindow *parent = current->GetData();

        // Do a recursive search.
        wxWindow *wnd = parent->FindItemByHWND(hWnd);
        if ( wnd )
            return wnd;

        if ( !controlOnly
#if wxUSE_CONTROLS
                || parent->IsKindOf(CLASSINFO(wxControl))
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
    return FALSE;
}

// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindowMSW::Init()
{
    // generic
    InitBase();

    // MSW specific
    m_isBeingDeleted = FALSE;
    m_oldWndProc = NULL;
    m_useCtl3D = FALSE;
    m_mouseInWindow = FALSE;
    m_lastKeydownProcessed = FALSE;

    m_childrenDisabled = NULL;

    // wxWnd
    m_hMenu = 0;

    m_hWnd = 0;

    m_xThumbSize = 0;
    m_yThumbSize = 0;
    m_backgroundTransparent = FALSE;

    // as all windows are created with WS_VISIBLE style...
    m_isShown = TRUE;

#if wxUSE_MOUSEEVENT_HACK
    m_lastMouseX =
    m_lastMouseY = -1;
    m_lastMouseEvent = -1;
#endif // wxUSE_MOUSEEVENT_HACK
}

// Destructor
wxWindowMSW::~wxWindowMSW()
{
    m_isBeingDeleted = TRUE;

#ifndef __WXUNIVERSAL__
    // VS: make sure there's no wxFrame with last focus set to us:
    for ( wxWindow *win = GetParent(); win; win = win->GetParent() )
    {
        wxTopLevelWindow *frame = wxDynamicCast(win, wxTopLevelWindow);
        if ( frame )
        {
            if ( frame->GetLastFocus() == this )
            {
                frame->SetLastFocus(NULL);
            }
            break;
        }
    }
#endif // __WXUNIVERSAL__

    // VS: destroy children first and _then_ detach *this from its parent.
    //     If we'd do it the other way around, children wouldn't be able
    //     find their parent frame (see above).
    DestroyChildren();

    if ( m_parent )
        m_parent->RemoveChild(this);

    if ( m_hWnd )
    {
        // VZ: test temp removed to understand what really happens here
        //if (::IsWindow(GetHwnd()))
        {
            if ( !::DestroyWindow(GetHwnd()) )
                wxLogLastError(wxT("DestroyWindow"));
        }

        // remove hWnd <-> wxWindow association
        wxRemoveHandleAssociation(this);
    }

    delete m_childrenDisabled;
}

// real construction (Init() must have been called before!)
bool wxWindowMSW::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindow without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

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

    // all windows are created visible by default except popup ones (which are
    // like the wxTopLevelWindows in this aspect)
    if ( style & wxPOPUP_WINDOW )
    {
        msflags &= ~WS_VISIBLE;
        m_isShown = FALSE;
    }
    else
    {
        msflags |= WS_VISIBLE;
    }

    return MSWCreate(wxCanvasClassName, NULL, pos, size, msflags, exstyle);
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindowMSW::SetFocus()
{
    HWND hWnd = GetHwnd();
    wxCHECK_RET( hWnd, _T("can't set focus to invalid window") );

#ifndef __WXMICROWIN__
    ::SetLastError(0);
#endif

    if ( !::SetFocus(hWnd) )
    {
#if defined(__WXDEBUG__) && !defined(__WXMICROWIN__)
        // was there really an error?
        DWORD dwRes = ::GetLastError();
        if ( dwRes )
        {
            HWND hwndFocus = ::GetFocus();
            if ( hwndFocus != hWnd )
            {
                wxLogApiError(_T("SetFocus"), dwRes);
            }
        }
#endif // Debug
    }
}

void wxWindowMSW::SetFocusFromKbd()
{
    wxWindowBase::SetFocusFromKbd();

    // when the focus is given to the control with DLGC_HASSETSEL style from
    // keyboard its contents should be entirely selected: this is what
    // ::IsDialogMessage() does and so we should do it as well to provide the
    // same LNF as the native programs
    if ( ::SendMessage(GetHwnd(), WM_GETDLGCODE, 0, 0) & DLGC_HASSETSEL )
    {
        ::SendMessage(GetHwnd(), EM_SETSEL, 0, -1);
    }
}

// Get the window with the focus
wxWindow *wxWindowBase::FindFocus()
{
    HWND hWnd = ::GetFocus();
    if ( hWnd )
    {
        return wxGetWindowFromHWND((WXHWND)hWnd);
    }

    return NULL;
}

bool wxWindowMSW::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    HWND hWnd = GetHwnd();
    if ( hWnd )
        ::EnableWindow(hWnd, (BOOL)enable);

    // the logic below doesn't apply to the top level windows -- otherwise
    // showing a modal dialog would result in total greying out (and ungreying
    // out later) of everything which would be really ugly
    if ( IsTopLevel() )
        return TRUE;

    // when the parent is disabled, all of its children should be disabled as
    // well but when it is enabled back, only those of the children which
    // hadn't been already disabled in the beginning should be enabled again,
    // so we have to keep the list of those children
    for ( wxWindowList::Node *node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        if ( child->IsTopLevel() )
        {
            // the logic below doesn't apply to top level children
            continue;
        }

        if ( enable )
        {
            // enable the child back unless it had been disabled before us
            if ( !m_childrenDisabled || !m_childrenDisabled->Find(child) )
                child->Enable();
        }
        else // we're being disabled
        {
            if ( child->IsEnabled() )
            {
                // disable it as children shouldn't stay enabled while the
                // parent is not
                child->Disable();
            }
            else // child already disabled, remember it
            {
                // have we created the list of disabled children already?
                if ( !m_childrenDisabled )
                    m_childrenDisabled = new wxWindowList;

                m_childrenDisabled->Append(child);
            }
        }
    }

    if ( enable && m_childrenDisabled )
    {
        // we don't need this list any more, don't keep unused memory
        delete m_childrenDisabled;
        m_childrenDisabled = NULL;
    }

    return TRUE;
}

bool wxWindowMSW::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    HWND hWnd = GetHwnd();
    int cshow = show ? SW_SHOW : SW_HIDE;
    ::ShowWindow(hWnd, cshow);

    if ( show && IsTopLevel() )
    {
        wxBringWindowToTop(hWnd);
    }

    return TRUE;
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

void wxWindowMSW::SetTitle( const wxString& title)
{
    SetWindowText(GetHwnd(), title.c_str());
}

wxString wxWindowMSW::GetTitle() const
{
    return wxGetWindowText(GetHWND());
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
        wxLogLastError(_T("ReleaseCapture"));
    }
}

/* static */ wxWindow *wxWindowBase::GetCapture()
{
    HWND hwnd = ::GetCapture();
    return hwnd ? wxFindWinFromHandle((WXHWND)hwnd) : (wxWindow *)NULL;
}

bool wxWindowMSW::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    HWND hWnd = GetHwnd();
    if ( hWnd != 0 )
    {
        WXHANDLE hFont = m_font.GetResourceHandle();

        wxASSERT_MSG( hFont, wxT("should have valid font") );

        ::SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
    }

    return TRUE;
}
bool wxWindowMSW::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return FALSE;
    }

    if ( m_cursor.Ok() )
    {
        HWND hWnd = GetHwnd();

        // Change the cursor NOW if we're within the correct window
        POINT point;
        ::GetCursorPos(&point);

        RECT rect = wxGetWindowRect(hWnd);

        if ( ::PtInRect(&rect, point) && !wxIsBusy() )
            ::SetCursor(GetHcursorOf(m_cursor));
    }

    return TRUE;
}

void wxWindowMSW::WarpPointer (int x, int y)
{
    ClientToScreen(&x, &y);

    if ( !::SetCursorPos(x, y) )
    {
        wxLogLastError(_T("SetCursorPos"));
    }
}

#if WXWIN_COMPATIBILITY
void wxWindowMSW::MSWDeviceToLogical (float *x, float *y) const
{
}
#endif // WXWIN_COMPATIBILITY

// ---------------------------------------------------------------------------
// scrolling stuff
// ---------------------------------------------------------------------------

// convert wxHORIZONTAL/wxVERTICAL to SB_HORZ/SB_VERT
static inline int wxDirToWinStyle(int orient)
{
    return orient == wxHORIZONTAL ? SB_HORZ : SB_VERT;
}

#if WXWIN_COMPATIBILITY
void wxWindowMSW::SetScrollRange(int orient, int range, bool refresh)
{
    int range1 = range;

    // Try to adjust the range to cope with page size > 1
    // - a Windows API quirk
    int pageSize = GetScrollPage(orient);
    if ( pageSize > 1 && range > 0)
    {
        range1 += (pageSize - 1);
    }

    WinStruct<SCROLLINFO> info;
    info.nPage = pageSize; // Have to set this, or scrollbar goes awry
    info.nMin = 0;
    info.nMax = range1;
    info.fMask = SIF_RANGE | SIF_PAGE;

    HWND hWnd = GetHwnd();
    if ( hWnd )
        ::SetScrollInfo(hWnd, wxDirToWinStyle(orient), &info, refresh);
}

void wxWindowMSW::SetScrollPage(int orient, int page, bool refresh)
{
    WinStruct<SCROLLINFO> info;
    info.nPage = page;
    info.fMask = SIF_PAGE;

    HWND hWnd = GetHwnd();
    if ( hWnd )
        ::SetScrollInfo(hWnd, wxDirToWinStyle(orient), &info, refresh);
}

int wxWindowMSW::GetScrollPage(int orient) const
{
    return orient == wxHORIZONTAL ? m_xThumbSize : m_yThumbSize;
}

#endif // WXWIN_COMPATIBILITY

inline int GetScrollPosition(HWND hWnd, int wOrient)
{
#ifdef __WXMICROWIN__
    return ::GetScrollPosWX(hWnd, wOrient);
#else
    return ::GetScrollPos(hWnd, wOrient);
#endif
}

int wxWindowMSW::GetScrollPos(int orient) const
{
    HWND hWnd = GetHwnd();
    wxCHECK_MSG( hWnd, 0, _T("no HWND in GetScrollPos") );

    return GetScrollPosition(hWnd, orient == wxHORIZONTAL ? SB_HORZ : SB_VERT);
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindowMSW::GetScrollRange(int orient) const
{
    int minPos, maxPos;
    HWND hWnd = GetHwnd();
    if ( !hWnd )
        return 0;

    ::GetScrollRange(hWnd, orient == wxHORIZONTAL ? SB_HORZ : SB_VERT,
                     &minPos, &maxPos);

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
    wxCHECK_RET( hWnd, _T("SetScrollPos: no HWND") );

    WinStruct<SCROLLINFO> info;
    info.nPage = 0;
    info.nMin = 0;
    info.nPos = pos;
    info.fMask = SIF_POS;

    ::SetScrollInfo(hWnd, orient == wxHORIZONTAL ? SB_HORZ : SB_VERT,
                    &info, refresh);
}

// New function that will replace some of the above.
void wxWindowMSW::SetScrollbar(int orient,
                               int pos,
                               int pageSize,
                               int range,
                               bool refresh)
{
    WinStruct<SCROLLINFO> info;
    info.nPage = pageSize;
    info.nMin = 0;              // range is nMax - nMin + 1
    info.nMax = range - 1;      //  as both nMax and nMax are inclusive
    info.nPos = pos;
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    HWND hWnd = GetHwnd();
    if ( hWnd )
    {
        ::SetScrollInfo(hWnd, orient == wxHORIZONTAL ? SB_HORZ : SB_VERT,
                        &info, refresh);
    }

    *(orient == wxHORIZONTAL ? &m_xThumbSize : &m_yThumbSize) = pageSize;
}

void wxWindowMSW::ScrollWindow(int dx, int dy, const wxRect *prect)
{
    RECT rect;
    RECT *pr;
    if ( prect )
    {
        rect.left = prect->x;
        rect.top = prect->y;
        rect.right = prect->x + prect->width;
        rect.bottom = prect->y + prect->height;
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

// ---------------------------------------------------------------------------
// subclassing
// ---------------------------------------------------------------------------

void wxWindowMSW::SubclassWin(WXHWND hWnd)
{
    wxASSERT_MSG( !m_oldWndProc, wxT("subclassing window twice?") );

    HWND hwnd = (HWND)hWnd;
    wxCHECK_RET( ::IsWindow(hwnd), wxT("invalid HWND in SubclassWin") );

    wxAssociateWinWithHandle(hwnd, this);

    m_oldWndProc = (WXFARPROC)::GetWindowLong((HWND)hWnd, GWL_WNDPROC);

    // we don't need to subclass the window of our own class (in the Windows
    // sense of the word)
    if ( !wxCheckWindowWndProc(hWnd, (WXFARPROC)wxWndProc) )
    {
        ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG) wxWndProc);
    }
    else
    {
        // don't bother restoring it neither: this also makes it easy to
        // implement IsOfStandardClass() method which returns TRUE for the
        // standard controls and FALSE for the wxWindows own windows as it can
        // simply check m_oldWndProc
        m_oldWndProc = NULL;
    }
}

void wxWindowMSW::UnsubclassWin()
{
    wxRemoveHandleAssociation(this);

    // Restore old Window proc
    HWND hwnd = GetHwnd();
    if ( hwnd )
    {
        m_hWnd = 0;

        wxCHECK_RET( ::IsWindow(hwnd), wxT("invalid HWND in UnsubclassWin") );

        if ( m_oldWndProc )
        {
            if ( !wxCheckWindowWndProc((WXHWND)hwnd, m_oldWndProc) )
            {
                ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG) m_oldWndProc);
            }

            m_oldWndProc = NULL;
        }
    }
}

bool wxCheckWindowWndProc(WXHWND hWnd, WXFARPROC wndProc)
{
    // Unicows note: the code below works, but only because WNDCLASS contains
    // original window handler rather that the unicows fake one. This may not
    // be on purpose, though; if it stops working with future versions of
    // unicows.dll, we can override unicows hooks by setting
    // Unicows_{Set,Get}WindowLong and Unicows_RegisterClass to our own
    // versions that keep track of fake<->real wnd proc mapping.
    WNDCLASS cls;
    if ( !::GetClassInfo(wxGetInstance(), wxGetWindowClass(hWnd), &cls) )
    {
        wxLogLastError(_T("GetClassInfo"));

        return FALSE;
    }

    return wndProc == (WXFARPROC)cls.lpfnWndProc;
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

    // now update the Windows style as well if needed - and if the window had
    // been already created
    if ( !GetHwnd() )
        return;

    WXDWORD exstyle, exstyleOld;
    long style = MSWGetStyle(flags, &exstyle),
         styleOld = MSWGetStyle(flagsOld, &exstyleOld);

    if ( style != styleOld )
    {
        // some flags (e.g. WS_VISIBLE or WS_DISABLED) should not be changed by
        // this function so instead of simply setting the style to the new
        // value we clear the bits which were set in styleOld but are set in
        // the new one and set the ones which were not set before
        long styleReal = ::GetWindowLong(GetHwnd(), GWL_STYLE);
        styleReal &= ~styleOld;
        styleReal |= style;

        ::SetWindowLong(GetHwnd(), GWL_STYLE, styleReal);
    }

    // and the extended style
    if ( exstyle != exstyleOld )
    {
        long exstyleReal = ::GetWindowLong(GetHwnd(), GWL_EXSTYLE);
        exstyleReal &= ~exstyleOld;
        exstyleReal |= exstyle;

        ::SetWindowLong(GetHwnd(), GWL_EXSTYLE, exstyleReal);

        // we must call SetWindowPos() to flash the cached extended style and
        // also to make the change to wxSTAY_ON_TOP style take effect: just
        // setting the style simply doesn't work
        if ( !::SetWindowPos(GetHwnd(),
                             exstyleReal & WS_EX_TOPMOST ? HWND_TOPMOST
                                                         : HWND_NOTOPMOST,
                             0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE) )
        {
            wxLogLastError(_T("SetWindowPos"));
        }
    }
}

WXDWORD wxWindowMSW::MSWGetStyle(long flags, WXDWORD *exstyle) const
{
    // translate the style
    WXDWORD style = WS_CHILD | WS_VISIBLE;

    if ( flags & wxCLIP_CHILDREN )
        style |= WS_CLIPCHILDREN;

    if ( flags & wxCLIP_SIBLINGS )
        style |= WS_CLIPSIBLINGS;

    if ( flags & wxVSCROLL )
        style |= WS_VSCROLL;

    if ( flags & wxHSCROLL )
        style |= WS_HSCROLL;

    wxBorder border = (wxBorder)(flags & wxBORDER_MASK);
    
    // Check if we want to automatically give it a sunken style.
    // Note than because 'sunken' actually maps to WS_EX_CLIENTEDGE, which
    // is a more neutral term, we don't necessarily get a sunken effect in
    // Windows XP. Instead we get the appropriate style for the theme.

    if (border == wxBORDER_DEFAULT && wxTheApp->GetAuto3D() && 
        IsKindOf(CLASSINFO(wxControl)) &&
        GetParent() && (GetParent()->IsKindOf(CLASSINFO(wxPanel)) ||
                        GetParent()->IsKindOf(CLASSINFO(wxDialog))) &&
        ((GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS) != wxUSER_COLOURS))
    {
        border = (wxBorder)((flags & wxBORDER_MASK) | wxBORDER_SUNKEN);
    }   
    
    // Only give it WS_BORDER for wxBORDER_SIMPLE
    if (border & wxBORDER_SIMPLE)
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
                wxFAIL_MSG( _T("unknown border style") );
                // fall through

            case wxBORDER_NONE:
            case wxBORDER_SIMPLE:
            case wxBORDER_DEFAULT:
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

            case wxBORDER_DOUBLE:
                *exstyle |= WS_EX_DLGMODALFRAME;
                break;
        }

        // wxUniv doesn't use Windows dialog navigation functions at all
#ifndef __WXUNIVERSAL__
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

#if WXWIN_COMPATIBILITY
// If nothing defined for this, try the parent.
// E.g. we may be a button loaded from a resource, with no callback function
// defined.
void wxWindowMSW::OnCommand(wxWindow& win, wxCommandEvent& event)
{
    if ( GetEventHandler()->ProcessEvent(event)  )
        return;
    if ( m_parent )
        m_parent->GetEventHandler()->OnCommand(win, event);
}
#endif // WXWIN_COMPATIBILITY_2

#if WXWIN_COMPATIBILITY
wxObject* wxWindowMSW::GetChild(int number) const
{
    // Return a pointer to the Nth object in the Panel
    wxNode *node = GetChildren().First();
    int n = number;
    while (node && n--)
        node = node->Next();
    if ( node )
    {
        wxObject *obj = (wxObject *)node->Data();
        return(obj);
    }
    else
        return NULL;
}
#endif // WXWIN_COMPATIBILITY

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
    ::GetCursorPos(&pt);

    // find the window which currently has the cursor and go up the window
    // chain until we find this window - or exhaust it
    HWND hwnd = ::WindowFromPoint(pt);
    while ( hwnd && (hwnd != GetHwnd()) )
        hwnd = ::GetParent(hwnd);

    return hwnd != NULL;
}

void wxWindowMSW::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    // Check if we need to send a LEAVE event
    if ( m_mouseInWindow )
    {
        // note that we should generate the leave event whether the window has
        // or doesn't have mouse capture
        if ( !IsMouseInWindow() )
        {
            // Generate a LEAVE event
            m_mouseInWindow = FALSE;

            // Unfortunately the mouse button and keyboard state may have
            // changed by the time the OnIdle function is called, so 'state'
            // may be meaningless.
            int state = 0;
            if ( wxIsShiftDown() )
                state |= MK_SHIFT;
            if ( wxIsCtrlDown() )
                state |= MK_CONTROL;
            if ( GetKeyState( VK_LBUTTON ) )
                state |= MK_LBUTTON;
            if ( GetKeyState( VK_MBUTTON ) )
                state |= MK_MBUTTON;
            if ( GetKeyState( VK_RBUTTON ) )
                state |= MK_RBUTTON;

            POINT pt;
            if ( !::GetCursorPos(&pt) )
            {
                wxLogLastError(_T("GetCursorPos"));
            }

            // we need to have client coordinates here for symmetry with
            // wxEVT_ENTER_WINDOW
            RECT rect = wxGetWindowRect(GetHwnd());
            pt.x -= rect.left;
            pt.y -= rect.top;

            wxMouseEvent event2(wxEVT_LEAVE_WINDOW);
            InitMouseEvent(event2, pt.x, pt.y, state);

            (void)GetEventHandler()->ProcessEvent(event2);
        }
    }

    UpdateWindowUI();
}

// Set this window to be the child of 'parent'.
bool wxWindowMSW::Reparent(wxWindowBase *parent)
{
    if ( !wxWindowBase::Reparent(parent) )
        return FALSE;

    HWND hWndChild = GetHwnd();
    HWND hWndParent = GetParent() ? GetWinHwnd(GetParent()) : (HWND)0;

    ::SetParent(hWndChild, hWndParent);

    return TRUE;
}

void wxWindowMSW::Clear()
{
    wxClientDC dc((wxWindow *)this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

static inline void SendSetRedraw(HWND hwnd, bool on)
{
#ifndef __WXMICROWIN__
    ::SendMessage(hwnd, WM_SETREDRAW, (WPARAM)on, 0);
#endif
}

void wxWindowMSW::Freeze()
{
    SendSetRedraw(GetHwnd(), FALSE);
}

void wxWindowMSW::Thaw()
{
    SendSetRedraw(GetHwnd(), TRUE);

    // we need to refresh everything or otherwise he invalidated area is not
    // repainted
    Refresh();
}

void wxWindowMSW::Refresh(bool eraseBack, const wxRect *rect)
{
    HWND hWnd = GetHwnd();
    if ( hWnd )
    {
        if ( rect )
        {
            RECT mswRect;
            mswRect.left = rect->x;
            mswRect.top = rect->y;
            mswRect.right = rect->x + rect->width;
            mswRect.bottom = rect->y + rect->height;

            ::InvalidateRect(hWnd, &mswRect, eraseBack);
        }
        else
            ::InvalidateRect(hWnd, NULL, eraseBack);
    }
}

void wxWindowMSW::Update()
{
    if ( !::UpdateWindow(GetHwnd()) )
    {
        wxLogLastError(_T("UpdateWindow"));
    }

#if defined(__WIN32__) && !defined(__WXMICROWIN__)
    // just calling UpdateWindow() is not enough, what we did in our WM_PAINT
    // handler needs to be really drawn right now
    (void)::GdiFlush();
#endif // __WIN32__
}

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------


#if wxUSE_DRAG_AND_DROP
void wxWindowMSW::SetDropTarget(wxDropTarget *pDropTarget)
{
    if ( m_dropTarget != 0 ) {
        m_dropTarget->Revoke(m_hWnd);
        delete m_dropTarget;
    }

    m_dropTarget = pDropTarget;
    if ( m_dropTarget != 0 )
        m_dropTarget->Register(m_hWnd);
}
#endif // wxUSE_DRAG_AND_DROP

// old style file-manager drag&drop support: we retain the old-style
// DragAcceptFiles in parallel with SetDropTarget.
void wxWindowMSW::DragAcceptFiles(bool accept)
{
    HWND hWnd = GetHwnd();
    if ( hWnd )
        ::DragAcceptFiles(hWnd, (BOOL)accept);
}

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

// Get total size
void wxWindowMSW::DoGetSize(int *x, int *y) const
{
    RECT rect = wxGetWindowRect(GetHwnd());

    if ( x )
        *x = rect.right - rect.left;
    if ( y )
        *y = rect.bottom - rect.top;
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowMSW::DoGetClientSize(int *x, int *y) const
{
    RECT rect = wxGetClientRect(GetHwnd());

    if ( x )
        *x = rect.right;
    if ( y )
        *y = rect.bottom;
}

void wxWindowMSW::DoGetPosition(int *x, int *y) const
{
    RECT rect = wxGetWindowRect(GetHwnd());

    POINT point;
    point.x = rect.left;
    point.y = rect.top;

    // we do the adjustments with respect to the parent only for the "real"
    // children, not for the dialogs/frames
    if ( !IsTopLevel() )
    {
        HWND hParentWnd = 0;
        wxWindow *parent = GetParent();
        if ( parent )
            hParentWnd = GetWinHwnd(parent);

        // Since we now have the absolute screen coords, if there's a parent we
        // must subtract its top left corner
        if ( hParentWnd )
        {
            ::ScreenToClient(hParentWnd, &point);
        }

        if ( parent )
        {
            // We may be faking the client origin. So a window that's really at (0,
            // 30) may appear (to wxWin apps) to be at (0, 0).
            wxPoint pt(parent->GetClientAreaOrigin());
            point.x -= pt.x;
            point.y -= pt.y;
        }
    }

    if ( x )
        *x = point.x;
    if ( y )
        *y = point.y;
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

void wxWindowMSW::DoMoveWindow(int x, int y, int width, int height)
{
    // TODO: is this consistent with other platforms?
    // Still, negative width or height shouldn't be allowed
    if (width < 0)
        width = 0;
    if (height < 0)
        height = 0;
    if ( !::MoveWindow(GetHwnd(), x, y, width, height, TRUE) )
    {
        wxLogLastError(wxT("MoveWindow"));
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
    GetPosition(&currentX, &currentY);
    int currentW,currentH;
    GetSize(&currentW, &currentH);

    // ... and don't do anything (avoiding flicker) if it's already ok
    if ( x == currentX && y == currentY &&
         width == currentW && height == currentH )
    {
        return;
    }

    if ( x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        x = currentX;
    if ( y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        y = currentY;

    AdjustForParentClientOrigin(x, y, sizeFlags);

    wxSize size(-1, -1);
    if ( width == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
        {
            size = DoGetBestSize();
            width = size.x;
        }
        else
        {
            // just take the current one
            width = currentW;
        }
    }

    if ( height == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
        {
            if ( size.x == -1 )
            {
                size = DoGetBestSize();
            }
            //else: already called DoGetBestSize() above

            height = size.y;
        }
        else
        {
            // just take the current one
            height = currentH;
        }
    }

    DoMoveWindow(x, y, width, height);
}

void wxWindowMSW::DoSetClientSize(int width, int height)
{
    // setting the client size is less obvious than it it could have been
    // because in the result of changing the total size the window scrollbar
    // may [dis]appear and/or its menubar may [un]wrap and so the client size
    // will not be correct as the difference between the total and client size
    // changes - so we keep changing it until we get it right
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

        // if the size is already ok, stop here (rectClient.left = top = 0)
        if ( (rectClient.right == width || width == -1) &&
             (rectClient.bottom == height || height == -1) )
        {
            break;
        }

        int widthClient = width,
            heightClient = height;

        // Find the difference between the entire window (title bar and all)
        // and the client area; add this to the new client size to move the
        // window
        RECT rectWin;
        ::GetWindowRect(GetHwnd(), &rectWin);

        widthClient += rectWin.right - rectWin.left - rectClient.right;
        heightClient += rectWin.bottom - rectWin.top - rectClient.bottom;

        POINT point;
        point.x = rectWin.left;
        point.y = rectWin.top;

        // MoveWindow positions the child windows relative to the parent, so
        // adjust if necessary
        if ( !IsTopLevel() )
        {
            wxWindow *parent = GetParent();
            if ( parent )
            {
                ::ScreenToClient(GetHwndOf(parent), &point);
            }
        }

        DoMoveWindow(point.x, point.y, widthClient, heightClient);
    }
}

// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxWindowMSW::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
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

void wxWindowMSW::GetTextExtent(const wxString& string,
                             int *x, int *y,
                             int *descent, int *externalLeading,
                             const wxFont *theFont) const
{
    const wxFont *fontToUse = theFont;
    if ( !fontToUse )
        fontToUse = &m_font;

    HWND hWnd = GetHwnd();
    HDC dc = ::GetDC(hWnd);

    HFONT fnt = 0;
    HFONT hfontOld = 0;
    if ( fontToUse && fontToUse->Ok() )
    {
        fnt = (HFONT)((wxFont *)fontToUse)->GetResourceHandle(); // const_cast
        if ( fnt )
            hfontOld = (HFONT)SelectObject(dc,fnt);
    }

    SIZE sizeRect;
    TEXTMETRIC tm;
    GetTextExtentPoint(dc, string, (int)string.Length(), &sizeRect);
    GetTextMetrics(dc, &tm);

    if ( fontToUse && fnt && hfontOld )
        SelectObject(dc, hfontOld);

    ReleaseDC(hWnd, dc);

    if ( x )
        *x = sizeRect.cx;
    if ( y )
        *y = sizeRect.cy;
    if ( descent )
        *descent = tm.tmDescent;
    if ( externalLeading )
        *externalLeading = tm.tmExternalLeading;
}

#if wxUSE_CARET && WXWIN_COMPATIBILITY
// ---------------------------------------------------------------------------
// Caret manipulation
// ---------------------------------------------------------------------------

void wxWindowMSW::CreateCaret(int w, int h)
{
    SetCaret(new wxCaret(this, w, h));
}

void wxWindowMSW::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
    wxFAIL_MSG("not implemented");
}

void wxWindowMSW::ShowCaret(bool show)
{
    wxCHECK_RET( m_caret, "no caret to show" );

    m_caret->Show(show);
}

void wxWindowMSW::DestroyCaret()
{
    SetCaret(NULL);
}

void wxWindowMSW::SetCaretPos(int x, int y)
{
    wxCHECK_RET( m_caret, "no caret to move" );

    m_caret->Move(x, y);
}

void wxWindowMSW::GetCaretPos(int *x, int *y) const
{
    wxCHECK_RET( m_caret, "no caret to get position of" );

    m_caret->GetPosition(x, y);
}
#endif // wxUSE_CARET

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
    while ( ::PeekMessage(&msg, (HWND)0, WM_COMMAND, WM_COMMAND, PM_REMOVE)
                && msg.message != WM_QUIT )
    {
        wxTheApp->DoMessage((WXMSG *)&msg);
    }

    // If we retrieved a WM_QUIT, insert back into the message queue.
    if (msg.message == WM_QUIT)
        ::PostQuitMessage(0);
}

bool wxWindowMSW::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->SetInvokingWindow(this);
    menu->UpdateUI();

    HWND hWnd = GetHwnd();
    HMENU hMenu = GetHmenuOf(menu);
    POINT point;
    point.x = x;
    point.y = y;
    ::ClientToScreen(hWnd, &point);
    wxCurrentPopupMenu = menu;
    ::TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hWnd, NULL);

    // we need to do it righ now as otherwise the events are never going to be
    // sent to wxCurrentPopupMenu from HandleCommand()
    //
    // note that even eliminating (ugly) wxCurrentPopupMenu global wouldn't
    // help and we'd still need wxYieldForCommandsOnly() as the menu may be
    // destroyed as soon as we return (it can be a local variable in the caller
    // for example) and so we do need to process the event immediately
    wxYieldForCommandsOnly();

    wxCurrentPopupMenu = NULL;

    menu->SetInvokingWindow(NULL);

    return TRUE;
}

#endif // wxUSE_MENUS_NATIVE

// ===========================================================================
// pre/post message processing
// ===========================================================================

long wxWindowMSW::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( m_oldWndProc )
#ifdef __DIGITALMARS__
        return ::CallWindowProc( (FARPROC) m_oldWndProc, GetHwnd(), (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
#else
        return ::CallWindowProc(CASTWNDPROC m_oldWndProc, GetHwnd(), (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
#endif
    else
        return ::DefWindowProc(GetHwnd(), nMsg, wParam, lParam);
}

bool wxWindowMSW::MSWProcessMessage(WXMSG* pMsg)
{
    // wxUniversal implements tab traversal itself
#ifndef __WXUNIVERSAL__
    if ( m_hWnd != 0 && (GetWindowStyleFlag() & wxTAB_TRAVERSAL) )
    {
        // intercept dialog navigation keys
        MSG *msg = (MSG *)pMsg;

        // here we try to do all the job which ::IsDialogMessage() usually does
        // internally
#if 1
        if ( msg->message == WM_KEYDOWN )
        {
            bool bCtrlDown = wxIsCtrlDown();
            bool bShiftDown = wxIsShiftDown();

            // WM_GETDLGCODE: ask the control if it wants the key for itself,
            // don't process it if it's the case (except for Ctrl-Tab/Enter
            // combinations which are always processed)
            LONG lDlgCode = 0;
            if ( !bCtrlDown )
            {
                lDlgCode = ::SendMessage(msg->hwnd, WM_GETDLGCODE, 0, 0);

                // surprizingly, DLGC_WANTALLKEYS bit mask doesn't contain the
                // DLGC_WANTTAB nor DLGC_WANTARROWS bits although, logically,
                // it, of course, implies them
                if ( lDlgCode & DLGC_WANTALLKEYS )
                {
                    lDlgCode |= DLGC_WANTTAB | DLGC_WANTARROWS;
                }
            }

            bool bForward = TRUE,
                 bWindowChange = FALSE;

            // should we process this message specially?
            bool bProcess = TRUE;
            switch ( msg->wParam )
            {
                case VK_TAB:
                    // assume that nobody wants Shift-TAB for himself - if we
                    // don't do it there is no easy way for a control to grab
                    // TABs but still let Shift-TAB work as navugation key
                    if ( (lDlgCode & DLGC_WANTTAB) && !bShiftDown ) {
                        bProcess = FALSE;
                    }
                    else {
                        // Ctrl-Tab cycles thru notebook pages
                        bWindowChange = bCtrlDown;
                        bForward = !bShiftDown;
                    }
                    break;

                case VK_UP:
                case VK_LEFT:
                    if ( (lDlgCode & DLGC_WANTARROWS) || bCtrlDown )
                        bProcess = FALSE;
                    else
                        bForward = FALSE;
                    break;

                case VK_DOWN:
                case VK_RIGHT:
                    if ( (lDlgCode & DLGC_WANTARROWS) || bCtrlDown )
                        bProcess = FALSE;
                    break;

                case VK_RETURN:
                    {
                        if ( (lDlgCode & DLGC_WANTMESSAGE) && !bCtrlDown )
                        {
                            // control wants to process Enter itself, don't
                            // call IsDialogMessage() which would interpret
                            // it
                            return FALSE;
                        }
                        else if ( lDlgCode & DLGC_BUTTON )
                        {
                            // let IsDialogMessage() handle this for all
                            // buttons except the owner-drawn ones which it
                            // just seems to ignore
                            long style = ::GetWindowLong(msg->hwnd, GWL_STYLE);
                            if ( (style & BS_OWNERDRAW) == BS_OWNERDRAW )
                            {
                                // emulate the button click
                                wxWindow *btn = wxFindWinFromHandle((WXHWND)msg->hwnd);
                                if ( btn )
                                    btn->MSWCommand(BN_CLICKED, 0 /* unused */);
                            }

                            bProcess = FALSE;
                        }
                        // FIXME: this should be handled by
                        //        wxNavigationKeyEvent handler and not here!!
                        else
                        {
#if wxUSE_BUTTON
                            wxButton *btn = wxDynamicCast(GetDefaultItem(),
                                                          wxButton);
                            if ( btn && btn->IsEnabled() )
                            {
                                // if we do have a default button, do press it
                                btn->MSWCommand(BN_CLICKED, 0 /* unused */);

                                return TRUE;
                            }
                            else // no default button
                            {
#endif // wxUSE_BUTTON
                                // this is a quick and dirty test for a text
                                // control
                                if ( !(lDlgCode & DLGC_HASSETSEL) )
                                {
                                    // don't process Enter, the control might
                                    // need it for itself and don't let
                                    // ::IsDialogMessage() have it as it can
                                    // eat the Enter events sometimes
                                    return FALSE;
                                }
                                else if (!IsTopLevel())
                                {
                                    // if not a top level window, let parent
                                    // handle it
                                    return FALSE;
                                }
                                //else: treat Enter as TAB: pass to the next
                                //      control as this is the best thing to do
                                //      if the text doesn't handle Enter itself
                            }
                        }
                    }
                    break;

                default:
                    bProcess = FALSE;
            }

            if ( bProcess )
            {
                wxNavigationKeyEvent event;
                event.SetDirection(bForward);
                event.SetWindowChange(bWindowChange);
                event.SetEventObject(this);

                if ( GetEventHandler()->ProcessEvent(event) )
                {
                    return TRUE;
                }
            }
        }
#else // 0
        // let ::IsDialogMessage() do almost everything and handle just the
        // things it doesn't here: Ctrl-TAB for switching notebook pages
        if ( msg->message == WM_KEYDOWN )
        {
            // don't process system keys here
            if ( !(HIWORD(msg->lParam) & KF_ALTDOWN) )
            {
                if ( (msg->wParam == VK_TAB) && wxIsCtrlDown() )
                {
                    // find the first notebook parent and change its page
                    wxWindow *win = this;
                    wxNotebook *nbook = NULL;
                    while ( win && !nbook )
                    {
                        nbook = wxDynamicCast(win, wxNotebook);
                        win = win->GetParent();
                    }

                    if ( nbook )
                    {
                        bool forward = !wxIsShiftDown();

                        nbook->AdvanceSelection(forward);
                    }
                }
            }
        }
#endif // 1/0

        // we handle VK_ESCAPE ourselves in wxDialog::OnCharHook() and we
        // shouldn't let IsDialogMessage() get it as it _always_ eats the
        // message even when there is no cancel button and when the message is
        // needed by the control itself: in particular, it prevents the tree in
        // place edit control from being closed with Escape in a dialog
        if ( msg->message != WM_KEYDOWN || msg->wParam != VK_ESCAPE )
        {
            // ::IsDialogMessage() can enter in an infinite loop when the
            // currently focused window is disabled or hidden and its parent
            // has WS_EX_CONTROLPARENT style, so don't call it in this case
            bool canSafelyCallIsDlgMsg = TRUE;

            HWND hwndFocus = ::GetFocus();
            while ( hwndFocus )
            {
                if ( !::IsWindowEnabled(hwndFocus) ||
                        !::IsWindowVisible(hwndFocus) )
                {
                    // it would enter an infinite loop if we do this!
                    canSafelyCallIsDlgMsg = FALSE;

                    break;
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

            if ( canSafelyCallIsDlgMsg && ::IsDialogMessage(GetHwnd(), msg) )
            {
                // IsDialogMessage() did something...
                return TRUE;
            }
        }
    }
#endif // __WXUNIVERSAL__

#if wxUSE_TOOLTIPS
    if ( m_tooltip )
    {
        // relay mouse move events to the tooltip control
        MSG *msg = (MSG *)pMsg;
        if ( msg->message == WM_MOUSEMOVE )
            m_tooltip->RelayEvent(pMsg);
    }
#endif // wxUSE_TOOLTIPS

    return FALSE;
}

bool wxWindowMSW::MSWTranslateMessage(WXMSG* pMsg)
{
#if wxUSE_ACCEL && !defined(__WXUNIVERSAL__)
    return m_acceleratorTable.Translate(this, pMsg);
#else
    (void) pMsg;
    return FALSE;
#endif // wxUSE_ACCEL
}

bool wxWindowMSW::MSWShouldPreProcessMessage(WXMSG* WXUNUSED(pMsg))
{
    // preprocess all messages by default
    return TRUE;
}

// ---------------------------------------------------------------------------
// message params unpackers (different for Win16 and Win32)
// ---------------------------------------------------------------------------

#ifdef __WIN32__

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
                              WXWORD *nCtlColor, WXHDC *hdc, WXHWND *hwnd)
{
#ifndef __WXMICROWIN__
    *nCtlColor = CTLCOLOR_BTN;
    *hwnd = (WXHWND)lParam;
    *hdc = (WXHDC)wParam;
#endif
}

void wxWindowMSW::UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                                WXWORD *item, WXWORD *flags, WXHMENU *hmenu)
{
    *item = (WXWORD)wParam;
    *flags = HIWORD(wParam);
    *hmenu = (WXHMENU)lParam;
}

#else // Win16

void wxWindowMSW::UnpackCommand(WXWPARAM wParam, WXLPARAM lParam,
                             WXWORD *id, WXHWND *hwnd, WXWORD *cmd)
{
    *id = (WXWORD)wParam;
    *hwnd = (WXHWND)LOWORD(lParam);
    *cmd = HIWORD(lParam);
}

void wxWindowMSW::UnpackActivate(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *state, WXWORD *minimized, WXHWND *hwnd)
{
    *state = (WXWORD)wParam;
    *minimized = LOWORD(lParam);
    *hwnd = (WXHWND)HIWORD(lParam);
}

void wxWindowMSW::UnpackScroll(WXWPARAM wParam, WXLPARAM lParam,
                            WXWORD *code, WXWORD *pos, WXHWND *hwnd)
{
    *code = (WXWORD)wParam;
    *pos = LOWORD(lParam);
    *hwnd = (WXHWND)HIWORD(lParam);
}

void wxWindowMSW::UnpackCtlColor(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *nCtlColor, WXHDC *hdc, WXHWND *hwnd)
{
    *hwnd = (WXHWND)LOWORD(lParam);
    *nCtlColor = (int)HIWORD(lParam);
    *hdc = (WXHDC)wParam;
}

void wxWindowMSW::UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                                WXWORD *item, WXWORD *flags, WXHMENU *hmenu)
{
    *item = (WXWORD)wParam;
    *flags = LOWORD(lParam);
    *hmenu = (WXHMENU)HIWORD(lParam);
}

#endif // Win32/16

// ---------------------------------------------------------------------------
// Main wxWindows window proc and the window proc for wxWindow
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
LRESULT WXDLLEXPORT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // trace all messages - useful for the debugging
#ifdef __WXDEBUG__
#if wxUSE_LOG
    wxLogTrace(wxTraceMessages, wxT("Processing %s(wParam=%8lx, lParam=%8lx)"),
               wxGetMessageName(message), (long) wParam, lParam);
#endif // wxUSE_LOG
#endif // __WXDEBUG__

    wxWindowMSW *wnd = wxFindWinFromHandle((WXHWND) hWnd);

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

    if ( wnd )
        rc = wnd->MSWWindowProc(message, wParam, lParam);
    else
        rc = ::DefWindowProc(hWnd, message, wParam, lParam);

    return rc;
}

long wxWindowMSW::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    // did we process the message?
    bool processed = FALSE;

    // the return value
    union
    {
        bool        allow;
        long        result;
        WXHICON     hIcon;
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
            // never set processed to TRUE and *always* pass WM_DESTROY to
            // DefWindowProc() as Windows may do some internal cleanup when
            // processing it and failing to pass the message along may cause
            // memory and resource leaks!
            (void)HandleDestroy();
            break;

        case WM_MOVE:
            processed = HandleMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;

        case WM_MOVING:
			{
				LPRECT pRect = (LPRECT)lParam;
				wxRect rc;
				rc.SetLeft(pRect->left);
				rc.SetTop(pRect->top);
				rc.SetRight(pRect->right);
				rc.SetBottom(pRect->bottom);
				processed = HandleMoving(rc);
				if (processed) {
					pRect->left = rc.GetLeft();
					pRect->top = rc.GetTop();
					pRect->right = rc.GetRight();
					pRect->bottom = rc.GetBottom();
				}
			}
            break;

        case WM_SIZE:
            switch ( wParam )
            {
                case SIZE_MAXHIDE:
                case SIZE_MAXSHOW:
                    // we're not interested in these messages at all
                    break;

                case SIZE_MINIMIZED:
                    // we shouldn't send sizev events for these messages as the
                    // client size may be negative which breaks existing code
                    //
                    // OTOH we might send another (wxMinimizedEvent?) one or
                    // add an additional parameter to wxSizeEvent if this is
                    // useful to anybody
                    break;

                default:
                    wxFAIL_MSG( _T("unexpected WM_SIZE parameter") );
                    // fall through nevertheless

                case SIZE_MAXIMIZED:
                case SIZE_RESTORED:
                    processed = HandleSize(LOWORD(lParam), HIWORD(lParam),
                                           wParam);
            }
            break;

        case WM_SIZING:
            {
                LPRECT pRect = (LPRECT)lParam;
                wxRect rc;
                rc.SetLeft(pRect->left);
                rc.SetTop(pRect->top);
                rc.SetRight(pRect->right);
                rc.SetBottom(pRect->bottom);
                processed = HandleSizing(rc);
                if (processed) {
                    pRect->left = rc.GetLeft();
                    pRect->top = rc.GetTop();
                    pRect->right = rc.GetRight();
                    pRect->bottom = rc.GetBottom();
                }
            }
            break;

#ifndef __WXMICROWIN__
        case WM_ACTIVATEAPP:
            wxTheApp->SetActive(wParam != 0, FindFocus());
            break;
#endif

        case WM_ACTIVATE:
            {
                WXWORD state, minimized;
                WXHWND hwnd;
                UnpackActivate(wParam, lParam, &state, &minimized, &hwnd);

                processed = HandleActivate(state, minimized != 0, (WXHWND)hwnd);
            }
            break;

        case WM_SETFOCUS:
            processed = HandleSetFocus((WXHWND)(HWND)wParam);
            break;

        case WM_KILLFOCUS:
            processed = HandleKillFocus((WXHWND)(HWND)wParam);
            break;

        case WM_PAINT:
            processed = HandlePaint();
            break;

        case WM_CLOSE:
#ifdef __WXUNIVERSAL__
            // Universal uses its own wxFrame/wxDialog, so we don't receive
            // close events unless we have this.
            Close();
            processed = TRUE;
            rc.result = TRUE;
#else
            // don't let the DefWindowProc() destroy our window - we'll do it
            // ourselves in ~wxWindow
            processed = TRUE;
            rc.result = TRUE;
#endif
            break;

        case WM_SHOWWINDOW:
            processed = HandleShow(wParam != 0, (int)lParam);
            break;

        case WM_MOUSEMOVE:
            processed = HandleMouseMove(GET_X_LPARAM(lParam),
                                        GET_Y_LPARAM(lParam),
                                        wParam);
            break;

#if wxUSE_MOUSEWHEEL
        case WM_MOUSEWHEEL:
            processed = HandleMouseWheel(wParam, lParam);
            break;
#endif

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
            {
#ifdef __WXMICROWIN__
                // MicroWindows seems to ignore the fact that a window is
                // disabled. So catch mouse events and throw them away if
                // necessary.
                wxWindowMSW* win = this;
                for ( ;; )
                {
                    if (!win->IsEnabled())
                    {
                        processed = TRUE;
                        break;
                    }

                    win = win->GetParent();
                    if ( !win || win->IsTopLevel() )
                        break;
                }

                if ( processed )
                    break;

#endif // __WXMICROWIN__
                int x = GET_X_LPARAM(lParam),
                    y = GET_Y_LPARAM(lParam);

                // redirect the event to a static control if necessary by
                // finding one under mouse
                wxWindowMSW *win;
                if ( GetCapture() == this )
                {
                    // but don't do it if the mouse is captured by this window
                    // because then it should really get this event itself
                    win = this;
                }
                else
                {
                    win = FindWindowForMouseEvent(this, &x, &y);

                    // this should never happen
                    wxCHECK_MSG( win, 0,
                                 _T("FindWindowForMouseEvent() returned NULL") );

                    // for the standard classes their WndProc sets the focus to
                    // them anyhow and doing it from here results in some weird
                    // problems, but for our windows we want them to acquire
                    // focus when clicked
                    if ( !win->IsOfStandardClass() )
                    {
                        if ( message == WM_LBUTTONDOWN && win->AcceptsFocus() )
                            win->SetFocus();
                    }
                }

                processed = win->HandleMouseEvent(message, x, y, wParam);
            }
            break;

#ifdef MM_JOY1MOVE
        case MM_JOY1MOVE:
        case MM_JOY2MOVE:
        case MM_JOY1ZMOVE:
        case MM_JOY2ZMOVE:
        case MM_JOY1BUTTONDOWN:
        case MM_JOY2BUTTONDOWN:
        case MM_JOY1BUTTONUP:
        case MM_JOY2BUTTONUP:
            processed = HandleJoystickEvent(message,
                                            GET_X_LPARAM(lParam),
                                            GET_Y_LPARAM(lParam),
                                            wParam);
            break;
#endif // __WXMICROWIN__

        case WM_SYSCOMMAND:
            processed = HandleSysCommand(wParam, lParam);
            break;

        case WM_COMMAND:
            {
                WORD id, cmd;
                WXHWND hwnd;
                UnpackCommand(wParam, lParam, &id, &hwnd, &cmd);

                processed = HandleCommand(id, cmd, hwnd);
            }
            break;

#ifdef __WIN95__
        case WM_NOTIFY:
            processed = HandleNotify((int)wParam, lParam, &rc.result);
            break;
#endif  // Win95

            // for these messages we must return TRUE if process the message
#ifdef WM_DRAWITEM
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
            {
                int idCtrl = (UINT)wParam;
                if ( message == WM_DRAWITEM )
                {
                    processed = MSWOnDrawItem(idCtrl,
                                              (WXDRAWITEMSTRUCT *)lParam);
                }
                else
                {
                    processed = MSWOnMeasureItem(idCtrl,
                                                 (WXMEASUREITEMSTRUCT *)lParam);
                }

                if ( processed )
                    rc.result = TRUE;
            }
            break;
#endif // defined(WM_DRAWITEM)

        case WM_GETDLGCODE:
            if ( !IsOfStandardClass() )
            {
                // we always want to get the char events
                rc.result = DLGC_WANTCHARS;

                if ( GetWindowStyleFlag() & wxWANTS_CHARS )
                {
                    // in fact, we want everything
                    rc.result |= DLGC_WANTARROWS |
                                 DLGC_WANTTAB |
                                 DLGC_WANTALLKEYS;
                }

                processed = TRUE;
            }
            //else: get the dlg code from the DefWindowProc()
            break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            // If this has been processed by an event handler, return 0 now
            // (we've handled it).
            m_lastKeydownProcessed = HandleKeyDown((WORD) wParam, lParam);
            if ( m_lastKeydownProcessed )
            {
                processed = TRUE;
            }

            if ( !processed )
            {
                switch ( wParam )
                {
                    // we consider these message "not interesting" to OnChar, so
                    // just don't do anything more with them
                    case VK_SHIFT:
                    case VK_CONTROL:
                    case VK_MENU:
                    case VK_CAPITAL:
                    case VK_NUMLOCK:
                    case VK_SCROLL:
                        processed = TRUE;
                        break;

                    // avoid duplicate messages to OnChar for these ASCII keys:
                    // they will be translated by TranslateMessage() and received
                    // in WM_CHAR
                    case VK_ESCAPE:
                    case VK_SPACE:
                    case VK_RETURN:
                    case VK_BACK:
                    case VK_TAB:
                    case VK_ADD:
                    case VK_SUBTRACT:
                    case VK_MULTIPLY:
                    case VK_DIVIDE:
                    case VK_OEM_1:
                    case VK_OEM_2:
                    case VK_OEM_3:
                    case VK_OEM_4:
                    case VK_OEM_5:
                    case VK_OEM_6:
                    case VK_OEM_7:
                    case VK_OEM_PLUS:
                    case VK_OEM_COMMA:
                    case VK_OEM_MINUS:
                    case VK_OEM_PERIOD:
                        // but set processed to FALSE, not TRUE to still pass them
                        // to the control's default window proc - otherwise
                        // built-in keyboard handling won't work
                        processed = FALSE;
                        break;

#ifdef VK_APPS
                    // special case of VK_APPS: treat it the same as right mouse
                    // click because both usually pop up a context menu
                    case VK_APPS:
                        {
                            WPARAM flags;
                            int x, y;

                            TranslateKbdEventToMouse(this, &x, &y, &flags);
                            processed = HandleMouseEvent(WM_RBUTTONDOWN, x, y, flags);
                        }
                        break;
#endif // VK_APPS

                    default:
                        // do generate a CHAR event
                        processed = HandleChar((WORD)wParam, lParam);
                }
            }
            if (message == WM_SYSKEYDOWN)  // Let Windows still handle the SYSKEYs
                processed = FALSE;
            break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
#ifdef VK_APPS
            // special case of VK_APPS: treat it the same as right mouse button
            if ( wParam == VK_APPS )
            {
                WPARAM flags;
                int x, y;

                TranslateKbdEventToMouse(this, &x, &y, &flags);
                processed = HandleMouseEvent(WM_RBUTTONUP, x, y, flags);
            }
            else
#endif // VK_APPS
            {
                processed = HandleKeyUp((WORD) wParam, lParam);
            }
            break;

        case WM_SYSCHAR:
        case WM_CHAR: // Always an ASCII character
            if ( m_lastKeydownProcessed )
            {
                // The key was handled in the EVT_KEY_DOWN and handling
                // a key in an EVT_KEY_DOWN handler is meant, by
                // design, to prevent EVT_CHARs from happening
                m_lastKeydownProcessed = FALSE;
                processed = TRUE;
            }
            else
            {
                processed = HandleChar((WORD)wParam, lParam, TRUE);
            }
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

        // CTLCOLOR messages are sent by children to query the parent for their
        // colors#ifndef __WXMICROWIN__
#ifndef __WXMICROWIN__
#ifdef __WIN32__
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
#else // Win16
        case WM_CTLCOLOR:
#endif // Win32/16
            {
                WXWORD nCtlColor;
                WXHDC hdc;
                WXHWND hwnd;
                UnpackCtlColor(wParam, lParam, &nCtlColor, &hdc, &hwnd);

                processed = HandleCtlColor(&rc.hBrush,
                                           (WXHDC)hdc,
                                           (WXHWND)hwnd,
                                           nCtlColor,
                                           message,
                                           wParam,
                                           lParam);
            }
            break;
#endif // !__WXMICROWIN__

        case WM_SYSCOLORCHANGE:
            // the return value for this message is ignored
            processed = HandleSysColorChange();
            break;

        case WM_DISPLAYCHANGE:
            processed = HandleDisplayChange();
            break;

        case WM_PALETTECHANGED:
            processed = HandlePaletteChanged((WXHWND) (HWND) wParam);
            break;

        case WM_CAPTURECHANGED:
            processed = HandleCaptureChanged((WXHWND) (HWND) lParam);
            break;

        case WM_QUERYNEWPALETTE:
            processed = HandleQueryNewPalette();
            break;

        case WM_ERASEBKGND:
            processed = HandleEraseBkgnd((WXHDC)(HDC)wParam);
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
            processed = HandleInitDialog((WXHWND)(HWND)wParam);

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
            processed = HandleSetCursor((WXHWND)(HWND)wParam,
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

                if (dwObjId == OBJID_CLIENT && GetOrCreateAccessible())
                {
                    return LresultFromObject(IID_IAccessible, wParam, (IUnknown*) GetAccessible()->GetIAccessible());
                }
                break;
            }
#endif

#if defined(__WIN32__) && defined(WM_HELP)
        case WM_HELP:
            {
                HELPINFO* info = (HELPINFO*) lParam;
                // Don't yet process menu help events, just windows
                if (info->iContextType == HELPINFO_WINDOW)
                {
                    wxWindowMSW* subjectOfHelp = this;
                    bool eventProcessed = FALSE;
                    while (subjectOfHelp && !eventProcessed)
                    {
                        wxHelpEvent helpEvent(wxEVT_HELP,
                                              subjectOfHelp->GetId(),
                                              wxPoint(info->MousePos.x,
                                              info->MousePos.y) );
                        helpEvent.SetEventObject(this);
                        eventProcessed =
                            GetEventHandler()->ProcessEvent(helpEvent);

                        // Go up the window hierarchy until the event is
                        // handled (or not)
                        subjectOfHelp = subjectOfHelp->GetParent();
                    }

                    processed = eventProcessed;
                }
                else if (info->iContextType == HELPINFO_MENUITEM)
                {
                    wxHelpEvent helpEvent(wxEVT_HELP, info->iCtrlId);
                    helpEvent.SetEventObject(this);
                    processed = GetEventHandler()->ProcessEvent(helpEvent);

                }
                //else: processed is already FALSE
            }
            break;

        case WM_CONTEXTMENU:
            {
                // we don't convert from screen to client coordinates as
                // the event may be handled by a parent window
                wxPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

                wxContextMenuEvent evtCtx(wxEVT_CONTEXT_MENU, GetId(), pt);
                processed = GetEventHandler()->ProcessEvent(evtCtx);
            }
            break;

        case WM_MENUCHAR:
            // we're only interested in our own menus, not MF_SYSMENU
            if ( HIWORD(wParam) == MF_POPUP )
            {
                // handle menu chars for ownerdrawn menu items
                int i = HandleMenuChar(toupper(LOWORD(wParam)), lParam);
                if ( i != wxNOT_FOUND )
                {
                    rc.result = MAKELRESULT(i, MNC_EXECUTE);
                    processed = TRUE;
                }
            }
            break;
#endif // __WIN32__
    }

    if ( !processed )
    {
#ifdef __WXDEBUG__
#if wxUSE_LOG
        wxLogTrace(wxTraceMessages, wxT("Forwarding %s to DefWindowProc."),
                   wxGetMessageName(message));
#endif // wxUSE_LOG
#endif // __WXDEBUG__
        rc.result = MSWDefWindowProc(message, wParam, lParam);
    }

    return rc.result;
}

// ----------------------------------------------------------------------------
// wxWindow <-> HWND map
// ----------------------------------------------------------------------------

wxWinHashTable *wxWinHandleHash = NULL;

wxWindow *wxFindWinFromHandle(WXHWND hWnd)
{
    return wxWinHandleHash->Get((long)hWnd);
}

void wxAssociateWinWithHandle(HWND hWnd, wxWindowMSW *win)
{
    // adding NULL hWnd is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( hWnd != (HWND)NULL,
                 wxT("attempt to add a NULL hWnd to window list ignored") );

    wxWindow *oldWin = wxFindWinFromHandle((WXHWND) hWnd);
#ifdef __WXDEBUG__
    if ( oldWin && (oldWin != win) )
    {
        wxLogDebug(wxT("HWND %X already associated with another window (%s)"),
                   (int) hWnd, win->GetClassInfo()->GetClassName());
    }
    else
#endif // __WXDEBUG__
    if (!oldWin)
    {
        wxWinHandleHash->Put((long)hWnd, (wxWindow *)win);
    }
}

void wxRemoveHandleAssociation(wxWindowMSW *win)
{
    wxWinHandleHash->Delete((long)win->GetHWND());
}

// ----------------------------------------------------------------------------
// various MSW speciic class dependent functions
// ----------------------------------------------------------------------------

// Default destroyer - override if you destroy it in some other way
// (e.g. with MDI child windows)
void wxWindowMSW::MSWDestroyWindow()
{
}

bool wxWindowMSW::MSWGetCreateWindowCoords(const wxPoint& pos,
                                           const wxSize& size,
                                           int& x, int& y,
                                           int& w, int& h) const
{
    static const int DEFAULT_Y = 200;
    static const int DEFAULT_H = 250;

    bool nonDefault = FALSE;

    if ( pos.x == -1 )
    {
        // if set x to CW_USEDEFAULT, y parameter is ignored anyhow so we can
        // just as well set it to CW_USEDEFAULT as well
        x =
        y = CW_USEDEFAULT;
    }
    else
    {
        // OTOH, if x is not set to CW_USEDEFAULT, y shouldn't be set to it
        // neither because it is not handled as a special value by Windows then
        // and so we have to choose some default value for it
        x = pos.x;
        y = pos.y == -1 ? DEFAULT_Y : pos.y;

        nonDefault = TRUE;
    }

    /*
      NB: there used to be some code here which set the initial size of the
          window to the client size of the parent if no explicit size was
          specified. This was wrong because wxWindows programs often assume
          that they get a WM_SIZE (EVT_SIZE) upon creation, however this broke
          it. To see why, you should understand that Windows sends WM_SIZE from
          inside ::CreateWindow() anyhow. However, ::CreateWindow() is called
          from some base class ctor and so this WM_SIZE is not processed in the
          real class' OnSize() (because it's not fully constructed yet and the
          event goes to some base class OnSize() instead). So the WM_SIZE we
          rely on is the one sent when the parent frame resizes its children
          but here is the problem: if the child already has just the right
          size, nothing will happen as both wxWindows and Windows check for
          this and ignore any attempts to change the window size to the size it
          already has - so no WM_SIZE would be sent.
     */
    if ( size.x == -1 )
    {
        // as above, h is not used at all in this case anyhow
        w =
        h = CW_USEDEFAULT;
    }
    else
    {
        // and, again as above, we can't set the height to CW_USEDEFAULT here
        w = size.x;
        h = size.y == -1 ? DEFAULT_H  : size.y;

        nonDefault = TRUE;
    }

    return nonDefault;
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
    // choose the position/size for the new window
    int x, y, w, h;
    (void)MSWGetCreateWindowCoords(pos, size, x, y, w, h);

    // controlId is menu handle for the top level windows, so set it to 0
    // unless we're creating a child window
    int controlId = style & WS_CHILD ? GetId() : 0;

    // for each class "Foo" we have we also have "FooNR" ("no repaint") class
    // which is the same but without CS_[HV]REDRAW class styles so using it
    // ensures that the window is not fully repainted on each resize
    wxString className(wclass);
    if ( GetWindowStyleFlag() & wxNO_FULL_REPAINT_ON_RESIZE )
    {
        className += wxT("NR");
    }

    // do create the window
    wxWindowCreationHook hook(this);

    m_hWnd = (WXHWND)::CreateWindowEx
                       (
                            extendedStyle,
                            className,
                            title ? title : wxT(""),
                            style,
                            x, y, w, h,
                            (HWND)MSWGetParent(),
                            (HMENU)controlId,
                            wxGetInstance(),
                            NULL                        // no extra data
                       );

    if ( !m_hWnd )
    {
        wxLogSysError(_("Can't create window of class %s"), wclass);

        return FALSE;
    }

    SubclassWin(m_hWnd);

    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    return TRUE;
}

// ===========================================================================
// MSW message handlers
// ===========================================================================

// ---------------------------------------------------------------------------
// WM_NOTIFY
// ---------------------------------------------------------------------------

#ifdef __WIN95__

bool wxWindowMSW::HandleNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
#ifndef __WXMICROWIN__
    LPNMHDR hdr = (LPNMHDR)lParam;
    HWND hWnd = hdr->hwndFrom;
    wxWindow *win = wxFindWinFromHandle((WXHWND)hWnd);

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
    wxWindowList::Node *node = GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *child = node->GetData();
        if ( child->MSWOnNotify(idCtrl, lParam, result) )
        {
            return TRUE;
        }

        node = node->GetNext();
    }
#endif // 0

    // by default, handle it ourselves
    return MSWOnNotify(idCtrl, lParam, result);
#else // __WXMICROWIN__
    return FALSE;
#endif
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

    if ( !(code == (WXUINT) TTN_NEEDTEXTA || code == (WXUINT) TTN_NEEDTEXTW) || ttip.empty() )
    {
        // not a tooltip message or no tooltip to show anyhow
        return FALSE;
    }

    LPTOOLTIPTEXT ttText = (LPTOOLTIPTEXT)lParam;

#if !wxUSE_UNICODE
    if ( code == (WXUINT) TTN_NEEDTEXTA )
    {
        // we pass just the pointer as we store the string internally anyhow
        ttText->lpszText = (char *)ttip.c_str();
    }
    else // TTN_NEEDTEXTW
#endif // !Unicode
    {
#if wxUSE_UNICODE
        // in Unicode mode this is just what we need
        ttText->lpszText = (wxChar *)ttip.c_str();
#else // !Unicode
        MultiByteToWideChar(CP_ACP, 0, ttip, ttip.length()+1,
                            (wchar_t *)ttText->szText,
                            sizeof(ttText->szText) / sizeof(wchar_t));
#endif // Unicode/!Unicode
    }

    return TRUE;
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
            return TRUE;
        }
    }
#endif // wxUSE_TOOLTIPS

    return FALSE;
}

#endif // __WIN95__

// ---------------------------------------------------------------------------
// end session messages
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleQueryEndSession(long logOff, bool *mayEnd)
{
    wxCloseEvent event(wxEVT_QUERY_END_SESSION, -1);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(TRUE);
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
        return FALSE;

    // only send once
    if ( (this != wxTheApp->GetTopWindow()) )
        return FALSE;

    wxCloseEvent event(wxEVT_END_SESSION, -1);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(FALSE);
    event.SetLoggingOff( (logOff == (long)ENDSESSION_LOGOFF) );

    return wxTheApp->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// window creation/destruction
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleCreate(WXLPCREATESTRUCT cs, bool *mayCreate)
{
    // if we have WS_EX_CONTROLPARENT flag we absolutely *must* set it for our
    // parent as well as otherwise several Win32 functions using
    // GetNextDlgTabItem() to iterate over all controls such as
    // IsDialogMessage() or DefDlgProc() would enter an infinite loop: indeed,
    // all of them iterate over all the controls starting from the focus and
    // stop iterating when they get back to the focus but unless all parents
    // have WS_EX_CONTROLPARENT bit set, they would never get back to focus
    if ( ((CREATESTRUCT *)cs)->dwExStyle & WS_EX_CONTROLPARENT )
    {
        // there is no need to do anything for the top level windows
        const wxWindow *parent = GetParent();
        while ( parent && !parent->IsTopLevel() )
        {
            LONG exStyle = ::GetWindowLong(GetHwndOf(parent), GWL_EXSTYLE);
            if ( !(exStyle & WS_EX_CONTROLPARENT) )
            {
                // force the parent to have this style
                ::SetWindowLong(GetHwndOf(parent), GWL_EXSTYLE,
                                exStyle | WS_EX_CONTROLPARENT);
            }

            parent = parent->GetParent();
        }
    }

    // TODO: should generate this event from WM_NCCREATE
    wxWindowCreateEvent event((wxWindow *)this);
    (void)GetEventHandler()->ProcessEvent(event);

    *mayCreate = TRUE;

    return TRUE;
}

bool wxWindowMSW::HandleDestroy()
{
    SendDestroyEvent();

    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget != NULL )
    {
        m_dropTarget->Revoke(m_hWnd);

        delete m_dropTarget;
        m_dropTarget = NULL;
    }
#endif // wxUSE_DRAG_AND_DROP

    // WM_DESTROY handled
    return TRUE;
}

// ---------------------------------------------------------------------------
// activation/focus
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleActivate(int state,
                              bool WXUNUSED(minimized),
                              WXHWND WXUNUSED(activate))
{
    wxActivateEvent event(wxEVT_ACTIVATE,
                          (state == WA_ACTIVE) || (state == WA_CLICKACTIVE),
                          m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleSetFocus(WXHWND hwnd)
{
    // notify the parent keeping track of focus for the kbd navigation
    // purposes that we got it
    wxChildFocusEvent eventFocus((wxWindow *)this);
    (void)GetEventHandler()->ProcessEvent(eventFocus);

#if wxUSE_CARET
    // Deal with caret
    if ( m_caret )
    {
        m_caret->OnSetFocus();
    }
#endif // wxUSE_CARET

#if wxUSE_TEXTCTRL
    // If it's a wxTextCtrl don't send the event as it will be done
    // after the control gets to process it from EN_FOCUS handler
    if ( wxDynamicCastThis(wxTextCtrl) )
    {
        return FALSE;
    }
#endif // wxUSE_TEXTCTRL

    wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
    event.SetEventObject(this);

    // wxFindWinFromHandle() may return NULL, it is ok
    event.SetWindow(wxFindWinFromHandle(hwnd));

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleKillFocus(WXHWND hwnd)
{
#if wxUSE_CARET
    // Deal with caret
    if ( m_caret )
    {
        m_caret->OnKillFocus();
    }
#endif // wxUSE_CARET

#if wxUSE_TEXTCTRL
    // If it's a wxTextCtrl don't send the event as it will be done
    // after the control gets to process it.
    wxTextCtrl *ctrl = wxDynamicCastThis(wxTextCtrl);
    if ( ctrl )
    {
        return FALSE;
    }
#endif

    // Don't send the event when in the process of being deleted.  This can
    // only cause problems if the event handler tries to access the object.
    if ( m_isBeingDeleted )
    {
        return FALSE;
    }

    wxFocusEvent event(wxEVT_KILL_FOCUS, m_windowId);
    event.SetEventObject(this);

    // wxFindWinFromHandle() may return NULL, it is ok
    event.SetWindow(wxFindWinFromHandle(hwnd));

    return GetEventHandler()->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// miscellaneous
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleShow(bool show, int WXUNUSED(status))
{
    wxShowEvent event(GetId(), show);
    event.m_eventObject = this;

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleInitDialog(WXHWND WXUNUSED(hWndFocus))
{
    wxInitDialogEvent event(GetId());
    event.m_eventObject = this;

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleDropFiles(WXWPARAM wParam)
{
#if defined (__WXMICROWIN__) 
    return FALSE;
#else // __WXMICROWIN__
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
                        files[wIndex].GetWriteBuf(len), len);

        files[wIndex].UngetWriteBuf();
    }
    DragFinish (hFilesInfo);

    wxDropFilesEvent event(wxEVT_DROP_FILES, gwFilesDropped, files);
    event.m_eventObject = this;

    POINT dropPoint;
    DragQueryPoint(hFilesInfo, (LPPOINT) &dropPoint);
    event.m_pos.x = dropPoint.x;
    event.m_pos.y = dropPoint.y;

    return GetEventHandler()->ProcessEvent(event);
#endif
}

#ifdef __DIGITALMARS__
extern "C" HCURSOR wxGetCurrentBusyCursor();
#endif

bool wxWindowMSW::HandleSetCursor(WXHWND WXUNUSED(hWnd),
                                  short nHitTest,
                                  int WXUNUSED(mouseMsg))
{
#ifndef __WXMICROWIN__
    // the logic is as follows:
    // -1. don't set cursor for non client area, including but not limited to
    //     the title bar, scrollbars, &c
    //  0. allow the user to override default behaviour by using EVT_SET_CURSOR
    //  1. if we have the cursor set it unless wxIsBusy()
    //  2. if we're a top level window, set some cursor anyhow
    //  3. if wxIsBusy(), set the busy cursor, otherwise the global one

    if ( nHitTest != HTCLIENT )
    {
        return FALSE;
    }

    HCURSOR hcursor = 0;

    // first ask the user code - it may wish to set the cursor in some very
    // specific way (for example, depending on the current position)
    POINT pt;
#ifdef __WIN32__
    if ( !::GetCursorPos(&pt) )
    {
        wxLogLastError(wxT("GetCursorPos"));
    }
#else
    // In WIN16 it doesn't return a value.
    ::GetCursorPos(&pt);
#endif

    int x = pt.x,
        y = pt.y;
    ScreenToClient(&x, &y);
    wxSetCursorEvent event(x, y);

    bool processedEvtSetCursor = GetEventHandler()->ProcessEvent(event);
    if ( processedEvtSetCursor && event.HasCursor() )
    {
        hcursor = GetHcursorOf(event.GetCursor());
    }

    if ( !hcursor )
    {
        bool isBusy = wxIsBusy();

        // the test for processedEvtSetCursor is here to prevent using m_cursor
        // if the user code caught EVT_SET_CURSOR() and returned nothing from
        // it - this is a way to say that our cursor shouldn't be used for this
        // point
        if ( !processedEvtSetCursor && m_cursor.Ok() )
        {
            hcursor = GetHcursorOf(m_cursor);
        }

        if ( !GetParent() )
        {
            if ( isBusy )
            {
                hcursor = wxGetCurrentBusyCursor();
            }
            else if ( !hcursor )
            {
                const wxCursor *cursor = wxGetGlobalCursor();
                if ( cursor && cursor->Ok() )
                {
                    hcursor = GetHcursorOf(*cursor);
                }
            }
        }
    }

    if ( hcursor )
    {
//        wxLogDebug("HandleSetCursor: Setting cursor %ld", (long) hcursor);

        ::SetCursor(hcursor);

        // cursor set, stop here
        return TRUE;
    }
#endif // __WXMICROWIN__

    // pass up the window chain
    return FALSE;
}

// ---------------------------------------------------------------------------
// owner drawn stuff
// ---------------------------------------------------------------------------

bool wxWindowMSW::MSWOnDrawItem(int id, WXDRAWITEMSTRUCT *itemStruct)
{
#if wxUSE_OWNER_DRAWN

#if wxUSE_MENUS_NATIVE
    // is it a menu item?
    DRAWITEMSTRUCT *pDrawStruct = (DRAWITEMSTRUCT *)itemStruct;
    if ( id == 0 && pDrawStruct->CtlType == ODT_MENU )
    {
        wxMenuItem *pMenuItem = (wxMenuItem *)(pDrawStruct->itemData);

        wxCHECK( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

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

#if wxUSE_CONTROLS

    wxWindow *item = FindItem(id);
#if wxUSE_OWNER_DRAWN
    if ( item && item->IsKindOf(CLASSINFO(wxControl)) )
        return ((wxControl *)item)->MSWOnDraw(itemStruct);
#else
    if ( item && item->IsKindOf(CLASSINFO(wxButton)) )
        return ((wxButton *)item)->MSWOnDraw(itemStruct);
#endif // USE_OWNER_DRAWN

#endif // wxUSE_CONTROLS

    return FALSE;
}

bool wxWindowMSW::MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *itemStruct)
{
#if wxUSE_OWNER_DRAWN
    // is it a menu item?
    MEASUREITEMSTRUCT *pMeasureStruct = (MEASUREITEMSTRUCT *)itemStruct;
    if ( id == 0 && pMeasureStruct->CtlType == ODT_MENU )
    {
        wxMenuItem *pMenuItem = (wxMenuItem *)(pMeasureStruct->itemData);

        wxCHECK( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

        return pMenuItem->OnMeasureItem(&pMeasureStruct->itemWidth,
                                        &pMeasureStruct->itemHeight);
    }

    wxWindow *item = FindItem(id);
    if ( item && item->IsKindOf(CLASSINFO(wxControl)) )
    {
        return ((wxControl *)item)->MSWOnMeasure(itemStruct);
    }
#endif  // owner-drawn menus
    return FALSE;
}

// ---------------------------------------------------------------------------
// colours and palettes
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleSysColorChange()
{
    wxSysColourChangedEvent event;
    event.SetEventObject(this);

    (void)GetEventHandler()->ProcessEvent(event);

    // always let the system carry on the default processing to allow the
    // native controls to react to the colours update
    return FALSE;
}

bool wxWindowMSW::HandleDisplayChange()
{
    wxDisplayChangedEvent event;
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleCtlColor(WXHBRUSH *brush,
                              WXHDC pDC,
                              WXHWND pWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
#ifndef __WXMICROWIN__
    WXHBRUSH hBrush = 0;

    if ( nCtlColor == CTLCOLOR_DLG )
    {
        hBrush = OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);
    }
#if wxUSE_CONTROLS
    else
    {
        wxControl *item = (wxControl *)FindItemByHWND(pWnd, TRUE);
        if ( item )
            hBrush = item->OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);
    }
#endif // wxUSE_CONTROLS

    if ( hBrush )
        *brush = hBrush;

    return hBrush != 0;
#else // __WXMICROWIN__
    return FALSE;
#endif
}

// Define for each class of dialog and control
WXHBRUSH wxWindowMSW::OnCtlColor(WXHDC WXUNUSED(hDC),
                                 WXHWND WXUNUSED(hWnd),
                                 WXUINT WXUNUSED(nCtlColor),
                                 WXUINT WXUNUSED(message),
                                 WXWPARAM WXUNUSED(wParam),
                                 WXLPARAM WXUNUSED(lParam))
{
    return (WXHBRUSH)0;
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
                InvalidateRect((HWND) hWndPalChange, NULL, TRUE);
        }

    }
#endif // wxUSE_PALETTE

    wxPaletteChangedEvent event(GetId());
    event.SetEventObject(this);
    event.SetChangedWindow(wxFindWinFromHandle(hWndPalChange));

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleCaptureChanged(WXHWND hWndGainedCapture)
{
    wxMouseCaptureChangedEvent event(GetId(), wxFindWinFromHandle(hWndGainedCapture));
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleQueryNewPalette()
{

#if wxUSE_PALETTE
    // check to see if we our our parents have a custom palette
    wxWindowMSW *win = this;
    while (!win->HasCustomPalette() && win->GetParent()) win = win->GetParent();
    if (win->HasCustomPalette()) {
        /* realize the palette to see whether redrawing is needed */
        HDC hdc = GetDC((HWND) GetHWND());
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

    return GetEventHandler()->ProcessEvent(event) && event.GetPaletteRealized();
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
        gs_hasStdCmap = FALSE;
    }
    wxWindowList::Node *node = GetChildren().GetFirst();
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

    // update the colours we use if they were not set explicitly by the user:
    // this must be done or OnCtlColor() would continue to use the old colours
    if ( !m_hasFgCol )
    {
        m_foregroundColour = wxSystemSettings::
                                GetSystemColour(wxSYS_COLOUR_WINDOWTEXT);
    }

    if ( !m_hasBgCol )
    {
        m_backgroundColour = wxSystemSettings::
                                GetSystemColour(wxSYS_COLOUR_BTNFACE);
    }
}

extern wxCOLORMAP *wxGetStdColourMap()
{
    static COLORREF s_stdColours[wxSTD_COL_MAX];
    static wxCOLORMAP s_cmap[wxSTD_COL_MAX];

    if ( !gs_hasStdCmap )
    {
        static bool s_coloursInit = FALSE;

        if ( !s_coloursInit )
        {
            // When a bitmap is loaded, the RGB values can change (apparently
            // because Windows adjusts them to care for the old programs always
            // using 0xc0c0c0 while the transparent colour for the new Windows
            // versions is different). But we do this adjustment ourselves so
            // we want to avoid Windows' "help" and for this we need to have a
            // reference bitmap which can tell us what the RGB values change
            // to.
            wxBitmap stdColourBitmap(_T("wxBITMAP_STD_COLOURS"));
            if ( stdColourBitmap.Ok() )
            {
                // the pixels in the bitmap must correspond to wxSTD_COL_XXX!
                wxASSERT_MSG( stdColourBitmap.GetWidth() == wxSTD_COL_MAX,
                              _T("forgot to update wxBITMAP_STD_COLOURS!") );

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

            s_coloursInit = TRUE;
        }

        gs_hasStdCmap = TRUE;

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

// ---------------------------------------------------------------------------
// painting
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandlePaint()
{
//    if (GetExtraStyle() & wxWS_EX_THEMED_BACKGROUND)
//        return FALSE;
    
#ifdef __WIN32__
    HRGN hRegion = ::CreateRectRgn(0, 0, 0, 0); // Dummy call to get a handle
    if ( !hRegion )
        wxLogLastError(wxT("CreateRectRgn"));
    if ( ::GetUpdateRgn(GetHwnd(), hRegion, FALSE) == ERROR )
        wxLogLastError(wxT("GetUpdateRgn"));

    m_updateRegion = wxRegion((WXHRGN) hRegion);
#else // Win16
    RECT updateRect;
    ::GetUpdateRect(GetHwnd(), &updateRect, FALSE);

    m_updateRegion = wxRegion(updateRect.left, updateRect.top,
                              updateRect.right - updateRect.left,
                              updateRect.bottom - updateRect.top);
#endif // Win32/16

    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);

    bool processed = GetEventHandler()->ProcessEvent(event);

    // note that we must generate NC event after the normal one as otherwise
    // BeginPaint() will happily overwrite our decorations with the background
    // colour
    wxNcPaintEvent eventNc(m_windowId);
    eventNc.SetEventObject(this);
    GetEventHandler()->ProcessEvent(eventNc);

    return processed;
}

// Can be called from an application's OnPaint handler
void wxWindowMSW::OnPaint(wxPaintEvent& event)
{
#ifdef __WXUNIVERSAL__
    event.Skip();
#else
    HDC hDC = (HDC) wxPaintDC::FindDCInCache((wxWindow*) event.GetEventObject());
    if (hDC != 0)
    {
        MSWDefWindowProc(WM_PAINT, (WPARAM) hDC, 0);
    }
#endif
}

bool wxWindowMSW::HandleEraseBkgnd(WXHDC hdc)
{
    // Prevents flicker when dragging
    if ( ::IsIconic(GetHwnd()) )
        return TRUE;

#if 0
    if (GetParent() && GetParent()->GetExtraStyle() & wxWS_EX_THEMED_BACKGROUND)
    {
        return FALSE;
    }

    if (GetExtraStyle() & wxWS_EX_THEMED_BACKGROUND)
    {
        if (wxUxThemeEngine::Get())
        {
            WXHTHEME hTheme = wxUxThemeEngine::Get()->m_pfnOpenThemeData(GetHWND(), L"TAB");
            if (hTheme)
            {
                WXURECT rect;
                ::GetClientRect((HWND) GetHWND(), (RECT*) & rect);
                wxUxThemeEngine::Get()->m_pfnDrawThemeBackground(hTheme, hdc, 10 /* TABP_BODY */, 0, &rect, &rect);
                wxUxThemeEngine::Get()->m_pfnCloseThemeData(hTheme);
                return TRUE;
            }
        }
    }
#endif
    
    wxDCTemp dc(hdc);

    dc.SetHDC(hdc);
    dc.SetWindow((wxWindow *)this);
    dc.BeginDrawing();

    wxEraseEvent event(m_windowId, &dc);
    event.SetEventObject(this);
    bool rc = GetEventHandler()->ProcessEvent(event);

    dc.EndDrawing();

    // must be called manually as ~wxDC doesn't do anything for wxDCTemp
    dc.SelectOldObjects(hdc);

    return rc;
}

void wxWindowMSW::OnEraseBackground(wxEraseEvent& event)
{
    RECT rect;
    ::GetClientRect(GetHwnd(), &rect);

    COLORREF ref = PALETTERGB(m_backgroundColour.Red(),
                              m_backgroundColour.Green(),
                              m_backgroundColour.Blue());
    HBRUSH hBrush = ::CreateSolidBrush(ref);
    if ( !hBrush )
        wxLogLastError(wxT("CreateSolidBrush"));

    HDC hdc = (HDC)event.GetDC()->GetHDC();

    int mode = ::SetMapMode(hdc, MM_TEXT);

    ::FillRect(hdc, &rect, hBrush);
    ::DeleteObject(hBrush);
    ::SetMapMode(hdc, mode);
}

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleMinimize()
{
    wxIconizeEvent event(m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleMaximize()
{
    wxMaximizeEvent event(m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleMove(int x, int y)
{
    wxMoveEvent event(wxPoint(x, y), m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleMoving(wxRect& rect)
{
    wxMoveEvent event(rect, m_windowId);
    event.SetEventObject(this);
    
    bool rc = GetEventHandler()->ProcessEvent(event);
    if (rc)
        rect = event.GetRect();
    return rc;
}

bool wxWindowMSW::HandleSize(int WXUNUSED(w), int WXUNUSED(h),
                             WXUINT WXUNUSED(flag))
{
    // don't use w and h parameters as they specify the client size while
    // according to the docs EVT_SIZE handler is supposed to receive the total
    // size
    wxSizeEvent event(GetSize(), m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleSizing(wxRect& rect)
{
    wxSizeEvent event(rect, m_windowId);
    event.SetEventObject(this);
    
    bool rc = GetEventHandler()->ProcessEvent(event);
    if (rc)
        rect = event.GetRect();
    return rc;
}

bool wxWindowMSW::HandleGetMinMaxInfo(void *mmInfo)
{
    MINMAXINFO *info = (MINMAXINFO *)mmInfo;

    bool rc = FALSE;

    int minWidth = GetMinWidth(),
        minHeight = GetMinHeight(),
        maxWidth = GetMaxWidth(),
        maxHeight = GetMaxHeight();

    if ( minWidth != -1 )
    {
        info->ptMinTrackSize.x = minWidth;
        rc = TRUE;
    }

    if ( minHeight != -1 )
    {
        info->ptMinTrackSize.y = minHeight;
        rc = TRUE;
    }

    if ( maxWidth != -1 )
    {
        info->ptMaxTrackSize.x = maxWidth;
        rc = TRUE;
    }

    if ( maxHeight != -1 )
    {
        info->ptMaxTrackSize.y = maxHeight;
        rc = TRUE;
    }

    return rc;
}

// ---------------------------------------------------------------------------
// command messages
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
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
        // must cast to a signed type before comparing with other ids!
        win = FindItem((signed short)id);
    }

    if ( win )
    {
        return win->MSWCommand(cmd, id);
    }

    // the messages sent from the in-place edit control used by the treectrl
    // for label editing have id == 0, but they should _not_ be treated as menu
    // messages (they are EN_XXX ones, in fact) so don't translate anything
    // coming from a control to wxEVT_COMMAND_MENU_SELECTED
    if ( !control )
    {
        // If no child window, it may be an accelerator, e.g. for a popup menu
        // command

        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED);
        event.SetEventObject(this);
        event.SetId(id);
        event.SetInt(id);

        return GetEventHandler()->ProcessEvent(event);
    }
#if wxUSE_SPINCTRL && !defined(__WXUNIVERSAL__)
    else
    {
        // the text ctrl which is logically part of wxSpinCtrl sends WM_COMMAND
        // notifications to its parent which we want to reflect back to
        // wxSpinCtrl
        wxSpinCtrl *spin = wxSpinCtrl::GetSpinForTextCtrl(control);
        if ( spin && spin->ProcessTextCommand(cmd, id) )
            return TRUE;
    }
#endif // wxUSE_SPINCTRL

    return FALSE;
}

bool wxWindowMSW::HandleSysCommand(WXWPARAM wParam, WXLPARAM WXUNUSED(lParam))
{
    // 4 bits are reserved
    switch ( wParam & 0xFFFFFFF0 )
    {
        case SC_MAXIMIZE:
            return HandleMaximize();

        case SC_MINIMIZE:
            return HandleMinimize();
    }

    return FALSE;
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
 //   event.m_altDown = (::GetKeyState(VK_MENU) & 0x80000000) != 0;
    // Returns different negative values on WinME and WinNT,
    // so simply test for negative value.
    event.m_altDown = ::GetKeyState(VK_MENU) < 0;

    event.SetTimestamp(s_currentMsg.time);
    event.m_eventObject = this;
    event.SetId(GetId());

#if wxUSE_MOUSEEVENT_HACK
    m_lastMouseX = x;
    m_lastMouseY = y;
    m_lastMouseEvent = event.GetEventType();
#endif // wxUSE_MOUSEEVENT_HACK
}

// Windows doesn't send the mouse events to the static controls (which are
// transparent in the sense that their WM_NCHITTEST handler returns
// HTTRANSPARENT) at all but we want all controls to receive the mouse events
// and so we manually check if we don't have a child window under mouse and if
// we do, send the event to it instead of the window Windows had sent WM_XXX
// to.
//
// Notice that this is not done for the mouse move events because this could
// (would?) be too slow, but only for clicks which means that the static texts
// still don't get move, enter nor leave events.
static wxWindowMSW *FindWindowForMouseEvent(wxWindowMSW *win, int *x, int *y) //TW:REQ:Univ
{
    wxCHECK_MSG( x && y, win, _T("NULL pointer in FindWindowForMouseEvent") );

    // first try to find a non transparent child: this allows us to send events
    // to a static text which is inside a static box, for example
    POINT pt = { *x, *y };
    HWND hwnd = GetHwndOf(win),
         hwndUnderMouse;

#ifdef __WIN32__
    hwndUnderMouse = ::ChildWindowFromPointEx
                       (
                        hwnd,
                        pt,
                        CWP_SKIPINVISIBLE   |
                        CWP_SKIPDISABLED    |
                        CWP_SKIPTRANSPARENT
                       );

    if ( !hwndUnderMouse || hwndUnderMouse == hwnd )
#endif // __WIN32__
    {
        // now try any child window at all
        hwndUnderMouse = ::ChildWindowFromPoint(hwnd, pt);
    }

    // check that we have a child window which is susceptible to receive mouse
    // events: for this it must be shown and enabled
    if ( hwndUnderMouse &&
            hwndUnderMouse != hwnd &&
                ::IsWindowVisible(hwndUnderMouse) &&
                    ::IsWindowEnabled(hwndUnderMouse) )
    {
        wxWindow *winUnderMouse = wxFindWinFromHandle((WXHWND)hwndUnderMouse);
        if ( winUnderMouse )
        {
            // translate the mouse coords to the other window coords
            win->ClientToScreen(x, y);
            winUnderMouse->ScreenToClient(x, y);

            win = winUnderMouse;
        }
    }

    return win;
}

bool wxWindowMSW::HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags)
{
    // the mouse events take consecutive IDs from WM_MOUSEFIRST to
    // WM_MOUSELAST, so it's enough to substract WM_MOUSEMOVE == WM_MOUSEFIRST
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
        wxEVT_MIDDLE_DCLICK
    };

    wxMouseEvent event(eventsMouse[msg - WM_MOUSEMOVE]);
    InitMouseEvent(event, x, y, flags);

    return GetEventHandler()->ProcessEvent(event);
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
            m_mouseInWindow = TRUE;

            wxMouseEvent event(wxEVT_ENTER_WINDOW);
            InitMouseEvent(event, x, y, flags);

            (void)GetEventHandler()->ProcessEvent(event);
        }
    }

#if wxUSE_MOUSEEVENT_HACK
    // Window gets a click down message followed by a mouse move message even
    // if position isn't changed!  We want to discard the trailing move event
    // if x and y are the same.
    if ( (m_lastMouseEvent == wxEVT_RIGHT_DOWN ||
          m_lastMouseEvent == wxEVT_LEFT_DOWN ||
          m_lastMouseEvent == wxEVT_MIDDLE_DOWN) &&
         (m_lastMouseX == x && m_lastMouseY == y) )
    {
        m_lastMouseEvent = wxEVT_MOTION;

        return FALSE;
    }
#endif // wxUSE_MOUSEEVENT_HACK

    return HandleMouseEvent(WM_MOUSEMOVE, x, y, flags);
}


bool wxWindowMSW::HandleMouseWheel(WXWPARAM wParam, WXLPARAM lParam)
{
#if wxUSE_MOUSEWHEEL
    wxMouseEvent event(wxEVT_MOUSEWHEEL);
    InitMouseEvent(event,
                   GET_X_LPARAM(lParam),
                   GET_Y_LPARAM(lParam),
                   LOWORD(wParam));
    event.m_wheelRotation = (short)HIWORD(wParam);
    event.m_wheelDelta = WHEEL_DELTA;

#ifdef __WIN32__
    static int s_linesPerRotation = -1;
    if ( s_linesPerRotation == -1 )
    {
        if ( !::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0,
                                     &s_linesPerRotation, 0))
        {
            // this is not supposed to happen
            wxLogLastError(_T("SystemParametersInfo(GETWHEELSCROLLLINES)"));

            // the default is 3, so use it if SystemParametersInfo() failed
            s_linesPerRotation = 3;
        }
    }
#else // Win16
    // no SystemParametersInfo() under Win16
    static const int s_linesPerRotation = 3;
#endif

    event.m_linesPerAction = s_linesPerRotation;
    return GetEventHandler()->ProcessEvent(event);

#else
    (void) wParam;
    (void) lParam;

    return FALSE;
#endif
}


// ---------------------------------------------------------------------------
// keyboard handling
// ---------------------------------------------------------------------------

// create the key event of the given type for the given key - used by
// HandleChar and HandleKeyDown/Up
wxKeyEvent wxWindowMSW::CreateKeyEvent(wxEventType evType,
                                       int id,
                                       WXLPARAM lParam,
                                       WXWPARAM wParam) const
{
    wxKeyEvent event(evType);
    event.SetId(GetId());
    event.m_shiftDown = wxIsShiftDown();
    event.m_controlDown = wxIsCtrlDown();
    event.m_altDown = (HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN;

    event.m_eventObject = (wxWindow *)this; // const_cast
    event.m_keyCode = id;
    event.m_rawCode = (wxUint32) wParam;
    event.m_rawFlags = (wxUint32) lParam;
    event.SetTimestamp(s_currentMsg.time);

    // translate the position to client coords
    POINT pt;
    GetCursorPos(&pt);
    RECT rect;
    GetWindowRect(GetHwnd(),&rect);
    pt.x -= rect.left;
    pt.y -= rect.top;

    event.m_x = pt.x;
    event.m_y = pt.y;

    return event;
}

// isASCII is TRUE only when we're called from WM_CHAR handler and not from
// WM_KEYDOWN one
bool wxWindowMSW::HandleChar(WXWPARAM wParam, WXLPARAM lParam, bool isASCII)
{
    bool ctrlDown = FALSE;

    int id;
    if ( isASCII )
    {
        // If 1 -> 26, translate to either special keycode or just set
        // ctrlDown.  IOW, Ctrl-C should result in keycode == 3 and
        // ControlDown() == TRUE.
        id = wParam;
        if ( (id > 0) && (id < 27) )
        {
            switch (id)
            {
                case 13:
                    id = WXK_RETURN;
                    break;

                case 8:
                    id = WXK_BACK;
                    break;

                case 9:
                    id = WXK_TAB;
                    break;

                default:
                    ctrlDown = TRUE;
                    break;
            }
        }
    }
    else // we're called from WM_KEYDOWN
    {
        id = wxCharCodeMSWToWX(wParam);
        if ( id == 0 )
        {
            // it's ASCII and will be processed here only when called from
            // WM_CHAR (i.e. when isASCII = TRUE), don't process it now
            return FALSE;
        }
    }

    wxKeyEvent event(CreateKeyEvent(wxEVT_CHAR, id, lParam, wParam));

    // the alphanumeric keys produced by pressing AltGr+something on European
    // keyboards have both Ctrl and Alt modifiers which may confuse the user
    // code as, normally, keys with Ctrl and/or Alt don't result in anything
    // alphanumeric, so pretend that there are no modifiers at all (the
    // KEY_DOWN event would still have the correct modifiers if they're really
    // needed)
    if ( event.m_controlDown && event.m_altDown &&
            (id >= 32 && id < 256) )
    {
        event.m_controlDown =
        event.m_altDown = FALSE;
    }

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowMSW::HandleKeyDown(WXWPARAM wParam, WXLPARAM lParam)
{
    int id = wxCharCodeMSWToWX(wParam);

    if ( !id )
    {
        // normal ASCII char
        id = wParam;
    }

    if ( id != -1 ) // VZ: does this ever happen (FIXME)?
    {
        wxKeyEvent event(CreateKeyEvent(wxEVT_KEY_DOWN, id, lParam, wParam));
        if ( GetEventHandler()->ProcessEvent(event) )
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool wxWindowMSW::HandleKeyUp(WXWPARAM wParam, WXLPARAM lParam)
{
    int id = wxCharCodeMSWToWX(wParam);

    if ( !id )
    {
        // normal ASCII char
        id = wParam;
    }

    if ( id != -1 ) // VZ: does this ever happen (FIXME)?
    {
        wxKeyEvent event(CreateKeyEvent(wxEVT_KEY_UP, id, lParam, wParam));
        if ( GetEventHandler()->ProcessEvent(event) )
            return TRUE;
    }

    return FALSE;
}

#ifdef __WIN32__

int wxWindowMSW::HandleMenuChar(int chAccel, WXLPARAM lParam)
{
    const HMENU hmenu = (HMENU)lParam;

    MENUITEMINFO mii;
    wxZeroMemory(mii);
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_TYPE | MIIM_DATA;

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

                const wxChar *p = wxStrchr(item->GetText(), _T('&'));
                while ( p++ )
                {
                    if ( *p == _T('&') )
                    {
                        // this is not the accel char, find the real one
                        p = wxStrchr(p + 1, _T('&'));
                    }
                    else // got the accel char
                    {
                        // FIXME-UNICODE: this comparison doesn't risk to work
                        // for non ASCII accelerator characters I'm afraid, but
                        // what can we do?
                        if ( wxToupper(*p) == chAccel )
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
        else // failed ot get the menu text?
        {
            // it's not fatal, so don't show error, but still log
            // it
            wxLogLastError(_T("GetMenuItemInfo"));
        }
    }

    return wxNOT_FOUND;
}

#endif // __WIN32__

// ---------------------------------------------------------------------------
// joystick
// ---------------------------------------------------------------------------

bool wxWindowMSW::HandleJoystickEvent(WXUINT msg, int x, int y, WXUINT flags)
{
#ifdef JOY_BUTTON1
    int change = 0;
    if ( flags & JOY_BUTTON1CHG )
        change = wxJOY_BUTTON1;
    if ( flags & JOY_BUTTON2CHG )
        change = wxJOY_BUTTON2;
    if ( flags & JOY_BUTTON3CHG )
        change = wxJOY_BUTTON3;
    if ( flags & JOY_BUTTON4CHG )
        change = wxJOY_BUTTON4;

    int buttons = 0;
    if ( flags & JOY_BUTTON1 )
        buttons |= wxJOY_BUTTON1;
    if ( flags & JOY_BUTTON2 )
        buttons |= wxJOY_BUTTON2;
    if ( flags & JOY_BUTTON3 )
        buttons |= wxJOY_BUTTON3;
    if ( flags & JOY_BUTTON4 )
        buttons |= wxJOY_BUTTON4;

    // the event ids aren't consecutive so we can't use table based lookup
    int joystick;
    wxEventType eventType;
    switch ( msg )
    {
        case MM_JOY1MOVE:
            joystick = 1;
            eventType = wxEVT_JOY_MOVE;
            break;

        case MM_JOY2MOVE:
            joystick = 2;
            eventType = wxEVT_JOY_MOVE;
            break;

        case MM_JOY1ZMOVE:
            joystick = 1;
            eventType = wxEVT_JOY_ZMOVE;
            break;

        case MM_JOY2ZMOVE:
            joystick = 2;
            eventType = wxEVT_JOY_ZMOVE;
            break;

        case MM_JOY1BUTTONDOWN:
            joystick = 1;
            eventType = wxEVT_JOY_BUTTON_DOWN;
            break;

        case MM_JOY2BUTTONDOWN:
            joystick = 2;
            eventType = wxEVT_JOY_BUTTON_DOWN;
            break;

        case MM_JOY1BUTTONUP:
            joystick = 1;
            eventType = wxEVT_JOY_BUTTON_UP;
            break;

        case MM_JOY2BUTTONUP:
            joystick = 2;
            eventType = wxEVT_JOY_BUTTON_UP;
            break;

        default:
            wxFAIL_MSG(wxT("no such joystick event"));

            return FALSE;
    }

    wxJoystickEvent event(eventType, buttons, joystick, change);
    event.SetPosition(wxPoint(x, y));
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
#else
    return FALSE;
#endif
}

// ---------------------------------------------------------------------------
// scrolling
// ---------------------------------------------------------------------------

bool wxWindowMSW::MSWOnScroll(int orientation, WXWORD wParam,
                              WXWORD pos, WXHWND control)
{
    if ( control )
    {
        wxWindow *child = wxFindWinFromHandle(control);
        if ( child )
            return child->MSWOnScroll(orientation, wParam, pos, control);
    }

    wxScrollWinEvent event;
    event.SetPosition(pos);
    event.SetOrientation(orientation);
    event.m_eventObject = this;

    switch ( wParam )
    {
    case SB_TOP:
        event.m_eventType = wxEVT_SCROLLWIN_TOP;
        break;

    case SB_BOTTOM:
        event.m_eventType = wxEVT_SCROLLWIN_BOTTOM;
        break;

    case SB_LINEUP:
        event.m_eventType = wxEVT_SCROLLWIN_LINEUP;
        break;

    case SB_LINEDOWN:
        event.m_eventType = wxEVT_SCROLLWIN_LINEDOWN;
        break;

    case SB_PAGEUP:
        event.m_eventType = wxEVT_SCROLLWIN_PAGEUP;
        break;

    case SB_PAGEDOWN:
        event.m_eventType = wxEVT_SCROLLWIN_PAGEDOWN;
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
#ifdef __WIN32__
        // under Win32, the scrollbar range and position are 32 bit integers,
        // but WM_[HV]SCROLL only carry the low 16 bits of them, so we must
        // explicitly query the scrollbar for the correct position (this must
        // be done only for these two SB_ events as they are the only one
        // carrying the scrollbar position)
        {
            WinStruct<SCROLLINFO> scrollInfo;
            scrollInfo.fMask = SIF_TRACKPOS;

            if ( !::GetScrollInfo(GetHwnd(),
                                  orientation == wxHORIZONTAL ? SB_HORZ
                                                              : SB_VERT,
                                  &scrollInfo) )
            {
                wxLogLastError(_T("GetScrollInfo"));
            }

            event.SetPosition(scrollInfo.nTrackPos);
        }
#endif // Win32

        event.m_eventType = wParam == SB_THUMBPOSITION
                                ? wxEVT_SCROLLWIN_THUMBRELEASE
                                : wxEVT_SCROLLWIN_THUMBTRACK;
        break;

    default:
        return FALSE;
    }

    return GetEventHandler()->ProcessEvent(event);
}

// ===========================================================================
// global functions
// ===========================================================================

void wxGetCharSize(WXHWND wnd, int *x, int *y, const wxFont *the_font)
{
    TEXTMETRIC tm;
    HDC dc = ::GetDC((HWND) wnd);
    HFONT fnt =0;
    HFONT was = 0;
    if ( the_font )
    {
        //    the_font->UseResource();
        //    the_font->RealizeResource();
        fnt = (HFONT)((wxFont *)the_font)->GetResourceHandle(); // const_cast
        if ( fnt )
            was = (HFONT) SelectObject(dc,fnt);
    }
    GetTextMetrics(dc, &tm);
    if ( the_font && fnt && was )
    {
        SelectObject(dc,was);
    }
    ReleaseDC((HWND)wnd, dc);

    if ( x )
        *x = tm.tmAveCharWidth;
    if ( y )
        *y = tm.tmHeight + tm.tmExternalLeading;

    //  if ( the_font )
    //    the_font->ReleaseResource();
}

// Returns 0 if was a normal ASCII value, not a special key. This indicates that
// the key should be ignored by WM_KEYDOWN and processed by WM_CHAR instead.
int wxCharCodeMSWToWX(int keySym)
{
    int id;
    switch (keySym)
    {
        case VK_CANCEL:     id = WXK_CANCEL; break;
        case VK_BACK:       id = WXK_BACK; break;
        case VK_TAB:        id = WXK_TAB; break;
        case VK_CLEAR:      id = WXK_CLEAR; break;
        case VK_RETURN:     id = WXK_RETURN; break;
        case VK_SHIFT:      id = WXK_SHIFT; break;
        case VK_CONTROL:    id = WXK_CONTROL; break;
        case VK_MENU :      id = WXK_MENU; break;
        case VK_PAUSE:      id = WXK_PAUSE; break;
        case VK_CAPITAL:    id = WXK_CAPITAL; break;
        case VK_SPACE:      id = WXK_SPACE; break;
        case VK_ESCAPE:     id = WXK_ESCAPE; break;
        case VK_PRIOR:      id = WXK_PRIOR; break;
        case VK_NEXT :      id = WXK_NEXT; break;
        case VK_END:        id = WXK_END; break;
        case VK_HOME :      id = WXK_HOME; break;
        case VK_LEFT :      id = WXK_LEFT; break;
        case VK_UP:         id = WXK_UP; break;
        case VK_RIGHT:      id = WXK_RIGHT; break;
        case VK_DOWN :      id = WXK_DOWN; break;
        case VK_SELECT:     id = WXK_SELECT; break;
        case VK_PRINT:      id = WXK_PRINT; break;
        case VK_EXECUTE:    id = WXK_EXECUTE; break;
        case VK_INSERT:     id = WXK_INSERT; break;
        case VK_DELETE:     id = WXK_DELETE; break;
        case VK_HELP :      id = WXK_HELP; break;
        case VK_NUMPAD0:    id = WXK_NUMPAD0; break;
        case VK_NUMPAD1:    id = WXK_NUMPAD1; break;
        case VK_NUMPAD2:    id = WXK_NUMPAD2; break;
        case VK_NUMPAD3:    id = WXK_NUMPAD3; break;
        case VK_NUMPAD4:    id = WXK_NUMPAD4; break;
        case VK_NUMPAD5:    id = WXK_NUMPAD5; break;
        case VK_NUMPAD6:    id = WXK_NUMPAD6; break;
        case VK_NUMPAD7:    id = WXK_NUMPAD7; break;
        case VK_NUMPAD8:    id = WXK_NUMPAD8; break;
        case VK_NUMPAD9:    id = WXK_NUMPAD9; break;
        case VK_MULTIPLY:   id = WXK_NUMPAD_MULTIPLY; break;
        case VK_ADD:        id = WXK_NUMPAD_ADD; break;
        case VK_SUBTRACT:   id = WXK_NUMPAD_SUBTRACT; break;
        case VK_DECIMAL:    id = WXK_NUMPAD_DECIMAL; break;
        case VK_DIVIDE:     id = WXK_NUMPAD_DIVIDE; break;
        case VK_F1:         id = WXK_F1; break;
        case VK_F2:         id = WXK_F2; break;
        case VK_F3:         id = WXK_F3; break;
        case VK_F4:         id = WXK_F4; break;
        case VK_F5:         id = WXK_F5; break;
        case VK_F6:         id = WXK_F6; break;
        case VK_F7:         id = WXK_F7; break;
        case VK_F8:         id = WXK_F8; break;
        case VK_F9:         id = WXK_F9; break;
        case VK_F10:        id = WXK_F10; break;
        case VK_F11:        id = WXK_F11; break;
        case VK_F12:        id = WXK_F12; break;
        case VK_F13:        id = WXK_F13; break;
        case VK_F14:        id = WXK_F14; break;
        case VK_F15:        id = WXK_F15; break;
        case VK_F16:        id = WXK_F16; break;
        case VK_F17:        id = WXK_F17; break;
        case VK_F18:        id = WXK_F18; break;
        case VK_F19:        id = WXK_F19; break;
        case VK_F20:        id = WXK_F20; break;
        case VK_F21:        id = WXK_F21; break;
        case VK_F22:        id = WXK_F22; break;
        case VK_F23:        id = WXK_F23; break;
        case VK_F24:        id = WXK_F24; break;
        case VK_NUMLOCK:    id = WXK_NUMLOCK; break;
        case VK_SCROLL:     id = WXK_SCROLL; break;

        case VK_OEM_1:      id = ';'; break;
        case VK_OEM_PLUS:   id = '+'; break;
        case VK_OEM_COMMA:  id = ','; break;
        case VK_OEM_MINUS:  id = '-'; break;
        case VK_OEM_PERIOD: id = '.'; break;
        case VK_OEM_2:      id = '/'; break;
        case VK_OEM_3:      id = '~'; break;
        case VK_OEM_4:      id = '['; break;
        case VK_OEM_5:      id = '\\'; break;
        case VK_OEM_6:      id = ']'; break;
        case VK_OEM_7:      id = '\''; break;

#ifdef VK_APPS
        case VK_LWIN:       id = WXK_WINDOWS_LEFT; break;
        case VK_RWIN:       id = WXK_WINDOWS_RIGHT; break;
        case VK_APPS:       id = WXK_WINDOWS_MENU; break;
#endif // VK_APPS defined

        default:
            id = 0;
    }

    return id;
}

int wxCharCodeWXToMSW(int id, bool *isVirtual)
{
    *isVirtual = TRUE;
    int keySym = 0;
    switch (id)
    {
    case WXK_CANCEL:    keySym = VK_CANCEL; break;
    case WXK_CLEAR:     keySym = VK_CLEAR; break;
    case WXK_SHIFT:     keySym = VK_SHIFT; break;
    case WXK_CONTROL:   keySym = VK_CONTROL; break;
    case WXK_MENU :     keySym = VK_MENU; break;
    case WXK_PAUSE:     keySym = VK_PAUSE; break;
    case WXK_PRIOR:     keySym = VK_PRIOR; break;
    case WXK_NEXT :     keySym = VK_NEXT; break;
    case WXK_END:       keySym = VK_END; break;
    case WXK_HOME :     keySym = VK_HOME; break;
    case WXK_LEFT :     keySym = VK_LEFT; break;
    case WXK_UP:        keySym = VK_UP; break;
    case WXK_RIGHT:     keySym = VK_RIGHT; break;
    case WXK_DOWN :     keySym = VK_DOWN; break;
    case WXK_SELECT:    keySym = VK_SELECT; break;
    case WXK_PRINT:     keySym = VK_PRINT; break;
    case WXK_EXECUTE:   keySym = VK_EXECUTE; break;
    case WXK_INSERT:    keySym = VK_INSERT; break;
    case WXK_DELETE:    keySym = VK_DELETE; break;
    case WXK_HELP :     keySym = VK_HELP; break;
    case WXK_NUMPAD0:   keySym = VK_NUMPAD0; break;
    case WXK_NUMPAD1:   keySym = VK_NUMPAD1; break;
    case WXK_NUMPAD2:   keySym = VK_NUMPAD2; break;
    case WXK_NUMPAD3:   keySym = VK_NUMPAD3; break;
    case WXK_NUMPAD4:   keySym = VK_NUMPAD4; break;
    case WXK_NUMPAD5:   keySym = VK_NUMPAD5; break;
    case WXK_NUMPAD6:   keySym = VK_NUMPAD6; break;
    case WXK_NUMPAD7:   keySym = VK_NUMPAD7; break;
    case WXK_NUMPAD8:   keySym = VK_NUMPAD8; break;
    case WXK_NUMPAD9:   keySym = VK_NUMPAD9; break;
    case WXK_NUMPAD_MULTIPLY:  keySym = VK_MULTIPLY; break;
    case WXK_NUMPAD_ADD:       keySym = VK_ADD; break;
    case WXK_NUMPAD_SUBTRACT:  keySym = VK_SUBTRACT; break;
    case WXK_NUMPAD_DECIMAL:   keySym = VK_DECIMAL; break;
    case WXK_NUMPAD_DIVIDE:    keySym = VK_DIVIDE; break;
    case WXK_F1:        keySym = VK_F1; break;
    case WXK_F2:        keySym = VK_F2; break;
    case WXK_F3:        keySym = VK_F3; break;
    case WXK_F4:        keySym = VK_F4; break;
    case WXK_F5:        keySym = VK_F5; break;
    case WXK_F6:        keySym = VK_F6; break;
    case WXK_F7:        keySym = VK_F7; break;
    case WXK_F8:        keySym = VK_F8; break;
    case WXK_F9:        keySym = VK_F9; break;
    case WXK_F10:       keySym = VK_F10; break;
    case WXK_F11:       keySym = VK_F11; break;
    case WXK_F12:       keySym = VK_F12; break;
    case WXK_F13:       keySym = VK_F13; break;
    case WXK_F14:       keySym = VK_F14; break;
    case WXK_F15:       keySym = VK_F15; break;
    case WXK_F16:       keySym = VK_F16; break;
    case WXK_F17:       keySym = VK_F17; break;
    case WXK_F18:       keySym = VK_F18; break;
    case WXK_F19:       keySym = VK_F19; break;
    case WXK_F20:       keySym = VK_F20; break;
    case WXK_F21:       keySym = VK_F21; break;
    case WXK_F22:       keySym = VK_F22; break;
    case WXK_F23:       keySym = VK_F23; break;
    case WXK_F24:       keySym = VK_F24; break;
    case WXK_NUMLOCK:   keySym = VK_NUMLOCK; break;
    case WXK_SCROLL:    keySym = VK_SCROLL; break;
    default:
        {
            *isVirtual = FALSE;
            keySym = id;
            break;
        }
    }
    return keySym;
}

wxWindow *wxGetActiveWindow()
{
    HWND hWnd = GetActiveWindow();
    if ( hWnd != 0 )
    {
        return wxFindWinFromHandle((WXHWND) hWnd);
    }
    return NULL;
}

extern wxWindow *wxGetWindowFromHWND(WXHWND hWnd)
{
    HWND hwnd = (HWND)hWnd;

    // For a radiobutton, we get the radiobox from GWL_USERDATA (which is set
    // by code in msw/radiobox.cpp), for all the others we just search up the
    // window hierarchy
    wxWindow *win = (wxWindow *)NULL;
    if ( hwnd )
    {
        win = wxFindWinFromHandle((WXHWND)hwnd);
        if ( !win )
        {
            // all these hacks only work under Win32 anyhow
#ifdef __WIN32__

#if wxUSE_RADIOBOX
            // native radiobuttons return DLGC_RADIOBUTTON here and for any
            // wxWindow class which overrides WM_GETDLGCODE processing to
            // do it as well, win would be already non NULL
            if ( ::SendMessage(hwnd, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON )
            {
                win = (wxWindow *)::GetWindowLong(hwnd, GWL_USERDATA);
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

#endif // Win32
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
#ifndef __WXMICROWIN__
        if ( ::GetWindow(hwnd, GW_OWNER) )
        {
            // it's a dialog box, don't go upwards
            break;
        }
#endif

        hwnd = ::GetParent(hwnd);
        win = wxFindWinFromHandle((WXHWND)hwnd);
    }

    return win;
}

#ifndef __WXMICROWIN__

// Windows keyboard hook. Allows interception of e.g. F1, ESCAPE
// in active frames and dialogs, regardless of where the focus is.
static HHOOK wxTheKeyboardHook = 0;
static FARPROC wxTheKeyboardHookProc = 0;
int APIENTRY _EXPORT
wxKeyboardHook(int nCode, WORD wParam, DWORD lParam);

void wxSetKeyboardHook(bool doIt)
{
    if ( doIt )
    {
        wxTheKeyboardHookProc = MakeProcInstance((FARPROC) wxKeyboardHook, wxGetInstance());
        wxTheKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC) wxTheKeyboardHookProc, wxGetInstance(),

#if defined(__WIN32__)
            GetCurrentThreadId()
        //      (DWORD)GetCurrentProcess()); // This is another possibility. Which is right?
#else
            GetCurrentTask()
#endif
            );
    }
    else
    {
        UnhookWindowsHookEx(wxTheKeyboardHook);

        // avoids warning about statement with no effect (FreeProcInstance
        // doesn't do anything under Win32)
#if !defined(__WIN32__) && !defined(__NT__)
        FreeProcInstance(wxTheKeyboardHookProc);
#endif
    }
}

int APIENTRY _EXPORT
wxKeyboardHook(int nCode, WORD wParam, DWORD lParam)
{
    DWORD hiWord = HIWORD(lParam);
    if ( nCode != HC_NOREMOVE && ((hiWord & KF_UP) == 0) )
    {
        int id = wxCharCodeMSWToWX(wParam);
        if ( id != 0 )
        {
            wxKeyEvent event(wxEVT_CHAR_HOOK);
            if ( (HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN )
                event.m_altDown = TRUE;

            event.m_eventObject = NULL;
            event.m_keyCode = id;
            event.m_shiftDown = wxIsShiftDown();
            event.m_controlDown = wxIsCtrlDown();
            event.SetTimestamp(s_currentMsg.time);

            wxWindow *win = wxGetActiveWindow();
            wxEvtHandler *handler;
            if ( win )
            {
                handler = win->GetEventHandler();
                event.SetId(win->GetId());
            }
            else
            {
                handler = wxTheApp;
                event.SetId(-1);
            }

            if ( handler && handler->ProcessEvent(event) )
            {
                // processed
                return 1;
            }
        }
    }

    return (int)CallNextHookEx(wxTheKeyboardHook, nCode, wParam, lParam);
}

#endif // !__WXMICROWIN__

#ifdef __WXDEBUG__
const char *wxGetMessageName(int message)
{
    switch ( message )
    {
        case 0x0000: return "WM_NULL";
        case 0x0001: return "WM_CREATE";
        case 0x0002: return "WM_DESTROY";
        case 0x0003: return "WM_MOVE";
        case 0x0005: return "WM_SIZE";
        case 0x0006: return "WM_ACTIVATE";
        case 0x0007: return "WM_SETFOCUS";
        case 0x0008: return "WM_KILLFOCUS";
        case 0x000A: return "WM_ENABLE";
        case 0x000B: return "WM_SETREDRAW";
        case 0x000C: return "WM_SETTEXT";
        case 0x000D: return "WM_GETTEXT";
        case 0x000E: return "WM_GETTEXTLENGTH";
        case 0x000F: return "WM_PAINT";
        case 0x0010: return "WM_CLOSE";
        case 0x0011: return "WM_QUERYENDSESSION";
        case 0x0012: return "WM_QUIT";
        case 0x0013: return "WM_QUERYOPEN";
        case 0x0014: return "WM_ERASEBKGND";
        case 0x0015: return "WM_SYSCOLORCHANGE";
        case 0x0016: return "WM_ENDSESSION";
        case 0x0017: return "WM_SYSTEMERROR";
        case 0x0018: return "WM_SHOWWINDOW";
        case 0x0019: return "WM_CTLCOLOR";
        case 0x001A: return "WM_WININICHANGE";
        case 0x001B: return "WM_DEVMODECHANGE";
        case 0x001C: return "WM_ACTIVATEAPP";
        case 0x001D: return "WM_FONTCHANGE";
        case 0x001E: return "WM_TIMECHANGE";
        case 0x001F: return "WM_CANCELMODE";
        case 0x0020: return "WM_SETCURSOR";
        case 0x0021: return "WM_MOUSEACTIVATE";
        case 0x0022: return "WM_CHILDACTIVATE";
        case 0x0023: return "WM_QUEUESYNC";
        case 0x0024: return "WM_GETMINMAXINFO";
        case 0x0026: return "WM_PAINTICON";
        case 0x0027: return "WM_ICONERASEBKGND";
        case 0x0028: return "WM_NEXTDLGCTL";
        case 0x002A: return "WM_SPOOLERSTATUS";
        case 0x002B: return "WM_DRAWITEM";
        case 0x002C: return "WM_MEASUREITEM";
        case 0x002D: return "WM_DELETEITEM";
        case 0x002E: return "WM_VKEYTOITEM";
        case 0x002F: return "WM_CHARTOITEM";
        case 0x0030: return "WM_SETFONT";
        case 0x0031: return "WM_GETFONT";
        case 0x0037: return "WM_QUERYDRAGICON";
        case 0x0039: return "WM_COMPAREITEM";
        case 0x0041: return "WM_COMPACTING";
        case 0x0044: return "WM_COMMNOTIFY";
        case 0x0046: return "WM_WINDOWPOSCHANGING";
        case 0x0047: return "WM_WINDOWPOSCHANGED";
        case 0x0048: return "WM_POWER";

#ifdef  __WIN32__
        case 0x004A: return "WM_COPYDATA";
        case 0x004B: return "WM_CANCELJOURNAL";
        case 0x004E: return "WM_NOTIFY";
        case 0x0050: return "WM_INPUTLANGCHANGEREQUEST";
        case 0x0051: return "WM_INPUTLANGCHANGE";
        case 0x0052: return "WM_TCARD";
        case 0x0053: return "WM_HELP";
        case 0x0054: return "WM_USERCHANGED";
        case 0x0055: return "WM_NOTIFYFORMAT";
        case 0x007B: return "WM_CONTEXTMENU";
        case 0x007C: return "WM_STYLECHANGING";
        case 0x007D: return "WM_STYLECHANGED";
        case 0x007E: return "WM_DISPLAYCHANGE";
        case 0x007F: return "WM_GETICON";
        case 0x0080: return "WM_SETICON";
#endif  //WIN32

        case 0x0081: return "WM_NCCREATE";
        case 0x0082: return "WM_NCDESTROY";
        case 0x0083: return "WM_NCCALCSIZE";
        case 0x0084: return "WM_NCHITTEST";
        case 0x0085: return "WM_NCPAINT";
        case 0x0086: return "WM_NCACTIVATE";
        case 0x0087: return "WM_GETDLGCODE";
        case 0x00A0: return "WM_NCMOUSEMOVE";
        case 0x00A1: return "WM_NCLBUTTONDOWN";
        case 0x00A2: return "WM_NCLBUTTONUP";
        case 0x00A3: return "WM_NCLBUTTONDBLCLK";
        case 0x00A4: return "WM_NCRBUTTONDOWN";
        case 0x00A5: return "WM_NCRBUTTONUP";
        case 0x00A6: return "WM_NCRBUTTONDBLCLK";
        case 0x00A7: return "WM_NCMBUTTONDOWN";
        case 0x00A8: return "WM_NCMBUTTONUP";
        case 0x00A9: return "WM_NCMBUTTONDBLCLK";
        case 0x0100: return "WM_KEYDOWN";
        case 0x0101: return "WM_KEYUP";
        case 0x0102: return "WM_CHAR";
        case 0x0103: return "WM_DEADCHAR";
        case 0x0104: return "WM_SYSKEYDOWN";
        case 0x0105: return "WM_SYSKEYUP";
        case 0x0106: return "WM_SYSCHAR";
        case 0x0107: return "WM_SYSDEADCHAR";
        case 0x0108: return "WM_KEYLAST";

#ifdef  __WIN32__
        case 0x010D: return "WM_IME_STARTCOMPOSITION";
        case 0x010E: return "WM_IME_ENDCOMPOSITION";
        case 0x010F: return "WM_IME_COMPOSITION";
#endif  //WIN32

        case 0x0110: return "WM_INITDIALOG";
        case 0x0111: return "WM_COMMAND";
        case 0x0112: return "WM_SYSCOMMAND";
        case 0x0113: return "WM_TIMER";
        case 0x0114: return "WM_HSCROLL";
        case 0x0115: return "WM_VSCROLL";
        case 0x0116: return "WM_INITMENU";
        case 0x0117: return "WM_INITMENUPOPUP";
        case 0x011F: return "WM_MENUSELECT";
        case 0x0120: return "WM_MENUCHAR";
        case 0x0121: return "WM_ENTERIDLE";
        case 0x0200: return "WM_MOUSEMOVE";
        case 0x0201: return "WM_LBUTTONDOWN";
        case 0x0202: return "WM_LBUTTONUP";
        case 0x0203: return "WM_LBUTTONDBLCLK";
        case 0x0204: return "WM_RBUTTONDOWN";
        case 0x0205: return "WM_RBUTTONUP";
        case 0x0206: return "WM_RBUTTONDBLCLK";
        case 0x0207: return "WM_MBUTTONDOWN";
        case 0x0208: return "WM_MBUTTONUP";
        case 0x0209: return "WM_MBUTTONDBLCLK";
        case 0x020A: return "WM_MOUSEWHEEL";
        case 0x0210: return "WM_PARENTNOTIFY";
        case 0x0211: return "WM_ENTERMENULOOP";
        case 0x0212: return "WM_EXITMENULOOP";

#ifdef  __WIN32__
        case 0x0213: return "WM_NEXTMENU";
        case 0x0214: return "WM_SIZING";
        case 0x0215: return "WM_CAPTURECHANGED";
        case 0x0216: return "WM_MOVING";
        case 0x0218: return "WM_POWERBROADCAST";
        case 0x0219: return "WM_DEVICECHANGE";
#endif  //WIN32

        case 0x0220: return "WM_MDICREATE";
        case 0x0221: return "WM_MDIDESTROY";
        case 0x0222: return "WM_MDIACTIVATE";
        case 0x0223: return "WM_MDIRESTORE";
        case 0x0224: return "WM_MDINEXT";
        case 0x0225: return "WM_MDIMAXIMIZE";
        case 0x0226: return "WM_MDITILE";
        case 0x0227: return "WM_MDICASCADE";
        case 0x0228: return "WM_MDIICONARRANGE";
        case 0x0229: return "WM_MDIGETACTIVE";
        case 0x0230: return "WM_MDISETMENU";
        case 0x0233: return "WM_DROPFILES";

#ifdef  __WIN32__
        case 0x0281: return "WM_IME_SETCONTEXT";
        case 0x0282: return "WM_IME_NOTIFY";
        case 0x0283: return "WM_IME_CONTROL";
        case 0x0284: return "WM_IME_COMPOSITIONFULL";
        case 0x0285: return "WM_IME_SELECT";
        case 0x0286: return "WM_IME_CHAR";
        case 0x0290: return "WM_IME_KEYDOWN";
        case 0x0291: return "WM_IME_KEYUP";
#endif  //WIN32

        case 0x0300: return "WM_CUT";
        case 0x0301: return "WM_COPY";
        case 0x0302: return "WM_PASTE";
        case 0x0303: return "WM_CLEAR";
        case 0x0304: return "WM_UNDO";
        case 0x0305: return "WM_RENDERFORMAT";
        case 0x0306: return "WM_RENDERALLFORMATS";
        case 0x0307: return "WM_DESTROYCLIPBOARD";
        case 0x0308: return "WM_DRAWCLIPBOARD";
        case 0x0309: return "WM_PAINTCLIPBOARD";
        case 0x030A: return "WM_VSCROLLCLIPBOARD";
        case 0x030B: return "WM_SIZECLIPBOARD";
        case 0x030C: return "WM_ASKCBFORMATNAME";
        case 0x030D: return "WM_CHANGECBCHAIN";
        case 0x030E: return "WM_HSCROLLCLIPBOARD";
        case 0x030F: return "WM_QUERYNEWPALETTE";
        case 0x0310: return "WM_PALETTEISCHANGING";
        case 0x0311: return "WM_PALETTECHANGED";

#ifdef __WIN32__
        // common controls messages - although they're not strictly speaking
        // standard, it's nice to decode them nevertheless

        // listview
        case 0x1000 + 0: return "LVM_GETBKCOLOR";
        case 0x1000 + 1: return "LVM_SETBKCOLOR";
        case 0x1000 + 2: return "LVM_GETIMAGELIST";
        case 0x1000 + 3: return "LVM_SETIMAGELIST";
        case 0x1000 + 4: return "LVM_GETITEMCOUNT";
        case 0x1000 + 5: return "LVM_GETITEMA";
        case 0x1000 + 75: return "LVM_GETITEMW";
        case 0x1000 + 6: return "LVM_SETITEMA";
        case 0x1000 + 76: return "LVM_SETITEMW";
        case 0x1000 + 7: return "LVM_INSERTITEMA";
        case 0x1000 + 77: return "LVM_INSERTITEMW";
        case 0x1000 + 8: return "LVM_DELETEITEM";
        case 0x1000 + 9: return "LVM_DELETEALLITEMS";
        case 0x1000 + 10: return "LVM_GETCALLBACKMASK";
        case 0x1000 + 11: return "LVM_SETCALLBACKMASK";
        case 0x1000 + 12: return "LVM_GETNEXTITEM";
        case 0x1000 + 13: return "LVM_FINDITEMA";
        case 0x1000 + 83: return "LVM_FINDITEMW";
        case 0x1000 + 14: return "LVM_GETITEMRECT";
        case 0x1000 + 15: return "LVM_SETITEMPOSITION";
        case 0x1000 + 16: return "LVM_GETITEMPOSITION";
        case 0x1000 + 17: return "LVM_GETSTRINGWIDTHA";
        case 0x1000 + 87: return "LVM_GETSTRINGWIDTHW";
        case 0x1000 + 18: return "LVM_HITTEST";
        case 0x1000 + 19: return "LVM_ENSUREVISIBLE";
        case 0x1000 + 20: return "LVM_SCROLL";
        case 0x1000 + 21: return "LVM_REDRAWITEMS";
        case 0x1000 + 22: return "LVM_ARRANGE";
        case 0x1000 + 23: return "LVM_EDITLABELA";
        case 0x1000 + 118: return "LVM_EDITLABELW";
        case 0x1000 + 24: return "LVM_GETEDITCONTROL";
        case 0x1000 + 25: return "LVM_GETCOLUMNA";
        case 0x1000 + 95: return "LVM_GETCOLUMNW";
        case 0x1000 + 26: return "LVM_SETCOLUMNA";
        case 0x1000 + 96: return "LVM_SETCOLUMNW";
        case 0x1000 + 27: return "LVM_INSERTCOLUMNA";
        case 0x1000 + 97: return "LVM_INSERTCOLUMNW";
        case 0x1000 + 28: return "LVM_DELETECOLUMN";
        case 0x1000 + 29: return "LVM_GETCOLUMNWIDTH";
        case 0x1000 + 30: return "LVM_SETCOLUMNWIDTH";
        case 0x1000 + 31: return "LVM_GETHEADER";
        case 0x1000 + 33: return "LVM_CREATEDRAGIMAGE";
        case 0x1000 + 34: return "LVM_GETVIEWRECT";
        case 0x1000 + 35: return "LVM_GETTEXTCOLOR";
        case 0x1000 + 36: return "LVM_SETTEXTCOLOR";
        case 0x1000 + 37: return "LVM_GETTEXTBKCOLOR";
        case 0x1000 + 38: return "LVM_SETTEXTBKCOLOR";
        case 0x1000 + 39: return "LVM_GETTOPINDEX";
        case 0x1000 + 40: return "LVM_GETCOUNTPERPAGE";
        case 0x1000 + 41: return "LVM_GETORIGIN";
        case 0x1000 + 42: return "LVM_UPDATE";
        case 0x1000 + 43: return "LVM_SETITEMSTATE";
        case 0x1000 + 44: return "LVM_GETITEMSTATE";
        case 0x1000 + 45: return "LVM_GETITEMTEXTA";
        case 0x1000 + 115: return "LVM_GETITEMTEXTW";
        case 0x1000 + 46: return "LVM_SETITEMTEXTA";
        case 0x1000 + 116: return "LVM_SETITEMTEXTW";
        case 0x1000 + 47: return "LVM_SETITEMCOUNT";
        case 0x1000 + 48: return "LVM_SORTITEMS";
        case 0x1000 + 49: return "LVM_SETITEMPOSITION32";
        case 0x1000 + 50: return "LVM_GETSELECTEDCOUNT";
        case 0x1000 + 51: return "LVM_GETITEMSPACING";
        case 0x1000 + 52: return "LVM_GETISEARCHSTRINGA";
        case 0x1000 + 117: return "LVM_GETISEARCHSTRINGW";
        case 0x1000 + 53: return "LVM_SETICONSPACING";
        case 0x1000 + 54: return "LVM_SETEXTENDEDLISTVIEWSTYLE";
        case 0x1000 + 55: return "LVM_GETEXTENDEDLISTVIEWSTYLE";
        case 0x1000 + 56: return "LVM_GETSUBITEMRECT";
        case 0x1000 + 57: return "LVM_SUBITEMHITTEST";
        case 0x1000 + 58: return "LVM_SETCOLUMNORDERARRAY";
        case 0x1000 + 59: return "LVM_GETCOLUMNORDERARRAY";
        case 0x1000 + 60: return "LVM_SETHOTITEM";
        case 0x1000 + 61: return "LVM_GETHOTITEM";
        case 0x1000 + 62: return "LVM_SETHOTCURSOR";
        case 0x1000 + 63: return "LVM_GETHOTCURSOR";
        case 0x1000 + 64: return "LVM_APPROXIMATEVIEWRECT";
        case 0x1000 + 65: return "LVM_SETWORKAREA";

        // tree view
        case 0x1100 + 0: return "TVM_INSERTITEMA";
        case 0x1100 + 50: return "TVM_INSERTITEMW";
        case 0x1100 + 1: return "TVM_DELETEITEM";
        case 0x1100 + 2: return "TVM_EXPAND";
        case 0x1100 + 4: return "TVM_GETITEMRECT";
        case 0x1100 + 5: return "TVM_GETCOUNT";
        case 0x1100 + 6: return "TVM_GETINDENT";
        case 0x1100 + 7: return "TVM_SETINDENT";
        case 0x1100 + 8: return "TVM_GETIMAGELIST";
        case 0x1100 + 9: return "TVM_SETIMAGELIST";
        case 0x1100 + 10: return "TVM_GETNEXTITEM";
        case 0x1100 + 11: return "TVM_SELECTITEM";
        case 0x1100 + 12: return "TVM_GETITEMA";
        case 0x1100 + 62: return "TVM_GETITEMW";
        case 0x1100 + 13: return "TVM_SETITEMA";
        case 0x1100 + 63: return "TVM_SETITEMW";
        case 0x1100 + 14: return "TVM_EDITLABELA";
        case 0x1100 + 65: return "TVM_EDITLABELW";
        case 0x1100 + 15: return "TVM_GETEDITCONTROL";
        case 0x1100 + 16: return "TVM_GETVISIBLECOUNT";
        case 0x1100 + 17: return "TVM_HITTEST";
        case 0x1100 + 18: return "TVM_CREATEDRAGIMAGE";
        case 0x1100 + 19: return "TVM_SORTCHILDREN";
        case 0x1100 + 20: return "TVM_ENSUREVISIBLE";
        case 0x1100 + 21: return "TVM_SORTCHILDRENCB";
        case 0x1100 + 22: return "TVM_ENDEDITLABELNOW";
        case 0x1100 + 23: return "TVM_GETISEARCHSTRINGA";
        case 0x1100 + 64: return "TVM_GETISEARCHSTRINGW";
        case 0x1100 + 24: return "TVM_SETTOOLTIPS";
        case 0x1100 + 25: return "TVM_GETTOOLTIPS";

        // header
        case 0x1200 + 0: return "HDM_GETITEMCOUNT";
        case 0x1200 + 1: return "HDM_INSERTITEMA";
        case 0x1200 + 10: return "HDM_INSERTITEMW";
        case 0x1200 + 2: return "HDM_DELETEITEM";
        case 0x1200 + 3: return "HDM_GETITEMA";
        case 0x1200 + 11: return "HDM_GETITEMW";
        case 0x1200 + 4: return "HDM_SETITEMA";
        case 0x1200 + 12: return "HDM_SETITEMW";
        case 0x1200 + 5: return "HDM_LAYOUT";
        case 0x1200 + 6: return "HDM_HITTEST";
        case 0x1200 + 7: return "HDM_GETITEMRECT";
        case 0x1200 + 8: return "HDM_SETIMAGELIST";
        case 0x1200 + 9: return "HDM_GETIMAGELIST";
        case 0x1200 + 15: return "HDM_ORDERTOINDEX";
        case 0x1200 + 16: return "HDM_CREATEDRAGIMAGE";
        case 0x1200 + 17: return "HDM_GETORDERARRAY";
        case 0x1200 + 18: return "HDM_SETORDERARRAY";
        case 0x1200 + 19: return "HDM_SETHOTDIVIDER";

        // tab control
        case 0x1300 + 2: return "TCM_GETIMAGELIST";
        case 0x1300 + 3: return "TCM_SETIMAGELIST";
        case 0x1300 + 4: return "TCM_GETITEMCOUNT";
        case 0x1300 + 5: return "TCM_GETITEMA";
        case 0x1300 + 60: return "TCM_GETITEMW";
        case 0x1300 + 6: return "TCM_SETITEMA";
        case 0x1300 + 61: return "TCM_SETITEMW";
        case 0x1300 + 7: return "TCM_INSERTITEMA";
        case 0x1300 + 62: return "TCM_INSERTITEMW";
        case 0x1300 + 8: return "TCM_DELETEITEM";
        case 0x1300 + 9: return "TCM_DELETEALLITEMS";
        case 0x1300 + 10: return "TCM_GETITEMRECT";
        case 0x1300 + 11: return "TCM_GETCURSEL";
        case 0x1300 + 12: return "TCM_SETCURSEL";
        case 0x1300 + 13: return "TCM_HITTEST";
        case 0x1300 + 14: return "TCM_SETITEMEXTRA";
        case 0x1300 + 40: return "TCM_ADJUSTRECT";
        case 0x1300 + 41: return "TCM_SETITEMSIZE";
        case 0x1300 + 42: return "TCM_REMOVEIMAGE";
        case 0x1300 + 43: return "TCM_SETPADDING";
        case 0x1300 + 44: return "TCM_GETROWCOUNT";
        case 0x1300 + 45: return "TCM_GETTOOLTIPS";
        case 0x1300 + 46: return "TCM_SETTOOLTIPS";
        case 0x1300 + 47: return "TCM_GETCURFOCUS";
        case 0x1300 + 48: return "TCM_SETCURFOCUS";
        case 0x1300 + 49: return "TCM_SETMINTABWIDTH";
        case 0x1300 + 50: return "TCM_DESELECTALL";

        // toolbar
        case WM_USER+1: return "TB_ENABLEBUTTON";
        case WM_USER+2: return "TB_CHECKBUTTON";
        case WM_USER+3: return "TB_PRESSBUTTON";
        case WM_USER+4: return "TB_HIDEBUTTON";
        case WM_USER+5: return "TB_INDETERMINATE";
        case WM_USER+9: return "TB_ISBUTTONENABLED";
        case WM_USER+10: return "TB_ISBUTTONCHECKED";
        case WM_USER+11: return "TB_ISBUTTONPRESSED";
        case WM_USER+12: return "TB_ISBUTTONHIDDEN";
        case WM_USER+13: return "TB_ISBUTTONINDETERMINATE";
        case WM_USER+17: return "TB_SETSTATE";
        case WM_USER+18: return "TB_GETSTATE";
        case WM_USER+19: return "TB_ADDBITMAP";
        case WM_USER+20: return "TB_ADDBUTTONS";
        case WM_USER+21: return "TB_INSERTBUTTON";
        case WM_USER+22: return "TB_DELETEBUTTON";
        case WM_USER+23: return "TB_GETBUTTON";
        case WM_USER+24: return "TB_BUTTONCOUNT";
        case WM_USER+25: return "TB_COMMANDTOINDEX";
        case WM_USER+26: return "TB_SAVERESTOREA";
        case WM_USER+76: return "TB_SAVERESTOREW";
        case WM_USER+27: return "TB_CUSTOMIZE";
        case WM_USER+28: return "TB_ADDSTRINGA";
        case WM_USER+77: return "TB_ADDSTRINGW";
        case WM_USER+29: return "TB_GETITEMRECT";
        case WM_USER+30: return "TB_BUTTONSTRUCTSIZE";
        case WM_USER+31: return "TB_SETBUTTONSIZE";
        case WM_USER+32: return "TB_SETBITMAPSIZE";
        case WM_USER+33: return "TB_AUTOSIZE";
        case WM_USER+35: return "TB_GETTOOLTIPS";
        case WM_USER+36: return "TB_SETTOOLTIPS";
        case WM_USER+37: return "TB_SETPARENT";
        case WM_USER+39: return "TB_SETROWS";
        case WM_USER+40: return "TB_GETROWS";
        case WM_USER+42: return "TB_SETCMDID";
        case WM_USER+43: return "TB_CHANGEBITMAP";
        case WM_USER+44: return "TB_GETBITMAP";
        case WM_USER+45: return "TB_GETBUTTONTEXTA";
        case WM_USER+75: return "TB_GETBUTTONTEXTW";
        case WM_USER+46: return "TB_REPLACEBITMAP";
        case WM_USER+47: return "TB_SETINDENT";
        case WM_USER+48: return "TB_SETIMAGELIST";
        case WM_USER+49: return "TB_GETIMAGELIST";
        case WM_USER+50: return "TB_LOADIMAGES";
        case WM_USER+51: return "TB_GETRECT";
        case WM_USER+52: return "TB_SETHOTIMAGELIST";
        case WM_USER+53: return "TB_GETHOTIMAGELIST";
        case WM_USER+54: return "TB_SETDISABLEDIMAGELIST";
        case WM_USER+55: return "TB_GETDISABLEDIMAGELIST";
        case WM_USER+56: return "TB_SETSTYLE";
        case WM_USER+57: return "TB_GETSTYLE";
        case WM_USER+58: return "TB_GETBUTTONSIZE";
        case WM_USER+59: return "TB_SETBUTTONWIDTH";
        case WM_USER+60: return "TB_SETMAXTEXTROWS";
        case WM_USER+61: return "TB_GETTEXTROWS";
        case WM_USER+41: return "TB_GETBITMAPFLAGS";

#endif //WIN32

        default:
            static char s_szBuf[128];
            sprintf(s_szBuf, "<unknown message = %d>", message);
            return s_szBuf;
    }
}
#endif //__WXDEBUG__

static void TranslateKbdEventToMouse(wxWindowMSW *win,
                                     int *x, int *y, WPARAM *flags)
{
    // construct the key mask
    WPARAM& fwKeys = *flags;

    fwKeys = MK_RBUTTON;
    if ( wxIsCtrlDown() )
        fwKeys |= MK_CONTROL;
    if ( wxIsShiftDown() )
        fwKeys |= MK_SHIFT;

    // simulate right mouse button click
    DWORD dwPos = ::GetMessagePos();
    *x = GET_X_LPARAM(dwPos);
    *y = GET_Y_LPARAM(dwPos);

    win->ScreenToClient(x, y);
}

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
    HWND hWndHit = ::WindowFromPoint(pt2);

    wxWindow* win = wxFindWinFromHandle((WXHWND) hWndHit) ;
    HWND hWnd = hWndHit;

    // Try to find a window with a wxWindow associated with it
    while (!win && (hWnd != 0))
    {
        hWnd = ::GetParent(hWnd);
        win = wxFindWinFromHandle((WXHWND) hWnd) ;
    }
    return win;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    POINT pt;
    GetCursorPos( & pt );

    return wxPoint(pt.x, pt.y);
}

