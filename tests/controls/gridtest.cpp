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

// To disable tests which work locally, but not when run on GitHub CI.
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    #define wxSKIP_AUTOMATIC_TEST_IF_GTK2() \
        if ( IsAutomaticTest() ) return
#else
    #define wxSKIP_AUTOMATIC_TEST_IF_GTK2()
#endif

namespace
{

wxString CellCoordsToString(int row, int col)
{
    return wxString::Format("R%dC%d", col + 1, row + 1);
}

wxString CellSizeToString(int rows, int cols)
{
    return wxString::Format("%dx%d", rows, cols);
}

struct Multicell
{
    int row, col, rows, cols;

    Multicell(int row, int col, int rows, int cols)
        : row(row), col(col), rows(rows), cols(cols) { }

    wxString Coords() const { return CellCoordsToString(row, col); }

    wxString Size() const { return CellSizeToString(rows, cols); }

    wxString ToString() const
    {
        wxString s;

        if ( rows == 1 && cols == 1 )
        {
            s = "cell";
        }
        else
        {
            s = Size() + " ";
            if ( rows > 1 || cols > 1 )
                s += "multicell";
            else
                s += "inside cell";
        }

        return wxString::Format("%s at %s", s, Coords());
    }
};

// Stores insertion/deletion info of rows or columns.
struct EditInfo
{
    int pos, count;
    wxGridDirection direction;

    EditInfo(int pos = 0,
             int count = 0,
             wxGridDirection direction = wxGRID_COLUMN)
        : pos(pos), count(count), direction(direction) { }
};

// Derive a new class inheriting from wxGrid, also to get access to its
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

    bool HasAttr(int row, int col,
                 wxGridCellAttr::wxAttrKind kind = wxGridCellAttr::Cell) const
    {
        // Can't use GetCellAttr() here as it always returns an attr.
        wxGridCellAttr* attr = GetTable()->GetAttr(row, col, kind);
        if ( attr )
            attr->DecRef();

        return attr != NULL;
    }

    size_t GetCellAttrCount() const
    {
        // Note that only attributes in grid range can easily be checked
        // and this function only counts those, not any outside of
        // the grid (e.g. with invalid negative coords).
        size_t count = 0;
        for ( int row = 0; row < GetNumberRows(); ++row )
        {
            for ( int col = 0; col < GetNumberCols(); ++col )
                count += HasAttr(row, col);
        }

        return count;
    }

    void SetMulticell(const Multicell& multi)
    {
        SetCellSize(multi.row, multi.col, multi.rows, multi.cols);
    }

    // Performs given insertions/deletions on either rows or columns.
    void DoEdit(const EditInfo& edit);

    // Returns annotated grid represented as a string.
    wxString ToString() const;

    // Used when drawing annotated grid to know what happens to it.
    EditInfo m_edit;

    // Grid as string before editing, with edit info annotated.
    wxString m_beforeGridAnnotated;
};

// Compares two grids, checking for differences with attribute presence and
// cell sizes.
class GridAttrMatcher : public Catch::MatcherBase<TestableGrid>
{
public:
    GridAttrMatcher(const TestableGrid& grid);

    bool match(const TestableGrid& other) const wxOVERRIDE;

    std::string describe() const wxOVERRIDE;

private:
    const TestableGrid* m_grid;

    mutable wxString m_diffDesc;
    mutable wxString m_expectedGridDesc;
};

// Helper function for recreating a grid to fit (only) a multicell.
void FitGridToMulticell(TestableGrid* grid, const Multicell& multi)
{
    const int oldRowCount = grid->GetNumberRows();
    const int oldColCount = grid->GetNumberCols();

    const int margin = 1;
    const int newRowCount = multi.row + multi.rows + margin;
    const int newColCount = multi.col + multi.cols + margin;

    if ( !oldRowCount && !oldColCount )
    {
        grid->CreateGrid(newRowCount, newColCount);
    }
    else
    {
        grid->DeleteRows(0, oldRowCount);
        grid->DeleteCols(0, oldColCount);
        grid->AppendRows(newRowCount);
        grid->AppendCols(newColCount);
    }
}

} // anonymous namespace

