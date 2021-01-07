/////////////////////////////////////////////////////////////////////////////
// Name:        calendar.cpp
// Purpose:     wxCalendarCtrl sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     02.01.00
// Copyright:   (c) Vadim Zeitlin
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
    #include "wx/icon.h"
    #include "wx/button.h"
    #include "wx/sizer.h"
    #include "wx/textctrl.h"
    #include "wx/settings.h"
#endif

#include "wx/calctrl.h"
#include "wx/splitter.h"

#if wxUSE_DATEPICKCTRL
    #include "wx/datectrl.h"
    #if wxUSE_DATEPICKCTRL_GENERIC
        #include "wx/generic/datectrl.h"
    #endif // wxUSE_DATEPICKCTRL_GENERIC
#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL
    #include "wx/timectrl.h"
    #if wxUSE_TIMEPICKCTRL_GENERIC
        #include "wx/generic/timectrl.h"
    #endif // wxUSE_TIMEPICKCTRL_GENERIC
#endif // wxUSE_TIMEPICKCTRL

#include "../sample.xpm"

#ifdef wxHAS_NATIVE_CALENDARCTRL
    #include "wx/generic/calctrlg.h"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    MyApp();
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;

private:
    wxLocale m_locale;
};

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent);

    void OnCalendar(wxCalendarEvent& event);
    void OnCalendarWeekDayClick(wxCalendarEvent& event);
    void OnCalendarWeekClick(wxCalendarEvent& event);
    void OnCalendarChange(wxCalendarEvent& event);
    void OnCalMonthChange(wxCalendarEvent& event);

    wxCalendarCtrlBase *GetCal() const { return m_calendar; }

    // turn on/off the specified style bit on the calendar control
    void ToggleCalStyle(bool on, int style);

    bool IsUsingGeneric() const { return m_usingGeneric; }
    void ToggleUseGeneric()
    {
        m_usingGeneric = !m_usingGeneric;
        RecreateCalendar(m_calendar->GetWindowStyle());
    }

    void HighlightSpecial(bool on);
    void LimitDateRange(bool on);

    wxDateTime GetDate() const { return m_calendar->GetDate(); }
    void SetDate(const wxDateTime& dt) { m_calendar->SetDate(dt); }

