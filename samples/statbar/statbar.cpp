/////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     wxStatusBar sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.02.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statbar.cpp"
    #pragma interface "statbar.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_STATUSBAR
    #error "You need to set wxUSE_STATUSBAR to 1 to compile this sample"
#endif // wxUSE_STATUSBAR

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/statusbr.h"
    #include "wx/datetime.h"
    #include "wx/timer.h"
    #include "wx/checkbox.h"
    #include "wx/statbmp.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "green.xpm"
#include "red.xpm"

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

// A custom status bar which contains controls, icons &c
class MyStatusBar : public wxStatusBar
{
public:
    MyStatusBar(wxWindow *parent);
    virtual ~MyStatusBar();

    void UpdateClock();

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnToggleClock(wxCommandEvent& event);

private:
    enum
    {
        Field_Text,
        Field_Checkbox,
        Field_Bitmap,
        Field_Clock,
        Field_Max
    };

    class MyTimer : public wxTimer
    {
    public:
        MyTimer(MyStatusBar *statbar) {m_statbar = statbar; }

        virtual void Notify() { m_statbar->UpdateClock(); }

    private:
        MyStatusBar *m_statbar;
    } m_timer;

    wxCheckBox *m_checkbox;
    wxStaticBitmap *m_statbmp;

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnRecreateStatusBar(wxCommandEvent& event);

private:
    enum StatBarKind
    {
        StatBar_Default,
        StatBar_Custom,
        StatBar_Max
    } m_statbarKind;

    void DoCreateStatusBar(StatBarKind kind);

    wxStatusBar *m_statbarDefault;
    MyStatusBar *m_statbarCustom;

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
    StatusBar_Quit = 1,
    StatusBar_Recreate,
    StatusBar_About,
    StatusBar_Checkbox = 1000
};

static const int BITMAP_SIZE_X = 32;
static const int BITMAP_SIZE_Y = 15;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(StatusBar_Quit,  MyFrame::OnQuit)
    EVT_MENU(StatusBar_Recreate, MyFrame::OnRecreateStatusBar)
    EVT_MENU(StatusBar_About, MyFrame::OnAbout)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyStatusBar, wxStatusBar)
    EVT_SIZE(MyStatusBar::OnSize)
    EVT_CHECKBOX(StatusBar_Checkbox, MyStatusBar::OnToggleClock)
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

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame("wxStatusBar sample",
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
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    m_statbarDefault = NULL;
    m_statbarCustom = NULL;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = StatusBar_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(StatusBar_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *statbarMenu = new wxMenu;
    statbarMenu->Append(StatusBar_Recreate, "&Recreate\tCtrl-R",
                        "Toggle status bar format");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(StatusBar_About, "&About...\tCtrl-A", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(statbarMenu, "&Status bar");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create default status bar to start with
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");

    m_statbarDefault = GetStatusBar();
}

MyFrame::~MyFrame()
{
    SetStatusBar(NULL);

    delete m_statbarDefault;
    delete m_statbarCustom;
}

void MyFrame::DoCreateStatusBar(MyFrame::StatBarKind kind)
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        statbarOld->Hide();
    }

    switch ( kind )
    {
        case StatBar_Default:
            SetStatusBar(m_statbarDefault);
            break;

        case StatBar_Custom:
            if ( !m_statbarCustom )
            {
                m_statbarCustom = new MyStatusBar(this);
            }
            SetStatusBar(m_statbarCustom);
            break;

        default:
            wxFAIL_MSG("unknown stat bar kind");
    }

    PositionStatusBar();
    GetStatusBar()->Show();

    m_statbarKind = kind;
}

// event handlers
void MyFrame::OnRecreateStatusBar(wxCommandEvent& WXUNUSED(event))
{
    DoCreateStatusBar(m_statbarKind == StatBar_Custom ? StatBar_Default
                                                      : StatBar_Custom);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("wxStatusBar sample\n(c) 2000 Vadim Zeitlin",
                 "About statbar", wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// MyStatusBar
// ----------------------------------------------------------------------------

MyStatusBar::MyStatusBar(wxWindow *parent)
           : wxStatusBar(parent, -1), m_timer(this)
{
    static const int widths[Field_Max] = { -1, 150, BITMAP_SIZE_X, 100 };

    SetFieldsCount(Field_Max);
    SetStatusWidths(Field_Max, widths);

    m_checkbox = new wxCheckBox(this, StatusBar_Checkbox, _T("&Toggle clock"));
    m_checkbox->SetValue(TRUE);

    m_statbmp = new wxStaticBitmap(this, -1, wxICON(green));

    m_timer.Start(1000);

    UpdateClock();
}

MyStatusBar::~MyStatusBar()
{
    if ( m_timer.IsRunning() )
    {
        m_timer.Stop();
    }
}

void MyStatusBar::OnSize(wxSizeEvent& event)
{
    wxRect rect;
    GetFieldRect(Field_Checkbox, rect);

    m_checkbox->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);

    GetFieldRect(Field_Bitmap, rect);
    m_statbmp->Move(rect.x + (rect.width - BITMAP_SIZE_X) / 2,
                    rect.y + (rect.height - BITMAP_SIZE_Y) / 2);

    event.Skip();
}

void MyStatusBar::OnToggleClock(wxCommandEvent& event)
{
    if ( m_checkbox->GetValue() )
    {
        m_timer.Start(1000);

        m_statbmp->SetIcon(wxICON(green));

        UpdateClock();
    }
    else // don't show clock
    {
        m_timer.Stop();

        m_statbmp->SetIcon(wxICON(red));

        SetStatusText("", Field_Clock);
    }
}

void MyStatusBar::UpdateClock()
{
    SetStatusText(wxDateTime::Now().FormatTime(), Field_Clock);
}
