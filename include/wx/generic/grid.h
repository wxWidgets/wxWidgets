/////////////////////////////////////////////////////////////////////////////
// Name:        grid.h
// Purpose:     wxGrid and related classes
// Author:      Michael Bedward (based on code by Julian Smart, Robin Dunn)
// Modified by:
// Created:     1/08/1999
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#include "wx/defs.h"

#if !defined(wxUSE_NEW_GRID) || !(wxUSE_NEW_GRID)
#include "gridg.h"
#else

#ifndef __WXGRID_H__
#define __WXGRID_H__

#ifdef __GNUG__
#pragma interface "grid.h"
#endif

#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/string.h"
#include "wx/scrolbar.h"
#include "wx/event.h"
#include "wx/textctrl.h"
#include "wx/combobox.h"
#include "wx/dynarray.h"


// Default parameters for wxGrid
//
#define WXGRID_DEFAULT_NUMBER_ROWS            10
#define WXGRID_DEFAULT_NUMBER_COLS            10
#ifdef __WXMSW__
#define WXGRID_DEFAULT_ROW_HEIGHT             25
#else
#define WXGRID_DEFAULT_ROW_HEIGHT             30
#endif  // __WXMSW__
#define WXGRID_DEFAULT_COL_WIDTH              80
#define WXGRID_DEFAULT_COL_LABEL_HEIGHT       32
#define WXGRID_DEFAULT_ROW_LABEL_WIDTH        82
#define WXGRID_LABEL_EDGE_ZONE                 5
#define WXGRID_MIN_ROW_HEIGHT                 15
#define WXGRID_MIN_COL_WIDTH                  15
#define WXGRID_DEFAULT_SCROLLBAR_WIDTH        16

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellAttrProviderData;
class WXDLLEXPORT wxGridRowLabelWindow;
class WXDLLEXPORT wxGridColLabelWindow;
class WXDLLEXPORT wxGridCornerLabelWindow;
class WXDLLEXPORT wxGridWindow;
class WXDLLEXPORT wxGrid;

// ----------------------------------------------------------------------------
// wxGridCellAttr: this class can be used to alter the cells appearance in
// the grid by changing their colour/font/... from default. An object of this
// class may be returned by wxGridTable::GetAttr().
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellAttr
{
public:
    // ctors
    wxGridCellAttr()
    {
        SetAlignment(0, 0);
    }

    wxGridCellAttr(const wxColour& colText,
                   const wxColour& colBack,
                   const wxFont& font,
                   int hAlign,
                   int vAlign)
        : m_colText(colText), m_colBack(colBack), m_font(font)
    {
        SetAlignment(hAlign, vAlign);
    }

    // default copy ctor ok

    // setters
    void SetTextColour(const wxColour& colText) { m_colText = colText; }
    void SetBackgroundColour(const wxColour& colBack) { m_colBack = colBack; }
    void SetFont(const wxFont& font) { m_font = font; }
    void SetAlignment(int hAlign, int vAlign)
    {
        m_hAlign = hAlign;
        m_vAlign = vAlign;
    }

    // accessors
    bool HasTextColour() const { return m_colText.Ok(); }
    bool HasBackgroundColour() const { return m_colBack.Ok(); }
    bool HasFont() const { return m_font.Ok(); }
    bool HasAlignment() const { return m_hAlign || m_vAlign; }

    const wxColour& GetTextColour() const { return m_colText; }
    const wxColour& GetBackgroundColour() const { return m_colBack; }
    const wxFont& GetFont() const { return m_font; }
    void GetAlignment(int *hAlign, int *vAlign)
    {
        if ( hAlign ) *hAlign = m_hAlign;
        if ( vAlign ) *vAlign = m_vAlign;
    }

private:
    wxColour m_colText,
             m_colBack;
    wxFont   m_font;
    int      m_hAlign,
             m_vAlign;
};

// ----------------------------------------------------------------------------
// wxGridCellAttrProvider: class used by wxGridTableBase to retrieve/store the
// cell attributes.
// ----------------------------------------------------------------------------

// implementation note: we separate it from wxGridTableBase because we wish to
// avoid deriving a new table class if possible, and sometimes it will be
// enough to just derive another wxGridCellAttrProvider instead

class WXDLLEXPORT wxGridCellAttrProvider
{
public:
    wxGridCellAttrProvider();
    virtual ~wxGridCellAttrProvider();

    virtual wxGridCellAttr *GetAttr(int row, int col) const;
    virtual void SetAttr(const wxGridCellAttr *attr, int row, int col);

private:
    void InitData();

    wxGridCellAttrProviderData *m_data;
};

//////////////////////////////////////////////////////////////////////
//
//  Grid table classes
//
//////////////////////////////////////////////////////////////////////


class WXDLLEXPORT wxGridTableBase : public wxObject
{
public:
    wxGridTableBase();
    virtual ~wxGridTableBase();

