///////////////////////////////////////////////////////////////////////////////
// Name:        msw/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.12.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
    #include "wx/control.h"
#endif //WX_PRECOMP

#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// stubs for missing functions under MicroWindows
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
wxWindowList wxModelessWindows;

// the name of the default wxWindows class
extern void          wxAssociateWinWithHandle( HWND         hWnd
                                              ,wxWindowOS2* pWin
                                             );
bool                 wxTopLevelWindowOS2::m_sbInitialized = FALSE;

// ============================================================================
// wxTopLevelWindowMSW implementation
// ============================================================================

// Dialog window proc
MRESULT EXPENTRY wxDlgProc( HWND WXUNUSED(hWnd)
                           ,UINT uMessage
                           ,MPARAM WXUNUSED(wParam)
                           ,MPARAM WXUNUSED(lParam)
                          )
{
    if (uMessage == WM_INITDLG)
    {
        //
        // For this message, returning TRUE tells system to set focus to the
        // first control in the dialog box.
        //
        return (MRESULT)TRUE;
    }
    else
    {
        //
        // For all the other ones, FALSE means that we didn't process the
        // message
        //
        return (MRESULT)FALSE;
    }
} // end of wxDlgProc

// ----------------------------------------------------------------------------
// wxTopLevelWindowOS2 creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowOS2::Init()
{
    m_bIconized = m_bMaximizeOnShow = FALSE;

    //
    // Unlike (almost?) all other windows, frames are created hidden
    //
    m_isShown = FALSE;

    //
    // Data to save/restore when calling ShowFullScreen
    m_lFsStyle          = 0;
    m_lFsOldWindowStyle = 0;
    m_bFsIsMaximized    = FALSE;
    m_bFsIsShowing      = FALSE;

    m_hFrame    = NULLHANDLE;
    memset(&m_vSwp, 0, sizeof(SWP));
    memset(&m_vSwpClient, 0, sizeof(SWP));
} // end of wxTopLevelWindowIOS2::Init

long wxTopLevelWindowOS2::OS2GetCreateWindowFlags(
  long*                             plExflags
) const
{
    long                            lStyle = GetWindowStyle();
    long                            lMsflags = 0;

    if (lStyle == wxDEFAULT_FRAME_STYLE)
        lMsflags = FCF_SIZEBORDER | FCF_TITLEBAR | FCF_SYSMENU |
                        FCF_MINMAX | FCF_TASKLIST;
    else
    {
        if ((lStyle & wxCAPTION) == wxCAPTION)
            lMsflags = FCF_TASKLIST;
        else
            lMsflags = FCF_NOMOVEWITHOWNER;

        if ((lStyle & wxVSCROLL) == wxVSCROLL)
            lMsflags |= FCF_VERTSCROLL;
        if ((lStyle & wxHSCROLL) == wxHSCROLL)
            lMsflags |= FCF_HORZSCROLL;
        if (lStyle & wxMINIMIZE_BOX)
            lMsflags |= FCF_MINBUTTON;
        if (lStyle & wxMAXIMIZE_BOX)
            lMsflags |= FCF_MAXBUTTON;
        if (lStyle & wxTHICK_FRAME)
            lMsflags |= FCF_DLGBORDER;
        if (lStyle & wxSYSTEM_MENU)
            lMsflags |= FCF_SYSMENU;
        if (lStyle & wxCAPTION)
            lMsflags |= FCF_TASKLIST;
        if (lStyle & wxCLIP_CHILDREN)
        {
            // Invalid for frame windows under PM
        }

        if (lStyle & wxTINY_CAPTION_VERT)
            lMsflags |= FCF_TASKLIST;
        if (lStyle & wxTINY_CAPTION_HORIZ)
            lMsflags |= FCF_TASKLIST;

        if ((lStyle & wxTHICK_FRAME) == 0)
            lMsflags |= FCF_BORDER;
        if (lStyle & wxFRAME_TOOL_WINDOW)
            *plExflags = kFrameToolWindow;

        if (lStyle & wxSTAY_ON_TOP)
            lMsflags |= FCF_SYSMODAL;
    }
    return lMsflags;
} // end of wxTopLevelWindowOS2::OS2GetCreateWindowFlags

