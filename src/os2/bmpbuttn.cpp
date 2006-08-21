/////////////////////////////////////////////////////////////////////////////
// Name:        src/os2/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

#ifndef WX_PRECOMP
#endif

#include "wx/os2/private.h"


IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#define BUTTON_HEIGHT_FACTOR (EDIT_CONTROL_FACTOR * 1.1)

bool wxBitmapButton::Create( wxWindow*          pParent,
                             wxWindowID         vId,
                             const wxBitmap&    rBitmap,
                             const wxPoint&     rPos,
                             const wxSize&      rSize,
                             long               lStyle,
                             const wxValidator& rValidator,
                             const wxString&    rsName )
{
    m_bmpNormal = rBitmap;
    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif

    pParent->AddChild(this);

    m_backgroundColour = pParent->GetBackgroundColour() ;
    m_foregroundColour = pParent->GetForegroundColour() ;
    m_windowStyle = lStyle;

    if (lStyle & wxBU_AUTODRAW)
    {
        m_marginX = wxDEFAULT_BUTTON_MARGIN;
        m_marginY = wxDEFAULT_BUTTON_MARGIN;
    }

    int nX      = rPos.x;
    int nY      = rPos.y;
    int nWidth  = rSize.x;
    int nHeight = rSize.y;

    if (vId == wxID_ANY)
        m_windowId = NewControlId();
    else
        m_windowId = vId;

    if (nWidth == wxDefaultCoord && rBitmap.Ok())
        nWidth = rBitmap.GetWidth() + 4 * m_marginX;

    if (nHeight == wxDefaultCoord && rBitmap.Ok())
        nHeight = rBitmap.GetHeight() + 4 * m_marginY;

    ULONG                           ulOS2Style = WS_VISIBLE | WS_TABSTOP | BS_USERBUTTON;

    if (m_windowStyle & wxCLIP_SIBLINGS)
        ulOS2Style |= WS_CLIPSIBLINGS;

    m_hWnd = (WXHWND)::WinCreateWindow( GetHwndOf(pParent)
                                       ,WC_BUTTON
                                       ,(PSZ)wxEmptyString
                                       ,ulOS2Style
                                       ,0, 0, 0, 0
                                       ,GetHwndOf(pParent)
                                       ,HWND_TOP
                                       ,m_windowId
                                       ,NULL
                                       ,NULL
                                      );

    //
    //Subclass again for purposes of dialog editing mode
    //
    SubclassWin(m_hWnd);
    SetFont(*wxSMALL_FONT);
    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );
    return true;
} // end of wxBitmapButton::Create

bool wxBitmapButton::OS2OnDraw( WXDRAWITEMSTRUCT* pItem)
{
    PUSERBUTTON  pUser     = (PUSERBUTTON)pItem;
    bool         bAutoDraw = (GetWindowStyleFlag() & wxBU_AUTODRAW) != 0;

    if (!pUser)
        return false;

    wxBitmap*  pBitmap;
    bool       bIsSelected = pUser->fsState & BDS_HILITED;
    wxClientDC vDc(this);

    if (bIsSelected && m_bmpSelected.Ok())
        pBitmap = &m_bmpSelected;
    else if ((pUser->fsState & BDS_DEFAULT) && m_bmpFocus.Ok())
        pBitmap = &m_bmpFocus;
    else if ((pUser->fsState & BDS_DISABLED) && m_bmpDisabled.Ok())
        pBitmap = &m_bmpDisabled;
    else
        pBitmap = &m_bmpNormal;

    if (!pBitmap->Ok() )
        return false;


    //
    // Centre the bitmap in the control area
    //
    int                             nX1        = 0;
    int                             nY1        = 0;
    int                             nWidth     = vDc.m_vRclPaint.xRight - vDc.m_vRclPaint.xLeft;
    int                             nHeight    = vDc.m_vRclPaint.yTop - vDc.m_vRclPaint.yBottom;
    int                             nBmpWidth  = pBitmap->GetWidth();
    int                             nBmpHeight = pBitmap->GetHeight();

    nX1 = (nWidth - nBmpWidth) / 2;
    nY1 = (nHeight - nBmpHeight) / 2;

    if (bIsSelected && bAutoDraw)
    {
        nX1++;
        nY1++;
    }

    //
    // Draw the button face
    //
    DrawFace( vDc, bIsSelected );

    //
    // Draw the bitmap
    //
    vDc.DrawBitmap( *pBitmap, nX1, nY1, true );

    //
    // Draw focus / disabled state, if auto-drawing
    //
    if ((pUser->fsState == BDS_DISABLED) && bAutoDraw)
    {
        DrawButtonDisable( vDc, *pBitmap );
    }
    else if ((pUser->fsState == BDS_DEFAULT) && bAutoDraw)
    {
        DrawButtonFocus(vDc);
    }
    return true;
} // end of wxBitmapButton::OS2OnDraw

