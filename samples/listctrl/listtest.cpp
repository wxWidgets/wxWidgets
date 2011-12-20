/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:     wxListCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

#ifndef __WXMSW__
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

#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/timer.h"           // for wxStopWatch
#include "wx/colordlg.h"        // for wxGetColourFromUser
#include "wx/settings.h"
#include "wx/sysopt.h"
#include "wx/numdlg.h"

#include "listtest.h"


// ----------------------------------------------------------------------------
// Constants and globals
// ----------------------------------------------------------------------------

const wxChar *SMALL_VIRTUAL_VIEW_ITEMS[][2] =
{
    { wxT("Cat"), wxT("meow") },
    { wxT("Cow"), wxT("moo") },
    { wxT("Crow"), wxT("caw") },
    { wxT("Dog"), wxT("woof") },
    { wxT("Duck"), wxT("quack") },
    { wxT("Mouse"), wxT("squeak") },
    { wxT("Owl"), wxT("hoo") },
    { wxT("Pig"), wxT("oink") },
    { wxT("Pigeon"), wxT("coo") },
    { wxT("Sheep"), wxT("baaah") },
};

// number of items in icon/small icon view
static const int NUM_ICONS = 9;

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


// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame *frame = new MyFrame(wxT("wxListCtrl Test"));

    // Show the frame
    frame->Show(true);

    return true;
}



// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnSize)

    EVT_MENU(LIST_QUIT, MyFrame::OnQuit)
    EVT_MENU(LIST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(LIST_LIST_VIEW, MyFrame::OnListView)
    EVT_MENU(LIST_REPORT_VIEW, MyFrame::OnReportView)
    EVT_MENU(LIST_ICON_VIEW, MyFrame::OnIconView)
    EVT_MENU(LIST_ICON_TEXT_VIEW, MyFrame::OnIconTextView)
    EVT_MENU(LIST_SMALL_ICON_VIEW, MyFrame::OnSmallIconView)
    EVT_MENU(LIST_SMALL_ICON_TEXT_VIEW, MyFrame::OnSmallIconTextView)
    EVT_MENU(LIST_VIRTUAL_VIEW, MyFrame::OnVirtualView)
    EVT_MENU(LIST_SMALL_VIRTUAL_VIEW, MyFrame::OnSmallVirtualView)

    EVT_MENU(LIST_SET_ITEMS_COUNT, MyFrame::OnSetItemsCount)

    EVT_MENU(LIST_GOTO, MyFrame::OnGoTo)
    EVT_MENU(LIST_FOCUS_LAST, MyFrame::OnFocusLast)
    EVT_MENU(LIST_TOGGLE_FIRST, MyFrame::OnToggleFirstSel)
    EVT_MENU(LIST_DESELECT_ALL, MyFrame::OnDeselectAll)
    EVT_MENU(LIST_SELECT_ALL, MyFrame::OnSelectAll)
    EVT_MENU(LIST_DELETE, MyFrame::OnDelete)
    EVT_MENU(LIST_ADD, MyFrame::OnAdd)
    EVT_MENU(LIST_EDIT, MyFrame::OnEdit)
    EVT_MENU(LIST_DELETE_ALL, MyFrame::OnDeleteAll)
    EVT_MENU(LIST_SORT, MyFrame::OnSort)
    EVT_MENU(LIST_SET_FG_COL, MyFrame::OnSetFgColour)
    EVT_MENU(LIST_SET_BG_COL, MyFrame::OnSetBgColour)
    EVT_MENU(LIST_TOGGLE_MULTI_SEL, MyFrame::OnToggleMultiSel)
    EVT_MENU(LIST_SHOW_COL_INFO, MyFrame::OnShowColInfo)
    EVT_MENU(LIST_SHOW_SEL_INFO, MyFrame::OnShowSelInfo)
    EVT_MENU(LIST_SHOW_VIEW_RECT, MyFrame::OnShowViewRect)
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
    EVT_MENU(LIST_SET_COL_ORDER, MyFrame::OnSetColOrder)
    EVT_MENU(LIST_GET_COL_ORDER, MyFrame::OnGetColOrder)
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
    EVT_MENU(LIST_FREEZE, MyFrame::OnFreeze)
    EVT_MENU(LIST_THAW, MyFrame::OnThaw)
    EVT_MENU(LIST_TOGGLE_LINES, MyFrame::OnToggleLines)
    EVT_MENU(LIST_TOGGLE_HEADER, MyFrame::OnToggleHeader)
#ifdef __WXOSX__
    EVT_MENU(LIST_MAC_USE_GENERIC, MyFrame::OnToggleMacUseGeneric)
#endif // __WXOSX__
    EVT_MENU(LIST_FIND, MyFrame::OnFind)

    EVT_UPDATE_UI(LIST_SHOW_COL_INFO, MyFrame::OnUpdateUIEnableInReport)
    EVT_UPDATE_UI(LIST_TOGGLE_HEADER, MyFrame::OnUpdateUIEnableInReport)

    EVT_UPDATE_UI(LIST_TOGGLE_MULTI_SEL, MyFrame::OnUpdateToggleMultiSel)
    EVT_UPDATE_UI(LIST_TOGGLE_HEADER, MyFrame::OnUpdateToggleHeader)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(const wxChar *title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 500))
{
    m_listCtrl = NULL;
    m_logWindow = NULL;
    m_smallVirtual = false;
    m_numListItems = 10;

    // Give it an icon
    SetIcon(wxICON(sample));

    // Make an image list containing large icons
    m_imageListNormal = new wxImageList(32, 32, true);
    m_imageListSmall = new wxImageList(16, 16, true);

#ifdef __WXMSW__
    m_imageListNormal->Add( wxIcon(wxT("icon1"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon2"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon3"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon4"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon5"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon6"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon7"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon8"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(wxT("icon9"), wxBITMAP_TYPE_ICO_RESOURCE) );

    m_imageListSmall->Add( wxIcon(wxT("iconsmall"), wxBITMAP_TYPE_ICO_RESOURCE) );

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
    menuFile->Append(LIST_ABOUT, wxT("&About"));
    menuFile->AppendSeparator();
    menuFile->Append(LIST_QUIT, wxT("E&xit\tAlt-X"));

    wxMenu *menuView = new wxMenu;
    menuView->Append(LIST_LIST_VIEW, wxT("&List view\tF1"));
    menuView->Append(LIST_REPORT_VIEW, wxT("&Report view\tF2"));
    menuView->Append(LIST_ICON_VIEW, wxT("&Icon view\tF3"));
    menuView->Append(LIST_ICON_TEXT_VIEW, wxT("Icon view with &text\tF4"));
    menuView->Append(LIST_SMALL_ICON_VIEW, wxT("&Small icon view\tF5"));
    menuView->Append(LIST_SMALL_ICON_TEXT_VIEW, wxT("Small icon &view with text\tF6"));
    menuView->Append(LIST_VIRTUAL_VIEW, wxT("&Virtual view\tF7"));
    menuView->Append(LIST_SMALL_VIRTUAL_VIEW, wxT("Small virtual vie&w\tF8"));
    menuView->AppendSeparator();
    menuView->Append(LIST_SET_ITEMS_COUNT, "Set &number of items");
#ifdef __WXOSX__
    menuView->AppendSeparator();
    menuView->AppendCheckItem(LIST_MAC_USE_GENERIC, wxT("Mac: Use Generic Control"));
#endif

    wxMenu *menuList = new wxMenu;
    menuList->Append(LIST_GOTO, wxT("&Go to item #3\tCtrl-3"));
    menuList->Append(LIST_FOCUS_LAST, wxT("&Make last item current\tCtrl-L"));
    menuList->Append(LIST_TOGGLE_FIRST, wxT("To&ggle first item\tCtrl-G"));
    menuList->Append(LIST_DESELECT_ALL, wxT("&Deselect All\tCtrl-D"));
    menuList->Append(LIST_SELECT_ALL, wxT("S&elect All\tCtrl-A"));
    menuList->AppendSeparator();
    menuList->Append(LIST_SHOW_COL_INFO, wxT("Show &column info\tCtrl-C"));
    menuList->Append(LIST_SHOW_SEL_INFO, wxT("Show &selected items\tCtrl-S"));
    menuList->Append(LIST_SHOW_VIEW_RECT, wxT("Show &view rect"));
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
    menuList->Append(LIST_SET_COL_ORDER, wxT("Se&t columns order\tShift-Ctrl-O"));
    menuList->Append(LIST_GET_COL_ORDER, wxT("Sho&w columns order\tCtrl-O"));
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
    menuList->AppendSeparator();
    menuList->Append(LIST_SORT, wxT("Sor&t\tCtrl-T"));
    menuList->Append(LIST_FIND, "Test Find() performance");
    menuList->AppendSeparator();
    menuList->Append(LIST_ADD, wxT("&Append an item\tCtrl-P"));
    menuList->Append(LIST_EDIT, wxT("&Edit the item\tCtrl-E"));
    menuList->Append(LIST_DELETE, wxT("&Delete first item\tCtrl-X"));
    menuList->Append(LIST_DELETE_ALL, wxT("Delete &all items"));
    menuList->AppendSeparator();
    menuList->Append(LIST_FREEZE, wxT("Free&ze\tCtrl-Z"));
    menuList->Append(LIST_THAW, wxT("Tha&w\tCtrl-W"));
    menuList->AppendSeparator();
    menuList->AppendCheckItem(LIST_TOGGLE_LINES, wxT("Toggle &lines\tCtrl-I"));
    menuList->AppendCheckItem(LIST_TOGGLE_MULTI_SEL,
                              wxT("&Multiple selection\tCtrl-M"));
    menuList->Check(LIST_TOGGLE_MULTI_SEL, true);
    menuList->AppendCheckItem(LIST_TOGGLE_HEADER, "Toggle &header\tCtrl-H");
    menuList->Check(LIST_TOGGLE_HEADER, true);

    wxMenu *menuCol = new wxMenu;
    menuCol->Append(LIST_SET_FG_COL, wxT("&Foreground colour..."));
    menuCol->Append(LIST_SET_BG_COL, wxT("&Background colour..."));

    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuFile, wxT("&File"));
    menubar->Append(menuView, wxT("&View"));
    menubar->Append(menuList, wxT("&List"));
    menubar->Append(menuCol, wxT("&Colour"));
    SetMenuBar(menubar);

    m_panel = new wxPanel(this, wxID_ANY);
    m_logWindow = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);

    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));

    RecreateList(wxLC_REPORT | wxLC_SINGLE_SEL);

