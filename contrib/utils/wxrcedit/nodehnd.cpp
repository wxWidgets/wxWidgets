/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "nodehnd.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "nodehnd.h"
#include "wx/xml/xml.h"
#include "wx/filefn.h"
#include "wx/wx.h"
#include "wx/arrimpl.cpp"
#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/listctrl.h"
#include "editor.h"
#include "treedt.h"
#include "xmlhelpr.h"


WX_DEFINE_OBJARRAY(NodeInfoArray);



void NodeInfo::Read(const wxString& filename)
{
    HandlerType tp = HANDLER_NONE;
    wxString nd, cht;
    bool ab = FALSE;
    long icn = -1;

    Node.Empty();

    wxPathList list;
    // if modifying, don't forget to modify it in all places --
    // search for wxINSTALL_PREFIX in editor.cpp
    list.Add(".");
    list.Add("./df");
#ifdef __UNIX__ 
    list.Add(wxGetHomeDir() + "/.wxrcedit");
    #ifdef wxINSTALL_PREFIX
    list.Add(wxINSTALL_PREFIX "/share/wx/wxrcedit");
    #endif
#endif
    
    wxString path = list.FindValidPath(filename);
    if (path.IsEmpty()) return;
    
    wxTextFile tf;
    tf.Open(path);
    
    if (!tf.IsOpened()) return;

    for (size_t i = 0; i < tf.GetLineCount(); i++)
    {
        if (tf[i].IsEmpty() || tf[i][0] == '#') continue;
        wxStringTokenizer tkn(tf[i], ' ');
        wxString s = tkn.GetNextToken();
        if (s == "node")
            nd = tkn.GetNextToken();
        else if (s == "childtype")
            cht = tkn.GetNextToken();
        else if (s == "icon")
            tkn.GetNextToken().ToLong(&icn);
        else if (s == "derived")
        {
            if (tkn.GetNextToken() == "from")
            {
                s = tkn.GetNextToken();
                DerivedFrom.Add(s);
                Read(s + ".df");
            }
        }
        else if (s == "abstract")
            ab = true;
        else if (s == "type")
        {
            s = tkn.GetNextToken();
            if (s == "sizer") tp = HANDLER_SIZER;
            else if (s == "sizeritem") tp = HANDLER_SIZERITEM;
            else if (s == "panel") tp = HANDLER_PANEL;
            else if (s == "notebook") tp = HANDLER_NOTEBOOK;
            else if (s == "notebookpage") tp = HANDLER_NOTEBOOKPAGE;
            else /*if (s == "normal")*/ tp = HANDLER_NORMAL;
        }
        else if (s == "var")
        {
            PropertyInfo pi;
            pi.Name = tkn.GetNextToken();
            tkn.GetNextToken();
            wxString typ = tkn.GetNextToken();
            if (tkn.HasMoreTokens()) pi.MoreInfo = tkn.GetNextToken();
            /* ADD NEW PROPERTY TYPES HERE 
               (search for other occurences of this comment in _all_ files) */
            if (typ == "color") pi.Type = PROP_COLOR;
            else if (typ == "flags") pi.Type = PROP_FLAGS;
            else if (typ == "bool") pi.Type = PROP_BOOL;
            else if (typ == "integer") pi.Type = PROP_INTEGER;
            else if (typ == "coord") pi.Type = PROP_COORD;
            else if (typ == "dimension") pi.Type = PROP_DIMENSION;
            else if (typ == "not_implemented") pi.Type = PROP_NOT_IMPLEMENTED;
            else /*if (typ == "text")*/ pi.Type = PROP_TEXT;
            
            bool fnd = FALSE;
            for (size_t j = 0; j < Props.GetCount(); j++)
            {
                if (Props[j].Name == pi.Name)
                {
                    if (Props[j].Type == pi.Type && pi.Type == PROP_FLAGS)
                        Props[j].MoreInfo << ',' << pi.MoreInfo;
                    else
                        Props[j] = pi;
                    fnd = TRUE;
                }
            }
            
            if (!fnd) Props.Add(pi);
        }
    }
    
    if (!nd.IsEmpty()) Node = nd;
    if (!cht.IsEmpty()) ChildType = cht;
    if (tp != HANDLER_NONE) Type = tp;
    if (icn != -1) Icon = icn;
    Abstract = ab;
}



