/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//
// For compilers that support precompilation, includes "wx.h".
//
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #define INCL_DOS
    #define INCL_PM
    #include <os2.h>
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/setup.h"
    #include "wx/menu.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/panel.h"
    #include "wx/layout.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/listbox.h"
    #include "wx/button.h"
    #include "wx/msgdlg.h"

    #include <stdio.h>
#endif

#if     wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#if     wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#include "wx/menuitem.h"
#include "wx/log.h"

#include "wx/os2/private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#include "wx/intl.h"
#include "wx/log.h"


#include "wx/textctrl.h"

#include <string.h>

//
// Place compiler, OS specific includes here
//

//
// Standard macros -- these are for OS/2 PM, but most GUI's have something similar
//
#ifndef GET_X_LPARAM
//
//  SHORT1FROMMP -- LOWORD
//
    #define GET_X_LPARAM(mp) ((unsigned short)(unsigned long)(mp))
//
//  SHORT2FROMMP -- HIWORD
//
    #define GET_Y_LPARAM(mp) ((unsigned short)(unsigned long)(mp >> 16))
#endif // GET_X_LPARAM

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

//
// The last Windows message we got (MT-UNSAFE)
//
extern WXMSGID            s_currentMsg;

wxMenu*                   wxCurrentPopupMenu = NULL;
extern wxList WXDLLEXPORT wxPendingDelete;
extern wxChar*            wxCanvasClassName;
wxList*                   wxWinHandleList = NULL;

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

//
// the window proc for all our windows; most gui's have something similar
//
MRESULT wxWndProc( HWND hWnd
                  ,ULONG message
                  ,MPARAM mp1
                  ,MPARAM mp2
                 );

#ifdef  __WXDEBUG__
    const char *wxGetMessageName(int message);
#endif  //__WXDEBUG__

void      wxRemoveHandleAssociation(wxWindow* pWin);
void      wxAssociateWinWithHandle( HWND      hWnd
                                   ,wxWindow* pWin
                                  );
wxWindow* wxFindWinFromHandle(WXHWND hWnd);

//
// This magical function is used to translate VK_APPS key presses to right
// mouse clicks
//
static void TranslateKbdEventToMouse( wxWindow* pWin
                                     ,int*      pX
                                     ,int*      pY
                                     ,MPARAM*   pFlags
                                    );

//
// get the current state of SHIFT/CTRL keys
//
static inline bool IsShiftDown() { return (::WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000) != 0; }
static inline bool IsCtrlDown() { return (::WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000) != 0; }
// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)

BEGIN_EVENT_TABLE(wxWindow, wxWindowBase)
    EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
    EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindow::OnInitDialog)
    EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

//
// Find an item given the PM Window id
//
wxWindow* wxWindow::FindItem(
  long                              lId
) const
{
    wxControl*                      pItem = wxDynamicCast( this
                                                          ,wxControl
                                                         );

    if (pItem)
    {
        //
        // I it we or one of our "internal" children?
        //
        if (pItem->GetId() == lId ||
             (pItem->GetSubcontrols().Index(lId) != wxNOT_FOUND))
        {
            return pItem;
        }
    }

    wxWindowList::Node*             pCurrent = GetChildren().GetFirst();

    while (pCurrent)
    {
        wxWindow*                   pChildWin = pCurrent->GetData();
        wxWindow*                   pWnd = pChildWin->FindItem(lId);

        if (pWnd)
            return pWnd;

        pCurrent = pCurrent->GetNext();
    }
    return(NULL);
} // end of wxWindow::FindItem

//
// Find an item given the PM Window handle
//
wxWindow* wxWindow::FindItemByHWND(
  WXHWND                            hWnd
, bool                              bControlOnly
) const
{
    wxWindowList::Node*             pCurrent = GetChildren().GetFirst();

    while (pCurrent)
    {
        wxWindow*                   pParent = pCurrent->GetData();

        //
        // Do a recursive search.
        //
        wxWindow*                   pWnd = pParent->FindItemByHWND(hWnd);

        if (pWnd)
            return(pWnd);

        if (!bControlOnly || pParent->IsKindOf(CLASSINFO(wxControl)))
        {
            wxWindow*               pItem = pCurrent->GetData();

            if (pItem->GetHWND() == hWnd)
                return(pItem);
            else
            {
                if (pItem->ContainsHWND(hWnd))
                    return(pItem);
            }
        }
        pCurrent = pCurrent->GetNext();
    }
    return(NULL);
} // end of wxWindow::FindItemByHWND

//
// Default command handler
//
bool wxWindow::OS2Command(
  WXUINT                            WXUNUSED(uParam)
, WXWORD                            WXUNUSED(uId)
)
{
    return(FALSE);
}

// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindow::Init()
{
    //
    // Generic
    //
    InitBase();

    //
    // PM specific
    //
    m_bDoubleClickAllowed = 0;
    m_bWinCaptured = FALSE;

    m_isBeingDeleted = FALSE;
    m_fnOldWndProc = 0;
    m_bUseCtl3D = FALSE;
    m_bMouseInWindow = FALSE;

    //
    // wxWnd
    //
    m_hMenu = 0;
    m_hWnd = 0;

    //
    // Pass WM_GETDLGCODE to DefWindowProc()
    m_lDlgCode = 0;

    m_nXThumbSize = 0;
    m_nYThumbSize = 0;
    m_bBackgroundTransparent = FALSE;

    //
    // As all windows are created with WS_VISIBLE style...
    //
    m_isShown = TRUE;

#if wxUSE_MOUSEEVENT_HACK
    m_lLastMouseX =
    m_lLastMouseY = -1;
    m_nLastMouseEvent = -1;
#endif // wxUSE_MOUSEEVENT_HACK
} // wxWindow::Init

//
// Destructor
//
wxWindow::~wxWindow()
{
    m_isBeingDeleted = TRUE;

    OS2DetachWindowMenu();
    if (m_parent)
        m_parent->RemoveChild(this);
    DestroyChildren();
    if (m_hWnd)
    {
        if(!::WinDestroyWindow(GetHWND()))
            wxLogLastError(wxT("DestroyWindow"));
        //
        // remove hWnd <-> wxWindow association
        //
        wxRemoveHandleAssociation(this);
    }
} // end of wxWindow::~wxWindow

bool wxWindow::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rName
)
{
    wxCHECK_MSG(pParent, FALSE, wxT("can't create wxWindow without parent"));

    if ( !CreateBase( pParent
                     ,vId
                     ,rPos
                     ,rSize
                     ,lStyle
                     ,wxDefaultValidator
                     ,rName
                    ))
        return(FALSE);

    pParent->AddChild(this);

    ULONG                           ulFlags = 0L;

    //
    // Frame windows and their derivatives only
    //
    if (lStyle & wxBORDER)
        ulFlags |= FCF_BORDER;
    if (lStyle & wxTHICK_FRAME )
        ulFlags |= FCF_SIZEBORDER;

    //
    // Some generic window styles
    //
    ulFlags |= WS_VISIBLE;
    if (lStyle & wxCLIP_CHILDREN )
        ulFlags |= WS_CLIPCHILDREN;

    bool                            bWant3D;
    WXDWORD                         dwExStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &bWant3D);

    //
    // OS/2 PM doesn't have "extended" styles but if the library specifies
    // them and we are creating a frame window then at least give it a border
    //
    if ( bWant3D ||
        (m_windowStyle & wxSIMPLE_BORDER) ||
        (m_windowStyle & wxRAISED_BORDER ) ||
        (m_windowStyle & wxSUNKEN_BORDER) ||
        (m_windowStyle & wxDOUBLE_BORDER)
       )
    {
        ulFlags |= FCF_BORDER;
    }
    OS2Create( m_windowId
              ,pParent
              ,wxCanvasClassName
              ,this
              ,NULL
              ,rPos.x
              ,rPos.y
              ,WidthDefault(rSize.x)
              ,HeightDefault(rSize.y)
              ,ulFlags
              ,NULL
              ,dwExStyle
             );
    return(TRUE);
} // end of wxWindow::Create

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindow::SetFocus()
{
    HWND                            hWnd = GetHwnd();

    if (hWnd)
        ::WinSetFocus(HWND_DESKTOP, hWnd);
} // end of wxWindow::SetFocus

wxWindow* wxWindowBase::FindFocus()
{
    HWND                            hWnd = ::WinQueryFocus(HWND_DESKTOP);

    if (hWnd)
    {
        return wxFindWinFromHandle((WXHWND)hWnd);
    }
    return NULL;
} // wxWindowBase::FindFocus

bool wxWindow::Enable(
  bool                              bEnable
)
{
    if (!wxWindowBase::Enable(bEnable))
        return(FALSE);

    HWND                            hWnd = GetHwnd();

    if ( hWnd )
        ::WinEnableWindow(hWnd, (BOOL)bEnable);

    wxWindowList::Node*             pNode = GetChildren().GetFirst();

    while (pNode)
    {
        wxWindow*                   pChild = pNode->GetData();

        pChild->Enable(bEnable);
        pNode = pNode->GetNext();
    }
    return(TRUE);
} // end of wxWindow::Enable

bool wxWindow::Show(
  bool                              bShow
)
{
    if (!wxWindowBase::Show(bShow))
        return(FALSE);

    HWND                            hWnd = GetHwnd();

    ::WinShowWindow(hWnd, bShow);

    if (bShow)
    {
        ::WinSetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_ACTIVATE | SWP_ZORDER);
    }
    return(TRUE);
} // end of wxWindow::Show

void wxWindow::Raise()
{
    ::WinSetWindowPos(GetHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_ZORDER | SWP_ACTIVATE);
} // end of wxWindow::Raise

void wxWindow::Lower()
{
    ::WinSetWindowPos(GetHwnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER | SWP_DEACTIVATE);
} // end of wxWindow::Lower

void wxWindow::SetTitle(
  const wxString&                   rTitle
)
{
    ::WinSetWindowText(GetHwnd(), rTitle.c_str());
} // end of wxWindow::SetTitle

wxString wxWindow::GetTitle() const
{
    return wxGetWindowText(GetHWND());
} // end of wxWindow::GetTitle

void wxWindow::CaptureMouse()
{
    HWND                            hWnd = GetHwnd();

    if (hWnd && !m_bWinCaptured)
    {
        ::WinSetCapture(HWND_DESKTOP, hWnd);
        m_bWinCaptured = TRUE;
    }
} // end of wxWindow::GetTitle

void wxWindow::ReleaseMouse()
{
    if (m_bWinCaptured)
    {
        ::WinSetCapture(HWND_DESKTOP, NULLHANDLE);
        m_bWinCaptured = FALSE;
    }
} // end of wxWindow::ReleaseMouse

bool wxWindow::SetFont(
  const wxFont&                     rFont
)
{
    if (!wxWindowBase::SetFont(rFont))
    {
        // nothing to do
        return(FALSE);
    }

    HWND                            hWnd = GetHwnd();

    if (hWnd != 0)
    {
        wxChar                      zFont[128];

        sprintf(zFont, "%d.%s", rFont.GetPointSize(), rFont.GetFaceName().c_str());
        return(::WinSetPresParam(hWnd, PP_FONTNAMESIZE, strlen(zFont), (PVOID)zFont));
    }
    return(TRUE);
}

bool wxWindow::SetCursor(
  const wxCursor&                   rCursor
) // check if base implementation is OK
{
    if ( !wxWindowBase::SetCursor(rCursor))
    {
        // no change
        return FALSE;
    }

    wxASSERT_MSG( m_cursor.Ok(),
                  wxT("cursor must be valid after call to the base version"));

    HWND                            hWnd = GetHwnd();
    POINTL                          vPoint;
    RECTL                           vRect;
    HPS                             hPS;
    HRGN                            hRGN;

    hPS = ::WinGetPS(hWnd);

    ::WinQueryPointerPos(HWND_DESKTOP, &vPoint);
    ::WinQueryWindowRect(hWnd, &vRect);

    hRGN = ::GpiCreateRegion(hPS, 1L, &vRect);

    if ((::GpiPtInRegion(hPS, hRGN, &vPoint) == PRGN_INSIDE) && !wxIsBusy())
    {
        ::WinSetPointer(HWND_DESKTOP, (HPOINTER)m_cursor.GetHCURSOR());
    }
    return TRUE;
} // end of wxWindow::SetCursor

void wxWindow::WarpPointer(
  int                               nXPos
, int                               nYPos
)
{
    int                             nX = nXPos;
    int                             nY = nYPos;
    RECTL                           vRect;

    ::WinQueryWindowRect(GetHwnd(), &vRect);
    nX += vRect.xLeft;
    nY += vRect.yBottom;

    ::WinSetPointerPos(HWND_DESKTOP, (LONG)nX, (LONG)(nY));
} // end of wxWindow::WarpPointer

#if WXWIN_COMPATIBILITY
void wxWindow::OS2DeviceToLogical (float *x, float *y) const
{
}
#endif // WXWIN_COMPATIBILITY

// ---------------------------------------------------------------------------
// scrolling stuff
// ---------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY
void wxWindow::SetScrollRange(
  int                               nOrient
, int                               nRange
, bool                              bRefresh
)
{
    ::WinSendMsg(GetHwnd(), SBM_SETSCROLLBAR, (MPARAM)0, MPFROM2SHORT(0, nRange));
} // end of wxWindow::SetScrollRange

