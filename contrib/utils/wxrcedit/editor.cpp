/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "editor.h"
    #pragma implementation "treedt.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/xrc/xmlres.h"
#include "wx/splitter.h"
#include "wx/config.h"
#include "wx/dir.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"

#include "treedt.h"
#include "editor.h"
#include "nodehnd.h"
#include "xmlhelpr.h"
#include "preview.h"
#include "propframe.h"


void wxXmlRcEditDocument::UpgradeNodeValue(wxXmlNode *node)
{
    wxXmlNode *n = node;
    if (n == NULL) return;
    n = n->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_TEXT_NODE ||
            n->GetType() == wxXML_CDATA_SECTION_NODE)
        {
            wxString str1 = n->GetContent();
            const wxChar *dt;

            for (dt = str1.c_str(); *dt; dt++)
            {
                // Remap amp_char to &, map double amp_char to amp_char (for things
                // like "&File..." -- this is illegal in XML, so we use "_File..."):
                if (*dt == '$')
                {
                    if ( *(++dt) != '$' )
                        str1[size_t(dt-str1.c_str()-1)] = '_';
                }
            }
            n->SetContent(str1);
        }
        n = n->GetNext();
    }
}

void wxXmlRcEditDocument::UpgradeNode(wxXmlNode *node)
{
    if (node)
    {
        UpgradeNodeValue(node);
        UpgradeNode(node->GetNext());
        UpgradeNode(node->GetChildren());
    }
}

void wxXmlRcEditDocument::Upgrade()
{
    int v1,v2,v3,v4;
    long version;
    wxXmlNode *node = GetRoot();
    wxString verstr = wxT("0.0.0.0");
    node->GetPropVal(wxT("version"),verstr);
    if (wxSscanf(verstr.c_str(), wxT("%i.%i.%i.%i"),
        &v1, &v2, &v3, &v4) == 4)
        version = v1*256*256*256+v2*256*256+v3*256+v4;
    else
        version = 0;
    if (!version)
    {
        UpgradeNode(node);
    }
    node->DeleteProperty(wxT("version"));
    node->AddProperty(wxT("version"), WX_XMLRES_CURRENT_VERSION_STRING);
}


class EditorTreeCtrl : public wxTreeCtrl
{
    public:
        EditorTreeCtrl(wxWindow *parent, int id, EditorFrame *frame)
                 : wxTreeCtrl(parent, id), m_EdFrame(frame) {}

    private:
        EditorFrame *m_EdFrame;

        void OnRightClick(wxMouseEvent &event)
        {
            wxTreeItemId item =
                m_EdFrame->m_TreeCtrl->HitTest(event.GetPosition());
            if (item.IsOk())
            {
                m_EdFrame->m_TreeCtrl->SelectItem(item);
                m_EdFrame->OnRightClickTree(event.GetPosition());
            }
        }
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(EditorTreeCtrl, wxTreeCtrl)
    EVT_RIGHT_DOWN(EditorTreeCtrl::OnRightClick)
END_EVENT_TABLE()


enum
{
    ID_PREVIEW = wxID_HIGHEST + 100,
    ID_NEW,
    ID_OPEN,
    ID_CLOSE,
    ID_SAVE,
    ID_SAVEAS,
    ID_DELETE_NODE,
    ID_EXIT,
    ID_TREE,

    ID_CUT,
    ID_PASTE_SYBLING,
    ID_PASTE_CHILD,
    ID_COPY,

