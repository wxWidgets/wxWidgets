///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/selstoretest.cpp
// Purpose:     wxSelectionStore unit test
// Author:      Vadim Zeitlin
// Created:     2008-03-31
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/selstore.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class SelStoreTestCase : public CppUnit::TestCase
{
public:
    SelStoreTestCase() { }

    virtual void setUp() wxOVERRIDE
    {
        m_store.SetItemCount(NUM_ITEMS);
    }

private:
    CPPUNIT_TEST_SUITE( SelStoreTestCase );
        CPPUNIT_TEST( SelectItem );
        CPPUNIT_TEST( SelectRange );
        CPPUNIT_TEST( SetItemCount );
        CPPUNIT_TEST( Clear );
        CPPUNIT_TEST( Iterate );
        CPPUNIT_TEST( ItemsAddDelete );
    CPPUNIT_TEST_SUITE_END();

    void SelectItem();
    void SelectRange();
    void SetItemCount();
    void Clear();
    void Iterate();
    void ItemsAddDelete();

    // NB: must be even
    static const unsigned NUM_ITEMS;

    wxSelectionStore m_store;

    wxDECLARE_NO_COPY_CLASS(SelStoreTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SelStoreTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SelStoreTestCase, "SelStoreTestCase" );

const unsigned SelStoreTestCase::NUM_ITEMS = 10; // NB: must be even

void SelStoreTestCase::SelectItem()
{
    m_store.SelectItem(0);
    CHECK( m_store.GetSelectedCount() == 1 );
    CHECK( m_store.IsSelected(0) );

    m_store.SelectItem(NUM_ITEMS - 1);
    CHECK( m_store.GetSelectedCount() == 2 );
    CHECK( m_store.IsSelected(NUM_ITEMS - 1) );

    m_store.SelectItem(0, false);
    CHECK( m_store.GetSelectedCount() == 1 );
    CHECK( !m_store.IsSelected(0) );
}

void SelStoreTestCase::SelectRange()
{
    m_store.SelectRange(0, NUM_ITEMS/2);
    CHECK( m_store.GetSelectedCount() == NUM_ITEMS/2 + 1 );
    CHECK( m_store.IsSelected(0) );
    CHECK( !m_store.IsSelected(NUM_ITEMS - 1) );

    m_store.SelectRange(NUM_ITEMS/2, NUM_ITEMS - 1);
    CHECK( m_store.GetSelectedCount() == NUM_ITEMS );
    CHECK( m_store.IsSelected(0) );
    CHECK( m_store.IsSelected(NUM_ITEMS - 1) );

    m_store.SelectRange(1, NUM_ITEMS - 2, false);
    CHECK( m_store.GetSelectedCount() == 2 );
    CHECK( m_store.IsSelected(0) );
    CHECK( !m_store.IsSelected(NUM_ITEMS/2) );
    CHECK( m_store.IsSelected(NUM_ITEMS - 1) );
}

void SelStoreTestCase::SetItemCount()
{
    m_store.SelectRange(1, NUM_ITEMS - 2);
    CHECK( m_store.GetSelectedCount() == NUM_ITEMS - 2 );

    m_store.SetItemCount(NUM_ITEMS/2);
    CHECK( m_store.GetSelectedCount() == NUM_ITEMS/2 - 1 );


    m_store.Clear();
    m_store.SetItemCount(NUM_ITEMS);


    m_store.SelectItem(NUM_ITEMS/2 - 1);
    m_store.SelectItem(NUM_ITEMS/2 + 1);
    m_store.SetItemCount(NUM_ITEMS/2);
    CHECK( m_store.GetSelectedCount() == 1 );
}

void SelStoreTestCase::Clear()
{
    CHECK(m_store.IsEmpty());
    CHECK( m_store.GetSelectedCount() == 0 );

    m_store.SelectItem(0);

    CHECK(!m_store.IsEmpty());

    m_store.Clear();

    CHECK(m_store.IsEmpty());
    CHECK( m_store.GetSelectedCount() == 0 );
}

void SelStoreTestCase::Iterate()
{
    m_store.SelectRange(NUM_ITEMS/2 - 1, NUM_ITEMS/2 + 1);

    wxSelectionStore::IterationState cookie;
    CHECK(NUM_ITEMS/2 - 1 == m_store.GetFirstSelectedItem(cookie));
    CHECK(NUM_ITEMS/2 == m_store.GetNextSelectedItem(cookie));
    CHECK(NUM_ITEMS/2 + 1 == m_store.GetNextSelectedItem(cookie));

    CHECK(wxSelectionStore::NO_SELECTION == m_store.GetNextSelectedItem(cookie));


    m_store.SelectRange(0, NUM_ITEMS - 1);
    m_store.SelectItem(0, false);
    CHECK(1 == m_store.GetFirstSelectedItem(cookie));
}

void SelStoreTestCase::ItemsAddDelete()
{
    m_store.SelectItem(0);
    m_store.SelectItem(NUM_ITEMS/2);
    m_store.SelectItem(NUM_ITEMS - 1);

    m_store.OnItemsInserted(NUM_ITEMS/2 + 1, 1);
    CHECK(m_store.IsSelected(0));
    CHECK(m_store.IsSelected(NUM_ITEMS/2));
    CHECK(m_store.IsSelected(NUM_ITEMS));
    CHECK(m_store.GetSelectedCount() == 3);

    CHECK(m_store.OnItemsDeleted(NUM_ITEMS/2 - 1, 2));
    CHECK(m_store.IsSelected(0));
    CHECK(m_store.IsSelected(NUM_ITEMS - 2));
    CHECK(m_store.GetSelectedCount() == 2);

    m_store.OnItemsInserted(0, 2);
    CHECK(m_store.IsSelected(2));
    CHECK(m_store.IsSelected(NUM_ITEMS));
    CHECK(m_store.GetSelectedCount() == 2);

    m_store.OnItemDelete(0);

    m_store.SelectRange(0, NUM_ITEMS - 1);
    CHECK(m_store.OnItemsDeleted(0, NUM_ITEMS/2));
    CHECK(m_store.GetSelectedCount() == NUM_ITEMS/2);
    CHECK(m_store.IsSelected(0));
    CHECK(m_store.IsSelected(NUM_ITEMS/2));
}
