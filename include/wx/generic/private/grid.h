/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/grid.h
// Purpose:     Private wxGrid structures
// Author:      Michael Bedward (based on code by Julian Smart, Robin Dunn)
// Modified by: Santiago Palacios
// Created:     1/08/1999
// Copyright:   (c) Michael Bedward
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_GRID_PRIVATE_H_
#define _WX_GENERIC_GRID_PRIVATE_H_

#include "wx/defs.h"

#if wxUSE_GRID

#include "wx/headerctrl.h"

// for wxGridOperations
#include "wx/generic/gridsel.h"

// ----------------------------------------------------------------------------
// array classes
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY_WITH_DECL_PTR(wxGridCellAttr *, wxArrayAttrs,
                                 class WXDLLIMPEXP_ADV);

WX_DECLARE_HASH_MAP_WITH_DECL(wxLongLong_t, wxGridCellAttr*,
                              wxIntegerHash, wxIntegerEqual,
                              wxGridCoordsToAttrMap, class WXDLLIMPEXP_CORE);

// ----------------------------------------------------------------------------
// enumerations
// ----------------------------------------------------------------------------

// Flags to pass to wxGrid::RefreshArea()
enum wxGridArea
{
    wxGA_Corner      = 0x001, // m_cornerLabelWin
    wxGA_RowLabels   = 0x002, // m_rowLabelWin, [m_rowFrozenLabelWin]
    wxGA_ColLabels   = 0x004, // m_colLabelWin, [m_colFrozenLabelWin]
    wxGA_Cells       = 0x008, // m_gridwin, [m_frozenCornerGridWin,
                              // m_frozenColGridWin, m_frozenRowGridWin]

    wxGA_Heading     = wxGA_ColLabels | wxGA_Corner,
    wxGA_Labels      = wxGA_RowLabels | wxGA_Heading,
    wxGA_All         = wxGA_Cells | wxGA_Labels
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// header column providing access to the column information stored in wxGrid
// via wxHeaderColumn interface
class wxGridHeaderColumn : public wxHeaderColumn
{
public:
    wxGridHeaderColumn(wxGrid *grid, int col)
        : m_grid(grid),
          m_col(col)
    {
    }

    virtual wxString GetTitle() const override { return m_grid->GetColLabelValue(m_col); }
    virtual wxBitmap GetBitmap() const override { return wxNullBitmap; }
    virtual wxBitmapBundle GetBitmapBundle() const override { return wxBitmapBundle(); }
    virtual int GetWidth() const override { return m_grid->GetColSize(m_col); }
    virtual int GetMinWidth() const override { return m_grid->GetColMinimalWidth(m_col); }
    virtual wxAlignment GetAlignment() const override
    {
        int horz,
            vert;
        m_grid->GetColLabelAlignment(&horz, &vert);

        return static_cast<wxAlignment>(horz);
    }

    virtual int GetFlags() const override
    {
        // we can't know in advance whether we can sort by this column or not
        // with wxGrid API so suppose we can by default
        int flags = wxCOL_SORTABLE;
        if ( m_grid->CanDragColSize(m_col) )
            flags |= wxCOL_RESIZABLE;
        if ( m_grid->CanDragColMove() )
            flags |= wxCOL_REORDERABLE;
        if ( GetWidth() == 0 )
            flags |= wxCOL_HIDDEN;

        return flags;
    }

    virtual bool IsSortKey() const override
    {
        return m_grid->IsSortingBy(m_col);
    }

    virtual bool IsSortOrderAscending() const override
    {
        return m_grid->IsSortOrderAscending();
    }

private:
    // these really should be const but are not because the column needs to be
    // assignable to be used in a wxVector (in STL build, in non-STL build we
    // avoid the need for this)
    wxGrid *m_grid;
    int m_col;
};

// header control retrieving column information from the grid
class wxGridHeaderCtrl : public wxHeaderCtrl
{
public:
    wxGridHeaderCtrl(wxGrid *owner)
        : wxHeaderCtrl(owner,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       (owner->CanHideColumns() ? wxHD_ALLOW_HIDE : 0) |
                       (owner->CanDragColMove() ? wxHD_ALLOW_REORDER : 0))
    {
        m_inResizing = 0;
    }

    // Special method to call from wxGrid::DoSetColSize(), see comments there.
    void UpdateIfNotResizing(unsigned int idx)
    {
        if ( !m_inResizing )
            UpdateColumn(idx);
    }

protected:
    virtual const wxHeaderColumn& GetColumn(unsigned int idx) const override
    {
        return m_columns[idx];
    }

    wxGrid *GetOwner() const { return static_cast<wxGrid *>(GetParent()); }

private:
    wxMouseEvent GetDummyMouseEvent() const
    {
        // make up a dummy event for the grid event to use -- unfortunately we
        // can't do anything else here
        wxMouseEvent e;
        e.SetState(wxGetMouseState());
        GetOwner()->ScreenToClient(&e.m_x, &e.m_y);
        return e;
    }

    // override the base class method to update our m_columns array
    virtual void OnColumnCountChanging(unsigned int count) override
    {
        const unsigned countOld = m_columns.size();
        if ( count < countOld )
        {
            // just discard the columns which don't exist any more (notice that
            // we can't use resize() here as it would require the vector
            // value_type, i.e. wxGridHeaderColumn to be default constructible,
            // which it is not)
            m_columns.erase(m_columns.begin() + count, m_columns.end());
        }
        else // new columns added
        {
            // add columns for the new elements
            for ( unsigned n = countOld; n < count; n++ )
                m_columns.push_back(wxGridHeaderColumn(GetOwner(), n));
        }
    }

    // override to implement column auto sizing
    virtual bool UpdateColumnWidthToFit(unsigned int idx, int WXUNUSED(widthTitle)) override
    {
        GetOwner()->HandleColumnAutosize(idx, GetDummyMouseEvent());

        return true;
    }

