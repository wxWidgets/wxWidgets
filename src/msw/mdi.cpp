/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
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
#include "wx/statusbr.h"
#include "wx/settings.h"
#endif

#include "wx/mdi.h"
#include "wx/msw/private.h"

#if wxUSE_NATIVE_STATUSBAR
#include <wx/msw/statbr95.h>
#endif

#include <string.h>

extern wxList wxModelessWindows;
extern wxMenu *wxCurrentPopupMenu;

#define IDM_WINDOWTILE  4001
#define IDM_WINDOWCASCADE 4002
#define IDM_WINDOWICONS 4003
#define IDM_WINDOWNEXT 4004
// This range gives a maximum of 500
// MDI children. Should be enough :-)
#define wxFIRST_MDI_CHILD 4100
#define wxLAST_MDI_CHILD 4600

// Status border dimensions
#define         wxTHICK_LINE_BORDER 3
#define         wxTHICK_LINE_WIDTH  1

extern char wxMDIFrameClassName[];
extern char wxMDIChildFrameClassName[];
extern wxWindow *wxWndHook;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
  EVT_SIZE(wxMDIParentFrame::OnSize)
  EVT_ACTIVATE(wxMDIParentFrame::OnActivate)
  EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxWindow)
  EVT_SCROLL(wxMDIClientWindow::OnScroll)
END_EVENT_TABLE()

#endif

wxMDIParentFrame::wxMDIParentFrame()
{
    m_clientWindow = NULL;
    m_currentChild = NULL;
    m_windowMenu = 0;
    m_parentFrameActive = TRUE;
}

bool wxMDIParentFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  m_defaultIcon = (WXHICON) (wxSTD_MDIPARENTFRAME_ICON ? wxSTD_MDIPARENTFRAME_ICON : wxDEFAULT_MDIPARENTFRAME_ICON);

  m_clientWindow = NULL;
  m_currentChild = NULL;
  m_windowMenu = 0;
  m_parentFrameActive = TRUE;

  if (!parent)
    wxTopLevelWindows.Append(this);

  SetName(name);
  m_windowStyle = style;

  if (parent) parent->AddChild(this);

  if ( id > -1 )
    m_windowId = id;
  else
    m_windowId = (int)NewControlId();

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowMenu = (WXHMENU) ::LoadMenu(wxGetInstance(), "wxWindowMenu");

  DWORD msflags = WS_OVERLAPPED ;
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

  // Adding WS_CLIPCHILDREN causes children not to be properly
  // drawn when first displaying them.
//  if (style & wxCLIP_CHILDREN)
//    msflags |= WS_CLIPCHILDREN;

  wxWindow::MSWCreate(m_windowId, parent, wxMDIFrameClassName, this, title, x, y, width, height,
         msflags);

  wxModelessWindows.Append(this);

  return TRUE;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
  DestroyChildren();

  DestroyMenu((HMENU) m_windowMenu); // Destroy dummy "Window" menu
  m_windowMenu = 0;

  if (m_clientWindow->MSWGetOldWndProc())
      m_clientWindow->UnsubclassWin();

  m_clientWindow->m_hWnd = 0;
  delete m_clientWindow;
}

// Get size *available for subwindows* i.e. excluding menu bar.
void wxMDIParentFrame::GetClientSize(int *x, int *y) const
{
  RECT rect;
  ::GetClientRect((HWND) GetHWND(), &rect);

  int cwidth = rect.right;
  int cheight = rect.bottom;

  if ( GetStatusBar() )
  {
    int sw, sh;
    GetStatusBar()->GetSize(&sw, &sh);
    cheight -= sh;
  }

  wxPoint pt(GetClientAreaOrigin());
  cheight -= pt.y;
  cwidth -= pt.x;

  *x = cwidth;
  *y = cheight;
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menu_bar)
{
  if (!menu_bar)
  {
    m_frameMenuBar = NULL;
    return;
  }

  if ( menu_bar->IsAttached() )
     return;

  m_hMenu = menu_bar->Create();

  if (m_frameMenuBar)
    delete m_frameMenuBar;

  // MDI parent-specific code follows

  HMENU subMenu = GetSubMenu((HMENU) m_windowMenu, 0);

  // Try to insert Window menu in front of Help, otherwise append it.
  HMENU menu = (HMENU)m_hMenu;
  int N = GetMenuItemCount(menu);
  bool success = FALSE;
  for (int i = 0; i < N; i++)
  {
        char buf[100];
        int chars = GetMenuString(menu, i, buf, 100, MF_BYPOSITION);
        if ((chars > 0) && (strcmp(buf, "&Help") == 0 ||
                            strcmp(buf, "Help") == 0))
        {
           success = TRUE;
           InsertMenu(menu, i, MF_BYPOSITION | MF_POPUP | MF_STRING,
                      (UINT)subMenu, "&Window");
           break;
        }
  }
  if (!success)
    AppendMenu(menu, MF_POPUP,
                         (UINT)subMenu,
                         "&Window");
  m_parentFrameActive = TRUE;
#ifdef __WIN32__
  SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDISETMENU,
                (WPARAM)menu,
                  (LPARAM)subMenu);
