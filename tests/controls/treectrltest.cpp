///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treectrltest.cpp
// Purpose:     wxTreeCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_TREECTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/uiaction.h"
#include "testableframe.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TreeCtrlTestCase : public CppUnit::TestCase
{
public:
    TreeCtrlTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( TreeCtrlTestCase );
        WXUISIM_TEST( ItemClick );
        CPPUNIT_TEST( DeleteItem );
        CPPUNIT_TEST( DeleteChildren );
        CPPUNIT_TEST( DeleteAllItems );
        WXUISIM_TEST( LabelEdit );
        WXUISIM_TEST( KeyDown );
        WXUISIM_TEST( CollapseExpandEvents );
        WXUISIM_TEST( SelectionChange );
        WXUISIM_TEST( Menu );
        CPPUNIT_TEST( ItemData );
        CPPUNIT_TEST( Iteration );
        CPPUNIT_TEST( Parent );
        CPPUNIT_TEST( CollapseExpand );
        CPPUNIT_TEST( AssignImageList );
        CPPUNIT_TEST( Focus );
        CPPUNIT_TEST( Bold );
        CPPUNIT_TEST( Visible );
        CPPUNIT_TEST( Sort );
        WXUISIM_TEST( KeyNavigation );
        CPPUNIT_TEST( HasChildren );
        CPPUNIT_TEST( SelectItemSingle );
        CPPUNIT_TEST( PseudoTest_MultiSelect );
        CPPUNIT_TEST( SelectItemMulti );
        CPPUNIT_TEST( PseudoTest_SetHiddenRoot );
        CPPUNIT_TEST( HasChildren );
        CPPUNIT_TEST( GetCount );
    CPPUNIT_TEST_SUITE_END();

    void ItemClick();
    void DeleteItem();
    void DeleteChildren();
    void DeleteAllItems();
    void LabelEdit();
    void KeyDown();
    void CollapseExpandEvents();
    void SelectionChange();
    void Menu();
    void ItemData();
    void Iteration();
    void Parent();
    void CollapseExpand();
    void AssignImageList();
    void Focus();
    void Bold();
    void Visible();
    void Sort();
    void KeyNavigation();
    void HasChildren();
    void GetCount();
    void SelectItemSingle();
    void SelectItemMulti();
    void PseudoTest_MultiSelect() { ms_multiSelect = true; }
    void PseudoTest_SetHiddenRoot() { ms_hiddenRoot = true; }

    static bool ms_multiSelect;
    static bool ms_hiddenRoot;

    // the tree control itself
    wxTreeCtrl *m_tree;

    // and some of its items
    wxTreeItemId m_root,
                 m_child1,
                 m_child2,
                 m_grandchild;

    wxDECLARE_NO_COPY_CLASS(TreeCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreeCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TreeCtrlTestCase, "TreeCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

bool TreeCtrlTestCase::ms_multiSelect = false;
bool TreeCtrlTestCase::ms_hiddenRoot = false;

void TreeCtrlTestCase::setUp()
{
    m_tree = new wxTreeCtrl(wxTheApp->GetTopWindow(),
                            wxID_ANY,
                            wxDefaultPosition,
                            wxSize(400, 200),
                            wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS);

    if ( ms_multiSelect )
        m_tree->ToggleWindowStyle(wxTR_MULTIPLE);

    if ( ms_hiddenRoot )
        m_tree->ToggleWindowStyle(wxTR_HIDE_ROOT); // actually set it

    m_root = m_tree->AddRoot("root");
    m_child1 = m_tree->AppendItem(m_root, "child1");
    m_child2 = m_tree->AppendItem(m_root, "child2");
    m_grandchild = m_tree->AppendItem(m_child1, "grandchild");

    m_tree->SetSize(400, 200);
    m_tree->ExpandAll();
    m_tree->Refresh();
    m_tree->Update();
}

void TreeCtrlTestCase::tearDown()
{
    delete m_tree;
    m_tree = NULL;

    m_root =
    m_child1 =
    m_child2 =
    m_grandchild = wxTreeItemId();
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

void TreeCtrlTestCase::HasChildren()
{
    CPPUNIT_ASSERT( m_tree->HasChildren(m_root) );
    CPPUNIT_ASSERT( m_tree->HasChildren(m_child1) );
    CPPUNIT_ASSERT( !m_tree->HasChildren(m_child2) );
    CPPUNIT_ASSERT( !m_tree->HasChildren(m_grandchild) );
}

void TreeCtrlTestCase::GetCount()
{
    CPPUNIT_ASSERT_EQUAL(3, m_tree->GetCount());
}

void TreeCtrlTestCase::SelectItemSingle()
{
    // this test should be only ran in single-selection control
    CPPUNIT_ASSERT( !m_tree->HasFlag(wxTR_MULTIPLE) );

    // initially nothing is selected
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child1) );

    // selecting an item should make it selected
    m_tree->SelectItem(m_child1);
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child1) );

    // selecting it again shouldn't change anything
    m_tree->SelectItem(m_child1);
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child1) );

    // selecting another item should switch the selection to it
    m_tree->SelectItem(m_child2);
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child1) );
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child2) );

    // selecting it again still shouldn't change anything
    m_tree->SelectItem(m_child2);
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child1) );
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child2) );

    // deselecting an item should remove the selection entirely
    m_tree->UnselectItem(m_child2);
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child1) );
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child2) );
}

