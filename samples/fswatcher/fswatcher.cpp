/////////////////////////////////////////////////////////////////////////////
// Name:        samples/fswatcher/fswatcher.cpp
// Purpose:     wxFileSystemWatcher sample
// Author:      Bartosz Bekier
// Created:     2009-06-27
// RCS-ID:      $Id$
// Copyright:   (c) Bartosz Bekier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "wx/fswatcher.h"
#include "wx/listctrl.h"
#include "wx/cmdline.h"

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

    void AddDirectory(const wxString& dir);

    bool CreateWatcherIfNecessary();

private:
    // file system watcher creation
    void CreateWatcher();

    // event handlers
    void OnClear(wxCommandEvent& WXUNUSED(event)) { m_evtConsole->Clear(); }
    void OnQuit(wxCommandEvent& WXUNUSED(event)) { Close(true); }
    void OnWatch(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnAdd(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);

    void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
    void LogEvent(const wxFileSystemWatcherEvent& event);

    wxTextCtrl *m_evtConsole;         // events console
    wxListView *m_filesList;          // list of watched paths
    wxFileSystemWatcher* m_watcher;   // file system watcher

    const static wxString LOG_FORMAT; // how to format events
};

const wxString MyFrame::LOG_FORMAT = " %-12s %-36s    %-36s";

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // 'Main program' equivalent: the program execution "starts" here
    virtual bool OnInit()
    {
        if ( !wxApp::OnInit() )
            return false;

        wxLog::AddTraceMask("EventSource");
        wxLog::AddTraceMask(wxTRACE_FSWATCHER);

        // create the main application window
        m_frame = new MyFrame("File System Watcher wxWidgets App");

        // If we returned false here, the application would exit immediately.
        return true;
    }

    // create the file system watcher here, because it needs an active loop
    virtual void OnEventLoopEnter(wxEventLoopBase* WXUNUSED(loop))
    {
        if ( m_frame->CreateWatcherIfNecessary() )
        {
            if ( !m_dirToWatch.empty() )
                m_frame->AddDirectory(m_dirToWatch);
        }
    }

    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        wxApp::OnInitCmdLine(parser);
        parser.AddParam("directory to watch",
                        wxCMD_LINE_VAL_STRING,
                        wxCMD_LINE_PARAM_OPTIONAL);
    }

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser)
    {
        if ( !wxApp::OnCmdLineParsed(parser) )
            return false;

        if ( parser.GetParamCount() )
            m_dirToWatch = parser.GetParam();

        return true;
    }

private:
    MyFrame *m_frame;

    // The directory to watch if specified on the command line.
    wxString m_dirToWatch;
};

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)


// ============================================================================
// implementation
// ============================================================================

// frame constructor
MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title),
      m_watcher(NULL)
{
    SetIcon(wxICON(sample));

    // IDs for menu and buttons
    enum
    {
        MENU_ID_QUIT = wxID_EXIT,
        MENU_ID_CLEAR = wxID_CLEAR,
        MENU_ID_WATCH = 101,

        BTN_ID_ADD = 200,
        BTN_ID_REMOVE = 201,
    };

    // ================================================================
    // menu

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(MENU_ID_CLEAR, "&Clear log\tCtrl-L");
    menuFile->AppendSeparator();
    menuFile->Append(MENU_ID_QUIT, "E&xit\tAlt-X", "Quit this program");

    // "Watch" menu
    wxMenu *menuMon = new wxMenu;
    wxMenuItem* it = menuMon->AppendCheckItem(MENU_ID_WATCH, "&Watch\tCtrl-W");
    // started by default, because file system watcher is started by default
    it->Check(true);

    // the "About" item should be in the help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, "&About...\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuMon, "&Watch");
    menuBar->Append(menuHelp, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // ================================================================
    // upper panel

    // panel
    wxPanel *panel = new wxPanel(this);
    wxSizer *panelSizer = new wxGridSizer(2);
    wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

    // label
    wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Watched paths");
    leftSizer->Add(label, wxSizerFlags().Center().Border(wxALL));

    // list of files
    m_filesList = new wxListView(panel, wxID_ANY, wxPoint(-1,-1),
                                 wxSize(300,200), wxLC_LIST | wxLC_SINGLE_SEL);
    leftSizer->Add(m_filesList, wxSizerFlags(1).Expand());

    // buttons
    wxButton* buttonAdd = new wxButton(panel, BTN_ID_ADD, "&Add");
    wxButton* buttonRemove = new wxButton(panel, BTN_ID_REMOVE, "&Remove");
    wxSizer *btnSizer = new wxGridSizer(2);
    btnSizer->Add(buttonAdd, wxSizerFlags().Center().Border(wxALL));
    btnSizer->Add(buttonRemove, wxSizerFlags().Center().Border(wxALL));

    // and put it all together
    leftSizer->Add(btnSizer, wxSizerFlags(0).Expand());
    panelSizer->Add(leftSizer, wxSizerFlags(1).Expand());
    panel->SetSizerAndFit(panelSizer);

    // ================================================================
    // lower panel

    wxTextCtrl *headerText = new wxTextCtrl(this, wxID_ANY, "",
                                            wxDefaultPosition, wxDefaultSize,
                                            wxTE_READONLY);
    wxString h = wxString::Format(LOG_FORMAT, "event", "path", "new path");
    headerText->SetValue(h);

    // event console
    m_evtConsole = new wxTextCtrl(this, wxID_ANY, "",
                               wxDefaultPosition, wxSize(200,200),
                               wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);

    // set monospace font to have output in nice columns
    wxFont font(9, wxFONTFAMILY_TELETYPE,
                wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    headerText->SetFont(font);
    m_evtConsole->SetFont(font);

    // ================================================================
    // laying out whole frame

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(panel, wxSizerFlags(1).Expand());
    sizer->Add(headerText, wxSizerFlags().Expand());
    sizer->Add(m_evtConsole, wxSizerFlags(1).Expand());
    SetSizerAndFit(sizer);

    // set size and position on screen
    SetSize(800, 600);
    CentreOnScreen();

    // ================================================================
    // event handlers & show

    // menu
    Connect(MENU_ID_CLEAR, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MyFrame::OnClear));
    Connect(MENU_ID_QUIT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MyFrame::OnQuit));
    Connect(MENU_ID_WATCH, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MyFrame::OnWatch));
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MyFrame::OnAbout));

    // buttons
    Connect(BTN_ID_ADD, wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MyFrame::OnAdd));
    Connect(BTN_ID_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MyFrame::OnRemove));

    // and show itself (the frames, unlike simple controls, are not shown when
    // created initially)
    Show(true);
}

