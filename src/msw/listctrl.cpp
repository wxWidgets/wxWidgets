/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:     wxListCtrl
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if defined(__WIN95__)

#include "wx/listctrl.h"
#include "wx/log.h"

#include "wx/msw/private.h"

#ifndef __GNUWIN32__
#include <commctrl.h>
#endif

#ifdef __GNUWIN32__
#include "wx/msw/gnuwin32/extra.h"
#endif

static void wxConvertToMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& tvItem);
static void wxConvertFromMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& tvItem, HWND getFullInfo = 0);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

#endif

wxListCtrl::wxListCtrl(void)
{
  m_imageListNormal = NULL;
  m_imageListSmall = NULL;
  m_imageListState = NULL;
  m_baseStyle = 0;
    m_colCount = 0;
    m_textCtrl = NULL;
}

bool wxListCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator, const wxString& name)
{
  m_imageListNormal = NULL;
  m_imageListSmall = NULL;
  m_imageListState = NULL;
  m_textCtrl = NULL;
  m_colCount = 0;

  SetValidator(validator);
  SetName(name);

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  SetParent(parent);

  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  m_windowId = (id == -1) ? NewControlId() : id;

  DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP;
  
  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if ( want3D || wxStyleHasBorder(m_windowStyle) )
    wstyle |= WS_BORDER;

  wstyle |= LVS_SHAREIMAGELISTS;
  m_baseStyle = wstyle;

  long oldStyle = 0; // Dummy
  wstyle |= ConvertToMSWStyle(oldStyle, m_windowStyle);

  // Create the ListView control.
  m_hWnd = (WXHWND)CreateWindowEx(exStyle,
    WC_LISTVIEW,
    "",
    wstyle,
    x, y, width, height,
    (HWND) parent->GetHWND(),
    (HMENU)m_windowId,
    wxGetInstance(),
    NULL);

  if ( !m_hWnd ) {
     wxLogError("Can't create list control window.");

     return FALSE;
  }

  wxSystemSettings settings;
  SetBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_WINDOW));
  SetForegroundColour(parent->GetForegroundColour());

  if (parent) parent->AddChild(this);
  
  SubclassWin((WXHWND) m_hWnd);

  return TRUE;
}

wxListCtrl::~wxListCtrl(void)
{
    if (m_textCtrl)
    {
      m_textCtrl->UnsubclassWin();
      m_textCtrl->SetHWND(0);
      delete m_textCtrl;
      m_textCtrl = NULL;
    }
}

// Add or remove a single window style
void wxListCtrl::SetSingleStyle(long style, bool add)
{
  long flag = GetWindowStyleFlag();

  // Get rid of conflicting styles
  if ( add )
  {
    if ( style & wxLC_MASK_TYPE)
      flag = flag & ~wxLC_MASK_TYPE ;
    if ( style & wxLC_MASK_ALIGN )
      flag = flag & ~wxLC_MASK_ALIGN ;
    if ( style & wxLC_MASK_SORT )
      flag = flag & ~wxLC_MASK_SORT ;
    }

  if ( flag & style )
  {
    if ( !add )
      flag -= style;
  }
  else
  {
    if ( add )
    {
      flag |= style;
    }
  }

  m_windowStyle = flag;

  RecreateWindow();
}

// Set the whole window style
void wxListCtrl::SetWindowStyleFlag(long flag)
{
  m_windowStyle = flag;

  RecreateWindow();
}

void wxListCtrl::RecreateWindow(void)
{
  if ( GetHWND() )
  {
    long oldStyle = 0;
      long style = ConvertToMSWStyle(oldStyle, m_windowStyle);
    style |= m_baseStyle;
//      ::SetWindowLong((HWND) GetHWND(), GWL_STYLE, style);

      // The following recreation of the window appears to be necessary
    // because SetWindowLong doesn't seem to do it.

      int x, y, width, height;
      GetPosition(&x, &y);
      GetSize(&width, &height);

      UnsubclassWin();
      ::DestroyWindow((HWND) GetHWND());

      // Experimental
      // Recreate the ListView control: unfortunately I can't
    // make it work by using SetWindowLong.
      bool want3D;
      WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;
      HWND hWndListControl = CreateWindowEx(exStyle,
        WC_LISTVIEW,
        "",
        style,
        x, y, width, height,
        (HWND) GetParent()->GetHWND(),
        (HMENU)m_windowId,
        wxGetInstance(),
        NULL );

      m_hWnd = (WXHWND) hWndListControl;
      SubclassWin((WXHWND) m_hWnd);

      if ( m_imageListNormal )
      SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
      if ( m_imageListSmall )
      SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);
      if ( m_imageListState )
      SetImageList(m_imageListState, wxIMAGE_LIST_STATE);
  }
}

