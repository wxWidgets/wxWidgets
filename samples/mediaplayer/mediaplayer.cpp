///////////////////////////////////////////////////////////////////////////////
// Name:        mediaplayer.cpp
// Purpose:     wxMediaCtrl sample
// Author:      Ryan Norton
// Modified by:
// Created:     11/10/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MediaPlayer
//
// This is a simple example of how to use all the funtionality of
// the wxMediaCtrl class in wxWidgets.
//
// To use this sample, simply select Open File from the file menu,
// select the file you want to play - and MediaPlayer will play the file,
// showing video if neccessary.
//
// You can select one of the menu options, or move the slider around
// to manipulate what is playing.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ============================================================================
// Definitions
// ============================================================================

// ----------------------------------------------------------------------------
// Pre-compiled header stuff
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

#include "wx/mediactrl.h"   //for wxMediaCtrl
#include "wx/filedlg.h"     //for opening files from OpenFile
#include "wx/slider.h"      //for a slider for seeking within media
#include "wx/sizer.h"       //for positioning controls/wxBoxSizer
#include "wx/timer.h"       //timer for updating status bar
#include "wx/textdlg.h"     //for getting user text from OpenURL

// ----------------------------------------------------------------------------
// Bail out if the user doesn't want one of the
// things we need
// ----------------------------------------------------------------------------

#if !wxUSE_GUI
#error "This is a GUI sample"
#endif

#if !wxUSE_MEDIACTRL || !wxUSE_MENUS || !wxUSE_SLIDER || !wxUSE_TIMER
#error "menus, slider, mediactrl, and timers must all enabled for this sample!"
#endif

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// Enumurations
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    wxID_LOOP = 1,
    wxID_OPENFILE,
    wxID_PLAY,
    wxID_PAUSE,
//    wxID_STOP,   [built-in to wxWidgets]
//    wxID_ABOUT,  [built-in to wxWidgets]
//    wxID_EXIT,   [built-in to wxWidgets]

    // id for our slider
    wxID_SLIDER,

    // id for our wxMediaCtrl
    wxID_MEDIACTRL
};

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    // Ctor/Dtor
    MyFrame(const wxString& title);
    ~MyFrame();

    // Menu event handlers
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLoop(wxCommandEvent& event);

    void OnOpenFile(wxCommandEvent& event);
    void OnOpenURL(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);

    // Slider event handlers
    void OnSeek(wxCommandEvent& event);

    // Media event handlers
    void OnMediaStop(wxMediaEvent& event);

private:
    // Rebuild base status string (see Implementation)
    void ResetStatus();

    wxMediaCtrl* m_mediactrl;   //Our media control
    wxSlider* m_slider;         //The slider below our media control
    class MyTimer* m_timer;     //Timer to write info to status bar
    wxString m_basestatus;      //Base status string (see ResetStatus())
    int m_nLoops;                //Counter, incremented each time media loops

    // So that mytimer can access MyFrame's members
    friend class MyTimer;
};

// ----------------------------------------------------------------------------
// MyTimer
// ----------------------------------------------------------------------------

class MyTimer : public wxTimer
{
public:
    //Ctor
    MyTimer(MyFrame* frame) {m_frame = frame;}

    //Called each time the timer's timeout expires
    void Notify();

    MyFrame* m_frame;       //The MyFrame
};

// ============================================================================
//
// Implementation
//
// ============================================================================

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// [Functions]
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxGetMediaStateText
//
// Converts a wxMediaCtrl state into something useful that we can display
// to the user
// ----------------------------------------------------------------------------
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// MyApp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// This sets up this wxApp as the global wxApp that gui calls in wxWidgets
// use.  For example, if you were to be in windows and use a file dialog,
// wxWidgets would use wxTheApp->GetHInstance() which would get the instance
// handle of the application.  These routines in wx _DO NOT_ check to see if
// the wxApp exists, and thus will crash the application if you try it.
//
// IMPLEMENT_APP does this, and also implements the platform-specific entry
// routine, such as main or WinMain().  Use IMPLEMENT_APP_NO_MAIN if you do
// not desire this behavior.
// ----------------------------------------------------------------------------
IMPLEMENT_APP(MyApp)