namespace Catch
{

template <> struct StringMaker<TestableGrid>
{
    static std::string convert(const TestableGrid& grid)
    {
        return ("Content before edit:\n" + grid.m_beforeGridAnnotated
                + "\nContent after edit:\n" + grid.ToString()).ToStdString();
    }
};

template <> struct StringMaker<Multicell>
{
    static std::string convert(const Multicell& multi)
    {
        return multi.ToString().ToStdString();
    }
};

} // namespace Catch

class GridTestCase
{
public:
    GridTestCase();
    ~GridTestCase();

protected:
    void InsertRows(int pos = 0, int count = 1)
    {
        m_grid->DoEdit(EditInfo(pos, count, wxGRID_ROW));
    }

    void InsertCols(int pos = 0, int count = 1)
    {
        m_grid->DoEdit(EditInfo(pos, count, wxGRID_COLUMN));
    }

    void DeleteRows(int pos = 0, int count = 1)
    {
        m_grid->DoEdit(EditInfo(pos, -count, wxGRID_ROW));
    }

    void DeleteCols(int pos = 0, int count = 1)
    {
        m_grid->DoEdit(EditInfo(pos, -count, wxGRID_COLUMN));
    }

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

    bool HasCellAttr(int row, int col) const
    {
        return m_grid->HasAttr(row, col, wxGridCellAttr::Cell);
    }

    void SetCellAttr(int row, int col)
    {
        m_grid->SetAttr(row, col, new wxGridCellAttr);
    }

    // Fills temp. grid with a multicell and returns a matcher with it.
    GridAttrMatcher HasMulticellOnly(const Multicell& multi)
    {
        return CheckMulticell(multi);
    }

    // Returns a matcher with empty (temp.) grid.
    GridAttrMatcher HasEmptyGrid()
    {
        return CheckMulticell(Multicell(0, 0, 1, 1));
    }

    // Helper function used by the previous two functions.
    GridAttrMatcher CheckMulticell(const Multicell& multi)
    {
        TestableGrid* grid = GetTempGrid();

        FitGridToMulticell(grid, multi);

        if ( multi.rows > 1 || multi.cols > 1 )
            grid->SetMulticell(multi);

        return GridAttrMatcher(*grid);
    }

    TestableGrid* GetTempGrid()
    {
        if ( !m_tempGrid )
        {
            m_tempGrid = new TestableGrid(wxTheApp->GetTopWindow());
            m_tempGrid->Hide();
        }

        return m_tempGrid;
    }

    TestableGrid *m_grid;

    // Temporary/scratch grid filled with expected content, used when
    // comparing against m_grid.
    TestableGrid *m_tempGrid;

    wxDECLARE_NO_COPY_CLASS(GridTestCase);
};

GridTestCase::GridTestCase() : m_tempGrid(NULL)
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
    delete m_tempGrid;
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
    // Grid could not pass the test under OSX and Universal.
    // So there may has bug in Grid implementation
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXOSX__) && !defined(__WXUNIVERSAL__)
    if ( !EnableUITests() )
        return;

    wxSKIP_AUTOMATIC_TEST_IF_GTK2();

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

    wxSKIP_AUTOMATIC_TEST_IF_GTK2();

#ifdef __WXGTK3__
    // works locally, but not when run on GitHub CI.
    if ( IsAutomaticTest() )
    {
        wxString useASAN;
        if ( wxGetEnv("wxUSE_ASAN", &useASAN) && useASAN == "1" )
        {
            WARN("Skipping test failing for unknown reason");
            return;
        }
    }
