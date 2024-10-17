///////////////////////////////////////////////////////////////////////////////
// Name:        mediaplayer.cpp
// Purpose:     wxMediaCtrl sample
// Author:      Ryan Norton
// Created:     11/10/04
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
// select the file you want to play - and MediaPlayer will play the file in
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
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ============================================================================
// Definitions
// ============================================================================

// ----------------------------------------------------------------------------
// Pre-compiled header stuff
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

#include "wx/mediactrl.h"   // for wxMediaCtrl
#include "wx/filedlg.h"     // for opening files from OpenFile
#include "wx/slider.h"      // for a slider for seeking within media
#include "wx/sizer.h"       // for positioning controls/wxBoxSizer
#include "wx/timer.h"       // timer for updating status bar
#include "wx/textdlg.h"     // for getting user text from OpenURL/Debug
#include "wx/notebook.h"    // for wxNotebook and putting movies in pages
#include "wx/cmdline.h"     // for wxCmdLineParser (optional)
#include "wx/listctrl.h"    // for wxListCtrl
#include "wx/dnd.h"         // drag and drop for the playlist
#include "wx/filename.h"    // For wxFileName::GetName()
#include "wx/config.h"      // for native wxConfig
#include "wx/vector.h"

// Under MSW we have several different backends but when linking statically
// they may be discarded by the linker (this definitely happens with MSVC) so
// force linking them. You don't have to do this in your code if you don't plan
// to use them, of course.
#if defined(__WXMSW__) && !defined(WXUSINGDLL)
    #include "wx/link.h"
    wxFORCE_LINK_MODULE(wxmediabackend_am)
    wxFORCE_LINK_MODULE(wxmediabackend_qt)
    wxFORCE_LINK_MODULE(wxmediabackend_wmp10)
#endif // static wxMSW build

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// Bail out if the user doesn't want one of the
// things we need
// ----------------------------------------------------------------------------

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
    virtual void MacOpenFiles(const wxArrayString & fileNames ) override;
#endif

#if wxUSE_CMDLINE_PARSER
    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;

    // Files specified on the command line, if any.
    wxVector<wxString> m_params;
#endif // wxUSE_CMDLINE_PARSER

    virtual bool OnInit() override;

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

    class wxMediaPlayerTimer* m_timer;     // Timer to write info to status bar
    wxNotebook* m_notebook;     // Notebook containing our pages

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
    bool IsBeingDragged();      // accessor for m_bIsBeingDragged

    // make wxMediaPlayerFrame able to access the private members
    friend class wxMediaPlayerFrame;

    int      m_nLastFileId;     // List ID of played file in listctrl
    wxString m_szFile;          // Name of currently playing file/location

    wxMediaCtrl* m_mediactrl;   // Our media control
    class wxMediaPlayerListCtrl* m_playlist;  // Our playlist
    wxSlider* m_slider;         // The slider below our media control
    wxSlider* m_pbSlider;       // Lower-left slider for adjusting speed
    wxSlider* m_volSlider;      // Lower-right slider for adjusting volume
    int m_nLoops;               // Number of times media has looped
    bool m_bLoop;               // Whether we are looping or not
    bool m_bIsBeingDragged;     // Whether the user is dragging the scroll bar
    wxMediaPlayerFrame* m_parentFrame;  // Main wxFrame of our sample
    wxButton* m_prevButton;     // Go to previous file button
    wxButton* m_playButton;     // Play/pause file button
    wxButton* m_stopButton;     // Stop playing file button
    wxButton* m_nextButton;     // Next file button
    wxButton* m_vdButton;       // Volume down button
    wxButton* m_vuButton;       // Volume up button
    wxGauge*  m_gauge;          // Gauge to keep in line with slider
};

// ----------------------------------------------------------------------------
// wxMediaPlayerTimer
// ----------------------------------------------------------------------------

class wxMediaPlayerTimer : public wxTimer
{
public:
    // Ctor
    wxMediaPlayerTimer(wxMediaPlayerFrame* frame) {m_frame = frame;}

    // Called each time the timer's timeout expires
    void Notify() override;

