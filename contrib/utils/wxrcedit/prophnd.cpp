/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "prophnd.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "prophnd.h"
#include "wx/xml/xml.h"
#include "wx/wx.h"
#include "wx/arrimpl.cpp"
#include "wx/valtext.h"
#include "wx/tokenzr.h"
#include "wx/checklst.h"
#include "wx/listctrl.h"
#include "xmlhelpr.h"
#include "editor.h"


WX_DEFINE_OBJARRAY(PropertyInfoArray);


enum
{
    ID_EDITCTRL = wxID_HIGHEST + 1000,
    ID_XEDIT,
    ID_YEDIT,
    ID_USEDLG,
    ID_BOOLVAL,
    ID_CHECKLIST
};



class PropertyPanel : public wxPanel
{
    public:
        PropertyPanel(wxWindow *parent, PropertyHandler *hnd, PropsListInfo *pli) 
                    : wxPanel(parent, -1), m_Handler(hnd), m_PLI(pli) {}
        
        void Update(const wxString& value)
        {
            XmlWriteValue(m_PLI->m_Node, m_PLI->m_PropInfo->Name, value);
            m_PLI->m_ListCtrl->SetItemImage(m_PLI->m_Index, 1, 1);
            m_PLI->m_ListCtrl->SetItem(m_PLI->m_Index, 1, 
                   m_Handler->GetBriefValue(m_PLI->m_Node, m_PLI->m_PropInfo));
        }
    
    protected:
        PropertyHandler *m_Handler;
        PropsListInfo *m_PLI;
};





int PropertyHandler::CreateListItem(wxListCtrl *listctrl, wxXmlNode *node, PropertyInfo *pi)
{
    wxString name, value;
    int iconnum;
    
    if (XmlFindNode(node, pi->Name) == NULL) iconnum = 0; else iconnum = 1;
    name = pi->Name;
    value = GetBriefValue(node, pi);

    long pos = listctrl->GetItemCount();
    listctrl->InsertItem(pos, name, iconnum);
    listctrl->SetItem(pos, 1, value);
    return pos;
}



wxString PropertyHandler::GetBriefValue(wxXmlNode *node, PropertyInfo *pi)
{
    return XmlReadValue(node, pi->Name);
}






class TextPropPanel : public PropertyPanel
{
    public:
        TextPropPanel(wxWindow *parent, PropertyHandler *hnd, PropsListInfo *pli) : PropertyPanel(parent, hnd, pli)
        {
            wxSizer *sz = new wxBoxSizer(wxVERTICAL);
            wxTextCtrl *tc;
    
            sz->Add(new wxStaticText(this, -1, _("Value:")), 0, wxLEFT, 5);
            sz->Add(tc = new wxTextCtrl(this, ID_EDITCTRL, XmlReadValue(pli->m_Node, pli->m_PropInfo->Name)), 0, wxALL|wxEXPAND, 5);
            tc->SetFocus();
    
            SetAutoLayout(TRUE);
            SetSizer(sz);
            Layout();
        }
        
        void OnEdit(wxCommandEvent &event)
        {
            Update(((wxTextCtrl*)event.GetEventObject())->GetValue());
        }
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TextPropPanel, PropertyPanel)
    EVT_TEXT(ID_EDITCTRL, TextPropPanel::OnEdit)
END_EVENT_TABLE()


wxPanel *TextPropertyHandler::CreateEditPanel(wxWindow *parent, PropsListInfo *pli)
{
    return new TextPropPanel(parent, this, pli);
}











