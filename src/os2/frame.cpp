/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     wxFrame
// Author:      David Webster
// Modified by:
// Created:     10/27/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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

#include "wx/os2/private.h"

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
extern wxChar wxFrameClassName[];
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
    m_bIconized = FALSE;

#if wxUSE_TOOLTIPS
    m_hHwndToolTip = 0;
#endif
    // Data to save/restore when calling ShowFullScreen
    m_lFsStyle           = 0L;
    m_lFsOldWindowStyle  = 0L;
    m_nFsStatusBarFields = 0;
    m_nFsStatusBarHeight = 0;
    m_nFsToolBarHeight   = 0;
    m_bFsIsMaximized     = FALSE;
    m_bFsIsShowing       = FALSE;
} // end of wxFrame::Init

bool wxFrame::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsTitle
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    int                             nX      = rPos.x;
    int                             nY      = rPos.y;
    int                             nWidth  = rSize.x;
    int                             nHeight = rSize.y;

    SetName(rsName);
    m_windowStyle    = lStyle;
    m_frameMenuBar   = NULL;
    m_frameToolBar   = NULL;
    m_frameStatusBar = NULL;

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

    if (vId > -1 )
        m_windowId = vId;
    else
        m_windowId = (int)NewControlId();

    if (pParent)
        pParent->AddChild(this);

    m_bIconized = FALSE;

    //
    // We pass NULL as parent to MSWCreate because frames with parents behave
    // very strangely under Win95 shell.
    // Alteration by JACS: keep normal Windows behaviour (float on top of parent)
    // with this style.
    //
    if ((m_windowStyle & wxFRAME_FLOAT_ON_PARENT) == 0)
        pParent = NULL;

    if (!pParent)
        wxTopLevelWindows.Append(this);

    OS2Create( m_windowId
              ,pParent
              ,wxFrameClassName
              ,this
              ,rsTitle
              ,nX
              ,nY
              ,nWidth
              ,nHeight
              ,lStyle
             );

    wxModelessWindows.Append(this);
    return TRUE;
} // end of wxFrame::Create

wxFrame::~wxFrame()
{
    m_isBeingDeleted = TRUE;
    wxTopLevelWindows.DeleteObject(this);

    DeleteAllBars();

    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
        wxTheApp->SetTopWindow(NULL);

        if (wxTheApp->GetExitOnFrameDelete())
        {
            ::WinPostMsg(GetHwnd(), WM_QUIT, 0, 0);
        }
    }
    wxModelessWindows.DeleteObject(this);

    //
    // For some reason, wxWindows can activate another task altogether
    // when a frame is destroyed after a modal dialog has been invoked.
    // Try to bring the parent to the top.
    //
    // MT:Only do this if this frame is currently the active window, else weird
    // things start to happen.
    //
    if (wxGetActiveWindow() == this)
    {
        if (GetParent() && GetParent()->GetHWND())
        {
            ::WinSetWindowPos( (HWND) GetParent()->GetHWND()
                              ,HWND_TOP
                              ,0
                              ,0
                              ,0
                              ,0
                              ,SWP_ZORDER
                             );
        }
    }
} // end of wxFrame::~wxFrame

//
// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
//
void wxFrame::DoGetClientSize(
  int*                              pX
, int*                              pY
) const
{
    //
    // OS/2 PM's coordinates go from bottom-left not 
    // top-left thus the += instead of the -=
    //
    RECTL                           vRect;
    HWND                            hWndClient;

    //
    // PM has no GetClientRect that inherantly knows about the client window
    // We have to explicitly go fetch it!
    //
    hWndClient = ::WinWindowFromId(GetHwnd(), FID_CLIENT);
    ::WinQueryWindowRect(hWndClient, &vRect);

#if wxUSE_STATUSBAR
    if ( GetStatusBar() )
    {
        int                         nStatusX
        int                         nStatusY;

        GetStatusBar()->GetClientSize( &nStatusX
                                      ,&nStatusY
                                     );
        vRect.yBottom += nStatusY;
    }
#endif // wxUSE_STATUSBAR

    wxPoint                         vPoint(GetClientAreaOrigin());
  
    vRect.bottom += pt.y;
    vRect.right -= pt.x;

    if (pX)
        *pX = vRect.xRight;
    if (pY)
        *pY = vRect.yBottom;
} // end of wxFrame::DoGetClientSize

