/////////////////////////////////////////////////////////////////////////////
// Name:      rotate.cpp
// Purpose:   Image rotation test
// Author:    Carlos Moreno
// Modified by:
// Created:   6/2/2000
// RCS-ID:    $Id$
// Copyright: (c) 2000
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/numdlg.h"
#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// application class
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
    const wxImage& GetImage() const { return m_image; }

private:
    wxImage m_image;
};

// ----------------------------------------------------------------------------
// data class for cards that need to be rendered
// ----------------------------------------------------------------------------

class MyRenderedCard
{
public:
    MyRenderedCard(const wxBitmap& bmp, int x, int y)
        : m_bmp(bmp), m_x(x), m_y(y) { }
    wxBitmap m_bmp;
    int m_x, m_y;
};

// Declare a wxArray type to hold MyRenderedCards.
WX_DECLARE_OBJARRAY(MyRenderedCard, ArrayOfCards);

// ----------------------------------------------------------------------------
// custom canvas control that we can draw on
// ----------------------------------------------------------------------------

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow* parent);

    void ClearCards();

    void OnMouseLeftUp (wxMouseEvent & event);
    void OnMouseRightUp (wxMouseEvent & event);
    void OnPaint (wxPaintEvent & event);

private:
    ArrayOfCards m_cards;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit (wxCommandEvent &);
    void OnAngle(wxCommandEvent &);
    void OnClear(wxCommandEvent &);

    double  m_angle;

    DECLARE_EVENT_TABLE()

private:
    MyCanvas *m_canvas;
};

// ----------------------------------------------------------------------------
// menu item identifiers
// ----------------------------------------------------------------------------

enum
{
    ID_Quit = 1,
    ID_Angle,
    ID_Clear
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// application class
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    m_image = wxImage(_T("kclub.bmp"), wxBITMAP_TYPE_BMP);

    // any unused colour will do
    m_image.SetMaskColour( 0, 255, 255 );

    if ( !m_image.Ok() )
    {
        wxLogError(wxT("Can't load the test image, please copy it to the ")
                   wxT("program directory"));
        return false;
    }

    MyFrame *frame = new MyFrame (_T("wxWidgets rotate sample"),
                                  wxPoint(20, 20), wxSize(600, 450));

    frame->Show (true);
    SetTopWindow (frame);
    return true;
}

// ----------------------------------------------------------------------------
// data class for cards that need to be rendered
// ----------------------------------------------------------------------------

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayOfCards);

// ----------------------------------------------------------------------------
// custom canvas control that we can draw on
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_LEFT_UP (MyCanvas::OnMouseLeftUp)
    EVT_RIGHT_UP (MyCanvas::OnMouseRightUp)
    EVT_PAINT (MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas(wxWindow* parent):
  wxScrolledWindow(parent, wxID_ANY)
{
    SetBackgroundColour (wxColour (0,80,60));
    ClearBackground();
}

void MyCanvas::ClearCards ()
{
    m_cards.Clear();
    Refresh(true);
}

// Rotate with interpolation and with offset correction
void MyCanvas::OnMouseLeftUp (wxMouseEvent & event)
{
    MyFrame* frame = (MyFrame*) GetParent();

    wxPoint offset;
    const wxImage& img = wxGetApp().GetImage();
    wxImage img2 = img.Rotate(frame->m_angle,
        wxPoint(img.GetWidth() / 2, img.GetHeight() / 2), true, &offset);

    // Add the cards to an array to be drawn later in OnPaint()
    m_cards.Add(new MyRenderedCard(wxBitmap(img2),
        event.m_x + offset.x, event.m_y + offset.y));
    Refresh(false);
}

// without interpolation, and without offset correction
void MyCanvas::OnMouseRightUp (wxMouseEvent & event)
{
    MyFrame* frame = (MyFrame*) GetParent();

    const wxImage& img = wxGetApp().GetImage();
    wxImage img2 = img.Rotate(frame->m_angle,
        wxPoint(img.GetWidth() / 2, img.GetHeight() / 2), false);

    // Add the cards to an array to be drawn later in OnPaint()
    m_cards.Add(new MyRenderedCard(wxBitmap(img2), event.m_x, event.m_y));
    Refresh(false);
}

void MyCanvas::OnPaint (wxPaintEvent &)
{
    size_t numCards = m_cards.GetCount();

    wxPaintDC dc(this);
    dc.BeginDrawing();

    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.DrawText(wxT("Click on the canvas to draw a card."), 10, 10);

    for (size_t i = 0; i < numCards; i++) {
        MyRenderedCard & card = m_cards.Item(i);
        dc.DrawBitmap(card.m_bmp, card.m_x, card.m_y, true);
    }

    dc.EndDrawing();
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU (ID_Quit, MyFrame::OnQuit)
    EVT_MENU (ID_Angle, MyFrame::OnAngle)
    EVT_MENU (ID_Clear, MyFrame::OnClear)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    m_angle = 0.1;

    m_canvas = new MyCanvas(this);

    wxMenu *menuFile = new wxMenu;
    menuFile->Append (ID_Angle, _T("Set &angle...\tCtrl-A"));
    menuFile->Append (ID_Clear, _T("&Clear all cards\tCtrl-C"));
    menuFile->AppendSeparator();
    menuFile->Append (ID_Quit, _T("E&xit\tAlt-X"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append (menuFile, _T("&File"));

    SetMenuBar (menuBar);
}

void MyFrame::OnAngle (wxCommandEvent &)
{
    long degrees = (long)((180*m_angle)/M_PI);
    degrees = wxGetNumberFromUser(_T("Change the image rotation angle"),
                                  _T("Angle in degrees:"),
                                  _T("wxWidgets rotate sample"),
                                  degrees,
                                  -180, +180,
                                  this);
    if ( degrees != -1 )
        m_angle = (degrees * M_PI) / 180.0;
}

void MyFrame::OnQuit (wxCommandEvent &)
{
    Close (true);
}

void MyFrame::OnClear (wxCommandEvent &)
{
    m_canvas->ClearCards ();
}
