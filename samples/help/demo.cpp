/////////////////////////////////////////////////////////////////////////////
// Name:        demo.cpp
// Purpose:     wxHelpController demo
// Author:      Karsten Ballueder
// Modified by:
// Created:     04/01/98
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

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#   include "wx/image.h"
#   include "wx/help.h"
#   include "wx/cshelp.h"

#if wxUSE_TOOLTIPS
#   include "wx/tooltip.h"
#endif

// define this to 1 to use HTML help even under Windows (by default, Windows
// version will use WinHelp).
// Please also see samples/html/helpview.
#define USE_HTML_HELP 1

// define this to 1 to use external help controller (not used by default)
#define USE_EXT_HELP 0

// Define this to 0 to use the help controller as the help
// provider, or to 1 to use the 'simple help provider'
// (the one implemented with wxTipWindow).
#define USE_SIMPLE_HELP_PROVIDER 0

#if !wxUSE_HTML
#undef USE_HTML_HELP
#define USE_HTML_HELP 0
#endif

#if USE_HTML_HELP
#include "wx/filesys.h"
#include "wx/fs_zip.h"

#include "wx/html/helpctrl.h"
#endif

#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
#include "wx/msw/helpchm.h"
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
#include "wx/msw/helpbest.h"
#endif

#if USE_EXT_HELP
#include "wx/generic/helpext.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
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
    virtual bool OnInit() wxOVERRIDE;

    // do some clean up here
    virtual int OnExit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    wxHelpControllerBase& GetHelpController() { return m_help; }

#if USE_HTML_HELP
    wxHtmlHelpController& GetAdvancedHtmlHelpController() { return m_advancedHtmlHelp; }
#endif
#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    wxCHMHelpController& GetMSHtmlHelpController() { return m_msHtmlHelp; }
#endif
#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
    wxBestHelpController& GetBestHelpController() { return m_bestHelp; }
#endif

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnAdvancedHtmlHelp(wxCommandEvent& event);
#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    void OnMSHtmlHelp(wxCommandEvent& event);
#endif
#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
    void OnBestHelp(wxCommandEvent& event);
#endif
#if USE_HTML_HELP
    void OnModalHtmlHelp(wxCommandEvent& event);
#endif

    void OnShowContextHelp(wxCommandEvent& event);
    void OnShowDialogContextHelp(wxCommandEvent& event);

    void ShowHelp(int commandId, wxHelpControllerBase& helpController);

private:
#if USE_EXT_HELP
   wxExtHelpController      m_help;
#else
   wxHelpController         m_help;
#endif

#if USE_HTML_HELP
   wxHtmlHelpController     m_advancedHtmlHelp;
   wxHtmlHelpController     m_embeddedHtmlHelp;
   wxHtmlHelpWindow*        m_embeddedHelpWindow;
#endif

#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    wxCHMHelpController     m_msHtmlHelp;
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
    wxBestHelpController    m_bestHelp;
#endif

    // any class wishing to process wxWidgets events must use this macro
   wxDECLARE_EVENT_TABLE();
};

// A custom modal dialog
class MyModalDialog : public wxDialog
{
public:
    MyModalDialog(wxWindow *parent);

private:

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    HelpDemo_Quit = 100,
    HelpDemo_Help_Index,
    HelpDemo_Help_Classes,
    HelpDemo_Help_Functions,
    HelpDemo_Help_Help,
    HelpDemo_Help_Search,
    HelpDemo_Help_ContextHelp,
    HelpDemo_Help_DialogContextHelp,

    HelpDemo_Html_Help_Index,
    HelpDemo_Html_Help_Classes,
    HelpDemo_Html_Help_Functions,
    HelpDemo_Html_Help_Help,
    HelpDemo_Html_Help_Search,

    HelpDemo_Advanced_Html_Help_Index,
    HelpDemo_Advanced_Html_Help_Classes,
    HelpDemo_Advanced_Html_Help_Functions,
    HelpDemo_Advanced_Html_Help_Help,
    HelpDemo_Advanced_Html_Help_Search,
    HelpDemo_Advanced_Html_Help_Modal,

    HelpDemo_MS_Html_Help_Index,
    HelpDemo_MS_Html_Help_Classes,
    HelpDemo_MS_Html_Help_Functions,
    HelpDemo_MS_Html_Help_Help,
    HelpDemo_MS_Html_Help_Search,

    HelpDemo_Best_Help_Index,
    HelpDemo_Best_Help_Classes,
    HelpDemo_Best_Help_Functions,
    HelpDemo_Best_Help_Help,
    HelpDemo_Best_Help_Search,