    // overridden to react to the actions using the columns popup menu
    virtual void UpdateColumnVisibility(unsigned int idx, bool show) override
    {
        GetOwner()->SetColSize(idx, show ? wxGRID_AUTOSIZE : 0);

        // as this is done by the user we should notify the main program about
        // it
        GetOwner()->SendGridSizeEvent(wxEVT_GRID_COL_SIZE, idx,
                                      GetDummyMouseEvent());
    }

    // overridden to react to the columns order changes in the customization
    // dialog
    virtual void UpdateColumnsOrder(const wxArrayInt& order) override
    {
        GetOwner()->SetColumnsOrder(order);
    }


    // event handlers forwarding wxHeaderCtrl events to wxGrid
    void OnClick(wxHeaderCtrlEvent& event)
    {
        GetOwner()->SendEvent(wxEVT_GRID_LABEL_LEFT_CLICK,
                              -1, event.GetColumn(),
                              GetDummyMouseEvent());

        GetOwner()->DoColHeaderClick(event.GetColumn());
    }

    void OnDoubleClick(wxHeaderCtrlEvent& event)
    {
        if ( !GetOwner()->SendEvent(wxEVT_GRID_LABEL_LEFT_DCLICK,
                                    -1, event.GetColumn(),
                                    GetDummyMouseEvent()) )
        {
            event.Skip();
        }
    }

    void OnRightClick(wxHeaderCtrlEvent& event)
    {
        if ( !GetOwner()->SendEvent(wxEVT_GRID_LABEL_RIGHT_CLICK,
                                    -1, event.GetColumn(),
                                    GetDummyMouseEvent()) )
        {
            event.Skip();
        }
    }

    void OnBeginResize(wxHeaderCtrlEvent& event)
    {
        GetOwner()->DoHeaderStartDragResizeCol(event.GetColumn());

        event.Skip();
    }

    void OnResizing(wxHeaderCtrlEvent& event)
    {
        // Calling wxGrid method results in a call to our own UpdateColumn()
        // because it ends up in wxGrid::SetColSize() which must indeed update
        // the column when it's called by the program -- but in the case where
        // the size change comes from the column itself, it is useless and, in
        // fact, harmful, as it results in extra flicker due to the inefficient
        // implementation of UpdateColumn() in wxMSW wxHeaderCtrl, so skip
        // calling it from our overridden version by setting this flag for the
        // duration of this function execution and checking it in our
        // UpdateIfNotResizing().
        m_inResizing++;

        GetOwner()->DoHeaderDragResizeCol(event.GetWidth());

        m_inResizing--;
    }

    void OnEndResize(wxHeaderCtrlEvent& event)
    {
        GetOwner()->DoHeaderEndDragResizeCol(event.GetWidth());

        event.Skip();
    }

    void OnBeginReorder(wxHeaderCtrlEvent& event)
    {
        GetOwner()->DoStartMoveRowOrCol(event.GetColumn());
    }

    void OnEndReorder(wxHeaderCtrlEvent& event)
    {
        GetOwner()->DoEndMoveCol(event.GetNewOrder());
    }

    wxVector<wxGridHeaderColumn> m_columns;

    // The count of OnResizing() call nesting, 0 if not inside it.
    int m_inResizing;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxGridHeaderCtrl);
};

// common base class for various grid subwindows
class WXDLLIMPEXP_ADV wxGridSubwindow : public wxWindow
{
public:
    wxGridSubwindow(wxGrid *owner,
                    int additionalStyle = 0,
                    const wxString& name = wxASCII_STR(wxPanelNameStr))
        : wxWindow(owner, wxID_ANY,
                   wxDefaultPosition, wxDefaultSize,
                   wxBORDER_NONE | additionalStyle,
                   name)
    {
        m_owner = owner;
    }

    virtual wxWindow *GetMainWindowOfCompositeControl() override { return m_owner; }

    virtual bool AcceptsFocus() const override { return false; }

    wxGrid *GetOwner() { return m_owner; }

    virtual bool IsFrozen() const { return false; }

protected:
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

    wxGrid *m_owner;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxGridSubwindow);
};

class WXDLLIMPEXP_ADV wxGridRowLabelWindow : public wxGridSubwindow
{
public:
    wxGridRowLabelWindow(wxGrid *parent)
      : wxGridSubwindow(parent)
    {
    }

private:
    void OnPaint( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnMouseWheel( wxMouseEvent& event );

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxGridRowLabelWindow);
};


class wxGridRowFrozenLabelWindow : public wxGridRowLabelWindow
{
public:
    wxGridRowFrozenLabelWindow(wxGrid *parent)
        : wxGridRowLabelWindow(parent)
    {
    }

    virtual bool IsFrozen() const override { return true; }
};


class WXDLLIMPEXP_ADV wxGridColLabelWindow : public wxGridSubwindow
{
public:
    wxGridColLabelWindow(wxGrid *parent)
        : wxGridSubwindow(parent)
    {
    }

private:
    void OnPaint( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnMouseWheel( wxMouseEvent& event );

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxGridColLabelWindow);
};


class wxGridColFrozenLabelWindow : public wxGridColLabelWindow
{
public:
    wxGridColFrozenLabelWindow(wxGrid *parent)
        : wxGridColLabelWindow(parent)
    {
    }

    virtual bool IsFrozen() const override { return true; }
};


class WXDLLIMPEXP_ADV wxGridCornerLabelWindow : public wxGridSubwindow
{
public:
    wxGridCornerLabelWindow(wxGrid *parent)
        : wxGridSubwindow(parent)
    {
    }

private:
    void OnMouseEvent( wxMouseEvent& event );
    void OnMouseWheel( wxMouseEvent& event );
    void OnPaint( wxPaintEvent& event );

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxGridCornerLabelWindow);
};