bool wxTopLevelWindowOS2::CreateDialog(
  ULONG                             ulDlgTemplate
, const wxString&                   rsTitle
, const wxPoint&                    rPos
, const wxSize&                     rSize
)
{
    wxWindow*                       pParent = GetParent();

    //
    // For the dialogs without wxDIALOG_NO_PARENT style, use the top level
    // app window as parent - this avoids creating modal dialogs without
    // parent
    //
    if (!pParent && !(GetWindowStyleFlag() & wxDIALOG_NO_PARENT))
    {
        pParent = wxTheApp->GetTopWindow();

        if (pParent)
        {
            //
            // Don't use transient windows as parents, this is dangerous as it
            // can lead to a crash if the parent is destroyed before the child
            //
            // also don't use the window which is currently hidden as then the
            // dialog would be hidden as well
            if ((pParent->GetExtraStyle() & wxWS_EX_TRANSIENT) ||
                 !pParent->IsShown())
            {
                pParent = NULL;
            }
        }
    }

    HWND                            hWndDlg;
    HWND                            hWndOwner;

    if (pParent)
        hWndOwner = GetHwndOf(pParent);
    else
        hWndOwner = HWND_DESKTOP;

    hWndDlg = ::WinLoadDlg( HWND_DESKTOP
                           ,hWndOwner
                           ,(PFNWP)wxDlgProc
                           ,NULL
                           ,(ULONG)ulDlgTemplate
                           ,(PVOID)this
                          );

    m_hWnd = (WXHWND) hWndDlg;

    if ( !m_hWnd )
    {
        wxFAIL_MSG(_("Did you forget to include wx/os2/wx.rc in your resources?"));

        wxLogSysError(_("Can't create dialog using template '%ul'"), ulDlgTemplate);

        return FALSE;
    }

    //
    // Move the dialog to its initial position without forcing repainting
    //
    int                             nX;
    int                             nY;
    int                             nWidth;
    int                             nHeight;

    if (!OS2GetCreateWindowCoords( rPos
                                  ,rSize
                                  ,nX
                                  ,nY
                                  ,nWidth
                                  ,nHeight
                                 ))
    {
        nX = nWidth = (int)CW_USEDEFAULT;
    }

    //
    // We can't use CW_USEDEFAULT here as we're not calling CreateWindow()
    // and passing CW_USEDEFAULT to MoveWindow() results in resizing the
    // window to (0, 0) size which breaks quite a lot of things, e.g. the
    // sizer calculation in wxSizer::Fit()
    //
    if (nWidth == (int)CW_USEDEFAULT)
    {
        //
        // The exact number doesn't matter, the dialog will be resized
        // again soon anyhow but it should be big enough to allow
        // calculation relying on "totalSize - clientSize > 0" work, i.e.
        // at least greater than the title bar height
        //
        nWidth = nHeight = 100;
    }
    if (nX == (int)CW_USEDEFAULT)
    {
        //
        // Centre it on the screen - what else can we do?
        //
        wxSize                      vSizeDpy = wxGetDisplaySize();

        nX = (vSizeDpy.x - nWidth) / 2;
        nY = (vSizeDpy.y - nHeight) / 2;
    }
    m_backgroundColour.Set(wxString("LIGHT GREY"));

    LONG                            lColor = (LONG)m_backgroundColour.GetPixel();

    if (!::WinSetPresParam( m_hWnd
                           ,PP_BACKGROUNDCOLOR
                           ,sizeof(LONG)
                           ,(PVOID)&lColor
                          ))
    {
        return FALSE;
    }

    ::WinSetWindowPos( GetHwnd()
                      ,HWND_TOP
                      ,nX
                      ,nY
                      ,nWidth
                      ,nHeight
                      ,SWP_MOVE | SWP_SIZE | SWP_ZORDER | SWP_SHOW
                     );
    m_hFrame = m_hWnd;
    SubclassWin(m_hWnd);
    return TRUE;
} // end of wxTopLevelWindowOS2::CreateDialog

