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
    ID_XMLIDEDIT,
    ID_XMLIDPICK,
    ID_EDITCODE,
    ID_PROPSLIST,
    ID_CLEARPROP,
    
    ID_CUT,
    ID_PASTE_SYBLING,
    ID_PASTE_CHILD,
    ID_COPY,

    ID_NEWDIALOG,
    ID_NEWPANEL,
    ID_NEWMENU,
    ID_NEWMENUBAR,
    ID_NEWTOOLBAR,   
    ID_NEWNODE = wxID_HIGHEST + 1000,
    ID_NEWSYBNODE = ID_NEWNODE + 2000
};





BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_TREE_SEL_CHANGED(ID_TREE, EditorFrame::OnTreeSel)
    EVT_TOOL_RANGE(ID_PREVIEW, ID_EXIT, EditorFrame::OnToolbar)
    EVT_MENU_RANGE(ID_NEWDIALOG, ID_NEWSYBNODE + 1000, EditorFrame::OnNewNode)
    EVT_MENU_RANGE(ID_CUT, ID_COPY, EditorFrame::OnClipboardAction)
    EVT_TEXT(ID_XMLIDEDIT, EditorFrame::OnXMLIDEdit)
    EVT_BUTTON(ID_XMLIDPICK, EditorFrame::OnXMLIDPick)
    EVT_BUTTON(ID_EDITCODE, EditorFrame::OnEditCode)
    EVT_BUTTON(ID_CLEARPROP, EditorFrame::OnClearProp)
    EVT_LIST_ITEM_SELECTED(ID_PROPSLIST, EditorFrame::OnPropSel)
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

#include "bitmaps/unused.xpm"
#include "bitmaps/used.xpm"
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
    m_SelectedProp = -1;

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

    // handlers:
    m_Handlers.DeleteContents(TRUE);
    RegisterHandlers(".");
    RegisterHandlers("./df");
    // if modifying, don't forget to modify other places --
    // search for wxINSTALL_PREFIX in nodehnd.cpp
#ifdef __UNIX__
    RegisterHandlers(wxGetHomeDir() + "/.wxrcedit");
    #ifdef wxINSTALL_PREFIX
    RegisterHandlers(wxINSTALL_PREFIX "/share/wx/wxrcedit");
    #endif
#endif
    // must stay last:
    m_Handlers.Append(new NodeHandlerUnknown(this));
  
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
    
    // Create layout:    
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxPanel *p = new wxPanel(this);
    sizer->Add(p, 1, wxEXPAND);
    wxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);

    m_Splitter = new wxSplitterWindow(p);
    sizer2->Add(m_Splitter, 1, wxEXPAND);
    

    // Create tree control:
    m_TreeCtrl = new EditorTreeCtrl(m_Splitter, ID_TREE, this);
    m_ImgList = new wxImageList(16, 16);
    m_ImgList->Add(wxICON(control));
    m_ImgList->Add(wxICON(panel));
    m_ImgList->Add(wxICON(vsizer));
    m_ImgList->Add(wxICON(hsizer));
    m_ImgList->Add(wxICON(gsizer));
    m_ImgList->Add(wxICON(resicon));

    m_TreeCtrl->SetImageList(m_ImgList);
 
    
    // Create properties panel:
    m_Splitter2 = new wxSplitterWindow(m_Splitter);
    m_PropsPanel = new wxPanel(m_Splitter2, -1, wxDefaultPosition,
                           wxDefaultSize, wxTAB_TRAVERSAL);

    wxSizer *sizer3 = new wxBoxSizer(wxVERTICAL);
    
    wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);

    sizer3->Add(new wxButton(m_PropsPanel, ID_EDITCODE, "Edit XML code"), 
            0, wxALL | wxEXPAND, 2);
    sz->Add(new wxStaticText(m_PropsPanel, -1, _("XMLID name:")),
            0, wxLEFT | wxALIGN_CENTER_VERTICAL, 2);
    m_XMLIDCtrl = new wxTextCtrl(m_PropsPanel, ID_XMLIDEDIT, "");
    sz->Add(m_XMLIDCtrl, 1, wxLEFT|wxRIGHT, 2);
    sz->Add(new wxButton(m_PropsPanel, ID_XMLIDPICK, "...", wxDefaultPosition, wxSize(16,-1)), 
            0, wxRIGHT, 2);  
    sizer3->Add(sz, 0, wxTOP|wxEXPAND, 2); 
    
    m_PropsList = new wxListCtrl(m_PropsPanel, ID_PROPSLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL); 

    m_ImgListProp = new wxImageList(16, 16);
    m_ImgListProp->Add(wxICON(unused));
    m_ImgListProp->Add(wxICON(used));
    m_PropsList->SetImageList(m_ImgListProp, wxIMAGE_LIST_SMALL);

    m_PropsList->InsertColumn(0, _("Property"));
    m_PropsList->InsertColumn(1, _("Value"));
    m_PropsList->SetColumnWidth(0, cfg->Read("editor_col0", wxLIST_AUTOSIZE_USEHEADER));
    m_PropsList->SetColumnWidth(1, cfg->Read("editor_col1", wxLIST_AUTOSIZE_USEHEADER));

    sizer3->Add(m_PropsList, 1, wxALL | wxEXPAND, 2);
       
    m_PropsPanel->SetAutoLayout(TRUE);
    m_PropsPanel->SetSizer(sizer3);
    m_PropsPanel->Layout();

    m_PropsEditPanel = new wxScrolledWindow(m_Splitter2, -1, wxDefaultPosition,
                           wxDefaultSize, wxTAB_TRAVERSAL);

    m_Splitter->SplitVertically(m_TreeCtrl, m_Splitter2);
    m_Splitter->SetSashPosition(cfg->Read("editor_sash", 140));

    m_Splitter2->SplitHorizontally(m_PropsPanel, m_PropsEditPanel);
    m_Splitter2->SetSashPosition(cfg->Read("editor_sash2", 100));
                                      
    p->SetAutoLayout(TRUE);
    p->SetSizer(sizer2);



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

    wxConfigBase *cfg = wxConfigBase::Get();
    
    cfg->Write("editor_x", (long)GetPosition().x);
    cfg->Write("editor_y", (long)GetPosition().y);
    cfg->Write("editor_w", (long)GetSize().x);
    cfg->Write("editor_h", (long)GetSize().y);
    cfg->Write("editor_sash", (long)m_Splitter->GetSashPosition());
    cfg->Write("editor_sash2", (long)m_Splitter2->GetSashPosition());
    cfg->Write("editor_col0", (long)m_PropsList->GetColumnWidth(0));
    cfg->Write("editor_col1", (long)m_PropsList->GetColumnWidth(1));

    delete m_ImgList;
    delete m_ImgListProp;
    RefreshProps(NULL);
    
    delete m_Clipboard;
}



