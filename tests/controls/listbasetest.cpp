///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbasetest.cpp
// Purpose:     Base class for wxListCtrl and wxListView tests
// Author:      Steven Lamerton
// Created:     2010-07-20
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>,
//              (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/listctrl.h"
#include "listbasetest.h"
#include "testableframe.h"
#include "asserthelper.h"
#include "wx/uiaction.h"
#include "wx/imaglist.h"
#include "wx/artprov.h"

void ListBaseTestCase::ColumnsOrder()
{
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
    wxListCtrl* const list = GetList();

    int n;
    wxListItem li;
    li.SetMask(wxLIST_MASK_TEXT);

    // first set up some columns
    static const int NUM_COLS = 3;

    list->InsertColumn(0, "Column 0");
    list->InsertColumn(1, "Column 1");
    list->InsertColumn(2, "Column 2");

    // and a couple of test items too
    list->InsertItem(0, "Item 0");
    list->SetItem(0, 1, "first in first");

    list->InsertItem(1, "Item 1");
    list->SetItem(1, 2, "second in second");


    // check that the order is natural in the beginning
    const wxArrayInt orderOrig = list->GetColumnsOrder();
    for ( n = 0; n < NUM_COLS; n++ )
        CPPUNIT_ASSERT_EQUAL( n, orderOrig[n] );

    // then rearrange them: using { 2, 0, 1 } order means that column 2 is
    // shown first, then column 0 and finally column 1
    wxArrayInt order(3);
    order[0] = 2;
    order[1] = 0;
    order[2] = 1;
    list->SetColumnsOrder(order);

    // check that we get back the same order as we set
    const wxArrayInt orderNew = list->GetColumnsOrder();
    for ( n = 0; n < NUM_COLS; n++ )
        CPPUNIT_ASSERT_EQUAL( order[n], orderNew[n] );

    // and the order -> index mappings for individual columns
    for ( n = 0; n < NUM_COLS; n++ )
        CPPUNIT_ASSERT_EQUAL( order[n], list->GetColumnIndexFromOrder(n) );

    // and also the reverse mapping
    CPPUNIT_ASSERT_EQUAL( 1, list->GetColumnOrder(0) );
    CPPUNIT_ASSERT_EQUAL( 2, list->GetColumnOrder(1) );
    CPPUNIT_ASSERT_EQUAL( 0, list->GetColumnOrder(2) );


    // finally check that accessors still use indices, not order
    CPPUNIT_ASSERT( list->GetColumn(0, li) );
    CPPUNIT_ASSERT_EQUAL( "Column 0", li.GetText() );

    li.SetId(0);
    li.SetColumn(1);
    CPPUNIT_ASSERT( list->GetItem(li) );
    CPPUNIT_ASSERT_EQUAL( "first in first", li.GetText() );

    li.SetId(1);
    li.SetColumn(2);
    CPPUNIT_ASSERT( list->GetItem(li) );
    CPPUNIT_ASSERT_EQUAL( "second in second", li.GetText() );

    //tidy up when we are finished
    list->ClearAll();
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
}



