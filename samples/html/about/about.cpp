/////////////////////////////////////////////////////////////////////////////
// Name:        about.cpp
// Purpose:     wxHtml sample: about dialog test
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
#include "wx/imagpng.h"
#include "wx/wxhtml.h"
#include "wx/statline.h"

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
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
   EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
   EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)
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

    // we use a PNG image in our HTML page
    wxImage::AddHandler(new wxPNGHandler);

    // create and show the main application window
    MyFrame *frame = new MyFrame(_("wxHtmlWindow testing application"));
    frame->Show();

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(wxID_ABOUT);
    menuFile->Append(wxID_EXIT);

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxBoxSizer *topsizer;
    wxHtmlWindow *html;
    wxDialog dlg(this, wxID_ANY, wxString(_("About")));

    topsizer = new wxBoxSizer(wxVERTICAL);

    html = new wxHtmlWindow(&dlg, wxID_ANY, wxDefaultPosition, wxSize(380, 160), wxHW_SCROLLBAR_NEVER);
    html -> SetBorders(0);
    html -> LoadPage("data/about.htm");
    html -> SetHTMLBackgroundImage(wxBitmapBundle::FromSVGFile("data/bg.svg", wxSize(65, 45)));
    html -> SetInitialSize(wxSize(html -> GetInternalRepresentation() -> GetWidth(),
                                  html -> GetInternalRepresentation() -> GetHeight()));

    topsizer -> Add(html, 1, wxALL, 10);

#if wxUSE_STATLINE
    topsizer -> Add(new wxStaticLine(&dlg, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
#endif // wxUSE_STATLINE

    wxButton *bu1 = new wxButton(&dlg, wxID_OK, _("OK"));
    bu1 -> SetDefault();

    topsizer -> Add(bu1, 0, wxALL | wxALIGN_RIGHT, 15);

    dlg.SetSizer(topsizer);
    topsizer -> Fit(&dlg);

    dlg.ShowModal();
}

