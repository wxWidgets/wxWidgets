/////////////////////////////////////////////////////////////////////////////
// Name:        gridg.h
// Purpose:     wxGenericGrid
// Author:      Julian Smart
// Modified by: Michael Bedward
//                Added edit in place facility, 20 April 1999
//                Added cursor key control, 29 Jun 1999
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GRIDH_G__
#define __GRIDH_G__

#ifdef __GNUG__
#pragma interface "gridg.h"
#endif

#include "wx/defs.h"
#include "wx/panel.h"
#include "wx/string.h"
#include "wx/scrolbar.h"
#include "wx/event.h"

#define wxGRID_DEFAULT_EDIT_WIDTH 300
#define wxGRID_DEFAULT_EDIT_HEIGHT 27
#define wxGRID_DEFAULT_EDIT_X 2
#define wxGRID_DEFAULT_EDIT_Y 1
#define wxGRID_DEFAULT_SHEET_TOP 31
#define wxGRID_DEFAULT_SHEET_LEFT 0
#define wxGRID_DEFAULT_CELL_HEIGHT 20
#define wxGRID_DEFAULT_CELL_WIDTH 80
#define wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH 40
#define wxGRID_DEFAULT_HORIZONAL_LABEL_HEIGHT 20

#define WXGENERIC_GRID_VERSION  0.5

class WXDLLEXPORT wxGridEvent;
class WXDLLEXPORT wxGridCell;

class WXDLLEXPORT wxGenericGrid : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxGenericGrid)

