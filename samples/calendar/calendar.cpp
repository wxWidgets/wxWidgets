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


    DECLARE_EVENT_TABLE()
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

#ifdef wxHAS_NATIVE_CALENDARCTRL
    void OnCalGeneric(wxCommandEvent& WXUNUSED(event))
    {
        m_panel->ToggleUseGeneric();
    }
#endif // wxHAS_NATIVE_CALENDARCTRL

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
    DECLARE_EVENT_TABLE()
};

#if wxUSE_DATEPICKCTRL

// Define a simple modal dialog which asks the user for a date
class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow *parent, const wxDateTime& dt, int dtpStyle);

    wxDateTime GetDate() const { return m_datePicker->GetValue(); }

private:
    void OnDateChange(wxDateEvent& event);


    wxDatePickerCtrlBase *m_datePicker;
    wxTextCtrl *m_text;


    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_DATEPICKCTRL

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
    Calendar_CalCtrl = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Calendar_File_About, MyFrame::OnAbout)
    EVT_MENU(Calendar_File_ClearLog, MyFrame::OnClearLog)
    EVT_MENU(Calendar_File_Quit,  MyFrame::OnQuit)

#if wxUSE_DATEPICKCTRL
    EVT_MENU(Calendar_DatePicker_AskDate, MyFrame::OnAskDate)

    EVT_UPDATE_UI(Calendar_DatePicker_StartWithNone,
                  MyFrame::OnUpdateUIStartWithNone)
#endif // wxUSE_DATEPICKCTRL

#ifdef wxHAS_NATIVE_CALENDARCTRL
    EVT_MENU(Calendar_Cal_Generic, MyFrame::OnCalGeneric)
#endif // wxHAS_NATIVE_CALENDARCTRL

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
    EVT_UPDATE_UI(Calendar_Cal_Monday, MyFrame::OnUpdateUIGenericOnly)
    EVT_UPDATE_UI(Calendar_Cal_Holidays, MyFrame::OnUpdateUIGenericOnly)
#endif
    EVT_UPDATE_UI(Calendar_Cal_Special, MyFrame::OnUpdateUIGenericOnly)
    EVT_UPDATE_UI(Calendar_Cal_SurroundWeeks, MyFrame::OnUpdateUIGenericOnly)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_CALENDAR(Calendar_CalCtrl, MyPanel::OnCalendar)
    EVT_CALENDAR_PAGE_CHANGED(Calendar_CalCtrl, MyPanel::OnCalMonthChange)
    EVT_CALENDAR_SEL_CHANGED(Calendar_CalCtrl, MyPanel::OnCalendarChange)
    EVT_CALENDAR_WEEKDAY_CLICKED(Calendar_CalCtrl, MyPanel::OnCalendarWeekDayClick)
    EVT_CALENDAR_WEEK_CLICKED(Calendar_CalCtrl,  MyPanel::OnCalendarWeekClick)
END_EVENT_TABLE()

#if wxUSE_DATEPICKCTRL

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_DATE_CHANGED(wxID_ANY, MyDialog::OnDateChange)
END_EVENT_TABLE()

#endif // wxUSE_DATEPICKCTRL

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
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("Calendar wxWidgets sample")
#ifndef __WXWINCE__
                                 ,wxPoint(50, 50), wxSize(450, 340)
