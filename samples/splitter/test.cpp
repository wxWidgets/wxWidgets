/////////////////////////////////////////////////////////////////////////////
// Name:        m_splitter.cpp
// Purpose:     wxSplitterWindow sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/splitter.h"

class MyApp;
class MyFrame;
class MyCanvas;

class MyApp: public wxApp
{
public:
  bool OnInit();
};

class MySplitterWindow : public wxSplitterWindow
{
public:
  MySplitterWindow(wxFrame *parent, wxWindowID id) : wxSplitterWindow(parent, id)
  {
    m_frame = parent;
  }

  virtual bool OnSashPositionChange(int newSashPosition)
  {
    if ( !wxSplitterWindow::OnSashPositionChange(newSashPosition) )
      return FALSE;
    
    wxString str;
    str.Printf( _T("Sash position = %d"), newSashPosition);
    m_frame->SetStatusText(str);

    return TRUE;
  }
  
private:
  wxFrame *m_frame;
};

class MyFrame: public wxFrame
{
public:
  MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size);
  virtual ~MyFrame();

  // Menu commands
  void SplitHorizontal(wxCommandEvent& event);
  void SplitVertical(wxCommandEvent& event);
  void Unsplit(wxCommandEvent& event);
  void SetMinSize(wxCommandEvent& event);
  void Quit(wxCommandEvent& event);

  // Menu command update functions
  void UpdateUIHorizontal(wxUpdateUIEvent& event);
  void UpdateUIVertical(wxUpdateUIEvent& event);
  void UpdateUIUnsplit(wxUpdateUIEvent& event);

private:
  void UpdatePosition();

  wxMenu*    fileMenu;
  wxMenuBar*  menuBar;
  MyCanvas*  m_leftCanvas;
  MyCanvas*  m_rightCanvas;
  MySplitterWindow* m_splitter;

DECLARE_EVENT_TABLE()
};

class MyCanvas: public wxScrolledWindow
{
public:
	MyCanvas(wxWindow* parent, wxWindowID id, int x, int y, int w, int h, const wxString &name);
	virtual ~MyCanvas();

  virtual void OnDraw(wxDC& dc);

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
END_EVENT_TABLE()

// ID for the menu commands
enum
{
  SPLIT_QUIT,
  SPLIT_HORIZONTAL,
  SPLIT_VERTICAL,
  SPLIT_UNSPLIT,
  SPLIT_SETMINSIZE
};

// Window ids
#define SPLITTER_WINDOW     100
#define SPLITTER_FRAME      101
#define CANVAS1             102
#define CANVAS2             103

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
  MyFrame* frame = new MyFrame((wxFrame *) NULL, "wxSplitterWindow Example",
                               wxPoint(50, 50), wxSize(420, 300));

  // Show the frame
  frame->Show(TRUE);
  
  SetTopWindow(frame);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SPLIT_VERTICAL, MyFrame::SplitVertical)
  EVT_MENU(SPLIT_HORIZONTAL, MyFrame::SplitHorizontal)
  EVT_MENU(SPLIT_UNSPLIT, MyFrame::Unsplit)
  EVT_MENU(SPLIT_QUIT, MyFrame::Quit)
  EVT_MENU(SPLIT_SETMINSIZE, MyFrame::SetMinSize)

  EVT_UPDATE_UI(SPLIT_VERTICAL, MyFrame::UpdateUIVertical)
  EVT_UPDATE_UI(SPLIT_HORIZONTAL, MyFrame::UpdateUIHorizontal)
  EVT_UPDATE_UI(SPLIT_UNSPLIT, MyFrame::UpdateUIUnsplit)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size):
	wxFrame(frame, SPLITTER_FRAME, title, pos, size)
{
  CreateStatusBar(2);

  // Make a menubar
  fileMenu = new wxMenu;
  fileMenu->Append(SPLIT_VERTICAL, "Split &Vertically", "Split vertically");
  fileMenu->Append(SPLIT_HORIZONTAL, "Split &Horizontally", "Split horizontally");
  fileMenu->Append(SPLIT_UNSPLIT, "&Unsplit", "Unsplit");
  fileMenu->AppendSeparator();
  fileMenu->Append(SPLIT_SETMINSIZE, "Set &min size", "Set minimum pane size");
  fileMenu->AppendSeparator();
  fileMenu->Append(SPLIT_QUIT, "E&xit", "Exit");

  menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");

  SetMenuBar(menuBar);

  m_splitter = new MySplitterWindow(this, SPLITTER_WINDOW);

  m_leftCanvas = new MyCanvas(m_splitter, CANVAS1, 0, 0, 400, 400, "Test1" );
  m_leftCanvas->SetBackgroundColour(*wxRED);
  m_leftCanvas->SetScrollbars(20, 20, 50, 50);
  m_leftCanvas->SetCursor(wxCursor(wxCURSOR_MAGNIFIER));

  m_rightCanvas = new MyCanvas(m_splitter, CANVAS2, 0, 0, 400, 400, "Test2" );
  m_rightCanvas->SetBackgroundColour(*wxCYAN);
  m_rightCanvas->SetScrollbars(20, 20, 50, 50);
  m_rightCanvas->Show(FALSE);

  m_splitter->Initialize(m_leftCanvas);
  SetStatusText("Min pane size = 0", 1);
}

