/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/tablectrl.h
// Purpose:     wxMoTableCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_TABLECTRL_H_
#define _WX_MOBILE_GENERIC_TABLECTRL_H_

#include "wx/dynarray.h"

#include "wx/mobile/scrollwin.h"
#include "wx/mobile/tablecell.h"
#include "wx/mobile/button.h"

// Table control styles
#define wxTC_PLAIN          0x0800
#define wxTC_GROUPED        0x1000

class WXDLLEXPORT wxMoTableDataSource;
class WXDLLEXPORT wxTableCtrlEvent;

/**
    @class wxTablePath

    This class encapsulates a section and row in a table, by zero-based index.

    @category{wxMobile}
*/

class WXDLLEXPORT wxTablePath: public wxObject
{
public:
    /// Copy constructor.
    wxTablePath(const wxTablePath& path) { Copy(path); }

    /// Constructor.
    wxTablePath(int section = 0, int row = 0) { Init(); m_row = row; m_section = section; }

    void Init() { m_row = 0; m_section = 0; }

    /// Assignment operator.
    void operator=(const wxTablePath& path) { Copy(path); }

    /// Equality operator.
    bool operator==(const wxTablePath& path) const { return m_row == path.m_row && m_section == path.m_section; }
    bool operator!=(const wxTablePath& path) const { return m_row != path.m_row || m_section != path.m_section; }

    /// Copys the path.
    void Copy(const wxTablePath& path) { m_row = path.m_row; m_section = path.m_section; }

    /// Sets the row index.
    void SetRow(int row) { m_row = row; }

    /// Gets the row index.
    int GetRow() const { return m_row; }

    /// Sets the section index.
    void SetSection(int section) { m_section = section; }

    /// Gets the section index.
    int GetSection() const { return m_section; }

    /// Returns true if the path is valid (neither row nor section are -1).
    bool IsValid() const { return m_row != -1 && m_section != -1; }

protected:
    int m_row;
    int m_section;

    DECLARE_DYNAMIC_CLASS(wxTablePath)
};

WX_DECLARE_OBJARRAY(wxTablePath, wxTablePathArray);

/*
    @class wxMoTableRow

    Represents a row in a table view. This is for implementation only.

    @category{wxMobileImplementation}
*/

class WXDLLEXPORT wxMoTableRow: public wxObject
{
public:
    wxMoTableRow(const wxMoTableRow& row) { Copy(row); }
    wxMoTableRow() { Init(); }
    ~wxMoTableRow();

    void Init();

    void operator=(const wxMoTableRow& row) { Copy(row); }
    void Copy(const wxMoTableRow& row);

    // Can be NULL if the cell has been reused.
    wxMoTableCell*    m_tableCell;
    int             m_rowY;
    int             m_rowHeight;

    DECLARE_CLASS(wxMoTableRow)
};

WX_DECLARE_OBJARRAY(wxMoTableRow, wxMoTableRowArray);

/*
    @class wxMoTableSection

    Represents a section in a table view. This is for implementation only.

    @category{wxMobileImplementation}
*/

class WXDLLEXPORT wxMoTableSection: public wxObject
{
public:
    wxMoTableSection(const wxMoTableSection& section) { Copy(section); }
    wxMoTableSection() { Init(); }

    void Init();

    void operator=(const wxMoTableSection& section) { Copy(section); }
    void Copy(const wxMoTableSection& section);

    void SetSectionName(const wxString& name) { m_sectionName = name; }
    const wxString& GetSectionName() const { return m_sectionName; }

    wxString            m_sectionName;
    int                 m_sectionWidth;
    int                 m_sectionHeight;
    int                 m_sectionY;
    int                 m_sectionHeaderHeight;
    int                 m_sectionFooterHeight;
    int                 m_rowCount;
    wxMoTableRowArray   m_rows;

    DECLARE_CLASS(wxMoTableSection)
};

WX_DECLARE_OBJARRAY(wxMoTableSection, wxMoTableSectionArray);

