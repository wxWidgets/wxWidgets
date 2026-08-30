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
#include <memory>
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

class ToolbookTestCase : public BookCtrlBaseTestCase
{
public:
    ToolbookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_toolbook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGING; }

    virtual void Realize() override { m_toolbook->GetToolBar()->Realize(); }

    std::unique_ptr<ToolbookForTesting> m_toolbook;

    wxDECLARE_NO_COPY_CLASS(ToolbookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(ToolbookTestCase, "Toolbook",
                       "[toolbook][book][ToolbookTestCase]");

ToolbookTestCase::ToolbookTestCase()
{
    m_toolbook = make_unique<ToolbookForTesting>(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}


TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::ToolBar", "[toolbook][ToolbookTestCase]")
{
    wxToolBar* toolbar = static_cast<wxToolBar*>(m_toolbook->GetToolBar());

    CHECK(toolbar);
    CHECK(toolbar->GetToolsCount() == 3);
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::VetoRestoresToolById",
                 "[toolbook][ToolbookTestCase]")
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    REQUIRE(toolbar);
    REQUIRE(m_panel1->GetId() != 0);

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

    REQUIRE(m_toolbook->GetSelection() == 0);
    REQUIRE(toolbar->GetToolState(m_panel1->GetId()));
    REQUIRE(!toolbar->GetToolState(m_panel2->GetId()));
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::BitmapLookupReentryIsPrePublication",
                 "[toolbook][ToolbookTestCase]")
{
    ToolbookForTesting* const book = m_toolbook.get();
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
                REQUIRE(book->RemovePage(2));
            }
        });

    const bool inserted =
        book->InsertPage(1, candidate, "pre-publication reentry", false, 0);

    REQUIRE(nested);
    REQUIRE(!inserted);
    REQUIRE(book->FindPage(candidate) == wxNOT_FOUND);
    REQUIRE(book->GetToolBar()->GetToolsCount() == book->GetPageCount());

    delete removedPage;
    delete candidate;
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::BitmapLookupSameCandidateIsConsumed",
                 "[toolbook][ToolbookTestCase]")
{
    ToolbookForTesting* const book = m_toolbook.get();
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
                REQUIRE(book->AddPage(
                    candidate, "nested candidate", false, 0));
            }
        });

    REQUIRE(book->InsertPage(
        1, candidate, "outer candidate", false, 0));
    REQUIRE(nested);
    REQUIRE(book->FindPage(candidate) != wxNOT_FOUND);
    REQUIRE(book->GetToolBar()->GetToolsCount() == book->GetPageCount());

    size_t occurrences = 0;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        if ( book->GetPage(i) == candidate )
            ++occurrences;
    }
    REQUIRE(occurrences == static_cast<size_t>(1));
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::InsertHideReentryKeepsOwnership",
                 "[toolbook][ToolbookTestCase]")
{
    ToolbookForTesting* const book = m_toolbook.get();
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

    REQUIRE(nested);
    REQUIRE(inserted);
    REQUIRE(book->GetSelection() == 2);
    REQUIRE(book->FindPage(candidate) == 1);
    REQUIRE(!candidate->IsShown());
    REQUIRE(book->GetToolBar()->GetToolsCount() == book->GetPageCount());
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::EmptyRealizeKeepsNoSelection",
                 "[toolbook][ToolbookTestCase]")
{
    wxToolbook* const empty = new wxToolbook(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(200, 100));

    empty->Realize();
    REQUIRE(empty->GetSelection() == wxNOT_FOUND);

    delete empty;
}

