/////////////////////////////////////////////////////////////////////////////
// Name:        expended.cpp
// Purpose:     Layout/foldpanelbar sample
// Author:      Jorgen Bodde
// Modified by:
// Created:     24/07/2004
// Copyright:   (c) Jorgen Bodde based upon FoldPanelBarTest (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wx/toolbar.h"
#include "wx/laywin.h"
#include <wx/spinctrl.h>
#include <wx/slider.h>

#include "extended.h"

MyFrame *frame = NULL;
wxList my_children;

IMPLEMENT_APP(MyApp)

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

int winNumber = 1;

// Initialise this in OnInit, not statically
bool MyApp::OnInit(void)
{
	// Create the main frame window

	frame = new MyFrame(NULL, -1, _T("FoldPanelBar Extended Demo"), wxPoint(-1, -1), wxSize(500, 600),
						wxDEFAULT_FRAME_STYLE |
						wxNO_FULL_REPAINT_ON_RESIZE |
						wxHSCROLL | wxVSCROLL);

	// Give it an icon (this is ignored in MDI mode: uses resources)

#ifdef __WXMSW__
	frame->SetIcon(wxIcon(_T("sashtest_icn")));
#endif

	// Associate the menu bar with the frame
	frame->SetMenuBar(CreateMenuBar(false));

	frame->CreateStatusBar();

	frame->Show(TRUE);

	SetTopWindow(frame);

	return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(FPBTEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(FPBTEST_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(FPBTEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(FPBTEST_TOGGLE_WINDOW, MyFrame::OnToggleWindow)
    EVT_SASH_DRAGGED_RANGE(ID_WINDOW_TOP, ID_WINDOW_BOTTOM, MyFrame::OnFoldPanelBarDrag)
    EVT_MENU(FPB_BOTTOM_STICK, MyFrame::OnCreateBottomStyle)
    EVT_MENU(FPB_SINGLE_FOLD, MyFrame::OnCreateNormalStyle)
	EVT_BUTTON(ID_COLLAPSEME, MyFrame::OnCollapseMe)
	EVT_BUTTON(ID_APPLYTOALL, MyFrame::OnExpandMe)
	EVT_SCROLL(MyFrame::OnSlideColour)
	EVT_RADIOBUTTON(ID_USE_HGRADIENT, MyFrame::OnStyleChange)
	EVT_RADIOBUTTON(ID_USE_VGRADIENT, MyFrame::OnStyleChange)
	EVT_RADIOBUTTON(ID_USE_SINGLE, MyFrame::OnStyleChange)
	EVT_RADIOBUTTON(ID_USE_RECTANGLE, MyFrame::OnStyleChange)
	EVT_RADIOBUTTON(ID_USE_FILLED_RECTANGLE, MyFrame::OnStyleChange)
END_EVENT_TABLE()


// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
    const long style)
  : wxMDIParentFrame(parent, id, title, pos, size, style)
  , _flags(0)
{  
	m_leftWindow1 = new wxSashLayoutWindow(this, ID_WINDOW_LEFT1,
								wxDefaultPosition, wxSize(200, 30),
								wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

	m_leftWindow1->SetDefaultSize(wxSize(160, 1000));
	m_leftWindow1->SetOrientation(wxLAYOUT_VERTICAL);
	m_leftWindow1->SetAlignment(wxLAYOUT_LEFT);
	m_leftWindow1->SetSashVisible(wxSASH_RIGHT, TRUE);
	m_leftWindow1->SetExtraBorderSize(0);

	_pnl = 0;
	ReCreateFoldPanel(0);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
      Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	(void)wxMessageBox(_T("wxWidgets 2.0 FoldPanelBar Demo\nAuthor: Julian Smart (c) 1998"), _T("About FoldPanelBar Demo"));
}

void MyFrame::OnToggleWindow(wxCommandEvent& WXUNUSED(event))
{
  	m_leftWindow1->Show(!m_leftWindow1->IsShown());
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
}

void MyFrame::OnFoldPanelBarDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    if(event.GetId() == ID_WINDOW_LEFT1)
        m_leftWindow1->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));

	wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event))
{
      // Make another frame, containing a canvas
      MyChild *subframe = new MyChild(frame, _T("Canvas Frame"),
                                      wxPoint(10, 10), wxSize(300, 300),
                                      wxDEFAULT_FRAME_STYLE |
                                      wxNO_FULL_REPAINT_ON_RESIZE);

      subframe->SetTitle(wxString::Format(_T("Canvas Frame %d"), winNumber));
      winNumber ++;

      // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
      subframe->SetIcon(wxIcon(_T("sashtest_icn")));
#endif

	// Give it a status line
	subframe->CreateStatusBar();

	// Associate the menu bar with the frame
	subframe->SetMenuBar(CreateMenuBar(true));

	int width, height;
	subframe->GetClientSize(&width, &height);
	MyCanvas *canvas = new MyCanvas(subframe, wxPoint(0, 0), wxSize(width, height));
	canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
	subframe->canvas = canvas;

	// Give it scrollbars
	canvas->SetScrollbars(20, 20, 50, 50);

	subframe->Show(TRUE);
}

