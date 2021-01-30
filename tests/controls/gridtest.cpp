///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/gridtest.cpp
// Purpose:     wxGrid unit test
// Author:      Steven Lamerton
// Created:     2010-06-25
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_GRID


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

    // Helper to check that the selection is equal to the specified block.
    void CheckSelection(const wxGridBlockCoords& block)
    {
        const wxGridBlocks selected = m_grid->GetSelectedBlocks();
        wxGridBlocks::iterator it = selected.begin();

        REQUIRE( it != selected.end() );
        CHECK( *it == block );
        CHECK( ++it == selected.end() );
    }

    // Or specified ranges.
    struct RowRange
    {
        RowRange(int top, int bottom) : top(top), bottom(bottom) { }

        int top, bottom;
    };

    typedef wxVector<RowRange> RowRanges;

    void CheckRowSelection(const RowRanges& ranges)
    {
        const wxGridBlockCoordsVector sel = m_grid->GetSelectedRowBlocks();
        REQUIRE( sel.size() == ranges.size() );

        for ( size_t n = 0; n < sel.size(); ++n )
        {
            INFO("n = " << n);

            const RowRange& r = ranges[n];
            CHECK( sel[n] == wxGridBlockCoords(r.top, 0, r.bottom, 1) );
        }
    }

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

    waitForPaint.YieldUntilPainted();
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

    wxYield();

    sim.Char(WXK_RETURN);

    wxYield();

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

    CHECK(lclick.GetCount() == 1);
    lclick.Clear();

    sim.MouseDblClick();
    wxYield();

    //A double click event sends a single click event first
    //test to ensure this still happens in the future
    CHECK(lclick.GetCount() == 1);
    CHECK(ldclick.GetCount() == 1);

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CHECK(rclick.GetCount() == 1);
    rclick.Clear();

    sim.MouseDblClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CHECK(rclick.GetCount() == 1);
    CHECK(rdclick.GetCount() == 1);
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

    CHECK(click.GetCount() == 1);
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

    CHECK(cell.GetCount() == 1);

    cell.Clear();

    m_grid->SetGridCursor(1, 1);
    m_grid->GoToCell(1, 0);

    sim.MouseMove(point);
    wxYield();

    sim.MouseDblClick();
    wxYield();

    CHECK(cell.GetCount() == 3);
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

    CHECK(lclick.GetCount() == 1);

    sim.MouseDblClick();
    wxYield();

    CHECK(ldclick.GetCount() == 1);

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CHECK(rclick.GetCount() == 1);
    rclick.Clear();

    sim.MouseDblClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    if ( m_grid->IsUsingNativeHeader() )
    {
        //Right double click not supported with native headers so we get two
        //right click events
        CHECK(rclick.GetCount() == 2);
    }
    else
    {
        CHECK(rclick.GetCount() == 1);
        CHECK(rdclick.GetCount() == 1);
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

    CHECK(sort.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::Size", "[grid]")
{
    // TODO on OSX resizing interactively works, but not automated
    // Grid could not pass the test under GTK, OSX, and Universal.
    // So there may has bug in Grid implementation
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXOSX__) && !defined(__WXUNIVERSAL__)
    if ( !EnableUITests() )
        return;

#ifdef __WXGTK20__
    // Works locally, but not when run on Travis CI.
    if ( IsAutomaticTest() )
        return;
#endif

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

    CHECK(colsize.GetCount() == 1);

    pt = m_grid->ClientToScreen(wxPoint(5, m_grid->GetColLabelSize() +
                                        m_grid->GetRowSize(0)));

    sim.MouseDragDrop(pt.x, pt.y, pt.x, pt.y + 50);

    wxYield();

    CHECK(rowsize.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::RangeSelect", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter select(m_grid, wxEVT_GRID_RANGE_SELECTED);

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

    CHECK(select.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::Cursor", "[grid]")
{
    m_grid->SetGridCursor(1, 1);

    CHECK(m_grid->GetGridCursorCol() == 1);
    CHECK(m_grid->GetGridCursorRow() == 1);

    m_grid->MoveCursorDown(false);
    m_grid->MoveCursorLeft(false);
    m_grid->MoveCursorUp(false);
    m_grid->MoveCursorUp(false);
    m_grid->MoveCursorRight(false);

    CHECK(m_grid->GetGridCursorCol() == 1);
    CHECK(m_grid->GetGridCursorRow() == 0);

    m_grid->SetCellValue(0, 0, "some text");
    m_grid->SetCellValue(3, 0, "other text");
    m_grid->SetCellValue(0, 1, "more text");
    m_grid->SetCellValue(3, 1, "extra text");

    m_grid->Update();
    m_grid->Refresh();

    m_grid->MoveCursorLeftBlock(false);

    CHECK(m_grid->GetGridCursorCol() == 0);
    CHECK(m_grid->GetGridCursorRow() == 0);

    m_grid->MoveCursorDownBlock(false);

    CHECK(m_grid->GetGridCursorCol() == 0);
    CHECK(m_grid->GetGridCursorRow() == 3);

    m_grid->MoveCursorRightBlock(false);

    CHECK(m_grid->GetGridCursorCol() == 1);
    CHECK(m_grid->GetGridCursorRow() == 3);

    m_grid->MoveCursorUpBlock(false);

    CHECK(m_grid->GetGridCursorCol() == 1);
    CHECK(m_grid->GetGridCursorRow() == 0);
}

TEST_CASE_METHOD(GridTestCase, "Grid::KeyboardSelection", "[grid][selection]")
{
    m_grid->SetCellValue(1, 1, "R2C2");
    m_grid->SetCellValue(2, 0, "R3C1");
    m_grid->SetCellValue(3, 0, "R4C1");
    m_grid->SetCellValue(4, 0, "R5C1");
    m_grid->SetCellValue(7, 0, "R8C1");

    CHECK(m_grid->GetGridCursorCoords() == wxGridCellCoords(0, 0));

    m_grid->MoveCursorRight(true);
    CheckSelection(wxGridBlockCoords(0, 0, 0, 1));

    m_grid->MoveCursorDownBlock(true);
    CheckSelection(wxGridBlockCoords(0, 0, 2, 1));

    m_grid->MoveCursorDownBlock(true);
    CheckSelection(wxGridBlockCoords(0, 0, 4, 1));

    m_grid->MoveCursorDownBlock(true);
    CheckSelection(wxGridBlockCoords(0, 0, 7, 1));

    m_grid->MoveCursorUpBlock(true);
    CheckSelection(wxGridBlockCoords(0, 0, 4, 1));

    m_grid->MoveCursorLeft(true);
    CheckSelection(wxGridBlockCoords(0, 0, 4, 0));
}

TEST_CASE_METHOD(GridTestCase, "Grid::Selection", "[grid]")
{
    m_grid->SelectAll();

    CHECK(m_grid->IsSelection());
    CHECK(m_grid->IsInSelection(0, 0));
    CHECK(m_grid->IsInSelection(9, 1));

    m_grid->SelectBlock(1, 0, 3, 1);

    wxGridCellCoordsArray topleft = m_grid->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray bottomright = m_grid->GetSelectionBlockBottomRight();

    CHECK(topleft.Count() == 1);
    CHECK(bottomright.Count() == 1);

    CHECK(topleft.Item(0).GetCol() == 0);
    CHECK(topleft.Item(0).GetRow() == 1);
    CHECK(bottomright.Item(0).GetCol() == 1);
    CHECK(bottomright.Item(0).GetRow() == 3);

    m_grid->SelectCol(1);

    CHECK(m_grid->IsInSelection(0, 1));
    CHECK(m_grid->IsInSelection(9, 1));
    CHECK(!m_grid->IsInSelection(3, 0));

    m_grid->SelectRow(4, true /* add to selection */);

    CHECK(m_grid->IsInSelection(4, 0));
    CHECK(m_grid->IsInSelection(4, 1));
    CHECK(!m_grid->IsInSelection(3, 0));

    // Check that deselecting a row does deselect the cells in it, but leaves
    // the other ones selected.
    m_grid->DeselectRow(4);
    CHECK(!m_grid->IsInSelection(4, 0));
    CHECK(!m_grid->IsInSelection(4, 1));
    CHECK(m_grid->IsInSelection(0, 1));

    m_grid->DeselectCol(1);
    CHECK(!m_grid->IsInSelection(0, 1));
}

TEST_CASE_METHOD(GridTestCase, "Grid::SelectionRange", "[grid]")
{
    const wxGridBlocks empty = m_grid->GetSelectedBlocks();
    CHECK( empty.begin() == empty.end() );

    m_grid->SelectBlock(1, 0, 3, 1);

    wxGridBlocks sel = m_grid->GetSelectedBlocks();
    REQUIRE( sel.begin() != sel.end() );
    CHECK( *sel.begin() == wxGridBlockCoords(1, 0, 3, 1) );

#if __cplusplus >= 201103L || wxCHECK_VISUALC_VERSION(11)
    m_grid->SelectBlock(4, 0, 7, 1, true);
    int index = 0;
    for ( const wxGridBlockCoords& block : m_grid->GetSelectedBlocks() )
    {
        switch ( index )
        {
        case 0:
            CHECK(block == wxGridBlockCoords(1, 0, 3, 1));
            break;
        case 1:
            CHECK(block == wxGridBlockCoords(4, 0, 7, 1));
            break;
        default:
            FAIL("Unexpected iterations count");
            break;
        }
        ++index;
    }
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::SelectEmptyGrid", "[grid]")
{
    for ( int i = 0; i < 2; ++i )
    {
        SECTION(i == 0 ? "No rows" : "No columns")
        {

            if ( i == 0 )
            {
                m_grid->DeleteRows(0, 10);
                REQUIRE( m_grid->GetNumberRows() == 0 );
            }
            else
            {
                m_grid->DeleteCols(0, 2);
                REQUIRE( m_grid->GetNumberCols() == 0 );
            }

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
    }

    CHECK( m_grid->GetSelectedCells().Count() == 0 );
    CHECK( m_grid->GetSelectionBlockTopLeft().Count() == 0 );
    CHECK( m_grid->GetSelectionBlockBottomRight().Count() == 0 );
    CHECK( m_grid->GetSelectedRows().Count() == 0 );
    CHECK( m_grid->GetSelectedCols().Count() == 0 );
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
    m_grid->GoToCell(1, 1);
    for ( int i = 0; i < 8; ++i )
    {
        m_grid->MoveCursorDown(true);
    }
    CHECK( m_grid->IsVisible(9, 1) );
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
    CHECK(m_grid->GetNumberRows() == 10);
    CHECK(m_grid->GetNumberCols() == 2);

    m_grid->AppendCols();
    m_grid->AppendRows();

    CHECK(m_grid->GetNumberRows() == 11);
    CHECK(m_grid->GetNumberCols() == 3);

    m_grid->AppendCols(2);
    m_grid->AppendRows(2);

    CHECK(m_grid->GetNumberRows() == 13);
    CHECK(m_grid->GetNumberCols() == 5);

    m_grid->InsertCols(1, 2);
    m_grid->InsertRows(2, 3);

    CHECK(m_grid->GetNumberRows() == 16);
    CHECK(m_grid->GetNumberCols() == 7);
}

TEST_CASE_METHOD(GridTestCase, "Grid::DeleteAndAddRowCol", "[grid]")
{
    SECTION("Default") {}
    SECTION("Native header") { m_grid->UseNativeColHeader(); }

    CHECK(m_grid->GetNumberRows() == 10);
    CHECK(m_grid->GetNumberCols() == 2);

    m_grid->DeleteRows(0, 10);
    m_grid->DeleteCols(0, 2);

    CHECK(m_grid->GetNumberRows() == 0);
    CHECK(m_grid->GetNumberCols() == 0);

    m_grid->AppendRows(5);
    m_grid->AppendCols(3);

    CHECK(m_grid->GetNumberRows() == 5);
    CHECK(m_grid->GetNumberCols() == 3);

    // The order of functions calls can be important
    m_grid->DeleteCols(0, 3);
    m_grid->DeleteRows(0, 5);

    CHECK(m_grid->GetNumberRows() == 0);
    CHECK(m_grid->GetNumberCols() == 0);

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

    CHECK(m_grid->GetNumberCols() == 4);

    wxArrayInt neworder;
    neworder.push_back(1);
    neworder.push_back(3);
    neworder.push_back(2);
    neworder.push_back(0);

    m_grid->SetColumnsOrder(neworder);

    CHECK(m_grid->GetColPos(1) == 0);
    CHECK(m_grid->GetColPos(3) == 1);
    CHECK(m_grid->GetColPos(2) == 2);
    CHECK(m_grid->GetColPos(0) == 3);

    CHECK(m_grid->GetColAt(0) == 1);
    CHECK(m_grid->GetColAt(1) == 3);
    CHECK(m_grid->GetColAt(2) == 2);
    CHECK(m_grid->GetColAt(3) == 0);

    m_grid->ResetColPos();

    CHECK(m_grid->GetColPos(0) == 0);
    CHECK(m_grid->GetColPos(1) == 1);
    CHECK(m_grid->GetColPos(2) == 2);
    CHECK(m_grid->GetColPos(3) == 3);
}

TEST_CASE_METHOD(GridTestCase, "Grid::ColumnVisibility", "[grid]")
{
    m_grid->AppendCols(3);
    CHECK( m_grid->IsColShown(1) );

    m_grid->HideCol(1);
    CHECK( !m_grid->IsColShown(1) );
    CHECK( m_grid->IsColShown(2) );

    m_grid->ShowCol(1);
    CHECK( m_grid->IsColShown(1) );
}

TEST_CASE_METHOD(GridTestCase, "Grid::LineFormatting", "[grid]")
{
    CHECK(m_grid->GridLinesEnabled());

    m_grid->EnableGridLines(false);

    CHECK(!m_grid->GridLinesEnabled());

    m_grid->EnableGridLines();

    m_grid->SetGridLineColour(*wxRED);

    CHECK(*wxRED == m_grid->GetGridLineColour());
}

TEST_CASE_METHOD(GridTestCase, "Grid::SortSupport", "[grid]")
{
    CHECK(m_grid->GetSortingColumn() == wxNOT_FOUND);

    m_grid->SetSortingColumn(1);

    CHECK(!m_grid->IsSortingBy(0));
    CHECK(m_grid->IsSortingBy(1));
    CHECK(m_grid->IsSortOrderAscending());

    m_grid->SetSortingColumn(0, false);

    CHECK(m_grid->IsSortingBy(0));
    CHECK(!m_grid->IsSortingBy(1));
    CHECK(!m_grid->IsSortOrderAscending());

    m_grid->UnsetSortingColumn();

    CHECK(!m_grid->IsSortingBy(0));
    CHECK(!m_grid->IsSortingBy(1));
}

TEST_CASE_METHOD(GridTestCase, "Grid::Labels", "[grid]")
{
    CHECK(m_grid->GetColLabelValue(0) == "A");
    CHECK(m_grid->GetRowLabelValue(0) == "1");

    m_grid->SetColLabelValue(0, "Column 1");
    m_grid->SetRowLabelValue(0, "Row 1");

    CHECK(m_grid->GetColLabelValue(0) == "Column 1");
    CHECK(m_grid->GetRowLabelValue(0) == "Row 1");

    m_grid->SetLabelTextColour(*wxGREEN);
    m_grid->SetLabelBackgroundColour(*wxRED);

    CHECK(m_grid->GetLabelTextColour() == *wxGREEN);
    CHECK(m_grid->GetLabelBackgroundColour() == *wxRED);

    m_grid->SetColLabelTextOrientation(wxVERTICAL);

    CHECK(m_grid->GetColLabelTextOrientation() == wxVERTICAL);
}

TEST_CASE_METHOD(GridTestCase, "Grid::SelectionMode", "[grid]")
{
    //We already test this mode in Select
    CHECK(m_grid->GetSelectionMode() == wxGrid::wxGridSelectCells);

    // Select an individual cell and an entire row.
    m_grid->SelectBlock(3, 1, 3, 1);
    m_grid->SelectRow(5, true /* add to selection */);

    // Test that after switching to row selection mode only the row remains
    // selected.
    m_grid->SetSelectionMode(wxGrid::wxGridSelectRows);
    CHECK( m_grid->IsInSelection(5, 0) );
    CHECK( m_grid->IsInSelection(5, 1) );
    CHECK( !m_grid->IsInSelection(3, 1) );

    //Test row selection be selecting a single cell and checking the whole
    //row is selected
    m_grid->ClearSelection();
    m_grid->SelectBlock(3, 1, 3, 1);

    wxArrayInt selectedRows = m_grid->GetSelectedRows();
    CHECK(selectedRows.Count() == 1);
    CHECK(selectedRows[0] == 3);

    // Check that overlapping selection blocks are handled correctly.
    m_grid->ClearSelection();
    m_grid->SelectBlock(0, 0, 4, 1);
    m_grid->SelectBlock(2, 0, 6, 1, true /* add to selection */);
    CHECK( m_grid->GetSelectedRows().size() == 7 );

    CHECK( m_grid->GetSelectedColBlocks().empty() );

    RowRanges rowRanges;
    rowRanges.push_back(RowRange(0, 6));
    CheckRowSelection(rowRanges);

    m_grid->SelectBlock(6, 0, 8, 1);
    m_grid->SelectBlock(1, 0, 4, 1, true /* add to selection */);
    m_grid->SelectBlock(0, 0, 2, 1, true /* add to selection */);
    CHECK( m_grid->GetSelectedRows().size() == 8 );

    rowRanges.clear();
    rowRanges.push_back(RowRange(0, 4));
    rowRanges.push_back(RowRange(6, 8));
    CheckRowSelection(rowRanges);

    // Select all odd rows.
    m_grid->ClearSelection();
    rowRanges.clear();
    for ( int i = 1; i < m_grid->GetNumberRows(); i += 2 )
    {
        m_grid->SelectBlock(i, 0, i, 1, true);
        rowRanges.push_back(RowRange(i, i));
    }

    CheckRowSelection(rowRanges);

    // Now select another block overlapping 2 of them and bordering 2 others.
    m_grid->SelectBlock(2, 0, 6, 1, true);

    rowRanges.clear();
    rowRanges.push_back(RowRange(1, 7));
    rowRanges.push_back(RowRange(9, 9));
    CheckRowSelection(rowRanges);

    CHECK(m_grid->GetSelectionMode() == wxGrid::wxGridSelectRows);


    //Test column selection be selecting a single cell and checking the whole
    //column is selected
    m_grid->SetSelectionMode(wxGrid::wxGridSelectColumns);
    m_grid->SelectBlock(3, 1, 3, 1);

    CHECK( m_grid->GetSelectedRowBlocks().empty() );

    wxArrayInt selectedCols = m_grid->GetSelectedCols();
    CHECK(selectedCols.Count() == 1);
    CHECK(selectedCols[0] == 1);

    wxGridBlockCoordsVector colBlocks = m_grid->GetSelectedColBlocks();
    CHECK( colBlocks.size() == 1 );
    CHECK( colBlocks.at(0) == wxGridBlockCoords(0, 1, 9, 1) );

    CHECK(m_grid->GetSelectionMode() == wxGrid::wxGridSelectColumns);
}

TEST_CASE_METHOD(GridTestCase, "Grid::CellFormatting", "[grid]")
{
    //Check that initial alignment is default
    int horiz, cellhoriz, vert, cellvert;

    m_grid->GetDefaultCellAlignment(&horiz, &vert);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CHECK(horiz == cellhoriz);
    CHECK(vert == cellvert);

    //Check initial text colour and background colour are default
    wxColour text, back;

    back = m_grid->GetDefaultCellBackgroundColour();

    CHECK(m_grid->GetCellBackgroundColour(0, 0) == back);

    back = m_grid->GetDefaultCellTextColour();

    CHECK(m_grid->GetCellTextColour(0, 0) == back);

#if WXWIN_COMPATIBILITY_2_8
    m_grid->SetCellAlignment(wxALIGN_CENTRE, 0, 0);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CHECK(cellhoriz == wxALIGN_CENTRE);
    CHECK(cellvert == wxALIGN_CENTRE);
#endif // WXWIN_COMPATIBILITY_2_8

    m_grid->SetCellAlignment(0, 0, wxALIGN_LEFT, wxALIGN_BOTTOM);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CHECK(cellhoriz == wxALIGN_LEFT);
    CHECK(cellvert == wxALIGN_BOTTOM);

#if WXWIN_COMPATIBILITY_2_8
    m_grid->SetCellTextColour(*wxRED, 0, 0);
    CHECK(m_grid->GetCellTextColour(0, 0) == *wxRED);
#endif // WXWIN_COMPATIBILITY_2_8

    m_grid->SetCellTextColour(0, 0, *wxGREEN);
    CHECK(m_grid->GetCellTextColour(0,0) == *wxGREEN);
}

TEST_CASE_METHOD(GridTestCase, "Grid::GetNonDefaultAlignment", "[grid]")
{
    // GetNonDefaultAlignment() is used by several renderers having their own
    // preferred alignment, so check that if we don't reset the alignment
    // explicitly, it doesn't override the alignment used by default.
    wxGridCellAttrPtr attr;
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

    // This is only indirectly related, but test here for CanOverflow() working
    // correctly for the cells with non-default alignment, as this used to be
    // broken.
    m_grid->SetCellAlignment(0, 0, wxALIGN_INVALID, wxALIGN_CENTRE);
    attr = m_grid->CallGetCellAttr(0, 0);
    REQUIRE( attr );
    CHECK( attr->CanOverflow() );
}

TEST_CASE_METHOD(GridTestCase, "Grid::Editable", "[grid]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    //As the grid is not editable we shouldn't create an editor
    EventCounter created(m_grid, wxEVT_GRID_EDITOR_CREATED);

    wxUIActionSimulator sim;

    CHECK(m_grid->IsEditable());

    m_grid->EnableEditing(false);

    CHECK(!m_grid->IsEditable());

    m_grid->SetFocus();
    m_grid->SetGridCursor(1, 1);

    sim.Text("abab");
    wxYield();

    sim.Char(WXK_RETURN);
    wxYield();

    CHECK(created.GetCount() == 0);
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

    CHECK(!m_grid->IsReadOnly(1, 1));

    m_grid->SetReadOnly(1, 1);

    CHECK(m_grid->IsReadOnly(1, 1));

    m_grid->SetFocus();

    m_grid->SetGridCursor(1, 1);

    CHECK(m_grid->IsCurrentCellReadOnly());

    sim.Text("abab");
    wxYield();

    sim.Char(WXK_RETURN);
    wxYield();

    CHECK(created.GetCount() == 0);
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

    CHECK(created.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::ResizeScrolledHeader", "[grid]")
{
    // TODO this test currently works only under Windows unfortunately
#if wxUSE_UIACTIONSIMULATOR && (defined(__WXMSW__) || defined(__WXGTK__))
    if ( !EnableUITests() )
        return;

#ifdef __WXGTK20__
    // Works locally, but not when run on Travis CI.
    if ( IsAutomaticTest() )
        return;
#endif

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

    CHECK(m_grid->GetColSize(0) == startwidth + draglength);
#endif
}

TEST_CASE_METHOD(GridTestCase, "Grid::ColumnMinWidth", "[grid]")
{
    // TODO this test currently works only under Windows unfortunately
#if wxUSE_UIACTIONSIMULATOR && (defined(__WXMSW__) || defined(__WXGTK__))
    if ( !EnableUITests() )
        return;

#ifdef __WXGTK20__
    // Works locally, but not when run on Travis CI.
    if ( IsAutomaticTest() )
        return;
#endif

    SECTION("Default") {}
    SECTION("Native header")
    {
        // For some unknown reason, this test fails under AppVeyor even though
        // it passes locally, so disable it there. If anybody can reproduce the
        // problem locally, where it can be debugged, please let us know.
        if ( IsAutomaticTest() )
            return;

        m_grid->UseNativeColHeader();
    }

    int const startminwidth = m_grid->GetColMinimalAcceptableWidth();
    m_grid->SetColMinimalAcceptableWidth(startminwidth*2);
    int const newminwidth = m_grid->GetColMinimalAcceptableWidth();
    int const startwidth = m_grid->GetColSize(0);

    CHECK(m_grid->GetColMinimalAcceptableWidth() < startwidth);

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

    CHECK(m_grid->GetColSize(0) == newminwidth);
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

    wxGridCellAttrPtr attr(m_grid->GetOrCreateCellAttr(0, 0));
    wxGridCellRendererPtr renderer(attr->GetRenderer(m_grid, 0, 0));
    REQUIRE(renderer);

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

    SECTION("Column with auto wrapping contents taller than row")
    {
        // Verify row height remains unchanged with an auto-wrapping multi-line
        // cell.
        // Also shouldn't continuously try to fit the multi-line content into
        // a single line, which is not possible. See
        // https://trac.wxwidgets.org/ticket/15943 .

        m_grid->SetCellValue(0, 0, multilineStr);
        m_grid->SetCellRenderer(0 , 0, new wxGridCellAutoWrapStringRenderer);
        m_grid->AutoSizeColumn(0);

        wxYield();
        CHECK( m_grid->GetRowSize(0) == m_grid->GetDefaultRowSize() );
    }
}

TEST_CASE_METHOD(GridTestCase, "Grid::DrawInvalidCell", "[grid][multicell]")
{
    // Set up a multicell with inside an overflowing cell.
    // This is artificial and normally inside cells are probably not expected
    // to have a value but this is merely done to check if inside cells are
    // drawn, which they shouldn't be.
    m_grid->SetCellSize(0, 0, 2, 1);
    m_grid->SetCellValue( 1, 0, wxString('W', 42) );

    // Update()s, yields and sleep are needed to try to make the test fail with
    // macOS, GTK and MSW.
    // MSW needs just the yields (or updates), macOS in addition needs to sleep
    // (doesn't work with updates) and for GTK it's usually enough to just do
    // two updates (not yields). This test does all unconditionally.
    m_grid->Update();
    wxYield();
    wxMilliSleep(20);

    // Try to force redrawing of the inside cell: if it still draws there will
    // be an infinite recursion.
    m_grid->SetColSize(1, m_grid->GetColSize(1) + 1);

    m_grid->Update();
    wxYield();
}

// Test wxGridBlockCoords here because it'a a part of grid sources.

std::ostream& operator<<(std::ostream& os, const wxGridBlockCoords& block) {
    os << "wxGridBlockCoords(" <<
        block.GetTopRow() << ", " << block.GetLeftCol() << ", " <<
        block.GetBottomRow() << ", " << block.GetRightCol() << ")";
    return os;
}

TEST_CASE("GridBlockCoords::Canonicalize", "[grid]")
{
    const wxGridBlockCoords block =
        wxGridBlockCoords(4, 3, 2, 1).Canonicalize();

    CHECK(block.GetTopRow() == 2);
    CHECK(block.GetLeftCol() == 1);
    CHECK(block.GetBottomRow() == 4);
    CHECK(block.GetRightCol() == 3);
}

TEST_CASE("GridBlockCoords::Intersects", "[grid]")
{
    // Inside.
    CHECK(wxGridBlockCoords(1, 1, 3, 3).Intersects(wxGridBlockCoords(1, 2, 2, 3)));

    // Intersects.
    CHECK(wxGridBlockCoords(1, 1, 3, 3).Intersects(wxGridBlockCoords(2, 2, 4, 4)));

    // Doesn't intersects.
    CHECK(!wxGridBlockCoords(1, 1, 3, 3).Intersects(wxGridBlockCoords(4, 4, 6, 6)));
}

TEST_CASE("GridBlockCoords::Contains", "[grid]")
{
    // Inside.
    CHECK(wxGridBlockCoords(1, 1, 3, 3).Contains(wxGridCellCoords(2, 2)));

    // Outside.
    CHECK(!wxGridBlockCoords(1, 1, 3, 3).Contains(wxGridCellCoords(5, 5)));

    wxGridBlockCoords block1(1, 1, 5, 5);
    wxGridBlockCoords block2(1, 1, 3, 3);
    wxGridBlockCoords block3(2, 2, 7, 7);
    wxGridBlockCoords block4(10, 10, 12, 12);

    CHECK( block1.Contains(block2));
    CHECK(!block2.Contains(block1));
    CHECK(!block1.Contains(block3));
    CHECK(!block1.Contains(block4));
    CHECK(!block3.Contains(block1));
    CHECK(!block4.Contains(block1));
}

TEST_CASE("GridBlockCoords::Difference", "[grid]")
{
    SECTION("Subtract contained block (splitted horizontally)")
    {
        const wxGridBlockCoords block1(1, 1, 7, 7);
        const wxGridBlockCoords block2(3, 3, 5, 5);
        const wxGridBlockDiffResult result =
            block1.Difference(block2, wxHORIZONTAL);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 2, 7));
        CHECK(result.m_parts[1] == wxGridBlockCoords(6, 1, 7, 7));
        CHECK(result.m_parts[2] == wxGridBlockCoords(3, 1, 5, 2));
        CHECK(result.m_parts[3] == wxGridBlockCoords(3, 6, 5, 7));
    }

    SECTION("Subtract contained block (splitted vertically)")
    {
        const wxGridBlockCoords block1(1, 1, 7, 7);
        const wxGridBlockCoords block2(3, 3, 5, 5);
        const wxGridBlockDiffResult result =
            block1.Difference(block2, wxVERTICAL);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 7, 2));
        CHECK(result.m_parts[1] == wxGridBlockCoords(1, 6, 7, 7));
        CHECK(result.m_parts[2] == wxGridBlockCoords(1, 3, 2, 5));
        CHECK(result.m_parts[3] == wxGridBlockCoords(6, 3, 7, 5));
    }

    SECTION("Blocks intersect by the corner (splitted horizontally)")
    {
        const wxGridBlockCoords block1(1, 1, 5, 5);
        const wxGridBlockCoords block2(3, 3, 7, 7);
        const wxGridBlockDiffResult result =
            block1.Difference(block2, wxHORIZONTAL);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 2, 5));
        CHECK(result.m_parts[1] == wxGridNoBlockCoords);
        CHECK(result.m_parts[2] == wxGridBlockCoords(3, 1, 5, 2));
        CHECK(result.m_parts[3] == wxGridNoBlockCoords);
    }

    SECTION("Blocks intersect by the corner (splitted vertically)")
    {
        const wxGridBlockCoords block1(1, 1, 5, 5);
        const wxGridBlockCoords block2(3, 3, 7, 7);
        const wxGridBlockDiffResult result =
            block1.Difference(block2, wxVERTICAL);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 5, 2));
        CHECK(result.m_parts[1] == wxGridNoBlockCoords);
        CHECK(result.m_parts[2] == wxGridBlockCoords(1, 3, 2, 5));
        CHECK(result.m_parts[3] == wxGridNoBlockCoords);
    }

    SECTION("Blocks are the same")
    {
        const wxGridBlockCoords block1(1, 1, 3, 3);
        const wxGridBlockCoords block2(1, 1, 3, 3);
        const wxGridBlockDiffResult result =
            block1.Difference(block2, wxHORIZONTAL);

        CHECK(result.m_parts[0] == wxGridNoBlockCoords);
        CHECK(result.m_parts[1] == wxGridNoBlockCoords);
        CHECK(result.m_parts[2] == wxGridNoBlockCoords);
        CHECK(result.m_parts[3] == wxGridNoBlockCoords);
    }

    SECTION("Blocks doesn't intersects")
    {
        const wxGridBlockCoords block1(1, 1, 3, 3);
        const wxGridBlockCoords block2(5, 5, 7, 7);
        const wxGridBlockDiffResult result =
            block1.Difference(block2, wxHORIZONTAL);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 3, 3));
        CHECK(result.m_parts[1] == wxGridNoBlockCoords);
        CHECK(result.m_parts[2] == wxGridNoBlockCoords);
        CHECK(result.m_parts[3] == wxGridNoBlockCoords);
    }
}


