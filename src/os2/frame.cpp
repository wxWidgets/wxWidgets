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
    bool wxFrame::m_bUseNativeStatusBar = TRUE;
#else
    bool wxFrame::m_bUseNativeStatusBar = FALSE;
#endif

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
    m_bIconized = FALSE;

#if wxUSE_TOOLTIPS
    m_hWndToolTip = 0;
#endif
    // Data to save/restore when calling ShowFullScreen
    m_lFsStyle           = 0L;
    m_lFsOldWindowStyle  = 0L;
    m_nFsStatusBarFields = 0;
    m_nFsStatusBarHeight = 0;
    m_nFsToolBarHeight   = 0;
    m_bFsIsMaximized     = FALSE;
    m_bFsIsShowing       = FALSE;
    m_bIsShown           = FALSE;
    m_pWinLastFocused    = (wxWindow *)NULL;

    //
    // Initialize SWP's
    //
    memset(&m_vSwp, 0, sizeof(SWP));
    memset(&m_vSwpClient, 0, sizeof(SWP));
    memset(&m_vSwpTitleBar, 0, sizeof(SWP));
    memset(&m_vSwpMenuBar, 0, sizeof(SWP));
    memset(&m_vSwpHScroll, 0, sizeof(SWP));
    memset(&m_vSwpVScroll, 0, sizeof(SWP));
    memset(&m_vSwpStatusBar, 0, sizeof(SWP));
    memset(&m_vSwpToolBar, 0, sizeof(SWP));
} // end of wxFrame::Init

bool wxFrame::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsTitle
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lulStyle
, const wxString&                   rsName
)
{
    int                             nX      = rPos.x;
    int                             nY      = rPos.y;
    int                             nWidth  = rSize.x;
    int                             nHeight = rSize.y;

    SetName(rsName);
    m_windowStyle    = lulStyle;
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
              ,lulStyle
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
            ::WinPostMsg(NULL, WM_QUIT, 0, 0);
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

    //
    // PM has no GetClientRect that inherantly knows about the client window
    // We have to explicitly go fetch it!
    //
    ::WinQueryWindowRect(GetHwnd(), &vRect);

#if wxUSE_STATUSBAR
    if ( GetStatusBar() )
    {
        int                         nStatusX;
        int                         nStatusY;

        GetStatusBar()->GetClientSize( &nStatusX
                                      ,&nStatusY
                                     );
        vRect.yBottom += nStatusY;
    }
#endif // wxUSE_STATUSBAR

    wxPoint                         vPoint(GetClientAreaOrigin());

    vRect.yBottom += vPoint.y;
    vRect.xRight -= vPoint.x;

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
    RECTL                           vRect;
    RECTL                           vRect2;

    ::WinQueryWindowRect(GetHwnd(), &vRect);

    ::WinQueryWindowRect(GetHWND(), &vRect2);

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
    vPointl.y  = vRect2.yTop;

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

    ::WinQueryWindowRect(GetHWND(), &vRect);
    *pWidth = vRect.xRight - vRect.xLeft;
    *pHeight = vRect.yTop - vRect.yBottom;
} // end of wxFrame::DoGetSize

void wxFrame::DoGetPosition(
  int*                              pX
, int*                              pY
) const
{
    RECTL                           vRect;
    POINTL                          vPoint;

    ::WinQueryWindowRect(GetHWND(), &vRect);
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
  int                               bShowCmd
)
{
    ::WinShowWindow(GetHWND(), (BOOL)bShowCmd);
    m_bIconized = bShowCmd == SWP_MINIMIZE;
} // end of wxFrame::DoShowWindow

