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

// under Windows the icons are in the .rc file
#ifndef __WXMSW__
  #include "icon1.xpm"
  #include "icon2.xpm"
  #include "mondrian.xpm"
#endif

#include "wx/log.h"

#include "wx/imaglist.h"
#include "wx/treectrl.h"

#include "treetest.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(TreeTest_Quit, MyFrame::OnQuit)
  EVT_MENU(TreeTest_About, MyFrame::OnAbout)
  EVT_MENU(TreeTest_Dump, MyFrame::OnDump)
  EVT_MENU(TreeTest_Bold, MyFrame::OnSetBold)
  EVT_MENU(TreeTest_UnBold, MyFrame::OnClearBold)
  EVT_MENU(TreeTest_Delete, MyFrame::OnDelete)
  EVT_MENU(TreeTest_DeleteAll, MyFrame::OnDeleteAll)
  EVT_MENU(TreeTest_Recreate, MyFrame::OnRecreate)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
  EVT_TREE_BEGIN_DRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginDrag)
  EVT_TREE_BEGIN_RDRAG(TreeTest_Ctrl, MyTreeCtrl::OnBeginRDrag)
  EVT_TREE_BEGIN_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnBeginLabelEdit)
  EVT_TREE_END_LABEL_EDIT(TreeTest_Ctrl, MyTreeCtrl::OnEndLabelEdit)
  EVT_TREE_DELETE_ITEM(TreeTest_Ctrl, MyTreeCtrl::OnDeleteItem)
  EVT_TREE_GET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnGetInfo)
  EVT_TREE_SET_INFO(TreeTest_Ctrl, MyTreeCtrl::OnSetInfo)
  EVT_TREE_ITEM_EXPANDED(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanded)
  EVT_TREE_ITEM_EXPANDING(TreeTest_Ctrl, MyTreeCtrl::OnItemExpanding)
  EVT_TREE_ITEM_COLLAPSED(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsed)
  EVT_TREE_ITEM_COLLAPSING(TreeTest_Ctrl, MyTreeCtrl::OnItemCollapsing)
  EVT_TREE_SEL_CHANGED(TreeTest_Ctrl, MyTreeCtrl::OnSelChanged)
  EVT_TREE_SEL_CHANGING(TreeTest_Ctrl, MyTreeCtrl::OnSelChanging)
  EVT_TREE_KEY_DOWN(TreeTest_Ctrl, MyTreeCtrl::OnTreeKeyDown)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
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
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(TreeTest_Dump, "D&ump tree items");
  file_menu->Append(TreeTest_Delete, "&Delete this item");
  file_menu->Append(TreeTest_DeleteAll, "Delete &all items");
  file_menu->Append(TreeTest_Recreate, "&Recreate the tree");
  file_menu->AppendSeparator();
  file_menu->Append(TreeTest_Bold, "Make item &bold");
  file_menu->Append(TreeTest_UnBold, "Make item &not bold");
  file_menu->AppendSeparator();
  file_menu->Append(TreeTest_About, "&About...");
  file_menu->AppendSeparator();
  file_menu->Append(TreeTest_Quit, "E&xit");

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  SetMenuBar(menu_bar);

  // Make a panel with a message
  m_treeCtrl = new MyTreeCtrl(this, TreeTest_Ctrl,
                              wxDefaultPosition, wxDefaultSize,
                              wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
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
  wxMessageDialog dialog(this,
                         "Tree test sample\n"
                         "Julian Smart (c) 1997",
                         "About tree test", wxOK);

  dialog.ShowModal();
}

void MyFrame::OnDump(wxCommandEvent& WXUNUSED(event))
{
  wxTreeItemId root = m_treeCtrl->GetSelection();
  m_treeCtrl->GetItemsRecursively(root, -1);
}

void MyFrame::DoSetBold(bool bold)
{
  m_treeCtrl->SetItemBold(m_treeCtrl->GetSelection(), bold);
}

void MyFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
  wxTreeItemId item = m_treeCtrl->GetSelection();
  m_treeCtrl->Delete(item);
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

