/////////////////////////////////////////////////////////////////////////////
// Name:        joytest.cpp
// Purpose:     Joystick sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/msw/wave.h>
#include <wx/msw/joystick.h>

#include "joytest.h"

MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

int winNumber = 1;

// Initialise this in OnInit, not statically
bool MyApp::OnInit(void)
{
  wxJoystick stick(wxJOYSTICK1);
  if (!stick.IsOk())
  {
    wxMessageBox("No joystick detected!");
    return FALSE;
  }
  m_fire.Create("gun.wav");

  m_maxX = stick.GetXMax();
  m_maxY = stick.GetYMax();

  // Create the main frame window

  frame = new MyFrame(NULL, "Joystick Demo", wxPoint(0, 0), wxSize(500, 400),
   wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

  // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("joyicon"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("joyicon.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(JOYTEST_QUIT, "&Exit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(JOYTEST_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->CreateStatusBar();

  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
	EVT_JOYSTICK_EVENTS(MyCanvas::OnJoystickEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size):
 wxScrolledWindow(parent, -1, pos, size, wxSUNKEN_BORDER)
{
    wxJoystick joystick(wxJOYSTICK1);
    joystick.SetCapture(this);
}

MyCanvas::~MyCanvas(void)
{
    wxJoystick joystick(wxJOYSTICK1);
    joystick.ReleaseCapture();
}

void MyCanvas::OnJoystickEvent(wxJoystickEvent& event)
{
  wxClientDC dc(this);

  wxPoint pt(event.GetPosition());

  // Scale to canvas size
  int cw, ch;
  GetSize(&cw, &ch);

  pt.x = (long) (((double)pt.x/(double)wxGetApp().m_maxX) * cw);
  pt.y = (long) (((double)pt.y/(double)wxGetApp().m_maxY) * ch);

  if (xpos > -1 && ypos > -1 && event.IsMove() && event.ButtonIsDown())
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(xpos, ypos, pt.x, pt.y);
  }
  xpos = pt.x;
  ypos = pt.y;

  char buf[100];
  if (event.ButtonDown())
    sprintf(buf, "Joystick (%ld, %ld) Fire!", pt.x, pt.y);
  else
    sprintf(buf, "Joystick (%ld, %ld)", pt.x, pt.y);
  frame->SetStatusText(buf);

  if (event.ButtonDown() && wxGetApp().m_fire.IsOk())
  {
    wxGetApp().m_fire.Play();
  }
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(JOYTEST_QUIT, MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
const long style):
  wxFrame(parent, -1, title, pos, size, style)
{
  canvas = new MyCanvas(this);
}

MyFrame::~MyFrame(void)
{
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
      Close(TRUE);
}

void MyFrame::OnActivate(wxActivateEvent& event)
{
  if (event.GetActive() && canvas)
    canvas->SetFocus();
}
