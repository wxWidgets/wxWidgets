///////////////////////////////////////////////////////////////////////////////
// Name:        generic/calctrl.cpp
// Purpose:     implementation fo the generic wxCalendarCtrl
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "calctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/combobox.h"
    #include "wx/stattext.h"
#endif //WX_PRECOMP

#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"

#define DEBUG_PAINT 0

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxMonthComboBox : public wxComboBox
{
public:
    wxMonthComboBox(wxCalendarCtrl *cal);

    void OnMonthChange(wxCommandEvent& event) { m_cal->OnMonthChange(event); }

private:
    wxCalendarCtrl *m_cal;

    DECLARE_EVENT_TABLE()
};

class wxYearSpinCtrl : public wxSpinCtrl
{
public:
    wxYearSpinCtrl(wxCalendarCtrl *cal);

    void OnYearChange(wxSpinEvent& event) { m_cal->OnYearChange(event); }

private:
    wxCalendarCtrl *m_cal;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxCalendarCtrl, wxControl)
    EVT_PAINT(wxCalendarCtrl::OnPaint)

    EVT_CHAR(wxCalendarCtrl::OnChar)

    EVT_LEFT_DOWN(wxCalendarCtrl::OnClick)
    EVT_LEFT_DCLICK(wxCalendarCtrl::OnDClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMonthComboBox, wxComboBox)
    EVT_COMBOBOX(-1, wxMonthComboBox::OnMonthChange)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxYearSpinCtrl, wxSpinCtrl)
    EVT_SPINCTRL(-1, wxYearSpinCtrl::OnYearChange)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxCalendarCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxCalendarEvent, wxCommandEvent)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMonthComboBox and wxYearSpinCtrl
// ----------------------------------------------------------------------------

wxMonthComboBox::wxMonthComboBox(wxCalendarCtrl *cal)
               : wxComboBox(cal->GetParent(), -1,
                            wxEmptyString,
                            wxDefaultPosition,
                            wxDefaultSize,
                            0, NULL,
                            wxCB_READONLY)
{
    m_cal = cal;

    wxDateTime::Month m;
    for ( m = wxDateTime::Jan; m < wxDateTime::Inv_Month; wxNextMonth(m) )
    {
        Append(wxDateTime::GetMonthName(m));
    }

    SetSelection(m_cal->GetDate().GetMonth());
    SetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH|wxSIZE_AUTO_HEIGHT);
}

wxYearSpinCtrl::wxYearSpinCtrl(wxCalendarCtrl *cal)
              : wxSpinCtrl(cal->GetParent(), -1,
                           cal->GetDate().Format(_T("%Y")),
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxSP_ARROW_KEYS,
                           -4300, 10000, cal->GetDate().GetYear())
{
    m_cal = cal;
}

// ----------------------------------------------------------------------------
// wxCalendarCtrl
// ----------------------------------------------------------------------------

void wxCalendarCtrl::Init()
{
    m_comboMonth = NULL;
    m_spinYear = NULL;

    m_widthCol =
    m_heightRow = 0;

    wxDateTime::WeekDay wd;
    for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
    {
        m_weekdays[wd] = wxDateTime::GetWeekDayName(wd, wxDateTime::Name_Abbr);
    }

    for ( size_t n = 0; n < WXSIZEOF(m_attrs); n++ )
    {
        m_attrs[n] = NULL;
    }

    wxSystemSettings ss;
    m_colHighlightFg = ss.GetSystemColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    m_colHighlightBg = ss.GetSystemColour(wxSYS_COLOUR_HIGHLIGHT);

    m_colHolidayFg = *wxRED;
    // don't set m_colHolidayBg - by default, same as our bg colour

    m_colHeaderFg = *wxBLUE;
    m_colHeaderBg = *wxLIGHT_GREY;
}

