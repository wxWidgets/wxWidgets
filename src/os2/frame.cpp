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
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/dynarray.h"
    #include "wx/list.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/event.h"
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

extern void  wxAssociateWinWithHandle( HWND      hWnd
                                      ,wxWindow* pWin
                                     );

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
#if wxUSE_STATUSBAR

#if wxUSE_NATIVE_STATUSBAR
    bool wxFrame::m_bUseNativeStatusBar = TRUE;
#else
    bool wxFrame::m_bUseNativeStatusBar = FALSE;
#endif

#endif //wxUSE_STATUSBAR

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

    m_hFrame    = NULL;
    m_hTitleBar = NULL;
    m_hHScroll  = NULL;
    m_hVScroll  = NULL;

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
    bool                            bOk = FALSE;

    SetName(rsName);
    m_windowStyle    = lulStyle;
    m_frameMenuBar   = NULL;
#if wxUSE_TOOLBAR
    m_frameToolBar   = NULL;
#endif //wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
    m_frameStatusBar = NULL;
#endif //wxUSE_STATUSBAR

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

    bOk = OS2Create( m_windowId
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
    if (bOk)
    {
        if (!pParent)
            wxTopLevelWindows.Append(this);
        wxModelessWindows.Append(this);
    }
    return(bOk);
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
    RECTL                           vRect;
    ::WinQueryWindowRect(GetHwnd(), &vRect);
    if (pX)
        *pX = vRect.xRight - vRect.xLeft;
    if (pY)
        *pY = vRect.yTop - vRect.yBottom;
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
    ::WinQueryWindowRect(GetHwnd(), &vRect2);

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

    ::WinQueryWindowRect(m_hFrame, &vRect);
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

    ::WinQueryWindowRect(m_hFrame, &vRect);
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
    ::WinShowWindow(m_hFrame, (BOOL)bShowCmd);
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

        ::WinQueryWindowPos(m_hFrame, &vSwp);
        m_bIconized = vSwp.fl & SWP_MINIMIZE;
        ::WinSendMsg(m_hFrame, WM_UPDATEFRAME, (MPARAM)~0, 0);
        ::WinEnableWindow(m_hFrame, TRUE);
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

    ::WinQueryWindowPos(m_hFrame, &vSwp);

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

    ::WinQueryWindowPos(m_hFrame, &vSwp);
    return (vSwp.fl & SWP_MAXIMIZE);
} // end of wxFrame::IsMaximized

