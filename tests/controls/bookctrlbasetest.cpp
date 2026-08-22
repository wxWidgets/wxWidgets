///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bookctrlbasetest.cpp
// Purpose:     wxBookCtrlBase unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BOOKCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/bookctrl.h"
#include "wx/weakref.h"
#include "bookctrlbasetest.h"
#include "testableframe.h"

#include <functional>
#include <utility>
#include <vector>

namespace
{

class ReentrantIsShownPanel final : public wxPanel
{
public:
    explicit ReentrantIsShownPanel(wxWindow* const parent)
        : wxPanel(parent)
    {
    }

    void SetIsShownHook(std::function<void()> hook)
    {
        m_hook = std::move(hook);
    }

    bool IsShown() const override
    {
        const bool shown = wxPanel::IsShown();
        if ( m_hook && !m_inHook )
        {
            m_inHook = true;
            m_hook();
            m_inHook = false;
        }

        return shown;
    }

private:
    std::function<void()> m_hook;
    mutable bool m_inHook{false};
};

// A deliberately small concrete wxBookCtrlBase. In particular it doesn't
// override DoSize(), allowing the tests below to exercise the exact common
// implementation rather than wxSimplebook::DoSize().
class BookLifetimeHarness final : public wxBookCtrlBase
{
public:
    bool Create(wxWindow* const parent)
    {
        if ( !wxBookCtrlBase::Create(parent, wxID_ANY,
                                     wxDefaultPosition, wxSize(320, 240),
                                     wxBK_TOP, "book-lifetime-harness") )
        {
            return false;
        }

        m_bookctrl = new wxPanel(this, wxID_ANY,
                                 wxDefaultPosition, wxSize(80, 24));
        return true;
    }

    size_t GetPageCount() const override
    {
        ++m_pageCountQueries;
        if ( m_getPageCountHook && !m_inGetPageCountHook )
        {
            m_inGetPageCountHook = true;
            m_getPageCountHook();
            m_inGetPageCountHook = false;
        }
        return wxBookCtrlBase::GetPageCount();
    }

    wxWindow* GetPage(size_t n) const override
    {
        ++m_pageQueries;
        if ( m_getPageHook && !m_inGetPageHook )
        {
            m_inGetPageHook = true;
            m_getPageHook(n);
            m_inGetPageHook = false;
        }
        return wxBookCtrlBase::GetPage(n);
    }

    bool InsertPage(size_t n,
                    wxWindow* page,
                    const wxString& text,
                    bool select = false,
                    int image = NO_IMAGE) override
    {
        const InsertPageResult modelResult =
            DoInsertPageIntoModel(n, page, text, select, image);
        if ( modelResult == InsertPageResult::Failed )
            return false;
        if ( modelResult == InsertPageResult::OwnershipConsumed )
            return true;

        m_controllerTexts.insert(m_controllerTexts.begin() + n, text);
        m_images.insert(m_images.begin() + n, image);

        if ( !DoSetSelectionAfterInsertion(n, select) && page )
            DoShowPage(page, false);

        return true;
    }

    int SetSelection(size_t n) override
    {
        return DoSetSelection(n, SetSelection_SendEvent);
    }

    int ChangeSelection(size_t n) override
    {
        return DoSetSelection(n);
    }

    bool SetPageText(size_t n, const wxString& text) override
    {
        if ( n >= m_controllerTexts.size() )
            return false;

        m_controllerTexts[n] = text;
        return true;
    }

    wxString GetPageText(size_t n) const override
    {
        return n < m_controllerTexts.size()
                    ? m_controllerTexts[n]
                    : wxString();
    }

    int GetPageImage(size_t n) const override
    {
        return n < m_images.size() ? m_images[n] : NO_IMAGE;
    }

    bool SetPageImage(size_t n, int image) override
    {
        if ( n >= m_images.size() )
            return false;

        m_images[n] = image;
        return true;
    }

    bool IsControllerSynchronized() const
    {
        return m_controllerTexts.size() == GetPageCount() &&
               m_images.size() == GetPageCount();
    }

    void ReplaceControllerForTesting()
    {
        delete m_bookctrl;
        m_bookctrl = new wxPanel(this, wxID_ANY,
                                 wxDefaultPosition, wxSize(80, 24));
    }

