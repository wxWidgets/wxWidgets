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
// select the file you want to play - and MediaPlayer will play the file in a 
// new notebook page, showing video if neccessary.
//
// You can select one of the menu options, or move the slider around
// to manipulate what is playing.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Known bugs with wxMediaCtrl:
// 
// 1) Certain backends can't play the same media file at the same time (MCI,
//    Cocoa NSMovieView-Quicktime).
// 2) Positioning on Mac Carbon is messed up if put in a sub-control like a 
//    Notebook (like this sample does) on OS versions < 10.2.
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
#include "wx/notebook.h"    //for wxNotebook and putting movies in pages

// Use some stuff that's not part of the current API, such as loading
// media from a URL, etc.
#define wxUSE_UNOFFICIALSTUFF 0

//Libraries for MSVC with optional backends
#ifdef _MSC_VER
    #if wxUSE_DIRECTSHOW
        #pragma comment(lib,"strmiids.lib")
    #endif
    #if wxUSE_QUICKTIME
        #pragma comment(lib,"qtmlClient.lib")
    #endif
#endif

// ----------------------------------------------------------------------------
// Bail out if the user doesn't want one of the
// things we need
// ----------------------------------------------------------------------------

#if !wxUSE_GUI
#error "This is a GUI sample"
#endif

#if !wxUSE_MEDIACTRL || !wxUSE_MENUS || !wxUSE_SLIDER || !wxUSE_TIMER || !wxUSE_NOTEBOOK
#error "menus, slider, mediactrl, notebook, and timers must all be enabled for this sample!"
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
    wxID_OPENFILESAMEPAGE,
    wxID_OPENFILENEWPAGE,
    wxID_OPENURLSAMEPAGE,
    wxID_OPENURLNEWPAGE,
    wxID_CLOSECURRENTPAGE,
    wxID_PLAY,
    wxID_PAUSE,
//    wxID_STOP,   [built-in to wxWidgets]
//    wxID_ABOUT,  [built-in to wxWidgets]
//    wxID_EXIT,   [built-in to wxWidgets]

    // event id for our slider
    wxID_SLIDER,

    // event id for our notebook
    wxID_NOTEBOOK,

    // event id for our wxMediaCtrl
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

    void OnOpenFileSamePage(wxCommandEvent& event);
    void OnOpenFileNewPage(wxCommandEvent& event);
    void OnOpenURLSamePage(wxCommandEvent& event);
    void OnOpenURLNewPage(wxCommandEvent& event);
    void OnCloseCurrentPage(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);

    // Notebook event handlers
    void OnPageChange(wxNotebookEvent& event);

private:
    // Rebuild base status string (see Implementation)
    void ResetStatus();

    // Common open file code
    void OpenFile(bool bNewPage);
    void OpenURL(bool bNewPage);
    
    // Get the media control and slider of current notebook page
    wxMediaCtrl* GetCurrentMediaCtrl();
    wxSlider*    GetCurrentSlider();

    class MyTimer* m_timer;     //Timer to write info to status bar
    wxString m_basestatus;      //Base status string (see ResetStatus())
    wxNotebook* m_notebook;

    // So that mytimer can access MyFrame's members
    friend class MyTimer;
};



// ----------------------------------------------------------------------------
// MyNotebookPage
// ----------------------------------------------------------------------------

class MyNotebookPage : public wxPanel
{
    MyNotebookPage(wxNotebook* book);
    
    // Slider event handlers
    void OnSeek(wxCommandEvent& event);

    // Media event handlers
    void OnMediaFinished(wxMediaEvent& event);
    
public:
    friend class MyFrame;       //make MyFrame able to access private members
    wxMediaCtrl* m_mediactrl;   //Our media control
    wxSlider* m_slider;         //The slider below our media control
    int m_nLoops;               //Number of times media has looped
    bool m_bLoop;               //Whether we are looping or not
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
// 2) Create our notebook control and add it to the frame
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

    menuFile->Append(wxID_OPENFILESAMEPAGE, _T("&Open File"), 
                        _T("Open a File in the current notebook page"));
    menuFile->Append(wxID_OPENFILENEWPAGE, _T("&Open File in a new page"), 
                        _T("Open a File in a new notebook page"));
#if wxUSE_UNOFFICIALSTUFF
    menuFile->Append(wxID_OPENURLSAMEPAGE, _T("&Open URL"), 
                        _T("Open a URL in the current notebook page"));
    menuFile->Append(wxID_OPENURLNEWPAGE, _T("&Open URL in a new page"), 
                        _T("Open a URL in a new notebook page"));
#endif
    menuFile->AppendSeparator();
    menuFile->Append(wxID_CLOSECURRENTPAGE, _T("&Close Current Page"), 
                        _T("Close current notebook page"));
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
    // Create our notebook - using wxNotebook is luckily pretty 
    // simple and self-explanatory in most cases
    //
    m_notebook = new wxNotebook(this, wxID_NOTEBOOK);

    //
    //  Create our status bar
    //
#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
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

    this->Connect(wxID_OPENFILENEWPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnOpenFileNewPage);