class WXDLLIMPEXP_ADV wxGridWindow : public wxGridSubwindow
{
public:
    // grid window variants for scrolling possibilities
    enum wxGridWindowType
    {
        wxGridWindowNormal          = 0,
        wxGridWindowFrozenCol       = 1,
        wxGridWindowFrozenRow       = 2,
        wxGridWindowFrozenCorner    = wxGridWindowFrozenCol |
                                      wxGridWindowFrozenRow
    };

    wxGridWindow(wxGrid *parent, wxGridWindowType type)
        : wxGridSubwindow(parent,
                          wxWANTS_CHARS | wxCLIP_CHILDREN,
                          "GridWindow"),
          m_type(type)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
    }


    virtual void ScrollWindow( int dx, int dy, const wxRect *rect ) override;

    virtual bool AcceptsFocus() const override { return true; }

    wxGridWindowType GetType() const { return m_type; }

private:
    const wxGridWindowType m_type;

    void OnPaint( wxPaintEvent &event );
    void OnMouseWheel( wxMouseEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent& );
    void OnKeyUp( wxKeyEvent& );
    void OnChar( wxKeyEvent& );
    void OnFocus( wxFocusEvent& );

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxGridWindow);
};

// ----------------------------------------------------------------------------
// the internal data representation used by wxGridCellAttrProvider
// ----------------------------------------------------------------------------

// this class stores attributes set for cells
class WXDLLIMPEXP_ADV wxGridCellAttrData
{
public:
    ~wxGridCellAttrData();

    void SetAttr(wxGridCellAttr *attr, int row, int col);
    wxGridCellAttr *GetAttr(int row, int col) const;
    void UpdateAttrRows( size_t pos, int numRows );
    void UpdateAttrCols( size_t pos, int numCols );

private:
    // Tries to search for the attr for given cell.
    wxGridCoordsToAttrMap::iterator FindIndex(int row, int col) const;

    mutable wxGridCoordsToAttrMap m_attrs;
};

// this class stores attributes set for rows or columns
class WXDLLIMPEXP_ADV wxGridRowOrColAttrData
{
public:
    // empty ctor to suppress warnings
    wxGridRowOrColAttrData() {}
    ~wxGridRowOrColAttrData();

    void SetAttr(wxGridCellAttr *attr, int rowOrCol);
    wxGridCellAttr *GetAttr(int rowOrCol) const;
    void UpdateAttrRowsOrCols( size_t pos, int numRowsOrCols );

private:
    wxArrayInt m_rowsOrCols;
    wxArrayAttrs m_attrs;
};

// NB: this is just a wrapper around 3 objects: one which stores cell
//     attributes, and 2 others for row/col ones
class WXDLLIMPEXP_ADV wxGridCellAttrProviderData
{
public:
    wxGridCellAttrData m_cellAttrs;
    wxGridRowOrColAttrData m_rowAttrs,
                           m_colAttrs;
};

// ----------------------------------------------------------------------------
// operations classes abstracting the difference between operating on rows and
// columns
// ----------------------------------------------------------------------------

// This class allows to write a function only once because by using its methods
// it will apply to both columns and rows.
//
// This is an abstract interface definition, the two concrete implementations
// below should be used when working with rows and columns respectively.
class wxGridOperations
{
public:
    // Returns the operations in the other direction, i.e. wxGridRowOperations
    // if this object is a wxGridColumnOperations and vice versa.
    virtual wxGridOperations& Dual() const = 0;

    // returns wxHORIZONTAL or wxVERTICAL for row/col operations
    virtual int GetOrientation() const = 0;

    // return row/col specific cursor modes
    virtual wxGrid::CursorMode GetCursorModeResize() const = 0;
    virtual wxGrid::CursorMode GetCursorModeSelect() const = 0;
    virtual wxGrid::CursorMode GetCursorModeMove() const = 0;

    // Return the total number of rows or columns.
    virtual int GetTotalNumberOfLines(const wxGrid *grid) const = 0;

    // Return the current number of rows or columns of a grid window.
    virtual int GetNumberOfLines(const wxGrid *grid, wxGridWindow *gridWindow) const = 0;

    // Return the first line for this grid type.
    virtual int GetFirstLine(const wxGrid *grid, wxGridWindow *gridWindow) const = 0;

    // Return the selection mode which allows selecting rows or columns.
    virtual wxGrid::wxGridSelectionModes GetSelectionMode() const = 0;

    // Make a wxGridCellCoords from the given components: thisDir is row or
    // column and otherDir is column or row
    virtual wxGridCellCoords MakeCoords(int thisDir, int otherDir) const = 0;

    // Calculate the scrolled position of the given abscissa or ordinate.
    virtual int CalcScrolledPosition(wxGrid *grid, int pos) const = 0;

    // Selects the horizontal or vertical component from the given object.
    virtual int Select(const wxGridCellCoords& coords) const = 0;
    virtual int Select(const wxPoint& pt) const = 0;
    virtual int Select(const wxSize& sz) const = 0;
    virtual int Select(const wxRect& r) const = 0;
    virtual int& Select(wxRect& r) const = 0;

    // Return or set left/top or right/bottom component of a block.
    virtual int SelectFirst(const wxGridBlockCoords& block) const = 0;
    virtual int SelectLast(const wxGridBlockCoords& block) const = 0;
    virtual void SetFirst(wxGridBlockCoords& block, int line) const = 0;
    virtual void SetLast(wxGridBlockCoords& block, int line) const = 0;

    // Returns width or height of the rectangle
    virtual int& SelectSize(wxRect& r) const = 0;

    // Make a wxSize such that Select() applied to it returns first component
    virtual wxSize MakeSize(int first, int second) const = 0;

    // Sets the row or column component of the given cell coordinates
    virtual void Set(wxGridCellCoords& coords, int line) const = 0;


