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

class TreebookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    TreebookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_treebook; }

    virtual wxEventType GetChangedEvent() const override
        { return wxEVT_TREEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
        { return wxEVT_TREEBOOK_PAGE_CHANGING; }

    CPPUNIT_TEST_SUITE( TreebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( Image );
        CPPUNIT_TEST( SubPages );
        CPPUNIT_TEST( ContainerPage );
        CPPUNIT_TEST( ContainerDeletion );
        CPPUNIT_TEST( Expand );
        CPPUNIT_TEST( Delete );
        CPPUNIT_TEST( ControllerVetoRestoresSelection );
        CPPUNIT_TEST( DeleteAllPublishesEmptyTopology );
    CPPUNIT_TEST_SUITE_END();

    void SubPages();
    void ContainerPage();
    void ContainerDeletion();
    void Expand();
    void Delete();
    void ControllerVetoRestoresSelection();
    void DeleteAllPublishesEmptyTopology();

    wxTreebook *m_treebook;

    wxDECLARE_NO_COPY_CLASS(TreebookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreebookTestCase );

// also include in its own registry so that these tests can be run alone
wxREGISTER_UNIT_TEST_WITH_TAGS(
    TreebookTestCase,
    "[TreebookTestCase][winui-v0-supported]");

void TreebookTestCase::setUp()
{
    m_treebook = new wxTreebook(wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}

void TreebookTestCase::tearDown()
{
    wxDELETE(m_treebook);
}

void TreebookTestCase::SubPages()
{
    wxPanel* subpanel1 = new wxPanel(m_treebook);
    wxPanel* subpanel2 = new wxPanel(m_treebook);
    wxPanel* subpanel3 = new wxPanel(m_treebook);

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);

    CPPUNIT_ASSERT_EQUAL(2, m_treebook->GetPageParent(3));

    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);

    CPPUNIT_ASSERT_EQUAL(1, m_treebook->GetPageParent(2));

    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CPPUNIT_ASSERT_EQUAL(3, m_treebook->GetPageParent(5));
}

void TreebookTestCase::ContainerPage()
{
    // Get rid of the pages added in setUp().
    m_treebook->DeleteAllPages();
    CHECK( m_treebook->GetPageCount() == 0 );

    // Adding a page without the associated window should be allowed.
    REQUIRE_NOTHROW( m_treebook->AddPage(nullptr, "Container page") );
    CHECK( m_treebook->GetPageParent(0) == -1 );

    m_treebook->AddSubPage(new wxPanel(m_treebook), "Child page");
    CHECK( m_treebook->GetPageParent(1) == 0 );
}

void TreebookTestCase::ContainerDeletion()
{
    CPPUNIT_ASSERT(m_treebook->DeleteAllPages());

    CPPUNIT_ASSERT(m_treebook->AddPage(nullptr, "Container"));
    wxPanel* const child1 = new wxPanel(m_treebook);
    wxPanel* const child2 = new wxPanel(m_treebook);
    const wxWeakRef<wxWindow> child1Lifetime(child1);
    const wxWeakRef<wxWindow> child2Lifetime(child2);
    CPPUNIT_ASSERT(m_treebook->AddSubPage(child1, "Child 1"));
    CPPUNIT_ASSERT(m_treebook->AddSubPage(child2, "Child 2"));
    wxPanel* const survivor = new wxPanel(m_treebook);
    CPPUNIT_ASSERT(m_treebook->AddPage(survivor, "Survivor"));

    // A null page is a valid container: success must be derived from the
    // committed subtree topology, not from the removed window pointer.
    CPPUNIT_ASSERT(m_treebook->RemovePage(0));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1),
                         m_treebook->GetPageCount());
    CPPUNIT_ASSERT(m_treebook->GetPage(0) == survivor);
    CPPUNIT_ASSERT(!child1Lifetime);
    CPPUNIT_ASSERT(!child2Lifetime);

    CPPUNIT_ASSERT(m_treebook->DeleteAllPages());
    CPPUNIT_ASSERT(m_treebook->AddPage(nullptr, "Container"));
    CPPUNIT_ASSERT(m_treebook->AddSubPage(
        new wxPanel(m_treebook), "Child"));
    CPPUNIT_ASSERT(m_treebook->AddPage(
        new wxPanel(m_treebook), "Survivor"));
    CPPUNIT_ASSERT(m_treebook->DeletePage(0));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1),
                         m_treebook->GetPageCount());
}

