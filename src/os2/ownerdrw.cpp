///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ownerdrw.cpp
// Purpose:     implementation of wxOwnerDrawn class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/window.h"
  #include "wx/msw/private.h"
  #include "wx/font.h"
  #include "wx/bitmap.h"
  #include "wx/dcmemory.h"
  #include "wx/menu.h"
  #include "wx/utils.h"
#endif

#if wxUSE_OWNER_DRAWN

#include "wx/ownerdrw.h"
#include "wx/menuitem.h"


// ============================================================================
// implementation of wxOwnerDrawn class
// ============================================================================

//
// ctor
// ----
//
wxOwnerDrawn::wxOwnerDrawn(
  const wxString&                   rsStr
, bool                              bCheckable
, bool                              bMenuItem
)
: m_strName(rsStr)
{
    m_bCheckable   = bCheckable;
    m_bOwnerDrawn  = FALSE;
    m_nHeight      = 0;
    m_nMarginWidth = ms_nLastMarginWidth;
    if (wxNORMAL_FONT)
        m_font = *wxNORMAL_FONT;
} // end of wxOwnerDrawn::wxOwnerDrawn

size_t wxOwnerDrawn::ms_nDefaultMarginWidth = 15;

size_t wxOwnerDrawn::ms_nLastMarginWidth = ms_nDefaultMarginWidth;

//
// Drawing
// -------
//

bool wxOwnerDrawn::OnMeasureItem(
  size_t*                           pWidth
, size_t*                           pHeight
)
{
    wxMemoryDC                      vDC;

    vDC.SetFont(GetFont());

    wxString                        sStr = wxStripMenuCodes(m_strName);

    vDC.GetTextExtent( sStr
                      ,(long *)pWidth
                      ,(long *)pHeight
                     );

    (*pHeight) = (*pHeight) + 2;
    m_nHeight = *pHeight;        // remember height for use in OnDrawItem
    return TRUE;
} // end of wxOwnerDrawn::OnMeasureItem