NodeHandler *EditorFrame::FindHandler(wxXmlNode *node)
{
    wxNode *n = m_Handlers.GetFirst();
    while (n)
    {
        NodeHandler *h = (NodeHandler*) n->GetData();
        if (h->CanHandle(node))
            return h;
        n = n->GetNext();
    }
    return NULL;
}



void EditorFrame::RegisterHandlers(const wxString& dirname)
{
    if (!wxDirExists(dirname)) return;
    
    wxDir dir(dirname);
    wxString filename;
    bool cont;
    NodeHandler *hnd;
    
    cont = dir.GetFirst(&filename, "*.df");
    while (cont)
    {
        hnd = NodeHandler::CreateFromFile(filename, this);
        if (hnd) m_Handlers.Append(hnd);
        cont = dir.GetNext(&filename);
    }
    
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


void EditorFrame::RefreshProps(wxXmlNode *node)
{
    m_SelectedProp = -1;
    
    for (int i = 0; i < m_PropsList->GetItemCount(); i++)
        delete (wxObject*)(m_PropsList->GetItemData(i));   
    
    m_PropsList->DeleteAllItems();

    if (node == NULL) return;
    
    m_XMLIDCtrl->SetValue(FindHandler(node)->GetRealNode(node)->
                            GetPropVal("name", "-1"));
    CreatePropsList(m_PropsList, node);

    RefreshPropsEdit();
}



void EditorFrame::RefreshPropsEdit()
{
    m_PropsEditPanel->DestroyChildren();
    m_PropsEditPanel->SetSizer(NULL);
    
    if (!m_SelectedNode || m_SelectedProp == -1 ||
        m_PropsList->GetItemData(m_SelectedProp) == 0) return;
    
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    
    sizer->Add(new wxButton(m_PropsEditPanel, ID_CLEARPROP, _("Clear")),
               0, wxALL, 5);
        
    sizer->Add(
                FindHandler(m_SelectedNode)->CreatePropEditPanel(m_PropsEditPanel, m_PropsList, m_SelectedProp),
                1,  wxEXPAND, 0);
    
    m_PropsEditPanel->SetAutoLayout(TRUE);
    m_PropsEditPanel->SetSizer(sizer);
    m_PropsEditPanel->Layout();

    wxSize winsz = m_PropsEditPanel->GetSize();
    sizer->SetMinSize(winsz.x, winsz.y);
    
    wxSize minsz = sizer->GetMinSize();

    m_PropsEditPanel->SetScrollbars(8, 8, 1/*minsz.x/8*/, minsz.y/8);
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

    return FindHandler(node)->CreateTreeNode(treectrl, parent, node);
}



void EditorFrame::CreatePropsList(wxListCtrl *treectrl, wxXmlNode *node)
{
    if (!node) return;

    FindHandler(node)->CreatePropsList(treectrl, node);
}



void EditorFrame::NotifyChanged(int change_type)
{
    if (change_type & CHANGED_TREE)
        RefreshTree();

    if (change_type & CHANGED_TREE_SELECTED)
    {
        wxTreeItemId sel = m_TreeCtrl->GetSelection();
        m_TreeCtrl->SetItemText(sel, 
             FindHandler(m_SelectedNode)->GetTreeString(m_SelectedNode));
    }

    if (change_type & CHANGED_TREE_SELECTED_ICON)
    {
        wxTreeItemId sel = m_TreeCtrl->GetSelection();
        int icon = FindHandler(m_SelectedNode)->GetTreeIcon(m_SelectedNode);
        m_TreeCtrl->SetItemImage(sel, icon);
    }

    if (change_type & CHANGED_PROPS_PANEL)
        RefreshProps(m_SelectedNode);
}



void EditorFrame::OnTreeSel(wxTreeEvent& event)
{
    XmlTreeData *dt = (XmlTreeData*)(m_TreeCtrl->GetItemData(event.GetItem()));
    wxXmlNode *node = (dt) ? dt->Node : NULL;
        
    m_SelectedNode = node;
    RefreshProps(node);
}



void EditorFrame::OnXMLIDEdit(wxCommandEvent& event)
{
    if (!m_SelectedNode) return;
    wxXmlNode *node = FindHandler(m_SelectedNode)->GetRealNode(m_SelectedNode);

    node->DeleteProperty("name");
    wxString s = m_XMLIDCtrl->GetValue();
    if (!(s == "-1")) node->AddProperty("name", s);
    NotifyChanged(CHANGED_TREE_SELECTED);
}



void EditorFrame::OnXMLIDPick(wxCommandEvent& event)
{
    if (!m_SelectedNode) return;
    wxXmlNode *node = FindHandler(m_SelectedNode)->GetRealNode(m_SelectedNode);

    wxString choices[] = {wxString("-1")
    #define stdID(id) , wxString(#id)
    stdID(wxID_OK) stdID(wxID_CANCEL)
    stdID(wxID_YES) stdID(wxID_NO)
    stdID(wxID_APPLY) stdID(wxID_HELP) 
    stdID(wxID_HELP_CONTEXT)

    stdID(wxID_OPEN) stdID(wxID_CLOSE) stdID(wxID_NEW)
    stdID(wxID_SAVE) stdID(wxID_SAVEAS) stdID(wxID_REVERT)
    stdID(wxID_EXIT) stdID(wxID_UNDO) stdID(wxID_REDO)
    stdID(wxID_PRINT) stdID(wxID_PRINT_SETUP)
    stdID(wxID_PREVIEW) stdID(wxID_ABOUT) stdID(wxID_HELP_CONTENTS)
    stdID(wxID_HELP_COMMANDS) stdID(wxID_HELP_PROCEDURES)
    stdID(wxID_CUT) stdID(wxID_COPY) stdID(wxID_PASTE)
    stdID(wxID_CLEAR) stdID(wxID_FIND) stdID(wxID_DUPLICATE)
    stdID(wxID_SELECTALL) 
    stdID(wxID_STATIC) stdID(wxID_FORWARD) stdID(wxID_BACKWARD)
    stdID(wxID_DEFAULT) stdID(wxID_MORE) stdID(wxID_SETUP)
    stdID(wxID_RESET) 
    #undef stdID
    };

    wxString s = 
      wxGetSingleChoice(_("Choose from predefined IDs:"), _("XMLID"), 
                        38/*sizeof choices*/, choices);
    if (!s) return;

    m_XMLIDCtrl->SetValue(s);
    node->DeleteProperty("name");
    if (!(s == "-1")) node->AddProperty("name", s);
    NotifyChanged(CHANGED_TREE_SELECTED);
}



void EditorFrame::OnEditCode(wxCommandEvent& event)
{
    if (!m_SelectedNode) return;

    wxBusyCursor bcur;
    wxDialog dlg(this, -1, _("XML code editor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxTextCtrl *tc = new wxTextCtrl(&dlg, -1, "", wxDefaultPosition,
                         wxDefaultSize, wxTE_MULTILINE);
    sizer->Add(tc, 1, wxEXPAND | wxALL, 10);

    wxSizer *sz2 = new wxBoxSizer(wxHORIZONTAL);

    sz2->Add(new wxButton(&dlg, wxID_OK, _("Save")), 0);
    sz2->Add(new wxButton(&dlg, wxID_CANCEL, _("Cancel")), 0, wxLEFT, 10);

    sizer->Add(sz2, 0, wxALIGN_RIGHT | wxRIGHT|wxBOTTOM, 10);

    dlg.SetAutoLayout(TRUE);
    dlg.SetSizer(sizer);

    wxConfigBase *cfg = wxConfigBase::Get();
    
    dlg.SetSize(wxRect(wxPoint(cfg->Read("xmleditor_x", -1), cfg->Read("xmleditor_y", -1)),
            wxSize(cfg->Read("xmleditor_w", 400), cfg->Read("xmleditor_h", 400))));

    wxString tempfile;
    wxGetTempFileName("xmleditor", tempfile);

    {
        wxXmlDocument doc;
        doc.SetRoot(new wxXmlNode(*m_SelectedNode));
        doc.Save(tempfile, wxXML_IO_LIBXML);
    }
    tc->LoadFile(tempfile);

    if (dlg.ShowModal() == wxID_OK)
    {
        tc->SaveFile(tempfile);
        wxXmlDocument doc;
        if (doc.Load(tempfile))
        {
            (*m_SelectedNode) = *doc.GetRoot();
            NotifyChanged(CHANGED_TREE);
            //FIXME-instead, regenerate only children
        }
        else wxLogError(_("Illegal XML file, canceled."));
    }
    wxRemoveFile(tempfile);

    cfg->Write("xmleditor_x", (long)dlg.GetPosition().x);
    cfg->Write("xmleditor_y", (long)dlg.GetPosition().y);
    cfg->Write("xmleditor_w", (long)dlg.GetSize().x);
    cfg->Write("xmleditor_h", (long)dlg.GetSize().y);
}



void EditorFrame::OnClearProp(wxCommandEvent& event)
{
    m_PropsList->SetItemImage(m_SelectedProp, 0, 0);
    m_PropsList->SetItem(m_SelectedProp, 1, "");
    
    PropsListInfo *pli = (PropsListInfo*)m_PropsList->GetItemData(m_SelectedProp);
    
    wxXmlNode *nd = XmlFindNode(pli->m_Node, pli->m_PropInfo->Name);
    
    if (nd == NULL) return;
    nd->GetParent()->RemoveChild(nd);
    delete nd;
    RefreshPropsEdit();
}



void EditorFrame::OnPropSel(wxListEvent& event)
{
    m_SelectedProp = event.GetIndex();
    RefreshPropsEdit();
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
            wxString name = wxFileSelector("Open XML resource", "", "", "", "XML resources|*.xml", wxOPEN | wxFILE_MUST_EXIST);
            if (!name.IsEmpty())
                LoadFile(name);
            break;
            }

        case ID_SAVE :
            if (m_FileName != "") { SaveFile(m_FileName); break;}
            // else go to SAVEAS

        case ID_SAVEAS :
            {
            wxString name = wxFileSelector("Save as", "", m_FileName, "", "XML resources|*.xml", wxSAVE | wxOVERWRITE_PROMPT);
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

            wxXmlNode *realnode = FindHandler(nd)->GetRealNode(nd);
            NodeHandler *hnd = FindHandler(realnode);
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
        wxXmlNode *realnode = FindHandler(m_SelectedNode)->GetRealNode(m_SelectedNode);
        NodeHandler *hnd = FindHandler(realnode);
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
                FindHandler(FindHandler(m_SelectedNode)->GetRealNode(m_SelectedNode))->
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
                FindHandler(FindHandler(nd)->GetRealNode(nd))->
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

                wxXmlNode *realnode = FindHandler(nd)->GetRealNode(nd);
                NodeHandler *hnd = FindHandler(realnode);
                wxXmlNode *node = new wxXmlNode(*m_Clipboard);
                hnd->InsertNode(realnode, node, m_SelectedNode);
                wxTreeItemId root = m_TreeCtrl->GetSelection();
                SelectNode(node, &root);
            }
            }
            break;
            
        case ID_PASTE_CHILD:
            wxXmlNode *realnode = FindHandler(m_SelectedNode)->GetRealNode(m_SelectedNode);
            NodeHandler *hnd = FindHandler(realnode);
            wxXmlNode *node = new wxXmlNode(*m_Clipboard);
            hnd->InsertNode(realnode, node);
            wxTreeItemId root = m_TreeCtrl->GetSelection();
            SelectNode(node, &root);
            break;
    }
}

