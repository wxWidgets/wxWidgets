/////////////////////////////////////////////////////////////////////////////
// Name:        internat.cpp
// Purpose:     Demonstrates internationalisation support
// Author:      Vadim Zeitlin/Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/intl.h"
#include "wx/log.h"

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp();
    bool OnInit(void);
protected:
	wxLocale		m_locale;			  // locale we'll be using
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    
 public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	bool OnClose(void) { return TRUE; }

   DECLARE_EVENT_TABLE()
    
};

// ID for the menu commands
#define MINIMAL_QUIT 	1
#define MINIMAL_TEXT 	101
#define MINIMAL_ABOUT 	102

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(MINIMAL_QUIT, MyFrame::OnQuit)
	EVT_MENU(MINIMAL_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)


MyApp::MyApp():
  m_locale("french", "fr", "C")
{
  // catalogs we'll be using:
  /* not needed any more, done in wxLocale ctor
  m_locale.AddCatalog("wxstd");      // 1) for library messages
  */
  m_locale.AddCatalog("internat");      // 2) our private one
  /* this catalog is installed in standard location on Liux systems,
     it might not be installed on yours - just ignore the errrors
     or comment out this line then */
  m_locale.AddCatalog("fileutils");  // 3) and another just for testing
  
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Minimal wxWindows App", 50, 50, 450, 340);

  // Give it an icon
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("mondrian"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MINIMAL_ABOUT, "&About");
  file_menu->Append(MINIMAL_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame, -1, wxPoint(0, 0), wxSize(400, 200), wxTAB_TRAVERSAL);

  wxString msgString;
  int num = 2;
  if ( num == 0 )
    msgString = wxTString("you've probably entered an invalid number.");
  else if ( num == 9 ) // this message is not translated
    msgString = wxTString("You've found a bug in this program!");
  else if ( num != 17 )
    msgString = wxTString("bad luck! try again...");
  else {
    msgString = wxTString("congratulations! you've won. Here is the magic phrase:");
  }

  wxStaticText *msg = new wxStaticText(panel, 311, msgString, wxPoint(10, 10), wxSize(-1, -1),
    0);

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

void MyFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageDialog dialog(this, "This is a minimal sample\nA second line in the message box",
  	"About Minimal", wxYES_NO|wxCANCEL);

  dialog.ShowModal();
}