public:
    wxGenericGrid();

    wxGenericGrid(wxWindow *parent, int x, int y, int width, int height, long style = 0, char *name = "grid")
    {
        Create(parent, -1, wxPoint(x, y), wxSize(width, height), style, name);
    }
    wxGenericGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style = 0, const wxString& name = "grid")
    {
        Create(parent, id, pos, size, style, name);
    }
    ~wxGenericGrid();

    bool Create(wxWindow *parent, wxWindowID, const wxPoint& pos, const wxSize& size, long style = 0, const wxString& name = "grid");

    bool CreateGrid(int nRows, int nCols, wxString **cellValues = (wxString **) NULL, short *widths = (short *) NULL,
            short defaultWidth = wxGRID_DEFAULT_CELL_WIDTH, short defaultHeight = wxGRID_DEFAULT_CELL_HEIGHT);
    void PaintGrid(wxDC& dc);
    void ClearGrid();
    virtual wxGridCell *GetCell(int row, int col) const;
    wxGridCell ***GetCells() const { return m_gridCells; }
    bool InsertCols(int pos = 0, int n = 1, bool updateLabels = TRUE);
    bool InsertRows(int pos = 0, int n = 1, bool updateLabels = TRUE);
    bool AppendCols(int n = 1, bool updateLabels = TRUE);
    bool AppendRows(int n = 1, bool updateLabels = TRUE);
    bool DeleteCols(int pos = 0, int n = 1, bool updateLabels = TRUE);
    bool DeleteRows(int pos = 0, int n = 1, bool updateLabels = TRUE);

    // Cell accessors
    void SetCellValue(const wxString& val, int row, int col);
    wxString& GetCellValue(int row, int col) const;
    void SetCellAlignment(int flag, int row, int col);
    void SetCellAlignment(int flag);
    int GetCellAlignment(int row, int col) const;
    int GetCellAlignment() const;
    void SetCellTextColour(const wxColour& val, int row, int col);
    void SetCellTextColour(const wxColour& col);
    wxColour& GetCellTextColour(int row, int col) const;
    wxColour& GetCellTextColour() const { return (wxColour&) m_cellTextColour; }
    void SetCellBackgroundColour(const wxColour& col);
    void SetCellBackgroundColour(const wxColour& colour, int row, int col);
    wxColour& GetCellBackgroundColour() const { return (wxColour&) m_cellBackgroundColour; }
    wxColour& GetCellBackgroundColour(int row, int col) const;
    wxFont& GetCellTextFont() const { return (wxFont&) m_cellTextFont; }
    wxFont& GetCellTextFont(int row, int col) const;
    void SetCellTextFont(const wxFont& fnt);
    void SetCellTextFont(const wxFont& fnt, int row, int col);
    wxBitmap *GetCellBitmap(int row, int col) const;
    void SetCellBitmap(wxBitmap *bitmap, int row, int col);
    void *SetCellData(void *data, int row, int col);
    void *GetCellData(int row, int col);

    // Size accessors
    void SetColumnWidth(int col, int width);
    int GetColumnWidth(int col) const;
    void SetRowHeight(int row, int height);
    int GetRowHeight(int row) const;
    int GetViewHeight() const { return m_viewHeight; }
    int GetViewWidth() const { return m_viewWidth; }

    // Label accessors
    void SetLabelSize(int orientation, int sz);
    int GetLabelSize(int orientation) const;
    void SetLabelAlignment(int orientation, int alignment);
    int GetLabelAlignment(int orientation) const;
    wxGridCell *GetLabelCell(int orientation, int pos) const;
    void SetLabelValue(int orientation, const wxString& val, int pos);
    wxString& GetLabelValue(int orientation, int pos) const;
    void SetLabelTextColour(const wxColour& colour);
    void SetLabelBackgroundColour(const wxColour& colour);
    wxColour& GetLabelTextColour() const { return (wxColour&) m_labelTextColour; }
    wxColour& GetLabelBackgroundColour() { return (wxColour&) m_labelBackgroundColour; }
    wxFont& GetLabelTextFont() { return (wxFont&) m_labelTextFont; }
    void SetLabelTextFont(const wxFont& fnt) { m_labelTextFont = fnt; }

    // Miscellaneous accessors
    int GetCursorRow() const { return m_wCursorRow; }
    int GetCursorColumn() const { return m_wCursorColumn; }
    void SetGridCursor(int row, int col);
    int GetRows() const { return m_totalRows; }
    int GetCols() const { return m_totalCols; }
    int GetScrollPosX() const { return m_scrollPosX; }
    int GetScrollPosY() const { return m_scrollPosY; }
    void SetScrollPosX(int pos) { m_scrollPosX = pos; }
    void SetScrollPosY(int pos) { m_scrollPosY = pos; }
    wxTextCtrl *GetTextItem() const { return m_textItem; }
    wxScrollBar *GetHorizScrollBar() const { return m_hScrollBar; }
    wxScrollBar *GetVertScrollBar() const { return m_vScrollBar; }
    bool GetEditable() const { return m_editable; }
    void SetEditable(bool edit);

    bool GetEditInPlace() const { return m_editInPlace; }
    void SetEditInPlace(bool edit = TRUE);

    wxRect& GetCurrentRect() const { return (wxRect&) m_currentRect; }
    bool CurrentCellVisible() const { return m_currentRectVisible; }
    void SetDividerPen(const wxPen& pen) { m_divisionPen = pen; }
    wxPen& GetDividerPen() const { return (wxPen&) m_divisionPen; }

    // High-level event handling
    // Override e.g. to check value of current cell; but call
    // base member for default processing.
    virtual void OnSelectCellImplementation(wxDC *dc, int row, int col);

    virtual void OnSelectCell(int WXUNUSED(row), int WXUNUSED(col)) {};
    void _OnSelectCell(wxGridEvent& event);

    // Override to create your own class of grid cell
    virtual wxGridCell *OnCreateCell();
    void _OnCreateCell(wxGridEvent& event);

    // Override to change labels e.g. creation of grid, inserting/deleting a row/col.
    // By default, auto-labels the grid.
    virtual void OnChangeLabels();
    void _OnChangeLabels(wxGridEvent& event);

    // Override to change the label of the edit field when selecting a cell
    // By default, sets it to e.g. A12
    virtual void OnChangeSelectionLabel();
    void _OnChangeSelectionLabel(wxGridEvent& event);

    // Override for event processing
    virtual void OnCellChange(int WXUNUSED(row), int WXUNUSED(col)) {};
    virtual void OnCellLeftClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(control), bool WXUNUSED(shift)) {};
    virtual void OnCellRightClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(control), bool WXUNUSED(shift)) {};
    virtual void OnLabelLeftClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(control), bool WXUNUSED(shift)) {};
    virtual void OnLabelRightClick(int WXUNUSED(row), int WXUNUSED(col), int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(control), bool WXUNUSED(shift)) {};

    void _OnCellChange(wxGridEvent& event);
    void _OnCellLeftClick(wxGridEvent& event);
    void _OnCellRightClick(wxGridEvent& event);
    void _OnLabelLeftClick(wxGridEvent& event);
    void _OnLabelRightClick(wxGridEvent& event);

    // Activation: call from wxFrame::OnActivate
    void OnActivate(bool active);

    // Miscellaneous
    void AdjustScrollbars();
    void UpdateDimensions();

    void SetCurrentRect (int Row, int Column, int canvasW = -1, int canvasH = -1);
    void HighlightCell(wxDC *dc, bool doHighlight);
    void DrawCellText();
    void SetGridClippingRegion(wxDC *dc);

    virtual bool CellHitTest(int x, int y, int *row, int *col);
    virtual bool LabelSashHitTest(int x, int y, int *orientation, int *rowOrCol, int *startPos);
    virtual bool LabelHitTest(int x, int y, int *row, int *col);

    // Painting
    virtual void DrawLabelAreas(wxDC *dc);
    virtual void DrawEditableArea(wxDC *dc);
    virtual void DrawGridLines(wxDC *dc);
    virtual void DrawColumnLabels(wxDC *dc);
    virtual void DrawColumnLabel(wxDC *dc, wxRect *rect, int col);
    virtual void DrawRowLabels(wxDC *dc);
    virtual void DrawRowLabel(wxDC *dc, wxRect *rect, int row);
    virtual void DrawCells(wxDC *dc);
    virtual void DrawCellValue(wxDC *dc, wxRect *rect, int row, int col);
    virtual void DrawCellBackground(wxDC *dc, wxRect *rect, int row, int col);
    virtual void DrawTextRect(wxDC *dc, const wxString& text, wxRect *rect, int flag);
    virtual void DrawBitmapRect(wxDC *dc, wxBitmap *bitmap, wxRect *rect, int flag);

    // Refresh cell and optionally set the text field
    void RefreshCell(int row, int col, bool setText = FALSE);

    // Don't refresh within the outer pair of these.
    void BeginBatch() { m_batchCount ++; }
    void EndBatch() { m_batchCount --; }
    int GetBatchCount() { return m_batchCount; }

    // implementation from now on

    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnText(wxCommandEvent& ev);
    void OnTextEnter(wxCommandEvent& ev);
    void OnTextInPlace(wxCommandEvent& ev);
    void OnTextInPlaceEnter(wxCommandEvent& ev);
    void OnGridScroll(wxScrollEvent& ev);