    void FailNextRemovalAfterPartialControllerMutation()
    {
        m_removalsBeforePartialControllerFailure = 0;
    }

    void FailRemovalAfterSuccessfulSteps(size_t successfulSteps)
    {
        m_removalsBeforePartialControllerFailure =
            static_cast<int>(successfulSteps);
    }

    void ReplaceControllerOnNextRemoval()
    {
        m_replaceControllerOnNextRemoval = true;
    }

    void SetShowHook(std::function<void(wxWindow*, bool)> hook)
    {
        m_showHook = std::move(hook);
    }

    void SetGetPageHook(std::function<void(size_t)> hook)
    {
        m_getPageHook = std::move(hook);
    }

    void SetGetPageCountHook(std::function<void()> hook)
    {
        m_getPageCountHook = std::move(hook);
    }

    void SizePagesForTesting()
    {
        wxBookCtrlBase::DoSize();
    }

    bool ReconcileVisibilityForTesting()
    {
        return DoReconcilePageVisibility();
    }

    void ResetLookupCounters()
    {
        m_pageQueries = 0;
        m_pageCountQueries = 0;
    }

    size_t GetPageLookupCount() const
    {
        return m_pageQueries;
    }

protected:
    void UpdateSelectedPage(size_t WXUNUSED(selection)) override
    {
    }

    wxBookCtrlEvent* CreatePageChangingEvent() const override
    {
        return new wxBookCtrlEvent(wxEVT_BOOKCTRL_PAGE_CHANGING, GetId());
    }

    void MakeChangedEvent(wxBookCtrlEvent& event) override
    {
        event.SetEventType(wxEVT_BOOKCTRL_PAGE_CHANGED);
    }

    wxWindow* DoRemovePage(size_t page) override
    {
        if ( m_replaceControllerOnNextRemoval )
        {
            m_replaceControllerOnNextRemoval = false;
            ReplaceControllerForTesting();
            return nullptr;
        }

        if ( m_removalsBeforePartialControllerFailure == 0 )
        {
            m_removalsBeforePartialControllerFailure = -1;
            const wxString text = m_controllerTexts.at(page);
            const int image = m_images.at(page);

            // Model a native controller which reports failure after a partial
            // erase. A conforming existing DoRemovePage() slot must restore
            // its projection before returning failure to DeleteAllPages().
            m_controllerTexts.erase(m_controllerTexts.begin() + page);
            m_images.erase(m_images.begin() + page);
            m_controllerTexts.insert(m_controllerTexts.begin() + page, text);
            m_images.insert(m_images.begin() + page, image);
            return nullptr;
        }

        if ( m_removalsBeforePartialControllerFailure > 0 )
            --m_removalsBeforePartialControllerFailure;

        wxWindow* const removed = wxBookCtrlBase::DoRemovePage(page);
        if ( removed )
        {
            m_controllerTexts.erase(m_controllerTexts.begin() + page);
            m_images.erase(m_images.begin() + page);
            DoSetSelectionAfterRemoval(page);
        }

        return removed;
    }

    void DoShowPage(wxWindow* page, bool show) override
    {
        page->Show(show);
        if ( m_showHook )
            m_showHook(page, show);
    }

private:
    std::vector<wxString> m_controllerTexts;
    std::vector<int> m_images;
    std::function<void(wxWindow*, bool)> m_showHook;
    std::function<void(size_t)> m_getPageHook;
    std::function<void()> m_getPageCountHook;
    mutable bool m_inGetPageHook{false};
    mutable bool m_inGetPageCountHook{false};
    mutable size_t m_pageQueries{0};
    mutable size_t m_pageCountQueries{0};
    int m_removalsBeforePartialControllerFailure{-1};
    bool m_replaceControllerOnNextRemoval{false};
};

BookLifetimeHarness* CreateLifetimeBook()
{
    BookLifetimeHarness* const book = new BookLifetimeHarness;
    if ( !book->Create(wxTheApp->GetTopWindow()) )
    {
        delete book;
        return nullptr;
    }

    if ( !book->AddPage(new wxPanel(book), "first") ||
            !book->AddPage(new wxPanel(book), "second") ||
            !book->AddPage(new wxPanel(book), "third") )
    {
        delete book;
        return nullptr;
    }

    book->ChangeSelection(0);
    return book;
}

} // anonymous namespace

