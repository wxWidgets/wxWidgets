/*
 * File:	simple.cpp
 * Purpose:	Minimal wxWidgets plugin
 * Author:	Julian Smart
 * Created:	1997
 * Updated:	
 * Copyright:   (c) Julian Smart
 */

/* static const char sccsid[] = "%W% %G%"; */

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <fstream.h>

#include "NPApp.h"
#include "NPFrame.h"

// Define a new application type
class MyApp: public wxPluginApp
{ public:
    virtual bool OnInit(void);
    virtual wxPluginFrame* OnNewInstance(const wxPluginData& data);
};

// Define a new frame type
class MyFrame: public wxPluginFrame
{ public:
    MyFrame(const wxPluginData& data);
    
 public:
    // Let's paint directly onto the 'frame'; we don't need a subwindow
    void OnPaint(wxPaintEvent& event);
    void OnDraw(wxDC& dc);
    void OnMouseEvent(wxMouseEvent& event);

    // Called when the file has been downloaded
    virtual void OnNPNewFile(NPStream *stream, const wxString& fname);

    void CentreStrings(wxDC& dc);

   DECLARE_EVENT_TABLE()

 protected:
    wxStringList    m_strings;
    long            m_xpos;
    long            m_ypos;
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_SIZE(MyFrame::OnSize)
	EVT_PAINT(MyFrame::OnPaint)
	EVT_MOUSE_EVENTS(MyFrame::OnMouseEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// No app initialisation necessary, and for a plugin there is no
// top frame.
bool MyApp::OnInit(void)
{
    return TRUE;
}

// Called whenever a new plugin instance is called. We could check
// various things here in 'data' but we won't bother.
wxPluginFrame* MyApp::OnNewInstance(const wxPluginData& data)
{
    // Implicitly added to list of plugin frames
    return new MyFrame(data);
}

// My frame constructor
MyFrame::MyFrame(const wxPluginData& data):
  wxPluginFrame(data)
{
    m_xpos = -1;
    m_ypos = -1;
}

void MyFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    OnDraw(dc);
}

void MyFrame::OnDraw(wxDC& dc)
{
    dc.SetBrush(*wxCYAN_BRUSH);
    dc.SetPen(*wxRED_PEN);

    int w, h;
    GetClientSize(&w, &h);

    dc.DrawRectangle(0, 0, w, h);

    wxFont swissFont(10, wxSWISS, wxNORMAL, wxNORMAL);
    dc.SetFont(swissFont);
    dc.SetBackgroundMode(wxTRANSPARENT);

    CentreStrings(dc);
}

// Called when the file has been downloaded
void MyFrame::OnNPNewFile(NPStream *stream, const wxString& fname)
{
    ifstream str(fname);
    char buf[201];

    while ( !str.eof() )
    {
        buf[0] = 0;
        str.getline(buf, 200);

        if ( buf[0] != 0 )
            m_strings.Add(buf);
    }
    Refresh();
}

void MyFrame::CentreStrings(wxDC& dc)
{
    int y = 5;
    int cw, ch;
    GetClientSize(&cw, &ch);

    wxNode *node = m_strings.First();
    while ( node )
    {
        char *s = (char *)node->Data();
        long w, h;
        dc.GetTextExtent(s, &w, &h);

        int x = wxMax(0, (cw - w)/2);
        dc.DrawText(s, x, y);

        y += h + (h/2);

        node = node->Next();
    }
}

// This implements a tiny doodling program. Drag the mouse using
// the left button.
void MyFrame::OnMouseEvent(wxMouseEvent& event)
{
  long x, y;
  event.Position(&x, &y);
  wxClientDC dc(this);

  if (m_xpos > -1 && m_ypos > -1 && event.Dragging() && event.LeftIsDown())
  {
    dc.SetPen(wxBLACK_PEN);
    dc.SetBrush(wxTRANSPARENT_BRUSH);
    dc.DrawLine(m_xpos, m_ypos, x, y);
  }
  m_xpos = x;
  m_ypos = y;
}

