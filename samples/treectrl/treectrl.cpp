/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// important: the #pragma argument must be different from treectrl.cpp,
// otherwise gcc gets confused (as there is also treectrl.cpp in the library
// which has identical #pragma) and the sample crashes on startup!
#ifdef __GNUG__
  #pragma interface "treetest.cpp"
  #pragma implementation "treetest.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/log.h"

#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"

#include "math.h"

#ifdef __WIN32__
    // this is not supported by native control
    #define NO_VARIABLE_HEIGHT
#endif

#include "treectrl.h"

// under Windows the icons are in the .rc file
#ifndef __WXMSW__
    #include "icon1.xpm"
    #include "icon2.xpm"
    #include "icon3.xpm"
    #include "icon4.xpm"
    #include "icon5.xpm"
    #include "mondrian.xpm"
#endif

// verify that the item is ok and insult the user if it is not
#define CHECK_ITEM( item ) if ( !item.IsOk() ) {                                 \
                             wxMessageBox(wxT("Please select some item first!"), \
                                          wxT("Tree sample error"),              \
                                          wxOK | wxICON_EXCLAMATION,             \
                                          this);                                 \
                             return;                                             \
                           }

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnSize)

#define MENU_LINK(name) EVT_MENU(TreeTest_##name, MyFrame::On##name)
    MENU_LINK(Quit)
    MENU_LINK(About)
    MENU_LINK(TogButtons)
    MENU_LINK(TogTwist)
    MENU_LINK(TogLines)
    MENU_LINK(TogEdit)
    MENU_LINK(TogHideRoot)
    MENU_LINK(TogRootLines)
    MENU_LINK(TogBorder)
    MENU_LINK(TogFullHighlight)
    MENU_LINK(Dump)
#ifndef NO_MULTIPLE_SELECTION
    MENU_LINK(DumpSelected)
    MENU_LINK(Select)
    MENU_LINK(Unselect)
    MENU_LINK(ToggleSel)
#endif // NO_MULTIPLE_SELECTION
    MENU_LINK(Rename)
    MENU_LINK(Count)
    MENU_LINK(CountRec)
    MENU_LINK(Sort)
    MENU_LINK(SortRev)
    MENU_LINK(SetBold)
    MENU_LINK(ClearBold)
    MENU_LINK(Delete)
    MENU_LINK(DeleteChildren)
    MENU_LINK(DeleteAll)
    MENU_LINK(Recreate)
    MENU_LINK(ToggleImages)
    MENU_LINK(ToggleButtons)
    MENU_LINK(SetImageSize)
    MENU_LINK(CollapseAndReset)
    MENU_LINK(EnsureVisible)
    MENU_LINK(AddItem)
    MENU_LINK(InsertItem)
    MENU_LINK(IncIndent)
    MENU_LINK(DecIndent)
    MENU_LINK(IncSpacing)
    MENU_LINK(DecSpacing)
    MENU_LINK(ToggleIcon)
#undef MENU_LINK

END_EVENT_TABLE()

#if USE_GENERIC_TREECTRL
BEGIN_EVENT_TABLE(MyTreeCtrl, wxGenericTreeCtrl)
#else
BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
#endif
    EVT_TREE_BEGIN_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginRDrag)
    EVT_TREE_END_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnEndDrag)
    EVT_TREE_BEGIN_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnEndLabelEdit)
    EVT_TREE_DELETE_ITEM(TreeTest_Ctrl, MyTreeCtrl::OnDeleteItem)
#if 0       // there are so many of those that logging them causes flicker
    EVT_TREE_GET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnGetInfo)
