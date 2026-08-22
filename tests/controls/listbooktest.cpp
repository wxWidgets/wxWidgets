///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbooktest.cpp
// Purpose:     wxListbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/listbook.h"
#include "wx/listctrl.h"
#include "wx/weakref.h"
#include "bookctrlbasetest.h"

class ListbookForTesting final : public wxListbook
{
public:
    using wxListbook::wxListbook;

    void UpdateSelectedPageForTesting(size_t selection)
    {
        UpdateSelectedPage(selection);
    }
};

class ListbookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    ListbookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_listbook; }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGING; }

    virtual bool HasBrokenMnemonics() const override { return true; }

    CPPUNIT_TEST_SUITE( ListbookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( ListView );
        CPPUNIT_TEST( InsertItemDestruction );
        CPPUNIT_TEST( InsertItemTopologyReentry );
        CPPUNIT_TEST( SelectDestruction );
        CPPUNIT_TEST( SelectTopologyReentry );
        CPPUNIT_TEST( SetItemTopologyReentry );
        CPPUNIT_TEST( DeleteItemDestruction );
        CPPUNIT_TEST( DeleteItemTopologyReentry );
        CPPUNIT_TEST( DeleteAllItemsDestruction );
        CPPUNIT_TEST( DeleteAllItemsTopologyReentry );
    CPPUNIT_TEST_SUITE_END();

    void ListView();
    void InsertItemDestruction();
    void InsertItemTopologyReentry();
    void SelectDestruction();
    void SelectTopologyReentry();
    void SetItemTopologyReentry();
    void DeleteItemDestruction();
    void DeleteItemTopologyReentry();
    void DeleteAllItemsDestruction();
    void DeleteAllItemsTopologyReentry();

    ListbookForTesting *m_listbook;

    wxDECLARE_NO_COPY_CLASS(ListbookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListbookTestCase );

// also include in its own registry so that these tests can be run alone
wxREGISTER_UNIT_TEST_WITH_TAGS(
    ListbookTestCase,
    "[ListbookTestCase][winui-v0-supported]");

void ListbookTestCase::setUp()
{
    m_listbook = new ListbookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(400, 300));
    AddPanels();
}

void ListbookTestCase::tearDown()
{
    wxDELETE(m_listbook);
}

void ListbookTestCase::ListView()
{
    wxListView* listview = m_listbook->GetListView();

    CPPUNIT_ASSERT(listview);
    CPPUNIT_ASSERT_EQUAL(3, listview->GetItemCount());
    CPPUNIT_ASSERT_EQUAL("Panel 1", listview->GetItemText(0));
}

void ListbookTestCase::InsertItemDestruction()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);
    m_listbook = nullptr;

    list->Bind(
        wxEVT_LIST_INSERT_ITEM,
        [book](wxListEvent&)
        {
            // Destroying the event source is terminal. Calling Skip() here
            // would explicitly request continued dispatch on the dead
            // wxListView event handler.
            delete book;
        });

    const bool inserted = book->InsertPage(
        book->GetPageCount(), new wxPanel(book), "destructive insertion");
    const bool destroyed = !lifetime;
    if ( lifetime )
        delete book;

    // The common model and list controller had already accepted the page
    // before the controller notification destroyed the book. Ownership was
    // consumed (and the page destroyed with it), so the caller must not be
    // invited to delete the candidate again.
    CPPUNIT_ASSERT(inserted);
    CPPUNIT_ASSERT(destroyed);
}