#endif // __WXGTK3__

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

    // Move the mouse a bit while staying inside the first cell of the range
    // so that the range selection really starts off by the next move.
    sim.MouseMove(pt.x + 5, pt.y + 5);
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
    EventCounter select(m_grid, wxEVT_GRID_RANGE_SELECTED);

    m_grid->SelectAll();

    CHECK(select.GetCount() == 1);

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

    // Note that calling SelectCol() results in 2 events because there is a
    // deselection event first.
    select.Clear();
    m_grid->SelectCol(1);
    CHECK(select.GetCount() == 2);

    CHECK(m_grid->IsInSelection(0, 1));
    CHECK(m_grid->IsInSelection(9, 1));
    CHECK(!m_grid->IsInSelection(3, 0));

    // But if we explicitly avoid deselecting the existing selection, we should
    // get exactly one event.
    select.Clear();
    m_grid->SelectRow(4, true /* add to selection */);
    CHECK(select.GetCount() == 1);

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

    // Check that top left/bottom right selection functions still work in row
    // selection mode.
    wxGridCellCoordsArray arr = m_grid->GetSelectionBlockTopLeft();
    REQUIRE( arr.size() == 1 );
    CHECK( arr[0] == wxGridCellCoords(5, 0) );

    arr = m_grid->GetSelectionBlockBottomRight();
    REQUIRE( arr.size() == 1 );
    CHECK( arr[0] == wxGridCellCoords(5, 1) );

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

    m_grid->SetCellAlignment(0, 0, wxALIGN_LEFT, wxALIGN_BOTTOM);
    m_grid->GetCellAlignment(0, 0, &cellhoriz, &cellvert);

    CHECK(cellhoriz == wxALIGN_LEFT);
    CHECK(cellvert == wxALIGN_BOTTOM);

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
    // TODO this test currently works only under Windows and GTK unfortunately
#if wxUSE_UIACTIONSIMULATOR && (defined(__WXMSW__) || defined(__WXGTK__))
    if ( !EnableUITests() )
        return;

    wxSKIP_AUTOMATIC_TEST_IF_GTK2();

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
    // TODO this test currently works only under Windows and GTK unfortunately
#if wxUSE_UIACTIONSIMULATOR && (defined(__WXMSW__) || defined(__WXGTK__))
    if ( !EnableUITests() )
        return;

    wxSKIP_AUTOMATIC_TEST_IF_GTK2();

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
        // https://github.com/wxWidgets/wxWidgets/issues/15943 .

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

#define CHECK_ATTR_COUNT(n) CHECK( m_grid->GetCellAttrCount() == n )

