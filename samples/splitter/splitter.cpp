/////////////////////////////////////////////////////////////////////////////
// Name:        splitter.cpp
// Purpose:     wxSplitterWindow sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
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
    #include "wx/log.h"

    #include "wx/app.h"
    #include "wx/frame.h"

    #include "wx/scrolwin.h"
    #include "wx/menu.h"

    #include "wx/textdlg.h"       // for wxGetTextFromUser
#endif

#include "wx/splitter.h"
#include "wx/dc.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    SPLIT_QUIT,
    SPLIT_HORIZONTAL,
    SPLIT_VERTICAL,
    SPLIT_UNSPLIT,
    SPLIT_LIVE,
    SPLIT_SETPOSITION,
    SPLIT_SETMINSIZE
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit();
};

class MyFrame: public wxFrame
{
public:
    MyFrame();
    virtual ~MyFrame();

    // Menu commands
    void SplitHorizontal(wxCommandEvent& event);
    void SplitVertical(wxCommandEvent& event);
    void Unsplit(wxCommandEvent& event);
    void ToggleLive(wxCommandEvent& event);
    void SetPosition(wxCommandEvent& event);
    void SetMinSize(wxCommandEvent& event);

    void Quit(wxCommandEvent& event);

    // Menu command update functions
    void UpdateUIHorizontal(wxUpdateUIEvent& event);
    void UpdateUIVertical(wxUpdateUIEvent& event);
    void UpdateUIUnsplit(wxUpdateUIEvent& event);

private:
    wxScrolledWindow *m_left, *m_right;

    wxSplitterWindow* m_splitter;

    DECLARE_EVENT_TABLE()
};

class MySplitterWindow : public wxSplitterWindow
{
public:
    MySplitterWindow(wxFrame *parent);

    // event handlers
    void OnPositionChanged(wxSplitterEvent& event);
    void OnPositionChanging(wxSplitterEvent& event);
    void OnDClick(wxSplitterEvent& event);
    void OnUnsplit(wxSplitterEvent& event);

private:
    wxFrame *m_frame;