    this->Connect(wxID_OPENFILESAMEPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnOpenFileSamePage);

    this->Connect(wxID_OPENURLNEWPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnOpenURLNewPage);

    this->Connect(wxID_OPENURLSAMEPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnOpenURLSamePage);

    this->Connect(wxID_CLOSECURRENTPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyFrame::OnCloseCurrentPage);

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
    // Notebook events
    //
    this->Connect(wxID_NOTEBOOK, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxNotebookEventFunction) &MyFrame::OnPageChange);
    
    //
    // End of Events
    //

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
    wxMediaCtrl* currentMediaCtrl = GetCurrentMediaCtrl();

    m_basestatus = wxString::Format(_T("Size(x,y):%i,%i ")
                                    _T("Length(Seconds):%u Speed:%1.1fx"),
    currentMediaCtrl->GetBestSize().x,
    currentMediaCtrl->GetBestSize().y,
    (unsigned)((currentMediaCtrl->Length() / 1000)),
    currentMediaCtrl->GetPlaybackRate()
    );
}

// ----------------------------------------------------------------------------
// MyFrame::GetCurrentMediaCtrl
//
// Obtains the media control of the current page, or NULL if there are no
// pages open
// ----------------------------------------------------------------------------
wxMediaCtrl* MyFrame::GetCurrentMediaCtrl()
{
    wxASSERT(m_notebook->GetCurrentPage() != NULL);
    return ((MyNotebookPage*)m_notebook->GetCurrentPage())->m_mediactrl;
}

// ----------------------------------------------------------------------------
// MyFrame::GetCurrentSlider
//
// Obtains the slider of the current page, or NULL if there are no
// pages open
// ----------------------------------------------------------------------------
wxSlider*    MyFrame::GetCurrentSlider()
{
    wxASSERT(m_notebook->GetCurrentPage() != NULL);
    return ((MyNotebookPage*)m_notebook->GetCurrentPage())->m_slider;
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
    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    ((MyNotebookPage*)m_notebook->GetCurrentPage())->m_bLoop = 
            !((MyNotebookPage*)m_notebook->GetCurrentPage())->m_bLoop;
}

// ----------------------------------------------------------------------------
// MyFrame::OnOpenFileSamePage
//
// Called from file->openfile.
// Opens and plays a media file in the current notebook page
// ----------------------------------------------------------------------------
void MyFrame::OnOpenFileSamePage(wxCommandEvent& WXUNUSED(event))
{   
    OpenFile(false); 
}

// ----------------------------------------------------------------------------
// MyFrame::OnOpenFileNewPage
//
// Called from file->openfileinnewpage.
// Opens and plays a media file in a new notebook page
// ----------------------------------------------------------------------------
void MyFrame::OnOpenFileNewPage(wxCommandEvent& WXUNUSED(event))
{   
    OpenFile(true); 
}

// ----------------------------------------------------------------------------
// MyFrame::OpenFile
//
// Code to actually open the media file
//
// 1) Create file dialog and ask the user for input file
// 2) If the user didn't want anything, break out
// 3) Create a new page if the user wanted one or there isn't a current page
// 4) Load the media
// 5) Play the media
// 6) Reset the text on the status bar
// 7) Set the slider of the current page to accurately reflect media length
// ----------------------------------------------------------------------------
void MyFrame::OpenFile(bool bNewPage)
{
    wxFileDialog fd(this);

    if(fd.ShowModal() == wxID_OK)
    {
        if(bNewPage || !m_notebook->GetCurrentPage())
            m_notebook->AddPage(new MyNotebookPage(m_notebook), fd.GetPath(), true);
        
        if( !GetCurrentMediaCtrl()->Load(fd.GetPath()) )
            wxMessageBox(wxT("Couldn't load file!"));

        if( !GetCurrentMediaCtrl()->Play() )
            wxMessageBox(wxT("Couldn't play movie!"));

        ResetStatus();
        
        GetCurrentSlider()->SetRange(0, 
                        (int)(GetCurrentMediaCtrl()->Length() / 1000));

    }
}

// ----------------------------------------------------------------------------
// MyFrame::OnOpenURLSamePage
//
// Called from file->openurl.
// Opens and plays a media file from a URL in the current notebook page
// ----------------------------------------------------------------------------
void MyFrame::OnOpenURLSamePage(wxCommandEvent& WXUNUSED(event))
{   
    OpenURL(false); 
}

// ----------------------------------------------------------------------------
// MyFrame::OnOpenURLNewPage
//
// Called from file->openurlinnewpage.
// Opens and plays a media file from a URL in a new notebook page
// ----------------------------------------------------------------------------
void MyFrame::OnOpenURLNewPage(wxCommandEvent& WXUNUSED(event))
{   
    OpenURL(true); 
}

