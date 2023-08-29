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

class SelStoreTest
{
public:
    SelStoreTest()
    {
        m_store.SetItemCount(NUM_ITEMS);
    }

    // NB: must be even
    static const unsigned NUM_ITEMS;

    wxSelectionStore m_store;

    wxDECLARE_NO_COPY_CLASS(SelStoreTest);
};

const unsigned SelStoreTest::NUM_ITEMS = 10; // NB: must be even

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::SelectItem", "[selstore]")
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

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::SelectRange", "[selstore]")
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

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::SetItemCount", "[selstore]")
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

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::Clear", "[selstore]")
{
    CHECK(m_store.IsEmpty());
    CHECK( m_store.GetSelectedCount() == 0 );

    m_store.SelectItem(0);

    CHECK(!m_store.IsEmpty());

    m_store.Clear();

    CHECK(m_store.IsEmpty());
    CHECK( m_store.GetSelectedCount() == 0 );
}

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::Iterate", "[selstore]")
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

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::ItemsAddDelete", "[selstore]")
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

TEST_CASE_METHOD(SelStoreTest, "wxSelectionStore::InsertInSelected", "[selstore]")
{
    m_store.SelectRange(0, NUM_ITEMS - 1);
    CHECK( m_store.GetSelectedCount() == NUM_ITEMS );

    // Newly inserted items shouldn't be selected, even if all the existing
    // ones are.
    m_store.OnItemsInserted(1, 3);
    CHECK( !m_store.IsSelected(1) );
    CHECK( !m_store.IsSelected(2) );
    CHECK( !m_store.IsSelected(3) );
    CHECK( m_store.GetSelectedCount() == NUM_ITEMS );
}
