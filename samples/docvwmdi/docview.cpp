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
 * Purpose:  Document/view architecture demo for wxWindows class library - MDI
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
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "docview.h"
#include "doc.h"
#include "view.h"

MyFrame *frame = (MyFrame *) NULL;

IMPLEMENT_APP(MyApp)

MyApp::MyApp(void)
{
    m_docManager = (wxDocManager *) NULL;
}

bool MyApp::OnInit(void)
{
  //// Create a document manager
  m_docManager = new wxDocManager;

  //// Create a template relating drawing documents to their views
  (void) new wxDocTemplate((wxDocManager *) m_docManager, "Drawing", "*.drw", "", "drw", "Drawing Doc", "Drawing View",
          CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

  //// Create a template relating text documents to their views
  (void) new wxDocTemplate(m_docManager, "Text", "*.txt", "", "txt", "Text Doc", "Text View",
          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));

  //// Create the main frame window
  frame = new MyFrame((wxDocManager *) m_docManager, (wxFrame *) NULL, (const wxString) "DocView Demo", wxPoint(0, 0), wxSize(500, 400), wxDEFAULT_FRAME_STYLE);

  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("doc"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("doc.xbm"));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = (wxMenu *) NULL;

  file_menu->Append(wxID_NEW, "&New...");
  file_menu->Append(wxID_OPEN, "&Open...");

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
 * Called from view.cpp, when a view is created.
 */
 
wxMDIChildFrame *MyApp::CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas)
{
  //// Make a child frame
  wxDocMDIChildFrame *subframe = new wxDocMDIChildFrame(doc, view, GetMainFrame(), -1, "Child Frame",
        wxPoint(10, 10), wxSize(300, 300), wxDEFAULT_FRAME_STYLE);

#ifdef __WXMSW__
  subframe->SetIcon(wxString(isCanvas ? "chart" : "notepad"));
#endif
#ifdef __X__
  subframe->SetIcon(wxIcon("doc.xbm"));
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

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit");

  wxMenu *edit_menu = (wxMenu *) NULL;

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

  return subframe;
}

/*
 * This is the top-level window of the application.
 */
 
IMPLEMENT_CLASS(MyFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(MyFrame, wxDocMDIParentFrame)
    EVT_MENU(DOCVIEW_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type):
  wxDocMDIParentFrame(manager, frame, -1, title, pos, size, type, "myFrame")
{
  editMenu = (wxMenu *) NULL;
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox("DocView Demo\nAuthor: Julian Smart julian.smart@ukonline.co.uk\nUsage: docview.exe", "About DocView");
}

// Creates a canvas. Called from view.cpp when a new drawing
// view is created.
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

