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

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

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
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/mdi.h"
#include "wx/msw/private.h"

#if wxUSE_NATIVE_STATUSBAR
    #include "wx/msw/statbr95.h"
#endif

#include <string.h>

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern wxWindowList wxModelessWindows;      // from dialog.cpp
extern wxMenu *wxCurrentPopupMenu;

extern wxChar wxMDIFrameClassName[];
extern wxChar wxMDIChildFrameClassName[];
extern wxWindow *wxWndHook;                 // from window.cpp

extern void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win);

static HWND invalidHandle = 0;

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

static const int IDM_WINDOWTILE  = 4001;
static const int IDM_WINDOWTILEHOR  = 4001;
static const int IDM_WINDOWCASCADE = 4002;
static const int IDM_WINDOWICONS = 4003;
static const int IDM_WINDOWNEXT = 4004;
static const int IDM_WINDOWTILEVERT = 4005;

// This range gives a maximum of 500 MDI children. Should be enough :-)
static const int wxFIRST_MDI_CHILD = 4100;
static const int wxLAST_MDI_CHILD = 4600;

// Status border dimensions
static const int wxTHICK_LINE_BORDER = 3;
static const int wxTHICK_LINE_WIDTH  = 1;

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// set the MDI menus (by sending the WM_MDISETMENU message) and update the menu
// of the parent of win (which is supposed to be the MDI client window)
static void MDISetMenu(wxWindow *win, HMENU hmenuFrame, HMENU hmenuWindow);

// insert the window menu (subMenu) into menu just before "Help" submenu or at
// the very end if not found
static void InsertWindowMenu(wxWindow *win, WXHMENU menu, HMENU subMenu);

// is this an id of an MDI child?
inline bool IsMdiCommandId(int id)
{
    return (id >= wxFIRST_MDI_CHILD) && (id <= wxLAST_MDI_CHILD);
}

static void UnpackMDIActivate(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *activate, WXHWND *hwndAct, WXHWND *hwndDeact);

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
    IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
    IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxWindow)
#endif // USE_SHARED_LIBRARY

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
    EVT_SIZE(wxMDIParentFrame::OnSize)
    EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxWindow)
    EVT_SCROLL(wxMDIClientWindow::OnScroll)
END_EVENT_TABLE()

// ===========================================================================
// wxMDIParentFrame: the frame which contains the client window which manages
// the children
// ===========================================================================

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

  m_windowMenu = (WXHMENU) ::LoadMenu(wxGetInstance(), wxT("wxWindowMenu"));

  DWORD msflags = WS_OVERLAPPED;
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

  wxWindow::MSWCreate(m_windowId, parent, wxMDIFrameClassName, this, title, x, y, width, height,
         msflags);

  wxModelessWindows.Append(this);

  return TRUE;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
    DestroyChildren();

    ::DestroyMenu((HMENU)m_windowMenu);
    m_windowMenu = 0;

    if ( m_clientWindow )
    {
        if ( m_clientWindow->MSWGetOldWndProc() )
            m_clientWindow->UnsubclassWin();

        m_clientWindow->SetHWND(0);
        delete m_clientWindow;
    }
}

void wxMDIParentFrame::InternalSetMenuBar()
{
    HMENU subMenu = GetSubMenu((HMENU) m_windowMenu, 0);

    m_parentFrameActive = TRUE;

    InsertWindowMenu(GetClientWindow(), m_hMenu, subMenu);
}

void wxMDIParentFrame::OnSize(wxSizeEvent& event)
{
    if ( GetClientWindow() )
    {
        int width, height;
        GetClientSize(&width, &height);

        GetClientWindow()->SetSize(0, 0, width, height);
    }
}

// Returns the active MDI child window
wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
    HWND hWnd = (HWND)::SendMessage(GetWinHwnd(GetClientWindow()),
                                    WM_MDIGETACTIVE, 0, 0L);
    if ( hWnd == 0 )
        return NULL;
    else
        return (wxMDIChildFrame *)wxFindWinFromHandle((WXHWND) hWnd);
}