void BookCtrlBaseTestCase::AddPanels()
{
    wxBookCtrlBase * const base = GetBase();

    wxSize size(32, 32);

    m_list = new wxImageList(size.x, size.y);
    m_list->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, size));
    m_list->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size));
    m_list->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, size));

    base->AssignImageList(m_list);

    Realize();

    m_panel1 = new wxPanel(base);
    m_panel2 = new wxPanel(base);
    m_panel3 = new wxPanel(base);

    base->AddPage(m_panel1, "Panel &1", false, 0);
    base->AddPage(m_panel2, "Panel 2", false, 1);
    base->AddPage(m_panel3, "Panel 3", false, 2);
}

void BookCtrlBaseTestCase::Selection()
{
    wxBookCtrlBase * const base = GetBase();

    base->SetSelection(0);

    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel1, wxWindow), base->GetCurrentPage());

    base->AdvanceSelection(false);

    CPPUNIT_ASSERT_EQUAL(2, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel3, wxWindow), base->GetCurrentPage());

    base->AdvanceSelection();

    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel1, wxWindow), base->GetCurrentPage());

    base->ChangeSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel2, wxWindow), base->GetCurrentPage());
}

void BookCtrlBaseTestCase::Text()
{
    wxBookCtrlBase * const base = GetBase();

    const wxString expected(HasBrokenMnemonics() ? "Panel 1" : "Panel &1");
    CPPUNIT_ASSERT_EQUAL(expected, base->GetPageText(0));

    base->SetPageText(1, "Some other string");

    CPPUNIT_ASSERT_EQUAL("Some other string", base->GetPageText(1));

    base->SetPageText(2, "string with\nline break");

    CPPUNIT_ASSERT_EQUAL("string with\nline break", base->GetPageText(2));

    if ( !HasBrokenMnemonics() )
    {
        base->SetPageText(0, "With &mnemonic");
        CPPUNIT_ASSERT_EQUAL("With &mnemonic", base->GetPageText(0));
    }
}

void BookCtrlBaseTestCase::PageManagement()
{
    wxBookCtrlBase * const base = GetBase();

    base->InsertPage(0, new wxPanel(base), "New Panel", true, 0);

    Realize();

    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(4, base->GetPageCount());

    // Change the selection to verify that deleting a page before the currently
    // selected one correctly updates the selection.
    base->SetSelection(2);
    CPPUNIT_ASSERT_EQUAL(2, base->GetSelection());

    base->DeletePage(1);

    CPPUNIT_ASSERT_EQUAL(3, base->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(1, base->GetSelection());

    base->RemovePage(0);

    CPPUNIT_ASSERT_EQUAL(2, base->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());

    base->DeleteAllPages();

    CPPUNIT_ASSERT_EQUAL(0, base->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(-1, base->GetSelection());
}

void BookCtrlBaseTestCase::ChangeEvents()
{
    wxBookCtrlBase * const base = GetBase();

    base->SetSelection(0);

    EventCounter changing(base, GetChangingEvent());
    EventCounter changed(base, GetChangedEvent());

    base->SetSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());

    changed.Clear();
    changing.Clear();
    base->ChangeSelection(2);

    CPPUNIT_ASSERT_EQUAL(0, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, changed.GetCount());

    base->AdvanceSelection();

    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());

    changed.Clear();
    changing.Clear();
    base->AdvanceSelection(false);

    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());
}

void BookCtrlBaseTestCase::Image()
{
    wxBookCtrlBase * const base = GetBase();

    //Check AddPanels() set things correctly
    CPPUNIT_ASSERT_EQUAL(m_list, base->GetImageList());
    CPPUNIT_ASSERT_EQUAL(0, base->GetPageImage(0));
    CPPUNIT_ASSERT_EQUAL(1, base->GetPageImage(1));
    CPPUNIT_ASSERT_EQUAL(2, base->GetPageImage(2));

    base->SetPageImage(0, 2);

    CPPUNIT_ASSERT_EQUAL(2, base->GetPageImage(0));
}

