/////////////////////////////////////////////////////////////////////////////
// Name:        demo.cpp
// Purpose:     wxHelpController demo
// Author:      Karsten Ballueder
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballueder, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

// define this to 1 to use HTML help even under Windows (by default, Windows
// version will HLP-based help)
#define USE_HTML_HELP 1
#if USE_HTML_HELP
#   include "wx/helpbase.h"
#else
#   include "wx/help.h"
#endif

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    wxHelpController& GetHelpController() { return m_help; }

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);

private:
   wxHelpController m_help;

    // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    HelpDemo_Quit = 1,
    HelpDemo_Help_Index,
    HelpDemo_Help_Classes,
    HelpDemo_Help_Functions,
    HelpDemo_Help_Help,
    HelpDemo_Help_KDE,
    HelpDemo_Help_GNOME,
    HelpDemo_Help_Netscape,
    HelpDemo_Help_Search,
    // controls start here (the numbers are, of course, arbitrary)
    HelpDemo_Text = 1000,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(HelpDemo_Quit,  MyFrame::OnQuit)
    EVT_MENU(HelpDemo_Help_Index, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Classes, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Functions, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Help, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_KDE, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_GNOME, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Netscape, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Search, MyFrame::OnHelp)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame("HelpDemo wxWindows App",
                                 wxPoint(50, 50), wxSize(450, 340));

    frame->Show(TRUE);
    SetTopWindow(frame);


    // initialise the help system: this means that we'll use doc.hlp file under
    // Windows and that the HTML docs are in the subdirectory doc for platforms
    // using HTML help
    if ( !frame->GetHelpController().Initialize("doc") )
    {
        wxLogError("Cannot initialize the help system, aborting.");

        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(HelpDemo_Help_Index, "&Help Index...");
    menuFile->Append(HelpDemo_Help_Classes, "&Help on Classes...");
    menuFile->Append(HelpDemo_Help_Functions, "&Help on Functions...");
    menuFile->Append(HelpDemo_Help_Help, "&About Help Demo...");
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Help_Search, "&Search help...");
#ifndef __WXMSW__
#if !wxUSE_HTML
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Help_KDE, "Use &KDE");
    menuFile->Append(HelpDemo_Help_GNOME, "Use &GNOME");
    menuFile->Append(HelpDemo_Help_Netscape, "Use &Netscape");
#endif
#endif
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Quit, "E&xit");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText("Welcome to wxWindows!");

    // now create some controls

    // a panel first - if there were several controls, it would allow us to
    // navigate between them from the keyboard
    wxPanel *panel = new wxPanel(this, -1, wxPoint(0, 0), wxSize(400, 200));

    // and a static control whose parent is the panel
    (void)new wxStaticText(panel, -1, "Hello, world!", wxPoint(10, 10));
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnHelp(wxCommandEvent& event)
{
   switch(event.GetId())
   {

   // Note: these DisplaySection calls use ids that are specific to wxExtHelpController
   // (on Unix). For WinHelp, we'd need to use different context ids.

   case HelpDemo_Help_Classes:
      m_help.DisplaySection(1);
      break;
   case HelpDemo_Help_Functions:
      m_help.DisplaySection(2);
      break;
   case HelpDemo_Help_Help:
      m_help.DisplaySection(3);
      break;

   // These three calls are only used by wxExtHelpController

   case HelpDemo_Help_KDE:
      m_help.SetViewer("kdehelp");
      break;
   case HelpDemo_Help_GNOME:
      m_help.SetViewer("gnome-help-browser");
      break;
   case HelpDemo_Help_Netscape:
      m_help.SetViewer("netscape", wxHELP_NETSCAPE);
      break;

   case HelpDemo_Help_Search:
   {
      wxString key = wxGetTextFromUser("Search for?",
                                       "Search help for keyword",
                                       "",
                                       this);
      if(! key.IsEmpty())
         m_help.KeywordSearch(key);
   }
   break;
   case HelpDemo_Help_Index:
   default:
      m_help.DisplayContents();
      break;
   }
}