#endif
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
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Calendar_File_About, wxT("&About...\tCtrl-A"), wxT("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_ClearLog, wxT("&Clear log\tCtrl-L"));
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_Quit, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    wxMenu *menuCal = new wxMenu;
#ifdef wxHAS_NATIVE_CALENDARCTRL
    menuCal->AppendCheckItem(Calendar_Cal_Generic, "Use &generic version\tCtrl-G",
                             "Toggle between native and generic control");
    menuCal->AppendSeparator();
#endif // wxHAS_NATIVE_CALENDARCTRL
    menuCal->Append(Calendar_Cal_Monday,
                    wxT("Monday &first weekday\tCtrl-F"),
                    wxT("Toggle between Mon and Sun as the first week day"),
                    true);
    menuCal->Append(Calendar_Cal_Holidays, wxT("Show &holidays\tCtrl-H"),
                    wxT("Toggle highlighting the holidays"),
                    true);
    menuCal->Append(Calendar_Cal_Special, wxT("Highlight &special dates\tCtrl-S"),
                    wxT("Test custom highlighting"),
                    true);
    menuCal->Append(Calendar_Cal_SurroundWeeks,
                    wxT("Show s&urrounding weeks\tCtrl-W"),
                    wxT("Show the neighbouring weeks in the prev/next month"),
                    true);
    menuCal->Append(Calendar_Cal_WeekNumbers,
                    wxT("Show &week numbers"),
                    wxT("Toggle week numbers"),
                    true);
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_SeqMonth,
                    wxT("Toggle month selector st&yle\tCtrl-Y"),
                    wxT("Use another style for the calendar controls"),
                    true);
    menuCal->Append(Calendar_Cal_Month, wxT("&Month can be changed\tCtrl-M"),
                    wxT("Allow changing the month in the calendar"),
                    true);
    menuCal->AppendCheckItem(Calendar_Cal_LimitDates, wxT("Toggle date ra&nge\tCtrl-N"),
                    wxT("Limit the valid dates"));
    menuCal->AppendSeparator();
    menuCal->Append(Calendar_Cal_SetDate, wxT("Call &SetDate(2005-12-24)"), wxT("Set date to 2005-12-24."));
    menuCal->Append(Calendar_Cal_Today, wxT("Call &Today()"), wxT("Set to the current date."));
    menuCal->Append(Calendar_Cal_BeginDST, "Call SetDate(GetBeginDST())");
    menuCal->AppendSeparator();
    menuCal->AppendCheckItem(Calendar_Cal_Resizable, wxT("Make &resizable\tCtrl-R"));

#if wxUSE_DATEPICKCTRL
    wxMenu *menuDate = new wxMenu;
    menuDate->AppendCheckItem(Calendar_DatePicker_ShowCentury,
                              wxT("Al&ways show century"));
    menuDate->AppendCheckItem(Calendar_DatePicker_DropDown,
                              wxT("Use &drop down control"));
    menuDate->AppendCheckItem(Calendar_DatePicker_AllowNone,
                              wxT("Allow &no date"));
    menuDate->AppendCheckItem(Calendar_DatePicker_StartWithNone,
                              wxT("Start &with no date"));
#if wxUSE_DATEPICKCTRL_GENERIC
    menuDate->AppendCheckItem(Calendar_DatePicker_Generic,
                              wxT("Use &generic version of the control"));
#endif // wxUSE_DATEPICKCTRL_GENERIC
    menuDate->AppendSeparator();
    menuDate->Append(Calendar_DatePicker_AskDate, wxT("&Choose date...\tCtrl-D"), wxT("Show dialog with wxDatePickerCtrl"));
#endif // wxUSE_DATEPICKCTRL

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(menuCal, wxT("&Calendar"));
#if wxUSE_DATEPICKCTRL
    menuBar->Append(menuDate, wxT("&Date picker"));
#endif // wxUSE_DATEPICKCTRL

    menuBar->Check(Calendar_Cal_Monday, true);
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
    wxMessageBox(wxT("wxCalendarCtrl sample\n(c) 2000--2008 Vadim Zeitlin"),
                 wxT("About Calendar"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_logWindow->Clear();
}

void MyFrame::OnCalMonday(wxCommandEvent& event)
{
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
            // fall through

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

    MyDialog dlg(this, dt, style);
    if ( dlg.ShowModal() == wxID_OK )
    {
        dt = dlg.GetDate();
        if ( dt.IsValid() )
        {
            const wxDateTime today = wxDateTime::Today();

            if ( dt.GetDay() == today.GetDay() &&
                    dt.GetMonth() == today.GetMonth() )
            {
                wxMessageBox(wxT("Happy birthday!"), wxT("Calendar Sample"));
            }

            m_panel->SetDate(dt);

            wxLogStatus(wxT("Changed the date to your input"));
        }
        else
        {
            wxLogStatus(wxT("No date entered"));
        }
    }
}

#endif // wxUSE_DATEPICKCTRL

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
    date.Printf(wxT("Selected date: %s"),
                wxDateTime::Today().FormatISODate().c_str());
    m_date = new wxStaticText(this, wxID_ANY, date);
    m_calendar = DoCreateCalendar(wxDefaultDateTime,
                                  wxCAL_MONDAY_FIRST | wxCAL_SHOW_HOLIDAYS);

    // adjust to vertical/horizontal display, check mostly dedicated to WinCE
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
    wxLogMessage(wxT("Selected (and %smarked) %s from calendar."),
                 mark ? "" : "un", s_dateLast.FormatISODate().c_str());
}

