///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/notebooktest.cpp
// Purpose:     wxNotebook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_NOTEBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/notebook.h"

#include "asserthelper.h"
#include "bookctrlbasetest.h"
#include "testableframe.h"

#include <memory>

class NotebookTestCase : public BookCtrlBaseTestCase
{
public:
    NotebookTestCase();

    void OnPageChanged(wxNotebookEvent&) { m_numPageChanges++; }

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_notebook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_NOTEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_NOTEBOOK_PAGE_CHANGING; }

    std::unique_ptr<wxNotebook> m_notebook;

    int m_numPageChanges = 0;

    wxDECLARE_NO_COPY_CLASS(NotebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(NotebookTestCase, "Notebook",
                       "[notebook][book]");

// wxNotebook supports images, unlike most of the other book controls.
wxBOOK_CTRL_BASE_TEST_CASE(NotebookTestCase, "Notebook", Image,
                           "[notebook][book]");

NotebookTestCase::NotebookTestCase()
{
    m_notebook = make_unique<wxNotebook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}


TEST_CASE_METHOD(NotebookTestCase, "Notebook::RowCount", "[notebook]")
{
    CHECK(m_notebook->GetRowCount() == 1);

#ifdef __WXMSW__
    m_notebook = make_unique<wxNotebook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxSize(400, 200),
                                         wxNB_MULTILINE);

    for( unsigned int i = 0; i < 10; i++ )
    {
        m_notebook->AddPage(new wxPanel(m_notebook.get()), "Panel", false, 0);
    }

    CHECK( m_notebook->GetRowCount() != 1 );
#endif
}

TEST_CASE_METHOD(NotebookTestCase, "Notebook::NoEventsOnDestruction",
                 "[notebook]")
{
    // We can't use EventCounter helper here as it doesn't deal with the window
    // it's connected to being destroyed during its life-time, so do it
    // manually.
    m_notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED,
                     &NotebookTestCase::OnPageChanged, this);

    // Normally deleting a page before the selected one results in page
    // selection changing and the corresponding event.
    m_notebook->DeletePage(static_cast<size_t>(0));
    CHECK( m_numPageChanges == 1 );

    // But deleting the entire control shouldn't generate any events, yet it
    // used to do under GTK+ 3 when a page different from the first one was
    // selected.
    m_notebook->ChangeSelection(1);
    m_notebook.release()->Destroy();
    CHECK( m_numPageChanges == 1 );
}

TEST_CASE("wxNotebook::AddPageEvents", "[wxNotebook][AddPage][event]")
{
    wxNotebook* const
        notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                  wxDefaultPosition, wxSize(400, 200));
    std::unique_ptr<wxNotebook> cleanup(notebook);

    CHECK( notebook->GetSelection() == wxNOT_FOUND );

    EventCounter countPageChanging(notebook, wxEVT_NOTEBOOK_PAGE_CHANGING);
    EventCounter countPageChanged(notebook, wxEVT_NOTEBOOK_PAGE_CHANGED);

    // Add the first page, it is special.
    notebook->AddPage(new wxPanel(notebook), "Initial page");

    // The selection should have been changed.
    CHECK( notebook->GetSelection() == 0 );

    // But no events should have been generated.
    CHECK( countPageChanging.GetCount() == 0 );
    CHECK( countPageChanged.GetCount() == 0 );


    // Add another page without selecting it.
    notebook->AddPage(new wxPanel(notebook), "Unselected page");

    // Selection shouldn't have changed.
    CHECK( notebook->GetSelection() == 0 );

    // And no events should have been generated, of course.
    CHECK( countPageChanging.GetCount() == 0 );
    CHECK( countPageChanged.GetCount() == 0 );


    // Finally add another page and do select it.
    notebook->AddPage(new wxPanel(notebook), "Selected page", true);

    // It should have become selected.
    CHECK( notebook->GetSelection() == 2 );

    // And events for the selection change should have been generated.
    CHECK( countPageChanging.GetCount() == 1 );
    CHECK( countPageChanged.GetCount() == 1 );
}

TEST_CASE_METHOD(NotebookTestCase, "Notebook::GetTabRect", "[notebook]")
{
    wxNotebook *notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                          wxDefaultPosition, wxSize(400, 200));
    std::unique_ptr<wxNotebook> cleanup(notebook);

    notebook->AddPage(new wxPanel(notebook), "Page");

    // This function is only really implemented for wxMSW and wxUniv currently.
