/////////////////////////////////////////////////////////////////////////////
// Name:      test.cpp
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

#include "wx/image.h"

class MyApp: public wxApp
{
    virtual bool OnInit();
};


class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit (wxCommandEvent &);
    void OnMouseLeftUp (wxMouseEvent & event);
    void OnMouseRightUp (wxMouseEvent & event);

private:
    DECLARE_EVENT_TABLE()
};

enum
{
    ID_Quit = 1
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU (ID_Quit, MyFrame::OnQuit)
    EVT_LEFT_UP (MyFrame::OnMouseLeftUp)
    EVT_RIGHT_UP (MyFrame::OnMouseRightUp)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame ("wxWindows Skeleton", wxPoint(20,20), wxSize(600,450));

    frame->SetBackgroundColour (wxColour (0,80,60));

    frame->Show (TRUE);
    SetTopWindow (frame);
    return TRUE;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append (ID_Quit, "E&xit");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append (menuFile, "&File");

    SetMenuBar (menuBar);
}

void MyFrame::OnQuit (wxCommandEvent &)
{
    Close (TRUE);
}


// Rotate with interpolation and with offset correction
void MyFrame::OnMouseLeftUp (wxMouseEvent & event)
{
    static double angle = 0.1;
    const double pi = 3.14159265359;

    wxImage img ("kclub.bmp", wxBITMAP_TYPE_BMP);

    wxPoint offset;
    wxImage img2 = img.Rotate(angle, wxPoint(img.GetWidth()/2, img.GetHeight()/2), TRUE, &offset);
    angle += 0.05;

    wxBitmap bmp = img2.ConvertToBitmap ();

    wxClientDC dc (this);
    dc.DrawBitmap (bmp, event.m_x + offset.x, event.m_y + offset.y);

    return;
}

// without interpolation, and without offset correction
void MyFrame::OnMouseRightUp (wxMouseEvent & event)
{
    static double angle = 0.1;
    const double pi = 3.14159265359;

    wxImage img ("kclub.bmp", wxBITMAP_TYPE_BMP);

    wxImage img2 = img.Rotate(angle, wxPoint(img.GetWidth()/2, img.GetHeight()/2), FALSE);
    angle += 0.05;

    wxBitmap bmp = img2.ConvertToBitmap ();

    wxClientDC dc (this);
    dc.DrawBitmap (bmp, event.m_x, event.m_y);

    return;
}