// Can be just a single style, or a bitlist
long wxListCtrl::ConvertToMSWStyle(long& oldStyle, long style) const
{
  long wstyle = 0;
  if ( style & wxLC_ICON )
  {
  if ( (oldStyle & LVS_TYPEMASK) == LVS_SMALLICON )
    oldStyle -= LVS_SMALLICON;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_REPORT )
    oldStyle -= LVS_REPORT;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_LIST )
    oldStyle -= LVS_LIST;
  wstyle |= LVS_ICON;
  }

  if ( style & wxLC_SMALL_ICON )
  {
  if ( (oldStyle & LVS_TYPEMASK) == LVS_ICON )
    oldStyle -= LVS_ICON;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_REPORT )
    oldStyle -= LVS_REPORT;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_LIST )
    oldStyle -= LVS_LIST;
  wstyle |= LVS_SMALLICON;
  }

  if ( style & wxLC_LIST )
  {
  if ( (oldStyle & LVS_TYPEMASK) == LVS_ICON )
    oldStyle -= LVS_ICON;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_REPORT )
    oldStyle -= LVS_REPORT;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_SMALLICON )
    oldStyle -= LVS_SMALLICON;
  wstyle |= LVS_LIST;
  }

  if ( style & wxLC_REPORT )
  {
  if ( (oldStyle & LVS_TYPEMASK) == LVS_ICON )
    oldStyle -= LVS_ICON;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_LIST )
    oldStyle -= LVS_LIST;
  if ( (oldStyle & LVS_TYPEMASK) == LVS_SMALLICON )
    oldStyle -= LVS_SMALLICON;
  wstyle |= LVS_REPORT;
  }

  if ( style & wxLC_ALIGN_LEFT )
  {
  if ( oldStyle & LVS_ALIGNTOP )
    oldStyle -= LVS_ALIGNTOP;
  wstyle |= LVS_ALIGNLEFT;
  }

  if ( style & wxLC_ALIGN_TOP )
  {
  if ( oldStyle & LVS_ALIGNLEFT )
    oldStyle -= LVS_ALIGNLEFT;
  wstyle |= LVS_ALIGNTOP;
  }

  if ( style & wxLC_AUTOARRANGE )
  wstyle |= LVS_AUTOARRANGE;

  // Apparently, no such style (documentation wrong?)
/*
  if ( style & wxLC_BUTTON )
  wstyle |= LVS_BUTTON;
*/

  if ( style & wxLC_NO_SORT_HEADER )
  wstyle |= LVS_NOSORTHEADER;

  if ( style & wxLC_NO_HEADER )
  wstyle |= LVS_NOCOLUMNHEADER;

  if ( style & wxLC_EDIT_LABELS )
  wstyle |= LVS_EDITLABELS;

  if ( style & wxLC_SINGLE_SEL )
  wstyle |= LVS_SINGLESEL;

  if ( style & wxLC_SORT_ASCENDING )
  {
  if ( oldStyle & LVS_SORTDESCENDING )
    oldStyle -= LVS_SORTDESCENDING;
  wstyle |= LVS_SORTASCENDING;
  }

  if ( style & wxLC_SORT_DESCENDING )
  {
  if ( oldStyle & LVS_SORTASCENDING )
    oldStyle -= LVS_SORTASCENDING;
  wstyle |= LVS_SORTDESCENDING;
  }

  return wstyle;
}

// Sets the background colour (GetBackgroundColour already implicit in
// wxWindow class)
void wxListCtrl::SetBackgroundColour(const wxColour& col)
{
  wxWindow::SetBackgroundColour(col);

  ListView_SetBkColor((HWND) GetHWND(), PALETTERGB(col.Red(), col.Green(), col.Blue()));
}

// Gets information about this column
bool wxListCtrl::GetColumn(int col, wxListItem& item) const
{
  LV_COLUMN lvCol;
  lvCol.mask = 0;
  lvCol.fmt = 0;
  lvCol.pszText = NULL;

     if ( item.m_mask & wxLIST_MASK_TEXT )
  {
    lvCol.mask |= LVCF_TEXT;
    lvCol.pszText = new char[513];
    lvCol.cchTextMax = 512;
  }

  bool success = (ListView_GetColumn((HWND) GetHWND(), col, & lvCol) != 0);

//  item.m_subItem = lvCol.iSubItem;
  item.m_width = lvCol.cx;

     if ( (item.m_mask & wxLIST_MASK_TEXT) && lvCol.pszText )
  {
    item.m_text = lvCol.pszText;
    delete[] lvCol.pszText;
  }

  if ( item.m_mask & wxLIST_MASK_FORMAT )
  {
    if (lvCol.fmt == LVCFMT_LEFT)
      item.m_format = wxLIST_FORMAT_LEFT;
    else if (lvCol.fmt == LVCFMT_RIGHT)
      item.m_format = wxLIST_FORMAT_RIGHT;
    else if (lvCol.fmt == LVCFMT_CENTER)
      item.m_format = wxLIST_FORMAT_CENTRE;
    }

  return success;
}

// Sets information about this column
bool wxListCtrl::SetColumn(int col, wxListItem& item)
{
  LV_COLUMN lvCol;
  lvCol.mask = 0;
  lvCol.fmt = 0;
  lvCol.pszText = NULL;

     if ( item.m_mask & wxLIST_MASK_TEXT )
  {
    lvCol.mask |= LVCF_TEXT;
    lvCol.pszText = WXSTRINGCAST item.m_text;
    lvCol.cchTextMax = 0; // Ignored
  }
  if ( item.m_mask & wxLIST_MASK_FORMAT )
  {
    lvCol.mask |= LVCF_FMT;

    if ( item.m_format == wxLIST_FORMAT_LEFT )
      lvCol.fmt = LVCFMT_LEFT;
    if ( item.m_format == wxLIST_FORMAT_RIGHT )
      lvCol.fmt = LVCFMT_RIGHT;
    if ( item.m_format == wxLIST_FORMAT_CENTRE )
      lvCol.fmt = LVCFMT_CENTER;
    }

     if ( item.m_mask & wxLIST_MASK_WIDTH )
  {
    lvCol.mask |= LVCF_WIDTH;
    lvCol.cx = item.m_width;

    if ( lvCol.cx == wxLIST_AUTOSIZE)
      lvCol.cx = LVSCW_AUTOSIZE;
    else if ( lvCol.cx == wxLIST_AUTOSIZE_USEHEADER)
      lvCol.cx = LVSCW_AUTOSIZE_USEHEADER;
  }
  lvCol.mask |= LVCF_SUBITEM;
  lvCol.iSubItem = col;
  return (ListView_SetColumn((HWND) GetHWND(), col, & lvCol) != 0);
}

