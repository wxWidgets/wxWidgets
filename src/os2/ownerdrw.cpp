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

    wxString                        sTgt = "\t";
    size_t                          nIndex;

    nIndex = sStr.Find(sTgt.c_str());
    if (nIndex != -1)
        sStr.Remove(nIndex);
    sTgt = "~";
    nIndex = sStr.Find(sTgt.c_str());
    if (nIndex != -1)
        sStr.Replace(sTgt.c_str(), "", TRUE);

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
    if (eAction == wxODFocusChanged )
        return TRUE;
    //
    // WxWinGdi_CColour <-> RGB
    //
    #define   ToRGB(col)  OS2RGB(col.Red(), col.Green(), col.Blue())
    #define   UnRGB(col)  GetRValue(col), GetGValue(col), GetBValue(col)

    CHARBUNDLE                      vCbndText;
    CHARBUNDLE                      vCbndBack;
    HPS                             hPS= rDC.GetHPS();
    ULONG                           lColBack;
    ULONG                           lColText;

    if (eStatus & wxODSelected)
    {
        lColBack = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_MENUHILITEBGND // Light gray
                                             ,0L
                                            );
        lColText = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_MENUTEXT // Black
                                             ,0L
                                            );
    }
    else if (eStatus & wxODDisabled)
    {
        lColBack = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_MENU // Light gray
                                             ,0L
                                            );
        lColText = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_MENUDISABLEDTEXT // dark gray
                                             ,0L
                                            );
    }
    else
    {
        //
        // Fall back to default colors if none explicitly specified
        //
        lColBack = m_colBack.Ok() ? ToRGB(m_colBack) : ::WinQuerySysColor( HWND_DESKTOP
                                                                          ,SYSCLR_MENU  // we are using gray for all our window backgrounds in wxWindows
                                                                          ,0L
                                                                         );
        lColText = m_colText.Ok() ? ToRGB(m_colText) : ::WinQuerySysColor( HWND_DESKTOP
                                                                          ,SYSCLR_WINDOWTEXT // Black
                                                                          ,0L
                                                                         );
    }
    vCbndText.lColor = (LONG)lColText;
    vCbndBack.lColor = (LONG)lColBack;

    ::GpiSetAttrs( hPS
                  ,PRIM_CHAR
                  ,CBB_BACK_COLOR
                  ,0
                  ,&vCbndBack
                 );
    ::GpiSetAttrs( hPS
                  ,PRIM_CHAR
                  ,CBB_COLOR
                  ,0
                  ,&vCbndText
                 );


    //
    // Determine where to draw and leave space for a check-mark.
    //
    int                             nX = rRect.x + GetMarginWidth();

    //
    // Select the font and draw the text
    // ---------------------------------
    //

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
    // Unfortunately, unlike Win32, PM has no owner drawn specific text
    // drawing methods like ::DrawState that can cleanly handle accel
    // pneumonics and deal, automatically, with various states, so we have
    // to handle them ourselves. Notice Win32 can't handle \t in ownerdrawn
    // strings either.

    //
    // Manually replace the tab with spaces
    //
    wxString                        sTgt = "\t";
    wxString                        sReplace = " ";
    size_t                          nIndex;

    nIndex = m_strName.Find(sTgt.c_str());
    if (nIndex != -1)
        m_strName.Replace(sTgt.c_str(), sReplace.c_str(), TRUE);
    sTgt = "~";
    nIndex = m_strName.Find(sTgt.c_str());
    if (nIndex != -1)
        m_strName.Replace(sTgt.c_str(), "", TRUE);

    rDC.DrawText( m_strName
                 ,nX
                 ,rRect.y + 4
                );

#if 0
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
            vRect.yTop    = rRect.y + m_nHeight;

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
                LINEBUNDLE          vLine;

                vLine.lColor = lColBack;
                ::GpiSetAttrs( hPS
                              ,PRIM_LINE
                              ,LBB_COLOR
                              ,0
                              ,&vLine
                             );
                ::GpiBox( hPS
                         ,DRO_OUTLINE
                         ,(PPOINTL)&vRectBmp
                         ,0L
                         ,0L
                        );
            }
        }
    }
#endif
    return TRUE;
} // end of wxOwnerDrawn::OnDrawItem

#endif //wxUSE_OWNER_DRAWN
