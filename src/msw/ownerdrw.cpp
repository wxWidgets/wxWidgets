///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ownerdrw.cpp
// Purpose:     implementation of wxOwnerDrawn class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/msw/private.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/window.h"
  #include "wx/msw/private.h"
  #include "wx/font.h"
  #include "wx/bitmap.h"
  #include "wx/dcmemory.h"
  #include "wx/menu.h"
  #include "wx/utils.h"
#endif

#include "wx/settings.h"
#include "wx/ownerdrw.h"
#include "wx/menuitem.h"
#include "wx/fontutil.h"
#include "wx/module.h"

#if wxUSE_OWNER_DRAWN

class wxMSWSystemMenuFontModule : public wxModule
{
public:

    virtual bool OnInit()
    {
        ms_systemMenuFont = new wxFont;

#if defined(__WXMSW__) && defined(__WIN32__) && defined(SM_CXMENUCHECK)
        NONCLIENTMETRICS nm;
        nm.cbSize = sizeof(NONCLIENTMETRICS);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS,0,&nm,0);

        ms_systemMenuButtonWidth = nm.iMenuHeight;
        ms_systemMenuHeight = nm.iMenuHeight;

        // create menu font
        wxNativeFontInfo info;
        memcpy(&info.lf, &nm.lfMenuFont, sizeof(LOGFONT));
        ms_systemMenuFont->Create(info);
#endif

        return true;
    }

    virtual void OnExit()
    {
        delete ms_systemMenuFont;
        ms_systemMenuFont = NULL;
    }

    static wxFont* ms_systemMenuFont;
    static int ms_systemMenuButtonWidth;   // windows clean install default
    static int ms_systemMenuHeight;        // windows clean install default
private:
    DECLARE_DYNAMIC_CLASS(wxMSWSystemMenuFontModule)
};

// these static variables are by the wxMSWSystemMenuFontModule object
// and reflect the system settings returned by the Win32 API's
// SystemParametersInfo() call.

wxFont* wxMSWSystemMenuFontModule::ms_systemMenuFont = NULL;
int wxMSWSystemMenuFontModule::ms_systemMenuButtonWidth = 18;   // windows clean install default
int wxMSWSystemMenuFontModule::ms_systemMenuHeight = 18;        // windows clean install default

IMPLEMENT_DYNAMIC_CLASS(wxMSWSystemMenuFontModule, wxModule)

// ============================================================================
// implementation of wxOwnerDrawn class
// ============================================================================

// ctor
// ----
wxOwnerDrawn::wxOwnerDrawn(const wxString& str,
                           bool bCheckable, bool bMenuItem)
            : m_strName(str)
{
    // get the default menu height and font from the system
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof (NONCLIENTMETRICS);
    SystemParametersInfo (SPI_GETNONCLIENTMETRICS,0,&nm,0);
    m_nMinHeight = nm.iMenuHeight;

    // nm.iMenuWidth is the system default for the width of
    // menu icons and checkmarks
    if (ms_nDefaultMarginWidth == 0)
    {
       ms_nDefaultMarginWidth = ::GetSystemMetrics(SM_CXMENUCHECK) + wxSystemSettings::GetMetric(wxSYS_EDGE_X);
       ms_nLastMarginWidth = ms_nDefaultMarginWidth;
    }

    if (wxMSWSystemMenuFontModule::ms_systemMenuFont->Ok() && bMenuItem)
    {
        m_font = *wxMSWSystemMenuFontModule::ms_systemMenuFont;
    }
    else
    {
        m_font = *wxNORMAL_FONT;
    }

    m_bCheckable   = bCheckable;
    m_bOwnerDrawn  = false;
    m_nHeight      = 0;
    m_nMarginWidth = ms_nLastMarginWidth;
    m_nMinHeight   = wxMSWSystemMenuFontModule::ms_systemMenuHeight;

    m_bmpDisabled = wxNullBitmap;
}


// these items will be set during the first invocation of the c'tor,
// because the values will be determined by checking the system settings,
// which is a chunk of code
size_t wxOwnerDrawn::ms_nDefaultMarginWidth = 0;
size_t wxOwnerDrawn::ms_nLastMarginWidth = 0;


// drawing
// -------

