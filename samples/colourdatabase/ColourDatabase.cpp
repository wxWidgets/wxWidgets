/////////////////////////////////////////////////////////////////////////////
// Name:        ColourDatabase.cpp
// Purpose:     ColourDatabase wxWidgets sample
// Author:      Kenneth Porter
// Modified by:
// Created:     08/15/2022
// Copyright:   (c) Kenneth Porter, Vadim Zeitlin
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
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
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
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
 public:
     // ctor(s)
     MyFrame(const wxString& title);
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("ColourDatabase wxWidgets App");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
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
MyFrame::MyFrame(const wxString& title)
: wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    const wxVector<wxString> names = wxTheColourDatabase->GetAllNames();
    const unsigned rows = std::sqrt(names.size());
    // use a flex grid sizer to display all colours in the database as
    // button backgrounds
    wxFlexGridSizer* fgSizer = new wxFlexGridSizer( rows, 0, 0, 0 );
    fgSizer->SetFlexibleDirection( wxBOTH );
    fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    for (unsigned i = 0; i < names.size(); ++i)
    {
        wxButton* button = new wxButton( this, wxID_ANY, names[i], wxDefaultPosition, wxDefaultSize, 0 );
        button->SetBackgroundColour(names[i]);
        // set button's text color to black or white based on background
        // luminance value
        double luminance = wxTheColourDatabase->Find(names[i]).GetLuminance();
        button->SetForegroundColour((luminance > 0.5) ? "BLACK" : "WHITE");
        fgSizer->Add( button, 0, wxALL|wxEXPAND, 5 );
    }

    // make all the cells growable
    for (unsigned row = 0; row < rows; ++row)
        fgSizer->AddGrowableRow(row);
    const unsigned cols = fgSizer->GetEffectiveColsCount();
    for (unsigned col = 0; col < cols; ++col)
        fgSizer->AddGrowableCol(col);

    this->SetSizer( fgSizer );
    this->Layout();
    this->Fit();
    this->Centre( wxBOTH );
}