// Create the client window class (don't Create the window, just return a new
// class)
wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
    return new wxMDIClientWindow;
}

// Responds to colour changes, and passes event on to children.
void wxMDIParentFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    if ( m_clientWindow )
    {
        m_clientWindow->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
        m_clientWindow->Refresh();
    }

    event.Skip();
}

// ---------------------------------------------------------------------------
// MDI operations
// ---------------------------------------------------------------------------

void wxMDIParentFrame::Cascade()
{
    ::SendMessage(GetWinHwnd(GetClientWindow()), WM_MDICASCADE, 0, 0);
}

// TODO: add a direction argument (hor/vert)
void wxMDIParentFrame::Tile()
{
    ::SendMessage(GetWinHwnd(GetClientWindow()), WM_MDITILE, MDITILE_HORIZONTAL, 0);
}

void wxMDIParentFrame::ArrangeIcons()
{
    ::SendMessage(GetWinHwnd(GetClientWindow()), WM_MDIICONARRANGE, 0, 0);
}

void wxMDIParentFrame::ActivateNext()
{
    ::SendMessage(GetWinHwnd(GetClientWindow()), WM_MDINEXT, 0, 0);
}

void wxMDIParentFrame::ActivatePrevious()
{
    ::SendMessage(GetWinHwnd(GetClientWindow()), WM_MDINEXT, 0, 1);
}

// ---------------------------------------------------------------------------
// the MDI parent frame window proc
// ---------------------------------------------------------------------------

long wxMDIParentFrame::MSWWindowProc(WXUINT message,
                                     WXWPARAM wParam,
                                     WXLPARAM lParam)
{
    long rc = 0;
    bool processed = FALSE;

    switch ( message )
    {
        case WM_ACTIVATE:
            {
                WXWORD state, minimized;
                WXHWND hwnd;
                UnpackActivate(wParam, lParam, &state, &minimized, &hwnd);

                processed = HandleActivate(state, minimized != 0, hwnd);
            }
            break;

        case WM_COMMAND:
            {
                WXWORD id, cmd;
                WXHWND hwnd;
                UnpackCommand(wParam, lParam, &id, &hwnd, &cmd);

                (void)HandleCommand(id, cmd, hwnd);

                // even if the frame didn't process it, there is no need to try it
                // once again (i.e. call wxFrame::HandleCommand()) - we just dud it,
                // so pretend we processed the message anyhow
                processed = TRUE;
            }

            // always pass this message DefFrameProc(), otherwise MDI menu
            // commands (and sys commands - more surprizingly!) won't work
            MSWDefWindowProc(message, wParam, lParam);
            break;

        case WM_CREATE:
            m_clientWindow = OnCreateClient();
            // Uses own style for client style
            if ( !m_clientWindow->CreateClient(this, GetWindowStyleFlag()) )
            {
                wxLogMessage(_("Failed to create MDI parent frame."));

                rc = -1;
            }

            processed = TRUE;
            break;

        case WM_ERASEBKGND:
            processed = TRUE;

            // we erase background ourselves
            rc = TRUE;
            break;

        case WM_MENUSELECT:
            {
                WXWORD item, flags;
                WXHMENU hmenu;
                UnpackMenuSelect(wParam, lParam, &item, &flags, &hmenu);

                if ( m_parentFrameActive )
                {
                    processed = HandleMenuSelect(item, flags, hmenu);
                }
                else if (m_currentChild)
                {
                    processed = m_currentChild->
                        HandleMenuSelect(item, flags, hmenu);
                }
            }
            break;

        case WM_SIZE:
            // as we don't (usually) resize the MDI client to exactly fit the
            // client area (we put it below the toolbar, above statusbar &c),
            // we should not pass this one to DefFrameProc
            break;
    }

    if ( !processed )
        rc = wxFrame::MSWWindowProc(message, wParam, lParam);

    return rc;
}

