///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/gridtest.cpp
// Purpose:     wxGrid unit test
// Author:      Steven Lamerton
// Created:     2010-06-25
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_GRID

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
#endif // WX_PRECOMP

#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "testableframe.h"
#include "asserthelper.h"
#include "wx/uiaction.h"

#ifdef __WXGTK__
    #include "wx/stopwatch.h"
#endif // __WXGTK__

#include "waitforpaint.h"

class GridTestCase : public CppUnit::TestCase
{
public:
    GridTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( GridTestCase );
        WXUISIM_TEST( CellEdit );
        WXUISIM_TEST( CellClick );
        WXUISIM_TEST( ReorderedColumnsCellClick );
        WXUISIM_TEST( CellSelect );
        WXUISIM_TEST( LabelClick );
        WXUISIM_TEST( SortClick );
        WXUISIM_TEST( Size );
        WXUISIM_TEST( RangeSelect );
        CPPUNIT_TEST( Cursor );
        CPPUNIT_TEST( Selection );
        CPPUNIT_TEST( AddRowCol );
        CPPUNIT_TEST( DeleteAndAddRowCol );
        CPPUNIT_TEST( ColumnOrder );
        CPPUNIT_TEST( ColumnVisibility );
        CPPUNIT_TEST( LineFormatting );
        CPPUNIT_TEST( SortSupport );
        CPPUNIT_TEST( Labels );
        CPPUNIT_TEST( SelectionMode );
        CPPUNIT_TEST( CellFormatting );
        WXUISIM_TEST( Editable );
        WXUISIM_TEST( ReadOnly );
        WXUISIM_TEST( ResizeScrolledHeader );
        WXUISIM_TEST( ColumnMinWidth );
        WXUISIM_TEST( AutoSizeColumn );
        CPPUNIT_TEST( PseudoTest_NativeHeader );
        WXUISIM_TEST( LabelClick );
        WXUISIM_TEST( SortClick );
        CPPUNIT_TEST( ColumnOrder );
        WXUISIM_TEST( ResizeScrolledHeader );
        WXUISIM_TEST( ColumnMinWidth );
        WXUISIM_TEST( AutoSizeColumn );
        CPPUNIT_TEST( DeleteAndAddRowCol );
        CPPUNIT_TEST( PseudoTest_NativeLabels );
        WXUISIM_TEST( LabelClick );
        WXUISIM_TEST( SortClick );
        CPPUNIT_TEST( ColumnOrder );
        WXUISIM_TEST( WindowAsEditorControl );
        WXUISIM_TEST( AutoSizeColumn );
    CPPUNIT_TEST_SUITE_END();

    void CellEdit();
    void CellClick();
    void ReorderedColumnsCellClick();
    void CellSelect();
    void LabelClick();
    void SortClick();
    void Size();
    void RangeSelect();
    void Cursor();
    void Selection();
    void AddRowCol();
    void DeleteAndAddRowCol();
    void ColumnOrder();
    void ColumnVisibility();
    void LineFormatting();
    void SortSupport();
    void Labels();
    void SelectionMode();
    void CellFormatting();
    void Editable();
    void ReadOnly();
    void WindowAsEditorControl();
    void ResizeScrolledHeader();
    void ColumnMinWidth();
    void AutoSizeColumn();
    void PseudoTest_NativeHeader() { ms_nativeheader = true; }
    void PseudoTest_NativeLabels() { ms_nativeheader = false;
                                     ms_nativelabels = true; }

    // The helper function to determine the width of the column label depending
    // on whether the native column is used.
    int GetColumnLabelWidth(wxClientDC& dc, int col, int margin) const
    {
        if (ms_nativeheader)
            return m_grid->GetGridColHeader()->GetColumnTitleWidth(col);

        int w, h;
        dc.GetMultiLineTextExtent(m_grid->GetColLabelValue(col), &w, &h);
        return w + margin;
    }

    void CheckFirstColAutoSize(int expected);

    static bool ms_nativeheader;
    static bool ms_nativelabels;

    wxGrid *m_grid;

