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

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "../sample.xpm"
#endif

#include "smile.xpm"

#include "wx/taskbar.h"
#include "tbtest.h"

// Declare two frames
MyDialog   *dialog = NULL;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
    // Create the main frame window
    dialog = new MyDialog(NULL, wxID_ANY, wxT("wxTaskBarIcon Test Dialog"), wxDefaultPosition, wxSize(365, 290));

    dialog->Show(true);

    return true;
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

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void MyDialog::Init(void)
{
  (void)new wxStaticText(this, wxID_ANY, _T("Press 'Hide me' to hide me, Exit to quit."),
                         wxPoint(10, 20));

  (void)new wxStaticText(this, wxID_ANY, _T("Double-click on the taskbar icon to show me again."),
                         wxPoint(10, 40));

  (void)new wxButton(this, wxID_EXIT, _T("Exit"), wxPoint(185, 230), wxSize(80, 25));
  (new wxButton(this, wxID_OK, _T("Hide me"), wxPoint(100, 230), wxSize(80, 25)))->SetDefault();
  Centre(wxBOTH);
   
  m_taskBarIcon = new MyTaskBarIcon();
  if (!m_taskBarIcon->SetIcon(wxICON(sample), wxT("wxTaskBarIcon Sample")))
        wxMessageBox(wxT("Could not set icon."));
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
    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    dialog->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    dialog->Close(true);
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
    wxIcon icon(smile_xpm);

    if (!SetIcon(icon, wxT("wxTaskBarIcon Sample - a different icon")))
        wxMessageBox(wxT("Could not set new icon."));
}

// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    
    menu->Append(PU_RESTORE, _T("&Restore TBTest"));
    menu->Append(PU_NEW_ICON,_T("&Set New Icon"));
    menu->Append(PU_EXIT,    _T("E&xit"));

    return menu;
}

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    dialog->Show(true);
}
