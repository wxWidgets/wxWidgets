/////////////////////////////////////////////////////////////////////////////
// Name:        samples/nativdlg/nativdlg.cpp
// Purpose:     Native Windows dialog sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef __WXMSW__
#error Sorry, this sample is only appropriate under Windows.
#endif

#include <ctype.h>
#include "nativdlg.h"
#include "resource.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame   *frame = new MyFrame(NULL, wxID_ANY, _T("wxWidgets Native Dialog Sample"), wxPoint(0, 0), wxSize(300, 250));

#if wxUSE_STATUSBAR
  // Give it a status line
  frame->CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(RESOURCE_TEST1, _T("&Dialog box test"),                _T("Test dialog box resource"));
  file_menu->Append(RESOURCE_QUIT, _T("E&xit"),                _T("Quit program"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxWindow(frame, wxID_ANY, wxPoint(0, 0), wxSize(400, 400), 0, _T("MyMainFrame"));
  frame->Show(true);

  // Return the main frame window
  SetTopWindow(frame);

  return true;
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

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
#if ( defined(__WXPM__) || defined(__WXMSW__) ) && !defined(__WXUNIVERSAL__)
    MyDialog dialog;
    if (dialog.LoadNativeDialog(this, _T("dialog1")))
    {
        dialog.ShowModal();
    }
#else
    wxMessageBox(_T("No native dialog support"),_T("Platform limitation"));
#endif
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, MyDialog::OnCancel)
END_EVENT_TABLE()


void MyDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_OK);
}

void MyDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}