    wxDECLARE_NO_COPY_CLASS(GridTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GridTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GridTestCase, "GridTestCase" );

//initialise the static variable
bool GridTestCase::ms_nativeheader = false;
bool GridTestCase::ms_nativelabels = false;

void GridTestCase::setUp()
{
    m_grid = new wxGrid(wxTheApp->GetTopWindow(), wxID_ANY);
    m_grid->CreateGrid(10, 2);
    m_grid->SetSize(400, 200);

    if( ms_nativeheader )
        m_grid->UseNativeColHeader();

    if( ms_nativelabels )
        m_grid->SetUseNativeColLabels();

    WaitForPaint waitForPaint(m_grid->GetGridWindow());

    m_grid->Refresh();
    m_grid->Update();

    if ( !waitForPaint.YieldUntilPainted() )
    {
        WARN("Grid not repainted until timeout expiration");
    }
}

void GridTestCase::tearDown()
{
    // This is just a hack to continue the rest of the tests to run: if we
    // destroy the header control while it has capture, this results in an
    // assert failure and while handling an exception from it more bad things
    // happen (as it's thrown from a dtor), resulting in simply aborting
    // everything. So ensure that it doesn't have capture in any case.
    //
    // Of course, the right thing to do would be to understand why does it
    // still have capture when the grid is destroyed sometimes.
    wxWindow* const win = wxWindow::GetCapture();
    if ( win )
        win->ReleaseMouse();

    wxDELETE(m_grid);
}

void GridTestCase::CellEdit()
{
    // TODO on OSX when running the grid test suite solo this works
    // but not when running it together with other tests
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXOSX__)
    EventCounter changing(m_grid, wxEVT_GRID_CELL_CHANGING);
    EventCounter changed(m_grid, wxEVT_GRID_CELL_CHANGED);
    EventCounter created(m_grid, wxEVT_GRID_EDITOR_CREATED);

    wxUIActionSimulator sim;

    m_grid->SetFocus();
    m_grid->SetGridCursor(1, 1);

    wxYield();

    sim.Text("abab");

    // We need to wait until the editor is really shown under GTK, consider
    // that it happens once it gets focus.
#ifdef __WXGTK__
    for ( wxStopWatch sw; wxWindow::FindFocus() == m_grid; )
    {
        if ( sw.Time() > 250 )
        {
            WARN("Editor control not shown until timeout expiration");
            break;
        }

        wxYield();
    }
#endif // __WXGTK__

    sim.Char(WXK_RETURN);

    wxYield();

#ifdef __WXGTK__
    for ( wxStopWatch sw; wxWindow::FindFocus() != m_grid; )
    {
        if ( sw.Time() > 250 )
        {
            WARN("Editor control not hidden until timeout expiration");
            break;
        }

        wxYield();
    }
#endif // __WXGTK__

    CHECK(m_grid->GetCellValue(1, 1) == "abab");

    CHECK(created.GetCount() == 1);
    CHECK(changing.GetCount() == 1);
    CHECK(changed.GetCount() == 1);
#endif
}

void GridTestCase::CellClick()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter lclick(m_grid, wxEVT_GRID_CELL_LEFT_CLICK);
    EventCounter ldclick(m_grid, wxEVT_GRID_CELL_LEFT_DCLICK);
    EventCounter rclick(m_grid, wxEVT_GRID_CELL_RIGHT_CLICK);
    EventCounter rdclick(m_grid, wxEVT_GRID_CELL_RIGHT_DCLICK);


    wxUIActionSimulator sim;

    wxRect rect = m_grid->CellToRect(0, 0);
    wxPoint point = m_grid->CalcScrolledPosition(rect.GetPosition());
    point = m_grid->ClientToScreen(point + wxPoint(m_grid->GetRowLabelSize(),
                                                   m_grid->GetColLabelSize())
                                         + wxPoint(2, 2));

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, lclick.GetCount());
    lclick.Clear();

    sim.MouseDblClick();
    wxYield();

    //A double click event sends a single click event first
    //test to ensure this still happens in the future
    CPPUNIT_ASSERT_EQUAL(1, lclick.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, ldclick.GetCount());

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());
    rclick.Clear();

    sim.MouseDblClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, rdclick.GetCount());
#endif
}

