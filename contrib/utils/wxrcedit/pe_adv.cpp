/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "pe_adv.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/tokenzr.h"
#include "wx/wx.h"
#include "wx/dialog.h"
#include "wx/checklst.h"
#include "pe_basic.h"
#include "pe_adv.h"
#include "xmlhelpr.h"
#include "editor.h"
#include "preview.h"
#include "wx/colordlg.h"
#include "wx/config.h"



wxWindow* PropEditCtrlFont::CreateEditCtrl()
{
    PropEditCtrlTxt::CreateEditCtrl();
    m_TextCtrl->Enable(FALSE);
    return m_TextCtrl;
}



wxTreeItemId PropEditCtrlFont::CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo)
{
    wxTreeItemId ti = PropEditCtrlTxt::CreateTreeEntry(parent, pinfo);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("integer"), pinfo.Name + _T("/size"), wxEmptyString), &ti);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("text"), pinfo.Name + _T("/face"), wxEmptyString), &ti);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("choice"), pinfo.Name + _T("/style"), _T("normal,italic,slant")), &ti);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("choice"), pinfo.Name + _T("/weight"), _T("normal,light,bold")), &ti);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("choice"), pinfo.Name + _T("/family"), _T("default,decorative,roman,script,swiss,modern")), &ti);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("bool"), pinfo.Name + _T("/underlined"), wxEmptyString), &ti);
    m_PropFrame->AddSingleProp(PropertyInfo(_T("text"), pinfo.Name + _T("/encoding"), wxEmptyString), &ti);
    return ti;
}






BEGIN_EVENT_TABLE(PropEditCtrlChoice, PropEditCtrl)
    EVT_CHOICE(-1, PropEditCtrlChoice::OnChoice)
END_EVENT_TABLE()

wxWindow* PropEditCtrlChoice::CreateEditCtrl()
{
    m_Choice = new wxChoice(this, -1);
    
    return m_Choice;
}


        
void PropEditCtrlChoice::ReadValue()
{
    wxStringTokenizer tkn(m_PropInfo->MoreInfo, _T(","));
    m_Choice->Clear();
    while (tkn.HasMoreTokens())
        m_Choice->Append(tkn.GetNextToken());

    wxString sel =  XmlReadValue(GetNode(), m_PropInfo->Name);
    if (!!sel) m_Choice->SetStringSelection(sel);
}



void PropEditCtrlChoice::WriteValue()
{
    XmlWriteValue(GetNode(), m_PropInfo->Name,
                  m_Choice->GetStringSelection());
}



void PropEditCtrlChoice::OnChoice(wxCommandEvent& event)
{
    if (CanSave()) 
    {
        WriteValue();
        EditorFrame::Get()->NotifyChanged(CHANGED_PROPS);    
    }
}





void PropEditCtrlColor::OnDetails()
{
    wxColour clr;
    wxString txt = m_TextCtrl->GetValue();
    long unsigned tmp;
    
    if (txt.Length() == 7 && txt[0] == _T('#') &&
        wxSscanf(txt.c_str(), _T("#%lX"), &tmp) == 1)
        clr = wxColour((tmp & 0xFF0000) >> 16, 
                       (tmp & 0x00FF00) >> 8, 
                       (tmp & 0x0000FF));
    clr = wxGetColourFromUser(NULL, clr);
    if (clr.Ok())
    {
        txt.Printf(_T("#%02X%02X%02X"), clr.Red(), clr.Green(), clr.Blue());
        m_TextCtrl->SetValue(txt);
        WriteValue();
    }
}










