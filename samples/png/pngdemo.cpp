/////////////////////////////////////////////////////////////////////////////
// Name:        pngdemo.cpp
// Purpose:     Demos PNG reading
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "pngdemo.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef __WXMSW__
#include <wx/pnghand.h>
#endif

#include "pngdemo.h"

MyFrame   *frame = NULL;
wxBitmap  *g_TestBitmap = NULL;

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit(void)
{
#ifdef __WXMSW__
  wxBitmap::AddHandler(new wxPNGFileHandler);
#endif

  // Create the main frame window
  frame = new MyFrame(NULL, "wxPNGBitmap Demo", wxPoint(0, 0), wxSize(300, 300));

  // Give it a status line
  frame->CreateStatusBar(2);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *help_menu = new wxMenu;

  file_menu->Append(PNGDEMO_LOAD_FILE, "&Load file",                "Load file");
  file_menu->Append(PNGDEMO_QUIT, "E&xit",                "Quit program");
  help_menu->Append(PNGDEMO_ABOUT, "&About",              "About PNG demo");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  MyCanvas *canvas = new MyCanvas(frame, wxPoint(0, 0), wxSize(100, 100));

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
//  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
  frame->canvas = canvas;

  frame->Show(TRUE);

  frame->SetStatusText("Hello, wxWindows");

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(PNGDEMO_QUIT,      MyFrame::OnQuit)
    EVT_MENU(PNGDEMO_ABOUT,     MyFrame::OnAbout)
    EVT_MENU(PNGDEMO_LOAD_FILE, MyFrame::OnLoadFile)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, -1, title, pos, size)
{
  canvas = NULL;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("PNG demo\nJulian Smart (c) 1998",
            "About PNG Demo", wxOK);
}

void MyFrame::OnLoadFile(wxCommandEvent& WXUNUSED(event))
{
	// Show file selector.
	char *f = wxFileSelector("Open Image", NULL, NULL,"png",
		  "PNG files (*.png)|*.png");

	if (!f)
	  return;

    if ( g_TestBitmap )
        delete g_TestBitmap;
    g_TestBitmap = new wxBitmap(f, wxBITMAP_TYPE_PNG);
    if (!g_TestBitmap->Ok())
    {
        delete g_TestBitmap;
        g_TestBitmap = NULL;
    }

    canvas->Refresh();
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size):
 wxScrolledWindow(parent, -1, pos, size)
{
}

MyCanvas::~MyCanvas(void)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  dc.SetPen(wxRED_PEN);

  int i;
  for ( i = 0; i < 500; i += 10)
  {
    dc.DrawLine(0, i, 800, i);
  }
  if ( g_TestBitmap && g_TestBitmap->Ok() )
  {
    wxMemoryDC memDC;
    if ( g_TestBitmap->GetColourMap() )
    {
        memDC.SetColourMap(g_TestBitmap->GetColourMap());
        dc.SetColourMap(g_TestBitmap->GetColourMap());
    }
    memDC.SelectObject(g_TestBitmap);

    // Normal, non-transparent blitting
    dc.Blit(20, 20, g_TestBitmap->GetWidth(), g_TestBitmap->GetHeight(), & memDC, 0, 0, wxCOPY, FALSE);

    memDC.SelectObject(wxNullBitmap);
  }

  if ( g_TestBitmap && g_TestBitmap->Ok() )
  {
    wxMemoryDC memDC;
    memDC.SelectObject(g_TestBitmap);

    // Transparent blitting if there's a mask in the bitmap
    dc.Blit(20 + g_TestBitmap->GetWidth() + 20, 20, g_TestBitmap->GetWidth(), g_TestBitmap->GetHeight(), & memDC,
      0, 0, wxCOPY, TRUE);

    memDC.SelectObject(wxNullBitmap);
  }
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
bool MyFrame::OnClose(void)
{
  Show(FALSE);

  return TRUE;
}

