/////////////////////////////////////////////////////////////////////////////
// Name:        propsize.cpp
// Purpose:     wxWidgets propsize sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// License:     wxWindows license
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
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/statline.h"

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
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
    MyFrame *frame = new MyFrame(_T("Proportional resize"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    // @@@ what does it do exactly, in fact? is it necessary here?
    frame->Show(true);
    SetTopWindow(frame);

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
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);

    menuFile->Append(wxID_ABOUT, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    SetStatusText(_T("Resize the frame to see how controls react"));
#endif // wxUSE_STATUSBAR

#if wxUSE_STATLINE
    #define AddLine(orient) \
        Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxSize(2,2), orient), \
        0, wxEXPAND)
#else
    #define AddLine(orient) \
        Add( 2, 2)
#endif // wxUSE_STATLINE

#define AddButton(label,align) Add( \
    new wxButton( this, wxID_ANY, label, wxDefaultPosition, wxSize(100,50)), \
    1, wxSHAPED | align)

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    // top row -- top-aligned
    wxBoxSizer *hsizer1 = new wxBoxSizer( wxHORIZONTAL );
    hsizer1->AddButton( _T("one"), wxALIGN_LEFT | wxALIGN_TOP);
    hsizer1->AddLine(wxVERTICAL);
    hsizer1->AddButton( _T("two"), wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP);
    hsizer1->AddLine(wxVERTICAL);
    hsizer1->AddButton( _T("three"), wxALIGN_RIGHT | wxALIGN_TOP);

    topsizer->Add(hsizer1, 1, wxEXPAND);
    topsizer->AddLine(wxHORIZONTAL);

    wxBoxSizer *hsizer2 = new wxBoxSizer( wxHORIZONTAL );
    hsizer2->AddButton( _T("four"), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    hsizer2->AddLine(wxVERTICAL);
    // sizer that preserves it's shape
    wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );
    vsizer->Add(
        new wxButton( this, wxID_ANY, _T("up"), wxDefaultPosition, wxSize(100,25) ),
        1, wxEXPAND);

    vsizer->Add(
        new wxButton( this, wxID_ANY, _T("down"), wxDefaultPosition, wxSize(100,25) ),
        1, wxEXPAND);

    hsizer2->Add(vsizer, 1, wxSHAPED | wxALIGN_CENTER);
    hsizer2->AddLine(wxVERTICAL);
    hsizer2->AddButton( _T("six"), wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    topsizer->Add(hsizer2, 1, wxEXPAND);
    topsizer->AddLine(wxHORIZONTAL);

    wxBoxSizer *hsizer3 = new wxBoxSizer( wxHORIZONTAL );
    hsizer3->AddButton( _T("seven"), wxALIGN_LEFT | wxALIGN_BOTTOM);
    hsizer3->AddLine(wxVERTICAL);
    hsizer3->AddButton( _T("eight"), wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM);
    hsizer3->AddLine(wxVERTICAL);
    // wxEXPAND should have no effect
    hsizer3->AddButton( _T("nine"), wxEXPAND | wxALIGN_RIGHT | wxALIGN_BOTTOM);

    topsizer->Add(hsizer3, 1, wxEXPAND);

    // set frame to minimum size
    topsizer->Fit( this );

    // don't allow frame to get smaller than what the sizers tell ye
    // topsizer->SetSizeHints( this );

    SetSizer( topsizer );
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
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

    wxMessageBox(msg, _T("About Shaped Sizer"), wxOK | wxICON_INFORMATION, this);
}
