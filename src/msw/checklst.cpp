///////////////////////////////////////////////////////////////////////////////
// Name:        msw/checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     16.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#ifdef __GNUG__
#pragma implementation "checklst.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if USE_OWNER_DRAWN

#include <windows.h>

#include "wx/ownerdrw.h"
#include "wx/msw/checklst.h"

// ============================================================================
// implementation
// ============================================================================

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)
#endif

// ----------------------------------------------------------------------------
// declaration and implementation of wxCheckListBoxItem class
// ----------------------------------------------------------------------------

class wxCheckListBoxItem : public wxOwnerDrawn
{
public:
  // ctor
  wxCheckListBoxItem(wxCheckListBox *pParent, uint nIndex);

  // drawing functions
  virtual bool OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat);

  // simple accessors
  bool IsChecked() const  { return m_bChecked;        }
  void Check(bool bCheck) { m_bChecked = bCheck;      }
  void Toggle();

private:
  bool            m_bChecked;
  wxCheckListBox *m_pParent;
  uint            m_nIndex;
};

wxCheckListBoxItem::wxCheckListBoxItem(wxCheckListBox *pParent, uint nIndex)
                  : wxOwnerDrawn("", TRUE)   // checkable
{
  m_bChecked = FALSE;
  m_pParent  = pParent;
  m_nIndex   = nIndex;

  // we don't initialize m_nCheckHeight/Width vars because it's
  // done in OnMeasure while they are used only in OnDraw and we
  // know that there will always be OnMeasure before OnDraw

  // fix appearance
  SetFont(wxSystemSettings::GetSystemFont(wxSYS_ANSI_VAR_FONT));
  SetMarginWidth(GetDefaultMarginWidth());
}

/*
 * JACS - I've got the owner-draw stuff partially working with WIN16,
 * with a really horrible-looking cross for wxCheckListBox instead of a
 * check - could use a bitmap check-mark instead, defined in wx.rc.
 * Also there's a refresh problem whereby it doesn't always draw the
 * check until you click to the right of it, which is OK for WIN32.
 */

bool wxCheckListBoxItem::OnDrawItem(wxDC& dc, const wxRect& rc, 
                                    wxODAction act, wxODStatus stat)
{
  if ( IsChecked() )
    stat = (wxOwnerDrawn::wxODStatus)(stat | wxOwnerDrawn::wxODChecked);

  if ( wxOwnerDrawn::OnDrawItem(dc, rc, act, stat) ) {
    // ## using native API for performance and precision
    uint nCheckWidth  = GetDefaultMarginWidth(),
         nCheckHeight = m_pParent->GetItemHeight();
          
    int x = rc.GetX(), 
        y = rc.GetY();

    HDC hdc = (HDC)dc.GetHDC();

    // create pens
    HPEN hpenBack = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW)),
         hpenGray = CreatePen(PS_SOLID, 0, RGB(128, 128, 128)),
         hpenPrev = SelectObject(hdc, hpenBack);

    // we erase the 1-pixel border
    Rectangle(hdc, x, y, x + nCheckWidth, y + nCheckHeight);

    // shift check mark 1 pixel to the right (it looks better like this)
    x++;

    if ( IsChecked() ) {
      // first create a monochrome bitmap in a memory DC
      HDC hdcMem = CreateCompatibleDC(hdc);
      HBITMAP hbmpCheck = CreateBitmap(nCheckWidth, nCheckHeight, 1, 1, 0);
      HBITMAP hbmpOld = SelectObject(hdcMem, hbmpCheck);

      // then draw a check mark into it
      RECT rect = { 0, 0, nCheckWidth, nCheckHeight };

#ifdef __WIN32__
      DrawFrameControl(hdcMem, &rect, DFC_MENU, DFCS_MENUCHECK);
#else
      // In WIN16, draw a cross
      HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
      HPEN whiteBrush = GetStockObject(WHITE_BRUSH);
      HPEN hPenOld = ::SelectObject(hdcMem, blackPen);
      HPEN hBrushOld = ::SelectObject(hdcMem, whiteBrush);
      ::SetROP2(hdcMem, R2_COPYPEN);
      Rectangle(hdcMem, 0, 0, nCheckWidth, nCheckHeight);
      MoveTo(hdcMem, 0, 0);
      LineTo(hdcMem, nCheckWidth, nCheckHeight);
      MoveTo(hdcMem, nCheckWidth, 0);
      LineTo(hdcMem, 0, nCheckHeight);
      ::SelectObject(hdcMem, hPenOld);
      ::SelectObject(hdcMem, hBrushOld);
      ::DeleteObject(blackPen);
#endif

      // finally copy it to screen DC and clean up
      BitBlt(hdc, x, y, nCheckWidth - 1, nCheckHeight, 
             hdcMem, 0, 0, SRCCOPY);

      SelectObject(hdcMem, hbmpOld);
      DeleteObject(hbmpCheck);
      DeleteDC(hdcMem);
    }

    // now we draw the smaller rectangle
    y++;
    nCheckWidth  -= 2;
    nCheckHeight -= 2;

    // draw hollow gray rectangle
    (void)SelectObject(hdc, hpenGray);
    HBRUSH hbrPrev  = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x, y, x + nCheckWidth, y + nCheckHeight);

    // clean up
    (void)SelectObject(hdc, hpenPrev);
    (void)SelectObject(hdc, hbrPrev);

    DeleteObject(hpenBack);
    DeleteObject(hpenGray);

    /*
    dc.DrawRectangle(x, y, nCheckWidth, nCheckHeight);

    if ( IsChecked() ) {
      dc.DrawLine(x, y, x + nCheckWidth, y + nCheckHeight);
      dc.DrawLine(x, y + nCheckHeight, x + nCheckWidth, y);
    }
    */

    return TRUE;
  }

  return FALSE;
}