// Gets the column width
int wxListCtrl::GetColumnWidth(int col) const
{
  return ListView_GetColumnWidth((HWND) GetHWND(), col);
}

// Sets the column width
bool wxListCtrl::SetColumnWidth(int col, int width)
{
  int col2 = col;
  if ( m_windowStyle & wxLC_LIST )
    col2 = -1;

  int width2 = width;
  if ( width2 == wxLIST_AUTOSIZE)
    width2 = LVSCW_AUTOSIZE;
  else if ( width2 == wxLIST_AUTOSIZE_USEHEADER)
    width2 = LVSCW_AUTOSIZE_USEHEADER;

  return (ListView_SetColumnWidth((HWND) GetHWND(), col2, width2) != 0);
}

// Gets the number of items that can fit vertically in the
// visible area of the list control (list or report view)
// or the total number of items in the list control (icon
// or small icon view)
int wxListCtrl::GetCountPerPage(void) const
{
  return ListView_GetCountPerPage((HWND) GetHWND());
}

// Gets the edit control for editing labels.
wxTextCtrl* wxListCtrl::GetEditControl(void) const
{
    return m_textCtrl;
}

// Gets information about the item
bool wxListCtrl::GetItem(wxListItem& info) const
{
  LV_ITEM lvItem;
#ifdef __MINGW32__
  memset(&lvItem, 0, sizeof(lvItem));
#else
  ZeroMemory(&lvItem, sizeof(lvItem)); // must set all fields to 0
#endif

  lvItem.iItem = info.m_itemId;

  if ( info.m_mask & wxLIST_MASK_TEXT )
  {
    lvItem.mask |= LVIF_TEXT;
    lvItem.pszText = new char[513];
    lvItem.cchTextMax = 512;
  }
  else
  {
    lvItem.pszText = NULL;
  }

  if ( info.m_mask & wxLIST_MASK_STATE )
  {
    lvItem.mask |= LVIF_STATE;
    // the other bits are hardly interesting anyhow
    lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
  }

  bool success = ListView_GetItem((HWND)GetHWND(), &lvItem) != 0;
  if ( !success )
  {
    wxLogError(_("Couldn't retrieve information about list control item %d."),
               lvItem.iItem);
  }
  else
  {
    wxConvertFromMSWListItem(this, info, lvItem);
  }

  if (lvItem.pszText)
    delete[] lvItem.pszText;

  return success;
}

// Sets information about the item
bool wxListCtrl::SetItem(wxListItem& info)
{
  LV_ITEM item;
    wxConvertToMSWListItem(this, info, item);
  item.cchTextMax = 0;
  return (ListView_SetItem((HWND) GetHWND(), &item) != 0);
}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{
  wxListItem info;
  info.m_text = label;
  info.m_mask = wxLIST_MASK_TEXT;
  info.m_itemId = index;
  info.m_col = col;
  if ( imageId > -1 )
  {
    info.m_image = imageId;
    info.m_mask |= wxLIST_MASK_IMAGE;
  }
  return SetItem(info);
}


// Gets the item state
int wxListCtrl::GetItemState(long item, long stateMask) const
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_STATE ;
  info.m_stateMask = stateMask;
  info.m_itemId = item;

  if (!GetItem(info))
    return 0;

  return info.m_state;
}

// Sets the item state
bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_STATE ;
  info.m_state = state;
  info.m_stateMask = stateMask;
  info.m_itemId = item;

  return SetItem(info);
}

// Sets the item image
bool wxListCtrl::SetItemImage(long item, int image, int selImage)
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_IMAGE ;
  info.m_image = image;
  info.m_itemId = item;

  return SetItem(info);
}

// Gets the item text
wxString wxListCtrl::GetItemText(long item) const
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_TEXT ;
  info.m_itemId = item;

  if (!GetItem(info))
    return wxString("");
  return info.m_text;
}

// Sets the item text
void wxListCtrl::SetItemText(long item, const wxString& str)
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_TEXT ;
  info.m_itemId = item;
  info.m_text = str;

  SetItem(info);
}

// Gets the item data
long wxListCtrl::GetItemData(long item) const
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_DATA ;
  info.m_itemId = item;

  if (!GetItem(info))
    return 0;
  return info.m_data;
}

// Sets the item data
bool wxListCtrl::SetItemData(long item, long data)
{
  wxListItem info;

  info.m_mask = wxLIST_MASK_DATA ;
  info.m_itemId = item;
  info.m_data = data;

  return SetItem(info);
}

// Gets the item rectangle
bool wxListCtrl::GetItemRect(long item, wxRect& rect, int code) const
{
  RECT rect2;

  int code2 = LVIR_BOUNDS;
  if ( code == wxLIST_RECT_BOUNDS )
    code2 = LVIR_BOUNDS;
  else if ( code == wxLIST_RECT_ICON )
    code2 = LVIR_ICON;
  else if ( code == wxLIST_RECT_LABEL )
    code2 = LVIR_LABEL;

  bool success = (ListView_GetItemRect((HWND) GetHWND(), (int) item, &rect2, code2) != 0);

  rect.x = rect2.left;
  rect.y = rect2.top;
  rect.width = rect2.right - rect2.left;
  rect.height = rect2.bottom - rect2.left;
  return success;
}