class CoordPropPanel : public PropertyPanel
{
    public:
        CoordPropPanel(wxWindow *parent, PropertyHandler *hnd, PropsListInfo *pli) : PropertyPanel(parent, hnd, pli)
        {    
            wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
            wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
            m_ed1 = m_ed2 = NULL; m_chb = NULL;
    
            sz->Add(new wxStaticText(this, -1, _("X:")), 0, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5);
            sz->Add(m_ed1 = new wxTextCtrl(this, ID_XEDIT, "",
                                 wxDefaultPosition, wxDefaultSize, 0,
                                 wxTextValidator(wxFILTER_NUMERIC)),
                    1, wxRIGHT, 5);
            m_ed1->SetFocus();

            sz->Add(new wxStaticText(this, -1, _("Y:")), 0, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5);
            sz->Add(m_ed2 = new wxTextCtrl(this, ID_YEDIT, "",
                                 wxDefaultPosition, wxDefaultSize, 0,
                                 wxTextValidator(wxFILTER_NUMERIC)),
                    1, wxRIGHT, 5);
            sizer->Add(sz, 0, wxEXPAND|wxTOP, 5);
    
            sizer->Add(m_chb = new wxCheckBox(this, ID_USEDLG, _("Use dialog units")), 0, wxLEFT|wxTOP, 5);
    
            SetAutoLayout(TRUE);
            SetSizer(sizer);
            Layout();
            
            wxString val = XmlReadValue(pli->m_Node, pli->m_PropInfo->Name);
            m_chb->SetValue(val.Len()==0 || val[val.Len()-1] == 'd');
            m_ed1->SetValue(val.BeforeFirst(','));
            m_ed2->SetValue(val.AfterFirst(',').BeforeFirst('d'));
        }
        
        void OnEdit(wxCommandEvent &event)
        {
            wxString val, v1, v2;
            
            if (m_ed1 == NULL || m_ed2 == NULL || m_chb == NULL) return;
            
            v1 = m_ed1->GetValue();
            v2 = m_ed2->GetValue();
            if (v1.IsEmpty() || v2.IsEmpty()) return;
            val = v1 + "," + v2;
            if (m_chb->GetValue()) val << 'd';
            Update(val);
        }
        
        wxTextCtrl *m_ed1, *m_ed2;
        wxCheckBox *m_chb;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(CoordPropPanel, PropertyPanel)
    EVT_TEXT(ID_XEDIT, CoordPropPanel::OnEdit)
    EVT_TEXT(ID_YEDIT, CoordPropPanel::OnEdit)
    EVT_CHECKBOX(ID_USEDLG, CoordPropPanel::OnEdit)
END_EVENT_TABLE()

wxPanel *CoordPropertyHandler::CreateEditPanel(wxWindow *parent, PropsListInfo *pli)
{
    return new CoordPropPanel(parent, this, pli);
}







class DimensionPropPanel : public PropertyPanel
{
    public:
        DimensionPropPanel(wxWindow *parent, PropertyHandler *hnd, PropsListInfo *pli) : PropertyPanel(parent, hnd, pli)
        {    
            wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
            m_ed1 = NULL; m_chb = NULL;
    
            sizer->Add(new wxStaticText(this, -1, _("Value:")), 0, wxLEFT, 5);
            sizer->Add(m_ed1 = new wxTextCtrl(this, ID_XEDIT, "",
                                 wxDefaultPosition, wxDefaultSize, 0,
                                 wxTextValidator(wxFILTER_NUMERIC)),
                    0, wxEXPAND, 5);
            m_ed1->SetFocus();
    
            sizer->Add(m_chb = new wxCheckBox(this, ID_USEDLG, _("Use dialog units")), 0, wxLEFT|wxTOP, 5);
    
            SetAutoLayout(TRUE);
            SetSizer(sizer);
            Layout();
            
            wxString val = XmlReadValue(pli->m_Node, pli->m_PropInfo->Name);
            m_chb->SetValue(val.Len()>0 && val[val.Len()-1] == 'd');
            m_ed1->SetValue(val.BeforeFirst('d'));
        }
        
        void OnEdit(wxCommandEvent &event)
        {
            wxString val;
            
            if (m_ed1 == NULL || m_chb == NULL) return;
            
            val = m_ed1->GetValue();
            if (val.IsEmpty()) return;
            if (m_chb->GetValue()) val << 'd';
            Update(val);
        }
        