void MyPanel::OnCalendarChange(wxCalendarEvent& event)
{
    wxString s;
    s.Printf(wxT("Selected date: %s"), event.GetDate().FormatISODate().c_str());

    m_date->SetLabel(s);
    wxLogStatus(s);
}

void MyPanel::OnCalMonthChange(wxCalendarEvent& event)
{
    wxLogStatus(wxT("Calendar month changed to %s %d"),
                wxDateTime::GetMonthName(event.GetDate().GetMonth()),
                event.GetDate().GetYear());
}

void MyPanel::OnCalendarWeekDayClick(wxCalendarEvent& event)
{
    wxLogMessage(wxT("Clicked on %s"),
                 wxDateTime::GetWeekDayName(event.GetWeekDay()).c_str());
}

void MyPanel::OnCalendarWeekClick(wxCalendarEvent& event)
{
    wxLogMessage(wxT("Clicked on week %d"), event.GetDate().GetWeekOfYear());
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

    calendar->Connect(wxEVT_RIGHT_DOWN,
                      wxMouseEventHandler(MyFrame::OnCalRClick),
                      NULL,
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
// MyDialog
// ----------------------------------------------------------------------------

#if wxUSE_DATEPICKCTRL

MyDialog::MyDialog(wxWindow *parent, const wxDateTime& dt, int dtpStyle)
        : wxDialog(parent, wxID_ANY, wxString(wxT("Calendar: Choose a date")))
{
    wxStdDialogButtonSizer *sizerBtns = new wxStdDialogButtonSizer;
    sizerBtns->AddButton(new wxButton(this, wxID_OK));
    sizerBtns->AddButton(new wxButton(this, wxID_CANCEL));
    sizerBtns->Realize();

    wxSizer *sizerText = new wxBoxSizer(wxHORIZONTAL);
    sizerText->Add(new wxStaticText(this, wxID_ANY, wxT("Date in ISO format: ")),
                    wxSizerFlags().Border().Align(wxALIGN_CENTRE_VERTICAL));
    m_text = new wxTextCtrl(this, wxID_ANY);
    sizerText->Add(m_text, wxSizerFlags().
                        Expand().Border().Align(wxALIGN_CENTRE_VERTICAL));

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(new wxStaticText
                      (
                        this, wxID_ANY,
                        wxT("Enter your birthday date (not before 20th century):")
                      ),
                    wxSizerFlags().Border());

#if wxUSE_DATEPICKCTRL_GENERIC
    wxFrame *frame = (wxFrame *)wxGetTopLevelParent(parent);
    if ( frame && frame->GetMenuBar()->IsChecked(Calendar_DatePicker_Generic) )
        m_datePicker = new wxDatePickerCtrlGeneric(this, wxID_ANY, dt,
                                                   wxDefaultPosition,
                                                   wxDefaultSize,
                                                   dtpStyle);
    else
#endif // wxUSE_DATEPICKCTRL_GENERIC
    m_datePicker = new wxDatePickerCtrl(this, wxID_ANY, dt,
                                        wxDefaultPosition, wxDefaultSize,
                                        dtpStyle);
    m_datePicker->SetRange(wxDateTime(1, wxDateTime::Jan, 1900),
                            wxDefaultDateTime);
    sizerTop->Add(m_datePicker, wxSizerFlags().Expand().Border());

    sizerTop->AddStretchSpacer(1);
    sizerTop->Add(sizerText);

    sizerTop->Add(sizerBtns, wxSizerFlags().Centre().Border());

    SetSizerAndFit(sizerTop);
    Layout();
}

void MyDialog::OnDateChange(wxDateEvent& event)
{
    const wxDateTime dt = event.GetDate();
    if ( dt.IsValid() )
        m_text->SetValue(dt.FormatISODate());
    else
        m_text->SetValue(wxEmptyString);
}

#endif // wxUSE_DATEPICKCTRL