    ID_NEWDIALOG,
    ID_NEWFRAME,
    ID_NEWPANEL,
    ID_NEWMENU,
    ID_NEWMENUBAR,
    ID_NEWTOOLBAR,
    ID_NEWNODE = wxID_HIGHEST + 10000, // safely out of XRCID range :)
    ID_NEWSYBNODE = ID_NEWNODE + 20000
};





BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_TREE_SEL_CHANGED(ID_TREE, EditorFrame::OnTreeSel)
    EVT_TOOL_RANGE(ID_PREVIEW, ID_EXIT, EditorFrame::OnToolbar)
    EVT_MENU_RANGE(ID_NEWDIALOG, ID_NEWSYBNODE + 1000, EditorFrame::OnNewNode)
    EVT_MENU_RANGE(ID_CUT, ID_COPY, EditorFrame::OnClipboardAction)
    EVT_CLOSE(EditorFrame::OnCloseWindow)
END_EVENT_TABLE()



#if defined(__UNIX__)
#include "bitmaps/preview.xpm"
#include "bitmaps/close.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/open.xpm"

#include "bitmaps/control.xpm"
#include "bitmaps/vsizer.xpm"
#include "bitmaps/hsizer.xpm"
#include "bitmaps/panel.xpm"
#include "bitmaps/gsizer.xpm"
#include "bitmaps/resicon.xpm"
#endif



EditorFrame *EditorFrame::ms_Instance = NULL;

EditorFrame::EditorFrame(wxFrame *parent, const wxString& filename)
    : wxFrame(parent, wxID_ANY, filename + _("- wxWidgets resources editor"))
{
    ms_Instance = this;

    m_Clipboard = NULL;
    m_Modified = false;

    wxConfigBase *cfg = wxConfigBase::Get();

    SetSize(wxRect(wxPoint(cfg->Read(_T("editor_x"), wxDefaultCoord), cfg->Read(_T("editor_y"), wxDefaultCoord)),
            wxSize(cfg->Read(_T("editor_w"), 400), cfg->Read(_T("editor_h"), 400))));

    m_SelectedNode = NULL;
    m_Resource = NULL;
    m_FileName = wxEmptyString;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_NEW, _T("&New"));
    menuFile->Append(ID_OPEN, _T("&Open\tCtrl-O"));
    menuFile->Append(ID_SAVE, _T("&Save\tCtrl-S"));
    menuFile->Append(ID_SAVEAS, _T("Save &as..."));
    menuFile->AppendSeparator();
    menuFile->Append(ID_EXIT, _T("E&xit\tAlt-X"));

    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append(ID_CUT, _T("Cut\tCtrl-X"));
    menuEdit->Append(ID_COPY, _T("Copy\tCtrl-C"));
    menuEdit->Append(ID_PASTE_SYBLING, _T("Paste as sybling\tCtrl-V"));
    menuEdit->Append(ID_PASTE_CHILD, _T("Paste as child"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_DELETE_NODE,  _T("Delete"));

    menuEdit->Enable(ID_PASTE_SYBLING, false);
    menuEdit->Enable(ID_PASTE_CHILD, false);

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuEdit, _T("&Edit"));
    SetMenuBar(menuBar);

