///////////////////////////////////////////////////////////////////////////////
// Name:        regtest.cpp
// Purpose:     wxRegKey class demo
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifdef  __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/log.h"
#include "wx/treectrl.h"
#include "wx/msw/registry.h"
#include "wx/msw/imaglist.h"

#include "wx/tokenzr.h"

// ----------------------------------------------------------------------------
// application type
// ----------------------------------------------------------------------------
class RegApp : public wxApp
{
public:
  bool OnInit();
};

// ----------------------------------------------------------------------------
// image list with registry icons
// ----------------------------------------------------------------------------
class RegImageList : public wxImageList
{
public:
  enum Icon
  {
    Root,
    ClosedKey,
    OpenedKey,
    TextValue,
    BinaryValue,
  };

  RegImageList();
};

// ----------------------------------------------------------------------------
// our control
// ----------------------------------------------------------------------------
class RegTreeCtrl : public wxTreeCtrl
{
public:
  // ctor & dtor
  RegTreeCtrl(wxWindow *parent, wxWindowID id);
  virtual ~RegTreeCtrl();

  // notifications
  void OnDeleteItem   (wxTreeEvent& event);
  void OnItemExpanding(wxTreeEvent& event);
  void OnSelChanged   (wxTreeEvent& event);

  void OnBeginEdit    (wxTreeEvent& event);
  void OnEndEdit      (wxTreeEvent& event);

  void OnBeginDrag    (wxTreeEvent& event);
  void OnEndDrag      (wxTreeEvent& event);

  void OnRightClick   (wxMouseEvent& event);
  void OnChar         (wxKeyEvent& event);
  void OnIdle         (wxIdleEvent& event);

  // forwarded notifications (by the frame)
  void OnMenuTest();

  // operations
  void GoTo(const wxString& location);
  void Refresh();
  void DeleteSelected();
  void ShowProperties();
  void CreateNewKey(const wxString& strName);
  void CreateNewTextValue(const wxString& strName);
  void CreateNewBinaryValue(const wxString& strName);

  // information
  bool IsKeySelected() const;

private:
  // structure describing a registry key/value
  class TreeNode : public wxTreeItemData
  {
  WX_DEFINE_ARRAY(TreeNode *, TreeChildren);
  public:
      RegTreeCtrl  *m_pTree;     // must be !NULL
      TreeNode     *m_pParent;    // NULL only for the root node
      long          m_id;         // the id of the tree control item
      wxString      m_strName;    // name of the key/value
      TreeChildren  m_aChildren;  // array of subkeys/values
      bool          m_bKey;       // key or value?
      wxRegKey     *m_pKey;       // only may be !NULL if m_bKey == true

      // trivial accessors
      long      Id()     const { return m_id;              }
      bool      IsRoot() const { return m_pParent == NULL; }
      bool      IsKey()  const { return m_bKey;            }
      TreeNode *Parent() const { return m_pParent;         }

      // notifications
      bool OnExpand();
      void OnCollapse();

      // operations
      void Refresh();
      bool DeleteChild(TreeNode *child);
      void DestroyChildren();
      const char *FullName() const;

      // get the associated key: make sure the pointer is !NULL
      wxRegKey& Key() { if ( !m_pKey ) OnExpand(); return *m_pKey; }

      // dtor deletes all children
      ~TreeNode();
  };

  wxImageList *m_imageList;
  wxMenu      *m_pMenuPopup;

  TreeNode    *m_pRoot;

  TreeNode    *m_draggedItem;       // the item being dragged
  bool         m_copyOnDrop;        // if FALSE, then move

  bool         m_restoreStatus;     // after OnItemExpanding()

  wxString     m_nameOld;           // the initial value of item being renamed

  TreeNode *GetNode(const wxTreeEvent& event)
    { return (TreeNode *)GetItemData((WXHTREEITEM)event.GetItem()); }

public:
  // create a new node and insert it to the tree
  TreeNode *InsertNewTreeNode(TreeNode *pParent,
                              const wxString& strName,
                              int idImage = RegImageList::ClosedKey,
                              const wxString *pstrValue = NULL);
  // add standard registry keys
  void AddStdKeys();

private:
  DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// the main window of our application
// ----------------------------------------------------------------------------
class RegFrame : public wxFrame
{
public:
  // ctor & dtor
  RegFrame(wxFrame *parent, char *title, int x, int y, int w, int h);
  virtual ~RegFrame();

