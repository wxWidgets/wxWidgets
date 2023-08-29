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
#include "wx/stopwatch.h"

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
}

void ListBaseTestCase::MultiSelect()
{
#if wxUSE_UIACTIONSIMULATOR

#ifndef __WXMSW__
    // FIXME: This test fails on Travis CI although works fine on
    //        development machine, no idea why though!
    if ( IsAutomaticTest() )
        return;
#endif // !__WXMSW__

    wxListCtrl* const list = GetList();

    EventCounter focused(list, wxEVT_LIST_ITEM_FOCUSED);
    EventCounter selected(list, wxEVT_LIST_ITEM_SELECTED);
    EventCounter deselected(list, wxEVT_LIST_ITEM_DESELECTED);

    list->InsertColumn(0, "Header");

    for ( int i = 0; i < 10; ++i )
        list->InsertItem(i, wxString::Format("Item %d", i));

    wxUIActionSimulator sim;

    wxRect pos;
    list->GetItemRect(2, pos); // Choose the third item as anchor

    // We move in slightly so we are not on the edge
    wxPoint point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 10);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick(); // select the anchor
    wxYield();

    list->GetItemRect(5, pos);
    point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 10);

    sim.MouseMove(point);
    wxYield();

    sim.KeyDown(WXK_SHIFT);
    sim.MouseClick();
    sim.KeyUp(WXK_SHIFT);
    wxYield();

    // when the first item was selected the focus changes to it, but not
    // on subsequent clicks
    CPPUNIT_ASSERT_EQUAL(4, list->GetSelectedItemCount()); // item 2 to 5 (inclusive) are selected
    CPPUNIT_ASSERT_EQUAL(2, focused.GetCount()); // count the focus which was on the anchor
    CPPUNIT_ASSERT_EQUAL(4, selected.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, deselected.GetCount());

    focused.Clear();
    selected.Clear();
    deselected.Clear();

    sim.Char(WXK_END, wxMOD_SHIFT); // extend the selection to the last item
    wxYield();

    CPPUNIT_ASSERT_EQUAL(8, list->GetSelectedItemCount()); // item 2 to 9 (inclusive) are selected
    CPPUNIT_ASSERT_EQUAL(1, focused.GetCount()); // focus is on the last item
    CPPUNIT_ASSERT_EQUAL(4, selected.GetCount()); // only newly selected items got the event
    CPPUNIT_ASSERT_EQUAL(0, deselected.GetCount());

    focused.Clear();
    selected.Clear();
    deselected.Clear();

    sim.Char(WXK_HOME, wxMOD_SHIFT); // select from anchor to the first item
    wxYield();

    CPPUNIT_ASSERT_EQUAL(3, list->GetSelectedItemCount()); // item 0 to 2 (inclusive) are selected
    CPPUNIT_ASSERT_EQUAL(1, focused.GetCount()); // focus is on item 0
    CPPUNIT_ASSERT_EQUAL(2, selected.GetCount()); // events are only generated for item 0 and 1
    CPPUNIT_ASSERT_EQUAL(7, deselected.GetCount()); // item 2 (exclusive) to 9 are deselected

    focused.Clear();
    selected.Clear();
    deselected.Clear();

    list->EnsureVisible(0);
    wxYield();

    list->GetItemRect(2, pos);
    point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 10);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, list->GetSelectedItemCount()); // anchor is the only selected item
    CPPUNIT_ASSERT_EQUAL(1, focused.GetCount()); // because the focus changed from item 0 to anchor
    CPPUNIT_ASSERT_EQUAL(0, selected.GetCount()); // anchor is already in selection state
    CPPUNIT_ASSERT_EQUAL(2, deselected.GetCount()); // items 0 and 1 are deselected
#endif // wxUSE_UIACTIONSIMULATOR
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
    EventCounter deselected(list, wxEVT_LIST_ITEM_DESELECTED);

    wxUIActionSimulator sim;

    wxRect pos;
    list->GetItemRect(0, pos);

    //We move in slightly so we are not on the edge
    wxPoint point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 10);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    sim.MouseDblClick();
    wxYield();

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    // We want a point within the listctrl but below any items
    point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 50);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    // when the first item was selected the focus changes to it, but not
    // on subsequent clicks
    CPPUNIT_ASSERT_EQUAL(1, focused.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, selected.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, deselected.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, activated.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, rclick.GetCount());