    // Create toolbar:
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT);
    toolBar->SetMargins(2, 2);
    toolBar->SetToolBitmapSize(wxSize(24, 24));
    toolBar -> AddTool(ID_EXIT, wxBITMAP(close), wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Quit the editor"));
    toolBar -> AddTool(ID_OPEN, wxBITMAP(open), wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Open XML resource file"));
    toolBar -> AddTool(ID_SAVE, wxBITMAP(save), wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Save XML file"));
    toolBar -> AddTool(ID_PREVIEW, wxBITMAP(preview), wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Preview"));
    toolBar -> Realize();

    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

    // Create tree control:
    m_TreeCtrl = new EditorTreeCtrl(this, ID_TREE, this);
    wxImageList *imgList = new wxImageList(16, 16);
    imgList->Add(wxICON(control));
    imgList->Add(wxICON(panel));
    imgList->Add(wxICON(vsizer));
    imgList->Add(wxICON(hsizer));
    imgList->Add(wxICON(gsizer));
    imgList->Add(wxICON(resicon));
    m_TreeCtrl->AssignImageList(imgList);
    sizer->Add(m_TreeCtrl, 1, wxEXPAND);

    SetSizer(sizer);

    // Load file:
    if (!filename)
        NewFile();
    else
        LoadFile(filename);
}



EditorFrame::~EditorFrame()
{
    PreviewFrame::Get()->Close();
    PropertiesFrame::Get()->Close();

    wxConfigBase *cfg = wxConfigBase::Get();

    cfg->Write(_T("editor_x"), (long)GetPosition().x);
    cfg->Write(_T("editor_y"), (long)GetPosition().y);
    cfg->Write(_T("editor_w"), (long)GetSize().x);
    cfg->Write(_T("editor_h"), (long)GetSize().y);

    delete m_Clipboard;
}




void EditorFrame::LoadFile(const wxString& filename)
{
    if (!AskToSave()) return;

    delete m_Resource;

     // create new resource in order to handle version differences properly
    PreviewFrame::Get()->ResetResource();

    m_FileName = wxEmptyString;
    m_Resource = new wxXmlRcEditDocument;
    m_Modified = false;

    if (!m_Resource->Load(filename,  wxLocale::GetSystemEncodingName()))
    {
        delete m_Resource;
        m_Resource = NULL;
        NewFile();
        wxLogError(_T("Error parsing ") + filename);
    }
    else
    {
        m_FileName = filename;

        // Upgrades old versions
        m_Resource->Upgrade();
        RefreshTree();
    }
    RefreshTitle();
}



void EditorFrame::SaveFile(const wxString& filename)
{
    m_FileName = filename;

    // save it:
    if (!m_Resource->Save(filename))
        wxLogError(_("Error saving ") + filename);
    else
        m_Modified = false;

    RefreshTitle();
}



void EditorFrame::NewFile()
{
    if (!AskToSave()) return;

    delete m_Resource;

    m_FileName = wxEmptyString;
    m_Resource = new wxXmlRcEditDocument;
    m_Resource->SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, _("resource")));

    m_Resource->SetFileEncoding(_T("utf-8"));
#if !wxUSE_UNICODE
    m_Resource->SetEncoding(wxLocale::GetSystemEncodingName());
#endif

    m_Resource->GetRoot()->AddProperty(_T("version"),
                                       WX_XMLRES_CURRENT_VERSION_STRING);

    m_Modified = false;
    RefreshTree();
    RefreshTitle();
}



void EditorFrame::RefreshTitle()
{
    wxString s;
    if (m_Modified) s << _T("* ");
    s << _("wxrcedit");
    if (m_FileName != wxEmptyString)
        s << _T(" - ") << wxFileNameFromPath(m_FileName);
    SetTitle(s);
}



void EditorFrame::RefreshTree()
{
    wxXmlNode *sel = m_SelectedNode;

    m_TreeCtrl->DeleteAllItems();

    wxTreeItemId root = m_TreeCtrl->AddRoot(_T("Resource: ") + wxFileNameFromPath(m_FileName), 5, 5);

    wxXmlNode *n = m_Resource->GetRoot()->GetChildren();
    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE)
            CreateTreeNode(m_TreeCtrl, root, n);
        n = n->GetNext();
    }

    m_TreeCtrl->Expand(root);
    SelectNode(sel);
}




static void RecursivelyExpand(wxTreeCtrl *t, wxTreeItemId item)
{
    t->Expand(item);
    void* cookie;
    wxTreeItemId id = t->GetFirstChild(item, cookie);
    while (id.IsOk())
    {
        RecursivelyExpand(t, id);
        id = t->GetNextChild(item, cookie);
    }
}

bool EditorFrame::SelectNode(wxXmlNode *node, wxTreeItemId *root)
{
    if (root == NULL)
    {
        wxTreeItemId rootitem = m_TreeCtrl->GetRootItem();
        return SelectNode(node, &rootitem);
    }

    wxTreeItemId item;
    XmlTreeData *dt;
    wxXmlNode *nd;
    void* cookie;

    item = m_TreeCtrl->GetFirstChild(*root, cookie);
    while (item.IsOk())
    {
        dt = (XmlTreeData*)(m_TreeCtrl->GetItemData(item));
        nd = (dt) ? dt->Node : NULL;
        if (nd == node)
        {
            RecursivelyExpand(m_TreeCtrl, *root);
            m_TreeCtrl->SelectItem(item);
            m_TreeCtrl->EnsureVisible(item);
            return true;
        }
        if (m_TreeCtrl->ItemHasChildren(item) && SelectNode(node, &item))
            return true;
        item = m_TreeCtrl->GetNextChild(*root, cookie);
    }

    return false;
}



