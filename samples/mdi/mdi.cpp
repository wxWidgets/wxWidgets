/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
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
#include "wx/mdi.h"
#endif

#ifdef __WINDOWS__
#ifdef __WIN95__
#include <wx/tbar95.h>
#else
#include <wx/tbarmsw.h>
#endif
#endif

#include "mdi.h"

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

  frame = new MyFrame(NULL, -1, "MDI Demo", wxPoint(0, 0), wxSize(500, 400),
   wxDEFAULT_FRAME | wxHSCROLL | wxVSCROLL);

  // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WINDOWS__
  frame->SetIcon(wxIcon("mdi_icn"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MDI_NEW_WINDOW, "&New window");
  file_menu->Append(MDI_QUIT, "&Exit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(MDI_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->CreateStatusBar();

  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(MDI_QUIT, MyFrame::OnQuit)
    EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MDI_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
	const long style):
  wxMDIParentFrame(parent, id, title, pos, size, style)
{
    textWindow = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE|wxSUNKEN_BORDER);
    textWindow->SetValue("A help window");

#ifdef __WINDOWS__
    toolBar = new TestRibbon(this, 0, 0, 100, 30, wxNO_BORDER, wxVERTICAL, 1);
    SetToolBar(toolBar);
#endif
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
      Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
      (void)wxMessageBox("wxWindows 2.0 MDI Demo\nAuthor: Julian Smart (c) 1997\nUsage: mdi.exe", "About MDI Demo");
}