bool wxFrame::Show(
  bool                              bShow
)
{
    SWP                             vSwp;

    DoShowWindow((int)bShow);

    if (bShow)
    {
        wxActivateEvent             vEvent(wxEVT_ACTIVATE, TRUE, m_windowId);

        ::WinQueryWindowPos(GetHWND(), &vSwp);
        m_bIconized = vSwp.fl & SWP_MINIMIZE;
        ::WinEnableWindow(GetHWND(), TRUE);
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

            ::WinQueryWindowPos(hWndParent, &vSwp);
            m_bIconized = vSwp.fl & SWP_MINIMIZE;
            if (hWndParent)
                ::WinSetWindowPos( hWndParent
                                  ,HWND_TOP
                                  ,vSwp.x
                                  ,vSwp.y
                                  ,vSwp.cx
                                  ,vSwp.cy
                                  ,SWP_ZORDER | SWP_ACTIVATE | SWP_SHOW | SWP_MOVE
                                 );
            ::WinEnableWindow(hWndParent, TRUE);
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

    ::WinQueryWindowPos(GetHwnd(), &vSwp);

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

    ::WinQueryWindowPos(GetHWND(), &vSwp);
    return (vSwp.fl & SWP_MAXIMIZE);
} // end of wxFrame::IsMaximized

void wxFrame::SetIcon(
  const wxIcon&                     rIcon
)
{
    wxFrameBase::SetIcon(rIcon);

    if ((m_icon.GetHICON()) != NULLHANDLE)
    {
        ::WinSendMsg( GetHWND()
                     ,WM_SETICON
                     ,(MPARAM)((HPOINTER)m_icon.GetHICON())
                     ,NULL
                    );
        ::WinSendMsg( GetHWND()
                     ,WM_UPDATEFRAME
                     ,(MPARAM)FCF_ICON
                     ,(MPARAM)0
                    );
    }
} // end of wxFrame::SetIcon

#if wxUSE_STATUSBAR
wxStatusBar* wxFrame::OnCreateStatusBar(
  int                               nNumber
, long                              lulStyle
, wxWindowID                        vId
, const wxString&                   rName
)
{
    wxStatusBar*                    pStatusBar = NULL;
    SWP                             vSwp;
    ERRORID                         vError;
    wxString                        sError;
    HWND                            hWnd;

    pStatusBar = wxFrameBase::OnCreateStatusBar( nNumber
                                                ,lulStyle
                                                ,vId
                                                ,rName
                                               );
    //
    // The default parent set for the Statusbar is m_hWnd which, of course,
    // is the handle to the client window of the frame.  We don't want that,
    // so we have to set the parent to actually be the Frame.
    //
    hWnd = pStatusBar->GetHWND();
    if (!::WinSetParent(hWnd, GetHWND(), FALSE))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error setting parent for statusbar. Error: %s\n", sError);
        return NULL;
    }

    //
    // Also we need to reset it positioning to enable the SHOW attribute
    //
    if (!::WinQueryWindowPos((HWND)pStatusBar->GetHWND(), &vSwp))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error querying frame for statusbar position. Error: %s\n", sError);
        return NULL;
    }
    if (!::WinSetWindowPos( (HWND)pStatusBar->GetHWND()
                           ,HWND_TOP
                           ,vSwp.cx
                           ,vSwp.cy
                           ,vSwp.x
                           ,vSwp.y
                           ,SWP_SIZE | SWP_MOVE | SWP_SHOW | SWP_ZORDER
                          ))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error setting statusbar position. Error: %s\n", sError);
        return NULL;
    }
    return pStatusBar;
} // end of wxFrame::OnCreateStatusBar

void wxFrame::PositionStatusBar()
{
    SWP                             vSwp;
    ERRORID                         vError;
    wxString                        sError;

    //
    // Native status bar positions itself
    //
    if (m_frameStatusBar)
    {
        int                         nWidth;
        int                         nHeight;
        int                         nStatbarWidth;
        int                         nStatbarHeight;
        HWND                        hWndClient;
        RECTL                       vRect;

        ::WinQueryWindowRect(GetHwnd(), &vRect);
        nWidth = vRect.xRight - vRect.xLeft;
        nHeight = vRect.yTop - vRect.yBottom;

        m_frameStatusBar->GetSize( &nStatbarWidth
                                  ,&nStatbarHeight
                                 );

        //
        // Since we wish the status bar to be directly under the client area,
        // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
        //
        m_frameStatusBar->SetSize( 0
                                  ,nHeight
                                  ,nWidth
                                  ,nStatbarHeight
                                 );
        if (!::WinQueryWindowPos(m_frameStatusBar->GetHWND(), &vSwp))
        {
            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            wxLogError("Error setting parent for submenu. Error: %s\n", sError);
            return;
        }
        if (!::WinSetWindowPos( m_frameStatusBar->GetHWND()
                               ,HWND_TOP
                               ,nStatbarWidth
                               ,nStatbarHeight
                               ,vSwp.x
                               ,vSwp.y
                               ,SWP_SIZE | SWP_MOVE | SWP_SHOW | SWP_ZORDER
                              ))
        {
            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            wxLogError("Error setting parent for submenu. Error: %s\n", sError);
            return;
        }
    }
} // end of wxFrame::PositionStatusBar
#endif // wxUSE_STATUSBAR

void wxFrame::DetachMenuBar()
{
    if (m_frameMenuBar)
    {
        m_frameMenuBar->Detach();
        m_frameMenuBar = NULL;
    }
} // end of wxFrame::DetachMenuBar

