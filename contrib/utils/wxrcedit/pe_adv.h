/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "pe_adv.h"
#endif

#ifndef _PE_ADV_H_
#define _PE_ADV_H_

#include "propedit.h"
#include "pe_basic.h"

class WXDLLEXPORT wxChoice;


class PropEditCtrlFont : public PropEditCtrlTxt
{
    public:
        PropEditCtrlFont(PropertiesFrame *propFrame)
           : PropEditCtrlTxt(propFrame) {}
                
        virtual wxWindow* CreateEditCtrl();
        virtual wxTreeItemId CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo);
};



class PropEditCtrlChoice : public PropEditCtrl
{
    public:
        PropEditCtrlChoice(PropertiesFrame *propFrame)
           : PropEditCtrl(propFrame) {}
                
        virtual wxWindow* CreateEditCtrl();
        
        virtual void ReadValue();
        virtual void WriteValue();
    
    protected:
        wxChoice *m_Choice;

        DECLARE_EVENT_TABLE()
        void OnChoice(wxCommandEvent& event);
};


class PropEditCtrlColor : public PropEditCtrlTxt
{
    public:
        PropEditCtrlColor(PropertiesFrame *propFrame)
            : PropEditCtrlTxt(propFrame) {}

        virtual bool HasDetails() { return TRUE; }
        virtual void OnDetails();
};




class PropEditCtrlFlags : public PropEditCtrlTxt
{
    public:
        PropEditCtrlFlags(PropertiesFrame *propFrame)
            : PropEditCtrlTxt(propFrame) {}

        virtual bool HasDetails() { return TRUE; }
        virtual void OnDetails();
};



class PropEditCtrlFile : public PropEditCtrlTxt
{
    public:
        PropEditCtrlFile(PropertiesFrame *propFrame)
            : PropEditCtrlTxt(propFrame) {}

        virtual bool HasDetails() { return TRUE; }
        virtual void OnDetails();
        
        virtual wxString GetFileTypes();
};


class PropEditCtrlImageFile : public PropEditCtrlFile
{
    public:
        PropEditCtrlImageFile(PropertiesFrame *propFrame)
            : PropEditCtrlFile(propFrame) {}

        virtual wxString GetFileTypes();
};

#endif
