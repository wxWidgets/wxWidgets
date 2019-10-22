///////////////////////////////////////////////////////////////////////////////
// Name:        samples/treelist/treelist.cpp
// Purpose:     Sample showing wxTreeListCtrl.
// Author:      Vadim Zeitlin
// Created:     2011-08-19
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_TREELISTCTRL
    #error "wxUSE_TREELISTCTRL must be 1 for this sample."
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/sizer.h"
    #include "wx/statusbr.h"
    #include "wx/textctrl.h"
#endif

#include "wx/treelist.h"

#include "wx/aboutdlg.h"
#include "wx/artprov.h"

// ----------------------------------------------------------------------------
// Resources
// ----------------------------------------------------------------------------

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------

// Menu items.
enum
{
    Id_MultiSelect = 100,
    Id_FlatList,

    Id_Checkboxes_Start,
    Id_NoCheckboxes = Id_Checkboxes_Start,
    Id_Checkboxes2State,
    Id_Checkboxes3State,
    Id_CheckboxesUser3State,
    Id_Checkboxes_End,

    Id_DeleteAllItems,

    Id_DumpSelection,
    Id_Check_HTMLDocs,
    Id_Uncheck_HTMLDocs,
    Id_Indet_HTMLDocs,
    Id_Select_HTMLDocs
};

// Tree list columns.
enum
{
    Col_Component,
    Col_Files,
    Col_Size
};

// ----------------------------------------------------------------------------
// Custom comparator for tree list items comparison
// ----------------------------------------------------------------------------

// This is a toy class as in a real program you would have the original numeric
// data somewhere and wouldn't need to parse it back from strings presumably.
// Nevertheless it shows how to implement a custom comparator which is needed
// if you want to sort by a column with non-textual contents.
class MyComparator : public wxTreeListItemComparator
{
public:
    virtual int
    Compare(wxTreeListCtrl* treelist,
            unsigned column,
            wxTreeListItem item1,
            wxTreeListItem item2) wxOVERRIDE
    {
        wxString text1 = treelist->GetItemText(item1, column),
                 text2 = treelist->GetItemText(item2, column);

        switch ( column )
        {
            case Col_Component:
                // Simple alphabetical comparison is fine for those.
                return text1.CmpNoCase(text2);

            case Col_Files:
                // Compare strings as numbers.
                return GetNumFilesFromText(text1) - GetNumFilesFromText(text2);

            case Col_Size:
                // Compare strings as numbers but also take care of "KiB" and
                // "MiB" suffixes.
                return GetSizeFromText(text1) - GetSizeFromText(text2);
        }

        wxFAIL_MSG( "Sorting on unknown column?" );

        return 0;
    }

private:
    // Return the number of files handling special value "many". Notice that
    // the returned value is signed to allow using it in subtraction above.
    int GetNumFilesFromText(const wxString& text) const
    {
        unsigned long n;
        if ( !text.ToULong(&n) )
        {
            if ( text == "many" )
                n = 9999;
            else
                n = 0;
        }

        return n;
    }

    // Return the size in KiB from a string with either KiB or MiB suffix.
    int GetSizeFromText(const wxString& text) const
    {
        wxString size;
        unsigned factor = 1;
        if ( text.EndsWith(" MiB", &size) )
            factor = 1024;
        else if ( !text.EndsWith(" KiB", &size) )
            return 0;

        unsigned long n = 0;
        size.ToULong(&n);

        return n*factor;
    }
};

// ----------------------------------------------------------------------------
// Application class
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

// ----------------------------------------------------------------------------
// Main window class
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame();
    virtual ~MyFrame();

