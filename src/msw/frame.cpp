/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     wxFrame
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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
#endif // WX_PRECOMP

#include "wx/msw/private.h"
#include "wx/statusbr.h"
#include "wx/toolbar.h"
#include "wx/menuitem.h"
#include "wx/log.h"

#if wxUSE_NATIVE_STATUSBAR
    #include <wx/msw/statbr95.h>
#endif

extern wxWindowList wxModelessWindows;
extern wxList WXDLLEXPORT wxPendingDelete;
extern char wxFrameClassName[];
extern wxMenu *wxCurrentPopupMenu;

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxFrame, wxWindow)
  EVT_SIZE(wxFrame::OnSize)
  EVT_ACTIVATE(wxFrame::OnActivate)
  EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
  EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
  EVT_IDLE(wxFrame::OnIdle)
  EVT_CLOSE(wxFrame::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)
#endif

#if wxUSE_NATIVE_STATUSBAR
    bool wxFrame::m_useNativeStatusBar = TRUE;
#else
    bool wxFrame::m_useNativeStatusBar = FALSE;
#endif

wxFrame::wxFrame()
{
  m_frameToolBar = NULL ;
  m_frameMenuBar = NULL;
  m_frameStatusBar = NULL;

  m_iconized = FALSE;
}

bool wxFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
#if wxUSE_TOOLTIPS
    m_hwndToolTip = 0;
#endif

  if (!parent)
    wxTopLevelWindows.Append(this);

  SetName(name);
  m_windowStyle = style;
  m_frameMenuBar = NULL;
  m_frameToolBar = NULL ;
  m_frameStatusBar = NULL;

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

//  m_icon = NULL;
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

  MSWCreate(m_windowId, parent, wxFrameClassName, this, title,
            x, y, width, height, style);

  wxModelessWindows.Append(this);
  return TRUE;
}

wxFrame::~wxFrame()
{
  m_isBeingDeleted = TRUE;
  wxTopLevelWindows.DeleteObject(this);

  if (m_frameStatusBar)
    delete m_frameStatusBar;
  if (m_frameMenuBar)
    delete m_frameMenuBar;

/* New behaviour March 1998: check if it's the last top-level window */
//  if (wxTheApp && (this == wxTheApp->GetTopWindow()))

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
  if (GetParent() && GetParent()->GetHWND())
    ::BringWindowToTop((HWND) GetParent()->GetHWND());
}

// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
void wxFrame::DoGetClientSize(int *x, int *y) const
{
  RECT rect;
  ::GetClientRect(GetHwnd(), &rect);

  if ( GetStatusBar() )
  {
    int statusX, statusY;
    GetStatusBar()->GetClientSize(&statusX, &statusY);
    rect.bottom -= statusY;
  }

  wxPoint pt(GetClientAreaOrigin());
  rect.bottom -= pt.y;
  rect.right -= pt.x;

  *x = rect.right;
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

  if ( GetStatusBar() )
  {
    int statusX, statusY;
    GetStatusBar()->GetClientSize(&statusX, &statusY);
    actual_height += statusY;
  }

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

void wxFrame::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  int ww,hh ;
  GetSize(&ww,&hh) ;
  if (width == -1) w1 = ww ;
  if (height==-1) h1 = hh ;

  MoveWindow(GetHwnd(), x1, y1, w1, h1, (BOOL)TRUE);

  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
}

bool wxFrame::Show(bool show)
{
  int cshow;
  if (show)
    cshow = SW_SHOW;
  else
    cshow = SW_HIDE;

  if (!show)
  {
    // Try to highlight the correct window (the parent)
    HWND hWndParent = 0;
    if (GetParent())
    {
      hWndParent = (HWND) GetParent()->GetHWND();
      if (hWndParent)
        ::BringWindowToTop(hWndParent);
    }
  }

  ShowWindow(GetHwnd(), (BOOL)cshow);
  if (show)
  {
    BringWindowToTop(GetHwnd());

    wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_windowId);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
  }
  return TRUE;
}