void GridTestCase::ReorderedColumnsCellClick()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter click(m_grid, wxEVT_GRID_CELL_LEFT_CLICK);

    wxUIActionSimulator sim;

    wxArrayInt neworder;
    neworder.push_back(1);
    neworder.push_back(0);

    m_grid->SetColumnsOrder(neworder);

    wxRect rect = m_grid->CellToRect(0, 1);
    wxPoint point = m_grid->CalcScrolledPosition(rect.GetPosition());
    point = m_grid->ClientToScreen(point + wxPoint(m_grid->GetRowLabelSize(),
        m_grid->GetColLabelSize())
        + wxPoint(2, 2));

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, click.GetCount());
#endif
}

void GridTestCase::CellSelect()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter cell(m_grid, wxEVT_GRID_SELECT_CELL);

    wxUIActionSimulator sim;

    wxRect rect = m_grid->CellToRect(0, 0);
    wxPoint point = m_grid->CalcScrolledPosition(rect.GetPosition());
    point = m_grid->ClientToScreen(point + wxPoint(m_grid->GetRowLabelSize(),
                                                   m_grid->GetColLabelSize())
                                         + wxPoint(4, 4));

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, cell.GetCount());

    cell.Clear();

    m_grid->SetGridCursor(1, 1);
    m_grid->GoToCell(1, 0);

    sim.MouseMove(point);
    wxYield();

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(3, cell.GetCount());
#endif
}

void GridTestCase::LabelClick()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter lclick(m_grid, wxEVT_GRID_LABEL_LEFT_CLICK);
    EventCounter ldclick(m_grid, wxEVT_GRID_LABEL_LEFT_DCLICK);
    EventCounter rclick(m_grid, wxEVT_GRID_LABEL_RIGHT_CLICK);
    EventCounter rdclick(m_grid, wxEVT_GRID_LABEL_RIGHT_DCLICK);

    wxUIActionSimulator sim;

    wxPoint pos(m_grid->GetRowLabelSize() + 2, 2);
    pos = m_grid->ClientToScreen(pos);

    sim.MouseMove(pos);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, lclick.GetCount());

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, ldclick.GetCount());

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());
    rclick.Clear();

    sim.MouseDblClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    if( ms_nativeheader )
    {
        //Right double click not supported with native headers so we get two
        //right click events
        CPPUNIT_ASSERT_EQUAL(2, rclick.GetCount());
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());
        CPPUNIT_ASSERT_EQUAL(1, rdclick.GetCount());
    }
#endif
}

void GridTestCase::SortClick()
{
#if wxUSE_UIACTIONSIMULATOR
    m_grid->SetSortingColumn(0);

    EventCounter sort(m_grid, wxEVT_GRID_COL_SORT);

    wxUIActionSimulator sim;

    wxPoint pos(m_grid->GetRowLabelSize() + 4, 4);
    pos = m_grid->ClientToScreen(pos);

    sim.MouseMove(pos);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, sort.GetCount());
#endif
}

void GridTestCase::Size()
{
    // TODO on OSX resizing interactively works, but not automated
    // Grid could not pass the test under GTK, OSX, and Universal.
    // So there may has bug in Grid implementation
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXGTK__) && !defined(__WXOSX__) \
&& !defined(__WXUNIVERSAL__)
    EventCounter colsize(m_grid, wxEVT_GRID_COL_SIZE);
    EventCounter rowsize(m_grid, wxEVT_GRID_ROW_SIZE);

    wxUIActionSimulator sim;

    wxPoint pt = m_grid->ClientToScreen(wxPoint(m_grid->GetRowLabelSize() +
                                        m_grid->GetColSize(0), 5));

    sim.MouseMove(pt);
    wxYield();

    sim.MouseDown();
    wxYield();

    sim.MouseMove(pt.x + 50, pt.y);
    wxYield();

    sim.MouseUp();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, colsize.GetCount());

    pt = m_grid->ClientToScreen(wxPoint(5, m_grid->GetColLabelSize() +
                                        m_grid->GetRowSize(0)));

    sim.MouseDragDrop(pt.x, pt.y, pt.x, pt.y + 50);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, rowsize.GetCount());
#endif
}

void GridTestCase::RangeSelect()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter select(m_grid, wxEVT_GRID_RANGE_SELECT);

    wxUIActionSimulator sim;

    //We add the extra 10 to ensure that we are inside the cell
    wxPoint pt = m_grid->ClientToScreen(wxPoint(m_grid->GetRowLabelSize() + 10,
                                                m_grid->GetColLabelSize() + 10)
                                                );

    sim.MouseMove(pt);
    wxYield();

    sim.MouseDown();
    wxYield();

    sim.MouseMove(pt.x + 50, pt.y + 50);
    wxYield();

    sim.MouseUp();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, select.GetCount());
