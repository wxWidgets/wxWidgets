/////////////////////////////////////////////////////////////////////////////
// Name:        mmboard.cpp
// Purpose:     Multimedia Library sample
// Author:      Guilhem Lavaux (created from minimal by J. Smart)
// Modified by:
// Created:     13/02/2000
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "mmboard.cpp"
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

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// include multimedia classes
#include "sndbase.h"
#ifdef __WIN32__
    #include "sndwin.h"
#endif
#ifdef __UNIX__
    #include "sndoss.h"
    #include "sndesd.h"
#endif

#include "wx/statline.h"
#include "wx/stattext.h"

// include personnal classes
#include "mmboard.h"
#include "mmbman.h"

#include "play.xpm"
#include "stop.xpm"
#include "eject.xpm"
#include "pause.xpm"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Main Multimedia Board frame
class MMBoardFrame : public wxFrame
{
public:
  // ctor(s)
  MMBoardFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  // dtor
  ~MMBoardFrame();

  // event handlers
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnPlay(wxCommandEvent& event);
  void OnStop(wxCommandEvent& event);
  void OnPause(wxCommandEvent& event);
  void OnRefreshInfo(wxEvent& event);

  void OpenVideoWindow();
  void CloseVideoWindow();

private:
  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  
private:
  void UpdateMMedInfo(); 
  void UpdateInfoText();

  MMBoardFile *m_opened_file;

  wxSlider *m_positionSlider;
  wxBitmapButton *m_playButton, *m_pauseButton, *m_stopButton, *m_ejectButton;
  wxStaticText *m_fileType, *m_infoText;
  wxWindow *m_video_window;

  wxPanel *m_panel;
  wxSizer *m_sizer;

  wxTimer *m_refreshTimer;

};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MMBoard_Quit = 1,
    MMBoard_Open,
    MMBoard_About,
    MMBoard_PositionSlider,
    MMBoard_PlayButton,
    MMBoard_PauseButton,
    MMBoard_ResumeButton,
    MMBoard_StopButton,
    MMBoard_EjectButton,
    MMBoard_RefreshInfo
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MMBoardFrame, wxFrame)
  EVT_MENU(MMBoard_Quit,  MMBoardFrame::OnQuit)
  EVT_MENU(MMBoard_About, MMBoardFrame::OnAbout)
  EVT_MENU(MMBoard_Open, MMBoardFrame::OnOpen)
  EVT_BUTTON(MMBoard_PlayButton, MMBoardFrame::OnPlay)
  EVT_BUTTON(MMBoard_StopButton, MMBoardFrame::OnStop)
  EVT_BUTTON(MMBoard_PauseButton, MMBoardFrame::OnPause)
  EVT_CUSTOM(wxEVT_TIMER, MMBoard_RefreshInfo, MMBoardFrame::OnRefreshInfo)
END_EVENT_TABLE()

// ---------------------------------------------------------------------------
// Main board application launcher
// ---------------------------------------------------------------------------