bool wxTopLevelWindowOS2::CreateFrame(
  const wxString&                   rsTitle
, const wxPoint&                    rPos
, const wxSize&                     rSize
)
{
    long                            lExflags;
    long                            lFlags = OS2GetCreateWindowFlags(&lExflags);
    long                            lStyle = GetWindowStyleFlag();
    int                             nX = rPos.x;
    int                             nY = rPos.y;
    int                             nWidth = rSize.x;
    int                             nHeight = rSize.y;
    ULONG                           ulStyleFlags = 0L;
    ERRORID                         vError;
    wxString                        sError;
    wxWindow*                       pParent = GetParent();
    HWND                            hParent;
    HWND                            hFrame;
    HWND                            hClient;

    if (pParent)
        hParent = GetHwndOf(pParent);
    else
        hParent = HWND_DESKTOP;

    if ((lStyle & wxMINIMIZE) || (lStyle & wxICONIZE))
        ulStyleFlags |= WS_MINIMIZED;
    if (lStyle & wxMAXIMIZE)
        ulStyleFlags |= WS_MAXIMIZED;

    //
    // Clear the visible flag, we always call show
    //
    ulStyleFlags &= (unsigned long)~WS_VISIBLE;
    m_bIconized = FALSE;

    //
    // Create the frame window:  We break ranks with other ports now
    // and instead of calling down into the base wxWindow class' OS2Create
    // we do all our own stuff here.  We will set the needed pieces
    // of wxWindow manually, here.
    //

     hFrame = ::WinCreateStdWindow( hParent
                                   ,ulStyleFlags          // frame-window style
                                   ,(PULONG)&lFlags       // window style
                                   ,(PSZ)wxFrameClassName // class name
                                   ,(PSZ)rsTitle.c_str()  // window title
                                   ,0L                    // default client style
                                   ,NULLHANDLE            // resource in executable file
                                   ,0                     // resource id
                                   ,&hClient              // receives client window handle
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

    m_backgroundColour.Set(wxString("MEDIUM GREY"));

    LONG                            lColor = (LONG)m_backgroundColour.GetPixel();

    if (!::WinSetPresParam( m_hWnd
                           ,PP_BACKGROUNDCOLOR
                           ,sizeof(LONG)
                           ,(PVOID)&lColor
                          ))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Error creating frame. Error: %s\n", sError);
        return FALSE;
    }

    //
    // Now need to subclass window.  Instead of calling the SubClassWin in wxWindow
    // we manually subclass here because we don't want to use the main wxWndProc
    // by default
    //
    m_fnOldWndProc = (WXFARPROC) ::WinSubclassWindow(m_hFrame, (PFNWP)wxFrameMainWndProc);

    //
    // Now size everything.  If adding a menu the client will need to be resized.
    //

    if (pParent)
    {
        nY = pParent->GetSize().y - (nY + nHeight);
    }
    else
    {
        RECTL                   vRect;

        ::WinQueryWindowRect(HWND_DESKTOP, &vRect);
        nY = vRect.yTop - (nY + nHeight);
    }
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
    lStyle =  ::WinQueryWindowULong( m_hWnd
                                    ,QWL_STYLE
                                   );
    lStyle |= WS_CLIPCHILDREN;
    ::WinSetWindowULong( m_hWnd
                        ,QWL_STYLE
                        ,lStyle
                       );
    return TRUE;
} // end of wxTopLevelWindowOS2::CreateFrame