#endif
}

void GridTestCase::Cursor()
{
    m_grid->SetGridCursor(1, 1);

    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetGridCursorCol());
    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetGridCursorRow());

    m_grid->MoveCursorDown(false);
    m_grid->MoveCursorLeft(false);
    m_grid->MoveCursorUp(false);
    m_grid->MoveCursorUp(false);
    m_grid->MoveCursorRight(false);

    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetGridCursorCol());
    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetGridCursorRow());

    m_grid->SetCellValue(0, 0, "some text");
    m_grid->SetCellValue(3, 0, "other text");
    m_grid->SetCellValue(0, 1, "more text");
    m_grid->SetCellValue(3, 1, "extra text");

    m_grid->Update();
    m_grid->Refresh();

    m_grid->MoveCursorLeftBlock(false);

    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetGridCursorCol());
    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetGridCursorRow());

    m_grid->MoveCursorDownBlock(false);

    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetGridCursorCol());
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetGridCursorRow());

    m_grid->MoveCursorRightBlock(false);

    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetGridCursorCol());
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetGridCursorRow());

    m_grid->MoveCursorUpBlock(false);

    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetGridCursorCol());
    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetGridCursorRow());
}

void GridTestCase::Selection()
{
    m_grid->SelectAll();

    CPPUNIT_ASSERT(m_grid->IsSelection());
    CPPUNIT_ASSERT(m_grid->IsInSelection(0, 0));
    CPPUNIT_ASSERT(m_grid->IsInSelection(9, 1));

    m_grid->SelectBlock(1, 0, 3, 1);

    wxGridCellCoordsArray topleft = m_grid->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray bottomright = m_grid->GetSelectionBlockBottomRight();

    CPPUNIT_ASSERT_EQUAL(1, topleft.Count());
    CPPUNIT_ASSERT_EQUAL(1, bottomright.Count());

    CPPUNIT_ASSERT_EQUAL(0, topleft.Item(0).GetCol());
    CPPUNIT_ASSERT_EQUAL(1, topleft.Item(0).GetRow());
    CPPUNIT_ASSERT_EQUAL(1, bottomright.Item(0).GetCol());
    CPPUNIT_ASSERT_EQUAL(3, bottomright.Item(0).GetRow());

    m_grid->SelectCol(1);

    CPPUNIT_ASSERT(m_grid->IsInSelection(0, 1));
    CPPUNIT_ASSERT(m_grid->IsInSelection(9, 1));
    CPPUNIT_ASSERT(!m_grid->IsInSelection(3, 0));

    m_grid->SelectRow(4);

    CPPUNIT_ASSERT(m_grid->IsInSelection(4, 0));
    CPPUNIT_ASSERT(m_grid->IsInSelection(4, 1));
    CPPUNIT_ASSERT(!m_grid->IsInSelection(3, 0));
}

