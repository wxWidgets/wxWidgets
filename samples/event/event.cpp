/////////////////////////////////////////////////////////////////////////////
// Name:        event.cpp
// Purpose:     wxWidgets sample demonstrating different event usage
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.01.01
// Copyright:   (c) 2001-2009 Vadim Zeitlin
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

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include <wx/statline.h>
#include <wx/log.h>
#include "gestures.h"
#include "chessboard.h"

// ----------------------------------------------------------------------------
// event constants
// ----------------------------------------------------------------------------

// define a custom event type (we don't need a separate declaration here but
// usually you would use a matching wxDECLARE_EVENT in a header)
wxDEFINE_EVENT(wxEVT_MY_CUSTOM_COMMAND, wxCommandEvent);

// it may also be convenient to define an event table macro for this event type
#define EVT_MY_CUSTOM_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_CUSTOM_COMMAND, id, wxID_ANY, \
        wxCommandEventHandler(fn), \
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
    virtual bool OnInit() wxOVERRIDE;

    // these are regular event handlers used to highlight the events handling
    // order
    void OnClickDynamicHandlerApp(wxCommandEvent& event);
    void OnClickStaticHandlerApp(wxCommandEvent& event);

    // we override wxAppConsole::FilterEvent used to highlight the events
    // handling order
    virtual int FilterEvent(wxEvent& event) wxOVERRIDE;

private:
    wxDECLARE_EVENT_TABLE();
};

// Define a custom button used to highlight the events handling order
class MyEvtTestButton : public wxButton
{
public:
    static long BUTTON_ID;

    MyEvtTestButton(wxWindow *parent, const wxString& label)
        : wxButton(parent, BUTTON_ID, label)
    {
        // Add a dynamic handler for this button event to button itself
        Bind(wxEVT_BUTTON, &MyEvtTestButton::OnClickDynamicHandler, this);
    }

private:
    void OnClickDynamicHandler(wxCommandEvent& event)
    {
        wxLogMessage("Step 3 in \"How Events are Processed\":\n"
                     "Button::ownDynamicHandler");

        event.Skip();
    }

    void OnClickStaticHandler(wxCommandEvent& event)
    {
        wxLogMessage("Step 4 in \"How Events are Processed\":\n"
                     "Button::ownStaticHandler");

        event.Skip();
    }

    wxDECLARE_EVENT_TABLE();
};

