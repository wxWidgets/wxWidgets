/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "pe_basic.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/tokenzr.h"
#include "pe_basic.h"
#include "xmlhelpr.h"
#include "editor.h"
#include "preview.h"


BEGIN_EVENT_TABLE(PropEditCtrlTxt, PropEditCtrl)
    EVT_TEXT(-1, PropEditCtrlTxt::OnText)
END_EVENT_TABLE()


wxWindow *PropEditCtrlTxt::CreateEditCtrl()
{
    return (m_TextCtrl = new wxTextCtrl(this, -1));
}



void PropEditCtrlTxt::OnText(wxCommandEvent& event)
{
    if (CanSave()) WriteValue();
}



void PropEditCtrlTxt::ReadValue()
{
    m_TextCtrl->SetValue(XmlReadValue(GetNode(), m_PropInfo->Name));
}



void PropEditCtrlTxt::WriteValue()
{
    wxString newv = m_TextCtrl->GetValue();
    XmlWriteValue(GetNode(), m_PropInfo->Name, newv);
    m_TreeCtrl->SetItemBold(m_TreeItem, TRUE);
}



wxWindow *PropEditCtrlInt::CreateEditCtrl()
{
    PropEditCtrlTxt::CreateEditCtrl();
    m_TextCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    return m_TextCtrl;
}







BEGIN_EVENT_TABLE(PropEditCtrlBool, PropEditCtrl)
    EVT_CHOICE(-1, PropEditCtrlBool::OnChoice)
END_EVENT_TABLE()

wxWindow *PropEditCtrlBool::CreateEditCtrl()
{
    m_Choice = new wxChoice(this, -1);
    m_Choice->Append(_T("false"));
    m_Choice->Append(_T("true"));
    return m_Choice;
}



void PropEditCtrlBool::ReadValue()
{
    int sel;
    if (XmlReadValue(GetNode(), m_PropInfo->Name) == _T("0")) sel = 0;
    else sel = 1;
    m_Choice->SetSelection(sel);
}



void PropEditCtrlBool::WriteValue()
{
    wxString newv = m_Choice->GetSelection() == 0 ? _T("0") : _T("1");

    XmlWriteValue(GetNode(), m_PropInfo->Name, newv);
    m_TreeCtrl->SetItemBold(m_TreeItem, TRUE);
}



wxString PropEditCtrlBool::GetValueAsText(wxTreeItemId ti)
{
    PropertyInfo& pir = ((PETreeData*)m_TreeCtrl->GetItemData(ti))->PropInfo;
    wxString x = XmlReadValue(GetNode(), pir.Name);
    if (x == _T("1")) x = _T("true");
    else if (x == _T("0")) x = _T("false");
    return x;
}



void PropEditCtrlBool::OnChoice(wxCommandEvent& event)
{
    if (CanSave()) WriteValue();
}




// --------------------------- PropEditCtrlCoord -----------------------

class PropEditCtrlCoordXY : public PropEditCtrlInt
{
    public:
        PropEditCtrlCoordXY(PropertiesFrame *propFrame, int which)
           : PropEditCtrlInt(propFrame), m_which(which) {}
        
        virtual void ReadValue()
        {
            wxString s = XmlReadValue(GetNode(), m_PropInfo->Name);
            if (s.IsEmpty())
            {
                m_c[0] = m_c[1] = _T("-1");
            }
            else
            {
                wxStringTokenizer tkn(s.BeforeFirst(_T('d')), _T(","));
                m_c[0] = tkn.GetNextToken();
                m_c[1] = tkn.GetNextToken();
            }
            m_TextCtrl->SetValue(m_c[m_which]);
        }
        
        virtual void WriteValue()
        {
            m_c[m_which] = m_TextCtrl->GetValue();
            if (m_c[0].IsEmpty()) m_c[0] = _T("-1");
            if (m_c[1].IsEmpty()) m_c[1] = _T("-1");
            wxString s;
            s << m_c[0] << _T(',') << m_c[1];
            if (m_dlg) s << _T('d');
            XmlWriteValue(GetNode(), m_PropInfo->Name, s);
            m_TreeCtrl->SetItemBold(m_TreeCtrl->GetParent(m_TreeItem), TRUE);
        }
        