    DECLARE_EVENT_TABLE()
};

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow* parent);
    virtual ~MyCanvas();

    virtual void OnDraw(wxDC& dc);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // create and show the main frame
    MyFrame* frame = new MyFrame;

    frame->Show(TRUE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(SPLIT_VERTICAL, MyFrame::SplitVertical)
    EVT_MENU(SPLIT_HORIZONTAL, MyFrame::SplitHorizontal)
    EVT_MENU(SPLIT_UNSPLIT, MyFrame::Unsplit)
    EVT_MENU(SPLIT_LIVE, MyFrame::ToggleLive)
    EVT_MENU(SPLIT_SETPOSITION, MyFrame::SetPosition)
    EVT_MENU(SPLIT_SETMINSIZE, MyFrame::SetMinSize)

    EVT_MENU(SPLIT_QUIT, MyFrame::Quit)

    EVT_UPDATE_UI(SPLIT_VERTICAL, MyFrame::UpdateUIVertical)
    EVT_UPDATE_UI(SPLIT_HORIZONTAL, MyFrame::UpdateUIHorizontal)
    EVT_UPDATE_UI(SPLIT_UNSPLIT, MyFrame::UpdateUIUnsplit)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, -1, _T("wxSplitterWindow sample"),
                 wxDefaultPosition, wxSize(420, 300),
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    CreateStatusBar(2);

    // Make a menubar
    wxMenu *splitMenu = new wxMenu;
    splitMenu->Append(SPLIT_VERTICAL,
                      _T("Split &Vertically\tCtrl-V"),
                      _T("Split vertically"));
    splitMenu->Append(SPLIT_HORIZONTAL,
                      _T("Split &Horizontally\tCtrl-H"),
                      _T("Split horizontally"));
    splitMenu->Append(SPLIT_UNSPLIT,
                      _T("&Unsplit\tCtrl-U"),
                      _T("Unsplit"));
    splitMenu->AppendSeparator();

    splitMenu->AppendCheckItem(SPLIT_LIVE,
                               _T("&Live update\tCtrl-L"),
                               _T("Toggle live update mode"));
    splitMenu->Append(SPLIT_SETPOSITION,
                      _T("Set splitter &position\tCtrl-P"),
                      _T("Set the splitter position"));
    splitMenu->Append(SPLIT_SETMINSIZE,
                      _T("Set &min size\tCtrl-M"),
                      _T("Set minimum pane size"));
    splitMenu->AppendSeparator();

    splitMenu->Append(SPLIT_QUIT, _T("E&xit\tAlt-X"), _T("Exit"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(splitMenu, _T("&Splitter"));

    SetMenuBar(menuBar);

    menuBar->Check(SPLIT_LIVE, TRUE);
    m_splitter = new MySplitterWindow(this);

#if 1
    m_left = new MyCanvas(m_splitter);
    m_left->SetBackgroundColour(*wxRED);
    m_left->SetScrollbars(20, 20, 50, 50);
    m_left->SetCursor(wxCursor(wxCURSOR_MAGNIFIER));

    m_right = new MyCanvas(m_splitter);
    m_right->SetBackgroundColour(*wxCYAN);
    m_right->SetScrollbars(20, 20, 50, 50);
#else // for testing kbd navigation inside the splitter
    m_left = new wxTextCtrl(m_splitter, -1, _T("first text"));
    m_right = new wxTextCtrl(m_splitter, -1, _T("second text"));
#endif

    // you can also do this to start with a single window
#if 0
    m_right->Show(FALSE);
    m_splitter->Initialize(m_left);
#else
    // you can also try -100
    m_splitter->SplitVertically(m_left, m_right, 100);
#endif

    SetStatusText(_T("Min pane size = 0"), 1);
}

MyFrame::~MyFrame()
{
}

// menu command handlers

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MyFrame::SplitHorizontal(wxCommandEvent& WXUNUSED(event) )
{
    if ( m_splitter->IsSplit() )
        m_splitter->Unsplit();
    m_left->Show(TRUE);
    m_right->Show(TRUE);
    m_splitter->SplitHorizontally( m_left, m_right );

    SetStatusText(_T("Splitter split horizontally"), 1);
}

void MyFrame::SplitVertical(wxCommandEvent& WXUNUSED(event) )
{
    if ( m_splitter->IsSplit() )
        m_splitter->Unsplit();
    m_left->Show(TRUE);
    m_right->Show(TRUE);
    m_splitter->SplitVertically( m_left, m_right );

    SetStatusText(_T("Splitter split vertically"), 1);
}

void MyFrame::Unsplit(wxCommandEvent& WXUNUSED(event) )
{
    if ( m_splitter->IsSplit() )
        m_splitter->Unsplit();
    SetStatusText(_T("No splitter"));
}

void MyFrame::ToggleLive(wxCommandEvent& event )
{
    long style = m_splitter->GetWindowStyleFlag();
    if ( event.IsChecked() )
        style |= wxSP_LIVE_UPDATE;
    else
        style &= ~wxSP_LIVE_UPDATE;

    m_splitter->SetWindowStyleFlag(style);
}

void MyFrame::SetPosition(wxCommandEvent& WXUNUSED(event) )
{
    wxString str;
    str.Printf( wxT("%d"), m_splitter->GetSashPosition());
    str = wxGetTextFromUser(_T("Enter splitter position:"), _T(""), str, this);
    if ( str.empty() )
        return;

    long pos;
    if ( !str.ToLong(&pos) )
    {
        wxLogError(_T("The splitter position should be an integer."));
        return;
    }

    m_splitter->SetSashPosition(pos);

    wxLogStatus(this, _T("Splitter position set to %ld"), pos);
}

void MyFrame::SetMinSize(wxCommandEvent& WXUNUSED(event) )
{
    wxString str;
    str.Printf( wxT("%d"), m_splitter->GetMinimumPaneSize());
    str = wxGetTextFromUser(_T("Enter minimal size for panes:"), _T(""), str, this);
    if ( str.empty() )
        return;

    int minsize = wxStrtol( str, (wxChar**)NULL, 10 );
    m_splitter->SetMinimumPaneSize(minsize);
    str.Printf( wxT("Min pane size = %d"), minsize);
    SetStatusText(str, 1);
}

// Update UI handlers

void MyFrame::UpdateUIHorizontal(wxUpdateUIEvent& event)
{
    event.Enable( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_HORIZONTAL) );
}

void MyFrame::UpdateUIVertical(wxUpdateUIEvent& event)
{
    event.Enable( ( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_VERTICAL) ) );
}

void MyFrame::UpdateUIUnsplit(wxUpdateUIEvent& event)
{
    event.Enable( m_splitter->IsSplit() );
}

// ----------------------------------------------------------------------------
// MySplitterWindow
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MySplitterWindow, wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED(-1, MySplitterWindow::OnPositionChanged)
    EVT_SPLITTER_SASH_POS_CHANGING(-1, MySplitterWindow::OnPositionChanging)

    EVT_SPLITTER_DCLICK(-1, MySplitterWindow::OnDClick)

    EVT_SPLITTER_UNSPLIT(-1, MySplitterWindow::OnUnsplit)
END_EVENT_TABLE()

MySplitterWindow::MySplitterWindow(wxFrame *parent)
                : wxSplitterWindow(parent, -1,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN)
{
    m_frame = parent;
}

void MySplitterWindow::OnPositionChanged(wxSplitterEvent& event)
{
    wxLogStatus(m_frame, _T("Position has changed, now = %d (or %d)"),
                event.GetSashPosition(), GetSashPosition());

    event.Skip();
}

void MySplitterWindow::OnPositionChanging(wxSplitterEvent& event)
{
    wxLogStatus(m_frame, _T("Position is changing, now = %d (or %d)"),
                event.GetSashPosition(), GetSashPosition());

    event.Skip();
}

void MySplitterWindow::OnDClick(wxSplitterEvent& event)
{
    m_frame->SetStatusText(_T("Splitter double clicked"), 1);

    event.Skip();
}

void MySplitterWindow::OnUnsplit(wxSplitterEvent& event)
{
    m_frame->SetStatusText(_T("Splitter unsplit"), 1);

    event.Skip();
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

MyCanvas::MyCanvas(wxWindow* parent)
        : wxScrolledWindow(parent, -1)
{
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnDraw(wxDC& dc)
{
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(0, 0, 100, 100);

    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.DrawText(_T("Testing"), 50, 50);

    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxGREEN_BRUSH);
    dc.DrawRectangle(120, 120, 100, 80);
}