  // callbacks
  void OnQuit (wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnTest (wxCommandEvent& event);

  void OnGoTo (wxCommandEvent& event);

  void OnExpand  (wxCommandEvent& event);
  void OnCollapse(wxCommandEvent& event);
  void OnToggle  (wxCommandEvent& event);
  void OnRefresh (wxCommandEvent& event);

  void OnDelete   (wxCommandEvent& event);
  void OnNewKey   (wxCommandEvent& event);
  void OnNewText  (wxCommandEvent& event);
  void OnNewBinary(wxCommandEvent& event);

  void OnInfo     (wxCommandEvent& event);

  DECLARE_EVENT_TABLE()

private:
  RegTreeCtrl *m_treeCtrl;
};

// ----------------------------------------------------------------------------
// various ids
// ----------------------------------------------------------------------------

enum
{
  Menu_Quit     = 100,
  Menu_About,
  Menu_Test,
  Menu_GoTo,
  Menu_Expand,
  Menu_Collapse,
  Menu_Toggle,
  Menu_Refresh,
  Menu_New,
  Menu_NewKey,
  Menu_NewText,
  Menu_NewBinary,
  Menu_Delete,
  Menu_Info,

  Ctrl_RegTree  = 200,
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(RegFrame, wxFrame)
  EVT_MENU(Menu_Test,     RegFrame::OnTest)
  EVT_MENU(Menu_About,    RegFrame::OnAbout)
  EVT_MENU(Menu_Quit,     RegFrame::OnQuit)
  EVT_MENU(Menu_GoTo,     RegFrame::OnGoTo)
  EVT_MENU(Menu_Expand,   RegFrame::OnExpand)
  EVT_MENU(Menu_Collapse, RegFrame::OnCollapse)
  EVT_MENU(Menu_Toggle,   RegFrame::OnToggle)
  EVT_MENU(Menu_Refresh,  RegFrame::OnRefresh)
  EVT_MENU(Menu_Delete,   RegFrame::OnDelete)
  EVT_MENU(Menu_NewKey,   RegFrame::OnNewKey)
  EVT_MENU(Menu_NewText,  RegFrame::OnNewText)
  EVT_MENU(Menu_NewBinary,RegFrame::OnNewBinary)
  EVT_MENU(Menu_Info,     RegFrame::OnInfo)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(RegTreeCtrl, wxTreeCtrl)
  EVT_TREE_DELETE_ITEM   (Ctrl_RegTree, RegTreeCtrl::OnDeleteItem)
  EVT_TREE_ITEM_EXPANDING(Ctrl_RegTree, RegTreeCtrl::OnItemExpanding)
  EVT_TREE_SEL_CHANGED   (Ctrl_RegTree, RegTreeCtrl::OnSelChanged)

  EVT_TREE_BEGIN_LABEL_EDIT(Ctrl_RegTree, RegTreeCtrl::OnBeginEdit)
  EVT_TREE_END_LABEL_EDIT  (Ctrl_RegTree, RegTreeCtrl::OnEndEdit)

  EVT_TREE_BEGIN_DRAG    (Ctrl_RegTree, RegTreeCtrl::OnBeginDrag)
  EVT_TREE_BEGIN_RDRAG   (Ctrl_RegTree, RegTreeCtrl::OnBeginDrag)
  EVT_TREE_END_DRAG      (Ctrl_RegTree, RegTreeCtrl::OnEndDrag)

