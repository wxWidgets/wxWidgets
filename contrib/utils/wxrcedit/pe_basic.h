/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "pe_basic.h"
#endif

#ifndef _PE_BASIC_H_
#define _PE_BASIC_H_

#include "propedit.h"

class WXDLLEXPORT wxChoice;



class PropEditCtrlNull : public PropEditCtrl
{
    public:
        PropEditCtrlNull(PropertiesFrame *propFrame)
           : PropEditCtrl(propFrame) {}
                
        virtual void BeginEdit(const wxRect& rect, wxTreeItemId ti) {}
        virtual void EndEdit() {}
                
        virtual wxWindow* CreateEditCtrl() {return NULL;}
        
        virtual void ReadValue() {}
        virtual void WriteValue() {}
};



class PropEditCtrlTxt : public PropEditCtrl
{
    public:
        PropEditCtrlTxt(PropertiesFrame *propFrame)
           : PropEditCtrl(propFrame) {}
                
        virtual wxWindow* CreateEditCtrl();
        
        virtual void ReadValue();
        virtual void WriteValue();
        
    protected:
        wxTextCtrl *m_TextCtrl;

    private:
        DECLARE_EVENT_TABLE()
        void OnText(wxCommandEvent& event);
};



class PropEditCtrlInt : public PropEditCtrlTxt
{
    public:
        PropEditCtrlInt(PropertiesFrame *propFrame)
           : PropEditCtrlTxt(propFrame) {}
                
        virtual wxWindow* CreateEditCtrl();
};



class PropEditCtrlBool : public PropEditCtrl
{
    public:
        PropEditCtrlBool(PropertiesFrame *propFrame)
           : PropEditCtrl(propFrame) {}
                
        virtual wxWindow* CreateEditCtrl();
        
        virtual void ReadValue();
        virtual void WriteValue();
        virtual wxString GetValueAsText(wxTreeItemId ti);
    
    protected:
        wxChoice *m_Choice;

        DECLARE_EVENT_TABLE()
        void OnChoice(wxCommandEvent& event);
};


class PropEditCtrlCoord : public PropEditCtrlTxt
{
    public:
        PropEditCtrlCoord(PropertiesFrame *propFrame);
        ~PropEditCtrlCoord();
    
        virtual wxTreeItemId CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo);
        
    private:
        PropEditCtrl *m_CtrlX, *m_CtrlY, *m_CtrlDlg;
};


class PropEditCtrlDim : public PropEditCtrlTxt
{
    public:
        PropEditCtrlDim(PropertiesFrame *propFrame);
        ~PropEditCtrlDim();
    
        virtual wxTreeItemId CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo);
        
    private:
        PropEditCtrl *m_CtrlX, *m_CtrlDlg;
};



class PropEditCtrlXMLID : public PropEditCtrlTxt
{
    public:
        PropEditCtrlXMLID(PropertiesFrame *propFrame)
           : PropEditCtrlTxt(propFrame) {}
                
        virtual void ReadValue();
        virtual void WriteValue();
        virtual void Clear();
        virtual bool HasDetails() { return TRUE; }
        virtual void OnDetails();
        virtual wxString GetValueAsText(wxTreeItemId ti);
        virtual bool IsPresent(const PropertyInfo& pinfo);
};






#endif
