/////////////////////////////////////////////////////////////////////////////
// Name:        uiaction.cpp
// Purpose:     wxUIActionSimulator sample
// Author:      Kevin Ollivier
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Kevin Ollivier, Steven Lamerton
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
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_UIACTIONSIMULATOR
    #include "wx/uiaction.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    RunSimulation = 1
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

#if wxUSE_UIACTIONSIMULATOR

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    void OnButtonPressed(wxCommandEvent& event);
    void OnRunSimulation(wxCommandEvent& event);
    void OnExit(wxCommandEvent& WXUNUSED(event)) { Close(); }

private:
    wxButton* m_button;
    wxTextCtrl* m_text;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(wxID_ANY, MyFrame::OnButtonPressed)
    EVT_MENU(RunSimulation, MyFrame::OnRunSimulation)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

#endif // wxUSE_UIACTIONSIMULATOR

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_UIACTIONSIMULATOR
    MyFrame *frame = new MyFrame("wxUIActionSimulator sample application");
    frame->Show(true);

    return true;
#else // !wxUSE_UIACTIONSIMULATOR
    wxLogError("wxUSE_UIACTIONSIMULATOR must be 1 for this sample");
    return false;
#endif // wxUSE_UIACTIONSIMULATOR/!wxUSE_UIACTIONSIMULATOR
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

#if wxUSE_UIACTIONSIMULATOR

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_NEW, "&New File...", "Open a new file");
    fileMenu->Append(RunSimulation, "&Run Simulation...", "Run the UI action simulation");

    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    wxPanel *panel = new wxPanel(this);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(sizer);

    m_button = new wxButton(panel, wxID_ANY, "&Button");
    sizer->Add(m_button, wxSizerFlags().Centre().Border());

    m_text = new wxTextCtrl(panel, wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE);
    sizer->Add(m_text, wxSizerFlags(1).Expand().Border());
}


// event handlers

void MyFrame::OnRunSimulation(wxCommandEvent& WXUNUSED(event))
{
    wxUIActionSimulator sim;

    // Add some extra distance to take account of window decorations
    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    sim.MouseClick(wxMOUSE_BTN_LEFT);

    // Process the resulting button event
    wxYield();

    m_text->SetFocus();
    sim.Char('A');
    sim.Char('A', wxMOD_SHIFT);
    sim.Char(WXK_RETURN);
    sim.Char('Z');
    sim.Char('Z', wxMOD_SHIFT);
    sim.Char(WXK_RETURN);
    sim.Text("aAbBcC");
    sim.Char(WXK_RETURN);
}

void MyFrame::OnButtonPressed(wxCommandEvent& WXUNUSED(event))
{
    m_text->AppendText("Button pressed.\n");
}

#endif // wxUSE_UIACTIONSIMULATOR