    wxMediaPlayerFrame* m_frame;       // The wxMediaPlayerFrame
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
        this->SetItem(nID, 0, "*");
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
//  Drop target for playlist (i.e. allows users to drag a file from explorer into
//  the playlist to add that file)
// ----------------------------------------------------------------------------
#if wxUSE_DRAG_AND_DROP
class wxPlayListDropTarget : public wxFileDropTarget
{
public:
    wxPlayListDropTarget(wxMediaPlayerListCtrl& list) : m_list(list) {}
    ~wxPlayListDropTarget(){}
        virtual bool OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                         const wxArrayString& files) override
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
const wxString wxGetMediaStateText(int nState)
{
    switch(nState)
    {
        case wxMEDIASTATE_PLAYING:
            return "Playing";
        case wxMEDIASTATE_STOPPED:
            return "Stopped";
        ///case wxMEDIASTATE_PAUSED:
        default:
            return "Paused";
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
// wxIMPLEMENT_APP does this, and also implements the platform-specific entry
// routine, such as main or WinMain().  Use wxIMPLEMENT_APP_NO_MAIN if you do
// not desire this behaviour.
// ----------------------------------------------------------------------------
wxIMPLEMENT_APP(wxMediaPlayerApp);

// ----------------------------------------------------------------------------
// wxMediaPlayerApp command line parsing
// ----------------------------------------------------------------------------

#if wxUSE_CMDLINE_PARSER

void wxMediaPlayerApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddParam("input files",
                    wxCMD_LINE_VAL_STRING,
                    wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE);
}

bool wxMediaPlayerApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if ( !wxApp::OnCmdLineParsed(parser) )
        return false;

    for (size_t paramNr=0; paramNr < parser.GetParamCount(); ++paramNr)
        m_params.push_back(parser.GetParam(paramNr));

    return true;
}

#endif // wxUSE_CMDLINE_PARSER

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
    if ( !wxApp::OnInit() )
        return false;

    // SetAppName() lets wxConfig and others know where to write
    SetAppName("wxMediaPlayer");

    wxMediaPlayerFrame *frame =
        new wxMediaPlayerFrame("MediaPlayer wxWidgets Sample");
    frame->Show(true);

#if wxUSE_CMDLINE_PARSER
    if ( !m_params.empty() )
    {
        for ( size_t n = 0; n < m_params.size(); n++ )
            frame->AddToPlayList(m_params[n]);

        wxCommandEvent theEvent(wxEVT_MENU, wxID_NEXT);
        frame->AddPendingEvent(theEvent);
    }
#endif // wxUSE_CMDLINE_PARSER

    return true;
}

#ifdef __WXMAC__

void wxMediaPlayerApp::MacOpenFiles(const wxArrayString & fileNames )
{
    // Called when a user drags files over our app
    m_frame->DoOpenFile(fileNames[0], true /* new page */);
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
       : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1366,768))
{
    SetIcon(wxICON(sample));

    //
    //  Create Menus
    //
    wxMenu *fileMenu = new wxMenu;
    wxMenu *controlsMenu = new wxMenu;
    wxMenu *optionsMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *debugMenu = new wxMenu;

    fileMenu->Append(wxID_OPENFILESAMEPAGE, "&Open File\tCtrl-Shift-O",
                        "Open a File in the current notebook page");
    fileMenu->Append(wxID_OPENFILENEWPAGE, "&Open File in a new page",
                        "Open a File in a new notebook page");
    fileMenu->Append(wxID_OPENURLSAMEPAGE, "&Open URL",
                        "Open a URL in the current notebook page");
    fileMenu->Append(wxID_OPENURLNEWPAGE, "&Open URL in a new page",
                        "Open a URL in a new notebook page");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_CLOSECURRENTPAGE, "&Close Current Page\tCtrl-C",
                        "Close current notebook page");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT,
                     "E&xit\tAlt-X",
                     "Quit this program");

    controlsMenu->Append(wxID_PLAY, "&Play/Pause\tCtrl-P", "Resume/Pause playback");
    controlsMenu->Append(wxID_STOP, "&Stop\tCtrl-S", "Stop playback");
    controlsMenu->AppendSeparator();
    controlsMenu->Append(wxID_PREV, "&Previous\tCtrl-B", "Go to previous track");
    controlsMenu->Append(wxID_NEXT, "&Next\tCtrl-N", "Skip to next track");

    optionsMenu->AppendCheckItem(wxID_LOOP,
                              "&Loop\tCtrl-L",
                              "Loop Selected Media");
    optionsMenu->AppendCheckItem(wxID_SHOWINTERFACE,
                              "&Show Interface\tCtrl-I",
                              "Show wxMediaCtrl native controls");

    debugMenu->Append(wxID_SELECTBACKEND,
                     "&Select Backend...\tCtrl-D",
                     "Select a backend manually");

    helpMenu->Append(wxID_ABOUT,
                     "&About\tF1",
                     "Show about dialog");


    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(controlsMenu, "&Controls");
    menuBar->Append(optionsMenu, "&Options");
    menuBar->Append(debugMenu, "&Debug");
    menuBar->Append(helpMenu, "&Help");
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

    //
    // Menu events
    //
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnQuit, this,
         wxID_EXIT);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnAbout, this,
         wxID_ABOUT);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnLoop, this,
         wxID_LOOP);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnShowInterface, this,
         wxID_SHOWINTERFACE);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnOpenFileNewPage, this,
         wxID_OPENFILENEWPAGE);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnOpenFileSamePage, this,
         wxID_OPENFILESAMEPAGE);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnOpenURLNewPage, this,
         wxID_OPENURLNEWPAGE);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnOpenURLSamePage, this,
         wxID_OPENURLSAMEPAGE);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnCloseCurrentPage, this,
         wxID_CLOSECURRENTPAGE);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnPlay, this,
         wxID_PLAY);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnStop, this,
         wxID_STOP);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnNext, this,
         wxID_NEXT);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnPrev, this,
         wxID_PREV);
    Bind(wxEVT_MENU, &wxMediaPlayerFrame::OnSelectBackend, this,
         wxID_SELECTBACKEND);

    //
    // Key events
    //
    wxTheApp->Bind(wxEVT_KEY_DOWN, &wxMediaPlayerFrame::OnKeyDown, this);

    //
    // Close events
    //
    Bind(wxEVT_CLOSE_WINDOW, &wxMediaPlayerFrame::OnClose, this);

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
                        "",
                        true);


    // Don't load previous files if we have some specified on the command line,
    // we wouldn't play them otherwise (they'd have to be inserted into the
    // play list at the beginning instead of being appended but we don't
    // support this).