// change the state of the item and redraw it
void wxCheckListBoxItem::Toggle()
{ 
  m_bChecked = !m_bChecked;

  uint nHeight = m_pParent->GetItemHeight();
  uint y = m_nIndex * nHeight;
  RECT rcUpdate = { 0, y, GetDefaultMarginWidth(), y + nHeight};
  InvalidateRect((HWND)m_pParent->GetHWND(), &rcUpdate, FALSE);

  wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, m_pParent->GetId());
  event.SetInt(m_nIndex);
  event.SetEventObject(m_pParent);
  m_pParent->ProcessCommand(event);
}

// ----------------------------------------------------------------------------
// implementation of wxCheckListBox class
// ----------------------------------------------------------------------------

// define event table
// ------------------
BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
  EVT_CHAR(wxCheckListBox::OnChar)
  EVT_LEFT_DOWN(wxCheckListBox::OnLeftClick)
END_EVENT_TABLE()

// control creation
// ----------------

// def ctor: use Create() to really create the control
wxCheckListBox::wxCheckListBox() : wxListBox()
{
}

// ctor which creates the associated control
wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               int nStrings, const wxString choices[],
                               long style, const wxValidator& val,
                               const wxString& name) // , const wxFont& font)
                // don't use ctor with arguments! we must call Create()
                // ourselves from here.
              : wxListBox()
//                , m_font(font)
{
  Create(parent, id, pos, size, nStrings, choices, style|wxLB_OWNERDRAW, val, name);
}

// create/retrieve item
// --------------------

// create a check list box item
wxOwnerDrawn *wxCheckListBox::CreateItem(uint nIndex)
{
  wxCheckListBoxItem *pItem = new wxCheckListBoxItem(this, nIndex);
  if ( m_windowFont.Ok() )
    pItem->SetFont(m_windowFont);

  return pItem;
}

// get item (converted to right type)
#define GetItem(n)    ((wxCheckListBoxItem *)(GetItem(n)))

// return item size
// ----------------
bool wxCheckListBox::MSWOnMeasure(WXMEASUREITEMSTRUCT *item)
{
  if ( wxListBox::MSWOnMeasure(item) ) {
    MEASUREITEMSTRUCT *pStruct = (MEASUREITEMSTRUCT *)item;

    // save item height
    m_nItemHeight = pStruct->itemHeight;

    // add place for the check mark
    pStruct->itemWidth += wxOwnerDrawn::GetDefaultMarginWidth();

    return TRUE;
  }

  return FALSE;
}

// check items
// -----------

bool wxCheckListBox::IsChecked(uint uiIndex) const
{
  return GetItem(uiIndex)->IsChecked();
}

void wxCheckListBox::Check(uint uiIndex, bool bCheck)
{
  GetItem(uiIndex)->Check(bCheck);
}

// process events
// --------------

void wxCheckListBox::OnChar(wxKeyEvent& event)
{
  if ( event.KeyCode() == WXK_SPACE )
    GetItem(GetSelection())->Toggle();
  else
    wxListBox::OnChar(event);
}

void wxCheckListBox::OnLeftClick(wxMouseEvent& event)
{
  // clicking on the item selects it, clicking on the checkmark toggles
  if ( event.GetX() <= wxOwnerDrawn::GetDefaultMarginWidth() ) {
    // # better use LB_ITEMFROMPOINT perhaps?
    uint nItem = ((uint)event.GetY()) / m_nItemHeight;
    if ( nItem < (uint)m_noItems )
      GetItem(nItem)->Toggle();
    //else: it's not an error, just click outside of client zone
  }
  else {
    // implement default behaviour: clicking on the item selects it
    event.Skip();
  }
}

#endif