// MyTreeCtrl implementation
MyTreeCtrl::MyTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style)
          : wxTreeCtrl(parent, id, pos, size, style)
{
  // Make an image list containing small icons
  m_imageListNormal = new wxImageList(16, 16, TRUE);

  // should correspond to TreeCtrlIcon_xxx enum
  m_imageListNormal->Add(wxICON(icon1));
  m_imageListNormal->Add(wxICON(icon2));

  SetImageList(m_imageListNormal);

  // Add some items to the tree
  AddTestItemsToTree(3, 2);
}

MyTreeCtrl::~MyTreeCtrl()
{
  delete m_imageListNormal;
}

void MyTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent,
                                     size_t numChildren,
                                     size_t depth,
                                     size_t folder)
{
  if ( depth > 0 )
  {
    wxString str;
    for ( size_t n = 0; n < numChildren; n++ )
    {
      // at depth 1 elements won't have any more children
      if (depth == 1)
        str.Printf("%s child %d.%d", "File", folder, n + 1);
      else
        str.Printf("%s child %d","Folder", n + 1);

      int image = depth == 1 ? TreeCtrlIcon_File : TreeCtrlIcon_Folder;
      wxTreeItemId id = AppendItem(idParent, str, image, image,
                                   new MyTreeItemData(str));
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

  wxString text=GetItemText(id);
  wxLogMessage(text);

  if (ItemHasChildren(id))
    GetItemsRecursively(id,-1);

  GetItemsRecursively(idParent, cookie);
}


// avoid repetition
#define TREE_EVENT_HANDLER(name)                                \
    void MyTreeCtrl::name(wxTreeEvent& WXUNUSED(event))         \
    {                                                           \
      wxLogMessage(#name);                                      \
    }

TREE_EVENT_HANDLER(OnBeginDrag)
TREE_EVENT_HANDLER(OnBeginRDrag)
TREE_EVENT_HANDLER(OnBeginLabelEdit)
TREE_EVENT_HANDLER(OnEndLabelEdit)
TREE_EVENT_HANDLER(OnDeleteItem)
TREE_EVENT_HANDLER(OnGetInfo)
TREE_EVENT_HANDLER(OnSetInfo)
TREE_EVENT_HANDLER(OnItemExpanded)
TREE_EVENT_HANDLER(OnItemExpanding)
TREE_EVENT_HANDLER(OnItemCollapsed)
TREE_EVENT_HANDLER(OnSelChanged)
TREE_EVENT_HANDLER(OnSelChanging)

#undef TREE_EVENT_HANDLER

void MyTreeCtrl::OnItemCollapsing(wxTreeEvent& event)
{
  wxLogMessage("OnItemCollapsing");

  // for testing, prevent the user from collapsing the root item
  wxTreeItemId itemId = event.GetItem();
  if ( !GetParent(itemId).IsOk() )
  {
    wxMessageBox("You can't collapse the root item.");

    event.Veto();
  }
}

void MyTreeCtrl::OnTreeKeyDown(wxTreeEvent&WXUNUSED(event))
{
  // show some info about this item
  wxTreeItemId itemId = GetSelection();
  MyTreeItemData *item = (MyTreeItemData *)GetItemData(itemId);

  if ( item != NULL )
  {
    item->ShowInfo(this);
  }

  wxLogMessage("OnTreeKeyDown");
}

static inline const char *Bool2String(bool b)
{
  return b ? "" : "not ";
}

void MyTreeItemData::ShowInfo(wxTreeCtrl */*tree*/)
{
/*
  wxLogMessage("Item '%s': %sselected, %sexpanded, %sbold,\n"
               "%u children (%u immediately under this item).",
               m_desc.c_str(),
               Bool2String(tree->IsSelected(GetId())),
               Bool2String(tree->IsExpanded(GetId())),
               Bool2String(tree->IsBold(GetId())),
               tree->GetChildrenCount(GetId()),
               tree->GetChildrenCount(GetId(), FALSE));
*/
}
