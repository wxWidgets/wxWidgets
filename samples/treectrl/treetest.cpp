/////////////////////////////////////////////////////////////////////////////
// Name:        treetest.cpp
// Purpose:     wxTreeCtrl sample
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

#include "wx/log.h"

#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"

#include "math.h"

#ifdef __WXMSW__
    #define NO_MULTIPLE_SELECTION
    #define NO_VARIABLE_HEIGHT
#endif

#include "treetest.h"

// under Windows the icons are in the .rc file
#ifndef __WXMSW__
  #include "icon1.xpm"
  #include "icon2.xpm"
  #include "icon3.xpm"
  #include "icon4.xpm"
  #include "mondrian.xpm"
#endif

// verify that the item is ok and insult the user if it is not
#define CHECK_ITEM( item ) if ( !item.IsOk() ) {                            \
                             wxMessageBox("Please select some item first!", \
                                          "Tree sample error",              \
                                          wxOK | wxICON_EXCLAMATION,        \
                                          this);                            \
                             return;                                        \
                           }

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TreeTest_Quit, MyFrame::OnQuit)
    EVT_MENU(TreeTest_About, MyFrame::OnAbout)
    EVT_MENU(TreeTest_Dump, MyFrame::OnDump)
#ifndef NO_MULTIPLE_SELECTION
    EVT_MENU(TreeTest_DumpSelected, MyFrame::OnDumpSelected)
    EVT_MENU(TreeTest_Select, MyFrame::OnSelect)
    EVT_MENU(TreeTest_Unselect, MyFrame::OnUnselect)
#endif // NO_MULTIPLE_SELECTION
    EVT_MENU(TreeTest_Rename, MyFrame::OnRename)
    EVT_MENU(TreeTest_Sort, MyFrame::OnSort)
    EVT_MENU(TreeTest_SortRev, MyFrame::OnSortRev)
    EVT_MENU(TreeTest_Bold, MyFrame::OnSetBold)
    EVT_MENU(TreeTest_UnBold, MyFrame::OnClearBold)
    EVT_MENU(TreeTest_Delete, MyFrame::OnDelete)
    EVT_MENU(TreeTest_DeleteChildren, MyFrame::OnDeleteChildren)
    EVT_MENU(TreeTest_DeleteAll, MyFrame::OnDeleteAll)
    EVT_MENU(TreeTest_Recreate, MyFrame::OnRecreate)
    EVT_MENU(TreeTest_CollapseAndReset, MyFrame::OnCollapseAndReset)
    EVT_MENU(TreeTest_EnsureVisible, MyFrame::OnEnsureVisible)
    EVT_MENU(TreeTest_AddItem, MyFrame::OnAddItem)
    EVT_MENU(TreeTest_IncIndent, MyFrame::OnIncIndent)
    EVT_MENU(TreeTest_DecIndent, MyFrame::OnDecIndent)
    EVT_MENU(TreeTest_IncSpacing, MyFrame::OnIncSpacing)
    EVT_MENU(TreeTest_DecSpacing, MyFrame::OnDecSpacing)
    EVT_MENU(TreeTest_ToggleIcon, MyFrame::OnToggleIcon)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
    EVT_TREE_BEGIN_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginRDrag)
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
    MyFrame *frame = new MyFrame("wxTreeCtrl Test", 50, 50, 450, 340);

    // Show the frame
    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
}


// My frame constructor
MyFrame::MyFrame(const wxString& title, int x, int y, int w, int h)
       : wxFrame((wxFrame *)NULL, -1, title, wxPoint(x, y), wxSize(w, h))
{
    // This reduces flicker effects - even better would be to define
    // OnEraseBackground to do nothing. When the tree control's scrollbars are
    // show or hidden, the frame is sent a background erase event.
    SetBackgroundColour(wxColour(255, 255, 255));

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu,
           *tree_menu = new wxMenu,
           *item_menu = new wxMenu;

    file_menu->Append(TreeTest_About, "&About...");
    file_menu->AppendSeparator();
    file_menu->Append(TreeTest_Quit, "E&xit");

    tree_menu->Append(TreeTest_Recreate, "&Recreate the tree");
    tree_menu->Append(TreeTest_CollapseAndReset, "C&ollapse and reset");
    tree_menu->AppendSeparator();
    tree_menu->Append(TreeTest_AddItem, "Append a &new item");
    tree_menu->Append(TreeTest_Delete, "&Delete this item");
    tree_menu->Append(TreeTest_DeleteChildren, "Delete &children");
    tree_menu->Append(TreeTest_DeleteAll, "Delete &all items");
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
    item_menu->Append(TreeTest_Bold, "Make item &bold");
    item_menu->Append(TreeTest_UnBold, "Make item &not bold");
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_ToggleIcon, "Toggle the items &icon");

