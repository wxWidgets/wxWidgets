///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlparser.cpp
// Purpose:     wxHtmlParser tests
// Author:      Vadim Zeitlin
// Created:     2011-01-13
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML


#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
#endif // WX_PRECOMP

#include "wx/html/winpars.h"

#include <memory>
#include <vector>

namespace
{

static constexpr unsigned NBSP_VALUE = 0x00a0;

void AddTextCells(const wxHtmlCell *cell,
                  std::vector<const wxHtmlCell *>& cells)
{
    if ( !cell )
        return;

    const wxString text = cell->ConvertToText(nullptr);
    if ( !text.empty() )
        cells.push_back(cell);

    for ( const wxHtmlCell *child = cell->GetFirstChild(); child;
          child = child->GetNext() )
    {
        AddTextCells(child, cells);
    }
}

wxString GetNBSP(size_t count)
{
    wxString s;
    for ( size_t n = 0; n < count; n++ )
        s += wxUniChar(NBSP_VALUE);
    return s;
}

} // anonymous namespace

// Test that parsing invalid HTML simply fails but doesn't crash for example.
TEST_CASE("wxHtmlParser::ParseInvalid", "[html][parser][error]")
{
    class NullParser : public wxHtmlWinParser
    {
    protected:
        virtual void AddText(const wxString& WXUNUSED(txt)) override { }
    };

    NullParser p;
    wxMemoryDC dc;
    p.SetDC(&dc);

    delete p.Parse("<");
    delete p.Parse("<foo");
    delete p.Parse("<!--");
    delete p.Parse("<!---");
}

TEST_CASE("wxHtmlParser::NBSPLineBreak", "[html][parser]")
{
    wxBitmap bmp(100, 100);
    wxMemoryDC dc(bmp);
    wxHtmlWinParser p;
    p.SetDC(&dc);

    const wxString html = "aaa <font color=\"#ff0000\">"
                          "&nbsp;&nbsp;</font>bbb";
    std::unique_ptr<wxHtmlContainerCell> const top(
        static_cast<wxHtmlContainerCell *>(p.Parse(html)));
    REQUIRE(top);

    top->Layout(1);

    std::vector<const wxHtmlCell *> cells;
    AddTextCells(top.get(), cells);

    REQUIRE(cells.size() >= 3);
    CHECK(cells[0]->ConvertToText(nullptr) == "aaa ");
    CHECK(cells[1]->ConvertToText(nullptr) == GetNBSP(2));
    CHECK(cells[2]->ConvertToText(nullptr) == "bbb");
    CHECK(cells[0]->GetAbsPos().y == cells[1]->GetAbsPos().y);
    CHECK(cells[1]->GetAbsPos().y == cells[2]->GetAbsPos().y);
}

TEST_CASE("wxHtmlCell::Detach", "[html][cell]")
{
    wxMemoryDC dc;

    std::unique_ptr<wxHtmlContainerCell> const top(new wxHtmlContainerCell(nullptr));
    wxHtmlContainerCell* const cont = new wxHtmlContainerCell(nullptr);
    wxHtmlCell* const cell1 = new wxHtmlWordCell("Hello", dc);
    wxHtmlCell* const cell2 = new wxHtmlColourCell(*wxRED);
    wxHtmlCell* const cell3 = new wxHtmlWordCell("world", dc);

    cont->InsertCell(cell1);
    cont->InsertCell(cell2);
    cont->InsertCell(cell3);
    top->InsertCell(cont);

    SECTION("container")
    {
        top->Detach(cont);
        CHECK( top->GetFirstChild() == nullptr );

        delete cont;
    }

    SECTION("first-child")
    {
        cont->Detach(cell1);
        CHECK( cont->GetFirstChild() == cell2 );

        delete cell1;
    }

    SECTION("middle-child")
    {
        cont->Detach(cell2);
        CHECK( cont->GetFirstChild() == cell1 );
        CHECK( cell1->GetNext() == cell3 );

        delete cell2;
    }

    SECTION("last-child")
    {
        cont->Detach(cell3);
        CHECK( cont->GetFirstChild() == cell1 );
        CHECK( cell1->GetNext() == cell2 );
        CHECK( cell2->GetNext() == nullptr );

        delete cell3;
    }

    SECTION("invalid")
    {
        WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE
        (
            "Expected assertion for detaching non-child",
            top->Detach(cell1);
        );
    }
}

#endif //wxUSE_HTML