NodeHandler *NodeHandler::CreateFromFile(const wxString& filename, EditorFrame *frame)
{
    NodeHandler *hnd = NULL;

    if (s_AllNodes == NULL) s_AllNodes = new NodeInfoArray;
       
    NodeInfo *ni = new NodeInfo; 
    ni->Type = HANDLER_NONE;
    ni->Icon = 0;
    ni->Read(filename);
    s_AllNodes->Add(*ni); // add a copy
    
    if (ni->Type == HANDLER_NONE || ni->Node.IsEmpty() || ni->Abstract) 
        return NULL;
    
    switch (ni->Type)
    {
        case HANDLER_PANEL:
            hnd = new NodeHandlerPanel(frame, ni);
            break;
        case HANDLER_SIZER:
            hnd = new NodeHandlerSizer(frame, ni);
            break;
        case HANDLER_SIZERITEM:
            hnd = new NodeHandlerSizerItem(frame, ni);
            break;
        case HANDLER_NOTEBOOK:
            hnd = new NodeHandlerNotebook(frame, ni);
            break;
        case HANDLER_NOTEBOOKPAGE:
            hnd = new NodeHandlerNotebookPage(frame, ni);
            break;
        default:
            hnd = new NodeHandler(frame, ni);
            break;
    }
    
    return hnd;
}






PropertyHandler* NodeHandler::s_PropHandlers[PROP_TYPES_CNT] = {NULL};
int NodeHandler::s_RefCnt = 0;
NodeInfoArray* NodeHandler::s_AllNodes = NULL;


NodeHandler::NodeHandler(EditorFrame *frame, NodeInfo *ni) : 
        m_NodeInfo(ni)
{
    if (s_RefCnt++ == 0) CreatePropHandlers();
}


void NodeHandler::CreatePropHandlers()
{
    /* ADD NEW PROPERTY TYPES HERE 
       (search for other occurences of this comment in _all_ files) */
    s_PropHandlers[PROP_TEXT] = new TextPropertyHandler;
    s_PropHandlers[PROP_FLAGS] = new FlagsPropertyHandler;
    s_PropHandlers[PROP_COLOR] = new TextPropertyHandler;
    s_PropHandlers[PROP_BOOL] = new BoolPropertyHandler;
    s_PropHandlers[PROP_INTEGER] = new TextPropertyHandler;
    s_PropHandlers[PROP_COORD] = new CoordPropertyHandler;
    s_PropHandlers[PROP_DIMENSION] = new DimensionPropertyHandler;
    s_PropHandlers[PROP_NOT_IMPLEMENTED] = new NotImplPropertyHandler;
}


NodeHandler::~NodeHandler()
{
    if (--s_RefCnt == 0)
    {
        for (int i = 0; i < PROP_TYPES_CNT; i++)
            delete s_PropHandlers[i];
        delete s_AllNodes; s_AllNodes = NULL;
    }
    delete m_NodeInfo;
}



bool NodeHandler::CanHandle(wxXmlNode *node)
{
    return (m_NodeInfo->Node == node->GetName());
}



wxTreeItemId NodeHandler::CreateTreeNode(wxTreeCtrl *treectrl, 
                                         wxTreeItemId parent,
                                         wxXmlNode *node)
{
    int icon = GetTreeIcon(node);
    wxTreeItemId item = 
        treectrl->AppendItem(parent, GetTreeString(node),
                             icon, icon, new XmlTreeData(node));
    if (parent == treectrl->GetRootItem())
        treectrl->SetItemBold(item);
    return item;
}



wxString NodeHandler::GetTreeString(wxXmlNode *node)
{
    wxString xmlid = node->GetPropVal("name", "");
    if (xmlid.IsEmpty())
        return node->GetName();
    else
        return (node->GetName() + " '" + xmlid + "'");
}



void NodeHandler::CreatePropsList(wxListCtrl *listctrl, wxXmlNode *node)
{
    int index;
    
    for (size_t i = 0; i < m_NodeInfo->Props.GetCount(); i++)
    {
        PropertyInfo *p = &(m_NodeInfo->Props[i]);
        index = s_PropHandlers[p->Type]->CreateListItem(listctrl, node, p);
        listctrl->SetItemData(index, (long)new PropsListInfo(
                                     index, s_PropHandlers[p->Type], node, 
                                     p, listctrl));
    }
}



wxPanel *NodeHandler::CreatePropEditPanel(wxWindow *parent, wxListCtrl *listctrl, int index)
{
    PropsListInfo *pli = (PropsListInfo*)listctrl->GetItemData(index);
    
    return pli->m_Handler->CreateEditPanel(parent, pli);
}



wxArrayString& NodeHandler::GetChildTypes()
{
    if (m_ChildTypes.IsEmpty())
    {
        wxString basetype = m_NodeInfo->ChildType;
        
        for (size_t i = 0; i < s_AllNodes->GetCount(); i++)
        {
            NodeInfo &ni = (*s_AllNodes)[i];
            
            if (ni.Node == basetype && !ni.Abstract) 
                m_ChildTypes.Add(ni.Node);
            
            if (ni.DerivedFrom.Index(basetype) != wxNOT_FOUND && !ni.Abstract)
                m_ChildTypes.Add(ni.Node);
        }
        m_ChildTypes.Sort();
    }

    return m_ChildTypes;
}



