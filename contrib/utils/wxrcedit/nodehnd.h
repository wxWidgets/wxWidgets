/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "nodehnd.h"
#endif

#ifndef _NODEHND_H_
#define _NODEHND_H_


class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxList;
class WXDLLEXPORT wxListCtrl;

class EditorFrame;

#include "wx/treectrl.h"
#include "wx/xml/xml.h"
#include "nodesdb.h"




class NodeHandler : public wxObject
{
    public:
        static NodeHandler *Find(wxXmlNode *node);
    
        NodeHandler(NodeInfo *ni);
        virtual ~NodeHandler();
        
        virtual bool CanHandle(wxXmlNode *node);
        virtual wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent,
                                    wxXmlNode *node);
        virtual PropertyInfoArray& GetPropsList(wxXmlNode *node);
        virtual int GetTreeIcon(wxXmlNode * WXUNUSED(node)) {return m_NodeInfo->Icon;}
        virtual wxString GetTreeString(wxXmlNode *node);
        wxArrayString& GetChildTypes();
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
        virtual wxXmlNode *GetRealNode(wxXmlNode *node) { return node; }
        
    protected:

        NodeInfo *m_NodeInfo;
        wxArrayString m_ChildTypes;
        
        static wxList ms_Handlers;
        static bool ms_HandlersLoaded;
};

// wxPanel handler
class NodeHandlerPanel : public NodeHandler
{
    public:
        NodeHandlerPanel(NodeInfo *ni) : NodeHandler(ni) {}
        
        virtual wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent,
                                    wxXmlNode *node);
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
};


// wxSizers handler
class NodeHandlerSizer : public NodeHandlerPanel
{
    public:
        NodeHandlerSizer(NodeInfo *ni) : NodeHandlerPanel(ni) {}
        
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
        virtual int GetTreeIcon(wxXmlNode *node);
};


// item of wxSizer handler
class NodeHandlerSizerItem : public NodeHandler
{
    public:
        NodeHandlerSizerItem(NodeInfo *ni) : NodeHandler(ni) {}

        virtual wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent,
                                    wxXmlNode *node);
        virtual PropertyInfoArray& GetPropsList(wxXmlNode *node);
        virtual wxString GetTreeString(wxXmlNode *node);
        virtual int GetTreeIcon(wxXmlNode *node);
        virtual wxXmlNode *GetRealNode(wxXmlNode *node);

    private:
        PropertyInfoArray m_dummy;
};



// wxNotebook handler
class NodeHandlerNotebook : public NodeHandlerPanel
{
    public:
        NodeHandlerNotebook(NodeInfo *ni) : NodeHandlerPanel(ni) {}
        
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
};


// notebook page handler
class NodeHandlerNotebookPage : public NodeHandlerSizerItem
{
    public:
        NodeHandlerNotebookPage(NodeInfo *ni) : 
                                        NodeHandlerSizerItem(ni) {}
};



// Generic handler which handles everything
class NodeHandlerUnknown : public NodeHandler
{
    public:
        NodeHandlerUnknown() : NodeHandler(new NodeInfo) {}
        
        virtual bool CanHandle(wxXmlNode *node) { return TRUE; }
};


#endif 