bool wxCalendarCtrl::Create(wxWindow * WXUNUSED(parent),
                            wxWindowID WXUNUSED(id),
                            const wxDateTime& date,
                            const wxPoint& WXUNUSED(pos),
                            const wxSize& size,
                            long style,
                            const wxString& WXUNUSED(name))
{
    // needed to get the arrow keys normally used for the dialog navigation
    SetWindowStyle(style | wxWANTS_CHARS);

    m_date = date.IsValid() ? date : wxDateTime::Today();

    m_spinYear = new wxYearSpinCtrl(this);
    m_staticYear = new wxStaticText(GetParent(), -1, m_date.Format(_T("%Y")),
                                    wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_CENTRE);

    m_comboMonth = new wxMonthComboBox(this);
    m_staticMonth = new wxStaticText(GetParent(), -1, m_date.Format(_T("%B")),
                                     wxDefaultPosition, wxDefaultSize,
                                     wxALIGN_CENTRE);

    ShowCurrentControls();

    wxSize sizeReal;
    if ( size.x == -1 || size.y == -1 )
    {
        sizeReal = DoGetBestSize();
        if ( size.x != -1 )
            sizeReal.x = size.x;
        if ( size.y != -1 )
            sizeReal.y = size.y;
    }
    else
    {
        sizeReal = size;
    }

    SetSize(sizeReal);

    SetBackgroundColour(*wxWHITE);
    SetFont(*wxSWISS_FONT);

    SetHolidayAttrs();

    return TRUE;
}

wxCalendarCtrl::~wxCalendarCtrl()
{
    for ( size_t n = 0; n < WXSIZEOF(m_attrs); n++ )
    {
        delete m_attrs[n];
    }
}

// ----------------------------------------------------------------------------
// forward wxWin functions to subcontrols
// ----------------------------------------------------------------------------

bool wxCalendarCtrl::Show(bool show)
{
    if ( !wxControl::Show(show) )
    {
        return FALSE;
    }

    GetMonthControl()->Show(show);
    GetYearControl()->Show(show);

    return TRUE;
}

bool wxCalendarCtrl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return FALSE;
    }

    GetMonthControl()->Enable(enable);
    GetYearControl()->Enable(enable);

    return TRUE;
}

// ----------------------------------------------------------------------------
// enable/disable month/year controls
// ----------------------------------------------------------------------------

void wxCalendarCtrl::ShowCurrentControls()
{
    if ( AllowMonthChange() )
    {
        m_comboMonth->Show();
        m_staticMonth->Hide();

        if ( AllowYearChange() )
        {
            m_spinYear->Show();
            m_staticYear->Hide();

            // skip the rest
            return;
        }
    }
    else
    {
        m_comboMonth->Hide();
        m_staticMonth->Show();
    }

    // year change not allowed here
    m_spinYear->Hide();
    m_staticYear->Show();
}

wxControl *wxCalendarCtrl::GetMonthControl() const
{
    return AllowMonthChange() ? (wxControl *)m_comboMonth : (wxControl *)m_staticMonth;
}

wxControl *wxCalendarCtrl::GetYearControl() const
{
    return AllowYearChange() ? (wxControl *)m_spinYear : (wxControl *)m_staticYear;
}

void wxCalendarCtrl::EnableYearChange(bool enable)
{
    if ( enable != AllowYearChange() )
    {
        long style = GetWindowStyle();
        if ( enable )
            style &= ~wxCAL_NO_YEAR_CHANGE;
        else
            style |= wxCAL_NO_YEAR_CHANGE;
        SetWindowStyle(style);

        ShowCurrentControls();
    }
}

void wxCalendarCtrl::EnableMonthChange(bool enable)
{
    if ( enable != AllowMonthChange() )
    {
        long style = GetWindowStyle();
        if ( enable )
            style &= ~wxCAL_NO_MONTH_CHANGE;
        else
            style |= wxCAL_NO_MONTH_CHANGE;
        SetWindowStyle(style);

        ShowCurrentControls();
    }
}

// ----------------------------------------------------------------------------
// changing date
// ----------------------------------------------------------------------------