bool wxMDIParentFrame::HandleActivate(int state, bool minimized, WXHWND activate)
{
    bool processed = FALSE;

    if ( wxWindow::HandleActivate(state, minimized, activate) )
    {
        // already processed
        processed = TRUE;
    }

    // If this window is an MDI parent, we must also send an OnActivate message
    // to the current child.
    if ( (m_currentChild != NULL) &&
         ((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)) )
    {
        wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_currentChild->GetId());
        event.SetEventObject( m_currentChild );
        if ( m_currentChild->GetEventHandler()->ProcessEvent(event) )
            processed = TRUE;
    }

    return processed;
}

bool wxMDIParentFrame::HandleCommand(WXWORD id, WXWORD cmd, WXHWND hwnd)
{
    // In case it's e.g. a toolbar.
    if ( hwnd )
    {
        wxWindow *win = wxFindWinFromHandle(hwnd);
        if ( win )
            return win->MSWCommand(cmd, id);
    }

    // is it one of standard MDI commands?
    WXWPARAM wParam = 0;
    int msg;
    switch ( id )
    {
        case IDM_WINDOWCASCADE:
            msg = WM_MDICASCADE;
            wParam = MDITILE_SKIPDISABLED;
            break;

        case IDM_WINDOWTILEHOR:
            wParam |= MDITILE_HORIZONTAL;
            // fall through

        case IDM_WINDOWTILEVERT:
            if ( !wParam )
                wParam = MDITILE_VERTICAL;
            msg = WM_MDITILE;
            wParam |= MDITILE_SKIPDISABLED;
            break;

        case IDM_WINDOWICONS:
            msg = WM_MDIICONARRANGE;
            break;

        case IDM_WINDOWNEXT:
            msg = WM_MDINEXT;
            break;

        default:
            msg = 0;
    }

    if ( msg )
    {
        ::SendMessage(GetWinHwnd(GetClientWindow()), msg, wParam, 0);

        return TRUE;
    }

    // FIXME VZ: what does this test do??
    if (id >= 0xF000)
    {
        return FALSE; // Get WndProc to call default proc
    }

    if ( IsMdiCommandId(id) )
    {
        wxWindowList::Node* node = GetChildren().GetFirst();
        while ( node )
        {
            wxWindow* child = node->GetData();
            if ( child->GetHWND() )
            {
                long childId = wxGetWindowId(child->GetHWND());
                if (childId == (long)id)
                {
                    ::SendMessage( GetWinHwnd(GetClientWindow()),
                                   WM_MDIACTIVATE,
                                   (WPARAM)child->GetHWND(), 0);
                    return TRUE;
                }
            }
            node = node->GetNext();
        }
    }
    else if ( m_parentFrameActive )
    {
        return ProcessCommand(id);
    }
    else if ( m_currentChild )
    {
        return m_currentChild->HandleCommand(id, cmd, hwnd);
    }
    else
    {
        // this shouldn't happen because it means that our messages are being
        // lost (they're not sent to the parent frame nor to the children)
        wxFAIL_MSG(wxT("MDI parent frame is not active, "
                      "yet there is no active MDI child?"));
    }

    return FALSE;
}

long wxMDIParentFrame::MSWDefWindowProc(WXUINT message,
                                        WXWPARAM wParam,
                                        WXLPARAM lParam)
{
    WXHWND clientWnd;
    if ( GetClientWindow() )
        clientWnd = GetClientWindow()->GetHWND();
    else
        clientWnd = 0;

    return DefFrameProc(GetHwnd(), (HWND)clientWnd, message, wParam, lParam);
}

bool wxMDIParentFrame::MSWTranslateMessage(WXMSG* msg)
{
    MSG *pMsg = (MSG *)msg;

    if ( m_currentChild && m_currentChild->GetHWND() &&
         m_currentChild->MSWTranslateMessage(msg) )
    {
        return TRUE;
    }

    if ( m_acceleratorTable.Translate(this, msg) )
    {
        return TRUE;
    }

    if ( pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN )
    {
        if ( ::TranslateMDISysAccel(GetWinHwnd(GetClientWindow()), pMsg))
            return TRUE;
    }

    return FALSE;
}

