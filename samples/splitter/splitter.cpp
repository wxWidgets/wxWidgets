/////////////////////////////////////////////////////////////////////////////
// Name:        splitter.cpp
// Purpose:     wxSplitterWindow sample
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
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


#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/app.h"
    #include "wx/frame.h"

    #include "wx/scrolwin.h"
    #include "wx/menu.h"

    #include "wx/textdlg.h"       // for wxGetTextFromUser
#endif

#include "wx/splitter.h"
#include "wx/dcmirror.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    SPLIT_QUIT = 1,
    SPLIT_HORIZONTAL,
    SPLIT_VERTICAL,
    SPLIT_UNSPLIT,
    SPLIT_LIVE,
    SPLIT_BORDER,
    SPLIT_3DSASH,
    SPLIT_NOTHEME,
    SPLIT_CUSTOM_COLOUR,
    SPLIT_SETPOSITION,
    SPLIT_SETMINSIZE,
    SPLIT_SETGRAVITY,
    SPLIT_LOCKSASH,
    SPLIT_DCLICK,
    SPLIT_REPLACE,
    SPLIT_INVISIBLE
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    MyApp() { }

    virtual bool OnInit() override;

    wxDECLARE_NO_COPY_CLASS(MyApp);
};

class MyFrame: public wxFrame
{
public:
    MyFrame();
    virtual ~MyFrame();

    void ToggleFlag(int flag, bool enable);

    // Menu commands
    void OnSplitHorizontal(wxCommandEvent& event);
    void OnSplitVertical(wxCommandEvent& event);
    void OnUnsplit(wxCommandEvent& event);
    void OnToggleLive(wxCommandEvent& event)
                  { ToggleFlag(wxSP_LIVE_UPDATE, event.IsChecked()); }
    void OnToggleBorder(wxCommandEvent& event)
                  { ToggleFlag(wxSP_BORDER, event.IsChecked()); }
    void OnToggle3DSash(wxCommandEvent& event)
                  { ToggleFlag(wxSP_3DSASH, event.IsChecked()); }
    void OnToggleNoTheme(wxCommandEvent& event)
                  { ToggleFlag(wxSP_NO_XP_THEME, event.IsChecked()); }
    void OnToggleCustomColour(wxCommandEvent& event)
    {
        m_splitter->SetBackgroundColour((event.IsChecked() ? *wxBLUE : wxNullColour));
        m_splitter->Refresh();
    }

    void OnSetPosition(wxCommandEvent& event);
    void OnSetMinSize(wxCommandEvent& event);
    void OnSetGravity(wxCommandEvent& event);
    void OnLockSash(wxCommandEvent& event);
    void OnReplace(wxCommandEvent &event);
    void OnToggleInvisible(wxCommandEvent &event);
    void OnToggleDClick(wxCommandEvent &event);

    void OnQuit(wxCommandEvent& event);

    // Menu command update functions
    void OnUpdateUIHorizontal(wxUpdateUIEvent& event);
    void OnUpdateUIVertical(wxUpdateUIEvent& event);
    void OnUpdateUIUnsplit(wxUpdateUIEvent& event);
    void OnUpdateUIInvisible(wxUpdateUIEvent& event);

    bool AllowDClick() const { return m_allowDClick; }

    bool IsSashLocked() const { return m_lockSash; }
    void SetSashPos(int pos) { m_sashPos = pos; }
    int GetSashPos() const { return m_sashPos; }

private:
    wxWindow *m_left, *m_right;

    wxSplitterWindow* m_splitter;
    wxWindow *m_replacewindow;
    int m_sashPos;
    bool m_lockSash;
    bool m_allowDClick;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MyFrame);
};

class MySplitterWindow : public wxSplitterWindow
{
public:
    MySplitterWindow(MyFrame *parent);