#endif
    EVT_TREE_SET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnSetInfo)
    EVT_TREE_ITEM_EXPANDED(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanded)
    EVT_TREE_ITEM_EXPANDING(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanding)
    EVT_TREE_ITEM_COLLAPSED(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsed)
    EVT_TREE_ITEM_COLLAPSING(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsing)
    EVT_TREE_ITEM_RIGHT_CLICK(TreeTest_Ctrl, MyTreeCtrl::OnItemRightClick)

    EVT_RIGHT_UP(MyTreeCtrl::OnRMouseUp)
    EVT_TREE_SEL_CHANGED(TreeTest_Ctrl, MyTreeCtrl::OnSelChanged)
    EVT_TREE_SEL_CHANGING(TreeTest_Ctrl, MyTreeCtrl::OnSelChanging)
    EVT_TREE_KEY_DOWN(TreeTest_Ctrl, MyTreeCtrl::OnTreeKeyDown)
    EVT_TREE_ITEM_ACTIVATED(TreeTest_Ctrl, MyTreeCtrl::OnItemActivated)
    EVT_RIGHT_DCLICK(MyTreeCtrl::OnRMouseDClick)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame *frame = new MyFrame(wxT("wxTreeCtrl Test"), 50, 50, 450, 600);

    // Show the frame
    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
}


// My frame constructor
MyFrame::MyFrame(const wxString& title, int x, int y, int w, int h)
       : wxFrame((wxFrame *)NULL, -1, title, wxPoint(x, y), wxSize(w, h)),
         m_treeCtrl(NULL), m_textCtrl(NULL)
{
    // This reduces flicker effects - even better would be to define
    // OnEraseBackground to do nothing. When the tree control's scrollbars are
    // show or hidden, the frame is sent a background erase event.
    SetBackgroundColour(wxColour(255, 255, 255));

    // Give it an icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // Make a menubar
    wxMenu *file_menu = new wxMenu,
           *style_menu = new wxMenu,
           *tree_menu = new wxMenu,
           *item_menu = new wxMenu;

    file_menu->Append(TreeTest_About, wxT("&About..."));
    file_menu->AppendSeparator();
    file_menu->Append(TreeTest_Quit, wxT("E&xit\tAlt-X"));

    style_menu->Append(TreeTest_TogButtons, "Toggle &normal buttons");
    style_menu->Append(TreeTest_TogTwist, "Toggle &twister buttons");
    style_menu->Append(TreeTest_ToggleButtons, "Toggle image &buttons");
    style_menu->AppendSeparator();
    style_menu->Append(TreeTest_TogLines, "Toggle &connecting lines");
    style_menu->Append(TreeTest_TogRootLines, "Toggle &lines at root");
    style_menu->Append(TreeTest_TogHideRoot, "Toggle &hidden root");
    style_menu->Append(TreeTest_TogBorder, "Toggle &item border");
    style_menu->Append(TreeTest_TogFullHighlight, "Toggle &full row highlight");
    style_menu->Append(TreeTest_TogEdit, "Toggle &edit mode");
#ifndef NO_MULTIPLE_SELECTION
    style_menu->Append(TreeTest_ToggleSel, wxT("Toggle &selection mode"));
#endif // NO_MULTIPLE_SELECTION
    style_menu->Append(TreeTest_ToggleImages, wxT("Toggle show ima&ges"));
    style_menu->Append(TreeTest_SetImageSize, wxT("Set image si&ze..."));

    tree_menu->Append(TreeTest_Recreate, "&Recreate the tree");
    tree_menu->Append(TreeTest_CollapseAndReset, "C&ollapse and reset");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_AddItem, "Append a &new item");
    tree_menu->Append(TreeTest_InsertItem, "&Insert a new item");
    tree_menu->Append(TreeTest_Delete, "&Delete this item");
    tree_menu->Append(TreeTest_DeleteChildren, "Delete &children");
    tree_menu->Append(TreeTest_DeleteAll, "Delete &all items");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_Count, "Count children of current item");
    tree_menu->Append(TreeTest_CountRec, "Recursively count children of current item");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_Sort, "Sort children of current item");
    tree_menu->Append(TreeTest_SortRev, "Sort in reversed order");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_EnsureVisible, "Make the last item &visible");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_IncIndent, "Add 5 points to indentation\tAlt-I");
    tree_menu->Append(TreeTest_DecIndent, "Reduce indentation by 5 points\tAlt-R");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_IncSpacing, "Add 5 points to spacing\tCtrl-I");
    tree_menu->Append(TreeTest_DecSpacing, "Reduce spacing by 5 points\tCtrl-R");

    item_menu->Append(TreeTest_Dump, "&Dump item children");
    item_menu->Append(TreeTest_Rename, "&Rename item...");

    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_SetBold, "Make item &bold");
    item_menu->Append(TreeTest_ClearBold, "Make item &not bold");
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_ToggleIcon, "Toggle the item's &icon");

