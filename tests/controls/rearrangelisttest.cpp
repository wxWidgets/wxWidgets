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

#include <memory>

class RearrangeListTestCase : public ItemContainerTestCase
{
public:
    RearrangeListTestCase();

protected:
    virtual wxItemContainer *GetContainer() const override
    { return m_rearrange.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_rearrange.get(); }

    std::unique_ptr<wxRearrangeList> m_rearrange;

    wxDECLARE_NO_COPY_CLASS(RearrangeListTestCase);
};

wxITEM_CONTAINER_TESTS(RearrangeListTestCase, "RearrangeList",
                       "[rearrangelist][item-container]");

RearrangeListTestCase::RearrangeListTestCase()
{
    //We do not add items here as the wxITEM_CONTAINER_TESTS add their own
    wxArrayInt order;
    wxArrayString items;

    m_rearrange = make_unique<wxRearrangeList>(wxTheApp->GetTopWindow(),
                                               wxID_ANY, wxDefaultPosition,
                                               wxDefaultSize, order, items);
}


TEST_CASE_METHOD(RearrangeListTestCase, "RearrangeList::Move",
                 "[rearrangelist]")
{
    wxArrayInt order;
    order.push_back(1);
    order.push_back(~2);
    order.push_back(0);

    wxArrayString items;
    items.push_back("first");
    items.push_back("second");
    items.push_back("third");

    m_rearrange = make_unique<wxRearrangeList>(wxTheApp->GetTopWindow(),
                                               wxID_ANY, wxDefaultPosition,
                                               wxDefaultSize, order, items);

    //Confusingly setselection sets the physical item rather than the
    //item specified in the constructor
    m_rearrange->SetSelection(0);

    CHECK(!m_rearrange->CanMoveCurrentUp());
    CHECK(m_rearrange->CanMoveCurrentDown());

    m_rearrange->SetSelection(1);

    CHECK(m_rearrange->CanMoveCurrentUp());
    CHECK(m_rearrange->CanMoveCurrentDown());

    m_rearrange->SetSelection(2);

    CHECK(m_rearrange->CanMoveCurrentUp());
    CHECK(!m_rearrange->CanMoveCurrentDown());

    m_rearrange->MoveCurrentUp();
    m_rearrange->SetSelection(0);
    m_rearrange->MoveCurrentDown();

    wxArrayInt neworder = m_rearrange->GetCurrentOrder();

    CHECK(0 == neworder[0]);
    CHECK(1 == neworder[1]);
    CHECK(~2 == neworder[2]);

    CHECK(m_rearrange->GetString(0) == "first");
    CHECK(m_rearrange->GetString(1) == "second");
    CHECK(m_rearrange->GetString(2) == "third");
}

TEST_CASE_METHOD(RearrangeListTestCase, "RearrangeList::MoveClientData",
                 "[rearrangelist]")
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

    m_rearrange = make_unique<wxRearrangeList>(wxTheApp->GetTopWindow(),
                                               wxID_ANY, wxDefaultPosition,
                                               wxDefaultSize, order, items);

    m_rearrange->SetClientObject(0, item0data);
    m_rearrange->SetClientObject(1, item1data);
    m_rearrange->SetClientObject(2, item2data);

    m_rearrange->SetSelection(0);
    m_rearrange->MoveCurrentDown();

    m_rearrange->SetSelection(2);
    m_rearrange->MoveCurrentUp();

    CHECK(m_rearrange->GetClientObject(0) == item1data);
    CHECK(m_rearrange->GetClientObject(1) == item2data);
    CHECK(m_rearrange->GetClientObject(2) == item0data);

    CHECK(m_rearrange->GetString(0) == "second");
    CHECK(m_rearrange->GetString(1) == "third");
    CHECK(m_rearrange->GetString(2) == "first");
}

#endif
