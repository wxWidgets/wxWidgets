/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "editor.h"
#endif

#ifndef _EDITOR_H_
#define _EDITOR_H_


#include "wx/frame.h"
#include "wx/treectrl.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"

class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxTreeCtrl;
class WXDLLEXPORT wxScrolledWindow;
class WXDLLEXPORT wxSplitterWindow;
class NodeHandler;
class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxXmlDocument;
class EditorTreeCtrl;
class WXDLLEXPORT wxTreeCtrl;
class WXDLLEXPORT wxListCtrl;
class WXDLLEXPORT wxTreeItemId;
class WXDLLEXPORT wxImageList;
class WXDLLEXPORT wxTreeEvent;
class WXDLLEXPORT wxListEvent;

enum ChangeType
{
    CHANGED_NOTHING = 0,
    CHANGED_TREE = 1,
    CHANGED_TREE_SELECTED = 2,
    CHANGED_TREE_SELECTED_ICON = 4,
    CHANGED_PROPS = 8,
    CHANGED_PROPS_PANEL = 16,
    CHANGED_EVERYTHING = CHANGED_TREE | CHANGED_PROPS | CHANGED_PROPS_PANEL,
};


class EditorFrame : public wxFrame
{
    public:
        friend class EditorTreeCtrl;
    
        EditorFrame(wxFrame *parent, const wxString& filename);
        ~EditorFrame();

        void LoadFile(const wxString& filename);
        void NewFile();
        void SaveFile(const wxString& filename);
        wxString GetFileName() { return m_FileName; }
        
        void RefreshTree();
        bool SelectNode(wxXmlNode *node, wxTreeItemId *root = NULL);
        
        wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent, wxXmlNode *node);
  
        void NotifyChanged(int change_type);
        
        static EditorFrame *Get() { return ms_Instance; }
        
    private:
        static EditorFrame *ms_Instance;
    
        wxTreeCtrl *m_TreeCtrl;
        
        wxXmlNode *m_SelectedNode;
        
        wxXmlNode *m_Clipboard;

        wxString m_FileName;
        wxXmlDocument *m_Resource;

        DECLARE_EVENT_TABLE()
        void OnTreeSel(wxTreeEvent& event);
        void OnToolbar(wxCommandEvent& event);
        void OnNew(wxCommandEvent& event);
        void OnNewNode(wxCommandEvent& event);
        void OnRightClickTree(wxPoint pos);
        void OnClipboardAction(wxCommandEvent& event);
        
        void DeleteSelectedNode();
};


#endif
