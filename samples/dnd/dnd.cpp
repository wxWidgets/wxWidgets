/////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     Drag and drop sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include  "wx/intl.h"
#include  "wx/log.h"

#include  "wx/dnd.h"

#ifdef __WXMOTIF__
#error Sorry, drag and drop is not yet implemented on wxMotif.
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// Derive two simple classes which just put in the listbox the strings (text or
// file names) we drop on them
// ----------------------------------------------------------------------------

class DnDText : public wxTextDropTarget
{
public:
  DnDText(wxListBox *pOwner) { m_pOwner = pOwner; }

  virtual bool OnDropText(int x, int y, const char *psz );

private:
  wxListBox *m_pOwner;
};

class DnDFile : public wxFileDropTarget
{
public:
  DnDFile(wxListBox *pOwner) { m_pOwner = pOwner; }

  virtual bool OnDropFiles(int x, int y,
                           size_t nFiles, const char * const aszFiles[] );

private:
  wxListBox *m_pOwner;
};

// ----------------------------------------------------------------------------
// Define a new application type
// ----------------------------------------------------------------------------

class DnDApp : public wxApp
{
public:
  bool OnInit();
};

IMPLEMENT_APP(DnDApp);

// ----------------------------------------------------------------------------
// Define a new frame type
// ----------------------------------------------------------------------------
class DnDFrame : public wxFrame
{
public:
  DnDFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
 ~DnDFrame();

  void OnPaint(wxPaintEvent& event);
  void OnQuit (wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnDrag (wxCommandEvent& event);
  void OnHelp (wxCommandEvent& event);
  void OnLogClear(wxCommandEvent& event);

  void OnLeftDown(wxMouseEvent& event);
  void OnRightDown(wxMouseEvent& event);

  DECLARE_EVENT_TABLE()

private:
  wxListBox  *m_ctrlFile,
             *m_ctrlText;
  wxTextCtrl *m_ctrlLog;

  wxLog *m_pLog, *m_pLogPrev;

  wxString m_strText;
};

// ----------------------------------------------------------------------------
// IDs for the menu commands
// ----------------------------------------------------------------------------

enum
{
  Menu_Quit = 1,
  Menu_Drag,
  Menu_About = 101,
  Menu_Help,
  Menu_Clear,
};

BEGIN_EVENT_TABLE(DnDFrame, wxFrame)
  EVT_MENU(Menu_Quit,  DnDFrame::OnQuit)
  EVT_MENU(Menu_About, DnDFrame::OnAbout)
  EVT_MENU(Menu_Drag,  DnDFrame::OnDrag)
  EVT_MENU(Menu_Help,  DnDFrame::OnHelp)
  EVT_MENU(Menu_Clear, DnDFrame::OnLogClear)
  EVT_LEFT_DOWN(       DnDFrame::OnLeftDown)
  EVT_RIGHT_DOWN(      DnDFrame::OnRightDown)
  EVT_PAINT(           DnDFrame::OnPaint)
END_EVENT_TABLE()

// `Main program' equivalent, creating windows and returning main app frame
bool DnDApp::OnInit()
{
  // create the main frame window
  DnDFrame *frame = new DnDFrame((wxFrame  *) NULL, "Drag & Drop wxWindows App",
                                 50, 50, 450, 340);

  // activate it
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

DnDFrame::DnDFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
        : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h)),
          m_strText("wxWindows drag & drop works :-)")