void GridTestCase::AddRowCol()
{
    CPPUNIT_ASSERT_EQUAL(10, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(2, m_grid->GetNumberCols());

    m_grid->AppendCols();
    m_grid->AppendRows();

    CPPUNIT_ASSERT_EQUAL(11, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetNumberCols());

    m_grid->AppendCols(2);
    m_grid->AppendRows(2);

    CPPUNIT_ASSERT_EQUAL(13, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(5, m_grid->GetNumberCols());

    m_grid->InsertCols(1, 2);
    m_grid->InsertRows(2, 3);

    CPPUNIT_ASSERT_EQUAL(16, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(7, m_grid->GetNumberCols());
}

void GridTestCase::DeleteAndAddRowCol()
{
    CPPUNIT_ASSERT_EQUAL(10, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(2, m_grid->GetNumberCols());

    m_grid->DeleteRows(0, 10);
    m_grid->DeleteCols(0, 2);

    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetNumberCols());

    m_grid->AppendRows(5);
    m_grid->AppendCols(3);

    CPPUNIT_ASSERT_EQUAL(5, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetNumberCols());

    // The order of functions calls can be important
    m_grid->DeleteCols(0, 3);
    m_grid->DeleteRows(0, 5);

    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetNumberRows());
    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetNumberCols());

    // Different functions calls order
    m_grid->AppendCols(3);
    m_grid->AppendRows(5);
}

void GridTestCase::ColumnOrder()
{
    m_grid->AppendCols(2);

    CPPUNIT_ASSERT_EQUAL(4, m_grid->GetNumberCols());

    wxArrayInt neworder;
    neworder.push_back(1);
    neworder.push_back(3);
    neworder.push_back(2);
    neworder.push_back(0);

    m_grid->SetColumnsOrder(neworder);

    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetColPos(1));
    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetColPos(3));
    CPPUNIT_ASSERT_EQUAL(2, m_grid->GetColPos(2));
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetColPos(0));

    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetColAt(0));
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetColAt(1));
    CPPUNIT_ASSERT_EQUAL(2, m_grid->GetColAt(2));
    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetColAt(3));

    m_grid->ResetColPos();

    CPPUNIT_ASSERT_EQUAL(0, m_grid->GetColPos(0));
    CPPUNIT_ASSERT_EQUAL(1, m_grid->GetColPos(1));
    CPPUNIT_ASSERT_EQUAL(2, m_grid->GetColPos(2));
    CPPUNIT_ASSERT_EQUAL(3, m_grid->GetColPos(3));
}

void GridTestCase::ColumnVisibility()
{
    m_grid->AppendCols(3);
    CPPUNIT_ASSERT( m_grid->IsColShown(1) );

    m_grid->HideCol(1);
    CPPUNIT_ASSERT( !m_grid->IsColShown(1) );
    CPPUNIT_ASSERT( m_grid->IsColShown(2) );

    m_grid->ShowCol(1);
    CPPUNIT_ASSERT( m_grid->IsColShown(1) );
}

void GridTestCase::LineFormatting()
{
    CPPUNIT_ASSERT(m_grid->GridLinesEnabled());

    m_grid->EnableGridLines(false);

    CPPUNIT_ASSERT(!m_grid->GridLinesEnabled());

    m_grid->EnableGridLines();

    m_grid->SetGridLineColour(*wxRED);

    CPPUNIT_ASSERT_EQUAL(m_grid->GetGridLineColour(), *wxRED);
}

void GridTestCase::SortSupport()
{
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, m_grid->GetSortingColumn());

    m_grid->SetSortingColumn(1);

    CPPUNIT_ASSERT(!m_grid->IsSortingBy(0));
    CPPUNIT_ASSERT(m_grid->IsSortingBy(1));
    CPPUNIT_ASSERT(m_grid->IsSortOrderAscending());

    m_grid->SetSortingColumn(0, false);

    CPPUNIT_ASSERT(m_grid->IsSortingBy(0));
    CPPUNIT_ASSERT(!m_grid->IsSortingBy(1));
    CPPUNIT_ASSERT(!m_grid->IsSortOrderAscending());

    m_grid->UnsetSortingColumn();

    CPPUNIT_ASSERT(!m_grid->IsSortingBy(0));
    CPPUNIT_ASSERT(!m_grid->IsSortingBy(1));
}

void GridTestCase::Labels()
{
    CPPUNIT_ASSERT_EQUAL("A", m_grid->GetColLabelValue(0));
    CPPUNIT_ASSERT_EQUAL("1", m_grid->GetRowLabelValue(0));

    m_grid->SetColLabelValue(0, "Column 1");
    m_grid->SetRowLabelValue(0, "Row 1");

    CPPUNIT_ASSERT_EQUAL("Column 1", m_grid->GetColLabelValue(0));
    CPPUNIT_ASSERT_EQUAL("Row 1", m_grid->GetRowLabelValue(0));

    m_grid->SetLabelTextColour(*wxGREEN);
    m_grid->SetLabelBackgroundColour(*wxRED);

    CPPUNIT_ASSERT_EQUAL(*wxGREEN, m_grid->GetLabelTextColour());
    CPPUNIT_ASSERT_EQUAL(*wxRED, m_grid->GetLabelBackgroundColour());

    m_grid->SetColLabelTextOrientation(wxVERTICAL);

    CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxVERTICAL),
                         static_cast<int>(m_grid->GetColLabelTextOrientation()));
}

