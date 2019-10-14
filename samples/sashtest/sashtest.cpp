/////////////////////////////////////////////////////////////////////////////
// Name:        sashtest.cpp
// Purpose:     Layout/sash sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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

#include "sashtest.h"

MyFrame *frame = NULL;
wxList my_children;

wxIMPLEMENT_APP(MyApp);

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

int winNumber = 1;

// Initialise this in OnInit, not statically
bool MyApp::OnInit(void)
{
  if ( !wxApp::OnInit() )
    return false;

  // Create the main frame window

  frame = new MyFrame(NULL, wxID_ANY, "Sash Demo", wxPoint(0, 0), wxSize(500, 400),
                      wxDEFAULT_FRAME_STYLE |
                      wxHSCROLL | wxVSCROLL);

  // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("sashtest_icn"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(SASHTEST_NEW_WINDOW, "&New window");
  file_menu->Append(SASHTEST_TOGGLE_WINDOW, "&Toggle window");
  file_menu->Append(SASHTEST_QUIT, "&Exit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(SASHTEST_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
  frame->CreateStatusBar();
#endif // wxUSE_STATUSBAR

  frame->Show(true);

  return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(SASHTEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(SASHTEST_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(SASHTEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(SASHTEST_TOGGLE_WINDOW, MyFrame::OnToggleWindow)
    EVT_SASH_DRAGGED_RANGE(ID_WINDOW_TOP, ID_WINDOW_BOTTOM, MyFrame::OnSashDrag)
wxEND_EVENT_TABLE()


// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
    const long style):
  wxMDIParentFrame(parent, id, title, pos, size, style)
{
  // Create some dummy layout windows

  // A window like a toolbar
  wxSashLayoutWindow* win =
      new wxSashLayoutWindow(this, ID_WINDOW_TOP,
                             wxDefaultPosition, wxSize(200, 30),
                             wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

  win->SetDefaultSize(wxSize(1000, 30));
  win->SetOrientation(wxLAYOUT_HORIZONTAL);
  win->SetAlignment(wxLAYOUT_TOP);
  win->SetBackgroundColour(*wxRED);
  win->SetSashVisible(wxSASH_BOTTOM, true);

  m_topWindow = win;

  // A window like a statusbar
  win = new wxSashLayoutWindow(this, ID_WINDOW_BOTTOM,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(1000, 30));
  win->SetOrientation(wxLAYOUT_HORIZONTAL);
  win->SetAlignment(wxLAYOUT_BOTTOM);
  win->SetBackgroundColour(*wxBLUE);
  win->SetSashVisible(wxSASH_TOP, true);

  m_bottomWindow = win;

  // A window to the left of the client window
  win = new wxSashLayoutWindow(this, ID_WINDOW_LEFT1,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(120, 1000));
  win->SetOrientation(wxLAYOUT_VERTICAL);
  win->SetAlignment(wxLAYOUT_LEFT);
  win->SetBackgroundColour(*wxGREEN);
  win->SetSashVisible(wxSASH_RIGHT, true);
  win->SetExtraBorderSize(10);

  wxTextCtrl* textWindow = new wxTextCtrl(win, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE|wxSUNKEN_BORDER);
//        wxTE_MULTILINE|wxNO_BORDER);
  textWindow->SetValue("A help window");

  m_leftWindow1 = win;

  // Another window to the left of the client window
  win = new wxSashLayoutWindow(this, ID_WINDOW_LEFT2,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(120, 1000));
  win->SetOrientation(wxLAYOUT_VERTICAL);
  win->SetAlignment(wxLAYOUT_LEFT);
  win->SetBackgroundColour(wxColour(0, 255, 255));
  win->SetSashVisible(wxSASH_RIGHT, true);

  m_leftWindow2 = win;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
      Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
      (void)wxMessageBox("wxWidgets 2.0 Sash Demo\nAuthor: Julian Smart (c) 1998", "About Sash Demo");
}

void MyFrame::OnToggleWindow(wxCommandEvent& WXUNUSED(event))
{
    if (m_leftWindow1->IsShown())
    {
        m_leftWindow1->Show(false);
    }
    else
    {
        m_leftWindow1->Show(true);
    }
#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE
}

void MyFrame::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case ID_WINDOW_TOP:
        {
            m_topWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
        case ID_WINDOW_LEFT1:
        {
            m_leftWindow1->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }
        case ID_WINDOW_LEFT2:
        {
            m_leftWindow2->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }
        case ID_WINDOW_BOTTOM:
        {
            m_bottomWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
    }

#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event))
{
      // Make another frame, containing a canvas
      MyChild *subframe = new MyChild(frame, "Canvas Frame",
                                      wxPoint(10, 10), wxSize(300, 300));

      subframe->SetTitle(wxString::Format("Canvas Frame %d", winNumber));
      winNumber ++;

      // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
      subframe->SetIcon(wxIcon("sashtest_icn"));
#endif

#if wxUSE_STATUSBAR
      // Give it a status line
      subframe->CreateStatusBar();
#endif // wxUSE_STATUSBAR

      // Make a menubar
      wxMenu *file_menu = new wxMenu;

      file_menu->Append(SASHTEST_NEW_WINDOW, "&New window");
      file_menu->Append(SASHTEST_CHILD_QUIT, "&Close child");
      file_menu->Append(SASHTEST_QUIT, "&Exit");

      wxMenu *option_menu = new wxMenu;

      // Dummy option
      option_menu->Append(SASHTEST_REFRESH, "&Refresh picture");

      wxMenu *help_menu = new wxMenu;
      help_menu->Append(SASHTEST_ABOUT, "&About");

      wxMenuBar *menu_bar = new wxMenuBar;

      menu_bar->Append(file_menu, "&File");
      menu_bar->Append(option_menu, "&Options");
      menu_bar->Append(help_menu, "&Help");

      // Associate the menu bar with the frame
      subframe->SetMenuBar(menu_bar);

      int width, height;
      subframe->GetClientSize(&width, &height);
      MyCanvas *canvas = new MyCanvas(subframe, wxPoint(0, 0), wxSize(width, height));
      canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
      subframe->canvas = canvas;

      // Give it scrollbars
      canvas->SetScrollbars(20, 20, 50, 50);

      subframe->Show(true);
}

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnEvent)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, wxID_ANY, pos, size, wxSUNKEN_BORDER)
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
    dc.DrawText("This is a test string", 50, 230);

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
#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE
}

// Note that SASHTEST_NEW_WINDOW and SASHTEST_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.

wxBEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
  EVT_MENU(SASHTEST_CHILD_QUIT, MyChild::OnQuit)
wxEND_EVENT_TABLE()

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxMDIChildFrame(parent, wxID_ANY, title, pos, size)
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
      Close(true);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
  if (event.GetActive() && canvas)
    canvas->SetFocus();
}