    // event handlers
    void OnPositionChanged(wxSplitterEvent& event);
    void OnPositionChanging(wxSplitterEvent& event);
    void OnPositionResize(wxSplitterEvent& event);
    void OnDClick(wxSplitterEvent& event);
    void OnUnsplitEvent(wxSplitterEvent& event);

private:
    MyFrame *m_frame;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MySplitterWindow);
};

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow* parent, bool mirror);
    virtual ~MyCanvas(){}

    virtual void OnDraw(wxDC& dc) override;

private:
    bool m_mirror;

    wxDECLARE_NO_COPY_CLASS(MyCanvas);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create and show the main frame
    MyFrame* frame = new MyFrame;

    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(SPLIT_VERTICAL, MyFrame::OnSplitVertical)
    EVT_MENU(SPLIT_HORIZONTAL, MyFrame::OnSplitHorizontal)
    EVT_MENU(SPLIT_UNSPLIT, MyFrame::OnUnsplit)
    EVT_MENU(SPLIT_LIVE, MyFrame::OnToggleLive)
    EVT_MENU(SPLIT_BORDER, MyFrame::OnToggleBorder)
    EVT_MENU(SPLIT_3DSASH, MyFrame::OnToggle3DSash)
    EVT_MENU(SPLIT_NOTHEME, MyFrame::OnToggleNoTheme)
    EVT_MENU(SPLIT_CUSTOM_COLOUR, MyFrame::OnToggleCustomColour)
    EVT_MENU(SPLIT_SETPOSITION, MyFrame::OnSetPosition)
    EVT_MENU(SPLIT_SETMINSIZE, MyFrame::OnSetMinSize)
    EVT_MENU(SPLIT_SETGRAVITY, MyFrame::OnSetGravity)
    EVT_MENU(SPLIT_LOCKSASH, MyFrame::OnLockSash)
    EVT_MENU(SPLIT_DCLICK, MyFrame::OnToggleDClick)
    EVT_MENU(SPLIT_REPLACE, MyFrame::OnReplace)
    EVT_MENU(SPLIT_INVISIBLE, MyFrame::OnToggleInvisible)

    EVT_MENU(SPLIT_QUIT, MyFrame::OnQuit)

    EVT_UPDATE_UI(SPLIT_VERTICAL, MyFrame::OnUpdateUIVertical)
    EVT_UPDATE_UI(SPLIT_HORIZONTAL, MyFrame::OnUpdateUIHorizontal)
    EVT_UPDATE_UI(SPLIT_UNSPLIT, MyFrame::OnUpdateUIUnsplit)
    EVT_UPDATE_UI(SPLIT_INVISIBLE, MyFrame::OnUpdateUIInvisible)
