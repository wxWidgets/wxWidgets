///////////////////////////////////////////////////////////////////////////////
// Name:        src/os2/ownerdrw.cpp
// Purpose:     implementation of wxOwnerDrawn class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_OWNER_DRAWN

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/os2/private.h"
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/menuitem.h"
#endif

#include "wx/ownerdrw.h"

// ============================================================================
// implementation of wxOwnerDrawn class
// ============================================================================

//
// ctor
// ----
//
wxOwnerDrawn::wxOwnerDrawn( const wxString& rsStr,
                            bool            bCheckable,
                            bool            WXUNUSED(bMenuItem) )
: m_strName(rsStr)
{
    m_bCheckable   = bCheckable;
    m_bOwnerDrawn  = false;
    m_nHeight      = 0;
    m_nMarginWidth = ms_nLastMarginWidth;
    if (wxNORMAL_FONT)
        m_font = *wxNORMAL_FONT;
} // end of wxOwnerDrawn::wxOwnerDrawn

wxOwnerDrawn::~wxOwnerDrawn() { }

size_t wxOwnerDrawn::ms_nDefaultMarginWidth = 15;

size_t wxOwnerDrawn::ms_nLastMarginWidth = ms_nDefaultMarginWidth;

//
// Drawing
// -------
//

bool wxOwnerDrawn::OnMeasureItem( size_t* pWidth,
                                  size_t* pHeight )
{
    wxMemoryDC vDC;

    wxString  sStr = wxStripMenuCodes(m_strName);

    //
    // If we have a valid accel string, then pad out
    // the menu string so that the menu and accel string are not
    // placed on top of each other.
    if (!m_strAccel.empty() )
    {
        sStr.Pad(sStr.length()%8);
        sStr += m_strAccel;
    }
    vDC.SetFont(GetFont());
    vDC.GetTextExtent( sStr
                      ,(long *)pWidth
                      ,(long *)pHeight
                     );
    if (!m_strAccel.empty())
    {
        //
        // Measure the accelerator string, and add its width to
        // the total item width, plus 16 (Accelerators are right justified,
        // with the right edge of the text rectangle 16 pixels left of
        // the right edge of the menu)
        //
        int                         nAccelWidth;
        int                         nAccelHeight;

        vDC.GetTextExtent( m_strAccel
                          ,&nAccelWidth
                          ,&nAccelHeight
                         );
        *pWidth += nAccelWidth;
    }

    //
    // Add space at the end of the menu for the submenu expansion arrow.
    // This will also allow offsetting the accel string from the right edge
    //
    *pWidth = (size_t)(*pWidth + GetDefaultMarginWidth() * 1.5);

    //
    // JACS: items still look too tightly packed, so adding 5 pixels.
    //
    (*pHeight) += 5;

    //
    // Ray Gilbert's changes - Corrects the problem of a BMP
    // being placed next to text in a menu item, and the BMP does
    // not match the size expected by the system.  This will
    // resize the space so the BMP will fit.  Without this, BMPs
    // must be no larger or smaller than 16x16.
    //
    if (m_bmpChecked.Ok())
    {
        //
        // Is BMP height larger then text height?
        //
        size_t                      nAdjustedHeight = m_bmpChecked.GetHeight() +
                                                      wxSystemSettings::GetMetric(wxSYS_EDGE_Y);
        if (*pHeight < nAdjustedHeight)
            *pHeight = nAdjustedHeight;

        //
        // Does BMP encroach on default check menu position?
        //
        size_t                      nAdjustedWidth = m_bmpChecked.GetWidth() +
                                                     (wxSystemSettings::GetMetric(wxSYS_EDGE_X) * 2);

        //
        // Do we need to widen margin to fit BMP?
        //
        if ((size_t)GetMarginWidth() < nAdjustedWidth)
            SetMarginWidth(nAdjustedWidth);

        //
        // Add the size of the bitmap to our total size...
        //
        *pWidth += GetMarginWidth();
    }

    //
    // Add the size of the bitmap to our total size - even if we don't have
    // a bitmap we leave room for one...
    //
    *pWidth += GetMarginWidth();

    //
    // Make sure that this item is at least as
    // tall as the user's system settings specify
    //
    if (*pHeight < m_nMinHeight)
        *pHeight = m_nMinHeight;
    m_nHeight = *pHeight;                // remember height for use in OnDrawItem
    return true;
} // end of wxOwnerDrawn::OnMeasureItem

