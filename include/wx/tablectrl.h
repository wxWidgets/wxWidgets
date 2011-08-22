/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tablectrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABLECTRL_H_BASE_
#define _WX_TABLECTRL_H_BASE_

#include "wx/dynarray.h"

#include "wx/tablecell.h"
#include "wx/button.h"

// Table control styles
#define wxTC_PLAIN          0x0800
#define wxTC_GROUPED        0x1000

class WXDLLEXPORT wxTableDataSource;
class WXDLLEXPORT wxTableCtrlEvent;


#pragma mark wxTablePath

/**
    @class wxTablePath

    This class encapsulates a section and row in a table, by zero-based index.

    @category{wxbile}
*/

class WXDLLEXPORT wxTablePath: public wxObject
{
public:
        
    /// Copy constructor.
    wxTablePath(const wxTablePath& path) { Copy(path); }
    
    /// Constructor.
    wxTablePath(int section = 0, int row = 0) { Init(); m_row = row; m_section = section; }

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
    
    void Init() { m_row = 0; m_section = 0; }

    int m_row;
    int m_section;

    DECLARE_DYNAMIC_CLASS(wxTablePath)
};

WX_DECLARE_OBJARRAY(wxTablePath, wxTablePathArray);


#pragma mark wxTableRow

/*
    @class wxTableRow

    Represents a row in a table view. This is for implementation only.

    @category{wxbileImplementation}
*/

class WXDLLEXPORT wxTableRow: public wxObject
{
public:
    wxTableRow(const wxTableRow& row) { Copy(row); }
    wxTableRow() { Init(); }
    ~wxTableRow()
    {
        if (m_tableCell) {
            delete m_tableCell;
            m_tableCell = NULL;
        }
    }
    
    void Init() { m_tableCell = NULL; m_rowY = 0; m_rowHeight = 0;}

    void operator=(const wxTableRow& row) { Copy(row); }
    void Copy(const wxTableRow& row)
    {
        m_tableCell = row.m_tableCell;
        const_cast<wxTableRow&>(row).m_tableCell = NULL;
        m_rowY = row.m_rowY;
        m_rowHeight = row.m_rowHeight;
    }

protected:
    // Can be NULL if the cell has been reused.
    wxTableCell*    m_tableCell;
    int             m_rowY;
    int             m_rowHeight;

    DECLARE_CLASS(wxTableRow)
};

WX_DECLARE_OBJARRAY(wxTableRow, wxTableRowArray);


#pragma mark wxTableSection

/*
    @class wxTableSection

    Represents a section in a table view. This is for implementation only.

    @category{wxbileImplementation}
*/


class WXDLLEXPORT wxTableSection: public wxObject
{
public:
    wxTableSection(const wxTableSection& section) { Copy(section); }
    wxTableSection() { Init(); }

    void Init()
    {
        m_sectionWidth = 0;
        m_sectionHeight = 0;
        m_sectionY = 0;
        m_sectionHeaderHeight = 0;
        m_sectionFooterHeight = 0;
        m_rowCount = 0;
    }

    void operator=(const wxTableSection& section) { Copy(section); }
    void Copy(const wxTableSection& section)
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

    void SetSectionName(const wxString& name) { m_sectionName = name; }
    const wxString& GetSectionName() const { return m_sectionName; }

protected:
    wxString            m_sectionName;
    int                 m_sectionWidth;
    int                 m_sectionHeight;
    int                 m_sectionY;
    int                 m_sectionHeaderHeight;
    int                 m_sectionFooterHeight;
    int                 m_rowCount;
    wxTableRowArray     m_rows;

    DECLARE_CLASS(wxTableSection)
};

WX_DECLARE_OBJARRAY(wxTableSection, wxTableSectionArray);


#pragma mark wxTableCtrl