// draw the item
bool wxOwnerDrawn::OnDrawItem(
  wxDC&                             rDC
, const wxRect&                     rRect
, wxODAction                        eAction
, wxODStatus                        eStatus
)
{
    //
    // We do nothing on focus change
    //
    if (eAction == wxODFocusChanged )
        return TRUE;

    //
    // Select the font and draw the text
    // ---------------------------------
    //

    CHARBUNDLE                      vCbnd;
    HPS                             hPS= rDC.GetHPS();
    wxColour                        vColBack;
    wxColour                        vColText;
    COLORREF                        vRef;
    RECTL                           vRect = {rRect.x + 4, rRect.y + 1, rRect.x + (rRect.width - 2), rRect.y + rRect.height};

    //
    // Use default font if no font set
    //
    if (m_font.Ok())
    {
        m_font.RealizeResource();
    }
    else
    {
        ::GpiSetCharSet(hPS, LCID_DEFAULT);
    }

    //
    // Base on the status of the menu item pick the right colors
    //
    if (eStatus & wxODSelected)
    {
        wxColour                        vCol2("WHITE");
        vColBack.Set( (unsigned char)0
                     ,(unsigned char)0
                     ,(unsigned char)160
                    ); // no dark blue in color table
        vColText = vCol2;
    }
    else if (eStatus & wxODDisabled)
    {
        vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                         ,SYSCLR_MENU // Light gray
                                         ,0L
                                        );
        vColBack.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
        vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                         ,SYSCLR_MENUDISABLEDTEXT // dark gray
                                         ,0L
                                        );
        vColText.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
    }
    else
    {
        //
        // Fall back to default colors if none explicitly specified
        //
        vRef = ::WinQuerySysColor( HWND_DESKTOP
                                  ,SYSCLR_MENU  // we are using gray for all our window backgrounds in wxWindows
                                  ,0L
                                 );
        vColBack.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
        vRef = ::WinQuerySysColor( HWND_DESKTOP
                                  ,SYSCLR_WINDOWTEXT // Black
                                  ,0L
                                 );
        vColText.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
    }
    rDC.SetTextBackground(vColBack);
    rDC.SetTextForeground(vColText);
    rDC.SetBackgroundMode(wxTRANSPARENT);

    //
    // Paint the background
    //
    ::WinFillRect(hPS, &vRect, vColBack.GetPixel());

    //
    // Determine where to draw and leave space for a check-mark.
    //
    int                             nX = rRect.x + GetMarginWidth();

    //
    // Unfortunately, unlike Win32, PM has no owner drawn specific text
    // drawing methods like ::DrawState that can cleanly handle accel
    // pneumonics and deal, automatically, with various states, so we have
    // to handle them ourselves. Notice Win32 can't handle \t in ownerdrawn
    // strings either.  We cannot handle mneumonics either.  We display
    // it, though, in hopes we can figure it out some day.
    //

    //
    // Display main text and accel text separately to allign better
    //
    wxString                        sTgt = "\t";
    wxString                        sFullString = m_strName; // need to save the original text
    wxString                        sAccel;
    size_t                          nIndex;
    size_t                          nWidth;
    size_t                          nCharWidth;
    size_t                          nHeight;
    bool                            bFoundMneumonic = FALSE;
    bool                            bFoundAccel = FALSE;

    //
    // Deal with the tab, extracting the Accel text
    //
    nIndex = sFullString.Find(sTgt.c_str());
    if (nIndex != -1)
    {
        bFoundAccel = TRUE;
        sAccel = sFullString.Mid(nIndex + 1);
        sFullString.Remove(nIndex);
    }

    //
    // Deal with the mneumonic character
    //
    sTgt = "~";
    nIndex = sFullString.Find(sTgt.c_str());
    if (nIndex != -1)
    {
        wxString                    sTmp = sFullString;

        bFoundMneumonic = TRUE;
        sTmp.Remove(nIndex);
        rDC.GetTextExtent( sTmp
                          ,(long *)&nWidth
                          ,(long *)&nHeight
                         );
        sTmp = sFullString[nIndex + 1];
        rDC.GetTextExtent( sTmp
                          ,(long *)&nCharWidth
                          ,(long *)&nHeight
                         );
        sFullString.Replace(sTgt.c_str(), "", TRUE);
    }

    //
    // Draw the main item text sans the accel text
    rDC.DrawText( sFullString
                 ,nX
                 ,rRect.y + 4
                );
    if (bFoundMneumonic)
    {
        //
        // Underline the mneumonic -- still won't work, but at least it "looks" right
        //
        wxPen                       vPen;
        POINTL                      vPntStart = {nX + nWidth - 1, rRect.y + 2}; // Make it look pretty!
        POINTL                      vPntEnd = {nX + nWidth + nCharWidth - 3, rRect.y + 2}; //CharWidth is bit wide

        vPen = wxPen(vColText, 1, wxSOLID); // Assuming we are always black
        rDC.SetPen(vPen);
        ::GpiMove(hPS, &vPntStart);
        ::GpiLine(hPS, &vPntEnd);
    }

    //
    // Now draw the accel text
    //
    if (bFoundAccel)
    {
        size_t                      nWidth;
        size_t                      nHeight;

        rDC.GetTextExtent( sAccel
                          ,(long *)&nWidth
                          ,(long *)&nHeight
                         );
        //
        // Back off the starting position from the right edge
        //
        rDC.DrawText( sAccel
                     ,rRect.width - (nWidth + 7) // this seems to mimic the default OS/2 positioning
                     ,rRect.y + 4
                    );
    }

    //
    // Draw the bitmap
    // ---------------
    //
    if (IsCheckable() && !m_bmpChecked.Ok())
    {
        if (eStatus & wxODChecked)
        {
            RECTL                   vRect;
            HBITMAP                 hBmpCheck = ::WinGetSysBitmap(HWND_DESKTOP, SBMP_MENUCHECK);

            vRect.xLeft   = rRect.x;
            vRect.xRight  = rRect.x + GetMarginWidth();
            vRect.yBottom = rRect.y;
            vRect.yTop    = rRect.y + m_nHeight - 3;

            ::WinDrawBitmap( hPS             // PS for this menuitem
                            ,hBmpCheck       // system checkmark
                            ,NULL            // draw the whole bitmap
                            ,(PPOINTL)&vRect // destination -- bottom left corner of the menuitem area
                            ,0L              // ignored
                            ,0L              // draw a bitmap
                            ,DBM_NORMAL      // draw normal size
                           );
        }
    }
    else
    {
        //
        // For uncheckable item we use only the 'checked' bitmap
        //
        wxBitmap                    vBmp(GetBitmap(IsCheckable() ? ((eStatus & wxODChecked) != 0) : TRUE));

        if (vBmp.Ok())
        {
            wxMemoryDC              vDCMem(&rDC);

            vDCMem.SelectObject(vBmp);

            //
            // Center bitmap
            //
            int                     nBmpWidth = vBmp.GetWidth();
            int                     nBmpHeight = vBmp.GetHeight();

            //
            // There should be enough space!
            //
            wxASSERT((nBmpWidth <= rRect.width) && (nBmpHeight <= rRect.height));

            //
            //MT: blit with mask enabled.
            //
            rDC.Blit( rRect.x + (GetMarginWidth() - nBmpWidth) / 2
                     ,rRect.y + (m_nHeight - nBmpHeight) /2
                     ,nBmpWidth
                     ,nBmpHeight
                     ,&vDCMem
                     ,0
                     ,0
                     ,wxCOPY
                     ,TRUE
                    );

            if (eStatus & wxODSelected)
            {
                RECT                vRectBmp = { rRect.x
                                                ,rRect.y
                                                ,rRect.x + GetMarginWidth()
                                                ,rRect.y + m_nHeight
                                               };
                POINTL              vPnt1 = {2, 4}; // Leave a little background border
                POINTL              vPnt2 = {rRect.x + GetMarginWidth(), rRect.y + m_nHeight - 3};
                LINEBUNDLE          vLine;

                vLine.lColor = vColBack.GetPixel();
                ::GpiSetAttrs( hPS
                              ,PRIM_LINE
                              ,LBB_COLOR
                              ,0
                              ,&vLine
                             );
                ::GpiMove(hPS, &vPnt1);
                ::GpiBox( hPS
                         ,DRO_OUTLINE
                         ,&vPnt2
                         ,0L
                         ,0L
                        );
            }
        }
    }
    return TRUE;
} // end of wxOwnerDrawn::OnDrawItem

#endif //wxUSE_OWNER_DRAWN
