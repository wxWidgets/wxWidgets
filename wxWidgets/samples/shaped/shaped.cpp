/////////////////////////////////////////////////////////////////////////////
// Name:        shaped.cpp
// Purpose:     Shaped Window sample
// Author:      Robin Dunn
// Modified by:
// Created:     28-Mar-2003
// RCS-ID:      $Id$
// Copyright:   (c) Robin Dunn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/stattext.h"
    #include "wx/menu.h"
    #include "wx/layout.h"
    #include "wx/msgdlg.h"
    #include "wx/image.h"
#endif

#include "wx/dcclient.h"
#include "wx/image.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};


// Define a new frame type: this is going to be our main frame
class ShapedFrame : public wxFrame
{
public:
    // ctor(s)
    ShapedFrame();
    void SetWindowShape();

    // event handlers (these functions should _not_ be virtual)
    void OnDoubleClick(wxMouseEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    void OnExit(wxMouseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnWindowCreate(wxWindowCreateEvent& evt);

private:
    bool     m_hasShape;
    wxBitmap m_bmp;
    wxPoint  m_delta;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(ShapedFrame, wxFrame)
    EVT_LEFT_DCLICK(ShapedFrame::OnDoubleClick)
    EVT_LEFT_DOWN(ShapedFrame::OnLeftDown)
    EVT_LEFT_UP(ShapedFrame::OnLeftUp)
    EVT_MOTION(ShapedFrame::OnMouseMove)
    EVT_RIGHT_UP(ShapedFrame::OnExit)

    EVT_PAINT(ShapedFrame::OnPaint)

#ifdef __WXGTK__
    EVT_WINDOW_CREATE(ShapedFrame::OnWindowCreate)
#endif
END_EVENT_TABLE()


// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    // Create the main application window
    ShapedFrame *frame = new ShapedFrame();
    frame->Show(true);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
ShapedFrame::ShapedFrame()
       : wxFrame((wxFrame *)NULL, wxID_ANY, wxEmptyString,
                  wxDefaultPosition, wxSize(100, 100), //wxDefaultSize,
                  0
                  | wxFRAME_SHAPED
                  | wxSIMPLE_BORDER
                  | wxFRAME_NO_TASKBAR
                  | wxSTAY_ON_TOP
            )
{
    m_hasShape = false;
    m_bmp = wxBitmap(_T("star.png"), wxBITMAP_TYPE_PNG);
    SetSize(wxSize(m_bmp.GetWidth(), m_bmp.GetHeight()));
    SetToolTip(wxT("Right-click to exit"));

#ifndef __WXGTK__
    // On wxGTK we can't do this yet because the window hasn't been created
    // yet so we wait until the EVT_WINDOW_CREATE event happens.  On wxMSW and
    // wxMac the window has been created at this point so we go ahead and set
    // the shape now.
    SetWindowShape();
#endif
}

void ShapedFrame::SetWindowShape()
{
    wxRegion region(m_bmp, *wxWHITE);
    m_hasShape = SetShape(region);
}

void ShapedFrame::OnDoubleClick(wxMouseEvent& WXUNUSED(evt))
{
    if (m_hasShape)
    {
        wxRegion region;
        SetShape(region);
        m_hasShape = false;
    }
    else
        SetWindowShape();
}

void ShapedFrame::OnLeftDown(wxMouseEvent& evt)
{
    CaptureMouse();
    //printf("Mouse captured\n");
    wxPoint pos = ClientToScreen(evt.GetPosition());
    wxPoint origin = GetPosition();
    int dx =  pos.x - origin.x;
    int dy = pos.y - origin.y;
    m_delta = wxPoint(dx, dy);
}

void ShapedFrame::OnLeftUp(wxMouseEvent& WXUNUSED(evt))
{
    if (HasCapture())
    {
        ReleaseMouse();
        //printf("Mouse released\n");
    }
}

void ShapedFrame::OnMouseMove(wxMouseEvent& evt)
{
    wxPoint pt = evt.GetPosition();
    //printf("x:%d   y:%d\n", pt.x, pt.y);
    if (evt.Dragging() && evt.LeftIsDown())
    {
        wxPoint pos = ClientToScreen(pt);
        Move(wxPoint(pos.x - m_delta.x, pos.y - m_delta.y));
    }
}

void ShapedFrame::OnExit(wxMouseEvent& WXUNUSED(evt))
{
    Close();
}

void ShapedFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_bmp, 0, 0, true);
}

void ShapedFrame::OnWindowCreate(wxWindowCreateEvent& WXUNUSED(evt))
{
    SetWindowShape();
}

