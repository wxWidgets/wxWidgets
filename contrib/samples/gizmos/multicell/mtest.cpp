/*
 * File:	mtest.cpp
 * Purpose:	wxMultiCellSizer and wxMultiCellCanvas test
 * Author:	Alex Andruschak
 * Created:	2000
 * Updated:	
 * Copyright:
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
	#pragma implementation
	#pragma interface
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/gizmos/multicell.h"

class MyApp: public wxApp
{public:
	bool OnInit(void);
};

class MyFrame: public wxFrame
{

public:
	MyFrame(int type, wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnCloseWindow(wxCloseEvent& event);
	void OnPaint(wxPaintEvent& event);
	wxMultiCellSizer *sizer;

	DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
	MyFrame *frame = new MyFrame(1, (wxFrame *) NULL, (char *) "wxMultiCellSizer Sample", wxPoint(50, 50), wxDefaultSize); //, wxSize(600, 500));

	frame->Show(TRUE);

	SetTopWindow(frame);
	frame = new MyFrame(2, (wxFrame *) NULL, (char *) "wxMultiCellCanvas Sample", wxPoint(100, 100), wxSize(600, 500));

	frame->Show(TRUE);

	SetTopWindow(frame);
	return TRUE;
}



MyFrame::MyFrame(int type, wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
wxFrame(frame, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL)
{
	CreateStatusBar(1);
	sizer = NULL;
	if (type == 1)
	{
		// create sizer 4 columns 5 rows
		wxSize aa(4,9);
		sizer = new wxMultiCellSizer(aa);
		sizer->SetDefaultCellSize(wxSize(15,15));
		sizer->SetRowHeight(7,5,TRUE);
		sizer->SetRowHeight(8,5,FALSE);
		// add first row
		sizer->Add(
				  new wxButton( this, -1, "B1 - 0,0, horizontal resizable" ),
				  0, 0, 0, new wxMultiCellItemHandle(0,0,1,1, wxDefaultSize, wxHORIZENTAL_RESIZABLE, wxSize(2,2)));
		sizer->Add(
				  new wxButton( this, -1, "B2 - 0,1, vertical resizable" ),
				  0, 0, 0, new wxMultiCellItemHandle(0,1,1,1, wxDefaultSize, wxVERTICAL_RESIZABLE, wxSize(2, 2)));
		sizer->Add(
				  new wxButton( this, -1, "B3 - 0,2" ),
				  0, 0, 0, new wxMultiCellItemHandle(0,2,1,1, wxDefaultSize, wxNOT_RESIZABLE, wxSize(1,1), wxALIGN_CENTER_HORIZONTAL)); //, wxALIGN_CENTER));
		sizer->Add(
				  new wxStaticText(this, -1, "jbb 0,3, lower-right"),
				  0, 0, 0, new wxMultiCellItemHandle(0,3,1,1, wxDefaultSize, wxNOT_RESIZABLE, wxSize(1,1), wxALIGN_BOTTOM | wxALIGN_RIGHT));

		// add button for secord row
		sizer->Add(
				  new wxTextCtrl(this, -1, "Text control - 1,0, 4 cols wide"),
				  0, 0, 0, new wxMultiCellItemHandle(1,0,1,4));

		// add buttons for next row
		sizer->Add(
				  new wxButton( this, -1, "B6 - 2,0, 2 cols wide" ),
				  0, 0, 0, new wxMultiCellItemHandle(2,0,1,2));
		sizer->Add(
				  new wxButton( this, -1, "B7 - 2,3" ),
				  0, 0, 0, new wxMultiCellItemHandle(2,3,1,1));

		// and last additions
		sizer->Add(
				  new wxButton( this, -1, "B8 - 3,0, 4 rows high, vert resizable" ),
				  0, 0, 0, new wxMultiCellItemHandle(3,0,4,1,wxDefaultSize, wxVERTICAL_RESIZABLE));
		sizer->Add(
				  new wxButton( this, -1, "B9 - 3,2, 2 cols wide, vert resizable" ),
				  0, 0, 0, new wxMultiCellItemHandle(3,2,1,2,wxDefaultSize, wxVERTICAL_RESIZABLE));
		sizer->Add(
				  new wxButton( this, -1, "B10 - 4,1, 3 cols wide, vert resizable" ),
				  0, 0, 0, new wxMultiCellItemHandle(4,1,1,3,wxDefaultSize, wxVERTICAL_RESIZABLE));

		sizer->Add(
				  new wxButton( this, -1, "B11 - 5,1, 3 cols wide" ),
				  0, 0, 0, new wxMultiCellItemHandle(5,1,1,3));

		sizer->Add(
				  new wxButton( this, -1, "B12 - 6,1, 3 cols wide" ),
				  0, 0, 0, new wxMultiCellItemHandle(6,1,1,3));

		sizer->Add(
				  new wxButton( this, -1, "B13 - 7,1, 2 cols wide" ),
				  0, 0, 0, new wxMultiCellItemHandle(7,1,1,2));

		sizer->Add(
				  new wxButton( this, -1, "B14 - 8,1, 3 cols wide" ),
				  0, 0, 0, new wxMultiCellItemHandle(8,1,1,3));

		SetAutoLayout( TRUE );
//	sizer->SetMinSize(sizer->CalcMin());
		SetSizer( sizer );
		wxSize s = sizer->CalcMin();
		wxSize c = GetSize() - GetClientSize();
		SetSizeHints(s.GetWidth() + c.GetWidth() , s.GetHeight() + c.GetHeight());
		sizer->EnableGridLines(this);
	}
	else
	{
		// create sizer 4 columns 5 rows
		wxMultiCellCanvas *sizer = new wxMultiCellCanvas(this, 5,5);

		// add first row
		sizer->Add(
				  new wxButton( this, -1, "Button 1" ),
				  0, 0);
		sizer->Add(
				  new wxButton( this, -1, "Button 2" ),
				  0, 1);
		sizer->Add(
				  new wxButton( this, -1, "Button 3" ),
				  0, 2);
		sizer->Add(
				  new wxStaticText(this, -1, "jbb test"),
				  0, 3);

		sizer->Add(
				  new wxStaticText(this, -1, "jbb test 2"),
				  0, 4);

		// add button for secord row
		sizer->Add(
				  new wxTextCtrl(this, -1, "Text control"),
				  1, 0);

		// add buttons for next row
		sizer->Add(
				  new wxButton( this, -1, "Button 6" ),
				  2, 0);
		sizer->Add(
				  new wxButton( this, -1, "Button 7" ),
				  2, 3);

		// and last additions
		sizer->Add(
				  new wxButton( this, -1, "Button 8" ),
				  3, 0);
		sizer->Add(
				  new wxButton( this, -1, "Button 9" ),
				  3, 1);
		sizer->Add(
				  new wxButton( this, -1, "Button 10" ),
				  4, 1);

		sizer->CalculateConstraints();
		SetSizer( sizer );
		SetAutoLayout( TRUE );
	}
}
// Define the repainting behaviour


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
   EVT_PAINT(MyFrame::OnPaint)
   EVT_CLOSE(MyFrame::OnCloseWindow)
   END_EVENT_TABLE()

void MyFrame::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);

	if (sizer)
	{
		sizer->OnPaint(dc);
	}
}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
	Destroy();
}

