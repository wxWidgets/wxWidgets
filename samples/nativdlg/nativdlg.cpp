/////////////////////////////////////////////////////////////////////////////
// Name:        nativdlg.cpp
// Purpose:     Native Windows dialog sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/resource.h"

#include <ctype.h>
#include "nativdlg.h"
#include "resource.h"

// Declare two frames
MyFrame   *frame = NULL;

IMPLEMENT_APP(MyApp)

// Testing of ressources
MyApp::MyApp()
{
}

bool MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, -1, "wxWindows Native Dialog Sample", wxPoint(0, 0), wxSize(300, 250));

  // Give it a status line
  frame->CreateStatusBar(2);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(RESOURCE_TEST1, "&Dialog box test",                "Test dialog box resource");
  file_menu->Append(RESOURCE_QUIT, "E&xit",                "Quit program");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxWindow(frame, -1, wxPoint(0, 0), wxSize(400, 400), 0, "MyMainFrame");
  frame->Show(TRUE);

  // Return the main frame window
  SetTopWindow(frame);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(RESOURCE_QUIT, MyFrame::OnQuit)
	EVT_MENU(RESOURCE_TEST1, MyFrame::OnTest1)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(parent, id, title, pos, size)
{
  panel = NULL;
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
	Close(TRUE);
}

void MyFrame::OnTest1(wxCommandEvent& event)
{
      MyDialog *dialog = new MyDialog;
      if (dialog->LoadNativeDialog(this, "dialog1"))
      {
/*
        wxTextCtrl *text = (wxTextCtrl *)wxFindWindowByName("multitext3", dialog);
        if (text)
          text->SetValue("wxWindows resource demo");
*/
        dialog->SetModal(TRUE);
        dialog->ShowModal();
      }
      dialog->Close(TRUE);
}

bool MyFrame::OnClose(void)
{
  Show(FALSE);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_BUTTON(wxID_OK, MyDialog::OnOk)
	EVT_BUTTON(wxID_CANCEL, MyDialog::OnCancel)
END_EVENT_TABLE()


void MyDialog::OnOk(wxCommandEvent& event)
{
  EndModal(wxID_OK);
}

void MyDialog::OnCancel(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);
}


