///////////////////////////////////////////////////////////////////////////////
// Name:        regtest.cpp
// Purpose:     wxRegKey class demo
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifdef  __BORLANDC__
#   pragma hdrstop
#endif

#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#include "wx/treectrl.h"
#include "wx/config.h"
#include "wx/imaglist.h"
#include "wx/tokenzr.h"

#if wxUSE_CONFIG_NATIVE && defined( __WXMSW__ )
#   define DO_REGTEST 1
#else
#   define DO_REGTEST 0
#endif

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
        BinaryValue
    };

    RegImageList();
};

#if DO_REGTEST

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
    void OnDeleteItem    (wxTreeEvent& event);
    void OnItemExpanding (wxTreeEvent& event);
    void OnSelChanged    (wxTreeEvent& event);

    void OnBeginEdit     (wxTreeEvent& event);
    void OnEndEdit       (wxTreeEvent& event);

    void OnBeginDrag     (wxTreeEvent& event);
    void OnEndDrag       (wxTreeEvent& event);

    void OnRightClick    (wxMouseEvent& event);
    void OnChar          (wxKeyEvent& event);
    void OnIdle          (wxIdleEvent& event);

    // forwarded notifications (by the frame)
    void OnMenuTest();

    // operations
    void GoTo(const wxString& location);
    void DoRefresh();
    void DeleteSelected();
    void ShowProperties();
    void CreateNewKey(const wxString& strName);
    void CreateNewTextValue(const wxString& strName);
    void CreateNewBinaryValue(const wxString& strName);
    void SetRegistryView(wxRegKey::WOW64ViewMode viewMode);

    // information
    bool IsKeySelected() const;

private:
    // structure describing a registry key/value
    class TreeNode : public wxTreeItemData
    {
        WX_DEFINE_ARRAY_PTR(TreeNode *, TreeChildren);
    public:
        RegTreeCtrl  *m_pTree;     // must be !NULL
        TreeNode     *m_pParent;    // NULL only for the root node
        wxTreeItemId  m_id;         // the id of the tree control item
        wxString      m_strName;    // name of the key/value
        TreeChildren  m_aChildren;  // array of subkeys/values
        bool          m_bKey;       // key or value?
        wxRegKey     *m_pKey;       // only may be !NULL if m_bKey == true
        wxRegKey::WOW64ViewMode m_viewMode; // How to view the registry.

        // trivial accessors
        wxTreeItemId  Id()     const { return m_id;              }
        bool          IsRoot() const { return m_pParent == NULL; }
        bool          IsKey()  const { return m_bKey;            }
        TreeNode     *Parent() const { return m_pParent;         }

        // notifications
        bool OnExpand();
        void OnCollapse();

        // operations
        void Refresh();
        bool DeleteChild(TreeNode *child);
        void DestroyChildren();
        const wxChar *FullName() const;
        void SetRegistryView(wxRegKey::WOW64ViewMode viewMode);

        // get the associated key: make sure the pointer is !NULL
        wxRegKey& Key() { if ( !m_pKey ) OnExpand(); return *m_pKey; }

        // dtor deletes all children
        ~TreeNode();
    };

    wxImageList *m_imageList;
    wxMenu      *m_pMenuPopup;

    TreeNode    *m_pRoot;

    TreeNode    *m_draggedItem;       // the item being dragged
    bool         m_copyOnDrop;        // if false, then move

    bool         m_restoreStatus;     // after OnItemExpanding()

    wxString     m_nameOld;           // the initial value of item being renamed

    wxRegKey::WOW64ViewMode m_viewMode; // Registry view to use for keys.

    TreeNode *GetNode(const wxTreeEvent& event)
        { return (TreeNode *)GetItemData(event.GetItem()); }

public:
    // create a new node and insert it to the tree
    TreeNode *InsertNewTreeNode(TreeNode *pParent,
        const wxString& strName,
        int idImage = RegImageList::ClosedKey,
        const wxString *pstrValue = NULL,
        wxRegKey::WOW64ViewMode viewMode = wxRegKey::WOW64ViewMode_Default);

    // add standard registry keys
    void AddStdKeys();

private:
    DECLARE_EVENT_TABLE()
};

#endif // #if DO_REGTEST

