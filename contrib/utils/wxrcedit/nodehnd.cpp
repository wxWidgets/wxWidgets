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
#include "nodesdb.h"



wxList NodeHandler::ms_Handlers;
bool NodeHandler::ms_HandlersLoaded = FALSE;


NodeHandler *NodeHandler::Find(wxXmlNode *node)
{
    if (!ms_HandlersLoaded)
    {
        ms_HandlersLoaded = TRUE;
        ms_Handlers.DeleteContents(TRUE);
        
        NodeInfoArray& arr = NodesDb::Get()->GetNodesInfo();
        NodeHandler *hnd;
        for (size_t i = 0; i < arr.GetCount(); i++)
        {
            if (arr[i].NodeClass.IsEmpty() || arr[i].Abstract) continue;

                 if (arr[i].Type == _T("panel"))
                hnd = new NodeHandlerPanel(&(arr[i]));
            else if (arr[i].Type == _T("sizer"))
                hnd = new NodeHandlerSizer(&(arr[i]));
            else if (arr[i].Type == _T("sizeritem"))
                hnd = new NodeHandlerSizerItem(&(arr[i]));
            else if (arr[i].Type == _T("notebook"))
                hnd = new NodeHandlerNotebook(&(arr[i]));
            else if (arr[i].Type == _T("notebookpage"))
                hnd = new NodeHandlerNotebookPage(&(arr[i]));
            else
                hnd = new NodeHandler(&(arr[i]));
            if (hnd) ms_Handlers.Append(hnd);
        }
        ms_Handlers.Append(new NodeHandlerUnknown);        
    }

    wxNode *n = ms_Handlers.GetFirst();
    while (n)
    {
        NodeHandler *h = (NodeHandler*) n->GetData();
        if (h->CanHandle(node))
            return h;
        n = n->GetNext();
    }
    return NULL;
}




NodeHandler::NodeHandler(NodeInfo *ni) : 
        m_NodeInfo(ni)
{
}


NodeHandler::~NodeHandler()
{
}



bool NodeHandler::CanHandle(wxXmlNode *node)
{
    return (m_NodeInfo->NodeClass == XmlGetClass(node));
}




PropertyInfoArray& NodeHandler::GetPropsList(wxXmlNode *node)
{
    return m_NodeInfo->Props;
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
    wxString xmlid = node->GetPropVal(_T("name"), wxEmptyString);
    if (xmlid.IsEmpty())
        return XmlGetClass(node);
    else
        return XmlGetClass(node) + _T(" '") + xmlid + _T("'");
}




wxArrayString& NodeHandler::GetChildTypes()
{
    if (m_ChildTypes.IsEmpty())
    {
        wxString basetype = m_NodeInfo->ChildType;
        NodeInfoArray& arr = NodesDb::Get()->GetNodesInfo();
        
        for (size_t i = 0; i < arr.GetCount(); i++)
        {
            NodeInfo &ni = arr[i];
            
            if (ni.NodeClass == basetype && !ni.Abstract) 
                m_ChildTypes.Add(ni.NodeClass);
            
            if (ni.DerivedFrom.Index(basetype) != wxNOT_FOUND && !ni.Abstract)
                m_ChildTypes.Add(ni.NodeClass);
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
    
    wxXmlNode *n = XmlFindNode(node, "object");

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE &&
            n->GetName() == _T("object"))
            Find(n)->CreateTreeNode(treectrl, root, n);
        n = n->GetNext();
    }
    //treectrl->Expand(root);
    return root;
}



void NodeHandlerPanel::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{
    if (insert_before)
        parent->InsertChild(node, insert_before);
    else
        parent->AddChild(node);
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE);
}





void NodeHandlerSizer::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{  
    if (XmlGetClass(node) == _T("spacer") || XmlGetClass(node) == _T("sizeritem"))
    {
        if (insert_before)
            parent->InsertChild(node, insert_before);
        else
            parent->AddChild(node);
    }
    else
    {
        wxXmlNode *itemnode = new wxXmlNode(wxXML_ELEMENT_NODE, _T("object"));
        itemnode->AddProperty(_T("class"), _T("sizeritem"));
        itemnode->AddChild(node);

        if (insert_before)
            parent->InsertChild(itemnode, insert_before);
        else
            parent->AddChild(itemnode);
    }
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE);
}



int NodeHandlerSizer::GetTreeIcon(wxXmlNode *node)
{
    int orig = NodeHandler::GetTreeIcon(node);
    if (orig == 0)
    {
        if (XmlReadValue(node, _T("orient")) == _T("wxVERTICAL")) return 2;
        else return 3;
    }
    else return orig;
}



wxTreeItemId NodeHandlerSizerItem::CreateTreeNode(wxTreeCtrl *treectrl, 
                                         wxTreeItemId parent,
                                         wxXmlNode *node)
{
    wxTreeItemId root;

    root = Find(GetRealNode(node))->CreateTreeNode(treectrl, parent, GetRealNode(node));
    ((XmlTreeData*)treectrl->GetItemData(root))->Node = node;

    //treectrl->Expand(root);
    return root;
}



PropertyInfoArray& NodeHandlerSizerItem::GetPropsList(wxXmlNode *node)
{
    m_dummy = NodeHandler::GetPropsList(node);
    wxXmlNode *nd = GetRealNode(node);
    m_dummy.Add(PropertyInfo(wxEmptyString, wxEmptyString, wxEmptyString));
    size_t pos = m_dummy.GetCount();
    WX_APPEND_ARRAY(m_dummy, 
                    Find(nd)->GetPropsList(nd));
    for (size_t i = pos; i < m_dummy.GetCount(); i++)
        m_dummy[i].Name = _T("object/") + m_dummy[i].Name;
    
    return m_dummy;
}



wxString NodeHandlerSizerItem::GetTreeString(wxXmlNode *node)
{
    wxXmlNode *n = GetRealNode(node);
    return Find(n)->GetTreeString(n);
}



int NodeHandlerSizerItem::GetTreeIcon(wxXmlNode *node)
{
    wxXmlNode *n = GetRealNode(node);
    return Find(n)->GetTreeIcon(n);
}



wxXmlNode *NodeHandlerSizerItem::GetRealNode(wxXmlNode *node)
{
    return XmlFindNode(node, _T("object"));
}






void NodeHandlerNotebook::InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before)
{  
    {
        wxXmlNode *itemnode;
        
        if (XmlGetClass(node) == _T("notebookpage"))
            itemnode = node;
        else
        {
            itemnode = new wxXmlNode(wxXML_ELEMENT_NODE, _T("object"));
            itemnode->AddProperty(_T("class"), _T("notebookpage"));
            itemnode->AddChild(node);
        }

        if (insert_before)
            parent->InsertChild(itemnode, insert_before);
        else
            parent->AddChild(itemnode);
    }
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE);
}
