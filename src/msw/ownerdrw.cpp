///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ownerdrw.cpp
// Purpose:     implementation of wxOwnerDrawn class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/menu.h"
#endif

#include "wx/ownerdrw.h"
#include "wx/menuitem.h"

#include <windows.h>

// ============================================================================
// implementation of wxOwnerDrawn class
// ============================================================================

// ctor
// ----
wxOwnerDrawn::wxOwnerDrawn(const wxTString& str, 
                           bool bCheckable, bool bMenuItem)
            : m_strName(str)
{
  m_bCheckable   = bCheckable;
  m_bOwnerDrawn  = FALSE;
  m_nHeight      = 0;
  m_nMarginWidth = ms_nLastMarginWidth;
}

#if defined(__WXMSW__) && defined(__WIN32__)
  uint wxOwnerDrawn::ms_nDefaultMarginWidth = GetSystemMetrics(SM_CXMENUCHECK);
#else   // # what is the reasonable default?
  uint wxOwnerDrawn::ms_nDefaultMarginWidth = 15;
#endif

uint wxOwnerDrawn::ms_nLastMarginWidth = ms_nDefaultMarginWidth;

// drawing
// -------

// get size of the item
bool wxOwnerDrawn::OnMeasureItem(uint *pwidth, uint *pheight)
{
  wxMemoryDC dc;
  dc.SetFont(GetFont());

  // ## ugly...
  char *szStripped = new char[m_strName.Len()];
  wxStripMenuCodes((char *)m_strName.c_str(), szStripped);
  wxString str = szStripped;
  delete [] szStripped;

  // # without this menu items look too tightly packed (at least under Windows)
  str += 'W'; // 'W' is typically the widest letter

  dc.GetTextExtent(str, (long *)pwidth, (long *)pheight);
  m_nHeight = *pheight;                // remember height for use in OnDrawItem

  return TRUE;
}

// searching for this macro you'll find all the code where I'm using the native
// Win32 GDI functions and not wxWindows ones. Might help to whoever decides to
// port this code to X. (VZ)

#ifdef __WIN32__
#define   O_DRAW_NATIVE_API     // comments below explain why I use it
#endif

// draw the item
bool wxOwnerDrawn::OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus st)
{
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
        
  #ifdef  O_DRAW_NATIVE_API
    #define  hdc           (HDC)dc.GetHDC()
    COLORREF colOldText = ::SetTextColor(hdc, colText),
             colOldBack = ::SetBkColor(hdc, colBack);
  #else
    dc.SetTextForeground(wxColor(UnRGB(colText)));
    dc.SetTextBackground(wxColor(UnRGB(colBack)));
  #endif

  // select the font and draw the text
  // ---------------------------------

  // determine where to draw and leave space for a check-mark. 
  int x = rc.x + GetMarginWidth();

  // using native API because it reckognizes '&' 
  #ifdef  O_DRAW_NATIVE_API
    int nPrevMode = SetBkMode(hdc, TRANSPARENT);
    HBRUSH  hbr = CreateSolidBrush(colBack),
            hPrevBrush = (HBRUSH) SelectObject(hdc, hbr);

    RECT rectAll = { rc.GetLeft(), rc.GetTop(), rc.GetRight(), rc.GetBottom() };
    FillRect(hdc, &rectAll, hbr);

    // use default font if no font set
    HFONT hfont;
    if ( m_font.Ok() ) {
      m_font.RealizeResource();
      hfont = (HFONT)m_font.GetResourceHandle();
    }
    else {
      hfont = (HFONT)::GetStockObject(SYSTEM_FONT);
    }

    HFONT hPrevFont = (HFONT) ::SelectObject(hdc, hfont);
    DrawState(hdc, NULL, NULL, 
              (LPARAM)(const char *)m_strName, m_strName.Length(), 
              x, rc.y, rc.GetWidth(), rc.GetHeight(),
              DST_PREFIXTEXT | ( st & wxODDisabled ? DSS_DISABLED : 0) );

    (void)SelectObject(hdc, hPrevBrush);
    (void)SelectObject(hdc, hPrevFont);
    (void)SetBkMode(hdc, nPrevMode);
  #else
    dc.SetFont(GetFont());
    dc.DrawText(m_strName, x, rc.y);
  #endif  //O_DRAW_NATIVE_API

  // draw the bitmap
  // ---------------
  if ( IsCheckable() && !m_bmpChecked.Ok() ) {
    if ( st & wxODChecked ) {
      // using native APIs for performance and simplicity
#ifdef  O_DRAW_NATIVE_API
      // what goes on: DrawFrameControl creates a b/w mask, 
      // then we copy it to screen to have right colors

        // first create a monochrome bitmap in a memory DC
      HDC hdcMem = CreateCompatibleDC(hdc);
      HBITMAP hbmpCheck = CreateBitmap(GetMarginWidth(), m_nHeight, 1, 1, 0);
      SelectObject(hdcMem, hbmpCheck);

        // then draw a check mark into it
      RECT rect = { 0, 0, GetMarginWidth(), m_nHeight };
      DrawFrameControl(hdcMem, &rect, DFC_MENU, DFCS_MENUCHECK);

        // finally copy it to screen DC and clean up
      BitBlt(hdc, rc.x, rc.y, GetMarginWidth(), m_nHeight, 
             hdcMem, 0, 0, SRCCOPY);
      DeleteDC(hdcMem);
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

      dc.Blit(rc.x + (GetMarginWidth() - nBmpWidth) / 2, 
              rc.y + (m_nHeight - nBmpHeight) /2, 
              nBmpWidth, nBmpHeight, 
              &dcMem, 0, 0, wxCOPY);

      if ( st & wxODSelected ) {
        #ifdef  O_DRAW_NATIVE_API
          RECT rectBmp = { rc.GetLeft(), rc.GetTop(), 
                           rc.GetLeft() + GetMarginWidth(), 
                           rc.GetTop() + m_nHeight };
          SetBkColor(hdc, colBack);
          DrawEdge(hdc, &rectBmp, EDGE_RAISED, BF_SOFT | BF_RECT);
        #else
          // ## to write portable DrawEdge
        #endif  //O_DRAW_NATIVE_API
      }
    }
  }

  #ifdef  O_DRAW_NATIVE_API
    ::SetTextColor(hdc, colOldText);
    ::SetBkColor(hdc, colOldBack);

    #undef  hdc
  #endif  //O_DRAW_NATIVE_API

  return TRUE;
}

