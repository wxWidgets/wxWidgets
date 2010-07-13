///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/itemcontainertest.cpp
// Purpose:     wxItemContainer unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/ctrlsub.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"

void ItemContainerTestCase::Append()
{
    wxItemContainer * const container = GetContainer();

    container->Append("item 0");

    CPPUNIT_ASSERT_EQUAL("item 0", container->GetString(0));

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    container->Append(testitems);

    CPPUNIT_ASSERT_EQUAL("item 1", container->GetString(1));
    CPPUNIT_ASSERT_EQUAL("item 2", container->GetString(2));

    wxString arritems[] = { "item 3", "item 4" };

    container->Append(2, arritems);

    CPPUNIT_ASSERT_EQUAL("item 3", container->GetString(3));
    CPPUNIT_ASSERT_EQUAL("item 4", container->GetString(4));

    container->Clear();
}

void ItemContainerTestCase::Insert()
{
    wxItemContainer * const container = GetContainer();

    container->Insert("item 0", 0);

    CPPUNIT_ASSERT_EQUAL("item 0", container->GetString(0));

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    container->Insert(testitems, 0);

    CPPUNIT_ASSERT_EQUAL("item 1", container->GetString(0));
    CPPUNIT_ASSERT_EQUAL("item 2", container->GetString(1));

    wxString arritems[] = { "item 3", "item 4" };

    container->Insert(2, arritems, 1);

    CPPUNIT_ASSERT_EQUAL("item 3", container->GetString(1));
    CPPUNIT_ASSERT_EQUAL("item 4", container->GetString(2));

    container->Clear();
}

void ItemContainerTestCase::Count()
{
    wxItemContainer * const container = GetContainer();

    CPPUNIT_ASSERT(container->IsEmpty());

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    container->Append(testitems);

    CPPUNIT_ASSERT(!container->IsEmpty());
    CPPUNIT_ASSERT_EQUAL(4, container->GetCount());

    container->Delete(0);

    CPPUNIT_ASSERT_EQUAL(3, container->GetCount());

    container->Delete(0);
    container->Delete(0);

    CPPUNIT_ASSERT_EQUAL(1, container->GetCount());

    container->Insert(testitems, 1);

    CPPUNIT_ASSERT_EQUAL(5, container->GetCount());

    container->Clear();
}

void ItemContainerTestCase::ItemSelection()
{
    wxItemContainer * const container = GetContainer();

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    container->Append(testitems);

    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("", container->GetStringSelection());

    container->SetSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 1", container->GetStringSelection());

    container->SetStringSelection("item 2");

    CPPUNIT_ASSERT_EQUAL(2, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 2", container->GetStringSelection());

    container->Clear();
}

void ItemContainerTestCase::FindString()
{
   wxItemContainer * const container = GetContainer();

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    container->Append(testitems);

    CPPUNIT_ASSERT_EQUAL(1, container->FindString("item 1"));
    CPPUNIT_ASSERT_EQUAL(1, container->FindString("ITEM 1"));
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, container->FindString("ITEM 1", true));

    container->Clear();
}

void ItemContainerTestCase::ClientData()
{
    wxItemContainer * const container = GetContainer();

    wxStringClientData* item0data = new wxStringClientData("item0data");
    wxStringClientData* item1data = new wxStringClientData("item1data");
    wxStringClientData* item2data = new wxStringClientData("item2data");
    
    container->Append("item 0", item0data);

    CPPUNIT_ASSERT_EQUAL(static_cast<wxClientData*>(item0data), 
                         container->GetClientObject(0));

    container->Append("item 1");
    container->SetClientObject(1, item1data);

    CPPUNIT_ASSERT_EQUAL(static_cast<wxClientData*>(item1data),
                         container->GetClientObject(1));

    container->Insert("item 2", 2, item2data);

    CPPUNIT_ASSERT_EQUAL(static_cast<wxClientData*>(item2data),
                         container->GetClientObject(2));
}

void ItemContainerTestCase::VoidData()
{
    wxItemContainer * const container = GetContainer();

    wxString item0data("item0data"), item1data("item0data"),
             item2data("item0data");

    void* item0 = &item0data;
    void* item1 = &item1data;
    void* item2 = &item2data;
    
    container->Append("item 0", item0);

    CPPUNIT_ASSERT_EQUAL(item0, container->GetClientData(0));

    container->Append("item 1");
    container->SetClientData(1, item1);

    CPPUNIT_ASSERT_EQUAL(item1, container->GetClientData(1));

    container->Insert("item 2", 2, item2);

    CPPUNIT_ASSERT_EQUAL(item2, container->GetClientData(2));
}