    // You must override these functions in a derived table class
    //
    virtual long GetNumberRows() = 0;
    virtual long GetNumberCols() = 0;
    virtual wxString GetValue( int row, int col ) = 0;
    virtual void SetValue( int row, int col, const wxString& s ) = 0;
    virtual bool IsEmptyCell( int row, int col ) = 0;

    // Overriding these is optional
    //
    virtual void SetView( wxGrid *grid ) { m_view = grid; }
    virtual wxGrid * GetView() const { return m_view; }

    virtual void Clear() {}
    virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    virtual bool AppendRows( size_t numRows = 1 );
    virtual bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    virtual bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    virtual bool AppendCols( size_t numCols = 1 );
    virtual bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    virtual wxString GetRowLabelValue( int row );
    virtual wxString GetColLabelValue( int col );
    virtual void SetRowLabelValue( int WXUNUSED(row), const wxString& ) {}
    virtual void SetColLabelValue( int WXUNUSED(col), const wxString& ) {}

    // Attribute handling
    //

    // give us the attr provider to use - we take ownership of the pointer
    void SetAttrProvider(wxGridCellAttrProvider *attrProvider);

    // get the currently used attr provider (may be NULL)
    wxGridCellAttrProvider *GetAttrProvider() const { return m_attrProvider; }

    // by default forwarded to wxGridCellAttrProvider if any. May be
    // overridden to handle attributes directly in this class.
    virtual wxGridCellAttr *GetAttr( int row, int col );

    // takes ownership of the pointer
    virtual void SetAttr(const wxGridCellAttr *attr, int row, int col );

private:
    wxGrid * m_view;
    wxGridCellAttrProvider *m_attrProvider;

    DECLARE_ABSTRACT_CLASS( wxGridTableBase );
};


// ----------------------------------------------------------------------------
// wxGridTableMessage
// ----------------------------------------------------------------------------

// IDs for messages sent from grid table to view
//
enum wxGridTableRequest
{
    wxGRIDTABLE_REQUEST_VIEW_GET_VALUES = 2000,
    wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES,
    wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
    wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
    wxGRIDTABLE_NOTIFY_ROWS_DELETED,
    wxGRIDTABLE_NOTIFY_COLS_INSERTED,
    wxGRIDTABLE_NOTIFY_COLS_APPENDED,
    wxGRIDTABLE_NOTIFY_COLS_DELETED
};

class WXDLLEXPORT wxGridTableMessage
{
public:
    wxGridTableMessage();
    wxGridTableMessage( wxGridTableBase *table, int id,
                        int comInt1 = -1,
                        int comInt2 = -1 );

    void SetTableObject( wxGridTableBase *table ) { m_table = table; }
    wxGridTableBase * GetTableObject() const { return m_table; }
    void SetId( int id ) { m_id = id; }
    int  GetId() { return m_id; }
    void SetCommandInt( int comInt1 ) { m_comInt1 = comInt1; }
    int  GetCommandInt() { return m_comInt1; }
    void SetCommandInt2( int comInt2 ) { m_comInt2 = comInt2; }
    int  GetCommandInt2() { return m_comInt2; }

private:
    wxGridTableBase *m_table;
    int m_id;
    int m_comInt1;
    int m_comInt2;
};



// ------ wxGridStringArray
// A 2-dimensional array of strings for data values
//

WX_DECLARE_EXPORTED_OBJARRAY(wxArrayString, wxGridStringArray);



// ------ wxGridStringTable
//
// Simplest type of data table for a grid for small tables of strings
// that are stored in memory
//

class WXDLLEXPORT wxGridStringTable : public wxGridTableBase
{
public:
    wxGridStringTable();
    wxGridStringTable( int numRows, int numCols );
    ~wxGridStringTable();

    // these are pure virtual in wxGridTableBase
    //
    long GetNumberRows();
    long GetNumberCols();
    wxString GetValue( int row, int col );
    void SetValue( int row, int col, const wxString& s );
    bool IsEmptyCell( int row, int col );

    // overridden functions from wxGridTableBase
    //
    void Clear();
    bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    bool AppendRows( size_t numRows = 1 );
    bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    bool AppendCols( size_t numCols = 1 );
    bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    void SetRowLabelValue( int row, const wxString& );
    void SetColLabelValue( int col, const wxString& );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );

private:
    wxGridStringArray m_data;

    // These only get used if you set your own labels, otherwise the
    // GetRow/ColLabelValue functions return wxGridTableBase defaults
    //
    wxArrayString     m_rowLabels;
    wxArrayString     m_colLabels;

    DECLARE_DYNAMIC_CLASS( wxGridStringTable )
};



