///////////////////////////////////////////////////////////////////////////////
// Name:        fractal.cpp
// Purpose:     demo of wxConfig and related classes
// Author:      Andrew Davison
// Modified by:
// Created:     05.04.94
// Copyright:   (c) 1994 Andrew Davison
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


/*
Date: Tue, 5 Apr 1994 12:01:18 +1000
From: Andrew Davison <andrewd@au.com.sfe>
To: wxwin-users@ed.aiai
Subject: Fractal mountains

Hi,

This is a quick port of a fractal mountain generator originally
done for MS-Windows. On a Sun the colours look a little washed
out and there is not as much snow or high mountains (maybe the
random number generators fault). The viewing plane is not
quite right as the original code used SetViewportOrg() which there
doesn't seem to be an equivalent of under wxWidgets, and my quick
hack doesn't fix.
*/

#include "wx/wxprec.h"


#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/math.h"
#include "wx/stockitem.h"

#include <stdlib.h>
#include <time.h>

#define Random(x) (rand() % x)
#define Randomize() (srand((unsigned int)time(nullptr)))

static int detail = 9; // CHANGE THIS... 7,8,9 etc

static bool running = false;
static wxMenuBar *menuBar = nullptr;

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

// Define a new canvas which can receive some events
class MyCanvas: public wxWindow
{
public:
    MyCanvas(wxFrame *frame);
    void Draw(wxDC& dc);

private:
    void OnPaint(wxPaintEvent& event);
    void Fractal(wxDC& dc, int X1, int Y1, int X2, int Y2, int Z1, int Z2, int Z3, int Z4, int Iteration, double Std, double Ratio);
    wxPen SnowPen, MtnPen, GreenPen;
    wxBrush WaterBrush;
    int Sealevel;

    wxDECLARE_EVENT_TABLE();
};

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(nullptr, wxT("Fractal Mountains for wxWidgets"), wxDefaultPosition, wxSize(640, 480));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(wxID_EXIT, wxGetStockLabel(wxID_EXIT));
  menuBar = new wxMenuBar;
  menuBar->Append(file_menu, wxT("&File"));
  frame->SetMenuBar(menuBar);

  int width, height;
  frame->GetClientSize(&width, &height);

  (void) new MyCanvas(frame);

  // Show the frame
  frame->Show(true);

  return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_CLOSE(MyFrame::OnCloseWindow)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
wxEND_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE )
{
}

// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    this->Destroy();
}

void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    static bool destroyed = false;
    if (destroyed)
        return;

    this->Destroy();

    destroyed = true;
}

wxBEGIN_EVENT_TABLE(MyCanvas, wxWindow)
  EVT_PAINT(MyCanvas::OnPaint)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame):
 wxWindow(frame, wxID_ANY)
{
    wxColour wxCol1(255,255,255);
    SnowPen = wxPen(wxCol1, 2);

    wxColour wxCol2(128,0,0);
    MtnPen = wxPen(wxCol2);

    wxColour wxCol3(0,128,0);
    GreenPen = wxPen(wxCol3);

    wxColour wxCol4(0,0,128);
    WaterBrush = wxBrush(wxCol4);
}

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);
    Draw(dc);
}

void MyCanvas::Draw(wxDC& dc)
{
    if (running) return;

    running = true;
    menuBar->EnableTop(0, false);

    Randomize();

    dc.SetBackground(*wxLIGHT_GREY_BRUSH);
    dc.Clear();

    int Left, Top, Right, Bottom;
    GetClientSize(&Right, &Bottom);

    Right *= 3; Right /= 4;
    Bottom *= 3; Bottom /= 4;
    Left = 0;
    Top = Bottom/8;

    wxPoint Water[4];
    Water[0].x = Left;            Water[0].y = Top;
    Water[1].x = Right;           Water[1].y = Top;
    Water[2].x = Right+Bottom/2;  Water[2].y = Bottom;
    Water[3].x = Bottom/2;        Water[3].y = Bottom;

    dc.SetBrush(WaterBrush);
    dc.DrawPolygon(4, Water);

    double H = 0.75;
    double Scale = Bottom;
    double Ratio = 1.0 / pow(2.0, H);
    double Std = Scale * Ratio;
    Sealevel = Random(18) - 8;

    Fractal(dc, Left, Top, Right, Bottom, 0, 0, 0, 0, detail, Std, Ratio);

    menuBar->EnableTop(0, true);
    running = false;
}

void MyCanvas::Fractal(wxDC& dc, int X1, int Y1, int X2, int Y2, int Z1, int Z2, int Z3, int Z4, int Iteration, double Std, double Ratio)
{
    int Xmid = (X1 + X2) / 2;
    int Ymid = (Y1 + Y2) / 2;
    int Z23 = (Z2 + Z3) / 2;
    int Z41 = (Z4 + Z1) / 2;
    int Newz = (int)((Z1 + Z2 + Z3 + Z4) / 4 + (double)(Random(17) - 8) / 8.0 * Std);

    if (--Iteration)
    {
        int Z12 = (Z1 + Z2) / 2;
        int Z34 = (Z3 + Z4) / 2;
        double Stdmid = Std * Ratio;

        Fractal(dc, Xmid, Y1, X2, Ymid, Z12, Z2, Z23, Newz, Iteration, Stdmid, Ratio);
        Fractal(dc, X1, Y1, Xmid, Ymid, Z1, Z12, Newz, Z41, Iteration, Stdmid, Ratio);
        Fractal(dc, Xmid, Ymid, X2, Y2, Newz, Z23, Z3, Z34, Iteration, Stdmid, Ratio);
        Fractal(dc, X1, Ymid, Xmid, Y2, Z41, Newz, Z34, Z4, Iteration, Stdmid, Ratio);
    }
    else
    {
        if (Newz <= Sealevel)
        {
            wxPoint P[4];
            P[0].x = Y1 / 2 + X1;    P[0].y = Y1 + Z1;
            P[1].x = Y1 / 2 + X2;    P[1].y = Y1 + Z2;
            P[2].x = Y2 / 2 + X2;    P[2].y = Y2 + Z3;
            P[3].x = Y2 / 2 + X1;    P[3].y = Y2 + Z4;

            dc.SetPen(* wxBLACK_PEN);
            dc.SetBrush(* wxBLACK_BRUSH);

            dc.DrawPolygon(4, P);

            if (Z1 >= -(60+Random(25)))
                dc.SetPen(GreenPen);
            else if (Z1 >= -(100+Random(25)))
                dc.SetPen(MtnPen);
            else
                dc.SetPen(SnowPen);

            dc.DrawLine(Ymid/2+X2, Ymid+Z23, Ymid/2+X1, Ymid+Z41);
        }
    }
}

