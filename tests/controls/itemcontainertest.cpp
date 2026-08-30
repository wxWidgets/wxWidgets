///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/itemcontainertest.cpp
// Purpose:     wxItemContainer unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/ctrlsub.h"
#endif // WX_PRECOMP

#include "wx/scopeguard.h"
#include "wx/uiaction.h"

#include "itemcontainertest.h"

void ItemContainerTestCase::Append()
{
    wxItemContainer * const container = GetContainer();

    container->Append("item 0");

    CHECK(container->GetString(0) == "item 0");

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    container->Append(testitems);

    CHECK(container->GetString(1) == "item 1");
    CHECK(container->GetString(2) == "item 2");

    wxString arritems[] = { "item 3", "item 4" };

    container->Append(2, arritems);

    CHECK(container->GetString(3) == "item 3");
    CHECK(container->GetString(4) == "item 4");
}

void ItemContainerTestCase::Insert()
{
    wxItemContainer * const container = GetContainer();

    CHECK( container->Insert("item 0", 0) == 0 );
    CHECK(container->GetString(0) == "item 0");

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    CHECK( container->Insert(testitems, 0) == 1 );

    CHECK(container->GetString(0) == "item 1");
    CHECK(container->GetString(1) == "item 2");

    wxString arritems[] = { "item 3", "item 4" };

    CHECK( container->Insert(2, arritems, 1) == 2 );
    CHECK(container->GetString(1) == "item 3");
    CHECK(container->GetString(2) == "item 4");
}

void ItemContainerTestCase::Count()
{
    wxItemContainer * const container = GetContainer();

    CHECK(container->IsEmpty());
    WX_ASSERT_FAILS_WITH_ASSERT( container->GetString(0) );

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    container->Append(testitems);

    CHECK(!container->IsEmpty());
    CHECK(container->GetCount() == 4);

    container->Delete(0);

    CHECK(container->GetCount() == 3);

    container->Delete(0);
    container->Delete(0);

    CHECK(container->GetCount() == 1);

    container->Insert(testitems, 1);

    CHECK(container->GetCount() == 5);
    WX_ASSERT_FAILS_WITH_ASSERT( container->GetString(10) );
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
    CHECK(container->GetSelection() == wxNOT_FOUND);
    CHECK(container->GetStringSelection() == "");

    container->SetSelection(1);
    CHECK(container->GetSelection() == 1);
    CHECK(container->GetStringSelection() == "item 1");

    CHECK( container->SetStringSelection("item 2") );
    CHECK(container->GetSelection() == 2);
    CHECK(container->GetStringSelection() == "item 2");

    // Check that selecting a non-existent item fails.
    CHECK( !container->SetStringSelection("bloordyblop") );

    // Check that SetStringSelection() is case-insensitive.
    CHECK( container->SetStringSelection("ITEM 2") );
    CHECK(container->GetSelection() == 2);
    CHECK(container->GetStringSelection() == "item 2");
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

    CHECK(container->FindString("item 1") == 1);
    CHECK(container->FindString("ITEM 1") == 1);
    CHECK(container->FindString("ITEM 1", true) == wxNOT_FOUND);
}

void ItemContainerTestCase::ClientData()
{
    wxItemContainer * const container = GetContainer();

    wxStringClientData* item0data = new wxStringClientData("item0data");
    wxStringClientData* item1data = new wxStringClientData("item1data");
    wxStringClientData* item2data = new wxStringClientData("item2data");

    container->Append("item 0", item0data);

    CHECK(container->GetClientObject(0) == static_cast<wxClientData*>(item0data));

    container->Append("item 1");
    container->SetClientObject(1, item1data);

    CHECK(container->GetClientObject(1) == static_cast<wxClientData*>(item1data));

    container->Insert("item 2", 2, item2data);

    CHECK(container->GetClientObject(2) == static_cast<wxClientData*>(item2data));

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

    CHECK(container->GetClientData(0) == item0);

    container->Append("item 1");
    container->SetClientData(1, item1);

    CHECK(container->GetClientData(1) == item1);

    container->Insert("item 2", 2, item2);

    CHECK(container->GetClientData(2) == item2);

    WX_ASSERT_FAILS_WITH_ASSERT( container->SetClientData((unsigned)-1, nullptr) );
    WX_ASSERT_FAILS_WITH_ASSERT( container->SetClientData(12345, nullptr) );

    // wxMSW used to hace problems retrieving the client data of -1 from a few
    // standard controls, especially if the last error was set before doing it,
    // so test for this specially.
    const wxUIntPtr minus1 = static_cast<wxUIntPtr>(-1);
    container->Append("item -1", wxUIntToPtr(minus1));

#ifdef __WINDOWS__
    ::SetLastError(ERROR_INVALID_DATA);
#endif

    CHECK( wxPtrToUInt(container->GetClientData(3)) == minus1 );
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

    CHECK(container->GetCount() == 4);
    CHECK(container->GetString(1) == "new item 1");

    wxString arrnewitems[] = { "even newer 0", "event newer 1" };

    container->Set(2, arrnewitems);

    CHECK(container->GetCount() == 2);
    CHECK(container->GetString(0) == "even newer 0");
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

    container->SetSelection(0);
    container->SetString(0, "new item 0");
    CHECK(container->GetString(0) == "new item 0");

    // Modifying the item shouldn't deselect it.
    CHECK(container->GetSelection() == 0);

    // wxOSX doesn't support having empty items in some containers.
#ifndef __WXOSX__
    container->SetString(2, "");
    CHECK(container->GetString(2) == "");
#endif
}

void ItemContainerTestCase::SelectionAfterDelete()
{
    wxItemContainer * const container = GetContainer();

    container->Append("item 0");
    container->Append("item 1");
    container->Append("item 2");
    container->Append("item 3");

    container->SetSelection(1);
    CHECK( container->GetSelection() == 1 );

    container->Delete(3);
    CHECK( container->GetSelection() == 1 );

    container->Delete(1);
    CHECK( container->GetSelection() == wxNOT_FOUND );

    container->SetSelection(1);
    container->Delete(1);
    CHECK( container->GetSelection() == wxNOT_FOUND );

    container->SetSelection(0);
    container->Delete(0);
    CHECK( container->GetSelection() == wxNOT_FOUND );
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
        virtual bool ProcessEvent(wxEvent& event) override
        {
            INFO("unexpected command event from SetSelection");
            CHECK( !event.IsCommandEvent() );

            return wxEvtHandler::ProcessEvent(event);
        }
    } h;

    wxWindow * const win = GetContainerWindow();
    win->PushEventHandler(&h);
    wxON_BLOCK_EXIT_OBJ1( *win, wxWindow::PopEventHandler, false );

    container->SetSelection(0);
    CHECK( container->GetSelection() == 0 );

    container->SetSelection(1);
    CHECK( container->GetSelection() == 1 );
}

#if wxUSE_UIACTIONSIMULATOR

void ItemContainerTestCase::SimSelect()
{
    if ( !EnableUITests() )
        return;

    wxItemContainer * const container = GetContainer();

    container->Append("first");
    container->Append("second");
    container->Append("third");

    GetContainerWindow()->SetFocus();
    wxYield();

    wxUIActionSimulator sim;
    CHECK( sim.Select("third") );
    CHECK( container->GetSelection() == 2 );

    CHECK( sim.Select("first") );
    CHECK( container->GetSelection() == 0 );

    CHECK( !sim.Select("tenth") );
}

#endif // wxUSE_UIACTIONSIMULATOR
