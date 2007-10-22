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


// Define a new frame type: this is going to the frame showing the
// effect of wxFRAME_SHAPED
class ShapedFrame : public wxFrame
{
public:
    // ctor(s)
    ShapedFrame(wxFrame *parent);
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

// Define a new frame type: this is going to the frame showing the
// effect of wxWindow::SetTransparent and of
// wxWindow::SetBackgroundStyle(wxBG_STYLE_TRANSPARENT)
class SeeThroughFrame : public wxFrame
{
public:
    // ctor(s)
    SeeThroughFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnDoubleClick(wxMouseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);

private:
    enum State
    {
        STATE_SEETHROUGH,
        STATE_TRANSPARENT,
        STATE_OPAQUE,
        STATE_MAX
    };

    State m_currentState;

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
    if ( !wxApp::OnInit() )
        return false;

    wxInitAllImageHandlers();

    // Create the transparent window
    SeeThroughFrame *seeThroughFrame = new SeeThroughFrame();
    seeThroughFrame->Show(true);
    SetTopWindow(seeThroughFrame);

    // Create the shaped window
    ShapedFrame *shapedFrame = new ShapedFrame(seeThroughFrame);
    shapedFrame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// shaped frame
// ----------------------------------------------------------------------------

// frame constructor
ShapedFrame::ShapedFrame(wxFrame *parent)
       : wxFrame(parent, wxID_ANY, wxEmptyString,
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

// ----------------------------------------------------------------------------
// see-through frame
// ----------------------------------------------------------------------------

// frame constructor
SeeThroughFrame::SeeThroughFrame()
       : wxFrame(NULL, wxID_ANY, "Transparency test: double click here",
                  wxPoint(100, 30), wxSize(300, 300),
                  wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP),
         m_currentState(STATE_SEETHROUGH)
{
    SetBackgroundColour(wxColour(255, 255, 255, 255));
    SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
}

// Redraws the whole window on resize
void SeeThroughFrame::OnSize(wxSizeEvent& WXUNUSED(evt))
{
    Refresh();
}

// Paints a grid of varying hue and alpha
void SeeThroughFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    dc.SetPen(wxNullPen);

    int xcount = 8;
    int ycount = 8;

    float xstep = 1. / xcount;
    float ystep = 1. / ycount;

    int width = GetClientSize().GetWidth();
    int height = GetClientSize().GetHeight();

    for ( float x = 0.; x < 1.; x += xstep )
    {
        for ( float y = 0.; y < 1.; y += ystep )
        {
            wxImage::RGBValue v = wxImage::HSVtoRGB(wxImage::HSVValue(x, 1., 1.));
            dc.SetBrush(wxBrush(wxColour(v.red, v.green, v.blue,
                                (int)(255*(1. - y)))));
            int x1 = (int)(x * width);
            int y1 = (int)(y * height);
            int x2 = (int)((x + xstep) * width);
            int y2 = (int)((y + ystep) * height);
            dc.DrawRectangle(x1, y1, x2 - x1, y2 - y1);
        }
    }
}

// Switches between colour and transparent background on doubleclick
void SeeThroughFrame::OnDoubleClick(wxMouseEvent& WXUNUSED(evt))
{
    m_currentState = (State)((m_currentState + 1) % STATE_MAX);

    switch ( m_currentState )
    {
        case STATE_OPAQUE:
            SetBackgroundStyle(wxBG_STYLE_COLOUR);
            SetTransparent(255);
            SetTitle("Opaque");
            break;

        case STATE_SEETHROUGH:
            SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
            SetTransparent(255);
            SetTitle("See through");
            break;

        case STATE_TRANSPARENT:
            SetBackgroundStyle(wxBG_STYLE_COLOUR);
            SetTransparent(128);
            SetTitle("Semi-transparent");
            break;

        case STATE_MAX:
            wxFAIL_MSG( "unreachable" );
    }

    Refresh();
}

BEGIN_EVENT_TABLE(SeeThroughFrame, wxFrame)
    EVT_LEFT_DCLICK(SeeThroughFrame::OnDoubleClick)
    EVT_PAINT(SeeThroughFrame::OnPaint)
    EVT_SIZE(SeeThroughFrame::OnSize)
END_EVENT_TABLE()

