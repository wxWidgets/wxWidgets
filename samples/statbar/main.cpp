/////////////////////////////////////////////////////////////////////////////
// Name:        Main.cpp
// Purpose:     Minimal wxStatusBar sample
// Author:      Georg Tschojer
// Created:     15.06.26
// Copyright:   (c) Georg Tschojer
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// DECLARATIONS
// ============================================================================

#ifndef MAIN_HPP
#define MAIN_HPP

#include <wx/wx.h>

// Standard application class
class MyApp : public wxApp {
public:
    virtual bool OnInit() override;
};

// Standard main frame class
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);
};

#endif // MAIN_HPP


// ============================================================================
// IMPLEMENTATION
// ============================================================================

#include <wx/wx.h>
#include "main.hpp"

// Constructor for the main window
MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300)) {

    // Creates a default status bar at the bottom of the window
    wxStatusBar *statusBar = CreateStatusBar();

    // Sets the text for the first (and only) status bar field
    statusBar->SetStatusText("Ready");
}

// Macro to launch the application using the standard class
wxIMPLEMENT_APP(MyApp);

// Application entry point
bool MyApp::OnInit() {
    // Create the main frame with a default title
    MyFrame* frame = new MyFrame("wxStatusBar Demo");
    frame->Centre();
    frame->Show(true);

    return true;
}
