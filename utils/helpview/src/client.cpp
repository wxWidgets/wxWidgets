/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     Tests helpview in server mode.
// Author:      Julian Smart
// Modified by:
// Created:     2002-10-08
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "client.cpp"
    #pragma interface "client.cpp"
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

#include "wx/ipc.h"
#include "wx/process.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyConnection: public wxConnection
{
public:
    MyConnection();
    ~MyConnection();

    bool OnAdvise(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    bool OnDisconnect();
};

class MyClient: public wxClient
{
public:
    wxConnectionBase *OnMakeConnection();
};

// Define a new application type, each program should derive a class from wxApp
class HelpClientApp : public wxApp
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
class HelpClientFrame : public wxFrame
{
    friend MyClient;
    friend MyConnection;
public:
    // ctor(s)
    HelpClientFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnHelpPage(wxCommandEvent& event);
    void OnHelpContents(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    bool CreateConnection();
private:
    MyConnection*   m_connection;
    MyClient*       m_client;
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

HelpClientFrame* g_MainFrame = NULL;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    HelpClient_Quit = 1,
    HelpClient_HelpPage,
    HelpClient_HelpContents,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(HelpClientFrame, wxFrame)
EVT_MENU(HelpClient_Quit,  HelpClientFrame::OnQuit)
EVT_MENU(HelpClient_HelpPage, HelpClientFrame::OnHelpPage)
EVT_MENU(HelpClient_HelpContents, HelpClientFrame::OnHelpContents)
EVT_CLOSE(HelpClientFrame::OnClose)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. HelpClientApp and
// not wxApp)
IMPLEMENT_APP(HelpClientApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------
// `Main program' equivalent: the program execution "starts" here
bool HelpClientApp::OnInit()
{
    SetVendorName("wxWindows");
    SetAppName("Help Client Demo"); 
    
    // Create the main application window
    HelpClientFrame *frame = new HelpClientFrame("Help Client Demo",
        wxPoint(50, 50), wxSize(150, 50));
    
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
HelpClientFrame::HelpClientFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    g_MainFrame = this;

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    
    menuFile->Append(HelpClient_HelpContents, "&Help Contents");
    menuFile->Append(HelpClient_HelpPage, "Help Page");
    menuFile->AppendSeparator();
    menuFile->Append(HelpClient_Quit, "E&xit");
    
    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_connection = NULL;
    m_client = new MyClient;
}


// event handlers

void HelpClientFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void HelpClientFrame::OnHelpPage(wxCommandEvent& WXUNUSED(event))
{
    if (CreateConnection())
    {
        m_connection->Execute(wxT("Book 1"));
    }
}

void HelpClientFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event))
{
    if (CreateConnection())
    {
        m_connection->Execute(wxT("test.zip"));
    }
}

void HelpClientFrame::OnClose(wxCloseEvent& event)
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;
    }
    delete m_client;

    event.Skip();   
}

bool HelpClientFrame::CreateConnection()
{
    if (m_connection)
        return TRUE;

    wxString cmd;

#if defined(__WXMSW__)
    cmd = "helpview.exe --server test.zip";
#else
    cmd = "./helpview --server test.zip";
#endif

    wxProcess *process = new wxProcess(this);
    int pid = wxExecute( cmd, FALSE, process );
    if( pid <= 0 ) {
        wxMessageBox( "Failed to start server" );
        return FALSE;
    }
    
    // service name (DDE classes) or port number (TCP/IP based classes)
    wxString service = wxT("4242");
    
    // ignored under DDE, host name in TCP/IP based classes
    wxString hostName = wxT("localhost");
    
    int nsleep = 0;
    
    // suppress the log messages from MakeConnection()
    {
        
        wxLogNull nolog;
        m_connection = (MyConnection *)m_client->MakeConnection(hostName, service, wxT("HELP"));
        
        while ( !m_connection)
        {	
            //try every second for a while
            wxSleep(1);
            if( nsleep > 4 ) {
                if ( wxMessageBox("Failed to make connection to server.\nRetry?",
                    "Client Demo Error",
                    wxICON_ERROR | wxYES_NO | wxCANCEL ) != wxYES )
                {           
                    // no server
                    return FALSE;
                }
            }
            nsleep++;
            
            m_connection = (MyConnection *)m_client->MakeConnection(hostName, service, "HELP");
        }
    }

    return (m_connection != NULL);    
}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyConnection;
}

MyConnection::MyConnection()
            : wxConnection()
{
    g_MainFrame->m_connection = this;
}

MyConnection::~MyConnection()
{
    g_MainFrame->m_connection = NULL;
}

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format)
{
    return TRUE;
}

bool MyConnection::OnDisconnect()
{
	g_MainFrame->m_connection = NULL;

    delete this;

    return TRUE;
}

