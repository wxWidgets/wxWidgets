///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treectrltest.cpp
// Purpose:     wxTreeCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// RCS-ID:      $Id$
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

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( TreeCtrlTestCase );
        WXUISIM_TEST( ItemClick );
        CPPUNIT_TEST( DeleteItem );
        WXUISIM_TEST( LabelEdit );
        WXUISIM_TEST( KeyDown );
#ifndef __WXGTK__
        WXUISIM_TEST( CollapseExpandEvents );
        WXUISIM_TEST( SelectionChange );
#endif // !__WXGTK__
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
    CPPUNIT_TEST_SUITE_END();

    void ItemClick();
    void DeleteItem();
    void LabelEdit();
    void KeyDown();
#ifndef __WXGTK__
    void CollapseExpandEvents();
    void SelectionChange();
#endif // !__WXGTK__
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

    DECLARE_NO_COPY_CLASS(TreeCtrlTestCase)
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
}

void TreeCtrlTestCase::ItemClick()
{
#if wxUSE_UIACTIONSIMULATOR
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_ITEM_ACTIVATED);
    EventCounter count1(m_tree, wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK);

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

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_ACTIVATED));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK));
#endif // wxUSE_UIACTIONSIMULATOR
}

void TreeCtrlTestCase::DeleteItem()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_DELETE_ITEM);

    wxTreeItemId todelete = m_tree->AppendItem(m_root, "deleteme");
    m_tree->Delete(todelete);
    // We do not test DeleteAllItems() as under some versions of Windows events
    // are not generated.

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

#if wxUSE_UIACTIONSIMULATOR

void TreeCtrlTestCase::LabelEdit()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT);
    EventCounter count1(m_tree, wxEVT_COMMAND_TREE_END_LABEL_EDIT);

    wxUIActionSimulator sim;

    m_tree->SetFocusedItem(m_tree->GetRootItem());
    m_tree->EditLabel(m_tree->GetRootItem());

    sim.Text("newroottext");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());

    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

void TreeCtrlTestCase::KeyDown()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_KEY_DOWN);

    wxUIActionSimulator sim;

    m_tree->SetFocus();
    sim.Text("aAbB");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount());
}

#if !defined(__WXGTK__)

void TreeCtrlTestCase::CollapseExpandEvents()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    m_tree->CollapseAll();

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
    EventCounter count1(m_tree, wxEVT_COMMAND_TREE_ITEM_COLLAPSING);
    EventCounter count2(m_tree, wxEVT_COMMAND_TREE_ITEM_EXPANDED);
    EventCounter count3(m_tree, wxEVT_COMMAND_TREE_ITEM_EXPANDING);

    wxUIActionSimulator sim;

    wxRect pos;
    m_tree->GetBoundingRect(m_root, pos, true);

    // We move in slightly so we are not on the edge
    wxPoint point = m_tree->ClientToScreen(pos.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point);
    wxYield();

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_EXPANDING));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_EXPANDED));

    sim.MouseDblClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_COLLAPSING));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_COLLAPSED));
}

void TreeCtrlTestCase::SelectionChange()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    m_tree->ExpandAll();
    m_tree->UnselectAll();

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_SEL_CHANGED);
    EventCounter count1(m_tree, wxEVT_COMMAND_TREE_SEL_CHANGING);

    wxUIActionSimulator sim;

    wxRect poschild1, poschild2;
    m_tree->GetBoundingRect(m_child1, poschild1, true);
    m_tree->GetBoundingRect(m_child1, poschild2, true);

    // We move in slightly so we are not on the edge
    wxPoint point1 = m_tree->ClientToScreen(poschild1.GetPosition()) + wxPoint(4, 4);
    wxPoint point2 = m_tree->ClientToScreen(poschild2.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point1);
    wxYield();

    sim.MouseClick();
    wxYield();

    sim.MouseMove(point2);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_TREE_SEL_CHANGED));
    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_TREE_SEL_CHANGING));
}

#endif // !__WXGTK__

void TreeCtrlTestCase::Menu()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_tree, wxEVT_COMMAND_TREE_ITEM_MENU);
    wxUIActionSimulator sim;

    wxRect pos;
    m_tree->GetBoundingRect(m_child1, pos, true);

    // We move in slightly so we are not on the edge
    wxPoint point = m_tree->ClientToScreen(pos.GetPosition()) + wxPoint(4, 4);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_TREE_ITEM_MENU));
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
#if !defined(__WXGTK__) && !defined(__WXOSX__)
    m_tree->SetFocusedItem(m_child1);

    CPPUNIT_ASSERT_EQUAL(m_child1, m_tree->GetFocusedItem());

    m_tree->ClearFocusedItem();

    CPPUNIT_ASSERT(!m_tree->GetFocusedItem());
#endif
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
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXGTK__)
    wxUIActionSimulator sim;

    m_tree->CollapseAll();

    m_tree->SelectItem(m_root);

    m_tree->SetFocus();
    sim.Char(WXK_RIGHT);
    wxYield();

    CPPUNIT_ASSERT(m_tree->IsExpanded(m_root));

    sim.Char(WXK_LEFT);
    wxYield();

    CPPUNIT_ASSERT(!m_tree->IsExpanded(m_root));

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