wxEND_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "wxSplitterWindow sample",
                 wxDefaultPosition, wxSize(420, 300))
{
    m_lockSash = false;
    m_sashPos = 0;
    m_allowDClick = true;

    SetIcon(wxICON(sample));

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // Make a menubar
    wxMenu *splitMenu = new wxMenu;
    splitMenu->Append(SPLIT_VERTICAL,
                      "Split &Vertically\tCtrl-V",
                      "Split vertically");
    splitMenu->Append(SPLIT_HORIZONTAL,
                      "Split &Horizontally\tCtrl-H",
                      "Split horizontally");
    splitMenu->Append(SPLIT_UNSPLIT,
                      "&Unsplit\tCtrl-U",
                      "Unsplit");
    splitMenu->AppendCheckItem(SPLIT_INVISIBLE,
                      "Toggle sash &invisibility\tCtrl-I",
                      "Toggle sash invisibility");
    splitMenu->AppendSeparator();

    splitMenu->AppendCheckItem(SPLIT_LIVE,
                               "&Live update\tCtrl-L",
                               "Toggle live update mode");
    splitMenu->AppendCheckItem(SPLIT_BORDER,
                               "3D &Border",
                               "Toggle wxSP_BORDER flag");
    splitMenu->Check(SPLIT_BORDER, true);
    splitMenu->AppendCheckItem(SPLIT_3DSASH,
                               "&3D Sash",
                               "Toggle wxSP_3DSASH flag");
    splitMenu->Check(SPLIT_3DSASH, true);
#ifdef __WXMSW__
    splitMenu->AppendCheckItem(SPLIT_NOTHEME,
                               "Disable XP &theme",
                               "Toggle wxSP_NO_XP_THEME flag");
#endif
    splitMenu->AppendCheckItem(SPLIT_CUSTOM_COLOUR,
                               "Use custom &colour",
                               "Toggle custom colour");
    splitMenu->Append(SPLIT_SETPOSITION,
                      "Set splitter &position\tCtrl-P",
                      "Set the splitter position");
    splitMenu->Append(SPLIT_SETMINSIZE,
                      "Set &min size\tCtrl-M",
                      "Set minimum pane size");
    splitMenu->Append(SPLIT_SETGRAVITY,
                      "Set &gravity\tCtrl-G",
                      "Set gravity of sash");
    splitMenu->AppendCheckItem(SPLIT_LOCKSASH,
                      "Toggle sash &lock on resize\tCtrl-R",
                      "Keep the sash in a fixed position while resizing");
    splitMenu->AppendCheckItem(SPLIT_DCLICK,
                      "Toggle double click\tCtrl-D",
                      "Toggle allow/prevent double click on sash");
    splitMenu->Check(SPLIT_DCLICK, true);
    splitMenu->AppendSeparator();

    splitMenu->Append(SPLIT_REPLACE,
                      "&Replace right window",
                      "Replace right window");
    splitMenu->AppendSeparator();

    splitMenu->Append(SPLIT_QUIT, "E&xit\tAlt-X", "Exit");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(splitMenu, "&Splitter");

    SetMenuBar(menuBar);

    menuBar->Check(SPLIT_LIVE, true);
    m_splitter = new MySplitterWindow(this);

    // If you use non-zero gravity you must initialize the splitter with its
    // correct initial size, otherwise it will change the sash position by a
    // huge amount when it's resized from its initial default size to its real
    // size when the frame lays it out. This wouldn't be necessary if default
    // zero gravity were used (although it would do no harm either).
    m_splitter->SetSize(GetClientSize());
    m_splitter->SetSashGravity(1.0);

#if 1
    m_left = new MyCanvas(m_splitter, true);
    m_left->SetBackgroundColour(*wxRED);
    m_left->SetCursor(wxCursor(wxCURSOR_MAGNIFIER));
    m_left->SetToolTip("This is the left window");

    m_right = new MyCanvas(m_splitter, false);
    m_right->SetBackgroundColour(*wxCYAN);
    m_right->SetToolTip("And this is the window on the right");
#else // for testing kbd navigation inside the splitter
    m_left = new wxTextCtrl(m_splitter, wxID_ANY, "first text");
    m_right = new wxTextCtrl(m_splitter, wxID_ANY, "second text");
#endif

    // you can also do this to start with a single window
#if 0
    m_right->Show(false);
    m_splitter->Initialize(m_left);
#else
    // you can also try -100
    m_splitter->SplitVertically(m_left, m_right, 100);
#endif

#if wxUSE_STATUSBAR
    SetStatusText("Min pane size = 0", 1);
#endif // wxUSE_STATUSBAR

    m_replacewindow = nullptr;
}

MyFrame::~MyFrame()
{
    if ( m_replacewindow )
    {
        m_replacewindow->Destroy();
    }
}