#ifdef __WXMSW__
    // this is useful to know specially when debugging :)
    wxLogMessage("Your version of comctl32.dll is: %d",
                 wxApp::GetComCtl32Version());
#endif

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);

    delete m_imageListNormal;
    delete m_imageListSmall;
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    DoSize();

    event.Skip();
}

void MyFrame::DoSize()
{
    if ( !m_logWindow )
        return;

    wxSize size = GetClientSize();
    wxCoord y = (2*size.y)/3;
    m_listCtrl->SetSize(0, 0, size.x, y);
    m_logWindow->SetSize(0, y + 1, size.x, size.y - y -1);
}

bool MyFrame::CheckNonVirtual() const
{
    if ( !m_listCtrl->HasFlag(wxLC_VIRTUAL) )
        return true;

    // "this" == whatever
    wxLogWarning(wxT("Can't do this in virtual view, sorry."));

    return false;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dialog(this, wxT("List test sample\nJulian Smart (c) 1997"),
            wxT("About list test"), wxOK|wxCANCEL);

    dialog.ShowModal();
}

void MyFrame::OnFreeze(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(wxT("Freezing the control"));

    m_listCtrl->Freeze();
}

void MyFrame::OnThaw(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(wxT("Thawing the control"));

    m_listCtrl->Thaw();
}

void MyFrame::OnToggleLines(wxCommandEvent& event)
{
    m_listCtrl->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, event.IsChecked());
}

void MyFrame::OnToggleHeader(wxCommandEvent& event)
{
    wxLogMessage("%s the header", event.IsChecked() ? "Showing" : "Hiding");

    m_listCtrl->ToggleWindowStyle(wxLC_NO_HEADER);
}

#ifdef __WXOSX__

void MyFrame::OnToggleMacUseGeneric(wxCommandEvent& event)
{
    wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), event.IsChecked());
}

#endif // __WXOSX__

void MyFrame::OnGoTo(wxCommandEvent& WXUNUSED(event))
{
    long index = 3;
    m_listCtrl->SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);

    long sel = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
    if ( sel != -1 )
        m_listCtrl->SetItemState(sel, 0, wxLIST_STATE_SELECTED);
    m_listCtrl->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MyFrame::OnFocusLast(wxCommandEvent& WXUNUSED(event))
{
    long index = m_listCtrl->GetItemCount() - 1;
    if ( index == -1 )
    {
        return;
    }

    m_listCtrl->SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    m_listCtrl->EnsureVisible(index);
}

void MyFrame::OnToggleFirstSel(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetItemState(0, (~m_listCtrl->GetItemState(0, wxLIST_STATE_SELECTED) ) & wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MyFrame::OnDeselectAll(wxCommandEvent& WXUNUSED(event))
{
    if ( !CheckNonVirtual() )
        return;

    int n = m_listCtrl->GetItemCount();
    for (int i = 0; i < n; i++)
        m_listCtrl->SetItemState(i,0,wxLIST_STATE_SELECTED);
}

void MyFrame::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    if ( !CheckNonVirtual() )
        return;

    int n = m_listCtrl->GetItemCount();
    for (int i = 0; i < n; i++)
        m_listCtrl->SetItemState(i,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

// ----------------------------------------------------------------------------
// changing listctrl modes
// ----------------------------------------------------------------------------

void MyFrame::RecreateList(long flags, bool withText)
{
    // we could avoid recreating it if we don't set/clear the wxLC_VIRTUAL
    // style, but it is more trouble to do it than not
#if 0
    if ( !m_listCtrl || ((flags & wxLC_VIRTUAL) !=
            (m_listCtrl->GetWindowStyleFlag() & wxLC_VIRTUAL)) )
#endif
    {
        delete m_listCtrl;

        m_listCtrl = new MyListCtrl(m_panel, LIST_CTRL,
                                    wxDefaultPosition, wxDefaultSize,
                                    flags |
                                    wxBORDER_THEME | wxLC_EDIT_LABELS);

        switch ( flags & wxLC_MASK_TYPE )
        {
            case wxLC_LIST:
                InitWithListItems();
                break;

            case wxLC_ICON:
                InitWithIconItems(withText);
                break;

            case wxLC_SMALL_ICON:
                InitWithIconItems(withText, true);
                break;

            case wxLC_REPORT:
                if ( flags & wxLC_VIRTUAL )
                    InitWithVirtualItems();
                else
                    InitWithReportItems();
                break;

            default:
                wxFAIL_MSG( wxT("unknown listctrl mode") );
        }
    }

    DoSize();

    m_logWindow->Clear();
}

void MyFrame::OnListView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_LIST);
}

void MyFrame::InitWithListItems()
{
    for ( int i = 0; i < m_numListItems; i++ )
    {
        m_listCtrl->InsertItem(i, wxString::Format(wxT("Item %d"), i));
    }
}

void MyFrame::OnReportView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_REPORT);
}