// ----------------------------------------------------------------------------
// the main window of our application
// ----------------------------------------------------------------------------
class RegFrame : public wxFrame
{
public:
    // ctor & dtor
    RegFrame(wxFrame *parent, const wxChar *title, int x, int y, int w, int h);
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

    void OnViewChange (wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

private:

#if DO_REGTEST
    RegTreeCtrl *m_treeCtrl;
#endif
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
    Menu_View,
    Menu_ViewDefault,
    Menu_View32,
    Menu_View64,

    Ctrl_RegTree  = 200
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(RegFrame, wxFrame)
    EVT_MENU(Menu_Test,        RegFrame::OnTest)
    EVT_MENU(Menu_About,       RegFrame::OnAbout)
    EVT_MENU(Menu_Quit,        RegFrame::OnQuit)
    EVT_MENU(Menu_GoTo,        RegFrame::OnGoTo)
    EVT_MENU(Menu_Expand,      RegFrame::OnExpand)
    EVT_MENU(Menu_Collapse,    RegFrame::OnCollapse)
    EVT_MENU(Menu_Toggle,      RegFrame::OnToggle)
    EVT_MENU(Menu_Refresh,     RegFrame::OnRefresh)
    EVT_MENU(Menu_Delete,      RegFrame::OnDelete)
    EVT_MENU(Menu_NewKey,      RegFrame::OnNewKey)
    EVT_MENU(Menu_NewText,     RegFrame::OnNewText)
    EVT_MENU(Menu_NewBinary,   RegFrame::OnNewBinary)
    EVT_MENU(Menu_Info,        RegFrame::OnInfo)
    EVT_MENU(Menu_ViewDefault, RegFrame::OnViewChange)
    EVT_MENU(Menu_View32,      RegFrame::OnViewChange)
    EVT_MENU(Menu_View64,      RegFrame::OnViewChange)
END_EVENT_TABLE()

#if DO_REGTEST

BEGIN_EVENT_TABLE(RegTreeCtrl, wxTreeCtrl)
    EVT_TREE_DELETE_ITEM    (Ctrl_RegTree, RegTreeCtrl::OnDeleteItem)
    EVT_TREE_ITEM_EXPANDING (Ctrl_RegTree, RegTreeCtrl::OnItemExpanding)
    EVT_TREE_ITEM_COLLAPSING(Ctrl_RegTree, RegTreeCtrl::OnItemExpanding)
    EVT_TREE_SEL_CHANGED    (Ctrl_RegTree, RegTreeCtrl::OnSelChanged)

    EVT_TREE_BEGIN_LABEL_EDIT(Ctrl_RegTree, RegTreeCtrl::OnBeginEdit)
    EVT_TREE_END_LABEL_EDIT  (Ctrl_RegTree, RegTreeCtrl::OnEndEdit)

    EVT_TREE_BEGIN_DRAG    (Ctrl_RegTree, RegTreeCtrl::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG   (Ctrl_RegTree, RegTreeCtrl::OnBeginDrag)
    EVT_TREE_END_DRAG      (Ctrl_RegTree, RegTreeCtrl::OnEndDrag)

    EVT_CHAR      (RegTreeCtrl::OnChar)
    EVT_RIGHT_DOWN(RegTreeCtrl::OnRightClick)
    EVT_IDLE      (RegTreeCtrl::OnIdle)
END_EVENT_TABLE()

#endif

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
    pMenuNew->Append(Menu_NewKey,    wxT("&Key"),          wxT("Create a new key"));
    pMenuNew->AppendSeparator();
    pMenuNew->Append(Menu_NewText,   wxT("&Text value"),   wxT("Create a new text value"));
    pMenuNew->Append(Menu_NewBinary, wxT("&Binary value"), wxT("Create a new binary value"));

    wxMenu *pMenuView = new wxMenu;
    pMenuView->AppendRadioItem(
        Menu_ViewDefault,
        wxT("&Default"),
        wxT("Default registry view for the program environment."));
    pMenuView->AppendRadioItem(
        Menu_View32,
        wxT("32-bit Registry"),
        wxT("View 32-bit registry."));
    pMenuView->AppendRadioItem(
        Menu_View64,
        wxT("64-bit Registry"),
        wxT("View 64-bit registry."));

    wxMenu *pMenuReg = new wxMenu;
    pMenuReg->Append(Menu_New, wxT("&New"), pMenuNew);
    pMenuReg->Append(Menu_Delete,   wxT("&Delete..."), wxT("Delete selected key/value"));
    pMenuReg->AppendSeparator();
    pMenuReg->Append(Menu_GoTo,     wxT("&Go to...\tCtrl-G"),    wxT("Go to registry key"));
    pMenuReg->Append(Menu_Expand,   wxT("&Expand"),    wxT("Expand current key"));
    pMenuReg->Append(Menu_Collapse, wxT("&Collapse"),  wxT("Collapse current key"));
    pMenuReg->Append(Menu_Toggle,   wxT("&Toggle"),    wxT("Toggle current key"));
    pMenuReg->AppendSeparator();
    pMenuReg->Append(Menu_Refresh,  wxT("&Refresh"),   wxT("Refresh the subtree"));
    pMenuReg->Append(Menu_View,     wxT("&View"),      pMenuView);
    pMenuReg->AppendSeparator();
    pMenuReg->Append(Menu_Info,     wxT("&Properties"),wxT("Information about current selection"));

    return pMenuReg;
}

// ----------------------------------------------------------------------------
// application class
// ----------------------------------------------------------------------------
IMPLEMENT_APP(RegApp)

// `Main program' equivalent, creating windows and returning main app frame
bool RegApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main frame window and show it
    RegFrame *frame = new RegFrame(NULL, wxT("wxRegTest"), 50, 50, 600, 350);
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// RegFrame
// ----------------------------------------------------------------------------

RegFrame::RegFrame(wxFrame *parent, const wxChar *title, int x, int y, int w, int h)
        : wxFrame(parent, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    // this reduces flicker effects
    SetBackgroundColour(wxColour(255, 255, 255));

    // set the icon
    // ------------
    SetIcon(wxIcon(wxT("app_icon")));

    // create menu
    // -----------
    wxMenu *pMenuFile = new wxMenu;
    pMenuFile->Append(Menu_Test, wxT("Te&st"), wxT("Test key creation"));
    pMenuFile->AppendSeparator();
    pMenuFile->Append(Menu_About, wxT("&About..."), wxT("Show an extraordinarly beautiful dialog"));
    pMenuFile->AppendSeparator();
    pMenuFile->Append(Menu_Quit,  wxT("E&xit"), wxT("Quit this program"));

    wxMenuBar *pMenu = new wxMenuBar;
    pMenu->Append(pMenuFile, wxT("&File"));
    pMenu->Append(CreateRegistryMenu(),  wxT("&Registry"));
    SetMenuBar(pMenu);

#if DO_REGTEST
    // create child controls
    // ---------------------
    m_treeCtrl = new RegTreeCtrl(this, Ctrl_RegTree);
#endif

#if wxUSE_STATUSBAR
    // create the status line
    // ----------------------
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR
}

RegFrame::~RegFrame()
{
#if DO_REGTEST
    // this makes deletion of it *much* quicker
    m_treeCtrl->Hide();
#endif
}

void RegFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void RegFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dialog(this,
        wxT("wxRegistry sample\n")
        wxT("(c) 1998, 2000 Vadim Zeitlin"),
        wxT("About wxRegTest"), wxOK);

    dialog.ShowModal();
}

void RegFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->OnMenuTest();
#endif
}