// ===========================================================================
// wxMDIChildFrame
// ===========================================================================

wxMDIChildFrame::wxMDIChildFrame()
{
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
                             wxWindowID id,
                             const wxString& title,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
  m_defaultIcon = (WXHICON)(wxSTD_MDICHILDFRAME_ICON ? wxSTD_MDICHILDFRAME_ICON
                                                     : wxDEFAULT_MDICHILDFRAME_ICON);

  SetName(name);

  if ( id > -1 )
    m_windowId = id;
  else
    m_windowId = (int)NewControlId();

  if ( parent )
  {
      parent->AddChild(this);
  }

  wxWndHook = this;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  MDICREATESTRUCT mcs;

  mcs.szClass = wxMDIChildFrameClassName;
  mcs.szTitle = title;
  mcs.hOwner = wxGetInstance();
  if (x > -1)
      mcs.x = x;
  else
      mcs.x = CW_USEDEFAULT;

  if (y > -1)
      mcs.y = y;
  else
      mcs.y = CW_USEDEFAULT;

  if (width > -1)
      mcs.cx = width;
  else
      mcs.cx = CW_USEDEFAULT;

  if (height > -1)
      mcs.cy = height;
  else
      mcs.cy = CW_USEDEFAULT;

  DWORD msflags = WS_OVERLAPPED | WS_CLIPCHILDREN;
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

  DWORD Return = SendMessage(GetWinHwnd(parent->GetClientWindow()),
                             WM_MDICREATE, 0, (LONG)(LPSTR)&mcs);

  //handle = (HWND)LOWORD(Return);
  // Must be the DWORRD for WIN32. And in 16 bits, HIWORD=0 (says Microsoft)
  m_hWnd = (WXHWND)Return;

  wxWndHook = NULL;
  wxAssociateWinWithHandle((HWND) GetHWND(), this);

  // VZ: what's this? an act of piracy?
  //SetWindowLong(GetHwnd(), 0, (long)this);

  wxModelessWindows.Append(this);
  return TRUE;
}

wxMDIChildFrame::~wxMDIChildFrame()
{
    MSWDestroyWindow();
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxMDIChildFrame::DoSetClientSize(int width, int height)
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

void wxMDIChildFrame::DoGetPosition(int *x, int *y) const
{
  RECT rect;
  GetWindowRect(GetHwnd(), &rect);
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

void wxMDIChildFrame::InternalSetMenuBar()
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();

    HMENU subMenu = GetSubMenu((HMENU)parent->GetWindowMenu(), 0);

    InsertWindowMenu(parent->GetClientWindow(), m_hMenu, subMenu);

    parent->m_parentFrameActive = FALSE;
}

// ---------------------------------------------------------------------------
// MDI operations
// ---------------------------------------------------------------------------

void wxMDIChildFrame::Maximize(bool maximize)
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
    if ( parent && parent->GetClientWindow() )
    {
        ::SendMessage(GetWinHwnd(parent->GetClientWindow()),
                      maximize ? WM_MDIMAXIMIZE : WM_MDIRESTORE,
                      (WPARAM)GetHwnd(), 0);
    }
}

void wxMDIChildFrame::Restore()
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
    if ( parent && parent->GetClientWindow() )
    {
        ::SendMessage(GetWinHwnd(parent->GetClientWindow()), WM_MDIRESTORE,
                      (WPARAM) GetHwnd(), 0);
    }
}

void wxMDIChildFrame::Activate()
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();
    if ( parent && parent->GetClientWindow() )
    {
        ::SendMessage(GetWinHwnd(parent->GetClientWindow()), WM_MDIACTIVATE,
                      (WPARAM) GetHwnd(), 0);
    }
}

// ---------------------------------------------------------------------------
// MDI window proc and message handlers
// ---------------------------------------------------------------------------