#ifndef NO_MULTIPLE_SELECTION
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_DumpSelected, "Dump selected items\tAlt-D");
    item_menu->Append(TreeTest_Select, "Select current item\tAlt-S");
    item_menu->Append(TreeTest_Unselect, "Unselect everything\tAlt-U");
#endif // NO_MULTIPLE_SELECTION

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(style_menu, "&Style");
    menu_bar->Append(tree_menu, "&Tree");
    menu_bar->Append(item_menu, "&Item");
    SetMenuBar(menu_bar);

    //menu_bar->Check(TreeTest_ToggleImages, TRUE);
#endif // wxUSE_MENUS

    m_treeCtrl = new MyTreeCtrl(this, TreeTest_Ctrl,
                                wxDefaultPosition, wxDefaultSize,
                                wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS |
#ifndef NO_VARIABLE_HEIGHT
                                wxTR_HAS_VARIABLE_ROW_HEIGHT |
#endif
                                wxSUNKEN_BORDER);

//    m_treeCtrl->SetBackgroundColour( *wxLIGHT_GREY );

    m_textCtrl = new wxTextCtrl(this, -1, "",
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxSUNKEN_BORDER);

    // create a status bar with 3 panes
    CreateStatusBar(3);
    SetStatusText("", 0);

#ifdef __WXMOTIF__
    // For some reason, we get a memcpy crash in wxLogStream::DoLogStream
    // on gcc/wxMotif, if we use wxLogTextCtl. Maybe it's just gcc?
    delete wxLog::SetActiveTarget(new wxLogStderr);
#else
    // set our text control as the log target
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(m_textCtrl);
    delete wxLog::SetActiveTarget(logWindow);
#endif
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(NULL);
}

void MyFrame::TogStyle(long flag)
{
        m_treeCtrl->SetWindowStyle(m_treeCtrl->GetWindowStyle() ^ flag);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    if ( m_treeCtrl && m_textCtrl )
    {
        Resize(GetClientSize());
    }

    event.Skip();
}

void MyFrame::Resize(const wxSize& size)
{
    m_treeCtrl->SetSize(0, 0, size.x, 2*size.y/3);
    m_textCtrl->SetSize(0, 2*size.y/3, size.x, size.y/3);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Tree test sample\n"
                 "(c) Julian Smart 1997, Vadim Zeitlin 1998",
                 "About tree test",
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnRename(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    // old code - now we edit in place
#if 0
    static wxString s_text;
    s_text = wxGetTextFromUser("New name: ", "Tree sample question",
            s_text, this);
    if ( !s_text.IsEmpty() )
    {
        m_treeCtrl->SetItemText(item, s_text);
    }
#endif // 0

    // TODO demonstrate creating a custom edit control...
    (void)m_treeCtrl->EditLabel(item);
}

void MyFrame::OnCount(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    int i = m_treeCtrl->GetChildrenCount( item, FALSE );

    wxLogMessage(wxT("%d children"), i);
}

void MyFrame::OnCountRec(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    int i = m_treeCtrl->GetChildrenCount( item );

    wxLogMessage(wxT("%d children"), i);
}

void MyFrame::DoSort(bool reverse)
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    m_treeCtrl->DoSortChildren(item, reverse);
}

void MyFrame::OnDump(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId root = m_treeCtrl->GetSelection();

    CHECK_ITEM( root );

    m_treeCtrl->GetItemsRecursively(root, -1);
}

#ifndef NO_MULTIPLE_SELECTION

void MyFrame::OnToggleSel(wxCommandEvent& WXUNUSED(event))
{
    TogStyle(wxTR_MULTIPLE);
#if 0
    long style = m_treeCtrl->GetWindowStyle();
    if ( style & wxTR_MULTIPLE )
        style &= ~wxTR_MULTIPLE;
    else
        style |= wxTR_MULTIPLE;

    delete m_treeCtrl;

    m_treeCtrl = new MyTreeCtrl(this, TreeTest_Ctrl,
                                wxDefaultPosition, wxDefaultSize,
                                style);
    Resize(GetClientSize());
#endif
}