void ListBaseTestCase::ItemRect()
{
    wxListCtrl* const list = GetList();

    // set up for the test
    list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);
    list->InsertColumn(1, "Column 1", wxLIST_FORMAT_LEFT, 50);
    list->InsertColumn(2, "Column 2", wxLIST_FORMAT_LEFT, 40);

    list->InsertItem(0, "Item 0");
    list->SetItem(0, 1, "first column");
    list->SetItem(0, 1, "second column");

    // do test
    wxRect r;
    WX_ASSERT_FAILS_WITH_ASSERT( list->GetItemRect(1, r) );
    CPPUNIT_ASSERT( list->GetItemRect(0, r) );
    CPPUNIT_ASSERT_EQUAL( 150, r.GetWidth() );

    CPPUNIT_ASSERT( list->GetSubItemRect(0, 0, r) );
    CPPUNIT_ASSERT_EQUAL( 60, r.GetWidth() );

    CPPUNIT_ASSERT( list->GetSubItemRect(0, 1, r) );
    CPPUNIT_ASSERT_EQUAL( 50, r.GetWidth() );

    CPPUNIT_ASSERT( list->GetSubItemRect(0, 2, r) );
    CPPUNIT_ASSERT_EQUAL( 40, r.GetWidth() );

    WX_ASSERT_FAILS_WITH_ASSERT( list->GetSubItemRect(0, 3, r) );


    // As we have a header, the top item shouldn't be at (0, 0), but somewhere
    // below the header.
    //
    // Notice that we consider that the header can't be less than 10 pixels
    // because we don't know its exact height.
    CPPUNIT_ASSERT( list->GetItemRect(0, r) );
    CPPUNIT_ASSERT( r.y >= 10 );

    // However if we remove the header now, the item should be at (0, 0).
    list->SetWindowStyle(wxLC_REPORT | wxLC_NO_HEADER);
    CPPUNIT_ASSERT( list->GetItemRect(0, r) );
    CPPUNIT_ASSERT_EQUAL( 0, r.y );


    //tidy up when we are finished
    list->ClearAll();
}

void ListBaseTestCase::ItemText()
{
    wxListCtrl* const list = GetList();

    list->InsertColumn(0, "First");
    list->InsertColumn(1, "Second");

    list->InsertItem(0, "0,0");
    CPPUNIT_ASSERT_EQUAL( "0,0", list->GetItemText(0) );
    CPPUNIT_ASSERT_EQUAL( "", list->GetItemText(0, 1) );

    list->SetItem(0, 1, "0,1");
    CPPUNIT_ASSERT_EQUAL( "0,1", list->GetItemText(0, 1) );
}

void ListBaseTestCase::ChangeMode()
{
    wxListCtrl* const list = GetList();

    list->InsertColumn(0, "Header");
    list->InsertItem(0, "First");
    list->InsertItem(1, "Second");
    CPPUNIT_ASSERT_EQUAL( 2, list->GetItemCount() );

    // check that switching the mode preserves the items
    list->SetWindowStyle(wxLC_ICON);
    CPPUNIT_ASSERT_EQUAL( 2, list->GetItemCount() );
    CPPUNIT_ASSERT_EQUAL( "First", list->GetItemText(0) );

    // and so does switching back
    list->SetWindowStyle(wxLC_REPORT);
    CPPUNIT_ASSERT_EQUAL( 2, list->GetItemCount() );
    CPPUNIT_ASSERT_EQUAL( "First", list->GetItemText(0) );

    //tidy up when we are finished
    list->ClearAll();
}

void ListBaseTestCase::ItemClick()
{
#if wxUSE_UIACTIONSIMULATOR 

#ifdef __WXMSW__
    // FIXME: This test fails on MSW buildbot slaves although works fine on
    //        development machine, no idea why. It seems to be a problem with
    //        wxUIActionSimulator rather the wxListCtrl control itself however.
    if ( IsAutomaticTest() )
        return;
#endif // __WXMSW__

    wxListCtrl* const list = GetList();

    list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);
    list->InsertColumn(1, "Column 1", wxLIST_FORMAT_LEFT, 50);
    list->InsertColumn(2, "Column 2", wxLIST_FORMAT_LEFT, 40);

    list->InsertItem(0, "Item 0");
    list->SetItem(0, 1, "first column");
    list->SetItem(0, 2, "second column");

    EventCounter selected(list, wxEVT_LIST_ITEM_SELECTED);
    EventCounter focused(list, wxEVT_LIST_ITEM_FOCUSED);
    EventCounter activated(list, wxEVT_LIST_ITEM_ACTIVATED);
    EventCounter rclick(list, wxEVT_LIST_ITEM_RIGHT_CLICK);

    wxUIActionSimulator sim;

    wxRect pos;
    list->GetItemRect(0, pos);

    //We move in slightly so we are not on the edge
    wxPoint point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 5);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    sim.MouseDblClick();
    wxYield();

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    // when the first item was selected the focus changes to it, but not
    // on subsequent clicks
    
    // FIXME: This test fail under wxGTK & wxOSX because we get 3 FOCUSED events and
    //        2 SELECTED ones instead of the one of each we expect for some
    //        reason, this needs to be debugged as it may indicate a bug in the
    //        generic wxListCtrl implementation.
