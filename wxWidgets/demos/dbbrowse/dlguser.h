//----------------------------------------------------------------------------------------
// Name:        DlgUser.h,cpp
// Purpose:     Dialog mit Variable Gestaltung durch DlgUser.wxr
// Author:      Mark Johnson
// Modified by: 19991105.mj10777
// Created:     19991105
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
// DlgUser
//----------------------------------------------------------------------------------------
#define ID_DIALOG_DSN   100
#define ID_DSN          101
#define ID_USER         102
#define ID_PASSWORD     103
#define ID_TEXT         104

//----------------------------------------------------------------------------------------
class MainDoc;

//----------------------------------------------------------------------------------------
class DlgUser: public wxDialog
{
public:
    wxString s_DSN, s_User, s_Password;
    wxStaticText *m_Label1, *m_Label2;
    wxTextCtrl *m_UserName, *m_Password;
    wxButton *m_OK, *m_Cancel;
    MainDoc *pDoc;
    //---------------------------------------------------------------------------------------
    DlgUser(wxWindow *parent,MainDoc *pDoc, const wxString& title);
    void OnInit();
    void OnOk(wxCommandEvent& event);
    //---------------------------------------------------------------------------------------

    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------------------