void MyFrame::OnDumpSelected(wxCommandEvent& WXUNUSED(event))
{
    wxArrayTreeItemIds array;

    size_t count = m_treeCtrl->GetSelections(array);
    wxLogMessage(wxT("%u items selected"), count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxLogMessage(wxT("\t%s"), m_treeCtrl->GetItemText(array.Item(n)).c_str());
    }
}

void MyFrame::OnSelect(wxCommandEvent& event)
{
    m_treeCtrl->SelectItem(m_treeCtrl->GetSelection());
}

void MyFrame::OnUnselect(wxCommandEvent& event)
{
    m_treeCtrl->UnselectAll();
}

#endif // NO_MULTIPLE_SELECTION

void MyFrame::DoSetBold(bool bold)
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    m_treeCtrl->SetItemBold(item, bold);
}

void MyFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    m_treeCtrl->Delete(item);
}

void MyFrame::OnDeleteChildren(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    m_treeCtrl->DeleteChildren(item);
}

void MyFrame::OnDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->DeleteAllItems();
}

void MyFrame::OnRecreate(wxCommandEvent& event)
{
    OnDeleteAll(event);
    m_treeCtrl->AddTestItemsToTree(5, 2);
}

void MyFrame::OnSetImageSize(wxCommandEvent& event)
{
    int size = wxGetNumberFromUser("Enter the size for the images to use",
                                    "Size: ",
                                    "TreeCtrl sample",
                                    m_treeCtrl->ImageSize());
    if ( size == -1 )
        return;

    m_treeCtrl->CreateImageList(size);
    wxGetApp().SetShowImages(TRUE);
}

void MyFrame::OnToggleImages(wxCommandEvent& event)
{
    if ( wxGetApp().ShowImages() )
    {
        m_treeCtrl->CreateImageList(-1);
        wxGetApp().SetShowImages(FALSE);
    }
    else
    {
        m_treeCtrl->CreateImageList(0);
        wxGetApp().SetShowImages(TRUE);
    }
}

void MyFrame::OnToggleButtons(wxCommandEvent& event)
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if ( wxGetApp().ShowButtons() )
    {
        m_treeCtrl->CreateButtonsImageList(-1);
        wxGetApp().SetShowButtons(FALSE);
    }
    else
    {
        m_treeCtrl->CreateButtonsImageList(15);
        wxGetApp().SetShowButtons(TRUE);
    }
#endif
}

void MyFrame::OnCollapseAndReset(wxCommandEvent& event)
{
    m_treeCtrl->CollapseAndReset(m_treeCtrl->GetRootItem());
}

void MyFrame::OnEnsureVisible(wxCommandEvent& event)
{
    m_treeCtrl->DoEnsureVisible();
}

void MyFrame::OnInsertItem(wxCommandEvent& WXUNUSED(event))
{
    int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_File : -1;
    m_treeCtrl->InsertItem(m_treeCtrl->GetRootItem(), image, "2nd item");
}

void MyFrame::OnAddItem(wxCommandEvent& WXUNUSED(event))
{
    static int s_num = 0;

    wxString text;
    text.Printf(wxT("Item #%d"), ++s_num);

    m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(),
                           text /*,
                           MyTreeCtrl::TreeCtrlIcon_File */ );
}

void MyFrame::OnIncIndent(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetIndent();
    if (indent < 100)
        m_treeCtrl->SetIndent( indent+5 );
}

void MyFrame::OnDecIndent(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetIndent();
    if (indent > 10)
        m_treeCtrl->SetIndent( indent-5 );
}

void MyFrame::OnIncSpacing(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetSpacing();
    if (indent < 100)
        m_treeCtrl->SetSpacing( indent+5 );
}

void MyFrame::OnDecSpacing(wxCommandEvent& WXUNUSED(event))
{
    unsigned int indent = m_treeCtrl->GetSpacing();
    if (indent > 10)
        m_treeCtrl->SetSpacing( indent-5 );
}