TEST_CASE("wxBookCtrlBase::SelectionTransaction",
          "[bookctrl][lifetime][reentrancy]")
{
    SECTION("Nested selection is authoritative")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        int changingCount = 0;
        int changedCount = 0;
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [book, &changingCount](wxBookCtrlEvent& event)
            {
                ++changingCount;
                if ( event.GetSelection() == 1 )
                    book->ChangeSelection(2);
            });
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGED,
            [&changedCount](wxBookCtrlEvent&)
            {
                ++changedCount;
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(book->GetSelection() == 2);
        CHECK(changingCount == 1);
        CHECK(changedCount == 0);
        CHECK_FALSE(book->GetPage(0)->IsShown());
        CHECK(book->GetPage(2)->IsShown());

        delete book;
    }

    SECTION("Nested ABA selection is still the latest writer")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        int changingCount = 0;
        int changedCount = 0;
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [book, &changingCount](wxBookCtrlEvent& event)
            {
                ++changingCount;
                if ( event.GetSelection() == 1 )
                {
                    book->ChangeSelection(2);
                    book->ChangeSelection(0);
                }
            });
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGED,
            [&changedCount](wxBookCtrlEvent&)
            {
                ++changedCount;
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(book->GetSelection() == 0);
        CHECK(changingCount == 1);
        CHECK(changedCount == 0);
        CHECK(book->GetPage(0)->IsShown());
        CHECK_FALSE(book->GetPage(1)->IsShown());
        CHECK_FALSE(book->GetPage(2)->IsShown());

        delete book;
    }

    SECTION("Reentry while showing the target leaves one visible page")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        wxWindow* const second = book->GetPage(1);
        bool nested = false;
        book->SetShowHook(
            [book, second, &nested](wxWindow* page, bool show)
            {
                if ( show && page == second && !nested )
                {
                    nested = true;
                    book->ChangeSelection(2);
                }
            });

        int changedCount = 0;
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGED,
            [&changedCount](wxBookCtrlEvent&)
            {
                ++changedCount;
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(nested);
        CHECK(book->GetSelection() == 2);
        CHECK(changedCount == 0);
        CHECK_FALSE(book->GetPage(0)->IsShown());
        CHECK_FALSE(book->GetPage(1)->IsShown());
        CHECK(book->GetPage(2)->IsShown());

        delete book;
    }

    SECTION("Vetoed reentry while showing restores committed page")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        wxWindow* const second = book->GetPage(1);
        bool nested = false;
        book->SetShowHook(
            [book, second, &nested](wxWindow* page, bool show)
            {
                if ( show && page == second && !nested )
                {
                    nested = true;
                    book->SetSelection(2);
                }
            });
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [](wxBookCtrlEvent& event)
            {
                if ( event.GetSelection() == 2 )
                    event.Veto();
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(nested);
        CHECK(book->GetSelection() == 0);
        CHECK(book->GetPage(0)->IsShown());
        CHECK_FALSE(book->GetPage(1)->IsShown());
        CHECK_FALSE(book->GetPage(2)->IsShown());

        delete book;
    }

    SECTION("Vetoed reentry from target lookup restores hidden old page")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        bool nested = false;
        book->SetGetPageHook(
            [book, &nested](size_t page)
            {
                if ( page == 1 && !nested &&
                        !book->wxBookCtrlBase::GetPage(0)->IsShown() )
                {
                    nested = true;
                    book->SetSelection(2);
                }
            });
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [](wxBookCtrlEvent& event)
            {
                if ( event.GetSelection() == 2 )
                    event.Veto();
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(nested);
        CHECK(book->GetSelection() == 0);
        CHECK(book->wxBookCtrlBase::GetPage(0)->IsShown());
        CHECK_FALSE(book->wxBookCtrlBase::GetPage(1)->IsShown());
        CHECK_FALSE(book->wxBookCtrlBase::GetPage(2)->IsShown());

        delete book;
    }

    SECTION("Topology reentry while showing reconciles current model")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        wxWindow* const second = book->GetPage(1);
        bool nested = false;
        book->SetShowHook(
            [book, second, &nested](wxWindow* page, bool show)
            {
                if ( show && page == second && !nested )
                {
                    nested = true;
                    CHECK(book->RemovePage(2));
                }
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(nested);
        CHECK(book->GetPageCount() == 2);
        CHECK(book->GetSelection() == 0);
        CHECK(book->GetPage(0)->IsShown());
        CHECK_FALSE(book->GetPage(1)->IsShown());
        CHECK(book->IsControllerSynchronized());

        delete book;
    }

    SECTION("Veto restores the previous selection exactly once")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        int changingCount = 0;
        int changedCount = 0;
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [&changingCount](wxBookCtrlEvent& event)
            {
                ++changingCount;
                event.Veto();
            });
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGED,
            [&changedCount](wxBookCtrlEvent&)
            {
                ++changedCount;
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(book->GetSelection() == 0);
        CHECK(changingCount == 1);
        CHECK(changedCount == 0);

        delete book;
    }

    SECTION("Page mutation aborts the stale outer selection")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        int changedCount = 0;
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [book](wxBookCtrlEvent& event)
            {
                if ( event.GetSelection() == 1 )
                    CHECK(book->DeletePage(1));
            });
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGED,
            [&changedCount](wxBookCtrlEvent&)
            {
                ++changedCount;
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(book->GetPageCount() == 2);
        CHECK(book->GetSelection() == 0);
        CHECK(changedCount == 0);

        delete book;
    }

    SECTION("Self-sustaining visibility reentry is circuit-broken")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        unsigned reentryAttempts = 0;
        book->SetShowHook(
            [book, &reentryAttempts](wxWindow*, bool)
            {
                // This deliberately has no natural fixed point: every
                // visibility callback requests another selection. The high
                // test-only ceiling protects a broken implementation from
                // exhausting the C++ stack while still proving that the
                // production circuit breaker stopped it much earlier.
                if ( reentryAttempts < 256 )
                {
                    ++reentryAttempts;
                    book->ChangeSelection(1);
                }
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(reentryAttempts < 128);
        REQUIRE(book->GetSelection() == 1);

        unsigned shownCount = 0;
        for ( size_t i = 0; i < book->GetPageCount(); ++i )
        {
            if ( book->GetPage(i)->IsShown() )
                ++shownCount;
        }

        CHECK(shownCount == 1);
        CHECK(book->GetCurrentPage()->IsShown());

        // The breaker is scoped to the synchronous chain, not latched on the
        // control: the next independent request must still be honoured.
        book->SetShowHook({});
        CHECK(book->ChangeSelection(2) == 1);
        CHECK(book->GetSelection() == 2);
        CHECK(book->GetPage(2)->IsShown());

        delete book;
    }

    SECTION("Destruction in PAGE_CHANGING is terminal")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        const wxWeakRef<wxWindow> lifetime(book);
        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [book](wxBookCtrlEvent&)
            {
                delete book;
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(lifetime.get() == nullptr);
    }

    SECTION("Controller replacement aborts PAGE_CHANGING transaction")
    {
        BookLifetimeHarness* const book = CreateLifetimeBook();
        REQUIRE(book);

        book->Bind(
            wxEVT_BOOKCTRL_PAGE_CHANGING,
            [book](wxBookCtrlEvent&)
            {
                book->ReplaceControllerForTesting();
            });

        CHECK(book->SetSelection(1) == 0);
        CHECK(book->GetSelection() == 0);
        CHECK(book->GetPage(0)->IsShown());
        CHECK_FALSE(book->GetPage(1)->IsShown());

        delete book;
    }
}