void MyFrame::InitWithReportItems()
{
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    // note that under MSW for SetColumnWidth() to work we need to create the
    // items with images initially even if we specify dummy image id
    wxListItem itemCol;
    itemCol.SetText(wxT("Column 1"));
    itemCol.SetImage(-1);
    m_listCtrl->InsertColumn(0, itemCol);

    itemCol.SetText(wxT("Column 2"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    m_listCtrl->InsertColumn(1, itemCol);

    itemCol.SetText(wxT("Column 3"));
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    m_listCtrl->InsertColumn(2, itemCol);

    // to speed up inserting we hide the control temporarily
    m_listCtrl->Hide();

    wxStopWatch sw;

    for ( int i = 0; i < m_numListItems; i++ )
    {
        m_listCtrl->InsertItemInReportView(i);
    }

    m_logWindow->WriteText(wxString::Format(wxT("%d items inserted in %ldms\n"),
                                            m_numListItems, sw.Time()));
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

    m_listCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    m_listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
    m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );

    // Set images in columns
    m_listCtrl->SetItemColumnImage(1, 1, 0);

    wxListItem info;
    info.SetImage(0);
    info.SetId(3);
    info.SetColumn(2);
    m_listCtrl->SetItem(info);

    // test SetItemFont too
    m_listCtrl->SetItemFont(0, *wxITALIC_FONT);
}

void MyFrame::InitWithIconItems(bool withText, bool sameIcon)
{
    m_listCtrl->SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    for ( int i = 0; i < NUM_ICONS; i++ )
    {
        int image = sameIcon ? 0 : i;

        if ( withText )
        {
            // Make labels of different widths to test the layout.
            wxString label;
            if ( !(i % 5) )
                label.Printf("Longer label %d", i);
            else
                label.Printf("Label %d", i);

            m_listCtrl->InsertItem(i, label, image);
        }
        else
        {
            m_listCtrl->InsertItem(i, image);
        }
    }
}

void MyFrame::OnIconView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_ICON, false);
}

void MyFrame::OnIconTextView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_ICON);
}

void MyFrame::OnSmallIconView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_SMALL_ICON, false);
}

void MyFrame::OnSmallIconTextView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_SMALL_ICON);
}

void MyFrame::OnVirtualView(wxCommandEvent& WXUNUSED(event))
{
    m_smallVirtual = false;
    RecreateList(wxLC_REPORT | wxLC_VIRTUAL);
}

void MyFrame::OnSmallVirtualView(wxCommandEvent& WXUNUSED(event))
{
    m_smallVirtual = true;
    RecreateList(wxLC_REPORT | wxLC_VIRTUAL);
}

void MyFrame::OnSetItemsCount(wxCommandEvent& WXUNUSED(event))
{
    int numItems = wxGetNumberFromUser
                   (
                        "Enter the initial number of items for "
                        "the list and report views",
                        "Number of items:",
                        "wxWidgets wxListCtrl sample",
                        m_numListItems,
                        0,
                        10000,
                        this
                   );
    if ( numItems == -1 || numItems == m_numListItems )
        return;

    m_numListItems = numItems;

    if ( m_listCtrl->HasFlag(wxLC_REPORT) &&
            !m_listCtrl->HasFlag(wxLC_VIRTUAL) )
        RecreateList(wxLC_REPORT);
    else if ( m_listCtrl->HasFlag(wxLC_LIST) )
        RecreateList(wxLC_LIST);
}

void MyFrame::InitWithVirtualItems()
{
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    if ( m_smallVirtual )
    {
        m_listCtrl->InsertColumn(0, wxT("Animal"));
        m_listCtrl->InsertColumn(1, wxT("Sound"));
        m_listCtrl->SetItemCount(WXSIZEOF(SMALL_VIRTUAL_VIEW_ITEMS));
    }
    else
    {
        m_listCtrl->InsertColumn(0, wxT("First Column"));
        m_listCtrl->InsertColumn(1, wxT("Second Column"));
        m_listCtrl->SetColumnWidth(0, 150);
        m_listCtrl->SetColumnWidth(1, 150);
        m_listCtrl->SetItemCount(1000000);
    }
}

void MyFrame::OnSort(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    m_listCtrl->SortItems(MyCompareFunction, 0);

    m_logWindow->WriteText(wxString::Format(wxT("Sorting %d items took %ld ms\n"),
                                            m_listCtrl->GetItemCount(),
                                            sw.Time()));
}