void MyFrame::OnToggleIcon(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = m_treeCtrl->GetSelection();

    CHECK_ITEM( item );

    m_treeCtrl->DoToggleIcon(item);
}

// MyTreeCtrl implementation
#if USE_GENERIC_TREECTRL
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxGenericTreeCtrl)
#else
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxTreeCtrl)
#endif

MyTreeCtrl::MyTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style)
          : wxTreeCtrl(parent, id, pos, size, style)
{
    m_reverseSort = FALSE;

    CreateImageList();

    // Add some items to the tree
    AddTestItemsToTree(5, 2);
}

void MyTreeCtrl::CreateImageList(int size)
{
    if ( size == -1 )
    {
        SetImageList(NULL);
        return;
    }
    if ( size == 0 )
        size = m_imageSize;
    else
        m_imageSize = size;

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, TRUE);

    // should correspond to TreeCtrlIcon_xxx enum
#if defined(__WXMSW__) && defined(__WIN16__)
    images->Add(wxBitmap("bitmap1", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap2", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap3", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap4", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap5", wxBITMAP_TYPE_BMP_RESOURCE));
#else // !MSW
    wxBusyCursor wait;
    wxIcon icons[5];
    icons[0] = wxICON(icon1);
    icons[1] = wxICON(icon2);
    icons[2] = wxICON(icon3);
    icons[3] = wxICON(icon4);
    icons[4] = wxICON(icon5);

    int sizeOrig = icons[0].GetWidth();
    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxImage(icons[i]).Rescale(size, size).
                                    ConvertToBitmap());
        }
    }
#endif // MSW/!MSW

    AssignImageList(images);
}

void MyTreeCtrl::CreateButtonsImageList(int size)
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if ( size == -1 )
    {
        SetButtonsImageList(NULL);
        return;
    }

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, TRUE);

    // should correspond to TreeCtrlIcon_xxx enum
#if defined(__WXMSW__) && defined(__WIN16__)
    images->Add(wxBitmap("bitmap1", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap2", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap3", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap4", wxBITMAP_TYPE_BMP_RESOURCE));
    images->Add(wxBitmap("bitmap5", wxBITMAP_TYPE_BMP_RESOURCE));
#else // !MSW
    wxBusyCursor wait;
    wxIcon icons[4];
    icons[0] = wxICON(icon3);   // closed
    icons[1] = wxICON(icon3);   // closed, selected
    icons[2] = wxICON(icon5);   // open
    icons[3] = wxICON(icon5);   // open, selected

    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        int sizeOrig = icons[i].GetWidth();
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxImage(icons[i]).Rescale(size, size).
                                    ConvertToBitmap());
        }
    }
#endif // MSW/!MSW

    AssignButtonsImageList(images);
#endif
}

MyTreeCtrl::~MyTreeCtrl()
{
}

int MyTreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    if ( m_reverseSort )
    {
        // just exchange 1st and 2nd items
        return wxTreeCtrl::OnCompareItems(item2, item1);
    }
    else
    {
        return wxTreeCtrl::OnCompareItems(item1, item2);
    }
}

void MyTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent,
                                     size_t numChildren,
                                     size_t depth,
                                     size_t folder)
{
    if ( depth > 0 )
    {
        bool hasChildren = depth > 1;

        wxString str;
        for ( size_t n = 0; n < numChildren; n++ )
        {
            // at depth 1 elements won't have any more children
            if ( hasChildren )
                str.Printf(wxT("%s child %d"), wxT("Folder"), n + 1);
            else
                str.Printf(wxT("%s child %d.%d"), wxT("File"), folder, n + 1);

            // here we pass to AppendItem() normal and selected item images (we
            // suppose that selected image follows the normal one in the enum)
            int image, imageSel;
            if ( wxGetApp().ShowImages() )
            {
                image = depth == 1 ? TreeCtrlIcon_File : TreeCtrlIcon_Folder;
                imageSel = image + 1;
            }
            else
            {
                image = imageSel = -1;
            }
            wxTreeItemId id = AppendItem(idParent, str, image, imageSel,
                                         new MyTreeItemData(str));

            // and now we also set the expanded one (only for the folders)
            if ( hasChildren && wxGetApp().ShowImages() )
            {
                SetItemImage(id, TreeCtrlIcon_FolderOpened,
                             wxTreeItemIcon_Expanded);
            }

            // remember the last child for OnEnsureVisible()
            if ( !hasChildren && n == numChildren - 1 )
            {
                m_lastItem = id;
            }

            AddItemsRecursively(id, numChildren, depth - 1, n + 1);
        }
    }
    //else: done!
}

