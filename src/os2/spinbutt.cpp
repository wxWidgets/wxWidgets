/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "spinbutt.h"
    #pragma implementation "spinbutbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#if wxUSE_SPINBTN

// Can't resolve reference to CreateUpDownControl in
// TWIN32, but could probably use normal CreateWindow instead.


#include "wx/spinbutt.h"

extern void  wxAssociateWinWithHandle( HWND         hWnd
                                      ,wxWindowOS2* pWin
                                     );
static WXFARPROC fnWndProcSpinCtrl = (WXFARPROC)NULL;

IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent)

#include "wx/os2/private.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)

bool wxSpinButton::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
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
    SWP                             vSwp;

    m_min = 0;
    m_max = 100;
    if (vId == -1)
        m_windowId = NewControlId();
    else
        m_windowId = vId;
    m_backgroundColour = pParent->GetBackgroundColour();
    m_foregroundColour = pParent->GetForegroundColour();
    SetName(rsName);
    SetParent(pParent);
    m_windowStyle      = lStyle;

    //
    // Get the right size for the control
    //
    if (nWidth <= 0 || nHeight <= 0 )
    {
        wxSize                      vSize = DoGetBestSize();

        if (nWidth <= 0 )
            nWidth = vSize.x;
        if (nHeight <= 0 )
            nHeight = vSize.y;
    }
    if (nX < 0 )
        nX = 0;
    if (nY < 0 )
        nY = 0;

    long                            lSstyle = 0L;

    lSstyle = WS_VISIBLE      |
              WS_TABSTOP      |
              SPBS_MASTER     | // We use only single field spin buttons
              SPBS_NUMERICONLY; // We default to numeric data

    if (m_windowStyle & wxCLIP_SIBLINGS )
        lSstyle |= WS_CLIPSIBLINGS;

    m_hWnd = (WXHWND)::WinCreateWindow( GetWinHwnd(pParent)
                                       ,WC_SPINBUTTON
                                       ,(PSZ)NULL
                                       ,lSstyle
                                       ,0L, 0L, 0L, 0L
                                       ,GetWinHwnd(pParent)
                                       ,HWND_TOP
                                       ,(HMENU)vId
                                       ,NULL
                                       ,NULL
                                      );
    if (m_hWnd == 0)
    {
        return FALSE;
    }
    SetRange(m_min, m_max);
    if(pParent)
        pParent->AddChild((wxSpinButton *)this);

    ::WinQueryWindowPos(m_hWnd, &vSwp);
    SetXComp(vSwp.x);
    SetYComp(vSwp.y);
    wxFont*                          pTextFont = new wxFont( 10
                                                            ,wxMODERN
                                                            ,wxNORMAL
                                                            ,wxNORMAL
                                                           );
    SetFont(*pTextFont);
    //
    // For OS/2 we want to hide the text portion so we can substitute an
    // independent text ctrl in its place.  10 device units does this
    //
    SetSize( nX
            ,nY
            ,10L
            ,nHeight
           );
    wxAssociateWinWithHandle( m_hWnd
                             ,(wxWindowOS2*)this
                            );
#if 0
    // FIXME:
    // Apparently, this does not work, as it crashes in setvalue/setrange calls
    // What's it supposed to do anyway?
    ::WinSetWindowULong(GetHwnd(), QWL_USER, (LONG)this);
    fnWndProcSpinCtrl = (WXFARPROC)::WinSubclassWindow(m_hWnd, (PFNWP)wxSpinCtrlWndProc);
#endif
    delete pTextFont;
    return TRUE;
} // end of wxSpinButton::Create

wxSpinButton::~wxSpinButton()
{
} // end of wxSpinButton::~wxSpinButton

// ----------------------------------------------------------------------------
// size calculation
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestSize() const
{
    //
    // OS/2 PM does not really have system metrics so we'll just set  it to
    // 24x20 which is the size of the buttons and the borders.
    // Also we have no horizontal spin buttons.
    //
    return (wxSize(24,20));
} // end of wxSpinButton::DoGetBestSize

// ----------------------------------------------------------------------------
// Attributes
// ----------------------------------------------------------------------------

int wxSpinButton::GetValue() const
{
    long                            lVal = 0L;
    char                            zVal[10];

    ::WinSendMsg( GetHwnd()
                 ,SPBM_QUERYVALUE
                 ,MPFROMP(zVal)
                 ,MPFROM2SHORT( (USHORT)10
                               ,SPBQ_UPDATEIFVALID
                              )
                );
    lVal = atol(zVal);
    return ((int)lVal);
} // end of wxSpinButton::GetValue

bool wxSpinButton::OS2OnScroll(
  int                               nOrientation
, WXWORD                            wParam
, WXWORD                            wPos
, WXHWND                            hControl
)
{
    wxCHECK_MSG(hControl, FALSE, wxT("scrolling what?") )

    wxSpinEvent                     vEvent( wxEVT_SCROLL_THUMBTRACK
                                           ,m_windowId
                                          );
    int                             nVal = (int)wPos;    // cast is important for negative values!

    vEvent.SetPosition(nVal);
    vEvent.SetEventObject(this);
    return(GetEventHandler()->ProcessEvent(vEvent));
} // end of wxSpinButton::OS2OnScroll

bool wxSpinButton::OS2Command(
  WXUINT                            uCmd
, WXWORD                            wId
)
{
    return FALSE;
} // end of wxSpinButton::OS2Command

void wxSpinButton::SetRange(
  int                               nMinVal
, int                               nMaxVal
)
{
    m_min = nMinVal;
    m_max = nMaxVal;

    ::WinSendMsg( GetHwnd()
                 ,SPBM_SETLIMITS
                 ,MPFROMLONG(nMaxVal)
                 ,MPFROMLONG(nMinVal)
                );
} // end of wxSpinButton::SetRange

void wxSpinButton::SetValue(
  int                               nValue
)
{
    ::WinSendMsg(GetHwnd(), SPBM_SETCURRENTVALUE, MPFROMLONG(nValue), MPARAM(0));
} // end of wxSpinButton::SetValue

#endif //wxUSE_SPINBTN