MyFrame::~MyFrame()
{
}

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::SplitHorizontal(wxCommandEvent& WXUNUSED(event) )
{
  if ( m_splitter->IsSplit() )
    m_splitter->Unsplit();
  m_leftCanvas->Show(TRUE);
  m_rightCanvas->Show(TRUE);
  m_splitter->SplitHorizontally( m_leftCanvas, m_rightCanvas );
  UpdatePosition();
}

void MyFrame::SplitVertical(wxCommandEvent& WXUNUSED(event) )
{
  if ( m_splitter->IsSplit() )
    m_splitter->Unsplit();
  m_leftCanvas->Show(TRUE);
  m_rightCanvas->Show(TRUE);
  m_splitter->SplitVertically( m_leftCanvas, m_rightCanvas );
  UpdatePosition();
}

void MyFrame::Unsplit(wxCommandEvent& WXUNUSED(event) )
{
  if ( m_splitter->IsSplit() )
    m_splitter->Unsplit();
  SetStatusText("No splitter");
}

void MyFrame::SetMinSize(wxCommandEvent& WXUNUSED(event) )
{
  wxString str;
  str.Printf( _T("%d"), m_splitter->GetMinimumPaneSize());
  str = wxGetTextFromUser("Enter minimal size for panes:", "", str, this);
  if ( str.IsEmpty() )
    return;

  int minsize = wxStrtol( str, (wxChar**)NULL, 10 );
  m_splitter->SetMinimumPaneSize(minsize);
  str.Printf( _T("Min pane size = %d"), minsize);
  SetStatusText(str, 1);
}

void MyFrame::UpdateUIHorizontal(wxUpdateUIEvent& event)
{
  event.Enable( ( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_HORIZONTAL) ) );
}

void MyFrame::UpdateUIVertical(wxUpdateUIEvent& event)
{
  event.Enable( ( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_VERTICAL) ) );
}

void MyFrame::UpdateUIUnsplit(wxUpdateUIEvent& event)
{
  event.Enable( m_splitter->IsSplit() );
}

void MyFrame::UpdatePosition()
{
  wxString str;
  str.Printf( _("Sash position = %d"), m_splitter->GetSashPosition());
  SetStatusText(str);
}

MyCanvas::MyCanvas(wxWindow* parent, wxWindowID id, int x, int y, int w, int h, const wxString &name ) :
	wxScrolledWindow(parent, id, wxPoint(x, y), wxSize(w, h), 0, name )
{
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnDraw(wxDC& dc)
{
  dc.SetPen(*wxBLACK_PEN);
  dc.DrawLine(0, 0, 100, 100);

  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.DrawText("Testing", 50, 50);

  dc.SetPen(*wxRED_PEN);
  dc.SetBrush(*wxGREEN_BRUSH);
  dc.DrawRectangle(120, 120, 100, 80);
}