// ----------------------------------------------------------------------------
// MyApp::OnInit
//
// Where execution starts - akin to a main or WinMain.
// 1) Create the frame and show it to the user
// 2) return true specifying that we want execution to continue past OnInit
// ----------------------------------------------------------------------------
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(_T("MediaPlayer wxWidgets Sample"));
    frame->Show(true);

    return true;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// MyFrame
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// MyFrame Constructor
//
// 1) Create our menus
// 2) Create our controls and add them to some sizers
// 3) Create our status bar
// 4) Connect our events
// 5) Start our timer
// ----------------------------------------------------------------------------
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    //
    //  Create Menus
    //
    wxMenu *menuFile = new wxMenu;

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT,
                     _T("&About...\tF1"),
                     _T("Show about dialog"));

    menuFile->Append(wxID_OPENFILE, _T("&Open File"), _T("Open a File"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_PLAY, _T("&Play"), _T("Resume playback"));
    menuFile->Append(wxID_PAUSE, _T("P&ause"), _T("Pause playback"));
    menuFile->Append(wxID_STOP, _T("&Stop"), _T("Stop playback"));
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(wxID_LOOP,
                              _T("&Loop"),
                              _T("Loop Selected Media"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT,
                     _T("E&xit\tAlt-X"),
                     _T("Quit this program"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

    //
    //  Create and attach the first/main sizer
    //
    wxBoxSizer* vertsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(vertsizer);
    this->SetAutoLayout(true);

    //
    //  Create our media control
    //
    m_mediactrl = new wxMediaCtrl(this, wxID_MEDIACTRL);
    vertsizer->Add(m_mediactrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    //
    //  Create our slider
    //
    m_slider = new wxSlider(this, wxID_SLIDER, 0, //init
                            0, //start
                            0, //end
                            wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL );
    vertsizer->Add(m_slider, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);


    //
    //  Create the second sizer which will position things
    //  vertically -
    //
    //  -------Menu----------
    //  [m_mediactrl]
    //
    //  [m_slider]
    //
    wxBoxSizer* horzsizer = new wxBoxSizer(wxHORIZONTAL);
    vertsizer->Add(horzsizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    //
    //  Create our status bar
    //
#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    ResetStatus();
    SetStatusText(m_basestatus);
#endif // wxUSE_STATUSBAR

    //
    //  Connect events.
    //
    //  There are two ways in wxWidgets to use events -
    //  Message Maps and Connections.
    //
    //  Message Maps are implemented by putting
    //  DECLARE_MESSAGE_MAP in your wxEvtHandler-derived
    //  class you want to use for events, such as MyFrame.
    //
    //  Then after your class declaration you put
    //  BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    //  EVT_XXX(XXX)...
    //  END_EVENT_TABLE()
    //
    //  Where MyFrame is the class with the DECLARE_MESSAGE_MAP
    //  in it.  EVT_XXX(XXX) are each of your handlers, such
    //  as EVT_MENU for menu events and the XXX inside
    //  is the parameters to the event macro - in the case
    //  of EVT_MENU the menu id and then the function to call.
    //
    //  However, with wxEvtHandler::Connect you can avoid a
    //  global message map for your class and those annoying
    //  macros.  You can also change the context in which
    //  the call the handler (more later).
    //
    //  The downside is that due to the limitation that
    //  wxWidgets doesn't use templates in certain areas,
    //  You have to triple-cast the event function.
    //
    //  There are five parameters to wxEvtHandler::Connect -
    //
    //  The first is the id of the instance whose events
    //  you want to handle - i.e. a menu id for menus,
    //  a control id for controls (wxControl::GetId())
    //  and so on.
    //
    //  The second is the event id.  This is the same
    //  as the message maps (EVT_MENU) except prefixed
    //  with "wx" (wxEVT_MENU).
    //
    //  The third is the function handler for the event -
    //  You need to cast it to the specific event handler
    //  type, then to a wxEventFunction, then to a
    //  wxObjectEventFunction - I.E.
    //  (wxObjectEventFunction)(wxEventFunction)
    //  (wxCommandEventFunction) &MyFrame::MyHandler
    //
    //  The fourth is an optional userdata param -
    //  this is of historical relevance only and is
    //  there only for backwards compatability.
    //
    //  The fifth is the context in which to call the
    //  handler - by default (this param is optional)
    //  this.  For example in your event handler
    //  if you were to call "this->MyFunc()"
    //  it would literally do this->MyFunc.  However,
    //  if you were to pass myHandler as the fifth
    //  parameter, for instance, you would _really_
    //  be calling myHandler->MyFunc, even though
    //  the compiler doesn't really know it.
    //

    //
    // Menu events
    //
    this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnQuit);

    this->Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnAbout);

    this->Connect(wxID_LOOP, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnLoop);

    this->Connect(wxID_OPENFILE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnOpenFile);

    this->Connect(wxID_PLAY, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnPlay);

    this->Connect(wxID_PAUSE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnPause);

    this->Connect(wxID_STOP, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnStop);


    //
    // Slider events
    //
    this->Connect(wxID_SLIDER, wxEVT_COMMAND_SLIDER_UPDATED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnSeek);

    //
    // Media Control events
    //
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_STOP,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxMediaEventFunction) &MyFrame::OnMediaStop);

    //
    // End of Events
    //

    //
    //  Set our loop counter to 0
    //
    m_nLoops = 0;

    //
    //  Create a timer to update our status bar
    //
    m_timer = new MyTimer(this);
    m_timer->Start(100);
}

// ----------------------------------------------------------------------------
// MyFrame Destructor
//
// 1) Deletes child objects implicitly
// 2) Delete our timer explicitly
// ----------------------------------------------------------------------------
MyFrame::~MyFrame()
{
    delete m_timer;
}

// ----------------------------------------------------------------------------
// MyFrame::ResetStatus
//
// Here we just make a simple status string with some useful info about
// the media that we won't change later - such as the length of the media.
//
// We then append some other info that changes in MyTimer::Notify, then
// set the status bar to this text.
//
// In real applications, you'd want to find a better way to do this,
// such as static text controls (wxStaticText).
//
// We display info here in seconds (wxMediaCtrl uses milliseconds - that's why
// we divide by 1000).
//
// We also reset our loop counter here.
// ----------------------------------------------------------------------------
void MyFrame::ResetStatus()
{
    m_basestatus = wxString::Format(_T("Size(x,y):%i,%i ")
                                    _T("Length(Seconds):%u Speed:%1.1fx"),
    m_mediactrl->GetBestSize().x,
    m_mediactrl->GetBestSize().y,
    (unsigned)((m_mediactrl->GetDuration() / 1000).ToLong()),
    m_mediactrl->GetPlaybackRate()
    );

    m_slider->SetRange(0, (m_mediactrl->GetDuration() / 1000).ToLong());

    m_nLoops = 0;
}

// ----------------------------------------------------------------------------
// MyFrame::OnQuit
//
// Called from file->quit.
// Closes this application.
// ----------------------------------------------------------------------------
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

// ----------------------------------------------------------------------------
// MyFrame::OnAbout
//
// Called from help->about.
// Gets some info about this application.
// ----------------------------------------------------------------------------
void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is a test of wxMediaCtrl.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxMediaCtrl test"), wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// MyFrame::OnLoop
//
// Called from file->loop.
// Changes the state of whether we want to loop or not.
// ----------------------------------------------------------------------------
void MyFrame::OnLoop(wxCommandEvent& WXUNUSED(event))
{
    m_mediactrl->Loop( !m_mediactrl->IsLooped() );
}

// ----------------------------------------------------------------------------
// MyFrame::OnOpenFile
//
// Called from file->openfile.
// Opens and plays a media file
// ----------------------------------------------------------------------------
void MyFrame::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fd(this);

    if(fd.ShowModal() == wxID_OK)
    {
        if( !m_mediactrl->Load(fd.GetPath()) )
            wxMessageBox(wxT("Couldn't load file!"));

        if( !m_mediactrl->Play() )
            wxMessageBox(wxT("Couldn't play movie!"));

        ResetStatus();
    }
}