void wxCalendarCtrl::SetDate(const wxDateTime& date)
{
    bool sameMonth = m_date.GetMonth() == date.GetMonth(),
         sameYear = m_date.GetYear() == date.GetYear();

    if ( sameMonth && sameYear )
    {
        // just change the day
        ChangeDay(date);
    }
    else
    {
        if ( !AllowMonthChange() || (!AllowYearChange() && !sameYear) )
        {
            // forbidden
            return;
        }

        // change everything
        m_date = date;

        // update the controls
        m_comboMonth->SetSelection(m_date.GetMonth());

        if ( AllowYearChange() )
        {
            m_spinYear->SetValue(m_date.Format(_T("%Y")));
        }

        // as the month changed, holidays did too
        SetHolidayAttrs();

        // update the calendar
        Refresh();
    }
}

void wxCalendarCtrl::ChangeDay(const wxDateTime& date)
{
    if ( m_date != date )
    {
        // we need to refresh the row containing the old date and the one
        // containing the new one
        wxDateTime dateOld = m_date;
        m_date = date;

        RefreshDate(dateOld);

        // if the date is in the same row, it was already drawn correctly
        if ( GetWeek(m_date) != GetWeek(dateOld) )
        {
            RefreshDate(m_date);
        }
    }
}

void wxCalendarCtrl::SetDateAndNotify(const wxDateTime& date)
{
    wxDateTime::Tm tm1 = m_date.GetTm(),
                   tm2 = date.GetTm();

    wxEventType type;
    if ( tm1.year != tm2.year )
        type = wxEVT_CALENDAR_YEAR_CHANGED;
    else if ( tm1.mon != tm2.mon )
        type = wxEVT_CALENDAR_MONTH_CHANGED;
    else if ( tm1.mday != tm2.mday )
        type = wxEVT_CALENDAR_DAY_CHANGED;
    else
        return;

    SetDate(date);

    GenerateEvents(type, wxEVT_CALENDAR_SEL_CHANGED);
}

// ----------------------------------------------------------------------------
// date helpers
// ----------------------------------------------------------------------------

wxDateTime wxCalendarCtrl::GetStartDate() const
{
    wxDateTime::Tm tm = m_date.GetTm();

    wxDateTime date = wxDateTime(1, tm.mon, tm.year);

    // rewind back
    date.SetToPrevWeekDay(GetWindowStyle() & wxCAL_MONDAY_FIRST
                          ? wxDateTime::Mon : wxDateTime::Sun);

    return date;
}

bool wxCalendarCtrl::IsDateShown(const wxDateTime& date) const
{
    return date.GetMonth() == m_date.GetMonth();
}

size_t wxCalendarCtrl::GetWeek(const wxDateTime& date) const
{
    return date.GetWeekOfMonth(GetWindowStyle() & wxCAL_MONDAY_FIRST
                               ? wxDateTime::Monday_First
                               : wxDateTime::Sunday_First);
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

// this is a composite control and it must arrange its parts each time its
// size or position changes: the combobox and spinctrl are along the top of
// the available area and the calendar takes up therest of the space

// the static controls are supposed to be always smaller than combo/spin so we
// always use the latter for size calculations and position the static to take
// the same space

// the constants used for the layout
#define VERT_MARGIN     5           // distance between combo and calendar
#define HORZ_MARGIN    15           //                            spin

wxSize wxCalendarCtrl::DoGetBestSize() const
{
    // calc the size of the calendar
    ((wxCalendarCtrl *)this)->RecalcGeometry(); // const_cast

    wxCoord width = 7*m_widthCol,
            height = 7*m_heightRow;

    wxSize sizeCombo = m_comboMonth->GetBestSize(),
           sizeSpin = m_spinYear->GetBestSize();

    height += VERT_MARGIN + wxMax(sizeCombo.y, sizeSpin.y);

    if ( GetWindowStyle() & (wxRAISED_BORDER | wxSUNKEN_BORDER) )
    {
        // the border would clip the last line otherwise
        height += 4;
    }

    return wxSize(width, height);
}

void wxCalendarCtrl::DoSetSize(int x, int y,
                               int width, int height,
                               int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);
}