#if wxUSE_CMDLINE_PARSER
    if ( wxGetApp().m_params.empty() )
#endif // wxUSE_CMDLINE_PARSER
    {
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
    event.Skip(); // really close the frame
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
    msg.Printf( "This is a test of wxMediaCtrl.\n\n"

                "Instructions:\n"

                "The top slider shows the current the current position, "
                "which you can change by dragging and releasing it.\n"

                "The gauge (progress bar) shows the progress in "
                "downloading data of the current file - it may always be "
                "empty due to lack of support from the current backend.\n"

                "The lower-left slider controls the volume and the lower-"
                "right slider controls the playback rate/speed of the "
                "media\n\n"

                "Currently using: %s", wxVERSION_STRING);

    wxMessageBox(msg, "About wxMediaCtrl test",
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
        // error - uncheck and warn user
        wxMenuItem* pSIItem = GetMenuBar()->FindItem(wxID_SHOWINTERFACE);
        wxASSERT(pSIItem);
        pSIItem->Check(!event.IsChecked());

        if(event.IsChecked())
            wxMessageBox("Could not show player controls");
        else
            wxMessageBox("Could not hide player controls");
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
    currentpage->m_playlist->SetItem(nID, 0, "*");
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
// Pauses the file if it's the currently playing file,
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
                wxMessageBox("Couldn't pause movie!");
        }
        else
        {
            if( !currentpage->m_mediactrl->Play() )
                wxMessageBox("Couldn't play movie!");
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
                    currentpage->m_nLastFileId, 0, "*");

        wxURI uripath(path);
        if( uripath.IsReference() )
        {
            if( !currentpage->m_mediactrl->Load(path) )
            {
                wxMessageBox("Couldn't load file!");
                currentpage->m_playlist->SetItem(nNewId, 0, "E");
            }
            else
            {
                currentpage->m_playlist->SetItem(nNewId, 0, "O");
            }
        }
        else
        {
            if( !currentpage->m_mediactrl->Load(uripath) )
            {
                wxMessageBox("Couldn't load URL!");
                currentpage->m_playlist->SetItem(nNewId, 0, "E");
            }
            else
            {
                currentpage->m_playlist->SetItem(nNewId, 0, "O");
            }
        }

        currentpage->m_nLastFileId = nNewId;
        currentpage->m_szFile = path;
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId,
                                         1, wxFileName(path).GetName());
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId,
                                         2, "");
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
            wxMessageBox("Couldn't play movie!");
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId, 0, "E");
    }
    else
    {
        currentpage->m_playlist->SetItem(currentpage->m_nLastFileId, 0, ">");
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
    wxString sBackend = wxGetTextFromUser("Enter backend to use");

    if(sBackend.empty() == false)  // could have been cancelled by the user
    {
        int sel = m_notebook->GetSelection();

        if (sel != wxNOT_FOUND)
        {
            m_notebook->DeletePage(sel);
        }

        m_notebook->AddPage(new wxMediaPlayerNotebookPage(this, m_notebook,
                                                        sBackend
                                                        ), "", true);

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
        "Enter the URL that has the movie to play"
                                     );

    if(sUrl.empty() == false) // could have been cancelled by user
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
        wxMessageBox("Cannot close main page");
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
            // no items in list
            wxMessageBox("No items in playlist!");
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
       // delete all selected items
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

   // Could be wxGetTextFromUser or something else important
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
        wxMessageBox("Couldn't stop movie!");
    else
        currentpage->m_playlist->SetItem(
            currentpage->m_nLastFileId, 0, "[]");
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
        wxMessageBox("No selected item!");
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
        // nothing selected, default to the file before the currently playing one
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
        return; // already playing... nothing to do

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
        return; // already playing... nothing to do

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

    // Number of minutes/seconds total
    wxLongLong llLength = currentpage->m_mediactrl->Length();
    int nMinutes = (int) (llLength / 60000).GetValue();
    int nSeconds = (int) ((llLength % 60000)/1000).GetValue();

    // Duration string (i.e. MM:SS)
    wxString sDuration;
    sDuration.Printf("%2i:%02i", nMinutes, nSeconds);


    // Number of minutes/seconds total
    wxLongLong llTell = currentpage->m_mediactrl->Tell();
    nMinutes = (int) (llTell / 60000).GetValue();
    nSeconds = (int) ((llTell % 60000)/1000).GetValue();

    // Position string (i.e. MM:SS)
    wxString sPosition;
    sPosition.Printf("%2i:%02i", nMinutes, nSeconds);


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
                    "Size(x,y):%i,%i "
                    "Position:%s/%s Speed:%1.1fx "
                    "State:%s Loops:%i D/T:[%i]/[%i] V:%i%%",
                    videoSize.x,
                    videoSize.y,
                    sPosition,
                    sDuration,
                    currentMediaCtrl->GetPlaybackRate(),
                    wxGetMediaStateText(currentpage->m_mediactrl->GetState()),
                    currentpage->m_nLoops,
                    (int)llDownloadProgress.GetValue(),
                    (int)llDownloadTotal.GetValue(),
                    (int)(currentpage->m_mediactrl->GetVolume() * 100)));
