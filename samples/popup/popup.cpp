/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Popup wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// Created:     2005-02-04
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Robert Roebling
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
#endif

#include "wx/popupwin.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
class SimpleTransientPopup: public wxPopupTransientWindow
{
public:
    SimpleTransientPopup( wxWindow *parent );
    virtual ~SimpleTransientPopup();

    // wxPopupTransientWindow virtual methods are all overridden to log them
    virtual void Popup(wxWindow *focus = NULL);
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(wxMouseEvent& event);
    virtual bool Show( bool show = true );
    
    wxScrolledWindow* GetChild() { return m_panel; }
    
private:
    wxScrolledWindow *m_panel;
    
private:
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    DECLARE_CLASS(SimpleTransientPopup)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(SimpleTransientPopup,wxPopupTransientWindow)

BEGIN_EVENT_TABLE(SimpleTransientPopup,wxPopupTransientWindow)
    EVT_MOUSE_EVENTS( SimpleTransientPopup::OnMouse )
    EVT_SIZE( SimpleTransientPopup::OnSize )
    EVT_SET_FOCUS( SimpleTransientPopup::OnSetFocus )
    EVT_KILL_FOCUS( SimpleTransientPopup::OnKillFocus )
END_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup( wxWindow *parent ) :
    wxPopupTransientWindow( parent )
{
    m_panel = new wxScrolledWindow( this, -1 );
    m_panel->SetBackgroundColour( *wxLIGHT_GREY );
    wxStaticText *text = new wxStaticText( m_panel, -1, 
                          wxT("wx.PopupTransientWindow is a\n")
                          wxT("wx.PopupWindow which disappears\n")
                          wxT("automatically when the user\n")
                          wxT("clicks the mouse outside it or if it\n")
                          wxT("(or its first child) loses focus in \n")
                          wxT("any other way."), wxPoint( 10,10) );
    wxSize size = text->GetBestSize();
    m_panel->SetSize( size.x+20, size.y+20 );
    SetClientSize( size.x+20, size.y+20 );
}

SimpleTransientPopup::~SimpleTransientPopup()
{
}

void SimpleTransientPopup::Popup(wxWindow *focus)
{
    wxLogMessage( wxT("SimpleTransientPopup::Popup"));
    wxPopupTransientWindow::Popup(focus);
}

void SimpleTransientPopup::OnDismiss()
{
    wxLogMessage( wxT("SimpleTransientPopup::OnDismiss"));
    wxPopupTransientWindow::OnDismiss();
}

bool SimpleTransientPopup::ProcessLeftDown(wxMouseEvent& event)
{
    wxLogMessage( wxT("SimpleTransientPopup::ProcessLeftDown pos(%d, %d)"), event.GetX(), event.GetY());
    return wxPopupTransientWindow::ProcessLeftDown(event);
}
bool SimpleTransientPopup::Show( bool show )
{
    wxLogMessage( wxT("SimpleTransientPopup::Show %d"), int(show));
    return wxPopupTransientWindow::Show(show);
}

void SimpleTransientPopup::OnSize(wxSizeEvent &event)
{
    wxLogMessage( wxT("SimpleTransientPopup::OnSize"));
    event.Skip();
}

void SimpleTransientPopup::OnSetFocus(wxFocusEvent &event)
{
    wxLogMessage( wxT("SimpleTransientPopup::OnSetFocus"));
    event.Skip();
}

void SimpleTransientPopup::OnKillFocus(wxFocusEvent &event)
{
    wxLogMessage( wxT("SimpleTransientPopup::OnKillFocus"));
    event.Skip();
}

void SimpleTransientPopup::OnMouse(wxMouseEvent &event)
{
    wxLogMessage( wxT("SimpleTransientPopup::OnMouse pos(%d, %d)"), event.GetX(), event.GetY());
    event.Skip();
}

// ----------------------------------------------------------------------------
// ComplexTransientPopup
//   we push the event handler when the mouse isn't in the popup and
//   and pop the event handler when it is so that the child gets the events.
// ----------------------------------------------------------------------------

// Use EVT_IDLE to push and pop the handler, else use a wxTimer
#define USE_TIMER_TO_PUSHPOP 0

class ComplexTransientPopup : public SimpleTransientPopup
{
public:    
    ComplexTransientPopup(wxWindow *parent) : SimpleTransientPopup(parent)
    {
        Init();
    }
    virtual ~ComplexTransientPopup();
    