    // Draws a line parallel to the row or column, i.e. horizontal or vertical:
    // pos is the horizontal or vertical position of the line and start and end
    // are the coordinates of the line extremities in the other direction
    virtual void
        DrawParallelLine(wxDC& dc, int start, int end, int pos) const = 0;

    // Draw a horizontal or vertical line across the given rectangle
    // (this is implemented in terms of above and uses Select() to extract
    // start and end from the given rectangle)
    void DrawParallelLineInRect(wxDC& dc, const wxRect& rect, int pos) const
    {
        const int posStart = Select(rect.GetPosition());
        DrawParallelLine(dc, posStart, posStart + Select(rect.GetSize()), pos);
    }


    // Return the index of the row or column at the given pixel coordinate.
    virtual int
        PosToLine(const wxGrid *grid, int pos, wxGridWindow *gridWindow, bool clip = false) const = 0;

    // Get the top/left position, in pixels, of the given row or column
    virtual int GetLineStartPos(const wxGrid *grid, int line) const = 0;

    // Get the bottom/right position, in pixels, of the given row or column
    virtual int GetLineEndPos(const wxGrid *grid, int line) const = 0;

    // Get the height/width of the given row/column
    virtual int GetLineSize(const wxGrid *grid, int line) const = 0;

    // Get wxGrid::m_rowBottoms/m_colRights array
    virtual const wxArrayInt& GetLineEnds(const wxGrid *grid) const = 0;

    // Get default height row height or column width
    virtual int GetDefaultLineSize(const wxGrid *grid) const = 0;

    // Return the minimal acceptable row height or column width
    virtual int GetMinimalAcceptableLineSize(const wxGrid *grid) const = 0;

    // Return the minimal row height or column width
    virtual int GetMinimalLineSize(const wxGrid *grid, int line) const = 0;

    // Set the row height or column width
    virtual void SetLineSize(wxGrid *grid, int line, int size) const = 0;

    // Set the row default height or column default width
    virtual void SetDefaultLineSize(wxGrid *grid, int size, bool resizeExisting) const = 0;

    // auto size the row height or column width from the label content
    virtual void HandleLineAutosize(wxGrid *grid, int line, const wxMouseEvent& event) const = 0;

    // Return the index of the line at the given position
    virtual int GetLineAt(const wxGrid *grid, int pos) const = 0;

    // Return the display position of the line with the given index.
    virtual int GetLinePos(const wxGrid *grid, int line) const = 0;

    // Return the index of the line just before the given one or wxNOT_FOUND.
    virtual int GetLineBefore(const wxGrid* grid, int line) const = 0;

    // Get the row or column label window
    virtual wxWindow *GetHeaderWindow(wxGrid *grid) const = 0;

    // Get the width or height of the row or column label window
    virtual int GetHeaderWindowSize(wxGrid *grid) const = 0;

    // Get the row or column frozen grid window
    virtual wxGridWindow *GetFrozenGrid(wxGrid* grid) const = 0;


    // return the value of m_canDragRow/ColMove
    virtual bool CanDragMove(wxGrid *grid) const = 0;

    // call DoEndMoveRow or DoEndMoveColumn
    virtual void DoEndMove(wxGrid* grid, int line) const = 0;

    // return whether the given row/column can be interactively resized
    virtual bool CanDragLineSize(wxGrid *grid, int line) const = 0;

    // call DoEndDragResizeRow or DoEndDragResizeCol
    virtual void DoEndLineResize(wxGrid *grid, const wxMouseEvent& event, wxGridWindow* gridWindow) const = 0;


    // extend current selection block up to given row or column
    virtual bool SelectionExtendCurrentBlock(wxGrid *grid, int line,
        const wxMouseEvent &event,
        wxEventType eventType = wxEVT_GRID_RANGE_SELECTED) const = 0;

    // select or de-select a row or column
    virtual void SelectLine(wxGrid *grid, int line, wxMouseEvent &event) const = 0;
    virtual void DeselectLine(wxGrid * grid, int line) const = 0;

    // check whether the row or columns first cell is in selected
    virtual bool IsLineInSelection(wxGrid *grid, int line) const = 0;


    // sent a result with row or column and the other value -1
    virtual wxGrid::EventResult SendEvent(wxGrid *grid, wxEventType eventType,
        int line, const wxMouseEvent& event) const = 0;

    // call DrawRowLabel or DrawColumnLabel
    virtual void DrawLineLabel(wxGrid *grid, wxDC& dc, int line) const = 0;

    // helper for Process...MouseEvent for drawing markers on label windows
    void PrepareDCForLabels(wxGrid *grid, wxDC &dc) const;

    // make the specified line visible by doing a minimal amount of scrolling
    virtual void MakeLineVisible(wxGrid *grid, int line) const = 0;

    // set cursor into the first visible cell of the given row or column
    virtual void MakeLineCurrent(wxGrid *grid, int line) const = 0;


    // This class is never used polymorphically but give it a virtual dtor
    // anyhow to suppress g++ complaints about it
    virtual ~wxGridOperations() { }
};

class wxGridRowOperations : public wxGridOperations
{
public:
    virtual wxGridOperations& Dual() const override;

    virtual int GetOrientation() const override
        { return wxVERTICAL; }

    virtual wxGrid::CursorMode GetCursorModeResize() const override
        { return wxGrid::WXGRID_CURSOR_RESIZE_ROW; }
    virtual wxGrid::CursorMode GetCursorModeSelect() const override
        { return wxGrid::WXGRID_CURSOR_SELECT_ROW; }
    virtual wxGrid::CursorMode GetCursorModeMove() const override
        { return wxGrid::WXGRID_CURSOR_MOVE_ROW; }

    virtual int GetTotalNumberOfLines(const wxGrid *grid) const override
        { return grid->GetNumberRows(); }

    virtual int GetNumberOfLines(const wxGrid *grid, wxGridWindow *gridWindow) const override;