bool wxTopLevelWindowOS2::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsTitle
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    //
    // Init our fields
    //
    Init();
    m_windowStyle = lStyle;
    SetName(rsName);
    m_windowId = vId == -1 ? NewControlId() : vId;
    wxTopLevelWindows.Append(this);
    if (pParent)
        pParent->AddChild(this);

    if (GetExtraStyle() & wxTOPLEVEL_EX_DIALOG)
    {
        //
        // We have different dialog templates to allows creation of dialogs
        // with & without captions under OS2indows, resizeable or not (but a
        // resizeable dialog always has caption - otherwise it would look too
        // strange)
        //
        ULONG                       ulDlgTemplate;

        if (lStyle & wxRESIZE_BORDER)
            ulDlgTemplate = (ULONG)kResizeableDialog;
        else if (lStyle & wxCAPTION)
            ulDlgTemplate = (ULONG)kCaptionDialog;
        else
            ulDlgTemplate = (ULONG)kNoCaptionDialog;
        return CreateDialog( ulDlgTemplate
                            ,rsTitle
                            ,rPos
                            ,rSize
                           );
    }
    else // !dialog
    {
        return CreateFrame( rsTitle
                           ,rPos
                           ,rSize
                          );
    }
} // end of wxTopLevelWindowOS2::Create

wxTopLevelWindowOS2::~wxTopLevelWindowOS2()
{
    wxTopLevelWindows.DeleteObject(this);

    if (wxModelessWindows.Find(this))
        wxModelessWindows.DeleteObject(this);

    //
    // If this is the last top-level window, exit.
    //
    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
        wxTheApp->SetTopWindow(NULL);
        if ( wxTheApp->GetExitOnFrameDelete() )
        {
            ::WinPostMsg(NULL, WM_QUIT, 0, 0);
        }
    }
} // end of wxTopLevelWindowOS2::~wxTopLevelWindowOS2

//
//  IF we have child controls in the Frame's client we need to alter
//  the y position, because, OS/2 controls are positioned relative to
//  wxWindows orgin (top left) not the OS/2 origin (bottom left)
//
void wxTopLevelWindowOS2::AlterChildPos()
{
    //
    // OS/2 is the only OS concerned about this
    //
    wxWindow*                               pChild = NULL;
    wxControl*                              pCtrl = NULL;
    RECTL                                   vRect;
    SWP                                     vSwp;

    if (GetAutoLayout)
        //
        // Auto layouts taken care of elsewhere
        //
        return;

    ::WinQueryWindowRect(GetHwnd(), &vRect);
    for (wxWindowList::Node* pNode = GetChildren().GetFirst();
         pNode;
         pNode = pNode->GetNext())
    {
        wxWindow*                   pChild = pNode->GetData();

        ::WinQueryWindowPos(pChild->GetHWND(), &vSwp);
        vSwp.y += (vRect.yTop - m_vSwpClient.cy);
        if (pChild->IsKindOf(CLASSINFO(wxControl)))
        {
            pCtrl = wxDynamicCast(pChild, wxControl);
            //
            // Must deal with controls that have margins like ENTRYFIELD.  The SWP
            // struct of such a control will have and origin offset from its intended
            // position by the width of the margins.
            //
            vSwp.y -= pCtrl->GetYComp();
            vSwp.x -= pCtrl->GetXComp();
        }
        ::WinSetWindowPos( pChild->GetHWND()
                          ,HWND_TOP
                          ,vSwp.x
                          ,vSwp.y
                          ,vSwp.cx
                          ,vSwp.cy
                          ,SWP_MOVE
                         );
        ::WinQueryWindowPos(pChild->GetHWND(), &vSwp);
        pChild = NULL;
    }
    ::WinQueryWindowPos(GetHwnd(), &m_vSwpClient);
} // end of wxTopLevelWindowOS2::AlterChildPos

