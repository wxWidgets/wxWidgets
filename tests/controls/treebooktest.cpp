///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treebooktest.cpp
// Purpose:     wxtreebook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TREEBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/treebook.h"
#include "wx/treectrl.h"
#include "wx/weakref.h"
#include "bookctrlbasetest.h"

#include <memory>

class TreebookTestCase : public BookCtrlBaseTestCase
{
public:
    TreebookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_treebook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_TREEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_TREEBOOK_PAGE_CHANGING; }

    std::unique_ptr<wxTreebook> m_treebook;

    wxDECLARE_NO_COPY_CLASS(TreebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(TreebookTestCase, "Treebook",
                       "[treebook][book][TreebookTestCase][winui-v0-supported]");

// wxTreebook supports images, unlike most of the other book controls.
wxBOOK_CTRL_BASE_TEST_CASE(TreebookTestCase, "Treebook", Image,
                           "[treebook][book][TreebookTestCase][winui-v0-supported]");

TreebookTestCase::TreebookTestCase()
{
    m_treebook = make_unique<wxTreebook>(wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}


TEST_CASE_METHOD(TreebookTestCase, "Treebook::SubPages",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    wxPanel* subpanel1 = new wxPanel(m_treebook.get());
    wxPanel* subpanel2 = new wxPanel(m_treebook.get());
    wxPanel* subpanel3 = new wxPanel(m_treebook.get());

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);

    CHECK(m_treebook->GetPageParent(3) == 2);

    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);

    CHECK(m_treebook->GetPageParent(2) == 1);

    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CHECK(m_treebook->GetPageParent(5) == 3);
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::ContainerPage",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    // Get rid of the pages added in the fixture constructor.
    m_treebook->DeleteAllPages();
    CHECK( m_treebook->GetPageCount() == 0 );

    // Adding a page without the associated window should be allowed.
    REQUIRE_NOTHROW( m_treebook->AddPage(nullptr, "Container page") );
    CHECK( m_treebook->GetPageParent(0) == -1 );

    m_treebook->AddSubPage(new wxPanel(m_treebook.get()), "Child page");
    CHECK( m_treebook->GetPageParent(1) == 0 );
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::ContainerDeletion",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    REQUIRE(m_treebook->DeleteAllPages());

    REQUIRE(m_treebook->AddPage(nullptr, "Container"));
    wxPanel* const child1 = new wxPanel(m_treebook.get());
    wxPanel* const child2 = new wxPanel(m_treebook.get());
    const wxWeakRef<wxWindow> child1Lifetime(child1);
    const wxWeakRef<wxWindow> child2Lifetime(child2);
    REQUIRE(m_treebook->AddSubPage(child1, "Child 1"));
    REQUIRE(m_treebook->AddSubPage(child2, "Child 2"));
    wxPanel* const survivor = new wxPanel(m_treebook.get());
    REQUIRE(m_treebook->AddPage(survivor, "Survivor"));

    // A null page is a valid container: success must be derived from the
    // committed subtree topology, not from the removed window pointer.
    REQUIRE(m_treebook->RemovePage(0));
    REQUIRE(m_treebook->GetPageCount() == static_cast<size_t>(1));
    REQUIRE(m_treebook->GetPage(0) == survivor);
    REQUIRE(!child1Lifetime);
    REQUIRE(!child2Lifetime);

    REQUIRE(m_treebook->DeleteAllPages());
    REQUIRE(m_treebook->AddPage(nullptr, "Container"));
    REQUIRE(m_treebook->AddSubPage(
        new wxPanel(m_treebook.get()), "Child"));
    REQUIRE(m_treebook->AddPage(
        new wxPanel(m_treebook.get()), "Survivor"));
    REQUIRE(m_treebook->DeletePage(0));
    REQUIRE(m_treebook->GetPageCount() == static_cast<size_t>(1));
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::Expand",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    wxPanel* subpanel1 = new wxPanel(m_treebook.get());
    wxPanel* subpanel2 = new wxPanel(m_treebook.get());
    wxPanel* subpanel3 = new wxPanel(m_treebook.get());

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CHECK(!m_treebook->IsNodeExpanded(1));
    CHECK(!m_treebook->IsNodeExpanded(3));

    m_treebook->CollapseNode(1);

    CHECK(!m_treebook->IsNodeExpanded(1));

    m_treebook->ExpandNode(3, false);

    CHECK(!m_treebook->IsNodeExpanded(3));

    m_treebook->ExpandNode(1);

    CHECK(m_treebook->IsNodeExpanded(1));
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::Delete",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    wxPanel* subpanel1 = new wxPanel(m_treebook.get());
    wxPanel* subpanel2 = new wxPanel(m_treebook.get());
    wxPanel* subpanel3 = new wxPanel(m_treebook.get());

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CHECK(m_treebook->GetPageCount() == 6);

    m_treebook->DeletePage(3);

    CHECK(m_treebook->GetPageCount() == 3);

    m_treebook->DeletePage(1);

    CHECK(m_treebook->GetPageCount() == 1);

    m_treebook->DeletePage(0);

    CHECK(m_treebook->GetPageCount() == 0);
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::ControllerVetoRestoresSelection",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    class VetoObserver final : public wxEvtHandler
    {
    public:
        void OnChanging(wxBookCtrlEvent& event)
        {
            ++changingCount;
            event.Veto();
        }

        unsigned changingCount{0};
    } observer;

    REQUIRE(m_treebook->SetSelection(0) == 0);

    wxTreeCtrl* const tree = m_treebook->GetTreeCtrl();
    wxTreeItemIdValue cookie;
    const wxTreeItemId first =
        tree->GetFirstChild(tree->GetRootItem(), cookie);
    const wxTreeItemId second = tree->GetNextSibling(first);
    REQUIRE(first.IsOk());
    REQUIRE(second.IsOk());

    m_treebook->Bind(
        wxEVT_TREEBOOK_PAGE_CHANGING,
        &VetoObserver::OnChanging,
        &observer);
    tree->SelectItem(second);
    m_treebook->Unbind(
        wxEVT_TREEBOOK_PAGE_CHANGING,
        &VetoObserver::OnChanging,
        &observer);

    REQUIRE(observer.changingCount == 1);
    REQUIRE(m_treebook->GetSelection() == 0);
    REQUIRE(tree->GetSelection() == first);

    // The synchronous rollback must leave the controller ready for the next
    // selection, once the veto handler has been removed.
    tree->SelectItem(second);
    REQUIRE(m_treebook->GetSelection() == 1);
    REQUIRE(tree->GetSelection() == second);
}