private:
    wxCalendarCtrlBase *DoCreateCalendar(const wxDateTime& dt, long style);

    void RecreateCalendar(long style);

    wxCalendarCtrlBase *m_calendar;
    wxStaticText   *m_date;
    wxSizer        *m_sizer;

    bool m_usingGeneric;


    wxDECLARE_EVENT_TABLE();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    // event handlers (these functions should _not_ be virtual)
    void OnAbout(wxCommandEvent& event);
    void OnClearLog(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

#if wxUSE_DATEPICKCTRL
    void OnAskDate(wxCommandEvent& event);

    void OnUpdateUIStartWithNone(wxUpdateUIEvent& event);
#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL
    void OnAskTime(wxCommandEvent& event);
#endif // wxUSE_TIMEPICKCTRL

#ifdef wxHAS_NATIVE_CALENDARCTRL
    void OnCalGeneric(wxCommandEvent& WXUNUSED(event))
    {
        m_panel->ToggleUseGeneric();
    }
#endif // wxHAS_NATIVE_CALENDARCTRL

    void OnCalAutoWeekday(wxCommandEvent& event);
    void OnCalSunday(wxCommandEvent& event);
    void OnCalMonday(wxCommandEvent& event);
    void OnCalHolidays(wxCommandEvent& event);
    void OnCalSpecial(wxCommandEvent& event);

    void OnCalAllowMonth(wxCommandEvent& event);
    void OnCalLimitDates(wxCommandEvent& event);
    void OnCalSeqMonth(wxCommandEvent& event);
    void OnCalShowSurroundingWeeks(wxCommandEvent& event);
    void OnCalShowWeekNumbers(wxCommandEvent& event);

    void OnSetDate(wxCommandEvent& event);
    void OnToday(wxCommandEvent& event);
    void OnBeginDST(wxCommandEvent& event);

    void OnCalToggleResizable(wxCommandEvent& event);

    void OnUpdateUIGenericOnly(wxUpdateUIEvent& event)
    {
        event.Enable(m_panel->IsUsingGeneric());
    }

    void OnCalRClick(wxMouseEvent& event);

private:
    MyPanel *m_panel;
    wxTextCtrl *m_logWindow;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

#if wxUSE_DATEPICKCTRL

// Define a simple modal dialog which asks the user for a date
class MyDateDialog : public wxDialog
{
public:
    MyDateDialog(wxWindow *parent, const wxDateTime& dt, int dtpStyle);

    wxDateTime GetDate() const
    {
#if wxUSE_DATEPICKCTRL_GENERIC
        if ( m_datePickerGeneric )
            return m_datePickerGeneric->GetValue();
#endif // wxUSE_DATEPICKCTRL_GENERIC

        return m_datePicker->GetValue();
    }

private:
    void OnDateChange(wxDateEvent& event);


    wxDatePickerCtrl *m_datePicker;
#if wxUSE_DATEPICKCTRL_GENERIC
    wxDatePickerCtrlGeneric *m_datePickerGeneric;
#endif // wxUSE_DATEPICKCTRL_GENERIC

    wxStaticText *m_dateText;


    wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL

// Another simple dialog, this one asking for time.
class MyTimeDialog : public wxDialog
{
public:
    MyTimeDialog(wxWindow* parent);

    wxDateTime GetTime() const
    {
#if wxUSE_TIMEPICKCTRL_GENERIC
        if ( m_timePickerGeneric )
            return m_timePickerGeneric->GetValue();
#endif // wxUSE_TIMEPICKCTRL_GENERIC

        return m_timePicker->GetValue();
    }

private:
    void OnTimeChange(wxDateEvent& event);

    wxTimePickerCtrl* m_timePicker;
#if wxUSE_TIMEPICKCTRL_GENERIC
    wxTimePickerCtrlGeneric* m_timePickerGeneric;
#endif // wxUSE_TIMEPICKCTRL_GENERIC

    wxStaticText* m_timeText;

    wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_TIMEPICKCTRL

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Calendar_File_About = wxID_ABOUT,
    Calendar_File_ClearLog = wxID_CLEAR,
    Calendar_File_Quit = wxID_EXIT,
    Calendar_Cal_Generic = 200,
    Calendar_Cal_AutoWeekday,
    Calendar_Cal_Sunday,
    Calendar_Cal_Monday,
    Calendar_Cal_Holidays,
    Calendar_Cal_Special,
    Calendar_Cal_Month,
    Calendar_Cal_LimitDates,
    Calendar_Cal_SeqMonth,
    Calendar_Cal_SurroundWeeks,
    Calendar_Cal_WeekNumbers,
    Calendar_Cal_SetDate,
    Calendar_Cal_Today,
    Calendar_Cal_BeginDST,
    Calendar_Cal_Resizable,
#if wxUSE_DATEPICKCTRL
    Calendar_DatePicker_AskDate = 300,
    Calendar_DatePicker_ShowCentury,
    Calendar_DatePicker_DropDown,
    Calendar_DatePicker_AllowNone,
    Calendar_DatePicker_StartWithNone,
#if wxUSE_DATEPICKCTRL_GENERIC
    Calendar_DatePicker_Generic,
#endif // wxUSE_DATEPICKCTRL_GENERIC
#endif // wxUSE_DATEPICKCTRL
#if wxUSE_TIMEPICKCTRL
    Calendar_TimePicker_AskTime = 400,
#if wxUSE_TIMEPICKCTRL_GENERIC
    Calendar_TimePicker_Generic,
#endif // wxUSE_TIMEPICKCTRL_GENERIC
#endif // wxUSE_TIMEPICKCTRL
    Calendar_CalCtrl = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Calendar_File_About, MyFrame::OnAbout)
    EVT_MENU(Calendar_File_ClearLog, MyFrame::OnClearLog)
    EVT_MENU(Calendar_File_Quit,  MyFrame::OnQuit)

#if wxUSE_DATEPICKCTRL
    EVT_MENU(Calendar_DatePicker_AskDate, MyFrame::OnAskDate)

    EVT_UPDATE_UI(Calendar_DatePicker_StartWithNone,
                  MyFrame::OnUpdateUIStartWithNone)
#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL
    EVT_MENU(Calendar_TimePicker_AskTime, MyFrame::OnAskTime)
#endif // wxUSE_TIMEPICKCTRL

#ifdef wxHAS_NATIVE_CALENDARCTRL
    EVT_MENU(Calendar_Cal_Generic, MyFrame::OnCalGeneric)
#endif // wxHAS_NATIVE_CALENDARCTRL

    EVT_MENU(Calendar_Cal_AutoWeekday, MyFrame::OnCalAutoWeekday)
    EVT_MENU(Calendar_Cal_Sunday, MyFrame::OnCalSunday)
    EVT_MENU(Calendar_Cal_Monday, MyFrame::OnCalMonday)
    EVT_MENU(Calendar_Cal_Holidays, MyFrame::OnCalHolidays)
    EVT_MENU(Calendar_Cal_Special, MyFrame::OnCalSpecial)

    EVT_MENU(Calendar_Cal_Month, MyFrame::OnCalAllowMonth)

    EVT_MENU(Calendar_Cal_LimitDates, MyFrame::OnCalLimitDates)

    EVT_MENU(Calendar_Cal_SeqMonth, MyFrame::OnCalSeqMonth)
    EVT_MENU(Calendar_Cal_SurroundWeeks, MyFrame::OnCalShowSurroundingWeeks)
    EVT_MENU(Calendar_Cal_WeekNumbers, MyFrame::OnCalShowWeekNumbers)

    EVT_MENU(Calendar_Cal_SetDate, MyFrame::OnSetDate)
    EVT_MENU(Calendar_Cal_Today, MyFrame::OnToday)
    EVT_MENU(Calendar_Cal_BeginDST, MyFrame::OnBeginDST)

    EVT_MENU(Calendar_Cal_Resizable, MyFrame::OnCalToggleResizable)


    EVT_UPDATE_UI(Calendar_Cal_SeqMonth, MyFrame::OnUpdateUIGenericOnly)
#ifdef __WXGTK20__
    EVT_UPDATE_UI(Calendar_Cal_AutoWeekday, MyFrame::OnUpdateUIGenericOnly)
    EVT_UPDATE_UI(Calendar_Cal_Sunday, MyFrame::OnUpdateUIGenericOnly)
    EVT_UPDATE_UI(Calendar_Cal_Monday, MyFrame::OnUpdateUIGenericOnly)
    EVT_UPDATE_UI(Calendar_Cal_Holidays, MyFrame::OnUpdateUIGenericOnly)
#endif
    EVT_UPDATE_UI(Calendar_Cal_Special, MyFrame::OnUpdateUIGenericOnly)
    EVT_UPDATE_UI(Calendar_Cal_SurroundWeeks, MyFrame::OnUpdateUIGenericOnly)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_CALENDAR(Calendar_CalCtrl, MyPanel::OnCalendar)
    EVT_CALENDAR_PAGE_CHANGED(Calendar_CalCtrl, MyPanel::OnCalMonthChange)
    EVT_CALENDAR_SEL_CHANGED(Calendar_CalCtrl, MyPanel::OnCalendarChange)
    EVT_CALENDAR_WEEKDAY_CLICKED(Calendar_CalCtrl, MyPanel::OnCalendarWeekDayClick)
    EVT_CALENDAR_WEEK_CLICKED(Calendar_CalCtrl,  MyPanel::OnCalendarWeekClick)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

MyApp::MyApp() :
    // Locale affects on the language used in the calendar, and may affect
    // on the first day of the week.
    m_locale(wxLANGUAGE_DEFAULT)
{
}

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame("Calendar wxWidgets sample"
                                 ,wxPoint(50, 50), wxSize(450, 340)
                                 );

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
    // set the frame icon
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenuBar *menuBar = new wxMenuBar;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Calendar_File_About, "&About\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_ClearLog, "&Clear log\tCtrl-L");
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_Quit, "E&xit\tAlt-X", "Quit this program");
    menuBar->Append(menuFile, "&File");

    wxMenu *menuCal = new wxMenu;
#ifdef wxHAS_NATIVE_CALENDARCTRL
    menuCal->AppendCheckItem(Calendar_Cal_Generic, "Use &generic version\tCtrl-G",
                             "Toggle between native and generic control");
    menuCal->AppendSeparator();
#endif // wxHAS_NATIVE_CALENDARCTRL
    menuCal->AppendRadioItem(Calendar_Cal_AutoWeekday,
                    "Automatic &first weekday\tCtrl-V");
    menuCal->AppendRadioItem(Calendar_Cal_Sunday,
                    "Sunday &first weekday\tCtrl-Z");
    menuCal->AppendRadioItem(Calendar_Cal_Monday,
                    "Monday &first weekday\tCtrl-F");
    menuCal->Append(Calendar_Cal_Holidays, "Show &holidays\tCtrl-H",
                    "Toggle highlighting the holidays",
                    true);
    menuCal->Append(Calendar_Cal_Special, "Highlight &special dates\tCtrl-S",
                    "Test custom highlighting",
                    true);
    menuCal->Append(Calendar_Cal_SurroundWeeks,
                    "Show s&urrounding weeks\tCtrl-W",
                    "Show the neighbouring weeks in the prev/next month",
                    true);
    menuCal->Append(Calendar_Cal_WeekNumbers,
                    "Show &week numbers",
                    "Toggle week numbers",
                    true);
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_SeqMonth,
                    "Toggle month selector st&yle\tCtrl-Y",
                    "Use another style for the calendar controls",
                    true);
    menuCal->Append(Calendar_Cal_Month, "&Month can be changed\tCtrl-M",
                    "Allow changing the month in the calendar",
                    true);
    menuCal->AppendCheckItem(Calendar_Cal_LimitDates, "Toggle date ra&nge\tCtrl-N",
                    "Limit the valid dates");
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_SetDate, "Call &SetDate(2005-12-24)", "Set date to 2005-12-24.");
    menuCal->Append(Calendar_Cal_Today, "Call &Today()", "Set to the current date.");
    menuCal->Append(Calendar_Cal_BeginDST, "Call SetDate(GetBeginDST())");
    menuCal->AppendSeparator();
    menuCal->AppendCheckItem(Calendar_Cal_Resizable, "Make &resizable\tCtrl-R");
    menuBar->Append(menuCal, "&Calendar");