void NodeHandler::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{
    delete node;
    wxLogError(_("Cannot insert child into this type of node!"));
}








wxTreeItemId NodeHandlerPanel::CreateTreeNode(wxTreeCtrl *treectrl, 
                                         wxTreeItemId parent,
                                         wxXmlNode *node)
{
    wxTreeItemId root = NodeHandler::CreateTreeNode(treectrl, parent, node);
    
    wxXmlNode *n = XmlFindNode(node, "children");

    if (n) n = n->GetChildren();
    
    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE)
            EditorFrame::Get()->CreateTreeNode(treectrl, root, n);
        n = n->GetNext();
    }
    treectrl->Expand(root);
    return root;
}



void NodeHandlerPanel::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{
    wxXmlNode *cnd = XmlFindNode(parent, "children");
    if (cnd == NULL)
    {
        cnd = new wxXmlNode(wxXML_ELEMENT_NODE, "children");
        parent->AddChild(cnd);
    }
    if (insert_before)
        cnd->InsertChild(node, insert_before);
    else
        cnd->AddChild(node);
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE);
}





void NodeHandlerSizer::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{
    wxXmlNode *cnd = XmlFindNode(parent, "children");
    if (cnd == NULL)
    {
        cnd = new wxXmlNode(wxXML_ELEMENT_NODE, "children");
        parent->AddChild(cnd);
    }
    
    if (node->GetName() == "spacer" || node->GetName() == "sizeritem")
    {
        if (insert_before)
            cnd->InsertChild(node, insert_before);
        else
            cnd->AddChild(node);
    }
    else
    {
        wxXmlNode *itemnode = new wxXmlNode(wxXML_ELEMENT_NODE, "sizeritem");
        wxXmlNode *winnode = new wxXmlNode(wxXML_ELEMENT_NODE, "window");
        itemnode->AddChild(winnode);
        winnode->AddChild(node);

        if (insert_before)
            cnd->InsertChild(itemnode, insert_before);
        else
            cnd->AddChild(itemnode);
    }
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE);
}



int NodeHandlerSizer::GetTreeIcon(wxXmlNode *node)
{
    int orig = NodeHandler::GetTreeIcon(node);
    if (orig == 0)
    {
        if (XmlReadValue(node, "orient") == "wxVERTICAL") return 2;
        else return 3;
    }
    else return orig;
}



wxTreeItemId NodeHandlerSizerItem::CreateTreeNode(wxTreeCtrl *treectrl, 
                                         wxTreeItemId parent,
                                         wxXmlNode *node)
{
    wxTreeItemId root;

    root = EditorFrame::Get()->CreateTreeNode(treectrl, parent, GetRealNode(node));
    ((XmlTreeData*)treectrl->GetItemData(root))->Node = node;

    treectrl->Expand(root);
    return root;
}



void NodeHandlerSizerItem::CreatePropsList(wxListCtrl *listctrl, wxXmlNode *node)
{
    NodeHandler::CreatePropsList(listctrl, node);
    int item = listctrl->GetItemCount();
    listctrl->InsertItem(item, "------");
    listctrl->SetItemImage(item, 0, 0);
    EditorFrame::Get()->CreatePropsList(listctrl, GetRealNode(node));
}



wxString NodeHandlerSizerItem::GetTreeString(wxXmlNode *node)
{
    wxXmlNode *n = GetRealNode(node);
    return EditorFrame::Get()->FindHandler(n)->GetTreeString(n);
}



int NodeHandlerSizerItem::GetTreeIcon(wxXmlNode *node)
{
    wxXmlNode *n = GetRealNode(node);
    return EditorFrame::Get()->FindHandler(n)->GetTreeIcon(n);
}



wxXmlNode *NodeHandlerSizerItem::GetRealNode(wxXmlNode *node)
{
    wxXmlNode *n = XmlFindNode(node, "window");
    
    if (n) n = n->GetChildren();
    
    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE)
            return n;
        n = n->GetNext();
    }
    return NULL;
}






void NodeHandlerNotebook::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{
    wxXmlNode *cnd = XmlFindNode(parent, "children");
    if (cnd == NULL)
    {
        cnd = new wxXmlNode(wxXML_ELEMENT_NODE, "children");
        parent->AddChild(cnd);
    }
    
    {
        wxXmlNode *itemnode;
        
        if (node->GetName() == "notebookpage")
            itemnode = node;
        else
        {
            itemnode = new wxXmlNode(wxXML_ELEMENT_NODE, "notebookpage");
            wxXmlNode *winnode = new wxXmlNode(wxXML_ELEMENT_NODE, "window");
            itemnode->AddChild(winnode);
            winnode->AddChild(node);
        }

        if (insert_before)
            cnd->InsertChild(itemnode, insert_before);
        else
            cnd->AddChild(itemnode);
    }
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE);
}