void MyFrame::OnFind(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    const int itemCount = m_listCtrl->GetItemCount();
    for ( int i = 0; i < itemCount; i++ )
        m_listCtrl->FindItem(-1, i);

    wxLogMessage("Calling Find() for all %d items took %ld ms",
                 itemCount, sw.Time());
}

void MyFrame::OnShowSelInfo(wxCommandEvent& WXUNUSED(event))
{
    int selCount = m_listCtrl->GetSelectedItemCount();
    wxLogMessage(wxT("%d items selected:"), selCount);

    // don't show too many items
    size_t shownCount = 0;

    long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
    while ( item != -1 )
    {
        wxLogMessage(wxT("\t%ld (%s)"),
                     item, m_listCtrl->GetItemText(item).c_str());

        if ( ++shownCount > 10 )
        {
            wxLogMessage(wxT("\t... more selected items snipped..."));
            break;
        }

        item = m_listCtrl->GetNextItem(item, wxLIST_NEXT_ALL,
                                       wxLIST_STATE_SELECTED);
    }
}

void MyFrame::OnShowViewRect(wxCommandEvent& WXUNUSED(event))
{
    const wxRect r = m_listCtrl->GetViewRect();
    wxLogMessage("View rect: (%d, %d)-(%d, %d)",
                 r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}

// ----------------------------------------------------------------------------
// column order tests
// ----------------------------------------------------------------------------

#ifdef wxHAS_LISTCTRL_COLUMN_ORDER

static wxString DumpIntArray(const wxArrayInt& a)
{
    wxString s("{ ");
    const size_t count = a.size();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( n )
            s += ", ";
        s += wxString::Format("%lu", (unsigned long)a[n]);
    }

    s += " }";

    return s;
}

void MyFrame::OnSetColOrder(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt order(3);
    order[0] = 2;
    order[1] = 0;
    order[2] = 1;
    if ( m_listCtrl->SetColumnsOrder(order) )
    {
        wxLogMessage("Column order set to %s", DumpIntArray(order));
    }
}

void MyFrame::OnGetColOrder(wxCommandEvent& WXUNUSED(event))
{
    // show what GetColumnsOrder() returns
    const wxArrayInt order = m_listCtrl->GetColumnsOrder();
    wxString msg = "Columns order: " +
                        DumpIntArray(m_listCtrl->GetColumnsOrder()) + "\n";

    int n;
    const int count = m_listCtrl->GetColumnCount();

    // show the results of GetColumnOrder() for each column
    msg += "GetColumnOrder() results:\n";
    for ( n = 0; n < count; n++ )
    {
        msg += wxString::Format("    %2d -> %2d\n",
                                n, m_listCtrl->GetColumnOrder(n));
    }

    // and the results of GetColumnIndexFromOrder() too
    msg += "GetColumnIndexFromOrder() results:\n";
    for ( n = 0; n < count; n++ )
    {
        msg += wxString::Format("    %2d -> %2d\n",
                                n, m_listCtrl->GetColumnIndexFromOrder(n));
    }

    wxLogMessage("%s", msg);
}

#endif // wxHAS_LISTCTRL_COLUMN_ORDER

void MyFrame::OnShowColInfo(wxCommandEvent& WXUNUSED(event))
{
    int count = m_listCtrl->GetColumnCount();
    wxLogMessage(wxT("%d columns:"), count);
    for ( int c = 0; c < count; c++ )
    {
        wxLogMessage(wxT("\tcolumn %d has width %d"), c,
                     m_listCtrl->GetColumnWidth(c));
    }
}

void MyFrame::OnUpdateUIEnableInReport(wxUpdateUIEvent& event)
{
    event.Enable( (m_listCtrl->GetWindowStyleFlag() & wxLC_REPORT) != 0 );
}

void MyFrame::OnToggleMultiSel(wxCommandEvent& WXUNUSED(event))
{
    long flags = m_listCtrl->GetWindowStyleFlag();
    if ( flags & wxLC_SINGLE_SEL )
        flags &= ~wxLC_SINGLE_SEL;
    else
        flags |= wxLC_SINGLE_SEL;

    m_logWindow->WriteText(wxString::Format(wxT("Current selection mode: %sle\n"),
                           (flags & wxLC_SINGLE_SEL) ? wxT("sing") : wxT("multip")));

    RecreateList(flags);
}

void MyFrame::OnUpdateToggleMultiSel(wxUpdateUIEvent& event)
{
     event.Check(!m_listCtrl->HasFlag(wxLC_SINGLE_SEL));
}

void MyFrame::OnUpdateToggleHeader(wxUpdateUIEvent& event)
{
    event.Check(!m_listCtrl->HasFlag(wxLC_NO_HEADER));
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

void MyFrame::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), wxT("Appended item"));
}

