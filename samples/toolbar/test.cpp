/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
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
    // #include "bitmaps/paste.xpm"
    #include "bitmaps/print.xpm"
    #include "bitmaps/preview.xpm"
    #include "bitmaps/help.xpm"
#endif // GTK or Motif

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit();
    bool InitToolbar(wxToolBar* toolBar, bool smallicons = FALSE);
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

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnToggleToolbar(wxCommandEvent& event);
    void OnEnablePrint(wxCommandEvent& event) { DoEnablePrint(); }
    void OnDeletePrint(wxCommandEvent& event) { DoDeletePrint(); }
    void OnToggleHelp(wxCommandEvent& event) { DoToggleHelp(); }

    void OnToolLeftClick(wxCommandEvent& event);
    void OnToolEnter(wxCommandEvent& event);

    void OnCombo(wxCommandEvent& event);

private:
    void DoEnablePrint();
    void DoDeletePrint();
    void DoToggleHelp();

    bool                m_smallToolbar;
    wxTextCtrl*         m_textWindow;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;

enum
{
    IDM_TOOLBAR_TOGGLETOOLBAR = 200,
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_DELETEPRINT,
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

    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBAR, MyFrame::OnToggleToolbar)
    EVT_MENU(IDM_TOOLBAR_ENABLEPRINT, MyFrame::OnEnablePrint)
    EVT_MENU(IDM_TOOLBAR_DELETEPRINT, MyFrame::OnDeletePrint)
    EVT_MENU(IDM_TOOLBAR_TOGGLEHELP, MyFrame::OnToggleHelp)

    EVT_MENU(-1, MyFrame::OnToolLeftClick)

    EVT_COMBOBOX(ID_COMBO, MyFrame::OnCombo)

    EVT_TOOL_ENTER(ID_TOOLBAR, MyFrame::OnToolEnter)
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

bool MyApp::InitToolbar(wxToolBar* toolBar, bool smallicons)
{
  // Set up toolbar
  wxBitmap* toolBarBitmaps[8];

#ifdef __WXMSW__
  toolBarBitmaps[0] = new wxBitmap("icon1");
  toolBarBitmaps[1] = new wxBitmap("icon2");
  if ( !smallicons )
  {
      toolBarBitmaps[2] = new wxBitmap("icon3");
      toolBarBitmaps[3] = new wxBitmap("icon4");
      toolBarBitmaps[4] = new wxBitmap("icon5");
      toolBarBitmaps[5] = new wxBitmap("icon6");
      toolBarBitmaps[6] = new wxBitmap("icon7");
      toolBarBitmaps[7] = new wxBitmap("icon8");
  }
#else
  toolBarBitmaps[0] = new wxBitmap( new_xpm );
  toolBarBitmaps[1] = new wxBitmap( open_xpm );
  if ( !smallicons )
  {
      toolBarBitmaps[2] = new wxBitmap( save_xpm );
      toolBarBitmaps[3] = new wxBitmap( copy_xpm );
      toolBarBitmaps[4] = new wxBitmap( cut_xpm );
      toolBarBitmaps[5] = new wxBitmap( preview_xpm );
      toolBarBitmaps[6] = new wxBitmap( print_xpm );
      toolBarBitmaps[7] = new wxBitmap( help_xpm );
  }
#endif

#ifdef __WXMSW__
  int width = 24;
#else
  int width = 16;
#endif
  int currentX = 5;

  toolBar->AddTool(wxID_NEW, *(toolBarBitmaps[0]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "New file");
  currentX += width + 5;
  toolBar->AddTool(wxID_OPEN, *(toolBarBitmaps[1]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Open file");
  currentX += width + 5;
  toolBar->AddTool(wxID_SAVE, *(toolBarBitmaps[2]), wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Toggle button 1");

  wxComboBox *combo = new wxComboBox(toolBar, ID_COMBO);
  combo->Append("This");
  combo->Append("is a");
  combo->Append("combobox");
  combo->Append("in a");
  combo->Append("toolbar");
  toolBar->AddControl(combo);

  if ( !smallicons )
  {
      currentX += width + 5;
      toolBar->AddTool(wxID_COPY, *(toolBarBitmaps[3]), wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Toggle button 2");
      currentX += width + 5;
      toolBar->AddTool(wxID_CUT, *(toolBarBitmaps[4]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Toggle/Untoggle help button");
      currentX += width + 5;
      toolBar->AddTool(wxID_PASTE, *(toolBarBitmaps[5]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Paste");
      currentX += width + 5;
      toolBar->AddTool(wxID_PRINT, *(toolBarBitmaps[6]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Delete this tool");
      currentX += width + 5;
      toolBar->AddSeparator();
      toolBar->AddTool(wxID_HELP, *(toolBarBitmaps[7]), wxNullBitmap, TRUE, currentX, -1, (wxObject *) NULL, "Help button");
  }

  toolBar->Realize();

  // Can delete the bitmaps since they're reference counted
  int i, max = smallicons ? 3 : WXSIZEOF(toolBarBitmaps);
  for (i = 0; i < max; i++)
    delete toolBarBitmaps[i];

  return TRUE;
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

    // Give it a status line
    CreateStatusBar();

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *tbarMenu = new wxMenu;
    tbarMenu->Append(IDM_TOOLBAR_TOGGLETOOLBAR, "&Toggle toolbar", "Change the toolbar kind");
    tbarMenu->Append(IDM_TOOLBAR_ENABLEPRINT, "&Enable print button", "");
    tbarMenu->Append(IDM_TOOLBAR_DELETEPRINT, "&Delete print button", "");
    tbarMenu->Append(IDM_TOOLBAR_TOGGLEHELP, "Toggle &help button", "");

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
    wxToolBar *tbar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL |
                                    wxTB_FLAT | wxTB_DOCKABLE,
                                    ID_TOOLBAR);

    tbar->SetMargins( 4, 4 );

    wxGetApp().InitToolbar(tbar);
}

void MyFrame::OnToggleToolbar(wxCommandEvent& WXUNUSED(event))
{
    // delete and recreate the toolbar
    wxToolBar *tbar = GetToolBar();
    delete tbar;

    SetToolBar(NULL);
    tbar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL |
                         wxTB_FLAT | wxTB_DOCKABLE,
                         ID_TOOLBAR);

    m_smallToolbar = !m_smallToolbar;
    wxGetApp().InitToolbar(tbar, m_smallToolbar);
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
    // Not implemented in MSW
#ifdef __WXMSW__
    wxMessageBox("Sorry, wxToolBar::DeleteTool is not implemented under Windows.");
#else
    tb->DeleteTool( wxID_PRINT );
#endif
}

void MyFrame::DoToggleHelp()
{
    wxToolBar *tb = GetToolBar();
    tb->ToggleTool( wxID_HELP, !tb->GetToolState( wxID_HELP ) );
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