//////////////////////////////////////////////////////////////////////
//
//  Grid view classes
//
//////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxGridCellCoords
{
public:
    wxGridCellCoords() { m_row = m_col = -1; }
    wxGridCellCoords( int r, int c ) { m_row = r; m_col = c; }

    // default copy ctor is ok

    long GetRow() const { return m_row; }
    void SetRow( long n ) { m_row = n; }
    long GetCol() const { return m_col; }
    void SetCol( long n ) { m_col = n; }
    void Set( long row, long col ) { m_row = row; m_col = col; }

    wxGridCellCoords& operator=( const wxGridCellCoords& other )
    {
        if ( &other != this )
        {
            m_row=other.m_row;
            m_col=other.m_col;
        }
        return *this;
    }

    bool operator==( const wxGridCellCoords& other )
    {
        return (m_row == other.m_row  &&  m_col == other.m_col);
    }

    bool operator!=( const wxGridCellCoords& other )
    {
        return (m_row != other.m_row  ||  m_col != other.m_col);
    }

    bool operator!()
    {
        return (m_row == -1 && m_col == -1 );
    }

private:
    long m_row;
    long m_col;
};


// For comparisons...
//
extern wxGridCellCoords wxGridNoCellCoords;
extern wxRect           wxGridNoCellRect;

// An array of cell coords...
//
WX_DECLARE_EXPORTED_OBJARRAY(wxGridCellCoords, wxGridCellCoordsArray);



// This set of classes is to provide for the use of different types of
// cell edit controls in the grid while avoiding the wx class info
// system in deference to wxPython

class WXDLLEXPORT wxGridTextCtrl : public wxTextCtrl
{
public:
    wxGridTextCtrl() {}
    wxGridTextCtrl( wxWindow *,
                    wxGrid *,
                    bool isCellControl,
                    wxWindowID id,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0 );

    void     SetStartValue( const wxString& );
    wxString GetStartValue() { return startValue; }

private:
    wxGrid *m_grid;

    // TRUE for controls placed over cells,
    // FALSE for a control on a grid control panel
    bool m_isCellControl;

    wxString startValue;

    void OnKeyDown( wxKeyEvent& );

    DECLARE_DYNAMIC_CLASS( wxGridTextCtrl )
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxGrid
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGrid : public wxScrolledWindow
{
public:
    wxGrid()
        {
            m_table          = (wxGridTableBase *) NULL;
            m_gridWin        = (wxGridWindow *) NULL;
            m_rowLabelWin    = (wxGridRowLabelWindow *) NULL;
            m_colLabelWin    = (wxGridColLabelWindow *) NULL;
            m_cornerLabelWin = (wxGridCornerLabelWindow *) NULL;
            m_cellEditCtrl   = (wxWindow *) NULL;
        }

    wxGrid( wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxPanelNameStr );

    ~wxGrid();

    bool CreateGrid( int numRows, int numCols );


    // ------ grid dimensions
    //
    int      GetNumberRows() { return  m_numRows; }
    int      GetNumberCols() { return  m_numCols; }


    // ------ display update functions
    //
    void CalcRowLabelsExposed( wxRegion& reg );

    void CalcColLabelsExposed( wxRegion& reg );
    void CalcCellsExposed( wxRegion& reg );


    // ------ event handlers
    //
    void ProcessRowLabelMouseEvent( wxMouseEvent& event );
    void ProcessColLabelMouseEvent( wxMouseEvent& event );
    void ProcessCornerLabelMouseEvent( wxMouseEvent& event );
    void ProcessGridCellMouseEvent( wxMouseEvent& event );
    bool ProcessTableMessage( wxGridTableMessage& );

    void DoEndDragResizeRow();
    void DoEndDragResizeCol();

    wxGridTableBase * GetTable() const { return m_table; }
    void SetTable( wxGridTableBase *table ) { m_table = table; }

    void ClearGrid();
    bool InsertRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool AppendRows( int numRows = 1, bool updateLabels=TRUE );
    bool DeleteRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool InsertCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );
    bool AppendCols( int numCols = 1, bool updateLabels=TRUE );
    bool DeleteCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );

    void DrawGridCellArea( wxDC& dc );
    void DrawCellBorder( wxDC& dc, const wxGridCellCoords& );
    void DrawAllGridLines( wxDC& dc, const wxRegion & reg );
    void DrawCell( wxDC& dc, const wxGridCellCoords& );
    void DrawCellBackground( wxDC& dc, const wxGridCellCoords& );
    void DrawCellValue( wxDC& dc, const wxGridCellCoords& );

    void DrawRowLabels( wxDC& dc );
    void DrawRowLabel( wxDC& dc, int row );

    void DrawColLabels( wxDC& dc );
    void DrawColLabel( wxDC& dc, int col );


    // ------ Cell text drawing functions
    //
    void DrawTextRectangle( wxDC& dc, const wxString&, const wxRect&,
                            int horizontalAlignment = wxLEFT,
                            int verticalAlignment = wxTOP );

    // Split a string containing newline chararcters into an array of
    // strings and return the number of lines
    //
    void StringToLines( const wxString& value, wxArrayString& lines );

    void GetTextBoxSize( wxDC& dc,
                         wxArrayString& lines,
                         long *width, long *height );


    // ------
    // Code that does a lot of grid modification can be enclosed
    // between BeginBatch() and EndBatch() calls to avoid screen
    // flicker
    //
    void     BeginBatch() { m_batchCount++; }
    void     EndBatch() { if ( m_batchCount > 0 ) m_batchCount--; }
    int      GetBatchCount() { return m_batchCount; }


    // ------ edit control functions
    //
    bool IsEditable() { return m_editable; }
    void EnableEditing( bool edit );

