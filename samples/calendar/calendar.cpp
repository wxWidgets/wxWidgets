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

#if defined(__GNUG__) && !defined(__APPLE__)
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
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/stattext.h"
    #include "wx/menu.h"
    #include "wx/layout.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/sizer.h"
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
    void OnCalMonthChange(wxCalendarEvent& event);
    void OnCalYearChange(wxCalendarEvent& event);

    wxCalendarCtrl *GetCal() const { return m_calendar; }

    // turn on/off the specified style bit on the calendar control
    void ToggleCalStyle(bool on, int style);

    void HighlightSpecial(bool on);

    void SetDate();
    void Today();
    
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

    void OnCalSeqMonth(wxCommandEvent& event);
    void OnCalShowSurroundingWeeks(wxCommandEvent& event);

    void OnSetDate(wxCommandEvent& event);
    void OnToday(wxCommandEvent& event);

    void OnAllowYearUpdate(wxUpdateUIEvent& event);

private:
    MyPanel *m_panel;

    // any class wishing to process wxWidgets events must use this macro
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
    Calendar_Cal_SeqMonth,
    Calendar_Cal_SurroundWeeks,
    Calendar_Cal_SetDate,
    Calendar_Cal_Today,
    Calendar_CalCtrl = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
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

    EVT_MENU(Calendar_Cal_SeqMonth, MyFrame::OnCalSeqMonth)
    EVT_MENU(Calendar_Cal_SurroundWeeks, MyFrame::OnCalShowSurroundingWeeks)

    EVT_MENU(Calendar_Cal_SetDate, MyFrame::OnSetDate)
    EVT_MENU(Calendar_Cal_Today, MyFrame::OnToday)


    EVT_UPDATE_UI(Calendar_Cal_Year, MyFrame::OnAllowYearUpdate)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_CALENDAR            (Calendar_CalCtrl,   MyPanel::OnCalendar)
    EVT_CALENDAR_MONTH      (Calendar_CalCtrl,   MyPanel::OnCalMonthChange)
    EVT_CALENDAR_YEAR       (Calendar_CalCtrl,   MyPanel::OnCalYearChange)
    EVT_CALENDAR_SEL_CHANGED(Calendar_CalCtrl,   MyPanel::OnCalendarChange)
    EVT_CALENDAR_WEEKDAY_CLICKED(Calendar_CalCtrl, MyPanel::OnCalendarWeekDayClick)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
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
    MyFrame *frame = new MyFrame(_T("Calendar wxWidgets sample"),
                                 wxPoint(50, 50), wxSize(450, 340));

    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Calendar_File_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *menuCal = new wxMenu;
    menuCal->Append(Calendar_Cal_Monday,
                    _T("Monday &first weekday\tCtrl-F"),
                    _T("Toggle between Mon and Sun as the first week day"),
                    true);
    menuCal->Append(Calendar_Cal_Holidays, _T("Show &holidays\tCtrl-H"),
                    _T("Toggle highlighting the holidays"),
                    true);
    menuCal->Append(Calendar_Cal_Special, _T("Highlight &special dates\tCtrl-S"),
                    _T("Test custom highlighting"),
                    true);
    menuCal->Append(Calendar_Cal_SurroundWeeks,
                    _T("Show s&urrounding weeks\tCtrl-W"),
                    _T("Show the neighbouring weeks in the prev/next month"),
                    true);
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_SeqMonth,
                    _T("To&ggle month selector style\tCtrl-G"),
                    _T("Use another style for the calendar controls"),
                    true);
    menuCal->Append(Calendar_Cal_Month, _T("&Month can be changed\tCtrl-M"),
                    _T("Allow changing the month in the calendar"),
                    true);
    menuCal->Append(Calendar_Cal_Year, _T("&Year can be changed\tCtrl-Y"),
                    _T("Allow changing the year in the calendar"),
                    true);
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_SetDate, _T("SetDate()"), _T("Set date to 2005-12-24."));
    menuCal->Append(Calendar_Cal_Today, _T("Today()"), _T("Set the current date."));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuCal, _T("&Calendar"));

    menuBar->Check(Calendar_Cal_Monday, true);
    menuBar->Check(Calendar_Cal_Holidays, true);
    menuBar->Check(Calendar_Cal_Month, true);
    menuBar->Check(Calendar_Cal_Year, true);

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_panel = new MyPanel(this);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("wxCalendarCtrl sample\n(c) 2000 Vadim Zeitlin"),
                 _T("About Calendar"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnCalMonday(wxCommandEvent& event)
{
    bool enable = GetMenuBar()->IsChecked(event.GetId());

    m_panel->ToggleCalStyle(enable, wxCAL_MONDAY_FIRST);
}

void MyFrame::OnCalHolidays(wxCommandEvent& event)
{
    bool enable = GetMenuBar()->IsChecked(event.GetId());

    m_panel->GetCal()->EnableHolidayDisplay(enable);
}

void MyFrame::OnCalSpecial(wxCommandEvent& event)
{
    m_panel->HighlightSpecial(GetMenuBar()->IsChecked(event.GetId()));
}

void MyFrame::OnCalAllowMonth(wxCommandEvent& event)
{
    bool allow = GetMenuBar()->IsChecked(event.GetId());

    m_panel->GetCal()->EnableMonthChange(allow);
}

void MyFrame::OnCalAllowYear(wxCommandEvent& event)
{
    bool allow = GetMenuBar()->IsChecked(event.GetId());

    m_panel->GetCal()->EnableYearChange(allow);
}

void MyFrame::OnCalSeqMonth(wxCommandEvent& event)
{
    bool allow = GetMenuBar()->IsChecked(event.GetId());

    m_panel->ToggleCalStyle(allow, wxCAL_SEQUENTIAL_MONTH_SELECTION);
}

void MyFrame::OnCalShowSurroundingWeeks(wxCommandEvent& event)
{
    bool allow = GetMenuBar()->IsChecked(event.GetId());

    m_panel->ToggleCalStyle(allow, wxCAL_SHOW_SURROUNDING_WEEKS);
}

void MyFrame::OnAllowYearUpdate(wxUpdateUIEvent& event)
{
    event.Enable( GetMenuBar()->IsChecked(Calendar_Cal_Month));
}

void MyFrame::OnSetDate(wxCommandEvent &WXUNUSED(event))
{
    m_panel->SetDate();
}

void MyFrame::OnToday(wxCommandEvent &WXUNUSED(event))
{
    m_panel->Today();
}

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(wxFrame *frame)
       : wxPanel(frame, wxID_ANY)
{
    wxString date;
    date.Printf(wxT("Selected date: %s"),
                wxDateTime::Today().FormatISODate().c_str());
    m_date = new wxStaticText(this, wxID_ANY, date);
    m_calendar = new wxCalendarCtrl(this, Calendar_CalCtrl,
                                    wxDefaultDateTime,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxCAL_MONDAY_FIRST |
                                    wxCAL_SHOW_HOLIDAYS |
                                    wxRAISED_BORDER);

    wxBoxSizer *m_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_sizer->Add(m_date, 0, wxALIGN_CENTER | wxALL, 10 );
    m_sizer->Add(m_calendar, 0, wxALIGN_CENTER | wxALIGN_LEFT);

    SetSizer( m_sizer );
    m_sizer->SetSizeHints( this );
}

void MyPanel::OnCalendar(wxCalendarEvent& event)
{
    wxLogMessage(wxT("Selected %s from calendar"),
                 event.GetDate().FormatISODate().c_str());
}

void MyPanel::OnCalendarChange(wxCalendarEvent& event)
{
    wxString s;
    s.Printf(wxT("Selected date: %s"), event.GetDate().FormatISODate().c_str());

    m_date->SetLabel(s);
}

void MyPanel::OnCalMonthChange(wxCalendarEvent& WXUNUSED(event))
{
    wxLogStatus(wxT("Calendar month changed"));
}

void MyPanel::OnCalYearChange(wxCalendarEvent& WXUNUSED(event))
{
    wxLogStatus(wxT("Calendar year changed"));
}

void MyPanel::OnCalendarWeekDayClick(wxCalendarEvent& event)
{
    wxLogMessage(wxT("Clicked on %s"),
                 wxDateTime::GetWeekDayName(event.GetWeekDay()).c_str());
}

void MyPanel::ToggleCalStyle(bool on, int flag)
{
    long style = m_calendar->GetWindowStyle();
    if ( on )
        style |= flag;
    else
        style &= ~flag;

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

void MyPanel::SetDate()
{
    wxDateTime date(24, wxDateTime::Dec, 2005, 23, 59, 59);
    m_calendar->SetDate(date);
}

void MyPanel::Today()
{
    m_calendar->SetDate(wxDateTime::Today());
}