{
//  SetBackgroundColour(* wxWHITE);

  // frame icon and status bar
  SetIcon(wxICON(mondrian));

//  const int widths[] = { -1 };
  CreateStatusBar();

  // construct menu
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(Menu_Drag, "&Test drag...");
  file_menu->AppendSeparator();
  file_menu->Append(Menu_Quit, "E&xit");

  wxMenu *log_menu = new wxMenu;
  log_menu->Append(Menu_Clear, "Clear");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(Menu_Help, "&Help...");
  help_menu->AppendSeparator();
  help_menu->Append(Menu_About, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(log_menu,  "&Log");
  menu_bar->Append(help_menu, "&Help");

  SetMenuBar(menu_bar);

  // make a panel with 3 subwindows
  wxPoint pos(0, 0);
  wxSize  size(400, 200);

  wxString strFile("Drop files here!"), strText("Drop text on me");

  m_ctrlFile  = new wxListBox(this, -1, pos, size, 1, &strFile, wxLB_HSCROLL);
  m_ctrlText  = new wxListBox(this, -1, pos, size, 1, &strText, wxLB_HSCROLL);

  m_ctrlLog   = new wxTextCtrl(this, -1, "", pos, size,
                               wxTE_MULTILINE | wxTE_READONLY |
                               wxSUNKEN_BORDER );

  // redirect log messages to the text window (don't forget to delete it!)
  m_pLog = new wxLogTextCtrl(m_ctrlLog);
  m_pLogPrev = wxLog::SetActiveTarget(m_pLog);

  // associate drop targets with 2 text controls
  m_ctrlFile->SetDropTarget(new DnDFile(m_ctrlFile));
  m_ctrlText->SetDropTarget( new DnDText(m_ctrlText) );

  wxLayoutConstraints *c;

  // Top-left listbox
  c = new wxLayoutConstraints;
  c->left.SameAs(this, wxLeft);
  c->top.SameAs(this, wxTop);
  c->right.PercentOf(this, wxRight, 50);
  c->height.PercentOf(this, wxHeight, 40);
  m_ctrlFile->SetConstraints(c);

  // Top-right listbox
  c = new wxLayoutConstraints;
  c->left.SameAs    (m_ctrlFile, wxRight);
  c->top.SameAs     (this, wxTop);
  c->right.SameAs   (this, wxRight);
  c->height.PercentOf(this, wxHeight, 40);
  m_ctrlText->SetConstraints(c);

  // Lower text control
  c = new wxLayoutConstraints;
  c->left.SameAs    (this, wxLeft);
  c->right.SameAs   (this, wxRight);
  c->height.PercentOf(this, wxHeight, 40);
  c->top.SameAs(m_ctrlText, wxBottom);
  m_ctrlLog->SetConstraints(c);

  SetAutoLayout(TRUE);
}

void DnDFrame::OnQuit(wxCommandEvent& /* event */)
{
  Close(TRUE);
}

void DnDFrame::OnPaint(wxPaintEvent& /*event*/)
{
  int w = 0;
  int h = 0;
  GetClientSize( &w, &h );

  wxPaintDC dc(this);
  dc.SetFont( wxFont( 24, wxDECORATIVE, wxNORMAL, wxNORMAL, FALSE, "charter" ) );
  dc.DrawText( "Drag text from here!", 20, h-22 );
}

void DnDFrame::OnDrag(wxCommandEvent& /* event */)
{
  wxString strText = wxGetTextFromUser
  (
    "After you enter text in this dialog, press any mouse\n"
    "button in the bottom (empty) part of the frame and \n"
    "drag it anywhere - you will be in fact dragging the\n"
    "text object containing this text",
    "Please enter some text", m_strText, this
  );

  m_strText = strText;
}

void DnDFrame::OnAbout(wxCommandEvent& /* event */)
{
  wxMessageDialog dialog(this,
                         "Drag-&-Drop Demo\n"
                         "Please see \"Help|Help...\" for details\n"
                         "Copyright (c) 1998 Vadim Zeitlin",
                         "About wxDnD");

  dialog.ShowModal();
}

void DnDFrame::OnHelp(wxCommandEvent& /* event */)
{
  wxMessageDialog dialog(this,
"This small program demonstrates drag & drop support in wxWindows. The program window\n"
"consists of 3 parts: the bottom pane is for debug messages, so that you can see what's\n"
"going on inside. The top part is split into 2 listboxes, the left one accepts files\n"
"and the right one accepts text.\n"
"\n"
"To test wxDropTarget: open wordpad (write.exe), select some text in it and drag it to\n"
"the right listbox (you'll notice the usual visual feedback, i.e. the cursor will change).\n"
"Also, try dragging some files (you can select several at once) from Windows Explorer (or \n"
"File Manager) to the left pane. Hold down Ctrl/Shift keys when you drop text (doesn't \n"
"work with files) and see what changes.\n"
"\n"
"To test wxDropSource: just press any mouse button on the empty zone of the window and drag\n"
"it to wordpad or any other droptarget accepting text (and of course you can just drag it\n"
"to the right pane). Due to a lot of trace messages, the cursor might take some time to \n"
"change, don't release the mouse button until it does. You can change the string being\n"
"dragged in in \"File|Test drag...\" dialog.\n"
"\n"
"\n"
"Please send all questions/bug reports/suggestions &c to \n"
"Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>",
                        "wxDnD Help");

  dialog.ShowModal();
}

void DnDFrame::OnLogClear(wxCommandEvent& /* event */ )
{
  m_ctrlLog->Clear();
}

void DnDFrame::OnLeftDown(wxMouseEvent &WXUNUSED(event) )
{
  if ( !m_strText.IsEmpty() ) 
  {
    // start drag operation
#ifdef __WXMSW__
    wxTextDataObject textData(m_strText);
    wxDropSource dragSource( textData, this );
#else
    wxDropSource dragSource( new wxTextDataObject (m_strText), this, wxIcon(mondrian_xpm) );
#endif
    const char *pc;

    switch ( dragSource.DoDragDrop(TRUE) ) 
    {
      case wxDragError:   pc = "Error!";    break;
      case wxDragNone:    pc = "Nothing";   break;
      case wxDragCopy:    pc = "Copied";    break;
      case wxDragMove:    pc = "Moved";     break;
      case wxDragCancel:  pc = "Cancelled"; break;
      default:                    pc = "Huh?";      break;
    }

    SetStatusText(wxString("Drag result: ") + pc);
  }
}

void DnDFrame::OnRightDown(wxMouseEvent &event )
{
  wxMenu *menu = new wxMenu;

  menu->Append(Menu_Drag, "&Test drag...");
  menu->Append(Menu_About, "&About");
  menu->Append(Menu_Quit, "E&xit");

  PopupMenu( menu, event.GetX(), event.GetY() );
}

DnDFrame::~DnDFrame()
{
  if ( m_pLog != NULL ) {
    if ( wxLog::SetActiveTarget(m_pLogPrev) == m_pLog )
      delete m_pLog;
  }
}

// ----------------------------------------------------------------------------
// Notifications called by the base class
// ----------------------------------------------------------------------------
bool DnDText::OnDropText(int, int, const char *psz)
{
  m_pOwner->Append(psz);

  return TRUE;
}

bool DnDFile::OnDropFiles(int, int, size_t nFiles,
                          const char * const aszFiles[])
{
  wxString str;
  str.Printf("%d files dropped", nFiles);
  m_pOwner->Append(str);
  for ( size_t n = 0; n < nFiles; n++ ) {
    m_pOwner->Append(aszFiles[n]);
  }

  return TRUE;
}
