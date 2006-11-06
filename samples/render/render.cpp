/////////////////////////////////////////////////////////////////////////////
// Name:        render.cpp
// Purpose:     Render wxWidgets sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.08.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/panel.h"
    #include "wx/menu.h"
    #include "wx/textdlg.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
    #include "wx/icon.h"
#endif

#include "wx/apptrait.h"
#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// A renderer class draws the header buttons in a "special" way
class MyRenderer : public wxDelegateRendererNative
{
public:
    MyRenderer() : wxDelegateRendererNative(wxRendererNative::GetDefault()) { }

    virtual void DrawHeaderButton(wxWindow *WXUNUSED(win),
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int WXUNUSED(flags) = 0,
                                  wxHeaderSortIconType WXUNUSED(sortArrow) = wxHDR_SORT_ICON_NONE,
                                  wxHeaderButtonParams* WXUNUSED(params) = NULL)
    {
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.SetTextForeground(*wxWHITE);
        dc.DrawRoundedRectangle(rect, 5);
        dc.DrawLabel(_T("MyRenderer"), wxNullBitmap, rect, wxALIGN_CENTER);
    }
};

// To use a different renderer from the very beginning we must override
// wxAppTraits method creating the renderer (another, simpler, alternative is
// to call wxRendererNative::Set() a.s.a.p. which should work in 99% of the
// cases, but we show this here just for completeness)
class MyTraits : public wxGUIAppTraits
{
    virtual wxRendererNative *CreateRenderer()
    {
        // it will be deleted on program shutdown by wxWidgets itself
        return new MyRenderer;
    }
};

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit();

    // if we want MyTraits to be used we must override CreateTraits()
    virtual wxAppTraits *CreateTraits() { return new MyTraits; }
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame();
    virtual ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
#if wxUSE_DYNLIB_CLASS
    void OnLoad(wxCommandEvent& event);
    void OnUnload(wxCommandEvent& event);
#endif // wxUSE_DYNLIB_CLASS
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    wxPanel *m_panel;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// a very simple class just to have something to draw on
class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent) : wxPanel(parent) { }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);

        dc.DrawText(_T("Below is the standard header button drawn"), 10, 10);
        dc.DrawText(_T("using the current renderer:"), 10, 40);

        wxRendererNative::Get().DrawHeaderButton(this, dc,
                                                 wxRect(20, 70, 100, 60));
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_PAINT(MyPanel::OnPaint)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // our menu items
#if wxUSE_DYNLIB_CLASS
    Render_Load = 100,
    Render_Unload,
#endif // wxUSE_DYNLIB_CLASS

    // standard menu items
    Render_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Render_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
#if wxUSE_DYNLIB_CLASS
    EVT_MENU(Render_Load,  MyFrame::OnLoad)
    EVT_MENU(Render_Unload,MyFrame::OnUnload)
#endif // wxUSE_DYNLIB_CLASS
    EVT_MENU(Render_Quit,  MyFrame::OnQuit)

    EVT_MENU(Render_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
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
    new MyFrame;

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL,
                 wxID_ANY,
                 _T("Render wxWidgets Sample"),
                 wxPoint(50, 50),
                 wxSize(450, 340))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
#if wxUSE_DYNLIB_CLASS
    menuFile->Append(Render_Load, _T("&Load renderer...\tCtrl-L"));
    menuFile->Append(Render_Unload, _T("&Unload renderer\tCtrl-U"));
#endif // wxUSE_DYNLIB_CLASS
    menuFile->Append(Render_Quit, _T("E&xit\tCtrl-Q"), _T("Quit this program"));

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Render_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    m_panel = new MyPanel(this);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR

    Show();
}

MyFrame::~MyFrame()
{
    delete wxRendererNative::Set(NULL);
}


// event handlers

#if wxUSE_DYNLIB_CLASS

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_name = _T("renddll");

    wxString name = wxGetTextFromUser
                    (
                        _T("Name of the renderer to load:"),
                        _T("Render wxWidgets Sample"),
                        s_name,
                        this
                    );
    if ( name.empty() )
    {
        // cancelled
        return;
    }

    s_name = name;

    wxRendererNative *renderer = wxRendererNative::Load(name);
    if ( !renderer )
    {
        wxLogError(_T("Failed to load renderer \"%s\"."), name.c_str());
    }
    else // loaded ok
    {
        delete wxRendererNative::Set(renderer);

        m_panel->Refresh();

        wxLogStatus(this, _T("Successfully loaded the renderer \"%s\"."),
                    name.c_str());
    }
}

void MyFrame::OnUnload(wxCommandEvent& WXUNUSED(event))
{
    wxRendererNative *renderer = wxRendererNative::Set(NULL);
    if ( renderer )
    {
        delete renderer;

        m_panel->Refresh();

        wxLogStatus(this, _T("Unloaded the previously loaded renderer."));
    }
    else
    {
        wxLogWarning(_T("No renderer to unload."));
    }
}

#endif // wxUSE_DYNLIB_CLASS

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Render sample shows how to use custom renderers.\n")
                 _T("\n")
                 _T("(c) 2003 Vadim Zeitlin"),
                 _T("About Render wxWidgets Sample"),
                 wxOK | wxICON_INFORMATION, this);
}

