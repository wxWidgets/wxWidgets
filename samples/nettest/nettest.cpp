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

#include "wx/dialup.h"

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

    // called before the application termination
    virtual int OnExit();

    // event handlers
    void OnConnected(wxDialUpEvent& event);

    // accessor to dial up manager
    wxDialUpManager *GetDialer() const { return m_dial; }

private:
    wxDialUpManager *m_dial;

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
    void OnEnumISPs(wxCommandEvent& event);
    void OnCheck(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

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
    NetTest_Dial,
    NetTest_EnumISP,
    NetTest_Check,
    NetTest_Max
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
    EVT_MENU(NetTest_EnumISP, MyFrame::OnEnumISPs)
    EVT_MENU(NetTest_Check, MyFrame::OnCheck)

    EVT_UPDATE_UI(NetTest_Dial, MyFrame::OnUpdateUI)

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
    MyFrame *frame = new MyFrame("Dial-up wxWindows demo",
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    // Init dial up manager
    m_dial = wxDialUpManager::Create();

    if ( !m_dial->IsOk() )
    {
        wxLogError("The sample can't run on this system.");

        wxLog::GetActiveTarget()->Flush();

        // do it here, OnExit() won't be called
        delete m_dial;

        return FALSE;
    }

    frame->SetStatusText(GetDialer()->IsAlwaysOnline() ? "LAN" : "No LAN", 2);

    return TRUE;
}

int MyApp::OnExit()
{
    delete m_dial;

    // exit code is 0, everything is ok
    return 0;
}

void MyApp::OnConnected(wxDialUpEvent& event)
{
    const char *msg;
    if ( event.IsOwnEvent() )
    {
        msg = event.IsConnectedEvent() ? "Successfully connected"
                                       : "Dialing failed";

        wxLogStatus("");
    }
    else
    {
        msg = event.IsConnectedEvent() ? "Just connected!"
                                       : "Disconnected";
    }

    wxLogMessage(msg);
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(NetTest_Dial, "&Dial\tCtrl-D", "Dial default ISP");
    menuFile->Append(NetTest_HangUp, "&HangUp\tCtrl-H", "Hang up modem");
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_EnumISP, "&Enumerate ISPs...\tCtrl-E");
    menuFile->Append(NetTest_Check, "&Check connection status...\tCtrl-C");
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create status bar and fill the LAN field
    CreateStatusBar(3);
    static const int widths[3] = { -1, 100, 60 };
    SetStatusWidths(3, widths);
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
    if ( wxGetApp().GetDialer()->HangUp() )
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
    wxLogStatus(this, "Preparing to dial...");
    wxYield();
    wxBeginBusyCursor();

    if ( wxGetApp().GetDialer()->Dial() )
    {
        wxLogStatus(this, "Dialing...");
    }
    else
    {
        wxLogStatus(this, "Dialing attempt failed.");
    }

    wxEndBusyCursor();
}

void MyFrame::OnCheck(wxCommandEvent& WXUNUSED(event))
{
   if(wxGetApp().GetDialer()->IsOnline())
      wxLogMessage("Network is online.");
   else
      wxLogMessage("Network is offline.");
}

void MyFrame::OnEnumISPs(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString names;
    size_t nCount = wxGetApp().GetDialer()->GetISPNames(names);
    if ( nCount == 0 )
    {
        wxLogWarning("No ISPs found.");
    }
    else
    {
        wxString msg = "Known ISPs:\n";
        for ( size_t n = 0; n < nCount; n++ )
        {
            msg << names[n] << '\n';
        }

        wxLogMessage(msg);
    }
}

void MyFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    // disable this item while dialing
    event.Enable( !wxGetApp().GetDialer()->IsDialing() );
}

void MyFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    static int s_isOnline = -1; // not TRUE nor FALSE

    bool isOnline = wxGetApp().GetDialer()->IsOnline();
    if ( s_isOnline != (int)isOnline )
    {
        s_isOnline = isOnline;

        SetStatusText(isOnline ? "Online" : "Offline", 1);
    }
}
