/////////////////////////////////////////////////////////////////////////////
// Name:        resource.cpp
// Purpose:     Dialog resource sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
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
#include "resource.h"

// If we wanted to demonstrate total platform independence,
// then we'd use the dynamic file loading form for all platforms.
// But this shows how to embed the wxWindows resources
// in the program code/executable for UNIX and Windows
// platforms.

// In order to get the ID of the controls defined in the
// dialog, we include the header automatically generated
// by DialogEd

#include "dialog1.h"

// If you have a Windows compiler that can cope with long strings,
// then you can always use the #include form for simplicity.

// NOTE: Borland's brc32.exe resource compiler doesn't recognize
// the TEXT resource, for some reason, so either run-time file loading
// or file inclusion should be used.

#if defined(__WXMSW__)
// Under Windows, some compilers can't include
// a whole .wxr file. So we use a .rc user-defined resource
// instead. dialog1 will point to the whole .wxr 'file'.
static char *dialog1 = NULL;
static char *menu1 = NULL;
#else
// Other platforms should have sensible compilers that
// cope with long strings.
#include "dialog1.wxr"
#include "menu1.wxr"
#endif

// Declare two frames
MyFrame   *frame = (MyFrame *) NULL;

IMPLEMENT_APP(MyApp)

// Testing of ressources
MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
#if defined(__WXMSW__)
  // Load the .wxr 'file' from a .rc resource, under Windows.
  dialog1 = wxLoadUserResource("dialog1", "WXRDATA");
  menu1 = wxLoadUserResource("menu1", "WXRDATA");
  // All resources in the file (only one in this case) get parsed
  // by this call.
  wxResourceParseString(dialog1);
  wxResourceParseString(menu1);
#else
  // Simply parse the data pointed to by the variable dialog1.
  // If there were several resources, there would be several
  // variables, and this would need to be called several times.
  wxResourceParseData(dialog1);
  wxResourceParseData(menu1);
#endif

  // Create the main frame window
  frame = new MyFrame((wxFrame *) NULL, -1, (char *) "wxWindows Resource Sample", wxPoint(-1, -1), wxSize(300, 250));

  // Give it a status line
  frame->CreateStatusBar(2);

  wxMenuBar *menu_bar = wxResourceCreateMenuBar("menu1");
  
  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new MyPanel(frame, -1, wxPoint(0, 0), wxSize(400, 400), 0, "MyMainFrame");
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

MyApp::~MyApp()
{
    delete dialog1;
    delete menu1;
}

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
	EVT_LEFT_DOWN( MyPanel::OnClick)
END_EVENT_TABLE()

MyPanel::MyPanel( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
  int style, const wxString &name ) : 
  wxPanel( parent, id, pos, size, style, name )
{
}

void MyPanel::OnClick( wxMouseEvent &WXUNUSED(event2) )
{
  MyFrame *frame = (MyFrame*)(wxTheApp->GetTopWindow());
  wxCommandEvent event;
  frame->OnTestDialog( event );
}


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(RESOURCE_ABOUT, MyFrame::OnAbout)
	EVT_MENU(RESOURCE_QUIT, MyFrame::OnQuit)
	EVT_MENU(RESOURCE_TESTDIALOG, MyFrame::OnTestDialog)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(parent, id, title, pos, size)
{
  panel = (wxWindow *) NULL;
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox("wxWindows resource sample.\n"
                 "(c) Julian Smart", "About wxWindows sample",
                 wxICON_INFORMATION | wxOK);
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
	Close(TRUE);
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event) )
{
      MyDialog *dialog = new MyDialog;
      if (dialog->LoadFromResource(this, "dialog1"))
      {
        wxTextCtrl *text = (wxTextCtrl *)wxFindWindowByName("multitext3", dialog);
        if (text)
          text->SetValue("wxWindows resource demo");
        dialog->ShowModal();
      }
      dialog->Close(TRUE);
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
  //	EVT_BUTTON(RESOURCE_OK, MyDialog::OnOk)
	EVT_BUTTON(ID_BUTTON109, MyDialog::OnCancel)
END_EVENT_TABLE()


void MyDialog::OnOk(wxCommandEvent& WXUNUSED(event) )
{
  //  EndModal(RESOURCE_OK);
}

void MyDialog::OnCancel(wxCommandEvent& WXUNUSED(event) )
{
  EndModal(ID_BUTTON109);
}


