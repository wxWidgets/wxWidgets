/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.cpp
// Purpose:     wxGauge class
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/utils.h"
#endif

#include "wx/os2/private.h"
#include "wx/gauge.h"

static WXFARPROC fnWndProcGauge = (WXFARPROC)NULL;
extern void  wxAssociateWinWithHandle( HWND         hWnd
                                      ,wxWindowOS2* pWin
                                     );

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)

MRESULT EXPENTRY wxGaugeWndProc(
  HWND                              hWnd
, UINT                              uMessage
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    wxGauge*                        pGauge = (wxGauge *)::WinQueryWindowULong( hWnd
                                                                              ,QWL_USER
                                                                             );
    wxWindowOS2*                    pWindow = NULL;
    MRESULT                         rc = (MRESULT)0;
    bool                            bProcessed = FALSE;
    HPS                             hPS;
    RECTL                           vRect;
    RECTL                           vRect2;
    double                          dPixelToRange = 0.0;
    double                          dRange = 0.0;

    switch (uMessage )
    {
        case WM_PAINT:
            hPS = ::WinBeginPaint( hWnd
                                  ,NULLHANDLE
                                  ,&vRect
                                 );
            if(hPS)
            {
                ::WinQueryWindowRect(hWnd, &vRect);
                ::GpiCreateLogColorTable( hPS
                                         ,0L
                                         ,LCOLF_CONSECRGB
                                         ,0L
                                         ,(LONG)wxTheColourDatabase->m_nSize
                                         ,(PLONG)wxTheColourDatabase->m_palTable
                                        );
                ::GpiCreateLogColorTable( hPS
                                         ,0L
                                         ,LCOLF_RGB
                                         ,0L
                                         ,0L
                                         ,NULL
                                        );
                if (pGauge->GetWindowStyleFlag() & wxGA_VERTICAL)
                {
                    dRange = (double)(vRect.yTop - vRect.yBottom);
                    dPixelToRange  = dRange/(double)pGauge->GetRange();
                    vRect2.yTop    = (int)(pGauge->GetValue() * dPixelToRange);
                    vRect2.yBottom = vRect.yBottom;
                    vRect2.xLeft   = vRect.xLeft;
                    vRect2.xRight  = vRect.xRight;
                    vRect.yBottom  = vRect2.yTop;
                    ::WinFillRect(hPS, &vRect,  pGauge->GetBackgroundColour().GetPixel());
                    ::WinFillRect(hPS, &vRect2, pGauge->GetForegroundColour().GetPixel());
                }
                else
                {
                    dRange = (double)(vRect.xRight - vRect.xLeft);
                    dPixelToRange  = dRange/(double)pGauge->GetRange();
                    vRect2.yTop    = vRect.yTop;
                    vRect2.yBottom = vRect.yBottom;
                    vRect2.xLeft   = vRect.xLeft;
                    vRect2.xRight  = (int)(pGauge->GetValue() * dPixelToRange);
                    vRect.xLeft = vRect2.xRight;
                    ::WinFillRect(hPS, &vRect,  pGauge->GetBackgroundColour().GetPixel());
                    ::WinFillRect(hPS, &vRect2, pGauge->GetForegroundColour().GetPixel());
                }
                ::WinEndPaint(hPS);
            }
    }
    return (fnWndProcGauge( hWnd
                           ,(ULONG)uMessage
                           ,(MPARAM)wParam
                           ,(MPARAM)lParam
                          )
           );
} // end of wxGaugeWndProc

