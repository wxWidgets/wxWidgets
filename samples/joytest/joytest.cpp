/////////////////////////////////////////////////////////////////////////////
// Name:        joytest.cpp
// Purpose:     Joystick sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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

#if !wxUSE_JOYSTICK
#   error You must set wxUSE_JOYSTICK to 1 in setup.h
#endif

#if !wxUSE_STATUSBAR
#   error You must set wxUSE_STATUSBAR to 1 in setup.h
#endif

#include "wx/wave.h"
#include "wx/joystick.h"

#include "joytest.h"

MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

int winNumber = 1;

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    wxJoystick stick(wxJOYSTICK1);
    if (!stick.IsOk())
    {
        wxMessageBox(_T("No joystick detected!"));
        return FALSE;
    }

#if wxUSE_WAVE
    m_fire.Create(_T("gun.wav"));
#endif // wxUSE_WAVE

    m_maxX = stick.GetXMax();
    m_maxY = stick.GetYMax();

    // Create the main frame window

    frame = new MyFrame(NULL, _T("Joystick Demo"), wxDefaultPosition,
        wxSize(500, 400), wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

  // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
    frame->SetIcon(wxIcon(_T("joyicon")));
#endif
#ifdef __X__
    frame->SetIcon(wxIcon(_T("joyicon.xbm")));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(JOYTEST_QUIT, _T("&Exit"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

    frame->CreateStatusBar();

    frame->CenterOnScreen();
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

MyCanvas::~MyCanvas()
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

    wxString buf;
    if (event.ButtonDown())
        buf.Printf(_T("Joystick (%d, %d) Fire!"), pt.x, pt.y);
    else
        buf.Printf(_T("Joystick (%d, %d)"), pt.x, pt.y);

    frame->SetStatusText(buf);

#if wxUSE_WAVE
    if (event.ButtonDown() && wxGetApp().m_fire.IsOk())
    {
        wxGetApp().m_fire.Play();
    }
#endif // wxUSE_WAVE
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(JOYTEST_QUIT, MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, const long style)
    : wxFrame(parent, -1, title, pos, size, style)
{
    canvas = new MyCanvas(this);
}

MyFrame::~MyFrame()
{
    // Empty
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