protected:
    wxPanel*                  m_editingPanel;  // Contains the text control
    wxTextCtrl*               m_textItem;
    wxTextCtrl*               m_inPlaceTextItem;

    wxScrollBar*              m_hScrollBar;
    wxScrollBar*              m_vScrollBar;
    int                       m_wCursorRow;
    int                       m_wCursorColumn;
    wxRect                    m_currentRect;
    bool                      m_currentRectVisible;
    wxGridCell***             m_gridCells;
    wxGridCell**              m_rowLabelCells;
    wxGridCell**              m_colLabelCells;

    bool                      m_editCreated;
    bool                      m_editable;
    bool                      m_editInPlace;
    bool                      m_inOnTextInPlace;
    bool                      m_inScroll;

    int                       m_totalRows;
    int                       m_totalCols;

    // Row and column we're currently looking at
    int                       m_scrollPosX;
    int                       m_scrollPosY;

    // Dimensions
    int                       m_leftOfSheet;
    int                       m_topOfSheet;
    int                       m_rightOfSheet;    // Calculated from m_colWidths
    int                       m_bottomOfSheet;   // Calculated from m_rowHeights
    int                       m_totalGridWidth; // Total 'virtual' size
    int                       m_totalGridHeight;
    int                       m_viewHeight;        // Number of rows displayed
    int                       m_viewWidth;        // Number of columns displayed
    int                       m_cellHeight;      // For now, a default
    int                       m_verticalLabelWidth;
    int                       m_horizontalLabelHeight;
    int                       m_verticalLabelAlignment;
    int                       m_horizontalLabelAlignment;
    int                       m_cellAlignment;
    short*                    m_colWidths;   // Dynamically allocated
    short*                    m_rowHeights;  // Dynamically allocated
    int                       m_scrollWidth;    // Vert. scroll width, horiz. scroll height

    // Colours
    wxColour                  m_cellTextColour;
    wxColour                  m_cellBackgroundColour;
    wxFont                    m_cellTextFont;
    wxColour                  m_labelTextColour;
    wxColour                  m_labelBackgroundColour;
    wxBrush                   m_labelBackgroundBrush;
    wxFont                    m_labelTextFont;
    wxPen                     m_divisionPen;
    wxPen                     m_highlightPen;
    wxBitmap*                 m_doubleBufferingBitmap;

    // Position of Edit control
    wxRect                    m_editControlPosition;

    // Drag status
    int                       m_dragStatus;
    int                       m_dragRowOrCol;
    int                       m_dragStartPosition;
    int                       m_dragLastPosition;
    wxCursor                  m_horizontalSashCursor;
    wxCursor                  m_verticalSashCursor;

    // To avoid multiple refreshes, use Begin/EndBatch
    int                       m_batchCount;

    DECLARE_EVENT_TABLE()
};

#define wxGRID_TEXT_CTRL 2000
#define wxGRID_HSCROLL   2001
#define wxGRID_VSCROLL   2002
#define wxGRID_EDIT_IN_PLACE_TEXT_CTRL 2003

class WXDLLEXPORT wxGridCell : public wxObject
{
public:
    wxString  textValue;
    wxFont    font;
    wxColour  textColour;
    wxColour  backgroundColour;
    wxBrush   backgroundBrush;
    wxBitmap* cellBitmap;
    void*     cellData;        // intended for additional data associated with a cell
    int       alignment;