    HelpDemo_Help_KDE,
    HelpDemo_Help_GNOME,
    HelpDemo_Help_Netscape,
    // controls start here (the numbers are, of course, arbitrary)
    HelpDemo_Text = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(HelpDemo_Quit,  MyFrame::OnQuit)
    EVT_MENU(HelpDemo_Help_Index, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Classes, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Functions, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Help, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Search, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_ContextHelp, MyFrame::OnShowContextHelp)
    EVT_MENU(HelpDemo_Help_DialogContextHelp, MyFrame::OnShowDialogContextHelp)

    EVT_MENU(HelpDemo_Advanced_Html_Help_Index, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Classes, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Functions, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Help, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Search, MyFrame::OnAdvancedHtmlHelp)
#if USE_HTML_HELP
    EVT_MENU(HelpDemo_Advanced_Html_Help_Modal, MyFrame::OnModalHtmlHelp)
#endif

#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    EVT_MENU(HelpDemo_MS_Html_Help_Index, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Classes, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Functions, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Help, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Search, MyFrame::OnMSHtmlHelp)
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
    EVT_MENU(HelpDemo_Best_Help_Index, MyFrame::OnBestHelp)
#endif

    EVT_MENU(HelpDemo_Help_KDE, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_GNOME, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Netscape, MyFrame::OnHelp)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create a simple help provider to make SetHelpText() do something.
    // Note that this must be set before any SetHelpText() calls are made.
#if USE_SIMPLE_HELP_PROVIDER
    wxSimpleHelpProvider* provider = new wxSimpleHelpProvider;
#else
    wxHelpControllerHelpProvider* provider = new wxHelpControllerHelpProvider;
#endif
    wxHelpProvider::Set(provider);

#if USE_HTML_HELP
    #if wxUSE_GIF
        // Required for images in the online documentation
        wxImage::AddHandler(new wxGIFHandler);
    #endif // wxUSE_GIF

    // Required for advanced HTML help
    #if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB
        wxFileSystem::AddHandler(new wxZipFSHandler);
    #endif
#endif // wxUSE_HTML

    // Create the main application window
    MyFrame *frame = new MyFrame("HelpDemo wxWidgets App",
                                 wxPoint(50, 50), wxSize(450, 340));

#if !USE_SIMPLE_HELP_PROVIDER
#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    provider->SetHelpController(& frame->GetMSHtmlHelpController());
#else
    provider->SetHelpController(& frame->GetHelpController());
#endif
#endif // !USE_SIMPLE_HELP_PROVIDER

    frame->Show(true);

    // initialise the help system: this means that we'll use doc.hlp file under
    // Windows and that the HTML docs are in the subdirectory doc for platforms
    // using HTML help
    if ( !frame->GetHelpController().Initialize("doc") )
    {
        wxLogError("Cannot initialize the help system, aborting.");

        return false;
    }

#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    if( !frame->GetMSHtmlHelpController().Initialize("doc") )
    {
        wxLogError("Cannot initialize the MS HTML Help system.");
    }
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
    // you need to call Initialize in order to use wxBestHelpController
    if( !frame->GetBestHelpController().Initialize("doc") )
    {
        wxLogError("Cannot initialize the best help system, aborting.");
    }
#endif

#if USE_HTML_HELP
    // initialise the advanced HTML help system: this means that the HTML docs are in .htb
    // (zipped) form
    if ( !frame->GetAdvancedHtmlHelpController().Initialize("doc") )
    {
        wxLogError("Cannot initialize the advanced HTML help system, aborting.");

        return false;
    }
#endif

#if 0
    // defined(__WXMSW__) && wxUSE_MS_HTML_HELP
    wxString path(wxGetCwd());
    if ( !frame->GetMSHtmlHelpController().Initialize(path + "\\doc.chm") )
    {
        wxLogError("Cannot initialize the MS HTML help system, aborting.");

        return false;
    }
#endif

    return true;
}

int MyApp::OnExit()
{
    // clean up
    delete wxHelpProvider::Set(NULL);

    return 0;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, 300, title, pos, size)
#if USE_HTML_HELP
    , m_embeddedHtmlHelp(wxHF_EMBEDDED|wxHF_DEFAULT_STYLE)
#endif
{
    // set the frame icon
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(HelpDemo_Help_Index, "&Help Index...");
    menuFile->Append(HelpDemo_Help_Classes, "&Help on Classes...");
    menuFile->Append(HelpDemo_Help_Functions, "&Help on Functions...");
    menuFile->Append(HelpDemo_Help_ContextHelp, "&Context Help...");
    menuFile->Append(HelpDemo_Help_DialogContextHelp, "&Dialog Context Help...\tCtrl-H");
    menuFile->Append(HelpDemo_Help_Help, "&About Help Demo...");
    menuFile->Append(HelpDemo_Help_Search, "&Search help...");
#if USE_HTML_HELP
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Advanced_Html_Help_Index, "Advanced HTML &Help Index...");
    menuFile->Append(HelpDemo_Advanced_Html_Help_Classes, "Advanced HTML &Help on Classes...");
    menuFile->Append(HelpDemo_Advanced_Html_Help_Functions, "Advanced HTML &Help on Functions...");
    menuFile->Append(HelpDemo_Advanced_Html_Help_Help, "Advanced HTML &About Help Demo...");
    menuFile->Append(HelpDemo_Advanced_Html_Help_Search, "Advanced HTML &Search help...");
    menuFile->Append(HelpDemo_Advanced_Html_Help_Modal, "Advanced HTML Help &Modal Dialog...");