#if 0  // at the moment the cell edit control is always active
    void     EnableCellEditControl( bool enable );
#endif

    bool     IsCellEditControlEnabled()
             { return (m_cellEditCtrl && m_cellEditCtrlEnabled); }

    void ShowCellEditControl();
    void HideCellEditControl();
    void SetEditControlValue( const wxString& s = wxEmptyString );
    void SaveEditControlValue();


    // ------ grid location functions
    //  Note that all of these functions work with the logical coordinates of
    //  grid cells and labels so you will need to convert from device
    //  coordinates for mouse events etc.
    //
    void XYToCell( int x, int y, wxGridCellCoords& );
    int  YToRow( int y );
    int  XToCol( int x );

    int  YToEdgeOfRow( int y );
    int  XToEdgeOfCol( int x );

    wxRect CellToRect( int row, int col );
    wxRect CellToRect( const wxGridCellCoords& coords )
        { return CellToRect( coords.GetRow(), coords.GetCol() ); }

    int  GetGridCursorRow() { return m_currentCellCoords.GetRow(); }
    int  GetGridCursorCol() { return m_currentCellCoords.GetCol(); }

    // check to see if a cell is either wholly visible (the default arg) or
    // at least partially visible in the grid window
    //
    bool IsVisible( int row, int col, bool wholeCellVisible = TRUE );
    bool IsVisible( const wxGridCellCoords& coords, bool wholeCellVisible = TRUE )
        { return IsVisible( coords.GetRow(), coords.GetCol(), wholeCellVisible ); }
    void MakeCellVisible( int row, int col );
    void MakeCellVisible( const wxGridCellCoords& coords )
        { MakeCellVisible( coords.GetRow(), coords.GetCol() ); }


    // ------ grid cursor movement functions
    //
    void SetGridCursor( int row, int col )
        { SetCurrentCell( wxGridCellCoords(row, col) ); }

    bool MoveCursorUp();
    bool MoveCursorDown();
    bool MoveCursorLeft();
    bool MoveCursorRight();
    bool MovePageDown();
    bool MovePageUp();
    bool MoveCursorUpBlock();
    bool MoveCursorDownBlock();
    bool MoveCursorLeftBlock();
    bool MoveCursorRightBlock();


    // ------ label and gridline formatting
    //
    int      GetDefaultRowLabelSize() { return WXGRID_DEFAULT_ROW_LABEL_WIDTH; }
    int      GetRowLabelSize() { return m_rowLabelWidth; }
    int      GetDefaultColLabelSize() { return WXGRID_DEFAULT_COL_LABEL_HEIGHT; }
    int      GetColLabelSize() { return m_colLabelHeight; }
    wxColour GetLabelBackgroundColour() { return m_labelBackgroundColour; }
    wxColour GetLabelTextColour() { return m_labelTextColour; }
    wxFont   GetLabelFont() { return m_labelFont; }
    void     GetRowLabelAlignment( int *horiz, int *vert );
    void     GetColLabelAlignment( int *horiz, int *vert );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );
    wxColour GetGridLineColour() { return m_gridLineColour; }

        void     SetRowLabelSize( int width );
    void     SetColLabelSize( int height );
    void     SetLabelBackgroundColour( const wxColour& );
    void     SetLabelTextColour( const wxColour& );
    void     SetLabelFont( const wxFont& );
    void     SetRowLabelAlignment( int horiz, int vert );
    void     SetColLabelAlignment( int horiz, int vert );
    void     SetRowLabelValue( int row, const wxString& );
    void     SetColLabelValue( int col, const wxString& );
    void     SetGridLineColour( const wxColour& );

    void     EnableGridLines( bool enable = TRUE );
    bool     GridLinesEnabled() { return m_gridLinesEnabled; }


    // ------ row and col formatting
    //
    int      GetDefaultRowSize();
    int      GetRowSize( int row );
    int      GetDefaultColSize();
    int      GetColSize( int col );
    wxColour GetDefaultCellBackgroundColour();
    wxColour GetCellBackgroundColour( int row, int col );
    wxColour GetDefaultCellTextColour();
    wxColour GetCellTextColour( int row, int col );
    wxFont   GetDefaultCellFont();
    wxFont   GetCellFont( int row, int col );
    void     GetDefaultCellAlignment( int *horiz, int *vert );
    void     GetCellAlignment( int row, int col, int *horiz, int *vert );

    void     SetDefaultRowSize( int height, bool resizeExistingRows = FALSE );
    void     SetRowSize( int row, int height );
    void     SetDefaultColSize( int width, bool resizeExistingCols = FALSE );

    void     SetColSize( int col, int width );
    void     SetDefaultCellBackgroundColour( const wxColour& );
    void     SetCellBackgroundColour( int row, int col, const wxColour& );
    void     SetDefaultCellTextColour( const wxColour& );

    void     SetCellTextColour( int row, int col, const wxColour& );
    void     SetDefaultCellFont( const wxFont& );
    void     SetCellFont( int row, int col, const wxFont& );
    void     SetDefaultCellAlignment( int horiz, int vert );
    void     SetCellAlignment( int row, int col, int horiz, int vert );


    // ------ cell value accessors
    //
    wxString GetCellValue( int row, int col )
    {
        if ( m_table )
        {
            return m_table->GetValue( row, col );
        }
        else
        {
            return wxEmptyString;
        }
    }

    wxString GetCellValue( const wxGridCellCoords& coords )
        { return GetCellValue( coords.GetRow(), coords.GetCol() ); }

    void SetCellValue( int row, int col, const wxString& s );
    void SetCellValue( const wxGridCellCoords& coords, const wxString& s )
        { SetCellValue( coords.GetRow(), coords.GetCol(), s ); }



    // ------ selections of blocks of cells
    //
    void SelectRow( int row, bool addToSelected = FALSE );
    void SelectCol( int col, bool addToSelected = FALSE );

    void SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol );

    void SelectBlock( const wxGridCellCoords& topLeft,
                      const wxGridCellCoords& bottomRight )
        { SelectBlock( topLeft.GetRow(), topLeft.GetCol(),
                       bottomRight.GetRow(), bottomRight.GetCol() ); }

    void SelectAll();

    bool IsSelection()
        { return ( m_selectedTopLeft != wxGridNoCellCoords &&
                   m_selectedBottomRight != wxGridNoCellCoords );
        }

    void ClearSelection();

    bool IsInSelection( int row, int col )
        { return ( IsSelection() &&
                   row >= m_selectedTopLeft.GetRow() &&
                   col >= m_selectedTopLeft.GetCol() &&
                   row <= m_selectedBottomRight.GetRow() &&
                   col <= m_selectedBottomRight.GetCol() );
        }

    bool IsInSelection( const wxGridCellCoords& coords )
        { return IsInSelection( coords.GetRow(), coords.GetCol() ); }

    void GetSelection( int* topRow, int* leftCol, int* bottomRow, int* rightCol )
        {
            // these will all be -1 if there is no selected block
            //
            *topRow    = m_selectedTopLeft.GetRow();
            *leftCol   = m_selectedTopLeft.GetCol();
            *bottomRow = m_selectedBottomRight.GetRow();
            *rightCol  = m_selectedBottomRight.GetCol();
        }


    // This function returns the rectangle that encloses the block of cells
    // limited by TopLeft and BottomRight cell in device coords and clipped
    //  to the client size of the grid window.
    //
    wxRect BlockToDeviceRect( const wxGridCellCoords & topLeft,
                              const wxGridCellCoords & bottomRight );

    // This function returns the rectangle that encloses the selected cells
    // in device coords and clipped to the client size of the grid window.
    //
    wxRect SelectionToDeviceRect()
        {
            return BlockToDeviceRect( m_selectedTopLeft,
                                      m_selectedBottomRight );
        }


    // ------ For compatibility with previous wxGrid only...
    //
    //  ************************************************
    //  **  Don't use these in new code because they  **
    //  **  are liable to disappear in a future       **
    //  **  revision                                  **
    //  ************************************************
    //

    wxGrid( wxWindow *parent,
            int x = -1, int y = -1, int w = -1, int h = -1,
            long style = 0,
            const wxString& name = wxPanelNameStr )
        : wxScrolledWindow( parent, -1, wxPoint(x,y), wxSize(w,h), style, name )
        {
            Create();
        }

    void SetCellValue( const wxString& val, int row, int col )
        { SetCellValue( row, col, val ); }

    void UpdateDimensions()
        { CalcDimensions(); }

    int GetRows() { return GetNumberRows(); }
    int GetCols() { return GetNumberCols(); }
    int GetCursorRow() { return GetGridCursorRow(); }
    int GetCursorColumn() { return GetGridCursorCol(); }

    int GetScrollPosX() { return 0; }
    int GetScrollPosY() { return 0; }

    void SetScrollX( int x ) { }
    void SetScrollY( int y ) { }

    void SetColumnWidth( int col, int width )
        { SetColSize( col, width ); }

    int GetColumnWidth( int col )
        { return GetColSize( col ); }

    void SetRowHeight( int row, int height )
        { SetRowSize( row, height ); }

    int GetRowHeight( int row )
        { return GetRowSize( row ); }

    int GetViewHeight() // returned num whole rows visible
        { return 0; }

    int GetViewWidth() // returned num whole cols visible
        { return 0; }

    void SetLabelSize( int orientation, int sz )
        {
            if ( orientation == wxHORIZONTAL )
                SetColLabelSize( sz );
            else
                SetRowLabelSize( sz );
        }

    int GetLabelSize( int orientation )
        {
            if ( orientation == wxHORIZONTAL )
                return GetColLabelSize();
            else
                return GetRowLabelSize();
        }

    void SetLabelAlignment( int orientation, int align )
        {
            if ( orientation == wxHORIZONTAL )
                SetColLabelAlignment( align, -1 );
            else
                SetRowLabelAlignment( align, -1 );
        }

    int GetLabelAlignment( int orientation, int WXUNUSED(align) )
        {
            int h, v;
            if ( orientation == wxHORIZONTAL )
            {
                GetColLabelAlignment( &h, &v );
                return h;
            }
            else
            {
                GetRowLabelAlignment( &h, &v );
                return h;
            }
        }

    void SetLabelValue( int orientation, const wxString& val, int pos )
        {
            if ( orientation == wxHORIZONTAL )
                SetColLabelValue( pos, val );
            else
                SetRowLabelValue( pos, val );
        }

    wxString GetLabelValue( int orientation, int pos)
        {
            if ( orientation == wxHORIZONTAL )
                return GetColLabelValue( pos );
            else
                return GetRowLabelValue( pos );
        }

    wxFont GetCellTextFont() const
        { return m_defaultCellFont; }

    wxFont GetCellTextFont(int WXUNUSED(row), int WXUNUSED(col)) const
        { return m_defaultCellFont; }

    void SetCellTextFont(const wxFont& fnt)
        { SetDefaultCellFont( fnt ); }

    void SetCellTextFont(const wxFont& fnt, int row, int col)
        { SetCellFont( row, col, fnt ); }

    void SetCellTextColour(const wxColour& val, int row, int col)
        { SetCellTextColour( row, col, val ); }

    void SetCellTextColour(const wxColour& col)
        { SetDefaultCellTextColour( col ); }

    void SetCellBackgroundColour(const wxColour& col)
        { SetDefaultCellBackgroundColour( col ); }

    void SetCellBackgroundColour(const wxColour& colour, int row, int col)
        { SetCellBackgroundColour( row, col, colour ); }

    bool GetEditable() { return IsEditable(); }
    void SetEditable( bool edit = TRUE ) { EnableEditing( edit ); }
    bool GetEditInPlace() { return IsCellEditControlEnabled(); }

    void SetEditInPlace(bool edit = TRUE) { }

    void SetCellAlignment( int align, int row, int col)
    { SetCellAlignment(row, col, align, wxCENTER); }
    void SetCellAlignment( int WXUNUSED(align) ) {}
    void SetCellBitmap(wxBitmap *WXUNUSED(bitmap), int WXUNUSED(row), int WXUNUSED(col))
    { }
    void SetDividerPen(const wxPen& WXUNUSED(pen)) { }
    wxPen& GetDividerPen() const { return wxNullPen; }
    void OnActivate(bool WXUNUSED(active)) {}

    // ******** End of compatibility functions **********



    // ------ control IDs
    enum { wxGRID_CELLCTRL = 2000,
           wxGRID_TOPCTRL };

    // ------ control types
    enum { wxGRID_TEXTCTRL = 2100,
           wxGRID_CHECKBOX,
           wxGRID_CHOICE,
           wxGRID_COMBOBOX };

