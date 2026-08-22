///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/toolbooktest.cpp
// Purpose:     wxToolbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TOOLBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/toolbook.h"
#include "wx/toolbar.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "bookctrlbasetest.h"

#include <functional>
#include <utility>
#include <vector>

class ReentrantImageList final : public wxImageList
{
public:
    using wxImageList::wxImageList;

    void SetGetIconHook(std::function<void()> hook)
    {
        m_getIconHook = std::move(hook);
    }

    wxIcon GetIcon(int index) const override
    {
        if ( m_getIconHook && !m_inGetIconHook )
        {
            m_inGetIconHook = true;
            m_getIconHook();
            m_inGetIconHook = false;
        }

        return wxImageList::GetIcon(index);
    }

private:
    std::function<void()> m_getIconHook;
    mutable bool m_inGetIconHook{false};
};

class ToolbookForTesting final : public wxToolbook
{
public:
    using wxToolbook::wxToolbook;

    void SelectToolForTesting(int id)
    {
        wxCommandEvent event(wxEVT_TOOL, id);
        event.SetEventObject(GetToolBar());
        OnToolSelected(event);
    }

    void SetShowHook(std::function<void(wxWindow*, bool)> hook)
    {
        m_showHook = std::move(hook);
    }

protected:
    void DoShowPage(wxWindow* page, bool show) override
    {
        wxToolbook::DoShowPage(page, show);
        if ( m_showHook )
            m_showHook(page, show);
    }

private:
    std::function<void(wxWindow*, bool)> m_showHook;
};

#ifdef __WXWINUI__

struct ToolbookLoadedTopologyProbe
{
    wxToolbook *book = nullptr;
    wxWindow *candidate = nullptr;
    bool invoked = false;
    bool nestedRemove = true;
    bool nestedAdd = true;
    bool nestedDeleteAll = true;
};

struct ToolbookLoadedSelectionProbe
{
    wxToolbook *book = nullptr;
    int selection = wxNOT_FOUND;
    bool invoked = false;
};

void MutateToolbookTopologyDuringLoaded(wxToolBar *, void *context)
{
    ToolbookLoadedTopologyProbe * const probe =
        static_cast<ToolbookLoadedTopologyProbe *>(context);
    probe->invoked = true;
    probe->nestedRemove = probe->book->RemovePage(1);
    probe->nestedAdd =
        probe->book->AddPage(probe->candidate, "nested");
    probe->nestedDeleteAll = probe->book->DeleteAllPages();
}

void ChangeToolbookSelectionDuringLoaded(wxToolBar *, void *context)
{
    ToolbookLoadedSelectionProbe * const probe =
        static_cast<ToolbookLoadedSelectionProbe *>(context);
    probe->invoked = true;
    probe->book->SetSelection(probe->selection);
}

#endif // __WXWINUI__

class ToolbookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    ToolbookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_toolbook; }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGING; }

    virtual void Realize() override { m_toolbook->GetToolBar()->Realize(); }

    CPPUNIT_TEST_SUITE( ToolbookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( ToolBar );
        CPPUNIT_TEST( VetoRestoresToolById );
        CPPUNIT_TEST( BitmapLookupReentryIsPrePublication );
        CPPUNIT_TEST( BitmapLookupSameCandidateIsConsumed );
        CPPUNIT_TEST( InsertHideReentryKeepsOwnership );
        CPPUNIT_TEST( EmptyRealizeKeepsNoSelection );
#ifdef __WXWINUI__
        CPPUNIT_TEST( ControllerFailureIsAtomic );
        CPPUNIT_TEST( LoadedTopologyReentryIsRejected );
        CPPUNIT_TEST( LoadedSelectionReentryProjectsRadio );
        CPPUNIT_TEST( PageTextProjectsImmediately );
        CPPUNIT_TEST( DuplicatePageIdsUsePositionalController );
#endif
    CPPUNIT_TEST_SUITE_END();

    void ToolBar();
    void VetoRestoresToolById();
    void BitmapLookupReentryIsPrePublication();
    void BitmapLookupSameCandidateIsConsumed();
    void InsertHideReentryKeepsOwnership();
    void EmptyRealizeKeepsNoSelection();
#ifdef __WXWINUI__
    void ControllerFailureIsAtomic();
    void LoadedTopologyReentryIsRejected();
    void LoadedSelectionReentryProjectsRadio();
    void PageTextProjectsImmediately();
    void DuplicatePageIdsUsePositionalController();
#endif

    ToolbookForTesting *m_toolbook;

    wxDECLARE_NO_COPY_CLASS(ToolbookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ToolbookTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ToolbookTestCase, "ToolbookTestCase" );

void ToolbookTestCase::setUp()
{
    m_toolbook = new ToolbookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}

void ToolbookTestCase::tearDown()
{
    wxDELETE(m_toolbook);
}

void ToolbookTestCase::ToolBar()
{
    wxToolBar* toolbar = static_cast<wxToolBar*>(m_toolbook->GetToolBar());

    CPPUNIT_ASSERT(toolbar);
    CPPUNIT_ASSERT_EQUAL(3, toolbar->GetToolsCount());
}

void ToolbookTestCase::VetoRestoresToolById()
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    CPPUNIT_ASSERT(toolbar);
    CPPUNIT_ASSERT(m_panel1->GetId() != 0);

    m_toolbook->SetSelection(0);
    toolbar->Realize();
    toolbar->ToggleTool(m_panel1->GetId(), true);
    toolbar->ToggleTool(m_panel2->GetId(), true);

    m_toolbook->Bind(
        wxEVT_TOOLBOOK_PAGE_CHANGING,
        [](wxBookCtrlEvent& event)
        {
            event.Veto();
        });

    m_toolbook->SelectToolForTesting(m_panel2->GetId());

    CPPUNIT_ASSERT_EQUAL(0, m_toolbook->GetSelection());
    CPPUNIT_ASSERT(toolbar->GetToolState(m_panel1->GetId()));
    CPPUNIT_ASSERT(!toolbar->GetToolState(m_panel2->GetId()));
}

void ToolbookTestCase::BitmapLookupReentryIsPrePublication()
{
    ToolbookForTesting* const book = m_toolbook;
    wxWindow* const removedPage = book->GetPage(2);
    wxPanel* const candidate = new wxPanel(book);
    ReentrantImageList* const images =
        new ReentrantImageList(32, 32);
    images->Add(wxArtProvider::GetIcon(
        wxART_INFORMATION, wxART_OTHER, wxSize(32, 32)));
    book->AssignImageList(images);
    m_list = images;

    bool nested = false;
    images->SetGetIconHook(
        [book, &nested]()
        {
            if ( !nested )
            {
                nested = true;
                CPPUNIT_ASSERT(book->RemovePage(2));
            }
        });

    const bool inserted =
        book->InsertPage(1, candidate, "pre-publication reentry", false, 0);

    CPPUNIT_ASSERT(nested);
    CPPUNIT_ASSERT(!inserted);
    CPPUNIT_ASSERT(book->FindPage(candidate) == wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(book->GetPageCount(),
                         book->GetToolBar()->GetToolsCount());

    delete removedPage;
    delete candidate;
}

void ToolbookTestCase::BitmapLookupSameCandidateIsConsumed()
{
    ToolbookForTesting* const book = m_toolbook;
    wxPanel* const candidate = new wxPanel(book);
    ReentrantImageList* const images = new ReentrantImageList(32, 32);
    images->Add(wxArtProvider::GetIcon(
        wxART_INFORMATION, wxART_OTHER, wxSize(32, 32)));
    book->AssignImageList(images);
    m_list = images;

    bool nested = false;
    images->SetGetIconHook(
        [book, candidate, &nested]()
        {
            if ( !nested )
            {
                nested = true;
                CPPUNIT_ASSERT(book->AddPage(
                    candidate, "nested candidate", false, 0));
            }
        });

    CPPUNIT_ASSERT(book->InsertPage(
        1, candidate, "outer candidate", false, 0));
    CPPUNIT_ASSERT(nested);
    CPPUNIT_ASSERT(book->FindPage(candidate) != wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(book->GetPageCount(),
                         book->GetToolBar()->GetToolsCount());

    size_t occurrences = 0;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        if ( book->GetPage(i) == candidate )
            ++occurrences;
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), occurrences);
}