#if wxUSE_DATEPICKCTRL
    wxMenu *menuDate = new wxMenu;
    menuDate->AppendCheckItem(Calendar_DatePicker_ShowCentury,
                              "Al&ways show century");
    menuDate->AppendCheckItem(Calendar_DatePicker_DropDown,
                              "Use &drop down control");
    menuDate->AppendCheckItem(Calendar_DatePicker_AllowNone,
                              "Allow &no date");
    menuDate->AppendCheckItem(Calendar_DatePicker_StartWithNone,
                              "Start &with no date");
#if wxUSE_DATEPICKCTRL_GENERIC
    menuDate->AppendCheckItem(Calendar_DatePicker_Generic,
                              "Use &generic version of the control");
#endif // wxUSE_DATEPICKCTRL_GENERIC
    menuDate->AppendSeparator();
    menuDate->Append(Calendar_DatePicker_AskDate, "&Choose date...\tCtrl-D", "Show dialog with wxDatePickerCtrl");
    menuBar->Append(menuDate, "&Date picker");
#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL
    wxMenu *menuTime = new wxMenu;
#if wxUSE_TIMEPICKCTRL_GENERIC
    menuTime->AppendCheckItem(Calendar_TimePicker_Generic,
                              "Use &generic version of the control");
    menuTime->AppendSeparator();
