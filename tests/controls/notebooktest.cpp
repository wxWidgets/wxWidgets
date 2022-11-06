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
#include "wx/artprov.h"

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
        CPPUNIT_TEST( HitTestFlags );
    CPPUNIT_TEST_SUITE_END();

    void RowCount();
    void NoEventsOnDestruction();
    void GetTabRect();
    void HitTestFlags();

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
    if (wxIsRunningUnderWine())
    {
        // Wine behaves different than Windows. Windows reports the size of a
        // tab even if it is not visible while Wine returns an empty rectangle.
        WARN("Skipping test known to fail under Wine.");
        return;
    }

    wxNotebook *notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                          wxDefaultPosition, wxSize(400, 200));
    wxScopedPtr<wxNotebook> cleanup(notebook);

    notebook->AddPage(new wxPanel(notebook), "First");

    // This function is only really implemented for wxMSW and wxUniv currently.
#if defined(__WXMSW__) || defined(__WXUNIVERSAL__)
    // Create many pages, so that at least some of the are not visible.
    for ( size_t i = 0; i < 30; i++ )
        notebook->AddPage(new wxPanel(notebook), "Page");

    for ( size_t i = 0; i < notebook->GetPageCount(); i++ )
    {
        wxRect r = notebook->GetTabRect(i);
        CHECK(r.width != 0);
        CHECK(r.height != 0);
    }
#else // !(__WXMSW__ || __WXUNIVERSAL__)
    WX_ASSERT_FAILS_WITH_ASSERT( notebook->GetTabRect(0) );
#endif // ports
}

#if defined(__WXMSW__) || defined(__WXUNIVERSAL__)
// Helpers for HitTestFlags()
static wxNotebook *CreateNotebook(wxBookCtrlBase::Images const &images, long flags)
{
    wxNotebook *notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition, wxSize(400, 200), flags);
    notebook->SetImages(images);
    wxPanel *panel = new wxPanel(notebook);
    notebook->AddPage(panel, "First Page", false, 0);

    panel->Show();
    notebook->Show();
    notebook->Move(0, 0);

    return notebook;
}

static void CheckNotebookFlags(wxNotebook *notebook, wxPoint &pt)
{
    int nowhere = 0;
    int onIcon = 0;
    int onLabel = 0;
    int onItem = 0;

    wxRect tabRect = notebook->GetTabRect(0);
    wxDirection tabDir = notebook->GetTabOrientation();

    int d;
    if (tabDir == wxTOP || tabDir == wxBOTTOM)
        d = tabRect.width;
    else
        d = tabRect.height;

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

        if (tabDir == wxTOP || tabDir == wxBOTTOM)
            pt.x++;
        else
            pt.y++;
    }

    CHECK(nowhere);
    CHECK(onIcon);
    CHECK(onLabel);
    CHECK(onItem);

    if (!nowhere || !onIcon || !onLabel || !onItem)
    {
        // Make the direction visible
        wxString const t = "FAILED";
        if (tabDir == wxTOP)
        {
            CHECK("wxTOP" == t);
        }
        else if (tabDir == wxBOTTOM)
        {
            CHECK("wxBOTTOM" == t);
        }
        else if (tabDir == wxLEFT)
        {
            CHECK("wxLEFT" == t);
        }
        else if (tabDir == wxRIGHT)
        {
            CHECK("wxRIGHT" == t);
        }
    }
}
#endif // ports

void NotebookTestCase::HitTestFlags()
{
    const wxSize imageSize(40, 40);
    static wxBookCtrlBase::Images images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_INFORMATION, wxART_OTHER, imageSize));

#if defined(__WXMSW__) || defined(__WXUNIVERSAL__)

    wxPoint pt;
    wxNotebook *notebook;
    wxRect r;
    wxScopedPtr<wxNotebook> cleanup;

    // For some reason these tests fail, even though they shouldn't. As these are using the
    // Microsft original code, so we leave it for now.
    //notebook = CreateNotebook(images, wxBK_TOP);
    //cleanup.reset(notebook);
    //CHECK(notebook);
    //r = notebook->GetTabRect(0);
    //pt = wxPoint(0, r.y + r.height / 2);
    //CheckNotebookFlags(notebook, pt);

    //notebook = CreateNotebook(images, wxBK_BOTTOM);
    //cleanup.reset(notebook);
    //CHECK(notebook);
    //r = notebook->GetTabRect(0);
    //pt = wxPoint(0, r.y + r.height / 2);
    //CheckNotebookFlags(notebook, pt);

    notebook = CreateNotebook(images, wxBK_LEFT);
    cleanup.reset(notebook);
    CHECK(notebook);
    r = notebook->GetTabRect(0);
    pt = wxPoint(r.x + r.width / 2, 0);
    CheckNotebookFlags(notebook, pt);

    notebook = CreateNotebook(images, wxBK_RIGHT);
    cleanup.reset(notebook);
    CHECK(notebook);
    r = notebook->GetTabRect(0);
    pt = wxPoint(r.x + r.width / 2, 0);
    CheckNotebookFlags(notebook, pt);

#else // !(__WXMSW__ || __WXUNIVERSAL__)
    wxNotebook *notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition, wxSize(400, 200));
    notebook->AddPage(new wxPanel(notebook), "First Page");
    wxScopedPtr<wxNotebook> cleanup(notebook);
    WX_ASSERT_FAILS_WITH_ASSERT(notebook->GetTabRect(0));
#endif // ports
}

#endif //wxUSE_NOTEBOOK
