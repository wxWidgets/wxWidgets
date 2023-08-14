/////////////////////////////////////////////////////////////////////////////
// Name:        joytest.cpp
// Purpose:     Joystick sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_JOYSTICK
#   error You must set wxUSE_JOYSTICK to 1 in setup.h
#endif

#include "wx/sound.h"
#include "wx/joystick.h"
#include <wx/dcbuffer.h>

#include "joytest.h"

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

MyFrame *frame = nullptr;

wxIMPLEMENT_APP(MyApp);

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxJoystick stick(wxJOYSTICK1);
    if (!stick.IsOk())
    {
        wxMessageBox("No joystick detected!");
        return false;
    }

#if wxUSE_SOUND
    m_fire.Create("buttonpress.wav");
#endif // wxUSE_SOUND

    m_minX = stick.GetXMin();
    m_minY = stick.GetYMin();
    m_maxX = stick.GetXMax();
    m_maxY = stick.GetYMax();

    // Create the main frame window

    frame = new MyFrame(nullptr, "Joystick Demo");

    frame->SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_EXIT, "&Exit");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    frame->CreateStatusBar();
    frame->SetStatusText(wxString::Format("Device: %s, PID: %i, MID: %i, # of joysticks: %i",
        stick.GetProductName(),
        stick.GetProductId(),
        stick.GetManufacturerId(),
        wxJoystick::GetNumberJoysticks())
    );
#endif // wxUSE_STATUSBAR

    frame->SetSize(frame->FromDIP(wxSize(500, 400)));
    frame->CenterOnScreen();
    frame->Show(true);

    return true;
}

wxBEGIN_EVENT_TABLE(MyCanvas, wxPanel)
    EVT_JOYSTICK_EVENTS(MyCanvas::OnJoystickEvent)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_SIZE(MyCanvas::OnSize)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , m_pos(wxDefaultPosition)
    , m_point(wxDefaultPosition)
    , m_validPoint(false)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_stick = new wxJoystick(wxJOYSTICK1);
    m_nButtons = m_stick->GetNumberButtons();
    m_stick->SetCapture(this, 10);
}

MyCanvas::~MyCanvas()
{
    m_stick->ReleaseCapture();
    delete m_stick;
}

void MyCanvas::OnJoystickEvent(wxJoystickEvent& event)
{
    // We don't have valid (x, y) coordinates for z-move events.
    if ( !event.IsZMove() )
    {
        m_point = event.GetPosition();
        m_validPoint = true;

        if (event.IsMove() && event.ButtonIsDown()) {
            Refresh();
        }
    }

#if wxUSE_SOUND
    if (event.ButtonDown() && wxGetApp().m_fire.IsOk())
    {
        wxGetApp().m_fire.Play();
    }
#endif // wxUSE_SOUND

#if wxUSE_STATUSBAR
    wxString buf;

    for (int i = 0; i < m_nButtons; ++i)
        buf += wxString::Format("%c", (event.GetButtonState() & (1 << i)) ? 0x26AB : 0x26AA);

    buf += wxString::Format("  [%d, %d]", m_pos.x, m_pos.y);

    frame->SetStatusText(buf);
#endif // wxUSE_STATUSBAR
}

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);

    if (!m_validPoint)
    {
        dc.Clear();
        return;
    }

    // if negative positions are possible then shift everything up
    wxPoint pt(m_point);

    int xmin = wxGetApp().m_minX;
    int xmax = wxGetApp().m_maxX;
    int ymin = wxGetApp().m_minY;
    int ymax = wxGetApp().m_maxY;
    if (xmin < 0) {
        xmax += abs(xmin);
        pt.x += abs(xmin);
    }
    if (ymin < 0) {
        ymax += abs(ymin);
        pt.y += abs(ymin);
    }

    // Scale to canvas size
    wxSize cs = ToDIP(GetClientSize());
    pt.x = (long)(((double)pt.x / (double)xmax) * (cs.x - 1));
    pt.y = (long)(((double)pt.y / (double)ymax) * (cs.y - 1));

    if (m_pos == wxDefaultPosition)
        m_pos = wxPoint(cs.x / 2, cs.y / 2);

    if (m_stick->GetButtonState(0))
        dc.SetPen(*wxGREEN_PEN);
    else if (m_stick->GetButtonState(1))
        dc.SetPen(*wxRED_PEN);
    else if (m_stick->GetButtonState(2))
        dc.SetPen(*wxBLUE_PEN);
    else if (m_stick->GetButtonState(3))
        dc.SetPen(*wxYELLOW_PEN);
    else
        dc.SetPen(*wxCYAN_PEN);

    dc.DrawLine(FromDIP(m_pos), FromDIP(pt));

    m_pos = pt;
}

void MyCanvas::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    m_validPoint = false;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *parent, const wxString& title)
    : wxFrame(parent, wxID_ANY, title)
{
    canvas = new MyCanvas(this);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive() && canvas)
        canvas->SetFocus();
}
