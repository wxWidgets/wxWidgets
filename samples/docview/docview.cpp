/////////////////////////////////////////////////////////////////////////////
// Name:        docview.cpp
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation "docview.h"
#endif

/*
 * Purpose:  Document/view architecture demo for wxWindows class library
 *           Run with no arguments for multiple top-level windows, -single
 *           for a single window.
 */


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !USE_DOC_VIEW_ARCHITECTURE
#error You must set USE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "wx/docview.h"

#include "docview.h"
#include "doc.h"
#include "view.h"

MyFrame *frame = NULL;

// In single window mode, don't have any child windows; use
// main window.
bool singleWindowMode = FALSE;

IMPLEMENT_APP(MyApp)

MyApp::MyApp(void)
{
    m_docManager = NULL;
}

bool MyApp::OnInit(void)
{
  //// Find out if we're:
  ////  SDI : multiple windows and documents but not MDI
  ////  MDI : multiple windows and documents with containing frame - MSW only)
  ///   single window : (one document at a time, only one frame, as in Windows Write)
  if (argc > 1)
  {
    if (strcmp(argv[1], "-single") == 0)
    {
      singleWindowMode = TRUE;
    }
  }

  //// Create a document manager
  m_docManager = new wxDocManager;

  //// Create a template relating drawing documents to their views
  (void) new wxDocTemplate(m_docManager, "Drawing", "*.drw", "", "drw", "Drawing Doc", "Drawing View",
          CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

  if (singleWindowMode)
  {
    // If we've only got one window, we only get to edit
    // one document at a time. Therefore no text editing, just
    // doodling.
    m_docManager->SetMaxDocsOpen(1);
  }
  else
    //// Create a template relating text documents to their views
    (void) new wxDocTemplate(m_docManager, "Text", "*.txt", "", "txt", "Text Doc", "Text View",
          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));

  //// Create the main frame window
  frame = new MyFrame(m_docManager, NULL, "DocView Demo", wxPoint(0, 0), wxSize(500, 400), wxDEFAULT_FRAME_STYLE);

  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("doc_icn"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = NULL;

  file_menu->Append(wxID_NEW, "&New...");
  file_menu->Append(wxID_OPEN, "&Open...");

  if (singleWindowMode)
  {
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
    edit_menu->Append(DOCVIEW_CUT, "&Cut last segment");

    frame->editMenu = edit_menu;
  }
  
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit");

  // A nice touch: a history of files visited. Use this menu.
  m_docManager->FileHistoryUseMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  if (edit_menu)
    menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(help_menu, "&Help");

  if (singleWindowMode)
    frame->canvas = frame->CreateCanvas(NULL, frame);

  //// Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Centre(wxBOTH);
  frame->Show(TRUE);

  SetTopWindow(frame);
  return TRUE;
}

int MyApp::OnExit(void)
{
    delete m_docManager;
    return 0;
}

/*
 * Centralised code for creating a document frame.
 * Called from view.cpp, when a view is created, but not used at all
 * in 'single window' mode.
 */
 
wxFrame *MyApp::CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas)
{
  //// Make a child frame
  wxDocChildFrame *subframe = new wxDocChildFrame(doc, view, GetMainFrame(), "Child Frame",
        wxPoint(10, 10), wxSize(300, 300), wxDEFAULT_FRAME_STYLE);

#ifdef __WXMSW__
  subframe->SetIcon(wxString(isCanvas ? "chrt_icn" : "notepad_icn"));
#endif
#ifdef __X__
  subframe->SetIcon(wxIcon("aiai.xbm"));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_NEW, "&New...");
  file_menu->Append(wxID_OPEN, "&Open...");
  file_menu->Append(wxID_CLOSE, "&Close");
  file_menu->Append(wxID_SAVE, "&Save");
  file_menu->Append(wxID_SAVEAS, "Save &As...");

  if (isCanvas)
  {
    file_menu->AppendSeparator();
    file_menu->Append(wxID_PRINT, "&Print...");
    file_menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
    file_menu->Append(wxID_PREVIEW, "Print Pre&view");
  }

  wxMenu *edit_menu = NULL;

  if (isCanvas)
  {
    edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, "&Undo");
    edit_menu->Append(wxID_REDO, "&Redo");
    edit_menu->AppendSeparator();
    edit_menu->Append(DOCVIEW_CUT, "&Cut last segment");

    doc->GetCommandProcessor()->SetEditMenu(edit_menu);
  }

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  if (isCanvas)
    menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(help_menu, "&Help");

  //// Associate the menu bar with the frame
  subframe->SetMenuBar(menu_bar);

  subframe->Centre(wxBOTH);

  return subframe;
}

/*
 * This is the top-level window of the application.
 */
 
IMPLEMENT_CLASS(MyFrame, wxDocParentFrame)
BEGIN_EVENT_TABLE(MyFrame, wxDocParentFrame)
    EVT_MENU(DOCVIEW_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long type):
  wxDocParentFrame(manager, frame, title, pos, size, type)
{
  // This pointer only needed if in single window mode
  canvas = NULL;
  editMenu = NULL;
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    (void)wxMessageBox("DocView Demo\nAuthor: Julian Smart julian.smart@ukonline.co.uk\nUsage: docview.exe [-single]", "About DocView");
}

// Creates a canvas. Called either from view.cc when a new drawing
// view is created, or in OnInit as a child of the main window,
// if in 'single window' mode.
MyCanvas *MyFrame::CreateCanvas(wxView *view, wxFrame *parent)
{
  int width, height;
  parent->GetClientSize(&width, &height);

  // Non-retained canvas
  MyCanvas *canvas = new MyCanvas(view, parent, wxPoint(0, 0), wxSize(width, height), 0);
  canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));

  // Give it scrollbars
  canvas->SetScrollbars(20, 20, 50, 50);

  return canvas;
}

MyFrame *GetMainFrame(void)
{
  return frame;
}