#endif

#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_MS_Html_Help_Index, "MS HTML &Help Index...");
    menuFile->Append(HelpDemo_MS_Html_Help_Classes, "MS HTML &Help on Classes...");
    menuFile->Append(HelpDemo_MS_Html_Help_Functions, "MS HTML &Help on Functions...");
    menuFile->Append(HelpDemo_MS_Html_Help_Help, "MS HTML &About Help Demo...");
    menuFile->Append(HelpDemo_MS_Html_Help_Search, "MS HTML &Search help...");
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Best_Help_Index, "Best &Help Index...");
#endif

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

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

#if USE_HTML_HELP
    // Create embedded HTML Help window
    m_embeddedHelpWindow = new wxHtmlHelpWindow;
    // m_embeddedHtmlHelp.UseConfig(config, rootPath); // Can set your own config object here
    m_embeddedHtmlHelp.SetHelpWindow(m_embeddedHelpWindow);

    m_embeddedHelpWindow->Create(this,
        wxID_ANY, wxDefaultPosition, GetClientSize(), wxTAB_TRAVERSAL|wxNO_BORDER, wxHF_DEFAULT_STYLE);

    m_embeddedHtmlHelp.AddBook(wxFileName("doc.zip"));
    m_embeddedHtmlHelp.Display("Introduction");
#else
    // now create some controls

    // a panel first - if there were several controls, it would allow us to
    // navigate between them from the keyboard
    wxPanel *panel = new wxPanel(this, 301, wxPoint(0, 0), wxSize(400, 200));
    panel->SetHelpText(_("This panel just holds a static text control."));
    //panel->SetHelpText(wxContextId(300));

    // and a static control whose parent is the panel
    wxStaticText* staticText = new wxStaticText(panel, 302, "Hello, world!", wxPoint(10, 10));
    staticText->SetHelpText(_("This static text control isn't doing a lot right now."));
#endif
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnHelp(wxCommandEvent& event)
{
    ShowHelp(event.GetId(), m_help);
}

void MyFrame::OnShowContextHelp(wxCommandEvent& WXUNUSED(event))
{
    // This starts context help mode, then the user
    // clicks on a window to send a help message
    wxContextHelp contextHelp(this);
}

void MyFrame::OnShowDialogContextHelp(wxCommandEvent& WXUNUSED(event))
{
    MyModalDialog dialog(this);
    dialog.ShowModal();
}

void MyFrame::OnAdvancedHtmlHelp(wxCommandEvent& event)
{
#if USE_HTML_HELP
    ShowHelp(event.GetId(), m_advancedHtmlHelp);
#endif
}

#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
void MyFrame::OnMSHtmlHelp(wxCommandEvent& event)
{
    ShowHelp(event.GetId(), m_msHtmlHelp);
}
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP && !defined(__WXUNIVERSAL__)
void MyFrame::OnBestHelp(wxCommandEvent& event)
{
    ShowHelp(event.GetId(), m_bestHelp);
}
#endif

#if USE_HTML_HELP
void MyFrame::OnModalHtmlHelp(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlModalHelp(this, "doc.zip", "Introduction");
}
#endif

/*
 Notes: ShowHelp uses section ids for displaying particular topics,
 but you might want to use a unique keyword to display a topic, instead.

 Section ids are specified as follows for the different formats.

 WinHelp

   The [MAP] section specifies the topic to integer id mapping, e.g.

   [MAP]
   #define intro       100
   #define functions   1
   #define classes     2
   #define about       3

   The identifier name corresponds to the label used for that topic.
   You could also put these in a .h file and #include it in both the MAP
   section and your C++ source.

   Note that Tex2RTF doesn't currently generate the MAP section automatically.

 MS HTML Help

   The [MAP] section specifies the HTML filename root to integer id mapping, e.g.

   [MAP]
   #define doc1       100
   #define doc3       1
   #define doc2       2
   #define doc4       3

   The identifier name corresponds to the HTML filename used for that topic.
   You could also put these in a .h file and #include it in both the MAP
   section and your C++ source.

   Note that Tex2RTF doesn't currently generate the MAP section automatically.

 Simple wxHTML Help and External HTML Help

   A wxhelp.map file is used, for example:

   0 wx.htm             ; wxWidgets: Help index; additional keywords like overview
   1 wx204.htm          ; wxWidgets Function Reference
   2 wx34.htm           ; wxWidgets Class Reference

   Note that Tex2RTF doesn't currently generate the MAP section automatically.

 Advanced HTML Help

   An extension to the .hhc file format is used, specifying a new parameter
   with name="ID":

   <OBJECT type="text/sitemap">
   <param name="Local" value="doc2.htm#classes">
   <param name="Name" value="Classes">
   <param name="ID" value=2>
   </OBJECT>

   Again, this is not generated automatically by Tex2RTF, though it could
   be added quite easily.

   Unfortunately adding the ID parameters appears to interfere with MS HTML Help,
   so you should not try to compile a .chm file from a .hhc file with
   this extension, or the contents will be messed up.
 */