// get size of the item
// The item size includes the menu string, the accel string,
// the bitmap and size for a submenu expansion arrow...
bool wxOwnerDrawn::OnMeasureItem(size_t *pwidth, size_t *pheight)
{
  wxMemoryDC dc;

  wxString str = wxStripMenuCodes(m_strName);

  // if we have a valid accel string, then pad out
  // the menu string so the menu and accel string are not
  // placed ontop of eachother.
  if ( !m_strAccel.empty() )
   {
       str.Pad(str.Length()%8);
       str += m_strAccel;
   }

  if (m_font.Ok())
      dc.SetFont(GetFont());

  dc.GetTextExtent(str, (long *)pwidth, (long *)pheight);

  // add space at the end of the menu for the submenu expansion arrow
  // this will also allow offsetting the accel string from the right edge
  *pwidth += GetDefaultMarginWidth() + 16;

  // increase size to accommodate bigger bitmaps if necessary
  if (m_bmpChecked.Ok())
  {
      // Is BMP height larger then text height?
      size_t adjustedHeight = m_bmpChecked.GetHeight() +
                                2*wxSystemSettings::GetMetric(wxSYS_EDGE_Y);
      if (*pheight < adjustedHeight)
          *pheight = adjustedHeight;

      // Does BMP encroach on default check menu position?
      size_t adjustedWidth = m_bmpChecked.GetWidth();

      // Do we need to widen margin to fit BMP?
      if ((size_t)GetMarginWidth() < adjustedWidth)
          SetMarginWidth(adjustedWidth);
  }

  // make sure that this item is at least as
  // tall as the user's system settings specify
  if (*pheight < m_nMinHeight)
    *pheight = m_nMinHeight;

  // remember height for use in OnDrawItem
  m_nHeight = *pheight;

  return true;
}