// Gets the item position
bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
  POINT pt;

  bool success = (ListView_GetItemPosition((HWND) GetHWND(), (int) item, &pt) != 0);

  pos.x = pt.x; pos.y = pt.y;
  return success;
}

// Sets the item position.
bool wxListCtrl::SetItemPosition(long item, const wxPoint& pos)
{
  return (ListView_SetItemPosition((HWND) GetHWND(), (int) item, pos.x, pos.y) != 0);
}

// Gets the number of items in the list control
int wxListCtrl::GetItemCount(void) const
{
  return ListView_GetItemCount((HWND) GetHWND());
}

// Retrieves the spacing between icons in pixels.
// If small is TRUE, gets the spacing for the small icon
// view, otherwise the large icon view.
int wxListCtrl::GetItemSpacing(bool isSmall) const
{
  return ListView_GetItemSpacing((HWND) GetHWND(), (BOOL) isSmall);
}

// Gets the number of selected items in the list control
int wxListCtrl::GetSelectedItemCount(void) const
{
  return ListView_GetSelectedCount((HWND) GetHWND());
}

// Gets the text colour of the listview
wxColour wxListCtrl::GetTextColour(void) const
{
  COLORREF ref = ListView_GetTextColor((HWND) GetHWND());
  wxColour col(GetRValue(ref), GetGValue(ref), GetBValue(ref));
  return col;
}

// Sets the text colour of the listview
void wxListCtrl::SetTextColour(const wxColour& col)
{
  ListView_SetTextColor((HWND) GetHWND(), PALETTERGB(col.Red(), col.Blue(), col.Green()));
}

// Gets the index of the topmost visible item when in
// list or report view
long wxListCtrl::GetTopItem(void) const
{
  return (long) ListView_GetTopIndex((HWND) GetHWND());
}

// Searches for an item, starting from 'item'.
// 'geometry' is one of
// wxLIST_NEXT_ABOVE/ALL/BELOW/LEFT/RIGHT.
// 'state' is a state bit flag, one or more of
// wxLIST_STATE_DROPHILITED/FOCUSED/SELECTED/CUT.
// item can be -1 to find the first item that matches the
// specified flags.
// Returns the item or -1 if unsuccessful.
long wxListCtrl::GetNextItem(long item, int geom, int state) const
{
  long flags = 0;

  if ( geom == wxLIST_NEXT_ABOVE )
    flags |= LVNI_ABOVE;
  if ( geom == wxLIST_NEXT_ALL )
    flags |= LVNI_ALL;
  if ( geom == wxLIST_NEXT_BELOW )
    flags |= LVNI_BELOW;
  if ( geom == wxLIST_NEXT_LEFT )
    flags |= LVNI_TOLEFT;
  if ( geom == wxLIST_NEXT_RIGHT )
    flags |= LVNI_TORIGHT;

  if ( state & wxLIST_STATE_CUT )
    flags |= LVNI_CUT;
  if ( state & wxLIST_STATE_DROPHILITED )
    flags |= LVNI_DROPHILITED;
  if ( state & wxLIST_STATE_FOCUSED )
    flags |= LVNI_FOCUSED;
  if ( state & wxLIST_STATE_SELECTED )
    flags |= LVNI_SELECTED;

  return (long) ListView_GetNextItem((HWND) GetHWND(), item, flags);
}


wxImageList *wxListCtrl::GetImageList(int which) const
{
  if ( which == wxIMAGE_LIST_NORMAL )
    {
    return m_imageListNormal;
  }
  else if ( which == wxIMAGE_LIST_SMALL )
    {
    return m_imageListSmall;
  }
  else if ( which == wxIMAGE_LIST_STATE )
    {
    return m_imageListState;
  }
  return NULL;
}

void wxListCtrl::SetImageList(wxImageList *imageList, int which)
{
  int flags = 0;
  if ( which == wxIMAGE_LIST_NORMAL )
    {
    flags = LVSIL_NORMAL;
    m_imageListNormal = imageList;
  }
  else if ( which == wxIMAGE_LIST_SMALL )
    {
    flags = LVSIL_SMALL;
    m_imageListSmall = imageList;
  }
  else if ( which == wxIMAGE_LIST_STATE )
    {
    flags = LVSIL_STATE;
    m_imageListState = imageList;
  }
  ListView_SetImageList((HWND) GetHWND(), (HIMAGELIST) imageList ? imageList->GetHIMAGELIST() : 0, flags);
}

// Operations
////////////////////////////////////////////////////////////////////////////

// Arranges the items
bool wxListCtrl::Arrange(int flag)
{
  UINT code = 0;
  if ( flag == wxLIST_ALIGN_LEFT )
    code = LVA_ALIGNLEFT;
  else if ( flag == wxLIST_ALIGN_TOP )
    code = LVA_ALIGNTOP;
  else if ( flag == wxLIST_ALIGN_DEFAULT )
    code = LVA_DEFAULT;
  else if ( flag == wxLIST_ALIGN_SNAP_TO_GRID )
    code = LVA_SNAPTOGRID;

  return (ListView_Arrange((HWND) GetHWND(), code) != 0);
}