void wxWindow::SetScrollPage(
  int                               nOrient
, int                               nPage
, bool                              bRefresh
)
{
    if ( orient == wxHORIZONTAL )
        m_xThumbSize = page;
    else
        m_yThumbSize = page;
}

int wxWindow::OldGetScrollRange(
  int                               nOrient
) const
{
    MRESULT                         mRc;
    HWND                            hWnd = GetHwnd();

    if (hWnd)
    {
        mRc = WinSendMsg(hWnd, SBM_QUERYRANGE, (MPARAM)0L, (MPARAM)0L);
        return(SHORT2FROMMR(mRc));
     }
     return 0;
} // end of wxWindow::OldGetScrollRange

int  wxWindow::GetScrollPage(
  int                               nOrient
) const
{
    if (nOrient == wxHORIZONTAL)
        return m_nXThumbSize;
    else
        return m_nYThumbSize;
} // end of wxWindow::GetScrollPage
#endif // WXWIN_COMPATIBILITY

int  wxWindow::GetScrollPos(
  int                               nOrient
) const
{
    return((int)::WinSendMsg(GetHwnd(), SBM_QUERYPOS, (MPARAM)NULL, (MPARAM)NULL));
} // end of wxWindow::GetScrollPos

int wxWindow::GetScrollRange(
  int                               nOrient
) const
{
    MRESULT                         mr;

    mr = ::WinSendMsg(GetHwnd(), SBM_QUERYRANGE, (MPARAM)NULL, (MPARAM)NULL);
    return((int)SHORT2FROMMR(mr));
} // end of wxWindow::GetScrollRange

int wxWindow::GetScrollThumb(
  int                               nOrient
) const
{
    WNDPARAMS                       vWndParams;
    PSBCDATA                        pSbcd;

    ::WinSendMsg(GetHwnd(), WM_QUERYWINDOWPARAMS, (MPARAM)&vWndParams, (MPARAM)NULL);
    pSbcd = (PSBCDATA)vWndParams.pCtlData;
    return((int)pSbcd->posThumb);
} // end of wxWindow::GetScrollThumb

void wxWindow::SetScrollPos(
  int                               nOrient
, int                               nPos
, bool                              bRefresh
)
{
    ::WinSendMsg(GetHwnd(), SBM_SETPOS, (MPARAM)nPos, (MPARAM)NULL);
} // end of wxWindow::SetScrollPos(

void wxWindow::SetScrollbar(
  int                               nOrient
, int                               nPos
, int                               nThumbVisible
, int                               nRange
, bool                              bRefresh
)
{
    ::WinSendMsg(GetHwnd(), SBM_SETSCROLLBAR, (MPARAM)nPos, MPFROM2SHORT(0, nRange));
    if (nOrient == wxHORIZONTAL)
    {
        m_nXThumbSize = nThumbVisible;
    }
    else
    {
        m_nYThumbSize = nThumbVisible;
    }
} // end of wxWindow::SetScrollbar

void wxWindow::ScrollWindow(
  int                               nDx
, int                               nDy
, const wxRect*                     pRect
)
{
    RECTL                           vRect2;

    if (pRect)
    {
        vRect2.xLeft   = pRect->x;
        vRect2.yTop    = pRect->y;
        vRect2.xRight  = pRect->x + pRect->width;
        vRect2.yBottom = pRect->y + pRect->height;
    }

    if (pRect)
        ::WinScrollWindow(GetHwnd(), (LONG)nDx, (LONG)nDy, &vRect2, NULL, NULLHANDLE, NULL, 0L);
    else
        ::WinScrollWindow(GetHwnd(), nDx, nDy, NULL, NULL, NULLHANDLE, NULL, 0L);
} // end of wxWindow::ScrollWindow

// ---------------------------------------------------------------------------
// subclassing
// ---------------------------------------------------------------------------

void wxWindow::SubclassWin(
  WXHWND                            hWnd
)
{
    HAB                             hab;
    HWND                            hwnd = (HWND)hWnd;

    wxASSERT_MSG( !m_fnOldWndProc, wxT("subclassing window twice?") );

    wxCHECK_RET(::WinIsWindow(hab, hwnd), wxT("invalid HWND in SubclassWin") );

    wxAssociateWinWithHandle(hwnd, this);

    m_fnOldWndProc = (WXFARPROC) ::WinSubclassWindow(hwnd, wxWndProc);
    ::WinSetWindowULong(hwnd, QWS_USER, wxWndProc);
} // end of wxWindow::SubclassWin

void wxWindow::UnsubclassWin()
{
    HAB                             hab;

    wxRemoveHandleAssociation(this);

    //
    // Restore old Window proc
    //
    HWND                            hwnd = GetHwnd();

    if (hwnd)
    {
        m_hWnd = 0;

        wxCHECK_RET( ::WinIsWindow(hab, hwnd), wxT("invalid HWND in UnsubclassWin") );

        PFNWP                       fnProc = (PFNWP)::WinQueryWindowULong(hwnd, QWS_USER);
        if ( (m_fnOldWndProc != 0) && (fnProc != (PFNWP) m_fnOldWndProc))
        {
            WinSubclassWindow(hwnd, (PFNWP)m_fnOldWndProc);
            m_fnOldWndProc = 0;
        }
    }
} // end of wxWindow::UnsubclassWin

//
// Make a Windows extended style from the given wxWindows window style
//
WXDWORD wxWindow::MakeExtendedStyle(
  long                              lStyle
, bool                              bEliminateBorders
)
{
   //
   // PM does not support extended style
   //
    WXDWORD                         exStyle = 0;
    return exStyle;
} // end of wxWindow::MakeExtendedStyle

//
// Determines whether native 3D effects or CTL3D should be used,
// applying a default border style if required, and returning an extended
// style to pass to CreateWindowEx.
//
WXDWORD wxWindow::Determine3DEffects(
  WXDWORD                           dwDefaultBorderStyle
, YBool*                            pbWant3D
) const
{
    WXDWORD                         dwStyle = 0L; 
   
    //
    // Native PM does not have any specialize 3D effects like WIN32 does
    //
    *pbWant3D = FALSE;
    return dwStyle;
} // end of wxWindow::Determine3DEffects

#if WXWIN_COMPATIBILITY
void wxWindow::OnCommand(
  wxWindow&                         rWin
, wxCommandEvent&                   rEvent
)
{
    if (GetEventHandler()->ProcessEvent(rEvent))
        return;
    if (m_parent)
        m_parent->GetEventHandler()->OnCommand( rWin
                                               ,rEvent
                                              );
} // end of wxWindow::OnCommand

wxObject* wxWindow::GetChild(
  int                               nNumber
) const
{
    //
    // Return a pointer to the Nth object in the Panel
    //
    wxNode*                         pNode = GetChildren().First();
    int                             n = nNumber;

    while (pNode && n--)
        pNode = pNode->Next();
    if (pNode)
    {
        wxObject*                   pObj = (wxObject*)pNode->Data();
        return(pObj);
    }
    else
        return NULL;
} // end of wxWindow::GetChild

#endif // WXWIN_COMPATIBILITY

//
// Setup background and foreground colours correctly
//
void wxWindow::SetupColours()
{
    if ( GetParent() )
        SetBackgroundColour(GetParent()->GetBackgroundColour());
} // end of wxWindow::SetupColours

void wxWindow::OnIdle(
  wxIdleEvent&                      rEvent
)
{
    //
    // Check if we need to send a LEAVE event
    //
    if (m_bMouseInWindow)
    {
        POINTL                      vPoint;

        ::WinQueryPointerPos(HWND_DESKTOP, &vPoint);
        if (::WinWindowFromPoint(HWND_DESKTOP, &vPoint, FALSE) != (HWND)GetHwnd())
        {
            //
            // Generate a LEAVE event
            //
            m_bMouseInWindow = FALSE;

            //
            // Unfortunately the mouse button and keyboard state may have changed
            // by the time the OnIdle function is called, so 'state' may be
            // meaningless.
            //
            int                     nState = 0;

            if (::WinGetKeyState(HWND_DESKTOP, VK_SHIFT) != 0)
                nState |= VK_SHIFT;
            if (::WinGetKeyState(HWND_DESKTOP, VK_CTRL) != 0;
                nState |= VK_CTRL;

            wxMouseEvent            rEvent(wxEVT_LEAVE_WINDOW);

            InitMouseEvent( rEvent
                           ,vPoint.x
                           ,vPoint.y
                           ,nState
                          );
            (void)GetEventHandler()->ProcessEvent(event);
        }
    }
    UpdateWindowUI();
} // end of wxWindow::OnIdle

//
// Set this window to be the child of 'parent'.
//
bool wxWindow::Reparent(
  wxWindow*                         pParent
)
{
    if (!wxWindowBase::Reparent(pParent))
        return FALSE;

    HWND                            hWndChild = GetHwnd();
    HWND                            hWndParent = GetParent() ? GetWinHwnd(GetParent()) : (HWND)0;

    ::WinSetParent(hWndChild, hWndParent, TRUE);
    return TRUE;
} // end of wxWindow::Reparent

void wxWindow::Clear()
{
    wxClientDC                      vDc(this);
    wxBrush                         vBrush( GetBackgroundColour()
                                           ,wxSOLID
                                          );

    vDc.SetBackground(vBrush);
    vDc.Clear();
} // end of wxWindow::Clear

void wxWindow::Refresh(
  bool                              bEraseBack
, const wxRect*                     pRect
)
{
    HWND                            hWnd = GetHwnd();

    if (hWnd)
    {
        if (pRect)
        {
            RECTL                   vOs2Rect;

            vOs2Rect.xLeft   = pRect->x;
            vOS2Rect.yTop    = pRect->y;
            vOs2Rect.xRight  = pRect->x + pRect->width;
            vOs2Rect.yBottom = pRect->y + pRect->height;

            ::WinInvalidateRect(hWnd, &vOs2Rect, bEraseBack);
        }
        else
            ::WinInvalidateRect(hWnd, NULL, bEraseBack);
    }
} // end of wxWindow::Refresh

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP
void wxWindow::SetDropTarget(
  wxDropTarget*                     pDropTarget
)
{
    if (m_dropTarget != 0) 
    {
        m_dropTarget->Revoke(m_hWnd);
        delete m_dropTarget;
    }
    m_dropTarget = pDropTarget;
    if (m_dropTarget != 0)
        m_dropTarget->Register(m_hWnd);
} // end of wxWindow::SetDropTarget
#endif

//
// old style file-manager drag&drop support: we retain the old-style
// DragAcceptFiles in parallel with SetDropTarget.
//
void wxWindow::DragAcceptFiles(
  bool                              bAccept
)
{
    HWND                            hWnd = GetHwnd();

    if (hWnd && bAccept)
        ::DragAcceptDroppedFiles(hWnd, NULL, NULL, DO_COPY, 0L);
} // end of wxWindow::DragAcceptFiles

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindow::DoSetToolTip(
  wxToolTip*                        pTooltip
)
{
    wxWindowBase::DoSetToolTip(pTooltip);

    if (m_pTooltip)
        m_tooltip->SetWindow(this);
} // end of wxWindow::DoSetToolTip

#endif // wxUSE_TOOLTIPS

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

// Get total size
void wxWindow::DoGetSize( 
  int*                              pWidth
, int*                              pHeight
) const
{
    HWND                            hWnd = GetHwnd();
    RECTL                           vRect;

    ::WinQueryWindowRect(hWnd, &vRect);

    if (pWidth)
        *pWidth = vRect.xRight - vRect.xLeft;
    if (pHeight )
        // OS/2 PM is backwards from windows
        *pHeight = vRect.yTop - vRect.yBottom;
} // end of wxWindow::DoGetSize

void wxWindow::DoGetPosition(
  int*                              pX
, int*                              pY
) const
{
    HWND                            hWnd = GetHwnd();
    RECT                            vRect;
    POINTL                          vPoint;

    ::WinQueryWindowRect(hWnd, &vRect);

    vPoint.x = vRect.xLeft;
    vPoint.y = vRect.yBottom;

    //
    // We do the adjustments with respect to the parent only for the "real"
    // children, not for the dialogs/frames
    //
    if (!IsTopLevel())
    {
        HWND                        hParentWnd = 0;
        wxWindow*                   pParent = GetParent();

        if (pParent)
            hParentWnd = GetWinHwnd(pParent);

        //
        // Since we now have the absolute screen coords, if there's a parent we
        // must subtract its bottom left corner
        //
        if (hParentWnd)
        {
            RECTL                   vRect2;

            ::WinQueryWindowRect(hParentWnd, vRect2);
            vPoint.x -= vRect.xLeft;
            vPoint.y -= vRect.yBottom;
        }

        //
        // We may be faking the client origin. So a window that's really at (0,
        // 30) may appear (to wxWin apps) to be at (0, 0).
        //
        wxPoint                     vPt(pParent->GetClientAreaOrigin());

        vPoint.x -= vPt.x;
        vPoint.y -= vPt.y;
    }

    if (pX)
        *pX = vPoint.x;
    if  y)
        *pY = vPoint.y;
} // end of wxWindow::DoGetPosition

