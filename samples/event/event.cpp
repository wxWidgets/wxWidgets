/////////////////////////////////////////////////////////////////////////////
// Name:        event.cpp
// Purpose:     wxWidgets sample demonstrating different event usage
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

// ----------------------------------------------------------------------------
// event constants
// ----------------------------------------------------------------------------

// declare a custom event type
//
// note that in wxWin 2.3+ these macros expand simply into the following code:
//
//  extern const wxEventType wxEVT_MY_CUSTOM_COMMAND;
//
//  const wxEventType wxEVT_MY_CUSTOM_COMMAND = wxNewEventType();
//
// and you may use this code directly if you don't care about 2.2 compatibility
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_MY_CUSTOM_COMMAND, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_MY_CUSTOM_COMMAND)

// it may also be convenient to define an event table macro for this event type
#define EVT_MY_CUSTOM_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_CUSTOM_COMMAND, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

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

    void OnFireCustom(wxCommandEvent& event);
    void OnProcessCustom(wxCommandEvent& event);

    void OnUpdateUIPop(wxUpdateUIEvent& event);

protected:
    // number of pushed event handlers
    unsigned m_nPush;

private:
    // any class wishing to process wxWidgets events must use this macro
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
    unsigned m_level;

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
    Event_Custom,
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
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Event_Quit,  MyFrame::OnQuit)
    EVT_MENU(Event_About, MyFrame::OnAbout)

    EVT_MENU(Event_Connect, MyFrame::OnConnect)

    EVT_MENU(Event_Custom, MyFrame::OnFireCustom)
    EVT_MENU(Event_Test, MyFrame::OnTest)
    EVT_MENU(Event_Push, MyFrame::OnPushEventHandler)
    EVT_MENU(Event_Pop, MyFrame::OnPopEventHandler)

    EVT_UPDATE_UI(Event_Pop, MyFrame::OnUpdateUIPop)

    EVT_MY_CUSTOM_COMMAND(wxID_ANY, MyFrame::OnProcessCustom)

    // the line below would also work if OnProcessCustom() were defined as
    // taking a wxEvent (as required by EVT_CUSTOM) and not wxCommandEvent
    //EVT_CUSTOM(wxEVT_MY_CUSTOM_COMMAND, wxID_ANY, MyFrame::OnProcessCustom)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyEvtHandler, wxEvtHandler)
    EVT_MENU(Event_Test, MyEvtHandler::OnTest)
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

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("Event wxWidgets Sample"),
                                 wxPoint(50, 50), wxSize(600, 340));

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
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // init members
    m_nPush = 0;

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Event_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Event_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *menuEvent = new wxMenu;
    menuEvent->Append(Event_Connect, _T("&Connect\tCtrl-C"),
                     _T("Connect or disconnect the dynamic event handler"),
                     true /* checkable */);
    menuEvent->Append(Event_Dynamic, _T("&Dynamic event\tCtrl-D"),
                      _T("Dynamic event sample - only works after Connect"));
    menuEvent->AppendSeparator();
    menuEvent->Append(Event_Push, _T("&Push event handler\tCtrl-P"),
                      _T("Push event handler for test event"));
    menuEvent->Append(Event_Pop, _T("P&op event handler\tCtrl-O"),
                      _T("Pop event handler for test event"));
    menuEvent->Append(Event_Test, _T("Test event\tCtrl-T"),
                      _T("Test event processed by pushed event handler"));
    menuEvent->AppendSeparator();
    menuEvent->Append(Event_Custom, _T("Fire c&ustom event\tCtrl-U"),
                      _T("Generate a custom event"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuEvent, _T("&Event"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(3);
    SetStatusText(_T("Welcome to wxWidgets event sample"));
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
        PopEventHandler(true /* delete handler */);
    }
}

// ----------------------------------------------------------------------------
// standard event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( wxT("Event sample shows different ways of using events\n")
                  wxT("(c) 2001 Vadim Zeitlin"),
                  wxT("About Event Sample"), wxOK | wxICON_INFORMATION, this );
}

// ----------------------------------------------------------------------------
// dynamic event handling stuff
// ----------------------------------------------------------------------------

void MyFrame::OnDynamic(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox
    (
        wxT("This is a dynamic event handler which can be connected ")
        wxT("and disconnected at run-time."),
        wxT("Dynamic Event Handler"), wxOK | wxICON_INFORMATION, this
    );
}

void MyFrame::OnConnect(wxCommandEvent& event)
{
    if ( event.IsChecked() )
    {
        // disconnect
        Connect(Event_Dynamic, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction)
                (wxEventFunction)
                (wxCommandEventFunction)&MyFrame::OnDynamic);

#if wxUSE_STATUSBAR
        SetStatusText(_T("You can now use \"Dynamic\" item in the menu"));
        SetStatusText(_T("Dynamic: on"), Status_Dynamic);
#endif // wxUSE_STATUSBAR
    }
    else // connect
    {
        Disconnect(Event_Dynamic, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED);

#if wxUSE_STATUSBAR
        SetStatusText(_T("You can no more use \"Dynamic\" item in the menu"));
        SetStatusText(_T("Dynamic: off"), Status_Dynamic);
#endif // wxUSE_STATUSBAR
    }
}

// ----------------------------------------------------------------------------
// push/pop event handlers support
// ----------------------------------------------------------------------------

void MyFrame::OnPushEventHandler(wxCommandEvent& WXUNUSED(event))
{
    PushEventHandler(new MyEvtHandler(++m_nPush));

#if wxUSE_STATUSBAR
    SetStatusText(wxString::Format(_T("Push count: %u"), m_nPush), Status_Push);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnPopEventHandler(wxCommandEvent& WXUNUSED(event))
{
    wxCHECK_RET( m_nPush, _T("this command should be disabled!") );

    PopEventHandler(true /* delete handler */);
    m_nPush--;

#if wxUSE_STATUSBAR
    SetStatusText(wxString::Format(_T("Push count: %u"), m_nPush), Status_Push);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(_T("This is the test event handler in the main frame"));
}

void MyFrame::OnUpdateUIPop(wxUpdateUIEvent& event)
{
    event.Enable( m_nPush > 0 );
}

// ----------------------------------------------------------------------------
// custom event methods
// ----------------------------------------------------------------------------

void MyFrame::OnFireCustom(wxCommandEvent& WXUNUSED(event))
{
    wxCommandEvent eventCustom(wxEVT_MY_CUSTOM_COMMAND);

    wxPostEvent(this, eventCustom);
}

void MyFrame::OnProcessCustom(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(_T("Got a custom event!"));
}

