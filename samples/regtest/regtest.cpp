///////////////////////////////////////////////////////////////////////////////
// Name:        registry.cpp
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

// ----------------------------------------------------------------------------
// application type
// ----------------------------------------------------------------------------
class RegApp : public wxApp
{ 
public:
  bool OnInit(void);
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

// array of children of the node
//class TreeNode;

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

  void OnRightClick   (wxMouseEvent& event);
  void OnChar         (wxKeyEvent& event);

  // forwarded notifications (by the frame)
  void OnMenuTest();

  // operations
  void DeleteSelected();
  void CreateNewKey(const wxString& strName);
  void CreateNewTextValue(const wxString& strName);
  void CreateNewBinaryValue(const wxString& strName);

  // information
  bool IsKeySelected() const;

  DECLARE_EVENT_TABLE();

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
    long          m_lDummy;     // dummy subkey (to make expansion possible)

    // ctor
    TreeNode() { m_lDummy = 0; }

    // trivial accessors
    long      Id()     const { return m_id;              }
    bool      IsRoot() const { return m_pParent == NULL; }
    bool      IsKey()  const { return m_bKey;            }
    TreeNode *Parent() const { return m_pParent;         }

    // notifications
    bool OnExpand();
    void OnCollapse();

    // operations
    void Refresh() { OnCollapse(); OnExpand(); }
    void AddDummy();
    void DestroyChildren();
    const char *FullName() const;

    // get the associated key: make sure the pointer is !NULL
    wxRegKey& Key() { if ( !m_pKey ) OnExpand(); return *m_pKey; }

    // dtor deletes all children
    ~TreeNode();
  };

  wxMenu      *m_pMenuPopup;
  TreeNode    *m_pRoot;
  wxImageList *m_imageList;

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
};

// ----------------------------------------------------------------------------
// the main window of our application
// ----------------------------------------------------------------------------
class RegFrame : public wxFrame
{ 
public:
  // ctor & dtor
  RegFrame(wxFrame *parent, char *title, int x, int y, int w, int h);
  virtual ~RegFrame(void);
    
  // callbacks
  void OnQuit (wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnTest (wxCommandEvent& event);

  void OnExpand  (wxCommandEvent& event);
  void OnCollapse(wxCommandEvent& event);
  void OnToggle  (wxCommandEvent& event);

  void OnDelete   (wxCommandEvent& event);
  void OnNewKey   (wxCommandEvent& event);
  void OnNewText  (wxCommandEvent& event);
  void OnNewBinary(wxCommandEvent& event);

  bool OnClose  () { return TRUE; }

  DECLARE_EVENT_TABLE();

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
  Menu_Expand,
  Menu_Collapse,
  Menu_Toggle,
  Menu_New,
  Menu_NewKey,  
  Menu_NewText, 
  Menu_NewBinary,
  Menu_Delete,

  Ctrl_RegTree  = 200,
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(RegFrame, wxFrame)
  EVT_MENU(Menu_Test,     RegFrame::OnTest)
  EVT_MENU(Menu_About,    RegFrame::OnAbout)
  EVT_MENU(Menu_Quit,     RegFrame::OnQuit)
  EVT_MENU(Menu_Expand,   RegFrame::OnExpand)
  EVT_MENU(Menu_Collapse, RegFrame::OnCollapse)
  EVT_MENU(Menu_Toggle,   RegFrame::OnToggle)
  EVT_MENU(Menu_Delete,   RegFrame::OnDelete)
  EVT_MENU(Menu_NewKey,   RegFrame::OnNewKey)
  EVT_MENU(Menu_NewText,  RegFrame::OnNewText)
  EVT_MENU(Menu_NewBinary,RegFrame::OnNewBinary)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(RegTreeCtrl, wxTreeCtrl)
  EVT_TREE_DELETE_ITEM   (Ctrl_RegTree, RegTreeCtrl::OnDeleteItem)
  EVT_TREE_ITEM_EXPANDING(Ctrl_RegTree, RegTreeCtrl::OnItemExpanding)
  EVT_TREE_SEL_CHANGED   (Ctrl_RegTree, RegTreeCtrl::OnSelChanged)

  EVT_CHAR      (RegTreeCtrl::OnChar)
  EVT_RIGHT_DOWN(RegTreeCtrl::OnRightClick)
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
  pMenuReg->Append(Menu_Expand,   "&Expand",    "Expand current key");
  pMenuReg->Append(Menu_Collapse, "&Collapse",  "Collapse current key");
  pMenuReg->Append(Menu_Toggle,   "&Toggle",    "Toggle current key");

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
  RegFrame *frame = new RegFrame(NULL, "wxRegKey Test", 50, 50, 600, 350);
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
  int aWidths[2];
  aWidths[0] = 200;
  aWidths[1] = -1;
  CreateStatusBar(2);
  SetStatusWidths(2, aWidths);
}

RegFrame::~RegFrame(void)
{
}

void RegFrame::OnQuit(wxCommandEvent& event)
{
  Close(TRUE);
}

void RegFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageDialog dialog(this, "wxRegistry sample\n(c) 1998 Vadim Zeitlin",
                         "About wxRegistry", wxOK);