// menu command handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnSplitHorizontal(wxCommandEvent& WXUNUSED(event) )
{
    if ( m_splitter->IsSplit() )
        m_splitter->Unsplit();
    m_left->Show(true);
    m_right->Show(true);
    m_splitter->SplitHorizontally( m_left, m_right );
    m_replacewindow = nullptr;

#if wxUSE_STATUSBAR
    SetStatusText("Splitter split horizontally", 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnSplitVertical(wxCommandEvent& WXUNUSED(event) )
{
    if ( m_splitter->IsSplit() )
        m_splitter->Unsplit();
    m_left->Show(true);
    m_right->Show(true);
    m_splitter->SplitVertically( m_left, m_right );
    m_replacewindow = nullptr;

#if wxUSE_STATUSBAR
    SetStatusText("Splitter split vertically", 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnUnsplit(wxCommandEvent& WXUNUSED(event) )
{
    if ( m_splitter->IsSplit() )
        m_splitter->Unsplit();
#if wxUSE_STATUSBAR
    SetStatusText("No splitter");
#endif // wxUSE_STATUSBAR
}

void MyFrame::ToggleFlag(int flag, bool enable)
{
    long style = m_splitter->GetWindowStyleFlag();
    if ( enable )
        style |= flag;
    else
        style &= ~flag;

    m_splitter->SetWindowStyleFlag(style);

    // we need to move sash to redraw it
    int pos = m_splitter->GetSashPosition();
    m_splitter->SetSashPosition(pos + 1);
    m_splitter->SetSashPosition(pos);
}

void MyFrame::OnSetPosition(wxCommandEvent& WXUNUSED(event) )
{
    wxString str;
    str.Printf( "%d", m_splitter->GetSashPosition());
#if wxUSE_TEXTDLG
    str = wxGetTextFromUser("Enter splitter position:", "", str, this);
#endif
    if ( str.empty() )
        return;

    long pos;
    if ( !str.ToLong(&pos) )
    {
        wxLogError("The splitter position should be an integer.");
        return;
    }

    m_splitter->SetSashPosition(pos);

    wxLogStatus(this, "Splitter position set to %ld", pos);
}

void MyFrame::OnSetMinSize(wxCommandEvent& WXUNUSED(event) )
{
    wxString str;
    str.Printf( "%d", m_splitter->GetMinimumPaneSize());
#if wxUSE_TEXTDLG
    str = wxGetTextFromUser("Enter minimal size for panes:", "", str, this);
#endif
    if ( str.empty() )
        return;

    int minsize = wxStrtol( str, (wxChar**)nullptr, 10 );
    m_splitter->SetMinimumPaneSize(minsize);
#if wxUSE_STATUSBAR
    str.Printf( "Min pane size = %d", minsize);
    SetStatusText(str, 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnLockSash(wxCommandEvent &WXUNUSED(event))
{
    m_lockSash = !m_lockSash;
    m_sashPos = m_splitter->GetSashPosition();
}

void MyFrame::OnToggleDClick(wxCommandEvent &WXUNUSED(event))
{
    m_allowDClick = !m_allowDClick;
}

void MyFrame::OnSetGravity(wxCommandEvent& WXUNUSED(event) )
{
    wxString str;
    str.Printf( "%g", m_splitter->GetSashGravity());
#if wxUSE_TEXTDLG
    str = wxGetTextFromUser("Enter sash gravity (0,1):", "", str, this);
#endif
    if ( str.empty() )
        return;

    double gravity = wxStrtod( str, (wxChar**)nullptr);
    m_splitter->SetSashGravity(gravity);
#if wxUSE_STATUSBAR
    str.Printf( "Gravity = %g", gravity);
    SetStatusText(str, 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnReplace(wxCommandEvent& WXUNUSED(event) )
{
    if ( !m_replacewindow )
    {
        m_replacewindow = m_splitter->GetWindow2();
        if ( m_replacewindow )
        {
            m_splitter->ReplaceWindow(m_replacewindow, new wxPanel(m_splitter, wxID_ANY));
            m_replacewindow->Hide();
        }
    }
    else
    {
        wxWindow *empty = m_splitter->GetWindow2();
        wxASSERT(empty != m_replacewindow);
        m_splitter->ReplaceWindow(empty, m_replacewindow);
        m_replacewindow->Show();
        m_replacewindow = nullptr;
        empty->Destroy();
    }
}

void MyFrame::OnToggleInvisible(wxCommandEvent& WXUNUSED(event) )
{
    m_splitter->SetSashInvisible(!m_splitter->IsSashInvisible());
    m_splitter->SizeWindows();
}

// Update UI handlers

void MyFrame::OnUpdateUIHorizontal(wxUpdateUIEvent& event)
{
    event.Enable( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_HORIZONTAL) );
}

void MyFrame::OnUpdateUIVertical(wxUpdateUIEvent& event)
{
    event.Enable( ( (!m_splitter->IsSplit()) || (m_splitter->GetSplitMode() != wxSPLIT_VERTICAL) ) );
}

void MyFrame::OnUpdateUIUnsplit(wxUpdateUIEvent& event)
{
    event.Enable( m_splitter->IsSplit() );
}

void MyFrame::OnUpdateUIInvisible(wxUpdateUIEvent& event)
{
    event.Check( m_splitter->IsSashInvisible() );
}

// ----------------------------------------------------------------------------
// MySplitterWindow
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MySplitterWindow, wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, MySplitterWindow::OnPositionChanged)
    EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, MySplitterWindow::OnPositionChanging)
    EVT_SPLITTER_SASH_POS_RESIZE(wxID_ANY, MySplitterWindow::OnPositionResize)

    EVT_SPLITTER_DCLICK(wxID_ANY, MySplitterWindow::OnDClick)

    EVT_SPLITTER_UNSPLIT(wxID_ANY, MySplitterWindow::OnUnsplitEvent)
wxEND_EVENT_TABLE()

MySplitterWindow::MySplitterWindow(MyFrame *parent)
                : wxSplitterWindow(parent, wxID_ANY,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxSP_3D | wxSP_LIVE_UPDATE |
                                   wxCLIP_CHILDREN /* | wxSP_NO_XP_THEME */ )
{
    m_frame = parent;
}

void MySplitterWindow::OnPositionChanged(wxSplitterEvent& event)
{
    wxLogStatus(m_frame, "Position has changed, now = %d (or %d)",
                event.GetSashPosition(), GetSashPosition());

    // This event is only sent when the user manually dragged the sash.
    // In this case we accept the user input so the sash is locked at the
    // new position. If the sash is not locked, this has no effect but
    // doesn't hurt either.
    m_frame->SetSashPos(event.GetSashPosition());
}

void MySplitterWindow::OnPositionResize(wxSplitterEvent &event)
{
    // When the splitter is resizing we only allow the sash to be moved
    // if it is not locked. Otherwise we hold it at the position
    // the user specified by manually dragging.
    if (m_frame->IsSashLocked())
    {
        // We set the last known position to keep the sash in place.
        // For this particular example we could also simply use
        // event.Veto()
        // as well and it would have the same effect.
        event.SetSashPosition(m_frame->GetSashPos());
    }
}

void MySplitterWindow::OnPositionChanging(wxSplitterEvent& event)
{
    wxLogStatus(m_frame, "Position is changing, now = %d (or %d)",
                event.GetSashPosition(), GetSashPosition());
}

void MySplitterWindow::OnDClick(wxSplitterEvent& event)
{
#if wxUSE_STATUSBAR
    m_frame->SetStatusText("Splitter double clicked", 1);
#endif // wxUSE_STATUSBAR

    // Let the splitter window handle the event if it is not blocked.
    if (!m_frame->AllowDClick())
        event.Veto();
}

void MySplitterWindow::OnUnsplitEvent(wxSplitterEvent& event)
{
#if wxUSE_STATUSBAR
    m_frame->SetStatusText("Splitter unsplit", 1);
#endif // wxUSE_STATUSBAR

    // Let the splitter window handle the event as well.
    event.Skip();
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

MyCanvas::MyCanvas(wxWindow* parent, bool mirror)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL)
{
    m_mirror = mirror;
    SetScrollbars(20, 20, 5, 5);
}

void MyCanvas::OnDraw(wxDC& dcOrig)
{
    wxMirrorDC dc(dcOrig, m_mirror);

    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(0, 0, 100, 200);

    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.DrawText("Testing", 50, 50);

    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxGREEN_BRUSH);
    dc.DrawRectangle(120, 120, 100, 80);
}