    wxGridCell(wxGenericGrid *window = (wxGenericGrid *) NULL);
    ~wxGridCell();

    virtual wxString& GetTextValue() const { return (wxString&) textValue; }
    virtual void SetTextValue(const wxString& str) { textValue = str; }
    wxFont& GetFont() const { return (wxFont&) font; }
    void SetFont(const wxFont& f) { font = f; }
    wxColour& GetTextColour() const { return (wxColour&) textColour; }
    void SetTextColour(const wxColour& colour) { textColour = colour; }
    wxColour& GetBackgroundColour() const { return (wxColour&) backgroundColour; }
    void SetBackgroundColour(const wxColour& colour);
    wxBrush& GetBackgroundBrush() const { return (wxBrush&) backgroundBrush; }
    void SetBackgroundBrush(const wxBrush& brush) { backgroundBrush = brush; }
    int GetAlignment() const { return alignment; }
    void SetAlignment(int align) { alignment = align; }
    wxBitmap *GetCellBitmap() const { return cellBitmap; }
    void SetCellBitmap(wxBitmap *bitmap) { cellBitmap = bitmap; }

    void *SetCellData(void *data) { void *rc = cellData; cellData = data; return rc; }
    void *GetCellData() const { return cellData; }
};

class WXDLLEXPORT wxGrid : public wxGenericGrid
{
public:
    wxGrid() : wxGenericGrid() { }
    wxGrid(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
          long style=0, char *name = "gridWindow")
        : wxGenericGrid(parent, x, y, width, height, style, name)
        {
        }
};

class WXDLLEXPORT wxGridEvent : public wxCommandEvent
{
    DECLARE_DYNAMIC_CLASS(wxGridEvent)

public:
    wxGridEvent()
        : wxCommandEvent(), m_row(-1), m_col(-1), m_x(-1), m_y(-1),
    m_control(0), m_shift(0), m_cell(0)
    {
    }

    wxGridEvent(int id, wxEventType type, wxObject* obj,
            int row=-1, int col=-1, int x=-1, int y=-1,
            bool control=FALSE, bool shift=FALSE)
        : wxCommandEvent(type, id), m_row(row), m_col(col), m_x(x), m_y(y),
    m_control(control), m_shift(shift), m_cell(0)
    {
        SetEventObject(obj);
    }

//private:
    int         m_row;
    int         m_col;
    int         m_x;
    int         m_y;
    bool        m_control;
    bool        m_shift;
    wxGridCell* m_cell;

    int         GetRow() const { return m_row; }
    int         GetCol() const { return m_col; }
    wxPoint     GetPosition() const { return wxPoint( m_x, m_y ); }
    wxGridCell* GetCell() const { return m_cell; }
    bool        ControlDown() const { return m_control; }
    bool        ShiftDown() const { return m_shift; }
};

const wxEventType wxEVT_GRID_SELECT_CELL      = wxEVT_FIRST + 1575;
const wxEventType wxEVT_GRID_CREATE_CELL      = wxEVT_FIRST + 1576;
const wxEventType wxEVT_GRID_CHANGE_LABELS    = wxEVT_FIRST + 1577;
const wxEventType wxEVT_GRID_CHANGE_SEL_LABEL = wxEVT_FIRST + 1578;
const wxEventType wxEVT_GRID_CELL_CHANGE      = wxEVT_FIRST + 1579;
const wxEventType wxEVT_GRID_CELL_LCLICK      = wxEVT_FIRST + 1580;
const wxEventType wxEVT_GRID_CELL_RCLICK      = wxEVT_FIRST + 1581;
const wxEventType wxEVT_GRID_LABEL_LCLICK     = wxEVT_FIRST + 1582;
const wxEventType wxEVT_GRID_LABEL_RCLICK     = wxEVT_FIRST + 1583;


typedef void (wxEvtHandler::*wxGridEventFunction)(wxGridEvent&);

#define EVT_GRID_SELECT_CELL(fn)      { wxEVT_GRID_SELECT_CELL,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CREATE_CELL(fn)      { wxEVT_GRID_CREATE_CELL,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CHANGE_LABELS(fn)    { wxEVT_GRID_CHANGE_LABELS,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CHANGE_SEL_LABEL(fn) { wxEVT_GRID_CHANGE_SEL_LABEL, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_CHANGE(fn)      { wxEVT_GRID_CELL_CHANGE,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_LCLICK(fn)      { wxEVT_GRID_CELL_LCLICK,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_RCLICK(fn)      { wxEVT_GRID_CELL_RCLICK,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_LCLICK(fn)     { wxEVT_GRID_LABEL_LCLICK,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_RCLICK(fn)     { wxEVT_GRID_LABEL_RCLICK,     -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },

#endif // __GRIDH_G__

