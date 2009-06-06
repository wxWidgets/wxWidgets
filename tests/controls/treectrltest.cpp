///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treectrltest.cpp
// Purpose:     wxTreeCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/treectrl.h"

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
        CPPUNIT_TEST( HasChildren );
        CPPUNIT_TEST( SelectItemSingle );
        CPPUNIT_TEST( PseudoTest_MultiSelect );
        CPPUNIT_TEST( SelectItemMulti );
        CPPUNIT_TEST( PseudoTest_SetHiddenRoot );
        CPPUNIT_TEST( HasChildren );
    CPPUNIT_TEST_SUITE_END();

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

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TreeCtrlTestCase, "TreeCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

bool TreeCtrlTestCase::ms_multiSelect = false;
bool TreeCtrlTestCase::ms_hiddenRoot = false;

void TreeCtrlTestCase::setUp()
{
    m_tree = new wxTreeCtrl(wxTheApp->GetTopWindow());

    if ( ms_multiSelect )
        m_tree->ToggleWindowStyle(wxTR_MULTIPLE);

    if ( ms_hiddenRoot )
        m_tree->ToggleWindowStyle(wxTR_HIDE_ROOT); // actually set it

    m_root = m_tree->AddRoot("root");
    m_child1 = m_tree->AppendItem(m_root, "child1");
    m_child2 = m_tree->AppendItem(m_root, "child2");
    m_grandchild = m_tree->AppendItem(m_child1, "grandchild");
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

