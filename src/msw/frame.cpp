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
#endif

#include "wx/msw/private.h"
#include "wx/statusbr.h"
#include "wx/toolbar.h"
#include "wx/menuitem.h"

#ifdef LoadAccelerators
#undef LoadAccelerators
#endif

#if wxUSE_NATIVE_STATUSBAR
#include <wx/msw/statbr95.h>
#endif

extern wxList wxModelessWindows;
extern wxList wxPendingDelete;
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

wxFrame::wxFrame(void)
{
  m_frameToolBar = NULL ;
  m_frameMenuBar = NULL;
  m_frameStatusBar = NULL;

  m_windowParent = NULL;
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
  if (!parent)
    wxTopLevelWindows.Append(this);

  SetName(name);
//  m_modalShowing = FALSE;
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
  MSWCreate(m_windowId, NULL, wxFrameClassName, this, title,
            x, y, width, height, style);

  wxModelessWindows.Append(this);
  return TRUE;
}

wxFrame::~wxFrame(void)
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

WXHMENU wxFrame::GetWinMenu(void) const
{
  return m_hMenu;
}

// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
void wxFrame::GetClientSize(int *x, int *y) const
{
  RECT rect;
  GetClientRect((HWND) GetHWND(), &rect);

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
void wxFrame::SetClientSize(int width, int height)
{
  HWND hWnd = (HWND) GetHWND();

  RECT rect;
  GetClientRect(hWnd, &rect);

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

void wxFrame::GetSize(int *width, int *height) const
{
  RECT rect;
  GetWindowRect((HWND) GetHWND(), &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void wxFrame::GetPosition(int *x, int *y) const
{
  RECT rect;
  GetWindowRect((HWND) GetHWND(), &rect);
  POINT point;
  point.x = rect.left;
  point.y = rect.top;

  *x = point.x;
  *y = point.y;
}

void wxFrame::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  MoveWindow((HWND) GetHWND(), x1, y1, w1, h1, (BOOL)TRUE);

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

  ShowWindow((HWND) GetHWND(), (BOOL)cshow);
  if (show)
  {
    BringWindowToTop((HWND) GetHWND());

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
  ShowWindow((HWND) GetHWND(), (BOOL)cshow);
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
  ShowWindow((HWND) GetHWND(), cshow);
  m_iconized = FALSE;
}

bool wxFrame::IsIconized(void) const
{
  ((wxFrame *)this)->m_iconized = (::IsIconic((HWND) GetHWND()) != 0);
  return m_iconized;
}

void wxFrame::SetTitle(const wxString& title)
{
  SetWindowText((HWND) GetHWND(), (const char *)title);
}

wxString wxFrame::GetTitle(void) const
{
  GetWindowText((HWND) GetHWND(), wxBuffer, 1000);
  return wxString(wxBuffer);
}

void wxFrame::SetIcon(const wxIcon& icon)
{
  m_icon = icon;
#if defined(__WIN95__)
  if ( m_icon.Ok() )
    SendMessage((HWND) GetHWND(), WM_SETICON,
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
        dc.SetFont(* statusBar->GetFont());

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

void wxFrame::PositionStatusBar(void)
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
  
  if (menu_bar->m_menuBarFrame)
   return;

  int i;
  HMENU menu = CreateMenu();

  for (i = 0; i < menu_bar->m_menuCount; i ++)
  {
    HMENU popup = (HMENU)menu_bar->m_menus[i]->m_hMenu;
    //
    // After looking Bounds Checker result, it seems that all
    // menus must be individually destroyed. So, don't reset m_hMenu,
    // to  allow ~wxMenu to do the job.
    //
    menu_bar->m_menus[i]->m_savehMenu = (WXHMENU) popup;
    // Uncommenting for the moment... JACS
    menu_bar->m_menus[i]->m_hMenu = 0;
    AppendMenu(menu, MF_POPUP | MF_STRING, (UINT)popup, menu_bar->m_titles[i]);
  }

  menu_bar->m_hMenu = (WXHMENU)menu;
  if (m_frameMenuBar)
    delete m_frameMenuBar;

  this->m_hMenu = (WXHMENU) menu;

  DWORD err = 0;
  if (!SetMenu((HWND) GetHWND(), menu))
  {
#ifdef __WIN32__
    err = GetLastError();
#endif
  }

  m_frameMenuBar = menu_bar;
  menu_bar->m_menuBarFrame = this;
}

#if 0
bool wxFrame::LoadAccelerators(const wxString& table)
{
  m_acceleratorTable = (WXHANDLE)
#ifdef __WIN32__
#ifdef UNICODE
     ::LoadAcceleratorsW(wxGetInstance(), (const char *)table);
#else
     ::LoadAcceleratorsA(wxGetInstance(), (const char *)table);
#endif
#else
     ::LoadAccelerators(wxGetInstance(), (const char *)table);
#endif

  // The above is necessary because LoadAccelerators is a macro
  // which we have undefed earlier in the file to avoid confusion
  // with wxFrame::LoadAccelerators. Ugh!

  return (m_acceleratorTable != (WXHANDLE) NULL);
}
#endif

void wxFrame::Fit(void)
{
  // Work out max. size
  wxNode *node = GetChildren()->First();
  int max_width = 0;
  int max_height = 0;
  while (node)
  {
    // Find a child that's a subwindow, but not a dialog box.
    wxWindow *win = (wxWindow *)node->Data();

    if (!win->IsKindOf(CLASSINFO(wxFrame)) &&
         !win->IsKindOf(CLASSINFO(wxDialog)))
    {
      int width, height;
      int x, y;
      win->GetSize(&width, &height);
      win->GetPosition(&x, &y);

      if ((x + width) > max_width)
        max_width = x + width;
      if ((y + height) > max_height)
        max_height = y + height;
    }
    node = node->Next();
  }
  SetClientSize(max_width, max_height);
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
        m_frameStatusBar->ProcessEvent(event2);
    }

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

/*
 * Frame window
 *
 */

void wxFrame::MSWCreate(int id, wxWindow *parent, const char *wclass, wxWindow *wx_win, const char *title,
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

  if (style & wxSTAY_ON_TOP)
    extendedStyle |= WS_EX_TOPMOST;

  m_iconized = FALSE;
  wxWindow::MSWCreate(id, parent, wclass, wx_win, title, x, y, width, height,
         msflags, NULL, extendedStyle);
  // Seems to be necessary if we use WS_POPUP
  // style instead of WS_OVERLAPPED
  if (width > -1 && height > -1)
    ::PostMessage((HWND) GetHWND(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(width, height));
}

bool wxFrame::MSWOnPaint(void)
{
#if WXDEBUG > 1
  wxDebugMsg("wxFrameWnd::OnPaint %d\n", handle);
#endif
  RECT rect;
  if (GetUpdateRect((HWND) GetHWND(), &rect, FALSE))
  {
    if (m_iconized)
    {
      HICON the_icon;
    if (m_icon.Ok())
      the_icon = (HICON) m_icon.GetHICON();
      if (the_icon == 0)
        the_icon = (HICON) m_defaultIcon;

      PAINTSTRUCT ps;
      // Hold a pointer to the dc so long as the OnPaint() message
      // is being processed
      HDC cdc = BeginPaint((HWND) GetHWND(), &ps);
      
      // Erase background before painting or we get white background
      this->MSWDefWindowProc(WM_ICONERASEBKGND,(WORD)ps.hdc,0L);
      
      if (the_icon)
      {
        RECT rect;
        GetClientRect((HWND) GetHWND(), &rect);
        int icon_width = 32;
        int icon_height = 32;
        int icon_x = (int)((rect.right - icon_width)/2);
        int icon_y = (int)((rect.bottom - icon_height)/2);
        DrawIcon(cdc, icon_x, icon_y, the_icon);
      }

      EndPaint((HWND) GetHWND(), &ps);
    }
    else
    {
        wxPaintEvent event(m_windowId);
        event.m_eventObject = this;
        if (!GetEventHandler()->ProcessEvent(event))
            Default();
    }
    return 0;
  }
  return 1;
}

WXHICON wxFrame::MSWOnQueryDragIcon(void)
{
  if (m_icon.Ok() && (m_icon.GetHICON() != 0))
    return m_icon.GetHICON();
  else
    return m_defaultIcon;
}

void wxFrame::MSWOnSize(int x, int y, WXUINT id)
{
#if WXDEBUG > 1
  wxDebugMsg("wxFrameWnd::OnSize %d\n", m_hWnd);
#endif
  switch (id)
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

 if (!m_iconized)
 {
  // forward WM_SIZE to status bar control
#if wxUSE_NATIVE_STATUSBAR
  if (m_frameStatusBar && m_frameStatusBar->IsKindOf(CLASSINFO(wxStatusBar95)))
  {
    wxSizeEvent event(wxSize(x, y), m_frameStatusBar->GetId());
    event.SetEventObject( m_frameStatusBar );

    ((wxStatusBar95 *)m_frameStatusBar)->OnSize(event);
  }
#endif

  PositionStatusBar();
  PositionToolBar();

  wxSizeEvent event(wxSize(x, y), m_windowId);
  event.SetEventObject( this );
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
 }
}

bool wxFrame::MSWOnClose(void)
{
#if WXDEBUG > 1
  wxDebugMsg("wxFrameWnd::OnClose %d\n", handle);
#endif
    return Close();
}

bool wxFrame::MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
#if WXDEBUG > 1
  wxDebugMsg("wxFrameWnd::OnCommand %d\n", handle);
#endif
  if (cmd == 0 || cmd == 1 ) // Can be either a menu command or an accelerator.
  {
    // In case it's e.g. a toolbar.
    wxWindow *win = wxFindWinFromHandle(control);
    if (win)
      return win->MSWCommand(cmd, id);

    if (wxCurrentPopupMenu)
    {
        wxMenu *popupMenu = wxCurrentPopupMenu;
        wxCurrentPopupMenu = NULL;
        if (popupMenu->MSWCommand(cmd, id))
            return TRUE;
    }

    if (GetMenuBar() && GetMenuBar()->FindItemForId(id))
    {
      ProcessCommand(id);
      return TRUE;
    }
    else
      return FALSE;
  }
  else
    return FALSE;
}

void wxFrame::MSWOnMenuHighlight(WXWORD nItem, WXWORD nFlags, WXHMENU hSysMenu)
{
  if (nFlags == 0xFFFF && hSysMenu == 0)
  {
    wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, -1);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
  }
  else if (nFlags != MF_SEPARATOR)
  {
    wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, nItem);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
  }
}

bool wxFrame::MSWProcessMessage(WXMSG* pMsg)
{
  return FALSE;
}

bool wxFrame::MSWTranslateMessage(WXMSG* pMsg)
{
  if (m_acceleratorTable.Ok() &&
          ::TranslateAccelerator((HWND) GetHWND(), (HACCEL) m_acceleratorTable.GetHACCEL(), (MSG *)pMsg))
    return TRUE;
  
  return FALSE;
}

// Default resizing behaviour - if only ONE subwindow,
// resize to client rectangle size
void wxFrame::OnSize(wxSizeEvent& event)
{
  // if we're using constraints - do use them
  #if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() ) {
      Layout();
      return;
    }
  #endif

  // do we have _exactly_ one child?
  wxWindow *child = NULL;
  for ( wxNode *node = GetChildren()->First(); node; node = node->Next() )
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
  for(wxNode *node = GetChildren()->First(); node; node = node->Next())
  {
    // Find a child that's a subwindow, but not a dialog box.
    wxWindow *child = (wxWindow *)node->Data();
    if (!child->IsKindOf(CLASSINFO(wxFrame)) &&
         !child->IsKindOf(CLASSINFO(wxDialog)))
    {
#if WXDEBUG > 1
      wxDebugMsg("wxFrame::OnActivate: about to set the child's focus.\n");
#endif
      child->SetFocus();
      return;
    }
  }
}

// The default implementation for the close window event - calls
// OnClose for backward compatibility.

void wxFrame::OnCloseWindow(wxCloseEvent& event)
{
    // Compatibility
    if ( GetEventHandler()->OnClose() || event.GetForce())
    {
        this->Destroy();
    }
}

bool wxFrame::OnClose(void)
{
    return TRUE;
}

// Destroy the window (delayed, if a managed window)
bool wxFrame::Destroy(void)
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
    if (event.GetMenuId() == -1)
      SetStatusText("");
    else
    {
      wxMenuBar *menuBar = GetMenuBar();
      if (menuBar)
      {
        wxString helpString(menuBar->GetHelpString(event.GetMenuId()));
        if (helpString != "")
          SetStatusText(helpString);
      }
    }
  }
}

