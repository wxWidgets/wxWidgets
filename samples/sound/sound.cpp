/////////////////////////////////////////////////////////////////////////////
// Name:        sound.cpp
// Purpose:     Example of sound playing in wxWidgets
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/01/29
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vaclav Salvik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
    #include "wx/icon.h"
    #include "wx/textctrl.h"
    #include "wx/filedlg.h"
#endif

#include "wx/sound.h"
#include "wx/numdlg.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "../sample.xpm"
#endif

#define WAV_FILE _T("doggrowl.wav")

// ----------------------------------------------------------------------------
// private classes
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
    ~MyFrame() { delete m_sound; }

    // event handlers (these functions should _not_ be virtual)
    void OnPlaySync(wxCommandEvent& event);
    void OnPlayAsync(wxCommandEvent& event);
    void OnPlayAsyncOnStack(wxCommandEvent& event);
    void OnPlayLoop(wxCommandEvent& event);

    void OnSelectFile(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void NotifyUsingFile(const wxString& name);
    

private:
    wxSound*    m_sound;
    wxString    m_soundFile;
    wxTextCtrl* m_tc;
    
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Sound_Quit = wxID_EXIT,
    Sound_About = wxID_ABOUT,
    Sound_PlaySync = wxID_HIGHEST + 1,
    Sound_PlayAsync,
    Sound_PlayAsyncOnStack,
    Sound_PlayLoop,
    Sound_SelectFile
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Sound_SelectFile,       MyFrame::OnSelectFile)
    EVT_MENU(Sound_Quit,             MyFrame::OnQuit)
    EVT_MENU(Sound_About,            MyFrame::OnAbout)
    EVT_MENU(Sound_PlaySync,         MyFrame::OnPlaySync)
    EVT_MENU(Sound_PlayAsync,        MyFrame::OnPlayAsync)
    EVT_MENU(Sound_PlayAsyncOnStack, MyFrame::OnPlayAsyncOnStack)
    EVT_MENU(Sound_PlayLoop,         MyFrame::OnPlayLoop)
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
    MyFrame *frame = new MyFrame(_T("wxWidgets Sound Sample"));

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
       : wxFrame(NULL, wxID_ANY, title)
{
    m_sound = NULL;
    m_soundFile = WAV_FILE;

    // set the frame icon
    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *playMenu = new wxMenu;
    helpMenu->Append(Sound_About, _T("&About...\tF1"), _T("Show about dialog"));
    menuFile->Append(Sound_SelectFile, _T("&Select WAV file"), _T("Select a new wav file to play"));
    menuFile->Append(Sound_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));
    playMenu->Append(Sound_PlaySync, _T("Play sound &synchronously"));
    playMenu->Append(Sound_PlayAsync, _T("Play sound &asynchronously"));
    playMenu->Append(Sound_PlayAsyncOnStack, _T("Play sound asynchronously (&object on stack)"));
    playMenu->Append(Sound_PlayLoop, _T("&Loop sound"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(playMenu, _T("&Play"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_tc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                          wxDefaultPosition, wxDefaultSize,
                          wxTE_MULTILINE|wxTE_READONLY);
    NotifyUsingFile(m_soundFile);
}



void MyFrame::NotifyUsingFile(const wxString& name)
{
    wxString msg;
    msg << _T("Using sound file: ") << name << _T("\n");
    m_tc->AppendText(msg);
}


// event handlers


void MyFrame::OnSelectFile(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dlg(this, _T("Choose a sound file"),
                     wxEmptyString, wxEmptyString,
                     _T("WAV files (*.wav)|*.wav"), wxOPEN|wxCHANGE_DIR);
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_soundFile = dlg.GetPath();
        delete m_sound;
        m_sound = NULL;
        NotifyUsingFile(m_soundFile);
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnPlaySync(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor busy;
    if (!m_sound)
        m_sound = new wxSound(m_soundFile);
    if (m_sound->IsOk())
        m_sound->Play(wxSOUND_SYNC);
}

void MyFrame::OnPlayAsync(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor busy;
    if (!m_sound)
        m_sound = new wxSound(m_soundFile);
    if (m_sound->IsOk())
        m_sound->Play(wxSOUND_ASYNC);
}

void MyFrame::OnPlayAsyncOnStack(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor busy;
    wxSound snd(m_soundFile);
    if (snd.IsOk())
        snd.Play(wxSOUND_ASYNC);
}

void MyFrame::OnPlayLoop(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor busy;
    if (!m_sound)
        m_sound = new wxSound(m_soundFile);
    if (m_sound->IsOk())
        m_sound->Play(wxSOUND_ASYNC | wxSOUND_LOOP);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the sound sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About"), wxOK | wxICON_INFORMATION, this);
}