wxTreeItemId EditorFrame::CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent, wxXmlNode *node)
{
    if (!node)
    {
        wxTreeItemId invalid;
        return invalid;
    }

    return NodeHandler::Find(node)->CreateTreeNode(treectrl, parent, node);
}



void EditorFrame::NotifyChanged(int change_type)
{
    if (change_type & CHANGED_TREE)
        RefreshTree();

    if (change_type & CHANGED_TREE_SELECTED)
    {
        wxTreeItemId sel = m_TreeCtrl->GetSelection();
        m_TreeCtrl->SetItemText(sel,
             NodeHandler::Find(m_SelectedNode)->GetTreeString(m_SelectedNode));
    }

    if (change_type & CHANGED_TREE_SELECTED_ICON)
    {
        wxTreeItemId sel = m_TreeCtrl->GetSelection();
        int icon = NodeHandler::Find(m_SelectedNode)->GetTreeIcon(m_SelectedNode);
        m_TreeCtrl->SetItemImage(sel, icon);
    }

    if (!m_Modified)
    {
        m_Modified = true;
        RefreshTitle();
    }

    PreviewFrame::Get()->MakeDirty();
}



void EditorFrame::OnTreeSel(wxTreeEvent& event)
{
    XmlTreeData *dt = (XmlTreeData*)(m_TreeCtrl->GetItemData(event.GetItem()));
    wxXmlNode *node = (dt) ? dt->Node : NULL;

    m_SelectedNode = node;
    if (node)
        PropertiesFrame::Get()->ShowProps(node);

    if (m_TreeCtrl->GetItemParent(event.GetItem()) == m_TreeCtrl->GetRootItem())
    {
        wxTreeItemId it = event.GetOldItem();

        if (it.IsOk() && m_TreeCtrl->GetRootItem() != it)
        {
            while (m_TreeCtrl->GetItemParent(it) != m_TreeCtrl->GetRootItem())
                it = m_TreeCtrl->GetItemParent(it);
            m_TreeCtrl->Collapse(it);
        }
        RecursivelyExpand(m_TreeCtrl, event.GetItem());

        PreviewFrame::Get()->Preview(node,m_Resource);
    }
}



void EditorFrame::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case ID_PREVIEW :
            {
            XmlTreeData* dt = (XmlTreeData*)m_TreeCtrl->GetItemData(m_TreeCtrl->GetSelection());;
            if (dt != NULL && dt->Node != NULL)
                PreviewFrame::Get()->Preview(dt->Node, m_Resource);
            break;
            }

        case ID_EXIT :
            Close(true);
            break;

        case ID_NEW :
            NewFile();
            break;

        case ID_OPEN :
            {
            wxString cwd = wxGetCwd(); // workaround for 2.2
            wxString name = wxFileSelector(_("Open XML resource"), wxEmptyString, wxEmptyString, wxEmptyString, _("XML resources (*.xrc)|*.xrc"), wxOPEN | wxFILE_MUST_EXIST);
            wxSetWorkingDirectory(cwd);
            if (!name.IsEmpty())
                LoadFile(name);
            break;
            }

        case ID_SAVE :
            if (m_FileName != wxEmptyString) { SaveFile(m_FileName); break;}
            // else go to SAVEAS

        case ID_SAVEAS :
            {
            wxString cwd = wxGetCwd(); // workaround for 2.2
            wxString name = wxFileSelector(_("Save as"), wxEmptyString, m_FileName, wxEmptyString, _("XML resources (*.xrc)|*.xrc"), wxSAVE | wxOVERWRITE_PROMPT);
            wxSetWorkingDirectory(cwd);
            if (!name.IsEmpty())
                SaveFile((m_FileName = name));
            break;
            }

        case ID_DELETE_NODE :
            {
            DeleteSelectedNode();
            break;
            }
    }
}



void EditorFrame::DeleteSelectedNode()
{
    XmlTreeData *dt = (XmlTreeData*)
            (m_TreeCtrl->GetItemData(m_TreeCtrl->GetItemParent(m_TreeCtrl->GetSelection())));
    wxXmlNode *n = (dt) ? dt->Node : NULL;

    m_SelectedNode->GetParent()->RemoveChild(m_SelectedNode);
    NotifyChanged(CHANGED_TREE);
    SelectNode(n);
}