void TreeCtrlTestCase::SelectItemMulti()
{
    // this test should be only ran in multi-selection control
    CPPUNIT_ASSERT( m_tree->HasFlag(wxTR_MULTIPLE) );

    // initially nothing is selected
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child1) );

    // selecting an item should make it selected
    m_tree->SelectItem(m_child1);
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child1) );

    // selecting it again shouldn't change anything
    m_tree->SelectItem(m_child1);
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child1) );

    // selecting another item shouldn't deselect the previously selected one
    m_tree->SelectItem(m_child2);
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child1) );
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child2) );

    // selecting it again still shouldn't change anything
    m_tree->SelectItem(m_child2);
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child1) );
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child2) );

    // deselecting one of the items should leave the others selected
    m_tree->UnselectItem(m_child1);
    CPPUNIT_ASSERT( !m_tree->IsSelected(m_child1) );
    CPPUNIT_ASSERT( m_tree->IsSelected(m_child2) );

    // collapsing a branch with selected items should still leave them selected
    m_tree->Expand(m_child1);
    m_tree->SelectItem(m_grandchild);
    CHECK( m_tree->IsSelected(m_grandchild) );
    m_tree->Collapse(m_child1);
    CHECK( m_tree->IsSelected(m_grandchild) );
    m_tree->Expand(m_child1);
    CHECK( m_tree->IsSelected(m_grandchild) );
}

void TreeCtrlTestCase::ItemClick()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter activated(m_tree, wxEVT_TREE_ITEM_ACTIVATED);
    EventCounter rclick(m_tree, wxEVT_TREE_ITEM_RIGHT_CLICK);

    wxUIActionSimulator sim;

    wxRect pos;
    m_tree->GetBoundingRect(m_child1, pos, true);

    // We move in slightly so we are not on the edge
    wxPoint point = m_tree->ClientToScreen(pos.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point);
    wxYield();

    sim.MouseDblClick();
    wxYield();

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, activated.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());
#endif // wxUSE_UIACTIONSIMULATOR
}

void TreeCtrlTestCase::DeleteItem()
{
    EventCounter deleteitem(m_tree, wxEVT_TREE_DELETE_ITEM);

    wxTreeItemId todelete = m_tree->AppendItem(m_root, "deleteme");
    m_tree->AppendItem(todelete, "deleteme2");
    m_tree->Delete(todelete);

    CPPUNIT_ASSERT_EQUAL(2, deleteitem.GetCount());
}

void TreeCtrlTestCase::DeleteChildren()
{
    EventCounter deletechildren(m_tree, wxEVT_TREE_DELETE_ITEM);

    m_tree->AppendItem(m_child1, "another grandchild");
    m_tree->DeleteChildren(m_child1);

    CHECK( deletechildren.GetCount() == 2 );
}

void TreeCtrlTestCase::DeleteAllItems()
{
    EventCounter deleteall(m_tree, wxEVT_TREE_DELETE_ITEM);

    m_tree->DeleteAllItems();

    CHECK( deleteall.GetCount() == 4 );
}

#if wxUSE_UIACTIONSIMULATOR

void TreeCtrlTestCase::LabelEdit()
{
    EventCounter beginedit(m_tree, wxEVT_TREE_BEGIN_LABEL_EDIT);
    EventCounter endedit(m_tree, wxEVT_TREE_END_LABEL_EDIT);

    wxUIActionSimulator sim;

    m_tree->SetFocusedItem(m_tree->GetRootItem());
    m_tree->EditLabel(m_tree->GetRootItem());

    sim.Text("newroottext");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, beginedit.GetCount());

    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, endedit.GetCount());
}

void TreeCtrlTestCase::KeyDown()
{
    EventCounter keydown(m_tree, wxEVT_TREE_KEY_DOWN);

    wxUIActionSimulator sim;

    m_tree->SetFocus();
    sim.Text("aAbB");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(6, keydown.GetCount());
}

