/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.cpp
// Purpose:     wxTaskBarIcon demo
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx/msw/taskbar.h"
#include "tbtest.h"

// Declare two frames
MyDialog   *dialog = NULL;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
    wxIcon icon("mondrian_icon");

    if (!m_taskBarIcon.SetIcon(icon, "wxTaskBarIcon Sample"))
        wxMessageBox("Could not set icon.");

    // Create the main frame window
    dialog = new MyDialog(NULL, -1, "wxTaskBarIcon Test Dialog", wxPoint(-1, -1), wxSize(365, 290), wxDIALOG_MODELESS|wxDEFAULT_DIALOG_STYLE);

    dialog->Show(TRUE);

    return TRUE;
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
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
  int dialogWidth = 365;
  int dialogHeight = 290;

  wxStaticText* stat = new wxStaticText(this, -1, "Press OK to hide me, Exit to quit.",
    wxPoint(10, 20));
  
  wxStaticText* stat2 = new wxStaticText(this, -1, "Double-click on the taskbar icon to show me again.",
    wxPoint(10, 40));

  wxButton *okButton = new wxButton(this, wxID_OK, "OK", wxPoint(100, 230), wxSize(80, 25));
  wxButton *exitButton = new wxButton(this, wxID_EXIT, "Exit", wxPoint(185, 230), wxSize(80, 25));
  okButton->SetDefault();
  this->Centre(wxBOTH);
}

// Overridables
void MyTaskBarIcon::OnMouseMove(void)
{
}

void MyTaskBarIcon::OnLButtonDown(void)
{
}

void MyTaskBarIcon::OnLButtonUp(void)
{
}

void MyTaskBarIcon::OnRButtonDown(void)
{
}

void MyTaskBarIcon::OnRButtonUp(void)
{
}

void MyTaskBarIcon::OnLButtonDClick(void)
{
    dialog->Show(TRUE);
}

void MyTaskBarIcon::OnRButtonDClick(void)
{
}


