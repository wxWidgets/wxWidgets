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


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/uiaction.h"
#include "testableframe.h"
#include "waitfor.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TreeCtrlTestCase
{
public:
    explicit TreeCtrlTestCase(int exStyle = 0)
    {
        m_tree = new wxTreeCtrl(wxTheApp->GetTopWindow(),
                                wxID_ANY,
                                wxDefaultPosition,
                                wxSize(400, 200),
                                wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | exStyle);

        m_root = m_tree->AddRoot("root");
        m_child1 = m_tree->AppendItem(m_root, "child1");
        m_child2 = m_tree->AppendItem(m_root, "child2");
        m_grandchild = m_tree->AppendItem(m_child1, "grandchild");

        m_tree->SetSize(400, 200);
        m_tree->ExpandAll();
        m_tree->Refresh();
        m_tree->Update();
    }

    ~TreeCtrlTestCase()
    {
        delete m_tree;
    }

protected:

    // the tree control itself
    wxTreeCtrl *m_tree = nullptr;

    // and some of its items
    wxTreeItemId m_root,
                 m_child1,
                 m_child2,
                 m_grandchild;

    wxDECLARE_NO_COPY_CLASS(TreeCtrlTestCase);
};

// Notice that toggling the wxTR_HIDE_ROOT window style with ToggleWindowStyle
// has no effect under wxMSW if wxTreeCtrl::AddRoot() has already been called.
// So we need this fixture (used by HasChildren and GetCount below) to create
// the wxTreeCtrl with this style before AddRoot() is called.
class TreeCtrlHideRootTestCase : public TreeCtrlTestCase
{
public:
    TreeCtrlHideRootTestCase() : TreeCtrlTestCase(wxTR_HIDE_ROOT)
    {
    }
};

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(TreeCtrlHideRootTestCase, "wxTreeCtrl::HasChildren", "[treectrl]")
{
    CHECK( m_tree->HasChildren(m_root) );
    CHECK( m_tree->HasChildren(m_child1) );
    CHECK_FALSE( m_tree->HasChildren(m_child2) );
    CHECK_FALSE( m_tree->HasChildren(m_grandchild) );
}