void wxTopLevelWindowOS2::UpdateInternalSize(
  wxWindow*                         pChild
, int                               nChildWidth
, int                               nChildHeight
)
{
    int                             nWidthAdjust = 0;
    int                             nHeightAdjust = 0;
    int                             nPosX;
    int                             nPosY;
    bool                            bNewYSize = FALSE;
    bool                            bNewXSize = FALSE;

    //
    // Under OS/2, if we have a srolled window as the child, the
    // scrollbars will be SIBLINGS of the scrolled window.  So, in
    // order to be able to display the scrollbars properly we have to
    // resize the scrolled window.  Of course, that means dealing with
    // child windows of that window as well, because OS/2 does not
    // tend to put them in the right place.
    //
    if (nChildHeight != m_vSwpClient.cy)
        bNewYSize = TRUE;
    if (nChildWidth != m_vSwpClient.cx)
        bNewXSize = TRUE;
    if (bNewXSize || bNewYSize)
        pChild->SetSize( 0
                        ,0
                        ,nChildWidth
                        ,nChildHeight
                       );
    if(bNewYSize)
    {
        //
        // This is needed SetSize will mess up the OS/2 child window
        // positioning because we position in wxWindows coordinates,
        // not OS/2 coordinates.
        //
        pChild->MoveChildren(m_vSwpClient.cy - nChildHeight);
        pChild->Refresh();
    }

    if (pChild->GetScrollBarHorz() != NULLHANDLE ||
        pChild->GetScrollBarVert() != NULLHANDLE)
    {
        if (bNewXSize || bNewYSize)
        {
            pChild->GetSize( &nChildWidth
                            ,&nChildHeight
                           );
            if (pChild->GetScrollBarHorz() != NULLHANDLE)
                nHeightAdjust = 20;
            if (pChild->GetScrollBarVert() != NULLHANDLE)
                nWidthAdjust = 20;
            pChild->GetPosition( &nPosX
                                ,&nPosY
                               );
            ::WinSetWindowPos( pChild->GetHWND()
                              ,HWND_TOP
                              ,nPosX
                              ,nPosY + nHeightAdjust
                              ,nChildWidth - nWidthAdjust
                              ,nChildHeight - nHeightAdjust
                              ,SWP_MOVE | SWP_SIZE
                             );
        }
        if (bNewYSize && !m_sbInitialized)
        {
            //
            // Only need to readjust child control positions of
            // scrolled windows once on initialization.  After that
            // the sizing takes care of things itself.
            //
            pChild->MoveChildren(nHeightAdjust);
            m_sbInitialized = TRUE;
        }
        if (bNewXSize || bNewYSize)
        {
            //
            // Always refresh to keep scollbars visible.  They are
            // children of the Toplevel window, not the child panel.
            //
            pChild->Refresh();
        }
    }
    //
    // This brings the internal "last size" up to date.
    //
    ::WinQueryWindowPos(GetHwnd(), &m_vSwpClient);
} // end of wxTopLevelWindowOS2::UpdateInternalSize

// ----------------------------------------------------------------------------
// wxTopLevelWindowOS2 client size
// ----------------------------------------------------------------------------

void wxTopLevelWindowOS2::DoSetClientSize(
  int                               nWidth
, int                               nHeight
)
{
    //
    // Call GetClientAreaOrigin() to take the toolbar into account
    //
    wxPoint                         vPt = GetClientAreaOrigin();

    nWidth  += vPt.x;
    nHeight += vPt.y;

    wxWindow::DoSetClientSize( nWidth
                              ,nHeight
                             );
} // end of wxTopLevelWindowOS2::DoSetClientSize

void wxTopLevelWindowOS2::DoGetClientSize(
  int*                              pnX
, int*                              pnY
) const
{
    wxWindow::DoGetClientSize( pnX
                              ,pnY
                             );

    wxPoint                         vPt = GetClientAreaOrigin();

    if (pnX)
        *pnX -= vPt.x;

    if (pnY)
        *pnY += vPt.y;
} // end of wxTopLevelWindowOS2::DoGetClientSize

