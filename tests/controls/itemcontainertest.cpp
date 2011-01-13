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

#include "wx/scopeguard.h"

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
}

void ItemContainerTestCase::Insert()
{
    wxItemContainer * const container = GetContainer();

    CPPUNIT_ASSERT_EQUAL( 0, container->Insert("item 0", 0) );
    CPPUNIT_ASSERT_EQUAL("item 0", container->GetString(0));

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    CPPUNIT_ASSERT_EQUAL( 1, container->Insert(testitems, 0) );

    CPPUNIT_ASSERT_EQUAL("item 1", container->GetString(0));
    CPPUNIT_ASSERT_EQUAL("item 2", container->GetString(1));

    wxString arritems[] = { "item 3", "item 4" };

    CPPUNIT_ASSERT_EQUAL( 2, container->Insert(2, arritems, 1) );
    CPPUNIT_ASSERT_EQUAL("item 3", container->GetString(1));
    CPPUNIT_ASSERT_EQUAL("item 4", container->GetString(2));
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
}

void ItemContainerTestCase::ItemSelection()
{
    wxItemContainer * const container = GetContainer();

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("ITEM 2"); // The same as the last one except for case.

    container->Append(testitems);

    container->SetSelection(wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("", container->GetStringSelection());

    container->SetSelection(1);
    CPPUNIT_ASSERT_EQUAL(1, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 1", container->GetStringSelection());

    CPPUNIT_ASSERT( container->SetStringSelection("item 2") );
    CPPUNIT_ASSERT_EQUAL(2, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 2", container->GetStringSelection());

    // Check that selecting a non-existent item fails.
    CPPUNIT_ASSERT( !container->SetStringSelection("bloordyblop") );

    // Check that SetStringSelection() is case-insensitive.
    CPPUNIT_ASSERT( container->SetStringSelection("ITEM 2") );
    CPPUNIT_ASSERT_EQUAL(2, container->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 2", container->GetStringSelection());
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

    WX_ASSERT_FAILS_WITH_ASSERT( container->SetClientObject((unsigned)-1, item0data) );
    WX_ASSERT_FAILS_WITH_ASSERT( container->SetClientObject(12345, item0data) );
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

    WX_ASSERT_FAILS_WITH_ASSERT( container->SetClientData((unsigned)-1, NULL) );
    WX_ASSERT_FAILS_WITH_ASSERT( container->SetClientData(12345, NULL) );
}

void ItemContainerTestCase::Set()
{
    wxItemContainer * const container = GetContainer();

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");

    container->Append(testitems);

    wxArrayString newtestitems;
    newtestitems.Add("new item 0");
    newtestitems.Add("new item 1");
    newtestitems.Add("new item 2");
    newtestitems.Add("new item 3");

    container->Set(newtestitems);

    CPPUNIT_ASSERT_EQUAL(4, container->GetCount());
    CPPUNIT_ASSERT_EQUAL("new item 1", container->GetString(1));

    wxString arrnewitems[] = { "even newer 0", "event newer 1" };

    container->Set(2, arrnewitems);

    CPPUNIT_ASSERT_EQUAL(2, container->GetCount());
    CPPUNIT_ASSERT_EQUAL("even newer 0", container->GetString(0));
}

void ItemContainerTestCase::SetString()
{
   wxItemContainer * const container = GetContainer();

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    container->Append(testitems);

    container->SetString(0, "new item 0");
#ifndef __WXOSX__
    container->SetString(2, "");
#endif

    CPPUNIT_ASSERT_EQUAL("new item 0", container->GetString(0));
#ifndef __WXOSX__
    CPPUNIT_ASSERT_EQUAL("", container->GetString(2));
#endif
}

void ItemContainerTestCase::SetSelection()
{
    wxItemContainer * const container = GetContainer();

    container->Append("first");
    container->Append("second");

    // This class is used to check that SetSelection() doesn't generate any
    // events, as documented.
    class CommandEventHandler : public wxEvtHandler
    {
    public:
        virtual bool ProcessEvent(wxEvent& event)
        {
            CPPUNIT_ASSERT_MESSAGE
            (
                "unexpected command event from SetSelection",
                !event.IsCommandEvent()
            );

            return wxEvtHandler::ProcessEvent(event);
        }
    } h;

    wxWindow * const win = GetContainerWindow();
    win->PushEventHandler(&h);
    wxON_BLOCK_EXIT_OBJ1( *win, wxWindow::PopEventHandler, false );

    container->SetSelection(0);
    CPPUNIT_ASSERT_EQUAL( 0, container->GetSelection() );

    container->SetSelection(1);
    CPPUNIT_ASSERT_EQUAL( 1, container->GetSelection() );
}