#endif // wxUSE_UIACTIONSIMULATOR
}

void ListBaseTestCase::KeyDown()
{
#if wxUSE_UIACTIONSIMULATOR
    wxListCtrl* const list = GetList();

    EventCounter keydown(list, wxEVT_LIST_KEY_DOWN);

    wxUIActionSimulator sim;

    list->SetFocus();
    wxYield();
    sim.Text("aAbB"); // 4 letters + 2 shift mods.
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
    list->InsertItem(0, wxString::Format("string 0"));

    int count = list->GetCountPerPage();

    for( int i = 1; i < count + 10; i++ )
    {
        list->InsertItem(i, wxString::Format("string %d", i));
    }

    CPPUNIT_ASSERT_EQUAL(count + 10, list->GetItemCount());
    CPPUNIT_ASSERT_EQUAL(0, list->GetTopItem());
    CPPUNIT_ASSERT(list->IsVisible(0));
    CPPUNIT_ASSERT(!list->IsVisible(count + 1));

    CPPUNIT_ASSERT(list->EnsureVisible(count + 9));
    CPPUNIT_ASSERT(list->IsVisible(count + 9));
    CPPUNIT_ASSERT(!list->IsVisible(9));

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
    wxYield();

    sim.Text("sometext");
    wxYield();

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

void ListBaseTestCase::HitTest()
{
#ifdef __WXMSW__ // ..until proven to work with other platforms
    wxListCtrl* const list = GetList();
    list->SetWindowStyle(wxLC_REPORT);

    // set small image list
    wxSize size(16, 16);
    wxImageList* m_imglistSmall = new wxImageList(size.x, size.y);
    m_imglistSmall->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_LIST, size));
    list->AssignImageList(m_imglistSmall, wxIMAGE_LIST_SMALL);

    // insert 2 columns
    list->InsertColumn(0, "Column 0");
    list->InsertColumn(1, "Column 1");

    // and a couple of test items too
    list->InsertItem(0, "Item 0", 0);
    list->SetItem(0, 1, "0, 1");

    list->InsertItem(1, "Item 1", 0);

    // enable checkboxes to test state icon
    list->EnableCheckBoxes();

    // get coordinates
    wxRect rectSubItem0, rectIcon;
    list->GetSubItemRect(0, 0, rectSubItem0); // column 0
    list->GetItemRect(0, rectIcon, wxLIST_RECT_ICON); // icon
    int y = rectSubItem0.GetTop() + (rectSubItem0.GetBottom() -
            rectSubItem0.GetTop()) / 2;
    int flags = 0;

    // state icon (checkbox)
    int xCheckBox = rectSubItem0.GetLeft() + (rectIcon.GetLeft() -
                    rectSubItem0.GetLeft()) / 2;
    list->HitTest(wxPoint(xCheckBox, y), flags);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Expected wxLIST_HITTEST_ONITEMSTATEICON",
        wxLIST_HITTEST_ONITEMSTATEICON, flags);

    // icon
    int xIcon = rectIcon.GetLeft() + (rectIcon.GetRight() - rectIcon.GetLeft()) / 2;
    list->HitTest(wxPoint(xIcon, y), flags);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Expected wxLIST_HITTEST_ONITEMICON",
        wxLIST_HITTEST_ONITEMICON, flags);

    // label, beyond column 0
    wxRect rectItem;
    list->GetItemRect(0, rectItem); // entire item
    int xHit = rectSubItem0.GetRight() + (rectItem.GetRight() - rectSubItem0.GetRight()) / 2;
    list->HitTest(wxPoint(xHit, y), flags);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Expected wxLIST_HITTEST_ONITEMLABEL",
        wxLIST_HITTEST_ONITEMLABEL, flags);
#endif // __WXMSW__
}

namespace
{
    //From the sample but fixed so it actually inverts
    int wxCALLBACK
    MyCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
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