TEST_CASE("GridBlockCoords::SymDifference", "[grid]")
{
    SECTION("With contained block")
    {
        const wxGridBlockCoords block1(1, 1, 7, 7);
        const wxGridBlockCoords block2(3, 3, 5, 5);
        const wxGridBlockDiffResult result = block1.SymDifference(block2);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 2, 7));
        CHECK(result.m_parts[1] == wxGridBlockCoords(6, 1, 7, 7));
        CHECK(result.m_parts[2] == wxGridBlockCoords(3, 1, 5, 2));
        CHECK(result.m_parts[3] == wxGridBlockCoords(3, 6, 5, 7));
    }

    SECTION("Blocks intersect by the corner")
    {
        const wxGridBlockCoords block1(1, 1, 5, 5);
        const wxGridBlockCoords block2(3, 3, 7, 7);
        const wxGridBlockDiffResult result = block1.SymDifference(block2);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 2, 5));
        CHECK(result.m_parts[1] == wxGridBlockCoords(6, 3, 7, 7));
        CHECK(result.m_parts[2] == wxGridBlockCoords(3, 1, 5, 2));
        CHECK(result.m_parts[3] == wxGridBlockCoords(3, 6, 5, 7));
    }

    SECTION("Blocks are the same")
    {
        const wxGridBlockCoords block1(1, 1, 3, 3);
        const wxGridBlockCoords block2(1, 1, 3, 3);
        const wxGridBlockDiffResult result = block1.SymDifference(block2);

        CHECK(result.m_parts[0] == wxGridNoBlockCoords);
        CHECK(result.m_parts[1] == wxGridNoBlockCoords);
        CHECK(result.m_parts[2] == wxGridNoBlockCoords);
        CHECK(result.m_parts[3] == wxGridNoBlockCoords);
    }

    SECTION("Blocks doesn't intersects")
    {
        const wxGridBlockCoords block1(1, 1, 3, 3);
        const wxGridBlockCoords block2(5, 5, 7, 7);
        const wxGridBlockDiffResult result = block1.SymDifference(block2);

        CHECK(result.m_parts[0] == wxGridBlockCoords(1, 1, 3, 3));
        CHECK(result.m_parts[1] == wxGridBlockCoords(5, 5, 7, 7));
        CHECK(result.m_parts[2] == wxGridNoBlockCoords);
        CHECK(result.m_parts[3] == wxGridNoBlockCoords);
    }
}

#endif //wxUSE_GRID