MyFrame::~MyFrame()
{
    delete m_watcher;
}

bool MyFrame::CreateWatcherIfNecessary()
{
    if (m_watcher)
        return false;

    CreateWatcher();
    Connect(wxEVT_FSWATCHER,
            wxFileSystemWatcherEventHandler(MyFrame::OnFileSystemEvent));

    return true;
}

void MyFrame::CreateWatcher()
{
    wxCHECK_RET(!m_watcher, "Watcher already initialized");
    m_watcher = new wxFileSystemWatcher();
    m_watcher->SetOwner(this);
}

// ============================================================================
// event handlers
// ============================================================================

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Demonstrates the usage of file system watcher, "
                 "the wxWidgets monitoring system notifying you of "
                 "changes done to your files.\n"
                 "(c) 2009 Bartosz Bekier\n",
                 "About wxWidgets File System Watcher Sample",
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnWatch(wxCommandEvent& event)
{
    wxLogDebug("%s start=%d", __WXFUNCTION__, event.IsChecked());

    if (event.IsChecked())
    {
        wxCHECK_RET(!m_watcher, "Watcher already initialized");
        CreateWatcher();
    }
    else
    {
        wxCHECK_RET(m_watcher, "Watcher not initialized");
        m_filesList->DeleteAllItems();
        wxDELETE(m_watcher);
    }
}

void MyFrame::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    wxCHECK_RET(m_watcher, "Watcher not initialized");

    // TODO account for adding the files as well
    const wxString& dir = wxDirSelector("Choose a folder to watch", "",
                                    wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if ( dir.empty() )
        return;

    AddDirectory(dir);
}

void MyFrame::AddDirectory(const wxString& dir)
{
    wxLogDebug("Adding directory: '%s'", dir);

    if (!m_watcher->Add(wxFileName::DirName(dir), wxFSW_EVENT_ALL))
    {
        wxLogError("Error adding '%s' to watched paths", dir);
    }
    else
    {
        m_filesList->InsertItem(m_filesList->GetItemCount(), dir);
    }
}

void MyFrame::OnRemove(wxCommandEvent& WXUNUSED(event))
{
    wxCHECK_RET(m_watcher, "Watcher not initialized");
    long idx = m_filesList->GetFirstSelected();
    if (idx == -1)
        return;

    wxString path = m_filesList->GetItemText(idx);

    // TODO we know it is a dir, but it doesn't have to be
    if (!m_watcher->Remove(wxFileName::DirName(path)))
    {
        wxLogError("Error removing '%s' from watched paths", path);
    }
    else
    {
        m_filesList->DeleteItem(idx);
    }
}

void MyFrame::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    // TODO remove when code is rock-solid
    wxLogTrace(wxTRACE_FSWATCHER, "*** %s ***", event.ToString());
    LogEvent(event);
}


static wxString GetFSWEventChangeTypeName(int changeType)
{
    switch (changeType)
    {
    case wxFSW_EVENT_CREATE:
        return "CREATE";
    case wxFSW_EVENT_DELETE:
        return "DELETE";
    case wxFSW_EVENT_RENAME:
        return "RENAME";
    case wxFSW_EVENT_MODIFY:
        return "MODIFY";
    case wxFSW_EVENT_ACCESS:
        return "ACCESS";
    }

    return "INVALID_TYPE";
}

void MyFrame::LogEvent(const wxFileSystemWatcherEvent& event)
{
    wxString entry = wxString::Format(LOG_FORMAT + "\n",
                            GetFSWEventChangeTypeName(event.GetChangeType()),
                            event.GetPath().GetFullPath(),
                            event.GetNewPath().GetFullPath());
    m_evtConsole->AppendText(entry);
}
