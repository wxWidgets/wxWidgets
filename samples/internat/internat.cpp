/////////////////////////////////////////////////////////////////////////////
// Name:        internat.cpp
// Purpose:     Demonstrates internationalisation (i18n) support
// Author:      Vadim Zeitlin/Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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
#include "wx/file.h"
#include "wx/log.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif

// Define a new application type
class MyApp: public wxApp
{
public:
  virtual bool OnInit();

protected:
  wxLocale m_locale; // locale we'll be using
};

// Define a new frame type
class MyFrame: public wxFrame
{ 
public:
  MyFrame(wxFrame *frame, const char *title, int x, int y, int w, int h);

public:
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnPlay(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

// ID for the menu commands
enum
{
  MINIMAL_QUIT = 1,
  MINIMAL_TEXT,
  MINIMAL_ABOUT,
  MINIMAL_TEST,
  MINIMAL_OPEN
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(MINIMAL_QUIT, MyFrame::OnQuit)
  EVT_MENU(MINIMAL_ABOUT, MyFrame::OnAbout)
  EVT_MENU(MINIMAL_TEST, MyFrame::OnPlay)
  EVT_MENU(MINIMAL_OPEN, MyFrame::OnOpen)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)


// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
  // set the language to use
  const char *language = NULL;
  const char *langid = NULL;
  switch ( argc )
  {
      default:
          // ignore the other args, fall through

      case 3:
          language = argv[1];
          langid = argv[2];
          break;

      case 2:
          language = argv[1];
          break;

      case 1:
          language = "french";
          langid = "fr";
  };

  // there are very few systems right now which support locales other than "C"
  m_locale.Init(language, langid, "C");

  // Initialize the catalogs we'll be using
  /* not needed any more, done in wxLocale ctor
  m_locale.AddCatalog("wxstd");      // 1) for library messages
  */
  m_locale.AddCatalog("internat");      // 2) our private one
  /* this catalog is installed in standard location on Linux systems,
     it might not be installed on yours - just ignore the errrors
     or comment out this line then */
#ifdef __LINUX__
  m_locale.AddCatalog("fileutils");  // 3) and another just for testing
#endif
  
  // Create the main frame window
  MyFrame *frame = new MyFrame((wxFrame *) NULL, _("International wxWindows App"),
                               50, 50, 350, 60);

  // Give it an icon
  frame->SetIcon(wxICON(mondrian));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MINIMAL_ABOUT, _("&About..."));
  file_menu->AppendSeparator();
  file_menu->Append(MINIMAL_QUIT, _("E&xit"));

  wxMenu *test_menu = new wxMenu;
  test_menu->Append(MINIMAL_OPEN, _("&Open bogus file"));
  test_menu->Append(MINIMAL_TEST, _("&Play a game"));

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, _("&File"));
  menu_bar->Append(test_menu, _("&Test"));
  frame->SetMenuBar(menu_bar);

  // Show the frame
  frame->Show(TRUE);
  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const char *title, int x, int y, int w, int h)
       : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageDialog(this, _("I18n sample\n"
                          "© 1998, 1999 Vadim Zeitlin and Julian Smart"),
                  _("About Internat"), wxOK | wxICON_INFORMATION).ShowModal();
}

void MyFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
  wxString str = wxGetTextFromUser(_("Enter your number:"),
                                   _("Try to guess my number!"),
                                  "", this);
  if ( str.IsEmpty() )
      return;

  int num;
  sscanf(str, "%d", &num);
  if ( num == 0 )
    str = _("You've probably entered an invalid number.");
  else if ( num == 9 )  // this message is not translated (not in catalog)
    str = "You've found a bug in this program!";
  else if ( num != 17 ) // a more implicit way to write _()
    str = wxGetTranslation("Bad luck! try again...");
  else {
    str.Empty();
    // string must be split in two -- otherwise the translation won't be found
    str << _("Congratulations! you've won. Here is the magic phrase:")
        << _("cannot create fifo `%s'");
  }

  wxMessageBox(str, _("Result"), wxOK | wxICON_INFORMATION);
}

void MyFrame::OnOpen(wxCommandEvent&)
{
  // open a bogus file -- the error message should be also translated if you've
  // got wxstd.mo somewhere in the search path
  wxFile file("NOTEXIST.ING");
}