TEST_CASE("wxBookCtrlBase::InsertPageTransaction",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);
    REQUIRE(book->IsControllerSynchronized());

    wxPanel* const candidate = new wxPanel(book);
    candidate->SetSize(1, 1);

    bool mutated = false;
    candidate->Bind(
        wxEVT_SIZE,
        [book, &mutated](wxSizeEvent& event)
        {
            event.Skip();
            if ( !mutated )
            {
                mutated = true;
                CHECK(book->RemovePage(2));
            }
        });

    CHECK_FALSE(book->InsertPage(1, candidate, "candidate"));
    CHECK(mutated);
    CHECK(book->GetPageCount() == 2);
    CHECK(book->FindPage(candidate) == wxNOT_FOUND);
    CHECK(book->IsControllerSynchronized());

    delete book;
}

TEST_CASE("wxBookCtrlBase::InsertPageControllerReplacement",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);
    REQUIRE(book->IsControllerSynchronized());

    wxPanel* const candidate = new wxPanel(book);
    candidate->SetSize(1, 1);

    bool replaced = false;
    candidate->Bind(
        wxEVT_SIZE,
        [book, &replaced](wxSizeEvent& event)
        {
            event.Skip();
            if ( !replaced )
            {
                replaced = true;
                book->ReplaceControllerForTesting();
            }
        });

    CHECK_FALSE(book->InsertPage(1, candidate, "candidate"));
    CHECK(replaced);
    CHECK(book->GetPageCount() == 3);
    CHECK(book->FindPage(candidate) == wxNOT_FOUND);
    CHECK(book->IsControllerSynchronized());

    delete book;
}