void GridTestCase::SelectionMode()
{
    //We already test this mode in Select
    CPPUNIT_ASSERT_EQUAL(wxGrid::wxGridSelectCells,
                         m_grid->GetSelectionMode());

    //Test row selection be selecting a single cell and checking the whole
    //row is selected
    m_grid->SetSelectionMode(wxGrid::wxGridSelectRows);
    m_grid->SelectBlock(3, 1, 3, 1);

    wxArrayInt selectedRows = m_grid->GetSelectedRows();
    CPPUNIT_ASSERT_EQUAL(1, selectedRows.Count());
    CPPUNIT_ASSERT_EQUAL(3, selectedRows[0]);

    CPPUNIT_ASSERT_EQUAL(wxGrid::wxGridSelectRows,
                         m_grid->GetSelectionMode());


    //Test column selection be selecting a single cell and checking the whole
    //column is selected
    m_grid->SetSelectionMode(wxGrid::wxGridSelectColumns);
    m_grid->SelectBlock(3, 1, 3, 1);

    wxArrayInt selectedCols = m_grid->GetSelectedCols();
    CPPUNIT_ASSERT_EQUAL(1, selectedCols.Count());
    CPPUNIT_ASSERT_EQUAL(1, selectedCols[0]);

    CPPUNIT_ASSERT_EQUAL(wxGrid::wxGridSelectColumns,
                         m_grid->GetSelectionMode());
}

void GridTestCase::CellFormatting()
{
    //Check that initial alignment is default
    int horiz, cellhoriz, vert, cellvert;

    m_grid->GetDefaultCellAlignment(&horiz, &vert);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CPPUNIT_ASSERT_EQUAL(cellhoriz, horiz);
    CPPUNIT_ASSERT_EQUAL(cellvert, vert);

    //Check initial text colour and background colour are default
    wxColour text, back;

    back = m_grid->GetDefaultCellBackgroundColour();

    CPPUNIT_ASSERT_EQUAL(back, m_grid->GetCellBackgroundColour(0, 0));

    back = m_grid->GetDefaultCellTextColour();

    CPPUNIT_ASSERT_EQUAL(back, m_grid->GetCellTextColour(0, 0));

#if WXWIN_COMPATIBILITY_2_8
    m_grid->SetCellAlignment(wxALIGN_CENTRE, 0, 0);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxALIGN_CENTRE), cellhoriz);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxALIGN_CENTRE), cellvert);
#endif // WXWIN_COMPATIBILITY_2_8

    m_grid->SetCellAlignment(0, 0, wxALIGN_LEFT, wxALIGN_BOTTOM);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxALIGN_LEFT), cellhoriz);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxALIGN_BOTTOM), cellvert);

#if WXWIN_COMPATIBILITY_2_8
    m_grid->SetCellTextColour(*wxRED, 0, 0);
    CPPUNIT_ASSERT_EQUAL(*wxRED, m_grid->GetCellTextColour(0, 0));
#endif // WXWIN_COMPATIBILITY_2_8

    m_grid->SetCellTextColour(0, 0, *wxGREEN);
    CPPUNIT_ASSERT_EQUAL(*wxGREEN, m_grid->GetCellTextColour(0, 0));
}

void GridTestCase::Editable()
{
#if wxUSE_UIACTIONSIMULATOR
    //As the grid is not editable we shouldn't create an editor
    EventCounter created(m_grid, wxEVT_GRID_EDITOR_CREATED);

    wxUIActionSimulator sim;

    CPPUNIT_ASSERT(m_grid->IsEditable());

    m_grid->EnableEditing(false);

    CPPUNIT_ASSERT(!m_grid->IsEditable());

    m_grid->SetFocus();
    m_grid->SetGridCursor(1, 1);

    sim.Text("abab");
    wxYield();

    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, created.GetCount());
#endif
}

