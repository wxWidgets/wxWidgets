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


class wxXmlNode;
class WXDLLEXPORT wxTreeCtrl;
class WXDLLEXPORT wxScrolledWindow;
class WXDLLEXPORT wxSplitterWindow;

#include "wx/frame.h"
#include "wx/treectrl.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"

class NodeHandler;
class wxXmlNode;
class wxXmlDocument;
class EditorTreeCtrl;

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
        
        void RefreshTree();
        void RefreshPreview(wxXmlNode *node);
        void RefreshProps(wxXmlNode *node);
        void RefreshPropsEdit();
        bool SelectNode(wxXmlNode *node, wxTreeItemId *root = NULL);
        
        wxTreeItemId CreateTreeNode(wxTreeCtrl *treectrl, wxTreeItemId parent, wxXmlNode *node);
        void CreatePropsList(wxListCtrl *listctrl, wxXmlNode *node);
  
        void NotifyChanged(int change_type);
        
        void RegisterHandlers(const wxString& dir);
        
        NodeHandler *FindHandler(wxXmlNode *node);
        
        static EditorFrame *Get() { return ms_Instance; }
        
    private:
        static EditorFrame *ms_Instance;
    
        wxTreeCtrl *m_TreeCtrl;
        wxTextCtrl *m_XMLIDCtrl;
        wxImageList *m_ImgList, *m_ImgListProp;
        wxPanel *m_PropsPanel;
        wxScrolledWindow *m_PropsEditPanel;
        wxSplitterWindow *m_Splitter, *m_Splitter2;
        wxListCtrl *m_PropsList;
        int m_SelectedProp;
        
        wxList m_Handlers;

        wxXmlNode *m_SelectedNode;

        wxString m_FileName;
        wxXmlDocument *m_Resource;
        wxWindow *m_Preview;

        DECLARE_EVENT_TABLE()
        void OnTreeSel(wxTreeEvent& event);
        void OnToolbar(wxCommandEvent& event);
        void OnNew(wxCommandEvent& event);
        void OnXMLIDEdit(wxCommandEvent& event);
        void OnXMLIDPick(wxCommandEvent& event);
        void OnEditCode(wxCommandEvent& event);
        void OnClearProp(wxCommandEvent& event);
        void OnPropSel(wxListEvent& event);
        void OnNewNode(wxCommandEvent& event);
        void OnRightClickTree(wxPoint pos);
};


#endif