void ToolbookTestCase::InsertHideReentryKeepsOwnership()
{
    ToolbookForTesting* const book = m_toolbook;
    wxPanel* const candidate = new wxPanel(book);
    bool nested = false;
    book->SetShowHook(
        [book, candidate, &nested](wxWindow* page, bool show)
        {
            if ( page == candidate && !show && !nested )
            {
                nested = true;
                book->ChangeSelection(2);
            }
        });

    const bool inserted =
        book->InsertPage(1, candidate, "reentrant insertion");

    CPPUNIT_ASSERT(nested);
    CPPUNIT_ASSERT(inserted);
    CPPUNIT_ASSERT_EQUAL(2, book->GetSelection());
    CPPUNIT_ASSERT(book->FindPage(candidate) == 1);
    CPPUNIT_ASSERT(!candidate->IsShown());
    CPPUNIT_ASSERT_EQUAL(book->GetPageCount(),
                         book->GetToolBar()->GetToolsCount());
}

void ToolbookTestCase::EmptyRealizeKeepsNoSelection()
{
    wxToolbook* const empty = new wxToolbook(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(200, 100));

    empty->Realize();
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, empty->GetSelection());

    delete empty;
}

#ifdef __WXWINUI__

void ToolbookTestCase::ControllerFailureIsAtomic()
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    CPPUNIT_ASSERT(toolbar);
    m_toolbook->Realize();

    const int selection = m_toolbook->GetSelection();
    std::vector<wxWindow*> pages;
    for ( size_t i = 0; i < m_toolbook->GetPageCount(); ++i )
        pages.push_back(m_toolbook->GetPage(i));

    toolbar->WinUIFailNextRebuildForTesting();
    CPPUNIT_ASSERT(!m_toolbook->RemovePage(1));
    CPPUNIT_ASSERT_EQUAL(pages.size(), m_toolbook->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(pages.size(), toolbar->GetToolsCount());
    CPPUNIT_ASSERT_EQUAL(selection, m_toolbook->GetSelection());
    for ( size_t i = 0; i < pages.size(); ++i )
        CPPUNIT_ASSERT(m_toolbook->GetPage(i) == pages[i]);

    wxWindow* const removed = pages[1];
    CPPUNIT_ASSERT(m_toolbook->RemovePage(1));
    CPPUNIT_ASSERT_EQUAL(pages.size() - 1, m_toolbook->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(pages.size() - 1, toolbar->GetToolsCount());
    delete removed;

    pages.clear();
    for ( size_t i = 0; i < m_toolbook->GetPageCount(); ++i )
        pages.push_back(m_toolbook->GetPage(i));
    const int selectionBeforeDeleteAll = m_toolbook->GetSelection();

    toolbar->WinUIFailNextRebuildForTesting();
    CPPUNIT_ASSERT(!m_toolbook->DeleteAllPages());
    CPPUNIT_ASSERT_EQUAL(pages.size(), m_toolbook->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(pages.size(), toolbar->GetToolsCount());
    CPPUNIT_ASSERT_EQUAL(selectionBeforeDeleteAll,
                         m_toolbook->GetSelection());
    for ( size_t i = 0; i < pages.size(); ++i )
        CPPUNIT_ASSERT(m_toolbook->GetPage(i) == pages[i]);

    // A qualified base call must still dispatch through the historical
    // DoRemovePage() slot. This is the ABI-stable path which avoids adding a
    // fallible bulk-clear hook to wxBookCtrlBase's vtable.
    toolbar->WinUIFailNextRebuildForTesting();
    CPPUNIT_ASSERT(
        !m_toolbook->wxBookCtrlBase::DeleteAllPages());
    CPPUNIT_ASSERT_EQUAL(pages.size(), m_toolbook->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(pages.size(), toolbar->GetToolsCount());
    CPPUNIT_ASSERT_EQUAL(selectionBeforeDeleteAll,
                         m_toolbook->GetSelection());
    for ( size_t i = 0; i < pages.size(); ++i )
        CPPUNIT_ASSERT(m_toolbook->GetPage(i) == pages[i]);

    CPPUNIT_ASSERT(
        m_toolbook->wxBookCtrlBase::DeleteAllPages());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0),
                         m_toolbook->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0),
                         toolbar->GetToolsCount());
}