// Deletes an item
bool wxListCtrl::DeleteItem(long item)
{
  return (ListView_DeleteItem((HWND) GetHWND(), (int) item) != 0);
}

// Deletes all items
bool wxListCtrl::DeleteAllItems(void)
{
  return (ListView_DeleteAllItems((HWND) GetHWND()) != 0);
}

// Deletes all items
bool wxListCtrl::DeleteAllColumns(void)
{
  int i;
    for ( i = 0; i < m_colCount; i++)
    {
    if (ListView_DeleteColumn((HWND) GetHWND(), 0) != 0)
        m_colCount --;
    }
    return (m_colCount == 0);
}

// Deletes a column
bool wxListCtrl::DeleteColumn(int col)
{
  bool success = (ListView_DeleteColumn((HWND) GetHWND(), col) != 0);

    if ( success && (m_colCount > 0) )
        m_colCount --;
    return success;
}

// Clears items, and columns if there are any.
void wxListCtrl::ClearAll(void)
{
    DeleteAllItems();
    if ( m_colCount > 0 )
        DeleteAllColumns();
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    wxASSERT( (textControlClass->IsKindOf(CLASSINFO(wxTextCtrl))) );

    HWND hWnd = (HWND) ListView_EditLabel((HWND) GetHWND(), item);

    if (m_textCtrl)
    {
      m_textCtrl->UnsubclassWin();
      m_textCtrl->SetHWND(0);
      delete m_textCtrl;
      m_textCtrl = NULL;
    }

    m_textCtrl = (wxTextCtrl*) textControlClass->CreateObject();
    m_textCtrl->SetHWND((WXHWND) hWnd);
    m_textCtrl->SubclassWin((WXHWND) hWnd);

    return m_textCtrl;
}

// End label editing, optionally cancelling the edit
bool wxListCtrl::EndEditLabel(bool cancel)
{
    wxFAIL;

/* I don't know how to implement this: there's no such macro as ListView_EndEditLabelNow.
 * ???
    bool success = (ListView_EndEditLabelNow((HWND) GetHWND(), cancel) != 0);

    if (m_textCtrl)
    {
      m_textCtrl->UnsubclassWin();
      m_textCtrl->SetHWND(0);
      delete m_textCtrl;
      m_textCtrl = NULL;
    }
    return success;
*/
  return FALSE;
}


// Ensures this item is visible
bool wxListCtrl::EnsureVisible(long item)
{
  return (ListView_EnsureVisible((HWND) GetHWND(), (int) item, FALSE) != 0);
}

// Find an item whose label matches this string, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
  LV_FINDINFO findInfo;

  findInfo.flags = LVFI_STRING;
  if ( partial )
    findInfo.flags |= LVFI_STRING;
  findInfo.psz = WXSTRINGCAST str;

  return ListView_FindItem((HWND) GetHWND(), (int) start, & findInfo);
}

// Find an item whose data matches this data, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, long data)
{
  LV_FINDINFO findInfo;

  findInfo.flags = LVFI_PARAM;
  findInfo.lParam = data;

  return ListView_FindItem((HWND) GetHWND(), (int) start, & findInfo);
}

// Find an item nearest this position in the specified direction, starting from
// the item after 'start' or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxPoint& pt, int direction)
{
  LV_FINDINFO findInfo;

  findInfo.flags = LVFI_NEARESTXY;
  findInfo.pt.x = pt.x;
  findInfo.pt.y = pt.y;
  findInfo.vkDirection = VK_RIGHT;

  if ( direction == wxLIST_FIND_UP )
    findInfo.vkDirection = VK_UP;
  else if ( direction == wxLIST_FIND_DOWN )
    findInfo.vkDirection = VK_DOWN;
  else if ( direction == wxLIST_FIND_LEFT )
    findInfo.vkDirection = VK_LEFT;
  else if ( direction == wxLIST_FIND_RIGHT )
    findInfo.vkDirection = VK_RIGHT;

  return ListView_FindItem((HWND) GetHWND(), (int) start, & findInfo);
}

// Determines which item (if any) is at the specified point,
// giving details in 'flags' (see wxLIST_HITTEST_... flags above)
long wxListCtrl::HitTest(const wxPoint& point, int& flags)
{
    LV_HITTESTINFO hitTestInfo;
  hitTestInfo.pt.x = (int) point.x;
  hitTestInfo.pt.y = (int) point.y;

    ListView_HitTest((HWND) GetHWND(), & hitTestInfo);

  flags = 0;
  if ( hitTestInfo.flags & LVHT_ABOVE )
    flags |= wxLIST_HITTEST_ABOVE;
  if ( hitTestInfo.flags & LVHT_BELOW )
    flags |= wxLIST_HITTEST_BELOW;
  if ( hitTestInfo.flags & LVHT_NOWHERE )
    flags |= wxLIST_HITTEST_NOWHERE;
  if ( hitTestInfo.flags & LVHT_ONITEMICON )
    flags |= wxLIST_HITTEST_ONITEMICON;
  if ( hitTestInfo.flags & LVHT_ONITEMLABEL )
    flags |= wxLIST_HITTEST_ONITEMLABEL;
  if ( hitTestInfo.flags & LVHT_ONITEMSTATEICON )
    flags |= wxLIST_HITTEST_ONITEMSTATEICON;
  if ( hitTestInfo.flags & LVHT_TOLEFT )
    flags |= wxLIST_HITTEST_TOLEFT;
  if ( hitTestInfo.flags & LVHT_TORIGHT )
    flags |= wxLIST_HITTEST_TORIGHT;

  return (long) hitTestInfo.iItem ;
}