#endif // wxUSE_TIMEPICKCTRL_GENERIC
    menuTime->Append(Calendar_TimePicker_AskTime, "&Choose time...\tCtrl-T", "Show dialog with wxTimePickerCtrl");
    menuBar->Append(menuTime, "&Time picker");
#endif // wxUSE_TIMEPICKCTRL

    menuBar->Check(Calendar_Cal_AutoWeekday, true);
    menuBar->Check(Calendar_Cal_Sunday, false);
    menuBar->Check(Calendar_Cal_Monday, false);
    menuBar->Check(Calendar_Cal_Holidays, true);
    menuBar->Check(Calendar_Cal_Month, true);
    menuBar->Check(Calendar_Cal_LimitDates, false);

#if wxUSE_DATEPICKCTRL
    menuBar->Check(Calendar_DatePicker_ShowCentury, true);
#endif // wxUSE_DATEPICKCTRL

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY,
                                            wxDefaultPosition, wxDefaultSize,
                                            wxSP_NOBORDER);
    m_panel = new MyPanel(splitter);
    m_logWindow = new wxTextCtrl(splitter, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY | wxTE_MULTILINE);
    splitter->SplitHorizontally(m_panel, m_logWindow);
    splitter->SetMinimumPaneSize(20);
    wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("wxCalendarCtrl sample\n(c) 2000--2008 Vadim Zeitlin",
                 "About Calendar", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
}

