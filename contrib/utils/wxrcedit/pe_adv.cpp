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
#include "pe_basic.h"
#include "pe_adv.h"
#include "xmlhelpr.h"
#include "editor.h"
#include "preview.h"
#include "wx/colordlg.h"



wxWindow* PropEditCtrlFont::CreateEditCtrl()
{
    PropEditCtrlTxt::CreateEditCtrl();
    m_TextCtrl->Enable(FALSE);
    return m_TextCtrl;
}



wxTreeItemId PropEditCtrlFont::CreateTreeEntry(wxTreeItemId parent, const PropertyInfo& pinfo)
{
    wxTreeItemId ti = PropEditCtrlTxt::CreateTreeEntry(parent, pinfo);
    return ti;
}




BEGIN_EVENT_TABLE(PropEditCtrlChoice, PropEditCtrl)
    EVT_CHOICE(-1, PropEditCtrlChoice::OnChoice)
END_EVENT_TABLE()

wxWindow* PropEditCtrlChoice::CreateEditCtrl()
{
    m_Choice = new wxChoice(this, -1);
    m_Choice->Append(_T("false"));
    m_Choice->Append(_T("true"));
    return m_Choice;
}


        
void PropEditCtrlChoice::ReadValue()
{
}



void PropEditCtrlChoice::WriteValue()
{
}



void PropEditCtrlChoice::OnChoice(wxCommandEvent& event)
{
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