private:
    // Event handlers for the menu and wxTreeListCtrl events.
    void OnMultiSelect(wxCommandEvent& event);
    void OnFlatList(wxCommandEvent& event);
    void OnCheckboxes(wxCommandEvent& event);
    void OnDumpSelection(wxCommandEvent& event);
    void OnCheckHTMLDocs(wxCommandEvent& event);
    void OnSelectHTMLDocs(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    void OnDeleteAllItems(wxCommandEvent& event);

    void OnSelectionChanged(wxTreeListEvent& event);
    void OnItemExpanding(wxTreeListEvent& event);
    void OnItemExpanded(wxTreeListEvent& event);
    void OnItemChecked(wxTreeListEvent& event);
    void OnItemActivated(wxTreeListEvent& event);
    void OnItemContextMenu(wxTreeListEvent& event);


    enum
    {
        Icon_File,
        Icon_FolderClosed,
        Icon_FolderOpened
    };

    // Create the image list, called once only. Should add images to it in the
    // same order as they appear in the enum above.
    void InitImageList();

    // Create the control with the given styles.
    wxTreeListCtrl* CreateTreeListCtrl(long style);

    // Recreate an already existing control.
    void RecreateTreeListCtrl(long style);

    // Helper: return the text of the item or "NONE" if the item is invalid.
    wxString DumpItem(wxTreeListItem item) const;

    // Another helper: just translate wxCheckBoxState to user-readable text.
    static const char* CheckedStateString(wxCheckBoxState state);


    wxImageList* m_imageList;

    wxTreeListCtrl* m_treelist;

    MyComparator m_comparator;

    wxTreeListItem m_itemHTMLDocs;

    wxLog* m_oldLogTarget;

    bool m_isFlat;

    wxDECLARE_EVENT_TABLE();
};

// ============================================================================
// Implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Application class
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    new MyFrame;

    return true;
}

// ----------------------------------------------------------------------------
// Main window class
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Id_MultiSelect, MyFrame::OnMultiSelect)
    EVT_MENU(Id_FlatList, MyFrame::OnFlatList)
    EVT_MENU_RANGE(Id_Checkboxes_Start, Id_Checkboxes_End,
                   MyFrame::OnCheckboxes)

    EVT_MENU(Id_DumpSelection, MyFrame::OnDumpSelection)
    EVT_MENU_RANGE(Id_Check_HTMLDocs, Id_Indet_HTMLDocs,
                   MyFrame::OnCheckHTMLDocs)
    EVT_MENU(Id_Select_HTMLDocs, MyFrame::OnSelectHTMLDocs)

    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)

    EVT_MENU(Id_DeleteAllItems, MyFrame::OnDeleteAllItems)

    EVT_TREELIST_SELECTION_CHANGED(wxID_ANY, MyFrame::OnSelectionChanged)
    EVT_TREELIST_ITEM_EXPANDING(wxID_ANY, MyFrame::OnItemExpanding)
    EVT_TREELIST_ITEM_EXPANDED(wxID_ANY, MyFrame::OnItemExpanded)
    EVT_TREELIST_ITEM_CHECKED(wxID_ANY, MyFrame::OnItemChecked)
    EVT_TREELIST_ITEM_ACTIVATED(wxID_ANY, MyFrame::OnItemActivated)
    EVT_TREELIST_ITEM_CONTEXT_MENU(wxID_ANY, MyFrame::OnItemContextMenu)
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, "wxWidgets tree/list control sample",
                 wxDefaultPosition, wxSize(600, 450))
{
    m_isFlat = false;

    // Create menus and status bar.
    SetIcon(wxICON(sample));

    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT);

    wxMenu* treeStyle = new wxMenu;
    treeStyle->AppendCheckItem(Id_MultiSelect, "&Multiple selections\tCtrl-M");
    treeStyle->AppendSeparator();
    treeStyle->AppendRadioItem(Id_NoCheckboxes,
                               "&No checkboxes\tCtrl-1");
    treeStyle->AppendRadioItem(Id_Checkboxes2State,
                               "&2-state checkboxes\tCtrl-2");
    treeStyle->AppendRadioItem(Id_Checkboxes3State,
                               "&3-state checkboxes\tCtrl-3");
    treeStyle->AppendRadioItem(Id_CheckboxesUser3State,
                               "&User-settable 3-state checkboxes\tCtrl-4");
    treeStyle->AppendSeparator();
    treeStyle->AppendCheckItem(Id_FlatList, "&Flat list");

    wxMenu* treeOper = new wxMenu;
    treeOper->Append(Id_DumpSelection, "&Dump selection\tCtrl-D");
    treeOper->AppendSeparator();
    treeOper->Append(Id_Check_HTMLDocs, "&Check Doc/HTML item\tCtrl-C");
    treeOper->Append(Id_Uncheck_HTMLDocs, "&Uncheck Doc/HTML item\tCtrl-U");
    treeOper->Append(Id_Indet_HTMLDocs, "Make Doc/HTML &indeterminate\tCtrl-I");
    treeOper->Append(Id_Select_HTMLDocs, "&Select Doc/HTML item\tCtrl-S");

    treeOper->Append(Id_DeleteAllItems, "DeleteAllItems");

    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(treeStyle, "&Style");
    menuBar->Append(treeOper, "&Operations");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

    CreateStatusBar(1);


    // Construct the image list with the standard images.
    InitImageList();


    // Create and layout child controls.
    m_treelist = CreateTreeListCtrl(wxTL_DEFAULT_STYLE);

    wxTextCtrl* textLog = new wxTextCtrl(this, wxID_ANY, "",
                                         wxDefaultPosition, wxDefaultSize,
                                         wxTE_READONLY | wxTE_MULTILINE);
    m_oldLogTarget = wxLog::SetActiveTarget(new wxLogTextCtrl(textLog));

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_treelist, wxSizerFlags(2).Expand());
    sizer->Add(textLog, wxSizerFlags(1).Expand());
    SetSizer(sizer);


    // Finally show everything.
    Show();
}

