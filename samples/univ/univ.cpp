/////////////////////////////////////////////////////////////////////////////
// Name:        univ.cpp
// Purpose:     wxUniversal demo
// Author:      Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "univ.cpp"
    #pragma interface "univ.cpp"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dcclient.h"
#endif

// Define a new application type, each program should derive a class from wxApp
class MyUnivApp : public wxApp
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
class MyUnivFrame : public wxFrame
{
public:
    // ctor(s)
    MyUnivFrame(const wxString& title);

protected:
    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnLeftUp(wxMouseEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyUnivApp)

BEGIN_EVENT_TABLE(MyUnivFrame, wxFrame)
    EVT_PAINT(MyUnivFrame::OnPaint)

    EVT_LEFT_UP(MyUnivFrame::OnLeftUp)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

bool MyUnivApp::OnInit()
{
    wxFrame *frame = new MyUnivFrame(_T("wxUniversal demo"));
    frame->Show();

    return TRUE;
}

// ----------------------------------------------------------------------------
// top level frame class
// ----------------------------------------------------------------------------

MyUnivFrame::MyUnivFrame(const wxString& title)
           : wxFrame(NULL, -1, title, wxDefaultPosition, wxSize(300, 150))
{
}

void MyUnivFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    dc.DrawText(_T("Ctrl-click to exit."), 10, 10);
}

void MyUnivFrame::OnLeftUp(wxMouseEvent& event)
{
    if ( event.ControlDown() )
    {
        Close();
    }
}
