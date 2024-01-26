/////////////////////////////////////////////////////////////////////////////
// Name:        samples/vscroll/vstest.cpp
// Purpose:     VScroll wxWidgets sample
// Author:      Vadim Zeitlin
// Modified by: Brad Anderson
// Created:     04/01/98
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

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// definitions
// ----------------------------------------------------------------------------

#define MAX_LINES 10000

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class VarScrollApp : public wxApp
{
public:
    // create our main window
    virtual bool OnInit() override;
};

// Define a new frame type: this is going to be our main frame
class VarScrollFrame : public wxFrame
{
public:
    // ctor
    VarScrollFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnModeVScroll(wxCommandEvent& event);
    void OnModeHScroll(wxCommandEvent& event);
    void OnModeHVScroll(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event)
    {
        // show current size in the status bar
#if wxUSE_STATUSBAR
        if ( m_frameStatusBar )
        {
            wxSize sz = GetClientSize();
            SetStatusText(wxString::Format("%dx%d", sz.x, sz.y), 1);
        }
#endif // wxUSE_STATUSBAR

        event.Skip();
    }

private:
    // either a wxVScrolledWindow or a wxHVScrolled window, depending on current mode
    wxPanel *m_scrollWindow;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

class VScrollWindow : public wxVScrolledWindow
{
public:
    VScrollWindow(wxFrame *frame) : wxVScrolledWindow(frame, wxID_ANY)
    {
        m_frame = frame;

        SetRowCount(MAX_LINES);

        int i;
        for ( i = 0; i < MAX_LINES; ++i )
            m_heights[i] = rand()%25+16; // low: 16; high: 40

        m_changed = true;
    }