void wxCalendarCtrl::DoMoveWindow(int x, int y, int width, int height)
{
    wxSize sizeCombo = m_comboMonth->GetSize();
    wxSize sizeStatic = m_staticMonth->GetSize();

    int dy = (sizeCombo.y - sizeStatic.y) / 2;
    m_comboMonth->Move(x, y);
    m_staticMonth->SetSize(x, y + dy, sizeCombo.x, sizeStatic.y);

    int xDiff = sizeCombo.x + HORZ_MARGIN;
    m_spinYear->SetSize(x + xDiff, y, width - xDiff, sizeCombo.y);
    m_staticYear->SetSize(x + xDiff, y + dy, width - xDiff, sizeStatic.y);

    wxSize sizeSpin = m_spinYear->GetSize();
    int yDiff = wxMax(sizeSpin.y, sizeCombo.y) + VERT_MARGIN;

    wxControl::DoMoveWindow(x, y + yDiff, width, height - yDiff);
}

void wxCalendarCtrl::DoGetPosition(int *x, int *y) const
{
    wxControl::DoGetPosition(x, y);

    // our real top corner is not in this position
    if ( y )
    {
        *y -= GetMonthControl()->GetSize().y + VERT_MARGIN;
    }
}

void wxCalendarCtrl::DoGetSize(int *width, int *height) const
{
    wxControl::DoGetSize(width, height);

    // our real height is bigger
    if ( height )
    {
        *height += GetMonthControl()->GetSize().y + VERT_MARGIN;
    }
}

