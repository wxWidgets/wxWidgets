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
    #include "wx/dcmemory.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/html/winpars.h"

#include <memory>

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

TEST_CASE("wxHtmlParser::ImageAlignCenter", "[html][parser]")
{
    wxBitmap bmp(200, 200);
    wxMemoryDC dc(bmp);

    wxHtmlWinParser p;
    p.SetDC(&dc);

    wxLogNull noLog;
    std::unique_ptr<wxHtmlContainerCell> const cells(
        static_cast<wxHtmlContainerCell*>(p.Parse(
            "<img id=\"img\" width=\"100\" height=\"100\" "
            "align=\"center\" src=\"dummy\"/>Text")));

    REQUIRE( cells );
    cells->Layout(200);

    const wxString id("img");
    const wxHtmlCell* const image = cells->Find(wxHTML_COND_ISANCHOR, &id);
    REQUIRE( image );

    const wxHtmlCell* const word = image->GetNext();
    REQUIRE( word );
    REQUIRE_FALSE( word->IsFormattingCell() );

    CHECK( word->GetPosY() + word->GetHeight() / 2 ==
           image->GetPosY() + image->GetHeight() / 2 );
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
