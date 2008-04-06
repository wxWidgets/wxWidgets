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
#include "wx/stockitem.h"

static MyFrame* frame = NULL;

IMPLEMENT_APP(MyApp)

MyApp::MyApp(void)
{
    m_docManager = NULL;
}

bool MyApp::OnInit(void)
{
    if ( !wxApp::OnInit() )
        return false;
    //// Create a document manager
    SetAppName(wxT("DocView Demo"));

    //// Create a document manager
    m_docManager = new wxDocManager;

    //// Create a template relating drawing documents to their views
    new wxDocTemplate(m_docManager, wxT("Drawing"), wxT("*.drw"), wxT(""), wxT("drw"), wxT("Drawing Doc"), wxT("Drawing View"),
          CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

    //// Create a template relating text documents to their views
    new wxDocTemplate(m_docManager, wxT("Text"), wxT("*.txt"), wxT(""), wxT("txt"), wxT("Text Doc"), wxT("Text View"),
          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));

    //// Create the main frame window
    frame = new MyFrame(m_docManager, NULL,
                      GetAppDisplayName(), wxPoint(0, 0), wxSize(500, 400),
                      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

    //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
    frame->SetIcon(wxIcon(wxT("doc")));
#endif

    //// Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *edit_menu = NULL;

    file_menu->Append(wxID_NEW);
    file_menu->Append(wxID_OPEN);

    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT);

    // A nice touch: a history of files visited. Use this menu.
    m_docManager->FileHistoryUseMenu(file_menu);

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(DOCVIEW_ABOUT);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxGetStockLabel(wxID_FILE));
    if (edit_menu)
        menu_bar->Append(edit_menu, wxGetStockLabel(wxID_EDIT));
    menu_bar->Append(help_menu, wxGetStockLabel(wxID_HELP));

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
    wxDELETE(m_docManager)
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
      new wxDocMDIChildFrame(doc, view, GetMainFrame(), wxID_ANY, wxT("Child Frame"),
                             wxPoint(10, 10), wxSize(300, 300),
                             wxDEFAULT_FRAME_STYLE |
                             wxNO_FULL_REPAINT_ON_RESIZE);

#ifdef __WXMSW__
    subframe->SetIcon(wxString(isCanvas ? wxT("chart") : wxT("notepad")));
#endif
#ifdef __X__
    subframe->SetIcon(wxIcon(wxT("doc.xbm")));
#endif

    //// Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_NEW);
    file_menu->Append(wxID_OPEN);
    file_menu->Append(wxID_CLOSE);
    file_menu->Append(wxID_SAVE);
    file_menu->Append(wxID_SAVEAS);

    if (isCanvas)
    {
        file_menu->AppendSeparator();
        file_menu->Append(wxID_PRINT);
        file_menu->Append(wxID_PRINT_SETUP, wxT("Print &Setup..."));
        file_menu->Append(wxID_PREVIEW);
    }

    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT);

    wxMenu *edit_menu = new wxMenu;
    if (isCanvas)
    {
        edit_menu->Append(wxID_UNDO);
        edit_menu->Append(wxID_REDO);
        edit_menu->AppendSeparator();
        edit_menu->Append(DOCVIEW_CUT, wxT("&Cut last segment"));

        doc->GetCommandProcessor()->SetEditMenu(edit_menu);
    }
    else
    {
        edit_menu->Append(wxID_COPY);
        edit_menu->Append(wxID_PASTE);
        edit_menu->Append(wxID_SELECTALL);
    }
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(DOCVIEW_ABOUT);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxGetStockLabel(wxID_FILE));
    menu_bar->Append(edit_menu, wxGetStockLabel(wxID_EDIT));
    menu_bar->Append(help_menu, wxGetStockLabel(wxID_HELP));

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
  wxDocMDIParentFrame(manager, frame, wxID_ANY, title, pos, size, type, wxT("myFrame"))
{
    m_editMenu = NULL;
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(wxT("DocView Demo\nAuthor: Julian Smart\nUsage: docview.exe"), wxT("About DocView"));
/*
    Better, but brings in adv lib
    wxAboutDialogInfo info;
    info.SetName(wxTheApp->GetAppDisplayName());
    info.AddDeveloper(wxT("Julian Smart"));
    wxAboutBox(info);
*/
}

// Creates a canvas. Called from view.cpp when a new drawing
// view is created.
MyCanvas* MyFrame::CreateCanvas(DrawingView* view, wxMDIChildFrame* parent)
{
    wxSize size = parent->GetClientSize();

    // Non-retained canvas
    MyCanvas *canvas = new MyCanvas(view, parent, wxPoint(0, 0), size, 0);
    canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));

    // Give it scrollbars
    canvas->SetScrollbars(20, 20, 50, 50);

    return canvas;
}

MyFrame* GetMainFrame()
{
    return frame;
}