// Inserts an item, returning the index of the new item if successful,
// -1 otherwise.
long wxListCtrl::InsertItem(wxListItem& info)
{
  LV_ITEM item;
  wxConvertToMSWListItem(this, info, item);

  return (long) ListView_InsertItem((HWND) GetHWND(), & item);
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
  wxListItem info;
  info.m_text = label;
  info.m_mask = wxLIST_MASK_TEXT;
  info.m_itemId = index;
  return InsertItem(info);
}

// Inserts an image item
long wxListCtrl::InsertItem(long index, int imageIndex)
{
  wxListItem info;
  info.m_image = imageIndex;
  info.m_mask = wxLIST_MASK_IMAGE;
  info.m_itemId = index;
  return InsertItem(info);
}

// Inserts an image/string item
long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
  wxListItem info;
  info.m_image = imageIndex;
  info.m_text = label;
  info.m_mask = wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT;
  info.m_itemId = index;
  return InsertItem(info);
}

// For list view mode (only), inserts a column.
long wxListCtrl::InsertColumn(long col, wxListItem& item)
{
  LV_COLUMN lvCol;
  lvCol.mask = 0;
  lvCol.fmt = 0;
  lvCol.pszText = NULL;

     if ( item.m_mask & wxLIST_MASK_TEXT )
  {
    lvCol.mask |= LVCF_TEXT;
    lvCol.pszText = WXSTRINGCAST item.m_text;
    lvCol.cchTextMax = 0; // Ignored
  }
  if ( item.m_mask & wxLIST_MASK_FORMAT )
  {
    lvCol.mask |= LVCF_FMT;

    if ( item.m_format == wxLIST_FORMAT_LEFT )
      lvCol.fmt = LVCFMT_LEFT;
    if ( item.m_format == wxLIST_FORMAT_RIGHT )
      lvCol.fmt = LVCFMT_RIGHT;
    if ( item.m_format == wxLIST_FORMAT_CENTRE )
      lvCol.fmt = LVCFMT_CENTER;
    }

     if ( item.m_mask & wxLIST_MASK_WIDTH )
  {
    lvCol.mask |= LVCF_WIDTH;
    lvCol.cx = item.m_width;

    if ( lvCol.cx == wxLIST_AUTOSIZE)
      lvCol.cx = LVSCW_AUTOSIZE;
    else if ( lvCol.cx == wxLIST_AUTOSIZE_USEHEADER)
      lvCol.cx = LVSCW_AUTOSIZE_USEHEADER;
  }
  lvCol.mask |= LVCF_SUBITEM;
  lvCol.iSubItem = col;

  bool success = (ListView_InsertColumn((HWND) GetHWND(), col, & lvCol) != 0);
    if ( success )
        m_colCount ++;
    return success;
}

long wxListCtrl::InsertColumn(long col, const wxString& heading, int format,
    int width)
{
  wxListItem item;
  item.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT;
  item.m_text = heading;
  if ( width > -1 )
  {
    item.m_mask |= wxLIST_MASK_WIDTH;
    item.m_width = width;
  }
  item.m_format = format;

  return InsertColumn(col, item);
}

// Scrolls the list control. If in icon, small icon or report view mode,
// x specifies the number of pixels to scroll. If in list view mode, x
// specifies the number of columns to scroll.
// If in icon, small icon or list view mode, y specifies the number of pixels
// to scroll. If in report view mode, y specifies the number of lines to scroll.
bool wxListCtrl::ScrollList(int dx, int dy)
{
  return (ListView_Scroll((HWND) GetHWND(), dx, dy) != 0);
}

// Sort items.

// fn is a function which takes 3 long arguments: item1, item2, data.
// item1 is the long data associated with a first item (NOT the index).
// item2 is the long data associated with a second item (NOT the index).
// data is the same value as passed to SortItems.
// The return value is a negative number if the first item should precede the second
// item, a positive number of the second item should precede the first,
// or zero if the two items are equivalent.

// data is arbitrary data to be passed to the sort function.
bool wxListCtrl::SortItems(wxListCtrlCompare fn, long data)
{
  return (ListView_SortItems((HWND) GetHWND(), (PFNLVCOMPARE) fn, data) != 0);
}

bool wxListCtrl::MSWCommand(WXUINT cmd, WXWORD id)
{
  if (cmd == EN_UPDATE)
  {
    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, id);
    event.SetEventObject( this );
    ProcessCommand(event);
    return TRUE;
  }
  else if (cmd == EN_KILLFOCUS)
  {
    wxCommandEvent event(wxEVT_KILL_FOCUS, id);
    event.SetEventObject( this );
    ProcessCommand(event);
    return TRUE;
  }
  else return FALSE;
}

