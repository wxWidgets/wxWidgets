/////////////////////////////////////////////////////////////////////////////
// Name:        help.cpp
// Purpose:     wxHtml sample: help test
// Author:      ?
// Modified by:
// Created:     ?
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/html/helpfrm.h"
#include "wx/html/helpctrl.h"
#include "wx/filesys.h"
#include "wx/fs_zip.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../../sample.xpm"
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
};



// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
private:
    wxHtmlHelpController help;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_Help
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_Help, MyFrame::OnHelp)
    EVT_CLOSE(MyFrame::OnClose)
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

    wxInitAllImageHandlers();
#if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB
    wxFileSystem::AddHandler(new wxZipFSHandler);
#endif
    SetVendorName("wxWidgets");
    SetAppName("wxHTMLHelp");

    // Create the main application window
    MyFrame *frame = new MyFrame(_("HTML Help Sample"),
        wxDefaultPosition, wxDefaultSize);

    // Show it
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------


// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
            : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size),
                help(wxHF_DEFAULT_STYLE | wxHF_OPEN_FILES)
{
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Minimal_Help, _("&Help"));
    menuFile->Append(Minimal_Quit, _("E&xit"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    help.UseConfig(wxConfig::Get());
    bool ret;
    help.SetTempDir(".");

    wxPathList pathlist;
    pathlist.Add("./helpfiles");
    pathlist.Add("../helpfiles");
    pathlist.Add("../../html/help/helpfiles");

    ret = help.AddBook(wxFileName(pathlist.FindValidPath("testing.hhp"), wxPATH_UNIX));
    if (! ret)
        wxMessageBox("Failed adding book helpfiles/testing.hhp");
    ret = help.AddBook(wxFileName(pathlist.FindValidPath("another.hhp"), wxPATH_UNIX));
    if (! ret)
        wxMessageBox(_("Failed adding book helpfiles/another.hhp"));
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    help.Display("Test HELPFILE");
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    // Close the help frame; this will cause the config data to
    // get written.
    if ( help.GetFrame() ) // returns NULL if no help frame active
        help.GetFrame()->Close(true);
    // now we can safely delete the config pointer
    event.Skip();
    delete wxConfig::Set(NULL);
}