#else
  SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDISETMENU, 0,
              MAKELPARAM(menu, subMenu));
#endif
  DrawMenuBar((HWND) GetHWND());

  m_frameMenuBar = menu_bar;
  menu_bar->Attach(this);
}

void wxMDIParentFrame::OnSize(wxSizeEvent& event)
{
#if wxUSE_CONSTRAINTS
    if (GetAutoLayout())
      Layout();
#endif
    int x = 0;
    int y = 0;
    int width, height;
    GetClientSize(&width, &height);

    if ( GetClientWindow() )
        GetClientWindow()->SetSize(x, y, width, height);

/* Already done in MSWOnSize
  // forward WM_SIZE to status bar control
#if wxUSE_NATIVE_STATUSBAR
  if (m_frameStatusBar && m_frameStatusBar->IsKindOf(CLASSINFO(wxStatusBar95)))
    ((wxStatusBar95 *)m_frameStatusBar)->OnSize(event);
#endif
*/

}

void wxMDIParentFrame::OnActivate(wxActivateEvent& event)
{
    // Do nothing
}

// Returns the active MDI child window
wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
//  HWND hWnd = (HWND)LOWORD(SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDIGETACTIVE, 0, 0L));
  HWND hWnd = (HWND)SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDIGETACTIVE, 0, 0L);
  if (hWnd == 0)
    return NULL;
  else
    return (wxMDIChildFrame *)wxFindWinFromHandle((WXHWND) hWnd);
}

// Create the client window class (don't Create the window,
// just return a new class)
wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
    return new wxMDIClientWindow ;
}

// Responds to colour changes, and passes event on to children.
void wxMDIParentFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    if ( m_clientWindow )
    {
        m_clientWindow->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
        m_clientWindow->Refresh();
    }
/*
    if ( m_frameToolBar )
    {
        wxSysColourChangedEvent event2;
        event2.eventObject = m_frameToolBar;
        m_frameToolBar->GetEventHandler()->ProcessEvent(event2);
    }
*/

    // Propagate the event to the non-top-level children
    wxFrame::OnSysColourChanged(event);
}

// MDI operations
void wxMDIParentFrame::Cascade()
{
    ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDICASCADE, 0, 0);
}

void wxMDIParentFrame::Tile()
{
    ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDITILE, MDITILE_HORIZONTAL, 0);
}

void wxMDIParentFrame::ArrangeIcons()
{
    ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDIICONARRANGE, 0, 0);
}

void wxMDIParentFrame::ActivateNext()
{
    ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDINEXT, 0, 0);
}

void wxMDIParentFrame::ActivatePrevious()
{
    ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDINEXT, 0, 1);
}


/*
// Returns a style for the client window - usually 0
// or, for example, wxHSCROLL | wxVSCROLL
long wxMDIParentFrame::GetClientStyle() const
{
    return wxHSCROLL | wxVSCROLL ;
}
*/

bool wxMDIParentFrame::MSWOnDestroy()
{
  return FALSE;
}

void wxMDIParentFrame::MSWOnCreate(WXLPCREATESTRUCT WXUNUSED(cs))
{
    m_clientWindow = OnCreateClient();
    // Uses own style for client style
    m_clientWindow->CreateClient(this, GetWindowStyleFlag());
}