void wxFrame::SetMenuBar(
  wxMenuBar*                        pMenuBar
)
{
    ERRORID                         vError;
    wxString                        sError;
    HWND                            hClient = NULLHANDLE;
    HWND                            hFrame = NULLHANDLE;
    HWND                            hTitlebar = NULLHANDLE;
    HWND                            hHScroll = NULLHANDLE;
    HWND                            hVScroll = NULLHANDLE;
    HWND                            hMenuBar = NULLHANDLE;
    SWP                             vSwp;
    SWP                             vSwpTitlebar;
    SWP                             vSwpVScroll;
    SWP                             vSwpHScroll;
    SWP                             vSwpMenu;

    if (!pMenuBar)
    {
        DetachMenuBar();

        //
        // Actually remove the menu from the frame
        //
        m_hMenu = (WXHMENU)0;
        InternalSetMenuBar();
    }
    else // set new non NULL menu bar
    {
        m_frameMenuBar = NULL;

        //
        // Can set a menubar several times.
        // TODO: how to prevent a memory leak if you have a currently-unattached
        // menubar? wxWindows assumes that the frame will delete the menu (otherwise
        // there are problems for MDI).
        //
        if (pMenuBar->GetHMenu())
        {
            m_hMenu = pMenuBar->GetHMenu();
        }
        else
        {
            pMenuBar->Detach();
            m_hMenu = pMenuBar->Create();
            if (!m_hMenu)
                return;
        }
        InternalSetMenuBar();
        m_frameMenuBar = pMenuBar;
        pMenuBar->Attach(this);
    }

    //
    // Now resize the client to fit the new frame
    //
    WinQueryWindowPos(GetHWND(), &vSwp);
    hTitlebar = WinWindowFromID(GetHWND(), FID_TITLEBAR);
    WinQueryWindowPos(hTitlebar, &vSwpTitlebar);
    hHScroll = WinWindowFromID(GetHWND(), FID_HORZSCROLL);
    WinQueryWindowPos(hHScroll, &vSwpHScroll);
    hVScroll = WinWindowFromID(GetHWND(), FID_VERTSCROLL);
    WinQueryWindowPos(hVScroll, &vSwpVScroll);
    hMenuBar = WinWindowFromID(GetHWND(), FID_MENU);
    WinQueryWindowPos(hMenuBar, &vSwpMenu);
    WinSetWindowPos( GetHwnd()
                    ,HWND_TOP
                    ,SV_CXSIZEBORDER/2
                    ,(SV_CYSIZEBORDER/2) + vSwpHScroll.cy/2
                    ,vSwp.cx - ((SV_CXSIZEBORDER + 1) + vSwpVScroll.cx)
                    ,vSwp.cy - ((SV_CYSIZEBORDER + 1) + vSwpTitlebar.cy + vSwpMenu.cy + vSwpHScroll.cy/2)
                    ,SWP_SIZE | SWP_MOVE
                   );
} // end of wxFrame::SetMenuBar

void wxFrame::InternalSetMenuBar()
{
    ERRORID                         vError;
    wxString                        sError;
    //
    // Set the parent and owner of the menubar to be the frame
    //
    if (!::WinSetParent(m_hMenu, GetHWND(), FALSE))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error setting parent for submenu. Error: %s\n", sError);
    }

    if (!::WinSetOwner(m_hMenu, GetHWND()))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error setting parent for submenu. Error: %s\n", sError);
    }
    WinSendMsg((HWND)GetHWND(), WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
} // end of wxFrame::InternalSetMenuBar

//
// Responds to colour changes, and passes event on to children
//
void wxFrame::OnSysColourChanged(
  wxSysColourChangedEvent&          rEvent
)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
    Refresh();

    if (m_frameStatusBar)
    {
        wxSysColourChangedEvent     vEvent2;

        vEvent2.SetEventObject(m_frameStatusBar);
        m_frameStatusBar->GetEventHandler()->ProcessEvent(vEvent2);
    }

    //
    // Propagate the event to the non-top-level children
    //
    wxWindow::OnSysColourChanged(rEvent);
} // end of wxFrame::OnSysColourChanged