void MyFrame::ReCreateFoldPanel(int fpb_flags)
{
    // delete earlier panel
	m_leftWindow1->DestroyChildren();

	// recreate the foldpanelbar

	_pnl = new wxFoldPanelBar(m_leftWindow1, -1, wxDefaultPosition, wxSize(-1,-1), wxFPB_DEFAULT_STYLE, fpb_flags);
		
	wxFoldPanel item = _pnl->AddFoldPanel("Caption colours", false);

	_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), -1, _T("Adjust the first colour")), 
		                     wxFPB_ALIGN_WIDTH, 5, 20); 

	// RED color spin control
	_rslider1 = new wxSlider(item.GetParent(), -1, 0, 0, 255);
	_pnl->AddFoldPanelWindow(item, _rslider1, wxFPB_ALIGN_WIDTH, 
							 2, 20); 

	// GREEN color spin control
	_gslider1 = new wxSlider(item.GetParent(), -1, 0, 0, 255);
	_pnl->AddFoldPanelWindow(item, _gslider1, wxFPB_ALIGN_WIDTH, 
							 0, 20); 

	// BLUE color spin control
	_bslider1 = new wxSlider(item.GetParent(), -1, 0, 0, 255);
	_pnl->AddFoldPanelWindow(item, _bslider1, wxFPB_ALIGN_WIDTH, 
							 0, 20); 
	
	_pnl->AddFoldPanelSeperator(item);

	_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), -1, _T("Adjust the second colour")), 
		                     wxFPB_ALIGN_WIDTH, 5, 20); 

	// RED color spin control
	_rslider2 = new wxSlider(item.GetParent(), -1, 0, 0, 255);
	_pnl->AddFoldPanelWindow(item, _rslider2, wxFPB_ALIGN_WIDTH, 
							 2, 20); 

	// GREEN color spin control
	_gslider2 = new wxSlider(item.GetParent(), -1, 0, 0, 255);
	_pnl->AddFoldPanelWindow(item, _gslider2, wxFPB_ALIGN_WIDTH, 
							 0, 20); 

	// BLUE color spin control
	_bslider2 = new wxSlider(item.GetParent(), -1, 0, 0, 255);
	_pnl->AddFoldPanelWindow(item, _bslider2, wxFPB_ALIGN_WIDTH, 
							 0, 20); 

	_pnl->AddFoldPanelSeperator(item);
	
	_btn = new wxButton(item.GetParent(), ID_APPLYTOALL, "Apply to all");
	_pnl->AddFoldPanelWindow(item, _btn); 

	// read back current gradients and set the sliders
	// for the colour which is now taken as default

	wxCaptionBarStyle style = _pnl->GetCaptionStyle(item);
	wxColour col = style.GetFirstColour();
	_rslider1->SetValue(col.Red());
	_gslider1->SetValue(col.Green());
	_bslider1->SetValue(col.Blue());

	col = style.GetSecondColour();
	_rslider2->SetValue(col.Red());
	_gslider2->SetValue(col.Green());
	_bslider2->SetValue(col.Blue());

	// put down some caption styles from which the user can
	// select to show how the current or all caption bars will look like

	item = _pnl->AddFoldPanel("Caption style", false);

	wxRadioButton *currStyle =  new wxRadioButton(item.GetParent(), ID_USE_VGRADIENT, "&Vertical gradient");
	_pnl->AddFoldPanelWindow(item, currStyle,  wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 
	currStyle->SetValue(true);

	_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_HGRADIENT, "&Horizontal gradient"), 
							 wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 
	_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_SINGLE, "&Single colour"), 
							 wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 
	_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_RECTANGLE, "&Rectangle box"), 
							 wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 
	_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_FILLED_RECTANGLE, "&Filled rectangle box"), 
							 wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 

	_pnl->AddFoldPanelSeperator(item);

	_single = new wxCheckBox(item.GetParent(), -1, "&Only this caption");
	_pnl->AddFoldPanelWindow(item, _single, wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 


	// one more panel to finish it

	wxCaptionBarStyle cs;
	cs.SetCaptionStyle(wxCAPTIONBAR_RECTANGLE);

	item = _pnl->AddFoldPanel("Misc stuff", true, cs);

	_pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_COLLAPSEME, "Collapse All")); 

	_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), -1, _T("Enter some comments")), 
		                     wxFPB_ALIGN_WIDTH, 5, 20); 
	
	_pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), -1, "Comments"), 
							 wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 10); 

	m_leftWindow1->SizeWindows();

}