void wxMDIParentFrame::MSWOnSize(int x, int y, WXUINT id)
{
  switch (id)
  {
    case SIZEFULLSCREEN:
    case SIZENORMAL:
      m_iconized = FALSE;
    break;
    case SIZEICONIC:
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

bool wxMDIParentFrame::MSWOnActivate(int state, bool minimized, WXHWND activate)
{
    wxWindow::MSWOnActivate(state, minimized, activate);

    // If this window is an MDI parent, we must also send an OnActivate message
    // to the current child.
    if ((m_currentChild != NULL) && ((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)))
    {
        wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_currentChild->GetId());
        event.SetEventObject( m_currentChild );
        m_currentChild->GetEventHandler()->ProcessEvent(event);
    }
    return 0;
}

bool wxMDIParentFrame::MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
//  if (cmd == 0) // Why did I do this test?
  {
    // In case it's e.g. a toolbar.
    wxWindow *win = wxFindWinFromHandle(control);
    if (win)
      return win->MSWCommand(cmd, id);

/*
    if (wxCurrentPopupMenu)
    {
        wxMenu *popupMenu = wxCurrentPopupMenu;
        wxCurrentPopupMenu = NULL;
        if (!popupMenu->MSWCommand(cmd, id))
            return TRUE;
    }
*/

    switch (id)
    {
      case IDM_WINDOWCASCADE:
        SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDICASCADE, MDITILE_SKIPDISABLED, 0);
        return TRUE;
      case IDM_WINDOWTILE:
        SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDITILE, MDITILE_HORIZONTAL, 0);
        return TRUE;
      case IDM_WINDOWICONS:
        SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDIICONARRANGE, 0, 0);
        return TRUE;
      case IDM_WINDOWNEXT:
        SendMessage((HWND) GetClientWindow()->GetHWND(), WM_MDINEXT, 0, 0);
        return TRUE;
      default:
        break;
     }
    if (id >= 0xF000)
    {
      return FALSE; // Get WndProc to call default proc
    }

    if (m_parentFrameActive && (id < wxFIRST_MDI_CHILD || id > wxLAST_MDI_CHILD))
    {
      ProcessCommand(id);
      return TRUE;
    }
    else if (m_currentChild && (id < wxFIRST_MDI_CHILD || id > wxLAST_MDI_CHILD))
    {
      return m_currentChild->MSWOnCommand(id, cmd, control);
    }
  }
  if (id >= wxFIRST_MDI_CHILD && id <= wxLAST_MDI_CHILD)
  {
    wxNode* node = GetChildren().First();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->Data();
        if (child->GetHWND())
        {
#ifdef __WIN32__
            long childId = GetWindowLong((HWND) child->GetHWND(), GWL_ID);
#else
            long childId = GetWindowWord((HWND) child->GetHWND(), GWW_ID);
#endif
            if (childId == id)
            {
                ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDIACTIVATE, (WPARAM) (HWND) child->GetHWND(), 0);
                return TRUE;
            }
        }
        node = node->Next();
    }
/*
    wxWindow* child = FindItem(id);
    if (child)
    {
        ::SendMessage( (HWND) GetClientWindow()->GetHWND(), WM_MDIACTIVATE, (WPARAM) (HWND) child->GetHWND(), 0);
        return TRUE;
    }
*/
  }

  return wxWindow::MSWOnCommand(id, cmd, control);
}

void wxMDIParentFrame::MSWOnMenuHighlight(WXWORD nItem, WXWORD nFlags, WXHMENU hSysMenu)
{
  if (m_parentFrameActive)
  {
    if (nFlags == 0xFFFF && hSysMenu == (WXHMENU) NULL)
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
  else if (m_currentChild)
  {
    m_currentChild->MSWOnMenuHighlight(nItem, nFlags, hSysMenu);
  }
}

long wxMDIParentFrame::MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    WXHWND clientWnd;
    if ( GetClientWindow() )
        clientWnd = GetClientWindow()->GetHWND();
    else
        clientWnd = 0;

  return DefFrameProc((HWND) GetHWND(), (HWND) clientWnd, message, wParam, lParam);
}

bool wxMDIParentFrame::MSWProcessMessage(WXMSG* msg)
{
  if ((m_currentChild != (wxWindow *)NULL) && (m_currentChild->GetHWND() != (WXHWND) NULL) && m_currentChild->MSWProcessMessage(msg))
     return TRUE;

  return FALSE;
}

