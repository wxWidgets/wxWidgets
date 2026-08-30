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

#ifdef __WXQT__
    #include "wx/weakref.h"
    #include <QtWidgets/QTabWidget>
#endif

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
                       "[notebook][book][NotebookTestCase]");

// wxNotebook supports images, unlike most of the other book controls.
wxBOOK_CTRL_BASE_TEST_CASE(NotebookTestCase, "Notebook", Image,
                           "[notebook][book][NotebookTestCase]");

NotebookTestCase::NotebookTestCase()
{
    m_notebook = make_unique<wxNotebook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}


TEST_CASE_METHOD(NotebookTestCase, "Notebook::RowCount", "[notebook][NotebookTestCase]")
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
                 "[notebook][NotebookTestCase]")
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

TEST_CASE("wxNotebook::AddPageEvents",
          "[wxNotebook][AddPage][event][winui-v0-supported]")
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

#ifdef __WXQT__

TEST_CASE("Notebook::SelectionVetoKeepsNativeAndWxState", "[notebook][qt]")
{
    wxNotebook notebook(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE(notebook.AddPage(new wxPanel(&notebook), "First", true));
    REQUIRE(notebook.AddPage(new wxPanel(&notebook), "Second"));
    REQUIRE(notebook.GetSelection() == 0);

    int changing = 0;
    int changed = 0;
    bool veto = true;
    notebook.Bind(wxEVT_NOTEBOOK_PAGE_CHANGING,
                  [&](wxBookCtrlEvent& event)
                  {
                      ++changing;
                      CHECK(event.GetOldSelection() == 0);
                      CHECK(event.GetSelection() == 1);
                      if ( veto )
                          event.Veto();
                  });
    notebook.Bind(wxEVT_NOTEBOOK_PAGE_CHANGED,
                  [&](wxBookCtrlEvent& event)
                  {
                      ++changed;
                      CHECK(event.GetOldSelection() == 0);
                      CHECK(event.GetSelection() == 1);
                  });

    CHECK(notebook.SetSelection(1) == 0);
    CHECK(changing == 1);
    CHECK(changed == 0);
    CHECK(notebook.GetSelection() == 0);
    CHECK(notebook.GetQTabWidget()->currentIndex() == 0);
    CHECK(notebook.GetCurrentPage() == notebook.GetPage(0));

    veto = false;
    CHECK(notebook.SetSelection(1) == 0);
    CHECK(changing == 2);
    CHECK(changed == 1);
    CHECK(notebook.GetSelection() == 1);
    CHECK(notebook.GetQTabWidget()->currentIndex() == 1);

    CHECK(notebook.ChangeSelection(0) == 1);
    CHECK(notebook.GetSelection() == 0);
    CHECK(notebook.GetQTabWidget()->currentIndex() == 0);
    CHECK(changing == 2);
    CHECK(changed == 1);
}

TEST_CASE("Notebook::RemoveLastPageKeepsEmptySelection", "[notebook][qt]")
{
    wxNotebook source(wxTheApp->GetTopWindow(), wxID_ANY);
    wxNotebook destination(wxTheApp->GetTopWindow(), wxID_ANY);
    wxPanel* const page = new wxPanel(&source);
    const wxWeakRef<wxPanel> weakPage(page);
    REQUIRE(source.AddPage(page, "Movable", true));
    REQUIRE(source.GetSelection() == 0);

    // Removing the final tab produces Qt currentChanged(-1), not a request to
    // select a page with an unsigned index. RemovePage must keep the page alive.
    REQUIRE(source.RemovePage(0));
    CHECK(source.GetPageCount() == 0);
    CHECK(source.GetSelection() == wxNOT_FOUND);
    CHECK(source.GetCurrentPage() == nullptr);
    CHECK(source.GetQTabWidget()->count() == 0);
    CHECK(source.GetQTabWidget()->currentIndex() == -1);
    REQUIRE(weakPage.get() == page);

    REQUIRE(page->Reparent(&destination));
    REQUIRE(destination.AddPage(page, "Moved", true));
    CHECK(destination.GetSelection() == 0);
    CHECK(destination.GetCurrentPage() == page);
    CHECK(destination.GetQTabWidget()->currentIndex() == 0);
    CHECK(destination.GetPageImage(0) == wxNOT_FOUND);

    REQUIRE(source.AddPage(new wxPanel(&source), "Replacement", true));
    CHECK(source.GetSelection() == 0);
    CHECK(source.GetQTabWidget()->currentIndex() == 0);
    CHECK(source.GetPageImage(0) == wxNOT_FOUND);
    REQUIRE(source.DeletePage(0));
    CHECK(source.GetPageCount() == 0);
    CHECK(source.GetSelection() == wxNOT_FOUND);
    CHECK(source.GetQTabWidget()->currentIndex() == -1);
}

#endif // __WXQT__

#if defined(__WXWINUI__) && wxUSE_WINUI3

TEST_CASE("wxNotebook::TopContract",
          "[wxNotebook][winui-v0-supported]")
{
    wxNotebook notebook(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(400, 200), wxNB_TOP);
    REQUIRE(notebook.AddPage(new wxPanel(&notebook), "first", true));
    REQUIRE(notebook.AddPage(new wxPanel(&notebook), "second", false));

    CHECK(notebook.GetSelection() == 0);
    CHECK(notebook.SetSelection(1) == 0);
    CHECK(notebook.GetSelection() == 1);

    const wxRect firstTab = notebook.GetTabRect(0);
    REQUIRE(!firstTab.IsEmpty());
    long flags = 0;
    const wxPoint centre(
        firstTab.x + firstTab.width / 2,
        firstTab.y + firstTab.height / 2);
    CHECK(notebook.HitTest(centre, &flags) == 0);
    CHECK((flags & wxBK_HITTEST_NOWHERE) == 0);
}

TEST_CASE("wxNotebook::ExternalSelectionDoesNotStealFocus",
          "[wxNotebook][winui-focus]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE( top != nullptr );
    wxWindow * const originalFocus = wxWindow::FindFocus();

    std::unique_ptr<wxWindow> external(
        new wxWindow(top, wxID_ANY, wxPoint(4, 4), wxSize(20, 20)));
    std::unique_ptr<wxNotebook> notebook(
        new wxNotebook(top, wxID_ANY, wxPoint(30, 4), wxSize(240, 140)));

    external->SetFocus();
    REQUIRE( wxWindow::FindFocus() == external.get() );

    REQUIRE( notebook->AddPage(
        new wxPanel(notebook.get()), "first", false) );
    CHECK( wxWindow::FindFocus() == external.get() );
    REQUIRE( notebook->AddPage(
        new wxPanel(notebook.get()), "second", false) );

    notebook->ChangeSelection(1);
    CHECK( wxWindow::FindFocus() == external.get() );
    notebook->SetSelection(0);
    CHECK( wxWindow::FindFocus() == external.get() );

    notebook.reset();
    external.reset();
    if ( originalFocus && !originalFocus->IsBeingDeleted() )
        originalFocus->SetFocus();
}

TEST_CASE("wxNotebook::InternalSelectionTransfersFocus",
          "[wxNotebook][winui-focus]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE( top != nullptr );
    wxWindow * const originalFocus = wxWindow::FindFocus();

    std::unique_ptr<wxNotebook> notebook(
        new wxNotebook(top, wxID_ANY, wxPoint(4, 4), wxSize(240, 140)));
    wxPanel * const first = new wxPanel(notebook.get());
    wxPanel * const second = new wxPanel(notebook.get());
    wxWindow * const firstChild =
        new wxWindow(first, wxID_ANY, wxPoint(4, 4), wxSize(20, 20));
    REQUIRE( notebook->AddPage(first, "first", true) );
    REQUIRE( notebook->AddPage(second, "second", false) );

    firstChild->SetFocus();
    REQUIRE( wxWindow::FindFocus() == firstChild );

    notebook->ChangeSelection(1);
    wxWindow *focused = wxWindow::FindFocus();
    CHECK( (focused == second ||
            (focused && second->IsDescendant(focused))) );

    notebook->SetSelection(0);
    focused = wxWindow::FindFocus();
    CHECK( (focused == first ||
            (focused && first->IsDescendant(focused))) );

    notebook.reset();
    if ( originalFocus && !originalFocus->IsBeingDeleted() )
        originalFocus->SetFocus();
}

TEST_CASE("wxNotebook::RTLTabRectsUseWxClientCoordinates",
          "[wxNotebook][winui-coordinates][rtl]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE( top != nullptr );

    wxNotebook notebook(
        top, wxID_ANY, wxPoint(4, 4), wxSize(640, 180));
    notebook.SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE( notebook.GetLayoutDirection() == wxLayout_RightToLeft );
    notebook.SetTabSize(notebook.FromDIP(wxSize(80, 42)));

    constexpr size_t PageCount = 3;
    for ( size_t page = 0; page < PageCount; ++page )
    {
        REQUIRE( notebook.AddPage(
            new wxPanel(&notebook),
            wxString::Format("RTL page %zu", page),
            page == 0) );
    }

    const wxRect clientRect(wxPoint(), notebook.GetClientSize());
    for ( size_t page = 0; page < PageCount; ++page )
    {
        const wxRect tabRect = notebook.GetTabRect(page);
        CAPTURE(page, tabRect);
        REQUIRE( !tabRect.IsEmpty() );

        const wxPoint centre(
            tabRect.x + tabRect.width / 2,
            tabRect.y + tabRect.height / 2);
        REQUIRE( clientRect.Contains(centre) );

        long flags = 0;
        CHECK( notebook.HitTest(centre, &flags) ==
               static_cast<int>(page) );
        CHECK( (flags & wxBK_HITTEST_NOWHERE) == 0 );
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3

TEST_CASE_METHOD(NotebookTestCase, "Notebook::GetTabRect", "[notebook][NotebookTestCase]")
{
    wxNotebook *notebook = new wxNotebook(wxTheApp->GetTopWindow(), wxID_ANY,
                                          wxDefaultPosition, wxSize(400, 200));
    std::unique_ptr<wxNotebook> cleanup(notebook);

    notebook->AddPage(new wxPanel(notebook), "Page");

    // MSW, wxUniv and WinUI report logical rectangles for every tab, including
    // virtualized/off-screen WinUI TabViewItem containers.
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
#else // !__WXMSW__ && !__WXUNIVERSAL__
    WX_ASSERT_FAILS_WITH_ASSERT( notebook->GetTabRect(0) );
#endif // ports
}

TEST_CASE_METHOD(NotebookTestCase, "Notebook::HitTestFlags", "[notebook][NotebookTestCase]")
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
