/////////////////////////////////////////////////////////////////////////////
// Name:        validate.cpp
// Purpose:     wxWindows validation sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/valtext.h"

#include "validate.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
	EVT_MENU(VALIDATE_TEST_DIALOG, MyFrame::OnTestDialog)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

MyData	g_data;

bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Validation Test", 50, 50, 300, 250);

  // Give it an icon
#ifdef __WINDOWS__
  frame->SetIcon(wxIcon("mondrian"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(VALIDATE_TEST_DIALOG, "&Test dialog");
  file_menu->Append(wxID_EXIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  frame->CreateStatusBar(1);

  // Show the frame
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;  
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{}

void MyFrame::OnQuit(wxCommandEvent& event)
{
  Close(TRUE);
}

void MyFrame::OnTestDialog(wxCommandEvent& event)
{
	MyDialog dialog(this, "Validation test dialog", wxPoint(100, 100), wxSize(340, 200));

	dialog.ShowModal();
}

MyDialog::MyDialog(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
	const long style):
	wxDialog(parent, VALIDATE_DIALOG_ID, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
  wxButton *but1 = new wxButton(this, wxID_OK, "OK", wxPoint(250, 10), wxSize(80, 30));
  wxButton *but2 = new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(250, 60), wxSize(80, 30));

  wxTextCtrl *txt1 = new wxTextCtrl(this, VALIDATE_TEXT, "",
    wxPoint(10, 10), wxSize(100, -1), 0, wxTextValidator(wxFILTER_ALPHA, &g_data.m_string));

//  SetBackgroundColour(wxColour(0,0,255));

  but1->SetFocus();
  but1->SetDefault();
}