protected:
    bool m_created;
    bool m_displayed;

    wxGridWindow             *m_gridWin;
    wxGridRowLabelWindow     *m_rowLabelWin;
    wxGridColLabelWindow     *m_colLabelWin;
    wxGridCornerLabelWindow  *m_cornerLabelWin;

    wxGridTableBase          *m_table;

    int m_left;
    int m_top;
    int m_right;
    int m_bottom;

    int m_numRows;
    int m_numCols;

    wxGridCellCoords m_currentCellCoords;

    wxGridCellCoords m_selectedTopLeft;
    wxGridCellCoords m_selectedBottomRight;

    int        m_defaultRowHeight;
    wxArrayInt m_rowHeights;
    wxArrayInt m_rowBottoms;

    int        m_defaultColWidth;
    wxArrayInt m_colWidths;
    wxArrayInt m_colRights;

    int m_rowLabelWidth;
    int m_colLabelHeight;

    wxColour   m_labelBackgroundColour;
    wxColour   m_labelTextColour;
    wxFont     m_labelFont;

    int        m_rowLabelHorizAlign;
    int        m_rowLabelVertAlign;
    int        m_colLabelHorizAlign;
    int        m_colLabelVertAlign;

    bool       m_defaultRowLabelValues;
    bool       m_defaultColLabelValues;

    wxColour   m_gridLineColour;
    bool       m_gridLinesEnabled;

    // default cell attributes
    wxFont     m_defaultCellFont;
    int        m_defaultCellHAlign,
               m_defaultCellVAlign;

    // do we have some place to store attributes in?
    bool CanHaveAttributes();

    wxGridCellCoordsArray  m_cellsExposed;
    wxArrayInt             m_rowsExposed;
    wxArrayInt             m_colsExposed;
    wxArrayInt             m_rowLabelsExposed;
    wxArrayInt             m_colLabelsExposed;

    bool m_inOnKeyDown;
    int  m_batchCount;

    enum CursorMode
    {
        WXGRID_CURSOR_SELECT_CELL,
        WXGRID_CURSOR_RESIZE_ROW,
        WXGRID_CURSOR_RESIZE_COL,
        WXGRID_CURSOR_SELECT_ROW,
        WXGRID_CURSOR_SELECT_COL
    };

    // this method not only sets m_cursorMode but also sets the correct cursor
    // for the given mode and, if captureMouse is not FALSE releases the mouse
    // if it was captured and captures it if it must be captured
    //
    // for this to work, you should always use it and not set m_cursorMode
    // directly!
    void ChangeCursorMode(CursorMode mode,
                          wxWindow *win = (wxWindow *)NULL,
                          bool captureMouse = TRUE);

    wxWindow *m_winCapture;     // the window which captured the mouse
    CursorMode m_cursorMode;

    int  m_dragLastPos;
    int  m_dragRowOrCol;
    bool m_isDragging;

    wxGridCellCoords m_selectionStart;

    wxCursor m_rowResizeCursor;
    wxCursor m_colResizeCursor;

    bool       m_editable;  // applies to whole grid
    int        m_editCtrlType;  // for current cell
    wxWindow*  m_cellEditCtrl;
    bool       m_cellEditCtrlEnabled;


    void Create();
    void Init();
    void CalcDimensions();
    void CalcWindowSizes();
    bool Redimension( wxGridTableMessage& );


    bool SendEvent( const wxEventType,
                    int row, int col,
                    wxMouseEvent& );

    bool SendEvent( const wxEventType,
                    int row, int col );


    void OnPaint( wxPaintEvent& );
    void OnSize( wxSizeEvent& );
    void OnKeyDown( wxKeyEvent& );


    void SetCurrentCell( const wxGridCellCoords& coords );
    void SetCurrentCell( int row, int col )
        { SetCurrentCell( wxGridCellCoords(row, col) ); }


    // ------ functions to get/send data (see also public functions)
    //
    bool GetModelValues();
    bool SetModelValues();


    DECLARE_DYNAMIC_CLASS( wxGrid )
    DECLARE_EVENT_TABLE()
};





