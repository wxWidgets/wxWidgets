/////////////////////////////////////////////////////////////////////////////
// Name:        samples/fswatcher/fswatcher.cpp
// Purpose:     wxFileSystemWatcher sample
// Author:      Bartosz Bekier
// Created:     2009-06-27
// Copyright:   (c) Bartosz Bekier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
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

    // Add an entry of the specified type asking the user for the filename if
    // the one passed to this function is empty.
    void AddEntry(wxFSWPathType type, wxString filename = wxString());

    bool CreateWatcherIfNecessary();

private:
    // file system watcher creation
    void CreateWatcher();

    // event handlers
    void OnClear(wxCommandEvent& WXUNUSED(event)) { m_evtConsole->Clear(); }
    void OnQuit(wxCommandEvent& WXUNUSED(event)) { Close(true); }
    void OnWatch(wxCommandEvent& event);
    void OnFollowLinks(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnAdd(wxCommandEvent& event);
    void OnAddTree(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    void OnRemoveAll(wxCommandEvent& WXUNUSED(event));
    void OnRemoveUpdateUI(wxUpdateUIEvent& event);
    void OnRemoveAllUpdateUI(wxUpdateUIEvent& event);

    void OnFileSystemEvent(wxFileSystemWatcherEvent& event);
    void LogEvent(const wxFileSystemWatcherEvent& event);

    wxTextCtrl *m_evtConsole;         // events console
    wxListView *m_filesList;          // list of watched paths
    wxFileSystemWatcher* m_watcher;   // file system watcher
    bool m_followLinks;               // should symlinks be dereferenced

    const static wxString LOG_FORMAT; // how to format events
};

const wxString MyFrame::LOG_FORMAT = " %-12s %-36s    %-36s";

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // 'Main program' equivalent: the program execution "starts" here
    virtual bool OnInit() wxOVERRIDE
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
    virtual void OnEventLoopEnter(wxEventLoopBase* WXUNUSED(loop)) wxOVERRIDE
    {
        if ( m_frame->CreateWatcherIfNecessary() )
        {
            if ( !m_dirToWatch.empty() )
                m_frame->AddEntry(wxFSWPath_Dir, m_dirToWatch);
        }
    }

    virtual void OnInitCmdLine(wxCmdLineParser& parser) wxOVERRIDE
    {
        wxApp::OnInitCmdLine(parser);
        parser.AddParam("directory to watch",
                        wxCMD_LINE_VAL_STRING,
                        wxCMD_LINE_PARAM_OPTIONAL);
    }

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) wxOVERRIDE
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
wxIMPLEMENT_APP(MyApp);


// ============================================================================
// implementation
// ============================================================================

