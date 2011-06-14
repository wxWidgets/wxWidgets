/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_tablectrl_n.mm
// Purpose:     wxMoTableCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*
Notes

  To construct index path in Cocoa Touch, use this constructor:

  [NSIndexPath indexPathForRow: inSection:]

  Need to call reloadData before scrolling to a row. Perhaps
  this can be incorporated into ScrollToRow.

  UITableViewCell memory management

  We need to support the reuse of cells.
  How do we manage these objects?
  Need to do what Cocoa Touch does, and maintain a list of cells
  which are no longer in use.
  Items can be cached so we're not always creating or reusing cells,
  only when more items scroll into view.

  Control needs to store just enough info to keep track of each row height,
  section dimensions etc.

  Since we need to allow customisable row appearance, we probably need to
  use content windows as per Cocoa Touch, so subwindows can be attached.
  So, the standard content window would draw the image and text, which
  would be obscured by a custom window placed in the content view.

  Add a wxMoTableCellContentWindow which does the image + label drawing,
  and can be used as the parent of custom cell windows.

  */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/dcbuffer.h"
#include "wx/arrimpl.cpp"

#include "wx/mobile/native/utils.h"
#include "wx/mobile/native/tablectrl.h"

#define MO_EDIT_CONTROL_MARGIN 35

IMPLEMENT_DYNAMIC_CLASS(wxMoTableCtrl, wxMoScrolledWindow)
IMPLEMENT_DYNAMIC_CLASS(wxTablePath, wxObject)
IMPLEMENT_CLASS(wxMoTableDataSource, wxEvtHandler)
IMPLEMENT_CLASS(wxMoTableRow, wxObject)
IMPLEMENT_CLASS(wxMoTableSection, wxObject)

WX_DEFINE_EXPORTED_OBJARRAY(wxMoTableSectionArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxMoTableRowArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTablePathArray);

BEGIN_EVENT_TABLE(wxMoTableCtrl, wxMoScrolledWindow)
    EVT_PAINT(wxMoTableCtrl::OnPaint)
    EVT_SIZE(wxMoTableCtrl::OnSize)
    EVT_TOUCH_SCROLL_DRAG(wxMoTableCtrl::OnTouchScrollDrag)
    EVT_TOUCH_SCROLL_CANCEL_TOUCHES(wxMoTableCtrl::OnCancelTouch)
    EVT_MOUSE_EVENTS(wxMoTableCtrl::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoTableCtrl::OnEraseBackground)
    EVT_MOUSE_CAPTURE_CHANGED(wxMoTableCtrl::OnMouseCaptureChanged)

    EVT_TABLE_ADD_CLICKED(wxID_ANY, wxMoTableCtrl::OnAddClicked)
    EVT_TABLE_DELETE_CLICKED(wxID_ANY, wxMoTableCtrl::OnDeleteClicked)
    EVT_TABLE_CONFIRM_DELETE_CLICKED(wxID_ANY, wxMoTableCtrl::OnConfirmDeleteClicked)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTableCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ROW_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ADD_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_MOVE_DRAGGED)

#define wxMoTableCtrlLoadBitmap(bitmapData) LoadBitmap(bitmapData, sizeof(bitmapData))

bool wxMoTableCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoTableCtrl::~wxMoTableCtrl()
{
    // FIXME stub
}

void wxMoTableCtrl::Init()
{
    // FIXME stub
}

wxSize wxMoTableCtrl::DoGetBestSize() const
{
    // FIXME stub
	wxSize best(100, 100);
    return best;
}

// Load a bitmap from data
wxBitmap wxMoTableCtrl::LoadBitmap(const char* bitmapData, size_t len)
{
    // FIXME stub

	wxBitmap empty;
	return empty;
}

bool wxMoTableCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoTableCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

void wxMoTableCtrl::Freeze()
{
    // FIXME stub
}

void wxMoTableCtrl::Thaw()
{
    // FIXME stub
}

bool wxMoTableCtrl::Enable(bool enable)
{
    // FIXME stub

	return true;
}

bool wxMoTableCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

bool wxMoTableCtrl::SetSectionFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

// Recalculates dimensions
bool wxMoTableCtrl::RecalculateDimensions()
{
    // FIXME stub

    return true;
}

void wxMoTableCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// FIXME stub
}

void wxMoTableCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

// Hit test
int wxMoTableCtrl::HitTest(const wxPoint& pt, int& retSection, int& retRow, int& region) const
{
    // FIXME stub
	return 0;
}

void wxMoTableCtrl::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

void wxMoTableCtrl::OnMouseCaptureChanged(wxMouseCaptureChangedEvent& WXUNUSED(event))
{
}

void wxMoTableCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoTableCtrl::OnTouchScrollDrag(wxTouchScrollEvent& event)
{
    // FIXME stub
}

// Handle touch cancelling from the scroll window
void wxMoTableCtrl::OnCancelTouch(wxTouchScrollEvent& event)
{
    // FIXME stub
}

// Repositions all row controls
void wxMoTableCtrl::PositionAllRowControls()
{
    // FIXME stub
}

void wxMoTableCtrl::OnInternalIdle()
{
    // FIXME stub
}

wxMoTableCell* wxMoTableCtrl::GetReusableCell(const wxString& reuseName)
{
    // FIXME stub

    return NULL;
}

// Finds the path for the cell
bool wxMoTableCtrl::FindPathForCell(wxMoTableCell* cell, wxTablePath& path) const
{
    // FIXME stub

    return false;
}

// Finds the cell for the path.
wxMoTableCell* wxMoTableCtrl::FindCellForPath(const wxTablePath& path) const
{
    // FIXME stub

    return NULL;
}

// Clears all data
void wxMoTableCtrl::Clear()
{
    // FIXME stub
}

bool wxMoTableCtrl::ReloadData(bool resetScrollbars)
{
    // FIXME stub

    return true;
}

// Loads the data within the specified rectangle.
bool wxMoTableCtrl::LoadVisibleData(const wxRect& rect1)
{
    // FIXME stub

    return true;
}

// Positions the controls in the specified row.
bool wxMoTableCtrl::PositionRowControls(const wxTablePath& path )
{
    // FIXME stub

    return true;
}

void wxMoTableCtrl::SetSelection(const wxTablePath& path)
{
    // FIXME stub
}

void wxMoTableCtrl::Deselect(const wxTablePath& path)
{
    // FIXME stub
}

wxTablePath* wxMoTableCtrl::GetSelection() const
{
    // FIXME stub

	return NULL;
}

// Scroll to the given section
bool wxMoTableCtrl::ScrollToSection(int section)
{
    // FIXME stub

	return true;
}

// Scroll to the given path (section/row)
bool wxMoTableCtrl::ScrollToPath(const wxTablePath& path)
{
    // FIXME stub

    return false;
}

// Deletes the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::DeleteRows(const wxTablePathArray& WXUNUSED(paths), RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub

    return true;
}

// Inserts the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::InsertRows(const wxTablePathArray& WXUNUSED(paths), RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub

    return true;
}

// Deletes the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::DeleteSections(const wxArrayInt& WXUNUSED(sections), RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub

    return true;
}

// Inserts the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::InsertSections(const wxArrayInt& WXUNUSED(sections), RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub

    return true;
}

// Refreshes the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::ReloadSections(const wxArrayInt& WXUNUSED(sections), RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub

    return true;
}

// Inserts the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::ReloadRows(const wxTablePathArray& WXUNUSED(paths), RowAnimationStyle WXUNUSED(animationStyle))
{
    // FIXME stub

    return true;
}

// Sets editing mode (not yet implemented).
bool wxMoTableCtrl::SetEditingMode(bool editingMode, bool WXUNUSED(animated))
{
    // FIXME stub

    return true;
}

void wxMoTableCtrl::DoShowDeleteButton(bool show, const wxTablePath& path)
{
    // FIXME stub
}

void wxMoTableCtrl::OnAddClicked(wxTableCtrlEvent& event)
{
    // FIXME stub
}

void wxMoTableCtrl::OnDeleteClicked(wxTableCtrlEvent& event)
{
    // FIXME stub
}

void wxMoTableCtrl::OnConfirmDeleteClicked(wxTableCtrlEvent& event)
{
    // FIXME stub
}

void wxMoTableCtrl::OnConfirmDeleteButtonClicked(wxCommandEvent& WXUNUSED(cmdEvent))
{
    // FIXME stub
}


void wxMoTableRow::Init()
{
    // FIXME stub
}

wxMoTableRow::~wxMoTableRow()
{
    // FIXME stub
}

void wxMoTableRow::Copy(const wxMoTableRow& row)
{
    // FIXME stub
}

void wxMoTableSection::Init()
{
    // FIXME stub
}

void wxMoTableSection::Copy(const wxMoTableSection& section)
{
    // FIXME stub
}

// Called by the table control to commit an insertion requested by the user. This function should
// then call InsertRows in response.
bool wxMoTableDataSource::CommitInsertRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    // FIXME stub

	return true;
}

// Called by the table control to commit a deletion requested by the user. This function should
// then call DeleteRows in response.
bool wxMoTableDataSource::CommitDeleteRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    // FIXME stub

	return true;
}
