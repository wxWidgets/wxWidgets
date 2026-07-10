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
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/html/htmlwin.h"
#include "wx/html/winpars.h"
#include "wx/sstream.h"

#include <memory>
#include <vector>

namespace
{

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

} // anonymous namespace

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

namespace
{

wxHtmlCell* FindTextCell(wxHtmlCell* cell, const wxString& text)
{
    if ( cell->ConvertToText(nullptr) == text )
        return cell;

    for ( wxHtmlCell* child = cell->GetFirstChild();
          child;
          child = child->GetNext() )
    {
        wxHtmlCell* found = FindTextCell(child, text);
        if ( found )
            return found;
    }

    return nullptr;
}

} // anonymous namespace

class TestHtmlWindowInterface : public wxHtmlWindowInterface
{
public:
    virtual void SetHTMLWindowTitle(const wxString& WXUNUSED(title)) override { }

    virtual void OnHTMLLinkClicked(const wxHtmlLinkInfo& WXUNUSED(link)) override { }

    virtual wxHtmlOpeningStatus OnHTMLOpeningURL(wxHtmlURLType type,
                                                 const wxString& url,
                                                 wxString *WXUNUSED(redirect)) const override
    {
        if ( type == wxHTML_URL_IMAGE )
            m_imageUrl = url;
        else if ( type == wxHTML_URL_PAGE )
            m_pageUrl = url;

        return wxHTML_BLOCK;
    }

    virtual wxPoint HTMLCoordsToWindow(wxHtmlCell *WXUNUSED(cell),
                                       const wxPoint& WXUNUSED(pos)) const override
        { return wxPoint(); }

    virtual wxWindow* GetHTMLWindow() override { return nullptr; }

    virtual wxColour GetHTMLBackgroundColour() const override { return *wxWHITE; }

    virtual void SetHTMLBackgroundColour(const wxColour& WXUNUSED(clr)) override { }

    virtual void SetHTMLBackgroundImage(const wxBitmapBundle& WXUNUSED(bmpBg)) override { }

    virtual void SetHTMLStatusText(const wxString& WXUNUSED(text)) override { }

    virtual wxCursor GetHTMLCursor(HTMLCursor WXUNUSED(type)) const override
        { return wxCursor(); }

    const wxString& GetImageUrl() const { return m_imageUrl; }
    const wxString& GetPageUrl() const { return m_pageUrl; }

private:
    mutable wxString m_imageUrl;
    mutable wxString m_pageUrl;
};

class TestHtmlParser : public wxHtmlWinParser
{
public:
    explicit TestHtmlParser(wxHtmlWindowInterface *windowInterface = nullptr)
        : wxHtmlWinParser(windowInterface)
    {
    }

protected:
    virtual void AddText(const wxString& WXUNUSED(txt)) override { }
};

// Test that parsing invalid HTML simply fails but doesn't crash for example.
TEST_CASE("wxHtmlParser::ParseInvalid", "[html][parser][error]")
{
    TestHtmlParser p;
    wxMemoryDC dc;
    p.SetDC(&dc);

    delete p.Parse("<");
    delete p.Parse("<foo");
    delete p.Parse("<!--");
    delete p.Parse("<!---");
}

TEST_CASE("wxHtmlParser::NestedDefinitionList", "[html][parser]")
{
    wxBitmap bmp(400, 400);
    wxMemoryDC dc(bmp);

    wxHtmlWinParser p;
    p.SetDC(&dc);

    std::unique_ptr<wxHtmlContainerCell> const top
    (
        static_cast<wxHtmlContainerCell*>
        (
            p.Parse("<dl><dd>add<dl><dd>ducts</dd><dd>links</dd>"
                    "<dd>nodes</dd></dl></dd></dl>")
        )
    );

    REQUIRE(top.get());

    top->Layout(400);

    wxHtmlCell* const add = FindTextCell(top.get(), "add");
    wxHtmlCell* const ducts = FindTextCell(top.get(), "ducts");

    REQUIRE(add);
    REQUIRE(ducts);

    CHECK(ducts->GetAbsPos().x > add->GetAbsPos().x);
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
            R"(<img id="img" width="100" height="100" align="center" src="dummy"/>Text)"
        ))
    );

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
    CHECK(cells[1]->ConvertToText(nullptr) == wxString(L'\xa0', 2));
    CHECK(cells[2]->ConvertToText(nullptr) == "bbb");
    CHECK(cells[0]->GetAbsPos().y == cells[1]->GetAbsPos().y);
    CHECK(cells[1]->GetAbsPos().y == cells[2]->GetAbsPos().y);
}

TEST_CASE("wxHtmlEntitiesParser::StrokedD", "[html][parser][entity]")
{
    wxHtmlEntitiesParser p;

    CHECK( p.Parse("&Dstrok;&dstrok;") == L"\u0110\u0111" );
}

TEST_CASE("wxHtmlWinParser::OpenURLAbsoluteArchivePath",
          "[html][parser][url]")
{
    class RecordingFSHandler : public wxFileSystemHandler
    {
    public:
        bool CanOpen(const wxString& location) override
        {
            return !location.empty();
        }

        wxFSFile *OpenFile(wxFileSystem& WXUNUSED(fs),
                           const wxString& location) override
        {
            m_location = location;
            return new wxFSFile(new wxStringInputStream("data"),
                                location,
                                wxEmptyString,
                                wxEmptyString
#if wxUSE_DATETIME
                                , wxDateTime::Now()
#endif // wxUSE_DATETIME
                                );
        }

        const wxString& GetLocation() const { return m_location; }

    private:
        wxString m_location;
    };

    class AutoFSHandler
    {
    public:
        AutoFSHandler(RecordingFSHandler *handler) : m_handler(handler)
            { wxFileSystem::AddHandler(m_handler); }
        ~AutoFSHandler()
            { wxFileSystem::RemoveHandler(m_handler); }

    private:
        RecordingFSHandler *m_handler;
    };

    RecordingFSHandler handler;
    AutoFSHandler autoFSHandler(&handler);

    wxFileSystem fs;
    fs.ChangePathTo("file:doc.chm#xchm:/d/e/", true);

    wxHtmlWinParser parser;
    parser.SetFS(&fs);

    const wxString expected("file:doc.chm#xchm:/a/b/c.jpg");
    std::unique_ptr<wxFSFile> file(
        parser.OpenURL(wxHTML_URL_IMAGE, "/a/b/c.jpg"));

    REQUIRE(file);
    CHECK(handler.GetLocation() == expected);
    CHECK(file->GetLocation() == expected);
}

TEST_CASE("wxHtmlParser::BaseHref", "[html][parser]")
{
    TestHtmlWindowInterface windowInterface;
    TestHtmlParser p(&windowInterface);
    wxFileSystem fs;
    wxMemoryDC dc;

    fs.ChangePathTo("http://example.com/original/page.html");
    p.SetDC(&dc);
    p.SetFS(&fs);

    delete p.Parse("<html><head><base href=\"http://example.com/base/\"></head>"
                   "<body><img src=\"image.png\"></body></html>");

    CHECK(windowInterface.GetImageUrl() == "http://example.com/base/image.png");

    delete p.OpenURL(wxHTML_URL_PAGE, "linked.html");

    CHECK(windowInterface.GetPageUrl() ==
          "http://example.com/base/linked.html");
}

TEST_CASE("wxHtmlCell::Detach", "[html][cell]")
{
    wxMemoryDC dc;

    auto const top = make_unique<wxHtmlContainerCell>(nullptr);
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
