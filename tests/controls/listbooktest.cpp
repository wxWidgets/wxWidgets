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

#include <memory>

class ListbookForTesting final : public wxListbook
{
public:
    using wxListbook::wxListbook;

    void UpdateSelectedPageForTesting(size_t selection)
    {
        UpdateSelectedPage(selection);
    }
};

class ListbookTestCase : public BookCtrlBaseTestCase
{
public:
    ListbookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_listbook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGING; }

    virtual bool HasBrokenMnemonics() const override { return true; }

    std::unique_ptr<ListbookForTesting> m_listbook;

    wxDECLARE_NO_COPY_CLASS(ListbookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(ListbookTestCase, "Listbook",
                       "[listbook][book][ListbookTestCase][winui-v0-supported]");

ListbookTestCase::ListbookTestCase()
{
    m_listbook = make_unique<ListbookForTesting>(
        wxTheApp->GetTopWindow(), wxID_ANY,
        wxDefaultPosition, wxSize(400, 300));
    AddPanels();
}


TEST_CASE_METHOD(ListbookTestCase, "Listbook::ListView",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    wxListView* listview = m_listbook->GetListView();

    CHECK(listview);
    CHECK(listview->GetItemCount() == 3);
    CHECK(listview->GetItemText(0) == "Panel 1");
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::InsertItemDestruction",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.release();
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);

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
    REQUIRE(inserted);
    REQUIRE(destroyed);
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::InsertItemTopologyReentry",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.get();
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
                REQUIRE(book->RemovePage(0));
            }
        });

    const bool inserted = book->InsertPage(
        book->GetPageCount(), candidate, "reentrant insertion");

    REQUIRE(nested);
    REQUIRE(inserted);
    REQUIRE(book->FindPage(candidate) != wxNOT_FOUND);
    REQUIRE(list->GetItemCount() == static_cast<long>(book->GetPageCount()));

    size_t shown = 0;
    for ( size_t i = 0; i < book->GetPageCount(); ++i )
    {
        if ( book->GetPage(i)->IsShown() )
            ++shown;
    }
    REQUIRE(shown == static_cast<size_t>(1));
    REQUIRE(book->GetCurrentPage());
    REQUIRE(book->GetCurrentPage()->IsShown());

    delete removedPage;
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::SelectDestruction",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.release();
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);

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

    REQUIRE(destroyed);
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::SelectTopologyReentry",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.get();
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

    REQUIRE(eventSeen);
    REQUIRE(removed);
    REQUIRE(book->FindPage(removedPage) == wxNOT_FOUND);
    REQUIRE(list->GetItemCount() == static_cast<long>(book->GetPageCount()));

    delete removedPage;
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::SetItemTopologyReentry",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.get();
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

    REQUIRE(eventSeen);
    REQUIRE(removed);
    REQUIRE(!set);
    REQUIRE(book->FindPage(removedPage) == wxNOT_FOUND);
    REQUIRE(list->GetItemCount() == static_cast<long>(book->GetPageCount()));

    delete removedPage;
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::DeleteItemDestruction",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.release();
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);

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

    REQUIRE(destroyed);
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::DeleteItemTopologyReentry",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.get();
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

    REQUIRE(nested);
    REQUIRE(!inserted);
    REQUIRE(!nestedRemoved);
    REQUIRE(!nestedDeleteAll);
    REQUIRE(removed);
    REQUIRE(book->FindPage(originalFirst) == wxNOT_FOUND);
    REQUIRE(book->FindPage(candidate) == wxNOT_FOUND);
    REQUIRE(list->GetItemCount() == static_cast<long>(book->GetPageCount()));

    // RemovePage() transfers ownership of the old page; the failed AddPage()
    // leaves ownership of its candidate with the caller.
    delete originalFirst;
    delete candidate;
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::DeleteAllItemsDestruction",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.release();
    wxListView* const list = book->GetListView();
    const wxWeakRef<wxWindow> lifetime(book);
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

    REQUIRE(destroyed);
}

TEST_CASE_METHOD(ListbookTestCase, "Listbook::DeleteAllItemsTopologyReentry",
                 "[listbook][ListbookTestCase][winui-v0-supported]")
{
    ListbookForTesting* const book = m_listbook.get();
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

    REQUIRE(eventSeen);
    REQUIRE(!insertResult);
    REQUIRE(!removeResult);
    REQUIRE(!nestedDeleteAllResult);
    // Controller-first removal keeps the old common topology visible during
    // the first notification and commits both models together afterwards.
    REQUIRE(modelCountDuringEvent == static_cast<size_t>(3));

    // The precise count during the native delete notification is
    // platform-defined, but it is either the old count or old count - 1.
    REQUIRE((controllerCountDuringEvent == 2 ||
                    controllerCountDuringEvent == 3));
    REQUIRE(outerResult);
    REQUIRE(book->GetPageCount() == static_cast<size_t>(0));
    REQUIRE(list->GetItemCount() == 0L);
    REQUIRE(book->GetSelection() == wxNOT_FOUND);

    // Failed insertion leaves ownership with the caller.
    delete candidate;
}

#endif //wxUSE_LISTBOOK