#ifdef __WXWINUI__

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::ControllerFailureIsAtomic",
                 "[toolbook][ToolbookTestCase]")
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    REQUIRE(toolbar);
    m_toolbook->Realize();

    const int selection = m_toolbook->GetSelection();
    std::vector<wxWindow*> pages;
    for ( size_t i = 0; i < m_toolbook->GetPageCount(); ++i )
        pages.push_back(m_toolbook->GetPage(i));

    toolbar->WinUIFailNextRebuildForTesting();
    REQUIRE(!m_toolbook->RemovePage(1));
    REQUIRE(m_toolbook->GetPageCount() == pages.size());
    REQUIRE(toolbar->GetToolsCount() == pages.size());
    REQUIRE(m_toolbook->GetSelection() == selection);
    for ( size_t i = 0; i < pages.size(); ++i )
        REQUIRE(m_toolbook->GetPage(i) == pages[i]);

    wxWindow* const removed = pages[1];
    REQUIRE(m_toolbook->RemovePage(1));
    REQUIRE(m_toolbook->GetPageCount() == pages.size() - 1);
    REQUIRE(toolbar->GetToolsCount() == pages.size() - 1);
    delete removed;

    pages.clear();
    for ( size_t i = 0; i < m_toolbook->GetPageCount(); ++i )
        pages.push_back(m_toolbook->GetPage(i));
    const int selectionBeforeDeleteAll = m_toolbook->GetSelection();

    toolbar->WinUIFailNextRebuildForTesting();
    REQUIRE(!m_toolbook->DeleteAllPages());
    REQUIRE(m_toolbook->GetPageCount() == pages.size());
    REQUIRE(toolbar->GetToolsCount() == pages.size());
    REQUIRE(m_toolbook->GetSelection() == selectionBeforeDeleteAll);
    for ( size_t i = 0; i < pages.size(); ++i )
        REQUIRE(m_toolbook->GetPage(i) == pages[i]);

    // A qualified base call must still dispatch through the historical
    // DoRemovePage() slot. This is the ABI-stable path which avoids adding a
    // fallible bulk-clear hook to wxBookCtrlBase's vtable.
    toolbar->WinUIFailNextRebuildForTesting();
    REQUIRE(
        !m_toolbook->wxBookCtrlBase::DeleteAllPages());
    REQUIRE(m_toolbook->GetPageCount() == pages.size());
    REQUIRE(toolbar->GetToolsCount() == pages.size());
    REQUIRE(m_toolbook->GetSelection() == selectionBeforeDeleteAll);
    for ( size_t i = 0; i < pages.size(); ++i )
        REQUIRE(m_toolbook->GetPage(i) == pages[i]);

    REQUIRE(
        m_toolbook->wxBookCtrlBase::DeleteAllPages());
    REQUIRE(m_toolbook->GetPageCount() == static_cast<size_t>(0));
    REQUIRE(toolbar->GetToolsCount() == static_cast<size_t>(0));
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::LoadedSelectionReentryProjectsRadio",
                 "[toolbook][ToolbookTestCase]")
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    REQUIRE(toolbar);
    m_toolbook->Realize();

    ToolbookLoadedSelectionProbe probe;
    probe.book = m_toolbook.get();
    // Select the candidate itself. Its XAML peer is provisional in Loaded,
    // while wxToolBarBase publishes its wrapper only after DoInsertTool().
    probe.selection = 1;
    toolbar->WinUISetNextRebuildLoadedHookForTesting(
        &ChangeToolbookSelectionDuringLoaded, &probe);

    wxPanel* const candidate = new wxPanel(m_toolbook.get());
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
            REQUIRE(toolbar->GetToolsCount() == m_toolbook->GetPageCount());
            wxWinUIToolPeerSnapshot candidateSnapshot;
            REQUIRE(
                toolbar->WinUIGetToolPeerStateForTesting(
                    candidate->GetId(), &candidateSnapshot));
            REQUIRE(candidateSnapshot.toggled);
            event.Skip();
        });
    REQUIRE(
        m_toolbook->InsertPage(1, candidate, "loaded-selection"));
    REQUIRE(probe.invoked);
    REQUIRE(changedEventObserved);
    REQUIRE(m_toolbook->GetSelection() == 1);

    for ( size_t i = 0; i < m_toolbook->GetPageCount(); ++i )
    {
        wxWinUIToolPeerSnapshot snapshot;
        REQUIRE(toolbar->WinUIGetToolPeerStateForTesting(
            m_toolbook->GetPage(i)->GetId(), &snapshot));
        const bool selected =
            i == static_cast<size_t>(m_toolbook->GetSelection());
        REQUIRE(snapshot.toggled == selected);
    }
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::PageTextProjectsImmediately",
                 "[toolbook][ToolbookTestCase]")
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    REQUIRE(toolbar);
    m_toolbook->Realize();

    const int pageId = m_toolbook->GetPage(1)->GetId();
    REQUIRE(
        m_toolbook->SetPageText(1, "projected immediately"));
    REQUIRE(m_toolbook->GetPageText(1) == wxString("projected immediately"));

    wxWinUIToolPeerSnapshot snapshot;
    REQUIRE(
        toolbar->WinUIGetToolPeerStateForTesting(pageId, &snapshot));
    REQUIRE(snapshot.automationName == wxString("projected immediately"));
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::DuplicatePageIdsUsePositionalController",
                 "[toolbook][ToolbookTestCase]")
{
    constexpr int duplicateId = 7341;
    ToolbookForTesting book(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(360, 180));
    wxPanel* const first = new wxPanel(&book, duplicateId);
    wxPanel* const second = new wxPanel(&book, duplicateId);

    wxImageList* const images = new wxImageList(16, 16);
    wxBitmap bitmap(16, 16);
    REQUIRE(bitmap.IsOk());
    REQUIRE(images->Add(bitmap) != wxNOT_FOUND);
    book.AssignImageList(images);

    REQUIRE(book.AddPage(first, "first", true));
    REQUIRE(book.AddPage(second, "second"));
    book.Realize();

    wxToolBarBase* const toolbar = book.GetToolBar();
    REQUIRE(toolbar);
    REQUIRE(toolbar->GetToolsCount() == static_cast<size_t>(2));
    wxToolBarToolBase* const firstTool = toolbar->GetToolByPos(0);
    wxToolBarToolBase* const secondTool = toolbar->GetToolByPos(1);
    REQUIRE(firstTool);
    REQUIRE(secondTool);
    REQUIRE(firstTool != secondTool);

    REQUIRE(book.SetPageText(1, "second-exact"));
    REQUIRE(firstTool->GetLabel() == wxString("first"));
    REQUIRE(secondTool->GetLabel() == wxString("second-exact"));
    REQUIRE(book.GetPageText(1) == wxString("second-exact"));

    book.ChangeSelection(1);
    REQUIRE(!firstTool->IsToggled());
    REQUIRE(secondTool->IsToggled());

    REQUIRE(book.EnablePage(1, false));
    REQUIRE(firstTool->IsEnabled());
    REQUIRE(!secondTool->IsEnabled());

    REQUIRE(!firstTool->GetNormalBitmapBundle().IsOk());
    REQUIRE(book.SetPageImage(1, 0));
    REQUIRE(!firstTool->GetNormalBitmapBundle().IsOk());
    REQUIRE(secondTool->GetNormalBitmapBundle().IsOk());
}

TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::LoadedTopologyReentryIsRejected",
                 "[toolbook][ToolbookTestCase]")
{
    wxToolBar* const toolbar =
        static_cast<wxToolBar*>(m_toolbook->GetToolBar());
    REQUIRE(toolbar);
    m_toolbook->Realize();

    wxWindow* const removed = m_toolbook->GetPage(0);
    wxPanel* const candidate = new wxPanel(m_toolbook.get());
    ToolbookLoadedTopologyProbe probe;
    probe.book = m_toolbook.get();
    probe.candidate = candidate;
    toolbar->WinUISetNextRebuildLoadedHookForTesting(
        &MutateToolbookTopologyDuringLoaded, &probe);

    REQUIRE(m_toolbook->RemovePage(0));
    REQUIRE(probe.invoked);
    REQUIRE(!probe.nestedRemove);
    REQUIRE(!probe.nestedAdd);
    REQUIRE(!probe.nestedDeleteAll);
    REQUIRE(m_toolbook->FindPage(candidate) == wxNOT_FOUND);
    REQUIRE(toolbar->GetToolsCount() == m_toolbook->GetPageCount());

    delete removed;
    delete candidate;
}

#endif // __WXWINUI__

#endif //wxUSE_TOOLBOOK
