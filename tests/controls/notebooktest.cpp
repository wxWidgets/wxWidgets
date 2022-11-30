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
#include "wx/scopedptr.h"

#include "asserthelper.h"
#include "bookctrlbasetest.h"
#include "testableframe.h"

class NotebookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    NotebookTestCase() { m_notebook = nullptr; m_numPageChanges = 0; }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_notebook; }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_NOTEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_NOTEBOOK_PAGE_CHANGING; }


    CPPUNIT_TEST_SUITE( NotebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( Image );
        CPPUNIT_TEST( RowCount );
        CPPUNIT_TEST( NoEventsOnDestruction );
        CPPUNIT_TEST( GetTabRect );
    CPPUNIT_TEST_SUITE_END();

    void RowCount();
    void NoEventsOnDestruction();
    void GetTabRect();

    void OnPageChanged(wxNotebookEvent&) { m_numPageChanges++; }

    wxNotebook *m_notebook;

    int m_numPageChanges;

    wxDECLARE_NO_COPY_CLASS(NotebookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( NotebookTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( NotebookTestCase, "NotebookTestCase" );

void NotebookTestCase::setUp()
{
    m_notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}

void NotebookTestCase::tearDown()
{
    wxDELETE(m_notebook);
}

void NotebookTestCase::RowCount()
{
    CPPUNIT_ASSERT_EQUAL(1, m_notebook->GetRowCount());

#ifdef __WXMSW__
    wxDELETE(m_notebook);
    m_notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                wxDefaultPosition, wxSize(400, 200),
                                wxNB_MULTILINE);

    for( unsigned int i = 0; i < 10; i++ )
    {
        m_notebook->AddPage(new wxPanel(m_notebook), "Panel", false, 0);
    }

    CPPUNIT_ASSERT( m_notebook->GetRowCount() != 1 );
#endif
}

void NotebookTestCase::NoEventsOnDestruction()
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
    m_notebook->Destroy();
    m_notebook = nullptr;
    CHECK( m_numPageChanges == 1 );
}

TEST_CASE("wxNotebook::AddPageEvents", "[wxNotebook][AddPage][event]")
{
    wxNotebook* const
        notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                  wxDefaultPosition, wxSize(400, 200));
    wxScopedPtr<wxNotebook> cleanup(notebook);

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

void NotebookTestCase::GetTabRect()
{
    wxNotebook *notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                          wxDefaultPosition, wxSize(400, 200));
    wxScopedPtr<wxNotebook> cleanup(notebook);

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

#endif //wxUSE_NOTEBOOK
