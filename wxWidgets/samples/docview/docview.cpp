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
* Purpose:  Document/view architecture demo for wxWidgets class library
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

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"

#include "docview.h"
#include "doc.h"
#include "view.h"
#ifdef __WXMAC__
#include "wx/filename.h"
#endif

MyFrame *frame = (MyFrame *) NULL;

// In single window mode, don't have any child windows; use
// main window.
bool singleWindowMode = false;

IMPLEMENT_APP(MyApp)

MyApp::MyApp(void)
{
    m_docManager = (wxDocManager *) NULL;
}

bool MyApp::OnInit(void)
{
    //// Find out if we're:
    ////  multiple window: multiple windows, each view in a separate frame
    ////  single window:   one view (within the main frame) and one document at a time, as in Windows Write.
    ////                   In single window mode, we only allow one document type
    if (argc > 1)
    {
        if (wxStrcmp(argv[1], _T("-single")) == 0)
        {
            singleWindowMode = true;
        }
    }
    
    //// Create a document manager
    m_docManager = new wxDocManager;

    //// Create a template relating drawing documents to their views
    (void) new wxDocTemplate(m_docManager, _T("Drawing"), _T("*.drw"), _T(""), _T("drw"), _T("Drawing Doc"), _T("Drawing View"),
        CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));
#ifdef __WXMAC__
    wxFileName::MacRegisterDefaultTypeAndCreator( wxT("drw") , 'WXMB' , 'WXMA' ) ;
#endif
    
    if (singleWindowMode)
    {
        // If we've only got one window, we only get to edit
        // one document at a time. Therefore no text editing, just
        // doodling.
        m_docManager->SetMaxDocsOpen(1);
    }
    else
    {
        //// Create a template relating text documents to their views
        (void) new wxDocTemplate(m_docManager, _T("Text"), _T("*.txt;*.text"), _T(""), _T("txt;text"), _T("Text Doc"), _T("Text View"),
        CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));
#ifdef __WXMAC__
        wxFileName::MacRegisterDefaultTypeAndCreator( wxT("txt") , 'TEXT' , 'WXMA' ) ;
#endif
    }
    
    //// Create the main frame window
    frame = new MyFrame(m_docManager, (wxFrame *) NULL, wxID_ANY, _T("DocView Demo"), wxPoint(0, 0), wxSize(500, 400), wxDEFAULT_FRAME_STYLE);
    
    //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
    frame->SetIcon(wxIcon(_T("doc_icn")));
#endif
    
    //// Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *edit_menu = (wxMenu *) NULL;
    
    file_menu->Append(wxID_NEW, _T("&New..."));
    file_menu->Append(wxID_OPEN, _T("&Open..."));
    
    if (singleWindowMode)
    {
        file_menu->Append(wxID_CLOSE, _T("&Close"));
        file_menu->Append(wxID_SAVE, _T("&Save"));
        file_menu->Append(wxID_SAVEAS, _T("Save &As..."));
        file_menu->AppendSeparator();
        file_menu->Append(wxID_PRINT, _T("&Print..."));
        file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
        file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"));
        
        edit_menu = new wxMenu;
        edit_menu->Append(wxID_UNDO, _T("&Undo"));
        edit_menu->Append(wxID_REDO, _T("&Redo"));
        edit_menu->AppendSeparator();
        edit_menu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
        
        frame->editMenu = edit_menu;
    }
    
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _T("E&xit"));
    
    // A nice touch: a history of files visited. Use this menu.
    m_docManager->FileHistoryUseMenu(file_menu);
    
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
    wxMenuBar *menu_bar = new wxMenuBar;
    
    menu_bar->Append(file_menu, _T("&File"));
    if (edit_menu)
        menu_bar->Append(edit_menu, _T("&Edit"));
    menu_bar->Append(help_menu, _T("&Help"));
    
    if (singleWindowMode)
        frame->canvas = frame->CreateCanvas((wxView *) NULL, frame);
    
    //// Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);
    
    frame->Centre(wxBOTH);
    frame->Show(true);
    
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
* Called from view.cpp, when a view is created, but not used at all
* in 'single window' mode.
*/

wxFrame *MyApp::CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas)
{
    //// Make a child frame
    wxDocChildFrame *subframe = new wxDocChildFrame(doc, view, GetMainFrame(), wxID_ANY, _T("Child Frame"),
        wxPoint(10, 10), wxSize(300, 300), wxDEFAULT_FRAME_STYLE);
    
#ifdef __WXMSW__
    subframe->SetIcon(wxString(isCanvas ? _T("chrt_icn") : _T("notepad_icn")));
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

MyFrame::MyFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
                 const wxPoint& pos, const wxSize& size, const long type):
wxDocParentFrame(manager, frame, id, title, pos, size, type)
{
    // This pointer only needed if in single window mode
    canvas = (MyCanvas *) NULL;
    editMenu = (wxMenu *) NULL;
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(_T("DocView Demo\nAuthor: Julian Smart\nUsage: docview.exe [-single]"), _T("About DocView"));
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
    canvas->SetBackgroundColour(*wxWHITE);
    canvas->ClearBackground();
    
    return canvas;
}

MyFrame *GetMainFrame(void)
{
    return frame;
}