#ifndef NO_MULTIPLE_SELECTION
    item_menu->AppendSeparator();
    item_menu->Append(TreeTest_DumpSelected, "Dump selected items\tAlt-D");
    item_menu->Append(TreeTest_Select, "Select current item\tAlt-S");
    item_menu->Append(TreeTest_Unselect, "Unselect everything\tAlt-U");
#endif

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(tree_menu, "&Tree");
    menu_bar->Append(item_menu, "&Item");
    SetMenuBar(menu_bar);

    m_treeCtrl = new MyTreeCtrl(this, TreeTest_Ctrl,
                                wxDefaultPosition, wxDefaultSize,
                                wxTR_HAS_BUTTONS |
                                wxTR_EDIT_LABELS |
#ifndef NO_MULTIPLE_SELECTION
                                wxTR_MULTIPLE |
#endif
#ifndef NO_VARIABLE_HEIGHT
                                wxTR_HAS_VARIABLE_ROW_HEIGHT |
#endif
                                wxSUNKEN_BORDER);
    wxTextCtrl *textCtrl = new wxTextCtrl(this, -1, "",
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxSUNKEN_BORDER);

    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->top.SameAs(this, wxTop);
    c->left.SameAs(this, wxLeft);
    c->right.SameAs(this, wxRight);
    c->height.PercentOf(this, wxHeight, 66);
    m_treeCtrl->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->top.Below(m_treeCtrl);
    c->left.SameAs(this, wxLeft);
    c->right.SameAs(this, wxRight);
    c->bottom.SameAs(this, wxBottom);
    textCtrl->SetConstraints(c);
    SetAutoLayout(TRUE);

    // create a status bar with 3 panes
    CreateStatusBar(3);
    SetStatusText("", 0);

#ifdef __WXMOTIF__
    // For some reason, we get a memcpy crash in wxLogStream::DoLogStream
    // on gcc/wxMotif, if we use wxLogTextCtl. Maybe it's just gcc?
    delete wxLog::SetActiveTarget(new wxLogStderr);
#else
    // set our text control as the log target
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(textCtrl);
    delete wxLog::SetActiveTarget(logWindow);
#endif
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(NULL);
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

void MyFrame::OnDumpSelected(wxCommandEvent& WXUNUSED(event))
{
    wxArrayTreeItemIds array;

    size_t count = m_treeCtrl->GetSelections(array);
    wxLogMessage(_T("%u items selected"), count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxLogMessage("\t%s", m_treeCtrl->GetItemText(array.Item(n)).c_str());
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
    m_treeCtrl->AddTestItemsToTree(3, 2);
}

void MyFrame::OnCollapseAndReset(wxCommandEvent& event)
{
    m_treeCtrl->CollapseAndReset(m_treeCtrl->GetRootItem());
}

void MyFrame::OnEnsureVisible(wxCommandEvent& event)
{
    m_treeCtrl->DoEnsureVisible();
}

void MyFrame::OnAddItem(wxCommandEvent& WXUNUSED(event))
{
    static int s_num = 0;

    wxString text;
    text.Printf("Item #%d", ++s_num);

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
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxTreeCtrl)

MyTreeCtrl::MyTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style)
          : wxTreeCtrl(parent, id, pos, size, style)
{
#ifndef NO_VARIABLE_HEIGHT
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage image;

    image.LoadFile(wxString("horse.jpg"), wxBITMAP_TYPE_JPEG );
#endif
#endif

    m_reverseSort = FALSE;

    // Make an image list containing small icons
    m_imageListNormal = new wxImageList(16, 16, TRUE);

    // should correspond to TreeCtrlIcon_xxx enum
#if defined(__WXMSW__) && defined(__WIN16__)
    // This is required in 16-bit Windows mode only because we can't load a specific (16x16)
    // icon image, so it comes out stretched
#  ifndef NO_VARIABLE_HEIGHT
    m_imageListNormal->Add(image.ConvertToBitmap());
#  else
    m_imageListNormal->Add(wxBitmap("bitmap1", wxBITMAP_TYPE_BMP_RESOURCE));
#  endif
    m_imageListNormal->Add(wxBitmap("bitmap2", wxBITMAP_TYPE_BMP_RESOURCE));
    m_imageListNormal->Add(wxBitmap("bitmap3", wxBITMAP_TYPE_BMP_RESOURCE));
    m_imageListNormal->Add(wxBitmap("bitmap4", wxBITMAP_TYPE_BMP_RESOURCE));
    m_imageListNormal->Add(wxBitmap("bitmap5", wxBITMAP_TYPE_BMP_RESOURCE));
#else
#  ifndef NO_VARIABLE_HEIGHT
    m_imageListNormal->Add(image.ConvertToBitmap());
#  else
    m_imageListNormal->Add(wxICON(icon1));
#  endif
    m_imageListNormal->Add(wxICON(icon2));
    m_imageListNormal->Add(wxICON(icon3));
    m_imageListNormal->Add(wxICON(icon4));
    m_imageListNormal->Add(wxICON(icon5));
#endif

    SetImageList(m_imageListNormal);

    // Add some items to the tree
    AddTestItemsToTree(3, 2);
}