// frame constructor
MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title),
      m_watcher(NULL), m_followLinks(false)
{
    SetIcon(wxICON(sample));

    // IDs for menu and buttons
    enum
    {
        MENU_ID_QUIT = wxID_EXIT,
        MENU_ID_CLEAR = wxID_CLEAR,
        MENU_ID_WATCH = 101,
        MENU_ID_DEREFERENCE,

        BTN_ID_ADD = 200,
        BTN_ID_ADD_TREE,
        BTN_ID_REMOVE,
        BTN_ID_REMOVE_ALL
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

#if defined(__UNIX__)
    // Let the user decide whether to dereference symlinks. If he makes the
    // wrong choice, asserts will occur if the symlink target is also watched
    it = menuMon->AppendCheckItem(MENU_ID_DEREFERENCE,
                                  "&Follow symlinks\tCtrl-F",
                                  _("If checked, dereference symlinks")
                                 );
    it->Check(false);
    Bind(wxEVT_MENU, &MyFrame::OnFollowLinks, this, MENU_ID_DEREFERENCE);
#endif // __UNIX__

    // the "About" item should be in the help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");

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
    wxButton* buttonAddTree = new wxButton(panel, BTN_ID_ADD_TREE, "Add &tree");
    wxButton* buttonRemove = new wxButton(panel, BTN_ID_REMOVE, "&Remove");
    wxButton* buttonRemoveAll = new wxButton(panel, BTN_ID_REMOVE_ALL, "Remove a&ll");
    wxSizer *btnSizer = new wxGridSizer(2);
    btnSizer->Add(buttonAdd, wxSizerFlags().Center().Border(wxALL));
    btnSizer->Add(buttonAddTree, wxSizerFlags().Center().Border(wxALL));
    btnSizer->Add(buttonRemove, wxSizerFlags().Center().Border(wxALL));
    btnSizer->Add(buttonRemoveAll, wxSizerFlags().Center().Border(wxALL));

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
    wxFont font(wxFontInfo(9).Family(wxFONTFAMILY_TELETYPE));
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
    Bind(wxEVT_MENU, &MyFrame::OnClear, this, MENU_ID_CLEAR);
    Bind(wxEVT_MENU, &MyFrame::OnQuit, this, MENU_ID_QUIT);
    Bind(wxEVT_MENU, &MyFrame::OnWatch, this, MENU_ID_WATCH);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);

    // buttons
    Bind(wxEVT_BUTTON, &MyFrame::OnAdd, this, BTN_ID_ADD);
    Bind(wxEVT_BUTTON, &MyFrame::OnAddTree, this, BTN_ID_ADD_TREE);
    Bind(wxEVT_BUTTON, &MyFrame::OnRemove, this, BTN_ID_REMOVE);
    Bind(wxEVT_UPDATE_UI, &MyFrame::OnRemoveUpdateUI, this, BTN_ID_REMOVE);
    Bind(wxEVT_BUTTON, &MyFrame::OnRemoveAll, this, BTN_ID_REMOVE_ALL);
    Bind(wxEVT_UPDATE_UI, &MyFrame::OnRemoveAllUpdateUI, this, BTN_ID_REMOVE_ALL);

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
    Bind(wxEVT_FSWATCHER, &MyFrame::OnFileSystemEvent, this);

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

void MyFrame::OnFollowLinks(wxCommandEvent& event)
{
    m_followLinks = event.IsChecked();
}

void MyFrame::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    AddEntry(wxFSWPath_Dir);
}

void MyFrame::OnAddTree(wxCommandEvent& WXUNUSED(event))
{
    AddEntry(wxFSWPath_Tree);
}

void MyFrame::AddEntry(wxFSWPathType type, wxString filename)
{
    if ( filename.empty() )
    {
        // TODO account for adding the files as well
        filename = wxDirSelector("Choose a folder to watch", "",
                                 wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if ( filename.empty() )
            return;
    }

    wxCHECK_RET(m_watcher, "Watcher not initialized");

    wxLogDebug("Adding %s: '%s'",
               filename,
               type == wxFSWPath_Dir ? "directory" : "directory tree");

    wxString prefix;
    bool ok = false;

    // This will tell wxFileSystemWatcher whether to dereference symlinks
    wxFileName fn = wxFileName::DirName(filename);
    if (!m_followLinks)
    {
        fn.DontFollowLink();
    }

    switch ( type )
    {
        case wxFSWPath_Dir:
            ok = m_watcher->Add(fn);
            prefix = "Dir:  ";
            break;

        case wxFSWPath_Tree:
            ok = m_watcher->AddTree(fn);
            prefix = "Tree: ";
            break;

        case wxFSWPath_File:
        case wxFSWPath_None:
            wxFAIL_MSG( "Unexpected path type." );
    }

    if (!ok)
    {
        wxLogError("Error adding '%s' to watched paths", filename);
        return;
    }

    // Prepend 'prefix' to the filepath, partly for display
    // but mostly so that OnRemove() can work out the correct way to remove it
    m_filesList->InsertItem(m_filesList->GetItemCount(),
                            prefix + wxFileName::DirName(filename).GetFullPath());
}

void MyFrame::OnRemove(wxCommandEvent& WXUNUSED(event))
{
    wxCHECK_RET(m_watcher, "Watcher not initialized");
    long idx = m_filesList->GetFirstSelected();
    if (idx == -1)
        return;

    bool ret = false;
    wxString path = m_filesList->GetItemText(idx).Mid(6);

    // This will tell wxFileSystemWatcher whether to dereference symlinks
    wxFileName fn = wxFileName::DirName(path);
    if (!m_followLinks)
    {
        fn.DontFollowLink();
    }

    // TODO we know it is a dir, but it doesn't have to be
    if (m_filesList->GetItemText(idx).StartsWith("Dir:  "))
    {
        ret = m_watcher->Remove(fn);
    }
    else if (m_filesList->GetItemText(idx).StartsWith("Tree: "))
    {
        ret = m_watcher->RemoveTree(fn);
    }
    else
    {
        wxFAIL_MSG("Unexpected item in wxListView.");
    }

    if (!ret)
    {
        wxLogError("Error removing '%s' from watched paths", path);
    }
    else
    {
        m_filesList->DeleteItem(idx);
    }
}

void MyFrame::OnRemoveAll(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_watcher->RemoveAll() )
    {
        wxLogError("Error removing all paths from watched paths");
    }

    m_filesList->DeleteAllItems();
}