// ----------------------------------------------------------------------------
// MyFrame::OpenURL
//
// Code to actually open the media file from a URL
//
// 1) Create text input dialog and ask the user for an input URL
// 2) If the user didn't want anything, break out
// 3) Create a new page if the user wanted one or there isn't a current page
// 4) Load the media
// 5) Play the media
// 6) Reset the text on the status bar
// 7) Set the slider of the current page to accurately reflect media length
// ----------------------------------------------------------------------------
void MyFrame::OpenURL(bool bNewPage)
{
   wxString theURL = wxGetTextFromUser(wxT("Enter the URL that has the movie to play"));

    if(!theURL.empty())
    {
        if(bNewPage || !m_notebook->GetCurrentPage())
            m_notebook->AddPage(new MyNotebookPage(m_notebook), theURL, true);

        if( !GetCurrentMediaCtrl()->Load(wxURI(theURL)) )
            wxMessageBox(wxT("Couldn't load URL!"));

        if( !GetCurrentMediaCtrl()->Play() )
            wxMessageBox(wxT("Couldn't play movie!"));
            
        ResetStatus();

        GetCurrentSlider()->SetRange(0, 
                        (int)(GetCurrentMediaCtrl()->Length() / 1000));
    }
}

// ----------------------------------------------------------------------------
// MyFrame::OnCloseCurrentPage
//
// Called when the user wants to close the current notebook page
//
// 1) Get the current page number (wxControl::GetSelection)
// 2) If there is no current page, break out
// 3) Delete the current page
// ----------------------------------------------------------------------------
void MyFrame::OnCloseCurrentPage(wxCommandEvent& WXUNUSED(event))
{
    int sel = m_notebook->GetSelection();

    if (sel != wxNOT_FOUND)
    {
        m_notebook->DeletePage(sel);
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
    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    if( !GetCurrentMediaCtrl()->Play() )
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
    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    if( !GetCurrentMediaCtrl()->Pause() )
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
    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    if( !GetCurrentMediaCtrl()->Stop() )
        wxMessageBox(wxT("Couldn't stop movie!"));
}

// ----------------------------------------------------------------------------
// MyFrame::OnCloseCurrentPage
//
// Called when the user wants to closes the current notebook page
// ----------------------------------------------------------------------------

void MyFrame::OnPageChange(wxNotebookEvent& WXUNUSED(event))
{
    ResetStatus();
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
    if (!m_frame->m_notebook->GetCurrentPage()) return;
        wxMediaCtrl* m_mediactrl = ((MyNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_mediactrl; 
        wxSlider* m_slider = ((MyNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_slider; 
        if (!m_mediactrl) return;

    long lPosition = (long)( m_mediactrl->Tell() / 1000 );
    m_slider->SetValue(lPosition);

#if wxUSE_STATUSBAR
    m_frame->SetStatusText(wxString::Format(
                     _T("%s Pos:%u State:%s Loops:%i"),
                     m_frame->m_basestatus.c_str(),
                     (unsigned int)lPosition,
                     wxGetMediaStateText(m_mediactrl->GetState()),
                    ((MyNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_nLoops
                    
                                            )
                           );
#endif

}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// MyNotebookPage
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// MyNotebookPage Constructor
//
// Creates a media control and slider and adds it to this panel,
// along with some sizers for positioning
// ----------------------------------------------------------------------------

MyNotebookPage::MyNotebookPage(wxNotebook* theBook) :
    wxPanel(theBook, wxID_ANY), m_nLoops(0), m_bLoop(false)
{
    //
    //  Create and attach the first/main sizer
    //
    wxBoxSizer* vertsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(vertsizer);
    this->SetAutoLayout(true);

    //
    //  Create our media control
    //
    m_mediactrl = new wxMediaCtrl();
    
    //  Make sure creation was successful
    bool bOK = m_mediactrl->Create(this, wxID_MEDIACTRL);
    wxASSERT_MSG(bOK, wxT("Could not create media control!"));
    
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
    // Slider events
    //
    this->Connect(wxID_SLIDER, wxEVT_COMMAND_SLIDER_UPDATED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxCommandEventFunction) &MyNotebookPage::OnSeek);

    //
    // Media Control events
    //
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_FINISHED,
                  (wxObjectEventFunction) (wxEventFunction)
                  (wxMediaEventFunction) &MyNotebookPage::OnMediaFinished);
}

// ----------------------------------------------------------------------------
// MyNotebook::OnSeek
//
// Called from file->seek.
// Called when the user moves the slider -
// seeks to a position within the media
// ----------------------------------------------------------------------------
void MyNotebookPage::OnSeek(wxCommandEvent& WXUNUSED(event))
{
    if( m_mediactrl->Seek( 
            m_slider->GetValue() * 1000 
                                   ) == wxInvalidOffset )
        wxMessageBox(wxT("Couldn't seek in movie!"));
}

// ----------------------------------------------------------------------------
// OnMediaFinished
//
// Called when the media stops playing.
// Here we loop it if the user wants to (has been selected from file menu)
// ----------------------------------------------------------------------------
void MyNotebookPage::OnMediaFinished(wxMediaEvent& WXUNUSED(event))
{
    if(m_bLoop)
    {
        if ( !m_mediactrl->Play() )
            wxMessageBox(wxT("Couldn't loop movie!"));
        else
            ++m_nLoops;
    }
}

//
// End of MediaPlayer sample
//