void TreeCtrlTestCase::CollapseExpandEvents()
{
#ifdef __WXGTK20__
    // Works locally, but not when run on Travis CI.
    if ( IsAutomaticTest() )
        return;
#endif

    m_tree->CollapseAll();

    EventCounter collapsed(m_tree, wxEVT_TREE_ITEM_COLLAPSED);
    EventCounter collapsing(m_tree, wxEVT_TREE_ITEM_COLLAPSING);
    EventCounter expanded(m_tree, wxEVT_TREE_ITEM_EXPANDED);
    EventCounter expanding(m_tree, wxEVT_TREE_ITEM_EXPANDING);

    wxUIActionSimulator sim;

    wxRect pos;
    m_tree->GetBoundingRect(m_root, pos, true);

    // We move in slightly so we are not on the edge
    wxPoint point = m_tree->ClientToScreen(pos.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point);
    wxYield();

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, expanding.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, expanded.GetCount());

#ifdef __WXGTK__
    // Don't even know the reason why, but GTK has to sleep
    // no less than 1200 for the test case to succeed.
    wxMilliSleep(1200);
#endif

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, collapsing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, collapsed.GetCount());
}

void TreeCtrlTestCase::SelectionChange()
{
    m_tree->ExpandAll();

    // This is currently needed to work around a problem under wxMSW: clicking
    // on an item in an unfocused control generates two selection change events
    // because of the SetFocus() call in TVN_SELCHANGED handler in wxMSW code.
    // This is, of course, wrong on its own, but fixing it without breaking
    // anything else is non-obvious, so for now at least work around this
    // problem in the test.
    m_tree->SetFocus();

    EventCounter changed(m_tree, wxEVT_TREE_SEL_CHANGED);
    EventCounter changing(m_tree, wxEVT_TREE_SEL_CHANGING);

    wxUIActionSimulator sim;

    wxRect poschild1, poschild2;
    m_tree->GetBoundingRect(m_child1, poschild1, true);
    m_tree->GetBoundingRect(m_child2, poschild2, true);

    // We move in slightly so we are not on the edge
    wxPoint point1 = m_tree->ClientToScreen(poschild1.GetPosition()) + wxPoint(4, 4);
    wxPoint point2 = m_tree->ClientToScreen(poschild2.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point1);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());

    sim.MouseMove(point2);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(2, changed.GetCount());
    CPPUNIT_ASSERT_EQUAL(2, changing.GetCount());
}

void TreeCtrlTestCase::Menu()
{
    EventCounter menu(m_tree, wxEVT_TREE_ITEM_MENU);
    wxUIActionSimulator sim;

    wxRect pos;
    m_tree->GetBoundingRect(m_child1, pos, true);

    // We move in slightly so we are not on the edge
    wxPoint point = m_tree->ClientToScreen(pos.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, menu.GetCount());
}

#endif // wxUSE_UIACTIONSIMULATOR

void TreeCtrlTestCase::ItemData()
{
    wxTreeItemData* child1data = new wxTreeItemData();
    wxTreeItemData* appenddata = new wxTreeItemData();
    wxTreeItemData* insertdata = new wxTreeItemData();

    m_tree->SetItemData(m_child1, child1data);

    CPPUNIT_ASSERT_EQUAL(child1data, m_tree->GetItemData(m_child1));
    CPPUNIT_ASSERT_EQUAL(m_child1, child1data->GetId());

    wxTreeItemId append = m_tree->AppendItem(m_root, "new", -1, -1, appenddata);

    CPPUNIT_ASSERT_EQUAL(appenddata, m_tree->GetItemData(append));
    CPPUNIT_ASSERT_EQUAL(append, appenddata->GetId());

    wxTreeItemId insert = m_tree->InsertItem(m_root, m_child1, "new", -1, -1,
                                             insertdata);

    CPPUNIT_ASSERT_EQUAL(insertdata, m_tree->GetItemData(insert));
    CPPUNIT_ASSERT_EQUAL(insert, insertdata->GetId());
}

void TreeCtrlTestCase::Iteration()
{
    // Get first / next / last child
    wxTreeItemIdValue cookie;
    CPPUNIT_ASSERT_EQUAL(m_tree->GetFirstChild(m_root, cookie), m_child1);
    CPPUNIT_ASSERT_EQUAL(m_tree->GetNextChild(m_root, cookie),
                         m_tree->GetLastChild(m_root));
    CPPUNIT_ASSERT_EQUAL(m_child2, m_tree->GetLastChild(m_root));

    // Get next / previous sibling
    CPPUNIT_ASSERT_EQUAL(m_child2, m_tree->GetNextSibling(m_child1));
    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetPrevSibling(m_child2));
}

void TreeCtrlTestCase::Parent()
{
    CPPUNIT_ASSERT_EQUAL(m_root, m_tree->GetRootItem());
    CPPUNIT_ASSERT_EQUAL(m_root, m_tree->GetItemParent(m_child1));
    CPPUNIT_ASSERT_EQUAL(m_root, m_tree->GetItemParent(m_child2));
    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetItemParent(m_grandchild));
}

