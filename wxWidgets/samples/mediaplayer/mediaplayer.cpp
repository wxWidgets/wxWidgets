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
// This is a somewhat comprehensive example of how to use all the funtionality
// of the wxMediaCtrl class in wxWidgets.
//
// To use this sample, simply select Open File from the file menu,
// select the file you want to play - and MediaPlayer will play the file in a
// the current notebook page, showing video if necessary.
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
//    Notebook (like this sample does).
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
#include "wx/textdlg.h"     //for getting user text from OpenURL/Debug
#include "wx/notebook.h"    //for wxNotebook and putting movies in pages
#include "wx/cmdline.h"     //for wxCmdLineParser (optional)
#include "wx/listctrl.h"    //for wxListCtrl
#include "wx/dnd.h"         //drag and drop for the playlist
#include "wx/filename.h"    //For wxFileName::GetName()
#include "wx/config.h"      //for native wxConfig

// ----------------------------------------------------------------------------
// Bail out if the user doesn't want one of the
// things we need
// ----------------------------------------------------------------------------

// RN:  I'm not sure why this is here - even minimal doesn't check for
//      wxUSE_GUI.  I may have added it myself though...
#if !wxUSE_GUI
#error "This is a GUI sample"
#endif

#if !wxUSE_MEDIACTRL || !wxUSE_MENUS || !wxUSE_SLIDER || !wxUSE_TIMER || \
    !wxUSE_NOTEBOOK || !wxUSE_LISTCTRL
#error "Not all required elements are enabled.  Please modify setup.h!"
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
    // Menu event IDs
    wxID_LOOP = 1,
    wxID_OPENFILESAMEPAGE,
    wxID_OPENFILENEWPAGE,
    wxID_OPENURLSAMEPAGE,
    wxID_OPENURLNEWPAGE,
    wxID_CLOSECURRENTPAGE,
    wxID_PLAY,
    wxID_PAUSE,
    wxID_NEXT,
    wxID_PREV,
    wxID_SELECTBACKEND,
    wxID_SHOWINTERFACE,
//    wxID_STOP,   [built-in to wxWidgets]
//    wxID_ABOUT,  [built-in to wxWidgets]
//    wxID_EXIT,   [built-in to wxWidgets]
    // Control event IDs
    wxID_SLIDER,
    wxID_PBSLIDER,
    wxID_VOLSLIDER,
    wxID_NOTEBOOK,
    wxID_MEDIACTRL,
    wxID_BUTTONNEXT,
    wxID_BUTTONPREV,
    wxID_BUTTONSTOP,
    wxID_BUTTONPLAY,
    wxID_BUTTONVD,
    wxID_BUTTONVU,
    wxID_LISTCTRL,
    wxID_GAUGE
};

// ----------------------------------------------------------------------------
// wxMediaPlayerApp
// ----------------------------------------------------------------------------

class wxMediaPlayerApp : public wxApp
{
public:
#ifdef __WXMAC__
    virtual void MacOpenFile(const wxString & fileName );
#endif

    virtual bool OnInit();

protected:
    class wxMediaPlayerFrame* m_frame;
};

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame
// ----------------------------------------------------------------------------

class wxMediaPlayerFrame : public wxFrame
{
public:
    // Ctor/Dtor
    wxMediaPlayerFrame(const wxString& title);
    ~wxMediaPlayerFrame();

    // Menu event handlers
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnOpenFileSamePage(wxCommandEvent& event);
    void OnOpenFileNewPage(wxCommandEvent& event);
    void OnOpenURLSamePage(wxCommandEvent& event);
    void OnOpenURLNewPage(wxCommandEvent& event);
    void OnCloseCurrentPage(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);
    void OnPrev(wxCommandEvent& event);
    void OnVolumeDown(wxCommandEvent& event);
    void OnVolumeUp(wxCommandEvent& event);

    void OnLoop(wxCommandEvent& event);
    void OnShowInterface(wxCommandEvent& event);

    void OnSelectBackend(wxCommandEvent& event);

    // Key event handlers
    void OnKeyDown(wxKeyEvent& event);

    // Quickie for playing from command line
    void AddToPlayList(const wxString& szString);

    // ListCtrl event handlers
    void OnChangeSong(wxListEvent& event);

    // Media event handlers
    void OnMediaLoaded(wxMediaEvent& event);

    // Close event handlers
    void OnClose(wxCloseEvent& event);

private:
    // Common open file code
    void OpenFile(bool bNewPage);
    void OpenURL(bool bNewPage);
    void DoOpenFile(const wxString& path, bool bNewPage);
    void DoPlayFile(const wxString& path);

    class wxMediaPlayerTimer* m_timer;     //Timer to write info to status bar
    wxNotebook* m_notebook;     //Notebook containing our pages

    // Maybe I should use more accessors, but for simplicity
    // I'll allow the other classes access to our members
    friend class wxMediaPlayerApp;
    friend class wxMediaPlayerNotebookPage;
    friend class wxMediaPlayerTimer;
};



// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage
// ----------------------------------------------------------------------------

class wxMediaPlayerNotebookPage : public wxPanel
{
    wxMediaPlayerNotebookPage(wxMediaPlayerFrame* parentFrame,
        wxNotebook* book, const wxString& be = wxEmptyString);

    // Slider event handlers
    void OnBeginSeek(wxScrollEvent& event);
    void OnEndSeek(wxScrollEvent& event);
    void OnPBChange(wxScrollEvent& event);
    void OnVolChange(wxScrollEvent& event);

    // Media event handlers
    void OnMediaPlay(wxMediaEvent& event);
    void OnMediaPause(wxMediaEvent& event);
    void OnMediaStop(wxMediaEvent& event);
    void OnMediaFinished(wxMediaEvent& event);

public:
    bool IsBeingDragged();      //accessor for m_bIsBeingDragged

    //make wxMediaPlayerFrame able to access the private members
    friend class wxMediaPlayerFrame;

    int      m_nLastFileId;     //List ID of played file in listctrl
    wxString m_szFile;          //Name of currently playing file/location