  EVT_CHAR      (RegTreeCtrl::OnChar)
  EVT_RIGHT_DOWN(RegTreeCtrl::OnRightClick)
  EVT_IDLE      (RegTreeCtrl::OnIdle)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// create the "registry operations" menu
wxMenu *CreateRegistryMenu()
{
  wxMenu *pMenuNew = new wxMenu;
  pMenuNew->Append(Menu_NewKey,    "&Key",          "Create a new key");
  pMenuNew->AppendSeparator();
  pMenuNew->Append(Menu_NewText,   "&Text value",   "Create a new text value");
  pMenuNew->Append(Menu_NewBinary, "&Binary value", "Create a new binary value");

  wxMenu *pMenuReg = new wxMenu;
  pMenuReg->Append(Menu_New, "&New", pMenuNew);
  pMenuReg->Append(Menu_Delete,   "&Delete...", "Delete selected key/value");
  pMenuReg->AppendSeparator();
  pMenuReg->Append(Menu_GoTo,     "&Go to...\tCtrl-G",    "Go to registry key");
  pMenuReg->Append(Menu_Expand,   "&Expand",    "Expand current key");
  pMenuReg->Append(Menu_Collapse, "&Collapse",  "Collapse current key");
  pMenuReg->Append(Menu_Toggle,   "&Toggle",    "Toggle current key");
  pMenuReg->AppendSeparator();
  pMenuReg->Append(Menu_Refresh,  "&Refresh",   "Refresh the subtree");
  pMenuReg->AppendSeparator();
  pMenuReg->Append(Menu_Info,     "&Properties","Information about current selection");

  return pMenuReg;
}

// ----------------------------------------------------------------------------
// application class
// ----------------------------------------------------------------------------
IMPLEMENT_APP(RegApp)

// `Main program' equivalent, creating windows and returning main app frame
bool RegApp::OnInit()
{
  // create the main frame window and show it
  RegFrame *frame = new RegFrame(NULL, "wxRegTest", 50, 50, 600, 350);
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

// ----------------------------------------------------------------------------
// RegFrame
// ----------------------------------------------------------------------------

RegFrame::RegFrame(wxFrame *parent, char *title, int x, int y, int w, int h)
        : wxFrame(parent, -1, title, wxPoint(x, y), wxSize(w, h))
{
  // this reduces flicker effects
  SetBackgroundColour(wxColour(255, 255, 255));

  // set the icon
  // ------------
  SetIcon(wxIcon("app_icon"));

  // create menu
  // -----------
  wxMenu *pMenuFile = new wxMenu;
  pMenuFile->Append(Menu_Test, "Te&st", "Test key creation");
  pMenuFile->AppendSeparator();
  pMenuFile->Append(Menu_About, "&About...", "Show an extraordinarly beautiful dialog");
  pMenuFile->AppendSeparator();
  pMenuFile->Append(Menu_Quit,  "E&xit", "Quit this program");

  wxMenuBar *pMenu = new wxMenuBar;
  pMenu->Append(pMenuFile, "&File");
  pMenu->Append(CreateRegistryMenu(),  "&Registry");
  SetMenuBar(pMenu);

  // create child controls
  // ---------------------
  m_treeCtrl = new RegTreeCtrl(this, Ctrl_RegTree);

  // create the status line
  // ----------------------
  CreateStatusBar(2);
}

RegFrame::~RegFrame()
{
  // this makes deletion of it *much* quicker
  m_treeCtrl->Hide();
}

void RegFrame::OnQuit(wxCommandEvent& event)
{
  Close(TRUE);
}

void RegFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageDialog dialog(this,
                         "wxRegistry sample\n"
                         "© 1998, 2000 Vadim Zeitlin",
                         "About wxRegTest", wxOK);

  dialog.ShowModal();
}

void RegFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
  m_treeCtrl->OnMenuTest();
}

void RegFrame::OnGoTo(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_location = _T("HKEY_CURRENT_USER\\Software\\wxWindows");

    wxString location = wxGetTextFromUser
                        (
                         _T("Enter the location to go to:"),
                         _T("wxRegTest question"),
                         s_location,
                         this
                        );
    if ( !location )
        return;

    s_location = location;
    m_treeCtrl->GoTo(location);
}

void RegFrame::OnExpand(wxCommandEvent& WXUNUSED(event))
{
  m_treeCtrl->ExpandItem(m_treeCtrl->GetSelection(), wxTREE_EXPAND_EXPAND);
}

void RegFrame::OnCollapse(wxCommandEvent& WXUNUSED(event))
{
  m_treeCtrl->ExpandItem(m_treeCtrl->GetSelection(), wxTREE_EXPAND_COLLAPSE);
}

void RegFrame::OnToggle(wxCommandEvent& WXUNUSED(event))
{
  m_treeCtrl->ExpandItem(m_treeCtrl->GetSelection(), wxTREE_EXPAND_TOGGLE);
}

void RegFrame::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
  m_treeCtrl->Refresh();
}

void RegFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
  m_treeCtrl->DeleteSelected();
}

void RegFrame::OnNewKey(wxCommandEvent& WXUNUSED(event))
{
  if ( m_treeCtrl->IsKeySelected() ) {
    m_treeCtrl->CreateNewKey(
      wxGetTextFromUser("Enter the name of the new key"));
  }
}