TEST_CASE("wxBookCtrlBase::NestedInsertionConsumesCandidate",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxPanel* const candidate = new wxPanel(book);
    candidate->SetSize(1, 1);
    bool nested = false;
    candidate->Bind(
        wxEVT_SIZE,
        [book, candidate, &nested](wxSizeEvent& event)
        {
            event.Skip();
            if ( !nested )
            {
                nested = true;
                CHECK(book->AddPage(candidate, "nested candidate"));
            }
        });

    // The outer transaction is superseded before it publishes anything, but
    // the nested one now owns the same candidate. This is success from the
    // caller's ownership perspective and must not invite a second delete.
    CHECK(book->InsertPage(1, candidate, "outer candidate"));
    CHECK(nested);
    CHECK(book->FindPage(candidate) != wxNOT_FOUND);
    CHECK(book->IsControllerSynchronized());

    size_t occurrences = 0;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        if ( book->GetPage(i) == candidate )
            ++occurrences;
    }
    CHECK(occurrences == 1);

    delete book;
}

TEST_CASE("wxBookCtrlBase::AddPageCountLookupConsumesCandidate",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxPanel* const candidate = new wxPanel(book);
    bool nested = false;
    book->SetGetPageCountHook(
        [book, candidate, &nested]()
        {
            if ( !nested )
            {
                nested = true;
                CHECK(book->AddPage(candidate, "nested candidate"));
            }
        });

    CHECK(book->AddPage(candidate, "outer candidate"));
    book->SetGetPageCountHook({});
    CHECK(nested);
    CHECK(book->FindPage(candidate) != wxNOT_FOUND);
    CHECK(book->IsControllerSynchronized());

    size_t occurrences = 0;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        if ( book->GetPage(i) == candidate )
            ++occurrences;
    }
    CHECK(occurrences == 1);

    delete book;
}

TEST_CASE("wxBookCtrlBase::AddPageCountLookupPreservesAppend",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxPanel* const nestedPage = new wxPanel(book);
    wxPanel* const candidate = new wxPanel(book);
    bool nested = false;
    book->SetGetPageCountHook(
        [book, nestedPage, &nested]()
        {
            if ( !nested )
            {
                nested = true;
                CHECK(book->AddPage(nestedPage, "nested page"));
            }
        });

    CHECK(book->AddPage(candidate, "outer candidate"));
    book->SetGetPageCountHook({});

    REQUIRE(nested);
    REQUIRE(book->GetPageCount() == 5);
    CHECK(book->GetPage(3) == nestedPage);
    CHECK(book->GetPage(4) == candidate);
    CHECK(book->IsControllerSynchronized());

    delete book;
}

TEST_CASE("wxBookCtrlBase::InsertPageVetoReportsEffectiveSelection",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    book->Bind(
        wxEVT_BOOKCTRL_PAGE_CHANGING,
        [](wxBookCtrlEvent& event)
        {
            if ( event.GetSelection() == 3 )
                event.Veto();
        });

    wxPanel* const candidate = new wxPanel(book);
    CHECK(book->InsertPage(3, candidate, "candidate", true));
    CHECK(book->GetSelection() == 0);
    CHECK(book->FindPage(candidate) == 3);
    CHECK_FALSE(candidate->IsShown());
    CHECK(book->IsControllerSynchronized());

    delete book;
}