        wxTextCtrl *m_ed1;
        wxCheckBox *m_chb;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(DimensionPropPanel, PropertyPanel)
    EVT_TEXT(ID_XEDIT, DimensionPropPanel::OnEdit)
    EVT_TEXT(ID_YEDIT, DimensionPropPanel::OnEdit)
    EVT_CHECKBOX(ID_USEDLG, DimensionPropPanel::OnEdit)
END_EVENT_TABLE()

wxPanel *DimensionPropertyHandler::CreateEditPanel(wxWindow *parent, PropsListInfo *pli)
{
    return new DimensionPropPanel(parent, this, pli);
}







class BoolPropPanel : public PropertyPanel
{
    public:
        BoolPropPanel(wxWindow *parent, PropertyHandler *hnd, PropsListInfo *pli) : PropertyPanel(parent, hnd, pli)
        {    
            m_chb = NULL;
            wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(m_chb = new wxCheckBox(this, ID_BOOLVAL, _("On/Yes/True")), 0, wxLEFT|wxTOP, 5);
            SetAutoLayout(TRUE);
            SetSizer(sizer);
            Layout();
            m_chb->SetValue(XmlReadValue(pli->m_Node, pli->m_PropInfo->Name) == "1");
        }
        
        void OnEdit(wxCommandEvent &event)
        {
            if (m_chb == NULL) return;
            if (m_chb->GetValue()) Update("1");
            else Update("0");
        }
        
        wxCheckBox *m_chb;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(BoolPropPanel, PropertyPanel)
    EVT_CHECKBOX(ID_BOOLVAL, BoolPropPanel::OnEdit)
END_EVENT_TABLE()

wxPanel *BoolPropertyHandler::CreateEditPanel(wxWindow *parent, PropsListInfo *pli)
{
    return new BoolPropPanel(parent, this, pli);
}

wxString BoolPropertyHandler::GetBriefValue(wxXmlNode *node, PropertyInfo *pi)
{
    wxString v = XmlReadValue(node, pi->Name);
    if (v.IsEmpty()) return wxEmptyString;
    else if (v == "1") return "true";
    else return "false";
}








class FlagsPropPanel : public PropertyPanel
{
    public:
        FlagsPropPanel(wxWindow *parent, PropertyHandler *hnd, PropsListInfo *pli) : PropertyPanel(parent, hnd, pli)
        {    
            m_chl = NULL;
            wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(m_chl = new wxCheckListBox(this, ID_CHECKLIST), 1, wxEXPAND|wxALL, 5);
            SetAutoLayout(TRUE);
            SetSizer(sizer);
            Layout();
            
            {
                wxStringTokenizer tkn(pli->m_PropInfo->MoreInfo, ",");
                wxString s;
                while (tkn.HasMoreTokens())
                {
                    s = tkn.GetNextToken();
                    m_chl->Append(s);
                    m_flags.Add(s);
                }
            }

            {
                wxStringTokenizer tkn(XmlReadValue(pli->m_Node, pli->m_PropInfo->Name), "| ");
                int index;
                while (tkn.HasMoreTokens())
                {
                    index = m_flags.Index(tkn.GetNextToken());
                    if (index != wxNOT_FOUND)
                        m_chl->Check(index);
                }
            }
        }
        
        void OnEdit(wxCommandEvent &event)
        {
            wxString s;
            bool first = TRUE;
            
            for (size_t i = 0; i < m_flags.GetCount(); i++)
            {
                if (m_chl->IsChecked(i))
                {
                    if (!first) s << '|';
                    s << m_flags[i];
                    first = FALSE;
                }
            }
            Update(s);
            if (m_PLI->m_PropInfo->Name == "orient") 
                // FIXME - dirty hack related to sizers
                EditorFrame::Get()->NotifyChanged(CHANGED_TREE_SELECTED_ICON);
        }
        
        wxCheckListBox *m_chl;
        wxArrayString m_flags;
        
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(FlagsPropPanel, PropertyPanel)
    EVT_CHECKLISTBOX(ID_CHECKLIST, FlagsPropPanel::OnEdit)
END_EVENT_TABLE()

wxPanel *FlagsPropertyHandler::CreateEditPanel(wxWindow *parent, PropsListInfo *pli)
{
    return new FlagsPropPanel(parent, this, pli);
}




wxPanel *NotImplPropertyHandler::CreateEditPanel(wxWindow *parent, PropsListInfo *pli)
{
    wxPanel *p = new wxPanel(parent);
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(p, -1, _("Sorry, this is not supported.\nYou have to edit XML code directly.")), 1, wxEXPAND|wxALL, 5);
    p->SetAutoLayout(TRUE);
    p->SetSizer(sizer);
    p->Layout();
    return p;
}