void MyFrame::OnCalAutoWeekday(wxCommandEvent&)
{
    m_panel->ToggleCalStyle(false, wxCAL_SUNDAY_FIRST);
    m_panel->ToggleCalStyle(false, wxCAL_MONDAY_FIRST);
}

void MyFrame::OnCalSunday(wxCommandEvent& event)
{
    m_panel->ToggleCalStyle(false, wxCAL_MONDAY_FIRST);
    m_panel->ToggleCalStyle(event.IsChecked(), wxCAL_SUNDAY_FIRST);
}

void MyFrame::OnCalMonday(wxCommandEvent& event)
{
    m_panel->ToggleCalStyle(false, wxCAL_SUNDAY_FIRST);
    m_panel->ToggleCalStyle(event.IsChecked(), wxCAL_MONDAY_FIRST);
}

void MyFrame::OnCalHolidays(wxCommandEvent& event)
{
    m_panel->GetCal()->EnableHolidayDisplay(event.IsChecked());
}

void MyFrame::OnCalSpecial(wxCommandEvent& event)
{
    m_panel->HighlightSpecial(GetMenuBar()->IsChecked(event.GetId()));
}

void MyFrame::OnCalLimitDates(wxCommandEvent& event)
{
    m_panel->LimitDateRange(GetMenuBar()->IsChecked(event.GetId()));
}

void MyFrame::OnCalAllowMonth(wxCommandEvent& event)
{
    m_panel->GetCal()->EnableMonthChange(event.IsChecked());
}

void MyFrame::OnCalSeqMonth(wxCommandEvent& event)
{
    m_panel->ToggleCalStyle(event.IsChecked(),
                            wxCAL_SEQUENTIAL_MONTH_SELECTION);
}

void MyFrame::OnCalShowSurroundingWeeks(wxCommandEvent& event)
{
    m_panel->ToggleCalStyle(event.IsChecked(), wxCAL_SHOW_SURROUNDING_WEEKS);
}

void MyFrame::OnCalShowWeekNumbers(wxCommandEvent& event)
{
    m_panel->ToggleCalStyle(event.IsChecked(), wxCAL_SHOW_WEEK_NUMBERS);
}

void MyFrame::OnSetDate(wxCommandEvent &WXUNUSED(event))
{
    m_panel->SetDate(wxDateTime(24, wxDateTime::Dec, 2005, 22, 00, 00));
}

void MyFrame::OnToday(wxCommandEvent &WXUNUSED(event))
{
    m_panel->SetDate(wxDateTime::Today());
}

void MyFrame::OnBeginDST(wxCommandEvent &WXUNUSED(event))
{
    m_panel->SetDate(wxDateTime::GetBeginDST(m_panel->GetDate().GetYear()));
}

void MyFrame::OnCalToggleResizable(wxCommandEvent& event)
{
    wxSizer * const sizer = m_panel->GetSizer();
    wxSizerItem * const item = sizer->GetItem(m_panel->GetCal());
    if ( event.IsChecked() )
    {
        item->SetProportion(1);
        item->SetFlag(wxEXPAND);
    }
    else // not resizable
    {
        item->SetProportion(0);
        item->SetFlag(wxALIGN_CENTER);
    }

    sizer->Layout();
}

void MyFrame::OnCalRClick(wxMouseEvent& event)
{
    wxDateTime dt;
    wxDateTime::WeekDay wd;

    const wxPoint pt = event.GetPosition();
    wxString msg = wxString::Format("Point (%d, %d) is ", pt.x, pt.y);

    switch ( m_panel->GetCal()->HitTest(pt, &dt, &wd) )
    {
        default:
            wxFAIL_MSG( "unexpected" );
            wxFALLTHROUGH;

        case wxCAL_HITTEST_NOWHERE:
            msg += "nowhere";
            break;

        case wxCAL_HITTEST_HEADER:
            msg += wxString::Format("over %s", wxDateTime::GetWeekDayName(wd));
            break;

        case wxCAL_HITTEST_DAY:
            msg += wxString::Format("over %s", dt.FormatISODate());
            break;

        case wxCAL_HITTEST_INCMONTH:
            msg += "over next month button";
            break;

        case wxCAL_HITTEST_DECMONTH:
            msg += "over previous month button";
            break;

        case wxCAL_HITTEST_SURROUNDING_WEEK:
            msg += "over a day from another month";
            break;
    }

    wxLogMessage("%s", msg);
}