/**
    @class wxTableCtrl

    This class manages a table of one-column rows that can have sections (grouped) or no sections
    and an optional index (plain). Use the wxTC_GROUPED style for a grouped table,
    and wxTC_PLAIN for a plain one.

    To populate a wxTableCtrl, you don't have to derive a new class from it but you do need
    to give it a data source. Derive a new class from wxTableDataSource
    and implement various functions that let the table know about data and
    cell characteristics. Here are descriptions of the important functions.

    wxTableDataSource::GetCell returns the cell object for a given wxTablePath.
    You should first call wxTableCtrl::GetReusableCell with a unique name for this
    cell type to see if the table can use an existing cell that is not currently displayed.
    If one is available, set its properties and return that cell; otherwise, create a
    a new wxTableCell.

    Implement wxTableDataSource::GetSectionCount to return the number of sections
    in the table. This is relevant even for the plain style, since there can still
    be section names.

    wxTableDataSource::GetRowCount should return the number of rows in a given section.

    If wxTableDataSource::GetRowHeight returns a number greater than zero, the value is
    used for the height of the given row.

    To supply section titles, return an array of strings from wxTableDataSource::GetSectionTitles.

    If you're using the wxTC_PLAIN style you can return an array of strings for the index
    on the right hand side using wxTableDataSource::GetIndexTitles, with the same number of strings are there are sections.
    This will create an index that automatically takes the display to the start of the
    relevant section when index items are clicked.

    @category{wxbile}
*/

class WXDLLEXPORT wxTableCtrlBase: public wxWindow
{
public:
    /// Cell separator style.
    enum {
        CellSeparatorStyleNone,
        CellSeparatorStyleSingleLine
    };

    /// Row animation style.
    typedef enum {
        RowAnimationNone,
        RowAnimationFade,
        RowAnimationRight,
        RowAnimationLeft,
        RowAnimationTop,
        RowAnimationBottom
    } RowAnimationStyle;


    /// Default constructor.
    wxTableCtrlBase() { Init(); };

    /// Constructor.
    wxTableCtrlBase(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxButtonNameStr);

    virtual ~wxTableCtrlBase() { }

    /// Reloads the data. Resets the scroll position to the start
    /// if resetScrollbars is true, otherwise resets it to the nearest
    /// position to the previous one.
    virtual bool ReloadData(bool resetScrollbars = true) = 0;

    /// Loads the data within the specified rectangle.
    virtual bool LoadVisibleData(const wxRect& rect) = 0;

    /// Clears all data
    virtual void Clear() = 0;

    /// Starts batching of operations within which no visual updates are performed.
    /// You can nest Freeze/Thaw operations.
    virtual void Freeze() = 0;

    /// Ends batching of operations, refetches the data, and refreshes the window.
    virtual void Thaw() = 0;

    /// Returns true if Freeze has been called more times than Thaw.
    virtual bool IsFrozen() const { return m_freezeCount > 0; }

    /// Scroll to the given path (section/row)
    virtual bool ScrollToPath(const wxTablePath& path) = 0;

    /// Deletes the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool DeleteRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle) = 0;

    /// Inserts the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool InsertRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle) = 0;

    /// Deletes the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool DeleteSections(const wxArrayInt& sections, RowAnimationStyle animationStyle) = 0;

    /// Inserts the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool InsertSections(const wxArrayInt& sections, RowAnimationStyle animationStyle) = 0;

    /// Refreshes the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool ReloadSections(const wxArrayInt& sections, RowAnimationStyle animationStyle) = 0;

    /// Inserts the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool ReloadRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle) = 0;

    /// Sets editing mode (not yet implemented).
    virtual bool SetEditingMode(bool editingMode, bool animated = true) = 0;

    /// Returns true if in edit mode (not implemented).
    virtual bool IsEditing() const { return m_editingMode; }

    // Finds a cell that isn't being displayed currently,
    // either from the current pool or from an area of
    // the table that isn't currently showing.
    virtual wxTableCell* GetReusableCell(const wxString& reuseName) = 0;

    /// Gets the data source
    wxTableDataSource* GetDataSource() const { return m_dataSource; }

    /// Sets the data source
    void SetDataSource(wxTableDataSource* dataSource, bool ownsDataSource = true)
    {
        m_dataSource = dataSource;
        m_ownsDataSource = ownsDataSource;
    }

    /// Finds the path for the cell.
    virtual bool FindPathForCell(wxTableCell* cell, wxTablePath& path) const = 0;

    /// Finds the cell for the path.
    virtual wxTableCell* FindCellForPath(const wxTablePath& path) const = 0;

    /// Returns true if the control is plain.
    virtual bool IsPlain() const { return !HasFlag(wxTC_GROUPED); }