void ToolbookTestCase::LoadedSelectionReentryProjectsRadio()
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    CPPUNIT_ASSERT(toolbar);
    m_toolbook->Realize();

    ToolbookLoadedSelectionProbe probe;
    probe.book = m_toolbook;
    // Select the candidate itself. Its XAML peer is provisional in Loaded,
    // while wxToolBarBase publishes its wrapper only after DoInsertTool().
    probe.selection = 1;
    toolbar->WinUISetNextRebuildLoadedHookForTesting(
        &ChangeToolbookSelectionDuringLoaded, &probe);

    wxPanel* const candidate = new wxPanel(m_toolbook);
    bool changedEventObserved = false;
    m_toolbook->Bind(
        wxEVT_TOOLBOOK_PAGE_CHANGED,
        [this, toolbar, candidate,
         &changedEventObserved](wxBookCtrlEvent& event)
        {
            if ( event.GetSelection() != 1 )
            {
                event.Skip();
                return;
            }

            changedEventObserved = true;
            CPPUNIT_ASSERT_EQUAL(
                m_toolbook->GetPageCount(),
                toolbar->GetToolsCount());
            wxWinUIToolPeerSnapshot candidateSnapshot;
            CPPUNIT_ASSERT(
                toolbar->WinUIGetToolPeerStateForTesting(
                    candidate->GetId(), &candidateSnapshot));
            CPPUNIT_ASSERT(candidateSnapshot.toggled);
            event.Skip();
        });
    CPPUNIT_ASSERT(
        m_toolbook->InsertPage(1, candidate, "loaded-selection"));
    CPPUNIT_ASSERT(probe.invoked);
    CPPUNIT_ASSERT(changedEventObserved);
    CPPUNIT_ASSERT_EQUAL(1, m_toolbook->GetSelection());

    for ( size_t i = 0; i < m_toolbook->GetPageCount(); ++i )
    {
        wxWinUIToolPeerSnapshot snapshot;
        CPPUNIT_ASSERT(toolbar->WinUIGetToolPeerStateForTesting(
            m_toolbook->GetPage(i)->GetId(), &snapshot));
        const bool selected =
            i == static_cast<size_t>(m_toolbook->GetSelection());
        CPPUNIT_ASSERT_EQUAL(selected, snapshot.toggled);
    }
}

void ToolbookTestCase::PageTextProjectsImmediately()
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    CPPUNIT_ASSERT(toolbar);
    m_toolbook->Realize();

    const int pageId = m_toolbook->GetPage(1)->GetId();
    CPPUNIT_ASSERT(
        m_toolbook->SetPageText(1, "projected immediately"));
    CPPUNIT_ASSERT_EQUAL(
        wxString("projected immediately"),
        m_toolbook->GetPageText(1));

    wxWinUIToolPeerSnapshot snapshot;
    CPPUNIT_ASSERT(
        toolbar->WinUIGetToolPeerStateForTesting(pageId, &snapshot));
    CPPUNIT_ASSERT_EQUAL(
        wxString("projected immediately"),
        snapshot.automationName);
}

