//---------------------------------------------------------------------------
// Name:        DlgUser.h,cpp
// Purpose:     Dialog mit Variable Gestaltung durch DlgUser.wxr
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by: 19991105.mj10777
// Created:     19991105
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
//---------------------------------------------------------------------------
// DlgUser
//---------------------------------------------------------------------------
#define ID_DIALOG_DSN   100
#define ID_DSN          101
#define ID_USER         102
#define ID_PASSWORD     103
#define ID_TEXT         104
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class DlgUser: public wxDialog
{
 public:
 bool canceled;
 wxString s_DSN, s_User, s_Password;
 wxTextCtrl *tc_User, *tc_Password;
 //---------------------------------------------------------------------------
 DlgUser(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,const long style = wxDEFAULT_DIALOG_STYLE);
 void OnInit();
 void OnOk(wxCommandEvent& event);
 void OnCancel(wxCommandEvent& event);
 //---------------------------------------------------------------------------
  DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------
