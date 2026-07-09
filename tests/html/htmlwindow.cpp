///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlwindow.cpp
// Purpose:     wxHtmlWindow tests
// Author:      Vaclav Slavik
// Created:     2008-10-15
// Copyright:   (c) 2008 Vaclav Slavik <vslavik@fastmail.fm>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/timer.h"
#endif // WX_PRECOMP

#include "wx/html/helpctrl.h"
#include "wx/html/helpdlg.h"
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

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( HtmlWindowTestCase );
        CPPUNIT_TEST( SelectionToText );
        CPPUNIT_TEST( Title );
#if wxUSE_UIACTIONSIMULATOR
        WXUISIM_TEST( CellClick );
        WXUISIM_TEST( LinkClick );
#endif // wxUSE_UIACTIONSIMULATOR
#if wxUSE_WXHTML_HELP
        CPPUNIT_TEST( DisplayMissingHelpTopic );
#endif // wxUSE_WXHTML_HELP
        CPPUNIT_TEST( ImageMapCoordinates );
        CPPUNIT_TEST( AppendToPage );
    CPPUNIT_TEST_SUITE_END();

    void SelectionToText();
    void Title();
    void CellClick();
    void LinkClick();
#if wxUSE_WXHTML_HELP
    void DisplayMissingHelpTopic();
#endif // wxUSE_WXHTML_HELP
    void ImageMapCoordinates();
    void AppendToPage();

    wxHtmlWindow *m_win;

    wxDECLARE_NO_COPY_CLASS(HtmlWindowTestCase);
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
    DeleteTestWindow(m_win);
    m_win = NULL;
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

static const char *TEST_MARKUP_IMAGEMAP =
    "<html><body>"
    "Text<br>"
    "<img src=\"missing.png\" width=\"100\" height=\"100\" usemap=\"#map\">"
    "<map name=\"map\">"
    "<area shape=\"rect\" coords=\"10,10,20,20\" href=\"hit\">"
    "</map>"
    "</body></html>";

#if wxUSE_WXHTML_HELP

class CloseModalHelpDialogTimer : public wxTimer
{
public:
    CloseModalHelpDialogTimer(wxHtmlHelpController& controller)
        : m_controller(controller),
          m_modalShown(false)
    {
    }

    bool WasModalShown() const { return m_modalShown; }

private:
    virtual void Notify() wxOVERRIDE
    {
        wxHtmlHelpDialog *dialog = m_controller.GetDialog();
        if ( dialog && dialog->IsModal() )
        {
            m_modalShown = true;
            dialog->EndModal(wxID_CANCEL);
        }
    }

    wxHtmlHelpController& m_controller;
    bool m_modalShown;
};

#endif // wxUSE_WXHTML_HELP

static wxHtmlCell *FindCellWithLink(wxHtmlCell *cell, wxPoint *pos)
{
    if ( !cell->GetFirstChild() )
    {
        for ( int y = 0; y < cell->GetHeight(); y++ )
        {
            for ( int x = 0; x < cell->GetWidth(); x++ )
            {
                if ( cell->GetLink(x, y) )
                {
                    *pos = wxPoint(x, y);
                    return cell;
                }
            }
        }
    }

    for ( wxHtmlCell *child = cell->GetFirstChild();
          child;
          child = child->GetNext() )
    {
        wxHtmlCell *found = FindCellWithLink(child, pos);
        if ( found )
            return found;
    }

    return NULL;
}

void HtmlWindowTestCase::SelectionToText()
{
#if wxUSE_CLIPBOARD
    m_win->SetPage(TEST_MARKUP);
    m_win->SelectAll();

    CPPUNIT_ASSERT_EQUAL( TEST_PLAIN_TEXT, m_win->SelectionToText() );
#endif // wxUSE_CLIPBOARD
}

void HtmlWindowTestCase::Title()
{
    m_win->SetPage(TEST_MARKUP);

    CPPUNIT_ASSERT_EQUAL("Page", m_win->GetOpenedPageTitle());
}

#if wxUSE_UIACTIONSIMULATOR
void HtmlWindowTestCase::CellClick()
{
    EventCounter clicked(m_win, wxEVT_HTML_CELL_CLICKED);

    wxUIActionSimulator sim;

    m_win->SetPage(TEST_MARKUP);
    m_win->Update();
    m_win->Refresh();

    sim.MouseMove(m_win->ClientToScreen(wxPoint(15, 15)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, clicked.GetCount());
}

void HtmlWindowTestCase::LinkClick()
{
    EventCounter clicked(m_win, wxEVT_HTML_LINK_CLICKED);

    wxUIActionSimulator sim;

    m_win->SetPage(TEST_MARKUP_LINK);
    m_win->Update();
    m_win->Refresh();

    sim.MouseMove(m_win->ClientToScreen(wxPoint(15, 15)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, clicked.GetCount());
}
#endif // wxUSE_UIACTIONSIMULATOR

#if wxUSE_WXHTML_HELP
void HtmlWindowTestCase::DisplayMissingHelpTopic()
{
    wxHtmlHelpController controller(
        wxHF_DEFAULT_STYLE | wxHF_DIALOG | wxHF_MODAL,
        wxTheApp->GetTopWindow());
    CloseModalHelpDialogTimer timer(controller);

    timer.StartOnce(50);

    CPPUNIT_ASSERT(!controller.Display("missing topic"));

    timer.Stop();

    CPPUNIT_ASSERT(!timer.WasModalShown());
    controller.Quit();
}
#endif // wxUSE_WXHTML_HELP

void HtmlWindowTestCase::ImageMapCoordinates()
{
    m_win->SetBorders(0);
    m_win->SetPage(TEST_MARKUP_IMAGEMAP);

    wxHtmlContainerCell *root = m_win->GetInternalRepresentation();
    wxPoint hitpos;
    wxHtmlCell *image = FindCellWithLink(root, &hitpos);

    CPPUNIT_ASSERT(image);

    const wxPoint imgpos = image->GetAbsPos(root);
    wxHtmlLinkInfo *link = root->GetLink(imgpos.x + hitpos.x,
                                         imgpos.y + hitpos.y);

    CPPUNIT_ASSERT(link);
    CPPUNIT_ASSERT_EQUAL("hit", link->GetHref());
    CPPUNIT_ASSERT(!root->GetLink(imgpos.x, imgpos.y));
}

void HtmlWindowTestCase::AppendToPage()
{
#if wxUSE_CLIPBOARD
    m_win->SetPage(TEST_MARKUP_LINK);
    m_win->AppendToPage("A new paragraph");

    CPPUNIT_ASSERT_EQUAL("link A new paragraph", m_win->ToText());
#endif // wxUSE_CLIPBOARD
}

#endif //wxUSE_HTML