void wxCalendarCtrl::RecalcGeometry()
{
    if ( m_widthCol != 0 )
        return;

    wxClientDC dc(this);

    dc.SetFont(m_font);

    // determine the column width (we assume that the weekday names are always
    // wider (in any language) than the numbers)
    m_widthCol = 0;
    wxDateTime::WeekDay wd;
    for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
    {
        wxCoord width;
        dc.GetTextExtent(m_weekdays[wd], &width, &m_heightRow);
        if ( width > m_widthCol )
        {
            m_widthCol = width;
        }
    }

    // leave some margins
    m_widthCol += 2;
    m_heightRow += 2;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    dc.SetFont(m_font);

    RecalcGeometry();

#if DEBUG_PAINT
    wxLogDebug("--- starting to paint, selection: %s, week %u\n",
           m_date.Format("%a %d-%m-%Y %H:%M:%S").c_str(),
           GetWeek(m_date));
#endif

    // first draw the week days
    if ( IsExposed(0, 0, 7*m_widthCol, m_heightRow) )
    {
#if DEBUG_PAINT
        wxLogDebug("painting the header");
#endif

        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.SetTextForeground(m_colHeaderFg);
        dc.SetBrush(wxBrush(m_colHeaderBg, wxSOLID));
        dc.SetPen(wxPen(m_colHeaderBg, 1, wxSOLID));
        dc.DrawRectangle(0, 0, 7*m_widthCol, m_heightRow);

        bool startOnMonday = (GetWindowStyle() & wxCAL_MONDAY_FIRST) != 0;
        for ( size_t wd = 0; wd < 7; wd++ )
        {
            size_t n;
            if ( startOnMonday )
                n = wd == 6 ? 0 : wd + 1;
            else
                n = wd;

            dc.DrawText(m_weekdays[n], wd*m_widthCol + 1, 0);
        }
    }

    // then the calendar itself
    dc.SetTextForeground(*wxBLACK);
    //dc.SetFont(*wxNORMAL_FONT);

    wxCoord y = m_heightRow;

    wxDateTime date = GetStartDate();
#if DEBUG_PAINT
    wxLogDebug("starting calendar from %s\n",
            date.Format("%a %d-%m-%Y %H:%M:%S").c_str());
#endif

    dc.SetBackgroundMode(wxSOLID);
    for ( size_t nWeek = 1; nWeek <= 6; nWeek++, y += m_heightRow )
    {
        // if the update region doesn't intersect this row, don't paint it
        if ( !IsExposed(0, y, 7*m_widthCol, m_heightRow - 1) )
        {
            date += wxDateSpan::Week();

            continue;
        }

#if DEBUG_PAINT
        wxLogDebug("painting week %d at y = %d\n", nWeek, y);
#endif

        for ( size_t wd = 0; wd < 7; wd++ )
        {
            if ( IsDateShown(date) )
            {
                // don't use wxDate::Format() which prepends 0s
                unsigned int day = date.GetDay();
                wxString dayStr = wxString::Format(_T("%u"), day);
                wxCoord width;
                dc.GetTextExtent(dayStr, &width, (wxCoord *)NULL);

                bool changedColours = FALSE,
                     changedFont = FALSE;

                wxCalendarDateAttr *attr = m_attrs[day - 1];

                bool isSel = m_date == date;
                if ( isSel )
                {
                    dc.SetTextForeground(m_colHighlightFg);
                    dc.SetTextBackground(m_colHighlightBg);

                    changedColours = TRUE;
                }
                else if ( attr )
                {
                    wxColour colFg, colBg;

                    if ( attr->IsHoliday() )
                    {
                        colFg = m_colHolidayFg;
                        colBg = m_colHolidayBg;
                    }
                    else
                    {
                        colFg = attr->GetTextColour();
                        colBg = attr->GetBackgroundColour();
                    }

                    if ( colFg.Ok() )
                    {
                        dc.SetTextForeground(colFg);
                        changedColours = TRUE;
                    }

                    if ( colBg.Ok() )
                    {
                        dc.SetTextBackground(colBg);
                        changedColours = TRUE;
                    }

                    if ( attr->HasFont() )
                    {
                        dc.SetFont(attr->GetFont());
                        changedFont = TRUE;
                    }
                }

                wxCoord x = wd*m_widthCol + (m_widthCol - width) / 2;
                dc.DrawText(dayStr, x, y + 1);

                if ( !isSel && attr && attr->HasBorder() )
                {
                    wxColour colBorder;
                    if ( attr->HasBorderColour() )
                    {
                        colBorder = attr->GetBorderColour();
                    }
                    else
                    {
                        colBorder = m_foregroundColour;
                    }

                    wxPen pen(colBorder, 1, wxSOLID);
                    dc.SetPen(pen);
                    dc.SetBrush(*wxTRANSPARENT_BRUSH);

                    switch ( attr->GetBorder() )
                    {
                        case wxCAL_BORDER_SQUARE:
                            dc.DrawRectangle(x - 2, y,
                                             width + 4, m_heightRow);
                            break;

                        case wxCAL_BORDER_ROUND:
                            dc.DrawEllipse(x - 2, y,
                                           width + 4, m_heightRow);
                            break;

                        default:
                            wxFAIL_MSG(_T("unknown border type"));
                    }
                }

                if ( changedColours )
                {
                    dc.SetTextForeground(m_foregroundColour);
                    dc.SetTextBackground(m_backgroundColour);
                }

                if ( changedFont )
                {
                    dc.SetFont(m_font);
                }
            }
            //else: just don't draw it

            date += wxDateSpan::Day();
        }
    }
#if DEBUG_PAINT
    wxLogDebug("+++ finished painting");
#endif
}

void wxCalendarCtrl::RefreshDate(const wxDateTime& date)
{
    RecalcGeometry();

    wxRect rect;

    // always refresh the whole row at once because our OnPaint() will draw
    // the whole row anyhow - and this allows the small optimisation in
    // OnClick() below to work
    rect.x = 0;
    rect.y = m_heightRow * GetWeek(date);
    rect.width = 7*m_widthCol;
    rect.height = m_heightRow;

#ifdef __WXMSW__
    // VZ: for some reason, the selected date seems to occupy more space under
    //     MSW - this is probably some bug in the font size calculations, but I
    //     don't know where exactly. This fix is ugly and leads to more
    //     refreshes than really needed, but without it the selected days
    //     leaves even more ugly underscores on screen.
    rect.Inflate(0, 1);
#endif // MSW

#if DEBUG_PAINT
    wxLogDebug("*** refreshing week %d at (%d, %d)-(%d, %d)\n",
           GetWeek(date),
           rect.x, rect.y,
           rect.x + rect.width, rect.y + rect.height);
#endif

    Refresh(TRUE, &rect);
}