TEST_CASE_METHOD(TreebookTestCase, "Treebook::DeleteAllPublishesEmptyTopology",
                 "[treebook][TreebookTestCase][winui-v0-supported]")
{
    class DeleteObserver final : public wxEvtHandler
    {
    public:
        explicit DeleteObserver(wxTreebook* const book)
            : m_book(book)
        {
        }

        void OnDelete(wxTreeEvent&)
        {
            seen = true;
            const size_t count = m_book->GetPageCount();
            countsOnlyDecrease = countsOnlyDecrease &&
                                 count < previousCount;
            previousCount = count;
        }

        wxTreebook* const m_book;
        bool seen{false};
        size_t previousCount{static_cast<size_t>(-1)};
        bool countsOnlyDecrease{true};
    } observer(m_treebook.get());

    wxTreeCtrl* const tree = m_treebook->GetTreeCtrl();
    tree->Bind(wxEVT_TREE_DELETE_ITEM,
               &DeleteObserver::OnDelete,
               &observer);
    REQUIRE(m_treebook->DeleteAllPages());
    tree->Unbind(wxEVT_TREE_DELETE_ITEM,
                 &DeleteObserver::OnDelete,
                 &observer);

    REQUIRE(observer.seen);
    REQUIRE(observer.countsOnlyDecrease);
    REQUIRE(m_treebook->GetPageCount() == static_cast<size_t>(0));
}

#endif // wxUSE_TREEBOOK
