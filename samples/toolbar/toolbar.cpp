/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/toolbar.h>
#include <wx/log.h>

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/preview.xpm"  // paste XPM
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif // GTK or Motif

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent,
            wxWindowID id = -1,
            const wxString& title = "wxToolBar Sample",
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);

    void RecreateToolbar();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnToggleToolbarSize(wxCommandEvent& event);
    void OnToggleToolbarOrient(wxCommandEvent& event);

    void OnEnablePrint(wxCommandEvent& event) { DoEnablePrint(); }
    void OnDeletePrint(wxCommandEvent& event) { DoDeletePrint(); }
    void OnInsertPrint(wxCommandEvent& event);
    void OnToggleHelp(wxCommandEvent& event) { DoToggleHelp(); }

    void OnToolLeftClick(wxCommandEvent& event);
    void OnToolEnter(wxCommandEvent& event);

    void OnCombo(wxCommandEvent& event);

    void OnUpdateCopyAndCut(wxUpdateUIEvent& event);

private:
    void DoEnablePrint();
    void DoDeletePrint();
    void DoToggleHelp();

    bool                m_smallToolbar,
                        m_horzToolbar;
    wxTextCtrl*         m_textWindow;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;

enum
{
    IDM_TOOLBAR_TOGGLETOOLBARSIZE = 200,
    IDM_TOOLBAR_TOGGLETOOLBARORIENT,
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_DELETEPRINT,
    IDM_TOOLBAR_INSERTPRINT,
    IDM_TOOLBAR_TOGGLEHELP,

    ID_COMBO = 1000
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

// Notice that wxID_HELP will be processed for the 'About' menu and the toolbar
// help button.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_HELP, MyFrame::OnAbout)

    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARSIZE, MyFrame::OnToggleToolbarSize)
    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARORIENT, MyFrame::OnToggleToolbarOrient)

    EVT_MENU(IDM_TOOLBAR_ENABLEPRINT, MyFrame::OnEnablePrint)
    EVT_MENU(IDM_TOOLBAR_DELETEPRINT, MyFrame::OnDeletePrint)
    EVT_MENU(IDM_TOOLBAR_INSERTPRINT, MyFrame::OnInsertPrint)
    EVT_MENU(IDM_TOOLBAR_TOGGLEHELP, MyFrame::OnToggleHelp)

    EVT_MENU(-1, MyFrame::OnToolLeftClick)

    EVT_COMBOBOX(ID_COMBO, MyFrame::OnCombo)

    EVT_TOOL_ENTER(ID_TOOLBAR, MyFrame::OnToolEnter)

    EVT_UPDATE_UI(wxID_COPY, MyFrame::OnUpdateCopyAndCut)
    EVT_UPDATE_UI(wxID_CUT, MyFrame::OnUpdateCopyAndCut)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame* frame = new MyFrame((wxFrame *) NULL, -1,
                                 "wxToolBar Sample",
                                 wxPoint(100, 100), wxSize(450, 300));

    // VZ: what's this for??
#if 0
    // Force a resize. This should probably be replaced by a call to a wxFrame
    // function that lays out default decorations and the remaining content window.
    wxSizeEvent event(wxSize(-1, -1), frame->GetId());
    frame->OnSize(event);
#endif // 0

    frame->Show(TRUE);

    frame->SetStatusText("Hello, wxWindows");

    SetTopWindow(frame);

    return TRUE;
}

