//---------------------------------------------------------------------------
// Name:        DlgUser.h,cpp
// Purpose:     Dialog mit Variable Gestaltung durch DlgUser.wxr
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by: 19991105.mj10777
// Created:     19991105
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//---------------------------------------------------------------------------
//-- all #ifdefs that the whole Project needs. ------------------------------
//---------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif
//---------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#endif
//---------------------------------------------------------------------------
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
//---------------------------------------------------------------------------
//-- all #includes that every .cpp needs             ----19990807.mj10777 ---
//---------------------------------------------------------------------------
#include "dlguser.h"
//---------------------------------------------------------------------------
DlgUser::DlgUser(wxWindow *parent, const wxString& title) :
  wxDialog(parent, ID_DIALOG_DSN, title)
{
  SetBackgroundColour("wheat");
  
  wxLayoutConstraints* layout;
  SetAutoLayout(TRUE);
 
//   m_Dsn = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
//   layout = new wxLayoutConstraints;
//   layout->centreX.SameAs(this, wxCentreX);
//   layout->top.SameAs(this, wxTop,10);
//   layout->height.AsIs();
//   layout->width.AsIs();
//   m_Dsn->SetConstraints(layout);

  m_Label1 = new wxStaticText(this, -1, _("User ID:"));
  layout = new wxLayoutConstraints;
  layout->left.SameAs(this, wxLeft, 10);
  layout->top.SameAs(this, wxTop, 10);
  layout->height.AsIs();
  layout->width.Absolute(75);
  m_Label1->SetConstraints(layout);

  m_UserName = new wxTextCtrl(this, -1, "");
  layout = new wxLayoutConstraints;
  layout->left.SameAs(m_Label1, wxRight, 10);
//  layout->top.SameAs(m_Label1, wxTop);
  layout->centreY.SameAs(m_Label1,wxCentreY);
  layout->width.Absolute(200);
  layout->height.AsIs();
  m_UserName->SetConstraints(layout); 
  
  
  m_Label2 = new wxStaticText(this, -1, _("Password:"));
  layout = new wxLayoutConstraints;
  layout->left.SameAs(m_Label1, wxLeft);
  layout->top.SameAs(m_Label1, wxBottom, 10);
  layout->height.AsIs();
  layout->width.SameAs(m_Label1, wxWidth);
  m_Label2->SetConstraints(layout);
  
  m_Password = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
  layout = new wxLayoutConstraints;
  layout->left.SameAs(m_UserName, wxLeft);
  layout->width.SameAs(m_UserName, wxWidth);
 // layout->top.SameAs(m_Label2, wxTop);
  layout->centreY.SameAs(m_Label2,wxCentreY);
  layout->height.AsIs();
  m_Password->SetConstraints(layout);
  
  m_OK = new wxButton(this, wxID_OK, _("Ok"));
  layout = new wxLayoutConstraints;
  layout->left.SameAs(this, wxLeft, 10);
  layout->top.SameAs(m_Label2, wxBottom,10);
  layout->height.AsIs();
  layout->width.Absolute(75);
  m_OK->SetConstraints(layout);

  m_Cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
  layout = new wxLayoutConstraints;
  layout->left.SameAs(m_OK, wxRight, 10);
  layout->top.SameAs(m_OK, wxTop);
  layout->height.AsIs();
  layout->width.SameAs(m_OK, wxWidth);
  m_Cancel->SetConstraints(layout);

  m_OK->SetDefault();
  m_UserName->SetFocus();
  
  
  s_User = "";
  s_Password = "";
  
  Layout();




// wxButton *but1 = new wxButton(this, wxID_OK, "OK", wxPoint(55,110), wxSize(80, 30));
//  wxButton *but2 = new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(210,110), wxSize(80, 30));
//  (void)new wxStaticText(this, -1,_("User ID"), wxPoint(20, 40), wxSize(50, 20),wxALIGN_LEFT);
//  (void)new wxStaticText(this, -1,_("Password"), wxPoint(20, 80), wxSize(50, 20),wxALIGN_LEFT);
  // but1->SetFocus();
//  m_OK->SetDefault();
}

 void DlgUser::OnInit()
 {
   wxString Temp; Temp.Printf(_(">>> %s <<< "),s_DSN.c_str());
   SetTitle(Temp);
   
   m_UserName->SetLabel(s_User);
   m_Password->SetLabel(s_Password);

 //   (void)new wxStaticText(this, -1, Temp, wxPoint(10, 10), wxSize(300, 20),wxALIGN_CENTRE );
//    tc_User     = new wxTextCtrl(this, ID_USER, s_User, wxPoint(75, 35), wxSize(200, 25), 0, wxDefaultValidator);
//    tc_Password = new wxTextCtrl(this, ID_PASSWORD, s_Password, wxPoint(75, 75), wxSize(200, 25),wxTE_PASSWORD, wxDefaultValidator);
//    tc_User->SetFocus();
 }
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DlgUser, wxDialog)
  EVT_BUTTON(wxID_OK, DlgUser::OnOk)
  EVT_BUTTON(wxID_CANCEL, DlgUser::OnCancel)
END_EVENT_TABLE()
  
  
  //---------------------------------------------------------------------------
void DlgUser::OnOk(wxCommandEvent& WXUNUSED(event) )
{
  //canceled = FALSE;
  s_User    = m_UserName->GetValue();
  s_Password = m_Password->GetValue();
  EndModal(wxID_OK);
}
//---------------------------------------------------------------------------
//void DlgUser::OnCancel(wxCommandEvent& WXUNUSED(event) )
// {
//   canceled = TRUE;
//   EndModal(wxID_CANCEL);
// }
//---------------------------------------------------------------------------