//
// ------ Grid event class and event types
//

class WXDLLEXPORT wxGridEvent : public wxNotifyEvent
{
public:
    wxGridEvent()
        : wxNotifyEvent(), m_row(-1), m_col(-1), m_x(-1), m_y(-1),
        m_control(0), m_meta(0), m_shift(0), m_alt(0)
        {
        }

    wxGridEvent(int id, wxEventType type, wxObject* obj,
                int row=-1, int col=-1, int x=-1, int y=-1,
                bool control=FALSE, bool shift=FALSE, bool alt=FALSE, bool meta=FALSE);

    virtual int GetRow() { return m_row; }
    virtual int GetCol() { return m_col; }
    wxPoint     GetPosition() { return wxPoint( m_x, m_y ); }
    bool        ControlDown() { return m_control; }
    bool        MetaDown() { return m_meta; }
    bool        ShiftDown() { return m_shift; }
    bool        AltDown() { return m_alt; }

protected:
    int         m_row;
    int         m_col;
    int         m_x;
    int         m_y;
    bool        m_control;
    bool        m_meta;
    bool        m_shift;
    bool        m_alt;

    DECLARE_DYNAMIC_CLASS(wxGridEvent)
};


class WXDLLEXPORT wxGridSizeEvent : public wxNotifyEvent
{
public:
    wxGridSizeEvent()
        : wxNotifyEvent(), m_rowOrCol(-1), m_x(-1), m_y(-1),
        m_control(0), m_meta(0), m_shift(0), m_alt(0)
        {
        }