    virtual int GetFirstLine(const wxGrid *grid, wxGridWindow *gridWindow) const override;

    virtual wxGrid::wxGridSelectionModes GetSelectionMode() const override
        { return wxGrid::wxGridSelectRows; }

    virtual wxGridCellCoords MakeCoords(int thisDir, int otherDir) const override
        { return wxGridCellCoords(thisDir, otherDir); }

    virtual int CalcScrolledPosition(wxGrid *grid, int pos) const override
        { return grid->CalcScrolledPosition(wxPoint(pos, 0)).x; }

    virtual int Select(const wxGridCellCoords& c) const override { return c.GetRow(); }
    virtual int Select(const wxPoint& pt) const override { return pt.x; }
    virtual int Select(const wxSize& sz) const override { return sz.x; }
    virtual int Select(const wxRect& r) const override { return r.x; }
    virtual int& Select(wxRect& r) const override { return r.x; }
    virtual int SelectFirst(const wxGridBlockCoords& block) const override
        { return block.GetTopRow(); }
    virtual int SelectLast(const wxGridBlockCoords& block) const override
        { return block.GetBottomRow(); }
    virtual void SetFirst(wxGridBlockCoords& block, int line) const override
        { block.SetTopRow(line); }
    virtual void SetLast(wxGridBlockCoords& block, int line) const override
        { block.SetBottomRow(line); }
    virtual int& SelectSize(wxRect& r) const override { return r.width; }
    virtual wxSize MakeSize(int first, int second) const override
        { return wxSize(first, second); }
    virtual void Set(wxGridCellCoords& coords, int line) const override
        { coords.SetRow(line); }

    virtual void DrawParallelLine(wxDC& dc, int start, int end, int pos) const override
        { dc.DrawLine(start, pos, end, pos); }

    virtual int PosToLine(const wxGrid *grid, int pos, wxGridWindow *gridWindow , bool clip = false) const override
        { return grid->YToRow(pos, clip, gridWindow); }
    virtual int GetLineStartPos(const wxGrid *grid, int line) const override
        { return grid->GetRowTop(line); }
    virtual int GetLineEndPos(const wxGrid *grid, int line) const override
        { return grid->GetRowBottom(line); }
    virtual int GetLineSize(const wxGrid *grid, int line) const override
        { return grid->GetRowHeight(line); }
    virtual const wxArrayInt& GetLineEnds(const wxGrid *grid) const override
        { return grid->m_rowBottoms; }
    virtual int GetDefaultLineSize(const wxGrid *grid) const override
        { return grid->GetDefaultRowSize(); }
    virtual int GetMinimalAcceptableLineSize(const wxGrid *grid) const override
        { return grid->GetRowMinimalAcceptableHeight(); }
    virtual int GetMinimalLineSize(const wxGrid *grid, int line) const override
        { return grid->GetRowMinimalHeight(line); }
    virtual void SetLineSize(wxGrid *grid, int line, int size) const override
        { grid->SetRowSize(line, size); }
    virtual void SetDefaultLineSize(wxGrid *grid, int size, bool resizeExisting) const override
        {  grid->SetDefaultRowSize(size, resizeExisting); }
    virtual void HandleLineAutosize(wxGrid *grid, int line, const wxMouseEvent& event) const override
        {  grid->HandleRowAutosize(line, event); }

    virtual int GetLineAt(const wxGrid *grid, int pos) const override
        { return grid->GetRowAt(pos); }
    virtual int GetLinePos(const wxGrid *grid, int line) const override
        { return grid->GetRowPos(line); }

    virtual int GetLineBefore(const wxGrid *grid, int line) const override
    {
        int posBefore = grid->GetRowPos(line) - 1;
        return posBefore >= 0 ? grid->GetRowAt(posBefore) : wxNOT_FOUND;
    }

    virtual wxWindow *GetHeaderWindow(wxGrid *grid) const override
        { return grid->GetGridRowLabelWindow(); }
    virtual int GetHeaderWindowSize(wxGrid *grid) const override
        { return grid->GetRowLabelSize(); }

    virtual wxGridWindow *GetFrozenGrid(wxGrid* grid) const override
        { return (wxGridWindow*)grid->GetFrozenRowGridWindow(); }

    virtual bool CanDragMove(wxGrid *grid) const override
        { return grid->m_canDragRowMove; }
    virtual void DoEndMove(wxGrid* grid, int line) const override
        { grid->DoEndMoveRow(line); }

    virtual bool CanDragLineSize(wxGrid *grid, int line) const override
        { return grid->CanDragRowSize(line); }
    virtual void DoEndLineResize(wxGrid *grid, const wxMouseEvent& event,
        wxGridWindow* gridWindow) const override
        { grid->DoEndDragResizeRow(event, gridWindow); }


    virtual bool SelectionExtendCurrentBlock(wxGrid *grid, int line,
        const wxMouseEvent &event,
        wxEventType eventType = wxEVT_GRID_RANGE_SELECTED) const override
        {
            return grid->m_selection->ExtendCurrentBlock
                (
                    wxGridCellCoords(grid->m_currentCellCoords.GetRow(), 0),
                    wxGridCellCoords(line, grid->GetNumberCols() - 1),
                    event,
                    eventType
                );
        }

    virtual void SelectLine(wxGrid *grid, int line, wxMouseEvent &event) const override
        { grid->m_selection->SelectRow(line, event); };
    virtual void DeselectLine(wxGrid * grid, int line) const override
        { grid->DeselectRow(line); }

    virtual bool IsLineInSelection(wxGrid *grid, int line) const override
        { return grid->m_selection->IsInSelection(line, 0); }

    virtual wxGrid::EventResult SendEvent(wxGrid *grid, wxEventType eventType,
        int line, const wxMouseEvent& event) const override
        { return grid->SendEvent(eventType, line, -1, event ); }

    virtual void DrawLineLabel(wxGrid *grid, wxDC& dc, int line) const override
        { grid->DrawRowLabel(dc, line); }