void ListbookTestCase::InsertItemTopologyReentry()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    wxWindow* const removedPage = book->GetPage(0);
    wxPanel* const candidate = new wxPanel(book);
    bool nested = false;

    list->Bind(
        wxEVT_LIST_INSERT_ITEM,
        [book, &nested](wxListEvent& event)
        {
            event.Skip();
            if ( !nested )
            {
                nested = true;
                CPPUNIT_ASSERT(book->RemovePage(0));
            }
        });

    const bool inserted = book->InsertPage(
        book->GetPageCount(), candidate, "reentrant insertion");

    CPPUNIT_ASSERT(nested);
    CPPUNIT_ASSERT(inserted);
    CPPUNIT_ASSERT(book->FindPage(candidate) != wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(book->GetPageCount()),
                         list->GetItemCount());

    size_t shown = 0;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        if ( book->GetPage(i)->IsShown() )
            ++shown;
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), shown);
    CPPUNIT_ASSERT(book->GetCurrentPage());
    CPPUNIT_ASSERT(book->GetCurrentPage()->IsShown());

    delete removedPage;
}

void ListbookTestCase::SelectDestruction()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);
    m_listbook = nullptr;

    list->Bind(
        wxEVT_LIST_ITEM_SELECTED,
        [book](wxListEvent&)
        {
            delete book;
        });

    book->UpdateSelectedPageForTesting(1);
    const bool destroyed = !lifetime;
    if ( lifetime )
        delete book;

    CPPUNIT_ASSERT(destroyed);
}

void ListbookTestCase::SelectTopologyReentry()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    wxWindow* const removedPage = book->GetPage(0);
    bool eventSeen = false;
    bool removed = false;

    list->Bind(
        wxEVT_LIST_ITEM_SELECTED,
        [book, &eventSeen, &removed](wxListEvent& event)
        {
            event.Skip();
            if ( !eventSeen )
            {
                eventSeen = true;
                removed = book->RemovePage(0);
            }
        });

    // Selecting item 1 enters the generic list controller synchronously. The
    // handler removes an earlier item, invalidating every index held by the
    // outer SetItemState() call while leaving the control itself alive.
    book->UpdateSelectedPageForTesting(1);

    CPPUNIT_ASSERT(eventSeen);
    CPPUNIT_ASSERT(removed);
    CPPUNIT_ASSERT(book->FindPage(removedPage) == wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(book->GetPageCount()),
                         list->GetItemCount());

    delete removedPage;
}

void ListbookTestCase::SetItemTopologyReentry()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    wxWindow* const removedPage = book->GetPage(0);
    bool eventSeen = false;
    bool removed = false;

    list->Bind(
        wxEVT_LIST_ITEM_SELECTED,
        [book, &eventSeen, &removed](wxListEvent& event)
        {
            event.Skip();
            if ( !eventSeen )
            {
                eventSeen = true;
                removed = book->RemovePage(0);
            }
        });

    wxListItem item;
    item.SetId(1);
    item.SetMask(wxLIST_MASK_STATE);
    item.SetState(wxLIST_STATE_SELECTED);

    // The item data write has happened, but the nested topology mutation makes
    // the state operation terminal. Propagate this fact through SetItem()
    // instead of refreshing the stale original index and reporting success.
    const bool set = list->SetItem(item);

    CPPUNIT_ASSERT(eventSeen);
    CPPUNIT_ASSERT(removed);
    CPPUNIT_ASSERT(!set);
    CPPUNIT_ASSERT(book->FindPage(removedPage) == wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(book->GetPageCount()),
                         list->GetItemCount());

    delete removedPage;
}

void ListbookTestCase::DeleteItemDestruction()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);
    m_listbook = nullptr;

    list->Bind(
        wxEVT_LIST_DELETE_ITEM,
        [book](wxListEvent&)
        {
            delete book;
        });

    (void)book->RemovePage(0);
    const bool destroyed = !lifetime;
    if ( lifetime )
        delete book;

    CPPUNIT_ASSERT(destroyed);
}