        virtual wxString GetValueAsText(wxTreeItemId ti)
        {
            PropertyInfo *pi = &(((PETreeData*)m_TreeCtrl->GetItemData(ti))->PropInfo);
            wxString s = XmlReadValue(GetNode(), pi->Name);
            if (s.IsEmpty())
            {
                m_c[0] = m_c[1] = _T("-1");
            }
            else
            {
                wxStringTokenizer tkn(s.BeforeFirst(_T('d')), _T(","));
                m_c[0] = tkn.GetNextToken();
                m_c[1] = tkn.GetNextToken();
            }
            return m_c[m_which];
        }
        
        virtual wxString GetPropName(const PropertyInfo& pinfo)
        {
            if (m_which == 0) return _T("x"); else return _T("y");
        }

        virtual bool HasClearButton() { return FALSE; }
        virtual bool IsPresent(const PropertyInfo& pinfo) { return FALSE; } 

    
    protected:
        wxString m_c[2];
        bool m_dlg;
        int m_which;
};


class PropEditCtrlCoordDlg : public PropEditCtrlBool
{
    public:
        PropEditCtrlCoordDlg(PropertiesFrame *propFrame)
           : PropEditCtrlBool(propFrame) {}
        
        virtual void ReadValue()
        {
            wxString s = XmlReadValue(GetNode(), m_PropInfo->Name);
            if (s.IsEmpty()) m_Choice->SetSelection(0);
            else if (s[s.Length()-1] == _T('d'))
                m_Choice->SetSelection(1);
            else
                m_Choice->SetSelection(0);
        }
        
        virtual void WriteValue()
        {
            wxString s = XmlReadValue(GetNode(), m_PropInfo->Name).BeforeFirst(_T('d'));
            if (m_Choice->GetSelection() == 1) s << _T('d');
            XmlWriteValue(GetNode(), m_PropInfo->Name, s);
            m_TreeCtrl->SetItemBold(m_TreeCtrl->GetParent(m_TreeItem), TRUE);
        }
        
        virtual wxString GetValueAsText(wxTreeItemId ti)
        {
            PropertyInfo *pi = &(((PETreeData*)m_TreeCtrl->GetItemData(ti))->PropInfo);
            wxString s = XmlReadValue(GetNode(), pi->Name);
            if (s.IsEmpty() || s[s.Length()-1] != _T('d'))
                return _("false");
            else
                return _("true");
        }
        
        virtual wxString GetPropName(const PropertyInfo& pinfo)
        {
            return _T("dlg");
        }

        virtual bool HasClearButton() { return FALSE; }
        virtual bool IsPresent(const PropertyInfo& pinfo) { return FALSE; } 
};




PropEditCtrlCoord::PropEditCtrlCoord(PropertiesFrame *propFrame)
           : PropEditCtrlTxt(propFrame)
{
    m_CtrlX = new PropEditCtrlCoordXY(propFrame, 0);
    m_CtrlY = new PropEditCtrlCoordXY(propFrame, 1);
    m_CtrlDlg = new PropEditCtrlCoordDlg(propFrame);
}

PropEditCtrlCoord::~PropEditCtrlCoord()
{
    delete m_CtrlX;
    delete m_CtrlY;
    delete m_CtrlDlg;
}



wxTreeItemId PropEditCtrlCoord::CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo)
{
    wxTreeItemId id = PropEditCtrlTxt::CreateTreeEntry(parent, pinfo);
    m_CtrlX->CreateTreeEntry(id, PropertyInfo(_T("text"), pinfo.Name, _T("")));
    m_CtrlY->CreateTreeEntry(id, PropertyInfo(_T("text"), pinfo.Name, _T("")));
    m_CtrlDlg->CreateTreeEntry(id, PropertyInfo(_T("bool"), pinfo.Name, _T("")));
    return id;
}