TEST_CASE_METHOD(GridTestCase, "Grid::CellAttribute", "[attr][cell][grid]")
{
    SECTION("Overwrite")
    {
        CHECK_ATTR_COUNT( 0 );

        m_grid->SetAttr(0, 0, NULL);
        CHECK_ATTR_COUNT( 0 );

        SetCellAttr(0, 0);
        CHECK_ATTR_COUNT( 1 );

        // Overwrite existing attribute with another.
        SetCellAttr(0, 0);
        CHECK_ATTR_COUNT( 1 );

        m_grid->SetAttr(0, 0, NULL);
        CHECK_ATTR_COUNT( 0 );

        SetCellAttr(0, 0);
        m_grid->SetCellBackgroundColour(0, 1, *wxGREEN);
        CHECK_ATTR_COUNT( 2 );

        m_grid->SetAttr(0, 1, NULL);
        m_grid->SetAttr(0, 0, NULL);
        CHECK_ATTR_COUNT( 0 );
    }

    SECTION("Cloning")
    {
        CHECK_ATTR_COUNT( 0 );

        m_grid->GetOrCreateCellAttrPtr(0, 0)
            ->SetClientObject(new wxStringClientData("test"));
        CHECK_ATTR_COUNT( 1 );

        m_grid->SetAttr(0, 1, m_grid->GetOrCreateCellAttrPtr(0, 0)->Clone());
        CHECK_ATTR_COUNT( 2 );

        wxClientData* const
            data = m_grid->GetOrCreateCellAttrPtr(0, 1)->GetClientObject();
        CHECK( static_cast<wxStringClientData*>(data)->GetData() == "test" );
    }

    // Fill the grid with attributes for next sections.

    const int numRows = m_grid->GetNumberRows();
    const int numCols = m_grid->GetNumberCols();

    for ( int row = 0; row < numRows; ++row )
    {
        for ( int col = 0; col < numCols; ++col )
            SetCellAttr(row, col);
    }

    size_t numAttrs = static_cast<size_t>(numRows * numCols);

    CHECK_ATTR_COUNT( numAttrs );

    SECTION("Expanding")
    {
        CHECK( !HasCellAttr(numRows, numCols) );

        m_grid->InsertCols();
        CHECK_ATTR_COUNT( numAttrs );
        CHECK( !HasCellAttr(0, 0) );
        CHECK( HasCellAttr(0, numCols) );

        m_grid->InsertRows();
        CHECK_ATTR_COUNT( numAttrs );
        CHECK( HasCellAttr(numRows, numCols) );
    }

    SECTION("Shrinking")
    {
        CHECK( HasCellAttr(numRows - 1 , numCols - 1) );
        CHECK( HasCellAttr(0, numCols - 1) );

        m_grid->DeleteCols();
        numAttrs -= m_grid->GetNumberRows();
        CHECK_ATTR_COUNT( numAttrs );
        CHECK( HasCellAttr(0, 0) );
        CHECK( !HasCellAttr(0, numCols - 1) );

        m_grid->DeleteRows();
        numAttrs -= m_grid->GetNumberCols();
        CHECK_ATTR_COUNT( numAttrs );
        CHECK( !HasCellAttr(numRows - 1 , numCols - 1) );
    }
}

#define CHECK_MULTICELL() CHECK_THAT( *m_grid, HasMulticellOnly(multi) )

#define CHECK_NO_MULTICELL() CHECK_THAT( *m_grid, HasEmptyGrid() )

#define WHEN_N(s, n) WHEN(wxString::Format(s, n).ToStdString())

