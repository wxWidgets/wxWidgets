/////////////////////////////////////////////////////////////////////////////
// Name:        keyboard.cpp
// Purpose:     Keyboard wxWindows sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.04.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin
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
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

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

    ~MyFrame() { delete m_logTarget; }

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnSkip(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);

private:
    wxLog *m_logTarget;

    class TextWindow *m_winText;
    wxListBox *m_lboxLog;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// A log target which just redirects the messages to a listbox
class LboxLogger : public wxLog
{
public:
    LboxLogger(wxListBox *lbox, wxLog *logOld)
    {
        m_lbox = lbox;
        //m_lbox->Disable(); -- looks ugly under MSW
        m_logOld = logOld;
    }

    virtual ~LboxLogger()
    {
        wxLog::SetActiveTarget(m_logOld);
    }

private:
    // implement sink functions
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
    {
        // don't put trace messages into listbox or we can get into infinite
        // recursion
        if ( level == wxLOG_Trace )
        {
            if ( m_logOld )
            {
                // cast is needed to call protected method
                ((LboxLogger *)m_logOld)->DoLog(level, szString, t);
            }
        }
        else
        {
            wxLog::DoLog(level, szString, t);
        }
    }

    virtual void DoLogString(const wxChar *szString, time_t t)
    {
        wxString msg;
        TimeStamp(&msg);
        msg += szString;

        #ifdef __WXUNIVERSAL__
            m_lbox->AppendAndEnsureVisible(msg);
        #else // other ports don't have this method yet
            m_lbox->Append(msg);
            m_lbox->SetFirstItem(m_lbox->GetCount() - 1);
        #endif
    }

    // the control we use
    wxListBox *m_lbox;

    // the old log target
    wxLog *m_logOld;
};

class TextWindow : public wxWindow
{
public:
    TextWindow(wxWindow *parent)
        : wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                   wxRAISED_BORDER)
    {
        m_skip = TRUE;

        SetBackgroundColour(*wxBLUE);
    }

    void SetSkip(bool skip) { m_skip = skip; }

protected:
    void OnKeyDown(wxKeyEvent& event) { LogEvent(_T("Key down"), event); }
    void OnKeyUp(wxKeyEvent& event) { LogEvent(_T("Key up"), event); }
    void OnChar(wxKeyEvent& event) { LogEvent(_T("Char"), event); }

    void OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);
        dc.SetTextForeground(*wxWHITE);
        dc.DrawLabel(_T("Press keys here"), GetClientRect(), wxALIGN_CENTER);
    }

