/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.cpp
// Purpose:     wxTaskBarIcon demo
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/taskbar.h"
#include "tbtest.h"

// Declare two frames
MyDialog   *dialog = NULL;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
    wxIcon icon(wxT("mondrian_icon"));

    if (!m_taskBarIcon.SetIcon(icon, wxT("wxTaskBarIcon Sample")))
        wxMessageBox(wxT("Could not set icon."));

    // Create the main frame window
    dialog = new MyDialog(NULL, -1, wxT("wxTaskBarIcon Test Dialog"), wxPoint(-1, -1), wxSize(365, 290), wxDIALOG_MODELESS|wxDEFAULT_DIALOG_STYLE);

    dialog->Show(TRUE);

    return TRUE;
}


BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
    EVT_CLOSE(MyDialog::OnCloseWindow)
END_EVENT_TABLE()



MyDialog::MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxDialog(parent, id, title, pos, size, windowStyle)
{
    Init();
}

void MyDialog::OnOK(wxCommandEvent& event)
{
    Show(FALSE);
}

void MyDialog::OnExit(wxCommandEvent& event)
{
    Close(TRUE);
}

void MyDialog::OnCloseWindow(wxCloseEvent& event)
{
    Destroy();
}

void MyDialog::Init(void)
{
  (void)new wxStaticText(this, -1, _T("Press OK to hide me, Exit to quit."),
                         wxPoint(10, 20));

  (void)new wxStaticText(this, -1, _T("Double-click on the taskbar icon to show me again."),
                         wxPoint(10, 40));

  (void)new wxButton(this, wxID_EXIT, _T("Exit"), wxPoint(185, 230), wxSize(80, 25));
  (new wxButton(this, wxID_OK, _T("OK"), wxPoint(100, 230), wxSize(80, 25)))->SetDefault();
  Centre(wxBOTH);
}


enum {
    PU_RESTORE = 10001,
    PU_NEW_ICON,
    PU_EXIT,
};


BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_NEW_ICON,MyTaskBarIcon::OnMenuSetNewIcon)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    dialog->Show(TRUE);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    dialog->Close(TRUE);

    // Nudge wxWindows into destroying the dialog, since
    // with a hidden window no messages will get sent to put
    // it into idle processing.
    wxGetApp().ProcessIdle();
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
#ifdef __WXMSW__
    wxIcon icon(wxT("wxDEFAULT_FRAME"));

    if (!SetIcon(icon, wxT("wxTaskBarIcon Sample")))
        wxMessageBox(wxT("Could not set new icon."));
#endif
}

// Overridables
void MyTaskBarIcon::OnMouseMove(wxEvent&)
{
}

void MyTaskBarIcon::OnLButtonDown(wxEvent&)
{
}

void MyTaskBarIcon::OnLButtonUp(wxEvent&)
{
}

void MyTaskBarIcon::OnRButtonDown(wxEvent&)
{
}

void MyTaskBarIcon::OnRButtonUp(wxEvent&)
{
    wxMenu      menu;

    menu.Append(PU_RESTORE, _T("&Restore TBTest"));
#ifdef __WXMSW__
    menu.Append(PU_NEW_ICON,_T("&Set New Icon"));
#endif
    menu.Append(PU_EXIT,    _T("E&xit"));

    PopupMenu(&menu);
}

void MyTaskBarIcon::OnLButtonDClick(wxEvent&)
{
    dialog->Show(TRUE);
}

void MyTaskBarIcon::OnRButtonDClick(wxEvent&)
{
}