TEST_CASE_METHOD(GridTestCase,
                 "Grid::InsertionsWithMulticell",
                 "[attr][cell][grid][insert][multicell]")
{
    int insertions = 0, offset = 0;

    Multicell multi(1, 1, 3, 5);

    SECTION("Sanity checks")
    {
        FitGridToMulticell(m_grid, multi);
        m_grid->SetMulticell(multi);

        REQUIRE( static_cast<int>(m_grid->GetCellAttrCount())
                    == multi.rows * multi.cols );

        int row, col, rows, cols;

        // Check main cell.
        row = multi.row,
        col = multi.col;
        wxGrid::CellSpan span = m_grid->GetCellSize(row, col, &rows, &cols);

        REQUIRE( span == wxGrid::CellSpan_Main );
        REQUIRE( rows == multi.rows );
        REQUIRE( cols == multi.cols );

        // Check inside cell at opposite of main.
        row = multi.row + multi.rows - 1;
        col = multi.col + multi.cols - 1;
        span = m_grid->GetCellSize(row, col, &rows, &cols);

        REQUIRE( span == wxGrid::CellSpan_Inside );
        REQUIRE( rows == multi.row - row );
        REQUIRE( cols == multi.col - col );
    }

    // Do some basic testing with column insertions first and do more tests
    // with edge cases later on just with rows. It's not really needed to
    // repeat the same tests for both rows and columns as the code for
    // updating them works symmetrically.

    GIVEN(Catch::toString(multi))
    {
        FitGridToMulticell(m_grid, multi);
        m_grid->SetMulticell(multi);

        insertions = 2;

        WHEN("inserting any columns in multicell, at main")
        {
            InsertCols(multi.col + 0, insertions);

            THEN("the position changes but not the size")
            {
                multi.col += insertions;
                CHECK_MULTICELL();
            }
        }

        WHEN("inserting any columns in multicell, just after main")
        {
            InsertCols(multi.col + 1, insertions);

            THEN("the size changes but not the position")
            {
                multi.cols += insertions;
                CHECK_MULTICELL();
            }
        }

    }

    // Do more extensive testing with rows.

    wxSwap(multi.rows, multi.cols);

    GIVEN(Catch::toString(multi))
    {
        FitGridToMulticell(m_grid, multi);
        m_grid->SetMulticell(multi);

        const int insertionCounts[] = {1, 2, multi.rows};

        for ( size_t i = 0; i < WXSIZEOF(insertionCounts); ++i )
        {
            insertions = insertionCounts[i];

            WHEN_N("inserting %d row(s), just before main", insertions)
            {
                InsertRows(multi.row - 1, insertions);

                THEN("the position changes but not the size")
                {
                    multi.row += insertions;
                    CHECK_MULTICELL();
                }
            }

            WHEN_N("inserting %d row(s) in multicell, at main", insertions)
            {
                InsertRows(multi.row + 0, insertions);

                THEN("the position changes but not the size")
                {
                    multi.row += insertions;
                    CHECK_MULTICELL();
                }
            }
        }

        insertions = multi.rows / 2;

        // Check insertions within multicell, at and near edges.
        const int insertionOffsets[] = {1, 2, multi.rows - 2, multi.rows - 1};

        for ( size_t i = 0; i < WXSIZEOF(insertionOffsets); ++i )
        {
            offset = insertionOffsets[i];

            WHEN_N("inserting rows in multicell, %d row(s) after main", offset)
            {
                InsertRows(multi.row + offset, insertions);

                THEN("the size changes but not the position")
                {
                    multi.rows += insertions;
                    CHECK_MULTICELL();
                }
            }
        }

        // Check at least one case of inserting after multicell.
        WHEN("inserting rows, just after multicell")
        {
            insertions = 2;
            InsertRows(multi.row + multi.rows, insertions);

            THEN("neither size nor position change")
            {
                CHECK_MULTICELL();
            }
        }
    }

}