//    
// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
//
void wxFrame::DoSetClientSize(
  int                               nWidth
, int                               nHeight
)
{
    HWND                            hWnd = GetHwnd();
    HWND                            hWndClient;
    RECTL                           vRect;
    RECT                            vRect2;

    hWndClient = ::WinWindowFromId(GetHwnd(), FID_CLIENT);
    ::WinQueryWindowRect(hWndClient, &vRect);

    ::WinQueryWindowRect(hWnd, &vRect2);

    //
    // Find the difference between the entire window (title bar and all)
    // and the client area; add this to the new client size to move the
    // window.  Remember OS/2's backwards y coord system!
    //
    int                             nActualWidth = vRect2.xRight - vRect2.xLeft - vRect.xRight + nWidth;
    int                             nActualHeight = vRect2.yTop + vRect2.yTop - vRect.yTop + nHeight;

#if wxUSE_STATUSBAR
    if ( GetStatusBar() )
    {
        int                         nStatusX;
        int                         nStatusY;

        GetStatusBar()->GetClientSize( &nStatusX
                                      ,&nStatusY
                                     );
        nActualHeight += nStatusY;
    }
#endif // wxUSE_STATUSBAR

    wxPoint                         vPoint(GetClientAreaOrigin());
    nActualWidth  += vPoint.y;
    nActualHeight += vPoint.x;

    POINTL                          vPointl;

    vPointl.x = vRect2.xLeft;
    vPoint.y  = vRect2.yTop;

    ::WinSetWindowPos( hWnd
                      ,HWND_TOP
                      ,vPointl.x
                      ,vPointl.y
                      ,nActualWidth
                      ,nActualHeight
                      ,SWP_MOVE | SWP_SIZE | SWP_SHOW
                     );

    wxSizeEvent                     vEvent( wxSize( nWidth
                                                   ,nHeight
                                                  )
                                           ,m_windowId
                                          );
    vEvent.SetEventObject(this);
    GetEventHandler()->ProcessEvent(vEvent);
} // end of wxFrame::DoSetClientSize

void wxFrame::DoGetSize(
  int*                              pWidth
, int*                              pHeight
) const
{
    RECTL                           vRect;

    ::WinQueryWindowRect(GetHwnd(), &vRect);
    *pWidth = vRect.xRight - vRect.xLeft;
    *pHeight = vRect.yTop - vRect.yBottom;
} // end of wxFrame::DoGetSize

void wxFrame::DoGetPosition(
  int*                              pX
, int*                              pY
) const
{
    RECTL                           vRect;
    POINT                           vPoint;

    ::WinQueryWindowRect(GetHwnd(), &vRect);
    vPoint.x = vRect.xLeft;

    //
    // OS/2 is backwards [WIN32 it is vRect.yTop]
    //
    vPoint.y = vRect.yBottom;

    *pX = vPoint.x;
    *pY = vPoint.y;
} // end of wxFrame::DoGetPosition

// ----------------------------------------------------------------------------
// variations around ::ShowWindow()
// ----------------------------------------------------------------------------

void wxFrame::DoShowWindow(
  int                               nShowCmd
)
{
    ::WinShowWindow(GetHwnd(), nShowCmd);
    m_bIconized = nShowCmd == SWP_MINIMIZE;
} // end of wxFrame::DoShowWindow

bool wxFrame::Show(
  bool                              bShow
)
{
    DoShowWindow(show ? SWP_SHOW : SW_HIDE);

    if (bShow)
    {
        wxActivateEvent             vEvent(wxEVT_ACTIVATE, TRUE, m_windowId);

        ::WinSetWindowPos( (HWND) GetHWND()
                          ,HWND_TOP
                          ,0
                          ,0
                          ,0
                          ,0
                          ,SWP_ZORDER
                         );
        vEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(vEvent);
    }
    else
    {
        //
        // Try to highlight the correct window (the parent)
        //
        if (GetParent())
        {
            HWND                    hWndParent = GetHwndOf(GetParent());

            if (hWndParent)
                ::WinSetWindowPos( hWndParent
                                  ,HWND_TOP
                                  ,0
                                  ,0
                                  ,0
                                  ,0
                                  ,SWP_ZORDER
                                 );
        }
    }
    return TRUE;
} // end of wxFrame::Show

void wxFrame::Iconize(
  bool                              bIconize
)
{
    DoShowWindow(bIconize ? SWP_MINIMIZE : SWP_RESTORE);
} // end of wxFrame::Iconize

void wxFrame::Maximize(
  bool                              bMaximize)
{
    DoShowWindow(bMaximize ? SWP_MAXIMIZE : SWP_RESTORE);
} // end of wxFrame::Maximize

void wxFrame::Restore()
{
    DoShowWindow(SWP_RESTORE);
} // end of wxFrame::Restore

bool wxFrame::IsIconized() const
{
    SWP                             vSwp;
    bool                            bIconic;

    ::WinQueryWindowPos(GetHwnd(), &vSwp)
    
    if (vSwp.fl & SWP_MINIMIZE)
        ((wxFrame*)this)->m_bIconized = TRUE;
    else
        ((wxFrame*)this)->m_bIconized = FALSE;
    return m_bIconized;
} // end of wxFrame::IsIconized

// Is it maximized?
bool wxFrame::IsMaximized() const
{
    SWP                             vSwp;
    bool                            bIconic;

    ::WinQueryWindowPos(GetHwnd(), &vSwp)
    return (vSwp.fl & SWP_MAXIMIZE);
} // end of wxFrame::IsMaximized

void wxFrame::SetIcon(const wxIcon& icon)
{
    wxFrameBase::SetIcon(icon);

// TODO:
/*
    if ( m_icon.Ok() )
    {
        SendMessage(GetHwnd(), WM_SETICON,
                    (WPARAM)TRUE, (LPARAM)(HICON) m_icon.GetHICON());
    }
*/
}