bool wxMDIParentFrame::MSWTranslateMessage(WXMSG* msg)
{
  MSG *pMsg = (MSG *)msg;

  if ((m_currentChild != (wxWindow *)NULL) && (m_currentChild->GetHWND() != (WXHWND) NULL) && m_currentChild->MSWTranslateMessage(msg))
     return TRUE;

  if (m_acceleratorTable.Ok() &&
          ::TranslateAccelerator((HWND) GetHWND(), (HACCEL) m_acceleratorTable.GetHACCEL(), pMsg))
    return TRUE;

  if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
  {
    if (::TranslateMDISysAccel((HWND) GetClientWindow()->GetHWND(), pMsg))
      return TRUE;
  }

  return FALSE;
}


bool wxMDIParentFrame::MSWOnEraseBkgnd(WXHDC WXUNUSED(pDC))
{
  return TRUE;
}

extern wxWindow *wxWndHook;
extern wxList *wxWinHandleList;

wxMDIChildFrame::wxMDIChildFrame()
{
//    m_active = FALSE;
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  m_defaultIcon = (WXHICON) (wxSTD_MDICHILDFRAME_ICON ? wxSTD_MDICHILDFRAME_ICON : wxDEFAULT_MDICHILDFRAME_ICON);

  SetName(name);

  if ( id > -1 )
    m_windowId = id;
  else
    m_windowId = (int)NewControlId();

  if (parent) parent->AddChild(this);

  wxWndHook = this;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  MDICREATESTRUCT mcs;

  mcs.szClass = wxMDIChildFrameClassName;
  mcs.szTitle = title;
  mcs.hOwner = wxGetInstance();
  if (x > -1) mcs.x = x;
  else mcs.x = CW_USEDEFAULT;

  if (y > -1) mcs.y = y;
  else mcs.y = CW_USEDEFAULT;

  if (width > -1) mcs.cx = width;
  else mcs.cx = CW_USEDEFAULT;

  if (height > -1) mcs.cy = height;
  else mcs.cy = CW_USEDEFAULT;

  DWORD msflags = WS_OVERLAPPED | WS_CLIPCHILDREN ;
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

  mcs.style = msflags;

  mcs.lParam = 0;

  DWORD Return = SendMessage((HWND) parent->GetClientWindow()->GetHWND(),
        WM_MDICREATE, 0, (LONG)(LPSTR)&mcs);

  //handle = (HWND)LOWORD(Return);
  // Must be the DWORRD for WIN32. And in 16 bits, HIWORD=0 (says Microsoft)
  m_hWnd = (WXHWND)Return;

  // This gets reassigned so can't be stored
//  m_windowId = GetWindowLong((HWND) m_hWnd, GWL_ID);

  wxWndHook = NULL;
  wxWinHandleList->Append((long)GetHWND(), this);

  SetWindowLong((HWND) GetHWND(), 0, (long)this);

  wxModelessWindows.Append(this);
  return TRUE;
}