/**
    @class wxMoTableCtrl

    This class manages a table of one-column rows that can have sections (grouped) or no sections
    and an optional index (plain). Use the wxTC_GROUPED style for a grouped table,
    and wxTC_PLAIN for a plain one.

    To populate a wxMoTableCtrl, you don't have to derive a new class from it but you do need
    to give it a data source. Derive a new class from wxMoTableDataSource
    and implement various functions that let the table know about data and
    cell characteristics. Here are descriptions of the important functions.

    wxMoTableDataSource::GetCell returns the cell object for a given wxTablePath.
    You should first call wxMoTableCtrl::GetReusableCell with a unique name for this
    cell type to see if the table can use an existing cell that is not currently displayed.
    If one is available, set its properties and return that cell; otherwise, create a
    a new wxMoTableCell.

    Implement wxMoTableDataSource::GetSectionCount to return the number of sections
    in the table. This is relevant even for the plain style, since there can still
    be section names.

    wxMoTableDataSource::GetRowCount should return the number of rows in a given section.

    If wxMoTableDataSource::GetRowHeight returns a number greater than zero, the value is
    used for the height of the given row.

    To supply section titles, return an array of strings from wxMoTableDataSource::GetSectionTitles.

    If you're using the wxTC_PLAIN style you can return an array of strings for the index
    on the right hand side using wxMoTableDataSource::GetIndexTitles, with the same number of strings are there are sections.
    This will create an index that automatically takes the display to the start of the
    relevant section when index items are clicked.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoTableCtrl: public wxMoScrolledWindow
{
public:
    /// Cell separator style.
    enum { CellSeparatorStyleNone, CellSeparatorStyleSingleLine };

    /// Row animation style.
    typedef enum { RowAnimationNone, RowAnimationFade, RowAnimationRight, RowAnimationLeft,
        RowAnimationTop, RowAnimationBottom } RowAnimationStyle;

    /// Edit style.
    typedef enum {  EditStyleNone =                      0x00,
                    EditStyleShowDeleteButton =          0x01,
                    EditStyleShowInsertButton =          0x02 }
                 CellEditStyle;

    // Implementation only.
    enum { HitTestNone, HitTestAccessory, HitTestIndex, HitTestRow, HitTestAdd, HitTestDelete, HitTestConfirmDelete, HitTestReorder  };
    enum { HitTestRegionUnknown, HitTestRegionTop, HitTestRegionBottom };
    enum { MouseStatusNone, MouseStatusClicking, MouseStatusDraggingRow };

    /// Default constructor.
    wxMoTableCtrl() { Init(); }

    /// Constructor.
    wxMoTableCtrl(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxButtonNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxButtonNameStr);

    virtual ~wxMoTableCtrl();

    void Init();

    /// Reloads the data. Resets the scroll position to the start
    /// if resetScrollbars is true, otherwise resets it to the nearest
    /// position to the previous one.
    virtual bool ReloadData(bool resetScrollbars = true);

    /// Loads the data within the specified rectangle.
    virtual bool LoadVisibleData(const wxRect& rect);

    /// Clears all data
    virtual void Clear();

    /// Starts batching of operations within which no visual updates are performed.
    /// You can nest Freeze/Thaw operations.
    virtual void Freeze();

    /// Ends batching of operations, refetches the data, and refreshes the window.
    virtual void Thaw();

    /// Returns true if Freeze has been called more times than Thaw.
    virtual bool IsFrozen() const { return m_freezeCount > 0; }

    /// Scroll to the given section
    virtual bool ScrollToSection(int section);

    /// Scroll to the given path (section/row)
    virtual bool ScrollToPath(const wxTablePath& path);

    /// Deletes the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool DeleteRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle);

    /// Inserts the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool InsertRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle);

    /// Deletes the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool DeleteSections(const wxArrayInt& sections, RowAnimationStyle animationStyle);

    /// Inserts the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool InsertSections(const wxArrayInt& sections, RowAnimationStyle animationStyle);

    /// Refreshes the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool ReloadSections(const wxArrayInt& sections, RowAnimationStyle animationStyle);

    /// Inserts the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool ReloadRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle);

    /// Sets editing mode (not yet implemented).
    virtual bool SetEditingMode(bool editingMode, bool animated = true);

    /// Returns true if in edit mode (not implemented).
    virtual bool IsEditing() const { return m_editingMode; }

    // Finds a cell that isn't being displayed currently,
    // either from the current pool or from an area of
    // the table that isn't currently showing.
    wxMoTableCell* GetReusableCell(const wxString& reuseName);

    /// Gets the data source
    wxMoTableDataSource* GetDataSource() const { return m_dataSource; }

    /// Sets the data source
    virtual void SetDataSource(wxMoTableDataSource* dataSource, bool ownsDataSource = true)
    {
        m_dataSource = dataSource;
        m_ownsDataSource = ownsDataSource;
    }

    /// Finds the path for the cell.
    bool FindPathForCell(wxMoTableCell* cell, wxTablePath& path) const;

    /// Finds the cell for the path.
    wxMoTableCell* FindCellForPath(const wxTablePath& path) const;

    /// Returns true if the control is plain.
    virtual bool IsPlain() const { return !HasFlag(wxTC_GROUPED); }

    /// Returns true if the control is grouped.
    virtual bool IsGrouped() const { return HasFlag(wxTC_GROUPED); }

    /// Sets the selection to the given path.
    void SetSelection(const wxTablePath& path);

    /// Removes the selection at the given path.
    void Deselect(const wxTablePath& path);

    /// Gets the selection, or NULL if there is none.
    wxTablePath* GetSelection() const;

    /// Sets the cell separator style.
    void SetCellSeparatorStyle(int style) { m_separatorStyle = style; }

    /// Gets the cell separator style.
    int GetCellSeparatorStyle() const { return m_separatorStyle; }

    /// Sets the standard row height in pixels.
    void SetRowHeight(int height) { m_rowHeight = height; }

    /// Gets the standard row height in pixels.
    int GetRowHeight() const { return m_rowHeight; }

    /// Sets the section header height in pixels.
    void SetSectionHeaderHeight(int height) { m_sectionHeaderHeight = height; }

    /// Gets the section header height in pixels.
    int GetSectionHeaderHeight() const { return m_sectionHeaderHeight; }

    /// Sets the section footer height in pixels.
    void SetSectionFooterHeight(int height) { m_sectionFooterHeight = height; }

    /// Gets the section footer height in pixels.
    int GetSectionFooterHeight() const { return m_sectionFooterHeight; }

    /// Is the delete confirmation button showing for this path?
    bool IsDeleteButtonShowing(const wxTablePath& path) const { m_deletePath == path; }

    /// Returns @true if row selection is allowed during editing. The default is @false.
    bool GetAllowSelectionDuringEditing() const { return m_allowsSelectionDuringEditing; }

    /// Pass @true to allow row selection during editing.
    void SetAllowSelectionDuringEditing(bool allow) { m_allowsSelectionDuringEditing = allow; }

// Implementation-only from here

    // Repositions all row controls
    void PositionAllRowControls();

    // Positions the controls in the specified row.
    bool PositionRowControls(const wxTablePath& path);

    // Sets the left and right margin in grouped mode.
    void SetLeftRightMargin(int margin) { m_leftRightMargin = margin; }

    // Gets the left and right margin in grouped mode.
    int GetLeftRightMargin() const { return m_leftRightMargin; }

    // Sets the top and bottom margin in grouped mode.
    void SetTopBottomMargin(int margin) { m_topBottomMargin = margin; }

    // Gets the top and bottom margin in grouped mode.
    int GetTopBottomMargin() const { return m_topBottomMargin; }

    void SetInterSectionSpacing(int spacing) { m_interSectionSpacing = spacing; }
    int GetInterSectionSpacing() const { return m_interSectionSpacing; }

    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);

    virtual bool SetSectionFont(const wxFont& font);
    virtual wxFont GetSectionFont() const { return m_sectionFont; }

    virtual bool HasIndex() const { return m_indexTitles.GetCount() > 0; }

    void DoShowDeleteButton(bool show, const wxTablePath& path);
    void ShowDeleteButton(const wxTablePath& path) { DoShowDeleteButton(true, path); }
    void HideDeleteButton() { DoShowDeleteButton(false, wxTablePath(-1, -1)); }
    wxTablePath GetDeletePath() const { return m_deletePath; }
    bool DeletingThisRow(const wxTablePath& path) const { return m_deletePath == path; }
    bool Deleting() const { return m_deletePath.IsValid(); }

    // Load a bitmap from data
    wxBitmap LoadBitmap(const char* bitmapData, size_t len);
    wxBitmap LoadBitmap(unsigned char bitmapData[], size_t len) { return LoadBitmap((const char*) bitmapData, len); }

protected:

    virtual wxSize DoGetBestSize() const;
    virtual void OnInternalIdle();

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTouchScrollDrag(wxTouchScrollEvent & event);
    void OnCancelTouch(wxTouchScrollEvent& event);
    void OnMouseCaptureChanged(wxMouseCaptureChangedEvent& event);

    void OnAddClicked(wxTableCtrlEvent& event);
    void OnDeleteClicked(wxTableCtrlEvent& event);
    void OnConfirmDeleteClicked(wxTableCtrlEvent& event);
    void OnConfirmDeleteButtonClicked(wxCommandEvent& event);

    /// Hit test
    int HitTest(const wxPoint& pt, int& retSection, int& retRow, int& region) const;

    // Recalculates dimensions
    bool RecalculateDimensions();

    wxMoTableCellArray      m_reusableCells;
    wxMoTableSectionArray   m_sections;
    wxMoTableDataSource*    m_dataSource;
    bool                    m_ownsDataSource;
    int                     m_leftRightMargin;
    int                     m_topBottomMargin;
    int                     m_interSectionSpacing;
    int                     m_rowHeight;
    int                     m_sectionHeaderHeight;
    int                     m_sectionFooterHeight;
    int                     m_totalTableHeight;
    int                     m_accessoryWidth;
    int                     m_separatorStyle;
    wxBitmap                m_disclosureBitmap;
    wxBitmap                m_detailDisclosureBitmap;
    wxBitmap                m_checkmarkBitmap;
    wxBitmap                m_deleteBitmap;
    wxBitmap                m_rotatedDeleteBitmap;
    wxBitmap                m_addBitmap;
    wxBitmap                m_reorderBitmap;
    wxFont                  m_sectionFont;
    wxTablePath             m_selection;
    wxArrayString           m_indexTitles;
    int                     m_mouseStatus;
    int                     m_freezeCount;
    bool                    m_editingMode;
    bool                    m_allowsSelectionDuringEditing;
    wxMoButton*             m_deleteButton;
    wxTablePath             m_deletePath;
    wxTablePath             m_insertAt; // for drawing insertion marker
    wxTablePath             m_originalInsertionPoint; // starting insertion point

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoTableCtrl)
    DECLARE_EVENT_TABLE()
};

/**
    @class wxMoTableDataSource

    This class provides data for a table. Derive from the class
    and override the various functions to return section and row
    counts, row height, and actual cell data.

    For more information, see wxMoTableCtrl and wxMoTableCell.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoTableDataSource: public wxEvtHandler
{
public:
    wxMoTableDataSource() { Init(); }

    ~wxMoTableDataSource() {}

    void Init() {}

    /// Returns a table cell for the give location. Call wxMoTableCtrl::GetReusableCell
    /// to check whether an off-screen cell can be reused, and create a new wxMoTableCel
    /// otherwise.
    virtual wxMoTableCell* GetCell(wxMoTableCtrl* ctrl, const wxTablePath& path) = 0;

    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxMoTableCtrl* ctrl) const = 0;

    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxMoTableCtrl* ctrl, int section) const = 0;

    /// Returns the height of the row at this location in pixels.
    virtual int GetRowHeight(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) const { return 0; }

    /// Returns the height of the specified section header in pixels. If this returns -1, the table uses the
    /// value returned by wxMoTableCtrl::GetSectionHeaderHeight.
    virtual int GetSectionHeaderHeight(wxMoTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const { return -1; }

    /// Returns the height of the specified section footer in pixels. If this returns -1, the table uses the
    /// value returned by wxMoTableCtrl::GetSectionFooterHeight.
    virtual int GetSectionFooterHeight(wxMoTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const { return -1; }

    /// Returns the section titles.
    virtual wxArrayString GetSectionTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const { return wxArrayString(); }

    /// Returns an array of strings, each one corresponding to an index title to be shown
    /// on the right hand side of a plain list.
    virtual wxArrayString GetIndexTitles(wxMoTableCtrl* WXUNUSED(ctrl)) const { return wxArrayString(); }

    /// Called by the table control to commit an insertion requested by the user. This function should
    /// then call InsertRows in response.
    virtual bool CommitInsertRow(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Called by the table control to commit a deletion requested by the user. This function should
    /// then call DeleteRows in response.
    virtual bool CommitDeleteRow(wxMoTableCtrl* ctrl, const wxTablePath& path);

    /// Returns true if this row can be edited.
    virtual bool CanEditRow(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) { return true; }

    /// Returns true if this row can be moved.
    virtual bool CanMoveRow(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) { return false; }

    /// Requests a move operation from 'from' to 'to'. The data should then reflect
    /// the new ordering. Return false if the reordering was not done.
    virtual bool MoveRow(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(pathFrom), const wxTablePath& WXUNUSED(pathTo)) { return false; }

    /// Get the edit style for a row
    virtual wxMoTableCtrl::CellEditStyle GetCellEditStyle(wxMoTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path))
    { return wxMoTableCtrl::EditStyleShowDeleteButton; }

    DECLARE_CLASS(wxMoTableDataSource)
};

/**
    @class wxTableCtrlEvent

    Use this event for table controls.

    @category{wxMobile}

*/

