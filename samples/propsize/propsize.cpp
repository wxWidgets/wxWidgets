/////////////////////////////////////////////////////////////////////////////
// Name:        propsize.cpp
// Purpose:     Minimal wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "propsize.cpp"
    #pragma interface "propsize.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/statline.h"

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

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
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
    Minimal_Quit = 1,
    Minimal_About
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
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
    MyFrame *frame = new MyFrame("Proportional resize",
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    // @@@ what does it do exactly, in fact? is it necessary here?
    frame->Show(TRUE);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
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
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);

    menuFile->Append(Minimal_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    SetStatusText("Resize the frame to see how controls react");
#endif // wxUSE_STATUSBAR

#define AddLine(orient) \
     Add( new wxStaticLine( this, -1, wxDefaultPosition, wxSize(2,2), orient), \
     0, wxEXPAND)
#define AddButton(label,align) Add( \
     new wxButton( this, -1, label, wxDefaultPosition, wxSize(100,50)), \
     1, wxSHAPED | align)

  wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
  // top row -- top-aligned
  wxBoxSizer *hsizer1 = new wxBoxSizer( wxHORIZONTAL );
  hsizer1->AddButton( "one", wxALIGN_LEFT | wxALIGN_TOP);
  hsizer1->AddLine(wxVERTICAL);
  hsizer1->AddButton( "two", wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP);
  hsizer1->AddLine(wxVERTICAL);
  hsizer1->AddButton( "three", wxALIGN_RIGHT | wxALIGN_TOP);

  topsizer->Add(hsizer1, 1, wxEXPAND);
  topsizer->AddLine(wxHORIZONTAL);

  wxBoxSizer *hsizer2 = new wxBoxSizer( wxHORIZONTAL );
  hsizer2->AddButton( "four", wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
  hsizer2->AddLine(wxVERTICAL);
  // sizer that preserves it's shape
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );
  vsizer->Add(
     new wxButton( this, -1, "up", wxDefaultPosition, wxSize(100,25)), \
     1, wxEXPAND);
  vsizer->Add(
     new wxButton( this, -1, "down", wxDefaultPosition, wxSize(100,25)), \
     1, wxEXPAND);
  hsizer2->Add(vsizer, 1, wxSHAPED | wxALIGN_CENTER);
  hsizer2->AddLine(wxVERTICAL);
  hsizer2->AddButton( "six", wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

  topsizer->Add(hsizer2, 1, wxEXPAND);
  topsizer->AddLine(wxHORIZONTAL);

  wxBoxSizer *hsizer3 = new wxBoxSizer( wxHORIZONTAL );
  hsizer3->AddButton( "seven", wxALIGN_LEFT | wxALIGN_BOTTOM);
  hsizer3->AddLine(wxVERTICAL);
  hsizer3->AddButton( "eight", wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM);
  hsizer3->AddLine(wxVERTICAL);
  // wxEXPAND should have no effect
  hsizer3->AddButton( "nine", wxEXPAND | wxALIGN_RIGHT | wxALIGN_BOTTOM);

  topsizer->Add(hsizer3, 1, wxEXPAND);

  // set frame to minimum size
  topsizer->Fit( this );

  // don't allow frame to get smaller than what the sizers tell ye
  // topsizer->SetSizeHints( this );

  SetSizer( topsizer );
  SetAutoLayout( TRUE );
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of proportional sizer test.\n")
                _T("Welcome to %s")
#ifdef wxBETA_NUMBER
               _T(" (beta %d)!")
#endif // wxBETA_NUMBER
               , wxVERSION_STRING
#ifdef wxBETA_NUMBER
               , wxBETA_NUMBER
#endif // wxBETA_NUMBER
              );

    wxMessageBox(msg, "About Shaped Sizer", wxOK | wxICON_INFORMATION, this);
}
