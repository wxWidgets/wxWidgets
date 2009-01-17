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
        CPPUNIT_TEST( PseudoTest_SetHiddenRoot );
        CPPUNIT_TEST( HasChildren );
    CPPUNIT_TEST_SUITE_END();

    void HasChildren();
    void PseudoTest_SetHiddenRoot() { ms_hiddenRoot = true; }

    static bool ms_hiddenRoot;

    wxTreeCtrl *m_tree;

    DECLARE_NO_COPY_CLASS(TreeCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreeCtrlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TreeCtrlTestCase, "TreeCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

bool TreeCtrlTestCase::ms_hiddenRoot = false;

void TreeCtrlTestCase::setUp()
{
    m_tree = new wxTreeCtrl(wxTheApp->GetTopWindow());
    if ( ms_hiddenRoot )
        m_tree->ToggleWindowStyle(wxTR_HIDE_ROOT); // actually set it
}

void TreeCtrlTestCase::tearDown()
{
    delete m_tree;
    m_tree = NULL;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

void TreeCtrlTestCase::HasChildren()
{
    const wxTreeItemId root = m_tree->AddRoot("root");
    const wxTreeItemId child1 = m_tree->AppendItem(root, "child1");
    const wxTreeItemId child2 = m_tree->AppendItem(root, "child2");
    const wxTreeItemId grandchild = m_tree->AppendItem(child1, "grandchild");

    CPPUNIT_ASSERT( m_tree->HasChildren(root) );
    CPPUNIT_ASSERT( m_tree->HasChildren(child1) );
    CPPUNIT_ASSERT( !m_tree->HasChildren(child2) );
    CPPUNIT_ASSERT( !m_tree->HasChildren(grandchild) );
}

