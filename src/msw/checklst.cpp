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
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "checklst.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_OWNER_DRAWN

#include "wx/object.h"
#include "wx/colour.h"
#include "wx/font.h"
#include "wx/bitmap.h"
#include "wx/window.h"
#include "wx/listbox.h"
#include "wx/ownerdrw.h"
#include "wx/settings.h"
#include "wx/dcmemory.h"
#include "wx/msw/checklst.h"
#include "wx/log.h"

#include <windows.h>
#include <windowsx.h>

#ifdef __GNUWIN32__
#include "wx/msw/gnuwin32/extra.h"
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// get item (converted to right type)
#define GetItem(n)    ((wxCheckListBoxItem *)(GetItem(n)))

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
friend class wxCheckListBox;
public:
  // ctor
  wxCheckListBoxItem(wxCheckListBox *pParent, size_t nIndex);

  // drawing functions
  virtual bool OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat);

  // simple accessors
  bool IsChecked() const  { return m_bChecked;        }
  void Check(bool bCheck);
  void Toggle() { Check(!IsChecked()); }

private:
  bool            m_bChecked;
  wxCheckListBox *m_pParent;
  size_t            m_nIndex;
};

wxCheckListBoxItem::wxCheckListBoxItem(wxCheckListBox *pParent, size_t nIndex)
                  : wxOwnerDrawn("", TRUE)   // checkable
{
  m_bChecked = FALSE;
  m_pParent  = pParent;
  m_nIndex   = nIndex;

  // we don't initialize m_nCheckHeight/Width vars because it's
  // done in OnMeasure while they are used only in OnDraw and we
  // know that there will always be OnMeasure before OnDraw

  // fix appearance
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
    size_t nCheckWidth  = GetDefaultMarginWidth(),
         nCheckHeight = m_pParent->GetItemHeight();

    int x = rc.GetX(),
        y = rc.GetY();

    HDC hdc = (HDC)dc.GetHDC();

    // create pens
    HPEN hpenBack = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOW)),
         hpenGray = CreatePen(PS_SOLID, 0, RGB(128, 128, 128)),
         hpenPrev = (HPEN)SelectObject(hdc, hpenBack);

    // we erase the 1-pixel border
    Rectangle(hdc, x, y, x + nCheckWidth, y + nCheckHeight);

    // shift check mark 1 pixel to the right (it looks better like this)
    x++;

    if ( IsChecked() ) {
      // first create a monochrome bitmap in a memory DC
      HDC hdcMem = CreateCompatibleDC(hdc);
      HBITMAP hbmpCheck = CreateBitmap(nCheckWidth, nCheckHeight, 1, 1, 0);
      HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpCheck);

      // then draw a check mark into it
      RECT rect = { 0, 0, nCheckWidth, nCheckHeight };

#ifdef __WIN32__
#ifndef __SC__
      DrawFrameControl(hdcMem, &rect, DFC_MENU, DFCS_MENUCHECK);
#endif
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
    HBRUSH hbrPrev  = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
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
void wxCheckListBoxItem::Check(bool check)
{
    m_bChecked = check;

    // index may be chanegd because new items were added/deleted
    if ( m_pParent->GetItemIndex(this) != (int)m_nIndex )
    {
        // update it
        int index = m_pParent->GetItemIndex(this);

        wxASSERT_MSG( index != wxNOT_FOUND, "what does this item do here?" );

        m_nIndex = (size_t)index;
    }

    HWND hwndListbox = (HWND)m_pParent->GetHWND();

    #ifdef __WIN32__
        RECT rcUpdate;

        if ( ::SendMessage(hwndListbox, LB_GETITEMRECT,
                           m_nIndex, (LPARAM)&rcUpdate) == LB_ERR )
        {
            wxLogDebug("LB_GETITEMRECT failed");
        }
    #else // Win16
        // FIXME this doesn't work if the listbox is scrolled!
        size_t nHeight = m_pParent->GetItemHeight();
        size_t y = m_nIndex * nHeight;
        RECT rcUpdate = { 0, y, GetDefaultMarginWidth(), y + nHeight};
    #endif  // Win32/16

    InvalidateRect(hwndListbox, &rcUpdate, FALSE);

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
                               const wxString& name)
              : wxListBox()
{
    Create(parent, id, pos, size, nStrings, choices,
           style | wxLB_OWNERDRAW, val, name);
}

void wxCheckListBox::Delete(int N)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::Delete" );

    wxListBox::Delete(N);

    // free memory
    delete m_aItems[N];

    m_aItems.Remove(N);
}

void wxCheckListBox::InsertItems(int nItems, const wxString items[], int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_noItems,
                 "invalid index in wxCheckListBox::InsertItems" );

    wxListBox::InsertItems(nItems, items, pos);

    int i;
    for ( i = 0; i < nItems; i++ ) {
        wxOwnerDrawn *pNewItem = CreateItem((size_t)(pos + i));
        pNewItem->SetName(items[i]);
        m_aItems.Insert(pNewItem, (size_t)(pos + i));
        ListBox_SetItemData((HWND)GetHWND(), i + pos, pNewItem);
    }
}


bool wxCheckListBox::SetFont( const wxFont &font )
{
    size_t i;
    for (i=0; i < m_aItems.GetCount(); i++)
        m_aItems[i]->SetFont(font);
    wxListBox::SetFont(font);
    return TRUE;
}

// create/retrieve item
// --------------------

// create a check list box item
wxOwnerDrawn *wxCheckListBox::CreateItem(size_t nIndex)
{
  wxCheckListBoxItem *pItem = new wxCheckListBoxItem(this, nIndex);
  return pItem;
}

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

bool wxCheckListBox::IsChecked(size_t uiIndex) const
{
  return GetItem(uiIndex)->IsChecked();
}

void wxCheckListBox::Check(size_t uiIndex, bool bCheck)
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
    event.Skip();
}

void wxCheckListBox::OnLeftClick(wxMouseEvent& event)
{
  // clicking on the item selects it, clicking on the checkmark toggles
  if ( event.GetX() <= wxOwnerDrawn::GetDefaultMarginWidth() ) {
    #ifdef __WIN32__
      size_t nItem = (size_t)::SendMessage
                               (
                                (HWND)GetHWND(),
                                LB_ITEMFROMPOINT,
                                0,
                                MAKELPARAM(event.GetX(), event.GetY())
                               );
    #else // Win16
        // FIXME this doesn't work when the listbox is scrolled!
        size_t nItem = ((size_t)event.GetY()) / m_nItemHeight;
    #endif // Win32/16

    if ( nItem < (size_t)m_noItems )
      GetItem(nItem)->Toggle();
    //else: it's not an error, just click outside of client zone
  }
  else {
    // implement default behaviour: clicking on the item selects it
    event.Skip();
  }
}

#endif