private:
    static inline wxChar GetChar(bool on, wxChar c) { return on ? c : _T('-'); }

    void LogEvent(const wxChar *name, wxKeyEvent& event);

    bool m_skip;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TextWindow, wxWindow)
    EVT_KEY_DOWN(TextWindow::OnKeyDown)
    EVT_KEY_UP(TextWindow::OnKeyUp)
    EVT_CHAR(TextWindow::OnChar)

    EVT_PAINT(TextWindow::OnPaint)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Keyboard_Quit = 1,

    Keyboard_Clear,
    Keyboard_Skip,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Keyboard_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Keyboard_Quit,  MyFrame::OnQuit)
    EVT_MENU(Keyboard_About, MyFrame::OnAbout)

    EVT_MENU(Keyboard_Clear, MyFrame::OnClear)
    EVT_MENU(Keyboard_Skip, MyFrame::OnSkip)

    EVT_SIZE(MyFrame::OnSize)
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
    MyFrame *frame = new MyFrame(_T("Keyboard wxWindows App"),
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
       : wxFrame(NULL, -1, title, pos, size, style),
         m_winText(NULL)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Keyboard_Clear, _T("&Clear log\tCtrl-L"));
    menuFile->AppendSeparator();
    menuFile->Append(Keyboard_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *menuKeys = new wxMenu;
    menuKeys->AppendCheckItem(Keyboard_Skip, _T("&Skip key down\tCtrl-S"));

    // the "About" item should be in the help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(Keyboard_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuKeys, _T("&Keys"));
    menuBar->Append(menuHelp, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    menuBar->Check(Keyboard_Skip, TRUE);
#endif // wxUSE_MENUS

    m_winText = new TextWindow(this);
    m_lboxLog = new wxListBox(this, -1);

    m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
    wxLog::SetActiveTarget(m_logTarget);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of keyboard sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About Keyboard"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClear(wxCommandEvent& event)
{
    m_lboxLog->Clear();
}

void MyFrame::OnSkip(wxCommandEvent& event)
{
    m_winText->SetSkip(event.IsChecked());
}

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if ( m_winText )
    {
        wxSize size = GetClientSize();
        m_winText->SetSize(0, 0, size.x, 50);
        m_lboxLog->SetSize(0, 51, size.x, size.y - 50);
    }
}

// ----------------------------------------------------------------------------
// TextWindow
// ----------------------------------------------------------------------------

void TextWindow::LogEvent(const wxChar *name, wxKeyEvent& event)
{
    wxString key;
    long keycode = event.KeyCode();
    {
        switch ( keycode )
        {
            case WXK_BACK: key = "BACK"; break;
            case WXK_TAB: key = "TAB"; break;
            case WXK_RETURN: key = "RETURN"; break;
            case WXK_ESCAPE: key = "ESCAPE"; break;
            case WXK_SPACE: key = "SPACE"; break;
            case WXK_DELETE: key = "DELETE"; break;
            case WXK_START: key = "START"; break;
            case WXK_LBUTTON: key = "LBUTTON"; break;
            case WXK_RBUTTON: key = "RBUTTON"; break;
            case WXK_CANCEL: key = "CANCEL"; break;
            case WXK_MBUTTON: key = "MBUTTON"; break;
            case WXK_CLEAR: key = "CLEAR"; break;
            case WXK_SHIFT: key = "SHIFT"; break;
            case WXK_ALT: key = "ALT"; break;
            case WXK_CONTROL: key = "CONTROL"; break;
            case WXK_MENU: key = "MENU"; break;
            case WXK_PAUSE: key = "PAUSE"; break;
            case WXK_CAPITAL: key = "CAPITAL"; break;
            case WXK_PRIOR: key = "PRIOR"; break;
            case WXK_NEXT: key = "NEXT"; break;
            case WXK_END: key = "END"; break;
            case WXK_HOME: key = "HOME"; break;
            case WXK_LEFT: key = "LEFT"; break;
            case WXK_UP: key = "UP"; break;
            case WXK_RIGHT: key = "RIGHT"; break;
            case WXK_DOWN: key = "DOWN"; break;
            case WXK_SELECT: key = "SELECT"; break;
            case WXK_PRINT: key = "PRINT"; break;
            case WXK_EXECUTE: key = "EXECUTE"; break;
            case WXK_SNAPSHOT: key = "SNAPSHOT"; break;
            case WXK_INSERT: key = "INSERT"; break;
            case WXK_HELP: key = "HELP"; break;
            case WXK_NUMPAD0: key = "NUMPAD0"; break;
            case WXK_NUMPAD1: key = "NUMPAD1"; break;
            case WXK_NUMPAD2: key = "NUMPAD2"; break;
            case WXK_NUMPAD3: key = "NUMPAD3"; break;
            case WXK_NUMPAD4: key = "NUMPAD4"; break;
            case WXK_NUMPAD5: key = "NUMPAD5"; break;
            case WXK_NUMPAD6: key = "NUMPAD6"; break;
            case WXK_NUMPAD7: key = "NUMPAD7"; break;
            case WXK_NUMPAD8: key = "NUMPAD8"; break;
            case WXK_NUMPAD9: key = "NUMPAD9"; break;
            case WXK_MULTIPLY: key = "MULTIPLY"; break;
            case WXK_ADD: key = "ADD"; break;
            case WXK_SEPARATOR: key = "SEPARATOR"; break;
            case WXK_SUBTRACT: key = "SUBTRACT"; break;
            case WXK_DECIMAL: key = "DECIMAL"; break;
            case WXK_DIVIDE: key = "DIVIDE"; break;
            case WXK_F1: key = "F1"; break;
            case WXK_F2: key = "F2"; break;
            case WXK_F3: key = "F3"; break;
            case WXK_F4: key = "F4"; break;
            case WXK_F5: key = "F5"; break;
            case WXK_F6: key = "F6"; break;
            case WXK_F7: key = "F7"; break;
            case WXK_F8: key = "F8"; break;
            case WXK_F9: key = "F9"; break;
            case WXK_F10: key = "F10"; break;
            case WXK_F11: key = "F11"; break;
            case WXK_F12: key = "F12"; break;
            case WXK_F13: key = "F13"; break;
            case WXK_F14: key = "F14"; break;
            case WXK_F15: key = "F15"; break;
            case WXK_F16: key = "F16"; break;
            case WXK_F17: key = "F17"; break;
            case WXK_F18: key = "F18"; break;
            case WXK_F19: key = "F19"; break;
            case WXK_F20: key = "F20"; break;
            case WXK_F21: key = "F21"; break;
            case WXK_F22: key = "F22"; break;
            case WXK_F23: key = "F23"; break;
            case WXK_F24: key = "F24"; break;
            case WXK_NUMLOCK: key = "NUMLOCK"; break;
            case WXK_SCROLL: key = "SCROLL"; break;
            case WXK_PAGEUP: key = "PAGEUP"; break;
            case WXK_PAGEDOWN: key = "PAGEDOWN"; break;
            case WXK_NUMPAD_SPACE: key = "NUMPAD_SPACE"; break;
            case WXK_NUMPAD_TAB: key = "NUMPAD_TAB"; break;
            case WXK_NUMPAD_ENTER: key = "NUMPAD_ENTER"; break;
            case WXK_NUMPAD_F1: key = "NUMPAD_F1"; break;
            case WXK_NUMPAD_F2: key = "NUMPAD_F2"; break;
            case WXK_NUMPAD_F3: key = "NUMPAD_F3"; break;
            case WXK_NUMPAD_F4: key = "NUMPAD_F4"; break;
            case WXK_NUMPAD_HOME: key = "NUMPAD_HOME"; break;
            case WXK_NUMPAD_LEFT: key = "NUMPAD_LEFT"; break;
            case WXK_NUMPAD_UP: key = "NUMPAD_UP"; break;
            case WXK_NUMPAD_RIGHT: key = "NUMPAD_RIGHT"; break;
            case WXK_NUMPAD_DOWN: key = "NUMPAD_DOWN"; break;
            case WXK_NUMPAD_PRIOR: key = "NUMPAD_PRIOR"; break;
            case WXK_NUMPAD_PAGEUP: key = "NUMPAD_PAGEUP"; break;
            case WXK_NUMPAD_PAGEDOWN: key = "NUMPAD_PAGEDOWN"; break;
            case WXK_NUMPAD_END: key = "NUMPAD_END"; break;
            case WXK_NUMPAD_BEGIN: key = "NUMPAD_BEGIN"; break;
            case WXK_NUMPAD_INSERT: key = "NUMPAD_INSERT"; break;
            case WXK_NUMPAD_DELETE: key = "NUMPAD_DELETE"; break;
            case WXK_NUMPAD_EQUAL: key = "NUMPAD_EQUAL"; break;
            case WXK_NUMPAD_MULTIPLY: key = "NUMPAD_MULTIPLY"; break;
            case WXK_NUMPAD_ADD: key = "NUMPAD_ADD"; break;
            case WXK_NUMPAD_SEPARATOR: key = "NUMPAD_SEPARATOR"; break;
            case WXK_NUMPAD_SUBTRACT: key = "NUMPAD_SUBTRACT"; break;
            case WXK_NUMPAD_DECIMAL: key = "NUMPAD_DECIMAL"; break;

            default:
            {
               if ( wxIsprint((int)keycode) )
                   key.Printf(_T("'%c'"), (char)keycode);
               else if ( keycode > 0 && keycode < 27 )
                   key.Printf(_("Ctrl-%c"), _T('A') + keycode - 1);
               else
                   key.Printf(_T("unknown (%ld)"), keycode);
            }
        }
    }

    wxLogMessage( _T("%s event: %s (flags = %c%c%c%c)"),
                  name,
                  key.c_str(),
                  GetChar( event.ControlDown(), _T('C') ),
                  GetChar( event.AltDown(), _T('A') ),
                  GetChar( event.ShiftDown(), _T('S') ),
                  GetChar( event.MetaDown(), _T('M') ) );

    if ( m_skip )
    {
        event.Skip();
    }
}
