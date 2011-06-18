///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/selstoretest.cpp
// Purpose:     wxSelectionStore unit test
// Author:      Vadim Zeitlin
// Created:     2008-03-31
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/selstore.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class SelStoreTestCase : public CppUnit::TestCase
{
public:
    SelStoreTestCase() { m_store = NULL; }

    virtual void setUp()
    {
        m_store = new wxSelectionStore;
        m_store->SetItemCount(NUM_ITEMS);
    }

    virtual void tearDown()
    {
        delete m_store;
        m_store = NULL;
    }

private:
    CPPUNIT_TEST_SUITE( SelStoreTestCase );
        CPPUNIT_TEST( SelectItem );
        CPPUNIT_TEST( SelectRange );
        CPPUNIT_TEST( SetItemCount );
        CPPUNIT_TEST( Clear );
    CPPUNIT_TEST_SUITE_END();

    void SelectItem();
    void SelectRange();
    void SetItemCount();
    void Clear();

    // NB: must be even
    static const unsigned NUM_ITEMS;

    wxSelectionStore *m_store;

    DECLARE_NO_COPY_CLASS(SelStoreTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SelStoreTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SelStoreTestCase, "SelStoreTestCase" );

const unsigned SelStoreTestCase::NUM_ITEMS = 10; // NB: must be even

void SelStoreTestCase::SelectItem()
{
    m_store->SelectItem(0);
    CPPUNIT_ASSERT_EQUAL( 1u, m_store->GetSelectedCount() );
    CPPUNIT_ASSERT( m_store->IsSelected(0) );

    m_store->SelectItem(NUM_ITEMS - 1);
    CPPUNIT_ASSERT_EQUAL( 2u, m_store->GetSelectedCount() );
    CPPUNIT_ASSERT( m_store->IsSelected(NUM_ITEMS - 1) );

    m_store->SelectItem(0, false);
    CPPUNIT_ASSERT_EQUAL( 1u, m_store->GetSelectedCount() );
    CPPUNIT_ASSERT( !m_store->IsSelected(0) );
}

void SelStoreTestCase::SelectRange()
{
    m_store->SelectRange(0, NUM_ITEMS/2);
    CPPUNIT_ASSERT_EQUAL( NUM_ITEMS/2 + 1, m_store->GetSelectedCount() );
    CPPUNIT_ASSERT( m_store->IsSelected(0) );
    CPPUNIT_ASSERT( !m_store->IsSelected(NUM_ITEMS - 1) );

    m_store->SelectRange(NUM_ITEMS/2, NUM_ITEMS - 1);
    CPPUNIT_ASSERT_EQUAL( NUM_ITEMS, m_store->GetSelectedCount() );
    CPPUNIT_ASSERT( m_store->IsSelected(0) );
    CPPUNIT_ASSERT( m_store->IsSelected(NUM_ITEMS - 1) );

    m_store->SelectRange(1, NUM_ITEMS - 2, false);
    CPPUNIT_ASSERT_EQUAL( 2u, m_store->GetSelectedCount() );
    CPPUNIT_ASSERT( m_store->IsSelected(0) );
    CPPUNIT_ASSERT( !m_store->IsSelected(NUM_ITEMS/2) );
    CPPUNIT_ASSERT( m_store->IsSelected(NUM_ITEMS - 1) );
}

void SelStoreTestCase::SetItemCount()
{
    m_store->SelectRange(1, NUM_ITEMS - 2);
    CPPUNIT_ASSERT_EQUAL( NUM_ITEMS - 2, m_store->GetSelectedCount() );

    m_store->SetItemCount(NUM_ITEMS/2);
    CPPUNIT_ASSERT_EQUAL( NUM_ITEMS/2 - 1, m_store->GetSelectedCount() );


    m_store->Clear();
    m_store->SetItemCount(NUM_ITEMS);


    m_store->SelectItem(NUM_ITEMS/2 - 1);
    m_store->SelectItem(NUM_ITEMS/2 + 1);
    m_store->SetItemCount(NUM_ITEMS/2);
    CPPUNIT_ASSERT_EQUAL( 1u, m_store->GetSelectedCount() );
}

void SelStoreTestCase::Clear()
{
    CPPUNIT_ASSERT_EQUAL( 0u, m_store->GetSelectedCount() );

    m_store->SelectItem(0);
    m_store->Clear();

    CPPUNIT_ASSERT_EQUAL( 0u, m_store->GetSelectedCount() );
}