void MyFrame::ShowHelp(int commandId, wxHelpControllerBase& helpController)
{
   switch(commandId)
   {
       case HelpDemo_Help_Classes:
       case HelpDemo_Html_Help_Classes:
       case HelpDemo_Advanced_Html_Help_Classes:
       case HelpDemo_MS_Html_Help_Classes:
       case HelpDemo_Best_Help_Classes:
          helpController.DisplaySection(2);
          //helpController.DisplaySection("Classes"); // An alternative form for most controllers
          break;

       case HelpDemo_Help_Functions:
       case HelpDemo_Html_Help_Functions:
       case HelpDemo_Advanced_Html_Help_Functions:
       case HelpDemo_MS_Html_Help_Functions:
          helpController.DisplaySection(1);
          //helpController.DisplaySection("Functions"); // An alternative form for most controllers
          break;

       case HelpDemo_Help_Help:
       case HelpDemo_Html_Help_Help:
       case HelpDemo_Advanced_Html_Help_Help:
       case HelpDemo_MS_Html_Help_Help:
       case HelpDemo_Best_Help_Help:
          helpController.DisplaySection(3);
          //helpController.DisplaySection("About"); // An alternative form for most controllers
          break;

       case HelpDemo_Help_Search:
       case HelpDemo_Html_Help_Search:
       case HelpDemo_Advanced_Html_Help_Search:
       case HelpDemo_MS_Html_Help_Search:
       case HelpDemo_Best_Help_Search:
       {
          wxString key = wxGetTextFromUser("Search for?",
                                           "Search help for keyword",
                                           wxEmptyString,
                                           this);
          if(! key.IsEmpty())
             helpController.KeywordSearch(key);
       }
       break;

       case HelpDemo_Help_Index:
       case HelpDemo_Html_Help_Index:
       case HelpDemo_Advanced_Html_Help_Index:
       case HelpDemo_MS_Html_Help_Index:
       case HelpDemo_Best_Help_Index:
          helpController.DisplayContents();
          break;

       // These three calls are only used by wxExtHelpController

       case HelpDemo_Help_KDE:
          helpController.SetViewer("kdehelp");
          break;
       case HelpDemo_Help_GNOME:
          helpController.SetViewer("gnome-help-browser");
          break;
       case HelpDemo_Help_Netscape:
          helpController.SetViewer("netscape", wxHELP_NETSCAPE);
          break;
   }
}

// ----------------------------------------------------------------------------
// MyModalDialog
// Demonstrates context-sensitive help
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyModalDialog, wxDialog)
wxEND_EVENT_TABLE()

MyModalDialog::MyModalDialog(wxWindow *parent)
             : wxDialog(parent, wxID_ANY, wxString("Modal dialog"))
{
    // Add the context-sensitive help button on the caption for the platforms
    // which support it (currently MSW only)
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);


    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnOK = new wxButton(this, wxID_OK, "&OK");
    btnOK->SetHelpText(_("The OK button confirms the dialog choices."));

    wxButton* btnCancel = new wxButton(this, wxID_CANCEL, "&Cancel");
    btnCancel->SetHelpText(_("The Cancel button cancels the dialog."));

    sizerRow->Add(btnOK, 0, wxALIGN_CENTER | wxALL, 5);
    sizerRow->Add(btnCancel, 0, wxALIGN_CENTER | wxALL, 5);

    // Add explicit context-sensitive help button for non-MSW
#ifndef __WXMSW__
    sizerRow->Add(new wxContextHelpButton(this), 0, wxALIGN_CENTER | wxALL, 5);
#endif

    wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, "A demo text control",
                                      wxDefaultPosition, wxSize(300, 100),
                                      wxTE_MULTILINE);
    text->SetHelpText(_("Type text here if you have got nothing more interesting to do"));
    sizerTop->Add(text, 0, wxEXPAND|wxALL, 5 );
    sizerTop->Add(sizerRow, 0, wxALIGN_RIGHT|wxALL, 5 );

    SetSizerAndFit(sizerTop);

    btnOK->SetFocus();
    btnOK->SetDefault();
}