void wxWindow::DoScreenToClient(
  int*                              pX
, int*                              pY
) const
{
    HWND                            hWnd = GetHwnd();
    RECTL                           vRect;

    ::WinQueryWindowPos(hWnd, &vRect);
    
    if (pX)
        *pX -= vRect.xLeft;
    if (pY)
        *pY -= vRect.yBottom;
} // end of wxWindow::DoScreenToClient

void wxWindow::DoClientToScreen(
  int*                              pX
, int*                              pY
) const
{
    HWND                            hWnd = GetHwnd();
    RECTL                           vRect;

    ::WinQueryWindowPos(hWnd, &vRect);

    if (pX)
        *pX += vRect.xLeft;
    if (pY)
        *pY += vRect.yBottom;
} // end of wxWindow::DoClientToScreen

//
// Get size *available for subwindows* i.e. excluding menu bar etc.
// Must be a frame type window
//
void wxWindow::DoGetClientSize(
  int*                              pWidth
, int*                              pHeight
) const
{
    HWND                            hWnd = GetHwnd();
    HWND                            hWndClient;
    RECTL                           vRect;

    hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
    ::WinQueryWindowRect(hWndClient, &vRect);

    if (pX)
        *pWidth  = vRect.xRight;
    if (pY)
        *pHeight = vRect.yTop;
} // end of wxWindow::DoGetClientSize

void wxWindow::DoMoveWindow(
  int                               nX
, int                               nY
, int                               nWidth
, int                               nHeight
)
{
    ::WinSetWindowPos(
    if ( !::MoveWindow( GetHwnd()
                       ,HWND_TOP
                       ,(LONG)nX
                       ,(LONG)nY
                       ,(LONG)nWidth
                       ,(LONG)nHeight
                       ,SWP_SIZE | SWP_MOVE
                       ,
                      ))
    {
        wxLogLastError("MoveWindow");
    }
} // end of wxWindow::DoMoveWindow

//
// Set the size of the window: if the dimensions are positive, just use them,
// but if any of them is equal to -1, it means that we must find the value for
// it ourselves (unless sizeFlags contains wxSIZE_ALLOW_MINUS_ONE flag, in
// which case -1 is a valid value for x and y)
//
// If sizeFlags contains wxSIZE_AUTO_WIDTH/HEIGHT flags (default), we calculate
// the width/height to best suit our contents, otherwise we reuse the current
// width/height
//
void wxWindow::DoSetSize(
  int                               nX
, int                               nY
, int                               nWidth
, int                               nHeight
, int                               nSizeFlags
)
{
    //
    // Get the current size and position...
    //
    int                             nCurrentX;
    int                             nCurrentY;
    int                             nCurrentWidth;
    int                             nCurrentHeight;
    wxSize                          size(-1, -1);

    GetPosition( &nCurrentX
                ,&nCurrentY
               );
    GetSize( &nCurrentWidth
            ,&nCurrentHeight
           );

    //
    // ... and don't do anything (avoiding flicker) if it's already ok
    //
    if ( nX == nCurrentX && 
         nY == nCurrentY &&
         nWidth == nCurrentWidth && 
         nHeight == nCurrentHeight
       )
    {
        return;
    }

    if (nX == -1 && !(nSizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        nX = nCurrentX;
    if (y == -1 && !(nSizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        nY = nCurrentY;

    AdjustForParentClientOrigin( nX
                                ,nY
                                ,nSizeFlags
                               );

    if (nWidth == -1)
    {
        if (nSizeFlags & wxSIZE_AUTO_WIDTH)
        {
            vSize  = DoGetBestSize();
            nWidth = vSize.x;
        }
        else
        {
            //
            // Just take the current one
            //
            nWidth = nCurrentWidth;
        }
    }

    if (nHeight == -1)
    {
        if (nSizeFlags & wxSIZE_AUTO_HEIGHT)
        {
            if (vSize.x == -1)
            {
                vSize = DoGetBestSize();
            }
            nHeight = vSize.y;
        }
        else
        {
            // just take the current one
            nHeight = nCurrentHeight;
        }
    }

    DoMoveWindow( nX
                 ,nY
                 ,nWidth
                 ,nHeight
                );
} // end of wxWindow::DoSetSize

void wxWindow::DoSetClientSize(
  int                               nWidth
, int                               nHeight
)
{
    wxWindow*                       pParent = GetParent();
    HWND                            hWnd = GetHwnd();
    HWND                            hParentWnd = (HWND)0;
    HWND                            hClientWnd = (HWND)0;
    RECTL                           vRect;
    RECT                            vRect2;
    RECT                            vRect3;

    hWndClient = ::WinWindowFromID(GetHwnd(), FID_CLIENT);
    ::WinQueryWindowRect(hClientWnd, &vRect2);

    if (pParent)
        hParentWnd = (HWND) pParent->GetHWND();

    ::WinQueryWindowRect(hWnd, &vRect);
    ::WinQueryWindowRect(hParentWnd, &vRect3);
    //
    // Find the difference between the entire window (title bar and all)
    // and the client area; add this to the new client size to move the
    // window. OS/2 is backward from windows on height
    //
    int                             nActualWidth = vRect2.xRight - vRect2.xLeft - vRect.xRight + nWidth;
    int                             nActualHeight = vRect2.yTop - vRect2.yBottom - vRect.yTop + nHeight;

    //
    // If there's a parent, must subtract the parent's bottom left corner
    // since MoveWindow moves relative to the parent
    //
    POINTL                          vPoint;

    vPoint.x = vRect2.xLeft;
    vPoint.y = vRect2.yBottom;
    if (pParent)
    {
        vPoint.x -= vRect3.xLeft;
        vPoint.y -= vRect3.xBottom;
    }

    DoMoveWindow( vPoint.x
                 ,vPoint.y
                 ,nActualWidth
                 ,nActualHeight
                );

    wxSizeEvent                     vEvent( wxSize( nWidth
                                                  ,nHeight
                                                 )
                                           ,m_windowId
                                          );

    vEvent.SetEventObject(this);
    GetEventHandler()->ProcessEvent(vEvent);
} // end of wxWindow::DoSetClientSize

wxPoint wxWindow::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
} // end of wxWindow::GetClientAreaOrigin

void wxWindow::AdjustForParentClientOrigin(
  int&                              rX
, int&                              rY
, int                               nSizeFlags
)
{
    //
    // Don't do it for the dialogs/frames - they float independently of their
    // parent
    //
    if (!IsTopLevel())
    {
        wxWindow*                   pParent = GetParent();

        if (!(nSizeFlags & wxSIZE_NO_ADJUSTMENTS) && pParent)
        {
            wxPoint                 vPoint(pParent->GetClientAreaOrigin());
            rX += vPoint.x; 
            rY += vPoint.y;
        }
    }
} // end of wxWindow::AdjustForParentClientOrigin

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindow::GetCharHeight() const
{
    HPS                             hPs;
    FONTMETRICS                     vFontMetrics;
    BOOL                            bRc;

    hPs = ::WinGetPS(GetHwnd());

    if(!GipQueryFontMetrics(hPs, sizeof(FONTMETIRCS), &vFontMetrics);
        return (0);
    else
        return(vFontMetrics.lMaxAscender + vFontMetrics.lMaxDescender);
    ::WinReleasePS(hPs);
} // end of wxWindow::GetCharHeight

int wxWindow::GetCharWidth() const
{
    hPs = ::WinGetPS(GetHwnd());

    if(!GipQueryFontMetrics(hPs, sizeof(FONTMETIRCS), &vFontMetrics);
        return (0);
    else
        return(vFontMetrics.lAveCharWidth);
    ::WinReleasePS(hPs);
} // end of wxWindow::GetCharWidth

void wxWindow::GetTextExtent( 
  const wxString&                   rString
, int*                              pX
, int*                              pY
, int*                              pDescent
, int*                              pExternalLeading
, const wxFont*                     pTheFont
) const
{
    const wxFont*                   pFontToUse = pTheFont;
    HPS                             hPs;

    hPs = ::WinGetPS(GetHwnd());
/*
// TODO: Will have to play with fonts later

    if (!pFontToUse)
        pFontToUse = &m_font;

    HFONT                           hFnt = 0;
    HFONT                           hFfontOld = 0;

    if (pFontToUse && pFontToUse->Ok())
    {
        ::GpiCreateLog
        hFnt = (HFONT)((wxFont *)pFontToUse)->GetResourceHandle(); // const_cast
        if (hFnt)
            hFontOld = (HFONT)SelectObject(dc,fnt);
    }

    SIZE sizeRect;
    TEXTMETRIC tm;
    GetTextExtentPoint(dc, string, (int)string.Length(), &sizeRect);
    GetTextMetrics(dc, &tm);

    if ( fontToUse && fnt && hfontOld )
        SelectObject(dc, hfontOld);

    ReleaseDC(hWnd, dc);

    if ( x )
        *x = sizeRect.cx;
    if ( y )
        *y = sizeRect.cy;
    if ( descent )
        *descent = tm.tmDescent;
    if ( externalLeading )
        *externalLeading = tm.tmExternalLeading;
*/
    ::WinReleasePS(hPs);
}

#if wxUSE_CARET && WXWIN_COMPATIBILITY
// ---------------------------------------------------------------------------
// Caret manipulation
// ---------------------------------------------------------------------------

void wxWindow::CreateCaret(
  int                               nWidth
, int                               nHeight
)
{
    SetCaret(new wxCaret( this
                         ,nWidth
                         ,nHeight
                        ));
} // end of wxWindow::CreateCaret

void wxWindow::CreateCaret(
  const wxBitmap*                   pBitmap
)
{
    wxFAIL_MSG("not implemented");
} // end of wxWindow::CreateCaret

void wxWindow::ShowCaret(
  bool                              bShow
)
{
    wxCHECK_RET( m_caret, "no caret to show" );

    m_caret->Show(bShow);
} // end of wxWindow::ShowCaret

void wxWindow::DestroyCaret()
{
    SetCaret(NULL);
} // end of wxWindow::DestroyCaret

void wxWindow::SetCaretPos(
  int                               nX
, int                               nY)
{
    wxCHECK_RET( m_caret, "no caret to move" );

    m_caret->Move( nX
                  ,nY
                 );
} // end of wxWindow::SetCaretPos

void wxWindow::GetCaretPos(
  int*                              pX
, int*                              pY
) const
{
    wxCHECK_RET( m_caret, "no caret to get position of" );

    m_caret->GetPosition( pX
                         ,pY
                        );
} // end of wxWindow::GetCaretPos

#endif //wxUSE_CARET

// ---------------------------------------------------------------------------
// popup menu
// ---------------------------------------------------------------------------

bool wxWindow::DoPopupMenu( wxMenu *menu, int x, int y )
{
    // TODO:
    return(TRUE);
}

// ===========================================================================
// pre/post message processing
// ===========================================================================

MRESULT wxWindow::OS2DefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
   // TODO:
   return (MRESULT)0;
}

bool wxWindow::OS2ProcessMessage(WXMSG* pMsg)
{
   // TODO:
    return FALSE;
}

bool wxWindow::OS2TranslateMessage(WXMSG* pMsg)
{
    return m_acceleratorTable.Translate(this, pMsg);
}

// ---------------------------------------------------------------------------
// message params unpackers (different for Win16 and Win32)
// ---------------------------------------------------------------------------

void wxWindow::UnpackCommand(WXWPARAM wParam, WXLPARAM lParam,
                             WORD *id, WXHWND *hwnd, WORD *cmd)
{
    *id = LOWORD(wParam);
    *hwnd = (WXHWND)lParam;
    *cmd = HIWORD(wParam);
}

void wxWindow::UnpackActivate(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *state, WXWORD *minimized, WXHWND *hwnd)
{
    *state = LOWORD(wParam);
    *minimized = HIWORD(wParam);
    *hwnd = (WXHWND)lParam;
}

void wxWindow::UnpackScroll(WXWPARAM wParam, WXLPARAM lParam,
                            WXWORD *code, WXWORD *pos, WXHWND *hwnd)
{
    *code = LOWORD(wParam);
    *pos = HIWORD(wParam);
    *hwnd = (WXHWND)lParam;
}

void wxWindow::UnpackCtlColor(WXWPARAM wParam, WXLPARAM lParam,
                              WXWORD *nCtlColor, WXHDC *hdc, WXHWND *hwnd)
{
    *nCtlColor = 0; // TODO: CTLCOLOR_BTN;
    *hwnd = (WXHWND)lParam;
    *hdc = (WXHDC)wParam;
}

void wxWindow::UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                                WXWORD *item, WXWORD *flags, WXHMENU *hmenu)
{
    *item = (WXWORD)LOWORD(wParam);
    *flags = HIWORD(wParam);
    *hmenu = (WXHMENU)lParam;
}

// ---------------------------------------------------------------------------
// Main wxWindows window proc and the window proc for wxWindow
// ---------------------------------------------------------------------------

// Hook for new window just as it's being created, when the window isn't yet
// associated with the handle
wxWindow *wxWndHook = NULL;

// Main window proc
MRESULT wxWndProc(HWND hWnd, ULONG message, MPARAM wParam, MPARAM lParam)
{
    // trace all messages - useful for the debugging
#ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, wxT("Processing %s(wParam=%8lx, lParam=%8lx)"),
               wxGetMessageName(message), wParam, lParam);
#endif // __WXDEBUG__

    wxWindow *wnd = wxFindWinFromHandle((WXHWND) hWnd);

    // when we get the first message for the HWND we just created, we associate
    // it with wxWindow stored in wxWndHook
    if ( !wnd && wxWndHook )
    {
#if 0 // def __WXDEBUG__
        char buf[512];
        ::GetClassNameA((HWND) hWnd, buf, 512);
        wxString className(buf);
#endif

        wxAssociateWinWithHandle(hWnd, wxWndHook);
        wnd = wxWndHook;
        wxWndHook = NULL;
        wnd->SetHWND((WXHWND)hWnd);
    }

    MRESULT rc;

    // Stop right here if we don't have a valid handle in our wxWindow object.
    if ( wnd && !wnd->GetHWND() )
    {
        // FIXME: why do we do this?
        wnd->SetHWND((WXHWND) hWnd);
        rc = wnd->OS2DefWindowProc(message, wParam, lParam );
        wnd->SetHWND(0);
    }
    else
    {
        if ( wnd )
            rc = wnd->OS2WindowProc(message, wParam, lParam);
        else
            rc = 0; //TODO: DefWindowProc( hWnd, message, wParam, lParam );
    }

    return rc;
}

MRESULT wxWindow::OS2WindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    // did we process the message?
    bool processed = FALSE;

    // the return value
    union
    {
        bool        allow;
        long        result;
        WXHICON     hIcon;
        WXHBRUSH    hBrush;
    } rc;

    // for most messages we should return 0 when we do process the message
    rc.result = 0;
    // TODO:
/*
    switch ( message )
    {
        case WM_CREATE:
            {
                bool mayCreate;
                processed = HandleCreate((WXLPCREATESTRUCT)lParam, &mayCreate);
                if ( processed )
                {
                    // return 0 to allow window creation
                    rc.result = mayCreate ? 0 : -1;
                }
            }
            break;

        case WM_DESTROY:
            processed = HandleDestroy();
            break;

        case WM_MOVE:
            processed = HandleMove(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_SIZE:
            processed = HandleSize(LOWORD(lParam), HIWORD(lParam), wParam);
            break;

        case WM_ACTIVATE:
            {
                WXWORD state, minimized;
                WXHWND hwnd;
                UnpackActivate(wParam, lParam, &state, &minimized, &hwnd);

                processed = HandleActivate(state, minimized != 0, (WXHWND)hwnd);
            }
            break;

        case WM_SETFOCUS:
            processed = HandleSetFocus((WXHWND)(HWND)wParam);
            break;

        case WM_KILLFOCUS:
            processed = HandleKillFocus((WXHWND)(HWND)wParam);
            break;

        case WM_PAINT:
            processed = HandlePaint();
            break;

        case WM_CLOSE:
            // don't let the DefWindowProc() destroy our window - we'll do it
            // ourselves in ~wxWindow
            processed = TRUE;
            rc.result = TRUE;
            break;

        case WM_SHOWWINDOW:
            processed = HandleShow(wParam != 0, (int)lParam);
            break;

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
            {
                short x = LOWORD(lParam);
                short y = HIWORD(lParam);

                processed = HandleMouseEvent(message, x, y, wParam);
            }
            break;

        case MM_JOY1MOVE:
        case MM_JOY2MOVE:
        case MM_JOY1ZMOVE:
        case MM_JOY2ZMOVE:
        case MM_JOY1BUTTONDOWN:
        case MM_JOY2BUTTONDOWN:
        case MM_JOY1BUTTONUP:
        case MM_JOY2BUTTONUP:
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);

                processed = HandleJoystickEvent(message, x, y, wParam);
            }
            break;

        case WM_SYSCOMMAND:
            processed = HandleSysCommand(wParam, lParam);
            break;

        case WM_COMMAND:
            {
                WORD id, cmd;
                WXHWND hwnd;
                UnpackCommand(wParam, lParam, &id, &hwnd, &cmd);

                processed = HandleCommand(id, cmd, hwnd);
            }
            break;

#ifdef __WIN95__
        case WM_NOTIFY:
            processed = HandleNotify((int)wParam, lParam, &rc.result);
            break;
#endif  // Win95

            // for these messages we must return TRUE if process the message
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
            {
                int idCtrl = (UINT)wParam;
                if ( message == WM_DRAWITEM )
                {
                    processed = MSWOnDrawItem(idCtrl,
                                              (WXDRAWITEMSTRUCT *)lParam);
                }
                else
                {
                    processed = MSWOnMeasureItem(idCtrl,
                                                 (WXMEASUREITEMSTRUCT *)lParam);
                }

                if ( processed )
                    rc.result = TRUE;
            }
            break;

        case WM_GETDLGCODE:
            if ( m_lDlgCode )
            {
                rc.result = m_lDlgCode;
                processed = TRUE;
            }
            //else: get the dlg code from the DefWindowProc()
            break;

        case WM_KEYDOWN:
            // If this has been processed by an event handler,
            // return 0 now (we've handled it).
            if ( HandleKeyDown((WORD) wParam, lParam) )
            {
                processed = TRUE;

                break;
            }

            // we consider these message "not interesting" to OnChar
            if ( wParam == VK_SHIFT || wParam == VK_CONTROL )
            {
                processed = TRUE;

                break;
            }

            switch ( wParam )
            {
                // avoid duplicate messages to OnChar for these ASCII keys: they
                // will be translated by TranslateMessage() and received in WM_CHAR
                case VK_ESCAPE:
                case VK_SPACE:
                case VK_RETURN:
                case VK_BACK:
                case VK_TAB:
                    // but set processed to FALSE, not TRUE to still pass them to
                    // the control's default window proc - otherwise built-in
                    // keyboard handling won't work
                    processed = FALSE;

                    break;

#ifdef VK_APPS
                // special case of VK_APPS: treat it the same as right mouse
                // click because both usually pop up a context menu
                case VK_APPS:
                    {
                        // construct the key mask
                        WPARAM fwKeys = MK_RBUTTON;
                        if ( (::GetKeyState(VK_CONTROL) & 0x100) != 0 )
                            fwKeys |= MK_CONTROL;
                        if ( (::GetKeyState(VK_SHIFT) & 0x100) != 0 )
                            fwKeys |= MK_SHIFT;

                        // simulate right mouse button click
                        DWORD dwPos = ::GetMessagePos();
                        int x = GET_X_LPARAM(dwPos),
                            y = GET_Y_LPARAM(dwPos);

                        ScreenToClient(&x, &y);
                        processed = HandleMouseEvent(WM_RBUTTONDOWN, x, y, fwKeys);
                    }
                    break;
#endif // VK_APPS

                case VK_LEFT:
                case VK_RIGHT:
                case VK_DOWN:
                case VK_UP:
                default:
                    processed = HandleChar((WORD)wParam, lParam);
            }
            break;

        case WM_KEYUP:
            processed = HandleKeyUp((WORD) wParam, lParam);
            break;

        case WM_CHAR: // Always an ASCII character
            processed = HandleChar((WORD)wParam, lParam, TRUE);
            break;

        case WM_HSCROLL:
        case WM_VSCROLL:
            {
                WXWORD code, pos;
                WXHWND hwnd;
                UnpackScroll(wParam, lParam, &code, &pos, &hwnd);

                processed = MSWOnScroll(message == WM_HSCROLL ? wxHORIZONTAL
                                                              : wxVERTICAL,
                                        code, pos, hwnd);
            }
            break;

        // CTLCOLOR messages are sent by children to query the parent for their
        // colors
#ifdef __WIN32__
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
#else // Win16
        case WM_CTLCOLOR:
#endif // Win32/16
            {
                WXWORD nCtlColor;
                WXHDC hdc;
                WXHWND hwnd;
                UnpackCtlColor(wParam, lParam, &nCtlColor, &hdc, &hwnd);

                processed = HandleCtlColor(&rc.hBrush,
                                           (WXHDC)hdc,
                                           (WXHWND)hwnd,
                                           nCtlColor,
                                           message,
                                           wParam,
                                           lParam);
            }
            break;

            // the return value for this message is ignored
        case WM_SYSCOLORCHANGE:
            processed = HandleSysColorChange();
            break;

        case WM_PALETTECHANGED:
            processed = HandlePaletteChanged((WXHWND) (HWND) wParam);
            break;

        case WM_QUERYNEWPALETTE:
            processed = HandleQueryNewPalette();
            break;

        case WM_ERASEBKGND:
            processed = HandleEraseBkgnd((WXHDC)(HDC)wParam);
            if ( processed )
            {
                // we processed the message, i.e. erased the background
                rc.result = TRUE;
            }
            break;

        case WM_DROPFILES:
            processed = HandleDropFiles(wParam);
            break;

        case WM_INITDIALOG:
            processed = HandleInitDialog((WXHWND)(HWND)wParam);

            if ( processed )
            {
                // we never set focus from here
                rc.result = FALSE;
            }
            break;

        case WM_QUERYENDSESSION:
            processed = HandleQueryEndSession(lParam, &rc.allow);
            break;

        case WM_ENDSESSION:
            processed = HandleEndSession(wParam != 0, lParam);
            break;

        case WM_GETMINMAXINFO:
            processed = HandleGetMinMaxInfo((MINMAXINFO*)lParam);
            break;

        case WM_SETCURSOR:
            processed = HandleSetCursor((WXHWND)(HWND)wParam,
                                        LOWORD(lParam),     // hit test
                                        HIWORD(lParam));    // mouse msg

            if ( processed )
            {
                // returning TRUE stops the DefWindowProc() from further
                // processing this message - exactly what we need because we've
                // just set the cursor.
                rc.result = TRUE;
            }
            break;
    }

    if ( !processed )
    {
#ifdef __WXDEBUG__
        wxLogTrace(wxTraceMessages, wxT("Forwarding %s to DefWindowProc."),
                   wxGetMessageName(message));
#endif // __WXDEBUG__
        rc.result = MSWDefWindowProc(message, wParam, lParam);
    }
*/
    return (MRESULT)0;
}

// Dialog window proc
MRESULT wxDlgProc(HWND hWnd, UINT message, MPARAM wParam, MPARAM lParam)
{
   // TODO:
/*
    if ( message == WM_INITDIALOG )
    {
        // for this message, returning TRUE tells system to set focus to the
        // first control in the dialog box
        return TRUE;
    }
    else
    {
        // for all the other ones, FALSE means that we didn't process the
        // message
        return 0;
    }
*/
    return (MRESULT)0;
}

wxWindow *wxFindWinFromHandle(WXHWND hWnd)
{
    wxNode *node = wxWinHandleList->Find((long)hWnd);
    if ( !node )
        return NULL;
    return (wxWindow *)node->Data();
}

void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win)
{
    // adding NULL hWnd is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( hWnd != (HWND)NULL,
                 wxT("attempt to add a NULL hWnd to window list ignored") );


    wxWindow *oldWin = wxFindWinFromHandle((WXHWND) hWnd);
    if ( oldWin && (oldWin != win) )
    {
        wxString str(win->GetClassInfo()->GetClassName());
        wxLogError("Bug! Found existing HWND %X for new window of class %s", (int) hWnd, (const char*) str);
    }
    else if (!oldWin)
    {
        wxWinHandleList->Append((long)hWnd, win);
    }
}