    /// Returns true if the control is grouped.
    virtual bool IsGrouped() const { return HasFlag(wxTC_GROUPED); }

    /// Sets the selection to the given path.
    virtual void SetSelection(const wxTablePath& path) = 0;

    /// Removes the selection at the given path.
    virtual void Deselect(const wxTablePath& path) = 0;

    /// Gets the selection, or NULL if there is none.
    virtual wxTablePath* GetSelection() const = 0;

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
    bool IsDeleteButtonShowing(const wxTablePath& path) const { return (m_deletePath == path); }

    /// Returns @true if row selection is allowed during editing. The default is @false.
    bool GetAllowSelectionDuringEditing() const { return m_allowsSelectionDuringEditing; }

    /// Pass @true to allow row selection during editing.
    void SetAllowSelectionDuringEditing(bool allow) { m_allowsSelectionDuringEditing = allow; }

// Implementation-only from here

    /*
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
    */

    /*
    virtual bool SetBackgroundColour(const wxColour &colour) = 0;
    virtual bool SetForegroundColour(const wxColour &colour) = 0;
    virtual bool SetFont(const wxFont& font) = 0;
    virtual bool Enable(bool enable) = 0;
    */
    
    /*
    virtual bool SetSectionFont(const wxFont& font);
    virtual wxFont GetSectionFont() const { return m_sectionFont; }
    */

    virtual bool HasIndex() const { return m_indexTitles.GetCount() > 0; }

    /*
    void DoShowDeleteButton(bool show, const wxTablePath& path);
    void ShowDeleteButton(const wxTablePath& path) { DoShowDeleteButton(true, path); }
    void HideDeleteButton() { DoShowDeleteButton(false, wxTablePath(-1, -1)); }
    wxTablePath GetDeletePath() const { return m_deletePath; }
    bool DeletingThisRow(const wxTablePath& path) const { return m_deletePath == path; }
    bool Deleting() const { return m_deletePath.IsValid(); }
    */

    // Load a bitmap from data
    /*
    wxBitmap LoadBitmap(const char* bitmapData, size_t len);
    wxBitmap LoadBitmap(unsigned char bitmapData[], size_t len) { return LoadBitmap((const char*) bitmapData, len); }
    wxBitmap LoadBitmap(const void *bitmapData, size_t len) { return LoadBitmap((const char*) bitmapData, len); }
    */
    
protected:
    
    void Init() { m_dataSource = NULL; }

    /*
    virtual wxSize DoGetBestSize() const;
    virtual void OnInternalIdle();

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxuseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTouchScrollDrag(wxTouchScrollEvent & event);
    void OnCancelTouch(wxTouchScrollEvent& event);
    void OnMouseCaptureChanged(wxuseCaptureChangedEvent& event);

    void OnAddClicked(wxTableCtrlEvent& event);
    void OnDeleteClicked(wxTableCtrlEvent& event);
    void OnConfirmDeleteClicked(wxTableCtrlEvent& event);
    void OnConfirmDeleteButtonClicked(wxCommandEvent& event);

    /// Hit test
    int HitTest(const wxPoint& pt, int& retSection, int& retRow, int& region) const;

    // Recalculates dimensions
    bool RecalculateDimensions();
    */

    wxTableCellArray        m_reusableCells;
    wxTableSectionArray     m_sections;
    wxTableDataSource*      m_dataSource;
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
    wxButton*               m_deleteButton;
    wxTablePath             m_deletePath;
    wxTablePath             m_insertAt; // for drawing insertion marker
    wxTablePath             m_originalInsertionPoint; // starting insertion point

private:

    wxDECLARE_NO_COPY_CLASS(wxTableCtrlBase);
};


#pragma mark wxTableDataSource

/**
    @class wxTableDataSource

    This class provides data for a table. Derive from the class
    and override the various functions to return section and row
    counts, row height, and actual cell data.

    For more information, see wxTableCtrl and wxTableCell.

    @category{wxbile}
*/

class WXDLLEXPORT wxTableDataSource: public wxEvtHandler
{
public:
    wxTableDataSource() { Init(); }

    ~wxTableDataSource() {}

