/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "propedit.h"
#endif

#ifndef _PROPEDIT_H_
#define _PROPEDIT_H_

#include "wx/panel.h"
#include "wx/treectrl.h"
#include "nodesdb.h"
#include "propframe.h"

class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxGenericTreeCtrl;
class WXDLLEXPORT wxTextCtrl;



class PropEditCtrl : public wxPanel
{
    public:
        PropEditCtrl(PropertiesFrame *propFrame)
           : wxPanel(propFrame->m_valueWindow, -1),
             m_PropFrame(propFrame), m_Created(FALSE), m_TreeCtrl(propFrame->m_tree) 
             {Show(FALSE);}
                
        virtual void BeginEdit(const wxRect& rect, wxTreeItemId ti);
        virtual void EndEdit();
        
        virtual wxTreeItemId CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo);
        virtual wxWindow* CreateEditCtrl() = 0;
        
        virtual bool IsPresent(const PropertyInfo& pinfo);
        
        virtual void Clear();
        virtual void ReadValue() = 0;
        virtual void WriteValue() = 0;
        virtual wxString GetValueAsText(wxTreeItemId ti);
        virtual wxString GetPropName(const PropertyInfo& pinfo)
                { return pinfo.Name.AfterLast(_T('/')); }
        
        virtual bool HasDetails() { return FALSE; }
        virtual void OnDetails() {}
        virtual bool HasClearButton() { return TRUE; }
        
        void OnButtonDetails(wxCommandEvent& event);
        void OnButtonClear(wxCommandEvent& event);
    
    protected:
        wxXmlNode *GetNode() { return m_PropFrame->m_Node; }
        bool CanSave() { return m_CanSave; }
    
        PropertiesFrame *m_PropFrame;
        bool m_Created;
        wxGenericTreeCtrl *m_TreeCtrl;
        wxTreeItemId m_TreeItem;
        wxWindow *m_TheCtrl;
        PropertyInfo *m_PropInfo;
        
        bool m_CanSave;
        
        DECLARE_EVENT_TABLE()
};



class PETreeData : public wxTreeItemData
{
    public:
        PETreeData(PropEditCtrl *p, const PropertyInfo& pi) : 
                wxTreeItemData(),
                EditCtrl(p), PropInfo(pi) {}
        PropEditCtrl *EditCtrl;
        PropertyInfo PropInfo;
};


#endif
