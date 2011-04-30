///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlwindow.cpp
// Purpose:     wxHtmlWindow tests
// Author:      Vaclav Slavik
// Created:     2008-10-15
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vaclav Slavik <vslavik@fastmail.fm>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/html/htmlwin.h"
#include "wx/uiaction.h"
#include "testableframe.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class HtmlWindowTestCase : public CppUnit::TestCase
{
public:
    HtmlWindowTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( HtmlWindowTestCase );
        CPPUNIT_TEST( SelectionToText );
        CPPUNIT_TEST( Title );
#if wxUSE_UIACTIONSIMULATOR
        WXUISIM_TEST( CellClick );
        WXUISIM_TEST( LinkClick );
#endif // wxUSE_UIACTIONSIMULATOR
        CPPUNIT_TEST( AppendToPage );
    CPPUNIT_TEST_SUITE_END();

    void SelectionToText();
    void Title();
    void CellClick();
    void LinkClick();
    void AppendToPage();

    wxHtmlWindow *m_win;

    DECLARE_NO_COPY_CLASS(HtmlWindowTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HtmlWindowTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HtmlWindowTestCase, "HtmlWindowTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void HtmlWindowTestCase::setUp()
{
    m_win = new wxHtmlWindow(wxTheApp->GetTopWindow(), wxID_ANY,
                             wxDefaultPosition, wxSize(400, 200));
}

void HtmlWindowTestCase::tearDown()
{
    wxDELETE(m_win);
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

static const char *TEST_MARKUP =
    "<html><body>"
    "<title>Page</title>"
    "  Title<p>"
    "  A longer line<br>"
    "  and the last line."
    "</body></html>";

static const char *TEST_MARKUP_LINK =
    "<html><body>"
    "<a href=\"link\">link<\\a> "
    "</body></html>";

static const char *TEST_PLAIN_TEXT =
    "Title\nA longer line\nand the last line.";

void HtmlWindowTestCase::SelectionToText()
{
    m_win->SetPage(TEST_MARKUP);
    m_win->SelectAll();

    CPPUNIT_ASSERT_EQUAL( TEST_PLAIN_TEXT, m_win->SelectionToText() );
}

void HtmlWindowTestCase::Title()
{
    m_win->SetPage(TEST_MARKUP);

    CPPUNIT_ASSERT_EQUAL("Page", m_win->GetOpenedPageTitle());
}

#if wxUSE_UIACTIONSIMULATOR
void HtmlWindowTestCase::CellClick()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count1(m_win, wxEVT_COMMAND_HTML_CELL_CLICKED);

    wxUIActionSimulator sim;

    m_win->SetPage(TEST_MARKUP);
    m_win->Update();
    m_win->Refresh();

    sim.MouseMove(m_win->ClientToScreen(wxPoint(15, 15)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

void HtmlWindowTestCase::LinkClick()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count1(m_win, wxEVT_COMMAND_HTML_LINK_CLICKED);

    wxUIActionSimulator sim;

    m_win->SetPage(TEST_MARKUP_LINK);
    m_win->Update();
    m_win->Refresh();

    sim.MouseMove(m_win->ClientToScreen(wxPoint(15, 15)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}
#endif // wxUSE_UIACTIONSIMULATOR

void HtmlWindowTestCase::AppendToPage()
{
    m_win->SetPage(TEST_MARKUP_LINK);
    m_win->AppendToPage("A new paragraph");

    CPPUNIT_ASSERT_EQUAL("link A new paragraph", m_win->ToText());
}

#endif //wxUSE_HTML