// ----------------------------------------------------------------------------
// mouse handling
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnDClick(wxMouseEvent& event)
{
    if ( HitTest(event.GetPosition()) != wxCAL_HITTEST_DAY )
    {
        event.Skip();
    }
    else
    {
        GenerateEvent(wxEVT_CALENDAR_DOUBLECLICKED);
    }
}

void wxCalendarCtrl::OnClick(wxMouseEvent& event)
{
    wxDateTime date;
    wxDateTime::WeekDay wday;
    switch ( HitTest(event.GetPosition(), &date, &wday) )
    {
        case wxCAL_HITTEST_DAY:
            ChangeDay(date);

            GenerateEvents(wxEVT_CALENDAR_DAY_CHANGED,
                           wxEVT_CALENDAR_SEL_CHANGED);
            break;

        case wxCAL_HITTEST_HEADER:
            {
                wxCalendarEvent event(this, wxEVT_CALENDAR_WEEKDAY_CLICKED);
                event.m_wday = wday;
                (void)GetEventHandler()->ProcessEvent(event);
            }
            break;

        default:
            wxFAIL_MSG(_T("unknown hittest code"));
            // fall through

        case wxCAL_HITTEST_NOWHERE:
            event.Skip();
            break;
    }
}

wxCalendarHitTestResult wxCalendarCtrl::HitTest(const wxPoint& pos,
                                                wxDateTime *date,
                                                wxDateTime::WeekDay *wd)
{
    RecalcGeometry();

    int wday = pos.x / m_widthCol;

    wxCoord y = pos.y;
    if ( y < m_heightRow )
    {
        if ( wd )
        {
            if ( GetWindowStyle() & wxCAL_MONDAY_FIRST )
            {
                wday = wday == 6 ? 0 : wday + 1;
            }

            *wd = (wxDateTime::WeekDay)wday;
        }

        return wxCAL_HITTEST_HEADER;
    }

    int week = (y - m_heightRow) / m_heightRow;
    if ( week >= 6 || wday >= 7 )
    {
        return wxCAL_HITTEST_NOWHERE;
    }

    wxDateTime dt = GetStartDate() + wxDateSpan::Days(7*week + wday);

    if ( IsDateShown(dt) )
    {
        if ( date )
            *date = dt;

        return wxCAL_HITTEST_DAY;
    }
    else
    {
        return wxCAL_HITTEST_NOWHERE;
    }
}

// ----------------------------------------------------------------------------
// subcontrols events handling
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnMonthChange(wxCommandEvent& event)
{
    wxDateTime::Tm tm = m_date.GetTm();

    wxDateTime::Month mon = (wxDateTime::Month)event.GetInt();
    if ( tm.mday > wxDateTime::GetNumberOfDays(mon, tm.year) )
    {
        tm.mday = wxDateTime::GetNumberOfDays(mon, tm.year);
    }

    SetDateAndNotify(wxDateTime(tm.mday, mon, tm.year));
}

void wxCalendarCtrl::OnYearChange(wxSpinEvent& event)
{
    wxDateTime::Tm tm = m_date.GetTm();

    int year = (int)event.GetInt();
    if ( tm.mday > wxDateTime::GetNumberOfDays(tm.mon, year) )
    {
        tm.mday = wxDateTime::GetNumberOfDays(tm.mon, year);
    }

    SetDateAndNotify(wxDateTime(tm.mday, tm.mon, year));
}