MyFrame::~MyFrame()
{
    delete m_imageList;

    delete wxLog::SetActiveTarget(m_oldLogTarget);
}

void MyFrame::InitImageList()
{
    wxSize iconSize = wxArtProvider::GetSizeHint(wxART_LIST);
    if ( iconSize == wxDefaultSize )
        iconSize = wxSize(16, 16);

    m_imageList = new wxImageList(iconSize.x, iconSize.y);

    // The order should be the same as for the enum elements.
    static const wxString icons[] =
    {
        wxART_NORMAL_FILE,
        wxART_FOLDER,
        wxART_FOLDER_OPEN
    };

    for ( unsigned n = 0; n < WXSIZEOF(icons); n++ )
    {
        m_imageList->Add
                     (
                        wxArtProvider::GetIcon(icons[n], wxART_LIST, iconSize)
                     );
    }
}

wxTreeListCtrl* MyFrame::CreateTreeListCtrl(long style)
{
    wxTreeListCtrl* const
        tree = new wxTreeListCtrl(this, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,
                                  style);
    tree->SetImageList(m_imageList);

    tree->AppendColumn("Component",
                       wxCOL_WIDTH_AUTOSIZE,
                       wxALIGN_LEFT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    tree->AppendColumn("# Files",
                       tree->WidthFor("1,000,000"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    tree->AppendColumn("Size",
                       tree->WidthFor("1,000,000 KiB"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

    // Define a shortcut to save on typing here.
    #define ADD_ITEM(item, parent, files, size) \
        wxTreeListItem item = tree->AppendItem(m_isFlat ? root : parent, \
                                               #item, \
                                               Icon_FolderClosed, \
                                               Icon_FolderOpened); \
        tree->SetItemText(item, Col_Files, files); \
        tree->SetItemText(item, Col_Size, size)

    wxTreeListItem root = tree->GetRootItem();
    ADD_ITEM(Code, root, "", "");
        ADD_ITEM(wxMSW, Code, "313", "3.94 MiB");
        ADD_ITEM(wxGTK, Code, "180", "1.66 MiB");

        ADD_ITEM(wxOSX, Code, "265", "2.36 MiB");
            ADD_ITEM(Core, wxOSX, "31", "347 KiB");
            ADD_ITEM(Carbon, wxOSX, "91", "1.34 MiB");
            ADD_ITEM(Cocoa, wxOSX, "46", "512 KiB");

    ADD_ITEM(Documentation, root, "", "");
        ADD_ITEM(HTML, Documentation, "many", "");
        ADD_ITEM(CHM, Documentation, "1", "");

    ADD_ITEM(Samples, root, "", "");
        ADD_ITEM(minimal, Samples, "1", "7 KiB");
        ADD_ITEM(widgets, Samples, "28", "419 KiB");

    #undef ADD_ITEM

    // Remember this one for subsequent tests.
    m_itemHTMLDocs = HTML;

    // Set a custom comparator to compare strings containing numbers correctly.
    tree->SetItemComparator(&m_comparator);

    return tree;
}

void MyFrame::RecreateTreeListCtrl(long style)
{
    wxTreeListCtrl* const treelist = CreateTreeListCtrl(style);
    GetSizer()->Replace(m_treelist, treelist);

    delete m_treelist;
    m_treelist = treelist;

    Layout();
}

void MyFrame::OnMultiSelect(wxCommandEvent& event)
{
    long style = m_treelist->GetWindowStyle();

    if ( event.IsChecked() )
        style |= wxTL_MULTIPLE;
    else
        style &= ~wxTL_MULTIPLE;

    RecreateTreeListCtrl(style);
}

void MyFrame::OnFlatList(wxCommandEvent& event)
{
    m_isFlat = event.IsChecked();

    RecreateTreeListCtrl(m_treelist->GetWindowStyle());
}

void MyFrame::OnCheckboxes(wxCommandEvent& event)
{
    long style = m_treelist->GetWindowStyle();
    style &= ~(wxTL_CHECKBOX | wxTL_3STATE | wxTL_USER_3STATE);

    switch ( event.GetId() )
    {
        case Id_NoCheckboxes:
            break;

        case Id_Checkboxes2State:
            style |= wxTL_CHECKBOX;
            break;

        case Id_Checkboxes3State:
            style |= wxTL_3STATE;
            break;

        case Id_CheckboxesUser3State:
            style |= wxTL_USER_3STATE;
            break;

        default:
            wxFAIL_MSG( "Unknown checkbox style" );
            return;
    }

    RecreateTreeListCtrl(style);
}

void MyFrame::OnDumpSelection(wxCommandEvent& WXUNUSED(event))
{
    if ( m_treelist->HasFlag(wxTL_MULTIPLE) )
    {
        wxTreeListItems selections;
        const unsigned numSelected = m_treelist->GetSelections(selections);

        switch ( numSelected )
        {
            case 0:
                wxLogMessage("No items selected");
                break;

            case 1:
                wxLogMessage("Single item selected: %s",
                             DumpItem(selections[0]));
                break;

            default:
                wxLogMessage("%u items selected:", numSelected);
                for ( unsigned n = 0; n < numSelected; n++ )
                {
                    wxLogMessage("\t%s", DumpItem(selections[n]));
                }
        }
    }
    else // Single selection
    {
        wxLogMessage("Selection: %s", DumpItem(m_treelist->GetSelection()));
    }
}

void MyFrame::OnCheckHTMLDocs(wxCommandEvent& event)
{
    wxCheckBoxState state;

    switch ( event.GetId() )
    {
        case Id_Uncheck_HTMLDocs:
            state = wxCHK_UNCHECKED;
            break;

        case Id_Check_HTMLDocs:
            state = wxCHK_CHECKED;
            break;

        case Id_Indet_HTMLDocs:
            state = wxCHK_UNDETERMINED;
            break;

        default:
            wxFAIL_MSG( "Unknown check state" );
            return;
    }

    m_treelist->CheckItem(m_itemHTMLDocs, state);
}

void MyFrame::OnSelectHTMLDocs(wxCommandEvent& WXUNUSED(event))
{
    m_treelist->Select(m_itemHTMLDocs);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    info.SetDescription("wxTreeListCtrl wxWidgets sample.");
    info.SetCopyright("(C) 2011 Vadim Zeitlin <vadim@wxwidgets.org>");

    wxAboutBox(info, this);
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnDeleteAllItems(wxCommandEvent& WXUNUSED(event))
{
    m_treelist->DeleteAllItems();
}

wxString MyFrame::DumpItem(wxTreeListItem item) const
{
    return item.IsOk() ? m_treelist->GetItemText(item) : wxString("NONE");
}

/* static */
const char* MyFrame::CheckedStateString(wxCheckBoxState state)
{
    switch ( state )
    {
        case wxCHK_UNCHECKED:
            return "unchecked";

        case wxCHK_UNDETERMINED:
            return "undetermined";

        case wxCHK_CHECKED:
            return "checked";
    }

    return "invalid";
}

void MyFrame::OnSelectionChanged(wxTreeListEvent& event)
{
    const char* msg;

    if ( m_treelist->HasFlag(wxTL_MULTIPLE) )
        msg = "Selection of the \"%s\" item changed.";
    else
        msg = "Selection changed, now is \"%s\".";

    wxLogMessage(msg, DumpItem(event.GetItem()));
}

void MyFrame::OnItemExpanding(wxTreeListEvent& event)
{
    wxLogMessage("Item \"%s\" is expanding", DumpItem(event.GetItem()));
}

void MyFrame::OnItemExpanded(wxTreeListEvent& event)
{
    wxLogMessage("Item \"%s\" expanded", DumpItem(event.GetItem()));
}

void MyFrame::OnItemChecked(wxTreeListEvent& event)
{
    wxTreeListItem item = event.GetItem();

    wxLogMessage("Item \"%s\" toggled, now %s (was %s)",
                 DumpItem(item),
                 CheckedStateString(m_treelist->GetCheckedState(item)),
                 CheckedStateString(event.GetOldCheckedState()));
}

void MyFrame::OnItemActivated(wxTreeListEvent& event)
{
    wxLogMessage("Item \"%s\" activated", DumpItem(event.GetItem()));
}

void MyFrame::OnItemContextMenu(wxTreeListEvent& event)
{
    enum
    {
        Id_Check_Item,
        Id_Uncheck_Item,
        Id_Indet_Item,
        Id_Check_Recursively,
        Id_Update_Parent
    };

    wxMenu menu;
    menu.Append(Id_Check_Item, "&Check item");
    menu.Append(Id_Uncheck_Item, "&Uncheck item");
    if ( m_treelist->HasFlag(wxTL_3STATE) )
        menu.Append(Id_Indet_Item, "Make item &indeterminate");
    menu.AppendSeparator();
    menu.Append(Id_Check_Recursively, "Check &recursively");
    menu.Append(Id_Update_Parent, "Update &parent");

    const wxTreeListItem item = event.GetItem();
    switch ( m_treelist->GetPopupMenuSelectionFromUser(menu) )
    {
        case Id_Check_Item:
            m_treelist->CheckItem(item);
            break;

        case Id_Uncheck_Item:
            m_treelist->UncheckItem(item);
            break;

        case Id_Indet_Item:
            m_treelist->CheckItem(item, wxCHK_UNDETERMINED);
            break;

        case Id_Check_Recursively:
            m_treelist->CheckItemRecursively(item);
            break;

        case Id_Update_Parent:
            m_treelist->UpdateItemParentStateRecursively(item);
            break;

        default:
            wxFAIL_MSG( "Unexpected menu selection" );
            wxFALLTHROUGH;

        case wxID_NONE:
            return;
    }
}