// ----------------------------------------------------------------------------
// MyFrame::OnPlay
//
// Called from file->play.
// Resumes the media if it is paused or stopped.
// ----------------------------------------------------------------------------
void MyFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->Play() )
        wxMessageBox(wxT("Couldn't play movie!"));
}

// ----------------------------------------------------------------------------
// MyFrame::OnPause
//
// Called from file->pause.
// Pauses the media in-place.
// ----------------------------------------------------------------------------
void MyFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->Pause() )
        wxMessageBox(wxT("Couldn't pause movie!"));
}

// ----------------------------------------------------------------------------
// MyFrame::OnStop
//
// Called from file->stop.
// Where it stops depends on whether you can seek in the
// media control or not - if you can it stops and seeks to the beginning,
// otherwise it will appear to be at the end - but it will start over again
// when Play() is called
// ----------------------------------------------------------------------------
void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->Stop() )
        wxMessageBox(wxT("Couldn't stop movie!"));
}

// ----------------------------------------------------------------------------
// MyFrame::OnSeek
//
// Called from file->seek.
// Called when the user moves the slider -
// seeks to a position within the media
// ----------------------------------------------------------------------------
void MyFrame::OnSeek(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->SetPosition( m_slider->GetValue() * 1000 ) )
        wxMessageBox(wxT("Couldn't seek in movie!"));
}

// ----------------------------------------------------------------------------
// MyFrame::OnMediaStop
//
// Called when the media is about to stop playing.
// Here we just increase our loop counter
// ----------------------------------------------------------------------------
void MyFrame::OnMediaStop(wxMediaEvent& WXUNUSED(event))
{
    ++m_nLoops;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// MyTimer
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// MyTimer::Notify
//
// 1) Update our slider with the position were are in in the media
// 2) Update our status bar with the base text from MyFrame::ResetStatus,
//    append some non-static (changing) info to it, then set the
//    status bar text to that result
// ----------------------------------------------------------------------------
void MyTimer::Notify()
{
    long lPosition = (m_frame->m_mediactrl->GetPosition() / 1000).ToLong();
    m_frame->m_slider->SetValue(lPosition);

#if wxUSE_STATUSBAR
    m_frame->SetStatusText(wxString::Format(
                     _T("%s Pos:%u State:%s Loops:%i"),
                     m_frame->m_basestatus.c_str(),
                     (unsigned int)lPosition,
                     wxGetMediaStateText(m_frame->m_mediactrl->GetState()),
                     m_frame->m_nLoops
                                            )
                           );
#endif
}

//
// End of MediaPlayer sample
//
