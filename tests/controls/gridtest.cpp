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

namespace
{

// Derive a new class inheriting from wxGrid just to get access to its
// protected GetCellAttr(). This is not pretty, but we don't have any other way
// of testing this function.
class TestableGrid : public wxGrid
{
public:
    explicit TestableGrid(wxWindow* parent)
        : wxGrid(parent, wxID_ANY)
    {
    }

    wxGridCellAttr* CallGetCellAttr(int row, int col) const
    {
        return GetCellAttr(row, col);
    }
};

} // anonymous namespace

class GridTestCase
{
public:
    GridTestCase();
    ~GridTestCase();

protected:
    // The helper function to determine the width of the column label depending
    // on whether the native column header is used.
    int GetColumnLabelWidth(wxClientDC& dc, int col, int margin) const
    {
        if ( m_grid->IsUsingNativeHeader() )
            return m_grid->GetGridColHeader()->GetColumnTitleWidth(col);

        int w, h;
        dc.GetMultiLineTextExtent(m_grid->GetColLabelValue(col), &w, &h);
        return w + margin;
    }

    void CheckFirstColAutoSize(int expected);

    TestableGrid *m_grid;

    wxDECLARE_NO_COPY_CLASS(GridTestCase);
};

GridTestCase::GridTestCase()
{
    m_grid = new TestableGrid(wxTheApp->GetTopWindow());
    m_grid->CreateGrid(10, 2);
    m_grid->SetSize(400, 200);

    WaitForPaint waitForPaint(m_grid->GetGridWindow());

    m_grid->Refresh();
    m_grid->Update();

    if ( !waitForPaint.YieldUntilPainted() )
    {
        WARN("Grid not repainted until timeout expiration");
    }
}

GridTestCase::~GridTestCase()
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

TEST_CASE_METHOD(GridTestCase, "Grid::CellEdit", "[grid]")
{
    // TODO on OSX when running the grid test suite solo this works
    // but not when running it together with other tests
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXOSX__)
    if ( !EnableUITests() )
        return;

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