void wxFrame::SetIcon(
  const wxIcon&                     rIcon
)
{
    wxFrameBase::SetIcon(rIcon);

    if ((m_icon.GetHICON()) != NULLHANDLE)
    {
        ::WinSendMsg( m_hFrame
                     ,WM_SETICON
                     ,(MPARAM)((HPOINTER)m_icon.GetHICON())
                     ,NULL
                    );
        ::WinSendMsg( m_hFrame
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

    pStatusBar = wxFrameBase::OnCreateStatusBar( nNumber
                                                ,lulStyle
                                                ,vId
                                                ,rName
                                                 );

    if( !pStatusBar )
        return NULL;

    ::WinSetParent( pStatusBar->GetHWND()
                   ,m_hFrame
                   ,FALSE
                  );
    ::WinSetOwner( pStatusBar->GetHWND()
                  ,m_hFrame
                 );
    //
    // to show statusbar
    //
    if(::WinIsWindowShowing(m_hFrame))
        ::WinSendMsg(m_hFrame, WM_UPDATEFRAME, (MPARAM)~0, 0);

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
        int                         nStatbarWidth;
        int                         nStatbarHeight;
        HWND                        hWndClient;
        RECTL                       vRect;
        RECTL                       vFRect;

        ::WinQueryWindowRect(m_hFrame, &vRect);
        ::WinMapWindowPoints(m_hFrame, HWND_DESKTOP, (PPOINTL)&vRect, 2);
        vFRect = vRect;
        ::WinCalcFrameRect(m_hFrame, &vRect, TRUE);
        nWidth = vRect.xRight - vRect.xLeft;

        m_frameStatusBar->GetSize( &nStatbarWidth
                                  ,&nStatbarHeight
                                 );

        //
        // Since we wish the status bar to be directly under the client area,
        // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
        //
        m_frameStatusBar->SetSize( vRect.xLeft - vFRect.xLeft
                                  ,vRect.yBottom - vFRect.yBottom
                                  ,nWidth
                                  ,nStatbarHeight
                                 );
        if (!::WinQueryWindowPos(m_frameStatusBar->GetHWND(), &vSwp))
        {
            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            wxLogError("Error setting parent for StautsBar. Error: %s\n", sError);
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
} // end of wxFrame::SetMenuBar

void wxFrame::InternalSetMenuBar()
{
    ERRORID                         vError;
    wxString                        sError;
    //
    // Set the parent and owner of the menubar to be the frame
    //
    if (!::WinSetParent(m_hMenu, m_hFrame, FALSE))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error setting parent for submenu. Error: %s\n", sError);
    }

    if (!::WinSetOwner(m_hMenu, m_hFrame))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error setting parent for submenu. Error: %s\n", sError);
    }
    ::WinSendMsg(m_hFrame, WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
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

#if wxUSE_STATUSBAR
    if (m_frameStatusBar)
    {
        wxSysColourChangedEvent     vEvent2;

        vEvent2.SetEventObject(m_frameStatusBar);
        m_frameStatusBar->GetEventHandler()->ProcessEvent(vEvent2);
    }
#endif //wxUSE_STATUSBAR

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

#if wxUSE_TOOLBAR
	    wxToolBar*                  pTheToolBar = GetToolBar();
#endif //wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
	    wxStatusBar*                pTheStatusBar = GetStatusBar();
#endif //wxUSE_STATUSBAR

        int                         nDummyWidth;

#if wxUSE_TOOLBAR
        if (pTheToolBar)
            pTheToolBar->GetSize(&nDummyWidth, &m_nFsToolBarHeight);
#endif //wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
        if (pTheStatusBar)
            pTheStatusBar->GetSize(&nDummyWidth, &m_nFsStatusBarHeight);
#endif //wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
        //
        // Zap the toolbar, menubar, and statusbar
        //
        if ((lStyle & wxFULLSCREEN_NOTOOLBAR) && pTheToolBar)
        {
            pTheToolBar->SetSize(-1,0);
            pTheToolBar->Show(FALSE);
        }
#endif //wxUSE_TOOLBAR

        if (lStyle & wxFULLSCREEN_NOMENUBAR)
        {
            ::WinSetParent(m_hMenu, m_hFrame, FALSE);
            ::WinSetOwner(m_hMenu, m_hFrame);
            ::WinSendMsg((HWND)m_hFrame, WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
        }

#if wxUSE_STATUSBAR
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
#endif //wxUSE_STATUSBAR

        //
        // Zap the frame borders
        //

        //
        // Save the 'normal' window style
        //
        m_lFsOldWindowStyle = ::WinQueryWindowULong(m_hFrame, QWL_STYLE);

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
        ::WinSetWindowULong((HWND)m_hFrame, QWL_STYLE, (ULONG)lNewStyle);

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

#if wxUSE_TOOLBAR
        wxToolBar*                  pTheToolBar = GetToolBar();

        //
        // Restore the toolbar, menubar, and statusbar
        //
        if (pTheToolBar && (m_lFsStyle & wxFULLSCREEN_NOTOOLBAR))
        {
            pTheToolBar->SetSize(-1, m_nFsToolBarHeight);
            pTheToolBar->Show(TRUE);
        }
#endif //wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
        if ((m_lFsStyle & wxFULLSCREEN_NOSTATUSBAR) && (m_nFsStatusBarFields > 0))
        {
            CreateStatusBar(m_nFsStatusBarFields);
//          PositionStatusBar();
        }
#endif //wxUSE_STATUSBAR

        if ((m_lFsStyle & wxFULLSCREEN_NOMENUBAR) && (m_hMenu != 0))
        {
            ::WinSetParent(m_hMenu, m_hFrame, FALSE);
            ::WinSetOwner(m_hMenu, m_hFrame);
            ::WinSendMsg(m_hFrame, WM_UPDATEFRAME, (MPARAM)FCF_MENU, (MPARAM)0);
        }
        Maximize(m_bFsIsMaximized);

        ::WinSetWindowULong( m_hFrame
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
    HWND                            hTitlebar = NULLHANDLE;
    HWND                            hHScroll = NULLHANDLE;
    HWND                            hVScroll = NULLHANDLE;
    HWND                            hFrame = NULLHANDLE;
    HWND                            hClient = NULLHANDLE;
    SWP                             vSwp[10];
    RECTL                           vRect[10];
    USHORT                          uCtlCount;
    ERRORID                         vError;
    wxString                        sError;

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
    // Create the frame window:  We break ranks with other ports now
    // and instead of calling down into the base wxWindow class' OS2Create
    // we do all our own stuff here.  We will set the needed pieces
    // of wxWindow manually, here.
    //

     hFrame = ::WinCreateStdWindow( hParent
                                   ,ulStyleFlags   // frame-window style
                                   ,&ulCreateFlags // window style
                                   ,(PSZ)zWclass   // class name
                                   ,(PSZ)zTitle    // window title
                                   ,0L             // default client style
                                   ,NULLHANDLE     // resource in executable file
                                   ,0              // resource id
                                   ,&hClient       // receives client window handle
                                  );
    if (!hFrame)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error creating frame. Error: %s\n", sError);
        return FALSE;
    }

    //
    // wxWindow class' m_hWnd set here and needed associations
    //
    m_hFrame = hFrame;
    m_hWnd   = hClient;
    wxAssociateWinWithHandle(m_hWnd, this);
    wxAssociateWinWithHandle(m_hFrame, this);

    //
    // Now need to subclass window.  Instead of calling the SubClassWin in wxWindow
    // we manually subclass here because we don't want to use the main wxWndProc
    // by default
    //
    m_fnOldWndProc = (WXFARPROC) ::WinSubclassWindow(m_hFrame, (PFNWP)wxFrameMainWndProc);

    //
    // Now size everything.  If adding a menu the client will need to be resized.
    //

    if (!::WinSetWindowPos( m_hFrame
                           ,HWND_TOP
                           ,nX
                           ,nY
                           ,nWidth
                           ,nHeight
                           ,SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_ZORDER
                          ))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error sizing frame. Error: %s\n", sError);
        return FALSE;
    }
    //
    // We may have to be smarter here when variable sized toolbars are added!
    //
    if (!::WinSetWindowPos( m_hWnd
                           ,HWND_TOP
                           ,nX + 20
                           ,nY + 20
                           ,nWidth - 60
                           ,nHeight - 60
                           ,SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_ZORDER
                          ))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error sizing client. Error: %s\n", sError);
        return FALSE;
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

#if wxUSE_TOOLBAR
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
#endif //wxUSE_TOOLBAR
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

#if wxUSE_ACCEL
    const wxAcceleratorTable&       rAcceleratorTable = pMenuBar->GetAccelTable();
    return rAcceleratorTable.Translate(GetHWND(), pMsg);
#else
    return FALSE;
#endif //wxUSE_ACCEL
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
                hIcon = (HPOINTER)::WinSendMsg(m_hFrame, WM_QUERYICON, 0L, 0L);
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
            wxWindow::HandlePaint();
            return TRUE;
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
#if  wxUSE_TOOLBAR
        PositionToolBar();
#endif // wxUSE_TOOLBAR

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
    if (nCmd == CMDSRC_MENU || nCmd == CMDSRC_ACCELERATOR)
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
    if( !nFlags )
    {
        MENUITEM mItem;
        MRESULT  rc;

        rc = ::WinSendMsg(hMenu, MM_QUERYITEM, MPFROM2SHORT(nItem, TRUE), (MPARAM)&mItem);

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
// Main Frame window proc
// ---------------------------------------------------------------------------
MRESULT EXPENTRY wxFrameMainWndProc(
  HWND                              hWnd
, ULONG                             ulMsg
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    MRESULT                         rc = (MRESULT)0;
    bool                            bProcessed = FALSE;
    wxFrame*                        pWnd  = NULL;

    pWnd = (wxFrame*) wxFindWinFromHandle((WXHWND) hWnd);
    switch (ulMsg)
    {
        case WM_QUERYFRAMECTLCOUNT:
            if(pWnd && pWnd->m_fnOldWndProc)
            {
                USHORT              uItemCount = SHORT1FROMMR(pWnd->m_fnOldWndProc(hWnd, ulMsg, wParam, lParam));

                rc = MRFROMSHORT(uItemCount);
            }
            break;

        case WM_FORMATFRAME:
/////////////////////////////////////////////////////////////////////////////////
// Applications that subclass frame controls may find that the frame is already
// subclassed the number of frame controls is variable.
// The WM_FORMATFRAME and WM_QUERYFRAMECTLCOUNT messages must always be
// subclassed by calling the previous window procedure and modifying its result.
////////////////////////////////////////////////////////////////////////////////
            {
                int                 nItemCount;
                int                 i;
                PSWP                pSWP = NULL;
                SWP                 vSwpStb;
                RECTL               vRectl;
                RECTL               vRstb;
                int                 nHeight=0;

                pSWP = (PSWP)PVOIDFROMMP(wParam);
                nItemCount = SHORT1FROMMR(pWnd->m_fnOldWndProc(hWnd, ulMsg, wParam, lParam));
                if(pWnd->m_frameStatusBar)
                {
                    ::WinQueryWindowRect(pWnd->m_frameStatusBar->GetHWND(), &vRstb);
                    pWnd->m_frameStatusBar->GetSize(NULL, &nHeight);
                    ::WinQueryWindowRect(pWnd->m_hFrame, &vRectl);
                    ::WinMapWindowPoints(pWnd->m_hFrame, HWND_DESKTOP, (PPOINTL)&vRectl, 2);
                    vRstb = vRectl;
                    ::WinCalcFrameRect(pWnd->m_hFrame, &vRectl, TRUE);

                    vSwpStb.x                = vRectl.xLeft - vRstb.xLeft;
                    vSwpStb.y                = vRectl.yBottom - vRstb.yBottom;
                    vSwpStb.cx               = vRectl.xRight - vRectl.xLeft - 1; //?? -1 ??
                    vSwpStb.cy               = nHeight;
                    vSwpStb.fl               = SWP_SIZE |SWP_MOVE | SWP_SHOW;
                    vSwpStb.hwnd             = pWnd->m_frameStatusBar->GetHWND();
                    vSwpStb.hwndInsertBehind = HWND_TOP;
                }
                ::WinQueryWindowRect(pWnd->m_hFrame, &vRectl);
                ::WinMapWindowPoints(pWnd->m_hFrame, HWND_DESKTOP, (PPOINTL)&vRectl, 2);
                ::WinCalcFrameRect(pWnd->m_hFrame, &vRectl, TRUE);
                ::WinMapWindowPoints(HWND_DESKTOP, pWnd->m_hFrame, (PPOINTL)&vRectl, 2);
                for(i = 0; i < nItemCount; i++)
                {
                    if(pWnd->m_hWnd && pSWP[i].hwnd == pWnd->m_hWnd)
                    {
                        pSWP[i].x    = vRectl.xLeft;
                        pSWP[i].y    = vRectl.yBottom + nHeight;
                        pSWP[i].cx   = vRectl.xRight - vRectl.xLeft;
                        pSWP[i].cy   = vRectl.yTop - vRectl.yBottom - nHeight;
                        pSWP[i].fl   = SWP_SIZE | SWP_MOVE | SWP_SHOW;
                        pSWP[i].hwndInsertBehind = HWND_TOP;
                    }
                }
                bProcessed = TRUE;
                rc = MRFROMSHORT(nItemCount);
            }
            break;

       default:
            if(pWnd && pWnd->m_fnOldWndProc)
                rc = pWnd->m_fnOldWndProc(hWnd, ulMsg, wParam, lParam);
            else
                rc = ::WinDefWindowProc(hWnd, ulMsg, wParam, lParam);
    }
    return rc;
} // end of wxFrameMainWndProc

MRESULT EXPENTRY wxFrameWndProc(
  HWND                              hWnd
, ULONG                             ulMsg
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    //
    // Trace all ulMsgs - useful for the debugging
    //
    HWND                            parentHwnd;
    wxFrame*                        pWnd  = NULL;

    parentHwnd  = WinQueryWindow(hWnd,QW_PARENT);
    pWnd = (wxFrame*) wxFindWinFromHandle((WXHWND) hWnd);

    //
    // When we get the first message for the HWND we just created, we associate
    // it with wxWindow stored in wxWndHook
    //

    MRESULT                         rc = (MRESULT)0;
    bool                            bProcessed = FALSE;

    //
    // Stop right here if we don't have a valid handle in our wxWindow object.
    //
    if (pWnd && !pWnd->GetHWND())
    {
        pWnd->SetHWND((WXHWND) hWnd);
        rc = pWnd->OS2DefWindowProc(ulMsg, wParam, lParam );
        pWnd->SetHWND(0);
    }
    else
    {
        if (pWnd)
            rc = pWnd->OS2WindowProc(ulMsg, wParam, lParam);
        else
            rc = ::WinDefWindowProc(hWnd, ulMsg, wParam, lParam);
    }
    return rc;
} // end of wxFrameWndProc

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

        case WM_PAINT:
            {
                HPS                             hPS;
                RECTL                           vRect;
                wxPaintEvent                    vEvent;

                hPS = WinBeginPaint(m_hWnd, 0L, &vRect);
                ::WinFillRect(hPS, &vRect,  CLR_BLUE  /* SYSCLR_WINDOW */);
                ::WinEndPaint(hPS);

                mRc = (MRESULT)FALSE;
                vEvent.SetEventObject(this);
                GetEventHandler()->ProcessEvent(vEvent);
                bProcessed = TRUE;
            }
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

        case WM_SIZE:
            {
                SHORT               nScxold = SHORT1FROMMP(wParam); // Old horizontal size.
                SHORT               nScyold = SHORT2FROMMP(wParam); // Old vertical size.
                SHORT               nScxnew = SHORT1FROMMP(lParam); // New horizontal size.
                SHORT               nScynew = SHORT2FROMMP(lParam); // New vertical size.

                lParam = MRFROM2SHORT( nScxnew - 20
                                      ,nScynew - 30
                                     );
            }
            bProcessed = HandleSize(LOWORD(lParam), HIWORD(lParam), (WXUINT)wParam);
            mRc = (MRESULT)FALSE;
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
    }

    if (!bProcessed )
        mRc = wxWindow::OS2WindowProc( uMessage
                                      ,wParam
                                      ,lParam
                                     );
    return (MRESULT)mRc;
} // wxFrame::OS2WindowProc

void wxFrame::SetClient(WXHWND c_Hwnd)
{
   // Duh...nothing to do under OS/2
}

void wxFrame::SetClient(
  wxWindow*                         pWindow
)
{
    wxWindow*                       pOldClient      = this->GetClient();
    bool                            bClientHasFocus = pOldClient && (pOldClient == wxWindow::FindFocus());

    if(pOldClient == pWindow)  // nothing to do
        return;
    if(pWindow == NULL) // just need to remove old client
    {
        if(pOldClient == NULL) // nothing to do
            return;

        if(bClientHasFocus )
            this->SetFocus();

        pOldClient->Enable( FALSE );
        pOldClient->Show( FALSE );
        ::WinSetWindowUShort(pOldClient->GetHWND(), QWS_ID, (USHORT)pOldClient->GetId());
        // to avoid OS/2 bug need to update frame
        ::WinSendMsg((HWND)this->GetFrame(), WM_UPDATEFRAME, (MPARAM)~0, 0);
        return;
    }

    //
    // Else need to change client
    //
    if(bClientHasFocus)
         this->SetFocus();

    ::WinEnableWindowUpdate((HWND)GetHWND(), FALSE);
    if(pOldClient)
    {
        pOldClient->Enable(FALSE);
        pOldClient->Show(FALSE);
        ::WinSetWindowUShort(pOldClient->GetHWND(), QWS_ID, (USHORT)pOldClient->GetId());
    }
    pWindow->Reparent(this);
    ::WinSetWindowUShort(pWindow->GetHWND(), QWS_ID, FID_CLIENT);
    ::WinEnableWindowUpdate((HWND)GetHWND(), TRUE);
    pWindow->Enable();
    pWindow->Show();   // ensure client is showing
    if( this->IsShown() )
    {
        this->Show();
        ::WinSendMsg(m_hFrame, WM_UPDATEFRAME, (MPARAM)~0, 0);
    }
}

wxWindow* wxFrame::GetClient()
{
    return wxFindWinFromHandle((WXHWND)::WinWindowFromID(m_hFrame, FID_CLIENT));
}