// ----------------------------------------------------------------------------
// wxTopLevelWindowOS2 showing
// ----------------------------------------------------------------------------

void wxTopLevelWindowOS2::DoShowWindow(
  int                               nShowCmd
)
{
    ::WinShowWindow(m_hFrame, (BOOL)(nShowCmd & SWP_SHOW));

    //
    // Need to artificially send a size event as wxApps often expect to do some
    // final child control sizing
    SendSizeEvent();
    m_bIconized = nShowCmd == SWP_MINIMIZE;
} // end of wxTopLevelWindowOS2::DoShowWindow

bool wxTopLevelWindowOS2::Show(
  bool                              bShow
)
{
    int                             nShowCmd;
    SWP                             vSwp;
    RECTL                           vRect;

    if (bShow)
    {
        if (m_bMaximizeOnShow)
        {
            nShowCmd = SWP_MAXIMIZE;
            m_bMaximizeOnShow = FALSE;
        }
        else
        {
            nShowCmd = SWP_SHOW;
        }
    }
    else // hide
    {
        nShowCmd = SWP_HIDE;
    }
    DoShowWindow(nShowCmd);

    if (bShow)
    {
        wxActivateEvent             vEvent(wxEVT_ACTIVATE, TRUE, m_windowId);

        ::WinQueryWindowPos(m_hFrame, &vSwp);
        m_bIconized = vSwp.fl & SWP_MINIMIZE;
        ::WinQueryWindowPos(m_hWnd, &m_vSwpClient);
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
} // end of wxTopLevelWindowOS2::Show

// ----------------------------------------------------------------------------
// wxTopLevelWindowOS2 maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowOS2::Maximize(
  bool                              bMaximize
)
{
    if (IsShown())
    {
        //
        // Just maximize it directly
        //
        DoShowWindow(bMaximize ? SWP_MAXIMIZE : SWP_RESTORE);
    }
    else // hidden
    {
        //
        // We can't maximize the hidden frame because it shows it as well, so
        // just remember that we should do it later in this case
        //
        m_bMaximizeOnShow = TRUE;
    }
} // end of wxTopLevelWindowOS2::Maximize

bool wxTopLevelWindowOS2::IsMaximized() const
{
    bool                            bIconic;

    ::WinQueryWindowPos(m_hFrame, (PSWP)&m_vSwp);
    return (m_vSwp.fl & SWP_MAXIMIZE);
} // end of wxTopLevelWindowOS2::IsMaximized

void wxTopLevelWindowOS2::Iconize(
  bool                              bIconize
)
{
    DoShowWindow(bIconize ? SWP_MINIMIZE : SWP_RESTORE);
} // end of wxTopLevelWindowOS2::Iconize

bool wxTopLevelWindowOS2::IsIconized() const
{
    // also update the current state
    ::WinQueryWindowPos(m_hFrame, (PSWP)&m_vSwp);
    if (m_vSwp.fl & SWP_MINIMIZE)
        ((wxTopLevelWindow*)this)->m_bIconized = TRUE;
    else
        ((wxTopLevelWindow*)this)->m_bIconized = FALSE;
    return m_bIconized;
} // end of wxTopLevelWindowOS2::IsIconized

void wxTopLevelWindowOS2::Restore()
{
    DoShowWindow(SWP_RESTORE);
} // end of wxTopLevelWindowOS2::Restore

// generate an artificial resize event
void wxTopLevelWindowOS2::SendSizeEvent()
{
    if (!m_bIconized)
    {
        RECTL                       vRect = wxGetWindowRect(GetHwnd());

        (void)::WinPostMsg( m_hFrame
                           ,WM_SIZE
                           ,MPFROM2SHORT(vRect.xRight - vRect.xLeft, vRect.yTop - vRect.yBottom)
                           ,MPFROM2SHORT(vRect.xRight - vRect.xLeft, vRect.yTop - vRect.yBottom)
                          );
    }
} // end of wxTopLevelWindowOS2::SendSizeEvent