wxMenuBar *wxFrame::GetMenuBar(void) const
{
  return m_frameMenuBar;
}

void wxFrame::Centre(int direction)
{
  int display_width, display_height, width, height, x, y;
  wxDisplaySize(&display_width, &display_height);

  GetSize(&width, &height);
  GetPosition(&x, &y);

  if (direction & wxHORIZONTAL)
    x = (int)((display_width - width)/2);
  if (direction & wxVERTICAL)
    y = (int)((display_height - height)/2);

  SetSize(x, y, width, height);
}

// Call this to simulate a menu command
void wxFrame::Command(int id)
{
  ProcessCommand(id);
}

void wxFrame::ProcessCommand(int id)
{
  wxCommandEvent commandEvent(wxEVENT_TYPE_MENU_COMMAND, id);
  commandEvent.SetInt( id );
  commandEvent.SetEventObject( this );

  wxMenuBar *bar = GetMenuBar() ;
  if (!bar)
    return;

  wxMenuItem *item = bar->FindItemForId(id) ;
  if (item && item->IsCheckable())
  {
    bar->Check(id,!bar->Checked(id)) ;
  }
  GetEventHandler()->ProcessEvent(commandEvent);
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

void wxFrame::PositionToolBar(void)
{
    RECT rect;
    ::GetClientRect((HWND) GetHWND(), &rect);

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

// propagate our state change to all child frames
void wxFrame::IconizeChildFrames(bool bIconize)
{
  wxWindow *child = NULL;
  for ( wxNode *node = GetChildren()->First(); node; node = node->Next() ) {
    wxWindow *win = (wxWindow *)node->Data();
    if ( win->IsKindOf(CLASSINFO(wxFrame)) ) {
      ((wxFrame *)win)->Iconize(bIconize);
    }
  }
}

