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

    #include "wx/button.h"
    #include "wx/scrolbar.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    Univ_Button1,
    Univ_Button2
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

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
    void OnButton(wxCommandEvent& event);
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
    EVT_BUTTON(-1, MyUnivFrame::OnButton)

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
           : wxFrame(NULL, -1, title, wxDefaultPosition, wxSize(600, 450))
{
    SetBackgroundColour(*wxLIGHT_GREY);

    wxStaticText *text;

    new wxStaticText(this, _T("Test static text"), wxPoint(10, 10));
    new wxStaticText(this,
                     _T("&Multi line\n(and very very very very long)\nstatic text"),
                     wxPoint(210, 10));

    (new wxStaticText(this, _T("&Disabled text"), wxPoint(10, 30)))->Disable();

    text = new wxStaticText(this, _T("Demo of &border styles:"), wxPoint(10, 60));
    text->SetFont(*wxITALIC_FONT);
    text->SetBackgroundColour(*wxWHITE);
    text->SetForegroundColour(*wxBLUE);

    wxCoord x = 10;
    #define CREATE_STATIC_BORDER_DEMO(border) \
        (new wxStaticText(this, -1,  _T(#border), \
                     wxPoint(x, 100), wxSize(70, -1), wx##border##_BORDER)); \
        x += 80

    CREATE_STATIC_BORDER_DEMO(NO);
    CREATE_STATIC_BORDER_DEMO(SIMPLE);
    CREATE_STATIC_BORDER_DEMO(SUNKEN);
    CREATE_STATIC_BORDER_DEMO(RAISED);
    CREATE_STATIC_BORDER_DEMO(STATIC);
    CREATE_STATIC_BORDER_DEMO(DOUBLE);

    #undef CREATE_STATIC_BORDER_DEMO

    wxStaticBox *box = new wxStaticBox(this, _T("&Alignments demo:"),
                                       wxPoint(10, 150),
                                       wxSize(500, 120));
    box->SetForegroundColour(*wxRED);
    box->SetBackground(wxBitmap("bricks.bmp", wxBITMAP_TYPE_BMP), 0, wxTILE);

    x = 15;
    #define CREATE_STATIC_ALIGN_DEMO(align) \
        (new wxStaticText(this, -1,  _T(#align), \
                     wxPoint(x, 175), wxSize(70, 70), \
                     wxSIMPLE_BORDER | wxALIGN_##align)); \
        x += 80

    CREATE_STATIC_ALIGN_DEMO(LEFT);
    CREATE_STATIC_ALIGN_DEMO(RIGHT);
    CREATE_STATIC_ALIGN_DEMO(CENTRE);
    CREATE_STATIC_ALIGN_DEMO(TOP);
    CREATE_STATIC_ALIGN_DEMO(BOTTOM);
    CREATE_STATIC_ALIGN_DEMO(CENTRE_VERTICAL);

    #undef CREATE_STATIC_ALIGN_DEMO

    new wxButton(this, Univ_Button1, _T("&Press me"), wxPoint(10, 300));
    new wxButton(this, Univ_Button2, _T("&And me"), wxPoint(100, 300));

    wxScrollBar *sb;
    sb = new wxScrollBar(this, -1, wxPoint(200, 300), wxSize(100, -1));
    sb->SetScrollbar(0, 10, 100, 10);
    sb = new wxScrollBar(this, -1, wxPoint(200, 330), wxSize(-1, 50), wxSB_VERTICAL);
    sb->SetScrollbar(50, 50, 100, 10);
}

void MyUnivFrame::OnButton(wxCommandEvent& event)
{
    wxLogDebug(_T("Button %d pressed."),
               event.GetId() == Univ_Button1 ? 1 : 2);
}

void MyUnivFrame::OnLeftUp(wxMouseEvent& event)
{
    if ( event.ControlDown() )
    {
        Close();
    }
}