void MyFrame::OnNewWindow(wxCommandEvent& event)
{
      // Make another frame, containing a canvas
      MyChild *subframe = new MyChild(frame, "Canvas Frame", wxPoint(10, 10), wxSize(300, 300),
                             wxDEFAULT_FRAME);

      char titleBuf[100];
      sprintf(titleBuf, "Canvas Frame %d", winNumber);
      subframe->SetTitle(titleBuf);
      winNumber ++;

      // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WINDOWS__
      subframe->SetIcon(wxIcon("chrt_icn"));
#endif
#ifdef __X__
      subframe->SetIcon(wxIcon("aiai.xbm"));
#endif

      // Give it a status line
      subframe->CreateStatusBar();

      // Make a menubar
      wxMenu *file_menu = new wxMenu;

      file_menu->Append(MDI_NEW_WINDOW, "&New window");
      file_menu->Append(MDI_CHILD_QUIT, "&Close child");
      file_menu->Append(MDI_QUIT, "&Exit");

      wxMenu *option_menu = new wxMenu;

      // Dummy option
      option_menu->Append(MDI_REFRESH, "&Refresh picture");

      wxMenu *help_menu = new wxMenu;
      help_menu->Append(MDI_ABOUT, "&About");

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

      subframe->Show(TRUE);
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
	EVT_MOUSE_EVENTS(MyCanvas::OnEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size):
 wxScrolledWindow(parent, -1, pos, size, wxSUNKEN_BORDER)
{
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
  dc.DrawSpline(50, 200, 50, 100, 200, 10);
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

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
bool MyFrame::OnClose(void)
{
  // Must delete children
  wxNode *node = my_children.First();
  while (node)
  {
    MyChild *child = (MyChild *)node->Data();
    wxNode *next = node->Next();
    child->OnClose();
    delete child;
    node = next;
  }
  return TRUE;
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);
    int tw = 0;
    int th = 0;
    
#ifdef __WINDOWS__
    wxWindow* tbar = GetToolBar();
    if (tbar)
    {
        tbar->GetSize(&tw, &th);
        tbar->SetSize(w, th);
    }
#endif

    textWindow->SetSize(0, th, 200, h-th);
    GetClientWindow()->SetSize(200, th, w - 200, h-th);
}

// Note that MDI_NEW_WINDOW and MDI_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.

BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
  EVT_MENU(MDI_CHILD_QUIT, MyChild::OnQuit)
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

void MyChild::OnQuit(wxCommandEvent& event)
{
      Close(TRUE);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
  if (event.GetActive() && canvas)
    canvas->SetFocus();
}

bool MyChild::OnClose(void)
{
  return TRUE;
}

#ifdef __WINDOWS__

BEGIN_EVENT_TABLE(TestRibbon, wxToolBar)
    EVT_PAINT(TestRibbon::OnPaint)
END_EVENT_TABLE()

TestRibbon::TestRibbon(wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
  wxToolBar(frame, -1, wxPoint(x, y), wxSize(w, h), style, direction, RowsOrColumns)
{
    wxBitmap* bitmaps[8];

    bitmaps[0] = new wxBitmap("icon1", wxBITMAP_TYPE_RESOURCE);
    bitmaps[1] = new wxBitmap("icon2", wxBITMAP_TYPE_RESOURCE);
    bitmaps[2] = new wxBitmap("icon3", wxBITMAP_TYPE_RESOURCE);
    bitmaps[3] = new wxBitmap("icon4", wxBITMAP_TYPE_RESOURCE);
    bitmaps[4] = new wxBitmap("icon5", wxBITMAP_TYPE_RESOURCE);
    bitmaps[5] = new wxBitmap("icon6", wxBITMAP_TYPE_RESOURCE);
    bitmaps[6] = new wxBitmap("icon7", wxBITMAP_TYPE_RESOURCE);
    bitmaps[7] = new wxBitmap("icon8", wxBITMAP_TYPE_RESOURCE);

#ifdef __WINDOWS__
  int width = 24;
#else
  int width = 16;
#endif
  int offX = 5;
  int currentX = 5;

  AddTool(0, *bitmaps[0], wxNullBitmap, FALSE, currentX, -1, NULL, "New file");
  currentX += width + 5;
  AddTool(1, *bitmaps[1], wxNullBitmap, FALSE, currentX, -1, NULL, "Open file");
  currentX += width + 5;
  AddTool(2, *bitmaps[2], wxNullBitmap, FALSE, currentX, -1, NULL, "Save file");
  currentX += width + 5;
  AddSeparator();
  AddTool(3, *bitmaps[3], wxNullBitmap, FALSE, currentX, -1, NULL, "Copy");
  currentX += width + 5;
  AddTool(4, *bitmaps[4], wxNullBitmap, FALSE, currentX, -1, NULL, "Cut");
  currentX += width + 5;
  AddTool(5, *bitmaps[5], wxNullBitmap, FALSE, currentX, -1, NULL, "Paste");
  currentX += width + 5;
  AddSeparator();
  AddTool(6, *bitmaps[6], wxNullBitmap, FALSE, currentX, -1, NULL, "Print");
  currentX += width + 5;
  AddSeparator();
  AddTool(7, *bitmaps[7], wxNullBitmap, TRUE, currentX, -1, NULL, "Help");

  CreateTools();

  int i;
  for (i = 0; i < 8; i++)
    delete bitmaps[i];
}

bool TestRibbon::OnLeftClick(int toolIndex, bool toggled)
{
  char buf[200];
  sprintf(buf, "Clicked on tool %d", toolIndex);
  frame->SetStatusText(buf);
  return TRUE;
}

void TestRibbon::OnMouseEnter(int toolIndex)
{
  char buf[200];
  if (toolIndex > -1)
  {
    sprintf(buf, "This is tool number %d", toolIndex);
    frame->SetStatusText(buf);
  }
  else frame->SetStatusText("");
}

void TestRibbon::OnPaint(wxPaintEvent& event)
{
  wxToolBar::OnPaint(event);

  wxPaintDC dc(this);
  
  int w, h;
  GetSize(&w, &h);
  dc.SetPen(*wxBLACK_PEN);
  dc.SetBrush(*wxTRANSPARENT_BRUSH);
  dc.DrawLine(0, h-1, w, h-1);
}

#endif