bool wxListCtrl::MSWNotify(WXWPARAM wParam, WXLPARAM lParam, WXLPARAM *result)
{
  wxListEvent event(wxEVT_NULL, m_windowId);
  wxEventType eventType = wxEVT_NULL;
  NMHDR *hdr1 = (NMHDR *) lParam;
  switch ( hdr1->code )
  {
    case LVN_BEGINRDRAG:
        eventType = wxEVT_COMMAND_LIST_BEGIN_RDRAG;
        // fall through

    case LVN_BEGINDRAG:
        if ( eventType == wxEVT_NULL )
        {
            eventType = wxEVT_COMMAND_LIST_BEGIN_DRAG;
        }

        {
          NM_LISTVIEW *hdr = (NM_LISTVIEW *)lParam;
          event.m_itemIndex = hdr->iItem;
          event.m_pointDrag.x = hdr->ptAction.x;
          event.m_pointDrag.y = hdr->ptAction.y;
        }
        break;

    case LVN_BEGINLABELEDIT:
    {
      eventType = wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT;
      LV_DISPINFO *info = (LV_DISPINFO *)lParam;
      wxConvertFromMSWListItem(this, event.m_item, info->item, (HWND) GetHWND());
      break;
    }

    case LVN_COLUMNCLICK:
    {
      eventType = wxEVT_COMMAND_LIST_COL_CLICK;
      NM_LISTVIEW* hdr = (NM_LISTVIEW*)lParam;
      event.m_itemIndex = -1;
      event.m_col = hdr->iSubItem;
      break;
    }
    case LVN_DELETEALLITEMS:
    {
      eventType = wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS;
//      NM_LISTVIEW* hdr = (NM_LISTVIEW*)lParam;
      event.m_itemIndex = -1;
      break;
    }
    case LVN_DELETEITEM:
    {
      eventType = wxEVT_COMMAND_LIST_DELETE_ITEM;
      NM_LISTVIEW* hdr = (NM_LISTVIEW*)lParam;
      event.m_itemIndex = hdr->iItem;
      break;
    }
    case LVN_ENDLABELEDIT:
    {
      eventType = wxEVT_COMMAND_LIST_END_LABEL_EDIT;
      LV_DISPINFO *info = (LV_DISPINFO *)lParam;
      wxConvertFromMSWListItem(this, event.m_item, info->item, (HWND) GetHWND());
      if ( info->item.pszText == NULL || info->item.iItem == -1 )
        event.m_cancelled = TRUE;
      break;
    }
    case LVN_GETDISPINFO:
    {
//      return FALSE;
      // TODO: some text buffering here, I think
      // TODO: API for getting Windows to retrieve values
      // on demand.
      eventType = wxEVT_COMMAND_LIST_GET_INFO;
      LV_DISPINFO *info = (LV_DISPINFO *)lParam;
      wxConvertFromMSWListItem(this, event.m_item, info->item, (HWND) GetHWND());
      break;
    }
    case LVN_INSERTITEM:
    {
      eventType = wxEVT_COMMAND_LIST_INSERT_ITEM;
      NM_LISTVIEW* hdr = (NM_LISTVIEW*)lParam;
      event.m_itemIndex = hdr->iItem;
      break;
    }
    case LVN_ITEMCHANGED:
    {
      // This needs to be sent to wxListCtrl as a rather more
      // concrete event. For now, just detect a selection
      // or deselection.
      NM_LISTVIEW* hdr = (NM_LISTVIEW*)lParam;
      if ( (hdr->uNewState & LVIS_SELECTED) && !(hdr->uOldState & LVIS_SELECTED) )
      {
        eventType = wxEVT_COMMAND_LIST_ITEM_SELECTED;
        event.m_itemIndex = hdr->iItem;
      }
      else if ( !(hdr->uNewState & LVIS_SELECTED) && (hdr->uOldState & LVIS_SELECTED) )
      {
        eventType = wxEVT_COMMAND_LIST_ITEM_DESELECTED;
        event.m_itemIndex = hdr->iItem;
      }
      else
        return FALSE;
      break;
    }
    case LVN_KEYDOWN:
    {
      eventType = wxEVT_COMMAND_LIST_KEY_DOWN;
      LV_KEYDOWN *info = (LV_KEYDOWN *)lParam;
      event.m_code = wxCharCodeMSWToWX(info->wVKey);
      break;
    }
    case LVN_SETDISPINFO:
    {
      eventType = wxEVT_COMMAND_LIST_SET_INFO;
      LV_DISPINFO *info = (LV_DISPINFO *)lParam;
      wxConvertFromMSWListItem(this, event.m_item, info->item, (HWND) GetHWND());
      break;
    }

    default :
      return wxControl::MSWNotify(wParam, lParam, result);
  }

  event.SetEventObject( this );
  event.SetEventType(eventType);

  if ( !GetEventHandler()->ProcessEvent(event) )
    return FALSE;

  if (hdr1->code == LVN_GETDISPINFO)
  {
    LV_DISPINFO *info = (LV_DISPINFO *)lParam;
    if ( info->item.mask & LVIF_TEXT )
    {
      if ( !event.m_item.m_text.IsNull() )
      {
        info->item.pszText = AddPool(event.m_item.m_text);
        info->item.cchTextMax = strlen(info->item.pszText) + 1;
      }
    }
//    wxConvertToMSWListItem(this, event.m_item, info->item);
  }

  *result = !event.IsAllowed();

  return TRUE;
}

char *wxListCtrl::AddPool(const wxString& str)
{
  // Remove the first element if 3 strings exist
  if ( m_stringPool.Number() == 3 )
  {
    wxNode *node = m_stringPool.First();
    delete[] (char *)node->Data();
    delete node;
  }
  wxNode *node = m_stringPool.Add((char *) (const char *)str);
  return (char *)node->Data();
}

// List item structure
wxListItem::wxListItem(void)
{
    m_mask = 0;
    m_itemId = 0;
    m_col = 0;
    m_state = 0;
    m_stateMask = 0;
    m_image = 0;
  m_data = 0;

  m_format = wxLIST_FORMAT_CENTRE;
  m_width = 0;
}

