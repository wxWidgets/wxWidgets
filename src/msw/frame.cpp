/////////////////////////////////////////////////////////////////////////////
// Name:        msw/frame.cpp
// Purpose:     wxFrame
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "frame.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/mdi.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#if wxUSE_STATUSBAR
    #include "wx/statusbr.h"
    #include "wx/generic/statusbr.h"
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    #include "wx/toolbar.h"
#endif // wxUSE_TOOLBAR

#include "wx/menuitem.h"
#include "wx/log.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

extern wxWindowList wxModelessWindows;
extern wxList WXDLLEXPORT wxPendingDelete;
extern const wxChar *wxFrameClassName;
extern wxMenu *wxCurrentPopupMenu;

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
    EVT_ACTIVATE(wxFrame::OnActivate)
    EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static class members
// ----------------------------------------------------------------------------

#if wxUSE_NATIVE_STATUSBAR
    bool wxFrame::m_useNativeStatusBar = TRUE;
#else
    bool wxFrame::m_useNativeStatusBar = FALSE;
#endif

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
    m_iconized = FALSE;

#if wxUSE_TOOLTIPS
    m_hwndToolTip = 0;
#endif

    // Data to save/restore when calling ShowFullScreen
    m_fsStyle = 0;
    m_fsOldWindowStyle = 0;
    m_fsStatusBarFields = 0;
    m_fsStatusBarHeight = 0;
    m_fsToolBarHeight = 0;
//  m_fsMenu = 0;
    m_fsIsMaximized = FALSE;
    m_fsIsShowing = FALSE;

    m_winLastFocused = (wxWindow *)NULL;

    // unlike (almost?) all other windows, frames are created hidden
    m_isShown = FALSE;
}

bool wxFrame::Create(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
  SetName(name);
  m_windowStyle = style;
  m_frameMenuBar = NULL;
  m_frameToolBar = NULL;
  m_frameStatusBar = NULL;

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

  if ( id > -1 )
    m_windowId = id;
  else
    m_windowId = (int)NewControlId();

  if (parent) parent->AddChild(this);

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_iconized = FALSE;

  // we pass NULL as parent to MSWCreate because frames with parents behave
  // very strangely under Win95 shell
  // Alteration by JACS: keep normal Windows behaviour (float on top of parent)
  // with this style.
  if ((m_windowStyle & wxFRAME_FLOAT_ON_PARENT) == 0)
    parent = NULL;

  wxTopLevelWindows.Append(this);

  MSWCreate(m_windowId, parent, wxFrameClassName, this, title,
            x, y, width, height, style);

  wxModelessWindows.Append(this);

  return TRUE;
}

wxFrame::~wxFrame()
{
  m_isBeingDeleted = TRUE;
  wxTopLevelWindows.DeleteObject(this);

  // the ~wxToolBar() code relies on the previous line to be executed before
  // this one, i.e. the frame should remove itself from wxTopLevelWindows
  // before destorying its toolbar
  DeleteAllBars();

  if (wxTheApp && (wxTopLevelWindows.Number() == 0))
  {
    wxTheApp->SetTopWindow(NULL);

    if (wxTheApp->GetExitOnFrameDelete())
    {
       PostQuitMessage(0);
    }
  }

  wxModelessWindows.DeleteObject(this);

  // For some reason, wxWindows can activate another task altogether
  // when a frame is destroyed after a modal dialog has been invoked.
  // Try to bring the parent to the top.
  // MT:Only do this if this frame is currently the active window, else weird
  // things start to happen
  if ( wxGetActiveWindow() == this )
  if (GetParent() && GetParent()->GetHWND())
    ::BringWindowToTop((HWND) GetParent()->GetHWND());
}

// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
void wxFrame::DoGetClientSize(int *x, int *y) const
{
  RECT rect;
  ::GetClientRect(GetHwnd(), &rect);

#if wxUSE_STATUSBAR
  if ( GetStatusBar() && GetStatusBar()->IsShown() )
  {
    int statusX, statusY;
    GetStatusBar()->GetClientSize(&statusX, &statusY);
    rect.bottom -= statusY;
  }
#endif // wxUSE_STATUSBAR

  wxPoint pt(GetClientAreaOrigin());
  rect.bottom -= pt.y;
  rect.right -= pt.x;

  if ( x )
    *x = rect.right;
  if ( y )
    *y = rect.bottom;
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxFrame::DoSetClientSize(int width, int height)
{
  HWND hWnd = GetHwnd();

  RECT rect;
  ::GetClientRect(hWnd, &rect);

  RECT rect2;
  GetWindowRect(hWnd, &rect2);

  // Find the difference between the entire window (title bar and all)
  // and the client area; add this to the new client size to move the
  // window
  int actual_width = rect2.right - rect2.left - rect.right + width;
  int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

#if wxUSE_STATUSBAR
  if ( GetStatusBar() && GetStatusBar()->IsShown())
  {
    int statusX, statusY;
    GetStatusBar()->GetClientSize(&statusX, &statusY);
    actual_height += statusY;
  }
#endif // wxUSE_STATUSBAR

  wxPoint pt(GetClientAreaOrigin());
  actual_width += pt.y;
  actual_height += pt.x;

  POINT point;
  point.x = rect2.left;
  point.y = rect2.top;

  MoveWindow(hWnd, point.x, point.y, actual_width, actual_height, (BOOL)TRUE);

  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
}

void wxFrame::DoGetSize(int *width, int *height) const
{
  RECT rect;
  GetWindowRect(GetHwnd(), &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxFrame::DoGetPosition(int *x, int *y) const
{
  RECT rect;
  GetWindowRect(GetHwnd(), &rect);
  POINT point;
  point.x = rect.left;
  point.y = rect.top;

  *x = point.x;
  *y = point.y;
}

// ----------------------------------------------------------------------------
// variations around ::ShowWindow()
// ----------------------------------------------------------------------------

void wxFrame::DoShowWindow(int nShowCmd)
{
    ::ShowWindow(GetHwnd(), nShowCmd);

    m_iconized = nShowCmd == SW_MINIMIZE;
}

bool wxFrame::Show(bool show)
{
    // don't use wxWindow version as we want to call DoShowWindow()
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    DoShowWindow(show ? SW_SHOW : SW_HIDE);

    if ( show )
    {
        ::BringWindowToTop(GetHwnd());

        wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_windowId);
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent(event);
    }
    else
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

void wxFrame::Iconize(bool iconize)
{
    DoShowWindow(iconize ? SW_MINIMIZE : SW_RESTORE);
}

void wxFrame::Maximize(bool maximize)
{
    DoShowWindow(maximize ? SW_MAXIMIZE : SW_RESTORE);
}

void wxFrame::Restore()
{
    DoShowWindow(SW_RESTORE);
}

bool wxFrame::IsIconized() const
{
  ((wxFrame *)this)->m_iconized = (::IsIconic(GetHwnd()) != 0);
  return m_iconized;
}

// Is it maximized?
bool wxFrame::IsMaximized() const
{
    return (::IsZoomed(GetHwnd()) != 0);
}

void wxFrame::SetIcon(const wxIcon& icon)
{
    wxFrameBase::SetIcon(icon);

#if defined(__WIN95__)
    if ( m_icon.Ok() )
    {
        SendMessage(GetHwnd(), WM_SETICON,
                    (WPARAM)TRUE, (LPARAM)(HICON) m_icon.GetHICON());
    }
#endif // __WIN95__
}

#if wxUSE_STATUSBAR
wxStatusBar *wxFrame::OnCreateStatusBar(int number,
                                        long style,
                                        wxWindowID id,
                                        const wxString& name)
{
    wxStatusBar *statusBar = NULL;

#if wxUSE_NATIVE_STATUSBAR
    if ( !UsesNativeStatusBar() )
    {
        statusBar = (wxStatusBar *)new wxStatusBarGeneric(this, id, style);
    }
    else
#endif
    {
        statusBar = new wxStatusBar(this, id, style, name);
    }

    // Set the height according to the font and the border size
    wxClientDC dc(statusBar);
    dc.SetFont(statusBar->GetFont());

    wxCoord y;
    dc.GetTextExtent(_T("X"), NULL, &y );

    int height = (int)( (11*y)/10 + 2*statusBar->GetBorderY());

    statusBar->SetSize(-1, -1, -1, height);

    statusBar->SetFieldsCount(number);

    return statusBar;
}

void wxFrame::PositionStatusBar()
{
    if ( !m_frameStatusBar )
        return;

    int w, h;
    GetClientSize(&w, &h);
    int sw, sh;
    m_frameStatusBar->GetSize(&sw, &sh);

    // Since we wish the status bar to be directly under the client area,
    // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
    m_frameStatusBar->SetSize(0, h, w, sh);
}
#endif // wxUSE_STATUSBAR

void wxFrame::DetachMenuBar()
{
    if ( m_frameMenuBar )
    {
        m_frameMenuBar->Detach();
        m_frameMenuBar = NULL;
    }
}

void wxFrame::SetMenuBar(wxMenuBar *menubar)
{
    if ( !menubar )
    {
        DetachMenuBar();

        // actually remove the menu from the frame
        m_hMenu = (WXHMENU)0;
        InternalSetMenuBar();
    }
    else // set new non NULL menu bar
    {
        m_frameMenuBar = NULL;

        // Can set a menubar several times.
        // TODO: how to prevent a memory leak if you have a currently-unattached
        // menubar? wxWindows assumes that the frame will delete the menu (otherwise
        // there are problems for MDI).
        if ( menubar->GetHMenu() )
        {
            m_hMenu = menubar->GetHMenu();
        }
        else
        {
            menubar->Detach();

            m_hMenu = menubar->Create();

            if ( !m_hMenu )
                return;
        }

        InternalSetMenuBar();

        m_frameMenuBar = menubar;
        menubar->Attach(this);
    }
}

void wxFrame::InternalSetMenuBar()
{
    if ( !::SetMenu(GetHwnd(), (HMENU)m_hMenu) )
    {
        wxLogLastError(wxT("SetMenu"));
    }
}

// Responds to colour changes, and passes event on to children.
void wxFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
    Refresh();

    if ( m_frameStatusBar )
    {
        wxSysColourChangedEvent event2;
        event2.SetEventObject( m_frameStatusBar );
        m_frameStatusBar->GetEventHandler()->ProcessEvent(event2);
    }

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

// Pass TRUE to show full screen, FALSE to restore.
bool wxFrame::ShowFullScreen(bool show, long style)
{
    if (show)
    {
        if (IsFullScreen())
            return FALSE;

        m_fsIsShowing = TRUE;
        m_fsStyle = style;

        wxToolBar *theToolBar = GetToolBar();
        wxStatusBar *theStatusBar = GetStatusBar();

        int dummyWidth;

        if (theToolBar)
            theToolBar->GetSize(&dummyWidth, &m_fsToolBarHeight);
        if (theStatusBar)
            theStatusBar->GetSize(&dummyWidth, &m_fsStatusBarHeight);

        // zap the toolbar, menubar, and statusbar

        if ((style & wxFULLSCREEN_NOTOOLBAR) && theToolBar)
        {
            theToolBar->SetSize(-1,0);
            theToolBar->Show(FALSE);
        }

        if (style & wxFULLSCREEN_NOMENUBAR)
            SetMenu((HWND)GetHWND(), (HMENU) NULL);

        // Save the number of fields in the statusbar
        if ((style & wxFULLSCREEN_NOSTATUSBAR) && theStatusBar)
        {
            m_fsStatusBarFields = theStatusBar->GetFieldsCount();
            SetStatusBar((wxStatusBar*) NULL);
            delete theStatusBar;
        }
        else
            m_fsStatusBarFields = 0;

        // zap the frame borders

        // save the 'normal' window style
        m_fsOldWindowStyle = GetWindowLong((HWND)GetHWND(), GWL_STYLE);

        // save the old position, width & height, maximize state
        m_fsOldSize = GetRect();
        m_fsIsMaximized = IsMaximized();

        // decide which window style flags to turn off
        LONG newStyle = m_fsOldWindowStyle;
        LONG offFlags = 0;

        if (style & wxFULLSCREEN_NOBORDER)
            offFlags |= WS_BORDER;
        if (style & wxFULLSCREEN_NOCAPTION)
            offFlags |= (WS_CAPTION | WS_SYSMENU);

        newStyle &= (~offFlags);

        // change our window style to be compatible with full-screen mode
        SetWindowLong((HWND)GetHWND(), GWL_STYLE, newStyle);

        // resize to the size of the desktop
        int width, height;

        RECT rect;
        ::GetWindowRect(GetDesktopWindow(), &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

        SetSize(width, height);

        // now flush the window style cache and actually go full-screen
        SetWindowPos((HWND)GetHWND(), HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);

        wxSizeEvent event(wxSize(width, height), GetId());
        GetEventHandler()->ProcessEvent(event);

        return TRUE;
    }
    else
    {
        if (!IsFullScreen())
            return FALSE;

        m_fsIsShowing = FALSE;

        wxToolBar *theToolBar = GetToolBar();

        // restore the toolbar, menubar, and statusbar
        if (theToolBar && (m_fsStyle & wxFULLSCREEN_NOTOOLBAR))
        {
            theToolBar->SetSize(-1, m_fsToolBarHeight);
            theToolBar->Show(TRUE);
        }

        if ((m_fsStyle & wxFULLSCREEN_NOSTATUSBAR) && (m_fsStatusBarFields > 0))
        {
            CreateStatusBar(m_fsStatusBarFields);
            PositionStatusBar();
        }

        if ((m_fsStyle & wxFULLSCREEN_NOMENUBAR) && (m_hMenu != 0))
            SetMenu((HWND)GetHWND(), (HMENU)m_hMenu);

        Maximize(m_fsIsMaximized);
        SetWindowLong((HWND)GetHWND(),GWL_STYLE, m_fsOldWindowStyle);
        SetWindowPos((HWND)GetHWND(),HWND_TOP,m_fsOldSize.x, m_fsOldSize.y,
            m_fsOldSize.width, m_fsOldSize.height, SWP_FRAMECHANGED);

        return TRUE;
    }
}

/*
 * Frame window
 *
 */

bool wxFrame::MSWCreate(int id, wxWindow *parent, const wxChar *wclass, wxWindow *wx_win, const wxChar *title,
                   int x, int y, int width, int height, long style)

{
  m_defaultIcon = (WXHICON) (wxSTD_FRAME_ICON ? wxSTD_FRAME_ICON : wxDEFAULT_FRAME_ICON);

  // If child windows aren't properly drawn initially, WS_CLIPCHILDREN
  // could be the culprit. But without it, you can get a lot of flicker.

  DWORD msflags = 0;
  if ((style & wxCAPTION) == wxCAPTION)
    msflags = WS_OVERLAPPED;
  else
    msflags = WS_POPUP;

  if (style & wxMINIMIZE_BOX)
    msflags |= WS_MINIMIZEBOX;
  if (style & wxMAXIMIZE_BOX)
    msflags |= WS_MAXIMIZEBOX;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  if (style & wxSYSTEM_MENU)
    msflags |= WS_SYSMENU;
  if ( style & wxMINIMIZE )
    msflags |= WS_MINIMIZE;
  if (style & wxMAXIMIZE)
    msflags |= WS_MAXIMIZE;
  if (style & wxCAPTION)
    msflags |= WS_CAPTION;
  if (style & wxCLIP_CHILDREN)
    msflags |= WS_CLIPCHILDREN;

  // Keep this in wxFrame because it saves recoding this function
  // in wxTinyFrame
#if wxUSE_ITSY_BITSY && !defined(__WIN32__)
  if (style & wxTINY_CAPTION_VERT)
    msflags |= IBS_VERTCAPTION;
  if (style & wxTINY_CAPTION_HORIZ)
    msflags |= IBS_HORZCAPTION;
#else
  if (style & wxTINY_CAPTION_VERT)
    msflags |= WS_CAPTION;
  if (style & wxTINY_CAPTION_HORIZ)
    msflags |= WS_CAPTION;
#endif
  if ((style & wxTHICK_FRAME) == 0)
    msflags |= WS_BORDER;

  WXDWORD extendedStyle = MakeExtendedStyle(style);

#if !defined(__WIN16__) && !defined(__SC__)
  if (style & wxFRAME_TOOL_WINDOW)
    extendedStyle |= WS_EX_TOOLWINDOW;
#endif

  if (style & wxSTAY_ON_TOP)
    extendedStyle |= WS_EX_TOPMOST;

  m_iconized = FALSE;
  if ( !wxWindow::MSWCreate(id, parent, wclass, wx_win, title, x, y, width, height,
         msflags, NULL, extendedStyle) )
         return FALSE;

  // Seems to be necessary if we use WS_POPUP
  // style instead of WS_OVERLAPPED
  if (width > -1 && height > -1)
    ::PostMessage(GetHwnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(width, height));

  return TRUE;
}

// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxFrame::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() )
    {
        // restore focus to the child which was last focused
        wxLogTrace(_T("focus"), _T("wxFrame %08x activated."), m_hWnd);

        wxSetFocusToChild(this, &m_winLastFocused);
    }
    else
    {
        // remember the last focused child
        m_winLastFocused = FindFocus();
        while ( m_winLastFocused )
        {
            if ( GetChildren().Find(m_winLastFocused) )
                break;

            m_winLastFocused = m_winLastFocused->GetParent();
        }

        wxLogTrace(_T("focus"),
                   _T("wxFrame %08x deactivated, last focused: %08x."),
                   m_hWnd,
                   m_winLastFocused ? GetHwndOf(m_winLastFocused)
                                    : NULL);

        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// tool/status bar stuff
// ----------------------------------------------------------------------------

#if wxUSE_TOOLBAR

wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    if ( wxFrameBase::CreateToolBar(style, id, name) )
    {
        PositionToolBar();
    }

    return m_frameToolBar;
}

void wxFrame::PositionToolBar()
{
    RECT rect;
    ::GetClientRect(GetHwnd(), &rect);

#if wxUSE_STATUSBAR
    if ( GetStatusBar() )
    {
        int statusX, statusY;
        GetStatusBar()->GetClientSize(&statusX, &statusY);
        rect.bottom -= statusY;
    }
#endif // wxUSE_STATUSBAR

    if ( GetToolBar() && GetToolBar()->IsShown() )
    {
        int tw, th;
        GetToolBar()->GetSize(&tw, &th);

        if ( GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            th = rect.bottom;
        }
        else
        {
            tw = rect.right;
        }

        // Use the 'real' MSW position here
        GetToolBar()->SetSize(0, 0, tw, th, wxSIZE_NO_ADJUSTMENTS);
    }
}
#endif // wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// frame state (iconized/maximized/...)
// ----------------------------------------------------------------------------

// propagate our state change to all child frames: this allows us to emulate X
// Windows behaviour where child frames float independently of the parent one
// on the desktop, but are iconized/restored with it
void wxFrame::IconizeChildFrames(bool bIconize)
{
    for ( wxWindowList::Node *node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();

        // the child MDI frames are a special case and should not be touched by
        // the parent frame - instead, they are managed by the user
        wxFrame *frame = wxDynamicCast(win, wxFrame);
        if ( frame && !wxDynamicCast(frame, wxMDIChildFrame) )
        {
            frame->Iconize(bIconize);
        }
    }
}

// ===========================================================================
// message processing
// ===========================================================================

// ---------------------------------------------------------------------------
// preprocessing
// ---------------------------------------------------------------------------

bool wxFrame::MSWTranslateMessage(WXMSG* pMsg)
{
    if ( wxWindow::MSWTranslateMessage(pMsg) )
        return TRUE;

    // try the menu bar accels
    wxMenuBar *menuBar = GetMenuBar();
    if ( !menuBar )
        return FALSE;

    const wxAcceleratorTable& acceleratorTable = menuBar->GetAccelTable();
    return acceleratorTable.Translate(this, pMsg);
}

// ---------------------------------------------------------------------------
// our private (non virtual) message handlers
// ---------------------------------------------------------------------------

bool wxFrame::HandlePaint()
{
    RECT rect;
    if ( GetUpdateRect(GetHwnd(), &rect, FALSE) )
    {
        if ( m_iconized )
        {
            HICON hIcon = m_icon.Ok() ? GetHiconOf(m_icon)
                                      : (HICON)m_defaultIcon;

            // Hold a pointer to the dc so long as the OnPaint() message
            // is being processed
            PAINTSTRUCT ps;
            HDC hdc = ::BeginPaint(GetHwnd(), &ps);

            // Erase background before painting or we get white background
            MSWDefWindowProc(WM_ICONERASEBKGND, (WORD)(LONG)ps.hdc, 0L);

            if ( hIcon )
            {
                RECT rect;
                ::GetClientRect(GetHwnd(), &rect);

                // FIXME: why hardcoded?
                static const int icon_width = 32;
                static const int icon_height = 32;

                int icon_x = (int)((rect.right - icon_width)/2);
                int icon_y = (int)((rect.bottom - icon_height)/2);

                ::DrawIcon(hdc, icon_x, icon_y, hIcon);
            }

            ::EndPaint(GetHwnd(), &ps);

            return TRUE;
        }
        else
        {
            return wxWindow::HandlePaint();
        }
    }
    else
    {
        // nothing to paint - processed
        return TRUE;
    }
}

bool wxFrame::HandleSize(int x, int y, WXUINT id)
{
    bool processed = FALSE;

    switch ( id )
    {
        case SIZENORMAL:
            // only do it it if we were iconized before, otherwise resizing the
            // parent frame has a curious side effect of bringing it under it's
            // children
            if ( !m_iconized )
                break;

            // restore all child frames too
            IconizeChildFrames(FALSE);

            // fall through

        case SIZEFULLSCREEN:
            m_iconized = FALSE;
            break;

        case SIZEICONIC:
            // iconize all child frames too
            IconizeChildFrames(TRUE);

            m_iconized = TRUE;
            break;
    }

    if ( !m_iconized )
    {
        PositionStatusBar();
        PositionToolBar();

        wxSizeEvent event(wxSize(x, y), m_windowId);
        event.SetEventObject( this );
        processed = GetEventHandler()->ProcessEvent(event);
    }

    return processed;
}

bool wxFrame::HandleCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
    if ( control )
    {
        // In case it's e.g. a toolbar.
        wxWindow *win = wxFindWinFromHandle(control);
        if ( win )
            return win->MSWCommand(cmd, id);
    }

    // handle here commands from menus and accelerators
    if ( cmd == 0 || cmd == 1 )
    {
        if ( wxCurrentPopupMenu )
        {
            wxMenu *popupMenu = wxCurrentPopupMenu;
            wxCurrentPopupMenu = NULL;

            return popupMenu->MSWCommand(cmd, id);
        }

        if ( ProcessCommand(id) )
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool wxFrame::HandleMenuSelect(WXWORD nItem, WXWORD flags, WXHMENU hMenu)
{
    int item;
    if ( flags == 0xFFFF && hMenu == 0 )
    {
        // menu was removed from screen
        item = -1;
    }
    else if ( !(flags & MF_POPUP) && !(flags & MF_SEPARATOR) )
    {
        item = nItem;
    }
    else
    {
        // don't give hints for separators (doesn't make sense) nor for the
        // items opening popup menus (they don't have them anyhow) but do clear
        // the status line - otherwise, we would be left with the help message
        // for the previous item which doesn't apply any more
        wxStatusBar *statbar = GetStatusBar();
        if ( statbar )
        {
            statbar->SetStatusText(wxEmptyString);
        }

        return FALSE;
    }

    wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, item);
    event.SetEventObject( this );

    return GetEventHandler()->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// the window proc for wxFrame
// ---------------------------------------------------------------------------

long wxFrame::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    long rc = 0;
    bool processed = FALSE;

    switch ( message )
    {
        case WM_CLOSE:
            // if we can't close, tell the system that we processed the
            // message - otherwise it would close us
            processed = !Close();
            break;

        case WM_COMMAND:
            {
                WORD id, cmd;
                WXHWND hwnd;
                UnpackCommand((WXWPARAM)wParam, (WXLPARAM)lParam,
                              &id, &hwnd, &cmd);

                processed = HandleCommand(id, cmd, (WXHWND)hwnd);
            }
            break;

        case WM_MENUSELECT:
            {
                WXWORD item, flags;
                WXHMENU hmenu;
                UnpackMenuSelect(wParam, lParam, &item, &flags, &hmenu);

                processed = HandleMenuSelect(item, flags, hmenu);
            }
            break;

        case WM_PAINT:
            processed = HandlePaint();
            break;

        case WM_QUERYDRAGICON:
            {
                HICON hIcon = m_icon.Ok() ? GetHiconOf(m_icon)
                                          : (HICON)(m_defaultIcon);
                rc = (long)hIcon;
                processed = rc != 0;
            }
            break;

        case WM_SIZE:
            processed = HandleSize(LOWORD(lParam), HIWORD(lParam), wParam);
            break;
    }

    if ( !processed )
        rc = wxWindow::MSWWindowProc(message, wParam, lParam);

    return rc;
}

