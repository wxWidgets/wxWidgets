/////////////////////////////////////////////////////////////////////////////
// Name:        splitter.cpp
// Purpose:     wxSplitterWindow sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/splitter.h"

class MyApp;
class MyFrame;
class MyCanvas;

class MyApp: public wxApp
{
public:
	bool OnInit();
};

class MyFrame: public wxFrame
{
public:
	MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual ~MyFrame();

	bool OnClose();

	// Menu commands
	void SplitHorizontal(wxCommandEvent& event);
	void SplitVertical(wxCommandEvent& event);
	void Unsplit(wxCommandEvent& event);
	void Quit(wxCommandEvent& event);

	// Menu command update functions
	void UpdateUIHorizontal(wxUpdateUIEvent& event);
	void UpdateUIVertical(wxUpdateUIEvent& event);
	void UpdateUIUnsplit(wxUpdateUIEvent& event);

    void OnIdle(wxIdleEvent& event);

private:
	wxMenu*		fileMenu;
	wxMenuBar*	menuBar;
	MyCanvas*	leftCanvas;
	MyCanvas*	rightCanvas;
    wxSplitterWindow* splitter;

DECLARE_EVENT_TABLE()
};

class MyCanvas: public wxScrolledWindow
{
public:
	MyCanvas(wxWindow* parent, int x, int y, int w, int h);
	virtual ~MyCanvas();

	virtual void OnDraw(wxDC& dc);

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
END_EVENT_TABLE()

// ID for the menu quit command
#define SPLIT_QUIT          1
#define SPLIT_HORIZONTAL    2
#define SPLIT_VERTICAL      3
#define SPLIT_UNSPLIT       4

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
	MyFrame* frame = new MyFrame(NULL, "wxSplitterWindow Example", wxPoint(50, 50), wxSize(400, 300));

	// Show the frame
	frame->Show(TRUE);
  
	SetTopWindow(frame);

	return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(SPLIT_VERTICAL, MyFrame::SplitVertical)
	EVT_MENU(SPLIT_HORIZONTAL, MyFrame::SplitHorizontal)
	EVT_MENU(SPLIT_UNSPLIT, MyFrame::Unsplit)
	EVT_MENU(SPLIT_QUIT, MyFrame::Quit)
	EVT_UPDATE_UI(SPLIT_VERTICAL, MyFrame::UpdateUIVertical)
	EVT_UPDATE_UI(SPLIT_HORIZONTAL, MyFrame::UpdateUIHorizontal)
	EVT_UPDATE_UI(SPLIT_UNSPLIT, MyFrame::UpdateUIUnsplit)
	EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size):
	wxFrame(frame, -1, title, pos, size)
{
	// set the icon
#ifdef __WINDOWS__
	SetIcon(wxIcon("mondrian"));
#endif
#ifdef __X__
	SetIcon(wxIcon("aiai.xbm"));
#endif

	CreateStatusBar(1);

	// Make a menubar
	fileMenu = new wxMenu;
	fileMenu->Append(SPLIT_VERTICAL, "Split &Vertically", "Split vertically");
	fileMenu->Append(SPLIT_HORIZONTAL, "Split &Horizontally", "Split horizontally");
	fileMenu->Append(SPLIT_UNSPLIT, "&Unsplit", "Unsplit");
	fileMenu->Append(SPLIT_QUIT, "E&xit", "Exit");

	menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, "&File");

	SetMenuBar(menuBar);

    splitter = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(400, 400),
//            wxSP_BORDER);
            wxSP_3D);
//            wxSP_NOBORDER);

	leftCanvas = new MyCanvas(splitter, 0, 0, 400, 400);
    leftCanvas->SetBackgroundColour(*wxRED);
    leftCanvas->SetScrollbars(20, 20, 50, 50);

	rightCanvas = new MyCanvas(splitter, 0, 0, 400, 400);
    rightCanvas->SetBackgroundColour(*wxCYAN);
    rightCanvas->SetScrollbars(20, 20, 50, 50);
    rightCanvas->Show(FALSE);

    splitter->Initialize(leftCanvas);

    // Set this to prevent unsplitting
//    splitter->SetMinimumPaneSize(20);
}

MyFrame::~MyFrame()
{
}

bool MyFrame::OnClose()
{
	return TRUE;
}

void MyFrame::Quit(wxCommandEvent& event)
{
	Close(TRUE);
}

void MyFrame::SplitHorizontal(wxCommandEvent& event)
{
    if ( splitter->IsSplit() )
        splitter->Unsplit();
    leftCanvas->Show(TRUE);
    rightCanvas->Show(TRUE);
    splitter->SplitHorizontally( leftCanvas, rightCanvas );
}

void MyFrame::SplitVertical(wxCommandEvent& event)
{
    if ( splitter->IsSplit() )
        splitter->Unsplit();
    leftCanvas->Show(TRUE);
    rightCanvas->Show(TRUE);
    splitter->SplitVertically( leftCanvas, rightCanvas );
}

void MyFrame::Unsplit(wxCommandEvent& event)
{
    if ( splitter->IsSplit() )
        splitter->Unsplit();
}

void MyFrame::UpdateUIHorizontal(wxUpdateUIEvent& event)
{
	event.Enable( ( (!splitter->IsSplit()) || (splitter->GetSplitMode() != wxSPLIT_HORIZONTAL) ) );
}

void MyFrame::UpdateUIVertical(wxUpdateUIEvent& event)
{
	event.Enable( ( (!splitter->IsSplit()) || (splitter->GetSplitMode() != wxSPLIT_VERTICAL) ) );
}

void MyFrame::UpdateUIUnsplit(wxUpdateUIEvent& event)
{
	event.Enable( splitter->IsSplit() );
}

void MyFrame::OnIdle(wxIdleEvent& event)
{
    if ( GetStatusBar()->GetStatusText(0) != "Ready" )
        SetStatusText("Ready");
}

MyCanvas::MyCanvas(wxWindow* parent, int x, int y, int w, int h) :
	wxScrolledWindow(parent, -1, wxPoint(x, y), wxSize(w, h))
{
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnDraw(wxDC& dc)
{
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawLine(0, 0, 100, 100);

    dc.SetBackgroundMode(wxTRANSPARENT);
	dc.DrawText("Testing", 50, 50);

    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxGREEN_BRUSH);
    dc.DrawRectangle(120, 120, 100, 80);
}