    wxMediaCtrl* m_mediactrl;   //Our media control
    class wxMediaPlayerListCtrl* m_playlist;  //Our playlist
    wxSlider* m_slider;         //The slider below our media control
    wxSlider* m_pbSlider;       //Lower-left slider for adjusting speed
    wxSlider* m_volSlider;      //Lower-right slider for adjusting volume
    int m_nLoops;               //Number of times media has looped
    bool m_bLoop;               //Whether we are looping or not
    bool m_bIsBeingDragged;     //Whether the user is dragging the scroll bar
    wxMediaPlayerFrame* m_parentFrame;  //Main wxFrame of our sample
    wxButton* m_prevButton;     //Go to previous file button
    wxButton* m_playButton;     //Play/pause file button
    wxButton* m_stopButton;     //Stop playing file button
    wxButton* m_nextButton;     //Next file button
    wxButton* m_vdButton;       //Volume down button
    wxButton* m_vuButton;       //Volume up button
    wxGauge*  m_gauge;          //Gauge to keep in line with slider
};

// ----------------------------------------------------------------------------
// wxMediaPlayerTimer
// ----------------------------------------------------------------------------

class wxMediaPlayerTimer : public wxTimer
{
public:
    //Ctor
    wxMediaPlayerTimer(wxMediaPlayerFrame* frame) {m_frame = frame;}

    //Called each time the timer's timeout expires
    void Notify();

    wxMediaPlayerFrame* m_frame;       //The wxMediaPlayerFrame
};

// ----------------------------------------------------------------------------
// wxMediaPlayerListCtrl
// ----------------------------------------------------------------------------
class wxMediaPlayerListCtrl : public wxListCtrl
{
public:
    void AddToPlayList(const wxString& szString)
    {
        wxListItem kNewItem;
        kNewItem.SetAlign(wxLIST_FORMAT_LEFT);

        int nID = this->GetItemCount();
        kNewItem.SetId(nID);
        kNewItem.SetMask(wxLIST_MASK_DATA);
        kNewItem.SetData(new wxString(szString));

        this->InsertItem(kNewItem);
        this->SetItem(nID, 0, wxT("*"));
        this->SetItem(nID, 1, wxFileName(szString).GetName());

        if (nID % 2)
        {
            kNewItem.SetBackgroundColour(wxColour(192,192,192));
            this->SetItem(kNewItem);
        }
    }

    void GetSelectedItem(wxListItem& listitem)
    {
        listitem.SetMask(wxLIST_MASK_TEXT |  wxLIST_MASK_DATA);
        int nLast = -1, nLastSelected = -1;
        while ((nLast = this->GetNextItem(nLast,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED)) != -1)
        {
            listitem.SetId(nLast);
            this->GetItem(listitem);
            if ((listitem.GetState() & wxLIST_STATE_FOCUSED) )
                break;
            nLastSelected = nLast;
        }
        if (nLast == -1 && nLastSelected == -1)
            return;
        listitem.SetId(nLastSelected == -1 ? nLast : nLastSelected);
        this->GetItem(listitem);
    }
};

// ----------------------------------------------------------------------------
// wxPlayListDropTarget
//
//  Drop target for playlist (i.e. user drags a file from explorer unto
//  playlist it adds the file)
// ----------------------------------------------------------------------------
#if wxUSE_DRAG_AND_DROP
class wxPlayListDropTarget : public wxFileDropTarget
{
public:
    wxPlayListDropTarget(wxMediaPlayerListCtrl& list) : m_list(list) {}
    ~wxPlayListDropTarget(){}
        virtual bool OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                         const wxArrayString& files)
    {
        for (size_t i = 0; i < files.GetCount(); ++i)
        {
            m_list.AddToPlayList(files[i]);
        }
        return true;
    }
    wxMediaPlayerListCtrl& m_list;
};
#endif

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
// wxMediaPlayerApp
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
IMPLEMENT_APP(wxMediaPlayerApp)

// ----------------------------------------------------------------------------
// wxMediaPlayerApp::OnInit
//
// Where execution starts - akin to a main or WinMain.
// 1) Create the frame and show it to the user
// 2) Process filenames from the commandline
// 3) return true specifying that we want execution to continue past OnInit
// ----------------------------------------------------------------------------
bool wxMediaPlayerApp::OnInit()
{
    // SetAppName() lets wxConfig and others know where to write
    SetAppName(wxT("wxMediaPlayer"));

    wxMediaPlayerFrame *frame =
        new wxMediaPlayerFrame(wxT("MediaPlayer wxWidgets Sample"));
    frame->Show(true);

#if wxUSE_CMDLINE_PARSER
    //
    //  What this does is get all the command line arguments
    //  and treat each one as a file to put to the initial playlist
    //
    wxCmdLineEntryDesc cmdLineDesc[2];
    cmdLineDesc[0].kind = wxCMD_LINE_PARAM;
    cmdLineDesc[0].shortName = NULL;
    cmdLineDesc[0].longName = NULL;
    cmdLineDesc[0].description = wxT("input files");
    cmdLineDesc[0].type = wxCMD_LINE_VAL_STRING;
    cmdLineDesc[0].flags = wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE;

    cmdLineDesc[1].kind = wxCMD_LINE_NONE;

    //gets the passed media files from cmd line
    wxCmdLineParser parser (cmdLineDesc, argc, argv);

    // get filenames from the commandline
    if (parser.Parse() == 0)
    {
        for (size_t paramNr=0; paramNr < parser.GetParamCount(); ++paramNr)
        {
            frame->AddToPlayList((parser.GetParam (paramNr)));
        }
        wxCommandEvent theEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_NEXT);
        frame->AddPendingEvent(theEvent);
    }
#endif

    return true;
}

#ifdef __WXMAC__

void wxMediaPlayerApp::MacOpenFile(const wxString & fileName )
{
    //Called when a user drags a file over our app
    m_frame->DoOpenFile(fileName, true /* new page */);
}

#endif // __WXMAC__

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerFrame
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame Constructor
//
// 1) Create our menus
// 2) Create our notebook control and add it to the frame
// 3) Create our status bar
// 4) Connect our events
// 5) Start our timer
// ----------------------------------------------------------------------------

