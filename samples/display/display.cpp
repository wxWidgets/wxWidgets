/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     wxWindows sample showing the features of wxDisplay class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.02.03
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".from here
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"

    #include "wx/stattext.h"

    #include "wx/layout.h"
#endif

#include "wx/notebook.h"

#include "wx/display.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Display_Quit = 1,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Display_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Display_Quit,  MyFrame::OnQuit)
    EVT_MENU(Display_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_("Display wxWindows Sample"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Display_About, _("&About...\tF1"), _("Show about dialog"));

    menuFile->Append(Display_Quit, _("E&xit\tAlt-X"), _("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(helpMenu, _("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    // create child controls
    wxNotebook *notebook = new wxNotebook(this, -1);
    const size_t count = wxDisplay::GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxDisplay display(n);

        wxWindow *page = new wxPanel(notebook, -1);

        // create 2 column flex grid sizer with growable 2nd column
        wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 10, 20);
        sizer->AddGrowableCol(1);

        const wxRect r(display.GetGeometry());
        sizer->Add(new wxStaticText(page, -1, _T("Geometry: ")));
        sizer->Add(new wxStaticText
                       (
                        page,
                        -1,
                        wxString::Format(_T("(%d, %d)-(%d, %d)"),
                                         r.x, r.y,
                                         r.x + r.width, r.y + r.height)
                       ));

        sizer->Add(new wxStaticText(page, -1, _T("Depth: ")));
        sizer->Add(new wxStaticText
                       (
                        page,
                        -1,
                        wxString::Format(_T("%d bpp"), display.GetDepth())
                       ));

        sizer->Add(new wxStaticText(page, -1, _T("Colour: ")));
        sizer->Add(new wxStaticText(page, -1, display.IsColour() ? _T("Yes")
                                                                 : _T("No")));

        // add it to another sizer to have borders around it
        wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
        sizerTop->Add(sizer, 1, wxALL | wxEXPAND, 10);
        page->SetSizer(sizerTop);

        notebook->AddPage(page,
                          wxString::Format(_T("Display %lu"), (unsigned long)n));
    }
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Demo program for wxDisplay class.\n\n(c) 2003 Vadim Zeitlin"),
                 _T("About Display Sample"),
                 wxOK | wxICON_INFORMATION,
                 this);
}