void wxRemoveHandleAssociation(wxWindow *win)
{
    wxWinHandleList->DeleteObject(win);
}

// Default destroyer - override if you destroy it in some other way
// (e.g. with MDI child windows)
void wxWindow::OS2DestroyWindow()
{
}

void wxWindow::OS2DetachWindowMenu()
{
    if ( m_hMenu )
    {
        HMENU hMenu = (HMENU)m_hMenu;

        int N = (int)WinSendMsg(hMenu, MM_QUERYITEMCOUNT, 0, 0);
        int i;
        for (i = 0; i < N; i++)
        {
            wxChar buf[100];
            int chars = (int)WinSendMsg(hMenu, MM_QUERYITEMTEXT, MPFROM2SHORT(i, N), buf);
            if ( !chars )
            {
                wxLogLastError(wxT("GetMenuString"));

                continue;
            }

            if ( wxStrcmp(buf, wxT("&Window")) == 0 )
            {
                WinSendMsg(hMenu, MM_DELETEITEM, MPFROM2SHORT(i, TRUE), 0);
                break;
            }
        }
    }
}

bool wxWindow::OS2Create(int id,
                         wxWindow *parent,
                         const wxChar *wclass,
                         wxWindow *wx_win,
                         const wxChar *title,
                         int x,
                         int y,
                         int width,
                         int height,
                         WXDWORD style,
                         const wxChar *dialog_template,
                         WXDWORD extendedStyle)
{
   // TODO:
/*
    int x1 = CW_USEDEFAULT;
    int y1 = 0;
    int width1 = CW_USEDEFAULT;
    int height1 = 100;

    // Find parent's size, if it exists, to set up a possible default
    // panel size the size of the parent window
    RECT parent_rect;
    if ( parent )
    {
        ::GetClientRect((HWND) parent->GetHWND(), &parent_rect);

        width1 = parent_rect.right - parent_rect.left;
        height1 = parent_rect.bottom - parent_rect.top;
    }

    if ( x > -1 ) x1 = x;
    if ( y > -1 ) y1 = y;
    if ( width > -1 ) width1 = width;
    if ( height > -1 ) height1 = height;

    HWND hParent = (HWND)NULL;
    if ( parent )
        hParent = (HWND) parent->GetHWND();

    wxWndHook = this;

    if ( dialog_template )
    {
        m_hWnd = (WXHWND)::CreateDialog(wxGetInstance(),
                                        dialog_template,
                                        hParent,
                                        (DLGPROC)wxDlgProc);

        if ( m_hWnd == 0 )
        {
            wxLogError(_("Can't find dummy dialog template!\n"
                         "Check resource include path for finding wx.rc."));

            return FALSE;
        }

        // ::SetWindowLong(GWL_EXSTYLE) doesn't work for the dialogs, so try
        // to take care of (at least some) extended style flags ourselves
        if ( extendedStyle & WS_EX_TOPMOST )
        {
            if ( !::SetWindowPos(GetHwnd(), HWND_TOPMOST, 0, 0, 0, 0,
                                 SWP_NOSIZE | SWP_NOMOVE) )
            {
                wxLogLastError(wxT("SetWindowPos"));
            }
        }

        // move the dialog to its initial position without forcing repainting
        if ( !::MoveWindow(GetHwnd(), x1, y1, width1, height1, FALSE) )
        {
            wxLogLastError(wxT("MoveWindow"));
        }
    }
    else
    {
        int controlId = 0;
        if ( style & WS_CHILD )
            controlId = id;

        wxString className(wclass);
        if ( GetWindowStyleFlag() & wxNO_FULL_REPAINT_ON_RESIZE )
        {
            className += wxT("NR");
        }

        m_hWnd = (WXHWND)CreateWindowEx(extendedStyle,
                                        className,
                                        title ? title : wxT(""),
                                        style,
                                        x1, y1,
                                        width1, height1,
                                        hParent, (HMENU)controlId,
                                        wxGetInstance(),
                                        NULL);

        if ( !m_hWnd )
        {
            wxLogError(_("Can't create window of class %s!\n"
                         "Possible Windows 3.x compatibility problem?"),
                       wclass);

            return FALSE;
        }
    }

    wxWndHook = NULL;
#ifdef __WXDEBUG__
    wxNode* node = wxWinHandleList->Member(this);
    if (node)
    {
        HWND hWnd = (HWND) node->GetKeyInteger();
        if (hWnd != (HWND) m_hWnd)
        {
            wxLogError("A second HWND association is being added for the same window!");
        }
    }
#endif
*/
    wxAssociateWinWithHandle((HWND) m_hWnd, this);

    return TRUE;
}