void GridTestCase::ReadOnly()
{
#if wxUSE_UIACTIONSIMULATOR
    //As the cell is readonly we shouldn't create an editor
    EventCounter created(m_grid, wxEVT_GRID_EDITOR_CREATED);

    wxUIActionSimulator sim;

    CPPUNIT_ASSERT(!m_grid->IsReadOnly(1, 1));

    m_grid->SetReadOnly(1, 1);

    CPPUNIT_ASSERT(m_grid->IsReadOnly(1, 1));

    m_grid->SetFocus();

#ifdef __WXGTK__
    // This is a mystery, but we somehow get WXK_RETURN generated by the
    // previous test (Editable) in this one. In spite of wxYield() in that
    // test, the key doesn't get dispatched there and we have to consume it
    // here before setting the current grid cell, as getting WXK_RETURN later
    // would move the selection down, to a non read-only cell.
    wxYield();
#endif // __WXGTK__

    m_grid->SetGridCursor(1, 1);

    CPPUNIT_ASSERT(m_grid->IsCurrentCellReadOnly());

    sim.Text("abab");
    wxYield();

    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, created.GetCount());
#endif
}

void GridTestCase::WindowAsEditorControl()
{
#if wxUSE_UIACTIONSIMULATOR
    // A very simple editor using a window not derived from wxControl as the
    // editor.
    class TestEditor : public wxGridCellEditor
    {
    public:
        TestEditor() {}

        void Create(wxWindow* parent,
                    wxWindowID id,
                    wxEvtHandler* evtHandler) wxOVERRIDE
        {
            SetWindow(new wxWindow(parent, id));
            wxGridCellEditor::Create(parent, id, evtHandler);
        }

        void BeginEdit(int, int, wxGrid*) wxOVERRIDE {}

        bool EndEdit(int, int, wxGrid const*, wxString const&,
                     wxString* newval) wxOVERRIDE
        {
            *newval = GetValue();
            return true;
        }

        void ApplyEdit(int row, int col, wxGrid* grid) wxOVERRIDE
        {
            grid->GetTable()->SetValue(row, col, GetValue());
        }

        void Reset() wxOVERRIDE {}

        wxGridCellEditor* Clone() const wxOVERRIDE { return new TestEditor(); }

        wxString GetValue() const wxOVERRIDE { return "value"; }
    };

    wxGridCellAttr* attr = new wxGridCellAttr();
    attr->SetRenderer(new wxGridCellStringRenderer());
    attr->SetEditor(new TestEditor());
    m_grid->SetAttr(1, 1, attr);

    EventCounter created(m_grid, wxEVT_GRID_EDITOR_CREATED);

    wxUIActionSimulator sim;

    m_grid->SetFocus();
    m_grid->SetGridCursor(1, 1);
    m_grid->EnableCellEditControl();

    sim.Char(WXK_RETURN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, created.GetCount());
#endif
}

void GridTestCase::ResizeScrolledHeader()
{
    // TODO this test currently works only under Windows unfortunately
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__)
    int const startwidth = m_grid->GetColSize(0);
    int const draglength = 100;

    m_grid->AppendCols(8);
    m_grid->Scroll(5, 0);
    m_grid->Refresh();
    m_grid->Update();

    wxRect rect = m_grid->CellToRect(0, 1);
    wxPoint point = m_grid->CalcScrolledPosition(rect.GetPosition());
    point = m_grid->ClientToScreen(point
                                   + wxPoint(m_grid->GetRowLabelSize(),
                                             m_grid->GetColLabelSize())
                                   - wxPoint(0, 5));

    wxUIActionSimulator sim;

    wxYield();
    sim.MouseMove(point);

    wxYield();
    sim.MouseDown();

    wxYield();
    sim.MouseMove(point + wxPoint(draglength, 0));

    wxYield();
    sim.MouseUp();

    wxYield();

    CPPUNIT_ASSERT_EQUAL(startwidth + draglength, m_grid->GetColSize(0));
#endif
}

