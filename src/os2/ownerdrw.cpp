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

#include "wx/ownerdrw.h"
#include "wx/menuitem.h"


// ============================================================================
// implementation of wxOwnerDrawn class
// ============================================================================

// ctor
// ----
wxOwnerDrawn::wxOwnerDrawn(const wxString& str, 
                           bool bCheckable, bool bMenuItem)
            : m_strName(str)
{
  m_bCheckable   = bCheckable;
  m_bOwnerDrawn  = FALSE;
  m_nHeight      = 0;
  m_nMarginWidth = ms_nLastMarginWidth;
}

  size_t wxOwnerDrawn::ms_nDefaultMarginWidth = 15;

size_t wxOwnerDrawn::ms_nLastMarginWidth = ms_nDefaultMarginWidth;

// drawing
// -------

// get size of the item
bool wxOwnerDrawn::OnMeasureItem(size_t *pwidth, size_t *pheight)
{
  wxMemoryDC dc;
  dc.SetFont(GetFont());

  // ## ugly...
  wxChar *szStripped = new wxChar[m_strName.Len()];
  wxStripMenuCodes((wxChar *)m_strName.c_str(), szStripped);
  wxString str = szStripped;
  delete [] szStripped;

  // # without this menu items look too tightly packed (at least under Windows)
  str += wxT('W'); // 'W' is typically the widest letter

  dc.GetTextExtent(str, (long *)pwidth, (long *)pheight);

  // JACS: items still look too tightly packed, so adding 2 pixels.
  (*pheight) = (*pheight) + 2;

  m_nHeight = *pheight;                // remember height for use in OnDrawItem

  return TRUE;
}

// searching for this macro you'll find all the code where I'm using the native
// Win32 GDI functions and not wxWindows ones. Might help to whoever decides to
// port this code to X. (VZ)

// JACS: TODO. Why does a disabled but highlighted item still
// get drawn embossed? How can we tell DrawState that we don't want the
// embossing?

// draw the item
bool wxOwnerDrawn::OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus st)
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// Might want to check the native drawing apis for here and doo something like MSW does for WIN95
///////////////////////////////////////////////////////////////////////////////////////////////////

  // we do nothing on focus change
  if ( act == wxODFocusChanged )
    return TRUE;

  // wxColor <-> RGB
  #define   ToRGB(col)  RGB(col.Red(), col.Green(), col.Blue())
  #define   UnRGB(col)  GetRValue(col), GetGValue(col), GetBValue(col)

  // set the colors
  // --------------
  DWORD colBack, colText;
  if ( st & wxODSelected ) {
    colBack = GetSysColor(COLOR_HIGHLIGHT);
    colText = GetSysColor(COLOR_HIGHLIGHTTEXT);
  }
  else {
    // fall back to default colors if none explicitly specified
    colBack = m_colBack.Ok() ? ToRGB(m_colBack) : GetSysColor(COLOR_WINDOW);
    colText = m_colText.Ok() ? ToRGB(m_colText) : GetSysColor(COLOR_WINDOWTEXT);
  }
        
    dc.SetTextForeground(wxColor(UnRGB(colText)));
    dc.SetTextBackground(wxColor(UnRGB(colBack)));

  // select the font and draw the text
  // ---------------------------------

  // determine where to draw and leave space for a check-mark. 
  int x = rc.x + GetMarginWidth();

    dc.SetFont(GetFont());
    dc.DrawText(m_strName, x, rc.y);

  // draw the bitmap
  // ---------------
  if ( IsCheckable() && !m_bmpChecked.Ok() ) {
    if ( st & wxODChecked ) {
      // using native APIs for performance and simplicity
// TODO:
/*
      HDC hdcMem = CreateCompatibleDC(hdc);
      HBITMAP hbmpCheck = CreateBitmap(GetMarginWidth(), m_nHeight, 1, 1, 0);
      SelectObject(hdcMem, hbmpCheck);
        // then draw a check mark into it
      RECT rect = { 0, 0, GetMarginWidth(), m_nHeight };

        // finally copy it to screen DC and clean up
      BitBlt(hdc, rc.x, rc.y, GetMarginWidth(), m_nHeight, 
             hdcMem, 0, 0, SRCCOPY);
      DeleteDC(hdcMem);
*/
#else
        // #### to do: perhaps using Marlett font (create equiv. font under X)
//        wxFAIL("not implemented");
#endif  //O_DRAW_NATIVE_API
    }
  }
  else {
    // for uncheckable item we use only the 'checked' bitmap
    wxBitmap bmp(GetBitmap(IsCheckable() ? ((st & wxODChecked) != 0) : TRUE));
    if ( bmp.Ok() ) {
      wxMemoryDC dcMem(&dc);
      dcMem.SelectObject(bmp);

      // center bitmap
      int nBmpWidth = bmp.GetWidth(),
          nBmpHeight = bmp.GetHeight();

      // there should be enough place!
      wxASSERT((nBmpWidth <= rc.GetWidth()) && (nBmpHeight <= rc.GetHeight()));

      //MT: blit with mask enabled.
      dc.Blit(rc.x + (GetMarginWidth() - nBmpWidth) / 2, 
              rc.y + (m_nHeight - nBmpHeight) /2, 
              nBmpWidth, nBmpHeight, 
              &dcMem, 0, 0, wxCOPY,true);

      if ( st & wxODSelected ) {
// TODO:
/*
        #ifdef  O_DRAW_NATIVE_API
          RECT rectBmp = { rc.GetLeft(), rc.GetTop(), 
                           rc.GetLeft() + GetMarginWidth(), 
                           rc.GetTop() + m_nHeight };
          SetBkColor(hdc, colBack);
          DrawEdge(hdc, &rectBmp, EDGE_RAISED, BF_SOFT | BF_RECT);
*/
      }
    }
  }
/*
  #ifdef  O_DRAW_NATIVE_API
    ::SetTextColor(hdc, colOldText);
    ::SetBkColor(hdc, colOldBack);

    #undef  hdc
  #endif  //O_DRAW_NATIVE_API
*/
  return TRUE;
}

