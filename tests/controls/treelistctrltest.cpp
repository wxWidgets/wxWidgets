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


#include "wx/treelist.h"

#include "wx/app.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TreeListCtrlTestCase : public CppUnit::TestCase
{
public:
    TreeListCtrlTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( TreeListCtrlTestCase );
        CPPUNIT_TEST( Traversal );
        CPPUNIT_TEST( ItemText );
        CPPUNIT_TEST( ItemCheck );
    CPPUNIT_TEST_SUITE_END();

    // Create the control with the given style.
    void Create(long style);

    // Add an item to the tree and increment m_numItems.
    wxTreeListItem AddItem(const char *label,
                           wxTreeListItem parent = wxTreeListItem(),
                           const char *numFiles = "",
                           const char *size = "");


    // Tests:
    void Traversal();
    void ItemText();
    void ItemCheck();


    // The control itself.
    wxTreeListCtrl *m_treelist;

    // And some of its items.
    wxTreeListItem m_code,
                   m_code_osx,
                   m_code_osx_cocoa;

    // Also the total number of items in it initially
    unsigned m_numItems;

    wxDECLARE_NO_COPY_CLASS(TreeListCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreeListCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TreeListCtrlTestCase, "TreeListCtrlTestCase" );

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
    m_treelist = new wxTreeListCtrl(wxTheApp->GetTopWindow(),
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

void TreeListCtrlTestCase::setUp()
{
    m_numItems = 0;
    Create(wxTL_MULTIPLE | wxTL_3STATE);
}

void TreeListCtrlTestCase::tearDown()
{
    delete m_treelist;
    m_treelist = nullptr;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

// Test various tree traversal methods.
void TreeListCtrlTestCase::Traversal()
{
    // GetParent() tests:
    wxTreeListItem root = m_treelist->GetRootItem();
    CPPUNIT_ASSERT( !m_treelist->GetItemParent(root) );

    CPPUNIT_ASSERT_EQUAL( root, m_treelist->GetItemParent(m_code) );
    CPPUNIT_ASSERT_EQUAL( m_code, m_treelist->GetItemParent(m_code_osx) );


    // GetFirstChild() and GetNextSibling() tests:
    CPPUNIT_ASSERT_EQUAL( m_code, m_treelist->GetFirstChild(root) );
    CPPUNIT_ASSERT_EQUAL
    (
        m_code_osx,
        m_treelist->GetNextSibling
        (
            m_treelist->GetNextSibling
            (
                m_treelist->GetFirstChild(m_code)
            )
        )
    );

    // Get{First,Next}Item() test:
    unsigned numItems = 0;
    for ( wxTreeListItem item = m_treelist->GetFirstItem();
          item.IsOk();
          item = m_treelist->GetNextItem(item) )
    {
        numItems++;
    }

    CPPUNIT_ASSERT_EQUAL( m_numItems, numItems );
}

// Test accessing items text.
void TreeListCtrlTestCase::ItemText()
{
    CPPUNIT_ASSERT_EQUAL( "Cocoa", m_treelist->GetItemText(m_code_osx_cocoa) );
    CPPUNIT_ASSERT_EQUAL( "46", m_treelist->GetItemText(m_code_osx_cocoa, 1) );

    m_treelist->SetItemText(m_code_osx_cocoa, "wxCocoa");
    CPPUNIT_ASSERT_EQUAL( "wxCocoa", m_treelist->GetItemText(m_code_osx_cocoa) );

    m_treelist->SetItemText(m_code_osx_cocoa, 1, "47");
    CPPUNIT_ASSERT_EQUAL( "47", m_treelist->GetItemText(m_code_osx_cocoa, 1) );
}

// Test checking and unchecking items.
void TreeListCtrlTestCase::ItemCheck()
{
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNCHECKED,
                          m_treelist->GetCheckedState(m_code) );

    m_treelist->CheckItemRecursively(m_code);
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code_osx) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code_osx_cocoa) );

    m_treelist->UncheckItem(m_code_osx_cocoa);
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNCHECKED,
                          m_treelist->GetCheckedState(m_code_osx_cocoa) );

    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNDETERMINED,
                          m_treelist->GetCheckedState(m_code_osx) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNDETERMINED,
                          m_treelist->GetCheckedState(m_code) );

    m_treelist->CheckItemRecursively(m_code_osx, wxCHK_UNCHECKED);
    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNCHECKED,
                          m_treelist->GetCheckedState(m_code_osx) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNDETERMINED,
                          m_treelist->GetCheckedState(m_code) );
}

#endif // wxUSE_TREELISTCTRL