void GridTestCase::ColumnMinWidth()
{
    // TODO this test currently works only under Windows unfortunately
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__)
    int const startminwidth = m_grid->GetColMinimalAcceptableWidth();
    m_grid->SetColMinimalAcceptableWidth(startminwidth*2);
    int const newminwidth = m_grid->GetColMinimalAcceptableWidth();
    int const startwidth = m_grid->GetColSize(0);

    CPPUNIT_ASSERT(m_grid->GetColMinimalAcceptableWidth() < startwidth);

    wxRect rect = m_grid->CellToRect(0, 1);
    wxPoint point = m_grid->CalcScrolledPosition(rect.GetPosition());
    point = m_grid->ClientToScreen(point
                                   + wxPoint(m_grid->GetRowLabelSize(),
                                             m_grid->GetColLabelSize())
                                   - wxPoint(0, 5));

    wxUIActionSimulator sim;

    // Drag to reach the minimal width.
    wxYield();
    sim.MouseMove(point);
    wxYield();
    sim.MouseDown();
    wxYield();
    sim.MouseMove(point - wxPoint(startwidth - startminwidth, 0));
    wxYield();
    sim.MouseUp();
    wxYield();

    if ( ms_nativeheader )
        CPPUNIT_ASSERT_EQUAL(startwidth, m_grid->GetColSize(0));
    else
        CPPUNIT_ASSERT_EQUAL(newminwidth, m_grid->GetColSize(0));
#endif
}

void GridTestCase::CheckFirstColAutoSize(int expected)
{
    m_grid->AutoSizeColumn(0);

    wxYield();
    CHECK(m_grid->GetColSize(0) == expected);
}

void GridTestCase::AutoSizeColumn()
{
    // Hardcoded extra margin for the columns used in grid.cpp.
    const int margin = m_grid->FromDIP(10);

    wxGridCellAttr *attr = m_grid->GetOrCreateCellAttr(0, 0);
    wxGridCellRenderer *renderer = attr->GetRenderer(m_grid, 0, 0);
    REQUIRE(renderer != NULL);

    wxClientDC dcCell(m_grid->GetGridWindow());

    wxClientDC dcLabel(m_grid->GetGridWindow());
    dcLabel.SetFont(m_grid->GetLabelFont());

    const wxString shortStr     = "W";
    const wxString mediumStr    = "WWWW";
    const wxString longStr      = "WWWWWWWW";
    const wxString multilineStr = mediumStr + "\n" + longStr;

    SECTION("Empty column and label")
    {
        m_grid->SetColLabelValue(0, wxString());
        CheckFirstColAutoSize( m_grid->GetDefaultColSize() );
    }

    SECTION("Empty column with label")
    {
        m_grid->SetColLabelValue(0, mediumStr);
        CheckFirstColAutoSize( GetColumnLabelWidth(dcLabel, 0, margin) );
    }

    SECTION("Column with empty label")
    {
        m_grid->SetColLabelValue(0, wxString());
        m_grid->SetCellValue(0, 0, mediumStr);
        m_grid->SetCellValue(1, 0, shortStr);
        m_grid->SetCellValue(3, 0, longStr);

        CheckFirstColAutoSize(
            renderer->GetBestWidth(*m_grid, *attr, dcCell, 3, 0,
                                   m_grid->GetRowHeight(3)) + margin );
    }

    SECTION("Column with label longer than contents")
    {
        m_grid->SetColLabelValue(0, multilineStr);
        m_grid->SetCellValue(0, 0, mediumStr);
        m_grid->SetCellValue(1, 0, shortStr);
        CheckFirstColAutoSize( GetColumnLabelWidth(dcLabel, 0, margin) );
    }

    SECTION("Column with contents longer than label")
    {
        m_grid->SetColLabelValue(0, mediumStr);
        m_grid->SetCellValue(0, 0, mediumStr);
        m_grid->SetCellValue(1, 0, shortStr);
        m_grid->SetCellValue(3, 0, multilineStr);
        CheckFirstColAutoSize(
            renderer->GetBestWidth(*m_grid, *attr, dcCell, 3, 0,
                                   m_grid->GetRowHeight(3)) + margin );
    }

    SECTION("Column with equally sized contents and label")
    {
        m_grid->SetColLabelValue(0, mediumStr);
        m_grid->SetCellValue(0, 0, mediumStr);
        m_grid->SetCellValue(1, 0, mediumStr);
        m_grid->SetCellValue(3, 0, mediumStr);

        const int labelWidth = GetColumnLabelWidth(dcLabel, 0, margin);

        const int cellWidth =
            renderer->GetBestWidth(*m_grid, *attr, dcCell, 3, 0,
                                   m_grid->GetRowHeight(3))
            + margin;

        // We can't be sure which size will be greater because of different fonts
        // so just calculate the maximum width.
        CheckFirstColAutoSize( wxMax(labelWidth, cellWidth) );
    }
}

#endif //wxUSE_GRID