void MyFrame::RecreateToolbar()
{
    // delete and recreate the toolbar
    wxToolBar *toolBar = GetToolBar();
    delete toolBar;

    SetToolBar(NULL);

    long style = wxNO_BORDER | wxTB_FLAT | wxTB_DOCKABLE;
    style |= m_horzToolbar ? wxTB_HORIZONTAL : wxTB_VERTICAL;

    toolBar = CreateToolBar(style, ID_TOOLBAR);
    toolBar->SetMargins( 4, 4 );

    // Set up toolbar
    wxBitmap toolBarBitmaps[8];

    toolBarBitmaps[0] = wxBITMAP(new);
    toolBarBitmaps[1] = wxBITMAP(open);
    if ( !m_smallToolbar )
    {
        toolBarBitmaps[2] = wxBITMAP(save);
        toolBarBitmaps[3] = wxBITMAP(copy);
        toolBarBitmaps[4] = wxBITMAP(cut);
        toolBarBitmaps[5] = wxBITMAP(paste);
        toolBarBitmaps[6] = wxBITMAP(print);
        toolBarBitmaps[7] = wxBITMAP(help);
    }

#ifdef __WXMSW__
    int width = 24;
#else
    int width = 16;
#endif

    int currentX = 5;

    toolBar->AddTool(wxID_NEW, toolBarBitmaps[0], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "New file");
    currentX += width + 5;
    toolBar->AddTool(wxID_OPEN, toolBarBitmaps[1], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Open file");

    // adding a combo to a vertical toolbar is not very smart
    if ( m_horzToolbar )
    {
        wxComboBox *combo = new wxComboBox(toolBar, ID_COMBO);
        combo->Append("This");
        combo->Append("is a");
        combo->Append("combobox");
        combo->Append("in a");
        combo->Append("toolbar");
        toolBar->AddControl(combo);
    }

    if ( !m_smallToolbar )
    {
        currentX += width + 5;
        toolBar->AddTool(wxID_SAVE, toolBarBitmaps[2], wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Toggle button 1");
        currentX += width + 5;
        toolBar->AddTool(wxID_COPY, toolBarBitmaps[3], wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Toggle button 2");
        currentX += width + 5;
        toolBar->AddTool(wxID_CUT, toolBarBitmaps[4], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Toggle/Untoggle help button");
        currentX += width + 5;
        toolBar->AddTool(wxID_PASTE, toolBarBitmaps[5], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Paste");
        currentX += width + 5;
        toolBar->AddTool(wxID_PRINT, toolBarBitmaps[6], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Delete this tool");
        currentX += width + 5;
        toolBar->AddSeparator();
        toolBar->AddTool(wxID_HELP, toolBarBitmaps[7], wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Help button");
    }

    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    toolBar->Realize();
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
       : wxFrame(parent, id, title, pos, size, style)
{
    m_textWindow = new wxTextCtrl(this, -1, "", wxPoint(0, 0), wxSize(-1, -1), wxTE_MULTILINE);
    m_smallToolbar = FALSE;
    m_horzToolbar = FALSE;

    // Give it a status line
    CreateStatusBar();

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *tbarMenu = new wxMenu;
    tbarMenu->Append(IDM_TOOLBAR_TOGGLETOOLBARSIZE,
                     "&Toggle toolbar size\tCtrl-S",
                     "Toggle between big/small toolbar",
                     TRUE);
    tbarMenu->Append(IDM_TOOLBAR_TOGGLETOOLBARORIENT,
                     "Toggle toolbar &orientation\tCtrl-O",
                     "Toggle toolbar orientation",
                     TRUE);

    tbarMenu->AppendSeparator();

    tbarMenu->Append(IDM_TOOLBAR_ENABLEPRINT, "&Enable print button\tCtrl-E", "");
    tbarMenu->Append(IDM_TOOLBAR_DELETEPRINT, "&Delete print button\tCtrl-D", "");
    tbarMenu->Append(IDM_TOOLBAR_INSERTPRINT, "&Insert print button\tCtrl-I", "");
    tbarMenu->Append(IDM_TOOLBAR_TOGGLEHELP, "Toggle &help button\tCtrl-T", "");

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit", "Quit toolbar sample" );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, "&About", "About toolbar sample");

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(tbarMenu, "&Toolbar");
    menuBar->Append(helpMenu, "&Help");

    // Associate the menu bar with the frame
    SetMenuBar(menuBar);

    // Create the toolbar
    RecreateToolbar();
}

void MyFrame::OnToggleToolbarSize(wxCommandEvent& WXUNUSED(event))
{
    m_smallToolbar = !m_smallToolbar;

    RecreateToolbar();
}

void MyFrame::OnToggleToolbarOrient(wxCommandEvent& WXUNUSED(event))
{
    m_horzToolbar = !m_horzToolbar;

    RecreateToolbar();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("wxWindows toolbar sample", "About wxToolBar");
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
    wxString str;
    str.Printf( _T("Clicked on tool %d\n"), event.GetId());
    m_textWindow->WriteText( str );

    if (event.GetId() == wxID_HELP)
    {
        if ( event.GetExtraLong() != 0 )
            m_textWindow->WriteText( _T("Help button down now.\n") );
        else
            m_textWindow->WriteText( _T("Help button up now.\n") );
    }

    if (event.GetId() == wxID_COPY)
    {
        DoEnablePrint();
    }

    if (event.GetId() == wxID_CUT)
    {
        DoToggleHelp();
    }

    if (event.GetId() == wxID_PRINT)
    {
        DoDeletePrint();
    }
}

void MyFrame::OnCombo(wxCommandEvent& event)
{
    wxLogStatus(_T("Combobox string '%s' selected"), event.GetString().c_str());
}

void MyFrame::DoEnablePrint()
{
    wxToolBar *tb = GetToolBar();
    if (tb->GetToolEnabled(wxID_PRINT))
        tb->EnableTool( wxID_PRINT, FALSE );
    else
        tb->EnableTool( wxID_PRINT, TRUE );
}

void MyFrame::DoDeletePrint()
{
    wxToolBar *tb = GetToolBar();

    tb->DeleteTool( wxID_PRINT );
}

void MyFrame::DoToggleHelp()
{
    wxToolBar *tb = GetToolBar();
    tb->ToggleTool( wxID_HELP, !tb->GetToolState( wxID_HELP ) );
}

void MyFrame::OnUpdateCopyAndCut(wxUpdateUIEvent& event)
{
    event.Enable( m_textWindow->CanCopy() );
}

void MyFrame::OnInsertPrint(wxCommandEvent& WXUNUSED(event))
{
    wxBitmap bmp = wxBITMAP(print);

    GetToolBar()->InsertTool(0, wxID_PRINT, bmp, wxNullBitmap,
                             FALSE, (wxObject *) NULL,
                             "Delete this tool",
                             "This button was inserted into the toolbar");

    GetToolBar()->Realize();
}

void MyFrame::OnToolEnter(wxCommandEvent& event)
{
  if (event.GetSelection() > -1)
  {
    wxString str;
    str.Printf(_T("This is tool number %d"), event.GetSelection());
    SetStatusText(str);
  }
  else
    SetStatusText("");
}

