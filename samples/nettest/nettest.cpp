/////////////////////////////////////////////////////////////////////////////
// Name:        net.cpp
// Purpose:     wxWindows sample demonstrating network-related functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "nettest.cpp"
    #pragma interface "nettest.cpp"
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

#include "wx/net.h"

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

    // event handlers
    void OnConnected(wxDialUpEvent& event);

private:
    DECLARE_EVENT_TABLE();
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
    void OnHangUp(wxCommandEvent& event);
    void OnDial(wxCommandEvent& event);

    void OnIdle(wxIdleEvent& event);

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
    NetTest_Quit = 1,
    NetTest_About,
    NetTest_HangUp,
    NetTest_Dial
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_DIALUP_CONNECTED(MyApp::OnConnected)
    EVT_DIALUP_DISCONNECTED(MyApp::OnConnected)
END_EVENT_TABLE()

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(NetTest_Quit,  MyFrame::OnQuit)
    EVT_MENU(NetTest_About, MyFrame::OnAbout)
    EVT_MENU(NetTest_HangUp, MyFrame::OnHangUp)
    EVT_MENU(NetTest_Dial, MyFrame::OnDial)

    EVT_IDLE(MyFrame::OnIdle)
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
    MyFrame *frame = new MyFrame("Minimal wxWindows App",
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

void MyApp::OnConnected(wxDialUpEvent& event)
{
    wxMessageBox(event.IsConnectedEvent() ? "Just connected!"
                                          : "Disconnected",
                 "Dial Up Manager Notification",
                 wxOK | wxICON_INFORMATION,
                 GetTopWindow());
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

    menuFile->Append(NetTest_Dial, "&Dial\tCtrl-D", "Dial default ISP");
    menuFile->Append(NetTest_HangUp, "&HangUp\tCtrl-H", "Hang up modem");
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    CreateStatusBar(2);
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
    msg.Printf(_T("This is the network functions test sample.\n"
                  "© 1999 Vadim Zeitlin"));

    wxMessageBox(msg, _T("About NetTest"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnHangUp(wxCommandEvent& WXUNUSED(event))
{
    if ( wxDialUpManager::Get()->HangUp() )
    {
        wxLogStatus(this, "Connection was succesfully terminated.");
    }
    else
    {
        wxLogStatus(this, "Failed to hang up.");
    }
}

void MyFrame::OnDial(wxCommandEvent& WXUNUSED(event))
{
    wxLogStatus(this, "Dialing...");
    wxYield();
    wxBeginBusyCursor();

    if ( wxDialUpManager::Get()->Dial("Free",
                                      "zeitlin", "2ecj86ij") )
    {
        wxLogStatus(this, "Connection was succesfully established.");
    }
    else
    {
        wxLogStatus(this, "Dialing attempt failed.");
    }

    wxEndBusyCursor();
}

void MyFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    static int s_isOnline = -1; // not TRUE nor FALSE

    bool isOnline = wxDialUpManager::Get()->IsOnline();
    if ( s_isOnline != (int)isOnline )
    {
        s_isOnline = isOnline;

        SetStatusText(isOnline ? "Online" : "Offline", 1);
    }
}