long MyEvtTestButton::BUTTON_ID = wxNewId();

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~MyFrame();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnBind(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnDynamic(wxCommandEvent& event);
    void OnPushEventHandler(wxCommandEvent& event);
    void OnPopEventHandler(wxCommandEvent& event);
    void OnTest(wxCommandEvent& event);

    void OnFireCustom(wxCommandEvent& event);
    void OnProcessCustom(wxCommandEvent& event);

    void OnUpdateUIPop(wxUpdateUIEvent& event);

    // regular event handlers used to highlight the events handling order
    void OnClickDynamicHandlerFrame(wxCommandEvent& event);
    void OnClickDynamicHandlerButton(wxCommandEvent& event);
    void OnClickStaticHandlerFrame(wxCommandEvent& event);

    // Gesture
    void OnGesture(wxCommandEvent& event);

    // Demonstrates using a new event class
    void OnNewEventClass(wxCommandEvent& event);

private:
    // symbolic names for the status bar fields
    enum
    {
        Status_Main = 0,
        Status_Dynamic,
        Status_Push
    };

    void UpdateDynamicStatus(bool on)
    {
#if wxUSE_STATUSBAR
        if ( on )
        {
            SetStatusText("You can now use \"Dynamic\" item in the menu");
            SetStatusText("Dynamic: on", Status_Dynamic);
        }
        else
        {
            SetStatusText("You can no more use \"Dynamic\" item in the menu");
            SetStatusText("Dynamic: off", Status_Dynamic);
        }
#endif // wxUSE_STATUSBAR
    }

    // number of pushed event handlers
    unsigned m_nPush;

    // the button to whose event we connect dynamically
    wxButton *m_btnDynamic;

    // the button used to highlight the event handlers execution order
    MyEvtTestButton *m_testBtn;

    wxWindowRef m_gestureFrame;


    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// Define a custom event handler
class MyEvtHandler : public wxEvtHandler
{
public:
    MyEvtHandler(size_t level) { m_level = level; }

    void OnTest(wxCommandEvent& event)
    {
        wxLogMessage("This is the pushed test event handler #%u", m_level);

        // if we don't skip the event, the other event handlers won't get it:
        // try commenting out this line and see what changes
        event.Skip();
    }

private:
    unsigned m_level;

    wxDECLARE_EVENT_TABLE();
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
    Event_Bind,
    Event_Connect,
    Event_Dynamic,
    Event_Push,
    Event_Pop,
    Event_Custom,
    Event_Test,
    Event_Gesture,
    Event_NewEventClass
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// The event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
wxBEGIN_EVENT_TABLE(MyApp, wxApp)
    // Add a static handler for button Click event in the app
    EVT_BUTTON(MyEvtTestButton::BUTTON_ID, MyApp::OnClickStaticHandlerApp)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyEvtTestButton, wxButton)
    // Add a static handler to this button itself for its own event
    EVT_BUTTON(BUTTON_ID, MyEvtTestButton::OnClickStaticHandler)
wxEND_EVENT_TABLE()

// This can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Event_Quit,  MyFrame::OnQuit)
    EVT_MENU(Event_About, MyFrame::OnAbout)

    EVT_MENU(Event_Bind, MyFrame::OnBind)
    EVT_MENU(Event_Connect, MyFrame::OnConnect)

    EVT_MENU(Event_Custom, MyFrame::OnFireCustom)
    EVT_MENU(Event_Test, MyFrame::OnTest)
    EVT_MENU(Event_Push, MyFrame::OnPushEventHandler)
    EVT_MENU(Event_Pop, MyFrame::OnPopEventHandler)
    EVT_MENU(Event_Gesture, MyFrame::OnGesture)
    EVT_MENU(Event_NewEventClass, MyFrame::OnNewEventClass)

    EVT_UPDATE_UI(Event_Pop, MyFrame::OnUpdateUIPop)

    EVT_MY_CUSTOM_COMMAND(wxID_ANY, MyFrame::OnProcessCustom)

    // the line below would also work if OnProcessCustom() were defined as
    // taking a wxEvent (as required by EVT_CUSTOM) and not wxCommandEvent
    //EVT_CUSTOM(wxEVT_MY_CUSTOM_COMMAND, wxID_ANY, MyFrame::OnProcessCustom)

    // Add a static handler in the parent frame for button event
    EVT_BUTTON(MyEvtTestButton::BUTTON_ID, MyFrame::OnClickStaticHandlerFrame)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyEvtHandler, wxEvtHandler)
    EVT_MENU(Event_Test, MyEvtHandler::OnTest)
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

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Event wxWidgets Sample",
                                 wxPoint(50, 50), wxSize(600, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // Add a dynamic handler at the application level for the test button
    Bind(wxEVT_BUTTON, &MyApp::OnClickDynamicHandlerApp, this,
         MyEvtTestButton::BUTTON_ID);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// This is always the first to handle an event !
int MyApp::FilterEvent(wxEvent& event)
{
    if ( event.GetEventType() == wxEVT_BUTTON &&
            event.GetId() == MyEvtTestButton::BUTTON_ID )
    {
        wxLogMessage("Step 0 in \"How Events are Processed\":\n"
                     "App::FilterEvent");
    }

    return wxApp::FilterEvent(event);
}

void MyApp::OnClickDynamicHandlerApp(wxCommandEvent& event)
{
    wxLogMessage("Step 7, 3 in \"How Events are Processed\":\n"
                 "App::DynamicHandler_InAppTable");

    event.Skip();
}

void MyApp::OnClickStaticHandlerApp(wxCommandEvent& event)
{
    wxLogMessage("Step 7, 4 in \"How Events are Processed\":\n"
                 "App::StaticHandler_InAppTable");

    wxLogMessage("Button click processed, there should be no more messages "
                 "about handling events from the button.\n\n"
                 "The log below shows the order in which the handlers "
                 "were executed.");
    wxLog::FlushActive();

    event.Skip();
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    SetIcon(wxICON(sample));

    // init members
    m_nPush = 0;
    m_btnDynamic = NULL;

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Event_About, "&About\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Event_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuEvent = new wxMenu;
    menuEvent->AppendCheckItem(Event_Bind, "&Bind\tCtrl-B",
                               "Bind or unbind a dynamic event handler");
    menuEvent->AppendCheckItem(Event_Connect, "&Connect\tCtrl-C",
                     "Connect or disconnect the dynamic event handler");
    menuEvent->Append(Event_Dynamic, "&Dynamic event\tCtrl-D",
                      "Dynamic event sample - only works after Connect");
    menuEvent->AppendSeparator();
    menuEvent->Append(Event_Push, "&Push event handler\tCtrl-P",
                      "Push event handler for test event");
    menuEvent->Append(Event_Pop, "P&op event handler\tCtrl-O",
                      "Pop event handler for test event");
    menuEvent->Append(Event_Test, "Test event\tCtrl-T",
                      "Test event processed by pushed event handler");
    menuEvent->AppendSeparator();
    menuEvent->Append(Event_Custom, "Fire c&ustom event\tCtrl-U",
                      "Generate a custom event");
    menuEvent->Append(Event_Gesture, "&Gesture events\tCtrl-G",
                    "Gesture event");
    menuEvent->Append(Event_NewEventClass, "&New wxEvent class demo\tCtrl-N",
                    "Demonstrates a new wxEvent-derived class");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEvent, "&Event");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(3);
    SetStatusText("Welcome to wxWidgets event sample");
    SetStatusText("Dynamic: off", Status_Dynamic);
    SetStatusText("Push count: 0", Status_Push);
#endif // wxUSE_STATUSBAR

    wxPanel * const panel = new wxPanel(this);
    wxSizer * const mainSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer * const sizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSizerFlags centreY(wxSizerFlags().Centre().Border());
    sizer->Add(new wxStaticText(panel, wxID_ANY,
        "This button will only work if its handler is dynamically connected"),
        centreY);
    m_btnDynamic = new wxButton(panel, Event_Dynamic, "&Dynamic button");
    sizer->Add(m_btnDynamic, centreY);

    mainSizer->Add(sizer, 1, wxEXPAND);
    mainSizer->Add(new wxStaticLine(panel), 0, wxEXPAND);
    mainSizer->Add(new wxStaticLine(panel), 0, wxEXPAND);

    m_testBtn = new MyEvtTestButton(panel, "Test Event Handlers Execution Order");

    // After being created, an instance of MyEvtTestButton already has its own
    // event handlers (see class definition);

    // Add a dynamic handler for this button event in the parent frame
    Bind(wxEVT_BUTTON, &MyFrame::OnClickDynamicHandlerFrame, this,
         m_testBtn->GetId());

    // Bind a method of this frame (notice "this" argument!) to the button
    // itself
    m_testBtn->Bind(wxEVT_BUTTON, &MyFrame::OnClickDynamicHandlerButton, this);

    mainSizer->Add(m_testBtn);
    panel->SetSizer(mainSizer);
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
    if ( m_gestureFrame )
        m_gestureFrame->Close(true);
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Event sample shows different ways of using events\n"
                 "(c) 2001-2009 Vadim Zeitlin",
                 "About wxWidgets Event Sample",
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClickStaticHandlerFrame(wxCommandEvent& event)
{
    wxLogMessage("Step 6, 4 in \"How Events are Processed\":\n"
                 "parentWin::StaticHandler_InFrameTable");

    event.Skip();
}

// ----------------------------------------------------------------------------
// dynamic event handling stuff
// ----------------------------------------------------------------------------

void MyFrame::OnClickDynamicHandlerFrame(wxCommandEvent& event)
{
    wxLogMessage("Step 6, 3 in \"How Events are Processed\":\n"
                 "parentWin::DynamicHandler_InFrameTable");

    event.Skip();
}

void MyFrame::OnClickDynamicHandlerButton(wxCommandEvent& event)
{
    wxLogMessage("Step 3 in \"How Events are Processed\":\n"
                 "parentWin::DynamicHandler_InButtonTable");

    event.Skip();
}

void MyFrame::OnDynamic(wxCommandEvent& event)
{
    wxString origin;
    if ( event.GetEventObject() == this )
        origin = "menu item";
    else if ( event.GetEventObject() == m_btnDynamic )
        origin = "button";
    else
        origin = "unknown event source";

    wxMessageBox
    (
        "This message box is shown from the dynamically connected "
        "event handler in response to event generated by " + origin,
        "wxWidgets Event Sample", wxOK | wxICON_INFORMATION, this
    );
}

void MyFrame::OnBind(wxCommandEvent& event)
{
    if ( event.IsChecked() )
    {
        // as we bind directly to the button, there is no need to use an id
        // here: the button will only ever get its own events
        m_btnDynamic->Bind(wxEVT_BUTTON, &MyFrame::OnDynamic,
                           this);

        // but we do need the id for the menu command as the frame gets all of
        // them
        Bind(wxEVT_MENU, &MyFrame::OnDynamic, this,
             Event_Dynamic);
    }
    else // disconnect
    {
        m_btnDynamic->Unbind(wxEVT_BUTTON,
                             &MyFrame::OnDynamic, this);
        Unbind(wxEVT_MENU, &MyFrame::OnDynamic, this,
               Event_Dynamic);
    }

    UpdateDynamicStatus(event.IsChecked());
}

void MyFrame::OnConnect(wxCommandEvent& event)
{
    if ( event.IsChecked() )
    {
        m_btnDynamic->Bind(wxEVT_BUTTON, &MyFrame::OnDynamic, this);
        Bind(wxEVT_MENU, &MyFrame::OnDynamic, this, Event_Dynamic);
    }
    else // disconnect
    {
        m_btnDynamic->Unbind(wxEVT_BUTTON, &MyFrame::OnDynamic, this);
        Unbind(wxEVT_MENU, &MyFrame::OnDynamic, this, Event_Dynamic);
    }

    UpdateDynamicStatus(event.IsChecked());
}

// ----------------------------------------------------------------------------
// push/pop event handlers support
// ----------------------------------------------------------------------------

void MyFrame::OnPushEventHandler(wxCommandEvent& WXUNUSED(event))
{
    PushEventHandler(new MyEvtHandler(++m_nPush));

#if wxUSE_STATUSBAR
    SetStatusText(wxString::Format("Push count: %u", m_nPush), Status_Push);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnPopEventHandler(wxCommandEvent& WXUNUSED(event))
{
    wxCHECK_RET( m_nPush, "this command should be disabled!" );

    PopEventHandler(true /* delete handler */);
    m_nPush--;

#if wxUSE_STATUSBAR
    SetStatusText(wxString::Format("Push count: %u", m_nPush), Status_Push);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnGesture(wxCommandEvent& WXUNUSED(event))
{
    if ( m_gestureFrame )
    {
        m_gestureFrame->Raise();
    }
    else
    {
        m_gestureFrame = new MyGestureFrame();
        m_gestureFrame->Show(true);
    }
}

void MyFrame::OnNewEventClass(wxCommandEvent& WXUNUSED(event))
{
    MyChessBoardDialog dlg(this);

    dlg.ShowModal();
}

void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("This is the test event handler in the main frame");
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
    wxLogMessage("Got a custom event!");
}