long wxMDIChildFrame::MSWWindowProc(WXUINT message,
                                    WXWPARAM wParam,
                                    WXLPARAM lParam)
{
    long rc = 0;
    bool processed = FALSE;

    switch ( message )
    {
        case WM_COMMAND:
            {
                WORD id, cmd;
                WXHWND hwnd;
                UnpackCommand((WXWPARAM)wParam, (WXLPARAM)lParam,
                              &id, &hwnd, &cmd);

                processed = HandleCommand(id, cmd, (WXHWND)hwnd);
            }
            break;

        case WM_GETMINMAXINFO:
            // let the default window proc calculate the size of MDI children
            // frames because it is based on the size of the MDI client window,
            // not on the values specified in wxWindow m_min/max variables
            return MSWDefWindowProc(message, wParam, lParam);

        case WM_MDIACTIVATE:
            {
                WXWORD act;
                WXHWND hwndAct, hwndDeact;
                UnpackMDIActivate(wParam, lParam, &act, &hwndAct, &hwndDeact);

                processed = HandleMDIActivate(act, hwndAct, hwndDeact);
            }
            // fall through

        case WM_MOVE:
            // must pass WM_MOVE to DefMDIChildProc() to recalculate MDI client
            // scrollbars if necessary

            // fall through

        case WM_SIZE:
            // must pass WM_SIZE to DefMDIChildProc(), otherwise many weird
            // things happen
            MSWDefWindowProc(message, wParam, lParam);
            break;

        case WM_SYSCOMMAND:
            // DefMDIChildProc handles SC_{NEXT/PREV}WINDOW here, so pass it
            // the message (the base class version does not)
            return MSWDefWindowProc(message, wParam, lParam);

        case WM_WINDOWPOSCHANGING:
            processed = HandleWindowPosChanging((LPWINDOWPOS)lParam);
            break;
    }

    if ( !processed )
        rc = wxFrame::MSWWindowProc(message, wParam, lParam);

    return rc;
}

bool wxMDIChildFrame::HandleSize(int x, int y, WXUINT id)
{
    HWND hwnd = GetHwnd();

    if ( !hwnd || hwnd == invalidHandle )
    {
        return FALSE;
    }

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

        return wxWindow::HandleSize(x, y, id);
    }
    else
    {
        return FALSE;
    }
}

bool wxMDIChildFrame::HandleCommand(WXWORD id, WXWORD cmd, WXHWND hwnd)
{
    // In case it's e.g. a toolbar.
    if ( hwnd )
    {
        wxWindow *win = wxFindWinFromHandle(hwnd);
        if (win)
            return win->MSWCommand(cmd, id);
    }

    if (wxCurrentPopupMenu)
    {
        wxMenu *popupMenu = wxCurrentPopupMenu;
        wxCurrentPopupMenu = NULL;
        if (popupMenu->MSWCommand(cmd, id))
            return TRUE;
    }

    if (GetMenuBar() && GetMenuBar()->FindItem(id))
    {
        ProcessCommand(id);
        return TRUE;
    }
    else
        return FALSE;

    return TRUE;
}

bool wxMDIChildFrame::HandleMDIActivate(long WXUNUSED(activate),
                                        WXHWND hwndAct,
                                        WXHWND hwndDeact)
{
    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();

    HMENU menuToSet = 0;

    bool activated;

    if ( m_hWnd == hwndAct )
    {
        activated = TRUE;
        parent->m_currentChild = this;

        HMENU child_menu = (HMENU)GetWinMenu();
        if ( child_menu )
        {
            parent->m_parentFrameActive = FALSE;

            menuToSet = child_menu;
        }
    }
    else if ( m_hWnd == hwndDeact )
    {
        wxASSERT_MSG( parent->m_currentChild == this,
                      wxT("can't deactivate MDI child which wasn't active!") );

        activated = FALSE;
        parent->m_currentChild = NULL;

        HMENU parent_menu = (HMENU)parent->GetWinMenu();
        if ( parent_menu )
        {
            parent->m_parentFrameActive = TRUE;

            menuToSet = parent_menu;
        }
    }
    else
    {
        // we have nothing to with it
        return FALSE;
    }

    if ( menuToSet )
    {
        HMENU subMenu = GetSubMenu((HMENU) parent->GetWindowMenu(), 0);

        MDISetMenu(parent->GetClientWindow(), menuToSet, subMenu);
    }

    wxActivateEvent event(wxEVT_ACTIVATE, activated, m_windowId);
    event.SetEventObject( this );

    return GetEventHandler()->ProcessEvent(event);
}