    virtual void MakeLineVisible(wxGrid *grid, int line) const override
        { grid->MakeCellVisible(line, -1); }
    virtual void MakeLineCurrent(wxGrid *grid, int line) const override
        { grid->SetCurrentCell(line, grid->GetFirstFullyVisibleColumn()); }

};

class wxGridColumnOperations : public wxGridOperations
{

public:
    virtual wxGridOperations& Dual() const override;

    virtual int GetOrientation() const override
        { return wxHORIZONTAL; }

    virtual wxGrid::CursorMode GetCursorModeResize() const override
        { return wxGrid::WXGRID_CURSOR_RESIZE_COL; }
    virtual wxGrid::CursorMode GetCursorModeSelect() const override
        { return wxGrid::WXGRID_CURSOR_SELECT_COL; }
    virtual wxGrid::CursorMode GetCursorModeMove() const override
        { return wxGrid::WXGRID_CURSOR_MOVE_COL; }

    virtual int GetTotalNumberOfLines(const wxGrid *grid) const override
        { return grid->GetNumberCols(); }

    virtual int GetNumberOfLines(const wxGrid *grid, wxGridWindow *gridWindow) const override;

    virtual int GetFirstLine(const wxGrid *grid, wxGridWindow *gridWindow) const override;

    virtual wxGrid::wxGridSelectionModes GetSelectionMode() const override
        { return wxGrid::wxGridSelectColumns; }

    virtual wxGridCellCoords MakeCoords(int thisDir, int otherDir) const override
        { return wxGridCellCoords(otherDir, thisDir); }

    virtual int CalcScrolledPosition(wxGrid *grid, int pos) const override
        { return grid->CalcScrolledPosition(wxPoint(0, pos)).y; }

    virtual int Select(const wxGridCellCoords& c) const override { return c.GetCol(); }
    virtual int Select(const wxPoint& pt) const override { return pt.y; }
    virtual int Select(const wxSize& sz) const override { return sz.y; }
    virtual int Select(const wxRect& r) const override { return r.y; }
    virtual int& Select(wxRect& r) const override { return r.y; }
    virtual int SelectFirst(const wxGridBlockCoords& block) const override
        { return block.GetLeftCol(); }
    virtual int SelectLast(const wxGridBlockCoords& block) const override
        { return block.GetRightCol(); }
    virtual void SetFirst(wxGridBlockCoords& block, int line) const override
        { block.SetLeftCol(line); }
    virtual void SetLast(wxGridBlockCoords& block, int line) const override
        { block.SetRightCol(line); }
    virtual int& SelectSize(wxRect& r) const override { return r.height; }
    virtual wxSize MakeSize(int first, int second) const override
        { return wxSize(second, first); }
    virtual void Set(wxGridCellCoords& coords, int line) const override
        { coords.SetCol(line); }

    virtual void DrawParallelLine(wxDC& dc, int start, int end, int pos) const override
        { dc.DrawLine(pos, start, pos, end); }

    virtual int PosToLine(const wxGrid *grid, int pos, wxGridWindow *gridWindow, bool clip = false) const override
        { return grid->XToCol(pos, clip, gridWindow); }
    virtual int GetLineStartPos(const wxGrid *grid, int line) const override
        { return grid->GetColLeft(line); }
    virtual int GetLineEndPos(const wxGrid *grid, int line) const override
        { return grid->GetColRight(line); }
    virtual int GetLineSize(const wxGrid *grid, int line) const override
        { return grid->GetColWidth(line); }
    virtual const wxArrayInt& GetLineEnds(const wxGrid *grid) const override
        { return grid->m_colRights; }
    virtual int GetDefaultLineSize(const wxGrid *grid) const override
        { return grid->GetDefaultColSize(); }
    virtual int GetMinimalAcceptableLineSize(const wxGrid *grid) const override
        { return grid->GetColMinimalAcceptableWidth(); }
    virtual int GetMinimalLineSize(const wxGrid *grid, int line) const override
        { return grid->GetColMinimalWidth(line); }
    virtual void SetLineSize(wxGrid *grid, int line, int size) const override
        { grid->SetColSize(line, size); }
    virtual void SetDefaultLineSize(wxGrid *grid, int size, bool resizeExisting) const override
        {  grid->SetDefaultColSize(size, resizeExisting); }
    virtual void HandleLineAutosize(wxGrid *grid, int line, const wxMouseEvent& event) const override
        {  grid->HandleColumnAutosize(line, event); }

    virtual int GetLineAt(const wxGrid *grid, int pos) const override
        { return grid->GetColAt(pos); }
    virtual int GetLinePos(const wxGrid *grid, int line) const override
        { return grid->GetColPos(line); }

    virtual int GetLineBefore(const wxGrid* grid, int line) const override
    {
        int posBefore = grid->GetColPos(line) - 1;
        return posBefore >= 0 ? grid->GetColAt(posBefore) : wxNOT_FOUND;
    }

    virtual wxWindow *GetHeaderWindow(wxGrid *grid) const override
        { return grid->GetGridColLabelWindow(); }
    virtual int GetHeaderWindowSize(wxGrid *grid) const override
        { return grid->GetColLabelSize(); }

    virtual wxGridWindow *GetFrozenGrid(wxGrid* grid) const override
        { return (wxGridWindow*)grid->GetFrozenColGridWindow(); }

    virtual bool CanDragMove(wxGrid *grid) const override
        { return grid->m_canDragColMove; }
    virtual void DoEndMove(wxGrid* grid, int line) const override
        { grid->DoEndMoveCol(line); }

    virtual bool CanDragLineSize(wxGrid *grid, int line) const override
        { return grid->CanDragColSize(line); }
    virtual void DoEndLineResize(wxGrid *grid, const wxMouseEvent& event,
        wxGridWindow* gridWindow) const override
        { grid->DoEndDragResizeCol(event, gridWindow); }