class WXDLLEXPORT wxTableCtrlEvent: public wxNotifyEvent
{
public:
    wxTableCtrlEvent()
    {
        m_tableCtrl = NULL;
    }
    wxTableCtrlEvent(const wxTableCtrlEvent& event): wxNotifyEvent(event)
    {
        m_tableCtrl = event.m_tableCtrl;
        m_path = event.m_path;
    }
    wxTableCtrlEvent(wxEventType commandType, int ctrlId, wxMoTableCtrl* ctrl)
        : wxNotifyEvent(commandType, ctrlId)
    {
         m_tableCtrl = ctrl;
    }

    /// Set the table control
    void SetSheet(wxMoTableCtrl* tableCtrl) { m_tableCtrl = tableCtrl; }

    /// Get the table control
    wxMoTableCtrl* GetTableCtrl() const { return m_tableCtrl; }

    /// Set the path
    void SetPath(const wxTablePath& path) { m_path = path; }

    /// Get the path
    const wxTablePath& GetPath() const { return m_path; }

    virtual wxEvent *Clone() const { return new wxTableCtrlEvent(*this); }

private:
    wxTablePath     m_path;
    wxMoTableCtrl*  m_tableCtrl;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxTableCtrlEvent)
};

typedef void (wxEvtHandler::*wxTableCtrlEventFunction)(wxTableCtrlEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ROW_SELECTED,            880)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED,       881)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ADD_CLICKED,             882)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TABLE_DELETE_CLICKED,          883)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED,  884)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TABLE_MOVE_DRAGGED,            885)
END_DECLARE_EVENT_TYPES()

#define wxTableCtrlEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxTableCtrlEventHandler, &func)

#define EVT_TABLE_ROW_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TABLE_ROW_SELECTED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTableCtrlEventFunction, & fn ), NULL),

#define EVT_TABLE_ACCESSORY_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTableCtrlEventFunction, & fn ), NULL),

#define EVT_TABLE_ADD_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TABLE_ADD_CLICKED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTableCtrlEventFunction, & fn ), NULL),

#define EVT_TABLE_DELETE_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TABLE_DELETE_CLICKED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTableCtrlEventFunction, & fn ), NULL),

#define EVT_TABLE_CONFIRM_DELETE_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxTableCtrlEventFunction, & fn ), NULL),

#endif
    // _WX_MOBILE_GENERIC_TABLECTRL_H_
