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

#include "wx/osx/private.h"
#include "wx/osx/iphone/private/tablectrlimpl.h"
#include "wx/osx/iphone/private/tablecellimpl.h"

#include "wx/mobile/native/utils.h"
#include "wx/mobile/native/tablectrl.h"

#define MO_EDIT_CONTROL_MARGIN 35

IMPLEMENT_DYNAMIC_CLASS(wxMoTableCtrl, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxTablePath, wxObject)
IMPLEMENT_CLASS(wxMoTableDataSource, wxEvtHandler)
IMPLEMENT_CLASS(wxMoTableRow, wxObject)
IMPLEMENT_CLASS(wxMoTableSection, wxObject)

WX_DEFINE_EXPORTED_OBJARRAY(wxMoTableSectionArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxMoTableRowArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTablePathArray);

BEGIN_EVENT_TABLE(wxMoTableCtrl, wxWindow)
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


#pragma mark wxMoTableCtrl

/// Default constructor.
wxMoTableCtrl::wxMoTableCtrl()
{
    Init();
}

/// Constructor.
wxMoTableCtrl::wxMoTableCtrl(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, name);
}

void wxMoTableCtrl::Init()
{
    m_dataSource = NULL;
}

bool wxMoTableCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxWindow::Create( parent, id, pos, size, style, name )) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateTableViewCtrl( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;    
}

wxMoTableCtrl::~wxMoTableCtrl()
{
    
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
    return NULL;
    
    // Let the Cocoa part take care of the reusability.
    
    wxUITableView *tableView = (wxUITableView *)GetPeer()->GetWXWidget();
    if (! tableView) {
        return NULL;
    }
    
    NSString *cellIdentifier = wxCFStringRef(reuseName).AsNSString();
    if (! cellIdentifier) {
        return NULL;
    }
    
    wxUITableViewCell *cell = (wxUITableViewCell *)[tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (! cell) {
        return NULL;
    }
    
    return [cell moTableCell];
}

/// Gets the data source
wxMoTableDataSource* wxMoTableCtrl::GetDataSource() const {
    return m_dataSource;
}

/// Sets the data source
void wxMoTableCtrl::SetDataSource(wxMoTableDataSource* dataSource, bool ownsDataSource)
{
    m_dataSource = dataSource;
    m_ownsDataSource = ownsDataSource;
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
    m_sections.Clear();
    m_indexTitles.Clear();
    
    size_t i;
    for (i = 0; i < m_reusableCells.GetCount(); i++)
    {
        wxMoTableCell* cell = m_reusableCells[i];
        delete cell;
    }
    m_reusableCells.Clear();
    m_totalTableHeight = 0;
}

bool wxMoTableCtrl::ReloadData(bool resetScrollbars)
{
    Clear();

    wxTableViewCtrlIPhoneImpl *peer = (wxTableViewCtrlIPhoneImpl *)GetPeer();
    return peer->ReloadData();
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

#pragma mark wxMoTableRow

wxMoTableRow::wxMoTableRow(const wxMoTableRow& row)
{
    Copy(row);
}

wxMoTableRow::wxMoTableRow()
{
    Init();
}

wxMoTableRow::~wxMoTableRow()
{
    if (m_tableCell) {
        delete m_tableCell;
        m_tableCell = NULL;
    }
}

void wxMoTableRow::Init()
{
    m_tableCell = NULL;
    m_rowY = 0;
    m_rowHeight = 0;
}

void wxMoTableRow::operator=(const wxMoTableRow& row)
{
    Copy(row);
}

void wxMoTableRow::Copy(const wxMoTableRow& row)
{
    m_tableCell = row.m_tableCell;
    const_cast<wxMoTableRow&>(row).m_tableCell = NULL;
    m_rowY = row.m_rowY;
    m_rowHeight = row.m_rowHeight;    
}


#pragma mark wxTablePath

/// Copy constructor.
wxTablePath::wxTablePath(const wxTablePath& path)
{
    Copy(path);
}

/// Constructor.
wxTablePath::wxTablePath(int section, int row)
{
    Init();
    m_row = row;
    m_section = section;
}

void wxTablePath::Init()
{
    m_row = 0;
    m_section = 0;
}

/// Assignment operator.
void wxTablePath::operator=(const wxTablePath& path)
{
    Copy(path);
}

/// Equality operator.
bool wxTablePath::operator==(const wxTablePath& path) const
{
    return m_row == path.m_row && m_section == path.m_section;
}

bool wxTablePath::operator!=(const wxTablePath& path) const
{
    return m_row != path.m_row || m_section != path.m_section;
}

/// Copys the path.
void wxTablePath::Copy(const wxTablePath& path)
{
    m_row = path.m_row;
    m_section = path.m_section;
}

/// Sets the row index.
void wxTablePath::SetRow(int row)
{
    m_row = row;
}

/// Gets the row index.
int wxTablePath::GetRow() const
{
    return m_row;
}

/// Sets the section index.
void wxTablePath::SetSection(int section)
{
    m_section = section;
}

/// Gets the section index.
int wxTablePath::GetSection() const
{
    return m_section;
}

/// Returns true if the path is valid (neither row nor section are -1).
bool wxTablePath::IsValid() const
{
    return m_row != -1 && m_section != -1;
}


#pragma mark wxMoTableSection

wxMoTableSection::wxMoTableSection(const wxMoTableSection& section)
{
    Copy(section);
}

wxMoTableSection::wxMoTableSection()
{
    Init();
}

void wxMoTableSection::Init()
{
    m_sectionWidth = 0;
    m_sectionHeight = 0;
    m_sectionY = 0;
    m_sectionHeaderHeight = 0;
    m_sectionFooterHeight = 0;
    m_rowCount = 0;    
}

void wxMoTableSection::operator=(const wxMoTableSection& section)
{
    Copy(section);
}

void wxMoTableSection::Copy(const wxMoTableSection& section)
{
    m_sectionName = section.m_sectionName;
    m_sectionWidth = section.m_sectionWidth;
    m_sectionHeight = section.m_sectionHeight;
    m_sectionY = section.m_sectionY;
    m_rows = section.m_rows;
    m_sectionHeaderHeight = section.m_sectionHeaderHeight;
    m_sectionFooterHeight = section.m_sectionFooterHeight;
    m_rowCount = section.m_rowCount;    
}

void wxMoTableSection::SetSectionName(const wxString& name)
{
    m_sectionName = name;
}

const wxString& wxMoTableSection::GetSectionName() const
{
    return m_sectionName;
}

#pragma mark wxMoTableDataSource

// Called by the table control to commit an insertion requested by the user. This function should
// then call InsertRows in response.
bool wxMoTableDataSource::CommitInsertRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    wxTablePathArray paths;
    paths.Add(path);
    return ctrl->InsertRows(paths, wxMoTableCtrl::RowAnimationNone);
}

// Called by the table control to commit a deletion requested by the user. This function should
// then call DeleteRows in response.
bool wxMoTableDataSource::CommitDeleteRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    wxTablePathArray paths;
    paths.Add(path);
    return ctrl->DeleteRows(paths, wxMoTableCtrl::RowAnimationNone);
}
