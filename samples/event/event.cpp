/////////////////////////////////////////////////////////////////////////////
// Name:        event.cpp
// Purpose:     wxWindows sample demonstrating different event usage
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    // DO NOT use event.cpp here, it breaks statics initialization in
    // src/common/event.cpp and nothing works at all then!
    #pragma implementation "eventsample.cpp"
    #pragma interface "eventsample.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
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
    virtual ~MyFrame();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnDynamic(wxCommandEvent& event);
    void OnPushEventHandler(wxCommandEvent& event);
    void OnPopEventHandler(wxCommandEvent& event);
    void OnTest(wxCommandEvent& event);

    void OnUpdateUIPop(wxUpdateUIEvent& event);

protected:
    // number of pushed event handlers
    size_t m_nPush;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// Define a custom event handler
class MyEvtHandler : public wxEvtHandler
{
public:
    MyEvtHandler(size_t level) { m_level = level; }

    void OnTest(wxCommandEvent& event)
    {
        wxLogMessage(_T("This is the pushed test event handler #%u"), m_level);

        // if we don't skip the event, the other event handlers won't get it:
        // try commenting out this line and see what changes
        event.Skip();
    }

private:
    size_t m_level;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Event_Quit = 1,
    Event_About,
    Event_Connect,
    Event_Dynamic,
    Event_Push,
    Event_Pop,
    Event_Test
};

// status bar fields
enum
{
    Status_Main = 0,
    Status_Dynamic,
    Status_Push
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Event_Quit,  MyFrame::OnQuit)
    EVT_MENU(Event_About, MyFrame::OnAbout)

    EVT_MENU(Event_Connect, MyFrame::OnConnect)

    EVT_MENU(Event_Test, MyFrame::OnTest)
    EVT_MENU(Event_Push, MyFrame::OnPushEventHandler)
    EVT_MENU(Event_Pop, MyFrame::OnPopEventHandler)

    EVT_UPDATE_UI(Event_Pop, MyFrame::OnUpdateUIPop)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyEvtHandler, wxEvtHandler)
    EVT_MENU(Event_Test, MyEvtHandler::OnTest)
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

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame("Event wxWindows Sample",
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

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
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Event_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Event_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuEvent = new wxMenu;
    menuEvent->Append(Event_Connect, "&Connect\tCtrl-C",
                     "Connect or disconnect the dynamic event handler",
                     TRUE /* checkable */);
    menuEvent->Append(Event_Dynamic, "&Dynamic event\tCtrl-D",
                      "Dynamic event sample - only works after Connect");
    menuEvent->AppendSeparator();
    menuEvent->Append(Event_Push, "&Push event handler\tCtrl-P",
                      "Push event handler for test event");
    menuEvent->Append(Event_Pop, "P&op event handler\tCtrl-O",
                      "Pop event handler for test event");
    menuEvent->Append(Event_Test, "Test event\tCtrl-T",
                      "Test event processed by pushed event handler");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEvent, "&Event");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(3);
    SetStatusText("Welcome to wxWindows event sample");
    SetStatusText(_T("Dynamic: off"), Status_Dynamic);
    SetStatusText(_T("Push count: 0"), Status_Push);
#endif // wxUSE_STATUSBAR
}

MyFrame::~MyFrame()
{
    // we must pop any remaining event handlers to avoid memory leaks and
    // crashes!
    while ( m_nPush-- != 0 )
    {
        PopEventHandler(TRUE /* delete handler */);
    }
}

// ----------------------------------------------------------------------------
// standard event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Event sample shows different ways of using events\n"
                    "© 2001 Vadim Zeitlin"),
                 _T("About Event Sample"), wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// dynamic event handling stuff
// ----------------------------------------------------------------------------

void MyFrame::OnDynamic(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is a dynamic event handler which can be connected "
                    "and disconnected during run-time."),
                 _T("Dynamic Event Handler"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnConnect(wxCommandEvent& event)
{
    if ( event.IsChecked() )
    {
        // disconnect
        Connect(Event_Dynamic, -1, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction)
                (wxEventFunction)
                (wxCommandEventFunction)&MyFrame::OnDynamic);

        SetStatusText(_T("You can now use \"Dynamic\" item in the menu"));
        SetStatusText(_T("Dynamic: on"), Status_Dynamic);
    }
    else // connect
    {
        Disconnect(Event_Dynamic, -1, wxEVT_COMMAND_MENU_SELECTED);

        SetStatusText(_T("You can no more use \"Dynamic\" item in the menu"));
        SetStatusText(_T("Dynamic: off"), Status_Dynamic);
    }
}

// ----------------------------------------------------------------------------
// push/pop event handlers support
// ----------------------------------------------------------------------------

void MyFrame::OnPushEventHandler(wxCommandEvent& WXUNUSED(event))
{
    PushEventHandler(new MyEvtHandler(++m_nPush));

    SetStatusText(wxString::Format(_T("Push count: %u"), m_nPush), Status_Push);
}

void MyFrame::OnPopEventHandler(wxCommandEvent& WXUNUSED(event))
{
    wxCHECK_RET( m_nPush, _T("this command should be disabled!") );

    PopEventHandler(TRUE /* delete handler */);
    m_nPush--;

    SetStatusText(wxString::Format(_T("Push count: %u"), m_nPush), Status_Push);
}

void MyFrame::OnTest(wxCommandEvent& event)
{
    wxLogMessage(_T("This is the test event handler in the main frame"));
}

void MyFrame::OnUpdateUIPop(wxUpdateUIEvent& event)
{
    event.Enable( m_nPush > 0 );
}