// Pass TRUE to show full screen, FALSE to restore.
bool wxFrame::ShowFullScreen(
  bool                              bShow
, long                              lStyle
)
{
    if (bShow)
    {
        if (IsFullScreen())
            return FALSE;

        m_bFsIsShowing = TRUE;
        m_lFsStyle = lStyle;

	    wxToolBar*                  pTheToolBar = GetToolBar();
	    wxStatusBar*                pTheStatusBar = GetStatusBar();

        int                         nDummyWidth;

        if (pTheToolBar)
            pTheToolBar->GetSize(&nDummyWidth, &m_nFsToolBarHeight);
        if (pTheStatusBar)
            pTheStatusBar->GetSize(&nDummyWidth, &m_nFsStatusBarHeight);

        //
        // Zap the toolbar, menubar, and statusbar
        //
        if ((lStyle & wxFULLSCREEN_NOTOOLBAR) && pTheToolBar)
        {
            pTheToolBar->SetSize(-1,0);
            pTheToolBar->Show(FALSE);
        }

        if (lStyle & wxFULLSCREEN_NOMENUBAR)
        {
            ::WinSetParent(m_hMenu, GetHWND(), FALSE);
            ::WinSetOwner(m_hMenu, GetHWND());
            ::WinSendMsg((HWND)GetHWND(), WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
        }

        //
        // Save the number of fields in the statusbar
        //
        if ((lStyle & wxFULLSCREEN_NOSTATUSBAR) && pTheStatusBar)
        {
            m_nFsStatusBarFields = pTheStatusBar->GetFieldsCount();
            SetStatusBar((wxStatusBar*) NULL);
            delete pTheStatusBar;
        }
        else
            m_nFsStatusBarFields = 0;

        //
        // Zap the frame borders
        //

        //
        // Save the 'normal' window style
        //
        m_lFsOldWindowStyle = ::WinQueryWindowULong((HWND)GetHWND(), QWL_STYLE);

        //
	    // Save the old position, width & height, maximize state
        //
        m_vFsOldSize = GetRect();
        m_bFsIsMaximized = IsMaximized();

        //
	     // Decide which window style flags to turn off
        //
        LONG                        lNewStyle = m_lFsOldWindowStyle;
        LONG                        lOffFlags = 0;

        if (lStyle & wxFULLSCREEN_NOBORDER)
            lOffFlags |= FCF_BORDER;
        if (lStyle & wxFULLSCREEN_NOCAPTION)
            lOffFlags |= (FCF_TASKLIST | FCF_SYSMENU);

        lNewStyle &= (~lOffFlags);

        //
        // Change our window style to be compatible with full-screen mode
        //
        ::WinSetWindowULong((HWND)GetHWND(), QWL_STYLE, (ULONG)lNewStyle);

        //
        // Resize to the size of the desktop
        int                         nWidth;
        int                         nHeight;

        RECTL                       vRect;

        ::WinQueryWindowRect(HWND_DESKTOP, &vRect);
        nWidth = vRect.xRight - vRect.xLeft;
        //
        // Rmember OS/2 is backwards!
        //
        nHeight = vRect.yTop - vRect.yBottom;

        SetSize( nWidth
                ,nHeight
               );

        //
        // Now flush the window style cache and actually go full-screen
        //
        ::WinSetWindowPos( (HWND) GetParent()->GetHWND()
                          ,HWND_TOP
                          ,0
                          ,0
                          ,nWidth
                          ,nHeight
                          ,SWP_SIZE | SWP_SHOW
                         );

        wxSizeEvent                 vEvent( wxSize( nWidth
                                                   ,nHeight
                                                  )
                                           ,GetId()
                                          );

        GetEventHandler()->ProcessEvent(vEvent);
        return TRUE;
    }
    else
    {
        if (!IsFullScreen())
            return FALSE;

        m_bFsIsShowing = FALSE;

        wxToolBar*                  pTheToolBar = GetToolBar();

        //
        // Restore the toolbar, menubar, and statusbar
        //
        if (pTheToolBar && (m_lFsStyle & wxFULLSCREEN_NOTOOLBAR))
        {
            pTheToolBar->SetSize(-1, m_nFsToolBarHeight);
            pTheToolBar->Show(TRUE);
        }

        if ((m_lFsStyle & wxFULLSCREEN_NOSTATUSBAR) && (m_nFsStatusBarFields > 0))
        {
            CreateStatusBar(m_nFsStatusBarFields);
            PositionStatusBar();
        }

        if ((m_lFsStyle & wxFULLSCREEN_NOMENUBAR) && (m_hMenu != 0))
        {
            ::WinSetParent(m_hMenu, GetHWND(), FALSE);
            ::WinSetOwner(m_hMenu, GetHWND());
            ::WinSendMsg((HWND)GetHWND(), WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
        }
        Maximize(m_bFsIsMaximized);

        ::WinSetWindowULong( (HWND)GetHWND()
                            ,QWL_STYLE
                            ,(ULONG)m_lFsOldWindowStyle
                           );
        ::WinSetWindowPos( (HWND) GetParent()->GetHWND()
                          ,HWND_TOP
                          ,m_vFsOldSize.x
                          ,m_vFsOldSize.y
                          ,m_vFsOldSize.width
                          ,m_vFsOldSize.height
                          ,SWP_SIZE | SWP_SHOW
                         );
        return TRUE;
    }
} // end of wxFrame::ShowFullScreen

//
// Frame window
//
bool wxFrame::OS2Create(
  int                               nId
, wxWindow*                         pParent
, const wxChar*                     zWclass
, wxWindow*                         pWxWin
, const wxChar*                     zTitle
, int                               nX
, int                               nY
, int                               nWidth
, int                               nHeight
, long                              ulStyle
)
{
    ULONG                           ulCreateFlags = 0L;
    ULONG                           ulStyleFlags = 0L;
    ULONG                           ulExtraFlags = 0L;
    FRAMECDATA                      vFrameCtlData;
    HWND                            hParent = NULLHANDLE;
    HWND                            hClient = NULLHANDLE;
    HWND                            hFrame = NULLHANDLE;
    HWND                            hTitlebar = NULLHANDLE;
    HWND                            hHScroll = NULLHANDLE;
    HWND                            hVScroll = NULLHANDLE;
    SWP                             vSwp[10];
    RECTL                           vRect[10];
    USHORT                          uCtlCount;

    m_hDefaultIcon = (WXHICON) (wxSTD_FRAME_ICON ? wxSTD_FRAME_ICON : wxDEFAULT_FRAME_ICON);

    if (pParent)
        hParent = GetWinHwnd(pParent);
    else
        hParent = HWND_DESKTOP;

    if (ulStyle == wxDEFAULT_FRAME_STYLE)
        ulCreateFlags = FCF_SIZEBORDER | FCF_TITLEBAR | FCF_SYSMENU |
                        FCF_MINMAX | FCF_TASKLIST;
    else
    {
        if ((ulStyle & wxCAPTION) == wxCAPTION)
            ulCreateFlags = FCF_TASKLIST;
        else
            ulCreateFlags = FCF_NOMOVEWITHOWNER;

        if ((ulStyle & wxVSCROLL) == wxVSCROLL)
            ulCreateFlags |= FCF_VERTSCROLL;
        if ((ulStyle & wxHSCROLL) == wxHSCROLL)
            ulCreateFlags |= FCF_HORZSCROLL;
        if (ulStyle & wxMINIMIZE_BOX)
            ulCreateFlags |= FCF_MINBUTTON;
        if (ulStyle & wxMAXIMIZE_BOX)
            ulCreateFlags |= FCF_MAXBUTTON;
        if (ulStyle & wxTHICK_FRAME)
            ulCreateFlags |= FCF_DLGBORDER;
        if (ulStyle & wxSYSTEM_MENU)
            ulCreateFlags |= FCF_SYSMENU;
        if (ulStyle & wxCAPTION)
            ulCreateFlags |= FCF_TASKLIST;
        if (ulStyle & wxCLIP_CHILDREN)
        {
            // Invalid for frame windows under PM
        }

        if (ulStyle & wxTINY_CAPTION_VERT)
            ulCreateFlags |= FCF_TASKLIST;
        if (ulStyle & wxTINY_CAPTION_HORIZ)
            ulCreateFlags |= FCF_TASKLIST;

        if ((ulStyle & wxTHICK_FRAME) == 0)
            ulCreateFlags |= FCF_BORDER;
        if (ulStyle & wxFRAME_TOOL_WINDOW)
            ulExtraFlags = kFrameToolWindow;

        if (ulStyle & wxSTAY_ON_TOP)
            ulCreateFlags |= FCF_SYSMODAL;
    }
    if ((ulStyle & wxMINIMIZE) || (ulStyle & wxICONIZE))
        ulStyleFlags |= WS_MINIMIZED;
    if (ulStyle & wxMAXIMIZE)
        ulStyleFlags |= WS_MAXIMIZED;

    //
    // Clear the visible flag, we always call show
    //
    ulStyleFlags &= (unsigned long)~WS_VISIBLE;
    m_bIconized = FALSE;

    //
    // Set the frame control block
    //
    vFrameCtlData.cb            = sizeof(vFrameCtlData);
    vFrameCtlData.flCreateFlags = ulCreateFlags;
    vFrameCtlData.hmodResources = 0L;
    vFrameCtlData.idResources   = 0;

    //
    // Create the frame window
    //

    if (!wxWindow::OS2Create( hParent
                             ,WC_FRAME
                             ,(PSZ)zTitle
                             ,0
                             ,0, 0, 0, 0
                             ,NULLHANDLE
                             ,HWND_TOP
                             ,(ULONG)nId
                             ,(PVOID)&vFrameCtlData
                             ,NULL
                            ))
    {
        return FALSE;
    }

    //
    // Now size everything.  If adding a menu the client will need to be resized.
    //

    if (!::WinSetWindowPos( GetHWND()
                           ,HWND_TOP
                           ,nX
                           ,nY
                           ,nWidth
                           ,nHeight
                           ,SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_ZORDER
                          ))
        return FALSE;

    uCtlCount = SHORT1FROMMP(::WinSendMsg(GetHWND(), WM_FORMATFRAME, (MPARAM)vSwp, (MPARAM)vRect));
    for (int i = 0; i < uCtlCount; i++)
    {
        if (vSwp[i].hwnd == GetHWND())
            memcpy(&m_vSwp, &vSwp[i], sizeof(SWP));
        else if (vSwp[i].hwnd == m_hVScroll)
            memcpy(&m_vSwpVScroll, &vSwp[i], sizeof(SWP));
        else if (vSwp[i].hwnd == m_hHScroll)
            memcpy(&m_vSwpVScroll, &vSwp[i], sizeof(SWP));
        else if (vSwp[i].hwnd == m_hTitleBar)
            memcpy(&m_vSwpTitleBar, &vSwp[i], sizeof(SWP));
    }
    return TRUE;
} // end of wxFrame::OS2Create

//
// Default activation behaviour - set the focus for the first child
// subwindow found.
//
void wxFrame::OnActivate(
  wxActivateEvent&                  rEvent
)
{
    for (wxWindowList::Node* pNode = GetChildren().GetFirst();
         pNode;
         pNode = pNode->GetNext())
    {
        // FIXME all this is totally bogus - we need to do the same as wxPanel,
        //       but how to do it without duplicating the code?

        // restore focus
        wxWindow*                   pChild = pNode->GetData();

        if (!pChild->IsTopLevel()
#if wxUSE_TOOLBAR
             && !wxDynamicCast(pChild, wxToolBar)
#endif // wxUSE_TOOLBAR
#if wxUSE_STATUSBAR
             && !wxDynamicCast(pChild, wxStatusBar)
#endif // wxUSE_STATUSBAR
           )
        {
            pChild->SetFocus();
            return;
        }
    }
} // end of wxFrame::OnActivate

// ----------------------------------------------------------------------------
// wxFrame size management: we exclude the areas taken by menu/status/toolbars
// from the client area, so the client area is what's really available for the
// frame contents
// ----------------------------------------------------------------------------

// Checks if there is a toolbar, and returns the first free client position
wxPoint wxFrame::GetClientAreaOrigin() const
{
    wxPoint                         vPoint(0, 0);

    if (GetToolBar())
    {
        int                         nWidth;
        int                         nHeight;

        GetToolBar()->GetSize( &nWidth
                              ,&nHeight
                             );

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            vPoint.x += nWidth;
        }
        else
        {
            // PM is backwards from windows
            vPoint.y += nHeight;
        }
    }
    return vPoint;
} // end of wxFrame::GetClientAreaOrigin

// ----------------------------------------------------------------------------
// tool/status bar stuff
// ----------------------------------------------------------------------------

#if wxUSE_TOOLBAR

wxToolBar* wxFrame::CreateToolBar(
  long                              lStyle
, wxWindowID                        vId
, const wxString&                   rName
)
{
    if (wxFrameBase::CreateToolBar( lStyle
                                   ,vId
                                   ,rName
                                  ))
    {
        PositionToolBar();
    }
    return m_frameToolBar;
} // end of wxFrame::CreateToolBar

void wxFrame::PositionToolBar()
{
    HWND                            hWndClient;
    RECTL                           vRect;

    ::WinQueryWindowRect(GetHwnd(), &vRect);

#if wxUSE_STATUSBAR
    if (GetStatusBar())
    {
        int                         nStatusX;
        int                         nStatusY;

        GetStatusBar()->GetClientSize( &nStatusX
                                      ,&nStatusY
                                     );
        // PM is backwards from windows
        vRect.yBottom += nStatusY;
    }
#endif // wxUSE_STATUSBAR

    if ( GetToolBar() )
    {
        int                         nToolbarWidth;
        int                         nToolbarHeight;

        GetToolBar()->GetSize( &nToolbarWidth
                              ,&nToolbarHeight
                             );

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            nToolbarHeight = vRect.yBottom;
        }
        else
        {
            nToolbarWidth = vRect.xRight;
        }

        //
        // Use the 'real' PM position here
        //
        GetToolBar()->SetSize( 0
                              ,0
                              ,nToolbarWidth
                              ,nToolbarHeight
                              ,wxSIZE_NO_ADJUSTMENTS
                             );
    }
} // end of wxFrame::PositionToolBar
#endif // wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// frame state (iconized/maximized/...)
// ----------------------------------------------------------------------------