void MyFrame::OnCreateBottomStyle(wxCommandEvent& event)
{
	// recreate with style collapse to bottom, which means
	// all panels that are collapsed are placed at the bottom,
	// or normal
	
	if(event.IsChecked())
		_flags |= wxFPB_COLLAPSE_TO_BOTTOM;
	else
		_flags &= ~wxFPB_COLLAPSE_TO_BOTTOM;

	ReCreateFoldPanel(_flags);
}

void MyFrame::OnCreateNormalStyle(wxCommandEvent& event)
{
	// receate with style where only one panel at the time is
	// allowed to be opened
	
	// TODO: Not yet implemented!

	if(event.IsChecked())
		_flags |= wxFPB_SINGLE_FOLD;
	else
		_flags &= ~wxFPB_SINGLE_FOLD;

	ReCreateFoldPanel(_flags);
}

void MyFrame::OnCollapseMe(wxCommandEvent &event)
{
	wxFoldPanel item(0);
	for(size_t i = 0; i < _pnl->GetCount(); i++)
	{
		item = _pnl->Item(i);
		_pnl->Collapse(item);
	}
}

void MyFrame::OnExpandMe(wxCommandEvent &event)
{
	wxColour col1(_rslider1->GetValue(), _gslider1->GetValue(), _bslider1->GetValue()),
		     col2(_rslider2->GetValue(), _gslider2->GetValue(), _bslider2->GetValue());

	wxCaptionBarStyle style;

	style.SetFirstColour(col1);
	style.SetSecondColour(col2);

	_pnl->ApplyCaptionStyleAll(style);
}

wxMenuBar *CreateMenuBar(bool with_window)
{
	// Make a menubar
	wxMenu *file_menu = new wxMenu;

	file_menu->Append(FPBTEST_NEW_WINDOW, _T("&New window"));
	if(with_window)
		file_menu->Append(FPBTEST_CHILD_QUIT, _T("&Close child"));

	file_menu->AppendSeparator();
	file_menu->Append(FPBTEST_QUIT, _T("&Exit"));

	wxMenu *option_menu = 0;
	if(with_window)
	{
		// Dummy option
		option_menu = new wxMenu;
		option_menu->Append(FPBTEST_REFRESH, _T("&Refresh picture"));
	}

	// make fold panel menu
	
	wxMenu *fpb_menu = new wxMenu;
	fpb_menu->AppendCheckItem(FPB_BOTTOM_STICK, _T("Create with &wxFPB_COLLAPSE_TO_BOTTOM"));
	//fpb_menu->AppendCheckItem(FPB_SINGLE_FOLD, _T("Create with &wxFPB_SINGLE_FOLD"));

	fpb_menu->AppendSeparator();
	fpb_menu->Append(FPBTEST_TOGGLE_WINDOW, _T("&Toggle FoldPanelBar"));

	wxMenu *help_menu = new wxMenu;
	help_menu->Append(FPBTEST_ABOUT, _T("&About"));

	wxMenuBar *menu_bar = new wxMenuBar;

	menu_bar->Append(file_menu, _T("&File"));
	menu_bar->Append(fpb_menu, _T("&FoldPanel"));
	if(option_menu)
		menu_bar->Append(option_menu, _T("&Options"));
	menu_bar->Append(help_menu, _T("&Help"));

	return menu_bar;
}