#ifndef _WX_GENERIC_LISTCTRL_H_
    CPPUNIT_ASSERT_EQUAL(1, focused.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, selected.GetCount());
#endif
    CPPUNIT_ASSERT_EQUAL(1, activated.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());

    //tidy up when we are finished
    list->ClearAll();
#endif // wxUSE_UIACTIONSIMULATOR
}

void ListBaseTestCase::KeyDown()
{
#if wxUSE_UIACTIONSIMULATOR
    wxListCtrl* const list = GetList();

    EventCounter keydown(list, wxEVT_LIST_KEY_DOWN);

    wxUIActionSimulator sim;

    list->SetFocus();
    sim.Text("aAbB");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(6, keydown.GetCount());
#endif
}

void ListBaseTestCase::DeleteItems()
{
#ifndef __WXOSX__
    wxListCtrl* const list = GetList();

    EventCounter deleteitem(list, wxEVT_LIST_DELETE_ITEM);
    EventCounter deleteall(list, wxEVT_LIST_DELETE_ALL_ITEMS);


    list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);
    list->InsertColumn(1, "Column 1", wxLIST_FORMAT_LEFT, 50);
    list->InsertColumn(2, "Column 2", wxLIST_FORMAT_LEFT, 40);

    list->InsertItem(0, "Item 0");
    list->InsertItem(1, "Item 1");
    list->InsertItem(2, "Item 1");

    list->DeleteItem(0);
    list->DeleteItem(0);
    list->DeleteAllItems();

    //Add some new items to tests ClearAll with
    list->InsertColumn(0, "Column 0");
    list->InsertItem(0, "Item 0");
    list->InsertItem(1, "Item 1");

    //Check that ClearAll actually sends a DELETE_ALL_ITEMS event
    list->ClearAll();

    //ClearAll and DeleteAllItems shouldn't send an event if there was nothing
    //to clear
    list->ClearAll();
    list->DeleteAllItems();

    CPPUNIT_ASSERT_EQUAL(2, deleteitem.GetCount());
    CPPUNIT_ASSERT_EQUAL(2, deleteall.GetCount());
#endif
}

void ListBaseTestCase::InsertItem()
{
    wxListCtrl* const list = GetList();

    EventCounter insert(list, wxEVT_LIST_INSERT_ITEM);

    list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);

    wxListItem item;
    item.SetId(0);
    item.SetText("some text");

    list->InsertItem(item);
    list->InsertItem(1, "more text");

    CPPUNIT_ASSERT_EQUAL(2, insert.GetCount());
}

void ListBaseTestCase::Find()
{
    wxListCtrl* const list = GetList();

    // set up for the test
    list->InsertColumn(0, "Column 0");
    list->InsertColumn(1, "Column 1");

    list->InsertItem(0, "Item 0");
    list->SetItem(0, 1, "first column");

    list->InsertItem(1, "Item 1");
    list->SetItem(1, 1, "first column");

    list->InsertItem(2, "Item 40");
    list->SetItem(2, 1, "first column");

    list->InsertItem(3, "ITEM 01");
    list->SetItem(3, 1, "first column");

    CPPUNIT_ASSERT_EQUAL(1, list->FindItem(-1, "Item 1"));
    CPPUNIT_ASSERT_EQUAL(2, list->FindItem(-1, "Item 4", true));
    CPPUNIT_ASSERT_EQUAL(2, list->FindItem(1, "Item 40"));
    CPPUNIT_ASSERT_EQUAL(3, list->FindItem(2, "Item 0", true));
}