wxMediaPlayerFrame::wxMediaPlayerFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,600))
{
    //
    //  Create Menus
    //
    wxMenu *fileMenu = new wxMenu;
    wxMenu *controlsMenu = new wxMenu;
    wxMenu *optionsMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *debugMenu = new wxMenu;

    fileMenu->Append(wxID_OPENFILESAMEPAGE, wxT("&Open File\tCtrl-Shift-O"),
                        wxT("Open a File in the current notebook page"));
    fileMenu->Append(wxID_OPENFILENEWPAGE, wxT("&Open File in a new page"),
                        wxT("Open a File in a new notebook page"));
    fileMenu->Append(wxID_OPENURLSAMEPAGE, wxT("&Open URL"),
                        wxT("Open a URL in the current notebook page"));
    fileMenu->Append(wxID_OPENURLNEWPAGE, wxT("&Open URL in a new page"),
                        wxT("Open a URL in a new notebook page"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_CLOSECURRENTPAGE, wxT("&Close Current Page\tCtrl-C"),
                        wxT("Close current notebook page"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT,
                     wxT("E&xit\tAlt-X"),
                     wxT("Quit this program"));

    controlsMenu->Append(wxID_PLAY, wxT("&Play/Pause\tCtrl-P"), wxT("Resume/Pause playback"));
    controlsMenu->Append(wxID_STOP, wxT("&Stop\tCtrl-S"), wxT("Stop playback"));
    controlsMenu->AppendSeparator();
    controlsMenu->Append(wxID_PREV, wxT("&Previous\tCtrl-B"), wxT("Go to previous track"));
    controlsMenu->Append(wxID_NEXT, wxT("&Next\tCtrl-N"), wxT("Skip to next track"));

    optionsMenu->AppendCheckItem(wxID_LOOP,
                              wxT("&Loop\tCtrl-L"),
                              wxT("Loop Selected Media"));
    optionsMenu->AppendCheckItem(wxID_SHOWINTERFACE,
                              wxT("&Show Interface\tCtrl-I"),
                              wxT("Show wxMediaCtrl native controls"));

    debugMenu->Append(wxID_SELECTBACKEND,
                     wxT("&Select Backend...\tCtrl-D"),
                     wxT("Select a backend manually"));

    helpMenu->Append(wxID_ABOUT,
                     wxT("&About...\tF1"),
                     wxT("Show about dialog"));


    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(controlsMenu, wxT("&Controls"));
    menuBar->Append(optionsMenu, wxT("&Options"));
    menuBar->Append(debugMenu, wxT("&Debug"));
    menuBar->Append(helpMenu, wxT("&Help"));
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
    //  class you want to use for events, such as wxMediaPlayerFrame.
    //
    //  Then after your class declaration you put
    //  BEGIN_EVENT_TABLE(wxMediaPlayerFrame, wxFrame)
    //  EVT_XXX(XXX)...
    //  END_EVENT_TABLE()
    //
    //  Where wxMediaPlayerFrame is the class with the DECLARE_MESSAGE_MAP
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
    //  (wxCommandEventFunction) &wxMediaPlayerFrame::MyHandler
    //
    //  Or, you can use the new (2.5.5+) event handler
    //  conversion macros - for instance the above could
    //  be done as
    //  wxCommandEventHandler(wxMediaPlayerFrame::MyHandler)
    //  pretty simple, eh?
    //
    //  The fourth is an optional userdata param -
    //  this is of historical relevance only and is
    //  there only for backwards compatibility.
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
                  wxCommandEventHandler(wxMediaPlayerFrame::OnQuit));

    this->Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnAbout));

    this->Connect(wxID_LOOP, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnLoop));

    this->Connect(wxID_SHOWINTERFACE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnShowInterface));

    this->Connect(wxID_OPENFILENEWPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenFileNewPage));

    this->Connect(wxID_OPENFILESAMEPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenFileSamePage));

    this->Connect(wxID_OPENURLNEWPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenURLNewPage));

    this->Connect(wxID_OPENURLSAMEPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenURLSamePage));

    this->Connect(wxID_CLOSECURRENTPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnCloseCurrentPage));

    this->Connect(wxID_PLAY, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnPlay));

    this->Connect(wxID_STOP, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnStop));

    this->Connect(wxID_NEXT, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnNext));

    this->Connect(wxID_PREV, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnPrev));

    this->Connect(wxID_SELECTBACKEND, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnSelectBackend));

    //
    // Key events
    //
    wxTheApp->Connect(wxID_ANY, wxEVT_KEY_DOWN,
                  wxKeyEventHandler(wxMediaPlayerFrame::OnKeyDown),
                  (wxObject*)0, this);

    //
    // Close events
    //
    this->Connect(wxID_ANY, wxEVT_CLOSE_WINDOW,
                wxCloseEventHandler(wxMediaPlayerFrame::OnClose));

    //
    // End of Events
    //

    //
    //  Create an initial notebook page so the user has something
    //  to work with without having to go file->open every time :).
    //
    wxMediaPlayerNotebookPage* page = 
        new wxMediaPlayerNotebookPage(this, m_notebook);
    m_notebook->AddPage(page,
                        wxT(""),
                        true);

    //
    //  Here we load the our configuration -
    //  in our case we load all the files that were left in
    //  the playlist the last time the user closed our application
    //
    //  As an exercise to the reader try modifying it so that
    //  it properly loads the playlist for each page without
    //  conflicting (loading the same data) with the other ones.
    //
    wxConfig conf;
    wxString key, outstring;
    for(int i = 0; ; ++i)
    {
        key.clear();
        key << i;
        if(!conf.Read(key, &outstring))
            break;
        page->m_playlist->AddToPlayList(outstring);
    }

    //
    //  Create a timer to update our status bar
    //
    m_timer = new wxMediaPlayerTimer(this);
    m_timer->Start(500);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame Destructor