void wxFrame::Iconize(bool iconize)
{
  if (!iconize)
    Show(TRUE);

  int cshow;
  if (iconize)
    cshow = SW_MINIMIZE;
  else
    cshow = SW_RESTORE;
  ShowWindow(GetHwnd(), (BOOL)cshow);
  m_iconized = iconize;
}

// Equivalent to maximize/restore in Windows
void wxFrame::Maximize(bool maximize)
{
  Show(TRUE);
  int cshow;
  if (maximize)
    cshow = SW_MAXIMIZE;
  else
    cshow = SW_RESTORE;
  ShowWindow(GetHwnd(), cshow);
  m_iconized = FALSE;
}

bool wxFrame::IsIconized() const
{
  ((wxFrame *)this)->m_iconized = (::IsIconic(GetHwnd()) != 0);
  return m_iconized;
}

// Is it maximized?
bool wxFrame::IsMaximized() const
{
    return (::IsZoomed(GetHwnd()) != 0) ;
}

void wxFrame::SetIcon(const wxIcon& icon)
{
  m_icon = icon;
#if defined(__WIN95__)
  if ( m_icon.Ok() )
    SendMessage(GetHwnd(), WM_SETICON,
                (WPARAM)TRUE, (LPARAM)(HICON) m_icon.GetHICON());
#endif
}

wxStatusBar *wxFrame::OnCreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name)
{
    wxStatusBar *statusBar = NULL;

#if wxUSE_NATIVE_STATUSBAR
    if (UsesNativeStatusBar())
    {
        statusBar = new wxStatusBar95(this, id, style);
    }
    else
#endif
    {
        statusBar = new wxStatusBar(this, id, wxPoint(0, 0), wxSize(100, 20),
            style, name);

        // Set the height according to the font and the border size
        wxClientDC dc(statusBar);
        dc.SetFont(statusBar->GetFont());

        long x, y;
        dc.GetTextExtent("X", &x, &y);

        int height = (int)( (y  * 1.1) + 2* statusBar->GetBorderY());

        statusBar->SetSize(-1, -1, 100, height);
    }

  statusBar->SetFieldsCount(number);
  return statusBar;
}

wxStatusBar* wxFrame::CreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name)
{
  // VZ: calling CreateStatusBar twice is an error - why anyone would do it?
  wxCHECK_MSG( m_frameStatusBar == NULL, FALSE,
               "recreating status bar in wxFrame" );

  m_frameStatusBar = OnCreateStatusBar(number, style, id,
    name);
  if ( m_frameStatusBar )
  {
    PositionStatusBar();
    return m_frameStatusBar;
  }
  else
    return NULL;
}

void wxFrame::SetStatusText(const wxString& text, int number)
{
  wxCHECK_RET( m_frameStatusBar != NULL, "no statusbar to set text for" );

  m_frameStatusBar->SetStatusText(text, number);
}

void wxFrame::SetStatusWidths(int n, const int widths_field[])
{
  wxCHECK_RET( m_frameStatusBar != NULL, "no statusbar to set widths for" );

  m_frameStatusBar->SetStatusWidths(n, widths_field);
  PositionStatusBar();
}

void wxFrame::PositionStatusBar()
{
  // native status bar positions itself
  if (m_frameStatusBar
#if wxUSE_NATIVE_STATUSBAR
   && !m_frameStatusBar->IsKindOf(CLASSINFO(wxStatusBar95))
#endif
    )
  {
      int w, h;
      GetClientSize(&w, &h);
      int sw, sh;
      m_frameStatusBar->GetSize(&sw, &sh);

      // Since we wish the status bar to be directly under the client area,
      // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
      m_frameStatusBar->SetSize(0, h, w, sh);
  }
}

void wxFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    if (!menu_bar)
    {
        m_frameMenuBar = NULL;
        return;
    }

    wxCHECK_RET( !menu_bar->GetFrame(), "this menubar is already attached" );

    if (m_frameMenuBar)
        delete m_frameMenuBar;

    m_hMenu = menu_bar->Create();

    if ( !m_hMenu )
        return;

    InternalSetMenuBar();

    m_frameMenuBar = menu_bar;
    menu_bar->Attach(this);
}

