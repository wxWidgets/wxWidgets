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

class HtmlWindowTestCase
{
public:
    HtmlWindowTestCase();
    ~HtmlWindowTestCase();

protected:
    wxHtmlWindow *m_win;

    wxDECLARE_NO_COPY_CLASS(HtmlWindowTestCase);
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

HtmlWindowTestCase::HtmlWindowTestCase()
{
    m_win = new wxHtmlWindow(wxTheApp->GetTopWindow(), wxID_ANY,
                             wxDefaultPosition, wxSize(400, 200));
}

HtmlWindowTestCase::~HtmlWindowTestCase()
{
    DeleteTestWindow(m_win);
    m_win = nullptr;
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
    virtual void Notify() override
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

    return nullptr;
}

TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::SelectionToText", "[html][htmlwindow]")
{
#if wxUSE_CLIPBOARD
    m_win->SetPage(TEST_MARKUP);
    m_win->SelectAll();

    CHECK( m_win->SelectionToText() == TEST_PLAIN_TEXT );
#endif // wxUSE_CLIPBOARD
}

TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::Title", "[html][htmlwindow]")
{
    m_win->SetPage(TEST_MARKUP);

    CHECK(m_win->GetOpenedPageTitle() == "Page");
}

TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::InitialLineBreak", "[html][htmlwindow]")
{
    m_win->SetBorders(0);
    m_win->SetPage("<html><body>TEXT</body></html>");

    wxHtmlContainerCell *plainTextRoot = m_win->GetInternalRepresentation();

    CHECK(plainTextRoot);

    int plainTextHeight = plainTextRoot->GetHeight();

    m_win->SetPage("<html><body><br>TEXT</body></html>");

    wxHtmlContainerCell *rootWithBreak = m_win->GetInternalRepresentation();

    CHECK(rootWithBreak);
    CHECK(rootWithBreak->GetHeight() > plainTextHeight);
}

#if wxUSE_UIACTIONSIMULATOR
TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::CellClick", "[html][htmlwindow]")
{
    if ( !EnableUITests() )
        return;

    EventCounter clicked(m_win, wxEVT_HTML_CELL_CLICKED);

    wxUIActionSimulator sim;

    m_win->SetPage(TEST_MARKUP);
    m_win->Update();
    m_win->Refresh();

    sim.MouseMove(m_win->ClientToScreen(wxPoint(15, 15)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK(clicked.GetCount() == 1);
}

TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::LinkClick", "[html][htmlwindow]")
{
    if ( !EnableUITests() )
        return;

    EventCounter clicked(m_win, wxEVT_HTML_LINK_CLICKED);

    wxUIActionSimulator sim;

    m_win->SetPage(TEST_MARKUP_LINK);
    m_win->Update();
    m_win->Refresh();

    sim.MouseMove(m_win->ClientToScreen(wxPoint(15, 15)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK(clicked.GetCount() == 1);
}
#endif // wxUSE_UIACTIONSIMULATOR

#if wxUSE_WXHTML_HELP
TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::DisplayMissingHelpTopic", "[html][htmlwindow]")
{
    wxHtmlHelpController controller(
        wxHF_DEFAULT_STYLE | wxHF_DIALOG | wxHF_MODAL,
        wxTheApp->GetTopWindow());
    CloseModalHelpDialogTimer timer(controller);

    timer.StartOnce(50);

    CHECK(!controller.Display("missing topic"));

    timer.Stop();

    CHECK(!timer.WasModalShown());
    controller.Quit();
}
#endif // wxUSE_WXHTML_HELP

TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::ImageMapCoordinates", "[html][htmlwindow]")
{
    m_win->SetBorders(0);
    m_win->SetPage(TEST_MARKUP_IMAGEMAP);

    wxHtmlContainerCell *root = m_win->GetInternalRepresentation();
    wxPoint hitpos;
    wxHtmlCell *image = FindCellWithLink(root, &hitpos);

    CHECK(image);

    const wxPoint imgpos = image->GetAbsPos(root);
    wxHtmlLinkInfo *link = root->GetLink(imgpos.x + hitpos.x,
                                         imgpos.y + hitpos.y);

    CHECK(link);
    CHECK(link->GetHref() == "hit");
    CHECK(!root->GetLink(imgpos.x, imgpos.y));
}

TEST_CASE_METHOD(HtmlWindowTestCase, "HtmlWindow::AppendToPage", "[html][htmlwindow]")
{
#if wxUSE_CLIPBOARD
    m_win->SetPage(TEST_MARKUP_LINK);
    m_win->AppendToPage("A new paragraph");

    CHECK(m_win->ToText() == "link A new paragraph");
#endif // wxUSE_CLIPBOARD
}

#endif //wxUSE_HTML