//
// propagate our state change to all child frames: this allows us to emulate X
// Windows behaviour where child frames float independently of the parent one
// on the desktop, but are iconized/restored with it
//
void wxFrame::IconizeChildFrames(
  bool                              bIconize
)
{
    for (wxWindowList::Node* pNode = GetChildren().GetFirst();
         pNode;
         pNode = pNode->GetNext() )
    {
        wxWindow*                   pWin = pNode->GetData();

        if (pWin->IsKindOf(CLASSINFO(wxFrame)) )
        {
            ((wxFrame *)pWin)->Iconize(bIconize);
        }
    }
} // end of wxFrame::IconizeChildFrames

// ===========================================================================
// message processing
// ===========================================================================

// ---------------------------------------------------------------------------
// preprocessing
// ---------------------------------------------------------------------------
bool wxFrame::OS2TranslateMessage(
  WXMSG*                            pMsg
)
{
    //
    // try the menu bar accels
    //
    wxMenuBar*                      pMenuBar = GetMenuBar();

    if (!pMenuBar )
        return FALSE;

    const wxAcceleratorTable&       rAcceleratorTable = pMenuBar->GetAccelTable();
    return rAcceleratorTable.Translate(GetHWND(), pMsg);
} // end of wxFrame::OS2TranslateMessage