TEST_CASE_METHOD(GridTestCase,
                 "GridMulticell::DeletionsWithMulticell",
                 "[cellattr][delete][grid][multicell]")
{
    int deletions = 0, offset = 0;

    // Same as with the previous (insertions) test case but instead of some
    // basic testing with columns first, this time use rows for that and do more
    // extensive testing with columns.

    Multicell multi(1, 1, 5, 3);

    GIVEN(Catch::toString(multi))
    {
        FitGridToMulticell(m_grid, multi);
        m_grid->SetMulticell(multi);

        WHEN("deleting any rows, at main")
        {
            deletions = 1;
            DeleteRows(multi.row + 0, deletions);

            THEN("the multicell is deleted")
            {
                CHECK_NO_MULTICELL();
            }
        }

        WHEN("deleting more rows than length of multicell,"
             " at end of multicell")
        {
            deletions = multi.rows + 2;
            offset = multi.rows - 1;
            DeleteRows(multi.row + offset, deletions);

            THEN("the size changes but not the position")
            {
                multi.rows = offset;
                CHECK_MULTICELL();
            }
        }
    }

    // Do more extensive testing with columns.

    wxSwap(multi.rows, multi.cols);

    GIVEN(Catch::toString(multi))
    {
        FitGridToMulticell(m_grid, multi);
        m_grid->SetMulticell(multi);

        WHEN("deleting one column, just before main")
        {
            DeleteCols(multi.col - 1);

            THEN("the position changes but not the size")
            {
                multi.col--;
                CHECK_MULTICELL();
            }
        }

        WHEN("deleting multiple columns, just before main")
        {
            deletions = 2; // Must be at least 2 to affect main.
            DeleteCols(multi.col - 1, wxMax(2, deletions));

            THEN("the multicell is deleted")
            {
                CHECK_NO_MULTICELL();
            }
        }

        WHEN("deleting any columns, at main")
        {
            deletions = 1;
            DeleteCols(multi.col + 0, deletions);

            THEN("the multicell is deleted")
            {
                CHECK_NO_MULTICELL();
            }
        }

        WHEN("deleting one column within multicell, after main")
        {
            offset = 1;
            offset = wxClip(offset, 1, multi.cols - 1);
            DeleteCols(multi.col + offset, 1);

            THEN("the size changes but not the position")
            {
                multi.cols--;
                CHECK_MULTICELL();
            }
        }

        deletions = 2;

        // Check deletions within multicell, at and near edges.
        const int offsets[] = {1, 2, multi.cols - deletions};

        for ( size_t i = 0; i < WXSIZEOF(offsets); ++i )
        {
            offset = offsets[i];

            WHEN_N("deleting columns only within multicell,"
                   " %d column(s) after main", offset)
            {
                DeleteCols(multi.col + offset, deletions);

                THEN("the size changes but not the position")
                {
                    multi.cols -= deletions;
                    CHECK_MULTICELL();
                }
            }
        }

        // Instead of stuffing the multicell's length logic in the above test,
        // separately check at least two cases of starting many deletions
        // within multicell.

        WHEN("deleting more columns than length of multicell, just after main")
        {
            deletions = multi.cols + 2;
            offset = 1;
            DeleteCols(multi.col + offset, deletions);

            THEN("the size changes but not the position")
            {
                multi.cols = offset;
                CHECK_MULTICELL();
            }
        }

        WHEN("deleting more columns than length of multicell,"
             " at end of multicell")
        {
            deletions = multi.cols + 2;
            offset = multi.cols - 1;
            DeleteCols(multi.col + offset, deletions);

            THEN("the size changes but not the position")
            {
                multi.cols = offset;
                CHECK_MULTICELL();
            }
        }
    }

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

//
// TestableGrid
//

void TestableGrid::DoEdit(const EditInfo& edit)
{
    m_edit = edit;
    m_beforeGridAnnotated = ToString();

    switch ( edit.direction )
    {
    case wxGRID_COLUMN:
        if ( edit.count < 0 )
            DeleteCols(edit.pos, -edit.count);
        else
            InsertCols(edit.pos, edit.count);
        break;

    case wxGRID_ROW:
        if ( edit.count < 0 )
            DeleteRows(edit.pos, -edit.count);
        else
            InsertRows(edit.pos, edit.count);
        break;
    }
}

wxString TestableGrid::ToString() const
{
    const int numRows = GetNumberRows();
    const int numCols = GetNumberCols();

    const int colMargin = GetRowLabelValue(numRows - 1).length();
    const wxString leftIndent = wxString(' ', colMargin + 1);

    // String s contains the rendering of the grid, start with drawing
    // the header columns.
    wxString s = leftIndent;

    const int base = 10;
    // Draw the multiples of 10.
    for ( int col = base; col <= numCols; col += base)
    {
        s += wxString(' ', base - 1);
        s += ('0' + (col / base));
    }

    if ( numCols >= base )
        s += "\n" + leftIndent;

    // Draw the single digits.
    for ( int col = 1; col <= numCols; ++col )
        s += '0' + (col % base);
    s += "\n";

    // Draw horizontal divider.
    s += wxString(' ', colMargin) + '+' + wxString('-', numCols) + "\n";

    const int absEditCount = abs(m_edit.count);
    wxString action;
    action.Printf(" %s: %d",
                  m_edit.count < 0 ? "deletions" : "insertions",
                  absEditCount);

    // Will contain summary of grid (only multicells mentioned).
    wxString content;

    // Draw grid content.
    for ( int row = 0; row < numRows; ++row )
    {
        const wxString label = GetRowLabelValue(row);
        s += wxString(' ', colMargin - label.length());
        s += label + '|';

        for ( int col = 0; col < numCols; ++col )
        {
            char c = 'x';
            int rows, cols;
            switch ( GetCellSize(row, col, &rows, &cols) )
            {
            case wxGrid::CellSpan_None:
                c = HasAttr(row, col) ? '*' : '.';
                break;

            case wxGrid::CellSpan_Main:
                c = 'M';
                content += Multicell(row, col, rows, cols).ToString() + "\n";
                break;

            case wxGrid::CellSpan_Inside:
                // Check if the offset to main cell is correct.
                c = (GetCellSize(row + rows, col + cols, &rows, &cols)
                        == wxGrid::CellSpan_Main) ? 'm' : '?';
                break;
            }

            s += c;
        }

        // If applicable draw annotated row edits.
        if ( m_edit.count && m_edit.direction == wxGRID_ROW
             && row >= m_edit.pos && row < m_edit.pos + absEditCount)
        {
            s += (m_edit.count < 0 ? " ^" : " v");

            if ( row == m_edit.pos )
                s += action;
        }

        s += "\n";
    }

    // Draw annotated footer if columns edited.
    if ( m_edit.count && m_edit.direction == wxGRID_COLUMN )
    {
        s += leftIndent;

        for ( int col = 0; col < m_edit.pos + absEditCount; ++col )
        {
            if ( col < m_edit.pos )
                s += " ";
            else
                s += (m_edit.count < 0 ? "<" : ">");
        }

        s += action + "\n";
    }

    s += "\n";

    if ( content.empty() )
        content = "Empty grid\n";

    return content + s;
}

//
// GridAttrMatcher
//

GridAttrMatcher::GridAttrMatcher(const TestableGrid& grid) : m_grid(&grid)
{
   m_expectedGridDesc = m_grid->ToString();
}

bool GridAttrMatcher::match(const TestableGrid& other) const
{
    const int rows = wxMax(m_grid->GetNumberRows(), other.GetNumberRows());
    const int cols = wxMax(m_grid->GetNumberCols(), other.GetNumberCols());

    for ( int row = 0; row < rows; ++row )
    {
        for ( int col = 0; col < cols; ++col )
        {
            const bool hasAttr = m_grid->HasAttr(row, col);
            if ( hasAttr != other.HasAttr(row, col) )
            {
                m_diffDesc.Printf("%s: attribute presence (%d, expected %d)",
                                  CellCoordsToString(row, col),
                                  !hasAttr, hasAttr);

                return false;
            }

            int thisRows, thisCols;
            const wxGrid::CellSpan thisSpan
                = m_grid->GetCellSize(row, col, &thisRows, &thisCols);

            int otherRows, otherCols;
            (void) other.GetCellSize(row, col, &otherRows, &otherCols);

            if ( thisRows != otherRows || thisCols != otherCols )
            {
                wxString mismatchKind;
                switch ( thisSpan )
                {
                case wxGrid::CellSpan_None:
                    mismatchKind = "different cell size";
                    break;

                case wxGrid::CellSpan_Main:
                    mismatchKind = "different multicell size";
                    break;

                case wxGrid::CellSpan_Inside:
                    mismatchKind = "main offset mismatch";
                    break;
                }

                m_diffDesc.Printf( "%s: %s (%s, expected %s)",
                                   CellCoordsToString(row, col),
                                   mismatchKind,
                                   CellSizeToString(otherRows, otherCols),
                                   CellSizeToString(thisRows, thisCols)
                                   );

                return false;
            }
        }
    }

    return true;
}

std::string GridAttrMatcher::describe() const
{
    std::string desc = (m_diffDesc.empty() ? "Matches" : "Doesn't match");
    desc += " expected content:\n" + m_expectedGridDesc.ToStdString();

    if ( !m_diffDesc.empty() )
        desc += + "first difference at " + m_diffDesc.ToStdString();

    return desc;
}

#endif //wxUSE_GRID