// --------------------------- PropEditCtrlDim -----------------------

class PropEditCtrlDimX : public PropEditCtrlInt
{
    public:
        PropEditCtrlDimX(PropertiesFrame *propFrame)
           : PropEditCtrlInt(propFrame){}
        
        virtual void ReadValue()
        {
            wxString s = XmlReadValue(GetNode(), m_PropInfo->Name);
            m_c = s.BeforeFirst(_T('d'));
            m_TextCtrl->SetValue(m_c);
        }
        
        virtual void WriteValue()
        {
            wxString s = XmlReadValue(GetNode(), m_PropInfo->Name);
            bool dlg = !s.IsEmpty() && s[s.Length()-1] == _T('d');
            m_c = m_TextCtrl->GetValue();
            s = m_c;
            if (dlg) s << _T('d');
            XmlWriteValue(GetNode(), m_PropInfo->Name, s);
            m_TreeCtrl->SetItemBold(m_TreeCtrl->GetParent(m_TreeItem), TRUE);
        }
        
        virtual wxString GetValueAsText(wxTreeItemId ti)
        {
            PropertyInfo *pi = &(((PETreeData*)m_TreeCtrl->GetItemData(ti))->PropInfo);
            return XmlReadValue(GetNode(), pi->Name).BeforeFirst(_T('d'));
        }
        
        virtual wxString GetPropName(const PropertyInfo& pinfo)
        {
            return _T("val");
        }

        virtual bool HasClearButton() { return FALSE; }
        virtual bool IsPresent(const PropertyInfo& pinfo) { return FALSE; } 

    
    protected:
        wxString m_c;
};



PropEditCtrlDim::PropEditCtrlDim(PropertiesFrame *propFrame)
           : PropEditCtrlTxt(propFrame)
{
    m_CtrlX = new PropEditCtrlDimX(propFrame);
    m_CtrlDlg = new PropEditCtrlCoordDlg(propFrame);
}

PropEditCtrlDim::~PropEditCtrlDim()
{
    delete m_CtrlX;
    delete m_CtrlDlg;
}



wxTreeItemId PropEditCtrlDim::CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo)
{
    wxTreeItemId id = PropEditCtrlTxt::CreateTreeEntry(parent, pinfo);
    m_CtrlX->CreateTreeEntry(id, PropertyInfo(_T("text"), pinfo.Name, _T("")));
    m_CtrlDlg->CreateTreeEntry(id, PropertyInfo(_T("bool"), pinfo.Name, _T("")));
    return id;
}




// --------------------- PropEditCtrlXMLID -----------------------------


void PropEditCtrlXMLID::ReadValue()
{
    m_TextCtrl->SetValue(GetNode()->GetPropVal(_T("name"), wxEmptyString));
}



void PropEditCtrlXMLID::WriteValue()
{
    wxString s =m_TextCtrl->GetValue();
    if (s.IsEmpty()) s = _T("-1");

    GetNode()->DeleteProperty(_T("name"));
    GetNode()->AddProperty(_T("name"), s);

    m_TreeCtrl->SetItemBold(m_TreeItem, TRUE);
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE_SELECTED);
}



void PropEditCtrlXMLID::Clear()
{
    EndEdit();
    GetNode()->DeleteProperty(_T("name"));
    m_TreeCtrl->SetItemBold(m_TreeItem, FALSE);
    EditorFrame::Get()->NotifyChanged(CHANGED_TREE_SELECTED);
}



void PropEditCtrlXMLID::OnDetails()
{
    wxString choices[] = {wxString(_T("-1"))
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
    m_TextCtrl->SetValue(s);
    WriteValue();
}



wxString PropEditCtrlXMLID::GetValueAsText(wxTreeItemId ti)
{
    return GetNode()->GetPropVal(_T("name"), wxEmptyString);
}



bool PropEditCtrlXMLID::IsPresent(const PropertyInfo& pinfo)
{
    return GetNode()->HasProp(_T("name"));
}