// ---------------------------------------------------------------------------
// our private (non virtual) message handlers
// ---------------------------------------------------------------------------
bool wxFrame::HandlePaint()
{
    RECTL                           vRect;

    if (::WinQueryUpdateRect(GetHWND(), &vRect))
    {
        if (m_bIconized)
        {
            //
            // Icons in PM are the same as "pointers"
            //
            HPOINTER                hIcon;

            if (m_icon.Ok())
                hIcon = (HPOINTER)::WinSendMsg(GetHWND(), WM_QUERYICON, 0L, 0L);
            else
                hIcon = (HPOINTER)m_hDefaultIcon;

            //
            // Hold a pointer to the dc so long as the OnPaint() message
            // is being processed
            //
            RECTL                   vRect2;
            HPS                     hPs = ::WinBeginPaint(GetHwnd(), NULLHANDLE, &vRect2);

            //
            // Erase background before painting or we get white background
            //
            OS2DefWindowProc(WM_ERASEBACKGROUND, (MPARAM)hPs, (MPARAM)&vRect2);

            if (hIcon)
            {
                HWND                            hWndClient;
                RECTL                           vRect3;

                ::WinQueryWindowRect(GetHwnd(), &vRect3);

                static const int    nIconWidth = 32;
                static const int    nIconHeight = 32;
                int                 nIconX = (int)((vRect3.xRight - nIconWidth)/2);
                int                 nIconY = (int)((vRect3.yBottom + nIconHeight)/2);

                ::WinDrawPointer(hPs, nIconX, nIconY, hIcon, DP_NORMAL);
            }
            ::WinEndPaint(hPs);
            return TRUE;
        }
        else
        {
/*          DosBeep(500,500);
            HPS                             hPS;
            RECTL                           vRect;

            hPS = WinBeginPaint(GetHwnd(), 0L, &vRect);
            WinFillRect(hPS, &vRect, SYSCLR_WINDOW);
            WinEndPaint(hPS);*/

            return wxWindow::HandlePaint();
        }
    }
    else
    {
        // nothing to paint - processed
        return TRUE;
    }
    return FALSE;
} // end of wxFrame::HandlePaint