bool wxMDIChildFrame::HandleWindowPosChanging(void *pos)
{
    WINDOWPOS *lpPos = (WINDOWPOS *)pos;
#if defined(__WIN95__)
    if (!(lpPos->flags & SWP_NOSIZE))
    {
        RECT rectClient;
        DWORD dwExStyle = ::GetWindowLong(GetHwnd(), GWL_EXSTYLE);
        DWORD dwStyle = ::GetWindowLong(GetHwnd(), GWL_STYLE);
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
#endif // Win95

    return FALSE;
}

// ---------------------------------------------------------------------------
// MDI specific message translation/preprocessing
// ---------------------------------------------------------------------------

long wxMDIChildFrame::MSWDefWindowProc(WXUINT message, WXUINT wParam, WXLPARAM lParam)
{
    return DefMDIChildProc(GetHwnd(),
                           (UINT)message, (WPARAM)wParam, (LPARAM)lParam);
}

bool wxMDIChildFrame::MSWTranslateMessage(WXMSG* msg)
{
    return m_acceleratorTable.Translate(GetParent(), msg);
}

// ---------------------------------------------------------------------------
// misc
// ---------------------------------------------------------------------------

void wxMDIChildFrame::MSWDestroyWindow()
{
    MSWDetachWindowMenu();
    invalidHandle = GetHwnd();

    wxMDIParentFrame *parent = (wxMDIParentFrame *)GetParent();

    // Must make sure this handle is invalidated (set to NULL) since all sorts
    // of things could happen after the child client is destroyed, but before
    // the wxFrame is destroyed.

    HWND oldHandle = (HWND)GetHWND();
    SendMessage(GetWinHwnd(parent->GetClientWindow()), WM_MDIDESTROY,
                (WPARAM)oldHandle, 0);
    invalidHandle = 0;

    if (m_hMenu)
    {
        ::DestroyMenu((HMENU) m_hMenu);
        m_hMenu = 0;
    }
    m_hWnd = 0;
}

// Change the client window's extended style so we don't get a client edge
// style when a child is maximised (a double border looks silly.)
bool wxMDIChildFrame::ResetWindowStyle(void *vrect)
{
#if defined(__WIN95__)
    RECT *rect = (RECT *)vrect;
    wxMDIParentFrame* pFrameWnd = (wxMDIParentFrame *)GetParent();
    wxMDIChildFrame* pChild = pFrameWnd->GetActiveChild();
    if (!pChild || (pChild == this))
    {
        DWORD dwStyle = ::GetWindowLong(GetWinHwnd(pFrameWnd->GetClientWindow()), GWL_EXSTYLE);
        DWORD dwThisStyle = ::GetWindowLong(GetHwnd(), GWL_STYLE);
        DWORD dwNewStyle = dwStyle;
        if (pChild != NULL && (dwThisStyle & WS_MAXIMIZE))
            dwNewStyle &= ~(WS_EX_CLIENTEDGE);
        else
            dwNewStyle |= WS_EX_CLIENTEDGE;

        if (dwStyle != dwNewStyle)
        {
            HWND hwnd = GetWinHwnd(pFrameWnd->GetClientWindow());
            ::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwNewStyle);
            ::SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                           SWP_FRAMECHANGED | SWP_NOACTIVATE |
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                           SWP_NOCOPYBITS);
            if (rect)
                ::GetClientRect(hwnd, rect);

            return TRUE;
        }
    }