void MyFrame::OnEdit(wxCommandEvent& WXUNUSED(event))
{
    // demonstrate cancelling editing: this currently is wxMSW-only
#ifdef __WXMSW__
    if ( m_listCtrl->GetEditControl() )
    {
        m_listCtrl->EndEditLabel(true);
    }
    else // start editing
#endif // __WXMSW__
    {
        long itemCur = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL,
                                               wxLIST_STATE_FOCUSED);

        if ( itemCur != -1 )
        {
            m_listCtrl->EditLabel(itemCur);
        }
        else
        {
            m_logWindow->WriteText(wxT("No item to edit"));
        }
    }
}

void MyFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    if ( m_listCtrl->GetItemCount() )
    {
        m_listCtrl->DeleteItem(0);
    }
    else
    {
        m_logWindow->WriteText(wxT("Nothing to delete"));
    }
}

void MyFrame::OnDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    int itemCount = m_listCtrl->GetItemCount();

    m_listCtrl->DeleteAllItems();

    m_logWindow->WriteText(wxString::Format(wxT("Deleting %d items took %ld ms\n"),
                                            itemCount,
                                            sw.Time()));
}


// ----------------------------------------------------------------------------
// MyListCtrl
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(LIST_CTRL, MyListCtrl::OnBeginRDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnEndLabelEdit)
    EVT_LIST_DELETE_ITEM(LIST_CTRL, MyListCtrl::OnDeleteItem)
    EVT_LIST_DELETE_ALL_ITEMS(LIST_CTRL, MyListCtrl::OnDeleteAllItems)
    EVT_LIST_ITEM_SELECTED(LIST_CTRL, MyListCtrl::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LIST_CTRL, MyListCtrl::OnDeselected)
    EVT_LIST_KEY_DOWN(LIST_CTRL, MyListCtrl::OnListKeyDown)
    EVT_LIST_ITEM_ACTIVATED(LIST_CTRL, MyListCtrl::OnActivated)
    EVT_LIST_ITEM_FOCUSED(LIST_CTRL, MyListCtrl::OnFocused)

    EVT_LIST_COL_CLICK(LIST_CTRL, MyListCtrl::OnColClick)
    EVT_LIST_COL_RIGHT_CLICK(LIST_CTRL, MyListCtrl::OnColRightClick)
    EVT_LIST_COL_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnColBeginDrag)
    EVT_LIST_COL_DRAGGING(LIST_CTRL, MyListCtrl::OnColDragging)
    EVT_LIST_COL_END_DRAG(LIST_CTRL, MyListCtrl::OnColEndDrag)

    EVT_LIST_CACHE_HINT(LIST_CTRL, MyListCtrl::OnCacheHint)

#if USE_CONTEXT_MENU
    EVT_CONTEXT_MENU(MyListCtrl::OnContextMenu)
#endif
    EVT_CHAR(MyListCtrl::OnChar)

    EVT_RIGHT_DOWN(MyListCtrl::OnRightClick)
END_EVENT_TABLE()

void MyListCtrl::OnCacheHint(wxListEvent& event)
{
    wxLogMessage( wxT("OnCacheHint: cache items %ld..%ld"),
                  event.GetCacheFrom(), event.GetCacheTo() );
}

void MyListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

void MyListCtrl::OnColClick(wxListEvent& event)
{
    int col = event.GetColumn();

    // set or unset image
    static bool x = false;
    x = !x;
    SetColumnImage(col, x ? 0 : -1);

    wxLogMessage( wxT("OnColumnClick at %d."), col );
}

void MyListCtrl::OnColRightClick(wxListEvent& event)
{
    int col = event.GetColumn();
    if ( col != -1 )
    {
        SetColumnImage(col, -1);
    }

    // Show popupmenu at position
    wxMenu menu(wxT("Test"));
    menu.Append(LIST_ABOUT, wxT("&About"));
    PopupMenu(&menu, event.GetPoint());

    wxLogMessage( wxT("OnColumnRightClick at %d."), event.GetColumn() );
}

void MyListCtrl::LogColEvent(const wxListEvent& event, const wxChar *name)
{
    const int col = event.GetColumn();

    wxLogMessage(wxT("%s: column %d (width = %d or %d)."),
                 name,
                 col,
                 event.GetItem().GetWidth(),
                 GetColumnWidth(col));
}

void MyListCtrl::OnColBeginDrag(wxListEvent& event)
{
    LogColEvent( event, wxT("OnColBeginDrag") );

    if ( event.GetColumn() == 0 )
    {
        wxLogMessage(wxT("Resizing this column shouldn't work."));

        event.Veto();
    }
}

void MyListCtrl::OnColDragging(wxListEvent& event)
{
    LogColEvent( event, wxT("OnColDragging") );
}

void MyListCtrl::OnColEndDrag(wxListEvent& event)
{
    LogColEvent( event, wxT("OnColEndDrag") );
}

void MyListCtrl::OnBeginDrag(wxListEvent& event)
{
    const wxPoint& pt = event.m_pointDrag;

    int flags;
    wxLogMessage( wxT("OnBeginDrag at (%d, %d), item %ld."),
                  pt.x, pt.y, HitTest(pt, flags) );
}

void MyListCtrl::OnBeginRDrag(wxListEvent& event)
{
    wxLogMessage( wxT("OnBeginRDrag at %d,%d."),
                  event.m_pointDrag.x, event.m_pointDrag.y );
}

void MyListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
    wxLogMessage( wxT("OnBeginLabelEdit: %s"), event.m_item.m_text.c_str());

    wxTextCtrl * const text = GetEditControl();
    if ( !text )
    {
        wxLogMessage("BUG: started to edit but no edit control");
    }
    else
    {
        wxLogMessage("Edit control value: \"%s\"", text->GetValue());
    }
}

void MyListCtrl::OnEndLabelEdit(wxListEvent& event)
{
    wxLogMessage( wxT("OnEndLabelEdit: %s"),
        (
            event.IsEditCancelled() ?
            wxString("[cancelled]") :
            event.m_item.m_text
        ).c_str()
    );
}

void MyListCtrl::OnDeleteItem(wxListEvent& event)
{
    LogEvent(event, wxT("OnDeleteItem"));
    wxLogMessage( wxT("Number of items when delete event is sent: %d"), GetItemCount() );
}

void MyListCtrl::OnDeleteAllItems(wxListEvent& event)
{
    LogEvent(event, wxT("OnDeleteAllItems"));
}

void MyListCtrl::OnSelected(wxListEvent& event)
{
    LogEvent(event, wxT("OnSelected"));

    if ( GetWindowStyle() & wxLC_REPORT )
    {
        wxListItem info;
        info.m_itemId = event.m_itemIndex;
        info.m_col = 1;
        info.m_mask = wxLIST_MASK_TEXT;
        if ( GetItem(info) )
        {
            wxLogMessage(wxT("Value of the 2nd field of the selected item: %s"),
                         info.m_text.c_str());
        }
        else
        {
            wxFAIL_MSG(wxT("wxListCtrl::GetItem() failed"));
        }
    }
}

void MyListCtrl::OnDeselected(wxListEvent& event)
{
    LogEvent(event, wxT("OnDeselected"));
}

void MyListCtrl::OnActivated(wxListEvent& event)
{
    LogEvent(event, wxT("OnActivated"));
}

void MyListCtrl::OnFocused(wxListEvent& event)
{
    LogEvent(event, wxT("OnFocused"));

    event.Skip();
}