void RegFrame::OnNewText(wxCommandEvent& WXUNUSED(event))
{
  if ( m_treeCtrl->IsKeySelected() ) {
    m_treeCtrl->CreateNewTextValue(
      wxGetTextFromUser("Enter the name for the new text value"));
  }
}

void RegFrame::OnNewBinary(wxCommandEvent& WXUNUSED(event))
{
  if ( m_treeCtrl->IsKeySelected() ) {
    m_treeCtrl->CreateNewBinaryValue(
      wxGetTextFromUser("Enter the name for the new binary value"));
  }
}

void RegFrame::OnInfo(wxCommandEvent& WXUNUSED(event))
{
    m_treeCtrl->ShowProperties();
}

// ----------------------------------------------------------------------------
// RegImageList
// ----------------------------------------------------------------------------
RegImageList::RegImageList() : wxImageList(16, 16, TRUE)
{
  // should be in sync with enum RegImageList::RegIcon
  static const char *aszIcons[] = { "key1","key2","key3","value1","value2" };
  wxString str = "icon_";
  for ( unsigned int n = 0; n < WXSIZEOF(aszIcons); n++ ) {
    Add(wxIcon(str + aszIcons[n], wxBITMAP_TYPE_ICO_RESOURCE));
  }
}

// ----------------------------------------------------------------------------
// RegTreeCtrl
// ----------------------------------------------------------------------------

// create a new tree item and insert it into the tree
RegTreeCtrl::TreeNode *RegTreeCtrl::InsertNewTreeNode(TreeNode *pParent,
                                                      const wxString& strName,
                                                      int idImage,
                                                      const wxString *pstrValue)
{
  // create new item & insert it
  TreeNode *pNewNode = new TreeNode;
  pNewNode->m_pTree  = this;
  pNewNode->m_pParent = pParent;
  pNewNode->m_strName = strName;
  pNewNode->m_bKey    = pstrValue == NULL;
  pNewNode->m_pKey    = NULL;
  pNewNode->m_id      = InsertItem(pParent ? pParent->Id() : 0,
                                   pNewNode->IsKey() ? strName : *pstrValue,
                                   idImage);

  wxASSERT_MSG( pNewNode->m_id, "can't create tree control item!");

  // save the pointer in the item
  SetItemData(pNewNode->m_id, pNewNode);

  // add it to the list of parent's children
  if ( pParent != NULL ) {
    pParent->m_aChildren.Add(pNewNode);
  }

  if ( pNewNode->IsKey() ) {
    SetItemHasChildren(pNewNode->Id());

    if ( !pNewNode->IsRoot() ) {
        // set the expanded icon as well
        SetItemImage(pNewNode->Id(),
                     RegImageList::OpenedKey,
                     wxTreeItemIcon_Expanded);
    }
  }

  return pNewNode;
}

RegTreeCtrl::RegTreeCtrl(wxWindow *parent, wxWindowID id)
           : wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize,
                        wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS | wxSUNKEN_BORDER)
{
  // init members
  m_draggedItem = NULL;
  m_restoreStatus = FALSE;

  // create the image list
  // ---------------------
  m_imageList = new RegImageList;
  SetImageList(m_imageList, wxIMAGE_LIST_NORMAL);

  // create root keys
  // ----------------
  m_pRoot = InsertNewTreeNode(NULL, "Registry Root", RegImageList::Root);

  // create popup menu
  // -----------------
  m_pMenuPopup = CreateRegistryMenu();
}

RegTreeCtrl::~RegTreeCtrl()
{
  delete m_pMenuPopup;
  // delete m_pRoot; -- this is done by the tree now
  delete m_imageList;
}

void RegTreeCtrl::AddStdKeys()
{
  for ( unsigned int ui = 0; ui < wxRegKey::nStdKeys; ui++ ) {
    InsertNewTreeNode(m_pRoot, wxRegKey::GetStdKeyName(ui));
  }
}

// ----------------------------------------------------------------------------
// notifications
// ----------------------------------------------------------------------------

void RegTreeCtrl::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    if ( m_restoreStatus ) {
        // restore it after OnItemExpanding()
        wxLogStatus("Ok");
        wxSetCursor(*wxSTANDARD_CURSOR);

        m_restoreStatus = FALSE;
    }
}

void RegTreeCtrl::OnRightClick(wxMouseEvent& event)
{
  int iFlags;
  long lId = HitTest(wxPoint(event.GetX(), event.GetY()), iFlags);
  if ( iFlags & wxTREE_HITTEST_ONITEMLABEL ) {
    // select the item first
    SelectItem(lId);
  }
  //else: take the currently selected item if click not on item

  PopupMenu(m_pMenuPopup, event.GetX(), event.GetY());
}