// ----------------------------------------------------------------------------
// wxTopLevelWindowOS2 fullscreen
// ----------------------------------------------------------------------------

bool wxTopLevelWindowOS2::ShowFullScreen(
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

        //
        // Zap the frame borders
        //

        //
        // Save the 'normal' window lStyle
        //
        m_lFsOldWindowStyle = ::WinQueryWindowULong( (HWND)GetHWND()
                                                    ,QWL_STYLE
                                                   );

        //
        // Save the old position, width & height, maximize state
        //
        m_vFsOldSize = GetRect();
        m_bFsIsMaximized = IsMaximized();

        //
        // Decide which window lStyle flags to turn off
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
        ::WinSetWindowULong( (HWND)GetHWND()
                            ,QWL_STYLE
                            ,lNewStyle
                           );

        //
        // Resize to the size of the desktop
        //
        int                         nWidth;
        int                         nHeight;
        RECTL                       vRect = wxGetWindowRect(HWND_DESKTOP);

        nWidth = vRect.xRight - vRect.xLeft;
        nHeight = vRect.yTop - vRect.yBottom;

        SetSize( nWidth
                ,nHeight
               );

        //
        // Now flush the window style cache and actually go full-screen
        //
        ::WinSetWindowPos( m_hFrame
                          ,HWND_TOP
                          ,0
                          ,0
                          ,nWidth
                          ,nHeight
                          ,SWP_SIZE | SWP_MOVE
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
        Maximize(m_bFsIsMaximized);
        ::WinSetWindowULong( (HWND)GetHWND()
                            ,QWL_STYLE
                            ,m_lFsOldWindowStyle
                           );
        ::WinSetWindowPos( m_hFrame
                          ,HWND_TOP
                          ,m_vFsOldSize.x
                          ,m_vFsOldSize.y
                          ,m_vFsOldSize.width
                          ,m_vFsOldSize.height
                          ,SWP_SIZE | SWP_MOVE
                         );
        return TRUE;
    }
} // end of wxTopLevelWindowOS2::ShowFullScreen

// ----------------------------------------------------------------------------
// wxTopLevelWindowOS2 misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowOS2::SetIcon(
  const wxIcon&                     rIcon
)
{
    //
    // This sets m_icon
    //
    wxTopLevelWindowBase::SetIcon(rIcon);

    if (m_icon.Ok())
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
} // end of wxTopLevelWindowOS2::SetIcon

bool wxTopLevelWindowOS2::EnableCloseButton(
  bool                              bEnable
)
{
    //
    // Get system (a.k.a. window) menu
    //
    HMENU                           hMenu = ::WinWindowFromID(m_hFrame, FID_SYSMENU);

    if (!hMenu)
    {
        wxLogLastError(_T("GetSystemMenu"));
        return FALSE;
    }

    //
    // Enabling/disabling the close item from it also automatically
    // disables/enables the close title bar button
    //
    if (bEnable)
        (void)::WinSendMsg( hMenu
                           ,MM_SETITEMATTR
                           ,MPFROM2SHORT(SC_CLOSE, FALSE)
                           ,MPFROM2SHORT(MIA_DISABLED, FALSE)
                          );
    else
        (void)::WinSendMsg( hMenu
                           ,MM_SETITEMATTR
                           ,MPFROM2SHORT(SC_CLOSE, FALSE)
                           ,MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED)
                          );

    //
    // Update appearance immediately
    //
    ::WinSendMsg( m_hFrame
                 ,WM_UPDATEFRAME
                 ,(MPARAM)FCF_MENU
                 ,(MPARAM)0
                );
    return TRUE;
} // end of wxTopLevelWindowOS2::EnableCloseButton