    virtual bool SelectionExtendCurrentBlock(wxGrid *grid, int line,
        const wxMouseEvent &event,
        wxEventType eventType = wxEVT_GRID_RANGE_SELECTED) const override
        {
            return grid->m_selection->ExtendCurrentBlock
                (
                    wxGridCellCoords(0, grid->m_currentCellCoords.GetCol()),
                    wxGridCellCoords(grid->GetNumberRows() - 1, line),
                    event,
                    eventType
                );
        }

    virtual void SelectLine(wxGrid *grid, int line, wxMouseEvent &event) const override
        { grid->m_selection->SelectCol(line, event); };
    virtual void DeselectLine(wxGrid * grid, int line) const override
        { grid->DeselectCol(line); }

    virtual bool IsLineInSelection(wxGrid *grid, int line) const override
        { return grid->m_selection->IsInSelection(line, 0); }

    virtual wxGrid::EventResult SendEvent(wxGrid *grid, wxEventType eventType,
        int line, const wxMouseEvent& event) const override
        { return grid->SendEvent(eventType, -1, line, event ); }

    virtual void DrawLineLabel(wxGrid *grid, wxDC& dc, int line) const override
        { grid->DrawColLabel(dc, line); }

    virtual void MakeLineVisible(wxGrid *grid, int line) const override
        { grid->MakeCellVisible(-1, line); }
    virtual void MakeLineCurrent(wxGrid *grid, int line) const override
        { grid->SetCurrentCell(grid->GetFirstFullyVisibleRow(), line); }

};

// This class abstracts the difference between operations going forward
// (down/right) and backward (up/left) and allows to use the same code for
// functions which differ only in the direction of grid traversal.
//
// Notice that all operations in this class work with display positions and not
// internal indices which can be different if the columns were reordered.
//
// Like wxGridOperations it's an ABC with two concrete subclasses below. Unlike
// it, this is a normal object and not just a function dispatch table and has a
// non-default ctor.
//
// Note: the explanation of this discrepancy is the existence of (very useful)
// Dual() method in wxGridOperations which forces us to make wxGridOperations a
// function dispatcher only.
class wxGridDirectionOperations
{
public:
    // The oper parameter to ctor selects whether we work with rows or columns
    wxGridDirectionOperations(wxGrid *grid, const wxGridOperations& oper)
        : m_grid(grid),
          m_oper(oper)
    {
    }

    // Check if the component of this point in our direction is at the
    // boundary, i.e. is the first/last row/column
    virtual bool IsAtBoundary(const wxGridCellCoords& coords) const = 0;

    // Check if the component of this point in our direction is
    // valid, i.e. not -1
    bool IsValid(const wxGridCellCoords& coords) const
    {
        return m_oper.Select(coords) != -1;
    }

    // Make the coordinates with the other component value of -1.
    wxGridCellCoords MakeWholeLineCoords(const wxGridCellCoords& coords) const
    {
        return m_oper.MakeCoords(m_oper.Select(coords), -1);
    }

    // Increment the component of this point in our direction
    //
    // Note that this can't be called if IsAtBoundary() is true, use
    // TryToAdvance() if this might be the case.
    virtual void Advance(wxGridCellCoords& coords) const = 0;

    // Try to advance in our direction, return true if succeeded or false
    // otherwise, i.e. if the coordinates are already at the grid boundary.
    bool TryToAdvance(wxGridCellCoords& coords) const
    {
        if ( IsAtBoundary(coords) )
            return false;

        Advance(coords);

        return true;
    }

    // Find the line at the given distance, in pixels, away from this one
    // (this uses clipping, i.e. anything after the last line is counted as the
    // last one and anything before the first one as 0)
    //
    // TODO: Implementation of this method currently doesn't support column
    //       reordering as it mixes up indices and positions. But this doesn't
    //       really matter as it's only called for rows (Page Up/Down only work
    //       vertically) and row reordering is not currently supported. We'd
    //       need to fix it if this ever changes however.
    virtual int MoveByPixelDistance(int line, int distance) const = 0;

    // This class is never used polymorphically but give it a virtual dtor
    // anyhow to suppress g++ complaints about it
    virtual ~wxGridDirectionOperations() { }

protected:
    // Get the position of the row or column from the given coordinates pair.
    //
    // This is just a shortcut to avoid repeating m_oper and m_grid multiple
    // times in the derived classes code.
    int GetLinePos(const wxGridCellCoords& coords) const
    {
        return m_oper.GetLinePos(m_grid, m_oper.Select(coords));
    }

    // Get the index of the row or column from the position.
    int GetLineAt(int pos) const
    {
        return m_oper.GetLineAt(m_grid, pos);
    }

    // Check if the given line is visible, i.e. has non 0 size.
    bool IsLineVisible(int line) const
    {
        return m_oper.GetLineSize(m_grid, line) != 0;
    }


    wxGrid * const m_grid;
    const wxGridOperations& m_oper;
};

class wxGridBackwardOperations : public wxGridDirectionOperations
{
public:
    wxGridBackwardOperations(wxGrid *grid, const wxGridOperations& oper)
        : wxGridDirectionOperations(grid, oper)
    {
    }

    virtual bool IsAtBoundary(const wxGridCellCoords& coords) const override
    {
        wxASSERT_MSG( m_oper.Select(coords) >= 0, "invalid row/column" );

        int pos = GetLinePos(coords);
        while ( pos )
        {
            // Check the previous line.
            int line = GetLineAt(--pos);
            if ( IsLineVisible(line) )
            {
                // There is another visible line before this one, hence it's
                // not at boundary.
                return false;
            }
        }

        // We reached the boundary without finding any visible lines.
        return true;
    }

