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
#include "prophnd.h"



enum HandlerType
{
    HANDLER_NONE = 0,
    HANDLER_PANEL = 1,
    HANDLER_NORMAL,
    HANDLER_SIZER,
    HANDLER_SIZERITEM,
    HANDLER_NOTEBOOK,
    HANDLER_NOTEBOOKPAGE
};


class NodeInfo
{
    public:
        wxString Node;
        HandlerType Type;   
        PropertyInfoArray Props;
        wxArrayString DerivedFrom;
        bool Abstract;
        wxString ChildType;
        int Icon;
    
        void Read(const wxString& filename);
};

WX_DECLARE_OBJARRAY(NodeInfo, NodeInfoArray);




class NodeHandler : public wxObject
{
    public:
        static NodeHandler *CreateFromFile(const wxString& filename, EditorFrame *frame);
    
        NodeHandler(EditorFrame *frame, NodeInfo *ni);
        virtual ~NodeHandler();
        
        virtual bool CanHandle(wxXmlNode *node);
        virtual wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent,
                                    wxXmlNode *node);
        virtual void CreatePropsList(wxListCtrl *listctrl, wxXmlNode *node);
        virtual int GetTreeIcon(wxXmlNode * WXUNUSED(node)) {return m_NodeInfo->Icon;}
        virtual wxString GetTreeString(wxXmlNode *node);
        wxPanel *CreatePropEditPanel(wxWindow *parent, wxListCtrl *listctrl, int index);
        wxArrayString& GetChildTypes();
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
        virtual wxXmlNode *GetRealNode(wxXmlNode *node) { return node; }
        
    protected:
        NodeInfo *m_NodeInfo;
        wxArrayString m_ChildTypes;

        static PropertyHandler* s_PropHandlers[PROP_TYPES_CNT];
        static int s_RefCnt;       
        static NodeInfoArray* s_AllNodes;
        
        void CreatePropHandlers();
};

// wxPanel handler
class NodeHandlerPanel : public NodeHandler
{
    public:
        NodeHandlerPanel(EditorFrame *frame, NodeInfo *ni) : NodeHandler(frame, ni) {}
        
        virtual wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent,
                                    wxXmlNode *node);
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
};


// wxSizers handler
class NodeHandlerSizer : public NodeHandlerPanel
{
    public:
        NodeHandlerSizer(EditorFrame *frame, NodeInfo *ni) : NodeHandlerPanel(frame, ni) {}
        
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
        virtual int GetTreeIcon(wxXmlNode *node);
};


// item of wxSizer handler
class NodeHandlerSizerItem : public NodeHandler
{
    public:
        NodeHandlerSizerItem(EditorFrame *frame, NodeInfo *ni) : NodeHandler(frame, ni) {}

        virtual wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent,
                                    wxXmlNode *node);
        virtual void CreatePropsList(wxListCtrl *listctrl, wxXmlNode *node);
        virtual wxString GetTreeString(wxXmlNode *node);
        virtual int GetTreeIcon(wxXmlNode *node);
        virtual wxXmlNode *GetRealNode(wxXmlNode *node);
};



// wxNotebook handler
class NodeHandlerNotebook : public NodeHandlerPanel
{
    public:
        NodeHandlerNotebook(EditorFrame *frame, NodeInfo *ni) : NodeHandlerPanel(frame, ni) {}
        
        virtual void InsertNode(wxXmlNode *parent, wxXmlNode *node, wxXmlNode *insert_before = NULL);
};


// notebook page handler
class NodeHandlerNotebookPage : public NodeHandlerSizerItem
{
    public:
        NodeHandlerNotebookPage(EditorFrame *frame, NodeInfo *ni) : 
                                        NodeHandlerSizerItem(frame, ni) {}
};



// Generic handler which handles everything
class NodeHandlerUnknown : public NodeHandler
{
    public:
        NodeHandlerUnknown(EditorFrame *frame) : NodeHandler(frame, new NodeInfo) {}
        
        virtual bool CanHandle(wxXmlNode *node) { return TRUE; }
};


#endif 
