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

    //
    // We pass NULL as parent to MSWCreate because frames with parents behave
    // very strangely under Win95 shell.
    // Alteration by JACS: keep normal Windows behaviour (float on top of parent)
    // with this ulStyle.
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
    hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
    ::WinQueryWindowRect(hWndClient, &vRect);

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
    HWND                            hWndClient;
    RECTL                           vRect;
    RECT                            vRect2;

    hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
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
    POINTL                          vPoint;

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
    DoShowWindow(bShow ? SWP_SHOW : SWP_HIDE);

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

    ::WinQueryWindowPos(GetHwnd(), &vSwp);
    return (vSwp.fl & SWP_MAXIMIZE);
} // end of wxFrame::IsMaximized

void wxFrame::SetIcon(
  const wxIcon&                     rIcon
)
{
    wxFrameBase::SetIcon(rIcon);

    if (m_icon.Ok())
    {
        WinSendMsg( GetHwnd()
                   ,WM_SETICON
                   ,(MPARAM)((HICON)m_icon.GetHICON())
                   ,NULL
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

    pStatusBar = wxFrameBase::OnCreateStatusBar( nNumber
                                                ,lulStyle
                                                ,vId
                                                ,rName
                                               );
    return pStatusBar;
} // end of wxFrame::OnCreateStatusBar

void wxFrame::PositionStatusBar()
{
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

        hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
        ::WinQueryWindowRect(hWndClient, &vRect);
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
    if (!pMenuBar)
    {
        DetachMenuBar();
        return;
    }

    wxCHECK_RET(!pMenuBar->GetFrame(), wxT("this menubar is already attached"));

    if (m_frameMenuBar)
        delete m_frameMenuBar;

    m_hMenu = pMenuBar->Create();
    m_ulMenubarId = pMenuBar->GetMenubarId();
    if (m_ulMenubarId != FID_MENU)
    {
        ::WinSetWindowUShort( m_hMenu
                             ,QWS_ID
                             ,(unsigned short)m_ulMenubarId
                            );
    }

    if (!m_hMenu)
        return;

    InternalSetMenuBar();

    m_frameMenuBar = pMenuBar;
    pMenuBar->Attach(this);
} // end of wxFrame::SetMenuBar

void wxFrame::InternalSetMenuBar()
{

    ::WinPostMsg( GetHwnd()
                 ,WM_UPDATEFRAME
                 ,(MPARAM)FCF_MENU
                 ,NULL
                );
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
    ULONG                           ulPmFlags = 0;
    ULONG                           ulExtraFlags = 0;
    ULONG                           ulTempFlags = FCF_TITLEBAR |
                                                  FCF_SYSMENU |
                                                  FCF_MINBUTTON |
                                                  FCF_MAXBUTTON |
                                                  FCF_SIZEBORDER |
                                                  FCF_ICON |
                                                  FCF_MENU |
                                                  FCF_ACCELTABLE |
                                                  FCF_SHELLPOSITION |
                                                  FCF_TASKLIST;

    m_hDefaultIcon = (WXHICON) (wxSTD_FRAME_ICON ? wxSTD_FRAME_ICON : wxDEFAULT_FRAME_ICON);

    if ((ulStyle & wxCAPTION) == wxCAPTION)
        ulPmFlags = FCF_TASKLIST;
    else
        ulPmFlags = FCF_NOMOVEWITHOWNER;

    if (ulStyle & wxMINIMIZE_BOX)
        ulPmFlags |= FCF_MINBUTTON;
    if (ulStyle & wxMAXIMIZE_BOX)
        ulPmFlags |= FCF_MAXBUTTON;
    if (ulStyle & wxTHICK_FRAME)
        ulPmFlags |= FCF_DLGBORDER;
    if (ulStyle & wxSYSTEM_MENU)
        ulPmFlags |= FCF_SYSMENU;
    if ((ulStyle & wxMINIMIZE) || (ulStyle & wxICONIZE))
        ulPmFlags |= WS_MINIMIZED;
    if (ulStyle & wxMAXIMIZE)
        ulPmFlags |= WS_MAXIMIZED;
    if (ulStyle & wxCAPTION)
        ulPmFlags |= FCF_TASKLIST;
    if (ulStyle & wxCLIP_CHILDREN)
    {
        // Invalid for frame windows under PM
    }

    //
    // Keep this in wxFrame because it saves recoding this function
    // in wxTinyFrame
    //
#if wxUSE_ITSY_BITSY
    if (ulStyle & wxTINY_CAPTION_VERT)
        ulExtraFlags |= kVertCaption;
    if (ulStyle & wxTINY_CAPTION_HORIZ)
        ulExtraFlags |= kHorzCaption;
#else
    if (ulStyle & wxTINY_CAPTION_VERT)
        ulPmFlags |= FCF_TASKLIST;
    if (ulStyle & wxTINY_CAPTION_HORIZ)
        ulPmFlags |= FCF_TASKLIST;
#endif
    if ((ulStyle & wxTHICK_FRAME) == 0)
        ulPmFlags |= FCF_BORDER;
    if (ulStyle & wxFRAME_TOOL_WINDOW)
        ulExtraFlags = kFrameToolWindow;

    if (ulStyle & wxSTAY_ON_TOP)
        ulPmFlags |= FCF_SYSMODAL;

    if (ulPmFlags & ulTempFlags)
        ulPmFlags = FCF_STANDARD;
    //
    // Clear the visible flag, we always call show
    //
    ulPmFlags &= (unsigned long)~WS_VISIBLE;
    m_bIconized = FALSE;
    if ( !wxWindow::OS2Create( nId
                              ,pParent
                              ,zWclass
                              ,pWxWin
                              ,zTitle
                              ,nX
                              ,nY
                              ,nWidth
                              ,nHeight
                              ,ulPmFlags
                              ,NULL
                              ,ulExtraFlags))
    {
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

    hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
    ::WinQueryWindowRect(hWndClient, &vRect);

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
    if (wxWindow::OS2TranslateMessage(pMsg))
        return TRUE;
    //
    // try the menu bar accels
    //
    wxMenuBar*                      pMenuBar = GetMenuBar();

    if (!pMenuBar )
        return FALSE;

    const wxAcceleratorTable&       rAcceleratorTable = pMenuBar->GetAccelTable();
    return rAcceleratorTable.Translate(this, pMsg);
} // end of wxFrame::OS2TranslateMessage

// ---------------------------------------------------------------------------
// our private (non virtual) message handlers
// ---------------------------------------------------------------------------
bool wxFrame::HandlePaint()
{
    RECTL                           vRect;

    if (::WinQueryUpdateRect(GetHwnd(), &vRect))
    {
        if (m_bIconized)
        {
            //
            // Icons in PM are the same as "pointers"
            //
            HPOINTER                hIcon;

            if (m_icon.Ok())
                hIcon = (HPOINTER)::WinSendMsg(GetHwnd(), WM_QUERYICON, 0L, 0L);
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

                hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
                ::WinQueryWindowRect(hWndClient, &vRect3);

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
    int                             nMenuItem;

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
    wxMenuEvent                     vEvent(wxEVT_MENU_HIGHLIGHT, nMenuItem);

    vEvent.SetEventObject(this);
    return GetEventHandler()->ProcessEvent(vEvent);
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
            }
            break;

        case WM_PAINT:
            bProcessed = HandlePaint();
            break;

        case CM_QUERYDRAGIMAGE:
            {
                HPOINTER                hIcon;

                if (m_icon.Ok())
                    hIcon = (HPOINTER)::WinSendMsg(GetHwnd(), WM_QUERYICON, 0L, 0L);
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
    return (MRESULT)0;
} // wxFrame::OS2WindowProc