    /// Returns a table cell for the give location. Call wxTableCtrl::GetReusableCell
    /// to check whether an off-screen cell can be reused, and create a new wxTableCel
    /// otherwise.
    virtual wxTableCell* GetCell(wxTableCtrl* ctrl, const wxTablePath& path) = 0;

    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxTableCtrl* ctrl) const = 0;

    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxTableCtrl* ctrl, int section) const = 0;

    /// Returns the height of the row at this location in pixels.
    virtual int GetRowHeight(wxTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) const { return 0; }

    /// Returns the height of the specified section header in pixels. If this returns -1, the table uses the
    /// value returned by wxTableCtrl::GetSectionHeaderHeight.
    virtual int GetSectionHeaderHeight(wxTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const { return -1; }

    /// Returns the height of the specified section footer in pixels. If this returns -1, the table uses the
    /// value returned by wxTableCtrl::GetSectionFooterHeight.
    virtual int GetSectionFooterHeight(wxTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const { return -1; }

    /// Returns the section titles.
    virtual wxArrayString GetSectionTitles(wxTableCtrl* WXUNUSED(ctrl)) const { return wxArrayString(); }

    /// Returns an array of strings, each one corresponding to an index title to be shown
    /// on the right hand side of a plain list.
    virtual wxArrayString GetIndexTitles(wxTableCtrl* WXUNUSED(ctrl)) const { return wxArrayString(); }

    /// Called by the table control to commit an insertion requested by the user. This function should
    /// then call InsertRows in response.
    virtual bool CommitInsertRow(wxTableCtrl* ctrl, const wxTablePath& path);

    /// Called by the table control to commit a deletion requested by the user. This function should
    /// then call DeleteRows in response.
    virtual bool CommitDeleteRow(wxTableCtrl* ctrl, const wxTablePath& path);

    /// Returns true if this row can be edited.
    virtual bool CanEditRow(wxTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) { return true; }

    /// Returns true if this row can be moved.
    virtual bool CanMoveRow(wxTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(path)) { return false; }

    /// Requests a move operation from 'from' to 'to'. The data should then reflect
    /// the new ordering. Return false if the reordering was not done.
    virtual bool MoveRow(wxTableCtrl* WXUNUSED(ctrl), const wxTablePath& WXUNUSED(pathFrom), const wxTablePath& WXUNUSED(pathTo)) { return false; }
    
    /// Override to intercept row click; by default, sends an event to itself and then to control.
    virtual bool OnSelectRow(wxTableCtrl* ctrl,
                             wxTablePath path) { }

    /// Override to intercept row accessory click; by default, sends an event to itself and then to control.
    virtual bool OnClickAccessory(wxTableCtrl* ctrl,
                                  wxTablePath path) { }

    DECLARE_CLASS(wxTableDataSource)
    
protected:
    void Init() {}
        
};


#pragma mark wxTableCtrlEvent

/**
    @class wxTableCtrlEvent

    Use this event for table controls.

    @category{wxbile}

*/

class WXDLLEXPORT wxTableCtrlEvent: public wxNotifyEvent
{
public:
    wxTableCtrlEvent() { m_tableCtrl = NULL; }
    wxTableCtrlEvent(const wxTableCtrlEvent& event): wxNotifyEvent(event)
    {
        m_tableCtrl = event.m_tableCtrl;
        m_path = event.m_path;
    }
    wxTableCtrlEvent(wxEventType commandType, int ctrlId, wxTableCtrl* ctrl)
        : wxNotifyEvent(commandType, ctrlId)
    {
         m_tableCtrl = ctrl;
    }

    /// Set the table control
    void SetSheet(wxTableCtrl* tableCtrl) { m_tableCtrl = tableCtrl; }

    /// Get the table control
    wxTableCtrl* GetTableCtrl() const { return m_tableCtrl; }

    /// Set the path
    void SetPath(const wxTablePath& path) { m_path = path; }

    /// Get the path
    const wxTablePath& GetPath() const { return m_path; }

    virtual wxEvent *Clone() const { return new wxTableCtrlEvent(*this); }

private:
    wxTablePath     m_path;
    wxTableCtrl*    m_tableCtrl;

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
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxTableCtrlEventFunction, &func)

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




// ----------------------------------------------------------------------------
// wxTableCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/tablectrl.h"
#endif


#endif
    // _WX_TABLECTRL_H_BASE_