void TreebookTestCase::Expand()
{
    wxPanel* subpanel1 = new wxPanel(m_treebook);
    wxPanel* subpanel2 = new wxPanel(m_treebook);
    wxPanel* subpanel3 = new wxPanel(m_treebook);

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(1));
    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(3));

    m_treebook->CollapseNode(1);

    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(1));

    m_treebook->ExpandNode(3, false);

    CPPUNIT_ASSERT(!m_treebook->IsNodeExpanded(3));

    m_treebook->ExpandNode(1);

    CPPUNIT_ASSERT(m_treebook->IsNodeExpanded(1));
}

void TreebookTestCase::Delete()
{
    wxPanel* subpanel1 = new wxPanel(m_treebook);
    wxPanel* subpanel2 = new wxPanel(m_treebook);
    wxPanel* subpanel3 = new wxPanel(m_treebook);

    m_treebook->AddSubPage(subpanel1, "Subpanel 1", false, 0);
    m_treebook->InsertSubPage(1, subpanel2, "Subpanel 2", false, 1);
    m_treebook->AddSubPage(subpanel3, "Subpanel 3", false, 2);

    CPPUNIT_ASSERT_EQUAL(6, m_treebook->GetPageCount());

    m_treebook->DeletePage(3);

    CPPUNIT_ASSERT_EQUAL(3, m_treebook->GetPageCount());

    m_treebook->DeletePage(1);

    CPPUNIT_ASSERT_EQUAL(1, m_treebook->GetPageCount());

    m_treebook->DeletePage(0);

    CPPUNIT_ASSERT_EQUAL(0, m_treebook->GetPageCount());
}

void TreebookTestCase::ControllerVetoRestoresSelection()
{
    class VetoObserver final : public wxEvtHandler
    {
    public:
        void OnChanging(wxBookCtrlEvent& event)
        {
            seen = true;
            event.Veto();
        }

        bool seen{false};
    } observer;

    CPPUNIT_ASSERT_EQUAL(0, m_treebook->SetSelection(0));

    wxTreeCtrl* const tree = m_treebook->GetTreeCtrl();
    wxTreeItemIdValue cookie;
    const wxTreeItemId first =
        tree->GetFirstChild(tree->GetRootItem(), cookie);
    const wxTreeItemId second = tree->GetNextSibling(first);
    CPPUNIT_ASSERT(first.IsOk());
    CPPUNIT_ASSERT(second.IsOk());

    m_treebook->Bind(
        wxEVT_TREEBOOK_PAGE_CHANGING,
        &VetoObserver::OnChanging,
        &observer);
    tree->SelectItem(second);
    m_treebook->Unbind(
        wxEVT_TREEBOOK_PAGE_CHANGING,
        &VetoObserver::OnChanging,
        &observer);

    CPPUNIT_ASSERT(observer.seen);
    CPPUNIT_ASSERT_EQUAL(0, m_treebook->GetSelection());
    CPPUNIT_ASSERT(tree->GetSelection() == first);
}

void TreebookTestCase::DeleteAllPublishesEmptyTopology()
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
    } observer(m_treebook);

    wxTreeCtrl* const tree = m_treebook->GetTreeCtrl();
    tree->Bind(wxEVT_TREE_DELETE_ITEM,
               &DeleteObserver::OnDelete,
               &observer);
    CPPUNIT_ASSERT(m_treebook->DeleteAllPages());
    tree->Unbind(wxEVT_TREE_DELETE_ITEM,
                 &DeleteObserver::OnDelete,
                 &observer);

    CPPUNIT_ASSERT(observer.seen);
    CPPUNIT_ASSERT(observer.countsOnlyDecrease);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0),
                         m_treebook->GetPageCount());
}

#endif // wxUSE_TREEBOOK
