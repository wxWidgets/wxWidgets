///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/rearrangelisttest.cpp
// Purpose:     wxRearrangeList unit test
// Author:      Steven Lamerton
// Created:     2010-07-05
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef __WXOSX_IPHONE__


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/rearrangectrl.h"
#include "itemcontainertest.h"
#include "testableframe.h"

class RearrangeListTestCase : public ItemContainerTestCase, public CppUnit::TestCase
{
public:
    RearrangeListTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    virtual wxItemContainer *GetContainer() const wxOVERRIDE { return m_rearrange; }
    virtual wxWindow *GetContainerWindow() const wxOVERRIDE { return m_rearrange; }

    CPPUNIT_TEST_SUITE( RearrangeListTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Move );
        CPPUNIT_TEST( MoveClientData );
    CPPUNIT_TEST_SUITE_END();

    void Move();
    void MoveClientData();

    wxRearrangeList* m_rearrange;

    wxDECLARE_NO_COPY_CLASS(RearrangeListTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(RearrangeListTestCase,
                               "[RearrangeListTestCase][item-container]");

void RearrangeListTestCase::setUp()
{
    //We do not add items here as the wxITEM_CONTAINER_TESTS add their own
    wxArrayInt order;
    wxArrayString items;

    m_rearrange = new wxRearrangeList(wxTheApp->GetTopWindow(), wxID_ANY,
                                      wxDefaultPosition, wxDefaultSize, order,
                                      items);
}

void RearrangeListTestCase::tearDown()
{
    wxDELETE(m_rearrange);
}

void RearrangeListTestCase::Move()
{
    wxArrayInt order;
    order.push_back(1);
    order.push_back(~2);
    order.push_back(0);

    wxArrayString items;
    items.push_back("first");
    items.push_back("second");
    items.push_back("third");

    wxDELETE(m_rearrange);

    m_rearrange = new wxRearrangeList(wxTheApp->GetTopWindow(), wxID_ANY,
                                      wxDefaultPosition, wxDefaultSize, order,
                                      items);

    //Confusingly setselection sets the physical item rather than the
    //item specified in the constructor
    m_rearrange->SetSelection(0);

    CPPUNIT_ASSERT(!m_rearrange->CanMoveCurrentUp());
    CPPUNIT_ASSERT(m_rearrange->CanMoveCurrentDown());

    m_rearrange->SetSelection(1);

    CPPUNIT_ASSERT(m_rearrange->CanMoveCurrentUp());
    CPPUNIT_ASSERT(m_rearrange->CanMoveCurrentDown());

    m_rearrange->SetSelection(2);

    CPPUNIT_ASSERT(m_rearrange->CanMoveCurrentUp());
    CPPUNIT_ASSERT(!m_rearrange->CanMoveCurrentDown());

    m_rearrange->MoveCurrentUp();
    m_rearrange->SetSelection(0);
    m_rearrange->MoveCurrentDown();

    wxArrayInt neworder = m_rearrange->GetCurrentOrder();

    CPPUNIT_ASSERT_EQUAL(neworder[0], 0);
    CPPUNIT_ASSERT_EQUAL(neworder[1], 1);
    CPPUNIT_ASSERT_EQUAL(neworder[2], ~2);

    CPPUNIT_ASSERT_EQUAL("first", m_rearrange->GetString(0));
    CPPUNIT_ASSERT_EQUAL("second", m_rearrange->GetString(1));
    CPPUNIT_ASSERT_EQUAL("third", m_rearrange->GetString(2));
}

void RearrangeListTestCase::MoveClientData()
{
    wxArrayInt order;
    order.push_back(0);
    order.push_back(1);
    order.push_back(2);

    wxArrayString items;
    items.push_back("first");
    items.push_back("second");
    items.push_back("third");

    wxClientData* item0data = new wxStringClientData("item0data");
    wxClientData* item1data = new wxStringClientData("item1data");
    wxClientData* item2data = new wxStringClientData("item2data");

    wxDELETE(m_rearrange);

    m_rearrange = new wxRearrangeList(wxTheApp->GetTopWindow(), wxID_ANY,
                                      wxDefaultPosition, wxDefaultSize, order,
                                      items);

    m_rearrange->SetClientObject(0, item0data);
    m_rearrange->SetClientObject(1, item1data);
    m_rearrange->SetClientObject(2, item2data);

    m_rearrange->SetSelection(0);
    m_rearrange->MoveCurrentDown();

    m_rearrange->SetSelection(2);
    m_rearrange->MoveCurrentUp();

    CPPUNIT_ASSERT_EQUAL(item1data, m_rearrange->GetClientObject(0));
    CPPUNIT_ASSERT_EQUAL(item2data, m_rearrange->GetClientObject(1));
    CPPUNIT_ASSERT_EQUAL(item0data, m_rearrange->GetClientObject(2));

    CPPUNIT_ASSERT_EQUAL("second", m_rearrange->GetString(0));
    CPPUNIT_ASSERT_EQUAL("third", m_rearrange->GetString(1));
    CPPUNIT_ASSERT_EQUAL("first", m_rearrange->GetString(2));
}

#endif