    wxGridSizeEvent(int id, wxEventType type, wxObject* obj,
                int rowOrCol=-1, int x=-1, int y=-1,
                bool control=FALSE, bool shift=FALSE, bool alt=FALSE, bool meta=FALSE);

    int         GetRowOrCol() { return m_rowOrCol; }
    wxPoint     GetPosition() { return wxPoint( m_x, m_y ); }
    bool        ControlDown() { return m_control; }
    bool        MetaDown() { return m_meta; }
    bool        ShiftDown() { return m_shift; }
    bool        AltDown() { return m_alt; }

protected:
    int         m_rowOrCol;
    int         m_x;
    int         m_y;
    bool        m_control;
    bool        m_meta;
    bool        m_shift;
    bool        m_alt;

    DECLARE_DYNAMIC_CLASS(wxGridSizeEvent)
};


class WXDLLEXPORT wxGridRangeSelectEvent : public wxNotifyEvent
{
public:
    wxGridRangeSelectEvent()
        : wxNotifyEvent()
        {
            m_topLeft     = wxGridNoCellCoords;
            m_bottomRight = wxGridNoCellCoords;
            m_control     = FALSE;
            m_meta        = FALSE;
            m_shift       = FALSE;
            m_alt         = FALSE;
        }

    wxGridRangeSelectEvent(int id, wxEventType type, wxObject* obj,
                           const wxGridCellCoords& topLeft,
                           const wxGridCellCoords& bottomRight,
                           bool control=FALSE, bool shift=FALSE,
                           bool alt=FALSE, bool meta=FALSE);

