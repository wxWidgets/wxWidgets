/////////////////////////////////////////////////////////////////////////////
// Name:        mediactrltest.
// Purpose:     wxMediaCtrl sample
// Author:      Ryan Norton
// Modified by:
// Created:     11/10/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
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
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
//    #include "../sample.xpm"
#endif

#include "wx/mediactrl.h"
#include "wx/filedlg.h"
#include "wx/slider.h"
#include "wx/sizer.h"

#include "wx/timer.h"


#if !wxUSE_MEDIACTRL
#error "wxUSE_MEDIACTRL must be enabled to use this sample!"
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
    MyFrame(const wxString& title);
    ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLoop(wxCommandEvent& event);

    void OnOpenFile(wxCommandEvent& event);
    void OnOpenURL(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);

    void OnSeek(wxCommandEvent& event);

    void OnMediaFinished(wxMediaEvent& event);

private:
    void ResetStatus()
    {
        m_basestatus = wxString::Format(_T("Size(x,y):%i,%i Length(Seconds):%u Speed:%1.1fx"), 
        m_movie->GetBestSize().x, 
        m_movie->GetBestSize().y, 
        m_movie->GetDuration() / 1000,
        m_movie->GetPlaybackRate()
        );
        
        m_slider->SetRange(0, m_movie->GetDuration() / 1000);
    }
    
    wxMediaCtrl* m_movie;
    wxSlider* m_slider;
    wxBoxSizer* m_sizer;
    class MyTimer* m_timer;
    friend class MyTimer;
    wxString m_basestatus;

    bool m_bLoop;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

const wxChar* wxGetMediaStateText(int nState)
{
    switch(nState)
    {
        case wxMEDIASTATE_PLAYING:
            return wxT("Playing");
        case wxMEDIASTATE_STOPPED:
            return wxT("Stopped");
        ///case wxMEDIASTATE_PAUSED:
        default:
            return wxT("Paused");
    }
}

class MyTimer : public wxTimer
{
public:
    MyTimer(MyFrame* frame) {m_frame = frame;}

    void Notify()
    {
        long lPosition = m_frame->m_movie->GetPosition() / 1000;
        m_frame->m_slider->SetValue(lPosition);
            

        m_frame->SetStatusText(wxString::Format(_T("%s Pos:%u State:%s"),
                                    m_frame->m_basestatus.c_str(), 
                                    lPosition,
                                    wxGetMediaStateText(m_frame->m_movie->GetState())
                                                )
                               );
    
    }

    MyFrame* m_frame;
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,

    Minimal_Slider = 1,
    Minimal_Media,
    Minimal_Loop,
    Minimal_OpenFile,
    Minimal_OpenURL,
    Minimal_Play,
    Minimal_Pause,
    Minimal_Stop
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_Loop, MyFrame::OnLoop)
    EVT_MENU(Minimal_OpenFile, MyFrame::OnOpenFile)
    EVT_MENU(Minimal_OpenURL, MyFrame::OnOpenURL)
    EVT_MENU(Minimal_Play, MyFrame::OnPlay)
    EVT_MENU(Minimal_Pause, MyFrame::OnPause)
    EVT_MENU(Minimal_Stop, MyFrame::OnStop)
    EVT_SLIDER(Minimal_Slider, MyFrame::OnSeek)
    EVT_MEDIA_FINISHED(Minimal_Media, MyFrame::OnMediaFinished)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
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
    MyFrame *frame = new MyFrame(_T("Minimal wxWidgets App"));

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
       : wxFrame(NULL, wxID_ANY, title), m_timer(NULL)
{
    // set the frame icon
//    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(Minimal_OpenFile, _T("&Open File"), _T("Open a File"));
    menuFile->Append(Minimal_OpenURL, _T("Open &URL"), _T("Open a URL"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Play, _T("&Play"), _T("Resume playback"));
    menuFile->Append(Minimal_Pause, _T("P&ause"), _T("Pause playback"));
    menuFile->Append(Minimal_Stop, _T("&Stop"), _T("Stop playback"));
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(Minimal_Loop, _T("&Loop"), _T("Loop Selected Media"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    m_sizer = new wxBoxSizer(wxVERTICAL); 
    this->SetSizer(m_sizer);
    this->SetAutoLayout(true);

//    m_sizer->SetSizeHints(this);
//    m_sizer->Fit(this);
    
    m_movie = new wxMediaCtrl(this, Minimal_Media, wxT(""));
    m_sizer->Add(m_movie, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_slider = new wxSlider(this, Minimal_Slider, 0, //init
                            0, //start 
                            0, //end 
                            wxDefaultPosition, wxDefaultSize, 
                            wxSL_HORIZONTAL );
    m_sizer->Add(m_slider, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);

    wxBoxSizer* horzsizer = new wxBoxSizer(wxHORIZONTAL);
    m_sizer->Add(horzsizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_bLoop = false;

    m_timer = new MyTimer(this);
    m_timer->Start(100);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    ResetStatus();
    SetStatusText(m_basestatus);
#endif // wxUSE_STATUSBAR
}

MyFrame::~MyFrame()
{
    if (m_timer)
        delete m_timer;
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
    msg.Printf( _T("This is a test of wxMediaCtrl.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxMediaCtrl test"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnLoop(wxCommandEvent& WXUNUSED(event))
{
    m_bLoop = !m_bLoop;
}

void MyFrame::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fd(this);

    if(fd.ShowModal() == wxID_OK)
    {
        if( !m_movie->Load(fd.GetPath()) )
            wxMessageBox(wxT("Couldn't load file!"));

        if( !m_movie->Play() )
            wxMessageBox(wxT("Couldn't play movie!"));            
            
        ResetStatus();
    }
}

#include "wx/textdlg.h"

void MyFrame::OnOpenURL(wxCommandEvent& WXUNUSED(event))
{
    wxString theURL = wxGetTextFromUser(wxT("Enter the URL that has the movie to play"));

    if(!theURL.empty())
    {
        if( !m_movie->Load(wxURI(theURL)) )
            wxMessageBox(wxT("Couldn't load URL!"));

        if( !m_movie->Play() )
            wxMessageBox(wxT("Couldn't play movie!"));            
            
        ResetStatus();
    }
}

void MyFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    if( !m_movie->Play() )
        wxMessageBox(wxT("Couldn't play movie!"));
}

void MyFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
    if( !m_movie->Pause() )
        wxMessageBox(wxT("Couldn't pause movie!"));
}

void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    if( !m_movie->Stop() )
        wxMessageBox(wxT("Couldn't stop movie!"));
}

void MyFrame::OnSeek(wxCommandEvent& WXUNUSED(event))
{
    if( !m_movie->SetPosition( m_slider->GetValue() * 1000 ) )
        wxMessageBox(wxT("Couldn't seek in movie!"));
}

void MyFrame::OnMediaFinished(wxMediaEvent& WXUNUSED(event))
{
    if(m_bLoop)
    {
        if ( !m_movie->SetPosition(0) || !m_movie->Play() )
            wxMessageBox(wxT("Couldn't seek or play to loop movie!"));
    }
}