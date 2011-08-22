///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treectrltest.cpp
// Purpose:     wxDataViewCtrl unit test
// Author:      Vaclav Slavik
// Created:     2011-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vaclav Slavik <vslavik@gmail.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_DATAVIEWCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/dataview.h"

#include "testableframe.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DataViewCtrlTestCase : public CppUnit::TestCase
{
public:
    DataViewCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( DataViewCtrlTestCase );
        CPPUNIT_TEST( DeleteSelected );
        CPPUNIT_TEST( DeleteNotSelected );
    CPPUNIT_TEST_SUITE_END();

    void DeleteSelected();
    void DeleteNotSelected();

    // the dataview control itself
    wxDataViewTreeCtrl *m_dvc;

    // and some of its items
    wxDataViewItem m_root,
                   m_child1,
                   m_child2,
                   m_grandchild;

    DECLARE_NO_COPY_CLASS(DataViewCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DataViewCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataViewCtrlTestCase, "DataViewCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void DataViewCtrlTestCase::setUp()
{
    m_dvc = new wxDataViewTreeCtrl(wxTheApp->GetTopWindow(),
                                   wxID_ANY,
                                   wxDefaultPosition,
                                   wxSize(400, 200),
                                   wxDV_MULTIPLE);

    m_root = m_dvc->AppendContainer(wxDataViewItem(), "The root");
      m_child1 = m_dvc->AppendContainer(m_root, "child1");
        m_grandchild = m_dvc->AppendItem(m_child1, "grandchild");
      m_child2 = m_dvc->AppendItem(m_root, "child2");

    m_dvc->SetSize(400, 200);
    m_dvc->ExpandAncestors(m_root);
    m_dvc->Refresh();
    m_dvc->Update();
}

void DataViewCtrlTestCase::tearDown()
{
    delete m_dvc;
    m_dvc = NULL;

    m_root =
    m_child1 =
    m_child2 =
    m_grandchild = wxDataViewItem();
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

void DataViewCtrlTestCase::DeleteSelected()
{
    wxDataViewItemArray sel;
    sel.push_back(m_child1);
    sel.push_back(m_grandchild);
    sel.push_back(m_child2);
    m_dvc->SetSelections(sel);

    // delete a selected item
    m_dvc->DeleteItem(m_child1);

    m_dvc->GetSelections(sel);

    // m_child1 and its children should be removed from the selection now
    CPPUNIT_ASSERT_EQUAL( 1, sel.size() );
    CPPUNIT_ASSERT( sel[0] == m_child2 );
}

void DataViewCtrlTestCase::DeleteNotSelected()
{
    wxDataViewItemArray sel;
    sel.push_back(m_child1);
    sel.push_back(m_grandchild);
    m_dvc->SetSelections(sel);

    // delete unselected item
    m_dvc->DeleteItem(m_child2);

    m_dvc->GetSelections(sel);

    // m_child1 and its children should be removed from the selection now
    CPPUNIT_ASSERT_EQUAL( 2, sel.size() );
    CPPUNIT_ASSERT( sel[0] == m_child1 );
    CPPUNIT_ASSERT( sel[1] == m_grandchild );
}

#endif //wxUSE_TREECTRL
