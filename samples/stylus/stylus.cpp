/////////////////////////////////////////////////////////////////////////////
// Name:        stylus.cpp
// Purpose:     Stylus sample
// Author:      Iulian Serbanoiu
// Created:     2026-02-02
// Copyright:   (c) Iulian Serbanoiu
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/dcbuffer.h"

#include <deque>

// Define a new application
class wxMyApp: public wxApp
{
public:
    bool OnInit() override;
};

wxDECLARE_APP(wxMyApp);

enum
{
    ID_COLOR_FIRST = wxID_HIGHEST + 1,
    ID_COLOR_BLACK = ID_COLOR_FIRST,
    ID_COLOR_RED,
    ID_COLOR_GREEN,
    ID_COLOR_BLUE,
    ID_COLOR_LAST
};

struct colorInfo {
    int id;
    const char* name;
    const char* accelerator;
    const wxColor* color;
    bool selected;
} clrInfo[] = {
    { ID_COLOR_BLACK, "Black", "1", wxBLACK, false},
    { ID_COLOR_RED, "Red", "2", wxRED, false},
    { ID_COLOR_GREEN, "Green", "3", wxGREEN, false},
    { ID_COLOR_BLUE, "Blue", "4", wxBLUE, true},
};

class MyFrame: public wxFrame
{
private:
    wxColor m_drawingColor{*clrInfo[0].color};
    wxColor m_backgroundColor{*wxWHITE};
    bool m_penDown{false};
    wxBitmap m_backingBitmap;
    std::deque<wxPoint> m_positions;
    void DrawLines(int thickness, const wxColor& color);
    void DrawInfo();
public:
    MyFrame();

    void OnPenDown(wxStylusEvent& event);
    void OnPenUp(wxStylusEvent& event);
    void OnPenUpdate(wxStylusEvent& event);

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnColorChange(wxCommandEvent& event);
};

wxIMPLEMENT_APP(wxMyApp);

bool wxMyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxFrame* frame = new MyFrame();
    frame->CenterOnScreen();
    frame->Show(true);

    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Tablet Drawing Test",
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetClientSize(FromDIP(wxSize(800, 600)));

    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();
    wxMenu* colorMenu = new wxMenu();

    fileMenu->Append(wxID_EXIT);
    menuBar->Append(fileMenu, "&File");

    for ( const auto& info : clrInfo )
    {
        auto menuItem = colorMenu->AppendRadioItem(info.id, wxString::Format("%s\t%s", info.name, info.accelerator));

        if ( info.selected )
        {
            menuItem->Check();
            m_drawingColor = *info.color;
        }
    }
    menuBar->Append(colorMenu, "&Color");

    SetMenuBar(menuBar);

    Bind(wxEVT_STYLUS_DOWN, &MyFrame::OnPenDown, this);
    Bind(wxEVT_STYLUS_UP, &MyFrame::OnPenUp, this);
    Bind(wxEVT_STYLUS_UPDATE, &MyFrame::OnPenUpdate, this);

    Bind(wxEVT_MENU, &MyFrame::OnQuit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnColorChange, this, ID_COLOR_FIRST, ID_COLOR_LAST-1);

    Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);
    Bind(wxEVT_SIZE, &MyFrame::OnSize, this);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc(this);
    dc.DrawBitmap(m_backingBitmap, 0, 0);
}

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    m_backingBitmap.CreateWithLogicalSize(GetClientSize(), GetDPIScaleFactor());

    wxMemoryDC dc(m_backingBitmap);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    DrawInfo();

    Refresh();
}


void MyFrame::OnPenDown(wxStylusEvent& WXUNUSED(event))
{
    SetCursor(*wxCROSS_CURSOR);
    m_positions.clear();
    m_penDown = true;
}

void MyFrame::OnPenUp(wxStylusEvent& WXUNUSED(event))
{
    SetCursor(*wxSTANDARD_CURSOR);
    m_positions.clear();
    m_penDown = false;
}

void MyFrame::OnPenUpdate(wxStylusEvent& event)
{
    const double pressure = event.GetPressure();

    if ( !m_penDown || pressure < 0.0 )
    {
        event.Skip();
        return;
    }

    const bool eraser = event.IsUsingEraser();
    const wxColor color = eraser ? m_backgroundColor : m_drawingColor;
    const int maxThicknes = eraser ? 36 : 12;
    const int minThickness = eraser ? 6 : 2;
    const int thickness = minThickness + pressure * (maxThicknes - minThickness);

    m_positions.push_back(event.GetPosition());

    DrawLines(thickness, color);
}

void MyFrame::OnColorChange(wxCommandEvent& event)
{
    const int index = event.GetId() - ID_COLOR_FIRST;
    m_drawingColor = *clrInfo[index].color;
}

void MyFrame::DrawLines(int thickness, const wxColor& color)
{
    if ( m_positions.size() < 2 )
    {
        return;
    }

    wxMemoryDC dc(m_backingBitmap);
    dc.SetPen(wxPen(color, thickness));
    dc.SetBrush(wxBrush(color));

    wxPoint p1, p2;
    p1 = m_positions.front();
    for ( ;; )
    {
        m_positions.pop_front();
        p2 = m_positions.front();

        dc.DrawLine(p1, p2);

        if ( m_positions.size() > 1 )
        {
            m_positions.pop_front();
            p1 = p2; // last point of line is now the first point of next line
        }
        else
        {
            // only one position left - the last one
            break;
        }
    }
    Refresh();
}

void MyFrame::DrawInfo()
{
    const wxString info("This is a pen drawing demo.\n"
                        "Only pen events are accepted for drawing.\n"
                        "This demo uses the pressure and eraser features of the tablet pen.");
    wxMemoryDC dc(m_backingBitmap);

    dc.DrawText(info, FromDIP(wxPoint(5, 5)));
}