IMPLEMENT_APP(MMBoardApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MMBoardApp::OnInit()
{
    // create the main application window
    MMBoardFrame *frame = new MMBoardFrame("Multimedia Board",
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    m_caps = TestMultimediaCaps();

    if (!m_caps) {
      wxMessageBox("Your system has no multimedia capabilities. We are exiting now.", "Major error !", wxOK | wxICON_ERROR, NULL);
      return FALSE;
    }

    wxString msg;
    msg.Printf("Detected : %s%s%s", (m_caps & MM_SOUND_OSS) ? "OSS " : "",
	                            (m_caps & MM_SOUND_ESD) ? "ESD " : "",
	                            (m_caps & MM_SOUND_WIN) ? "WIN" : "");

    wxMessageBox(msg, "Good !", wxOK | wxICON_INFORMATION, NULL);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

wxUint8 MMBoardApp::TestMultimediaCaps()
{
  wxSoundStream *dev;
  wxUint8 caps;

  caps = 0;

#ifdef __UNIX__
  // We test the OSS (Open Sound System) support.

  dev = new wxSoundStreamOSS();
  if (dev->GetError() == wxSOUND_NOERR) 
    caps |= MM_SOUND_OSS;
  delete dev;

  // We now test the ESD support

  dev = new wxSoundStreamESD();
  if (dev->GetError() == wxSOUND_NOERR)
    caps |= MM_SOUND_ESD;
  delete dev;
#endif

#ifdef __WIN32__
  // We test the Windows sound support.

  dev = new wxSoundStreamWin();
  if (dev->GetError() == wxSOUND_NOERR)
    caps |= MM_SOUND_WIN;
  delete dev;
#endif

  return caps;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MMBoardFrame::MMBoardFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = MMBoard_About;
#endif

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(MMBoard_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    menuFile->Append(MMBoard_Open, _T("&Open\tAlt-O"), _T("Open file"));
    menuFile->AppendSeparator();
    menuFile->Append(MMBoard_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(3);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR

    // Misc variables
    m_opened_file = NULL;

    m_panel = new wxPanel(this, -1);

    // Initialize main slider
    m_positionSlider = new wxSlider( m_panel, MMBoard_PositionSlider, 0, 0, 60,
				     wxDefaultPosition, wxSize(300, -1),
				     wxSL_HORIZONTAL | wxSL_AUTOTICKS);
    m_positionSlider->SetPageSize(60);  // 60 secs

    // Initialize info panel
    wxPanel *infoPanel = new wxPanel( m_panel, -1);
    infoPanel->SetBackgroundColour(*wxBLACK);
    infoPanel->SetForegroundColour(*wxWHITE);

    wxBoxSizer *infoSizer = new wxBoxSizer(wxVERTICAL);

    m_fileType = new wxStaticText(infoPanel, -1, _T(""));
    wxStaticLine *line = new wxStaticLine(infoPanel, -1);
    m_infoText = new wxStaticText(infoPanel, -1, "");

    UpdateInfoText();

    infoSizer->Add(m_fileType, 0, wxGROW | wxALL, 1);
    infoSizer->Add(line, 0, wxGROW | wxCENTRE, 20);
    infoSizer->Add(m_infoText, 0, wxGROW | wxALL, 1);
 
    infoPanel->SetSizer(infoSizer);
    infoPanel->SetAutoLayout(TRUE);

    // Bitmap button panel
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxBitmap *play_bmp = new wxBitmap(play_back_xpm);
    wxBitmap *stop_bmp = new wxBitmap(stop_back_xpm);
    wxBitmap *eject_bmp = new wxBitmap(eject_xpm);
    wxBitmap *pause_bmp = new wxBitmap(pause_xpm);

    m_playButton = new wxBitmapButton(m_panel, MMBoard_PlayButton, *play_bmp);
    m_playButton->Enable(FALSE);
    m_pauseButton = new wxBitmapButton(m_panel, MMBoard_PauseButton, *pause_bmp);
    m_pauseButton->Enable(FALSE);
    m_stopButton = new wxBitmapButton(m_panel, MMBoard_StopButton, *stop_bmp);
    m_stopButton->Enable(FALSE);
    m_ejectButton = new wxBitmapButton(m_panel, MMBoard_EjectButton, *eject_bmp);
    m_ejectButton->Enable(FALSE);
    
    buttonSizer->Add(m_playButton, 0, wxALL, 2);
    buttonSizer->Add(m_pauseButton, 0, wxALL, 2);  
    buttonSizer->Add(m_stopButton, 0, wxALL, 2);
    buttonSizer->Add(m_ejectButton, 0, wxALL, 2);

    // Top sizer
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(new wxStaticLine(m_panel, -1), 0, wxGROW | wxCENTRE, 0);
    m_sizer->Add(m_positionSlider, 0, wxCENTRE | wxGROW | wxALL, 2);
    m_sizer->Add(new wxStaticLine(m_panel, -1), 0, wxGROW | wxCENTRE, 0);
    m_sizer->Add(buttonSizer, 0, wxALL, 0);
    m_sizer->Add(new wxStaticLine(m_panel, -1), 0, wxGROW | wxCENTRE, 0);
    m_sizer->Add(infoPanel, 1, wxCENTRE | wxGROW, 0);
    
    m_panel->SetSizer(m_sizer);
    m_panel->SetAutoLayout(TRUE);
    m_sizer->Fit(this);
    m_sizer->SetSizeHints(this);

    // Timer
    m_refreshTimer = new wxTimer(this, MMBoard_RefreshInfo);

    // Video window
    m_video_window = NULL;

    // Multimedia file
    m_opened_file = NULL;
}

MMBoardFrame::~MMBoardFrame()
{
  if (m_opened_file)
    delete m_opened_file;

  delete m_refreshTimer;
}

void MMBoardFrame::OpenVideoWindow()
{
  if (m_video_window)
    return;

  m_video_window = new wxWindow(m_panel, -1, wxDefaultPosition, wxSize(400, 400));
  m_video_window->SetBackgroundColour(*wxBLACK);
  m_sizer->Prepend(m_video_window, 0, wxGROW | wxSHRINK | wxCENTRE, 0);

  m_sizer->Fit(this);
}

void MMBoardFrame::CloseVideoWindow()
{
  if (!m_video_window)
    return;

  m_sizer->Remove(m_video_window);
  delete m_video_window;
  m_video_window = NULL;

  m_sizer->Fit(this);
}

// event handlers

void MMBoardFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MMBoardFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("wxWindows Multimedia board v1.0a, wxMMedia v2.0a:\n")
                _T("an example of the capabilities of the wxWindows multimedia classes.\n")
		_T("Copyright 1999, 2000, Guilhem Lavaux.\n"));
 
    wxMessageBox(msg, "About MMBoard", wxOK | wxICON_INFORMATION, this);
}

void MMBoardFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
  wxString selected_file;

  if (m_opened_file) {
    if (!m_opened_file->IsStopped()) {
      wxCommandEvent event2;
      OnStop(event2);
    }
    delete m_opened_file;
  }

  // select a file to be opened
  selected_file = wxLoadFileSelector("multimedia", "*", NULL, this);
  if (selected_file.IsNull())
    return;

  m_opened_file = MMBoardManager::Open(selected_file);

  // Change the range values of the slider.
  MMBoardTime length;

  length = m_opened_file->GetLength();
  m_positionSlider->SetRange(0, length.hours * 3600 + length.minutes * 60 + length.seconds);

  // Update misc info
  UpdateMMedInfo();
 
  SetStatusText(selected_file, 2);

  // Update info text
  UpdateInfoText();

  // Enable a few buttons
  m_playButton->Enable(TRUE);
  m_ejectButton->Enable(TRUE);

  if (m_opened_file->NeedWindow()) {
    OpenVideoWindow();
    m_opened_file->SetWindow(m_video_window);
  } else
    CloseVideoWindow();
}

void MMBoardFrame::UpdateInfoText()
{
  wxString infotext1, infotext2;

  if (m_opened_file) {
    infotext1 = _T("File type:\n\t");
    infotext1 += m_opened_file->GetStringType() + _T("\n");

    infotext2 = _T("File informations:\n\n");
    infotext2 += m_opened_file->GetStringInformation();
  } else {
    infotext1 = _T("File type: \n\tNo file opened");
    infotext2 = _T("File informations:\nNo information\n\n\n\n\n");
  }

  m_fileType->SetLabel(infotext1);
  m_infoText->SetLabel(infotext2);
}

void MMBoardFrame::UpdateMMedInfo()
{
  wxString temp_string;
  MMBoardTime current, length;

  if (m_opened_file) {
    current = m_opened_file->GetPosition();
    length  = m_opened_file->GetLength();
  }

  // We refresh the status bar
  temp_string.Printf("%02d:%02d / %02d:%02d", current.hours * 60 + current.minutes,
		     current.seconds, length.hours * 60 + length.minutes, length.seconds);
  SetStatusText(temp_string, 1);

  // We set the slider position
  m_positionSlider->SetValue(current.hours * 3600 + current.minutes * 60 + current.seconds);
}

// ----------------------------------------------------------------------------
// Playing management, refreshers, ...

void MMBoardFrame::OnRefreshInfo(wxEvent& WXUNUSED(event))
{
  UpdateMMedInfo();

  if (m_opened_file->IsStopped()) {
    m_refreshTimer->Stop();
    m_playButton->Enable(TRUE);
    m_stopButton->Enable(FALSE);
    m_pauseButton->Enable(FALSE);
  }
}

void MMBoardFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
  m_stopButton->Enable(TRUE);
  m_pauseButton->Enable(TRUE);
  m_playButton->Enable(FALSE);

  if (m_opened_file->IsPaused()) {
    m_opened_file->Resume();
    return;
  }

  m_refreshTimer->Start(1000, FALSE);

  m_opened_file->Play();

  m_stopButton->Enable(TRUE);
  m_pauseButton->Enable(TRUE);
  m_playButton->Enable(FALSE);
}

void MMBoardFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
  m_opened_file->Stop();
  m_refreshTimer->Stop();

  m_stopButton->Enable(FALSE);
  m_playButton->Enable(TRUE);

  UpdateMMedInfo();
}

void MMBoardFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
  m_opened_file->Pause();

  m_playButton->Enable(TRUE);
  m_pauseButton->Enable(FALSE);
}