void EditorFrame::OnNewNode(wxCommandEvent& event)
{
    if (event.GetId() >= ID_NEWSYBNODE)
    {
        XmlTreeData *pardt =
            (XmlTreeData*)(m_TreeCtrl->GetItemData(
                m_TreeCtrl->GetItemParent(m_TreeCtrl->GetSelection())));

        if (pardt && pardt->Node && pardt->Node != m_Resource->GetRoot())
        {
            wxXmlNode *nd = pardt->Node;

            wxXmlNode *realnode = NodeHandler::Find(nd)->GetRealNode(nd);
            NodeHandler *hnd = NodeHandler::Find(realnode);
            wxString name = hnd->GetChildTypes()[event.GetId()-ID_NEWSYBNODE];

            wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, _T("object"));
            node->AddProperty(_T("class"), name);

            hnd->InsertNode(realnode, node, m_SelectedNode);
            wxTreeItemId root = m_TreeCtrl->GetSelection();
            SelectNode(node, &root);
        }

    }

    else if (event.GetId() >= ID_NEWNODE)
    {
        wxXmlNode *realnode = NodeHandler::Find(m_SelectedNode)->GetRealNode(m_SelectedNode);
        NodeHandler *hnd = NodeHandler::Find(realnode);
        wxString name = hnd->GetChildTypes()[event.GetId()-ID_NEWNODE];

        wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, _T("object"));
        node->AddProperty(_T("class"), name);

        hnd->InsertNode(realnode, node);
        wxTreeItemId root = m_TreeCtrl->GetSelection();
        SelectNode(node, &root);
    }

    else
    {
        wxString name;
        switch (event.GetId())
        {
            case ID_NEWDIALOG : name = _T("wxDialog"); break;
            case ID_NEWFRAME : name = _T("wxFrame"); break;
            case ID_NEWPANEL : name = _T("wxPanel"); break;
            case ID_NEWMENU : name = _T("wxMenu"); break;
            case ID_NEWMENUBAR : name = _T("wxMenuBar"); break;
            case ID_NEWTOOLBAR : name = _T("wxToolBar"); break;
            default : return; // never occurs
        }

        wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, _T("object"));
        node->AddProperty(_T("class"), name);
        m_Resource->GetRoot()->AddChild(node);
        NotifyChanged(CHANGED_TREE);
        SelectNode(node);
    }
}