#if wxUSE_DATEPICKCTRL

void MyFrame::OnUpdateUIStartWithNone(wxUpdateUIEvent& event)
{
    // it only makes sense to start with invalid date if we can have no date
    event.Enable( GetMenuBar()->IsChecked(Calendar_DatePicker_AllowNone) );
}

void MyFrame::OnAskDate(wxCommandEvent& WXUNUSED(event))
{
    wxDateTime dt = m_panel->GetCal()->GetDate();

    int style = wxDP_DEFAULT;
    if ( GetMenuBar()->IsChecked(Calendar_DatePicker_ShowCentury) )
        style |= wxDP_SHOWCENTURY;
    if ( GetMenuBar()->IsChecked(Calendar_DatePicker_DropDown) )
        style |= wxDP_DROPDOWN;
    if ( GetMenuBar()->IsChecked(Calendar_DatePicker_AllowNone) )
    {
        style |= wxDP_ALLOWNONE;

        if ( GetMenuBar()->IsChecked(Calendar_DatePicker_StartWithNone) )
            dt = wxDefaultDateTime;
    }

    MyDateDialog dlg(this, dt, style);
    if ( dlg.ShowModal() == wxID_OK )
    {
        dt = dlg.GetDate();
        if ( dt.IsValid() )
        {
            const wxDateTime today = wxDateTime::Today();

            if ( dt.GetDay() == today.GetDay() &&
                    dt.GetMonth() == today.GetMonth() )
            {
                wxMessageBox("Happy birthday!", "Calendar Sample");
            }

            m_panel->SetDate(dt);

            wxLogStatus("Changed the date to your input");
        }
        else
        {
            wxLogStatus("No date entered");
        }
    }
}

#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL

void MyFrame::OnAskTime(wxCommandEvent& WXUNUSED(event))
{
    MyTimeDialog dlg(this);
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxLogMessage("You entered %s", dlg.GetTime().FormatISOTime());
    }
}

#endif // wxUSE_TIMEPICKCTRL

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(wxWindow *parent)
       : wxPanel(parent, wxID_ANY)
{
#ifdef wxHAS_NATIVE_CALENDARCTRL
    m_usingGeneric = false;
#else
    m_usingGeneric = true;
#endif

    wxString date;
    date.Printf("Selected date: %s",
                wxDateTime::Today().FormatISODate());
    m_date = new wxStaticText(this, wxID_ANY, date);
    m_calendar = DoCreateCalendar(wxDefaultDateTime,
                                  wxCAL_SHOW_HOLIDAYS);

    // adjust to vertical/horizontal display
    bool horizontal = ( wxSystemSettings::GetMetric(wxSYS_SCREEN_X) > wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) );
    m_sizer = new wxBoxSizer( horizontal ? wxHORIZONTAL : wxVERTICAL );

    m_sizer->Add(m_date, 0, wxALIGN_CENTER | wxALL, 10 );
    m_sizer->Add(m_calendar, 0, wxALIGN_CENTER | wxALIGN_LEFT);

    SetSizer( m_sizer );
    m_sizer->SetSizeHints( this );
}

void MyPanel::OnCalendar(wxCalendarEvent& event)
{
    // clicking the same date twice unmarks it (convenient for testing)
    static wxDateTime s_dateLast;
    const bool mark = !s_dateLast.IsValid() || event.GetDate() != s_dateLast;

    s_dateLast = event.GetDate();

    m_calendar->Mark(event.GetDate().GetDay(), mark);
    wxLogMessage("Selected (and %smarked) %s from calendar.",
                 mark ? "" : "un", s_dateLast.FormatISODate());
}

void MyPanel::OnCalendarChange(wxCalendarEvent& event)
{
    wxString s;
    s.Printf("Selected date: %s", event.GetDate().FormatISODate());

    m_date->SetLabel(s);
    wxLogStatus(s);
}

