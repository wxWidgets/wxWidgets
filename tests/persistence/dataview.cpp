///////////////////////////////////////////////////////////////////////////////
// Name:        tests/persistence/dataview.cpp
// Purpose:     wxDataViewCtrl persistence support unit tests
// Author:      wxWidgets Team
// Created:     2017-08-23
// Copyright:   (c) 2017 wxWidgets Team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "testpersistence.h"

#ifndef WX_PRECOMP
    #include "wx/dataview.h"
#endif // WX_PRECOMP

#include "wx/persist/dataview.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define DVC_PREFIX         PO_PREFIX "/DataView/dvc"
#define DVC_COL            "Column #"
#define DVC_COL_PREFIX     DVC_PREFIX "/Columns/" DVC_COL
#define DVC_SORT_PREFIX    DVC_PREFIX "/Sorting"

// --------------------------------------------------------------------------
// tests themselves
// --------------------------------------------------------------------------

// Note: The wxDataViewCtrl test currently uses the derivative class
// wxDataViewListCtrl for convenience.
TEST_CASE_METHOD(PersistenceTests, "wxPersistDVC", "[persist][wxDataViewCtrl]")
{
    // We can't just destroy the control itself directly, we need to destroy
    // its parent as only this will ensure that it gets wxWindowDestroyEvent
    // from which its state will be saved.
    wxWindow* const parent = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewListCtrl* const list = new wxDataViewListCtrl(parent, wxID_ANY);
    list->SetName("dvc");

    // Add some columns to the DVC.
    list->AppendTextColumn(DVC_COL "1",
                                wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT,
                                wxDATAVIEW_COL_RESIZABLE |
                                wxDATAVIEW_COL_REORDERABLE |
                                wxDATAVIEW_COL_SORTABLE);
    list->AppendTextColumn(DVC_COL "2",
                                wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT,
                                wxDATAVIEW_COL_RESIZABLE |
                                wxDATAVIEW_COL_REORDERABLE |
                                wxDATAVIEW_COL_SORTABLE);

    // Populate with DVC data.
    wxVector<wxVariant> data;

    data.push_back("AAAA");
    data.push_back("BBBB");
    list->AppendItem(data);

    data.clear();
    data.push_back("CCCC");
    data.push_back("DDDD");
    list->AppendItem(data);

    data.clear();
    data.push_back("EEEE");
    data.push_back("FFFF");
    list->AppendItem(data);

    SECTION("Save")
    {
        // Adjust the initial settings.
        list->GetColumn(0)->SetWidth(150);
        list->GetColumn(1)->SetWidth(250);
        list->GetColumn(1)->SetSortOrder(false);

        wxPersistenceManager::Get().Register(list);

        delete parent;

        // Test that the relevant keys have been stored correctly.
        int val;
        wxString text;

        const wxConfigBase* const conf = wxConfig::Get();

        CHECK(conf->Read(DVC_COL_PREFIX "1/Width", &val));
        CHECK(150 == val);

        CHECK(conf->Read(DVC_COL_PREFIX "2/Width", &val));
        CHECK(250 == val);

        CHECK(conf->Read(DVC_SORT_PREFIX "/Column", &text));
        CHECK(text == "Column #2");

        CHECK(conf->Read(DVC_SORT_PREFIX "/Asc", &val));
        CHECK(0 == val);
    }

    SECTION("Restore")
    {
        EnableCleanup();

        // Test that the object was registered and restored.
        CHECK(wxPersistenceManager::Get().RegisterAndRestore(list));

        // Test that the correct values were restored.
        CHECK(150 == list->GetColumn(0)->GetWidth());
        CHECK(250 == list->GetColumn(1)->GetWidth());
        CHECK(list->GetColumn(1)->IsSortKey());
        CHECK(!list->GetColumn(1)->IsSortOrderAscending());
    }
}