TEST_CASE("wxBookCtrlBase::DeleteRejectsRepublishingDyingPage",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxWindow* const page = book->GetPage(1);
    bool destroySeen = false;
    bool reinserted = true;
    page->Bind(
        wxEVT_DESTROY,
        [book, page, &destroySeen, &reinserted](wxWindowDestroyEvent& event)
        {
            event.Skip();
            destroySeen = true;
            reinserted = book->AddPage(page, "dying page");
        });

    CHECK(book->DeletePage(1));
    CHECK(destroySeen);
    CHECK_FALSE(reinserted);
    CHECK(book->FindPage(page) == wxNOT_FOUND);
    CHECK(book->IsControllerSynchronized());

    delete book;
}

TEST_CASE("wxBookCtrlBase::RemoveDoesNotHideRepublishedPage",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = new BookLifetimeHarness;
    REQUIRE(book->Create(wxTheApp->GetTopWindow()));

    ReentrantIsShownPanel* const page =
        new ReentrantIsShownPanel(book);
    REQUIRE(book->AddPage(page, "page", true));
    REQUIRE(page->IsShown());

    bool nested = false;
    page->SetIsShownHook(
        [book, page, &nested]()
        {
            if ( !nested )
            {
                nested = true;
                CHECK(book->AddPage(page, "republished", true));
            }
        });

    // RemovePage() has detached the page when it queries its visibility.
    // The query republishes and selects it. That nested writer is
    // authoritative, so the outer remove must neither hide the page nor claim
    // ownership transfer.
    CHECK_FALSE(book->RemovePage(0));
    REQUIRE(nested);
    CHECK(book->FindPage(page) == 0);
    CHECK(book->GetSelection() == 0);
    CHECK(page->wxPanel::IsShown());
    CHECK(book->IsControllerSynchronized());

    page->SetIsShownHook({});
    delete book;
}

TEST_CASE("wxBookCtrlBase::DeleteAllRejectsDestructorWriters",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxPanel* const candidate = new wxPanel(book);
    bool destroySeen = false;
    bool inserted = true;
    book->GetPage(0)->Bind(
        wxEVT_DESTROY,
        [book, candidate, &destroySeen, &inserted](
            wxWindowDestroyEvent& event)
        {
            event.Skip();
            destroySeen = true;
            inserted = book->AddPage(candidate, "nested candidate");
        });

    CHECK(book->DeleteAllPages());
    CHECK(destroySeen);
    CHECK_FALSE(inserted);
    CHECK(book->GetPageCount() == 0);
    CHECK(book->IsControllerSynchronized());

    delete candidate;
    delete book;
}

TEST_CASE("wxBookCtrlBase::DeleteAllFailedControllerStepIsCoherent",
          "[bookctrl][lifetime][transaction]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    std::vector<wxWindow*> pages;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
        pages.push_back(book->GetPage(i));
    const int selection = book->GetSelection();

    SECTION("partial native mutation is rolled back by existing slot")
    {
        book->FailNextRemovalAfterPartialControllerMutation();
    }

    SECTION("controller replacement aborts before either model changes")
    {
        book->ReplaceControllerOnNextRemoval();
    }

    CHECK_FALSE(book->DeleteAllPages());
    CHECK(book->GetPageCount() == pages.size());
    CHECK(book->GetSelection() == selection);
    CHECK(book->IsControllerSynchronized());
    for ( size_t i = 0; i < pages.size(); ++i )
        CHECK(book->GetPage(i) == pages[i]);

    CHECK(book->DeleteAllPages());
    CHECK(book->GetPageCount() == 0);
    CHECK(book->IsControllerSynchronized());
    delete book;
}

TEST_CASE("wxBookCtrlBase::DeleteAllLaterFailureIsCoherentlyPartial",
          "[bookctrl][lifetime][transaction]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxWindow* const first = book->GetPage(0);
    wxWindow* const second = book->GetPage(1);
    wxWindow* const third = book->GetPage(2);
    const wxWeakRef<wxWindow> firstLifetime(first);
    book->FailRemovalAfterSuccessfulSteps(1);

    // The first destructor is irreversible. Failure of the second controller
    // step is therefore reported honestly as a coherent partial result, not
    // as a fictitious all-or-nothing rollback.
    CHECK_FALSE(book->DeleteAllPages());
    CHECK_FALSE(firstLifetime);
    REQUIRE(book->GetPageCount() == 2);
    CHECK(book->GetPage(0) == second);
    CHECK(book->GetPage(1) == third);
    CHECK(book->IsControllerSynchronized());

    CHECK(book->DeleteAllPages());
    CHECK(book->GetPageCount() == 0);
    CHECK(book->IsControllerSynchronized());
    delete book;
}