MyTreeCtrl::~MyTreeCtrl()
{
    delete m_imageListNormal;
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
                str.Printf("%s child %d", "Folder", n + 1);
            else
                str.Printf("%s child %d.%d", "File", folder, n + 1);

            // here we pass to AppendItem() normal and selected item images (we
            // suppose that selected image follows the normal one in the enum)
            int image = depth == 1 ? TreeCtrlIcon_File : TreeCtrlIcon_Folder;
            wxTreeItemId id = AppendItem(idParent, str, image, image + 1,
                                         new MyTreeItemData(str));

            // and now we also set the expanded one (only for the folders)
            if ( hasChildren )
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
    wxTreeItemId rootId = AddRoot("Root",
                                  TreeCtrlIcon_Folder, TreeCtrlIcon_Folder,
                                  new MyTreeItemData("Root item"));
    SetItemImage(rootId, TreeCtrlIcon_FolderOpened, wxTreeItemIcon_Expanded);

    AddItemsRecursively(rootId, numChildren, depth, 0);
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
#define TREE_EVENT_HANDLER(name)                            \
void MyTreeCtrl::name(wxTreeEvent& WXUNUSED(event))         \
{                                                           \
    wxLogMessage(#name);                                    \
}

TREE_EVENT_HANDLER(OnBeginDrag)
TREE_EVENT_HANDLER(OnBeginRDrag)
TREE_EVENT_HANDLER(OnDeleteItem)
TREE_EVENT_HANDLER(OnGetInfo)
TREE_EVENT_HANDLER(OnSetInfo)
TREE_EVENT_HANDLER(OnItemExpanded)
TREE_EVENT_HANDLER(OnItemExpanding)
TREE_EVENT_HANDLER(OnItemCollapsed)
TREE_EVENT_HANDLER(OnSelChanged)
TREE_EVENT_HANDLER(OnSelChanging)

#undef TREE_EVENT_HANDLER

void MyTreeCtrl::OnBeginLabelEdit(wxTreeEvent& event)
{
    wxLogMessage("OnBeginLabelEdit");

    // for testing, prevent this items label editing
    wxTreeItemId itemId = event.GetItem();
    if ( IsTestItem(itemId) )
    {
        wxMessageBox("You can't edit this item.");

        event.Veto();
    }
}

void MyTreeCtrl::OnEndLabelEdit(wxTreeEvent& event)
{
    wxLogMessage("OnEndLabelEdit");

    // don't allow anything except letters in the labels
    if ( !event.GetLabel().IsWord() )
    {
        wxMessageBox("The label should contain only letters.");

        event.Veto();
    }
}

void MyTreeCtrl::OnItemCollapsing(wxTreeEvent& event)
{
    wxLogMessage("OnItemCollapsing");

    // for testing, prevent the user from collapsing the first child folder
    wxTreeItemId itemId = event.GetItem();
    if ( IsTestItem(itemId) )
    {
        wxMessageBox("You can't collapse this item.");

        event.Veto();
    }
}

void MyTreeCtrl::OnTreeKeyDown(wxTreeEvent&WXUNUSED(event))
{
    wxLogMessage("OnTreeKeyDown");
}

void MyTreeCtrl::OnItemActivated(wxTreeEvent&WXUNUSED(event))
{
    // show some info about this item
    wxTreeItemId itemId = GetSelection();
    MyTreeItemData *item = (MyTreeItemData *)GetItemData(itemId);

    if ( item != NULL )
    {
        item->ShowInfo(this);
    }

    wxLogMessage("OnItemActivated");
}

void MyTreeCtrl::OnRMouseDClick(wxMouseEvent& event)
{
    wxTreeItemId id = HitTest(event.GetPosition());
    if ( !id )
        wxLogMessage("No item under mouse");
    else
    {
        MyTreeItemData *item = (MyTreeItemData *)GetItemData(id);
        if ( item )
            wxLogMessage("Item '%s' under mouse", item->GetDesc());
    }
}

static inline const char *Bool2String(bool b)
{
    return b ? "" : "not ";
}

void MyTreeItemData::ShowInfo(wxTreeCtrl *tree)
{
    wxLogMessage("Item '%s': %sselected, %sexpanded, %sbold,\n"
                 "%u children (%u immediately under this item).",
                 m_desc.c_str(),
                 Bool2String(tree->IsSelected(GetId())),
                 Bool2String(tree->IsExpanded(GetId())),
                 Bool2String(tree->IsBold(GetId())),
                 tree->GetChildrenCount(GetId()),
                 tree->GetChildrenCount(GetId(), FALSE));
}