void RegTreeCtrl::OnDeleteItem(wxTreeEvent& event)
{
}

// test the key creation functions
void RegTreeCtrl::OnMenuTest()
{
  long lId = GetSelection();
  TreeNode *pNode = (TreeNode *)GetItemData(lId);

  wxCHECK_RET( pNode != NULL, "tree item without data?" );

  if ( pNode->IsRoot() ) {
    wxLogError("Can't create a subkey under the root key.");
    return;
  }
  if ( !pNode->IsKey() ) {
    wxLogError("Can't create a subkey under a value!");
    return;
  }

  wxRegKey key1(pNode->Key(), "key1");
  if ( key1.Create() ) {
    wxRegKey key2a(key1, "key2a"), key2b(key1, "key2b");
    if ( key2a.Create() && key2b.Create() ) {
      // put some values under the newly created keys
      key1.SetValue("first_term", "10");
      key1.SetValue("second_term", "7");
      key2a = "this is the unnamed value";
      key2b.SetValue("sum", 17);

      // refresh tree
      pNode->Refresh();
      wxLogStatus("Test keys successfully added.");
      return;
    }
  }

  wxLogError("Creation of test keys failed.");
}

void RegTreeCtrl::OnChar(wxKeyEvent& event)
{
  switch ( event.KeyCode() )
  {
      case WXK_DELETE:
          DeleteSelected();
          return;

      case WXK_RETURN:
          if ( event.AltDown() )
          {
              ShowProperties();

              return;
          }
  }

  event.Skip();
}

void RegTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
  wxFrame *pFrame = (wxFrame *)(wxWindow::GetParent());
  pFrame->SetStatusText(GetNode(event)->FullName(), 1);
}

void RegTreeCtrl::OnItemExpanding(wxTreeEvent& event)
{
  TreeNode *pNode = GetNode(event);
  bool bExpanding = event.GetCode() == wxTREE_EXPAND_EXPAND;

  // expansion might take some time
  wxSetCursor(*wxHOURGLASS_CURSOR);
  wxLogStatus("Working...");
  wxYield();  // to give the status line a chance to refresh itself
  m_restoreStatus = TRUE;   // some time later...

  if ( pNode->IsKey() ) {
    if ( bExpanding ) {
      // expanding: add subkeys/values
      if ( !pNode->OnExpand() )
        return;
    }
    else {
      // collapsing: clean up
      pNode->OnCollapse();
    }
  }
}

void RegTreeCtrl::OnBeginEdit(wxTreeEvent& event)
{
    TreeNode *pNode = GetNode(event);
    if ( pNode->IsRoot() || pNode->Parent()->IsRoot() ) {
        wxLogStatus(_T("This registry key can't be renamed."));

        event.Veto();
    }
    else {
        m_nameOld = pNode->m_strName;
    }
}

void RegTreeCtrl::OnEndEdit(wxTreeEvent& event)
{
    bool ok;

    wxString name = event.GetLabel();

    TreeNode *pNode = GetNode(event);
    if ( pNode->IsKey() )
    {
        wxRegKey& key = pNode->Key();
        ok = key.Rename(name);
    }
    else
    {
        pNode = pNode->Parent();
        wxRegKey& key = pNode->Key();

        ok = key.RenameValue(m_nameOld, name);
    }

    if ( !ok ) {
        wxLogError(_T("Failed to rename '%s' to '%s'."),
                   m_nameOld.c_str(), name.c_str());
    }
#if 0   // MSW tree ctrl doesn't like this at all, it hangs
    else {
        pNode->Refresh();
    }
#endif // 0
}

void RegTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
    m_copyOnDrop = event.GetEventType() == wxEVT_COMMAND_TREE_BEGIN_DRAG;

    TreeNode *pNode = GetNode(event);
    if ( pNode->IsRoot() || pNode->Parent()->IsRoot() )
    {
        wxLogStatus("This registry key can't be %s.",
                    m_copyOnDrop ? "copied" : "moved");
    }
    else
    {
        wxLogStatus("%s item %s...",
                    m_copyOnDrop ? "Copying" : "Moving",
                    pNode->FullName());

        m_draggedItem = pNode;

        event.Allow();
    }
}

void RegTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
    wxCHECK_RET( m_draggedItem, "end drag without begin drag?" );

    // clear the pointer anyhow
    TreeNode *src = m_draggedItem;
    m_draggedItem = NULL;

    // where are we going to drop it?
    TreeNode *dst = GetNode(event);
    if ( dst && !dst->IsKey() ) {
        // we need a parent key
        dst = dst->Parent();
    }
    if ( !dst || dst->IsRoot() ) {
        wxLogError("Can't create a key here.");

        return;
    }

    bool isKey = src->IsKey();
    if ( (isKey && (src == dst)) ||
         (!isKey && (dst->Parent() == src)) ) {
        wxLogStatus("Can't copy something on itself");

        return;
    }

    // remove the "Registry Root\\" from the full name
    wxString nameSrc, nameDst;
    nameSrc << wxString(src->FullName()).AfterFirst('\\');
    nameDst << wxString(dst->FullName()).AfterFirst('\\') << '\\'
            << wxString(src->FullName()).AfterLast('\\');

    wxString verb = m_copyOnDrop ? "copy" : "move";
    wxString what = isKey ? "key" : "value";

    if ( wxMessageBox(wxString::Format
                        (
                         "Do you really want to %s the %s %s to %s?",
                         verb.c_str(),
                         what.c_str(),
                         nameSrc.c_str(),
                         nameDst.c_str()
                        ),
                      "RegTest Confirm",
                      wxICON_QUESTION | wxYES_NO | wxCANCEL, this) != wxYES ) {
      return;
    }

    bool ok;
    if ( isKey ) {
        wxRegKey& key = src->Key();
        wxRegKey keyDst(dst->Key(), src->m_strName);
        ok = keyDst.Create(FALSE);
        if ( !ok ) {
            wxLogError("Key '%s' already exists");
        }
        else {
            ok = key.Copy(keyDst);
        }

        if ( ok && !m_copyOnDrop ) {
            // delete the old key
            ok = key.DeleteSelf();
            if ( ok ) {
                src->Parent()->Refresh();
            }
        }
    }
    else { // value
        wxRegKey& key = src->Parent()->Key();
        ok = key.CopyValue(src->m_strName, dst->Key());
        if ( ok && !m_copyOnDrop ) {
            // we moved it, so delete the old one
            ok = key.DeleteValue(src->m_strName);
        }
    }

    if ( !ok ) {
        wxLogError("Failed to %s registry %s.", verb.c_str(), what.c_str());
    }
    else {
        dst->Refresh();
    }
}

// ----------------------------------------------------------------------------
// TreeNode implementation
// ----------------------------------------------------------------------------
bool RegTreeCtrl::TreeNode::OnExpand()
{
  // we add children only once
  if ( !m_aChildren.IsEmpty() ) {
    // we've been already expanded
    return TRUE;
  }

  if ( IsRoot() ) {
    // we're the root key
    m_pTree->AddStdKeys();
    return TRUE;
  }

  if ( Parent()->IsRoot() ) {
    // we're a standard key
    m_pKey = new wxRegKey(m_strName);
  }
  else {
    // we're a normal key
    m_pKey = new wxRegKey(*(Parent()->m_pKey), m_strName);
  }

  if ( !m_pKey->Open() ) {
    wxLogError("The key '%s' can't be opened.", FullName());
    return FALSE;
  }

  // if we're empty, we shouldn't be expandable at all
  bool isEmpty = TRUE;

  // enumeration variables
  long l;
  wxString str;
  bool bCont;

  // enumerate all subkeys
  bCont = m_pKey->GetFirstKey(str, l);
  while ( bCont ) {
    m_pTree->InsertNewTreeNode(this, str, RegImageList::ClosedKey);
    bCont = m_pKey->GetNextKey(str, l);

    // we have at least this key...
    isEmpty = FALSE;
  }

  // enumerate all values
  bCont = m_pKey->GetFirstValue(str, l);
  while ( bCont ) {
    wxString strItem;
    if (str.IsEmpty())
        strItem = "<default>";
    else
        strItem = str;
    strItem += " = ";

    // determine the appropriate icon
    RegImageList::Icon icon;
    switch ( m_pKey->GetValueType(str) ) {
      case wxRegKey::Type_String:
      case wxRegKey::Type_Expand_String:
      case wxRegKey::Type_Multi_String:
        {
          wxString strValue;
          icon = RegImageList::TextValue;
          m_pKey->QueryValue(str, strValue);
          strItem += strValue;
        }
        break;

      case wxRegKey::Type_None:
        // @@ handle the error...
        icon = RegImageList::BinaryValue;
        break;

      case wxRegKey::Type_Dword:
        {
          long l;
          m_pKey->QueryValue(str, &l);
          strItem << l;
        }

        // fall through

      default:
        icon = RegImageList::BinaryValue;
    }

    m_pTree->InsertNewTreeNode(this, str, icon, &strItem);
    bCont = m_pKey->GetNextValue(str, l);

    // we have at least this value...
    isEmpty = FALSE;
  }

  if ( isEmpty ) {
    // this is for the case when our last child was just deleted
    wxTreeItemId theId(Id()); // Temp variable seems necessary for BC++
    m_pTree->Collapse(theId);

    // we won't be expanded any more
    m_pTree->SetItemHasChildren(theId, FALSE);
  }

  return TRUE;
}