void MyTreeCtrl::AddTestItemsToTree(size_t numChildren,
                                    size_t depth)
{
    int image = wxGetApp().ShowImages() ? MyTreeCtrl::TreeCtrlIcon_Folder : -1;
    wxTreeItemId rootId = AddRoot("Root",
                                  image, image,
                                  new MyTreeItemData("Root item"));
    if ( image != -1 )
    {
        SetItemImage(rootId, TreeCtrlIcon_FolderOpened, wxTreeItemIcon_Expanded);
    }

    AddItemsRecursively(rootId, numChildren, depth, 0);

    // set some colours/fonts for testing
    SetItemFont(rootId, *wxITALIC_FONT);

    long cookie;
    wxTreeItemId id = GetFirstChild(rootId, cookie);
    SetItemTextColour(id, *wxBLUE);

    id = GetNextChild(rootId, cookie);
    id = GetNextChild(rootId, cookie);
    SetItemTextColour(id, *wxRED);
    SetItemBackgroundColour(id, *wxLIGHT_GREY);
}

void MyTreeCtrl::GetItemsRecursively(const wxTreeItemId& idParent, long cookie)
{
    wxTreeItemId id;

    if( cookie == -1 )
        id = GetFirstChild(idParent, cookie);
    else
        id = GetNextChild(idParent, cookie);

    if(id <= 0)
        return;

    wxString text = GetItemText(id);
    wxLogMessage(text);

    if (ItemHasChildren(id))
        GetItemsRecursively(id,-1);

    GetItemsRecursively(idParent, cookie);
}

void MyTreeCtrl::DoToggleIcon(const wxTreeItemId& item)
{
    int image = GetItemImage(item) == TreeCtrlIcon_Folder ? TreeCtrlIcon_File
                                                          : TreeCtrlIcon_Folder;

    SetItemImage(item, image);
}


// avoid repetition
#define TREE_EVENT_HANDLER(name)                                 \
void MyTreeCtrl::name(wxTreeEvent& event)                        \
{                                                                \
    wxLogMessage(wxT(#name));                                    \
    event.Skip();                                                \
}

TREE_EVENT_HANDLER(OnBeginRDrag)
TREE_EVENT_HANDLER(OnDeleteItem)
TREE_EVENT_HANDLER(OnGetInfo)
TREE_EVENT_HANDLER(OnSetInfo)
TREE_EVENT_HANDLER(OnItemExpanded)
TREE_EVENT_HANDLER(OnItemExpanding)
TREE_EVENT_HANDLER(OnItemCollapsed)
TREE_EVENT_HANDLER(OnSelChanged)
TREE_EVENT_HANDLER(OnSelChanging)
TREE_EVENT_HANDLER(OnTreeKeyDown)

#undef TREE_EVENT_HANDLER

void MyTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
    // need to explicitly allow drag
    if ( event.GetItem() != GetRootItem() )
    {
        m_draggedItem = event.GetItem();

        wxLogMessage(wxT("OnBeginDrag: started dragging %s"),
                     GetItemText(m_draggedItem).c_str());

        event.Allow();
    }
    else
    {
        wxLogMessage(wxT("OnBeginDrag: this item can't be dragged."));
    }
}

void MyTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
    wxTreeItemId itemSrc = m_draggedItem,
                 itemDst = event.GetItem();
    m_draggedItem = (wxTreeItemId)0l;

    // where to copy the item?
    if ( itemDst.IsOk() && !ItemHasChildren(itemDst) )
    {
        // copy to the parent then
        itemDst = GetParent(itemDst);
    }

    if ( !itemDst.IsOk() )
    {
        wxLogMessage(wxT("OnEndDrag: can't drop here."));

        return;
    }

    wxString text = GetItemText(itemSrc);
    wxLogMessage(wxT("OnEndDrag: '%s' copied to '%s'."),
                 text.c_str(), GetItemText(itemDst).c_str());

    // just do append here - we could also insert it just before/after the item
    // on which it was dropped, but this requires slightly more work... we also
    // completely ignore the client data and icon of the old item but could
    // copy them as well.
    //
    // Finally, we only copy one item here but we might copy the entire tree if
    // we were dragging a folder.
    int image = wxGetApp().ShowImages() ? TreeCtrlIcon_File : -1;
    AppendItem(itemDst, text, image);
}

