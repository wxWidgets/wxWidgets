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

namespace
{

const wxHtmlCell *GetFirstNonFormattingChild(const wxHtmlContainerCell *cell)
{
    const wxHtmlCell *child = cell->GetFirstChild();
    while ( child && child->IsFormattingCell() )
        child = child->GetNext();

    return child;
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

TEST_CASE("wxHtmlParser::TableAlign", "[html][parser][table]")
{
    wxBitmap bmp(400, 400);
    wxMemoryDC dc(bmp);

    wxHtmlWinParser parser;
    parser.SetDC(&dc);

    std::unique_ptr<wxHtmlContainerCell> const root(
        static_cast<wxHtmlContainerCell*>(
            parser.Parse(
                "<table align=\"center\"><tr><td>test</td><td>ok</td></tr></table>"
            )
        )
    );
    REQUIRE( root );

    root->Layout(200);

    const wxHtmlCell *cell = GetFirstNonFormattingChild(root.get());
    REQUIRE( cell );

    const wxHtmlContainerCell *body =
        static_cast<const wxHtmlContainerCell*>(cell);

    cell = GetFirstNonFormattingChild(body);
    REQUIRE( cell );

    const wxHtmlContainerCell *tableWrapper =
        static_cast<const wxHtmlContainerCell*>(cell);

    cell = GetFirstNonFormattingChild(tableWrapper);
    REQUIRE( cell );

    const wxHtmlContainerCell *table =
        static_cast<const wxHtmlContainerCell*>(cell);

    CHECK( table->GetPosX() > 0 );

    cell = GetFirstNonFormattingChild(table);
    REQUIRE( cell );

    const wxHtmlContainerCell *td =
        static_cast<const wxHtmlContainerCell*>(cell);

    cell = GetFirstNonFormattingChild(td);
    REQUIRE( cell );

    const wxHtmlContainerCell *tdContent =
        static_cast<const wxHtmlContainerCell*>(cell);

    CHECK( tdContent->GetAlignHor() == wxHTML_ALIGN_LEFT );
}

#endif //wxUSE_HTML