void RegTreeCtrl::TreeNode::OnCollapse()
{
  DestroyChildren();

  delete m_pKey;
  m_pKey = NULL;
}

void RegTreeCtrl::TreeNode::Refresh()
{
    if ( !IsKey() )
        return;

    wxTreeItemId theId(Id()); // Temp variable seems necessary for BC++
    bool wasExpanded = m_pTree->IsExpanded(theId);
    if ( wasExpanded )
        m_pTree->Collapse(theId);

    OnCollapse();
    m_pTree->SetItemHasChildren(theId);
    if ( wasExpanded ) {
        m_pTree->Expand(theId);
        OnExpand();
    }
}

bool RegTreeCtrl::TreeNode::DeleteChild(TreeNode *child)
{
    int index = m_aChildren.Index(child);
    wxCHECK_MSG( index != wxNOT_FOUND, FALSE,
                 "our child in tree should be in m_aChildren" );

    m_aChildren.RemoveAt((size_t)index);

    bool ok;
    if ( child->IsKey() ) {
        // must close key before deleting it
        child->OnCollapse();

        ok = Key().DeleteKey(child->m_strName);
    }
    else {
        ok = Key().DeleteValue(child->m_strName);
    }

    if ( ok ) {
        wxTreeItemId theId(child->Id()); // Temp variable seems necessary for BC++
        m_pTree->Delete(theId);

        Refresh();
    }

    return ok;
}

void RegTreeCtrl::TreeNode::DestroyChildren()
{
  // destroy all children
  size_t nCount = m_aChildren.GetCount();
  for ( size_t n = 0; n < nCount; n++ ) {
    long lId = m_aChildren[n]->Id();
    // no, wxTreeCtrl will do it
    //delete m_aChildren[n];
    wxTreeItemId theId(lId); // Temp variable seems necessary for BC++
    m_pTree->Delete(theId);
  }

  m_aChildren.Empty();
}

RegTreeCtrl::TreeNode::~TreeNode()
{
  delete m_pKey;
}

const char *RegTreeCtrl::TreeNode::FullName() const
{
  static wxString s_strName;

  if ( IsRoot() ) {
    return "Registry Root";
  }
  else {
    // our own registry key might not (yet) exist or we might be a value,
    // so just use the parent's and concatenate
    s_strName = Parent()->FullName();
    s_strName << '\\' << m_strName;

    return s_strName;
  }
}

// ----------------------------------------------------------------------------
// operations on RegTreeCtrl
// ----------------------------------------------------------------------------

void RegTreeCtrl::GoTo(const wxString& location)
{
    wxStringTokenizer tk(location, _T("\\"));

    wxTreeItemId id = GetRootItem();

    while ( tk.HasMoreTokens() ) {
        wxString subkey = tk.GetNextToken();

        wxTreeItemId idCurrent = id;
        if ( !IsExpanded(idCurrent) )
            Expand(idCurrent);

        long dummy;
        id = GetFirstChild(idCurrent, dummy);

        if ( idCurrent == GetRootItem() ) {
            // special case: we understand both HKCU and HKEY_CURRENT_USER here
            for ( size_t key = 0; key < wxRegKey::nStdKeys; key++ ) {
                if ( subkey == wxRegKey::GetStdKeyName(key) ||
                     subkey == wxRegKey::GetStdKeyShortName(key) ) {
                    break;
                }

                id = GetNextChild(idCurrent, dummy);
            }
        }
        else {
            // enum all children
            while ( id.IsOk() ) {
                if ( subkey == ((TreeNode *)GetItemData(id))->m_strName )
                    break;

                id = GetNextChild(idCurrent, dummy);
            }
        }

        if ( !id.IsOk() ) {
            wxLogError(_T("No such key '%s'."), location.c_str());

            return;
        }
    }

    if ( id.IsOk() )
        SelectItem(id);
}