void wxBitmapButton::DrawFace (wxClientDC& rDC, bool bSel)
{
    //
    // Set up drawing colors
    //
    wxPen vHiLitePen(*wxWHITE, 2, wxSOLID); // White
    wxColour gray85(85, 85, 85);
    wxPen vDarkShadowPen(gray85, 2, wxSOLID);
    wxColour vFaceColor(204, 204, 204); // Light Grey

    //
    // Draw the main button face
    //
    // This triggers a redraw and destroys the bottom & left focus border and
    // doesn't seem to do anything useful.
    //    ::WinFillRect(rDC.GetHPS(), &rDC.m_vRclPaint, vFaceColor.GetPixel());

    //
    // Draw the border
    // Note: DrawLine expects wxWidgets coordinate system so swap
    //
    rDC.SetPen(bSel ? vDarkShadowPen : vHiLitePen);
    // top
    rDC.DrawLine( rDC.m_vRclPaint.xLeft + 1
                 ,rDC.m_vRclPaint.yBottom + 1
                 ,rDC.m_vRclPaint.xRight - 1
                 ,rDC.m_vRclPaint.yBottom + 1
                );
    // left
    rDC.DrawLine( rDC.m_vRclPaint.xLeft + 1
                 ,rDC.m_vRclPaint.yBottom + 1
                 ,rDC.m_vRclPaint.xLeft + 1
                 ,rDC.m_vRclPaint.yTop - 1
                );

    rDC.SetPen(bSel ? vHiLitePen : vDarkShadowPen);
    // bottom
    rDC.DrawLine( rDC.m_vRclPaint.xLeft + 1
                 ,rDC.m_vRclPaint.yTop - 1
                 ,rDC.m_vRclPaint.xRight - 1
                 ,rDC.m_vRclPaint.yTop - 1
                );
    // right
    rDC.DrawLine( rDC.m_vRclPaint.xRight - 1
                 ,rDC.m_vRclPaint.yBottom + 1
                 ,rDC.m_vRclPaint.xRight - 1
                 ,rDC.m_vRclPaint.yTop - 1
                );

} // end of wxBitmapButton::DrawFace

void wxBitmapButton::DrawButtonFocus (
  wxClientDC&                       rDC
)
{
    wxPen vBlackPen(*wxBLACK, 2, wxSOLID);

    //
    // Draw a thick black line around the outside of the button
    // Note: DrawLine expects wxWidgets coordinate system so swap
    //
    rDC.SetPen(vBlackPen);
    // top
    rDC.DrawLine( rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yBottom
                 ,rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yBottom
                );
    // right
    rDC.DrawLine( rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yBottom
                 ,rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yTop
                );
    // bottom
    rDC.DrawLine( rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yTop
                 ,rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yTop
                );
    // left
    rDC.DrawLine( rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yTop
                 ,rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yBottom
                );
} // end of wxBitmapButton::DrawButtonFocus

void wxBitmapButton::DrawButtonDisable( wxClientDC& rDC,
                                        wxBitmap& rBmp )
{
    wxPen vGreyPen(wxT("GREY"), 2, wxSOLID);

    //
    // Draw a thick black line around the outside of the button
    // Note: DrawLine expects wxWidgets coordinate system so swap
    //
    rDC.SetPen(vGreyPen);
    // top
    rDC.DrawLine( rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yBottom
                 ,rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yBottom
                );
    // right
    rDC.DrawLine( rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yBottom
                 ,rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yTop
                );
    // bottom
    rDC.DrawLine( rDC.m_vRclPaint.xRight
                 ,rDC.m_vRclPaint.yTop
                 ,rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yTop
                );
    // left
    rDC.DrawLine( rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yTop
                 ,rDC.m_vRclPaint.xLeft
                 ,rDC.m_vRclPaint.yBottom
                );
    wxDisableBitmap(rBmp, vGreyPen.GetColour().GetPixel());
} // end of wxBitmapButton::DrawButtonDisable

void wxBitmapButton::SetDefault()
{
    wxButton::SetDefault();
}

#endif // ndef for wxUSE_BMPBUTTON