void wxFrame::InternalSetMenuBar()
{
    if ( !::SetMenu(GetHwnd(), (HMENU)m_hMenu) )
    {
        wxLogLastError("SetMenu");
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

/*
 * Frame window
 *
 */

bool wxFrame::MSWCreate(int id, wxWindow *parent, const char *wclass, wxWindow *wx_win, const char *title,
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
  if ((style & wxMINIMIZE) || (style & wxICONIZE))
    msflags |= WS_MINIMIZE;
  if (style & wxMAXIMIZE)
    msflags |= WS_MAXIMIZE;
  if (style & wxCAPTION)
    msflags |= WS_CAPTION;
  if (style & wxCLIP_CHILDREN)
    msflags |= WS_CLIPCHILDREN;

  // Keep this in wxFrame because it saves recoding this function
  // in wxTinyFrame
#if wxUSE_ITSY_BITSY
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

// Default resizing behaviour - if only ONE subwindow, resize to client
// rectangle size
void wxFrame::OnSize(wxSizeEvent& event)
{
    // if we're using constraints - do use them
#if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() )
    {
        Layout();
        return;
    }
#endif

    // do we have _exactly_ one child?
    wxWindow *child = NULL;
    for ( wxNode *node = GetChildren().First(); node; node = node->Next() )
    {
        wxWindow *win = (wxWindow *)node->Data();
        if ( !win->IsKindOf(CLASSINFO(wxFrame))  &&
                !win->IsKindOf(CLASSINFO(wxDialog)) &&
                (win != GetStatusBar()) &&
                (win != GetToolBar()) )
        {
            if ( child )
                return;     // it's our second subwindow - nothing to do
            child = win;
        }
    }

    if ( child ) {
        // we have exactly one child - set it's size to fill the whole frame
        int clientW, clientH;
        GetClientSize(&clientW, &clientH);

        int x = 0;
        int y = 0;

        child->SetSize(x, y, clientW, clientH);
    }
}

// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxFrame::OnActivate(wxActivateEvent& event)
{
  for(wxNode *node = GetChildren().First(); node; node = node->Next())
  {
    // Find a child that's a subwindow, but not a dialog box.
    wxWindow *child = (wxWindow *)node->Data();
    if (!child->IsKindOf(CLASSINFO(wxFrame)) &&
         !child->IsKindOf(CLASSINFO(wxDialog)))
    {
      child->SetFocus();
      return;
    }
  }
}

// The default implementation for the close window event.
void wxFrame::OnCloseWindow(wxCloseEvent& event)
{
    Destroy();
}

// Destroy the window (delayed, if a managed window)
bool wxFrame::Destroy()
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);
  return TRUE;
}

// Default menu selection behaviour - display a help string
void wxFrame::OnMenuHighlight(wxMenuEvent& event)
{
  if (GetStatusBar())
  {
    int menuId = event.GetMenuId();
    if ( menuId != -1 )
    {
      wxMenuBar *menuBar = GetMenuBar();
      if (menuBar && menuBar->FindItem(menuId))
      {
        // set status text even if the string is empty - this will at
        // least remove the string from the item which was previously
        // selected
        SetStatusText(menuBar->GetHelpString(menuId));
      }
    }
  }
}

wxMenuBar *wxFrame::GetMenuBar() const
{
  return m_frameMenuBar;
}

bool wxFrame::ProcessCommand(int id)
{
    wxMenuBar *bar = GetMenuBar() ;
    if ( !bar )
        return FALSE;

    wxMenuItem *item = bar->FindItemForId(id);
    if ( !item )
        return FALSE;

    if ( item->IsCheckable() )
    {
        bar->Check(id, !bar->IsChecked(id)) ;
    }

    wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, id);
    commandEvent.SetInt( id );
    commandEvent.SetEventObject( this );

    return GetEventHandler()->ProcessEvent(commandEvent);
}