void MyTreeCtrl::OnBeginLabelEdit(wxTreeEvent& event)
{
    wxLogMessage(wxT("OnBeginLabelEdit"));

    // for testing, prevent this item's label editing
    wxTreeItemId itemId = event.GetItem();
    if ( IsTestItem(itemId) )
    {
        wxMessageBox(wxT("You can't edit this item."));

        event.Veto();
    }
}

void MyTreeCtrl::OnEndLabelEdit(wxTreeEvent& event)
{
    wxLogMessage(wxT("OnEndLabelEdit"));

    // don't allow anything except letters in the labels
    if ( !event.GetLabel().IsWord() )
    {
        wxMessageBox(wxT("The label should contain only letters."));

        event.Veto();
    }
}

void MyTreeCtrl::OnItemCollapsing(wxTreeEvent& event)
{
    wxLogMessage(wxT("OnItemCollapsing"));

    // for testing, prevent the user from collapsing the first child folder
    wxTreeItemId itemId = event.GetItem();
    if ( IsTestItem(itemId) )
    {
        wxMessageBox(wxT("You can't collapse this item."));

        event.Veto();
    }
}

void MyTreeCtrl::OnItemActivated(wxTreeEvent& event)
{
    // show some info about this item
    wxTreeItemId itemId = event.GetItem();
    MyTreeItemData *item = (MyTreeItemData *)GetItemData(itemId);

    if ( item != NULL )
    {
        item->ShowInfo(this);
    }

    wxLogMessage(wxT("OnItemActivated"));
}

void MyTreeCtrl::OnItemRightClick(wxTreeEvent& event)
{
    ShowMenu(event.GetItem(), event.GetPoint());
}

void MyTreeCtrl::OnRMouseUp(wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();
    ShowMenu(HitTest(pt), pt);
}

void MyTreeCtrl::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    wxString title;
    if ( id.IsOk() )
    {
        title << _T("Menu for ") << GetItemText(id);
    }
    else
    {
        title = _T("Menu for no particular item");
    }

#if wxUSE_MENUS
    wxMenu menu(title);
    menu.Append(TreeTest_About, _T("&About..."));
    menu.Append(TreeTest_Dump, _T("&Dump"));

    PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}

void MyTreeCtrl::OnRMouseDClick(wxMouseEvent& event)
{
    wxTreeItemId id = HitTest(event.GetPosition());
    if ( !id )
        wxLogMessage(wxT("No item under mouse"));
    else
    {
        MyTreeItemData *item = (MyTreeItemData *)GetItemData(id);
        if ( item )
            wxLogMessage(wxT("Item '%s' under mouse"), item->GetDesc());
    }
}

static inline const wxChar *Bool2String(bool b)
{
    return b ? wxT("") : wxT("not ");
}

void MyTreeItemData::ShowInfo(wxTreeCtrl *tree)
{
    wxLogMessage(wxT("Item '%s': %sselected, %sexpanded, %sbold,\n")
                 wxT("%u children (%u immediately under this item)."),
                 m_desc.c_str(),
                 Bool2String(tree->IsSelected(GetId())),
                 Bool2String(tree->IsExpanded(GetId())),
                 Bool2String(tree->IsBold(GetId())),
                 tree->GetChildrenCount(GetId()),
                 tree->GetChildrenCount(GetId(), FALSE));
}
