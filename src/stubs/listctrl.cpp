/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:     wxListCtrl. See also Robert's generic wxListCtrl
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listctrl.h"
#endif

#include "wx/stubs/textctrl.h"
#include "wx/stubs/listctrl.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

#endif

wxListCtrl::wxListCtrl()
{
	m_imageListNormal = NULL;
	m_imageListSmall = NULL;
	m_imageListState = NULL;
	m_baseStyle = 0;
    m_colCount = 0;
}

bool wxListCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator, const wxString& name)
{
    m_imageListNormal = NULL;
    m_imageListSmall = NULL;
    m_imageListState = NULL;
    m_colCount = 0;

    SetValidator(validator);
    SetName(name);

    m_windowStyle = style;

    SetParent(parent);

    m_windowId = (id == -1) ? NewControlId() : id;

    if (parent) parent->AddChild(this);

    // TODO create list control
    return TRUE;
}

wxListCtrl::~wxListCtrl()
{
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

	/* TODO RecreateWindow(); */
}

// Set the whole window style
void wxListCtrl::SetWindowStyleFlag(long flag)
{
	m_windowStyle = flag;

	/* TODO RecreateWindow(); */
}


// Gets information about this column
bool wxListCtrl::GetColumn(int col, wxListItem& item) const
{
	// TODO
    return FALSE;
}

// Sets information about this column
bool wxListCtrl::SetColumn(int col, wxListItem& item)
{
	// TODO
    return FALSE;
}

// Gets the column width
int wxListCtrl::GetColumnWidth(int col) const
{
	// TODO
    return 0;
}

// Sets the column width
bool wxListCtrl::SetColumnWidth(int col, int width)
{
	// TODO
    return FALSE;
}

// Gets the number of items that can fit vertically in the
// visible area of the list control (list or report view)
// or the total number of items in the list control (icon
// or small icon view)
int wxListCtrl::GetCountPerPage() const
{
	// TODO
    return 0;
}

// Gets the edit control for editing labels.
wxTextCtrl* wxListCtrl::GetEditControl() const
{
	return m_textCtrl;
}

// Gets information about the item
bool wxListCtrl::GetItem(wxListItem& info) const
{
	// TODO
    return FALSE;
}

// Sets information about the item
bool wxListCtrl::SetItem(wxListItem& info)
{
	// TODO
    return FALSE;
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
bool wxListCtrl::GetItemRect(long item, wxRectangle& rect, int code) const
{
	// TODO
    return FALSE;
}

// Gets the item position
bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
	// TODO
    return FALSE;
}

// Sets the item position.
bool wxListCtrl::SetItemPosition(long item, const wxPoint& pos)
{
	// TODO
    return FALSE;
}

// Gets the number of items in the list control
int wxListCtrl::GetItemCount() const
{
	// TODO
    return FALSE;
}

// Retrieves the spacing between icons in pixels.
// If small is TRUE, gets the spacing for the small icon
// view, otherwise the large icon view.
int wxListCtrl::GetItemSpacing(bool isSmall) const
{
	// TODO
    return FALSE;
}

// Gets the number of selected items in the list control
int wxListCtrl::GetSelectedItemCount() const
{
	// TODO
    return FALSE;
}

// Gets the text colour of the listview
wxColour wxListCtrl::GetTextColour() const
{
	// TODO
    return wxColour();
}

// Sets the text colour of the listview
void wxListCtrl::SetTextColour(const wxColour& col)
{
	// TODO
}

// Gets the index of the topmost visible item when in
// list or report view
long wxListCtrl::GetTopItem() const
{
	// TODO
    return 0;
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
	// TODO
    return 0;
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
		m_imageListNormal = imageList;
	}
	else if ( which == wxIMAGE_LIST_SMALL )
    {
		m_imageListSmall = imageList;
	}
	else if ( which == wxIMAGE_LIST_STATE )
    {
		m_imageListState = imageList;
	}
	// TODO set image list
}

// Operations
////////////////////////////////////////////////////////////////////////////

// Arranges the items
bool wxListCtrl::Arrange(int flag)
{
	// TODO
    return FALSE;
}

// Deletes an item
bool wxListCtrl::DeleteItem(long item)
{
	// TODO
    return FALSE;
}

// Deletes all items
bool wxListCtrl::DeleteAllItems()
{
	// TODO
    return FALSE;
}

// Deletes all items
bool wxListCtrl::DeleteAllColumns()
{
	// TODO
    return FALSE;
}

// Deletes a column
bool wxListCtrl::DeleteColumn(int col)
{
	// TODO
    return FALSE;
}

// Clears items, and columns if there are any.
void wxListCtrl::ClearAll()
{
    DeleteAllItems();
    if ( m_colCount > 0 )
        DeleteAllColumns();
}

// Edit the label
wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    // TODO
    return NULL;
}

// End label editing, optionally cancelling the edit
bool wxListCtrl::EndEditLabel(bool cancel)
{
    // TODO
    return FALSE;
}

// Ensures this item is visible
bool wxListCtrl::EnsureVisible(long item)
{
    // TODO
    return FALSE;
}

// Find an item whose label matches this string, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    // TODO
    return FALSE;
}

// Find an item whose data matches this data, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, long data)
{
    // TODO
    return 0;
}

// Find an item nearest this position in the specified direction, starting from
// the item after 'start' or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxPoint& pt, int direction)
{
    // TODO
    return 0;
}

// Determines which item (if any) is at the specified point,
// giving details in 'flags' (see wxLIST_HITTEST_... flags above)
long wxListCtrl::HitTest(const wxPoint& point, int& flags)
{
    // TODO
    return 0;
}

// Inserts an item, returning the index of the new item if successful,
// -1 otherwise.
long wxListCtrl::InsertItem(wxListItem& info)
{
    // TODO
    return 0;
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
    // TODO
    return 0;
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
    // TODO
    return FALSE;
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
    // TODO
    return FALSE;
}

// List item structure
wxListItem::wxListItem()
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

// List event
IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxCommandEvent)

wxListEvent::wxListEvent(wxEventType commandType, int id):
  wxCommandEvent(commandType, id)
{
	m_code = 0;
	m_itemIndex = 0;
	m_col = 0;
	m_cancelled = FALSE;
}