void MyPanel::OnCalMonthChange(wxCalendarEvent& event)
{
    wxLogStatus("Calendar month changed to %s %d",
                wxDateTime::GetMonthName(event.GetDate().GetMonth()),
                event.GetDate().GetYear());
}

void MyPanel::OnCalendarWeekDayClick(wxCalendarEvent& event)
{
    wxLogMessage("Clicked on %s",
                 wxDateTime::GetWeekDayName(event.GetWeekDay()));
}

void MyPanel::OnCalendarWeekClick(wxCalendarEvent& event)
{
    wxLogMessage("Clicked on week %d", event.GetDate().GetWeekOfYear());
}

wxCalendarCtrlBase *MyPanel::DoCreateCalendar(const wxDateTime& dt, long style)
{
    wxCalendarCtrlBase *calendar;
#ifdef wxHAS_NATIVE_CALENDARCTRL
    if ( m_usingGeneric )
        calendar = new wxGenericCalendarCtrl(this, Calendar_CalCtrl,
                                             dt,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             style);
    else
#endif // wxHAS_NATIVE_CALENDARCTRL
        calendar = new wxCalendarCtrl(this, Calendar_CalCtrl,
                                      dt,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      style);

    calendar->Bind(wxEVT_RIGHT_DOWN,
                   &MyFrame::OnCalRClick,
                   ( MyFrame * )wxGetTopLevelParent(this));

    return calendar;
}

void MyPanel::RecreateCalendar(long style)
{
    wxCalendarCtrlBase *calendar = DoCreateCalendar(m_calendar->GetDate(), style);

    m_sizer->Replace(m_calendar, calendar);
    delete m_calendar;
    m_calendar = calendar;

    m_sizer->Layout();
}

void MyPanel::ToggleCalStyle(bool on, int flag)
{
    long style = m_calendar->GetWindowStyle();
    if ( on )
        style |= flag;
    else
        style &= ~flag;

    if ( flag == wxCAL_SEQUENTIAL_MONTH_SELECTION
        || flag == wxCAL_SHOW_WEEK_NUMBERS)
    {
        // changing this style requires recreating the control
        RecreateCalendar(style);
    }
    else // just changing the style is enough
    {
        m_calendar->SetWindowStyle(style);

        m_calendar->Refresh();
    }
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

// Toggle a restricted date range to the six months centered on today's date.
void MyPanel::LimitDateRange(bool on)
{
    if ( on )
    {
        // limit the choice of date to 3 months around today
        const wxDateSpan diff = wxDateSpan::Months(3);
        const wxDateTime today = wxDateTime::Today();

        // Set the restricted date range.
        if ( m_calendar->SetDateRange(today - diff, today + diff) )
        {
            wxLogStatus("Date range limited to 3 months around today.");
            wxDateTime firstValidDate;
            wxDateTime lastValidDate;
            if ( m_calendar->GetDateRange(&firstValidDate, &lastValidDate) )
            {
                wxLogMessage("First valid date: %s, last valid date: %s",
                             firstValidDate.FormatISODate(),
                             lastValidDate.FormatISODate());
            }
            else
            {
                wxLogWarning("Failed to get back the valid dates range.");
            }
        }
        else
        {
            wxLogWarning("Date range not supported.");
        }
    }
    else // off
    {
        // Remove the date restrictions.
        if ( m_calendar->SetDateRange() )
        {
            wxLogStatus("Date choice is unlimited now.");
        }
        else
        {
            wxLogWarning("Date range not supported.");
        }
    }

    m_calendar->Refresh();
}

// ----------------------------------------------------------------------------
// MyDateDialog
// ----------------------------------------------------------------------------

#if wxUSE_DATEPICKCTRL

wxBEGIN_EVENT_TABLE(MyDateDialog, wxDialog)
    EVT_DATE_CHANGED(wxID_ANY, MyDateDialog::OnDateChange)
wxEND_EVENT_TABLE()

MyDateDialog::MyDateDialog(wxWindow *parent, const wxDateTime& dt, int dtpStyle)
        : wxDialog(parent, wxID_ANY, wxString("Calendar: Choose a date"))
{
    wxWindow* datePickerWindow = NULL;

#if wxUSE_DATEPICKCTRL_GENERIC
    m_datePickerGeneric = NULL;
    m_datePicker = NULL;

    wxFrame *frame = (wxFrame *)wxGetTopLevelParent(parent);
    if ( frame && frame->GetMenuBar()->IsChecked(Calendar_DatePicker_Generic) )
    {
        m_datePickerGeneric = new wxDatePickerCtrlGeneric(this, wxID_ANY, dt,
                                                          wxDefaultPosition,
                                                          wxDefaultSize,
                                                          dtpStyle);
        m_datePickerGeneric->SetRange(wxDateTime(1, wxDateTime::Jan, 1900),
                                      wxDefaultDateTime);

        datePickerWindow = m_datePickerGeneric;
    }
    else
#endif // wxUSE_DATEPICKCTRL_GENERIC
    {
        m_datePicker = new wxDatePickerCtrl(this, wxID_ANY, dt,
                                            wxDefaultPosition, wxDefaultSize,
                                            dtpStyle);
        m_datePicker->SetRange(wxDateTime(1, wxDateTime::Jan, 1900),
                               wxDefaultDateTime);

        datePickerWindow = m_datePicker;
    }

    m_dateText = new wxStaticText(this, wxID_ANY,
                                  dt.IsValid() ? dt.FormatISODate()
                                               : wxString());

    const wxSizerFlags flags = wxSizerFlags().Centre().Border();
    wxFlexGridSizer* const sizerMain = new wxFlexGridSizer(2);
    sizerMain->Add(new wxStaticText(this, wxID_ANY, "Enter &date:"), flags);
    sizerMain->Add(datePickerWindow, flags);

    sizerMain->Add(new wxStaticText(this, wxID_ANY, "Date in ISO format:"),
                   flags);
    sizerMain->Add(m_dateText, flags);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(sizerMain, flags);
    sizerTop->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), flags);

    SetSizerAndFit(sizerTop);
}