void RegFrame::OnGoTo(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_location = wxT("HKEY_CURRENT_USER\\Software\\wxWidgets");

    wxString location = wxGetTextFromUser(
        wxT("Enter the location to go to:"),
        wxT("wxRegTest question"),
        s_location,
        this);

    if ( !location )
        return;

    s_location = location;
#if DO_REGTEST
    m_treeCtrl->GoTo(location);
#endif
}

void RegFrame::OnExpand(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->Expand(m_treeCtrl->GetSelection());
#endif
}

void RegFrame::OnCollapse(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->Collapse(m_treeCtrl->GetSelection());
#endif
}

void RegFrame::OnToggle(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->Toggle(m_treeCtrl->GetSelection());
#endif
}

void RegFrame::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->DoRefresh();
#endif
}

void RegFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->DeleteSelected();
#endif
}

void RegFrame::OnNewKey(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    if ( m_treeCtrl->IsKeySelected() )
    {
        m_treeCtrl->CreateNewKey(
            wxGetTextFromUser(wxT("Enter the name of the new key")));
    }
#endif
}

void RegFrame::OnNewText(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    if ( m_treeCtrl->IsKeySelected() )
    {
        m_treeCtrl->CreateNewTextValue(
            wxGetTextFromUser(wxT("Enter the name for the new text value")));
    }
#endif
}