#endif // wxUSE_STATUSBAR
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
    //  Create and attach a 2-column grid sizer
    //
    wxFlexGridSizer* sizer = new wxFlexGridSizer(2);
    sizer->AddGrowableCol(0);
    this->SetSizer(sizer);

    //
    //  Create our media control
    //
    m_mediactrl = new wxMediaCtrl();

    //  Make sure creation was successful
    bool bOK = m_mediactrl->Create(this, wxID_MEDIACTRL, wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxMC_NO_AUTORESIZE,
// you could specify a macro backend here like
//  wxMEDIABACKEND_WMP10);
//        "wxPDFMediaBackend");
                                   szBackend);
// you could change the cursor here like
//    m_mediactrl->SetCursor(wxCURSOR_BLANK);
// note that this may not affect it if SetPlayerControls
// is set to something else than wxMEDIACTRLPLAYERCONTROLS_NONE
    wxASSERT_MSG(bOK, "Could not create media control!");
    wxUnusedVar(bOK);

    sizer->Add(m_mediactrl, wxSizerFlags().Expand().Border());

    //
    //  Create the playlist/listctrl
    //
    m_playlist = new wxMediaPlayerListCtrl();
    m_playlist->Create(this, wxID_LISTCTRL, wxDefaultPosition,
                    wxDefaultSize,
                    wxLC_REPORT // wxLC_LIST
                    | wxSUNKEN_BORDER);

    //  Set the background of our listctrl to white
    m_playlist->SetBackgroundColour(*wxWHITE);

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
    m_playlist->AppendColumn(_(""), wxLIST_FORMAT_CENTER, 20);
    m_playlist->AppendColumn(_("File"), wxLIST_FORMAT_LEFT, /*wxLIST_AUTOSIZE_USEHEADER*/305);
    m_playlist->AppendColumn(_("Length"), wxLIST_FORMAT_CENTER, 75);

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

    m_prevButton->Create(this, wxID_BUTTONPREV, "|<");
    m_prevButton->SetToolTip("Previous");
    m_playButton->Create(this, wxID_BUTTONPLAY, ">");
    m_playButton->SetToolTip("Play");
    m_stopButton->Create(this, wxID_BUTTONSTOP, "[]");
    m_stopButton->SetToolTip("Stop");
    m_nextButton->Create(this, wxID_BUTTONNEXT, ">|");
    m_nextButton->SetToolTip("Next");
    m_vdButton->Create(this, wxID_BUTTONVD, "((");
    m_vdButton->SetToolTip("Volume down");
    m_vuButton->Create(this, wxID_BUTTONVU, "))");
    m_vuButton->SetToolTip("Volume up");

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
    m_slider = new wxSlider(this, wxID_SLIDER, 0, // init
                            0, // start
                            1, // end, dummy but must be greater than start
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
                            wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL );
    horsizer3->Add(m_volSlider, 1, wxALL, 5);

    m_pbSlider = new wxSlider(this, wxID_PBSLIDER, 4, // init
                            1, // start
                            16, // end
                            wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL );
    horsizer3->Add(m_pbSlider, 1, wxALL, 5);
    sizer->Add(horsizer3, 1, wxCENTRE | wxALL, 5);

    // Now that we have all our rows make some of them growable
    sizer->AddGrowableRow(0);

    //
    // ListCtrl events
    //
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &wxMediaPlayerFrame::OnChangeSong, parentFrame,
         wxID_LISTCTRL);

    //
    // Slider events
    //
    Bind(wxEVT_SCROLL_THUMBTRACK, &wxMediaPlayerNotebookPage::OnBeginSeek, this,
         wxID_SLIDER);
    Bind(wxEVT_SCROLL_THUMBRELEASE, &wxMediaPlayerNotebookPage::OnEndSeek, this,
         wxID_SLIDER);
    Bind(wxEVT_SCROLL_THUMBRELEASE, &wxMediaPlayerNotebookPage::OnPBChange, this,
         wxID_PBSLIDER);
    Bind(wxEVT_SCROLL_THUMBRELEASE, &wxMediaPlayerNotebookPage::OnVolChange, this,
         wxID_VOLSLIDER);

    //
    // Media Control events
    //
    Bind(wxEVT_MEDIA_PLAY, &wxMediaPlayerNotebookPage::OnMediaPlay, this,
         wxID_MEDIACTRL);
    Bind(wxEVT_MEDIA_PAUSE, &wxMediaPlayerNotebookPage::OnMediaPause, this,
         wxID_MEDIACTRL);
    Bind(wxEVT_MEDIA_STOP, &wxMediaPlayerNotebookPage::OnMediaStop, this,
         wxID_MEDIACTRL);
    Bind(wxEVT_MEDIA_FINISHED, &wxMediaPlayerNotebookPage::OnMediaFinished, this,
         wxID_MEDIACTRL);
    Bind(wxEVT_MEDIA_LOADED, &wxMediaPlayerFrame::OnMediaLoaded, parentFrame,
         wxID_MEDIACTRL);

    //
    // Button events
    //
    Bind(wxEVT_BUTTON, &wxMediaPlayerFrame::OnPrev, parentFrame,
         wxID_BUTTONPREV);
    Bind(wxEVT_BUTTON, &wxMediaPlayerFrame::OnPlay, parentFrame,
         wxID_BUTTONPLAY);
    Bind(wxEVT_BUTTON, &wxMediaPlayerFrame::OnStop, parentFrame,
         wxID_BUTTONSTOP);
    Bind(wxEVT_BUTTON, &wxMediaPlayerFrame::OnNext, parentFrame,
         wxID_BUTTONNEXT);
    Bind(wxEVT_BUTTON, &wxMediaPlayerFrame::OnVolumeDown, parentFrame,
         wxID_BUTTONVD);
    Bind(wxEVT_BUTTON, &wxMediaPlayerFrame::OnVolumeUp, parentFrame,
         wxID_BUTTONVU);
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
        wxMessageBox("Couldn't seek in movie!");

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
        wxMessageBox("Couldn't set volume!");

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
        wxMessageBox("Couldn't set playbackrate!");

}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaPlay
//
// Called when the media plays.
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaPlay(wxMediaEvent& WXUNUSED(event))
{
    m_playlist->SetItem(m_nLastFileId, 0, ">");
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaPause
//
// Called when the media is paused.
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaPause(wxMediaEvent& WXUNUSED(event))
{
    m_playlist->SetItem(m_nLastFileId, 0, "||");
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::OnMediaStop
//
// Called when the media stops.
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaStop(wxMediaEvent& WXUNUSED(event))
{
    m_playlist->SetItem(m_nLastFileId, 0, "[]");
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
            wxMessageBox("Couldn't loop movie!");
            m_playlist->SetItem(m_nLastFileId, 0, "E");
        }
        else
            ++m_nLoops;
    }
    else
    {
        m_playlist->SetItem(m_nLastFileId, 0, "[]");
    }
}

//
// End of MediaPlayer sample
//