bool wxFrame::HandleSize(
  int                               nX
, int                               nY
, WXUINT                            nId
)
{
    bool                            bProcessed = FALSE;

    switch (nId)
    {
        case kSizeNormal:
            //
            // Only do it it if we were iconized before, otherwise resizing the
            // parent frame has a curious side effect of bringing it under it's
            // children
            if (!m_bIconized )
                break;

            //
            // restore all child frames too
            //
            IconizeChildFrames(FALSE);

            //
            // fall through
            //

        case kSizeMax:
            m_bIconized = FALSE;
            break;

        case kSizeMin:
            //
            // Iconize all child frames too
            //
            IconizeChildFrames(TRUE);
            m_bIconized = TRUE;
            break;
    }

    if (!m_bIconized)
    {
        //
        // forward WM_SIZE to status bar control
        //
#if wxUSE_NATIVE_STATUSBAR
        if (m_frameStatusBar && m_frameStatusBar->IsKindOf(CLASSINFO(wxStatusBar95)))
        {
            wxSizeEvent             vEvent( wxSize( nX
                                                   ,nY
                                                  )
                                           ,m_frameStatusBar->GetId()
                                          );

            vEvent.SetEventObject(m_frameStatusBar);
            m_frameStatusBar->OnSize(vEvent);
        }
#endif // wxUSE_NATIVE_STATUSBAR

        PositionStatusBar();
        PositionToolBar();
        wxSizeEvent                 vEvent( wxSize( nX
                                                   ,nY
                                                  )
                                           ,m_windowId
                                          );

        vEvent.SetEventObject(this);
        bProcessed = GetEventHandler()->ProcessEvent(vEvent);
    }
    return bProcessed;
} // end of wxFrame::HandleSize