    wxGridCellCoords GetTopLeftCoords() { return m_topLeft; }
    wxGridCellCoords GetBottomRightCoords() { return m_bottomRight; }
    int         GetTopRow()    { return m_topLeft.GetRow(); }
    int         GetBottomRow() { return m_bottomRight.GetRow(); }
    int         GetLeftCol()   { return m_topLeft.GetCol(); }
    int         GetRightCol()  { return m_bottomRight.GetCol(); }
    bool        ControlDown()  { return m_control; }
    bool        MetaDown()     { return m_meta; }
    bool        ShiftDown()    { return m_shift; }
    bool        AltDown()      { return m_alt; }

protected:
    wxGridCellCoords  m_topLeft;
    wxGridCellCoords  m_bottomRight;
    bool              m_control;
    bool              m_meta;
    bool              m_shift;
    bool              m_alt;

    DECLARE_DYNAMIC_CLASS(wxGridRangeSelectEvent)
};


const wxEventType EVT_GRID_CELL_LEFT_CLICK    = wxEVT_FIRST + 1580;
const wxEventType EVT_GRID_CELL_RIGHT_CLICK   = wxEVT_FIRST + 1581;
const wxEventType EVT_GRID_CELL_LEFT_DCLICK   = wxEVT_FIRST + 1582;
const wxEventType EVT_GRID_CELL_RIGHT_DCLICK  = wxEVT_FIRST + 1583;
const wxEventType EVT_GRID_LABEL_LEFT_CLICK   = wxEVT_FIRST + 1584;
const wxEventType EVT_GRID_LABEL_RIGHT_CLICK  = wxEVT_FIRST + 1585;
const wxEventType EVT_GRID_LABEL_LEFT_DCLICK  = wxEVT_FIRST + 1586;
const wxEventType EVT_GRID_LABEL_RIGHT_DCLICK = wxEVT_FIRST + 1587;
const wxEventType EVT_GRID_ROW_SIZE           = wxEVT_FIRST + 1588;
const wxEventType EVT_GRID_COL_SIZE           = wxEVT_FIRST + 1589;
const wxEventType EVT_GRID_RANGE_SELECT       = wxEVT_FIRST + 1590;
const wxEventType EVT_GRID_CELL_CHANGE        = wxEVT_FIRST + 1591;
const wxEventType EVT_GRID_SELECT_CELL        = wxEVT_FIRST + 1592;


typedef void (wxEvtHandler::*wxGridEventFunction)(wxGridEvent&);
typedef void (wxEvtHandler::*wxGridSizeEventFunction)(wxGridSizeEvent&);
typedef void (wxEvtHandler::*wxGridRangeSelectEventFunction)(wxGridRangeSelectEvent&);

#define EVT_GRID_CELL_LEFT_CLICK(fn)     { EVT_GRID_CELL_LEFT_CLICK,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_RIGHT_CLICK(fn)    { EVT_GRID_CELL_RIGHT_CLICK,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_LEFT_DCLICK(fn)    { EVT_GRID_CELL_LEFT_DCLICK,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_RIGHT_DCLICK(fn)   { EVT_GRID_CELL_RIGHT_DCLICK,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_LEFT_CLICK(fn)    { EVT_GRID_LABEL_LEFT_CLICK,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_RIGHT_CLICK(fn)   { EVT_GRID_LABEL_RIGHT_CLICK,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_LEFT_DCLICK(fn)   { EVT_GRID_LABEL_LEFT_DCLICK,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_RIGHT_DCLICK(fn)  { EVT_GRID_LABEL_RIGHT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_ROW_SIZE(fn)            { EVT_GRID_ROW_SIZE,           -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridSizeEventFunction) &fn, NULL },
#define EVT_GRID_COL_SIZE(fn)            { EVT_GRID_COL_SIZE,           -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridSizeEventFunction) &fn, NULL },
#define EVT_GRID_RANGE_SELECT(fn)        { EVT_GRID_RANGE_SELECT,       -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridRangeSelectEventFunction) &fn, NULL },
#define EVT_GRID_CELL_CHANGE(fn)         { EVT_GRID_CELL_CHANGE,        -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_SELECT_CELL(fn)         { EVT_GRID_SELECT_CELL,        -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },


#if 0  // TODO: implement these ?  others ?

const wxEventType EVT_GRID_CREATE_CELL      = wxEVT_FIRST + 1576;
const wxEventType EVT_GRID_CHANGE_LABELS    = wxEVT_FIRST + 1577;
const wxEventType EVT_GRID_CHANGE_SEL_LABEL = wxEVT_FIRST + 1578;

#define EVT_GRID_CREATE_CELL(fn)      { EVT_GRID_CREATE_CELL,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CHANGE_LABELS(fn)    { EVT_GRID_CHANGE_LABELS,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CHANGE_SEL_LABEL(fn) { EVT_GRID_CHANGE_SEL_LABEL, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },

#endif

#endif  // #ifndef __WXGRID_H__

#endif  // ifndef wxUSE_NEW_GRID

