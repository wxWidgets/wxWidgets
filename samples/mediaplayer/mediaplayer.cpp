/////////////////////////////////////////////////////////////////////////////
// Name:        mediaplayer.cpp
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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "wx/mediactrl.h"   //for wxMediaCtrl
#include "wx/filedlg.h"     //for opening files from OpenFile
#include "wx/slider.h"      //for a slider for seeking within media
#include "wx/sizer.h"       //for positioning controls/wxBoxSizer
#include "wx/timer.h"       //timer for updating status bar
#include "wx/textdlg.h"     //for getting user text from OpenURL


#if !wxUSE_MEDIACTRL
#error "wxUSE_MEDIACTRL must be enabled to use this sample!"
#endif

// ----------------------------------------------------------------------------
// Declarations
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

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
    void ResetStatus();

    wxMediaCtrl* m_mediactrl;
    wxSlider* m_slider;
    wxBoxSizer* m_sizer;
    class MyTimer* m_timer;
    friend class MyTimer;
    wxString m_basestatus;

    bool m_bLoop;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

//
//ResetStatus
//-----------
//Here we just make a simple status string
//with some useful info about the media
//We display info here in seconds (wxMediaCtrl
//uses milliseconds - that's why we divide by 1000)
//
void MyFrame::ResetStatus()
{
    m_basestatus = wxString::Format(_T("Size(x,y):%i,%i Length(Seconds):%u Speed:%1.1fx"),
    m_mediactrl->GetBestSize().x,
    m_mediactrl->GetBestSize().y,
    (unsigned)(m_mediactrl->GetDuration() / 1000),
    m_mediactrl->GetPlaybackRate()
    );

    m_slider->SetRange(0, m_mediactrl->GetDuration() / 1000);
}

//
//wxGetMediaStateText
//-------------------
//Converts a wxMediaCtrl state into something
//useful that we can display
//
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

    //
    //Notify
    //-----------
    //Updates the main frame's status bar with the current
    //position within the media and state the media is in
    //
    void Notify()
    {
        long lPosition = m_frame->m_mediactrl->GetPosition() / 1000;
        m_frame->m_slider->SetValue(lPosition);

        m_frame->SetStatusText(wxString::Format(_T("%s Pos:%u State:%s"),
                                    m_frame->m_basestatus.c_str(),
                                    (unsigned int)lPosition,
                                    wxGetMediaStateText(m_frame->m_mediactrl->GetState())
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
    Minimal_Loop,
    Minimal_OpenFile,
    Minimal_Play,
    Minimal_Pause,
    Minimal_Stop,
    Minimal_About = wxID_ABOUT,

    // id for our slider
    Minimal_Slider = 1,

    // id for our wxMediaCtrl
    Minimal_Media
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    //Menu events
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_Loop, MyFrame::OnLoop)
    EVT_MENU(Minimal_OpenFile, MyFrame::OnOpenFile)
    EVT_MENU(Minimal_Play, MyFrame::OnPlay)
    EVT_MENU(Minimal_Pause, MyFrame::OnPause)
    EVT_MENU(Minimal_Stop, MyFrame::OnStop)

    //Slider events
    EVT_SLIDER(Minimal_Slider, MyFrame::OnSeek)

    //wxMediaCtrl events
    EVT_MEDIA_FINISHED(Minimal_Media, MyFrame::OnMediaFinished)
END_EVENT_TABLE()

//main/WinMain()
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(_T("Minimal wxWidgets App"));
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

//
//MyFrame
//-------
//Creates our menus and controls
//
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title), m_timer(NULL)
{
    //
    //  Create Menus
    //
#if wxUSE_MENUS
    wxMenu *menuFile = new wxMenu;

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(Minimal_OpenFile, _T("&Open File"), _T("Open a File"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Play, _T("&Play"), _T("Resume playback"));
    menuFile->Append(Minimal_Pause, _T("P&ause"), _T("Pause playback"));
    menuFile->Append(Minimal_Stop, _T("&Stop"), _T("Stop playback"));
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(Minimal_Loop, _T("&Loop"), _T("Loop Selected Media"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    //
    //  Create and attach the first/main sizer
    //

    m_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(m_sizer);
    this->SetAutoLayout(true);

    //
    //  Create our media control
    //

    m_mediactrl = new wxMediaCtrl(this, Minimal_Media, wxT(""));
    m_sizer->Add(m_mediactrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    //
    //  Create our slider
    //

    m_slider = new wxSlider(this, Minimal_Slider, 0, //init
                            0, //start
                            0, //end
                            wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL );
    m_sizer->Add(m_slider, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);


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
    m_sizer->Add(horzsizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    //
    //  We arn't looping initially
    //

    m_bLoop = false;

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
    //  Create a timer to update our status bar
    //

    m_timer = new MyTimer(this);
    m_timer->Start(100);
}

//
//~MyFrame
//--------
//Deletes child objects implicitly and our timer explicitly
//
MyFrame::~MyFrame()
{
    delete m_timer;
}

//
//OnQuit
//------
//Called from file->quit.
//Closes this application.
//
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

//
//OnAbout
//-------
//Called from help->about.
//Gets some info about this application.
//
void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is a test of wxMediaCtrl.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxMediaCtrl test"), wxOK | wxICON_INFORMATION, this);
}

//
//OnLoop
//------
//Called from file->loop.
//Changes the state of whether we want to loop or not.
//
void MyFrame::OnLoop(wxCommandEvent& WXUNUSED(event))
{
    m_bLoop = !m_bLoop;
}

//
//OnOpenFile
//----------
//Called from file->openfile.
//Opens and plays a media file
//
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

//
//OnPlay
//------
//Called from file->play.
//Resumes the media if it is paused or stopped.
//
void MyFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->Play() )
        wxMessageBox(wxT("Couldn't play movie!"));
}

//
//OnPause
//-------
//Called from file->pause.
//Pauses the media in-place.
//
void MyFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->Pause() )
        wxMessageBox(wxT("Couldn't pause movie!"));
}

//
//OnStop
//------
//Called from file->stop.
//Where it stops depends on whether you can seek in the
//media control or not - if you can it stops and seeks to the beginning,
//otherwise it will appear to be at the end - but it will start over again
//when play() is called
//
void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->Stop() )
        wxMessageBox(wxT("Couldn't stop movie!"));
}

//
//OnSeek
//------
//Called from file->seek.
//Called when the user moves the slider -
//seeks to a position within the media
//
void MyFrame::OnSeek(wxCommandEvent& WXUNUSED(event))
{
    if( !m_mediactrl->SetPosition( m_slider->GetValue() * 1000 ) )
        wxMessageBox(wxT("Couldn't seek in movie!"));
}

//
//OnMediaFinished
//---------------
//Called when the media stops playing.
//Here we loop it if the user wants to (has been selected from file menu)
//
void MyFrame::OnMediaFinished(wxMediaEvent& WXUNUSED(event))
{
    if(m_bLoop)
    {
        if ( !m_mediactrl->Play() )
            wxMessageBox(wxT("Couldn't loop movie!"));
    }
}