//
// 1) Deletes child objects implicitly
// 2) Delete our timer explicitly
// ----------------------------------------------------------------------------
wxMediaPlayerFrame::~wxMediaPlayerFrame()
{
    //  Shut down our timer
    delete m_timer;

    //
    //  Here we save our info to the registry or whatever
    //  mechanism the OS uses.
    //
    //  This makes it so that when mediaplayer loads up again
    //  it restores the same files that were in the playlist
    //  this time, rather than the user manually re-adding them.
    //
    //  We need to do conf->DeleteAll() here because by default
    //  the config still contains the same files as last time
    //  so we need to clear it before writing our new ones.
    //
    //  TODO:  Maybe you could add a menu option to the
    //  options menu to delete the configuration on exit -
    //  all you'd need to do is just remove everything after
    //  conf->DeleteAll() here
    //
    //  As an exercise to the reader, try modifying this so
    //  that it saves the data for each notebook page
    //
    wxMediaPlayerListCtrl* playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetPage(0))->m_playlist;

    wxConfig conf;
    conf.DeleteAll();

    for(int i = 0; i < playlist->GetItemCount(); ++i)
    {
        wxString* pData = (wxString*) playlist->GetItemData(i);
        wxString s;
        s << i;
        conf.Write(s, *(pData));
        delete pData;
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnClose
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnClose(wxCloseEvent& event)
{
    event.Skip(); //really close the frame
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::AddToPlayList
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::AddToPlayList(const wxString& szString)
{
    wxMediaPlayerNotebookPage* currentpage =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage());

    currentpage->m_playlist->AddToPlayList(szString);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnQuit
//
// Called from file->quit.
// Closes this application.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnAbout
//
// Called from help->about.
// Gets some info about this application.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("This is a test of wxMediaCtrl.\n\n")

                wxT("Intructions:\n")

                wxT("The top slider shows the current the current position, ")
                wxT("which you can change by dragging and releasing it.\n")

                wxT("The gauge (progress bar) shows the progress in ")
                wxT("downloading data of the current file - it may always be ")
                wxT("Empty due to lack of support from the current backend.\n")

                wxT("The lower-left slider controls the volume and the lower-")
                wxT("right slider controls the playback rate/speed of the ")
                wxT("media\n\n")

                wxT("Currently using: %s"), wxVERSION_STRING);

    wxMessageBox(msg, wxT("About wxMediaCtrl test"),
                 wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnLoop
//
// Called from file->loop.
// Changes the state of whether we want to loop or not.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnLoop(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerNotebookPage* currentpage =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage());

    currentpage->m_bLoop = !currentpage->m_bLoop;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnLoop
//
// Called from file->loop.
// Changes the state of whether we want to loop or not.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnShowInterface(wxCommandEvent& event)
{
    wxMediaPlayerNotebookPage* currentpage =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage());

    if( !currentpage->m_mediactrl->ShowPlayerControls(event.IsChecked() ?
            wxMEDIACTRLPLAYERCONTROLS_DEFAULT :
             wxMEDIACTRLPLAYERCONTROLS_NONE)    )
    {
        //error - uncheck and warn user
        wxMenuItem* pSIItem = GetMenuBar()->FindItem(wxID_SHOWINTERFACE);
        wxASSERT(pSIItem);
        pSIItem->Check(!event.IsChecked());
        
        if(event.IsChecked())
            wxMessageBox(wxT("Could not show player controls"));
        else
            wxMessageBox(wxT("Could not hide player controls"));
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenFileSamePage
//
// Called from file->openfile.
// Opens and plays a media file in the current notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenFileSamePage(wxCommandEvent& WXUNUSED(event))
{
    OpenFile(false);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenFileNewPage
//
// Called from file->openfileinnewpage.
// Opens and plays a media file in a new notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenFileNewPage(wxCommandEvent& WXUNUSED(event))
{
    OpenFile(true);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OpenFile
//
// Opens a file dialog asking the user for a filename, then
// calls DoOpenFile which will add the file to the playlist and play it
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OpenFile(bool bNewPage)
{
    wxFileDialog fd(this);

    if(fd.ShowModal() == wxID_OK)
    {
        DoOpenFile(fd.GetPath(), bNewPage);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::DoOpenFile
//
// Adds the file to our playlist, selects it in the playlist,
// and then calls DoPlayFile to play it
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::DoOpenFile(const wxString& path, bool bNewPage)
{
    if(bNewPage)
    {
        m_notebook->AddPage(
            new wxMediaPlayerNotebookPage(this, m_notebook),
            path,
            true);
    }

    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    if(currentpage->m_nLastFileId != -1)
        currentpage->m_playlist->SetItemState(currentpage->m_nLastFileId, 
                                              0, wxLIST_STATE_SELECTED);

    wxListItem newlistitem;
    newlistitem.SetAlign(wxLIST_FORMAT_LEFT);

    int nID;

    newlistitem.SetId(nID = currentpage->m_playlist->GetItemCount());
    newlistitem.SetMask(wxLIST_MASK_DATA | wxLIST_MASK_STATE);
    newlistitem.SetState(wxLIST_STATE_SELECTED);
    newlistitem.SetData(new wxString(path));

    currentpage->m_playlist->InsertItem(newlistitem);
    currentpage->m_playlist->SetItem(nID, 0, wxT("*"));
    currentpage->m_playlist->SetItem(nID, 1, wxFileName(path).GetName());

    if (nID % 2)
    {
        newlistitem.SetBackgroundColour(wxColour(192,192,192));
        currentpage->m_playlist->SetItem(newlistitem);
    }

    DoPlayFile(path);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::DoPlayFile
//
// Pauses the file if its the currently playing file,
// otherwise it plays the file
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::DoPlayFile(const wxString& path)
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    wxListItem listitem;
    currentpage->m_playlist->GetSelectedItem(listitem);

    if( (  listitem.GetData() &&
           currentpage->m_nLastFileId == listitem.GetId() &&
           currentpage->m_szFile.compare(path) == 0 ) ||
        (  !listitem.GetData() &&
            currentpage->m_nLastFileId != -1 &&
            currentpage->m_szFile.compare(path) == 0)
      )
    {
        if(currentpage->m_mediactrl->GetState() == wxMEDIASTATE_PLAYING)
    {
            if( !currentpage->m_mediactrl->Pause() )
                wxMessageBox(wxT("Couldn't pause movie!"));
        }
        else
        {
            if( !currentpage->m_mediactrl->Play() )
                wxMessageBox(wxT("Couldn't play movie!"));
        }
    }
    else
    {
        int nNewId = listitem.GetData() ? listitem.GetId() : 
                            currentpage->m_playlist->GetItemCount()-1;
        m_notebook->SetPageText(m_notebook->GetSelection(),
                                wxFileName(path).GetName());

        if(currentpage->m_nLastFileId != -1)
           currentpage->m_playlist->SetItem(
                    currentpage->m_nLastFileId, 0, wxT("*"));

        wxURI uripath(path);
        if( uripath.IsReference() )
        {
            if( !currentpage->m_mediactrl->Load(path) )
            {
                wxMessageBox(wxT("Couldn't load file!"));
                currentpage->m_playlist->SetItem(nNewId, 0, wxT("E"));
            }
            else
            {
                currentpage->m_playlist->SetItem(nNewId, 0, wxT("O"));
            }
        }
        else
        {
            if( !currentpage->m_mediactrl->Load(uripath) )
            {
                wxMessageBox(wxT("Couldn't load URL!"));
                currentpage->m_playlist->SetItem(nNewId, 0, wxT("E"));
            }
            else
            {
                currentpage->m_playlist->SetItem(nNewId, 0, wxT("O"));
            }
        }

        currentpage->m_nLastFileId = nNewId;
        currentpage->m_szFile = path;
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId, 
                                         1, wxFileName(path).GetName());
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId, 
                                         2, wxT(""));
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnMediaLoaded
//
// Called when the media is ready to be played - and does
// so, also gets the length of media and shows that in the list control
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnMediaLoaded(wxMediaEvent& WXUNUSED(evt))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    if( !currentpage->m_mediactrl->Play() )
    {
            wxMessageBox(wxT("Couldn't play movie!"));
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId, 0, wxT("E"));
    }
    else
    {
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId, 0, wxT(">"));
    }

}


// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnSelectBackend
//
// Little debugging routine - enter the class name of a backend and it
// will use that instead of letting wxMediaCtrl search the wxMediaBackend
// RTTI class list.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnSelectBackend(wxCommandEvent& WXUNUSED(evt))
{
    wxString sBackend = wxGetTextFromUser(wxT("Enter backend to use"));

    if(sBackend.empty() == false)  //could have been cancelled by the user
    {
        int sel = m_notebook->GetSelection();

        if (sel != wxNOT_FOUND)
        {
            m_notebook->DeletePage(sel);
        }

        m_notebook->AddPage(new wxMediaPlayerNotebookPage(this, m_notebook,
                                                        sBackend
                                                        ), wxT(""), true);

        DoOpenFile(
            ((wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage())->m_szFile, 
            false);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenURLSamePage
//
// Called from file->openurl.
// Opens and plays a media file from a URL in the current notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenURLSamePage(wxCommandEvent& WXUNUSED(event))
{
    OpenURL(false);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenURLNewPage
//
// Called from file->openurlinnewpage.
// Opens and plays a media file from a URL in a new notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenURLNewPage(wxCommandEvent& WXUNUSED(event))
{
    OpenURL(true);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OpenURL
//
// Just calls DoOpenFile with the url path - which calls DoPlayFile
// which handles the real dirty work
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OpenURL(bool bNewPage)
{
    wxString sUrl = wxGetTextFromUser(
        wxT("Enter the URL that has the movie to play")
                                     );

    if(sUrl.empty() == false) //could have been cancelled by user
    {
        DoOpenFile(sUrl, bNewPage);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnCloseCurrentPage
//
// Called when the user wants to close the current notebook page
//
// 1) Get the current page number (wxControl::GetSelection)
// 2) If there is no current page, break out
// 3) Delete the current page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnCloseCurrentPage(wxCommandEvent& WXUNUSED(event))
{
    if( m_notebook->GetPageCount() > 1 )
    {
    int sel = m_notebook->GetSelection();

    if (sel != wxNOT_FOUND)
    {
        m_notebook->DeletePage(sel);
    }
    }
    else
    {
        wxMessageBox(wxT("Cannot close main page"));
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnPlay
//
// Called from file->play.
// Resumes the media if it is paused or stopped.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    wxListItem listitem;
    currentpage->m_playlist->GetSelectedItem(listitem);
    if ( !listitem.GetData() )
    {
        int nLast = -1;
        if ((nLast = currentpage->m_playlist->GetNextItem(nLast,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_DONTCARE)) == -1)
        {
            //no items in list
            wxMessageBox(wxT("No items in playlist!"));
    }
        else
        {
        listitem.SetId(nLast);
            currentpage->m_playlist->GetItem(listitem);
        listitem.SetMask(listitem.GetMask() | wxLIST_MASK_STATE);
        listitem.SetState(listitem.GetState() | wxLIST_STATE_SELECTED);
            currentpage->m_playlist->SetItem(listitem);
            wxASSERT(listitem.GetData());
            DoPlayFile((*((wxString*) listitem.GetData())));            
    }
    }
    else
    {
        wxASSERT(listitem.GetData());
        DoPlayFile((*((wxString*) listitem.GetData())));
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnKeyDown
//
// Deletes all selected files from the playlist if the backspace key is pressed
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnKeyDown(wxKeyEvent& event)
{
   if(event.GetKeyCode() == WXK_BACK/*DELETE*/)
    {
        wxMediaPlayerNotebookPage* currentpage = 
            (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();
       //delete all selected items
       while(true)
       {
           wxInt32 nSelectedItem = currentpage->m_playlist->GetNextItem(
                    -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
           if (nSelectedItem == -1)
               break;

           wxListItem listitem;
           listitem.SetId(nSelectedItem);
           currentpage->m_playlist->GetItem(listitem);
           delete (wxString*) listitem.GetData();

           currentpage->m_playlist->DeleteItem(nSelectedItem);
       }
    }

   //Could be wxGetTextFromUser or something else important
   if(event.GetEventObject() != this)
       event.Skip();
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnStop
//
// Called from file->stop.
// Where it stops depends on whether you can seek in the
// media control or not - if you can it stops and seeks to the beginning,
// otherwise it will appear to be at the end - but it will start over again
// when Play() is called
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnStop(wxCommandEvent& WXUNUSED(evt))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    if( !currentpage->m_mediactrl->Stop() )
        wxMessageBox(wxT("Couldn't stop movie!"));
    else
        currentpage->m_playlist->SetItem(
            currentpage->m_nLastFileId, 0, wxT("[]"));
}


// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnChangeSong
//
// Routine that plays the currently selected file in the playlist.
// Called when the user actives the song from the playlist,
// and from other various places in the sample
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnChangeSong(wxListEvent& WXUNUSED(evt))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    wxListItem listitem;
    currentpage->m_playlist->GetSelectedItem(listitem);
    if(listitem.GetData())
    DoPlayFile((*((wxString*) listitem.GetData())));
    else
        wxMessageBox(wxT("No selected item!"));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnPrev
//
// Tedious wxListCtrl stuff.  Goes to prevous song in list, or if at the
// beginning goes to the last in the list.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnPrev(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    if (currentpage->m_playlist->GetItemCount() == 0)
        return;

    wxInt32 nLastSelectedItem = -1;
    while(true)
    {
        wxInt32 nSelectedItem = currentpage->m_playlist->GetNextItem(nLastSelectedItem,
                                                     wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (nSelectedItem == -1)
            break;
        nLastSelectedItem = nSelectedItem;
        currentpage->m_playlist->SetItemState(nSelectedItem, 0, wxLIST_STATE_SELECTED);
    }

    if (nLastSelectedItem == -1) 
    {
        //nothing selected, default to the file before the currently playing one
        if(currentpage->m_nLastFileId == 0)
            nLastSelectedItem = currentpage->m_playlist->GetItemCount() - 1;
    else
            nLastSelectedItem = currentpage->m_nLastFileId - 1;
    }
    else if (nLastSelectedItem == 0) 
        nLastSelectedItem = currentpage->m_playlist->GetItemCount() - 1;
    else
        nLastSelectedItem -= 1;

    if(nLastSelectedItem == currentpage->m_nLastFileId)
        return; //already playing... nothing to do

    wxListItem listitem;
    listitem.SetId(nLastSelectedItem);
    listitem.SetMask(wxLIST_MASK_TEXT |  wxLIST_MASK_DATA);
    currentpage->m_playlist->GetItem(listitem);
    listitem.SetMask(listitem.GetMask() | wxLIST_MASK_STATE);
    listitem.SetState(listitem.GetState() | wxLIST_STATE_SELECTED);
    currentpage->m_playlist->SetItem(listitem);

    wxASSERT(listitem.GetData());
    DoPlayFile((*((wxString*) listitem.GetData())));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnNext
//
// Tedious wxListCtrl stuff.  Goes to next song in list, or if at the
// end goes to the first in the list.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnNext(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    if (currentpage->m_playlist->GetItemCount() == 0)
        return;

    wxInt32 nLastSelectedItem = -1;
    while(true)
    {
        wxInt32 nSelectedItem = currentpage->m_playlist->GetNextItem(nLastSelectedItem,
                                                     wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (nSelectedItem == -1)
            break;
        nLastSelectedItem = nSelectedItem;
        currentpage->m_playlist->SetItemState(nSelectedItem, 0, wxLIST_STATE_SELECTED);
    }

    if (nLastSelectedItem == -1)
    {
        if(currentpage->m_nLastFileId == currentpage->m_playlist->GetItemCount() - 1)
        nLastSelectedItem = 0;
    else
            nLastSelectedItem = currentpage->m_nLastFileId + 1;                
    }
    else if (nLastSelectedItem == currentpage->m_playlist->GetItemCount() - 1)
            nLastSelectedItem = 0;
        else
            nLastSelectedItem += 1;

    if(nLastSelectedItem == currentpage->m_nLastFileId)
        return; //already playing... nothing to do

    wxListItem listitem;
    listitem.SetMask(wxLIST_MASK_TEXT |  wxLIST_MASK_DATA);
    listitem.SetId(nLastSelectedItem);
    currentpage->m_playlist->GetItem(listitem);
    listitem.SetMask(listitem.GetMask() | wxLIST_MASK_STATE);
    listitem.SetState(listitem.GetState() | wxLIST_STATE_SELECTED);
    currentpage->m_playlist->SetItem(listitem);

    wxASSERT(listitem.GetData());
    DoPlayFile((*((wxString*) listitem.GetData())));
}


// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnVolumeDown
//
// Lowers the volume of the media control by 5%
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnVolumeDown(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    double dVolume = currentpage->m_mediactrl->GetVolume();
    currentpage->m_mediactrl->SetVolume(dVolume < 0.05 ? 0.0 : dVolume - .05);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnVolumeUp
//
// Increases the volume of the media control by 5%
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnVolumeUp(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_notebook->GetCurrentPage();

    double dVolume = currentpage->m_mediactrl->GetVolume();
    currentpage->m_mediactrl->SetVolume(dVolume > 0.95 ? 1.0 : dVolume + .05);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerTimer
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxMediaPlayerTimer::Notify
//
// 1) Updates media information on the status bar
// 2) Sets the max/min length of the slider and guage
//
// Note that the reason we continually do this and don't cache it is because
// some backends such as GStreamer are dynamic change values all the time
// and often don't have things like duration or video size available
// until the media is actually being played
// ----------------------------------------------------------------------------
void wxMediaPlayerTimer::Notify()
{
    wxMediaPlayerNotebookPage* currentpage = 
        (wxMediaPlayerNotebookPage*) m_frame->m_notebook->GetCurrentPage();
    wxMediaCtrl* currentMediaCtrl = currentpage->m_mediactrl;

    if(currentpage)
    {
        // Number of minutes/seconds total
        wxLongLong llLength = currentpage->m_mediactrl->Length();
        int nMinutes = (int) (llLength / 60000).GetValue();
        int nSeconds = (int) ((llLength % 60000)/1000).GetValue();

        // Duration string (i.e. MM:SS)
        wxString sDuration;
        sDuration.Printf(wxT("%2i:%02i"), nMinutes, nSeconds);


        // Number of minutes/seconds total
        wxLongLong llTell = currentpage->m_mediactrl->Tell();
        nMinutes = (int) (llTell / 60000).GetValue();
        nSeconds = (int) ((llTell % 60000)/1000).GetValue();

        // Position string (i.e. MM:SS)
        wxString sPosition;
        sPosition.Printf(wxT("%2i:%02i"), nMinutes, nSeconds);


        // Set the third item in the listctrl entry to the duration string
        if(currentpage->m_nLastFileId >= 0)
            currentpage->m_playlist->SetItem(
                    currentpage->m_nLastFileId, 2, sDuration);

        // Setup the slider and gauge min/max values
        currentpage->m_slider->SetRange(0, (int)(llLength / 1000).GetValue());
        currentpage->m_gauge->SetRange(100);


        // if the slider is not being dragged then update it with the song position
        if(currentpage->IsBeingDragged() == false)
            currentpage->m_slider->SetValue((long)(llTell / 1000).GetValue());


        // Update the gauge with the download progress
        wxLongLong llDownloadProgress =
            currentpage->m_mediactrl->GetDownloadProgress();
        wxLongLong llDownloadTotal =
            currentpage->m_mediactrl->GetDownloadTotal();

        if(llDownloadTotal.GetValue() != 0)
        {
            currentpage->m_gauge->SetValue(
                (int) ((llDownloadProgress * 100) / llDownloadTotal).GetValue()
                                          );
        }

        // GetBestSize holds the original video size
        wxSize videoSize = currentMediaCtrl->GetBestSize();

        // Now the big part - set the status bar text to
        // hold various metadata about the media
#if wxUSE_STATUSBAR
        m_frame->SetStatusText(wxString::Format(
                        wxT("Size(x,y):%i,%i ")
                        wxT("Position:%s/%s Speed:%1.1fx ")
                        wxT("State:%s Loops:%i D/T:[%i]/[%i] V:%i%%"),
                        videoSize.x,
                        videoSize.y,
                        sPosition.c_str(),
                        sDuration.c_str(),
                        currentMediaCtrl->GetPlaybackRate(),
                        wxGetMediaStateText(currentpage->m_mediactrl->GetState()),
                        currentpage->m_nLoops,
                        (int)llDownloadProgress.GetValue(),
                        (int)llDownloadTotal.GetValue(),
                        (int)(currentpage->m_mediactrl->GetVolume() * 100)));
#endif // wxUSE_STATUSBAR
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerNotebookPage
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage Constructor
//
// Creates a media control and slider and adds it to this panel,
// along with some sizers for positioning
// ----------------------------------------------------------------------------
wxMediaPlayerNotebookPage::wxMediaPlayerNotebookPage(wxMediaPlayerFrame* parentFrame,
                                                     wxNotebook* theBook,
                                                     const wxString& szBackend)
                         : wxPanel(theBook, wxID_ANY),
                           m_nLastFileId(-1),
                           m_nLoops(0),
                           m_bLoop(false),
                           m_bIsBeingDragged(false),
                           m_parentFrame(parentFrame)
{
    //
    //  Layout
    //
    //  [wxMediaCtrl]
    //  [playlist]
    //  [5 control buttons]
    //  [slider]
    //  [gauge]
    //

    //
    //  Create and attach the sizer
    //
    wxFlexGridSizer* sizer = new wxFlexGridSizer(2, 1, 0, 0);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);
    sizer->AddGrowableRow(0);
    sizer->AddGrowableCol(0);

    //
    //  Create our media control
    //
    m_mediactrl = new wxMediaCtrl();

    //  Make sure creation was successful
    bool bOK = m_mediactrl->Create(this, wxID_MEDIACTRL, wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize, 0,
//you could specify a macrod backend here like
//  wxMEDIABACKEND_WMP10);
//        wxT("wxPDFMediaBackend"));
                                   szBackend);
//you could change the cursor here like
//    m_mediactrl->SetCursor(wxCURSOR_BLANK);
//note that this may not effect it if SetPlayerControls
//is set to something else than wxMEDIACTRLPLAYERCONTROLS_NONE
    wxASSERT_MSG(bOK, wxT("Could not create media control!"));
    wxUnusedVar(bOK);

    sizer->Add(m_mediactrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5);

    //
    //  Create the playlist/listctrl
    //
    m_playlist = new wxMediaPlayerListCtrl();
    m_playlist->Create(this, wxID_LISTCTRL, wxDefaultPosition,
                    wxDefaultSize,
                    wxLC_REPORT //wxLC_LIST
                    | wxSUNKEN_BORDER);

    //  Set the background of our listctrl to white
    m_playlist->SetBackgroundColour(wxColour(255,255,255));

    //  The layout of the headers of the listctrl are like
    //  |   | File               |  Length
    //
    //  Where Column one is a character representing the state the file is in:
    //  * - not the current file
    //  E - Error has occured
    //  > - Currently Playing
    //  [] - Stopped
    //  || - Paused
    //  (( - Volume Down 5%
    //  )) - Volume Up 5%
    //
    //  Column two is the name of the file
    //
    //  Column three is the length in seconds of the file
    m_playlist->InsertColumn(0,_(""), wxLIST_FORMAT_CENTER, 20);
    m_playlist->InsertColumn(1,_("File"), wxLIST_FORMAT_LEFT, /*wxLIST_AUTOSIZE_USEHEADER*/305);
    m_playlist->InsertColumn(2,_("Length"), wxLIST_FORMAT_CENTER, 75);

#if wxUSE_DRAG_AND_DROP
    m_playlist->SetDropTarget(new wxPlayListDropTarget(*m_playlist));
#endif

    sizer->Add(m_playlist, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5);

    //
    //  Create the control buttons
    //  TODO/FIXME/HACK:  This part about sizers is really a nice hack
    //                    and probably isn't proper
    //
    wxBoxSizer* horsizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* vertsizer = new wxBoxSizer(wxHORIZONTAL);

    m_prevButton = new wxButton();
    m_playButton = new wxButton();
    m_stopButton = new wxButton();
    m_nextButton = new wxButton();
    m_vdButton = new wxButton();
    m_vuButton = new wxButton();

    m_prevButton->Create(this, wxID_BUTTONPREV, wxT("|<"));
    m_playButton->Create(this, wxID_BUTTONPLAY, wxT(">"));
    m_stopButton->Create(this, wxID_BUTTONSTOP, wxT("[]"));
    m_nextButton->Create(this, wxID_BUTTONNEXT, wxT(">|"));
    m_vdButton->Create(this, wxID_BUTTONVD, wxT("(("));
    m_vuButton->Create(this, wxID_BUTTONVU, wxT("))"));
    vertsizer->Add(m_prevButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_playButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_stopButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_nextButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_vdButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_vuButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    horsizer1->Add(vertsizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    sizer->Add(horsizer1, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 5);


    //
    //  Create our slider
    //
    m_slider = new wxSlider(this, wxID_SLIDER, 0, //init
                            0, //start
                            0, //end
                            wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL );
    sizer->Add(m_slider, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);

    //
    //  Create the gauge
    //
    m_gauge = new wxGauge();
    m_gauge->Create(this, wxID_GAUGE, 0, wxDefaultPosition, wxDefaultSize,
                        wxGA_HORIZONTAL | wxGA_SMOOTH);
    sizer->Add(m_gauge, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);

    //
    //  Create the speed/volume sliders
    //
    wxBoxSizer* horsizer3 = new wxBoxSizer(wxHORIZONTAL);

    m_volSlider = new wxSlider(this, wxID_VOLSLIDER, 100, // init
                            0, // start
                            100, // end
                            wxDefaultPosition, wxSize(250,20),
                            wxSL_HORIZONTAL );
    horsizer3->Add(m_volSlider, 1, wxALL, 5);

    m_pbSlider = new wxSlider(this, wxID_PBSLIDER, 4, // init
                            1, // start
                            16, // end
                            wxDefaultPosition, wxSize(250,20),
                            wxSL_HORIZONTAL );
    horsizer3->Add(m_pbSlider, 1, wxALL, 5);
    sizer->Add(horsizer3, 1, wxCENTRE | wxALL, 5);

    //
    // ListCtrl events
    //
    this->Connect( wxID_LISTCTRL, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
        wxListEventHandler(wxMediaPlayerFrame::OnChangeSong),
        (wxObject*)0, parentFrame);

    //
    // Slider events
    //
    this->Connect(wxID_SLIDER, wxEVT_SCROLL_THUMBTRACK,
                  wxScrollEventHandler(wxMediaPlayerNotebookPage::OnBeginSeek));
    this->Connect(wxID_SLIDER, wxEVT_SCROLL_THUMBRELEASE,
                  wxScrollEventHandler(wxMediaPlayerNotebookPage::OnEndSeek));
    this->Connect(wxID_PBSLIDER, wxEVT_SCROLL_THUMBRELEASE,
                    wxScrollEventHandler(wxMediaPlayerNotebookPage::OnPBChange));
    this->Connect(wxID_VOLSLIDER, wxEVT_SCROLL_THUMBRELEASE,
                    wxScrollEventHandler(wxMediaPlayerNotebookPage::OnVolChange));

    //
    // Media Control events
    //
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_PLAY,
                  wxMediaEventHandler(wxMediaPlayerNotebookPage::OnMediaPlay));
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_PAUSE,
                  wxMediaEventHandler(wxMediaPlayerNotebookPage::OnMediaPause));
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_STOP,
                  wxMediaEventHandler(wxMediaPlayerNotebookPage::OnMediaStop));
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_FINISHED,
                  wxMediaEventHandler(wxMediaPlayerNotebookPage::OnMediaFinished));
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_LOADED,
                  wxMediaEventHandler(wxMediaPlayerFrame::OnMediaLoaded),
                  (wxObject*)0, parentFrame);

    //
    // Button events
    //
    this->Connect( wxID_BUTTONPREV, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnPrev),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONPLAY, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnPlay),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONSTOP, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnStop),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONNEXT, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnNext),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONVD, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnVolumeDown),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONVU, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnVolumeUp),
        (wxObject*)0, parentFrame);
}

// ----------------------------------------------------------------------------
// MyNotebook::OnBeginSeek
//
// Sets m_bIsBeingDragged to true to stop the timer from changing the position
// of our slider
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnBeginSeek(wxScrollEvent& WXUNUSED(event))
{
    m_bIsBeingDragged = true;
}

// ----------------------------------------------------------------------------
// MyNotebook::OnEndSeek
//
// Called from file->seek.
// Called when the user moves the slider -
// seeks to a position within the media
// then sets m_bIsBeingDragged to false to ok the timer to change the position
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnEndSeek(wxScrollEvent& WXUNUSED(event))
{
    if( m_mediactrl->Seek(
            m_slider->GetValue() * 1000
                                   ) == wxInvalidOffset )
        wxMessageBox(wxT("Couldn't seek in movie!"));

    m_bIsBeingDragged = false;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::IsBeingDragged
//
// Returns true if the user is dragging the slider
// ----------------------------------------------------------------------------
bool wxMediaPlayerNotebookPage::IsBeingDragged()
{
    return m_bIsBeingDragged;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnVolChange
//
// Called when the user is done dragging the volume-changing slider
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnVolChange(wxScrollEvent& WXUNUSED(event))
{
    if( m_mediactrl->SetVolume(
            m_volSlider->GetValue() / 100.0
                                   ) == false )
        wxMessageBox(wxT("Couldn't set volume!"));

}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnPBChange
//
// Called when the user is done dragging the speed-changing slider
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnPBChange(wxScrollEvent& WXUNUSED(event))
{
    if( m_mediactrl->SetPlaybackRate(
            m_pbSlider->GetValue() * .25
                                   ) == false )
        wxMessageBox(wxT("Couldn't set playbackrate!"));

}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaPlay
//
// Called when the media plays.
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaPlay(wxMediaEvent& WXUNUSED(event))
{
    m_playlist->SetItem(m_nLastFileId, 0, wxT(">"));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaPause
//
// Called when the media is paused.
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaPause(wxMediaEvent& WXUNUSED(event))
{
    m_playlist->SetItem(m_nLastFileId, 0, wxT("||"));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaStop
//
// Called when the media stops.
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaStop(wxMediaEvent& WXUNUSED(event))
{
    m_playlist->SetItem(m_nLastFileId, 0, wxT("[]"));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaFinished
//
// Called when the media finishes playing.
// Here we loop it if the user wants to (has been selected from file menu)
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaFinished(wxMediaEvent& WXUNUSED(event))
{
    if(m_bLoop)
    {
        if ( !m_mediactrl->Play() )
        {
            wxMessageBox(wxT("Couldn't loop movie!"));
            m_playlist->SetItem(m_nLastFileId, 0, wxT("E"));
        }
        else
            ++m_nLoops;
    }
    else
    {
        m_playlist->SetItem(m_nLastFileId, 0, wxT("[]"));
    }
}

//
// End of MediaPlayer sample
//