TEST_CASE("wxBookCtrlBase::DoSizeStopsAfterPageDestruction",
          "[bookctrl][lifetime][size]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxWindow* const first = book->GetPage(0);
    first->SetSize(1, 1);

    bool destroyed = false;
    first->Bind(
        wxEVT_SIZE,
        [book, &destroyed](wxSizeEvent&)
        {
            // The handler destroys its own event source together with the
            // book. Propagation to that dead source is neither possible nor
            // requested.
            if ( !destroyed )
            {
                destroyed = true;
                delete book;
            }
        });

    const wxWeakRef<wxWindow> lifetime(book);
    book->SizePagesForTesting();

    CHECK(destroyed);
    CHECK(lifetime.get() == nullptr);
}

TEST_CASE("wxBookCtrlBase::DoSizeNestedLayoutIsLatestWriter",
          "[bookctrl][lifetime][size][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxWindow* const first = book->GetPage(0);
    first->SetSize(1, 1);

    bool nested = false;
    first->Bind(
        wxEVT_SIZE,
        [book, &nested](wxSizeEvent& event)
        {
            event.Skip();
            if ( !nested )
            {
                nested = true;
                book->SetSize(480, 360);
                book->SizePagesForTesting();
            }
        });

    book->SizePagesForTesting();

    REQUIRE(nested);
    const wxRect latestRect = book->GetPage(0)->GetRect();
    CHECK(book->GetPage(1)->GetRect() == latestRect);
    CHECK(book->GetPage(2)->GetRect() == latestRect);

    delete book;
}

TEST_CASE("wxBookCtrlBase::VisibilityReconcileHonoursNestedSelection",
          "[bookctrl][lifetime][reentrancy]")
{
    BookLifetimeHarness* const book = CreateLifetimeBook();
    REQUIRE(book);

    wxWindow* const first = book->GetPage(0);
    wxWindow* const staleVisible = book->GetPage(1);
    wxWindow* const nestedSelected = book->GetPage(2);
    REQUIRE(staleVisible->Show());

    bool nested = false;
    book->SetShowHook(
        [book, staleVisible, &nested](wxWindow* page, bool show)
        {
            if ( page == staleVisible && !show && !nested )
            {
                nested = true;
                CHECK(book->ChangeSelection(2) == 0);
            }
        });

    // The outer pass starts with page 0 selected and hides the extra visible
    // page 1. Its Show callback selects page 2. The nested writer owns the
    // final visibility; the superseded outer pass must not continue and hide
    // page 2 using its stale selected-page snapshot.
    CHECK_FALSE(book->ReconcileVisibilityForTesting());
    REQUIRE(nested);
    CHECK(book->GetSelection() == 2);
    CHECK_FALSE(first->IsShown());
    CHECK_FALSE(staleVisible->IsShown());
    CHECK(nestedSelected->IsShown());

    delete book;
}

TEST_CASE("wxBookCtrlBase::DoSizeTopologyChecksAreLinear",
          "[bookctrl][size][performance]")
{
    BookLifetimeHarness* const book = new BookLifetimeHarness;
    REQUIRE(book->Create(wxTheApp->GetTopWindow()));

    constexpr size_t PageCount = 256;
    for ( size_t i = 0; i < PageCount; ++i )
    {
        REQUIRE(book->AddPage(
            new wxPanel(book),
            wxString::Format("%u", static_cast<unsigned>(i))));
    }

    book->ResetLookupCounters();
    book->SizePagesForTesting();

    // One lookup builds the lifetime snapshot and one verifies each page.
    // Keep a little headroom for platform-specific virtual queries while
    // making the previous O(N²) validation fail by a wide margin.
    CHECK(book->GetPageLookupCount() <= 3 * PageCount + 16);

    delete book;
}

#endif