void ToolbookTestCase::DuplicatePageIdsUsePositionalController()
{
    constexpr int duplicateId = 7341;
    ToolbookForTesting book(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(360, 180));
    wxPanel* const first = new wxPanel(&book, duplicateId);
    wxPanel* const second = new wxPanel(&book, duplicateId);

    wxImageList* const images = new wxImageList(16, 16);
    wxBitmap bitmap(16, 16);
    CPPUNIT_ASSERT(bitmap.IsOk());
    CPPUNIT_ASSERT(images->Add(bitmap) != wxNOT_FOUND);
    book.AssignImageList(images);

    CPPUNIT_ASSERT(book.AddPage(first, "first", true));
    CPPUNIT_ASSERT(book.AddPage(second, "second"));
    book.Realize();

    wxToolBarBase* const toolbar = book.GetToolBar();
    CPPUNIT_ASSERT(toolbar);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2),
                         toolbar->GetToolsCount());
    wxToolBarToolBase* const firstTool = toolbar->GetToolByPos(0);
    wxToolBarToolBase* const secondTool = toolbar->GetToolByPos(1);
    CPPUNIT_ASSERT(firstTool);
    CPPUNIT_ASSERT(secondTool);
    CPPUNIT_ASSERT(firstTool != secondTool);

    CPPUNIT_ASSERT(book.SetPageText(1, "second-exact"));
    CPPUNIT_ASSERT_EQUAL(wxString("first"), firstTool->GetLabel());
    CPPUNIT_ASSERT_EQUAL(wxString("second-exact"), secondTool->GetLabel());
    CPPUNIT_ASSERT_EQUAL(wxString("second-exact"), book.GetPageText(1));

    book.ChangeSelection(1);
    CPPUNIT_ASSERT(!firstTool->IsToggled());
    CPPUNIT_ASSERT(secondTool->IsToggled());

    CPPUNIT_ASSERT(book.EnablePage(1, false));
    CPPUNIT_ASSERT(firstTool->IsEnabled());
    CPPUNIT_ASSERT(!secondTool->IsEnabled());

    CPPUNIT_ASSERT(!firstTool->GetNormalBitmapBundle().IsOk());
    CPPUNIT_ASSERT(book.SetPageImage(1, 0));
    CPPUNIT_ASSERT(!firstTool->GetNormalBitmapBundle().IsOk());
    CPPUNIT_ASSERT(secondTool->GetNormalBitmapBundle().IsOk());
}

void ToolbookTestCase::LoadedTopologyReentryIsRejected()
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    CPPUNIT_ASSERT(toolbar);
    m_toolbook->Realize();

    wxWindow* const removed = m_toolbook->GetPage(0);
    wxPanel* const candidate = new wxPanel(m_toolbook);
    ToolbookLoadedTopologyProbe probe;
    probe.book = m_toolbook;
    probe.candidate = candidate;
    toolbar->WinUISetNextRebuildLoadedHookForTesting(
        &MutateToolbookTopologyDuringLoaded, &probe);

    CPPUNIT_ASSERT(m_toolbook->RemovePage(0));
    CPPUNIT_ASSERT(probe.invoked);
    CPPUNIT_ASSERT(!probe.nestedRemove);
    CPPUNIT_ASSERT(!probe.nestedAdd);
    CPPUNIT_ASSERT(!probe.nestedDeleteAll);
    CPPUNIT_ASSERT(m_toolbook->FindPage(candidate) == wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(m_toolbook->GetPageCount(),
                         toolbar->GetToolsCount());

    delete removed;
    delete candidate;
}

#endif // __WXWINUI__

#endif //wxUSE_TOOLBOOK