// Checks if there is a toolbar, and returns the first free client position
wxPoint wxFrame::GetClientAreaOrigin() const
{
    wxPoint pt(0, 0);
    if (GetToolBar())
    {
        int w, h;
        GetToolBar()->GetSize(& w, & h);

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            pt.x += w;
        }
        else
        {
            pt.y += h;
        }
    }
    return pt;
}

void wxFrame::ScreenToClient(int *x, int *y) const
{
    wxWindow::ScreenToClient(x, y);

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt(GetClientAreaOrigin());
    *x -= pt.x;
    *y -= pt.y;
}

void wxFrame::ClientToScreen(int *x, int *y) const
{
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt1(GetClientAreaOrigin());
    *x += pt1.x;
    *y += pt1.y;

    wxWindow::ClientToScreen(x, y);
}

wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    wxCHECK_MSG( m_frameToolBar == NULL, FALSE,
                 "recreating toolbar in wxFrame" );

    wxToolBar* toolBar = OnCreateToolBar(style, id, name);
    if (toolBar)
    {
        SetToolBar(toolBar);
        PositionToolBar();
        return toolBar;
    }
    else
    {
        return NULL;
    }
}

wxToolBar* wxFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
    return new wxToolBar(this, id, wxDefaultPosition, wxDefaultSize, style, name);
}

void wxFrame::PositionToolBar()
{
    RECT rect;
    ::GetClientRect(GetHwnd(), &rect);

    if ( GetStatusBar() )
    {
      int statusX, statusY;
      GetStatusBar()->GetClientSize(&statusX, &statusY);
      rect.bottom -= statusY;
    }

    if (GetToolBar())
    {
        int tw, th;
        GetToolBar()->GetSize(& tw, & th);

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            // Use the 'real' MSW position
            GetToolBar()->SetSize(0, 0, tw, rect.bottom, wxSIZE_NO_ADJUSTMENTS);
        }
        else
        {
            // Use the 'real' MSW position
            GetToolBar()->SetSize(0, 0, rect.right, th, wxSIZE_NO_ADJUSTMENTS);
        }
    }
}

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

        if ( win->IsKindOf(CLASSINFO(wxFrame)) )
        {
            ((wxFrame *)win)->Iconize(bIconize);
        }
    }
}


// make the window modal (all other windows unresponsive)
void wxFrame::MakeModal(bool modal)
{
    if (modal) {
        wxEnableTopLevelWindows(FALSE);
        Enable(TRUE);           // keep this window enabled
    }
    else {
        wxEnableTopLevelWindows(TRUE);
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
    return acceleratorTable.Ok() &&
           ::TranslateAccelerator(GetHwnd(),
                                  GetTableHaccel(acceleratorTable),
                                  (MSG *)pMsg);
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
            HICON hIcon = m_icon.Ok() ? GetIconHicon(m_icon)
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
            wxPaintEvent event(m_windowId);
            event.m_eventObject = this;

            return GetEventHandler()->ProcessEvent(event);
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
        // forward WM_SIZE to status bar control
#if wxUSE_NATIVE_STATUSBAR
        if (m_frameStatusBar && m_frameStatusBar->IsKindOf(CLASSINFO(wxStatusBar95)))
        {
            wxSizeEvent event(wxSize(x, y), m_frameStatusBar->GetId());
            event.SetEventObject( m_frameStatusBar );

            ((wxStatusBar95 *)m_frameStatusBar)->OnSize(event);
        }
#endif // wxUSE_NATIVE_STATUSBAR

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

bool wxFrame::HandleMenuSelect(WXWORD nItem, WXWORD nFlags, WXHMENU hMenu)
{
    int item;
    if ( nFlags == 0xFFFF && hMenu == 0 )
    {
        // FIXME: what does this do? does it ever happen?
        item = -1;
    }
    else if ((nFlags != MF_SEPARATOR) && (nItem != 0) && (nItem != 65535))
    {
        item = nItem;
    }
    else
    {
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
                HICON hIcon = m_icon.Ok() ? GetIconHicon(m_icon)
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