#endif // Win95

    return FALSE;
}

// ===========================================================================
// wxMDIClientWindow: the window of predefined (by Windows) class which
// contains the child frames
// ===========================================================================

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);

    CLIENTCREATESTRUCT ccs;
    m_windowStyle = style;
    m_parent = parent;

    ccs.hWindowMenu = (HMENU)parent->GetWindowMenu();
    ccs.idFirstChild = wxFIRST_MDI_CHILD;

    DWORD msStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN;
    if ( style & wxHSCROLL )
        msStyle |= WS_HSCROLL;
    if ( style & wxVSCROLL )
        msStyle |= WS_VSCROLL;

#if defined(__WIN95__)
    DWORD exStyle = WS_EX_CLIENTEDGE;
#else
    DWORD exStyle = 0;
#endif

    wxWndHook = this;
    m_hWnd = (WXHWND)::CreateWindowEx
                       (
                        exStyle,
                        wxT("MDICLIENT"),
                        NULL,
                        msStyle,
                        0, 0, 0, 0,
                        GetWinHwnd(parent),
                        NULL,
                        wxGetInstance(),
                        (LPSTR)(LPCLIENTCREATESTRUCT)&ccs);
    if ( !m_hWnd )
    {
        wxLogLastError("CreateWindowEx(MDI client)");

        return FALSE;
    }

    SubclassWin(m_hWnd);
    wxWndHook = NULL;

    return TRUE;
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

    event.Skip();
}

// ---------------------------------------------------------------------------
// non member functions
// ---------------------------------------------------------------------------

static void MDISetMenu(wxWindow *win, HMENU hmenuFrame, HMENU hmenuWindow)
{
    ::SendMessage(GetWinHwnd(win), WM_MDISETMENU,
#ifdef __WIN32__
                  (WPARAM)hmenuFrame, (LPARAM)hmenuWindow);
#else
                  0, MAKELPARAM(hmenuFrame, hmenuWindow));
#endif

    // update menu bar of the parent window
    wxWindow *parent = win->GetParent();
    wxCHECK_RET( parent, wxT("MDI client without parent frame? weird...") );

    ::DrawMenuBar(GetWinHwnd(parent));
}

static void InsertWindowMenu(wxWindow *win, WXHMENU menu, HMENU subMenu)
{
    // Try to insert Window menu in front of Help, otherwise append it.
    HMENU hmenu = (HMENU)menu;
    int N = GetMenuItemCount(hmenu);
    bool success = FALSE;
    for ( int i = 0; i < N; i++ )
    {
        wxChar buf[256];
        int chars = GetMenuString(hmenu, i, buf, WXSIZEOF(buf), MF_BYPOSITION);
        if ( chars == 0 )
        {
            wxLogLastError(wxT("GetMenuString"));

            continue;
        }

        if ( wxStripMenuCodes(wxString(buf)).IsSameAs(wxT("Help")) )
        {
            success = TRUE;
            ::InsertMenu(hmenu, i, MF_BYPOSITION | MF_POPUP | MF_STRING,
                         (UINT)subMenu, wxT("&Window"));
            break;
        }
    }

    if ( !success )
    {
        ::AppendMenu(hmenu, MF_POPUP, (UINT)subMenu, wxT("&Window"));
    }

    MDISetMenu(win, hmenu, subMenu);
}

static void UnpackMDIActivate(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *activate, WXHWND *hwndAct, WXHWND *hwndDeact)
{
#ifdef __WIN32__
    *activate = TRUE;
    *hwndAct = (WXHWND)lParam;
    *hwndDeact = (WXHWND)wParam;
#else // Win16
    *activate = (WXWORD)wParam;
    *hwndAct = (WXHWND)LOWORD(lParam);
    *hwndDeact = (WXHWND)HIWORD(lParam);
#endif // Win32/Win16
}