// ===========================================================================
// OS2 PM message handlers
// ===========================================================================

// ---------------------------------------------------------------------------
// WM_NOTIFY
// ---------------------------------------------------------------------------

bool wxWindow::HandleNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
   // TODO:
   return FALSE;
}

bool wxWindow::OS2OnNotify(int WXUNUSED(idCtrl),
                           WXLPARAM lParam,
                           WXLPARAM* WXUNUSED(result))
{
    // TODO:
    return FALSE;
}

// ---------------------------------------------------------------------------
// end session messages
// ---------------------------------------------------------------------------

bool wxWindow::HandleQueryEndSession(long logOff, bool *mayEnd)
{
    wxCloseEvent event(wxEVT_QUERY_END_SESSION, -1);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(TRUE);
    event.SetLoggingOff(logOff == ENDSESSION_LOGOFF);

    bool rc = wxTheApp->ProcessEvent(event);

    if ( rc )
    {
        // we may end only if the app didn't veto session closing (double
        // negation...)
        *mayEnd = !event.GetVeto();
    }

    return rc;
}

bool wxWindow::HandleEndSession(bool endSession, long logOff)
{
    // do nothing if the session isn't ending
    if ( !endSession )
        return FALSE;

    wxCloseEvent event(wxEVT_END_SESSION, -1);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(FALSE);
    event.SetLoggingOff( (logOff == ENDSESSION_LOGOFF) );
    if ( (this == wxTheApp->GetTopWindow()) && // Only send once
        wxTheApp->ProcessEvent(event))
    {
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// window creation/destruction
// ---------------------------------------------------------------------------

bool wxWindow::HandleCreate(WXLPCREATESTRUCT cs, bool *mayCreate)
{
    // TODO: should generate this event from WM_NCCREATE
    wxWindowCreateEvent event(this);
    (void)GetEventHandler()->ProcessEvent(event);

    *mayCreate = TRUE;

    return TRUE;
}

bool wxWindow::HandleDestroy()
{
    wxWindowDestroyEvent event(this);
    (void)GetEventHandler()->ProcessEvent(event);

    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget != NULL )
    {
//        m_dropTarget->Revoke(m_hWnd);

        delete m_dropTarget;
        m_dropTarget = NULL;
    }
#endif // wxUSE_DRAG_AND_DROP

    // WM_DESTROY handled
    return TRUE;
}

// ---------------------------------------------------------------------------
// activation/focus
// ---------------------------------------------------------------------------

bool wxWindow::HandleActivate(int state,
                              bool WXUNUSED(minimized),
                              WXHWND WXUNUSED(activate))
{
    // TODO:
    /*
    wxActivateEvent event(wxEVT_ACTIVATE,
                          (state == WA_ACTIVE) || (state == WA_CLICKACTIVE),
                          m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
    */
    return FALSE;
}

bool wxWindow::HandleSetFocus(WXHWND WXUNUSED(hwnd))
{
#if wxUSE_CARET
    // Deal with caret
    if ( m_caret )
    {
        m_caret->OnSetFocus();
    }
#endif // wxUSE_CARET

    // panel wants to track the window which was the last to have focus in it
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    if ( panel )
    {
        panel->SetLastFocus(this);
    }

    wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleKillFocus(WXHWND WXUNUSED(hwnd))
{
#if wxUSE_CARET
    // Deal with caret
    if ( m_caret )
    {
        m_caret->OnKillFocus();
    }
#endif // wxUSE_CARET

    wxFocusEvent event(wxEVT_KILL_FOCUS, m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

// ---------------------------------------------------------------------------
// miscellaneous
// ---------------------------------------------------------------------------

bool wxWindow::HandleShow(bool show, int status)
{
    wxShowEvent event(GetId(), show);
    event.m_eventObject = this;

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleInitDialog(WXHWND WXUNUSED(hWndFocus))
{
    wxInitDialogEvent event(GetId());
    event.m_eventObject = this;

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleDropFiles(WXWPARAM wParam)
{
   // TODO:
    return FALSE;
}

bool wxWindow::HandleSetCursor(WXHWND hWnd,
                               short nHitTest,
                               int WXUNUSED(mouseMsg))
{
    // don't set cursor for other windows, only for this one: this prevents
    // children of this window from getting the same cursor as the parent has
    // (don't forget that this message is propagated by default up the window
    // parent-child hierarchy)
    if ( GetHWND() == hWnd )
    {
        // don't set cursor when the mouse is not in the client part
// TODO
/*
        if ( nHitTest == HTCLIENT || nHitTest == HTERROR )
        {
            HCURSOR hcursor = 0;
            if ( wxIsBusy() )
            {
                // from msw\utils.cpp
                extern HCURSOR gs_wxBusyCursor;

                hcursor = gs_wxBusyCursor;
            }
            else
            {
                wxCursor *cursor = NULL;

                if ( m_cursor.Ok() )
                {
                    cursor = &m_cursor;
                }
                else
                {
                    // from msw\data.cpp
                    extern wxCursor *g_globalCursor;

                    if ( g_globalCursor && g_globalCursor->Ok() )
                        cursor = g_globalCursor;
                }

                if ( cursor )
                    hcursor = (HCURSOR)cursor->GetHCURSOR();
            }

            if ( hcursor )
            {
//                ::SetCursor(hcursor);

                return TRUE;
            }
        }
*/
    }

    return FALSE;
}

// ---------------------------------------------------------------------------
// owner drawn stuff
// ---------------------------------------------------------------------------

bool wxWindow::OS2OnDrawItem(int id, WXDRAWITEMSTRUCT *itemStruct)
{
   // TODO:
/*
#if wxUSE_OWNER_DRAWN
    // is it a menu item?
    if ( id == 0 )
    {
        DRAWITEMSTRUCT *pDrawStruct = (DRAWITEMSTRUCT *)itemStruct;
        wxMenuItem *pMenuItem = (wxMenuItem *)(pDrawStruct->itemData);

        wxCHECK( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

        // prepare to call OnDrawItem()
        wxDC dc;
        dc.SetHDC((WXHDC)pDrawStruct->hDC, FALSE);
        wxRect rect(pDrawStruct->rcItem.left, pDrawStruct->rcItem.top,
                    pDrawStruct->rcItem.right - pDrawStruct->rcItem.left,
                    pDrawStruct->rcItem.bottom - pDrawStruct->rcItem.top);

        return pMenuItem->OnDrawItem
                          (
                            dc, rect,
                            (wxOwnerDrawn::wxODAction)pDrawStruct->itemAction,
                            (wxOwnerDrawn::wxODStatus)pDrawStruct->itemState
                          );
    }

    wxWindow *item = FindItem(id);
    if ( item && item->IsKindOf(CLASSINFO(wxControl)) )
    {
        return ((wxControl *)item)->MSWOnDraw(itemStruct);
    }
    else
#endif
        return FALSE;
*/
    return FALSE;
}

bool wxWindow::OS2OnMeasureItem(int id, WXMEASUREITEMSTRUCT *itemStruct)
{
   // TODO:
/*
#if wxUSE_OWNER_DRAWN
    // is it a menu item?
    if ( id == 0 )
    {
        MEASUREITEMSTRUCT *pMeasureStruct = (MEASUREITEMSTRUCT *)itemStruct;
        wxMenuItem *pMenuItem = (wxMenuItem *)(pMeasureStruct->itemData);

        wxCHECK( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

        return pMenuItem->OnMeasureItem(&pMeasureStruct->itemWidth,
                                        &pMeasureStruct->itemHeight);
    }

    wxWindow *item = FindItem(id);
    if ( item && item->IsKindOf(CLASSINFO(wxControl)) )
    {
        return ((wxControl *)item)->MSWOnMeasure(itemStruct);
    }
#endif  // owner-drawn menus
*/
    return FALSE;
}

// ---------------------------------------------------------------------------
// colours and palettes
// ---------------------------------------------------------------------------

bool wxWindow::HandleSysColorChange()
{
    wxSysColourChangedEvent event;
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleCtlColor(WXHBRUSH *brush,
                              WXHDC pDC,
                              WXHWND pWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
    WXHBRUSH hBrush = 0;
// TODO:
/*
    if ( nCtlColor == CTLCOLOR_DLG )
    {
        hBrush = OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);
    }
    else
    {
        wxControl *item = (wxControl *)FindItemByHWND(pWnd, TRUE);
        if ( item )
            hBrush = item->OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);
    }

    if ( hBrush )
        *brush = hBrush;

    return hBrush != 0;
*/
    return FALSE;
}

// Define for each class of dialog and control
WXHBRUSH wxWindow::OnCtlColor(WXHDC hDC,
                              WXHWND hWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
    return (WXHBRUSH)0;
}

bool wxWindow::HandlePaletteChanged(WXHWND hWndPalChange)
{
    wxPaletteChangedEvent event(GetId());
    event.SetEventObject(this);
    event.SetChangedWindow(wxFindWinFromHandle(hWndPalChange));

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleQueryNewPalette()
{
    wxQueryNewPaletteEvent event(GetId());
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event) && event.GetPaletteRealized();
}

// Responds to colour changes: passes event on to children.
void wxWindow::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    wxNode *node = GetChildren().First();
    while ( node )
    {
        // Only propagate to non-top-level windows
        wxWindow *win = (wxWindow *)node->Data();
        if ( win->GetParent() )
        {
            wxSysColourChangedEvent event2;
            event.m_eventObject = win;
            win->GetEventHandler()->ProcessEvent(event2);
        }

        node = node->Next();
    }
}

// ---------------------------------------------------------------------------
// painting
// ---------------------------------------------------------------------------

bool wxWindow::HandlePaint()
{
   // TODO:
   return FALSE;
}

bool wxWindow::HandleEraseBkgnd(WXHDC hdc)
{
    // Prevents flicker when dragging
//  if ( ::IsIconic(GetHwnd()) )
//      return TRUE;

    wxDC dc;

    dc.SetHDC(hdc);
    dc.SetWindow(this);
    dc.BeginDrawing();

    wxEraseEvent event(m_windowId, &dc);
    event.SetEventObject(this);
    bool rc = GetEventHandler()->ProcessEvent(event);

    dc.EndDrawing();
    dc.SelectOldObjects(hdc);
    dc.SetHDC((WXHDC) NULL);

    return rc;
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    // TODO:
}

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindow::HandleMinimize()
{
    wxIconizeEvent event(m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleMaximize()
{
    wxMaximizeEvent event(m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleMove(int x, int y)
{
    wxMoveEvent event(wxPoint(x, y), m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleSize(int w, int h, WXUINT WXUNUSED(flag))
{
    wxSizeEvent event(wxSize(w, h), m_windowId);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleGetMinMaxInfo(void *mmInfo)
{
// TODO:
/*
    MINMAXINFO *info = (MINMAXINFO *)mmInfo;

    bool rc = FALSE;

    if ( m_minWidth != -1 )
    {
        info->ptMinTrackSize.x = m_minWidth;
        rc = TRUE;
    }

    if ( m_minHeight != -1 )
    {
        info->ptMinTrackSize.y = m_minHeight;
        rc = TRUE;
    }

    if ( m_maxWidth != -1 )
    {
        info->ptMaxTrackSize.x = m_maxWidth;
        rc = TRUE;
    }

    if ( m_maxHeight != -1 )
    {
        info->ptMaxTrackSize.y = m_maxHeight;
        rc = TRUE;
    }

    return rc;
*/
    return FALSE;
}

// ---------------------------------------------------------------------------
// command messages
// ---------------------------------------------------------------------------

bool wxWindow::HandleCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
    if ( wxCurrentPopupMenu )
    {
        wxMenu *popupMenu = wxCurrentPopupMenu;
        wxCurrentPopupMenu = NULL;

        return popupMenu->OS2Command(cmd, id);
    }

    wxWindow *win = FindItem(id);
    if ( !win )
    {
        win = wxFindWinFromHandle(control);
    }

    if ( win )
        return win->OS2Command(cmd, id);

    return FALSE;
}

bool wxWindow::HandleSysCommand(WXWPARAM wParam, WXLPARAM lParam)
{
   // TODO:
    return FALSE;
}

// ---------------------------------------------------------------------------
// mouse events
// ---------------------------------------------------------------------------

void wxWindow::InitMouseEvent(wxMouseEvent& event, int x, int y, WXUINT flags)
{
// TODO:
/*
    event.m_x = x;
    event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(s_currentMsg.time);
    event.m_eventObject = this;

#if wxUSE_MOUSEEVENT_HACK
    m_lastMouseX = x;
    m_lastMouseY = y;
    m_lastMouseEvent = event.GetEventType();
#endif // wxUSE_MOUSEEVENT_HACK
*/
}

bool wxWindow::HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags)
{
    // the mouse events take consecutive IDs from WM_MOUSEFIRST to
    // WM_MOUSELAST, so it's enough to substract WM_MOUSEMOVE == WM_MOUSEFIRST
    // from the message id and take the value in the table to get wxWin event
    // id
    static const wxEventType eventsMouse[] =
    {
        wxEVT_MOTION,
        wxEVT_LEFT_DOWN,
        wxEVT_LEFT_UP,
        wxEVT_LEFT_DCLICK,
        wxEVT_RIGHT_DOWN,
        wxEVT_RIGHT_UP,
        wxEVT_RIGHT_DCLICK,
        wxEVT_MIDDLE_DOWN,
        wxEVT_MIDDLE_UP,
        wxEVT_MIDDLE_DCLICK
    };

    wxMouseEvent event(eventsMouse[msg - WM_MOUSEMOVE]);
    InitMouseEvent(event, x, y, flags);

    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::HandleMouseMove(int x, int y, WXUINT flags)
{
    if ( !m_bMouseInWindow )
    {
        // Generate an ENTER event
        m_bMouseInWindow = TRUE;

        wxMouseEvent event(wxEVT_ENTER_WINDOW);
        InitMouseEvent(event, x, y, flags);

        (void)GetEventHandler()->ProcessEvent(event);
    }

#if wxUSE_MOUSEEVENT_HACK
    // Window gets a click down message followed by a mouse move message even
    // if position isn't changed!  We want to discard the trailing move event
    // if x and y are the same.
    if ( (m_lastMouseEvent == wxEVT_RIGHT_DOWN ||
          m_lastMouseEvent == wxEVT_LEFT_DOWN ||
          m_lastMouseEvent == wxEVT_MIDDLE_DOWN) &&
         (m_lastMouseX == event.m_x && m_lastMouseY == event.m_y) )
    {
        m_lastMouseEvent = wxEVT_MOTION;

        return FALSE;
    }
#endif // wxUSE_MOUSEEVENT_HACK

    return HandleMouseEvent(WM_MOUSEMOVE, x, y, flags);
}

// ---------------------------------------------------------------------------
// keyboard handling
// ---------------------------------------------------------------------------

// isASCII is TRUE only when we're called from WM_CHAR handler and not from
// WM_KEYDOWN one
bool wxWindow::HandleChar(WXWORD wParam, WXLPARAM lParam, bool isASCII)
{
   // TODO:
   return FALSE;
}

bool wxWindow::HandleKeyDown(WXWORD wParam, WXLPARAM lParam)
{
   // TODO:
   return FALSE;
}

bool wxWindow::HandleKeyUp(WXWORD wParam, WXLPARAM lParam)
{
   // TODO:
   return FALSE;
}

// ---------------------------------------------------------------------------
// joystick
// ---------------------------------------------------------------------------

bool wxWindow::HandleJoystickEvent(WXUINT msg, int x, int y, WXUINT flags)
{
   // TODO:
   return FALSE;
}

// ---------------------------------------------------------------------------
// scrolling
// ---------------------------------------------------------------------------

bool wxWindow::OS2OnScroll(int orientation, WXWORD wParam,
                           WXWORD pos, WXHWND control)
{
    if ( control )
    {
        wxWindow *child = wxFindWinFromHandle(control);
        if ( child )
            return child->OS2OnScroll(orientation, wParam, pos, control);
    }

    wxScrollWinEvent event;
    event.SetPosition(pos);
    event.SetOrientation(orientation);
    event.m_eventObject = this;
   // TODO:
   return FALSE;
}

// ===========================================================================
// global functions
// ===========================================================================

void wxGetCharSize(WXHWND wnd, int *x, int *y,wxFont *the_font)
{
   // TODO:
}

// Returns 0 if was a normal ASCII value, not a special key. This indicates that
// the key should be ignored by WM_KEYDOWN and processed by WM_CHAR instead.
int wxCharCodeOS2ToWX(int keySym)
{
    int id = 0;
   // TODO:
/*
    switch (keySym)
    {
    case VK_CANCEL:     id = WXK_CANCEL; break;
    case VK_BACK:       id = WXK_BACK; break;
    case VK_TAB:        id = WXK_TAB; break;
    case VK_CLEAR:      id = WXK_CLEAR; break;
    case VK_RETURN:     id = WXK_RETURN; break;
    case VK_SHIFT:      id = WXK_SHIFT; break;
    case VK_CONTROL:    id = WXK_CONTROL; break;
    case VK_MENU :      id = WXK_MENU; break;
    case VK_PAUSE:      id = WXK_PAUSE; break;
    case VK_SPACE:      id = WXK_SPACE; break;
    case VK_ESCAPE:     id = WXK_ESCAPE; break;
    case VK_PRIOR:      id = WXK_PRIOR; break;
    case VK_NEXT :      id = WXK_NEXT; break;
    case VK_END:        id = WXK_END; break;
    case VK_HOME :      id = WXK_HOME; break;
    case VK_LEFT :      id = WXK_LEFT; break;
    case VK_UP:         id = WXK_UP; break;
    case VK_RIGHT:      id = WXK_RIGHT; break;
    case VK_DOWN :      id = WXK_DOWN; break;
    case VK_SELECT:     id = WXK_SELECT; break;
    case VK_PRINT:      id = WXK_PRINT; break;
    case VK_EXECUTE:    id = WXK_EXECUTE; break;
    case VK_INSERT:     id = WXK_INSERT; break;
    case VK_DELETE:     id = WXK_DELETE; break;
    case VK_HELP :      id = WXK_HELP; break;
    case VK_NUMPAD0:    id = WXK_NUMPAD0; break;
    case VK_NUMPAD1:    id = WXK_NUMPAD1; break;
    case VK_NUMPAD2:    id = WXK_NUMPAD2; break;
    case VK_NUMPAD3:    id = WXK_NUMPAD3; break;
    case VK_NUMPAD4:    id = WXK_NUMPAD4; break;
    case VK_NUMPAD5:    id = WXK_NUMPAD5; break;
    case VK_NUMPAD6:    id = WXK_NUMPAD6; break;
    case VK_NUMPAD7:    id = WXK_NUMPAD7; break;
    case VK_NUMPAD8:    id = WXK_NUMPAD8; break;
    case VK_NUMPAD9:    id = WXK_NUMPAD9; break;
    case VK_MULTIPLY:   id = WXK_MULTIPLY; break;
    case VK_ADD:        id = WXK_ADD; break;
    case VK_SUBTRACT:   id = WXK_SUBTRACT; break;
    case VK_DECIMAL:    id = WXK_DECIMAL; break;
    case VK_DIVIDE:     id = WXK_DIVIDE; break;
    case VK_F1:         id = WXK_F1; break;
    case VK_F2:         id = WXK_F2; break;
    case VK_F3:         id = WXK_F3; break;
    case VK_F4:         id = WXK_F4; break;
    case VK_F5:         id = WXK_F5; break;
    case VK_F6:         id = WXK_F6; break;
    case VK_F7:         id = WXK_F7; break;
    case VK_F8:         id = WXK_F8; break;
    case VK_F9:         id = WXK_F9; break;
    case VK_F10:        id = WXK_F10; break;
    case VK_F11:        id = WXK_F11; break;
    case VK_F12:        id = WXK_F12; break;
    case VK_F13:        id = WXK_F13; break;
    case VK_F14:        id = WXK_F14; break;
    case VK_F15:        id = WXK_F15; break;
    case VK_F16:        id = WXK_F16; break;
    case VK_F17:        id = WXK_F17; break;
    case VK_F18:        id = WXK_F18; break;
    case VK_F19:        id = WXK_F19; break;
    case VK_F20:        id = WXK_F20; break;
    case VK_F21:        id = WXK_F21; break;
    case VK_F22:        id = WXK_F22; break;
    case VK_F23:        id = WXK_F23; break;
    case VK_F24:        id = WXK_F24; break;
    case VK_NUMLOCK:    id = WXK_NUMLOCK; break;
    case VK_SCROLL:     id = WXK_SCROLL; break;
    default:
        {
            return 0;
        }
    }
*/
    return id;
}

int wxCharCodeWXToOS2(int id, bool *isVirtual)
{
    *isVirtual = TRUE;
    int keySym = 0;
    // TODO
/*
    switch (id)
    {
    case WXK_CANCEL:    keySym = VK_CANCEL; break;
    case WXK_CLEAR:     keySym = VK_CLEAR; break;
    case WXK_SHIFT:     keySym = VK_SHIFT; break;
    case WXK_CONTROL:   keySym = VK_CONTROL; break;
    case WXK_MENU :     keySym = VK_MENU; break;
    case WXK_PAUSE:     keySym = VK_PAUSE; break;
    case WXK_PRIOR:     keySym = VK_PRIOR; break;
    case WXK_NEXT :     keySym = VK_NEXT; break;
    case WXK_END:       keySym = VK_END; break;
    case WXK_HOME :     keySym = VK_HOME; break;
    case WXK_LEFT :     keySym = VK_LEFT; break;
    case WXK_UP:        keySym = VK_UP; break;
    case WXK_RIGHT:     keySym = VK_RIGHT; break;
    case WXK_DOWN :     keySym = VK_DOWN; break;
    case WXK_SELECT:    keySym = VK_SELECT; break;
    case WXK_PRINT:     keySym = VK_PRINT; break;
    case WXK_EXECUTE:   keySym = VK_EXECUTE; break;
    case WXK_INSERT:    keySym = VK_INSERT; break;
    case WXK_DELETE:    keySym = VK_DELETE; break;
    case WXK_HELP :     keySym = VK_HELP; break;
    case WXK_NUMPAD0:   keySym = VK_NUMPAD0; break;
    case WXK_NUMPAD1:   keySym = VK_NUMPAD1; break;
    case WXK_NUMPAD2:   keySym = VK_NUMPAD2; break;
    case WXK_NUMPAD3:   keySym = VK_NUMPAD3; break;
    case WXK_NUMPAD4:   keySym = VK_NUMPAD4; break;
    case WXK_NUMPAD5:   keySym = VK_NUMPAD5; break;
    case WXK_NUMPAD6:   keySym = VK_NUMPAD6; break;
    case WXK_NUMPAD7:   keySym = VK_NUMPAD7; break;
    case WXK_NUMPAD8:   keySym = VK_NUMPAD8; break;
    case WXK_NUMPAD9:   keySym = VK_NUMPAD9; break;
    case WXK_MULTIPLY:  keySym = VK_MULTIPLY; break;
    case WXK_ADD:       keySym = VK_ADD; break;
    case WXK_SUBTRACT:  keySym = VK_SUBTRACT; break;
    case WXK_DECIMAL:   keySym = VK_DECIMAL; break;
    case WXK_DIVIDE:    keySym = VK_DIVIDE; break;
    case WXK_F1:        keySym = VK_F1; break;
    case WXK_F2:        keySym = VK_F2; break;
    case WXK_F3:        keySym = VK_F3; break;
    case WXK_F4:        keySym = VK_F4; break;
    case WXK_F5:        keySym = VK_F5; break;
    case WXK_F6:        keySym = VK_F6; break;
    case WXK_F7:        keySym = VK_F7; break;
    case WXK_F8:        keySym = VK_F8; break;
    case WXK_F9:        keySym = VK_F9; break;
    case WXK_F10:       keySym = VK_F10; break;
    case WXK_F11:       keySym = VK_F11; break;
    case WXK_F12:       keySym = VK_F12; break;
    case WXK_F13:       keySym = VK_F13; break;
    case WXK_F14:       keySym = VK_F14; break;
    case WXK_F15:       keySym = VK_F15; break;
    case WXK_F16:       keySym = VK_F16; break;
    case WXK_F17:       keySym = VK_F17; break;
    case WXK_F18:       keySym = VK_F18; break;
    case WXK_F19:       keySym = VK_F19; break;
    case WXK_F20:       keySym = VK_F20; break;
    case WXK_F21:       keySym = VK_F21; break;
    case WXK_F22:       keySym = VK_F22; break;
    case WXK_F23:       keySym = VK_F23; break;
    case WXK_F24:       keySym = VK_F24; break;
    case WXK_NUMLOCK:   keySym = VK_NUMLOCK; break;
    case WXK_SCROLL:    keySym = VK_SCROLL; break;
    default:
        {
            *isVirtual = FALSE;
            keySym = id;
            break;
        }
    }
*/
    return keySym;
}

wxWindow *wxGetActiveWindow()
{
   // TODO
    return NULL;
}

// Windows keyboard hook. Allows interception of e.g. F1, ESCAPE
// in active frames and dialogs, regardless of where the focus is.
//static HHOOK wxTheKeyboardHook = 0;
//static FARPROC wxTheKeyboardHookProc = 0;
int wxKeyboardHook(int nCode, WORD wParam, DWORD lParam);

void wxSetKeyboardHook(bool doIt)
{
   // TODO:
}

int wxKeyboardHook(int nCode, WORD wParam, DWORD lParam)
{
   // TODO:

    return 0;
}

#ifdef __WXDEBUG__
const char *wxGetMessageName(int message)
{
   // TODO
/*
    switch ( message )
    {
        case 0x0000: return "WM_NULL";
        case 0x0001: return "WM_CREATE";
        case 0x0002: return "WM_DESTROY";
        case 0x0003: return "WM_MOVE";
        case 0x0005: return "WM_SIZE";
        case 0x0006: return "WM_ACTIVATE";
        case 0x0007: return "WM_SETFOCUS";
        case 0x0008: return "WM_KILLFOCUS";
        case 0x000A: return "WM_ENABLE";
        case 0x000B: return "WM_SETREDRAW";
        case 0x000C: return "WM_SETTEXT";
        case 0x000D: return "WM_GETTEXT";
        case 0x000E: return "WM_GETTEXTLENGTH";
        case 0x000F: return "WM_PAINT";
        case 0x0010: return "WM_CLOSE";
        case 0x0011: return "WM_QUERYENDSESSION";
        case 0x0012: return "WM_QUIT";
        case 0x0013: return "WM_QUERYOPEN";
        case 0x0014: return "WM_ERASEBKGND";
        case 0x0015: return "WM_SYSCOLORCHANGE";
        case 0x0016: return "WM_ENDSESSION";
        case 0x0017: return "WM_SYSTEMERROR";
        case 0x0018: return "WM_SHOWWINDOW";
        case 0x0019: return "WM_CTLCOLOR";
        case 0x001A: return "WM_WININICHANGE";
        case 0x001B: return "WM_DEVMODECHANGE";
        case 0x001C: return "WM_ACTIVATEAPP";
        case 0x001D: return "WM_FONTCHANGE";
        case 0x001E: return "WM_TIMECHANGE";
        case 0x001F: return "WM_CANCELMODE";
        case 0x0020: return "WM_SETCURSOR";
        case 0x0021: return "WM_MOUSEACTIVATE";
        case 0x0022: return "WM_CHILDACTIVATE";
        case 0x0023: return "WM_QUEUESYNC";
        case 0x0024: return "WM_GETMINMAXINFO";
        case 0x0026: return "WM_PAINTICON";
        case 0x0027: return "WM_ICONERASEBKGND";
        case 0x0028: return "WM_NEXTDLGCTL";
        case 0x002A: return "WM_SPOOLERSTATUS";
        case 0x002B: return "WM_DRAWITEM";
        case 0x002C: return "WM_MEASUREITEM";
        case 0x002D: return "WM_DELETEITEM";
        case 0x002E: return "WM_VKEYTOITEM";
        case 0x002F: return "WM_CHARTOITEM";
        case 0x0030: return "WM_SETFONT";
        case 0x0031: return "WM_GETFONT";
        case 0x0037: return "WM_QUERYDRAGICON";
        case 0x0039: return "WM_COMPAREITEM";
        case 0x0041: return "WM_COMPACTING";
        case 0x0044: return "WM_COMMNOTIFY";
        case 0x0046: return "WM_WINDOWPOSCHANGING";
        case 0x0047: return "WM_WINDOWPOSCHANGED";
        case 0x0048: return "WM_POWER";

#ifdef  __WIN32__
        case 0x004A: return "WM_COPYDATA";
        case 0x004B: return "WM_CANCELJOURNAL";
        case 0x004E: return "WM_NOTIFY";
        case 0x0050: return "WM_INPUTLANGCHANGEREQUEST";
        case 0x0051: return "WM_INPUTLANGCHANGE";
        case 0x0052: return "WM_TCARD";
        case 0x0053: return "WM_HELP";
        case 0x0054: return "WM_USERCHANGED";
        case 0x0055: return "WM_NOTIFYFORMAT";
        case 0x007B: return "WM_CONTEXTMENU";
        case 0x007C: return "WM_STYLECHANGING";
        case 0x007D: return "WM_STYLECHANGED";
        case 0x007E: return "WM_DISPLAYCHANGE";
        case 0x007F: return "WM_GETICON";
        case 0x0080: return "WM_SETICON";
#endif  //WIN32

        case 0x0081: return "WM_NCCREATE";
        case 0x0082: return "WM_NCDESTROY";
        case 0x0083: return "WM_NCCALCSIZE";
        case 0x0084: return "WM_NCHITTEST";
        case 0x0085: return "WM_NCPAINT";
        case 0x0086: return "WM_NCACTIVATE";
        case 0x0087: return "WM_GETDLGCODE";
        case 0x00A0: return "WM_NCMOUSEMOVE";
        case 0x00A1: return "WM_NCLBUTTONDOWN";
        case 0x00A2: return "WM_NCLBUTTONUP";
        case 0x00A3: return "WM_NCLBUTTONDBLCLK";
        case 0x00A4: return "WM_NCRBUTTONDOWN";
        case 0x00A5: return "WM_NCRBUTTONUP";
        case 0x00A6: return "WM_NCRBUTTONDBLCLK";
        case 0x00A7: return "WM_NCMBUTTONDOWN";
        case 0x00A8: return "WM_NCMBUTTONUP";
        case 0x00A9: return "WM_NCMBUTTONDBLCLK";
        case 0x0100: return "WM_KEYDOWN";
        case 0x0101: return "WM_KEYUP";
        case 0x0102: return "WM_CHAR";
        case 0x0103: return "WM_DEADCHAR";
        case 0x0104: return "WM_SYSKEYDOWN";
        case 0x0105: return "WM_SYSKEYUP";
        case 0x0106: return "WM_SYSCHAR";
        case 0x0107: return "WM_SYSDEADCHAR";
        case 0x0108: return "WM_KEYLAST";

#ifdef  __WIN32__
        case 0x010D: return "WM_IME_STARTCOMPOSITION";
        case 0x010E: return "WM_IME_ENDCOMPOSITION";
        case 0x010F: return "WM_IME_COMPOSITION";
#endif  //WIN32

        case 0x0110: return "WM_INITDIALOG";
        case 0x0111: return "WM_COMMAND";
        case 0x0112: return "WM_SYSCOMMAND";
        case 0x0113: return "WM_TIMER";
        case 0x0114: return "WM_HSCROLL";
        case 0x0115: return "WM_VSCROLL";
        case 0x0116: return "WM_INITMENU";
        case 0x0117: return "WM_INITMENUPOPUP";
        case 0x011F: return "WM_MENUSELECT";
        case 0x0120: return "WM_MENUCHAR";
        case 0x0121: return "WM_ENTERIDLE";
        case 0x0200: return "WM_MOUSEMOVE";
        case 0x0201: return "WM_LBUTTONDOWN";
        case 0x0202: return "WM_LBUTTONUP";
        case 0x0203: return "WM_LBUTTONDBLCLK";
        case 0x0204: return "WM_RBUTTONDOWN";
        case 0x0205: return "WM_RBUTTONUP";
        case 0x0206: return "WM_RBUTTONDBLCLK";
        case 0x0207: return "WM_MBUTTONDOWN";
        case 0x0208: return "WM_MBUTTONUP";
        case 0x0209: return "WM_MBUTTONDBLCLK";
        case 0x0210: return "WM_PARENTNOTIFY";
        case 0x0211: return "WM_ENTERMENULOOP";
        case 0x0212: return "WM_EXITMENULOOP";

#ifdef  __WIN32__
        case 0x0213: return "WM_NEXTMENU";
        case 0x0214: return "WM_SIZING";
        case 0x0215: return "WM_CAPTURECHANGED";
        case 0x0216: return "WM_MOVING";
        case 0x0218: return "WM_POWERBROADCAST";
        case 0x0219: return "WM_DEVICECHANGE";
#endif  //WIN32

        case 0x0220: return "WM_MDICREATE";
        case 0x0221: return "WM_MDIDESTROY";
        case 0x0222: return "WM_MDIACTIVATE";
        case 0x0223: return "WM_MDIRESTORE";
        case 0x0224: return "WM_MDINEXT";
        case 0x0225: return "WM_MDIMAXIMIZE";
        case 0x0226: return "WM_MDITILE";
        case 0x0227: return "WM_MDICASCADE";
        case 0x0228: return "WM_MDIICONARRANGE";
        case 0x0229: return "WM_MDIGETACTIVE";
        case 0x0230: return "WM_MDISETMENU";
        case 0x0233: return "WM_DROPFILES";

#ifdef  __WIN32__
        case 0x0281: return "WM_IME_SETCONTEXT";
        case 0x0282: return "WM_IME_NOTIFY";
        case 0x0283: return "WM_IME_CONTROL";
        case 0x0284: return "WM_IME_COMPOSITIONFULL";
        case 0x0285: return "WM_IME_SELECT";
        case 0x0286: return "WM_IME_CHAR";
        case 0x0290: return "WM_IME_KEYDOWN";
        case 0x0291: return "WM_IME_KEYUP";
#endif  //WIN32

        case 0x0300: return "WM_CUT";
        case 0x0301: return "WM_COPY";
        case 0x0302: return "WM_PASTE";
        case 0x0303: return "WM_CLEAR";
        case 0x0304: return "WM_UNDO";
        case 0x0305: return "WM_RENDERFORMAT";
        case 0x0306: return "WM_RENDERALLFORMATS";
        case 0x0307: return "WM_DESTROYCLIPBOARD";
        case 0x0308: return "WM_DRAWCLIPBOARD";
        case 0x0309: return "WM_PAINTCLIPBOARD";
        case 0x030A: return "WM_VSCROLLCLIPBOARD";
        case 0x030B: return "WM_SIZECLIPBOARD";
        case 0x030C: return "WM_ASKCBFORMATNAME";
        case 0x030D: return "WM_CHANGECBCHAIN";
        case 0x030E: return "WM_HSCROLLCLIPBOARD";
        case 0x030F: return "WM_QUERYNEWPALETTE";
        case 0x0310: return "WM_PALETTEISCHANGING";
        case 0x0311: return "WM_PALETTECHANGED";

#ifdef __WIN32__
        // common controls messages - although they're not strictly speaking
        // standard, it's nice to decode them nevertheless

        // listview
        case 0x1000 + 0: return "LVM_GETBKCOLOR";
        case 0x1000 + 1: return "LVM_SETBKCOLOR";
        case 0x1000 + 2: return "LVM_GETIMAGELIST";
        case 0x1000 + 3: return "LVM_SETIMAGELIST";
        case 0x1000 + 4: return "LVM_GETITEMCOUNT";
        case 0x1000 + 5: return "LVM_GETITEMA";
        case 0x1000 + 75: return "LVM_GETITEMW";
        case 0x1000 + 6: return "LVM_SETITEMA";
        case 0x1000 + 76: return "LVM_SETITEMW";
        case 0x1000 + 7: return "LVM_INSERTITEMA";
        case 0x1000 + 77: return "LVM_INSERTITEMW";
        case 0x1000 + 8: return "LVM_DELETEITEM";
        case 0x1000 + 9: return "LVM_DELETEALLITEMS";
        case 0x1000 + 10: return "LVM_GETCALLBACKMASK";
        case 0x1000 + 11: return "LVM_SETCALLBACKMASK";
        case 0x1000 + 12: return "LVM_GETNEXTITEM";
        case 0x1000 + 13: return "LVM_FINDITEMA";
        case 0x1000 + 83: return "LVM_FINDITEMW";
        case 0x1000 + 14: return "LVM_GETITEMRECT";
        case 0x1000 + 15: return "LVM_SETITEMPOSITION";
        case 0x1000 + 16: return "LVM_GETITEMPOSITION";
        case 0x1000 + 17: return "LVM_GETSTRINGWIDTHA";
        case 0x1000 + 87: return "LVM_GETSTRINGWIDTHW";
        case 0x1000 + 18: return "LVM_HITTEST";
        case 0x1000 + 19: return "LVM_ENSUREVISIBLE";
        case 0x1000 + 20: return "LVM_SCROLL";
        case 0x1000 + 21: return "LVM_REDRAWITEMS";
        case 0x1000 + 22: return "LVM_ARRANGE";
        case 0x1000 + 23: return "LVM_EDITLABELA";
        case 0x1000 + 118: return "LVM_EDITLABELW";
        case 0x1000 + 24: return "LVM_GETEDITCONTROL";
        case 0x1000 + 25: return "LVM_GETCOLUMNA";
        case 0x1000 + 95: return "LVM_GETCOLUMNW";
        case 0x1000 + 26: return "LVM_SETCOLUMNA";
        case 0x1000 + 96: return "LVM_SETCOLUMNW";
        case 0x1000 + 27: return "LVM_INSERTCOLUMNA";
        case 0x1000 + 97: return "LVM_INSERTCOLUMNW";
        case 0x1000 + 28: return "LVM_DELETECOLUMN";
        case 0x1000 + 29: return "LVM_GETCOLUMNWIDTH";
        case 0x1000 + 30: return "LVM_SETCOLUMNWIDTH";
        case 0x1000 + 31: return "LVM_GETHEADER";
        case 0x1000 + 33: return "LVM_CREATEDRAGIMAGE";
        case 0x1000 + 34: return "LVM_GETVIEWRECT";
        case 0x1000 + 35: return "LVM_GETTEXTCOLOR";
        case 0x1000 + 36: return "LVM_SETTEXTCOLOR";
        case 0x1000 + 37: return "LVM_GETTEXTBKCOLOR";
        case 0x1000 + 38: return "LVM_SETTEXTBKCOLOR";
        case 0x1000 + 39: return "LVM_GETTOPINDEX";
        case 0x1000 + 40: return "LVM_GETCOUNTPERPAGE";
        case 0x1000 + 41: return "LVM_GETORIGIN";
        case 0x1000 + 42: return "LVM_UPDATE";
        case 0x1000 + 43: return "LVM_SETITEMSTATE";
        case 0x1000 + 44: return "LVM_GETITEMSTATE";
        case 0x1000 + 45: return "LVM_GETITEMTEXTA";
        case 0x1000 + 115: return "LVM_GETITEMTEXTW";
        case 0x1000 + 46: return "LVM_SETITEMTEXTA";
        case 0x1000 + 116: return "LVM_SETITEMTEXTW";
        case 0x1000 + 47: return "LVM_SETITEMCOUNT";
        case 0x1000 + 48: return "LVM_SORTITEMS";
        case 0x1000 + 49: return "LVM_SETITEMPOSITION32";
        case 0x1000 + 50: return "LVM_GETSELECTEDCOUNT";
        case 0x1000 + 51: return "LVM_GETITEMSPACING";
        case 0x1000 + 52: return "LVM_GETISEARCHSTRINGA";
        case 0x1000 + 117: return "LVM_GETISEARCHSTRINGW";
        case 0x1000 + 53: return "LVM_SETICONSPACING";
        case 0x1000 + 54: return "LVM_SETEXTENDEDLISTVIEWSTYLE";
        case 0x1000 + 55: return "LVM_GETEXTENDEDLISTVIEWSTYLE";
        case 0x1000 + 56: return "LVM_GETSUBITEMRECT";
        case 0x1000 + 57: return "LVM_SUBITEMHITTEST";
        case 0x1000 + 58: return "LVM_SETCOLUMNORDERARRAY";
        case 0x1000 + 59: return "LVM_GETCOLUMNORDERARRAY";
        case 0x1000 + 60: return "LVM_SETHOTITEM";
        case 0x1000 + 61: return "LVM_GETHOTITEM";
        case 0x1000 + 62: return "LVM_SETHOTCURSOR";
        case 0x1000 + 63: return "LVM_GETHOTCURSOR";
        case 0x1000 + 64: return "LVM_APPROXIMATEVIEWRECT";
        case 0x1000 + 65: return "LVM_SETWORKAREA";

        // tree view
        case 0x1100 + 0: return "TVM_INSERTITEMA";
        case 0x1100 + 50: return "TVM_INSERTITEMW";
        case 0x1100 + 1: return "TVM_DELETEITEM";
        case 0x1100 + 2: return "TVM_EXPAND";
        case 0x1100 + 4: return "TVM_GETITEMRECT";
        case 0x1100 + 5: return "TVM_GETCOUNT";
        case 0x1100 + 6: return "TVM_GETINDENT";
        case 0x1100 + 7: return "TVM_SETINDENT";
        case 0x1100 + 8: return "TVM_GETIMAGELIST";
        case 0x1100 + 9: return "TVM_SETIMAGELIST";
        case 0x1100 + 10: return "TVM_GETNEXTITEM";
        case 0x1100 + 11: return "TVM_SELECTITEM";
        case 0x1100 + 12: return "TVM_GETITEMA";
        case 0x1100 + 62: return "TVM_GETITEMW";
        case 0x1100 + 13: return "TVM_SETITEMA";
        case 0x1100 + 63: return "TVM_SETITEMW";
        case 0x1100 + 14: return "TVM_EDITLABELA";
        case 0x1100 + 65: return "TVM_EDITLABELW";
        case 0x1100 + 15: return "TVM_GETEDITCONTROL";
        case 0x1100 + 16: return "TVM_GETVISIBLECOUNT";
        case 0x1100 + 17: return "TVM_HITTEST";
        case 0x1100 + 18: return "TVM_CREATEDRAGIMAGE";
        case 0x1100 + 19: return "TVM_SORTCHILDREN";
        case 0x1100 + 20: return "TVM_ENSUREVISIBLE";
        case 0x1100 + 21: return "TVM_SORTCHILDRENCB";
        case 0x1100 + 22: return "TVM_ENDEDITLABELNOW";
        case 0x1100 + 23: return "TVM_GETISEARCHSTRINGA";
        case 0x1100 + 64: return "TVM_GETISEARCHSTRINGW";
        case 0x1100 + 24: return "TVM_SETTOOLTIPS";
        case 0x1100 + 25: return "TVM_GETTOOLTIPS";

        // header
        case 0x1200 + 0: return "HDM_GETITEMCOUNT";
        case 0x1200 + 1: return "HDM_INSERTITEMA";
        case 0x1200 + 10: return "HDM_INSERTITEMW";
        case 0x1200 + 2: return "HDM_DELETEITEM";
        case 0x1200 + 3: return "HDM_GETITEMA";
        case 0x1200 + 11: return "HDM_GETITEMW";
        case 0x1200 + 4: return "HDM_SETITEMA";
        case 0x1200 + 12: return "HDM_SETITEMW";
        case 0x1200 + 5: return "HDM_LAYOUT";
        case 0x1200 + 6: return "HDM_HITTEST";
        case 0x1200 + 7: return "HDM_GETITEMRECT";
        case 0x1200 + 8: return "HDM_SETIMAGELIST";
        case 0x1200 + 9: return "HDM_GETIMAGELIST";
        case 0x1200 + 15: return "HDM_ORDERTOINDEX";
        case 0x1200 + 16: return "HDM_CREATEDRAGIMAGE";
        case 0x1200 + 17: return "HDM_GETORDERARRAY";
        case 0x1200 + 18: return "HDM_SETORDERARRAY";
        case 0x1200 + 19: return "HDM_SETHOTDIVIDER";

        // tab control
        case 0x1300 + 2: return "TCM_GETIMAGELIST";
        case 0x1300 + 3: return "TCM_SETIMAGELIST";
        case 0x1300 + 4: return "TCM_GETITEMCOUNT";
        case 0x1300 + 5: return "TCM_GETITEMA";
        case 0x1300 + 60: return "TCM_GETITEMW";
        case 0x1300 + 6: return "TCM_SETITEMA";
        case 0x1300 + 61: return "TCM_SETITEMW";
        case 0x1300 + 7: return "TCM_INSERTITEMA";
        case 0x1300 + 62: return "TCM_INSERTITEMW";
        case 0x1300 + 8: return "TCM_DELETEITEM";
        case 0x1300 + 9: return "TCM_DELETEALLITEMS";
        case 0x1300 + 10: return "TCM_GETITEMRECT";
        case 0x1300 + 11: return "TCM_GETCURSEL";
        case 0x1300 + 12: return "TCM_SETCURSEL";
        case 0x1300 + 13: return "TCM_HITTEST";
        case 0x1300 + 14: return "TCM_SETITEMEXTRA";
        case 0x1300 + 40: return "TCM_ADJUSTRECT";
        case 0x1300 + 41: return "TCM_SETITEMSIZE";
        case 0x1300 + 42: return "TCM_REMOVEIMAGE";
        case 0x1300 + 43: return "TCM_SETPADDING";
        case 0x1300 + 44: return "TCM_GETROWCOUNT";
        case 0x1300 + 45: return "TCM_GETTOOLTIPS";
        case 0x1300 + 46: return "TCM_SETTOOLTIPS";
        case 0x1300 + 47: return "TCM_GETCURFOCUS";
        case 0x1300 + 48: return "TCM_SETCURFOCUS";
        case 0x1300 + 49: return "TCM_SETMINTABWIDTH";
        case 0x1300 + 50: return "TCM_DESELECTALL";

        // toolbar
        case WM_USER+1: return "TB_ENABLEBUTTON";
        case WM_USER+2: return "TB_CHECKBUTTON";
        case WM_USER+3: return "TB_PRESSBUTTON";
        case WM_USER+4: return "TB_HIDEBUTTON";
        case WM_USER+5: return "TB_INDETERMINATE";
        case WM_USER+9: return "TB_ISBUTTONENABLED";
        case WM_USER+10: return "TB_ISBUTTONCHECKED";
        case WM_USER+11: return "TB_ISBUTTONPRESSED";
        case WM_USER+12: return "TB_ISBUTTONHIDDEN";
        case WM_USER+13: return "TB_ISBUTTONINDETERMINATE";
        case WM_USER+17: return "TB_SETSTATE";
        case WM_USER+18: return "TB_GETSTATE";
        case WM_USER+19: return "TB_ADDBITMAP";
        case WM_USER+20: return "TB_ADDBUTTONS";
        case WM_USER+21: return "TB_INSERTBUTTON";
        case WM_USER+22: return "TB_DELETEBUTTON";
        case WM_USER+23: return "TB_GETBUTTON";
        case WM_USER+24: return "TB_BUTTONCOUNT";
        case WM_USER+25: return "TB_COMMANDTOINDEX";
        case WM_USER+26: return "TB_SAVERESTOREA";
        case WM_USER+76: return "TB_SAVERESTOREW";
        case WM_USER+27: return "TB_CUSTOMIZE";
        case WM_USER+28: return "TB_ADDSTRINGA";
        case WM_USER+77: return "TB_ADDSTRINGW";
        case WM_USER+29: return "TB_GETITEMRECT";
        case WM_USER+30: return "TB_BUTTONSTRUCTSIZE";
        case WM_USER+31: return "TB_SETBUTTONSIZE";
        case WM_USER+32: return "TB_SETBITMAPSIZE";
        case WM_USER+33: return "TB_AUTOSIZE";
        case WM_USER+35: return "TB_GETTOOLTIPS";
        case WM_USER+36: return "TB_SETTOOLTIPS";
        case WM_USER+37: return "TB_SETPARENT";
        case WM_USER+39: return "TB_SETROWS";
        case WM_USER+40: return "TB_GETROWS";
        case WM_USER+42: return "TB_SETCMDID";
        case WM_USER+43: return "TB_CHANGEBITMAP";
        case WM_USER+44: return "TB_GETBITMAP";
        case WM_USER+45: return "TB_GETBUTTONTEXTA";
        case WM_USER+75: return "TB_GETBUTTONTEXTW";
        case WM_USER+46: return "TB_REPLACEBITMAP";
        case WM_USER+47: return "TB_SETINDENT";
        case WM_USER+48: return "TB_SETIMAGELIST";
        case WM_USER+49: return "TB_GETIMAGELIST";
        case WM_USER+50: return "TB_LOADIMAGES";
        case WM_USER+51: return "TB_GETRECT";
        case WM_USER+52: return "TB_SETHOTIMAGELIST";
        case WM_USER+53: return "TB_GETHOTIMAGELIST";
        case WM_USER+54: return "TB_SETDISABLEDIMAGELIST";
        case WM_USER+55: return "TB_GETDISABLEDIMAGELIST";
        case WM_USER+56: return "TB_SETSTYLE";
        case WM_USER+57: return "TB_GETSTYLE";
        case WM_USER+58: return "TB_GETBUTTONSIZE";
        case WM_USER+59: return "TB_SETBUTTONWIDTH";
        case WM_USER+60: return "TB_SETMAXTEXTROWS";
        case WM_USER+61: return "TB_GETTEXTROWS";
        case WM_USER+41: return "TB_GETBITMAPFLAGS";

#endif //WIN32

        default:
            static char s_szBuf[128];
            sprintf(s_szBuf, "<unknown message = %d>", message);
            return s_szBuf;
    }
*/
   return NULL;
}

#endif // __WXDEBUG__