    virtual void Advance(wxGridCellCoords& coords) const override
    {
        int pos = GetLinePos(coords);
        for ( ;; )
        {
            // This is not supposed to happen if IsAtBoundary() returned false.
            wxCHECK_RET( pos, "can't advance when already at boundary" );

            int line = GetLineAt(--pos);
            if ( IsLineVisible(line) )
            {
                m_oper.Set(coords, line);
                break;
            }
        }
    }

    virtual int MoveByPixelDistance(int line, int distance) const override
    {
        int pos = m_oper.GetLineStartPos(m_grid, line);
        return m_oper.PosToLine(m_grid, pos - distance + 1, nullptr, true);
    }
};

// Please refer to the comments above when reading this class code, it's
// absolutely symmetrical to wxGridBackwardOperations.
class wxGridForwardOperations : public wxGridDirectionOperations
{
public:
    wxGridForwardOperations(wxGrid *grid, const wxGridOperations& oper)
        : wxGridDirectionOperations(grid, oper),
          m_numLines(oper.GetTotalNumberOfLines(grid))
    {
    }

    virtual bool IsAtBoundary(const wxGridCellCoords& coords) const override
    {
        wxASSERT_MSG( m_oper.Select(coords) < m_numLines, "invalid row/column" );

        int pos = GetLinePos(coords);
        while ( pos < m_numLines - 1 )
        {
            int line = GetLineAt(++pos);
            if ( IsLineVisible(line) )
                return false;
        }

        return true;
    }

    virtual void Advance(wxGridCellCoords& coords) const override
    {
        int pos = GetLinePos(coords);
        for ( ;; )
        {
            wxCHECK_RET( pos < m_numLines - 1,
                         "can't advance when already at boundary" );

            int line = GetLineAt(++pos);
            if ( IsLineVisible(line) )
            {
                m_oper.Set(coords, line);
                break;
            }
        }
    }

    virtual int MoveByPixelDistance(int line, int distance) const override
    {
        int pos = m_oper.GetLineStartPos(m_grid, line);
        return m_oper.PosToLine(m_grid, pos + distance, nullptr, true);
    }

private:
    const int m_numLines;
};

// ----------------------------------------------------------------------------
// data structures used for the data type registry
// ----------------------------------------------------------------------------

struct wxGridDataTypeInfo
{
    wxGridDataTypeInfo(const wxString& typeName,
                       wxGridCellRenderer* renderer,
                       wxGridCellEditor* editor)
        : m_typeName(typeName), m_renderer(renderer), m_editor(editor)
        {}

    ~wxGridDataTypeInfo()
    {
        wxSafeDecRef(m_renderer);
        wxSafeDecRef(m_editor);
    }

    wxString            m_typeName;
    wxGridCellRenderer* m_renderer;
    wxGridCellEditor*   m_editor;

    wxDECLARE_NO_COPY_CLASS(wxGridDataTypeInfo);
};


WX_DEFINE_ARRAY_WITH_DECL_PTR(wxGridDataTypeInfo*, wxGridDataTypeInfoArray,
                                 class WXDLLIMPEXP_ADV);


class WXDLLIMPEXP_ADV wxGridTypeRegistry
{
public:
    wxGridTypeRegistry() {}
    ~wxGridTypeRegistry();

    void RegisterDataType(const wxString& typeName,
                     wxGridCellRenderer* renderer,
                     wxGridCellEditor* editor);

    // find one of already registered data types
    int FindRegisteredDataType(const wxString& typeName);

    // try to FindRegisteredDataType(), if this fails and typeName is one of
    // standard typenames, register it and return its index
    int FindDataType(const wxString& typeName);

    // try to FindDataType(), if it fails see if it is not one of already
    // registered data types with some params in which case clone the
    // registered data type and set params for it
    int FindOrCloneDataType(const wxString& typeName);

    wxGridCellRenderer* GetRenderer(int index);
    wxGridCellEditor*   GetEditor(int index);

private:
    wxGridDataTypeInfoArray m_typeinfo;
};

// Returns the rectangle for showing something of the given size in a cell with
// the given alignment.
//
// The function is used by wxGridCellBoolEditor and wxGridCellBoolRenderer to
// draw a check mark and position wxCheckBox respectively.
wxRect
wxGetContentRect(wxSize contentSize,
                 const wxRect& cellRect,
                 int hAlign,
                 int vAlign);

namespace wxGridPrivate
{

#if wxUSE_DATETIME

// This is used as TryGetValueAsDate() parameter.
class DateParseParams
{
public:
    // Unfortunately we have to provide the default ctor (and also make the
    // members non-const) because we use these objects as out-parameters as
    // they are not fully declared in the public headers. The factory functions
    // below must be used to create a really usable object.
    DateParseParams() : fallbackParseDate(false) { }

    // Use these functions to really initialize the object.
    static DateParseParams WithFallback(const wxString& format)
    {
        return DateParseParams(format, true);
    }

    static DateParseParams WithoutFallback(const wxString& format)
    {
        return DateParseParams(format, false);
    }

    // The usual format, e.g. "%x" or "%Y-%m-%d".
    wxString format;

    // Whether fall back to ParseDate() is allowed.
    bool fallbackParseDate;

private:
    DateParseParams(const wxString& format_, bool fallbackParseDate_)
        : format(format_),
          fallbackParseDate(fallbackParseDate_)
    {
    }
};

// Helper function trying to get a date from the given cell: if possible, get
// the date value from the table directly, otherwise get the string value for
// this cell and try to parse it using the specified date format and, if this
// doesn't work and fallbackParseDate is true, try using ParseDate() as a
// fallback. If this still fails, returns false.
bool
TryGetValueAsDate(wxDateTime& result,
                  const DateParseParams& params,
                  const wxGrid& grid,
                  int row, int col);

#endif // wxUSE_DATETIME

} // namespace wxGridPrivate

#endif // wxUSE_GRID
#endif // _WX_GENERIC_GRID_PRIVATE_H_