    void OnIdle(wxIdleEvent&)
    {
#if wxUSE_STATUSBAR
        m_frame->SetStatusText(wxString::Format
                               (
                                    "Page size = %d, pos = %d, max = %d",
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

        dc.SetPen(*wxBLACK_PEN);

        const size_t lineFirst = GetVisibleBegin(),
                     lineLast = GetVisibleEnd();

        const wxCoord hText = dc.GetCharHeight();

        wxSize clientSize = GetClientSize();

        wxCoord y = 0;
        for ( size_t line = lineFirst; line < lineLast; line++ )
        {
            dc.DrawLine(0, y, clientSize.GetWidth(), y);

            wxCoord hLine = OnGetRowHeight(line);
            dc.DrawText(wxString::Format("Line %lu", (unsigned long)line),
                        2, y + (hLine - hText) / 2);

            y += hLine;
            dc.DrawLine(0, y, 1000, y);
        }
    }

    void OnScroll(wxScrollWinEvent& event)
    {
        m_changed = true;

        event.Skip();
    }

    void OnMouse(wxMouseEvent& event)
    {
        if(event.LeftDown())
            CaptureMouse();
        else if(event.LeftUp())
            ReleaseMouse();
        event.Skip();
    }

    virtual wxCoord OnGetRowHeight(size_t n) const override
    {
        wxASSERT( n < GetRowCount() );

        return m_heights[n];
    }

private:
    wxFrame *m_frame;

    int m_heights[MAX_LINES];

    bool m_changed;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(VScrollWindow, wxVScrolledWindow)
    EVT_IDLE(VScrollWindow::OnIdle)
    EVT_PAINT(VScrollWindow::OnPaint)
    EVT_SCROLLWIN(VScrollWindow::OnScroll)
    EVT_MOUSE_EVENTS(VScrollWindow::OnMouse)
wxEND_EVENT_TABLE()

class HScrollWindow : public wxHScrolledWindow
{
public:
    HScrollWindow(wxFrame *frame) : wxHScrolledWindow(frame, wxID_ANY)
    {
        m_frame = frame;

        SetColumnCount(MAX_LINES);

        int i;
        for ( i = 0; i < MAX_LINES; ++i )
            m_heights[i] = rand()%25+16; // low: 15; high: 40

        m_changed = true;
    }

    void OnIdle(wxIdleEvent&)
    {
#if wxUSE_STATUSBAR
        m_frame->SetStatusText(wxString::Format
                               (
                                    "Page size = %d, pos = %d, max = %d",
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

        dc.SetPen(*wxBLACK_PEN);

        const size_t lineFirst = GetVisibleBegin(),
                     lineLast = GetVisibleEnd();

        const wxCoord hText = dc.GetCharHeight();

        wxSize clientSize = GetClientSize();

        wxCoord x = 0;
        for ( size_t line = lineFirst; line < lineLast; line++ )
        {
            dc.DrawLine(x, 0, x, clientSize.GetHeight());

            wxCoord wLine = OnGetColumnWidth(line);
            dc.DrawRotatedText(wxString::Format("Line %lu", (unsigned long)line),
                               x + (wLine - hText) / 2, clientSize.GetHeight() - 5, 90);

            x += wLine;
            dc.DrawLine(x, 0, x, 1000);
        }
    }

    void OnScroll(wxScrollWinEvent& event)
    {
        m_changed = true;

        event.Skip();
    }

    void OnMouse(wxMouseEvent& event)
    {
        if(event.LeftDown())
            CaptureMouse();
        else if(event.LeftUp())
            ReleaseMouse();
        event.Skip();
    }

    virtual wxCoord OnGetColumnWidth(size_t n) const override
    {
        wxASSERT( n < GetColumnCount() );

        return m_heights[n];
    }

private:
    wxFrame *m_frame;

    int m_heights[MAX_LINES];

    bool m_changed;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(HScrollWindow, wxHScrolledWindow)
    EVT_IDLE(HScrollWindow::OnIdle)
    EVT_PAINT(HScrollWindow::OnPaint)
    EVT_SCROLLWIN(HScrollWindow::OnScroll)
    EVT_MOUSE_EVENTS(HScrollWindow::OnMouse)
wxEND_EVENT_TABLE()

class HVScrollWindow : public wxHVScrolledWindow
{
public:
    HVScrollWindow(wxFrame *frame) : wxHVScrolledWindow(frame, wxID_ANY)
    {
        m_frame = frame;

        SetRowColumnCount(MAX_LINES, MAX_LINES);

        int i;
        for ( i = 0; i < MAX_LINES; ++i )
        {
            m_heights[i] = rand()%30+31; // low: 30; high: 60
            m_widths[i] = rand()%30+61;  // low: 60; high: 90
        }

        m_changed = true;
    }

    void OnIdle(wxIdleEvent&)
    {
#if wxUSE_STATUSBAR
        m_frame->SetStatusText(wxString::Format
                               (
                                    "Page size = %d rows %d columns; pos = row: %d, column: %d; max = %d rows, %d columns",
                                    GetScrollThumb(wxVERTICAL),
                                    GetScrollThumb(wxHORIZONTAL),
                                    GetScrollPos(wxVERTICAL),
                                    GetScrollPos(wxHORIZONTAL),
                                    GetScrollRange(wxVERTICAL),
                                    GetScrollRange(wxHORIZONTAL)
                               ));
#endif // wxUSE_STATUSBAR
        m_changed = false;
    }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);

        dc.SetPen(*wxBLACK_PEN);

        const size_t rowFirst = GetVisibleRowsBegin(),
                     rowLast = GetVisibleRowsEnd();
        const size_t columnFirst = GetVisibleColumnsBegin(),
                     columnLast = GetVisibleColumnsEnd();

        const wxCoord hText = dc.GetCharHeight();

        wxSize clientSize = GetClientSize();

        wxCoord y = 0;
        for ( size_t row = rowFirst; row < rowLast; row++ )
        {
            wxCoord rowHeight = OnGetRowHeight(row);
            dc.DrawLine(0, y, clientSize.GetWidth(), y);

            wxCoord x = 0;
            for ( size_t col = columnFirst; col < columnLast; col++ )
            {
                wxCoord colWidth = OnGetColumnWidth(col);

                if ( row == rowFirst )
                    dc.DrawLine(x, 0, x, clientSize.GetHeight());

                dc.DrawText(wxString::Format("Row %lu", (unsigned long)row),
                            x + 2, y + rowHeight / 2 - hText);
                dc.DrawText(wxString::Format("Col %lu", (unsigned long)col),
                            x + 2, y + rowHeight / 2);

                x += colWidth;
                if ( row == rowFirst)
                    dc.DrawLine(x, 0, x, clientSize.GetHeight());
            }

            y += rowHeight;
            dc.DrawLine(0, y, clientSize.GetWidth(), y);
        }
    }

    void OnScroll(wxScrollWinEvent& event)
    {
        m_changed = true;

        event.Skip();
    }

    void OnMouse(wxMouseEvent& event)
    {
        if(event.LeftDown())
            CaptureMouse();
        else if(event.LeftUp())
            ReleaseMouse();
        event.Skip();
    }

    virtual wxCoord OnGetRowHeight(size_t n) const override
    {
        wxASSERT( n < GetRowCount() );

        return m_heights[n];
    }

    virtual wxCoord OnGetColumnWidth(size_t n) const override
    {
        wxASSERT( n < GetColumnCount() );

        return m_widths[n];
    }

private:
    wxFrame *m_frame;

    int m_heights[MAX_LINES];
    int m_widths[MAX_LINES];

    bool m_changed;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(HVScrollWindow, wxHVScrolledWindow)
    EVT_IDLE(HVScrollWindow::OnIdle)
    EVT_PAINT(HVScrollWindow::OnPaint)
    EVT_SCROLLWIN(HVScrollWindow::OnScroll)
    EVT_MOUSE_EVENTS(HVScrollWindow::OnMouse)
wxEND_EVENT_TABLE()

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
    VScroll_About = wxID_ABOUT,

    VScroll_VScrollMode = wxID_HIGHEST,
    VScroll_HScrollMode,
    VScroll_HVScrollMode
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(VarScrollFrame, wxFrame)
    EVT_MENU(VScroll_Quit,  VarScrollFrame::OnQuit)
    EVT_MENU(VScroll_VScrollMode, VarScrollFrame::OnModeVScroll)
    EVT_MENU(VScroll_HScrollMode, VarScrollFrame::OnModeHScroll)
    EVT_MENU(VScroll_HVScrollMode, VarScrollFrame::OnModeHVScroll)
    EVT_MENU(VScroll_About, VarScrollFrame::OnAbout)
    EVT_SIZE(VarScrollFrame::OnSize)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. VarScrollApp and
// not wxApp)
wxIMPLEMENT_APP(VarScrollApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool VarScrollApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    VarScrollFrame *frame = new VarScrollFrame;

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
VarScrollFrame::VarScrollFrame()
               : wxFrame(nullptr,
                         wxID_ANY,
                         "VScroll wxWidgets Sample",
                         wxDefaultPosition,
                         wxSize(400, 350)),
                 m_scrollWindow(nullptr)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    wxMenu *menuMode = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(VScroll_About, "&About\tF1", "Show about dialog");

#ifdef wxHAS_RADIO_MENU_ITEMS
    menuMode->AppendRadioItem(VScroll_VScrollMode, "&Vertical\tAlt-V",
                              "Vertical scrolling only");
    menuMode->AppendRadioItem(VScroll_HScrollMode, "&Horizontal\tAlt-H",
                              "Horizontal scrolling only");
    menuMode->AppendRadioItem(VScroll_HVScrollMode,
                              "Hori&zontal/Vertical\tAlt-Z",
                              "Horizontal and vertical scrolling");
    menuMode->Check(VScroll_VScrollMode, true);
#else
    menuMode->Append(VScroll_VScrollMode, "&Vertical\tAlt-V",
                     "Vertical scrolling only");
    menuMode->Append(VScroll_HScrollMode, "&Horizontal\tAlt-H",
                     "Horizontal scrolling only");
    menuMode->Append(VScroll_HVScrollMode, "Hori&zontal/Vertical\tAlt-Z",
                     "Horizontal and vertical scrolling");
#endif

    menuFile->Append(VScroll_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuMode, "&Mode");
    menuBar->Append(menuHelp, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
    int widths[2];
    widths[0] = -1;
    widths[1] = 100;
    SetStatusWidths(2, widths);
#endif // wxUSE_STATUSBAR

    // create our one and only child -- it will take our entire client area
    if ( menuMode->IsChecked(VScroll_VScrollMode) )
        m_scrollWindow = new VScrollWindow(this);
    else if ( menuMode->IsChecked(VScroll_HScrollMode) )
        m_scrollWindow = new HScrollWindow(this);
    else
        m_scrollWindow = new HVScrollWindow(this);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void VarScrollFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void VarScrollFrame::OnModeVScroll(wxCommandEvent& WXUNUSED(event))
{
    if ( m_scrollWindow )
        m_scrollWindow->Destroy();

    m_scrollWindow = new VScrollWindow(this);
    SendSizeEvent();
}

void VarScrollFrame::OnModeHScroll(wxCommandEvent& WXUNUSED(event))
{
    if ( m_scrollWindow )
        m_scrollWindow->Destroy();

    m_scrollWindow = new HScrollWindow(this);
    SendSizeEvent();
}

void VarScrollFrame::OnModeHVScroll(wxCommandEvent& WXUNUSED(event))
{
    if ( m_scrollWindow )
        m_scrollWindow->Destroy();

    m_scrollWindow = new HVScrollWindow(this);
    SendSizeEvent();
}

void VarScrollFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("VScroll shows how to implement scrolling with\n"
                 "variable line widths and heights.\n"
                 "(c) 2003 Vadim Zeitlin",
                 "About VScroll",
                 wxOK | wxICON_INFORMATION,
                 this);
}