void RegTreeCtrl::DeleteSelected()
{
  long lCurrent = GetSelection(),
       lParent  = GetParent(lCurrent);

  if ( lParent == 0 ) {
    wxLogError("Can't delete root key.");
    return;
  }

  TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent),
           *pParent  = (TreeNode *)GetItemData(lParent);

  wxCHECK_RET( pCurrent && pParent, "either node or parent without data?" );

  if ( pParent->IsRoot() ) {
    wxLogError("Can't delete standard key.");
    return;
  }

  wxString what = pCurrent->IsKey() ? "key" : "value";
  if ( wxMessageBox(wxString::Format
                    (
                      "Do you really want to delete this %s?",
                      what.c_str()
                    ),
                    "Confirmation",
                    wxICON_QUESTION | wxYES_NO | wxCANCEL, this) != wxYES ) {
    return;
  }

  pParent->DeleteChild(pCurrent);
}

void RegTreeCtrl::CreateNewKey(const wxString& strName)
{
  long lCurrent = GetSelection();
  TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

  wxCHECK_RET( pCurrent != NULL, "node without data?" );

  wxASSERT( pCurrent->IsKey() );  // check must have been done before

  if ( pCurrent->IsRoot() ) {
    wxLogError("Can't create a new key under the root key.");
    return;
  }

  wxRegKey key(pCurrent->Key(), strName);
  if ( key.Create() )
    pCurrent->Refresh();
}

void RegTreeCtrl::CreateNewTextValue(const wxString& strName)
{
  long lCurrent = GetSelection();
  TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

  wxCHECK_RET( pCurrent != NULL, "node without data?" );

  wxASSERT( pCurrent->IsKey() );  // check must have been done before

  if ( pCurrent->IsRoot() ) {
    wxLogError("Can't create a new value under the root key.");
    return;
  }

  if ( pCurrent->Key().SetValue(strName, "") )
    pCurrent->Refresh();
}

void RegTreeCtrl::CreateNewBinaryValue(const wxString& strName)
{
  long lCurrent = GetSelection();
  TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

  wxCHECK_RET( pCurrent != NULL, "node without data?" );

  wxASSERT( pCurrent->IsKey() );  // check must have been done before

  if ( pCurrent->IsRoot() ) {
    wxLogError("Can't create a new value under the root key.");
    return;
  }

  if ( pCurrent->Key().SetValue(strName, 0) )
    pCurrent->Refresh();
}

void RegTreeCtrl::ShowProperties()
{
    long lCurrent = GetSelection();
    TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

    if ( !pCurrent || pCurrent->IsRoot() )
    {
        wxLogStatus("No properties");

        return;
    }

    if ( pCurrent->IsKey() )
    {
        const wxRegKey& key = pCurrent->Key();
        size_t nSubKeys, nValues;
        if ( !key.GetKeyInfo(&nSubKeys, NULL, &nValues, NULL) )
        {
            wxLogError("Couldn't get key info");
        }
        else
        {
            wxLogMessage("Key '%s' has %u subkeys and %u values.",
                         key.GetName().c_str(), nSubKeys, nValues);
        }
    }
    else // it's a value
    {
        TreeNode *parent = pCurrent->Parent();
        wxCHECK_RET( parent, "reg value without key?" );

        const wxRegKey& key = parent->Key();
        const char *value = pCurrent->m_strName.c_str();
        wxLogMessage("Value '%s' under the key '%s' is of type "
                     "%d (%s).",
                     value,
                     parent->m_strName.c_str(),
                     key.GetValueType(value),
                     key.IsNumericValue(value) ? "numeric" : "string");

    }
}

bool RegTreeCtrl::IsKeySelected() const
{
  long lCurrent = GetSelection();
  TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

  wxCHECK( pCurrent != NULL, FALSE );

  return pCurrent->IsKey();
}

void RegTreeCtrl::Refresh()
{
    long lId = GetSelection();
    if ( !lId )
        return;

    TreeNode *pNode = (TreeNode *)GetItemData(lId);

    wxCHECK_RET( pNode != NULL, "tree item without data?" );

    pNode->Refresh();
}

