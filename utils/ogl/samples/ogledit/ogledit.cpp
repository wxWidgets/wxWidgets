/////////////////////////////////////////////////////////////////////////////
// Name:        ogledit.cpp
// Purpose:     OGLEdit sample app
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "ogledit.h"
#include "palette.h"
#include "doc.h"
#include "view.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "ogl.xpm"
#endif

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_APP(MyApp)

MyApp::MyApp(void)
{
  frame = NULL;
  myDocManager= NULL;
}

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
  wxOGLInitialize();
  
  //// Create a document manager
  myDocManager = new wxDocManager;

  //// Create a template relating drawing documents to their views
  (void) new wxDocTemplate(myDocManager, "Diagram", "*.dia", "", "dia", "Diagram Doc", "Diagram View",
          CLASSINFO(DiagramDocument), CLASSINFO(DiagramView));

  // If we've only got one window, we only get to edit
  // one document at a time.
  myDocManager->SetMaxDocsOpen(1);

  //// Create the main frame window
  frame = new MyFrame(myDocManager, NULL, "OGLEdit Demo", wxPoint(0, 0), wxSize(500, 400), wxDEFAULT_FRAME_STYLE);

  //// Give it an icon
  frame->SetIcon(wxICON(ogl));

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = NULL;

  file_menu->Append(wxID_NEW, "&New...");
  file_menu->Append(wxID_OPEN, "&Open...");

  file_menu->Append(wxID_CLOSE, "&Close");
  file_menu->Append(wxID_SAVE, "&Save");
  file_menu->Append(wxID_SAVEAS, "Save &As...");
  file_menu->AppendSeparator();
  file_menu->Append(wxID_PRINT, "&Print...");
  file_menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
  file_menu->Append(wxID_PREVIEW, "Print Pre&view");

  edit_menu = new wxMenu;
  edit_menu->Append(wxID_UNDO, "&Undo");
  edit_menu->Append(wxID_REDO, "&Redo");
  edit_menu->AppendSeparator();
  edit_menu->Append(OGLEDIT_CUT, "&Cut");
  edit_menu->AppendSeparator();
  edit_menu->Append(OGLEDIT_CHANGE_BACKGROUND_COLOUR, "Change &background colour");
  edit_menu->Append(OGLEDIT_EDIT_LABEL, "Edit &label");

  frame->editMenu = edit_menu;
  
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit");

  // A nice touch: a history of files visited. Use this menu.
  myDocManager->FileHistoryUseMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(OGLEDIT_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  if (edit_menu)
    menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(help_menu, "&Help");

  frame->canvas = frame->CreateCanvas(NULL, frame);
  frame->palette = wxGetApp().CreatePalette(frame);
  myDocManager->CreateDocument("", wxDOC_NEW);

  //// Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->CreateStatusBar(1);

  frame->Centre(wxBOTH);
  frame->Show(TRUE);

  return TRUE;
}

int MyApp::OnExit(void)
{
    wxOGLCleanUp();
    delete myDocManager;
    return 0;
}

/*
 * This is the top-level window of the application.
 */

IMPLEMENT_CLASS(MyFrame, wxDocParentFrame)

BEGIN_EVENT_TABLE(MyFrame, wxDocParentFrame)
    EVT_MENU(OGLEDIT_ABOUT, MyFrame::OnAbout)
    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

MyFrame::MyFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type):
  wxDocParentFrame(manager, frame, -1, title, pos, size, type)
{
  canvas = NULL;
  palette = NULL;
  editMenu = NULL;
}

void MyFrame::OnSize(wxSizeEvent& event)
{
  if (canvas && palette)
  {
    int cw, ch;
    GetClientSize(&cw, &ch);
    int paletteX = 0;
    int paletteY = 0;
    int paletteW = 30;
    int paletteH = ch;
    int canvasX = paletteX + paletteW;
    int canvasY = 0;
    int canvasW = cw - paletteW;
    int canvasH = ch;
    
    palette->SetSize(paletteX, paletteY, paletteW, paletteH);
    canvas->SetSize(canvasX, canvasY, canvasW, canvasH);
  }
}

bool MyFrame::OnClose(void)
{
  if (wxDocParentFrame::OnClose())
  {
    wxOGLCleanUp();
    return TRUE;
  }
  else
    return FALSE;
}

// Intercept menu commands
void MyFrame::OnAbout(wxCommandEvent& event)
{
      (void)wxMessageBox("OGLEdit Demo\nTo draw a shape, select a shape on the toolbar and left-click on the canvas.\nTo draw a line, right-drag between shapes.\nFor further details, see the OGL manual.\n (c) Julian Smart 1996", "About OGLEdit");
}

// Creates a canvas. Called by OnInit as a child of the main window
MyCanvas *MyFrame::CreateCanvas(wxView *view, wxFrame *parent)
{
  int width, height;
  parent->GetClientSize(&width, &height);

  // Non-retained canvas
  MyCanvas *canvas = new MyCanvas(view, parent, -1, wxPoint(0, 0), wxSize(width, height), 0);
  canvas->SetCursor(wxCursor(wxCURSOR_HAND));

  // Give it scrollbars
  canvas->SetScrollbars(20, 20, 50, 50);

  return canvas;
}

MyFrame *GetMainFrame(void)
{
  return wxGetApp().frame;
}