void ListbookTestCase::DeleteItemTopologyReentry()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    wxWindow* const originalFirst = book->GetPage(0);
    wxPanel* const candidate = new wxPanel(book);
    bool nested = false;
    bool inserted = false;
    bool nestedRemoved = true;
    bool nestedDeleteAll = true;

    list->Bind(
        wxEVT_LIST_DELETE_ITEM,
        [book, candidate, &nested, &inserted, &nestedRemoved,
            &nestedDeleteAll](wxListEvent& event)
        {
            event.Skip();
            if ( !nested )
            {
                nested = true;
                inserted = book->AddPage(candidate, "nested insertion");
                nestedRemoved = book->RemovePage(0);
                nestedDeleteAll = book->DeleteAllPages();
            }
        });

    // The outer controller deletion is authoritative. A nested writer must
    // fail before publishing either a page or a controller item, allowing the
    // original removal to complete with both models synchronized.
    const bool removed = book->RemovePage(0);

    CPPUNIT_ASSERT(nested);
    CPPUNIT_ASSERT(!inserted);
    CPPUNIT_ASSERT(!nestedRemoved);
    CPPUNIT_ASSERT(!nestedDeleteAll);
    CPPUNIT_ASSERT(removed);
    CPPUNIT_ASSERT(book->FindPage(originalFirst) == wxNOT_FOUND);
    CPPUNIT_ASSERT(book->FindPage(candidate) == wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(book->GetPageCount()),
                         list->GetItemCount());

    // RemovePage() transfers ownership of the old page; the failed AddPage()
    // leaves ownership of its candidate with the caller.
    delete originalFirst;
    delete candidate;
}

void ListbookTestCase::DeleteAllItemsDestruction()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);
    m_listbook = nullptr;
    bool deleting = false;

    list->Bind(
        wxEVT_LIST_DELETE_ITEM,
        [book, &deleting](wxListEvent&)
        {
            // Keep this deliberately destructive handler one-shot: deleting
            // the book destroys the event source while its item notification
            // is still being dispatched.
            if ( !deleting )
            {
                deleting = true;
                delete book;
            }
        });

    (void)book->DeleteAllPages();
    const bool destroyed = !lifetime;
    if ( lifetime )
        delete book;

    CPPUNIT_ASSERT(destroyed);
}

void ListbookTestCase::DeleteAllItemsTopologyReentry()
{
    ListbookForTesting* const book = m_listbook;
    wxListView* const list = book->GetListView();
    wxPanel* const candidate = new wxPanel(book);

    bool eventSeen = false;
    bool insertResult = true;
    bool removeResult = true;
    bool nestedDeleteAllResult = true;
    size_t modelCountDuringEvent = 0;
    long controllerCountDuringEvent = 0;

    list->Bind(
        wxEVT_LIST_DELETE_ITEM,
        [=, &eventSeen, &insertResult, &removeResult,
            &nestedDeleteAllResult, &modelCountDuringEvent,
            &controllerCountDuringEvent](wxListEvent& event)
        {
            event.Skip();
            if ( eventSeen )
                return;

            eventSeen = true;

            // DeleteAll reuses the existing coherent single-item transaction.
            // All same-book topology writers still fail until the outer bulk
            // operation returns.
            insertResult =
                book->AddPage(candidate, "nested candidate", true);
            removeResult = book->RemovePage(0);
            nestedDeleteAllResult = book->DeleteAllPages();
            modelCountDuringEvent = book->GetPageCount();
            controllerCountDuringEvent = list->GetItemCount();
        });

    const bool outerResult = book->DeleteAllPages();

    CPPUNIT_ASSERT(eventSeen);
    CPPUNIT_ASSERT(!insertResult);
    CPPUNIT_ASSERT(!removeResult);
    CPPUNIT_ASSERT(!nestedDeleteAllResult);
    // Controller-first removal keeps the old common topology visible during
    // the first notification and commits both models together afterwards.
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), modelCountDuringEvent);

    // The precise count during the native delete notification is
    // platform-defined, but it is either the old count or old count - 1.
    CPPUNIT_ASSERT((controllerCountDuringEvent == 2 ||
                    controllerCountDuringEvent == 3));
    CPPUNIT_ASSERT(outerResult);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), book->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(0L, list->GetItemCount());
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, book->GetSelection());

    // Failed insertion leaves ownership with the caller.
    delete candidate;
}

#endif //wxUSE_LISTBOOK