wxMDIChildFrame::~wxMDIChildFrame()
{
    MSWDestroyWindow();

    ResetWindowStyle(NULL);
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxMDIChildFrame::SetClientSize(int width, int height)
{
  HWND hWnd = (HWND) GetHWND();

  RECT rect;
  ::GetClientRect(hWnd, &rect);

  RECT rect2;
  GetWindowRect(hWnd, &rect2);

  // Find the difference between the entire window (title bar and all)
  // and the client area; add this to the new client size to move the
  // window
  int actual_width = rect2.right - rect2.left - rect.right + width;
  int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

  if (GetStatusBar())
  {
    int sx, sy;
    GetStatusBar()->GetSize(&sx, &sy);
    actual_height += sy;
  }

  POINT point;
  point.x = rect2.left;
  point.y = rect2.top;

  // If there's an MDI parent, must subtract the parent's top left corner
  // since MoveWindow moves relative to the parent
  wxMDIParentFrame *mdiParent = (wxMDIParentFrame *)GetParent();
  ::ScreenToClient((HWND) mdiParent->GetClientWindow()->GetHWND(), &point);

  MoveWindow(hWnd, point.x, point.y, actual_width, actual_height, (BOOL)TRUE);

  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
}

void wxMDIChildFrame::GetPosition(int *x, int *y) const
{
  RECT rect;
  GetWindowRect((HWND) GetHWND(), &rect);
  POINT point;
  point.x = rect.left;
  point.y = rect.top;

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  wxMDIParentFrame *mdiParent = (wxMDIParentFrame *)GetParent();
  ::ScreenToClient((HWND) mdiParent->GetClientWindow()->GetHWND(), &point);

  *x = point.x;
  *y = point.y;
}

void wxMDIChildFrame::SetMenuBar(wxMenuBar *menu_bar)
{
  if (!menu_bar)
  {
    m_frameMenuBar = NULL;
    return;
  }

  if ( menu_bar->IsAttached() )
     return;

  m_hMenu = menu_bar->Create();

  if (m_frameMenuBar)
    delete m_frameMenuBar;

   wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();

   parent->m_parentFrameActive = FALSE;
   HMENU subMenu = GetSubMenu((HMENU) parent->GetWindowMenu(), 0);

   // Try to insert Window menu in front of Help, otherwise append it.
   HMENU menu = (HMENU)m_hMenu;
   int N = GetMenuItemCount(menu);
   bool success = FALSE;
   for (int i = 0; i < N; i++)
   {
        char buf[100];
        int chars = GetMenuString(menu, i, buf, 100, MF_BYPOSITION);
        if ((chars > 0) && (strcmp(buf, "&Help") == 0 ||
                            strcmp(buf, "Help") == 0))
        {
           success = TRUE;
           InsertMenu(menu, i, MF_BYPOSITION | MF_POPUP | MF_STRING,
                      (UINT)subMenu, "&Window");
           break;
        }
      }
  if (!success)
    AppendMenu(menu, MF_POPUP,
                         (UINT)subMenu,
                         "&Window");
#ifdef __WIN32__
  SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDISETMENU,
                  (WPARAM)menu,
                  (LPARAM)subMenu);
#else
  SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDISETMENU, 0,
                  MAKELPARAM(menu, subMenu));
#endif

  DrawMenuBar((HWND) parent->GetHWND());
  m_frameMenuBar = menu_bar;
  menu_bar->Attach(this);
}

// MDI operations
void wxMDIChildFrame::Maximize()
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
    if ( parent && parent->GetClientWindow() )
        ::SendMessage( (HWND) parent->GetClientWindow()->GetHWND(), WM_MDIMAXIMIZE, (WPARAM) (HWND) GetHWND(), 0);
}

void wxMDIChildFrame::Restore()
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
    if ( parent && parent->GetClientWindow() )
        ::SendMessage( (HWND) parent->GetClientWindow()->GetHWND(), WM_MDIRESTORE, (WPARAM) (HWND) GetHWND(), 0);
}

void wxMDIChildFrame::Activate()
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
    if ( parent && parent->GetClientWindow() )
        ::SendMessage( (HWND) parent->GetClientWindow()->GetHWND(), WM_MDIACTIVATE, (WPARAM) (HWND) GetHWND(), 0);
}

static HWND invalidHandle = 0;
void wxMDIChildFrame::MSWOnSize(int x, int y, WXUINT id)
{
  if (!GetHWND()) return;

  if (invalidHandle == (HWND) GetHWND())
  {
    return;
  }

  (void)MSWDefWindowProc(m_lastMsg, m_lastWParam, m_lastLParam);

  switch (id)
  {
    case SIZEFULLSCREEN:
    case SIZENORMAL:
      m_iconized = FALSE;
    break;
    case SIZEICONIC:
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

    wxWindow::MSWOnSize(x, y, id);
  }
}

bool wxMDIChildFrame::MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
//  if ((cmd == 0) && GetHWND())
  if (GetHWND())
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
    return TRUE;
  }
  else
    return wxWindow::MSWOnCommand(id, cmd, control);
}

long wxMDIChildFrame::MSWDefWindowProc(WXUINT message, WXUINT wParam, WXLPARAM lParam)
{
  if (GetHWND())
    return DefMDIChildProc((HWND) GetHWND(), (UINT) message, (WPARAM) wParam, (LPARAM) lParam);
  else return 0;
}

bool wxMDIChildFrame::MSWProcessMessage(WXMSG *msg)
{
  return FALSE;
}

