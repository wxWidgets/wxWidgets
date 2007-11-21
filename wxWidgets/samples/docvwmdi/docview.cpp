/////////////////////////////////////////////////////////////////////////////
// Name:        docview.cpp
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*
 * Purpose:  Document/view architecture demo for wxWidgets class library - MDI
 */


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#if !wxUSE_MDI_ARCHITECTURE
#error You must set wxUSE_MDI_ARCHITECTURE to 1 in setup.h!
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
  (void) new wxDocTemplate((wxDocManager *) m_docManager, _T("Drawing"), _T("*.drw"), _T(""), _T("drw"), _T("Drawing Doc"), _T("Drawing View"),
          CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

  //// Create a template relating text documents to their views
  (void) new wxDocTemplate(m_docManager, _T("Text"), _T("*.txt"), _T(""), _T("txt"), _T("Text Doc"), _T("Text View"),
          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));

  //// Create the main frame window
  frame = new MyFrame((wxDocManager *) m_docManager, (wxFrame *) NULL,
                      _T("DocView Demo"), wxPoint(0, 0), wxSize(500, 400),
                      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  frame->SetIcon(wxIcon(_T("doc")));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon(_T("doc.xbm")));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = (wxMenu *) NULL;

  file_menu->Append(wxID_NEW, _T("&New...\tCtrl-N"));
  file_menu->Append(wxID_OPEN, _T("&Open...\tCtrl-X"));

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));
  
  // A nice touch: a history of files visited. Use this menu.
  m_docManager->FileHistoryUseMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, _T("&About\tF1"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  if (edit_menu)
    menu_bar->Append(edit_menu, _T("&Edit"));
  menu_bar->Append(help_menu, _T("&Help"));

#ifdef __WXMAC__
  wxMenuBar::MacSetCommonMenuBar(menu_bar);
#endif //def __WXMAC__
  //// Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Centre(wxBOTH);
#ifndef __WXMAC__
  frame->Show(true);
#endif //ndef __WXMAC__

  SetTopWindow(frame);
  return true;
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
  wxDocMDIChildFrame *subframe =
      new wxDocMDIChildFrame(doc, view, GetMainFrame(), wxID_ANY, _T("Child Frame"),
                             wxPoint(10, 10), wxSize(300, 300),
                             wxDEFAULT_FRAME_STYLE |
                             wxNO_FULL_REPAINT_ON_RESIZE);

#ifdef __WXMSW__
  subframe->SetIcon(wxString(isCanvas ? _T("chart") : _T("notepad")));
#endif
#ifdef __X__
  subframe->SetIcon(wxIcon(_T("doc.xbm")));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_NEW, _T("&New..."));
  file_menu->Append(wxID_OPEN, _T("&Open..."));
  file_menu->Append(wxID_CLOSE, _T("&Close"));
  file_menu->Append(wxID_SAVE, _T("&Save"));
  file_menu->Append(wxID_SAVEAS, _T("Save &As..."));

  if (isCanvas)
  {
    file_menu->AppendSeparator();
    file_menu->Append(wxID_PRINT, _T("&Print..."));
    file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
    file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"));
  }

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, _T("E&xit"));

  wxMenu *edit_menu = (wxMenu *) NULL;

  if (isCanvas)
  {
    edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, _T("&Undo"));
    edit_menu->Append(wxID_REDO, _T("&Redo"));
    edit_menu->AppendSeparator();
    edit_menu->Append(DOCVIEW_CUT, _T("&Cut last segment"));

    doc->GetCommandProcessor()->SetEditMenu(edit_menu);
  }

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, _T("&About"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  if (isCanvas)
    menu_bar->Append(edit_menu, _T("&Edit"));
  menu_bar->Append(help_menu, _T("&Help"));

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
  wxDocMDIParentFrame(manager, frame, wxID_ANY, title, pos, size, type, _T("myFrame"))
{
  editMenu = (wxMenu *) NULL;
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(_T("DocView Demo\nAuthor: Julian Smart\nUsage: docview.exe"), _T("About DocView"));
}

// Creates a canvas. Called from view.cpp when a new drawing
// view is created.
MyCanvas *MyFrame::CreateCanvas(wxView *view, wxMDIChildFrame *parent)
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