bool wxGauge::Create(
  wxWindowOS2*                      pParent
, wxWindowID                        vId
, int                               nRange
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
#if wxUSE_VALIDATORS
, const wxValidator&                rValidator
#endif
, const wxString&                   rsName
)
{
    int                             nX       = rPos.x;
    int                             nY       = rPos.y;
    int                             nWidth   = rSize.x;
    int                             nHeight  = rSize.y;
    long                            lMsStyle = 0L;
    SWP                             vSwp;

    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif
    if (pParent)
        pParent->AddChild(this);
    SetBackgroundColour(pParent->GetBackgroundColour()) ;
    SetForegroundColour(pParent->GetForegroundColour()) ;

    m_nRangeMax   = nRange;
    m_nGaugePos   = 0;
    m_windowStyle = lStyle;

    if (vId == -1)
        m_windowId = (int)NewControlId();
    else
        m_windowId = vId;

    if (m_windowStyle & wxCLIP_SIBLINGS )
        lMsStyle |= WS_CLIPSIBLINGS;

    //
    // OS/2 will use an edit control for this, since there is not a native gauge
    // Other choices include using an armless slider but they are more difficult
    // to control and manipulate
    //

    lMsStyle = WS_VISIBLE | ES_MARGIN | ES_LEFT | ES_READONLY;
    if (m_windowStyle & wxCLIP_SIBLINGS)
        lMsStyle |= WS_CLIPSIBLINGS;



    m_hWnd = (WXHWND)::WinCreateWindow( (HWND)GetHwndOf(pParent) // Parent window handle
                                       ,WC_ENTRYFIELD            // Window class
                                       ,(PSZ)NULL                // Initial Text
                                       ,(ULONG)lMsStyle          // Style flags
                                       ,0L, 0L, 0L, 0L           // Origin -- 0 size
                                       ,(HWND)GetHwndOf(pParent) // owner window handle (same as parent
                                       ,HWND_TOP                 // initial z position
                                       ,(HMENU)m_windowId        // Window identifier
                                       ,NULL                     // Slider control data
                                       ,NULL                     // no Presentation parameters
                                      );

    wxAssociateWinWithHandle( m_hWnd
                             ,(wxWindowOS2*)this
                            );
    ::WinSetWindowULong(GetHwnd(), QWL_USER, (LONG)this);
    fnWndProcGauge = (WXFARPROC)::WinSubclassWindow(m_hWnd, (PFNWP)wxGaugeWndProc);
    ::WinQueryWindowPos(m_hWnd, &vSwp);
    SetXComp(vSwp.x);
    SetYComp(vSwp.y);
    SetFont(pParent->GetFont());
    if (nWidth == -1L)
        nWidth = 50L;
    if (nHeight == -1L)
        nHeight = 28L;
    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );
    ::WinShowWindow((HWND)GetHWND(), TRUE);
    return TRUE;
} // end of wxGauge::Create

int wxGauge::GetBezelFace() const
{
    return 0;
} // end of wxGauge::GetBezelFace

int wxGauge::GetRange() const
{
    return m_nRangeMax;
} // end of wxGauge::GetRange

int wxGauge::GetShadowWidth() const
{
    return 0;
} // end of wxGauge::GetShadowWidth

int wxGauge::GetValue() const
{
    return m_nGaugePos;
} // end of wxGauge::GetValue

bool wxGauge::SetBackgroundColour(
  const wxColour&                   rColour
)
{
    if (!wxControl::SetBackgroundColour(rColour))
        return FALSE;

    LONG                            lColor = (LONG)rColour.GetPixel();

    ::WinSetPresParam( GetHwnd()
                      ,PP_BACKGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&lColor
                     );
    return TRUE;
} // end of wxGauge::SetBackgroundColour

void wxGauge::SetBezelFace(
  int                               WXUNUSED(nWidth)
)
{
} // end of wxGauge::SetBezelFace

bool wxGauge::SetForegroundColour(
  const wxColour&                   rColour
)
{
    if (!wxControl::SetForegroundColour(rColour))
        return FALSE;

    LONG                            lColor = (LONG)rColour.GetPixel();

    ::WinSetPresParam( GetHwnd()
                      ,PP_FOREGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&lColor
                     );

    return TRUE;
} // end of wxGauge::SetForegroundColour

void wxGauge::SetRange(
  int                               nRange
)
{
    m_nRangeMax = nRange;
} // end of wxGauge::SetRange

void wxGauge::SetShadowWidth(
  int                               WXUNUSED(nWidth)
)
{
} // end of wxGauge::SetShadowWidth

void wxGauge::SetValue(
  int                               nPos
)
{
    RECT                            vRect;

    m_nGaugePos = nPos;
    ::WinQueryWindowRect(GetHwnd(), &vRect);
    ::WinInvalidateRect(GetHwnd(), &vRect, FALSE);
} // end of wxGauge::SetValue


