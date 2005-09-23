/////////////////////////////////////////////////////////////////////////////
// Name:        dynamic.cpp
// Purpose:     Dynamic events wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/clntdata.h"

#ifndef __WXMSW__
#include "mondrian.xpm"
#endif

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h);

public:
    void OnQuit(wxCommandEvent& event);
    void OnTest(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

protected:
    wxShadowObject m_shadow;
};

// Define another new frame type
class MySecondFrame: public MyFrame
{
public:
    MySecondFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h);
};

// ID for the menu commands
#define DYNAMIC_QUIT   wxID_EXIT
#define DYNAMIC_TEST   101
#define DYNAMIC_ABOUT  wxID_ABOUT

// Create a new application object
IMPLEMENT_APP  (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, _T("Dynamic wxWidgets App"), 50, 50, 450, 340);

    // Show the frame
    frame->Show(true);

    // Create the main frame window
    MySecondFrame *frame2 = new MySecondFrame(NULL, _T("Dynamic wxWidgets App"), 150, 150, 450, 340);

    // Show the frame
    frame2->Show(true);

    SetTopWindow(frame);

    return true;
}

// -------------------------------------
// MyFrame
// -------------------------------------

// Callback from wxShadowObject

int cb_MyFrame_InitStatusbar( void* window, void* WXUNUSED(param) )
{
    MyFrame *frame = (MyFrame*) window;
    frame->SetStatusText( wxT("Hello from MyFrame"), 0 );
    return 0;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h))
{
    // Give it an icon
#ifdef __WXMSW__
    SetIcon(wxIcon(_T("mondrian")));
#else
    SetIcon(wxIcon(mondrian_xpm));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(DYNAMIC_ABOUT, _T("&About"));
    file_menu->Append(DYNAMIC_TEST, _T("&Test"));
    file_menu->Append(DYNAMIC_QUIT, _T("E&xit"));
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    SetMenuBar(menu_bar);

    // Make a panel with a message
    wxPanel *panel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200), wxTAB_TRAVERSAL);

    (void)new wxStaticText(panel, 311, _T("Hello!"), wxPoint(10, 10), wxDefaultSize, 0);

    // You used to have to do some casting for param 4, but now there are type-safe handlers
    Connect( DYNAMIC_QUIT,  wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnQuit) );
    Connect( DYNAMIC_TEST, wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnTest) );
    Connect( DYNAMIC_ABOUT, wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnAbout) );

    CreateStatusBar();
    m_shadow.AddMethod( wxT("OnTest"), &cb_MyFrame_InitStatusbar );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event) )
{
    m_shadow.InvokeMethod( wxT("OnTest"), this, NULL, NULL );
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this, _T("This demonstrates dynamic event handling"),
        _T("About Dynamic"), wxYES_NO|wxCANCEL);

    dialog.ShowModal();
}


// -------------------------------------
// MySecondFrame
// -------------------------------------

// Callback from wxShadowObject

int cb_MySecondFrame_InitStatusbar( void* window, void* WXUNUSED(param) )
{
    MySecondFrame *frame = (MySecondFrame*) window;
    frame->SetStatusText( wxT("Hello from MySecondFrame"), 0 );
    return 0;
}

// My frame constructor
MySecondFrame::MySecondFrame(wxFrame *frame, wxChar *title, int x, int y, int w, int h):
  MyFrame(frame, title, x, y, w, h )
{
    m_shadow.AddMethod( wxT("OnTest"), &cb_MySecondFrame_InitStatusbar );
}