bool wxMDIChildFrame::MSWTranslateMessage(WXMSG* msg)
{
  MSG *pMsg = (MSG *)msg;
  if (m_acceleratorTable.Ok())
  {
    wxFrame *parent = (wxFrame *)GetParent();
    HWND parent_hwnd = (HWND) parent->GetHWND();
    return (::TranslateAccelerator(parent_hwnd, (HACCEL) m_acceleratorTable.GetHACCEL(), pMsg) != 0);
  }

  return FALSE;
}

long wxMDIChildFrame::MSWOnMDIActivate(long activate, WXHWND WXUNUSED(one), WXHWND WXUNUSED(two))
{
  wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
  HMENU parent_menu = (HMENU) parent->GetWinMenu();
  HMENU child_menu = (HMENU) GetWinMenu();

  if (activate)
  {
//    m_active = TRUE;
    parent->m_currentChild = this;
    if (child_menu)
    {
      parent->m_parentFrameActive = FALSE;
      HMENU subMenu = GetSubMenu((HMENU) parent->GetWindowMenu(), 0);
#ifdef __WIN32__
      ::SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDISETMENU,
                    (WPARAM)child_menu,
                    (LPARAM)subMenu);
#else
      ::SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDISETMENU, 0,
                  MAKELONG(child_menu, subMenu));
#endif

      ::DrawMenuBar((HWND) parent->GetHWND());
    }
    wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_windowId);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
  }
  else
  {
    if (parent->m_currentChild == this)
      parent->m_currentChild = NULL;

    wxActivateEvent event(wxEVT_ACTIVATE, FALSE, m_windowId);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);

//    m_active = FALSE;
    if (parent_menu)
    {
      parent->m_parentFrameActive = TRUE;
      HMENU subMenu = GetSubMenu((HMENU) parent->GetWindowMenu(), 0);
#ifdef __WIN32__
      ::SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDISETMENU,
                    (WPARAM)parent_menu,
                    (LPARAM)subMenu);
#else
      ::SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDISETMENU, 0,
                  MAKELONG(parent_menu, subMenu));
#endif

      ::DrawMenuBar((HWND) parent->GetHWND());
    }
  }
  bool flag = (activate != 0);
  wxActivateEvent event(wxEVT_ACTIVATE, flag, m_windowId);
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
  return 0;
}

void wxMDIChildFrame::MSWDestroyWindow()
{
  MSWDetachWindowMenu();
  invalidHandle = (HWND) GetHWND();

  wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();

  // Must make sure this handle is invalidated (set to NULL)
  // since all sorts of things could happen after the
  // child client is destroyed, but before the wxFrame is
  // destroyed.

  HWND oldHandle = (HWND)GetHWND();
#ifdef __WIN32__
  SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDIDESTROY, (WPARAM)oldHandle, (LPARAM)0);
#else
  SendMessage((HWND) parent->GetClientWindow()->GetHWND(), WM_MDIDESTROY, (HWND)oldHandle, 0);
#endif
  invalidHandle = 0;

  if (m_hMenu)
  {
    ::DestroyMenu((HMENU) m_hMenu);
    m_hMenu = 0;
  }
  m_hWnd = 0;
}

