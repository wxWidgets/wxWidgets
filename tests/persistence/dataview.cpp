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

#if wxUSE_DATAVIEWCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "testpersistence.h"

#ifndef WX_PRECOMP
    #include "wx/dataview.h"

    #ifdef __WXGTK__
        #include "wx/stopwatch.h"
    #endif // __WXGTK__
#endif // WX_PRECOMP

#include "wx/persist/dataview.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define DVC_PREFIX         PO_PREFIX "/DataView/dvc"
#define DVC_COL            "Column #"
#define DVC_COL_PREFIX     DVC_PREFIX "/Columns/" DVC_COL
#define DVC_SORT_PREFIX    DVC_PREFIX "/Sorting"

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

// Create the control used for testing.
static wxDataViewCtrl* CreatePersistenceTestDVC()
{
    // We can't just destroy the control itself directly, we need to destroy
    // its parent as only this will ensure that it gets wxWindowDestroyEvent
    // from which its state will be saved.
    wxWindow* const parent = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewListCtrl* const list = new wxDataViewListCtrl(parent, wxID_ANY);
    list->SetName("dvc");

    // Ensure the control is big enough to allow making its columns as wide as
    // we need them to be.
    parent->SetSize(parent->GetParent()->GetClientSize());
    list->SetSize(parent->GetClientSize());

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

    return list;
}

void GTKWaitRealized(wxDataViewCtrl* list)
{
#ifdef __WXGTK__
    wxStopWatch sw;
    while ( list->GetColumn(0)->GetWidth() == 0 )
    {
        if ( sw.Time() > 500 )
        {
            WARN("Timed out waiting for wxDataViewCtrl to be realized");
            break;
        }

        wxYield();
    }
#else // !__WXGTK__
    wxUnusedVar(list);
#endif // __WXGTK__/!__WXGTK__
}

// --------------------------------------------------------------------------
// tests themselves
// --------------------------------------------------------------------------

// Note: The wxDataViewCtrl test currently uses the derivative class
// wxDataViewListCtrl for convenience.
TEST_CASE_METHOD(PersistenceTests, "wxPersistDVC", "[persist][wxDataViewCtrl]")
{
    {
        wxDataViewCtrl* const list = CreatePersistenceTestDVC();

        // Adjust the initial settings.
        list->GetColumn(0)->SetWidth(150);
        list->GetColumn(1)->SetWidth(250);
        list->GetColumn(1)->SetSortOrder(false);

        CHECK(wxPersistenceManager::Get().Register(list));

        // We need to wait until the window is fully realized and the column
        // widths are actually set.
        GTKWaitRealized(list);

        // Deleting the control itself doesn't allow it to save its state as
        // the wxEVT_DESTROY handler is called too late, so delete its parent
        // (as would usually be the case) instead.
        delete list->GetParent();

        // Test that the relevant keys have been stored correctly.
        int val = -1;
        wxString text;

        CHECK(GetConfig().Read(DVC_COL_PREFIX "1/Width", &val));
        CHECK(150 == val);

        CHECK(GetConfig().Read(DVC_COL_PREFIX "2/Width", &val));
        CHECK(250 == val);

        CHECK(GetConfig().Read(DVC_SORT_PREFIX "/Column", &text));
        CHECK(text == "Column #2");

        CHECK(GetConfig().Read(DVC_SORT_PREFIX "/Asc", &val));
        CHECK(0 == val);
    }

    {
        wxDataViewCtrl* const list = CreatePersistenceTestDVC();

        // Test that the object was registered and restored.
        CHECK(wxPersistenceManager::Get().RegisterAndRestore(list));

        // Similar to above, we need to wait until it's realized after
        // restoring the widths.
        GTKWaitRealized(list);

        // Test that the correct values were restored.
        CHECK(150 == list->GetColumn(0)->GetWidth());
        CHECK(250 == list->GetColumn(1)->GetWidth());
        CHECK(list->GetColumn(1)->IsSortKey());
        CHECK(!list->GetColumn(1)->IsSortOrderAscending());

        delete list->GetParent();
    }
}

#endif