#if wxUSE_STATUSBAR
wxStatusBar *wxFrame::OnCreateStatusBar(int number,
                                        long style,
                                        wxWindowID id,
                                        const wxString& name)
{
    wxStatusBar *statusBar = NULL;

    statusBar = wxFrameBase::OnCreateStatusBar(number, style, id, name);

    return statusBar;
}

void wxFrame::PositionStatusBar()
{
// TODO:
/*
    // native status bar positions itself
    if ( m_frameStatusBar )
    {
        int w, h;
        GetClientSize(&w, &h);
        int sw, sh;
        m_frameStatusBar->GetSize(&sw, &sh);

        // Since we wish the status bar to be directly under the client area,
        // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
        m_frameStatusBar->SetSize(0, h, w, sh);
    }
*/
}
#endif // wxUSE_STATUSBAR

void wxFrame::DetachMenuBar()
{
    if (m_frameMenuBar)
    {
        m_frameMenuBar->Detach();
        m_frameMenuBar = NULL;
    }
}

void wxFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    if (!menu_bar)
    {
        DetachMenuBar();
        return;
    }

    wxCHECK_RET( !menu_bar->GetFrame(), wxT("this menubar is already attached") );

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
// TODO:
/*
    if ( !::SetMenu(GetHwnd(), (HMENU)m_hMenu) )
    {
        wxLogLastError("SetMenu");
    }
*/
}

// Responds to colour changes, and passes event on to children.
void wxFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
// TODO:
/*
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
*/
}

/*
 * Frame window
 *
 */

bool wxFrame::OS2Create(int id, wxWindow *parent, const wxChar *wclass, wxWindow *wx_win, const wxChar *title,
                   int x, int y, int width, int height, long style)

{
  m_hDefaultIcon = (WXHICON) (wxSTD_FRAME_ICON ? wxSTD_FRAME_ICON : wxDEFAULT_FRAME_ICON);

  // If child windows aren't properly drawn initially, WS_CLIPCHILDREN
  // could be the culprit. But without it, you can get a lot of flicker.

// TODO:
/*
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
*/
  return TRUE;
}

// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxFrame::OnActivate(wxActivateEvent& event)
{
    for ( wxWindowList::Node *node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        // FIXME all this is totally bogus - we need to do the same as wxPanel,
        //       but how to do it without duplicating the code?

        // restore focus
        wxWindow *child = node->GetData();

        if ( !child->IsTopLevel()
#if wxUSE_TOOLBAR
             && !wxDynamicCast(child, wxToolBar)
#endif // wxUSE_TOOLBAR
#if wxUSE_STATUSBAR
             && !wxDynamicCast(child, wxStatusBar)
#endif // wxUSE_STATUSBAR
           )
        {
            child->SetFocus();
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// wxFrame size management: we exclude the areas taken by menu/status/toolbars
// from the client area, so the client area is what's really available for the
// frame contents
// ----------------------------------------------------------------------------

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

void wxFrame::DoScreenToClient(int *x, int *y) const
{
    wxWindow::DoScreenToClient(x, y);

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt(GetClientAreaOrigin());
    *x -= pt.x;
    *y -= pt.y;
}

void wxFrame::DoClientToScreen(int *x, int *y) const
{
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt1(GetClientAreaOrigin());
    *x += pt1.x;
    *y += pt1.y;

    wxWindow::DoClientToScreen(x, y);
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
// TODO:
/*
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

    if ( GetToolBar() )
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
*/
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

        if ( win->IsKindOf(CLASSINFO(wxFrame)) )
        {
            ((wxFrame *)win)->Iconize(bIconize);
        }
    }
}

// ===========================================================================
// message processing
// ===========================================================================

// ---------------------------------------------------------------------------
// preprocessing
// ---------------------------------------------------------------------------

bool wxFrame::OS2TranslateMessage(WXMSG* pMsg)
{
// TODO:
/*
    if ( wxWindow::OS2TranslateMessage(pMsg) )
        return TRUE;
*/
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
// TODO:
/*
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
*/
    return FALSE;
}

bool wxFrame::HandleSize(int x, int y, WXUINT id)
{
    bool processed = FALSE;

// TODO:
/*
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
*/
    return processed;
}

bool wxFrame::HandleCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
// TODO:
/*
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
*/
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
// TODO:
/*
    else if ( !(flags & MF_POPUP) && !(flags & MF_SEPARATOR) )
    {
        item = nItem;
    }
    else
    {
        // don't give hints for separators (doesn't make sense) nor for the
        // items opening popup menus (they don't have them anyhow)
        return FALSE;
    }
*/
    wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, item);
    event.SetEventObject( this );

    return GetEventHandler()->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// the window proc for wxFrame
// ---------------------------------------------------------------------------

MRESULT wxFrame::OS2WindowProc(HWND hwnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    long rc = 0;
    bool processed = FALSE;

// TODO:
/*
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
*/
    return (MRESULT)0;
}

