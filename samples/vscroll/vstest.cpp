/////////////////////////////////////////////////////////////////////////////
// Name:        samples/vscroll/vstest.cpp
// Purpose:     VScroll wxWidgets sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04/01/98
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

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/app.h"
    #include "wx/frame.h"
#endif

// we need to include the headers not included from wx/wx.h explicitly anyhow
#include "wx/vscroll.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class VScrollApp : public wxApp
{
public:
    // create our main window
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class VScrollFrame : public wxFrame
{
public:
    // ctor
    VScrollFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event)
    {
        // show current size in the status bar
#if wxUSE_STATUSBAR
        if ( m_frameStatusBar )
        {
            wxSize sz = GetClientSize();
            SetStatusText(wxString::Format(_T("%dx%d"), sz.x, sz.y), 1);
        }
#endif // wxUSE_STATUSBAR

        event.Skip();
    }

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

class VScrollWindow : public wxVScrolledWindow
{
public:
    VScrollWindow(wxFrame *frame) : wxVScrolledWindow(frame, wxID_ANY)
    {
        m_frame = frame;

        SetLineCount(200);

        m_changed = true;
    }

    void OnIdle(wxIdleEvent&)
    {
#if wxUSE_STATUSBAR
        m_frame->SetStatusText(wxString::Format
                               (
                                    _T("Page size = %d, pos = %d, max = %d"),
                                    GetScrollThumb(wxVERTICAL),
                                    GetScrollPos(wxVERTICAL),
                                    GetScrollRange(wxVERTICAL)
                               ));
#endif // wxUSE_STATUSBAR
        m_changed = false;
    }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);

        dc.SetPen(*wxBLACK_DASHED_PEN);

        const size_t lineFirst = GetFirstVisibleLine(),
                     lineLast = GetLastVisibleLine();

        const wxCoord hText = dc.GetCharHeight();

        wxCoord y = 0;
        for ( size_t line = lineFirst; line <= lineLast; line++ )
        {
            dc.DrawLine(0, y, 1000, y);

            wxCoord hLine = OnGetLineHeight(line);
            dc.DrawText(wxString::Format(_T("Line %lu"), (unsigned long)line),
                        0, y + (hLine - hText) / 2);

            y += hLine;
            dc.DrawLine(0, y, 1000, y);
        }
    }

    void OnScroll(wxScrollWinEvent& event)
    {
        m_changed = true;

        event.Skip();
    }


    virtual wxCoord OnGetLineHeight(size_t n) const
    {
        wxASSERT( n < GetLineCount() );

        return n % 2 ? 15 : 30; // 15 + 2*n
    }

private:
    wxFrame *m_frame;

    bool m_changed;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(VScrollWindow, wxVScrolledWindow)
    EVT_IDLE(VScrollWindow::OnIdle)
    EVT_PAINT(VScrollWindow::OnPaint)
    EVT_SCROLLWIN(VScrollWindow::OnScroll)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    VScroll_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    VScroll_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(VScrollFrame, wxFrame)
    EVT_MENU(VScroll_Quit,  VScrollFrame::OnQuit)
    EVT_MENU(VScroll_About, VScrollFrame::OnAbout)
    EVT_SIZE(VScrollFrame::OnSize)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. VScrollApp and
// not wxApp)
IMPLEMENT_APP(VScrollApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool VScrollApp::OnInit()
{
    // create the main application window
    VScrollFrame *frame = new VScrollFrame;

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // ok
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
VScrollFrame::VScrollFrame()
            : wxFrame(NULL,
                      wxID_ANY,
                      _T("VScroll wxWidgets Sample"),
                      wxDefaultPosition,
                      wxSize(400, 350))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(VScroll_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(VScroll_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuHelp, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR

    // create our one and only child -- it will take our entire client area
    new VScrollWindow(this);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void VScrollFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void VScrollFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("VScroll shows how to implement scrolling with\n")
                 _T("variable line heights.\n")
                 _T("(c) 2003 Vadim Zeitlin"),
                 _T("About VScroll"),
                 wxOK | wxICON_INFORMATION,
                 this);
}
