///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     Drag and drop sample
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
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

// ----------------------------------------------------------------------------
// Derive 2 simple classes which just put in the listbox the strings (text or
// file names) we drop on them
// ----------------------------------------------------------------------------
class DnDText : public wxTextDropTarget
{
public:
  DnDText(wxListBox *pOwner) { m_pOwner = pOwner; }

  virtual bool OnDropText(long x, long y, const char *psz);

private:
  wxListBox *m_pOwner;
};

class DnDFile : public wxFileDropTarget
{
public:
  DnDFile(wxListBox *pOwner) { m_pOwner = pOwner; }

  virtual bool OnDropFiles(long x, long y,
                           size_t nFiles, const char * const aszFiles[]);

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
  void OnHelp (wxCommandEvent& event);

  bool OnClose();
  
  DECLARE_EVENT_TABLE()

private:
  wxListBox  *m_ctrlFile,
             *m_ctrlText;
  wxTextCtrl *m_ctrlLog;

  wxLogTarget *m_pLog, *m_pLogPrev;
};

// ----------------------------------------------------------------------------
// IDs for the menu commands
// ----------------------------------------------------------------------------
enum
{
  Menu_Quit = 1,
  Menu_About = 101,
  Menu_Help,
};

BEGIN_EVENT_TABLE(DnDFrame, wxFrame)
	EVT_MENU(Menu_Quit,  DnDFrame::OnQuit)
	EVT_MENU(Menu_About, DnDFrame::OnAbout)
	EVT_MENU(Menu_Help,  DnDFrame::OnHelp)

  //EVT_PAINT(DnDFrame::OnPaint)
END_EVENT_TABLE()

// `Main program' equivalent, creating windows and returning main app frame
bool DnDApp::OnInit(void)
{
  // create the main frame window
  DnDFrame *frame = new DnDFrame(NULL, "Drag & Drop wxWindows App", 
                                 50, 50, 450, 340);

  // activate it
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

DnDFrame::DnDFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
        : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
  SetIcon(wxIcon("mondrian"));

  // construct menu
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(Menu_Help, "&Help");
  file_menu->Append(Menu_About, "&About");
  file_menu->AppendSeparator();
  file_menu->Append(Menu_Quit, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  SetMenuBar(menu_bar);
  
  // make a panel with 3 subwindows
  wxPoint pos(0, 0);
  wxSize  size(400, 200);

  wxString strFile("Drop files here!"), strText("Drop text on me");

  m_ctrlFile  = new wxListBox(this, -1, pos, size, 1, &strFile, wxLB_HSCROLL);
  m_ctrlText  = new wxListBox(this, -1, pos, size, 1, &strText, wxLB_HSCROLL);
  m_ctrlLog   = new wxTextCtrl(this, -1, "", pos, size, 
                               wxTE_MULTILINE | wxTE_READONLY | 
                               wxSUNKEN_BORDER| wxHSCROLL);

  // redirect log messages to the text window (don't forget to delete it!)
//  m_pLog = new wxLogTextCtrl(m_ctrlLog);
  m_pLog = NULL;
  m_pLogPrev = wxLogTarget::SetActiveTarget(m_pLog);

  // associate drop targets with 2 text controls
  m_ctrlFile->SetDropTarget(new DnDFile(m_ctrlFile));
  m_ctrlText->SetDropTarget(new DnDText(m_ctrlText));

  wxLayoutConstraints *c;

  c = new wxLayoutConstraints;
  c->left.SameAs		(this, wxLeft);
  c->top.SameAs			(this, wxTop);
  c->right.PercentOf(this, wxRight, 50);
  c->height.PercentOf(this, wxHeight, 50);
  m_ctrlFile->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->left.SameAs    (m_ctrlFile, wxRight);
  c->top.SameAs     (this, wxTop);
  c->right.SameAs   (this, wxRight);
  c->height.PercentOf(this, wxHeight, 50);
  m_ctrlText->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->left.SameAs    (this, wxLeft);
  c->right.SameAs   (this, wxRight);
  c->height.PercentOf(this, wxHeight, 50);
  c->bottom.SameAs(this, wxBottom);

  m_ctrlLog->SetConstraints(c);

  SetAutoLayout(TRUE);
}

void DnDFrame::OnQuit(wxCommandEvent& /* event */)
{
  Close(TRUE);
}

void DnDFrame::OnAbout(wxCommandEvent& /* event */)
{
  wxMessageDialog dialog(this, 
                         "Drag-&-Drop Demo\n"
                         "Please see File|Help for details",
                         "About wxDnD");

  dialog.ShowModal();
}

void DnDFrame::OnHelp(wxCommandEvent& /* event */)
{
  wxMessageDialog dialog(this, 
"This small program demonstrates drag & drop support in wxWindows.\n"
"The program window consists of 3 parts: the bottom pane is for\n"
"debug messages, so that you can see what's going on inside.\n"
"The top part is split into 2 listboxes, the left one accepts\n"
"files and the right one accepts text.\n"
"\n"
"To test it: open wordpad (write.exe), select some text in it and\n"
"drag it to the right listbox (you'll notice the usual visual\n"
"feedback, i.e. the cursor will change). Also, try dragging some\n"
"files (you can select several at once) from Windows Explorer (or\n"
"File Manager) to the left pane. Hold down Ctrl/Shift keys when\n"
"you drop text (doesn't work with files) and see what changes.\n"
"\n"
"Please address any questions/bug reports/suggestions &c to\n"
"Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>",
                        "wxDnD Help");

  dialog.ShowModal();
}

bool DnDFrame::OnClose() 
{ 
  return TRUE; 
}

DnDFrame::~DnDFrame()
{
  if ( m_pLog != NULL ) {
    if ( wxLogTarget::SetActiveTarget(m_pLogPrev) == m_pLog )
      delete m_pLog;
  }
}

// ----------------------------------------------------------------------------
// Notifications called by the base class
// ----------------------------------------------------------------------------
bool DnDText::OnDropText(long, long, const char *psz)
{
  m_pOwner->Append(psz);

  return TRUE;
}

bool DnDFile::OnDropFiles(long, long, size_t nFiles, 
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