bool wxFrame::HandleCommand(
  WXWORD                            nId
, WXWORD                            nCmd
, WXHWND                            hControl
)
{
    if (hControl)
    {
        //
        // In case it's e.g. a toolbar.
        //
        wxWindow*                   pWin = wxFindWinFromHandle(hControl);

        if (pWin)
            return pWin->OS2Command( nCmd
                                    ,nId
                                   );
    }

    //
    // Handle here commands from menus and accelerators
    //
    if (nCmd == 0 || nCmd == 1)
    {
        if (wxCurrentPopupMenu)
        {
            wxMenu*                 pPopupMenu = wxCurrentPopupMenu;

            wxCurrentPopupMenu = NULL;

            return pPopupMenu->OS2Command( nCmd
                                          ,nId
                                         );
        }

        if (ProcessCommand(nId))
        {
            return TRUE;
        }
    }
    return FALSE;
} // end of wxFrame::HandleCommand

bool wxFrame::HandleMenuSelect(
  WXWORD                            nItem
, WXWORD                            nFlags
, WXHMENU                           hMenu
)
{
//    int                             nMenuItem;

/*  This is wrong section according to IBM's documentation
    if (nFlags == 0xFFFF && hMenu == 0)
    {
        //
        // Menu was removed from screen
        //
        nMenuItem = -1;
    }
    else if (!(nFlags & MIS_SUBMENU) && !(nFlags & MIS_SEPARATOR))
    {
        nMenuItem = nItem;
    }
    else
    {
        //
        // Don't give hints for separators (doesn't make sense) nor for the
        // items opening popup menus (they don't have them anyhow)
        //
        return FALSE;
    }
*/

    if( !nFlags )
    {
        MENUITEM mItem;
        MRESULT  rc;

        rc = WinSendMsg(hMenu, MM_QUERYITEM, MPFROM2SHORT(nItem, TRUE), (MPARAM)&mItem);

        if(rc && !(mItem.afStyle & (MIS_SUBMENU | MIS_SEPARATOR)))
        {
            wxMenuEvent                     vEvent(wxEVT_MENU_HIGHLIGHT, nItem);

            vEvent.SetEventObject(this);
            GetEventHandler()->ProcessEvent(vEvent); // return value would be ignored by PM
        }
    }
    return TRUE;
} // end of wxFrame::HandleMenuSelect

// ---------------------------------------------------------------------------
// the window proc for wxFrame
// ---------------------------------------------------------------------------

MRESULT wxFrame::OS2WindowProc(
  WXUINT                            uMessage
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    MRESULT                         mRc = 0L;
    bool                            bProcessed = FALSE;

    switch (uMessage)
    {
        case WM_CLOSE:
            //
            // If we can't close, tell the system that we processed the
            // message - otherwise it would close us
            //
            bProcessed = !Close();
            break;

        case WM_COMMAND:
            {
                WORD                wId;
                WORD                wCmd;
                WXHWND              hWnd;

                UnpackCommand( (WXWPARAM)wParam
                              ,(WXLPARAM)lParam
                              ,&wId
                              ,&hWnd
                              ,&wCmd
                             );
                bProcessed = HandleCommand( wId
                                           ,wCmd
                                           ,(WXHWND)hWnd
                                          );
            }
            break;

        case WM_MENUSELECT:
            {
                WXWORD              wItem;
                WXWORD              wFlags;
                WXHMENU             hMenu;

                UnpackMenuSelect( wParam
                                 ,lParam
                                 ,&wItem
                                 ,&wFlags
                                 ,&hMenu
                                );
                bProcessed = HandleMenuSelect( wItem
                                              ,wFlags
                                              ,hMenu
                                             );
                mRc = (MRESULT)TRUE;
            }
            break;

        case WM_PAINT:
            bProcessed = HandlePaint();
            break;

        case WM_ERASEBACKGROUND:
            //
            // Return TRUE to request PM to paint the window background
            // in SYSCLR_WINDOW.
            //
            bProcessed = TRUE;
            mRc = (MRESULT)(TRUE);
            break;

        case CM_QUERYDRAGIMAGE:
            {
                HPOINTER                hIcon;

                if (m_icon.Ok())
                    hIcon = (HPOINTER)::WinSendMsg(GetHWND(), WM_QUERYICON, 0L, 0L);
                else
                    hIcon = (HPOINTER)m_hDefaultIcon;
                mRc = (MRESULT)hIcon;
                bProcessed = mRc != 0;
            }
            break;

        case WM_SIZE:
            bProcessed = HandleSize(LOWORD(lParam), HIWORD(lParam), (WXUINT)wParam);
            break;
    }

    if (!bProcessed )
        mRc = wxWindow::OS2WindowProc( uMessage
                                      ,wParam
                                      ,lParam
                                     );
    return (MRESULT)mRc;
} // wxFrame::OS2WindowProc