// ----------------------------------------------------------------------------
// keyboard interface
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case _T('+'):
        case WXK_ADD:
            SetDateAndNotify(m_date + wxDateSpan::Year());
            break;

        case _T('-'):
        case WXK_SUBTRACT:
            SetDateAndNotify(m_date - wxDateSpan::Year());
            break;

        case WXK_PRIOR:
            SetDateAndNotify(m_date - wxDateSpan::Month());
            break;

        case WXK_NEXT:
            SetDateAndNotify(m_date + wxDateSpan::Month());
            break;

        case WXK_RIGHT:
            if ( event.ControlDown() )
                SetDateAndNotify(wxDateTime(m_date).SetToNextWeekDay(
                                 GetWindowStyle() & wxCAL_MONDAY_FIRST
                                 ? wxDateTime::Sun : wxDateTime::Sat));
            else
                SetDateAndNotify(m_date + wxDateSpan::Day());
            break;

        case WXK_LEFT:
            if ( event.ControlDown() )
                SetDateAndNotify(wxDateTime(m_date).SetToPrevWeekDay(
                                 GetWindowStyle() & wxCAL_MONDAY_FIRST
                                 ? wxDateTime::Mon : wxDateTime::Sun));
            else
                SetDateAndNotify(m_date - wxDateSpan::Day());
            break;

        case WXK_UP:
            SetDateAndNotify(m_date - wxDateSpan::Week());
            break;

        case WXK_DOWN:
            SetDateAndNotify(m_date + wxDateSpan::Week());
            break;

        case WXK_HOME:
            if ( event.ControlDown() )
                SetDateAndNotify(wxDateTime::Today());
            else
                SetDateAndNotify(wxDateTime(1, m_date.GetMonth(), m_date.GetYear()));
            break;

        case WXK_END:
            SetDateAndNotify(wxDateTime(m_date).SetToLastMonthDay());
            break;

        case WXK_RETURN:
            GenerateEvent(wxEVT_CALENDAR_DOUBLECLICKED);
            break;

        default:
            event.Skip();
    }
}

// ----------------------------------------------------------------------------
// holidays handling
// ----------------------------------------------------------------------------

void wxCalendarCtrl::EnableHolidayDisplay(bool display)
{
    long style = GetWindowStyle();
    if ( display )
        style |= wxCAL_SHOW_HOLIDAYS;
    else
        style &= ~wxCAL_SHOW_HOLIDAYS;

    SetWindowStyle(style);

    if ( display )
        SetHolidayAttrs();
    else
        ResetHolidayAttrs();

    Refresh();
}

void wxCalendarCtrl::SetHolidayAttrs()
{
    if ( GetWindowStyle() & wxCAL_SHOW_HOLIDAYS )
    {
        ResetHolidayAttrs();

        wxDateTime::Tm tm = m_date.GetTm();
        wxDateTime dtStart(1, tm.mon, tm.year),
                   dtEnd = dtStart.GetLastMonthDay();

        wxDateTimeArray hol;
        wxDateTimeHolidayAuthority::GetHolidaysInRange(dtStart, dtEnd, hol);

        size_t count = hol.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            SetHoliday(hol[n].GetDay());
        }
    }
}

void wxCalendarCtrl::SetHoliday(size_t day)
{
    wxCHECK_RET( day > 0 && day < 32, _T("invalid day in SetHoliday") );

    wxCalendarDateAttr *attr = GetAttr(day);
    if ( !attr )
    {
        attr = new wxCalendarDateAttr;
    }

    attr->SetHoliday(TRUE);

    // can't use SetAttr() because it would delete this pointer
    m_attrs[day - 1] = attr;
}

void wxCalendarCtrl::ResetHolidayAttrs()
{
    for ( size_t day = 0; day < 31; day++ )
    {
        if ( m_attrs[day] )
        {
            m_attrs[day]->SetHoliday(FALSE);
        }
    }
}

// ----------------------------------------------------------------------------
// wxCalendarEvent
// ----------------------------------------------------------------------------

void wxCalendarEvent::Init()
{
    m_wday = wxDateTime::Inv_WeekDay;
}

wxCalendarEvent::wxCalendarEvent(wxCalendarCtrl *cal, wxEventType type)
               : wxCommandEvent(type, cal->GetId())
{
    m_date = cal->GetDate();
}

#endif // wxUSE_CALENDARCTRL