void RegFrame::OnNewBinary(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    if ( m_treeCtrl->IsKeySelected() )
    {
        m_treeCtrl->CreateNewBinaryValue(
            wxGetTextFromUser(wxT("Enter the name for the new binary value")));
    }
#endif
}

void RegFrame::OnInfo(wxCommandEvent& WXUNUSED(event))
{
#if DO_REGTEST
    m_treeCtrl->ShowProperties();
#endif
}

void RegFrame::OnViewChange(wxCommandEvent& event)
{
#if DO_REGTEST
    wxRegKey::WOW64ViewMode view;
    switch ( event.GetId() )
    {
        case Menu_ViewDefault:
            view = wxRegKey::WOW64ViewMode_Default;
            break;

        case Menu_View32:
            view = wxRegKey::WOW64ViewMode_32;
            break;

        case Menu_View64:
            view = wxRegKey::WOW64ViewMode_64;
            break;

        default:
            wxFAIL_MSG("Unexpected event source for view change.");
            return;
    }

    m_treeCtrl->SetRegistryView(view);
#endif
}

// ----------------------------------------------------------------------------
// RegImageList
// ----------------------------------------------------------------------------
RegImageList::RegImageList() : wxImageList(16, 16, true)
{
    // should be in sync with enum RegImageList::RegIcon
    static const wxChar *aszIcons[] = { wxT("key1"),wxT("key2"),wxT("key3"),wxT("value1"),wxT("value2") };
    wxString str = wxT("icon_");
    for ( unsigned int n = 0; n < WXSIZEOF(aszIcons); n++ )
    {
        Add(wxIcon(str + aszIcons[n], wxBITMAP_TYPE_ICO_RESOURCE));
    }
}

#if DO_REGTEST

// ----------------------------------------------------------------------------
// RegTreeCtrl
// ----------------------------------------------------------------------------