// draw the item
bool wxOwnerDrawn::OnDrawItem( wxDC& rDC,
                               const wxRect& rRect,
                               wxODAction eAction,
                               wxODStatus eStatus )
{
    //
    // We do nothing on focus change
    //
    if (eAction == wxODFocusChanged )
        return true;

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

    memset(&vCbnd, 0, sizeof(CHARBUNDLE));

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
    // Based on the status of the menu item, pick the right colors
    //
    if (eStatus & wxODSelected)
    {
        vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                         ,SYSCLR_MENUHILITEBGND
                                         ,0L
                                        );
        vColBack.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
        vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                         ,SYSCLR_MENUHILITE
                                         ,0L
                                        );
        vColText.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
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
                                  ,SYSCLR_MENU  // we are using gray for all our window backgrounds in wxWidgets
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
    vCbnd.lColor     = vColText.GetPixel();
    vCbnd.lBackColor = vColBack.GetPixel();
    ::GpiSetAttrs( hPS
                  ,PRIM_CHAR
                  ,CBB_BACK_COLOR | CBB_COLOR
                  ,0
                  ,&vCbnd
                 );
    ::GpiSetBackMix( hPS
                    ,BM_LEAVEALONE
                   );

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
    // mnemonics and deal, automatically, with various states, so we have
    // to handle them ourselves. Notice Win32 can't handle \t in ownerdrawn
    // strings either.  We cannot handle mnemonics either.  We display
    // them, though, in the hope we can figure them out some day.
    //

    //
    // Display main text and accel text separately to align better
    //
    wxString sTgt = wxT("\t");
    wxString sFullString = m_strName; // need to save the original text
    wxString sAccel;
    int      nIndex;
    size_t   nWidth;
    size_t   nCharWidth;
    size_t   nHeight;
    bool     bFoundMnemonic = false;
    bool     bFoundAccel = false;

    //
    // Deal with the tab, extracting the Accel text
    //
    nIndex = sFullString.Find(sTgt.c_str());
    if (nIndex != -1)
    {
        bFoundAccel = true;
        sAccel = sFullString.Mid(nIndex + 1);
        sFullString.Remove(nIndex);
    }

    //
    // Deal with the mnemonic character
    //
    sTgt = wxT("~");
    nIndex = sFullString.Find(sTgt.c_str());
    if (nIndex != -1)
    {
        wxString sTmp = sFullString;

        bFoundMnemonic = true;
        sTmp.Remove(nIndex);
        rDC.GetTextExtent( sTmp
                          ,(long *)&nWidth
                          ,(long *)&nHeight
                         );
        sTmp = sFullString[(size_t)(nIndex + 1)];
        rDC.GetTextExtent( sTmp
                          ,(long *)&nCharWidth
                          ,(long *)&nHeight
                         );
        sFullString.Replace(sTgt.c_str(), wxEmptyString, true);
    }

    //
    // Draw the main item text sans the accel text
    //
    POINTL                      vPntStart = {nX, rRect.y + 4};
    ::GpiCharStringAt( rDC.GetHPS()
                      ,&vPntStart
                      ,sFullString.length()
                      ,(PCH)sFullString.c_str()
                     );
    if (bFoundMnemonic)
    {
        //
        // Underline the mnemonic -- still won't work, but at least it "looks" right
        //
        wxPen                       vPen;
        POINTL                      vPntEnd = {nX + nWidth + nCharWidth - 3, rRect.y + 2}; //CharWidth is bit wide

        vPntStart.x = nX + nWidth - 1;
        vPntStart.y = rRect.y + 2; // Make it look pretty!
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
        vPntStart.x = rRect.width - (nWidth + 7);
        vPntStart.y = rRect.y + 4;
        ::GpiCharStringAt( rDC.GetHPS()
                          ,&vPntStart
                          ,sAccel.length()
                          ,(PCH)sAccel.c_str()
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
        wxBitmap vBmp(GetBitmap(IsCheckable() ? ((eStatus & wxODChecked) != 0) : TRUE));

        if (vBmp.Ok())
        {

            wxMemoryDC              vDCMem(&rDC);
            wxMemoryDC*             pOldDC = (wxMemoryDC*)vBmp.GetSelectedInto();

            if(pOldDC != NULL)
            {
                vBmp.SetSelectedInto(NULL);
            }
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

            int                     nHeightDiff = m_nHeight - nBmpHeight;

            rDC.Blit( rRect.x + (GetMarginWidth() - nBmpWidth) / 2
                     ,rRect.y + nHeightDiff / 2
                     ,nBmpWidth
                     ,nBmpHeight
                     ,&vDCMem
                     ,0
                     ,0
                     ,wxCOPY
                     ,true
                    );

            if (eStatus & wxODSelected)
            {
                POINTL              vPnt1 = {rRect.x + 1, rRect.y + 3}; // Leave a little background border
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
            vBmp.SetSelectedInto(NULL);
        }
    }
    return true;
} // end of wxOwnerDrawn::OnDrawItem

#endif //wxUSE_OWNER_DRAWN
