/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:     wxListCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef __WXMSW__
    #include "mondrian.xpm"

    #include "bitmaps/toolbrai.xpm"
    #include "bitmaps/toolchar.xpm"
    #include "bitmaps/tooldata.xpm"
    #include "bitmaps/toolnote.xpm"
    #include "bitmaps/tooltodo.xpm"
    #include "bitmaps/toolchec.xpm"
    #include "bitmaps/toolgame.xpm"
    #include "bitmaps/tooltime.xpm"
    #include "bitmaps/toolword.xpm"
    #include "bitmaps/small1.xpm"
#endif

#include "wx/listctrl.h"
#include "wx/timer.h"           // for wxStopWatch
#include "wx/colordlg.h"        // for wxGetColourFromUser

#include "listtest.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(BUSY_ON, MyFrame::BusyOn)
    EVT_MENU(BUSY_OFF, MyFrame::BusyOff)
    EVT_MENU(LIST_QUIT, MyFrame::OnQuit)
    EVT_MENU(LIST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(LIST_LIST_VIEW, MyFrame::OnListView)
    EVT_MENU(LIST_REPORT_VIEW, MyFrame::OnReportView)
    EVT_MENU(LIST_ICON_VIEW, MyFrame::OnIconView)
    EVT_MENU(LIST_ICON_TEXT_VIEW, MyFrame::OnIconTextView)
    EVT_MENU(LIST_SMALL_ICON_VIEW, MyFrame::OnSmallIconView)
    EVT_MENU(LIST_SMALL_ICON_TEXT_VIEW, MyFrame::OnSmallIconTextView)
    EVT_MENU(LIST_TOGGLE_FIRST, MyFrame::OnToggleFirstSel)
    EVT_MENU(LIST_DESELECT_ALL, MyFrame::OnDeselectAll)
    EVT_MENU(LIST_SELECT_ALL, MyFrame::OnSelectAll)
    EVT_MENU(LIST_DELETE_ALL, MyFrame::OnDeleteAll)
    EVT_MENU(LIST_SORT, MyFrame::OnSort)
    EVT_MENU(LIST_SET_FG_COL, MyFrame::OnSetFgColour)
    EVT_MENU(LIST_SET_BG_COL, MyFrame::OnSetBgColour)
    EVT_MENU(LIST_TOGGLE_MULTI_SEL, MyFrame::OnToggleMultiSel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(LIST_CTRL, MyListCtrl::OnBeginRDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnEndLabelEdit)
    EVT_LIST_DELETE_ITEM(LIST_CTRL, MyListCtrl::OnDeleteItem)
    EVT_LIST_DELETE_ALL_ITEMS(LIST_CTRL, MyListCtrl::OnDeleteAllItems)
    EVT_LIST_GET_INFO(LIST_CTRL, MyListCtrl::OnGetInfo)
    EVT_LIST_SET_INFO(LIST_CTRL, MyListCtrl::OnSetInfo)
    EVT_LIST_ITEM_SELECTED(LIST_CTRL, MyListCtrl::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LIST_CTRL, MyListCtrl::OnDeselected)
    EVT_LIST_KEY_DOWN(LIST_CTRL, MyListCtrl::OnListKeyDown)
    EVT_LIST_ITEM_ACTIVATED(LIST_CTRL, MyListCtrl::OnActivated)
    EVT_LIST_COL_CLICK(LIST_CTRL, MyListCtrl::OnColClick)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

int wxCALLBACK MyCompareFunction(long item1, long item2, long sortData)
{
    // inverse the order
    return item1 < item2;
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
  // Create the main frame window
  MyFrame *frame = new MyFrame("wxListCtrl Test", 50, 50, 450, 340);

  // Show the frame
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(const wxChar *title, int x, int y, int w, int h)
       : wxFrame((wxFrame *)NULL, -1, title, wxPoint(x, y), wxSize(w, h))
{
    m_listCtrl = (MyListCtrl *) NULL;
    m_logWindow = (wxTextCtrl *) NULL;

    // Give it an icon
    SetIcon( wxICON(mondrian) );

    // Make an image list containing large icons
    m_imageListNormal = new wxImageList(32, 32, TRUE);
    m_imageListSmall = new wxImageList(16, 16, TRUE);

#ifdef __WXMSW__
    m_imageListNormal->Add( wxIcon("icon1", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon2", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon3", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon4", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon5", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon6", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon7", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon8", wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon("icon9", wxBITMAP_TYPE_ICO_RESOURCE) );

    m_imageListSmall->Add( wxIcon("iconsmall", wxBITMAP_TYPE_ICO_RESOURCE) );

#else
    m_imageListNormal->Add( wxIcon( toolbrai_xpm ) );
    m_imageListNormal->Add( wxIcon( toolchar_xpm ) );
    m_imageListNormal->Add( wxIcon( tooldata_xpm ) );
    m_imageListNormal->Add( wxIcon( toolnote_xpm ) );
    m_imageListNormal->Add( wxIcon( tooltodo_xpm ) );
    m_imageListNormal->Add( wxIcon( toolchec_xpm ) );
    m_imageListNormal->Add( wxIcon( toolgame_xpm ) );
    m_imageListNormal->Add( wxIcon( tooltime_xpm ) );
    m_imageListNormal->Add( wxIcon( toolword_xpm ) );

    m_imageListSmall->Add( wxIcon( small1_xpm) );
#endif

    // Make a menubar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(LIST_ABOUT, "&About");
    menuFile->AppendSeparator();
#if 0 // what is this for? (VZ)
    menuFile->Append(BUSY_ON,         "&Busy cursor on");
    menuFile->Append(BUSY_OFF,         "&Busy cursor off");
    menuFile->AppendSeparator();
#endif
    menuFile->Append(LIST_QUIT, "E&xit\tAlt-X");

    wxMenu *menuView = new wxMenu;
    menuView->Append(LIST_LIST_VIEW,         "&List view\tF1");
    menuView->Append(LIST_REPORT_VIEW,         "&Report view\tF2");
    menuView->Append(LIST_ICON_VIEW,         "&Icon view\tF3");
    menuView->Append(LIST_ICON_TEXT_VIEW,     "Icon view with &text\tF4");
    menuView->Append(LIST_SMALL_ICON_VIEW,     "&Small icon view\tF5");
    menuView->Append(LIST_SMALL_ICON_TEXT_VIEW,     "Small icon &view with text\tF6");

    wxMenu *menuList = new wxMenu;
    menuList->Append(LIST_TOGGLE_FIRST, "&Toggle first item\tCtrl-T");
    menuList->Append(LIST_DESELECT_ALL, "&Deselect All\tCtrl-D");
    menuList->Append(LIST_SELECT_ALL, "S&elect All\tCtrl-A");
    menuList->AppendSeparator();
    menuList->Append(LIST_SORT, "&Sort\tCtrl-S");
    menuList->AppendSeparator();
    menuList->Append(LIST_DELETE_ALL, "Delete &all items");
    menuList->AppendSeparator();
    menuList->Append(LIST_TOGGLE_MULTI_SEL, "&Multiple selection\tCtrl-M",
            "Toggle multiple selection", TRUE);

    wxMenu *menuCol = new wxMenu;
    menuCol->Append(LIST_SET_FG_COL, "&Foreground colour...");
    menuCol->Append(LIST_SET_BG_COL, "&Background colour...");

    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuFile, "&File");
    menubar->Append(menuView, "&View");
    menubar->Append(menuList, "&List");
    menubar->Append(menuCol, "&Colour");
    SetMenuBar(menubar);

    m_listCtrl = new MyListCtrl(this, LIST_CTRL,
                                wxDefaultPosition, wxDefaultSize,
                                wxLC_LIST |
                                wxSUNKEN_BORDER |
                                wxLC_EDIT_LABELS |
                                // wxLC_USER_TEXT requires app to supply all
                                // text on demand
                                //wxLC_USER_TEXT |
                                wxLC_SINGLE_SEL
                               );

    m_logWindow = new wxTextCtrl(this, -1, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxSUNKEN_BORDER);

    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));

    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->top.SameAs            (this, wxTop);
    c->left.SameAs        (this, wxLeft);
    c->right.SameAs        (this, wxRight);
    c->height.PercentOf    (this, wxHeight, 66);
    m_listCtrl->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->top.Below            (m_listCtrl);
    c->left.SameAs        (this, wxLeft);
    c->right.SameAs        (this, wxRight);
    c->bottom.SameAs        (this, wxBottom);
    m_logWindow->SetConstraints(c);
    SetAutoLayout(TRUE);

    for ( int i = 0; i < 30; i++ )
    {
        long idx = m_listCtrl->InsertItem(i, wxString::Format(_T("Item %d"), i));
        m_listCtrl->SetItemData(idx, i*i);
    }

    CreateStatusBar(3);
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);

    delete m_imageListNormal;
    delete m_imageListSmall;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::BusyOn(wxCommandEvent& WXUNUSED(event))
{
   wxBeginBusyCursor();
}

void MyFrame::BusyOff(wxCommandEvent& WXUNUSED(event))
{
   wxEndBusyCursor();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dialog(this, "List test sample\nJulian Smart (c) 1997",
            "About list test", wxOK|wxCANCEL);

    dialog.ShowModal();
}

void MyFrame::OnToggleFirstSel(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MyFrame::OnDeselectAll(wxCommandEvent& WXUNUSED(event))
{
    int n = m_listCtrl->GetItemCount();
    for (int i = 0; i < n; i++)
        m_listCtrl->SetItemState(i,0,wxLIST_STATE_SELECTED);
}

void MyFrame::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    int n = m_listCtrl->GetItemCount();
    for (int i = 0; i < n; i++)
        m_listCtrl->SetItemState(i,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MyFrame::OnListView(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->ClearAll();
    m_logWindow->Clear();

    m_listCtrl->SetSingleStyle(wxLC_LIST);
    m_listCtrl->SetImageList((wxImageList *) NULL, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList((wxImageList *) NULL, wxIMAGE_LIST_SMALL);

    for ( int i=0; i < 30; i++)
    {
        wxChar buf[20];
        wxSprintf(buf, _T("Item %d"), i);
        m_listCtrl->InsertItem(i, buf);
    }
}

void MyFrame::OnReportView(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
    m_listCtrl->ClearAll();

    m_listCtrl->SetSingleStyle(wxLC_REPORT);
    m_listCtrl->SetImageList((wxImageList *) NULL, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    m_listCtrl->InsertColumn(0, "Column 1"); // , wxLIST_FORMAT_LEFT, 140);
    m_listCtrl->InsertColumn(1, "Column 2"); // , wxLIST_FORMAT_LEFT, 140);
    m_listCtrl->InsertColumn(2, "One More Column (2)"); // , wxLIST_FORMAT_LEFT, 140);

    // to speed up inserting we hide the control temporarily
    m_listCtrl->Hide();

    wxStopWatch sw;

    wxString buf;
    static const int NUM_ITEMS = 3000;
    for ( int i = 0; i < NUM_ITEMS; i++ )
    {
        buf.Printf(_T("This is item %d"), i);
        long tmp = m_listCtrl->InsertItem(i, buf, 0);
        m_listCtrl->SetItemData(tmp, i);

        buf.Printf(_T("Col 1, item %d"), i);
        tmp = m_listCtrl->SetItem(i, 1, buf);

        buf.Printf(_T("Item %d in column 2"), i);
        tmp = m_listCtrl->SetItem(i, 2, buf);
    }

    m_logWindow->WriteText(wxString::Format(_T("%d items inserted in %ldms\n"),
                                            NUM_ITEMS, sw.Time()));
    m_listCtrl->Show();

    // we leave all mask fields to 0 and only change the colour
    wxListItem item;
    item.m_itemId = 0;
    item.SetTextColour(*wxRED);
    m_listCtrl->SetItem( item );

    item.m_itemId = 2;
    item.SetTextColour(*wxGREEN);
    m_listCtrl->SetItem( item );
    item.m_itemId = 4;
    item.SetTextColour(*wxLIGHT_GREY);
    item.SetFont(*wxITALIC_FONT);
    item.SetBackgroundColour(*wxRED);
    m_listCtrl->SetItem( item );

    m_listCtrl->SetTextColour(*wxBLUE);
    m_listCtrl->SetBackgroundColour(*wxLIGHT_GREY);

    m_listCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    m_listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
    m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );
}

void MyFrame::OnIconView(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
    m_listCtrl->ClearAll();

    m_listCtrl->SetSingleStyle(wxLC_ICON);
    m_listCtrl->SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    for ( int i=0; i < 9; i++)
    {
        m_listCtrl->InsertItem(i, i);
    }
}

void MyFrame::OnIconTextView(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
    m_listCtrl->ClearAll();

    m_listCtrl->SetSingleStyle(wxLC_ICON);
    m_listCtrl->SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    for ( int i=0; i < 9; i++)
    {
        wxChar buf[20];
        wxSprintf(buf, _T("Label %d"), i);
        m_listCtrl->InsertItem(i, buf, i);
    }
}

void MyFrame::OnSmallIconView(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
    m_listCtrl->ClearAll();

    m_listCtrl->SetSingleStyle(wxLC_SMALL_ICON);
    m_listCtrl->SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    for ( int i=0; i < 9; i++)
    {
        m_listCtrl->InsertItem(i, 0);
    }
}

void MyFrame::OnSmallIconTextView(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
    m_listCtrl->ClearAll();

    m_listCtrl->SetSingleStyle(wxLC_SMALL_ICON);
    m_listCtrl->SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    for ( int i=0; i < 9; i++)
    {
        m_listCtrl->InsertItem(i, "Label", 0);
    }
}

void MyFrame::OnSort(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    m_listCtrl->SortItems(MyCompareFunction, 0);

    m_logWindow->WriteText(wxString::Format(_T("Sorting %d items took %ld ms\n"),
                                            m_listCtrl->GetItemCount(),
                                            sw.Time()));
}

void MyFrame::OnToggleMultiSel(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->WriteText("Current selection mode: ");

    long flags = m_listCtrl->GetWindowStyleFlag();
    if ( flags & wxLC_SINGLE_SEL )
    {
        m_listCtrl->SetWindowStyleFlag(flags & ~wxLC_SINGLE_SEL);
        m_logWindow->WriteText("multiple");
    }
    else
    {
        m_listCtrl->SetWindowStyleFlag(flags | wxLC_SINGLE_SEL);
        m_logWindow->WriteText("single");
    }

    m_logWindow->WriteText("\nRecreate the control now\n");
}

void MyFrame::OnSetFgColour(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetForegroundColour(wxGetColourFromUser(this));
    m_listCtrl->Refresh();
}

void MyFrame::OnSetBgColour(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetBackgroundColour(wxGetColourFromUser(this));
    m_listCtrl->Refresh();
}

void MyFrame::OnDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    m_listCtrl->DeleteAllItems();

    m_logWindow->WriteText(wxString::Format(_T("Deleting %d items took %ld ms\n"),
                                            m_listCtrl->GetItemCount(),
                                            sw.Time()));
}

// MyListCtrl

void MyListCtrl::OnColClick(wxListEvent& event)
{
    wxLogMessage( "OnColumnClick at %d.", event.GetColumn() );
}

void MyListCtrl::OnBeginDrag(wxListEvent& event)
{
    wxLogMessage( "OnBeginDrag at %d,%d.",
                  event.m_pointDrag.x, event.m_pointDrag.y );
}

void MyListCtrl::OnBeginRDrag(wxListEvent& event)
{
    wxLogMessage( "OnBeginRDrag at %d,%d.",
                  event.m_pointDrag.x, event.m_pointDrag.y );
}

void MyListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
    wxLogMessage("OnBeginLabelEdit: %s", event.m_item.m_text.c_str());
}

void MyListCtrl::OnEndLabelEdit(wxListEvent& event)
{
    wxLogMessage("OnEndLabelEdit: %s", event.m_item.m_text.c_str());
}

void MyListCtrl::OnDeleteItem(wxListEvent& event)
{
    LogEvent(event, _T("OnDeleteItem"));
}

void MyListCtrl::OnDeleteAllItems(wxListEvent& event)
{
    LogEvent(event, _T("OnDeleteAllItems"));
}

void MyListCtrl::OnGetInfo(wxListEvent& event)
{
    wxString msg;

    msg << "OnGetInfo (" << event.m_item.m_itemId << ", " << event.m_item.m_col << ")";
    if ( event.m_item.m_mask & wxLIST_MASK_STATE )
        msg << " wxLIST_MASK_STATE";
    if ( event.m_item.m_mask & wxLIST_MASK_TEXT )
        msg << " wxLIST_MASK_TEXT";
    if ( event.m_item.m_mask & wxLIST_MASK_IMAGE )
        msg << " wxLIST_MASK_IMAGE";
    if ( event.m_item.m_mask & wxLIST_MASK_DATA )
        msg << " wxLIST_MASK_DATA";
    if ( event.m_item.m_mask & wxLIST_SET_ITEM )
        msg << " wxLIST_SET_ITEM";
    if ( event.m_item.m_mask & wxLIST_MASK_WIDTH )
        msg << " wxLIST_MASK_WIDTH";
    if ( event.m_item.m_mask & wxLIST_MASK_FORMAT )
        msg << " wxLIST_MASK_WIDTH";

    if ( event.m_item.m_mask & wxLIST_MASK_TEXT )
    {
        event.m_item.m_text = "My callback text";
    }

    wxLogMessage(msg);
}

void MyListCtrl::OnSetInfo(wxListEvent& event)
{
    LogEvent(event, _T("OnSetInfo"));
}

void MyListCtrl::OnSelected(wxListEvent& event)
{
    LogEvent(event, _T("OnSelected"));

    if ( GetWindowStyle() & wxLC_REPORT )
    {
        wxListItem info;
        info.m_itemId = event.m_itemIndex;
        info.m_col = 1;
        info.m_mask = wxLIST_MASK_TEXT;
        if ( GetItem(info) )
        {
            wxLogMessage("Value of the 2nd field of the selected item: %s",
                         info.m_text.c_str());
        }
        else
        {
            wxFAIL_MSG("wxListCtrl::GetItem() failed");
        }
    }
}

void MyListCtrl::OnDeselected(wxListEvent& event)
{
    LogEvent(event, _T("OnDeselected"));
}

void MyListCtrl::OnActivated(wxListEvent& event)
{
    LogEvent(event, _T("OnActivated"));
}

void MyListCtrl::OnListKeyDown(wxListEvent& event)
{
    LogEvent(event, _T("OnListKeyDown"));
}

void MyListCtrl::LogEvent(const wxListEvent& event, const wxChar *eventName)
{
    wxLogMessage(_T("Item %ld: %s (item data = %ld)"),
                 event.GetIndex(), eventName, event.GetData());
}

