/////////////////////////////////////////////////////////////////////////////
// Name:        calendar.cpp
// Purpose:     wxCalendarCtrl sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     02.01.00
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
    #pragma implementation "calendar.cpp"
    #pragma interface "calendar.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/stattext.h"
    #include "wx/menu.h"
    #include "wx/layout.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/calctrl.h"

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

class MyPanel : public wxPanel
{
public:
    MyPanel(wxFrame *frame);

    void OnCalendar(wxCalendarEvent& event);
    void OnCalendarWeekDayClick(wxCalendarEvent& event);
    void OnCalendarChange(wxCalendarEvent& event);

    wxCalendarCtrl *GetCal() const { return m_calendar; }

    void StartWithMonday(bool on);
    void HighlightSpecial(bool on);

private:
    wxCalendarCtrl *m_calendar;
    wxStaticText   *m_date;

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnCalMonday(wxCommandEvent& event);
    void OnCalHolidays(wxCommandEvent& event);
    void OnCalSpecial(wxCommandEvent& event);

    void OnCalAllowMonth(wxCommandEvent& event);
    void OnCalAllowYear(wxCommandEvent& event);

    void OnAllowYearUpdate(wxUpdateUIEvent& event);

private:
    MyPanel *m_panel;

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
    Calendar_File_About = 100,
    Calendar_File_Quit,
    Calendar_Cal_Monday = 200,
    Calendar_Cal_Holidays,
    Calendar_Cal_Special,
    Calendar_Cal_Month,
    Calendar_Cal_Year,
    Calendar_CalCtrl = 1000,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Calendar_File_Quit,  MyFrame::OnQuit)
    EVT_MENU(Calendar_File_About, MyFrame::OnAbout)

    EVT_MENU(Calendar_Cal_Monday, MyFrame::OnCalMonday)
    EVT_MENU(Calendar_Cal_Holidays, MyFrame::OnCalHolidays)
    EVT_MENU(Calendar_Cal_Special, MyFrame::OnCalSpecial)

    EVT_MENU(Calendar_Cal_Month, MyFrame::OnCalAllowMonth)
    EVT_MENU(Calendar_Cal_Year, MyFrame::OnCalAllowYear)

    EVT_UPDATE_UI(Calendar_Cal_Year, MyFrame::OnAllowYearUpdate)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_CALENDAR            (Calendar_CalCtrl,   MyPanel::OnCalendar)
    EVT_CALENDAR_SEL_CHANGED(Calendar_CalCtrl,   MyPanel::OnCalendarChange)
    EVT_CALENDAR_WEEKDAY_CLICKED(Calendar_CalCtrl, MyPanel::OnCalendarWeekDayClick)
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
    // Create the main application window
    MyFrame *frame = new MyFrame("Calendar wxWindows sample",
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    // @@@ what does it do exactly, in fact? is it necessary here?
    frame->Show(TRUE);
    SetTopWindow(frame);

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
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Calendar_File_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuCal = new wxMenu;
    menuCal->Append(Calendar_Cal_Monday,
                    "Monday &first weekday\tCtrl-F",
                    "Toggle between Mon and Sun as the first week day",
                    TRUE);
    menuCal->Append(Calendar_Cal_Holidays, "Show &holidays\tCtrl-H",
                    "Toggle highlighting the holidays",
                    TRUE);
    menuCal->Append(Calendar_Cal_Special, "Highlight &special dates\tCtrl-S",
                    "Test custom highlighting",
                    TRUE);
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_Month, "&Month can be changed\tCtrl-M",
                    "Allow changing the month in the calendar",
                    TRUE);
    menuCal->Append(Calendar_Cal_Year, "&Year can be changed\tCtrl-Y",
                    "Allow changing the year in the calendar",
                    TRUE);

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuCal, "&Calendar");

    menuBar->Check(Calendar_Cal_Monday, TRUE);
    menuBar->Check(Calendar_Cal_Holidays, TRUE);
    menuBar->Check(Calendar_Cal_Month, TRUE);
    menuBar->Check(Calendar_Cal_Year, TRUE);

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_panel = new MyPanel(this);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("wxCalendarCtrl sample\n© 2000 Vadim Zeitlin"),
                 _T("About Calendar"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnCalMonday(wxCommandEvent& event)
{
    m_panel->StartWithMonday(GetMenuBar()->IsChecked(event.GetInt()));
}

void MyFrame::OnCalHolidays(wxCommandEvent& event)
{
    bool enable = GetMenuBar()->IsChecked(event.GetInt());
    m_panel->GetCal()->EnableHolidayDisplay(enable);
}

void MyFrame::OnCalSpecial(wxCommandEvent& event)
{
    m_panel->HighlightSpecial(GetMenuBar()->IsChecked(event.GetInt()));
}

void MyFrame::OnCalAllowMonth(wxCommandEvent& event)
{
    bool allow = GetMenuBar()->IsChecked(event.GetInt());

    m_panel->GetCal()->EnableMonthChange(allow);
}

void MyFrame::OnCalAllowYear(wxCommandEvent& event)
{
    bool allow = GetMenuBar()->IsChecked(event.GetInt());

    m_panel->GetCal()->EnableYearChange(allow);
}

void MyFrame::OnAllowYearUpdate(wxUpdateUIEvent& event)
{
    event.Enable( GetMenuBar()->IsChecked(Calendar_Cal_Month));
}

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(wxFrame *frame)
       : wxPanel(frame, -1)
{
    SetAutoLayout(TRUE);

    wxString date;
    date.Printf("Selected date: %s",
                wxDateTime::Today().FormatISODate().c_str());
    m_date = new wxStaticText(this, -1, date);
    m_calendar = new wxCalendarCtrl(this, Calendar_CalCtrl,
                                    wxDefaultDateTime,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxCAL_MONDAY_FIRST |
                                    wxCAL_SHOW_HOLIDAYS |
                                    wxRAISED_BORDER);

    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs(this, wxLeft, 10);
    c->centreY.SameAs(m_calendar, wxCentreY);
    c->height.AsIs();
    c->width.AsIs();

    m_date->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->left.SameAs(m_date, wxRight, 10);
    c->top.SameAs(this, wxTop, 10);
    c->height.AsIs();
    c->width.AsIs();

    m_calendar->SetConstraints(c);
}

void MyPanel::OnCalendar(wxCalendarEvent& event)
{
    wxLogMessage("Selected %s from calendar",
                 event.GetDate().FormatISODate().c_str());
}

void MyPanel::OnCalendarChange(wxCalendarEvent& event)
{
    wxString s;
    s.Printf("Selected date: %s", event.GetDate().FormatISODate().c_str());

    m_date->SetLabel(s);
}

void MyPanel::OnCalendarWeekDayClick(wxCalendarEvent& event)
{
    wxLogMessage("Clicked on %s",
                 wxDateTime::GetWeekDayName(event.GetWeekDay()).c_str());
}

void MyPanel::StartWithMonday(bool on)
{
    long style = m_calendar->GetWindowStyle();
    if ( on )
        style |= wxCAL_MONDAY_FIRST;
    else
        style &= ~wxCAL_MONDAY_FIRST;

    m_calendar->SetWindowStyle(style);

    m_calendar->Refresh();
}

void MyPanel::HighlightSpecial(bool on)
{
    if ( on )
    {
        wxCalendarDateAttr
            *attrRedCircle = new wxCalendarDateAttr(wxCAL_BORDER_ROUND, *wxRED),
            *attrGreenSquare = new wxCalendarDateAttr(wxCAL_BORDER_SQUARE, *wxGREEN),
            *attrHeaderLike = new wxCalendarDateAttr(*wxBLUE, *wxLIGHT_GREY);

        m_calendar->SetAttr(17, attrRedCircle);
        m_calendar->SetAttr(29, attrGreenSquare);
        m_calendar->SetAttr(13, attrHeaderLike);
    }
    else // off
    {
        m_calendar->ResetAttr(17);
        m_calendar->ResetAttr(29);
        m_calendar->ResetAttr(13);
    }

    m_calendar->Refresh();
}