TEST_CASE_METHOD(GridTestCase, "Grid::CellClick", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::ReorderedColumnsCellClick", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::CellSelect", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::LabelClick", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }
    SECTION("Native labels") { m_grid->SetUseNativeColLabels(); }

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

    if ( m_grid->IsUsingNativeHeader() )
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

TEST_CASE_METHOD(GridTestCase, "Grid::SortClick", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }
    SECTION("Native labels") { m_grid->SetUseNativeColLabels(); }

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

TEST_CASE_METHOD(GridTestCase, "Grid::Size", "[grid]")
{
    // TODO on OSX resizing interactively works, but not automated
    // Grid could not pass the test under GTK, OSX, and Universal.
    // So there may has bug in Grid implementation
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXGTK__) && !defined(__WXOSX__) \
&& !defined(__WXUNIVERSAL__)
    if ( !EnableUITests() )
        return;

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

TEST_CASE_METHOD(GridTestCase, "Grid::RangeSelect", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

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

TEST_CASE_METHOD(GridTestCase, "Grid::Cursor", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::Selection", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::SelectEmptyGrid", "[grid]")
{
    SECTION("Delete rows/columns")
    {
        SECTION("No rows")
        {
            m_grid->DeleteRows(0, 10);
            REQUIRE( m_grid->GetNumberRows() == 0 );
        }
        SECTION("No columns")
        {
            m_grid->DeleteCols(0, 2);
            REQUIRE( m_grid->GetNumberCols() == 0 );
        }
    }

    SECTION("Select")
    {
        SECTION("Move right")
        {
            m_grid->MoveCursorRight(true);
        }
        SECTION("Move down")
        {
            m_grid->MoveCursorDown(true);
        }
        SECTION("Select row")
        {
            m_grid->SelectRow(1);
        }
        SECTION("Select column")
        {
            m_grid->SelectCol(1);
        }
    }

    CHECK( m_grid->GetSelectionBlockTopLeft().Count() == 0 );
    CHECK( m_grid->GetSelectionBlockBottomRight().Count() == 0 );
}

TEST_CASE_METHOD(GridTestCase, "Grid::ScrollWhenSelect", "[grid]")
{
    m_grid->AppendCols(10);

    REQUIRE( m_grid->GetGridCursorCol() == 0 );
    REQUIRE( m_grid->GetGridCursorRow() == 0 );
    REQUIRE( m_grid->IsVisible(0, 0) );
    REQUIRE( !m_grid->IsVisible(0, 4) );

    for ( int i = 0; i < 4; ++i )
    {
        m_grid->MoveCursorRight(true);
    }
    CHECK( m_grid->IsVisible(0, 4) );

    m_grid->ClearSelection();
    m_grid->SetGridCursor(1, 1);
    for ( int i = 0; i < 5; ++i )
    {
        m_grid->MoveCursorDown(true);
    }
    CHECK( m_grid->IsVisible(6, 1) );
}

TEST_CASE_METHOD(GridTestCase, "Grid::MoveGridCursorUsingEndKey", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;

    m_grid->AppendCols(10);

    REQUIRE( m_grid->GetGridCursorCol() == 0 );
    REQUIRE( m_grid->GetGridCursorRow() == 0 );
    REQUIRE( m_grid->IsVisible(0, 0) );

    // Hide the last row.
    m_grid->HideRow(9);
    // Hide the last column.
    m_grid->HideCol(11);
    // Move the penult column.
    m_grid->SetColPos(10, 5);

    m_grid->SetFocus();

    sim.KeyDown(WXK_END, wxMOD_CONTROL);
    sim.KeyUp(WXK_END, wxMOD_CONTROL);
    wxYield();

    CHECK( m_grid->GetGridCursorRow() == 8 );
    CHECK( m_grid->GetGridCursorCol() == 9 );
    CHECK( m_grid->IsVisible(8, 9) );
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::SelectUsingEndKey", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    wxUIActionSimulator sim;

    m_grid->AppendCols(10);

    REQUIRE( m_grid->GetGridCursorCol() == 0 );
    REQUIRE( m_grid->GetGridCursorRow() == 0 );
    REQUIRE( m_grid->IsVisible(0, 0) );

    m_grid->SetFocus();

    sim.KeyDown(WXK_END, wxMOD_CONTROL | wxMOD_SHIFT);
    sim.KeyUp(WXK_END, wxMOD_CONTROL | wxMOD_SHIFT);
    wxYield();

    wxGridCellCoordsArray topleft = m_grid->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray bottomright = m_grid->GetSelectionBlockBottomRight();

    CHECK( topleft.Count() == 1 );
    CHECK( bottomright.Count() == 1 );

    CHECK( topleft.Item(0).GetCol() == 0 );
    CHECK( topleft.Item(0).GetRow() == 0 );
    CHECK( bottomright.Item(0).GetCol() == 11 );
    CHECK( bottomright.Item(0).GetRow() == 9 );

    CHECK( m_grid->IsVisible(8, 9) );
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::AddRowCol", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::DeleteAndAddRowCol", "[grid]")
{
    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }

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

TEST_CASE_METHOD(GridTestCase, "Grid::ColumnOrder", "[grid]")
{
    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }
    SECTION("Native labels") { m_grid->SetUseNativeColLabels(); }

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

TEST_CASE_METHOD(GridTestCase, "Grid::ColumnVisibility", "[grid]")
{
    m_grid->AppendCols(3);
    CPPUNIT_ASSERT( m_grid->IsColShown(1) );

    m_grid->HideCol(1);
    CPPUNIT_ASSERT( !m_grid->IsColShown(1) );
    CPPUNIT_ASSERT( m_grid->IsColShown(2) );

    m_grid->ShowCol(1);
    CPPUNIT_ASSERT( m_grid->IsColShown(1) );
}

TEST_CASE_METHOD(GridTestCase, "Grid::LineFormatting", "[grid]")
{
    CPPUNIT_ASSERT(m_grid->GridLinesEnabled());

    m_grid->EnableGridLines(false);

    CPPUNIT_ASSERT(!m_grid->GridLinesEnabled());

    m_grid->EnableGridLines();

    m_grid->SetGridLineColour(*wxRED);

    CPPUNIT_ASSERT_EQUAL(m_grid->GetGridLineColour(), *wxRED);
}

TEST_CASE_METHOD(GridTestCase, "Grid::SortSupport", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::Labels", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::SelectionMode", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::CellFormatting", "[grid]")
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

TEST_CASE_METHOD(GridTestCase, "Grid::GetNonDefaultAlignment", "[grid]")
{
    // GetNonDefaultAlignment() is used by several renderers having their own
    // preferred alignment, so check that if we don't reset the alignment
    // explicitly, it doesn't override the alignment used by default.
    wxGridCellAttr* attr = NULL;
    int hAlign = wxALIGN_RIGHT,
        vAlign = wxALIGN_INVALID;

    attr = m_grid->CallGetCellAttr(0, 0);
    REQUIRE( attr );

    // Check that the specified alignment is preserved, while the unspecified
    // component is filled with the default value (which is "top" by default).
    attr->GetNonDefaultAlignment(&hAlign, &vAlign);
    CHECK( hAlign == wxALIGN_RIGHT );
    CHECK( vAlign == wxALIGN_TOP );

    // Now change the defaults and check that the unspecified alignment
    // component is filled with the new default.
    m_grid->SetDefaultCellAlignment(wxALIGN_CENTRE_HORIZONTAL,
                                    wxALIGN_CENTRE_VERTICAL);

    vAlign = wxALIGN_INVALID;

    attr = m_grid->CallGetCellAttr(0, 0);
    REQUIRE( attr );

    attr->GetNonDefaultAlignment(&hAlign, &vAlign);
    CHECK( hAlign == wxALIGN_RIGHT );
    CHECK( vAlign == wxALIGN_CENTRE_VERTICAL );
}

TEST_CASE_METHOD(GridTestCase, "Grid::Editable", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

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

TEST_CASE_METHOD(GridTestCase, "Grid::ReadOnly", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

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

TEST_CASE_METHOD(GridTestCase, "Grid::WindowAsEditorControl", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

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

TEST_CASE_METHOD(GridTestCase, "Grid::ResizeScrolledHeader", "[grid]")
{
    // TODO this test currently works only under Windows unfortunately
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__)
    if ( !EnableUITests() )
        return;

    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }

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

TEST_CASE_METHOD(GridTestCase, "Grid::ColumnMinWidth", "[grid]")
{
    // TODO this test currently works only under Windows unfortunately
#if wxUSE_UIACTIONSIMULATOR && defined(__WXMSW__)
    if ( !EnableUITests() )
        return;

    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }

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

TEST_CASE_METHOD(GridTestCase, "Grid::AutoSizeColumn", "[grid]")
{
    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }
    SECTION("Native labels") { m_grid->SetUseNativeColLabels(); }

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