void MyFrame::OnSlideColour(wxScrollEvent &event)
{
	wxColour col1(_rslider1->GetValue(), _gslider1->GetValue(), _bslider1->GetValue()),
		     col2(_rslider2->GetValue(), _gslider2->GetValue(), _bslider2->GetValue());
	//_btn->SetBackgroundColour(col);

	wxCaptionBarStyle style;

	style.SetFirstColour(col1);
	style.SetSecondColour(col2);

	wxFoldPanel item = _pnl->Item(0);
	_pnl->ApplyCaptionStyle(item, style);
}

void MyFrame::OnStyleChange(wxCommandEvent &event)
{
	wxCaptionBarStyle style;
	switch(event.GetId())
	{
	case ID_USE_HGRADIENT:
		style.SetCaptionStyle(wxCAPTIONBAR_GRADIENT_H);
		break;

	case ID_USE_VGRADIENT:
		style.SetCaptionStyle(wxCAPTIONBAR_GRADIENT_V);
		break;

	case ID_USE_SINGLE:
		style.SetCaptionStyle(wxCAPTIONBAR_SINGLE);
		break;

	case ID_USE_RECTANGLE:
		style.SetCaptionStyle(wxCAPTIONBAR_RECTANGLE);
		break;

	case ID_USE_FILLED_RECTANGLE:
		style.SetCaptionStyle(wxCAPTIONBAR_FILLED_RECTANGLE);
		break;
			
	default:
		break;
	}

	if(_single->GetValue())
	{
		wxFoldPanel item = _pnl->Item(1);
		_pnl->ApplyCaptionStyle(item, style);
	}
	else
		_pnl->ApplyCaptionStyleAll(style);

}

/* ----------------------------------------------------------------------------------------------- */

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size,
                           wxSUNKEN_BORDER | wxNO_FULL_REPAINT_ON_RESIZE)
{
    SetBackgroundColour(* wxWHITE);
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
  dc.SetFont(*wxSWISS_FONT);
  dc.SetPen(*wxGREEN_PEN);
  dc.DrawLine(0, 0, 200, 200);
  dc.DrawLine(200, 0, 0, 200);

  dc.SetBrush(*wxCYAN_BRUSH);
  dc.SetPen(*wxRED_PEN);
  dc.DrawRectangle(100, 100, 100, 50);
  dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

  dc.DrawEllipse(250, 250, 100, 50);
#if wxUSE_SPLINES
  dc.DrawSpline(50, 200, 50, 100, 200, 10);
#endif // wxUSE_SPLINES
  dc.DrawLine(50, 230, 200, 230);
  dc.DrawText(_T("This is a test string"), 50, 230);

  wxPoint points[3];
  points[0].x = 200; points[0].y = 300;
  points[1].x = 100; points[1].y = 400;
  points[2].x = 300; points[2].y = 400;
  
  dc.DrawPolygon(3, points);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
  wxClientDC dc(this);
  PrepareDC(dc);

  wxPoint pt(event.GetLogicalPosition(dc));

  if (xpos > -1 && ypos > -1 && event.Dragging())
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(xpos, ypos, pt.x, pt.y);
  }
  xpos = pt.x;
  ypos = pt.y;
}

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
}

// Note that FPBTEST_NEW_WINDOW and FPBTEST_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.

BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
  EVT_MENU(FPBTEST_CHILD_QUIT, MyChild::OnQuit)
END_EVENT_TABLE()

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
const long style):
  wxMDIChildFrame(parent, -1, title, pos, size, style)
{
  canvas = NULL;
  my_children.Append(this);
}

MyChild::~MyChild(void)
{
  my_children.DeleteObject(this);
}

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
      Close(TRUE);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
  if (event.GetActive() && canvas)
    canvas->SetFocus();
}