// Change the client window's extended style so we don't
// get a client edge style when a child is maximised (a double
// border looks silly.)
bool wxMDIChildFrame::ResetWindowStyle(void *vrect)
{
#if defined(__WIN95__)
    RECT *rect = (RECT *)vrect;
    wxMDIParentFrame* pFrameWnd = (wxMDIParentFrame *)GetParent();
    wxMDIChildFrame* pChild = pFrameWnd->GetActiveChild();
    if (!pChild || (pChild == this))
    {
        DWORD dwStyle = ::GetWindowLong((HWND) pFrameWnd->GetClientWindow()->GetHWND(), GWL_EXSTYLE);
        DWORD dwThisStyle = ::GetWindowLong((HWND) GetHWND(), GWL_STYLE);
        DWORD dwNewStyle = dwStyle;
        if (pChild != NULL && (dwThisStyle & WS_MAXIMIZE))
            dwNewStyle &= ~(WS_EX_CLIENTEDGE);
        else
            dwNewStyle |= WS_EX_CLIENTEDGE;

        if (dwStyle != dwNewStyle)
        {
            ::RedrawWindow((HWND) pFrameWnd->GetClientWindow()->GetHWND(), NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
            ::SetWindowLong((HWND) pFrameWnd->GetClientWindow()->GetHWND(), GWL_EXSTYLE, dwNewStyle);
            ::SetWindowPos((HWND) pFrameWnd->GetClientWindow()->GetHWND(), NULL, 0, 0, 0, 0,
                SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
            if (rect)
                ::GetClientRect((HWND) pFrameWnd->GetClientWindow()->GetHWND(), rect);
            return TRUE;
        }
    }
    return FALSE;
#else
    return FALSE;
#endif
}

void wxMDIChildFrame::MSWOnWindowPosChanging(void *pos)
{
    WINDOWPOS *lpPos = (WINDOWPOS *)pos;
#if defined(__WIN95__)
    if (!(lpPos->flags & SWP_NOSIZE))
    {
        RECT rectClient;
        DWORD dwExStyle = ::GetWindowLong((HWND) GetHWND(), GWL_EXSTYLE);
        DWORD dwStyle = ::GetWindowLong((HWND) GetHWND(), GWL_STYLE);
        if (ResetWindowStyle((void *) & rectClient) && (dwStyle & WS_MAXIMIZE))
        {
            ::AdjustWindowRectEx(&rectClient, dwStyle, FALSE, dwExStyle);
            lpPos->x = rectClient.left;
            lpPos->y = rectClient.top;
            lpPos->cx = rectClient.right - rectClient.left;
            lpPos->cy = rectClient.bottom - rectClient.top;
        }
        wxMDIParentFrame* pFrameWnd = (wxMDIParentFrame *)GetParent();
        if (pFrameWnd && pFrameWnd->GetToolBar())
        {
            pFrameWnd->GetToolBar()->Refresh();
        }
    }
#endif
    Default();
}

// Client window
wxMDIClientWindow::wxMDIClientWindow()
{
    m_scrollX = 0;
    m_scrollY = 0;
}

wxMDIClientWindow::~wxMDIClientWindow()
{
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
  m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);

  CLIENTCREATESTRUCT ccs;
  m_windowStyle = style;
  m_windowParent = parent;

  ccs.hWindowMenu = (HMENU) parent->GetWindowMenu();
  ccs.idFirstChild = wxFIRST_MDI_CHILD;

  DWORD msStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN ;
  if ( parent->GetWindowStyleFlag() & wxHSCROLL )
    msStyle |= WS_HSCROLL;
  if ( parent->GetWindowStyleFlag() & wxVSCROLL )
    msStyle |= WS_VSCROLL ;

#if defined(__WIN95__)
  DWORD exStyle = WS_EX_CLIENTEDGE;
#else
  DWORD exStyle = 0;
#endif

  wxWndHook = this;
  m_hWnd = (WXHWND) ::CreateWindowEx(exStyle, "mdiclient", NULL,
                msStyle, 0, 0, 0, 0, (HWND) parent->GetHWND(), NULL,
                wxGetInstance(), (LPSTR)(LPCLIENTCREATESTRUCT)&ccs);
  SubclassWin(m_hWnd);
  wxWndHook = NULL;

  return (m_hWnd != 0) ;
}

// Window procedure: here for debugging purposes
long wxMDIClientWindow::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
//    return MSWDefWindowProc(nMsg, wParam, lParam);
}

long wxMDIClientWindow::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( MSWGetOldWndProc() != 0)
          return ::CallWindowProc(CASTWNDPROC MSWGetOldWndProc(), (HWND) GetHWND(), (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
    else
        return ::DefWindowProc((HWND) m_hWnd, (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
}

// Explicitly call default scroll behaviour
void wxMDIClientWindow::OnScroll(wxScrollEvent& event)
{
    // Note: for client windows, the scroll position is not set in
    // WM_HSCROLL, WM_VSCROLL, so we can't easily determine what
    // scroll position we're at.
    // This makes it hard to paint patterns or bitmaps in the background,
    // and have the client area scrollable as well.

    if ( event.GetOrientation() == wxHORIZONTAL )
        m_scrollX = event.GetPosition(); // Always returns zero!
    else
        m_scrollY = event.GetPosition(); // Always returns zero!

    Default();
}

// Should hand the message to the default proc
long wxMDIClientWindow::MSWOnMDIActivate(long bActivate, WXHWND, WXHWND)
{
    return Default();
}