void TreeCtrlTestCase::CollapseExpand()
{
    m_tree->ExpandAll();

    CPPUNIT_ASSERT(m_tree->IsExpanded(m_root));
    CPPUNIT_ASSERT(m_tree->IsExpanded(m_child1));

    m_tree->CollapseAll();

    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_root));
    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_child1));

    m_tree->ExpandAllChildren(m_root);

    CPPUNIT_ASSERT(m_tree->IsExpanded(m_root));
    CPPUNIT_ASSERT(m_tree->IsExpanded(m_child1));

    m_tree->CollapseAllChildren(m_child1);

    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_child1));

    m_tree->Expand(m_child1);

    CPPUNIT_ASSERT(m_tree->IsExpanded(m_child1));

    m_tree->Collapse(m_root);

    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_root));
    CPPUNIT_ASSERT(m_tree->IsExpanded(m_child1));

    m_tree->CollapseAndReset(m_root);

    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_root));
}

void TreeCtrlTestCase::AssignImageList()
{
    wxSize size(16, 16);

    wxImageList *imagelist = new wxImageList(size.x, size.y);
    imagelist->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size));

    wxImageList *statelist = new wxImageList(size.x, size.y);
    statelist->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, size));

    m_tree->AssignImageList(imagelist);
    m_tree->AssignStateImageList(statelist);

    CPPUNIT_ASSERT_EQUAL(imagelist, m_tree->GetImageList());
    CPPUNIT_ASSERT_EQUAL(statelist, m_tree->GetStateImageList());
}

void TreeCtrlTestCase::Focus()
{
    m_tree->SetFocusedItem(m_child1);

    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetFocusedItem());

    m_tree->ClearFocusedItem();

    CPPUNIT_ASSERT(!m_tree->GetFocusedItem());
}

void TreeCtrlTestCase::Bold()
{
    CPPUNIT_ASSERT(!m_tree->IsBold(m_child1));

    m_tree->SetItemBold(m_child1);

    CPPUNIT_ASSERT(m_tree->IsBold(m_child1));

    m_tree->SetItemBold(m_child1, false);

    CPPUNIT_ASSERT(!m_tree->IsBold(m_child1));
}

void TreeCtrlTestCase::Visible()
{
    m_tree->CollapseAll();

    CPPUNIT_ASSERT(m_tree->IsVisible(m_root));
    CPPUNIT_ASSERT(!m_tree->IsVisible(m_child1));

    m_tree->EnsureVisible(m_grandchild);

    CPPUNIT_ASSERT(m_tree->IsVisible(m_grandchild));

    m_tree->ExpandAll();

    CPPUNIT_ASSERT_EQUAL(m_root, m_tree->GetFirstVisibleItem());
    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetNextVisible(m_root));
    CPPUNIT_ASSERT_EQUAL(m_grandchild, m_tree->GetNextVisible(m_child1));
    CPPUNIT_ASSERT_EQUAL(m_child2, m_tree->GetNextVisible(m_grandchild));

    CPPUNIT_ASSERT(!m_tree->GetNextVisible(m_child2));
    CPPUNIT_ASSERT(!m_tree->GetPrevVisible(m_root));
}

void TreeCtrlTestCase::Sort()
{
    wxTreeItemId zitem = m_tree->AppendItem(m_root, "zzzz");
    wxTreeItemId aitem = m_tree->AppendItem(m_root, "aaaa");

    m_tree->SortChildren(m_root);

    wxTreeItemIdValue cookie;

    CPPUNIT_ASSERT_EQUAL(aitem, m_tree->GetFirstChild(m_root, cookie));
    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetNextChild(m_root, cookie));
    CPPUNIT_ASSERT_EQUAL(m_child2, m_tree->GetNextChild(m_root, cookie));
    CPPUNIT_ASSERT_EQUAL(zitem, m_tree->GetNextChild(m_root, cookie));
}

void TreeCtrlTestCase::KeyNavigation()
{
#if wxUSE_UIACTIONSIMULATOR
    wxUIActionSimulator sim;

    m_tree->CollapseAll();

    m_tree->SelectItem(m_root);
    wxYield();

    m_tree->SetFocus();
    sim.Char(WXK_RIGHT);
    wxYield();

    CPPUNIT_ASSERT(m_tree->IsExpanded(m_root));

#ifdef wxHAS_GENERIC_TREECTRL
    sim.Char('-');
#else
    sim.Char(WXK_LEFT);
#endif

    wxYield();

    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_root));

    wxYield();

    sim.Char(WXK_RIGHT);
    sim.Char(WXK_DOWN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetSelection());

    sim.Char(WXK_DOWN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(m_child2, m_tree->GetSelection());
#endif
}

#endif //wxUSE_TREECTRL