void EditorFrame::OnRightClickTree(wxPoint pos)
{
    wxMenu *popup = new wxMenu;

    if (m_SelectedNode == NULL || m_SelectedNode == m_Resource->GetRoot())
    {
        popup->Append(ID_NEWDIALOG, _("New wxDialog"));
        popup->Append(ID_NEWFRAME, _("New wxFrame"));
        popup->Append(ID_NEWPANEL, _("New wxPanel"));
        popup->Append(ID_NEWMENU, _("New wxMenu"));
        popup->Append(ID_NEWMENUBAR, _("New wxMenuBar"));
        popup->Append(ID_NEWTOOLBAR, _("New wxToolBar"));
    }

    else
    {
        bool has_children;
        {
            wxArrayString& arr =
                NodeHandler::Find(NodeHandler::Find(m_SelectedNode)->GetRealNode(m_SelectedNode))->
                    GetChildTypes();

            has_children = !arr.IsEmpty();
            if (!arr.IsEmpty())
            {
                wxMenu *news = new wxMenu;
                wxMenu *news2 = news;
                for (size_t i = 0; i < arr.GetCount(); i++)
                {
                    news2->Append(i + ID_NEWNODE, arr[i]);
#ifdef __WXGTK__ // doesn't support Break
                    if (i % 20 == 19)
                    {
                        wxMenu *m = new wxMenu;
                        news2->Append(ID_NEWNODE+arr.GetCount(), _("More..."), m);
                        news2 = m;
                    }
#else
                    if (i % 16 == 15) news2->Break();
#endif
                }
                popup->Append(ID_NEWNODE-1, _("New child"), news);
            }
        }


        XmlTreeData *pardt =
            (XmlTreeData*)(m_TreeCtrl->GetItemData(
                m_TreeCtrl->GetItemParent(m_TreeCtrl->GetSelection())));
        if (pardt && pardt->Node && pardt->Node != m_Resource->GetRoot())
        {
            wxXmlNode *nd = pardt->Node;
            wxArrayString& arr =
                NodeHandler::Find(NodeHandler::Find(nd)->GetRealNode(nd))->
                    GetChildTypes();

            if (!arr.IsEmpty())
            {
                wxMenu *news = new wxMenu;
                wxMenu *news2 = news;
                for (size_t i = 0; i < arr.GetCount(); i++)
                {
                    news2->Append(i + ID_NEWSYBNODE, arr[i]);
#ifdef __WXGTK__ // doesn't support Break
                    if (i % 20 == 19)
                    {
                        wxMenu *m = new wxMenu;
                        news2->Append(ID_NEWSYBNODE+arr.GetCount(), _("More..."), m);
                        news2 = m;
                    }
#else
                    if (i % 16 == 15) news2->Break();
#endif
                }
                popup->Append(ID_NEWSYBNODE-1, _("New sybling"), news);
            }
        }


        popup->AppendSeparator();
        popup->Append(ID_CUT, _("Cut"));
        popup->Append(ID_COPY, _("Copy"));
        popup->Append(ID_PASTE_SYBLING, _("Paste as sybling"));
        popup->Append(ID_PASTE_CHILD, _("Paste as child"));
        popup->AppendSeparator();
        popup->Append(ID_DELETE_NODE, _("Delete"));
        popup->Enable(ID_PASTE_SYBLING, m_Clipboard != NULL);
        popup->Enable(ID_PASTE_CHILD, has_children && m_Clipboard != NULL);
    }

    m_TreeCtrl->PopupMenu(popup, pos);
    delete popup;
}



void EditorFrame::OnClipboardAction(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case ID_COPY:
        case ID_CUT:
            delete m_Clipboard;
            m_Clipboard = new wxXmlNode(*m_SelectedNode);
            GetMenuBar()->Enable(ID_PASTE_SYBLING, true);
            GetMenuBar()->Enable(ID_PASTE_CHILD, true);
            if (event.GetId() == ID_CUT) DeleteSelectedNode();
            break;

        case ID_PASTE_SYBLING:
            {
            XmlTreeData *pardt =
                (XmlTreeData*)(m_TreeCtrl->GetItemData(
                    m_TreeCtrl->GetItemParent(m_TreeCtrl->GetSelection())));

            if (pardt && pardt->Node && pardt->Node != m_Resource->GetRoot())
            {
                wxXmlNode *nd = pardt->Node;

                wxXmlNode *realnode = NodeHandler::Find(nd)->GetRealNode(nd);
                NodeHandler *hnd = NodeHandler::Find(realnode);
                wxXmlNode *node = new wxXmlNode(*m_Clipboard);
                hnd->InsertNode(realnode, node, m_SelectedNode);
                wxTreeItemId root = m_TreeCtrl->GetSelection();
                SelectNode(node, &root);
            }
            }
            break;

        case ID_PASTE_CHILD:
            wxXmlNode *realnode = NodeHandler::Find(m_SelectedNode)->GetRealNode(m_SelectedNode);
            NodeHandler *hnd = NodeHandler::Find(realnode);
            wxXmlNode *node = new wxXmlNode(*m_Clipboard);
            hnd->InsertNode(realnode, node);
            wxTreeItemId root = m_TreeCtrl->GetSelection();
            SelectNode(node, &root);
            break;
    }
}




bool EditorFrame::AskToSave()
    // asks the user to save current document (if modified)
    // returns false if user cancelled the action, true of he choosed
    // 'yes' or 'no'
{
    if (!m_Modified) return true;

    int res = wxMessageBox(_("File modified. Do you want to save changes?"), _("Save changes"),
                            wxYES_NO | wxCANCEL | wxCENTRE | wxICON_QUESTION);
    if (res == wxYES)
        SaveFile(m_FileName);
    return (res != wxCANCEL);
}



void EditorFrame::OnCloseWindow(wxCloseEvent&)
{
    if (!AskToSave()) return;
    Destroy();
}
