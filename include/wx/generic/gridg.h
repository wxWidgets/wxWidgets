/////////////////////////////////////////////////////////////////////////////
// Name:        gridg.h
// Purpose:     wxGenericGrid
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
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

#ifndef wxLEFT
#define wxLEFT 0x0400
#endif

#ifndef wxRIGHT
#define wxRIGHT 0x0800
#endif

#define WXGENERIC_GRID_VERSION  0.5

class WXDLLEXPORT wxGridEvent;
class WXDLLEXPORT wxGridCell;
class WXDLLEXPORT wxGenericGrid: public wxPanel
{
  DECLARE_DYNAMIC_CLASS(wxGenericGrid)
 public:
  wxGenericGrid(void);

  inline wxGenericGrid(wxWindow *parent, int x, int y, int width, int height, long style = 0, char *name = "grid")
  {
    Create(parent, -1, wxPoint(x, y), wxSize(width, height), style, name);
  }
  inline wxGenericGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style = 0, const wxString& name = "grid")
  {
    Create(parent, id, pos, size, style, name);
  }
  ~wxGenericGrid(void);

  void OnPaint(wxPaintEvent& event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnMouseEvent(wxMouseEvent& event);
  void OnSize(wxSizeEvent& event);

  bool Create(wxWindow *parent, wxWindowID, const wxPoint& pos, const wxSize& size, long style = 0, const wxString& name = "grid");

  bool CreateGrid(int nRows, int nCols, wxString **cellValues = (wxString **) NULL, short *widths = (short *) NULL,
     short defaultWidth = wxGRID_DEFAULT_CELL_WIDTH, short defaultHeight = wxGRID_DEFAULT_CELL_HEIGHT);
  void PaintGrid(wxDC& dc);
  void ClearGrid(void);
  virtual wxGridCell *GetCell(int row, int col) const;
  inline wxGridCell ***GetCells(void) const { return m_gridCells; }
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
  int GetCellAlignment(void) const;
  void SetCellTextColour(const wxColour& val, int row, int col);
  void SetCellTextColour(const wxColour& col);
  wxColour& GetCellTextColour(int row, int col) const;
  inline wxColour& GetCellTextColour(void) const { return (wxColour&) m_cellTextColour; }
  void SetCellBackgroundColour(const wxColour& col);
  void SetCellBackgroundColour(const wxColour& colour, int row, int col);
  inline wxColour& GetCellBackgroundColour(void) const { return (wxColour&) m_cellBackgroundColour; }
  wxColour& GetCellBackgroundColour(int row, int col) const;
  inline wxFont& GetCellTextFont(void) const { return (wxFont&) m_cellTextFont; }
  wxFont& GetCellTextFont(int row, int col) const;
  void SetCellTextFont(const wxFont& fnt);
  void SetCellTextFont(const wxFont& fnt, int row, int col);
  wxBitmap *GetCellBitmap(int row, int col) const;
  void SetCellBitmap(wxBitmap *bitmap, int row, int col);

  // Size accessors
  void SetColumnWidth(int col, int width);
  int GetColumnWidth(int col) const;
  void SetRowHeight(int row, int height);
  int GetRowHeight(int row) const;

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
  inline wxColour& GetLabelTextColour(void) const { return (wxColour&) m_labelTextColour; }
  inline wxColour& GetLabelBackgroundColour(void) { return (wxColour&) m_labelBackgroundColour; }
  inline wxFont& GetLabelTextFont(void) { return (wxFont&) m_labelTextFont; }
  inline void SetLabelTextFont(const wxFont& fnt) { m_labelTextFont = fnt; }

  // Miscellaneous accessors
  inline int GetCursorRow(void) const { return m_wCursorRow; }
  inline int GetCursorColumn(void) const { return m_wCursorColumn; }
  void SetGridCursor(int row, int col);
  inline int GetRows(void) const { return m_totalRows; }
  inline int GetCols(void) const { return m_totalCols; }
  inline int GetScrollPosX(void) const { return m_scrollPosX; }
  inline int GetScrollPosY(void) const { return m_scrollPosY; }
  inline void SetScrollPosX(int pos) { m_scrollPosX = pos; }
  inline void SetScrollPosY(int pos) { m_scrollPosY = pos; }
  inline wxTextCtrl *GetTextItem(void) const { return m_textItem; }
  inline wxScrollBar *GetHorizScrollBar(void) const { return m_hScrollBar; }
  inline wxScrollBar *GetVertScrollBar(void) const { return m_vScrollBar; }
  inline bool GetEditable(void) const { return m_editable; }
  void SetEditable(bool edit);
  inline wxRect& GetCurrentRect(void) const { return (wxRect&) m_currentRect; }
  inline bool CurrentCellVisible(void) const { return m_currentRectVisible; }
  inline void SetDividerPen(const wxPen& pen) { m_divisionPen = pen; }
  inline wxPen& GetDividerPen(void) const { return (wxPen&) m_divisionPen; }

  // High-level event handling
  // Override e.g. to check value of current cell; but call
  // base member for default processing.
  virtual void OnSelectCellImplementation(wxDC *dc, int row, int col);

  virtual void OnSelectCell(int WXUNUSED(row), int WXUNUSED(col)) {};
  void _OnSelectCell(wxGridEvent& event);

  // Override to create your own class of grid cell
  virtual wxGridCell *OnCreateCell(void);
  void _OnCreateCell(wxGridEvent& event);

  // Override to change labels e.g. creation of grid, inserting/deleting a row/col.
  // By default, auto-labels the grid.
  virtual void OnChangeLabels(void);
  void _OnChangeLabels(wxGridEvent& event);

  // Override to change the label of the edit field when selecting a cell
  // By default, sets it to e.g. A12
  virtual void OnChangeSelectionLabel(void);
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
  void AdjustScrollbars(void);
  void UpdateDimensions(void);

  /* INTERNAL
   */
  void SetCurrentRect (int Row, int Column, int canvasW = -1, int canvasH = -1);
  void HighlightCell (wxDC *dc);
  void DrawCellText(void);
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
  inline void BeginBatch(void) { m_batchCount ++; }
  inline void EndBatch(void) { m_batchCount --; }
  inline int GetBatchCount(void) { return m_batchCount; }

  void OnText(wxCommandEvent& ev);
  void OnGridScroll(wxScrollEvent& ev);

 protected:
  wxPanel*                  m_editingPanel;  // Contains the text control
  wxTextCtrl*               m_textItem;
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

class WXDLLEXPORT wxGridCell: public wxObject
{
 public:
  wxString  textValue;
  wxFont    font;
  wxColour  textColour;
  wxColour  backgroundColour;
  wxBrush   backgroundBrush;
  wxBitmap* cellBitmap;
  int alignment;

  wxGridCell(wxGenericGrid *window = (wxGenericGrid *) NULL);
  ~wxGridCell(void);

  virtual wxString& GetTextValue(void) const { return (wxString&) textValue; }
  virtual void SetTextValue(const wxString& str) { textValue = str; }
  inline wxFont& GetFont(void) const { return (wxFont&) font; }
  inline void SetFont(const wxFont& f) { font = f; }
  inline wxColour& GetTextColour(void) const { return (wxColour&) textColour; }
  inline void SetTextColour(const wxColour& colour) { textColour = colour; }
  inline wxColour& GetBackgroundColour(void) const { return (wxColour&) backgroundColour; }
  void SetBackgroundColour(const wxColour& colour);
  inline wxBrush& GetBackgroundBrush(void) const { return (wxBrush&) backgroundBrush; }
  inline void SetBackgroundBrush(const wxBrush& brush) { backgroundBrush = brush; }
  inline int GetAlignment(void) const { return alignment; }
  inline void SetAlignment(int align) { alignment = align; }
  inline wxBitmap *GetCellBitmap(void) const { return cellBitmap; }
  inline void SetCellBitmap(wxBitmap *bitmap) { cellBitmap = bitmap; }
};

class WXDLLEXPORT wxGrid: public wxGenericGrid
{
  public:
    wxGrid(void):wxGenericGrid() {}
    wxGrid(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0, char *name = "gridWindow"):
     wxGenericGrid(parent, x, y, width, height, style, name)
    {
    }
};

class WXDLLEXPORT wxGridEvent : public wxCommandEvent {
    DECLARE_DYNAMIC_CLASS(wxGridEvent)
public:
    wxGridEvent()
        : wxCommandEvent(), m_row(-1), m_col(-1), m_x(-1), m_y(-1),
          m_control(0), m_shift(0), m_cell(0)
        {}

    wxGridEvent(int id, wxEventType type, wxObject* obj,
                int row=-1, int col=-1, int x=-1, int y=-1,
                bool control=FALSE, bool shift=FALSE)
        : wxCommandEvent(type, id), m_row(row), m_col(col), m_x(x), m_y(y),
          m_control(control), m_shift(shift), m_cell(0)
        {
            SetEventObject(obj);
        }


    int         m_row;
    int         m_col;
    int         m_x;
    int         m_y;
    bool        m_control;
    bool        m_shift;
    wxGridCell* m_cell;
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

#endif

