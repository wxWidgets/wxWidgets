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

#include <wx/toolbar.h>

#ifdef __WXGTK__
#include "mondrian.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
// #include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/preview.xpm"
#include "bitmaps/help.xpm"
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
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("mdi_icn"));
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
    EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MDI_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(MDI_QUIT, MyFrame::OnQuit)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
	const long style):
  wxMDIParentFrame(parent, id, title, pos, size, style)
{
    textWindow = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE|wxSUNKEN_BORDER);
    textWindow->SetValue("A help window");

    CreateToolBar(wxNO_BORDER|wxTB_FLAT|wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());

#ifdef __WXMSW__
    // Accelerators
    wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_CTRL, (int) 'N', MDI_NEW_WINDOW);
    entries[1].Set(wxACCEL_CTRL, (int) 'X', MDI_QUIT);
    entries[2].Set(wxACCEL_CTRL, (int) 'A', MDI_ABOUT);
    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);
#endif
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
      Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
      (void)wxMessageBox("wxWindows 2.0 MDI Demo\nAuthor: Julian Smart (c) 1997\nUsage: mdi.exe", "About MDI Demo");
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event) )
{
      // Make another frame, containing a canvas
      MyChild *subframe = new MyChild(frame, "Canvas Frame", wxPoint(-1, -1), wxSize(-1, -1),
                             wxDEFAULT_FRAME);

      wxString title;
      title.Printf("Canvas Frame %d", winNumber);
      subframe->SetTitle(title);
      winNumber ++;

      // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
      subframe->SetIcon(wxIcon("chrt_icn"));
#else
      subframe->SetIcon(wxIcon(mondrian_xpm));
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

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event) )
{
    int w, h;
    GetClientSize(&w, &h);
    
    textWindow->SetSize(0, 0, 200, h);
    GetClientWindow()->SetSize(200, 0, w - 200, h);
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

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
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

void MyFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap* bitmaps[8];

#ifdef __WXMSW__
    bitmaps[0] = new wxBitmap("icon1", wxBITMAP_TYPE_RESOURCE);
    bitmaps[1] = new wxBitmap("icon2", wxBITMAP_TYPE_RESOURCE);
    bitmaps[2] = new wxBitmap("icon3", wxBITMAP_TYPE_RESOURCE);
    bitmaps[3] = new wxBitmap("icon4", wxBITMAP_TYPE_RESOURCE);
    bitmaps[4] = new wxBitmap("icon5", wxBITMAP_TYPE_RESOURCE);
    bitmaps[5] = new wxBitmap("icon6", wxBITMAP_TYPE_RESOURCE);
    bitmaps[6] = new wxBitmap("icon7", wxBITMAP_TYPE_RESOURCE);
    bitmaps[7] = new wxBitmap("icon8", wxBITMAP_TYPE_RESOURCE);
#else
  bitmaps[0] = new wxBitmap( new_xpm );
  bitmaps[1] = new wxBitmap( open_xpm );
  bitmaps[2] = new wxBitmap( save_xpm );
  bitmaps[3] = new wxBitmap( copy_xpm );
  bitmaps[4] = new wxBitmap( cut_xpm );
//  bitmaps[5] = new wxBitmap( paste_xpm );
  bitmaps[5] = new wxBitmap( preview_xpm );
  bitmaps[6] = new wxBitmap( print_xpm );
  bitmaps[7] = new wxBitmap( help_xpm );
#endif

#ifdef __WXMSW__
  int width = 24;
#else
  int width = 16;
#endif
  int currentX = 5;

  toolBar->AddTool(0, *bitmaps[0], wxNullBitmap, FALSE, currentX, -1, NULL, "New file");
  currentX += width + 5;
  toolBar->AddTool(1, *bitmaps[1], wxNullBitmap, FALSE, currentX, -1, NULL, "Open file");
  currentX += width + 5;
  toolBar->AddTool(2, *bitmaps[2], wxNullBitmap, FALSE, currentX, -1, NULL, "Save file");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(3, *bitmaps[3], wxNullBitmap, FALSE, currentX, -1, NULL, "Copy");
  currentX += width + 5;
  toolBar->AddTool(4, *bitmaps[4], wxNullBitmap, FALSE, currentX, -1, NULL, "Cut");
  currentX += width + 5;
  toolBar->AddTool(5, *bitmaps[5], wxNullBitmap, FALSE, currentX, -1, NULL, "Paste");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(6, *bitmaps[6], wxNullBitmap, FALSE, currentX, -1, NULL, "Print");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(7, *bitmaps[7], wxNullBitmap, TRUE, currentX, -1, NULL, "Help");

  toolBar->Realize();

  int i;
  for (i = 0; i < 8; i++)
    delete bitmaps[i];
}