#if defined(__WXMSW__) || defined(__WXUNIVERSAL__)
    // Create many pages, so that at least some of the are not visible.
    for ( size_t i = 0; i < 30; i++ )
        notebook->AddPage(new wxPanel(notebook), "Page");

    const wxRect rectPage = notebook->GetTabRect(0);
    REQUIRE(rectPage.width != 0);
    REQUIRE(rectPage.height != 0);

    int x = rectPage.x + rectPage.width;
    for ( size_t i = 1; i < notebook->GetPageCount(); i++ )
    {
        wxRect r = notebook->GetTabRect(i);

        if (wxIsRunningUnderWine())
        {
            // Wine behaves different than Windows. Windows reports the size of a
            // tab even if it is not visible while Wine returns an empty rectangle.
            if ( r == wxRect() )
            {
                WARN("Skipping test for pages after " << i << " under Wine.");
                break;
            }
        }

        INFO("Page #" << i << ": rect=" << r);
        REQUIRE(r.x == x);
        REQUIRE(r.y == rectPage.y);
        REQUIRE(r.width == rectPage.width);
        REQUIRE(r.height == rectPage.height);

        x += r.width;
    }
#else // !(__WXMSW__ || __WXUNIVERSAL__)
    WX_ASSERT_FAILS_WITH_ASSERT( notebook->GetTabRect(0) );
#endif // ports
}

TEST_CASE_METHOD(NotebookTestCase, "Notebook::HitTestFlags", "[notebook]")
{
    std::unique_ptr<wxNotebook> notebook;

#if defined(__WXMSW__) || defined(__WXUNIVERSAL__)
    long style = 0;

    SECTION("Top") { style = wxBK_TOP; }
    SECTION("Bottom") { style = wxBK_BOTTOM; }
    SECTION("Left") { style = wxBK_LEFT; }
    SECTION("Right") { style = wxBK_RIGHT; }

    INFO("Style=" << style);

    const bool isVertical = style == wxBK_TOP || style == wxBK_BOTTOM;

    // HitTest() uses TCM_HITTEST for the vertical orientations and it doesn't
    // seem to work correctly under Wine, so skip the test there (for the
    // horizontal tabs we use our own code which does work even under Wine).
    if ( isVertical && wxIsRunningUnderWine() )
        return;

    notebook = make_unique<wxNotebook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                       wxPoint(0, 0), wxSize(400, 200),
                                       style);

    // Simulate an icon of standard size, its contents doesn't matter.
    const wxSize imageSize(16, 16);
    wxBookCtrlBase::Images images;
    images.push_back(wxBitmapBundle::FromBitmap(wxBitmap(imageSize)));
    notebook->SetImages(images);

    notebook->AddPage(new wxPanel(notebook.get()), "First Page", false, 0);

    const wxRect r = notebook->GetTabRect(0);
    INFO("Rect=" << r);

    wxPoint pt;
    if ( isVertical )
        pt.y = r.y + r.height / 2;
    else
        pt.x = r.x + r.width / 2;

    int nowhere = 0;
    int onIcon = 0;
    int onLabel = 0;
    int onItem = 0;

    const int d = isVertical ? r.width : r.height;
    for (int i = 0; i < d; i++)
    {
        long flags = 0;
        notebook->HitTest(pt, &flags);

        if (flags & wxBK_HITTEST_NOWHERE)
            nowhere++;

        if (flags & wxBK_HITTEST_ONICON)
            onIcon++;

        if (flags & wxBK_HITTEST_ONLABEL)
            onLabel++;

        if (flags & wxBK_HITTEST_ONITEM)
            onItem++;

        if (isVertical)
            pt.x++;
        else
            pt.y++;
    }

    CHECK(nowhere);
    CHECK(onIcon);
    CHECK(onLabel);
    CHECK(onItem);
#else // !(__WXMSW__ || __WXUNIVERSAL__)
    notebook = make_unique<wxNotebook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                       wxDefaultPosition, wxSize(400, 200));
    notebook->AddPage(new wxPanel(notebook.get()), "First Page");

    WX_ASSERT_FAILS_WITH_ASSERT(notebook->GetTabRect(0));
#endif // ports
}

#endif //wxUSE_NOTEBOOK
