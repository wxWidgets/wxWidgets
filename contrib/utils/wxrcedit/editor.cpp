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
#include "wx/xml/xmlres.h"
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
    ID_NEWPANEL,
    ID_NEWMENU,
    ID_NEWMENUBAR,
    ID_NEWTOOLBAR,   
    ID_NEWNODE = wxID_HIGHEST + 10000, // safely out of XMLID range :)
    ID_NEWSYBNODE = ID_NEWNODE + 20000
};





BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_TREE_SEL_CHANGED(ID_TREE, EditorFrame::OnTreeSel)
    EVT_TOOL_RANGE(ID_PREVIEW, ID_EXIT, EditorFrame::OnToolbar)
    EVT_MENU_RANGE(ID_NEWDIALOG, ID_NEWSYBNODE + 1000, EditorFrame::OnNewNode)
    EVT_MENU_RANGE(ID_CUT, ID_COPY, EditorFrame::OnClipboardAction)
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
    : wxFrame(parent, -1, filename + _("- wxWindows resources editor"))
{
    ms_Instance = this;

    m_Clipboard = NULL;
    
    wxConfigBase *cfg = wxConfigBase::Get();
    
    SetSize(wxRect(wxPoint(cfg->Read("editor_x", -1), cfg->Read("editor_y", -1)),
            wxSize(cfg->Read("editor_w", 400), cfg->Read("editor_h", 400))));

    m_SelectedNode = NULL;
    m_Resource = NULL;
    m_FileName = wxEmptyString;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_NEW, "&New");
    menuFile->Append(ID_OPEN, "&Open\tCtrl-O");
    menuFile->Append(ID_SAVE, "&Save\tCtrl-S");
    menuFile->Append(ID_SAVEAS, "Save &as...");
    menuFile->AppendSeparator();
    menuFile->Append(ID_EXIT, "E&xit\tAlt-X");

    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append(ID_CUT, "Cut\tCtrl-X");
    menuEdit->Append(ID_COPY, "Copy\tCtrl-C");
    menuEdit->Append(ID_PASTE_SYBLING, "Paste as sybling\tCtrl-V");
    menuEdit->Append(ID_PASTE_CHILD, "Paste as child");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_DELETE_NODE,  "Delete");

    menuEdit->Enable(ID_PASTE_SYBLING, FALSE);
    menuEdit->Enable(ID_PASTE_CHILD, FALSE);
    
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEdit, "&Edit");
    SetMenuBar(menuBar);
  
    // Create toolbar:
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT);
    toolBar->SetMargins(2, 2);
    toolBar->SetToolBitmapSize(wxSize(24, 24));
    toolBar -> AddTool(ID_EXIT, wxBITMAP(close), wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Quit the editor"));
    toolBar -> AddTool(ID_OPEN, wxBITMAP(open), wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Open XML resource file"));   
    toolBar -> AddTool(ID_SAVE, wxBITMAP(save), wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Save XML file"));
    toolBar -> AddTool(ID_PREVIEW, wxBITMAP(preview), wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
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

    SetAutoLayout(TRUE);
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
    delete m_Resource;
    
    m_FileName = "";
    m_Resource = new wxXmlDocument;
    
    if (!m_Resource->Load(filename))
    {
        delete m_Resource;
        m_Resource = NULL;
        NewFile();
        wxLogError("Error parsing " + filename);
    }
    else
    {
        m_FileName = filename;
        RefreshTree();
        SetTitle("wxrcedit - " + wxFileNameFromPath(m_FileName));
    }
}



void EditorFrame::SaveFile(const wxString& filename)
{
    m_FileName = filename;
    SetTitle("wxrcedit - " + wxFileNameFromPath(m_FileName));

    if (!m_Resource->Save(filename, wxXML_IO_LIBXML))
        wxLogError("Error saving " + filename);
}



void EditorFrame::NewFile()
{  
    delete m_Resource;
    
    m_FileName = "";
    m_Resource = new wxXmlDocument;
    m_Resource->SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, "resource"));    
    
    RefreshTree();
    SetTitle("unnamed");
}



void EditorFrame::RefreshTree()
{
    wxXmlNode *sel = m_SelectedNode;
    
    m_TreeCtrl->DeleteAllItems(); 
    wxTreeItemId root = m_TreeCtrl->AddRoot("Resource: " + wxFileNameFromPath(m_FileName), 5, 5);

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
    long cookie;
    
    item = m_TreeCtrl->GetFirstChild(*root, cookie);
    while (item.IsOk())
    {
        dt = (XmlTreeData*)(m_TreeCtrl->GetItemData(item));
        nd = (dt) ? dt->Node : NULL;
        if (nd == node) 
        {
            m_TreeCtrl->SelectItem(item);
            m_TreeCtrl->EnsureVisible(item);
            return TRUE;
        }
        if (m_TreeCtrl->ItemHasChildren(item) && SelectNode(node, &item)) 
            return TRUE;
        item = m_TreeCtrl->GetNextChild(*root, cookie);
    }
    return FALSE;
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
}



static void RecursivelyExpand(wxTreeCtrl *t, wxTreeItemId item)
{
    t->Expand(item);
    long cookie;
    wxTreeItemId id = t->GetFirstChild(item, cookie);
    while (id.IsOk())
    {
        RecursivelyExpand(t, id);
        id = t->GetNextChild(item, cookie);
    }
}

void EditorFrame::OnTreeSel(wxTreeEvent& event)
{
    XmlTreeData *dt = (XmlTreeData*)(m_TreeCtrl->GetItemData(event.GetItem()));
    wxXmlNode *node = (dt) ? dt->Node : NULL;      
            
    m_SelectedNode = node;
    if (node)
        PropertiesFrame::Get()->ShowProps(node);

    if (m_TreeCtrl->GetParent(event.GetItem()) == m_TreeCtrl->GetRootItem())
    {
        wxTreeItemId it = event.GetOldItem();

        if (it.IsOk() && m_TreeCtrl->GetRootItem() != it)
        {
            while (m_TreeCtrl->GetParent(it) != m_TreeCtrl->GetRootItem())
                it = m_TreeCtrl->GetParent(it);
            m_TreeCtrl->Collapse(it);
        }
        RecursivelyExpand(m_TreeCtrl, event.GetItem());

        PreviewFrame::Get()->Preview(node);
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
                PreviewFrame::Get()->Preview(dt->Node);
            break;
            }

        case ID_EXIT :
            Close(TRUE);
            break;

        case ID_NEW :
            NewFile();
            break;

        case ID_OPEN :
            {
            wxString name = wxFileSelector(_("Open XML resource"), _T(""), _T(""), _T(""), _("XML resources (*.xrc)|*.xrc"), wxOPEN | wxFILE_MUST_EXIST);
            if (!name.IsEmpty())
                LoadFile(name);
            break;
            }

        case ID_SAVE :
            if (m_FileName != "") { SaveFile(m_FileName); break;}
            // else go to SAVEAS

        case ID_SAVEAS :
            {
            wxString name = wxFileSelector(_("Save as"), _T(""), m_FileName, _T(""), _("XML resources (*.xrc)|*.xrc"), wxSAVE | wxOVERWRITE_PROMPT);
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
            (m_TreeCtrl->GetItemData(m_TreeCtrl->GetParent(m_TreeCtrl->GetSelection())));
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
                m_TreeCtrl->GetParent(m_TreeCtrl->GetSelection())));

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
                m_TreeCtrl->GetParent(m_TreeCtrl->GetSelection())));
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
            GetMenuBar()->Enable(ID_PASTE_SYBLING, TRUE);
            GetMenuBar()->Enable(ID_PASTE_CHILD, TRUE);
            if (event.GetId() == ID_CUT) DeleteSelectedNode();
            break;
            
        case ID_PASTE_SYBLING:
            {
            XmlTreeData *pardt = 
                (XmlTreeData*)(m_TreeCtrl->GetItemData(
                    m_TreeCtrl->GetParent(m_TreeCtrl->GetSelection())));

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