void MyDateDialog::OnDateChange(wxDateEvent& event)
{
    const wxDateTime dt = event.GetDate();
    if ( dt.IsValid() )
        m_dateText->SetLabel(dt.FormatISODate());
    else
        m_dateText->SetLabel(wxEmptyString);
}

#endif // wxUSE_DATEPICKCTRL

// ----------------------------------------------------------------------------
// MyTimeDialog
// ----------------------------------------------------------------------------

#if wxUSE_TIMEPICKCTRL

wxBEGIN_EVENT_TABLE(MyTimeDialog, wxDialog)
    EVT_TIME_CHANGED(wxID_ANY, MyTimeDialog::OnTimeChange)
wxEND_EVENT_TABLE()

MyTimeDialog::MyTimeDialog(wxWindow *parent)
        : wxDialog(parent, wxID_ANY, wxString("Calendar: Choose time"))
{
    wxWindow* timePickerWindow = NULL;

#if wxUSE_TIMEPICKCTRL_GENERIC
    m_timePickerGeneric = NULL;
    m_timePicker = NULL;

    wxFrame *frame = (wxFrame *)wxGetTopLevelParent(parent);
    if ( frame && frame->GetMenuBar()->IsChecked(Calendar_TimePicker_Generic) )
    {
        m_timePickerGeneric = new wxTimePickerCtrlGeneric(this, wxID_ANY);
        timePickerWindow = m_timePickerGeneric;
    }
    else
#endif // wxUSE_TIMEPICKCTRL_GENERIC
    m_timePicker = new wxTimePickerCtrl(this, wxID_ANY);

    if ( !timePickerWindow )
        timePickerWindow = m_timePicker;

    m_timeText = new wxStaticText(this, wxID_ANY, GetTime().FormatISOTime());

    const wxSizerFlags flags = wxSizerFlags().Centre().Border();
    wxFlexGridSizer* const sizerMain = new wxFlexGridSizer(2);
    sizerMain->Add(new wxStaticText(this, wxID_ANY, "Enter &time:"), flags);
    sizerMain->Add(timePickerWindow, flags);

    sizerMain->Add(new wxStaticText(this, wxID_ANY, "Time in ISO format:"),
                   flags);
    sizerMain->Add(m_timeText, flags);

    wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(sizerMain, flags);
    sizerTop->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), flags);

    SetSizerAndFit(sizerTop);
}

void MyTimeDialog::OnTimeChange(wxDateEvent& event)
{
    m_timeText->SetLabel(event.GetDate().FormatISOTime());
}

#endif // wxUSE_TIMEPICKCTRL