void PropEditCtrlFlags::OnDetails()
{
    wxString t,txt = m_TextCtrl->GetValue();
    wxArrayString arr;
    size_t i;
    int j;
    
    wxStringTokenizer tkn(m_PropInfo->MoreInfo, _T(","));
    while (tkn.HasMoreTokens())
        arr.Add(tkn.GetNextToken());

    wxConfigBase *cfg = wxConfigBase::Get();
    
    wxDialog dlg(m_PropFrame, -1, _("Flags"), 
            wxPoint(cfg->Read(_T("flagsdlg_x"), -1), cfg->Read(_T("flagsdlg_y"), -1)),
            wxSize(cfg->Read(_T("flagsdlg_w"), 300), cfg->Read(_T("flagsdlg_h"), 300)),
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxSizer *sz = new wxBoxSizer(wxVERTICAL);
    wxCheckListBox *lbox = new wxCheckListBox(&dlg, -1);
    sz->Add(lbox, 1, wxEXPAND | wxALL, 10);
    wxSizer *sz2 = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btnok = new wxButton(&dlg, wxID_OK, _("OK"));
    btnok->SetDefault();
    sz2->Add(btnok);
    sz2->Add(new wxButton(&dlg, wxID_CANCEL, _("Cancel")), 0, wxLEFT, 10);
    sz->Add(sz2, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

    dlg.SetSizer(sz);
    dlg.SetAutoLayout(TRUE);
    dlg.Layout();
    
    for (i = 0; i < arr.GetCount(); i++)
        lbox->Append(arr[i]);
  
    tkn.SetString(txt, _T("| "));
    while (tkn.HasMoreTokens())
    {
        t = tkn.GetNextToken();
        j = arr.Index(t);
        if (j != wxNOT_FOUND) lbox->Check(j);
    }
            
    
    if (dlg.ShowModal() != wxID_OK) return;
    
    txt.Empty();
    
    for (i = 0; i < arr.GetCount(); i++)
        if (lbox->IsChecked(i))
            txt << arr[i] << _T('|');
    if (!txt.IsEmpty()) txt.RemoveLast();

    m_TextCtrl->SetValue(txt);
    WriteValue();

    cfg->Write(_T("flagsdlg_x"), (long)dlg.GetPosition().x);
    cfg->Write(_T("flagsdlg_y"), (long)dlg.GetPosition().y);
    cfg->Write(_T("flagsdlg_w"), (long)dlg.GetSize().x);
    cfg->Write(_T("flagsdlg_h"), (long)dlg.GetSize().y);
}








wxString PropEditCtrlFile::GetFileTypes()
{
    return m_PropInfo->MoreInfo;
}



void PropEditCtrlFile::OnDetails()
{
    wxString txt = m_TextCtrl->GetValue();
    txt = wxPathOnly(EditorFrame::Get()->GetFileName()) + _T("/") + txt;
    wxString name = wxFileSelector(_("Choose file"), 
                                   wxPathOnly(txt),
                                   wxFileNameFromPath(txt),
                                   _T(""),
                                   GetFileTypes(),
                                   wxOPEN | wxFILE_MUST_EXIST);
    if (!name) return;
    
    // compute relative path:
    wxArrayString axrc, afile;
    wxStringTokenizer tkn;
    tkn.SetString(name, _T("/\\"));
    while (tkn.HasMoreTokens()) afile.Add(tkn.GetNextToken());
    tkn.SetString(EditorFrame::Get()->GetFileName(), _T("/\\"));
    while (tkn.HasMoreTokens()) axrc.Add(tkn.GetNextToken());
    
    if (afile.GetCount() == 0 || axrc.GetCount() == 0)
        txt = name;
    else
    {
        while (axrc[0] == afile[0])
        {
            afile.Remove(0u);
            axrc.Remove(0u);
        }
        size_t i;
        txt.Empty();
        for (i = 0; i < axrc.GetCount()-1/*w/o filename*/; i++) txt << _T("../");
        for (i = 0; i < afile.GetCount(); i++) txt << afile[i] << _T("/");
        txt.RemoveLast();
    }

    m_TextCtrl->SetValue(txt);
    WriteValue();
}



wxString PropEditCtrlImageFile::GetFileTypes()
{
    return _("GIF files (*.gif)|*.gif|"
             "JPEG files (*.jpg)|*.jpg|"
             "PNG files (*.png)|*.png|"
             "BMP files (*.bmp)|*.bmp|"
             "All files (*)|*");
}
