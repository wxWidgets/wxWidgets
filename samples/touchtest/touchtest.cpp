/////////////////////////////////////////////////////////////////////////////
// Name:        touchtest.cpp
// Purpose:     Multitouch sample
// Author:      Martin Koegler
// Modified by:
// Created:     2015-07-29
// Copyright:   (c) Martin Koegler
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

// Define a new application
class wxMyApp: public wxApp
{
public:
    bool OnInit() override;
};

wxDECLARE_APP(wxMyApp);

constexpr unsigned int TOUCH_POINTS = 16;

class MyFrame: public wxFrame
{
private:
    struct TouchState
    {
        wxTouchSequenceId id;
        wxPoint last;
        wxPen pen;
    };
    TouchState m_TouchPoints[TOUCH_POINTS];
    wxTouchSequenceId m_MouseId;
    wxBitmap m_Bitmap;

    int FindIndexOfTouchId(const wxTouchSequenceId& id);

    void DrawStart(const wxTouchSequenceId& id, wxPoint pos);
    void DrawUpdate(const wxTouchSequenceId& id, wxPoint pos);
    void DrawEnd(const wxTouchSequenceId& id, wxPoint pos);

public:
    MyFrame(wxFrame *parent, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long style);

    void OnTouchBegin(wxMultiTouchEvent& event);
    void OnTouchMove(wxMultiTouchEvent& event);
    void OnTouchEnd(wxMultiTouchEvent& event);

    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnQuit(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(wxMyApp)

bool wxMyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxFrame* frame = new MyFrame(nullptr, wxT("Multi-touch Test"), wxDefaultPosition,
        wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
    SetTopWindow(frame);
    frame->CenterOnScreen();
    frame->Show(true);

    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_TOUCH_BEGIN(MyFrame::OnTouchBegin)
    EVT_TOUCH_MOVE(MyFrame::OnTouchMove)
    EVT_TOUCH_END(MyFrame::OnTouchEnd)
    EVT_TOUCH_CANCEL(MyFrame::OnTouchEnd)
    EVT_LEFT_DOWN(MyFrame::OnMouseDown)
    EVT_MOTION(MyFrame::OnMouseMove)
    EVT_LEFT_UP(MyFrame::OnMouseUp)

    EVT_PAINT(MyFrame::OnPaint)
    EVT_SIZE(MyFrame::OnSize)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, const long style)
    : wxFrame(parent, wxID_ANY, title, pos, size, style),
    m_MouseId(&m_MouseId)
{
    m_TouchPoints[0].pen = wxPen(*wxBLACK, 2);
    m_TouchPoints[1].pen = wxPen(*wxBLUE, 2);
    m_TouchPoints[2].pen = wxPen(*wxCYAN, 2);
    m_TouchPoints[3].pen = wxPen(*wxGREEN, 2);
    m_TouchPoints[4].pen = wxPen(*wxYELLOW, 2);
    m_TouchPoints[5].pen = wxPen(*wxRED, 2);
    m_TouchPoints[6].pen = wxPen(*wxLIGHT_GREY, 2);
    m_TouchPoints[7].pen = wxPen(*wxBLACK, 2, wxPENSTYLE_DOT_DASH);
    m_TouchPoints[8].pen = wxPen(*wxBLUE, 2, wxPENSTYLE_DOT_DASH);
    m_TouchPoints[9].pen = wxPen(*wxCYAN, 2, wxPENSTYLE_DOT_DASH);
    m_TouchPoints[10].pen = wxPen(*wxGREEN, 2, wxPENSTYLE_DOT_DASH);
    m_TouchPoints[11].pen = wxPen(*wxRED, 2, wxPENSTYLE_DOT_DASH);
    m_TouchPoints[12].pen = wxPen(*wxBLACK, 2, wxPENSTYLE_DOT);
    m_TouchPoints[13].pen = wxPen(*wxYELLOW, 2, wxPENSTYLE_DOT);
    m_TouchPoints[14].pen = wxPen(*wxBLUE, 2, wxPENSTYLE_DOT);
    m_TouchPoints[15].pen = wxPen(*wxRED, 2, wxPENSTYLE_DOT);

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_EXIT);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxT("&File"));
    SetMenuBar(menu_bar);

    if (!EnableTouchEvents(wxTOUCH_RAW_EVENTS))
    {
        wxLogError(wxT("Enabling touch events failed"));
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_Bitmap, 0, 0);
}

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxSize size = GetClientSize();
    m_Bitmap = wxBitmap(size.x, size.y, 24);

    wxMemoryDC dc(m_Bitmap);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
}

int MyFrame::FindIndexOfTouchId(const wxTouchSequenceId& id)
{
    int idx = -1;
    for (unsigned i = 0; i < TOUCH_POINTS; i++)
    {
        if (m_TouchPoints[i].id == id)
            idx = i;
    }
    return idx;
}

void MyFrame::DrawStart(const wxTouchSequenceId& id, wxPoint pos)
{
    int idx = FindIndexOfTouchId(id);
    if (idx == -1)
    {
        for (unsigned i = 0; i < TOUCH_POINTS; i++)
        {
            if (!m_TouchPoints[i].id.IsOk())
            {
                idx = i;
                break;
            }
        }
    }
    if (idx == -1)
        return;
    m_TouchPoints[idx].id = id;
    m_TouchPoints[idx].last = pos;
}

void MyFrame::DrawUpdate(const wxTouchSequenceId& id, wxPoint pos)
{
    int idx = FindIndexOfTouchId(id);
    if (idx == -1)
        return;

    wxMemoryDC dc(m_Bitmap);
    dc.SetPen(m_TouchPoints[idx].pen);
    dc.DrawLine(m_TouchPoints[idx].last, pos);

    m_TouchPoints[idx].last = pos;

    Refresh();
}

void MyFrame::DrawEnd(const wxTouchSequenceId& id, wxPoint pos)
{
    int idx = FindIndexOfTouchId(id);
    if (idx == -1)
        return;

    wxMemoryDC dc(m_Bitmap);
    dc.SetPen(m_TouchPoints[idx].pen);
    dc.DrawLine(m_TouchPoints[idx].last, pos);

    m_TouchPoints[idx].id.Unset();

    Refresh();
}

void MyFrame::OnTouchBegin(wxMultiTouchEvent& event)
{
    if (event.IsPrimary())
        return;
    DrawStart(event.GetSequenceId(), event.GetPosition());
}

void MyFrame::OnTouchMove(wxMultiTouchEvent& event)
{
    if (event.IsPrimary())
        return;
    DrawUpdate(event.GetSequenceId(), event.GetPosition());
}

void MyFrame::OnTouchEnd(wxMultiTouchEvent& event)
{
    if (event.IsPrimary())
        return;
    DrawEnd(event.GetSequenceId(), event.GetPosition());
}

void MyFrame::OnMouseDown(wxMouseEvent& event)
{
    DrawStart(m_MouseId, event.GetPosition());
}

void MyFrame::OnMouseMove(wxMouseEvent& event)
{
    if (event.LeftIsDown())
        DrawUpdate(m_MouseId, event.GetPosition());
}

void MyFrame::OnMouseUp(wxMouseEvent& event)
{
    DrawEnd(m_MouseId, event.GetPosition());
}