void ListBaseTestCase::Visible()
{
    wxListCtrl* const list = GetList();

    list->InsertColumn(0, "Column 0");

    int count = list->GetCountPerPage();

    for( int i = 0; i < count + 10; i++ )
    {
        list->InsertItem(i, wxString::Format("string %d", i));
    }

    CPPUNIT_ASSERT_EQUAL(count + 10, list->GetItemCount());
    CPPUNIT_ASSERT_EQUAL(0, list->GetTopItem());

    list->EnsureVisible(count + 9);

    CPPUNIT_ASSERT(list->GetTopItem() != 0);
}

void ListBaseTestCase::ItemFormatting()
{
    wxListCtrl* const list = GetList();

    list->InsertColumn(0, "Column 0");

    list->InsertItem(0, "Item 0");
    list->InsertItem(1, "Item 1");
    list->InsertItem(2, "Item 2");

    list->SetTextColour(*wxYELLOW);
    list->SetBackgroundColour(*wxGREEN);
    list->SetItemTextColour(0, *wxRED);
    list->SetItemBackgroundColour(1, *wxBLUE);

    CPPUNIT_ASSERT_EQUAL(*wxGREEN, list->GetBackgroundColour());
    CPPUNIT_ASSERT_EQUAL(*wxBLUE,list->GetItemBackgroundColour(1));

    CPPUNIT_ASSERT_EQUAL(*wxYELLOW, list->GetTextColour());
    CPPUNIT_ASSERT_EQUAL(*wxRED, list->GetItemTextColour(0));
}

void ListBaseTestCase::EditLabel()
{
#if wxUSE_UIACTIONSIMULATOR
    wxListCtrl* const list = GetList();

    list->SetWindowStyleFlag(wxLC_REPORT | wxLC_EDIT_LABELS);

    list->InsertColumn(0, "Column 0");

    list->InsertItem(0, "Item 0");
    list->InsertItem(1, "Item 1");

    EventCounter beginedit(list, wxEVT_LIST_BEGIN_LABEL_EDIT);
    EventCounter endedit(list, wxEVT_LIST_END_LABEL_EDIT);

    wxUIActionSimulator sim;

    list->EditLabel(0);

    sim.Text("sometext");
    sim.Char(WXK_RETURN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, beginedit.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, endedit.GetCount());
#endif
}

void ListBaseTestCase::ImageList()
{
    wxListCtrl* const list = GetList();

    wxSize size(32, 32);

    wxImageList* imglist = new wxImageList(size.x, size.y);
    imglist->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, size));
    imglist->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size));
    imglist->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, size));

    list->AssignImageList(imglist, wxIMAGE_LIST_NORMAL);

    CPPUNIT_ASSERT_EQUAL(imglist, list->GetImageList(wxIMAGE_LIST_NORMAL));
}

namespace
{
    //From the sample but fixed so it actually inverts
    int wxCALLBACK
    MyCompareFunction(long item1, long item2, wxIntPtr WXUNUSED(sortData))
    {
        // inverse the order
        if (item1 < item2)
            return 1;
        if (item1 > item2)
            return -1;

        return 0;
    }

}

void ListBaseTestCase::Sort()
{
    wxListCtrl* const list = GetList();

    list->InsertColumn(0, "Column 0");

    list->InsertItem(0, "Item 0");
    list->SetItemData(0, 0);
    list->InsertItem(1, "Item 1");
    list->SetItemData(1, 1);

    list->SortItems(MyCompareFunction, 0);

    CPPUNIT_ASSERT_EQUAL("Item 1", list->GetItemText(0));
    CPPUNIT_ASSERT_EQUAL("Item 0", list->GetItemText(1));
}

#endif //wxUSE_LISTCTRL
