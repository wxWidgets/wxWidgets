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

/* GRG: This is not ANSI standard, define M_PI explicitly
#include <math.h>       // M_PI
*/

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


class MyApp: public wxApp
{
public:
    virtual bool OnInit();

    const wxImage& GetImage() const { return m_image; }

private:
    wxImage m_image;
};


class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow* parent);

    void OnMouseLeftUp (wxMouseEvent & event);
    void OnMouseRightUp (wxMouseEvent & event);

private:

    DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit (wxCommandEvent &);
    void OnAngle(wxCommandEvent &);

    double  m_angle;

    DECLARE_EVENT_TABLE()
};

enum
{
    ID_Quit = 1,
    ID_Angle
};

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_LEFT_UP (MyCanvas::OnMouseLeftUp)
    EVT_RIGHT_UP (MyCanvas::OnMouseRightUp)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU (ID_Quit, MyFrame::OnQuit)
    EVT_MENU (ID_Angle, MyFrame::OnAngle)
END_EVENT_TABLE()

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
                                  wxPoint(20,20), wxSize(600,450));

    frame->Show (true);
    SetTopWindow (frame);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    m_angle = 0.1;

    new MyCanvas(this);

    wxMenu *menuFile = new wxMenu;
    menuFile->Append (ID_Angle, _T("Set &angle\tCtrl-A"));
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

MyCanvas::MyCanvas(wxWindow* parent):
  wxScrolledWindow(parent, wxID_ANY)
{
    SetBackgroundColour (wxColour (0,80,60));
    ClearBackground();
}

// Rotate with interpolation and with offset correction
void MyCanvas::OnMouseLeftUp (wxMouseEvent & event)
{
    MyFrame* frame = (MyFrame*) GetParent();

    wxPoint offset;
    const wxImage& img = wxGetApp().GetImage();
    wxImage img2 = img.Rotate(frame->m_angle, wxPoint(img.GetWidth()/2, img.GetHeight()/2), true, &offset);

    wxBitmap bmp(img2);

    wxClientDC dc (this);
    dc.DrawBitmap (bmp, event.m_x + offset.x, event.m_y + offset.y, true);
}

// without interpolation, and without offset correction
void MyCanvas::OnMouseRightUp (wxMouseEvent & event)
{
    MyFrame* frame = (MyFrame*) GetParent();

    const wxImage& img = wxGetApp().GetImage();
    wxImage img2 = img.Rotate(frame->m_angle, wxPoint(img.GetWidth()/2, img.GetHeight()/2), false);

    wxBitmap bmp(img2);

    wxClientDC dc (this);
    dc.DrawBitmap (bmp, event.m_x, event.m_y, true);
}