  dialog.ShowModal();
}

void RegFrame::OnTest(wxCommandEvent& event)
{
  m_treeCtrl->OnMenuTest();
}

void RegFrame::OnExpand(wxCommandEvent& event)
{
  m_treeCtrl->ExpandItem(m_treeCtrl->GetSelection(), wxTREE_EXPAND_EXPAND);
}

void RegFrame::OnCollapse(wxCommandEvent& event)
{
  m_treeCtrl->ExpandItem(m_treeCtrl->GetSelection(), wxTREE_EXPAND_COLLAPSE);
}

void RegFrame::OnToggle(wxCommandEvent& event)
{
  m_treeCtrl->ExpandItem(m_treeCtrl->GetSelection(), wxTREE_EXPAND_TOGGLE);
}

void RegFrame::OnDelete(wxCommandEvent& event)
{
  m_treeCtrl->DeleteSelected();
}

void RegFrame::OnNewKey(wxCommandEvent& event)
{
  if ( m_treeCtrl->IsKeySelected() ) {
    m_treeCtrl->CreateNewKey(
      wxGetTextFromUser("Enter the name of the new key"));
  }
}

void RegFrame::OnNewText(wxCommandEvent& event)
{
  if ( m_treeCtrl->IsKeySelected() ) {
    m_treeCtrl->CreateNewTextValue(
      wxGetTextFromUser("Enter the name for the new text value"));
  }
}

void RegFrame::OnNewBinary(wxCommandEvent& event)
{
  if ( m_treeCtrl->IsKeySelected() ) {
    m_treeCtrl->CreateNewBinaryValue(
      wxGetTextFromUser("Enter the name for the new binary value"));
  }
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
  pNewNode->m_id      = InsertItem(pParent ? pParent->m_id : 0, 
                                   pNewNode->IsKey() ? strName : *pstrValue,
                                   idImage);

  wxASSERT_MSG( pNewNode->m_id, "can't create tree control item!");

  // save the pointer in the item
  SetItemData(pNewNode->m_id, pNewNode);

  // add it to the list of parent's children
  if ( pParent != NULL ) {
    pParent->m_aChildren.Add(pNewNode);
  }

  // force the [+] button (@@@ not very elegant...)
  if ( pNewNode->IsKey() )
    pNewNode->AddDummy();

  return pNewNode;
}