// create a new tree item and insert it into the tree
RegTreeCtrl::TreeNode *RegTreeCtrl::InsertNewTreeNode(
    TreeNode *pParent,
    const wxString& strName,
    int idImage,
    const wxString *pstrValue,
    wxRegKey::WOW64ViewMode viewMode)
{
    // create new item & insert it
    TreeNode *pNewNode = new TreeNode;
    pNewNode->m_pTree  = this;
    pNewNode->m_pParent = pParent;
    pNewNode->m_strName = strName;
    pNewNode->m_bKey    = pstrValue == NULL;
    pNewNode->m_pKey    = NULL;
    pNewNode->m_viewMode = viewMode;
    if (pParent)
    {
        pNewNode->m_id  = AppendItem(pParent->Id(),
            pNewNode->IsKey() ? strName : *pstrValue,
            idImage);
    }
    else
    {
        pNewNode->m_id  = AddRoot(strName);
    }

    wxASSERT_MSG( pNewNode->m_id, wxT("can't create tree control item!"));

    // save the pointer in the item
    SetItemData(pNewNode->m_id, pNewNode);

    // add it to the list of parent's children
    if ( pParent != NULL )
    {
        pParent->m_aChildren.Add(pNewNode);
    }

    if ( pNewNode->IsKey() )
    {
        SetItemHasChildren(pNewNode->Id());

        if ( !pNewNode->IsRoot() )
        {
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
    m_restoreStatus = false;
    m_viewMode = wxRegKey::WOW64ViewMode_Default;

    // create the image list
    // ---------------------
    m_imageList = new RegImageList;
    SetImageList(m_imageList);

    // create root keys
    // ----------------
    m_pRoot =
        InsertNewTreeNode(
            NULL,
            wxT("Registry Root"),
            RegImageList::Root,
            NULL,
            m_viewMode);

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
    for ( unsigned int ui = 0; ui < wxRegKey::nStdKeys; ui++ )
    {
        InsertNewTreeNode(
            m_pRoot,
            wxRegKey::GetStdKeyName(ui),
            RegImageList::ClosedKey,
            NULL,
            m_viewMode);
    }
}

// ----------------------------------------------------------------------------
// notifications
// ----------------------------------------------------------------------------

void RegTreeCtrl::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    if ( m_restoreStatus )
    {
        // restore it after OnItemExpanding()
        wxLogStatus(wxT("Ok"));
        wxSetCursor(*wxSTANDARD_CURSOR);

        m_restoreStatus = false;
    }
}

void RegTreeCtrl::OnRightClick(wxMouseEvent& event)
{
    int iFlags;
    wxTreeItemId lId = HitTest(wxPoint(event.GetX(), event.GetY()), iFlags);
    if ( iFlags & wxTREE_HITTEST_ONITEMLABEL )
    {
        // select the item first
        SelectItem(lId);
    }
    //else: take the currently selected item if click not on item

    PopupMenu(m_pMenuPopup, event.GetX(), event.GetY());
}


void RegTreeCtrl::OnDeleteItem(wxTreeEvent& WXUNUSED(event))
{
}

// test the key creation functions
void RegTreeCtrl::OnMenuTest()
{
    wxTreeItemId lId = GetSelection();
    TreeNode *pNode = (TreeNode *)GetItemData(lId);

    wxCHECK_RET( pNode != NULL, wxT("tree item without data?") );

    if ( pNode->IsRoot() )
    {
        wxLogError(wxT("Can't create a subkey under the root key."));
        return;
    }

    if ( !pNode->IsKey() )
    {
        wxLogError(wxT("Can't create a subkey under a value!"));
        return;
    }

    wxRegKey key1(pNode->Key(), wxT("key1"));
    if ( key1.Create() )
    {
        wxRegKey key2a(key1, wxT("key2a")), key2b(key1, wxT("key2b"));
        if ( key2a.Create() && key2b.Create() )
        {
            // put some values under the newly created keys
            key1.SetValue(wxT("first_term"), wxT("10"));
            key1.SetValue(wxT("second_term"), wxT("7"));
            key2a = wxT("this is the unnamed value");
            key2b.SetValue(wxT("sum"), 17);

            // refresh tree
            pNode->Refresh();
            wxLogStatus(wxT("Test keys successfully added."));
            return;
        }
    }

    wxLogError(wxT("Creation of test keys failed."));
}

void RegTreeCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
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
#if wxUSE_STATUSBAR
    wxFrame *pFrame = (wxFrame *) wxWindow::GetParent();
    pFrame->SetStatusText(GetNode(event)->FullName(), 1);
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void RegTreeCtrl::OnItemExpanding(wxTreeEvent& event)
{
    TreeNode *pNode = GetNode(event);
    bool bExpanding = event.GetEventType() == wxEVT_COMMAND_TREE_ITEM_EXPANDING;

    // expansion might take some time
    wxSetCursor(*wxHOURGLASS_CURSOR);
    wxLogStatus(wxT("Working..."));
    wxYield();  // to give the status line a chance to refresh itself
    m_restoreStatus = true;   // some time later...

    if ( pNode->IsKey() )
    {
        if ( bExpanding )
        {
            // expanding: add subkeys/values
            if ( !pNode->OnExpand() )
                return;
        }
        else
        {
            // collapsing: clean up
            pNode->OnCollapse();
        }
    }
}

void RegTreeCtrl::OnBeginEdit(wxTreeEvent& event)
{
    TreeNode *pNode = GetNode(event);
    if ( pNode->IsRoot() || pNode->Parent()->IsRoot() )
    {
        wxLogStatus(wxT("This registry key can't be renamed."));

        event.Veto();
    }
    else
    {
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

    if ( !ok )
    {
        wxLogError(wxT("Failed to rename '%s' to '%s'."),
            m_nameOld.c_str(), name.c_str());
    }
#if 0   // MSW tree ctrl doesn't like this at all, it hangs
    else
    {
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
        wxLogStatus(wxT("This registry key can't be %s."),
            m_copyOnDrop ? wxT("copied") : wxT("moved"));
    }
    else
    {
        wxLogStatus(wxT("%s item %s..."),
            m_copyOnDrop ? wxT("Copying") : wxT("Moving"),
            pNode->FullName());

        m_draggedItem = pNode;

        event.Allow();
    }
}

void RegTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
    wxCHECK_RET( m_draggedItem, wxT("end drag without begin drag?") );

    // clear the pointer anyhow
    TreeNode *src = m_draggedItem;
    m_draggedItem = NULL;

    // where are we going to drop it?
    TreeNode *dst = GetNode(event);
    if ( dst && !dst->IsKey() )
    {
        // we need a parent key
        dst = dst->Parent();
    }

    if ( !dst || dst->IsRoot() )
    {
        wxLogError(wxT("Can't create a key here."));

        return;
    }

    bool isKey = src->IsKey();
    if ( (isKey && (src == dst)) ||
         (!isKey && (dst->Parent() == src)) ) {
        wxLogStatus(wxT("Can't copy something on itself"));

        return;
    }

    // remove the "Registry Root\\" from the full name
    wxString nameSrc, nameDst;
    nameSrc << wxString(src->FullName()).AfterFirst('\\');
    nameDst << wxString(dst->FullName()).AfterFirst('\\') << '\\'
            << wxString(src->FullName()).AfterLast('\\');

    wxString verb = m_copyOnDrop ? wxT("copy") : wxT("move");
    wxString what = isKey ? wxT("key") : wxT("value");

    if ( wxMessageBox(wxString::Format
                        (
                         wxT("Do you really want to %s the %s %s to %s?"),
                         verb.c_str(),
                         what.c_str(),
                         nameSrc.c_str(),
                         nameDst.c_str()
                        ),
                      wxT("RegTest Confirm"),
                      wxICON_QUESTION | wxYES_NO | wxCANCEL, this) != wxYES ) {
      return;
    }

    bool ok;
    if ( isKey )
    {
        wxRegKey& key = src->Key();
        wxRegKey keyDst(dst->Key(), src->m_strName);
        ok = keyDst.Create(false);
        if ( !ok )
        {
            wxLogError(wxT("Key '%s' already exists"), keyDst.GetName().c_str());
        }
        else
        {
            ok = key.Copy(keyDst);
        }

        if ( ok && !m_copyOnDrop )
        {
            // delete the old key
            ok = key.DeleteSelf();
            if ( ok )
            {
                src->Parent()->Refresh();
            }
        }
    }
    else // value
    {
        wxRegKey& key = src->Parent()->Key();
        ok = key.CopyValue(src->m_strName, dst->Key());
        if ( ok && !m_copyOnDrop )
        {
            // we moved it, so delete the old one
            ok = key.DeleteValue(src->m_strName);
        }
    }

    if ( !ok )
    {
        wxLogError(wxT("Failed to %s registry %s."),
                   verb.c_str(), what.c_str());
    }
    else
    {
        dst->Refresh();
    }
}

// ----------------------------------------------------------------------------
// TreeNode implementation
// ----------------------------------------------------------------------------
bool RegTreeCtrl::TreeNode::OnExpand()
{
    // we add children only once
    if ( !m_aChildren.IsEmpty() )
    {
        // we've been already expanded
        return true;
    }

    if ( IsRoot() )
    {
        // we're the root key
        m_pTree->AddStdKeys();
        return true;
    }

    if ( Parent()->IsRoot() )
    {
        // we're a standard key
        m_pKey = new wxRegKey(m_strName, m_viewMode);
    }
    else
    {
        // we're a normal key
        m_pKey = new wxRegKey(*(Parent()->m_pKey), m_strName);
    }

    if ( !m_pKey->Open() )
    {
        wxLogError(wxT("The key '%s' can't be opened."), FullName());
        return false;
    }

    // if we're empty, we shouldn't be expandable at all
    bool isEmpty = true;

    // enumeration variables
    long l;
    wxString str;
    bool bCont;

    // enumerate all subkeys
    bCont = m_pKey->GetFirstKey(str, l);
    while ( bCont )
    {
        m_pTree->InsertNewTreeNode(
            this,
            str,
            RegImageList::ClosedKey,
            NULL,
            m_viewMode);
        bCont = m_pKey->GetNextKey(str, l);

        // we have at least this key...
        isEmpty = false;
    }

    // enumerate all values
    bCont = m_pKey->GetFirstValue(str, l);
    while ( bCont )
    {
        wxString strItem;
        if (str.empty())
            strItem = wxT("<default>");
        else
            strItem = str;
        strItem += wxT(" = ");

        // determine the appropriate icon
        RegImageList::Icon icon;
        switch ( m_pKey->GetValueType(str) )
        {
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

        m_pTree->InsertNewTreeNode(this, str, icon, &strItem, m_viewMode);
        bCont = m_pKey->GetNextValue(str, l);

        // we have at least this value...
        isEmpty = false;
    }

    if ( isEmpty )
    {
        // this is for the case when our last child was just deleted
        wxTreeItemId theId(Id()); // Temp variable seems necessary for BC++
        m_pTree->Collapse(theId);

        // we won't be expanded any more
        m_pTree->SetItemHasChildren(theId, false);
    }

    return true;
}

void RegTreeCtrl::TreeNode::OnCollapse()
{
    DestroyChildren();

    wxDELETE(m_pKey);
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
    if ( wasExpanded )
    {
        m_pTree->Expand(theId);
        OnExpand();
    }
}

bool RegTreeCtrl::TreeNode::DeleteChild(TreeNode *child)
{
    int index = m_aChildren.Index(child);
    wxCHECK_MSG( index != wxNOT_FOUND, false,
                 wxT("our child in tree should be in m_aChildren") );

    m_aChildren.RemoveAt((size_t)index);

    bool ok;
    if ( child->IsKey() )
    {
        // must close key before deleting it
        child->OnCollapse();

        ok = Key().DeleteKey(child->m_strName);
    }
    else
    {
        ok = Key().DeleteValue(child->m_strName);
    }

    if ( ok )
    {
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
    for ( size_t n = 0; n < nCount; n++ )
    {
        wxTreeItemId lId = m_aChildren[n]->Id();
        m_pTree->Delete(lId);
    }

    m_aChildren.Empty();
}

RegTreeCtrl::TreeNode::~TreeNode()
{
    delete m_pKey;
}

const wxChar *RegTreeCtrl::TreeNode::FullName() const
{
    static wxString s_strName;

    if ( IsRoot() )
    {
        return wxT("Registry Root");
    }
    else
    {
        // our own registry key might not (yet) exist or we might be a value,
        // so just use the parent's and concatenate
        s_strName = Parent()->FullName();
        s_strName << wxT('\\') << m_strName;

        return s_strName;
    }
}

void RegTreeCtrl::TreeNode::SetRegistryView(wxRegKey::WOW64ViewMode viewMode)
{
    m_viewMode = viewMode;

    // Update children with new view.
    size_t nCount = m_aChildren.GetCount();
    for (size_t n = 0; n < nCount; n++)
        m_aChildren[n]->SetRegistryView(viewMode);
}

// ----------------------------------------------------------------------------
// operations on RegTreeCtrl
// ----------------------------------------------------------------------------

void RegTreeCtrl::GoTo(const wxString& location)
{
    wxStringTokenizer tk(location, wxT("\\"));

    wxTreeItemId id = GetRootItem();

    while ( tk.HasMoreTokens() )
    {
        wxString subkey = tk.GetNextToken();

        wxTreeItemId idCurrent = id;
        if ( !IsExpanded(idCurrent) )
            Expand(idCurrent);

        wxTreeItemIdValue dummy;
        id = GetFirstChild(idCurrent, dummy);

        if ( idCurrent == GetRootItem() )
        {
            // special case: we understand both HKCU and HKEY_CURRENT_USER here
            for ( size_t key = 0; key < wxRegKey::nStdKeys; key++ )
            {
                if ( subkey == wxRegKey::GetStdKeyName(key)
                     || subkey == wxRegKey::GetStdKeyShortName(key) )
                {
                    break;
                }

                id = GetNextChild(idCurrent, dummy);
            }
        }
        else
        {
            // enum all children
            while ( id.IsOk() )
            {
                if ( subkey == ((TreeNode *)GetItemData(id))->m_strName )
                    break;

                id = GetNextChild(idCurrent, dummy);
            }
        }

        if ( !id.IsOk() )
        {
            wxLogError(wxT("No such key '%s'."), location.c_str());

            return;
        }
    }

    if ( id.IsOk() )
        SelectItem(id);
}

void RegTreeCtrl::DeleteSelected()
{
    wxTreeItemId lCurrent = GetSelection(),
                 lParent  = GetItemParent(lCurrent);

    if ( lParent == GetRootItem() )
    {
        wxLogError(wxT("Can't delete root key."));
        return;
    }

    TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent),
             *pParent  = (TreeNode *)GetItemData(lParent);

    wxCHECK_RET(pCurrent && pParent, wxT("either node or parent without data?"));

    if ( pParent->IsRoot() )
    {
        wxLogError(wxT("Can't delete standard key."));
        return;
    }

    wxString what = pCurrent->IsKey() ? wxT("key") : wxT("value");
    if ( wxMessageBox(wxString::Format
        (
            wxT("Do you really want to delete this %s?"),
            what.c_str()
        ),
        wxT("Confirmation"),
        wxICON_QUESTION | wxYES_NO | wxCANCEL, this) != wxYES )
    {
        return;
    }

    pParent->DeleteChild(pCurrent);
}

void RegTreeCtrl::CreateNewKey(const wxString& strName)
{
    wxTreeItemId lCurrent = GetSelection();
    TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

    wxCHECK_RET( pCurrent != NULL, wxT("node without data?") );

    wxASSERT( pCurrent->IsKey() );  // check must have been done before

    if ( pCurrent->IsRoot() )
    {
        wxLogError(wxT("Can't create a new key under the root key."));
        return;
    }

    wxRegKey key(pCurrent->Key(), strName);
    if ( key.Create() )
        pCurrent->Refresh();
}

void RegTreeCtrl::CreateNewTextValue(const wxString& strName)
{
    wxTreeItemId lCurrent = GetSelection();
    TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

    wxCHECK_RET( pCurrent != NULL, wxT("node without data?") );

    wxASSERT( pCurrent->IsKey() );  // check must have been done before

    if ( pCurrent->IsRoot() )
    {
        wxLogError(wxT("Can't create a new value under the root key."));
        return;
    }

    if ( pCurrent->Key().SetValue(strName, wxEmptyString) )
        pCurrent->Refresh();
}

void RegTreeCtrl::CreateNewBinaryValue(const wxString& strName)
{
    wxTreeItemId lCurrent = GetSelection();
    TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

    wxCHECK_RET( pCurrent != NULL, wxT("node without data?") );

    wxASSERT( pCurrent->IsKey() );  // check must have been done before

    if ( pCurrent->IsRoot() )
    {
        wxLogError(wxT("Can't create a new value under the root key."));
        return;
    }

    if ( pCurrent->Key().SetValue(strName, 0) )
        pCurrent->Refresh();
}

void RegTreeCtrl::SetRegistryView(wxRegKey::WOW64ViewMode viewMode)
{
    m_viewMode = viewMode;
    m_pRoot->SetRegistryView(viewMode);
    m_pRoot->Refresh();
}

void RegTreeCtrl::ShowProperties()
{
    wxTreeItemId lCurrent = GetSelection();
    TreeNode *pCurrent = (TreeNode *)GetItemData(lCurrent);

    if ( !pCurrent || pCurrent->IsRoot() )
    {
        wxLogStatus(wxT("No properties"));

        return;
    }

    if ( pCurrent->IsKey() )
    {
        const wxRegKey& key = pCurrent->Key();
        size_t nSubKeys, nValues;
        if ( !key.GetKeyInfo(&nSubKeys, NULL, &nValues, NULL) )
        {
            wxLogError(wxT("Couldn't get key info"));
        }
        else
        {
            wxLogMessage(wxT("Key '%s' has %u subkeys and %u values."),
                         key.GetName().c_str(), nSubKeys, nValues);
        }
    }
    else // it's a value
    {
        TreeNode *parent = pCurrent->Parent();
        wxCHECK_RET( parent, wxT("reg value without key?") );

        const wxRegKey& key = parent->Key();
        const wxChar *value = pCurrent->m_strName.c_str();
        wxLogMessage(wxT("Value '%s' under the key '%s' is of type ")
            wxT("%d (%s)."),
            value,
            parent->m_strName.c_str(),
            key.GetValueType(value),
            key.IsNumericValue(value) ? wxT("numeric") : wxT("string"));

    }
}

bool RegTreeCtrl::IsKeySelected() const
{
    wxTreeItemId lCurrent = GetSelection();
    TreeNode *pCurrent = (TreeNode *) GetItemData(lCurrent);

    wxCHECK( pCurrent != NULL, false );

    return pCurrent->IsKey();
}

void RegTreeCtrl::DoRefresh()
{
    wxTreeItemId lId = GetSelection();
    if ( !lId )
        return;

    TreeNode *pNode = (TreeNode *) GetItemData(lId);

    wxCHECK_RET( pNode != NULL, wxT("tree item without data?") );

    pNode->Refresh();
}

#endif