TEST_CASE_METHOD(TreeCtrlHideRootTestCase, "wxTreeCtrl::GetCount", "[treectrl]")
{
    CHECK(m_tree->GetCount() == 3);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::SelectItemSingle", "[treectrl]")
{
    // this test should be only ran in single-selection control
    CHECK_FALSE( m_tree->HasFlag(wxTR_MULTIPLE) );

    // initially nothing is selected
    CHECK_FALSE( m_tree->IsSelected(m_child1) );

    // selecting an item should make it selected
    m_tree->SelectItem(m_child1);
    CHECK( m_tree->IsSelected(m_child1) );

    // selecting it again shouldn't change anything
    m_tree->SelectItem(m_child1);
    CHECK( m_tree->IsSelected(m_child1) );

    // selecting another item should switch the selection to it
    m_tree->SelectItem(m_child2);
    CHECK_FALSE( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );

    // selecting it again still shouldn't change anything
    m_tree->SelectItem(m_child2);
    CHECK_FALSE( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );

    // deselecting an item should remove the selection entirely
    m_tree->UnselectItem(m_child2);
    CHECK_FALSE( m_tree->IsSelected(m_child1) );
    CHECK_FALSE( m_tree->IsSelected(m_child2) );
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::SelectItemMulti", "[treectrl]")
{
    // this test should be only ran in multi-selection control
    m_tree->ToggleWindowStyle(wxTR_MULTIPLE);

    // initially nothing is selected
    CHECK_FALSE( m_tree->IsSelected(m_child1) );

    // selecting an item should make it selected
    m_tree->SelectItem(m_child1);
    CHECK( m_tree->IsSelected(m_child1) );

    // selecting it again shouldn't change anything
    m_tree->SelectItem(m_child1);
    CHECK( m_tree->IsSelected(m_child1) );

    // selecting another item shouldn't deselect the previously selected one
    m_tree->SelectItem(m_child2);
    CHECK( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );

    // selecting it again still shouldn't change anything
    m_tree->SelectItem(m_child2);
    CHECK( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );

    // deselecting one of the items should leave the others selected
    m_tree->UnselectItem(m_child1);
    CHECK_FALSE( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );

    // collapsing a branch with selected items should still leave them selected
    m_tree->Expand(m_child1);
    m_tree->SelectItem(m_grandchild);
    CHECK( m_tree->IsSelected(m_grandchild) );
    m_tree->Collapse(m_child1);
    CHECK( m_tree->IsSelected(m_grandchild) );
    m_tree->Expand(m_child1);
    CHECK( m_tree->IsSelected(m_grandchild) );
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::DeleteItem", "[treectrl]")
{
    EventCounter deleteitem(m_tree, wxEVT_TREE_DELETE_ITEM);

    wxTreeItemId todelete = m_tree->AppendItem(m_root, "deleteme");
    m_tree->AppendItem(todelete, "deleteme2");
    m_tree->Delete(todelete);

    CHECK(deleteitem.GetCount() == 2);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::DeleteChildren", "[treectrl]")
{
    EventCounter deletechildren(m_tree, wxEVT_TREE_DELETE_ITEM);

    m_tree->AppendItem(m_child1, "another grandchild");
    m_tree->DeleteChildren(m_child1);

    CHECK( deletechildren.GetCount() == 2 );
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::DeleteAllItems", "[treectrl]")
{
    EventCounter deleteall(m_tree, wxEVT_TREE_DELETE_ITEM);

    m_tree->DeleteAllItems();

    CHECK( deleteall.GetCount() == 4 );
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::ItemClick", "[treectrl]")
{
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

    CHECK(activated.GetCount() == 1);
    CHECK(rclick.GetCount() == 1);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::LabelEdit", "[treectrl]")
{
    EventCounter beginedit(m_tree, wxEVT_TREE_BEGIN_LABEL_EDIT);
    EventCounter endedit(m_tree, wxEVT_TREE_END_LABEL_EDIT);

    wxUIActionSimulator sim;

#ifdef __WXQT__
    m_tree->SetFocus();
    wxYield();
#endif

    m_tree->SetFocusedItem(m_tree->GetRootItem());
    m_tree->EditLabel(m_tree->GetRootItem());

    sim.Text("newroottext");
    wxYield();

    CHECK(beginedit.GetCount() == 1);

    sim.Char(WXK_RETURN);
    wxYield();

    CHECK(endedit.GetCount() == 1);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::KeyDown", "[treectrl]")
{
    EventCounter keydown(m_tree, wxEVT_TREE_KEY_DOWN);

    wxUIActionSimulator sim;

    m_tree->SetFocus();
    wxYield();
    sim.Text("aAbB");
    wxYield();

    CHECK(keydown.GetCount() == 6);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::CollapseExpandEvents", "[treectrl]")
{
#ifdef __WXGTK__
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

    CHECK(expanding.GetCount() == 1);
    CHECK(expanded.GetCount() == 1);

#ifdef __WXGTK__
    // Don't even know the reason why, but GTK has to sleep
    // no less than 1200 for the test case to succeed.
    wxMilliSleep(1200);
#endif

    sim.MouseDblClick();
    wxYield();

    CHECK(collapsing.GetCount() == 1);
    CHECK(collapsed.GetCount() == 1);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::SelectionChange", "[treectrl]")
{
    m_tree->ExpandAll();

    // This is currently needed to work around a problem under wxMSW: clicking
    // on an item in an unfocused control generates two selection change events
    // because of the SetFocus() call in TVN_SELCHANGED handler in wxMSW code.
    // This is, of course, wrong on its own, but fixing it without breaking
    // anything else is non-obvious, so for now at least work around this
    // problem in the test.
    m_tree->SetFocus();

    bool vetoChange = false;

    SECTION("Without veto selection change"){ }
    SECTION("With veto selection change")
    {
        vetoChange = true;
    }

    int changed   = 0;
    int changing  = 0;

    auto handler = [&](wxTreeEvent& event)
    {
        const auto eventType = event.GetEventType();

        if ( eventType == wxEVT_TREE_SEL_CHANGING )
        {
            ++changing;

            if ( vetoChange && changed == 1 )
            {
                event.Veto();
            }
        }
        else if ( eventType == wxEVT_TREE_SEL_CHANGED )
        {
            ++changed;
        }
    };

    m_tree->Bind(wxEVT_TREE_SEL_CHANGED, handler);
    m_tree->Bind(wxEVT_TREE_SEL_CHANGING, handler);

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

    CHECK(changed == 1);
    CHECK(changing == 1);

    sim.MouseMove(point2);
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK(changed == (vetoChange ? 1 : 2));
    CHECK(changing == 2);

    if ( vetoChange )
    {
        CHECK( m_tree->IsSelected(m_child1) );
        CHECK_FALSE( m_tree->IsSelected(m_child2) );
    }
    else
    {
        CHECK_FALSE( m_tree->IsSelected(m_child1) );
        CHECK( m_tree->IsSelected(m_child2) );
    }
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::SelectItemMultiInteractive", "[treectrl]")
{
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    // FIXME: This test fails on GitHub CI under wxGTK2 although works fine on
    //        development machine, no idea why though!
    if ( IsAutomaticTest() )
        return;
#endif // wxGTK2

    // this test should be only ran in multi-selection control
    m_tree->ToggleWindowStyle(wxTR_MULTIPLE);

    m_tree->ExpandAll();

    // This is currently needed to work around a problem under wxMSW: clicking
    // on an item in an unfocused control generates two selection change events
    // because of the SetFocus() call in TVN_SELCHANGED handler in wxMSW code.
    // This is, of course, wrong on its own, but fixing it without breaking
    // anything else is non-obvious, so for now at least work around this
    // problem in the test.
    m_tree->SetFocus();

    EventCounter beginedit(m_tree, wxEVT_TREE_BEGIN_LABEL_EDIT);

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

    sim.MouseMove(point2);
    wxYield();

    sim.KeyDown(WXK_CONTROL);
    sim.MouseClick();
    sim.KeyUp(WXK_CONTROL);
    wxYield();

    // m_child1 and m_child2 should be selected.
    CHECK( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );
    CHECK( beginedit.GetCount() == 0 );

    // Time needed (in ms) for the editor to display. The test will not pass
    // if the value is less than 400, 510, 800 under wxQt, wxGTK, wxMSW resp.
    const int BEGIN_EDIT_TIMEOUT = 800;

    YieldForAWhile(BEGIN_EDIT_TIMEOUT);
    sim.MouseClick();
    YieldForAWhile(BEGIN_EDIT_TIMEOUT);

    // Only m_child2 should be selected now.
    CHECK_FALSE( m_tree->IsSelected(m_child1) );
    CHECK( m_tree->IsSelected(m_child2) );
    CHECK( beginedit.GetCount() == 0 ); // No editing should take place in the event of deselection.

    sim.MouseClick();
    YieldForAWhile(BEGIN_EDIT_TIMEOUT);

    CHECK( beginedit.GetCount() == 1 ); // Start editing as usual.

    sim.Char(WXK_RETURN); // End editing and close the editor.
    wxYield();
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Menu", "[treectrl]")
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

    CHECK(menu.GetCount() == 1);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::KeyNavigation", "[treectrl]")
{
    wxUIActionSimulator sim;

    m_tree->CollapseAll();

    m_tree->SelectItem(m_root);
    wxYield();

    m_tree->SetFocus();
    sim.Char(WXK_RIGHT);
    wxYield();

    CHECK(m_tree->IsExpanded(m_root));

#ifdef wxHAS_GENERIC_TREECTRL
    sim.Char('-');
#else
    sim.Char(WXK_LEFT);
#endif

    wxYield();

    CHECK(!m_tree->IsExpanded(m_root));

    wxYield();

    sim.Char(WXK_RIGHT);
    sim.Char(WXK_DOWN);
    wxYield();

    CHECK(m_tree->GetSelection() == m_child1);

    sim.Char(WXK_DOWN);
    wxYield();

    CHECK(m_tree->GetSelection() == m_child2);
}

#endif // wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::ItemData", "[treectrl]")
{
    wxTreeItemData* child1data = new wxTreeItemData();
    wxTreeItemData* appenddata = new wxTreeItemData();
    wxTreeItemData* insertdata = new wxTreeItemData();

    m_tree->SetItemData(m_child1, child1data);

    CHECK(m_tree->GetItemData(m_child1) == child1data);
    CHECK(child1data->GetId() == m_child1);

    wxTreeItemId append = m_tree->AppendItem(m_root, "new", -1, -1, appenddata);

    CHECK(m_tree->GetItemData(append) == appenddata);
    CHECK(appenddata->GetId() == append);

    wxTreeItemId insert = m_tree->InsertItem(m_root, m_child1, "new", -1, -1,
                                             insertdata);

    CHECK(m_tree->GetItemData(insert) == insertdata);
    CHECK(insertdata->GetId() == insert);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Iteration", "[treectrl]")
{
    // Get first / next / last child
    wxTreeItemIdValue cookie;
    CHECK(m_tree->GetFirstChild(m_root, cookie) == m_child1);
    CHECK(m_tree->GetNextChild(m_root, cookie) == m_tree->GetLastChild(m_root));
    CHECK(m_tree->GetLastChild(m_root) == m_child2);

    // Get next / previous sibling
    CHECK(m_tree->GetNextSibling(m_child1) == m_child2);
    CHECK(m_tree->GetPrevSibling(m_child2) == m_child1);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Parent", "[treectrl]")
{
    CHECK(m_tree->GetRootItem() == m_root);
    CHECK(m_tree->GetItemParent(m_child1) == m_root);
    CHECK(m_tree->GetItemParent(m_child2) == m_root);
    CHECK(m_tree->GetItemParent(m_grandchild) == m_child1);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::CollapseExpand", "[treectrl]")
{
    m_tree->ExpandAll();

    CHECK(m_tree->IsExpanded(m_root));
    CHECK(m_tree->IsExpanded(m_child1));

    m_tree->CollapseAll();

    CHECK_FALSE(m_tree->IsExpanded(m_root));
    CHECK_FALSE(m_tree->IsExpanded(m_child1));

    m_tree->ExpandAllChildren(m_root);

    CHECK(m_tree->IsExpanded(m_root));
    CHECK(m_tree->IsExpanded(m_child1));

    m_tree->CollapseAllChildren(m_child1);

    CHECK_FALSE(m_tree->IsExpanded(m_child1));

    m_tree->Expand(m_child1);

    CHECK(m_tree->IsExpanded(m_child1));

    m_tree->Collapse(m_root);

    CHECK_FALSE(m_tree->IsExpanded(m_root));
    CHECK(m_tree->IsExpanded(m_child1));

    m_tree->CollapseAndReset(m_root);

    CHECK_FALSE(m_tree->IsExpanded(m_root));
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::AssignImageList", "[treectrl]")
{
    wxSize size(16, 16);

    wxImageList *imagelist = new wxImageList(size.x, size.y);
    imagelist->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size));

    wxImageList *statelist = new wxImageList(size.x, size.y);
    statelist->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, size));

    m_tree->AssignImageList(imagelist);
    m_tree->AssignStateImageList(statelist);

    CHECK(m_tree->GetImageList() == imagelist);
    CHECK(m_tree->GetStateImageList() == statelist);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Focus", "[treectrl]")
{
    m_tree->SetFocusedItem(m_child1);

    CHECK(m_tree->GetFocusedItem() == m_child1);

    m_tree->ClearFocusedItem();

    CHECK_FALSE(m_tree->GetFocusedItem());
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Bold", "[treectrl]")
{
    CHECK_FALSE(m_tree->IsBold(m_child1));

    m_tree->SetItemBold(m_child1);

    CHECK(m_tree->IsBold(m_child1));

    m_tree->SetItemBold(m_child1, false);

    CHECK_FALSE(m_tree->IsBold(m_child1));
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Visible", "[treectrl]")
{
    m_tree->CollapseAll();

    CHECK(m_tree->IsVisible(m_root));
    CHECK_FALSE(m_tree->IsVisible(m_child1));

    m_tree->EnsureVisible(m_grandchild);

    CHECK(m_tree->IsVisible(m_grandchild));

    m_tree->ExpandAll();

    CHECK(m_tree->GetFirstVisibleItem() == m_root);
    CHECK(m_tree->GetNextVisible(m_root) == m_child1);
    CHECK(m_tree->GetNextVisible(m_child1) == m_grandchild);
    CHECK(m_tree->GetNextVisible(m_grandchild) == m_child2);

    CHECK_FALSE(m_tree->GetNextVisible(m_child2));
    CHECK_FALSE(m_tree->GetPrevVisible(m_root));
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Scroll", "[treectrl]")
{
    // This trivial test just checks that calling ScrollTo() with the root item
    // doesn't crash any longer, as it used to do when the root item was hidden.
    m_tree->ScrollTo(m_root);
}

TEST_CASE_METHOD(TreeCtrlTestCase, "wxTreeCtrl::Sort", "[treectrl]")
{
    wxTreeItemId zitem = m_tree->AppendItem(m_root, "zzzz");
    wxTreeItemId aitem = m_tree->AppendItem(m_root, "aaaa");

    m_tree->SortChildren(m_root);

    wxTreeItemIdValue cookie;

    CHECK(m_tree->GetFirstChild(m_root, cookie) == aitem);
    CHECK(m_tree->GetNextChild(m_root, cookie) == m_child1);
    CHECK(m_tree->GetNextChild(m_root, cookie) == m_child2);
    CHECK(m_tree->GetNextChild(m_root, cookie) == zitem);
}

#endif //wxUSE_TREECTRL