// draw the item
bool wxOwnerDrawn::OnDrawItem(wxDC& dc,
                              const wxRect& rc,
                              wxODAction act,
                              wxODStatus st)
{
  // we do nothing on focus change
  if ( act == wxODFocusChanged )
    return true;


  // this flag determines whether or not an edge will
  // be drawn around the bitmap. In most "windows classic"
  // applications, a 1-pixel highlight edge is drawn around
  // the bitmap of an item when it is selected.  However,
  // with the new "luna" theme, no edge is drawn around
  // the bitmap because the background is white (this applies
  // only to "non-XP style" menus w/ bitmaps --
  // see IE 6 menus for an example)

  bool draw_bitmap_edge = true;

  // set the colors
  // --------------
  DWORD colBack, colText;
  if ( st & wxODSelected ) {
    colBack = GetSysColor(COLOR_HIGHLIGHT);
    if (!(st & wxODDisabled))
    {
        colText = GetSysColor(COLOR_HIGHLIGHTTEXT);
    }
     else
    {
        colText = GetSysColor(COLOR_GRAYTEXT);
    }
  }
  else {
    // fall back to default colors if none explicitly specified
    colBack = m_colBack.Ok() ? wxColourToPalRGB(m_colBack)
                             : GetSysColor(COLOR_WINDOW);
    colText = m_colText.Ok() ? wxColourToPalRGB(m_colText)
                             : GetSysColor(COLOR_WINDOWTEXT);
  }


  // don't draw an edge around the bitmap, if background is white ...
  DWORD menu_bg_color = GetSysColor(COLOR_MENU);
  if (    ( GetRValue( menu_bg_color ) >= 0xf0 &&
            GetGValue( menu_bg_color ) >= 0xf0 &&
            GetBValue( menu_bg_color ) >= 0xf0 )
       // ... or if the menu item is disabled
       || ( st & wxODDisabled )
    )
  {
      draw_bitmap_edge = false;
  }


  HDC hdc = GetHdcOf(dc);
  COLORREF colOldText = ::SetTextColor(hdc, colText),
           colOldBack = ::SetBkColor(hdc, colBack);

  int margin = GetMarginWidth() + wxSystemSettings::GetMetric(wxSYS_EDGE_X);

  // select the font and draw the text
  // ---------------------------------


  // determine where to draw and leave space for a check-mark.
  int xText = rc.x + margin;


  // using native API because it reckognizes '&'
  int nPrevMode = SetBkMode(hdc, TRANSPARENT);
  HBRUSH hbr = CreateSolidBrush(colBack),
         hPrevBrush = (HBRUSH)SelectObject(hdc, hbr);

  RECT rectFill = { rc.GetLeft(), rc.GetTop(),
                      rc.GetRight() + 1, rc.GetBottom() + 1 };

  if ( (st & wxODSelected) && m_bmpChecked.Ok() ) {
      // only draw the highlight under the text, not under
      // the bitmap or checkmark
      rectFill.left = xText;
  }

  FillRect(hdc, &rectFill, hbr);

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

  wxString strMenuText = m_strName.BeforeFirst('\t');

  SIZE sizeRect;
  GetTextExtentPoint32(hdc,strMenuText.c_str(), strMenuText.Length(),&sizeRect);
  ::DrawState(hdc, NULL, NULL,
              (LPARAM)strMenuText.c_str(), strMenuText.length(),
              xText, rc.y+( (int) ((rc.GetHeight()-sizeRect.cy)/2.0) )-1, // centre text vertically
              rc.GetWidth()-margin, sizeRect.cy,
              DST_PREFIXTEXT |
              (((st & wxODDisabled) && !(st & wxODSelected)) ? DSS_DISABLED : 0));

  // ::SetTextAlign(hdc, TA_RIGHT) doesn't work with DSS_DISABLED or DSS_MONO
  // as last parameter in DrawState() (at least with Windows98). So we have
  // to take care of right alignment ourselves.
  if ( !m_strAccel.empty() )
  {
      int accel_width, accel_height;
      dc.GetTextExtent(m_strAccel, &accel_width, &accel_height);
      // right align accel string with right edge of menu ( offset by the
      // margin width )
      ::DrawState(hdc, NULL, NULL,
              (LPARAM)m_strAccel.c_str(), m_strAccel.length(),
              rc.GetWidth()-margin-accel_width, rc.y+(int) ((rc.GetHeight()-sizeRect.cy)/2.0),
              rc.GetWidth()-margin-accel_width, sizeRect.cy,
              DST_TEXT |
              (((st & wxODDisabled) && !(st & wxODSelected)) ? DSS_DISABLED : 0));
  }

  (void)SelectObject(hdc, hPrevBrush);
  (void)SelectObject(hdc, hPrevFont);
  (void)SetBkMode(hdc, nPrevMode);

  DeleteObject(hbr);

  // draw the bitmap
  // ---------------
  if ( IsCheckable() && !m_bmpChecked.Ok() ) {
    if ( st & wxODChecked ) {
      // what goes on: DrawFrameControl creates a b/w mask,
      // then we copy it to screen to have right colors

        // first create a monochrome bitmap in a memory DC
      HDC hdcMem = CreateCompatibleDC(hdc);
      HBITMAP hbmpCheck = CreateBitmap(margin, m_nHeight, 1, 1, 0);
      SelectObject(hdcMem, hbmpCheck);

        // then draw a check mark into it
      RECT rect = { 0, 0, margin, m_nHeight };
      if ( m_nHeight > 0 )
      {
        ::DrawFrameControl(hdcMem, &rect, DFC_MENU, DFCS_MENUCHECK);
      }

        // finally copy it to screen DC and clean up
      BitBlt(hdc, rc.x, rc.y, margin, m_nHeight,
             hdcMem, 0, 0, SRCCOPY);

      DeleteDC(hdcMem);
      DeleteObject(hbmpCheck);
    }
  }
  else {
    wxBitmap bmp;

    if ( st & wxODDisabled )
    {
        bmp = GetDisabledBitmap();
    }

    if ( !bmp.Ok() )
    {
        // for not checkable bitmaps we should always use unchecked one because
        // their checked bitmap is not set
        bmp = GetBitmap(!IsCheckable() || (st & wxODChecked));
    }

    if ( bmp.Ok() ) {
      wxMemoryDC dcMem(&dc);
      dcMem.SelectObject(bmp);

      // center bitmap
      int nBmpWidth = bmp.GetWidth(),
          nBmpHeight = bmp.GetHeight();

      // there should be enough place!
      wxASSERT((nBmpWidth <= rc.GetWidth()) && (nBmpHeight <= rc.GetHeight()));

      int heightDiff = m_nHeight - nBmpHeight;
      dc.Blit(rc.x + (margin - nBmpWidth) / 2,
              rc.y + heightDiff / 2,
              nBmpWidth, nBmpHeight,
              &dcMem, 0, 0, wxCOPY, true /* use mask */);

      if ( st & wxODSelected && draw_bitmap_edge ) {
          RECT rectBmp = { rc.GetLeft(), rc.GetTop(),
                           rc.GetLeft() + margin,
                           rc.GetTop() + m_nHeight };
          SetBkColor(hdc, colBack);

          DrawEdge(hdc, &rectBmp, BDR_RAISEDOUTER, BF_SOFT | BF_RECT);
      }
    }
  }

  ::SetTextColor(hdc, colOldText);
  ::SetBkColor(hdc, colOldBack);

  return true;
}


#endif // wxUSE_OWNER_DRAWN

