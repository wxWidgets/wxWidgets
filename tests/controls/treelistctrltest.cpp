///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treelistctrltest.cpp
// Purpose:     wxTreeListCtrl unit test.
// Author:      Vadim Zeitlin
// Created:     2011-08-27
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_TREELISTCTRL


#include "wx/dataview.h"
#include "wx/treelist.h"
#include "wx/uiaction.h"

#include "wx/app.h"

#include "testableframe.h"
#include "waitfor.h"

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TreeListCtrlTestCase
{
public:
    TreeListCtrlTestCase();

protected:
    // Create the control with the given style.
    void Create(long style);

    // Add an item to the tree and increment m_numItems.
    wxTreeListItem AddItem(const char *label,
                           wxTreeListItem parent = wxTreeListItem(),
                           const char *numFiles = "",
                           const char *size = "");


    // Tests:

    // The control itself.
    std::unique_ptr<wxTreeListCtrl> m_treelist;

    // And some of its items.
    wxTreeListItem m_code,
                   m_code_osx,
                   m_code_osx_cocoa;

    // Also the total number of items in it initially
    unsigned m_numItems;

    wxDECLARE_NO_COPY_CLASS(TreeListCtrlTestCase);
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

wxTreeListItem
TreeListCtrlTestCase::AddItem(const char *label,
                              wxTreeListItem parent,
                              const char *numFiles,
                              const char *size)
{
    if ( !parent.IsOk() )
        parent = m_treelist->GetRootItem();

    wxTreeListItem item = m_treelist->AppendItem(parent, label);
    m_treelist->SetItemText(item, 1, numFiles);
    m_treelist->SetItemText(item, 2, size);

    m_numItems++;

    return item;
}

void TreeListCtrlTestCase::Create(long style)
{
    m_treelist = make_unique<wxTreeListCtrl>(wxTheApp->GetTopWindow(),
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxSize(400, 200),
                                             style);

    m_treelist->AppendColumn("Component");
    m_treelist->AppendColumn("# Files");
    m_treelist->AppendColumn("Size");

    // Fill the control with the same data as used in the treelist sample:
    m_code = AddItem("Code");
        AddItem("wxMSW", m_code, "313", "3.94 MiB");
        AddItem("wxGTK", m_code, "180", "1.66 MiB");

        m_code_osx = AddItem("wxOSX", m_code, "265", "2.36 MiB");
            AddItem("Core", m_code_osx, "31", "347 KiB");
            AddItem("Carbon", m_code_osx, "91", "1.34 MiB");
            m_code_osx_cocoa = AddItem("Cocoa", m_code_osx, "46", "512 KiB");

    wxTreeListItem Documentation = AddItem("Documentation");
        AddItem("HTML", Documentation, "many");
        AddItem("CHM", Documentation, "1");

    wxTreeListItem Samples = AddItem("Samples");
        AddItem("minimal", Samples, "1", "7 KiB");
        AddItem("widgets", Samples, "28", "419 KiB");

    m_treelist->Refresh();
    m_treelist->Update();
}

TreeListCtrlTestCase::TreeListCtrlTestCase()
{
    m_numItems = 0;
    Create(wxTL_MULTIPLE | wxTL_3STATE);
}


// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

// Test various tree traversal methods.
TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::Traversal", "[treelistctrl]")
{
    // GetParent() tests:
    wxTreeListItem root = m_treelist->GetRootItem();
    CHECK( !m_treelist->GetItemParent(root) );

    CHECK( m_treelist->GetItemParent(m_code) == root );
    CHECK( m_treelist->GetItemParent(m_code_osx) == m_code );

    // GetFirstChild() and GetNextSibling() tests:
    CHECK( m_treelist->GetFirstChild(root) == m_code );
    CHECK( m_treelist->GetNextSibling(
               m_treelist->GetNextSibling(
                   m_treelist->GetFirstChild(m_code))) == m_code_osx );

    // Get{First,Next}Item() test:
    unsigned numItems = 0;
    for ( wxTreeListItem item = m_treelist->GetFirstItem();
          item.IsOk();
          item = m_treelist->GetNextItem(item) )
    {
        numItems++;
    }

    CHECK( numItems == m_numItems );
}

// Test accessing items text.
TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::ItemText", "[treelistctrl]")
{
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa) == "Cocoa" );
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa, 1) == "46" );

    m_treelist->SetItemText(m_code_osx_cocoa, "wxCocoa");
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa) == "wxCocoa" );

    m_treelist->SetItemText(m_code_osx_cocoa, 1, "47");
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa, 1) == "47" );
}

// Test checking and unchecking items.
TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::ItemCheck", "[treelistctrl]")
{
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_UNCHECKED );

    m_treelist->CheckItemRecursively(m_code);
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_CHECKED );
    CHECK( m_treelist->GetCheckedState(m_code_osx) == wxCHK_CHECKED );
    CHECK( m_treelist->GetCheckedState(m_code_osx_cocoa) == wxCHK_CHECKED );

    m_treelist->UncheckItem(m_code_osx_cocoa);
    CHECK( m_treelist->GetCheckedState(m_code_osx_cocoa) == wxCHK_UNCHECKED );

    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CHECK( m_treelist->GetCheckedState(m_code_osx) == wxCHK_UNDETERMINED );
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_UNDETERMINED );

    m_treelist->CheckItemRecursively(m_code_osx, wxCHK_UNCHECKED);
    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CHECK( m_treelist->GetCheckedState(m_code_osx) == wxCHK_UNCHECKED );
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_UNDETERMINED );
}

TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::MultiSelectionClick",
                 "[treelistctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    wxTreeListItem item2 = m_treelist->GetNextSibling(m_code);
    REQUIRE( item2.IsOk() );

    m_treelist->Select(m_code);
    m_treelist->Select(item2);

    wxTreeListItems selections;
    CHECK( m_treelist->GetSelections(selections) == 2u );

    EventCounter selection(m_treelist.get(), wxEVT_TREELIST_SELECTION_CHANGED);

    const wxDataViewItem item(m_code.GetID());
    wxRect rect;

    m_treelist->GetDataView()->Layout();
#ifdef __WXGTK__
    // GTK may not know row cell areas until its pending layout has run.
    REQUIRE( WaitFor("wxTreeListCtrl item to be realized", [&]() {
        rect = m_treelist->GetDataView()->GetItemRect(item);
        return !rect.IsEmpty();
    }) );
#else // !__WXGTK__
    rect = m_treelist->GetDataView()->GetItemRect(item);
    REQUIRE( !rect.IsEmpty() );
#endif // __WXGTK__/!__WXGTK__

    wxPoint point = rect.GetPosition() + wxPoint(rect.GetWidth() / 2,
                                                 rect.GetHeight() / 2);
    point = m_treelist->GetDataView()->ClientToScreen(point);

    wxUIActionSimulator sim;
    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();

    REQUIRE( WaitFor("wxTreeListCtrl selection", [&]() {
        return selection.GetCount() != 0;
    }) );

    CHECK( m_treelist->GetSelections(selections) == 1u );
    CHECK( selections[0] == m_code );
#endif // wxUSE_UIACTIONSIMULATOR
}

#endif // wxUSE_TREELISTCTRL