static void wxConvertFromMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& lvItem, HWND getFullInfo)
{
  info.m_data = lvItem.lParam;
  info.m_mask = 0;
  info.m_state = 0;
  info.m_stateMask = 0;
  info.m_itemId = lvItem.iItem;

  long oldMask = lvItem.mask;

  bool needText = FALSE;
  if (getFullInfo != 0)
  {
    if ( lvItem.mask & LVIF_TEXT )
      needText = FALSE;
    else
      needText = TRUE;

    if ( needText )
    {
      lvItem.pszText = new char[513];
      lvItem.cchTextMax = 512;
    }
//    lvItem.mask |= TVIF_HANDLE | TVIF_STATE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM ;
    lvItem.mask |= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM ;
    ::SendMessage(getFullInfo, LVM_GETITEM, 0, (LPARAM)& lvItem) ;
  }

  if ( lvItem.mask & LVIF_STATE )
  {
    info.m_mask |= wxLIST_MASK_STATE;

      if ( lvItem.stateMask & LVIS_CUT)
    {
      info.m_stateMask |= wxLIST_STATE_CUT ;
      if ( lvItem.state & LVIS_CUT )
        info.m_state |= wxLIST_STATE_CUT ;
    }
      if ( lvItem.stateMask & LVIS_DROPHILITED)
    {
      info.m_stateMask |= wxLIST_STATE_DROPHILITED ;
      if ( lvItem.state & LVIS_DROPHILITED )
        info.m_state |= wxLIST_STATE_DROPHILITED ;
    }
      if ( lvItem.stateMask & LVIS_FOCUSED)
    {
      info.m_stateMask |= wxLIST_STATE_FOCUSED ;
      if ( lvItem.state & LVIS_FOCUSED )
        info.m_state |= wxLIST_STATE_FOCUSED ;
    }
      if ( lvItem.stateMask & LVIS_SELECTED)
    {
      info.m_stateMask |= wxLIST_STATE_SELECTED ;
      if ( lvItem.state & LVIS_SELECTED )
        info.m_state |= wxLIST_STATE_SELECTED ;
    }
  }

  if ( lvItem.mask & LVIF_TEXT )
  {
    info.m_mask |= wxLIST_MASK_TEXT;
    info.m_text = lvItem.pszText;
  }
  if ( lvItem.mask & LVIF_IMAGE )
  {
    info.m_mask |= wxLIST_MASK_IMAGE;
    info.m_image = lvItem.iImage;
  }
  if ( lvItem.mask & LVIF_PARAM )
    info.m_mask |= wxLIST_MASK_DATA;
  if ( lvItem.mask & LVIF_DI_SETITEM )
    info.m_mask |= wxLIST_SET_ITEM;
  info.m_col = lvItem.iSubItem;

  if (needText)
  {
         if (lvItem.pszText)
      delete[] lvItem.pszText;
  }
  lvItem.mask = oldMask ;
}

static void wxConvertToMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& lvItem)
{
  lvItem.iItem = (int) info.m_itemId ;

  lvItem.iImage = info.m_image ;
  lvItem.lParam = info.m_data;
  lvItem.stateMask = 0;
  lvItem.state = 0;
  lvItem.mask = 0;
  lvItem.iSubItem = info.m_col;

  if (info.m_mask & wxLIST_MASK_STATE)
  {
    lvItem.mask |= LVIF_STATE ;
    if (info.m_stateMask & wxLIST_STATE_CUT)
    {
        lvItem.stateMask |= LVIS_CUT ;
      if (info.m_state & wxLIST_STATE_CUT)
        lvItem.state |= LVIS_CUT;
    }
    if (info.m_stateMask & wxLIST_STATE_DROPHILITED)
    {
        lvItem.stateMask |= LVIS_DROPHILITED;
      if (info.m_state & wxLIST_STATE_DROPHILITED)
          lvItem.state |= LVIS_DROPHILITED;
    }
    if (info.m_stateMask & wxLIST_STATE_FOCUSED)
    {
        lvItem.stateMask |= LVIS_FOCUSED;
      if (info.m_state & wxLIST_STATE_FOCUSED)
          lvItem.state |= LVIS_FOCUSED;
    }
    if (info.m_stateMask & wxLIST_STATE_SELECTED)
    {
        lvItem.stateMask |= LVIS_SELECTED;
      if (info.m_state & wxLIST_STATE_SELECTED)
          lvItem.state |= LVIS_SELECTED;
    }
  }

  if (info.m_mask & wxLIST_MASK_TEXT)
  {
    lvItem.mask |= LVIF_TEXT ;
    if ( ctrl->GetWindowStyleFlag() & wxLC_USER_TEXT )
    {
      lvItem.pszText = LPSTR_TEXTCALLBACK;
    }
    else
    {
      lvItem.pszText = (char *) (const char *)info.m_text ;
      if ( lvItem.pszText )
        lvItem.cchTextMax = info.m_text.Length();
      else
        lvItem.cchTextMax = 0;
    }
  }
  if (info.m_mask & wxLIST_MASK_IMAGE)
    lvItem.mask |= LVIF_IMAGE ;
  if (info.m_mask & wxLIST_MASK_DATA)
    lvItem.mask |= LVIF_PARAM ;
}

// List event
IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxNotifyEvent)

wxListEvent::wxListEvent(wxEventType commandType, int id)
           : wxNotifyEvent(commandType, id)
{
  m_code = 0;
  m_itemIndex = 0;
  m_col = 0;
  m_cancelled = FALSE;
}

#endif