    virtual void Popup(wxWindow *focus = NULL);
    virtual void Dismiss();
    virtual bool ProcessLeftDown(wxMouseEvent& event);
    
protected:    

    // safely push and pop the event handler of the child
    void PushPopupHandler(wxWindow* child);
    void PopPopupHandler(wxWindow* child);

    void OnMouse( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent &event );
    
#if USE_TIMER_TO_PUSHPOP
    // start/stop timer that pushes and pops handler when the mouse goes over
    //  the scrollbars (if any) of the child window
    void StartTimer();
    void StopTimer();
    void OnTimer( wxTimerEvent& event );
    wxTimer      *m_timer;           // timer for tracking mouse position
#else // !USE_TIMER_TO_PUSHPOP
    void OnIdle( wxIdleEvent& event );
#endif // USE_TIMER_TO_PUSHPOP
    
    wxPoint       m_mouse;           // last/current mouse position
    bool          m_popped_handler;  // state of the event handler

private:
    void Init();
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// ComplexTransientPopup
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ComplexTransientPopup, SimpleTransientPopup)
    EVT_KEY_DOWN(ComplexTransientPopup::OnKeyDown)
    EVT_MOUSE_EVENTS(ComplexTransientPopup::OnMouse)
#if USE_TIMER_TO_PUSHPOP
    EVT_TIMER( wxID_ANY, ComplexTransientPopup::OnTimer )
#endif // USE_TIMER_TO_PUSHPOP
END_EVENT_TABLE()

void ComplexTransientPopup::Init()
{
#if USE_TIMER_TO_PUSHPOP
    m_timer          = NULL;
#endif // USE_TIMER_TO_PUSHPOP
    m_popped_handler = false;
}

ComplexTransientPopup::~ComplexTransientPopup()
{
#if USE_TIMER_TO_PUSHPOP
    StopTimer();
#endif // USE_TIMER_TO_PUSHPOP
}

void ComplexTransientPopup::PushPopupHandler(wxWindow* child)
{
    if (child && m_handlerPopup && m_popped_handler)
    {
        m_popped_handler = false;
        
        if (child->GetEventHandler() != (wxEvtHandler*)m_handlerPopup)
            child->PushEventHandler((wxEvtHandler*)m_handlerPopup);
        if (!child->HasCapture())
            child->CaptureMouse();
        
        child->SetFocus();
    }        
}
void ComplexTransientPopup::PopPopupHandler(wxWindow* child)
{
    if (child && m_handlerPopup && !m_popped_handler)
    {
        m_popped_handler = true;
        
        if (child->GetEventHandler() == (wxEvtHandler*)m_handlerPopup)
            child->PopEventHandler(false);         
        if (child->HasCapture())
            child->ReleaseMouse();
            
        child->SetFocus();            
    }    
}

#if USE_TIMER_TO_PUSHPOP
void ComplexTransientPopup::OnTimer( wxTimerEvent &WXUNUSED(event) )
{   
    if (!IsShown()) return;
   
    m_mouse = ScreenToClient(wxGetMousePosition());

    wxWindow *child = GetChild();
    if (!child) return; // nothing to do
    
    wxRect clientRect(wxPoint(0,0), GetClientSize());
    wxLogMessage(wxT("CTW::OnTimer mouse(%d, %d), popped %d, m_handlerPopup %d"), m_mouse.x, m_mouse.y, m_popped_handler, m_handlerPopup);
    // pop the event handler if inside the child window or 
    // restore the event handler if not in the child window
    if (clientRect.Inside(m_mouse))
        PopPopupHandler(child);
    else 
        PushPopupHandler(child);
}

void ComplexTransientPopup::StartTimer()
{
    if (!m_timer)
        m_timer = new wxTimer(this, wxID_ANY);
    
    m_timer->Start(200, false);    
}

void ComplexTransientPopup::StopTimer()
{
    if (m_timer) 
    {
        if (m_timer->IsRunning()) 
            m_timer->Stop();
        delete m_timer;
        m_timer = NULL;
    }        
}

#else // USE_TIMER_TO_PUSHPOP
void ComplexTransientPopup::OnIdle( wxIdleEvent& event )
{
    if (IsShown())
    {
        m_mouse = ScreenToClient(wxGetMousePosition());
        wxLogMessage(wxT("CTW::OnIdle mouse(%d, %d), popped %d, m_handlerPopup %d"), m_mouse.x, m_mouse.y, m_popped_handler, m_handlerPopup);
        
        wxWindow *child = GetChild();
        if (!child) return; // nothing to do
    
        wxRect clientRect(wxPoint(0,0), GetClientSize());
        //wxPrintf(wxT("**DropDownPopup::OnIdle mouse %d %d -- %d %d %d\n"), m_mouse.x, m_mouse.y, m_popped_handler, m_child, m_handlerPopup); fflush(stdout);
        // pop the event handler if inside the child window or 
        // restore the event handler if not in the child window
        if (clientRect.Inside(m_mouse))
            PopPopupHandler(child);
        else 
            PushPopupHandler(child);
    }
    event.Skip();
}
#endif // USE_TIMER_TO_PUSHPOP

void ComplexTransientPopup::OnMouse( wxMouseEvent& event )
{
    m_mouse = event.GetPosition();
    event.Skip();
}

void ComplexTransientPopup::OnKeyDown( wxKeyEvent &event )
{    
    if (GetChild() && GetChild()->ProcessEvent(event))
        event.Skip(false);
    else
        event.Skip(true);
}

void ComplexTransientPopup::Popup(wxWindow *focus)
{
    SimpleTransientPopup::Popup(focus);
    
#if USE_TIMER_TO_PUSHPOP
    // start the timer to track the mouse position
    // note: idle function not used in this case
    StartTimer();
#else              
    // note: all timer related functions aren't used in this case
    Connect(wxID_ANY, wxEVT_IDLE,
           (wxObjectEventFunction)(wxEventFunction)(wxIdleEventFunction)
            &ComplexTransientPopup::OnIdle, 0, this);
#endif // USE_TIMER_TO_PUSHPOP
}

void ComplexTransientPopup::Dismiss()
{
#if USE_TIMER_TO_PUSHPOP
    StopTimer();    
#else // USE_TIMER_TO_PUSHPOP
    Disconnect(wxID_ANY, wxEVT_IDLE, 
               (wxObjectEventFunction)(wxEventFunction)(wxIdleEventFunction)
               &ComplexTransientPopup::OnIdle, 0, this);
#endif // USE_TIMER_TO_PUSHPOP
    
    // restore the event handler if necessary for the base class Dismiss
    wxWindow *child = GetChild();
    if (child) PushPopupHandler(child);

    m_popped_handler = false;

    SimpleTransientPopup::Dismiss();
}

bool ComplexTransientPopup::ProcessLeftDown( wxMouseEvent &event )
{
    m_mouse = event.GetPosition();
    //wxPrintf(wxT("DropDownPopup::ProcessLeftDown %d %d\n"), m_mouse.x, m_mouse.y); fflush(stdout);
    
    if (m_popped_handler) return true; // shouldn't ever get here, but just in case

#if USE_TIMER_TO_PUSHPOP
    StopTimer();    
#endif // USE_TIMER_TO_PUSHPOP
    
    // don't let the click on the dropdown button actually press it
    // *** Here's where we stick code to ensure that if we click on a combobox
    //     dropdown box we don't try to reshow this dialog because they intend 
    //     hide it.
    
    if (wxRect(wxPoint(0,0), GetSize()).Inside(m_mouse))
        return false;

    Dismiss();
    return true;
}

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyDialog : public wxDialog
{
public:
    MyDialog(const wxString& title);

    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartScrolledPopup(wxCommandEvent& event);
    void OnStartComplexPopup(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

    void CreatePanel(wxWindow* parent);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTestDialog(wxCommandEvent& event);
    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartScrolledPopup(wxCommandEvent& event);
    void OnStartComplexPopup(wxCommandEvent& event);

private:
    wxLog *m_logOld;
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Minimal_TestDialog,
    Minimal_StartSimplePopup,
    Minimal_StartScrolledPopup,
    Minimal_StartComplexPopup,
    Minimal_LogWindow
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("Popup wxWidgets App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_TestDialog, MyFrame::OnTestDialog)
    EVT_BUTTON(Minimal_StartSimplePopup, MyFrame::OnStartSimplePopup)
    EVT_BUTTON(Minimal_StartScrolledPopup, MyFrame::OnStartScrolledPopup)
    EVT_BUTTON(Minimal_StartComplexPopup, MyFrame::OnStartComplexPopup)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(Minimal_TestDialog, _T("&Test dialog\tAlt-T"), _T("Test dialog"));
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    wxButton *button1 = new wxButton( this, Minimal_StartSimplePopup, wxT("Show simple popup"), wxPoint(20,20) );
    wxButton *button2 = new wxButton( this, Minimal_StartScrolledPopup, wxT("Show scrolled popup"), wxPoint(20,70) );
    wxButton *button3 = new wxButton( this, Minimal_StartComplexPopup, wxT("Show complex popup"), wxPoint(20,120) );

    wxTextCtrl* logWin = new wxTextCtrl( this, -1, wxEmptyString, wxDefaultPosition,
                             wxDefaultSize, wxTE_MULTILINE );
    wxLogTextCtrl* logger = new wxLogTextCtrl( logWin );
    m_logOld = logger->SetActiveTarget( logger );
    logger->SetTimestamp( NULL );
    
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( button1, 0 );
    topSizer->Add( button2, 0 );
    topSizer->Add( button3, 0 );
    topSizer->Add( logWin, 1, wxEXPAND );

    SetAutoLayout( true );
    SetSizer( topSizer );

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}

MyFrame::~MyFrame() 
{ 
    delete wxLog::SetActiveTarget(m_logOld); 
} 


// event handlers

void MyFrame::OnStartSimplePopup(wxCommandEvent& event)
{
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("================================================") );
    wxLogMessage( wxT("Simple Popup Shown pos(%d, %d) size(%d, %d)"), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyFrame::OnStartScrolledPopup(wxCommandEvent& event)
{
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    popup->GetChild()->SetScrollbars(1, 1, 1000, 1000);
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("================================================") );
    wxLogMessage( wxT("Scrolled Popup Shown pos(%d, %d) size(%d, %d)"), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyFrame::OnStartComplexPopup(wxCommandEvent& event)
{
    ComplexTransientPopup* popup = new ComplexTransientPopup( this );
    popup->GetChild()->SetScrollbars(1, 1, 1000, 1000);
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("================================================") );
    wxLogMessage( wxT("Complex Popup Shown pos(%d, %d) size(%d, %d)"), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event))
{
    MyDialog dialog( wxT("Test") );
    dialog.ShowModal();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the popup sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About Popup"), wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// test dialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(Minimal_StartSimplePopup, MyDialog::OnStartSimplePopup)
    EVT_BUTTON(Minimal_StartScrolledPopup, MyDialog::OnStartScrolledPopup)
    EVT_BUTTON(Minimal_StartComplexPopup, MyDialog::OnStartComplexPopup)
END_EVENT_TABLE()

MyDialog::MyDialog(const wxString& title)
       : wxDialog(NULL, wxID_ANY, title, wxPoint(50,50), wxSize(400,300))
{

    new wxButton( this, Minimal_StartSimplePopup, wxT("Show simple popup"), wxPoint(20,20) );
    new wxButton( this, Minimal_StartScrolledPopup, wxT("Show scrolled popup"), wxPoint(20,60) );
    new wxButton( this, Minimal_StartComplexPopup, wxT("Show complex popup"), wxPoint(20,100) );

    new wxButton( this, wxID_OK, wxT("OK"), wxPoint(20,200) );
}

void MyDialog::OnStartSimplePopup(wxCommandEvent& event)
{
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("================================================") );
    wxLogMessage( wxT("Dialog Simple Popup Shown pos(%d, %d) size(%d, %d)"), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyDialog::OnStartScrolledPopup(wxCommandEvent& event)
{
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    popup->GetChild()->SetScrollbars(1, 1, 1000, 1000);
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("================================================") );
    wxLogMessage( wxT("Dialog Scrolled Popup Shown pos(%d, %d) size(%d, %d)"), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyDialog::OnStartComplexPopup(wxCommandEvent& event)
{
    ComplexTransientPopup* popup = new ComplexTransientPopup( this );
    popup->GetChild()->SetScrollbars(1, 1, 1000, 1000);
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("================================================") );
    wxLogMessage( wxT("Dialog Complex Popup Shown pos(%d, %d) size(%d, %d)"), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