RegTreeCtrl::RegTreeCtrl(wxWindow *parent, wxWindowID id)
           : wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, 
                        wxTR_HAS_BUTTONS | wxSUNKEN_BORDER)
{
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
  delete m_pRoot;
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

void RegTreeCtrl::OnRightClick(wxMouseEvent& event)
{
  int iFlags;
  long lId = HitTest(wxPoint(event.GetX(), event.GetY()), iFlags);
  if ( iFlags & wxTREE_HITTEST_ONITEMLABEL ) {
    // popup menu only if an item was clicked
    wxASSERT( lId != 0 );
    SelectItem(lId);
    PopupMenu(m_pMenuPopup, event.GetX(), event.GetY());
  }
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
  if ( event.KeyCode() == WXK_DELETE )
    DeleteSelected();
  else
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

    // change icon for non root key
    if ( !pNode->IsRoot() ) {
      int idIcon = bExpanding ? RegImageList::OpenedKey 
                              : RegImageList::ClosedKey;
      SetItemImage(pNode->Id(), idIcon);
    }
  }

  wxLogStatus("Ok");
  wxSetCursor(*wxSTANDARD_CURSOR);
}

// ----------------------------------------------------------------------------
// TreeNode implementation
// ----------------------------------------------------------------------------
bool RegTreeCtrl::TreeNode::OnExpand()
{
  // remove dummy item
  if ( m_lDummy != 0 ) {
    m_pTree->Delete(m_lDummy);
    m_lDummy = 0;
  }
  else {
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

  // enumeration variables
  long l;
  wxString str;
  bool bCont;

  // enumerate all subkeys
  bCont = m_pKey->GetFirstKey(str, l);
  while ( bCont ) {
    m_pTree->InsertNewTreeNode(this, str, RegImageList::ClosedKey);
    bCont = m_pKey->GetNextKey(str, l);
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
          char szBuf[128];
          long l;
          m_pKey->QueryValue(str, &l);
          sprintf(szBuf, "%lx", l);
          strItem += szBuf;
        }

        // fall through

      default:
        icon = RegImageList::BinaryValue;
    }

    m_pTree->InsertNewTreeNode(this, str, icon, &strItem);
    bCont = m_pKey->GetNextValue(str, l);
  }

  return TRUE;
}

void RegTreeCtrl::TreeNode::OnCollapse()
{
  bool bHasChildren = !m_aChildren.IsEmpty();
  DestroyChildren();
  if ( bHasChildren )
    AddDummy();
  else
    m_lDummy = 0;

  delete m_pKey;
  m_pKey = NULL;
}

void RegTreeCtrl::TreeNode::AddDummy()
{
  // insert dummy item forcing appearance of [+] button
  m_lDummy = m_pTree->InsertItem(Id(), "");
}

void RegTreeCtrl::TreeNode::DestroyChildren()
{
  // destroy all children
  unsigned int nCount = m_aChildren.Count();
  for ( unsigned int n = 0; n < nCount; n++ ) {
	  long lId = m_aChildren[n]->Id();
    delete m_aChildren[n];
    m_pTree->Delete(lId);
  }

  m_aChildren.Empty();
}

RegTreeCtrl::TreeNode::~TreeNode()
{
  DestroyChildren();

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

  if ( pCurrent->IsKey() ) {
    if ( wxMessageBox("Do you really want to delete this key?",
                      "Confirmation", 
                      wxICON_QUESTION | wxYES_NO | wxCANCEL, this) != wxYES ) {
      return;
    }

    // must close key before deleting it
    pCurrent->OnCollapse();

    if ( pParent->Key().DeleteKey(pCurrent->m_strName) )
      pParent->Refresh();
  }
  else {
    if ( wxMessageBox("Do you really want to delete this value?",
                      "Confirmation", 
                      wxICON_QUESTION | wxYES_NO | wxCANCEL, this) != wxYES ) {
      return;
    }

    if ( pParent->Key().DeleteValue(pCurrent->m_strName) )
      pParent->Refresh();
  }
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

bool RegTreeCtrl::IsKeySelected() const
{
  long lCurrent = GetSelection();
  TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

  wxCHECK( pCurrent != NULL, FALSE );

  return pCurrent->IsKey();
}