void MyFrame::OnRemoveUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_filesList->GetFirstSelected() != wxNOT_FOUND);
}

void MyFrame::OnRemoveAllUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable( m_filesList->GetItemCount() != 0 );
}

void MyFrame::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    // TODO remove when code is rock-solid
    wxLogTrace(wxTRACE_FSWATCHER, "*** %s ***", event.ToString());
    LogEvent(event);

    int type = event.GetChangeType();
    if ((type == wxFSW_EVENT_DELETE) || (type == wxFSW_EVENT_RENAME))
    {
        // If path is one of our watched dirs, we need to react to this
        // otherwise there'll be asserts if later we try to remove it
        wxString eventpath = event.GetPath().GetFullPath();
        bool found(false);
        for (size_t n = m_filesList->GetItemCount(); n > 0; --n)
        {
            wxString path, foo = m_filesList->GetItemText(n-1);
            if ((!m_filesList->GetItemText(n-1).StartsWith("Dir:  ", &path)) &&
                (!m_filesList->GetItemText(n-1).StartsWith("Tree: ", &path)))
            {
                wxFAIL_MSG("Unexpected item in wxListView.");
            }
            if (path == eventpath)
            {
                if (type == wxFSW_EVENT_DELETE)
                {
                    m_filesList->DeleteItem(n-1);
                }
                else
                {
                    // At least in wxGTK, we'll never get here: renaming the top
                    // watched dir gives IN_MOVE_SELF and no new-name info.
                    // However I'll leave the code in case other platforms do
                    wxString newname = event.GetNewPath().GetFullPath();
                    if (newname.empty() ||
                        newname == event.GetPath().GetFullPath())
                    {
                        // Just in case either of these are possible...
                        wxLogTrace(wxTRACE_FSWATCHER,
                                   "Invalid attempt to rename to %s", newname);
                        return;
                    }
                    wxString prefix =
                        m_filesList->GetItemText(n-1).StartsWith("Dir:  ") ?
                                      "Dir:  " : "Tree: ";
                    m_filesList->SetItemText(n-1, prefix + newname);
                }
                found = true;
                // Don't break: a filepath may have been added more than once
            }
        }

        if (found)
        {
            wxString msg = wxString::Format(
                           "Your watched path %s has been deleted or renamed\n",
                           eventpath);
            m_evtConsole->AppendText(msg);
        }
    }
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
    case wxFSW_EVENT_ATTRIB:  // Currently this is wxGTK-only
        return "ATTRIBUTE";
#ifdef wxHAS_INOTIFY
    case wxFSW_EVENT_UNMOUNT: // Currently this is wxGTK-only
        return "UNMOUNT";
#endif
    case wxFSW_EVENT_WARNING:
        return "WARNING";
    case wxFSW_EVENT_ERROR:
        return "ERROR";
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