void MyListCtrl::OnListKeyDown(wxListEvent& event)
{
    long item;

    switch ( event.GetKeyCode() )
    {
        case 'C': // colorize
            {
                wxListItem info;
                info.m_itemId = event.GetIndex();
                if ( info.m_itemId == -1 )
                {
                    // no item
                    break;
                }

                GetItem(info);

                wxListItemAttr *attr = info.GetAttributes();
                if ( !attr || !attr->HasTextColour() )
                {
                    info.SetTextColour(*wxCYAN);

                    SetItem(info);

                    RefreshItem(info.m_itemId);
                }
            }
            break;

        case 'N': // next
            item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
            if ( item++ == GetItemCount() - 1 )
            {
                item = 0;
            }

            wxLogMessage(wxT("Focusing item %ld"), item);

            SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
            EnsureVisible(item);
            break;

        case 'R': // show bounding rectangle
            {
                item = event.GetIndex();
                wxRect r;
                if ( !GetItemRect(item, r) )
                {
                    wxLogError(wxT("Failed to retrieve rect of item %ld"), item);
                    break;
                }

                wxLogMessage(wxT("Bounding rect of item %ld is (%d, %d)-(%d, %d)"),
                             item, r.x, r.y, r.x + r.width, r.y + r.height);
            }
            break;

        case '1': // show sub item bounding rectangle for the given column
        case '2': // (and icon/label rectangle if Shift/Ctrl is pressed)
        case '3':
        case '4': // this column is invalid but we want to test it too
            if ( InReportView() )
            {
                int subItem = event.GetKeyCode() - '1';
                item = event.GetIndex();
                wxRect r;

                int code = wxLIST_RECT_BOUNDS;
                if ( wxGetKeyState(WXK_SHIFT) )
                    code = wxLIST_RECT_ICON;
                else if ( wxGetKeyState(WXK_CONTROL) )
                    code = wxLIST_RECT_LABEL;

                if ( !GetSubItemRect(item, subItem, r, code) )
                {
                    wxLogError(wxT("Failed to retrieve rect of item %ld column %d"), item, subItem + 1);
                    break;
                }

                wxLogMessage(wxT("Bounding rect of item %ld column %d is (%d, %d)-(%d, %d)"),
                             item, subItem + 1,
                             r.x, r.y, r.x + r.width, r.y + r.height);
            }
            break;

        case 'U': // update
            if ( !IsVirtual() )
                break;

            if ( m_updated != -1 )
                RefreshItem(m_updated);

            m_updated = event.GetIndex();
            if ( m_updated != -1 )
            {
                // we won't see changes to this item as it's selected, update
                // the next one (or the first one if we're on the last item)
                if ( ++m_updated == GetItemCount() )
                    m_updated = 0;

                wxLogMessage("Updating colour of the item %ld", m_updated);
                RefreshItem(m_updated);
            }
            break;

        case 'D': // delete
            item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            while ( item != -1 )
            {
                DeleteItem(item);

                wxLogMessage(wxT("Item %ld deleted"), item);

                // -1 because the indices were shifted by DeleteItem()
                item = GetNextItem(item - 1,
                                   wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            }
            break;

        case 'I': // insert
            if ( GetWindowStyle() & wxLC_REPORT )
            {
                if ( GetWindowStyle() & wxLC_VIRTUAL )
                {
                    SetItemCount(GetItemCount() + 1);
                }
                else // !virtual
                {
                    InsertItemInReportView(event.GetIndex());
                }
            }
            //else: fall through

        default:
            LogEvent(event, wxT("OnListKeyDown"));

            event.Skip();
    }
}

void MyListCtrl::OnChar(wxKeyEvent& event)
{
    wxLogMessage(wxT("Got char event."));

    switch ( event.GetKeyCode() )
    {
        case 'n':
        case 'N':
        case 'c':
        case 'C':
        case 'r':
        case 'R':
        case 'u':
        case 'U':
        case 'd':
        case 'D':
        case 'i':
        case 'I':
            // these are the keys we process ourselves
            break;

        default:
            event.Skip();
    }
}

void MyListCtrl::OnRightClick(wxMouseEvent& event)
{
    if ( !event.ControlDown() )
    {
        event.Skip();
        return;
    }

    int flags;
    long subitem;
    long item = HitTest(event.GetPosition(), flags, &subitem);

    wxString where;
    switch ( flags )
    {
        case wxLIST_HITTEST_ABOVE: where = wxT("above"); break;
        case wxLIST_HITTEST_BELOW: where = wxT("below"); break;
        case wxLIST_HITTEST_NOWHERE: where = wxT("nowhere near"); break;
        case wxLIST_HITTEST_ONITEMICON: where = wxT("on icon of"); break;
        case wxLIST_HITTEST_ONITEMLABEL: where = wxT("on label of"); break;
        case wxLIST_HITTEST_ONITEMRIGHT: where = wxT("right on"); break;
        case wxLIST_HITTEST_TOLEFT: where = wxT("to the left of"); break;
        case wxLIST_HITTEST_TORIGHT: where = wxT("to the right of"); break;
        default: where = wxT("not clear exactly where on"); break;
    }

    wxLogMessage(wxT("Right double click %s item %ld, subitem %ld"),
                 where.c_str(), item, subitem);
}

void MyListCtrl::LogEvent(const wxListEvent& event, const wxChar *eventName)
{
    wxLogMessage(wxT("Item %ld: %s (item text = %s, data = %ld)"),
                 event.GetIndex(), eventName,
                 event.GetText().c_str(), event.GetData());
}

wxString MyListCtrl::OnGetItemText(long item, long column) const
{
    if ( GetItemCount() == WXSIZEOF(SMALL_VIRTUAL_VIEW_ITEMS) )
    {
        return SMALL_VIRTUAL_VIEW_ITEMS[item][column];
    }
    else // "big" virtual control
    {
        return wxString::Format(wxT("Column %ld of item %ld"), column, item);
    }
}

int MyListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if (!column)
        return 0;

    if (!(item % 3) && column == 1)
        return 0;

    return -1;
}

wxListItemAttr *MyListCtrl::OnGetItemAttr(long item) const
{
    // test to check that RefreshItem() works correctly: when m_updated is
    // set to some item and it is refreshed, we highlight the item
    if ( item == m_updated )
    {
        static wxListItemAttr s_attrHighlight(*wxRED, wxNullColour, wxNullFont);
        return &s_attrHighlight;
    }

    return item % 2 ? NULL : (wxListItemAttr *)&m_attr;
}

void MyListCtrl::InsertItemInReportView(int i)
{
    wxString buf;
    buf.Printf(wxT("This is item %d"), i);
    long tmp = InsertItem(i, buf, 0);
    SetItemData(tmp, i);

    buf.Printf(wxT("Col 1, item %d"), i);
    SetItem(tmp, 1, buf);

    buf.Printf(wxT("Item %d in column 2"), i);
    SetItem(tmp, 2, buf);
}

#if USE_CONTEXT_MENU
void MyListCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    if (GetEditControl() == NULL)
    {
        wxPoint point = event.GetPosition();
        // If from keyboard
        if ( (point.x == -1) && (point.y == -1) )
        {
            wxSize size = GetSize();
            point.x = size.x / 2;
            point.y = size.y / 2;
        }
        else
        {
            point = ScreenToClient(point);
        }
        ShowContextMenu(point);
    }
    else
    {
        // the user is editing:
        // allow the text control to display its context menu
        // if it has one (it has on Windows) rather than display our one
        event.Skip();
    }
}
#endif

void MyListCtrl::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;

    menu.Append(wxID_ABOUT, wxT("&About"));
    menu.AppendSeparator();
    menu.Append(wxID_EXIT, wxT("E&xit"));

    PopupMenu(&menu, pos.x, pos.y);
}
