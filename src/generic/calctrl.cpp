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
#endif //WX_PRECOMP

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
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMonthComboBox, wxComboBox)
    EVT_COMBOBOX(-1, wxMonthComboBox::OnMonthChange)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxYearSpinCtrl, wxSpinCtrl)
    EVT_SPINCTRL(-1, wxYearSpinCtrl::OnYearChange)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxCalendarCtrl, wxControl)

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
}

bool wxCalendarCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    SetWindowStyle(style | (wxBORDER | wxWANTS_CHARS));

    m_date = date.IsValid() ? date : wxDateTime::Today();

    m_comboMonth = new wxMonthComboBox(this);
    m_spinYear = new wxYearSpinCtrl(this);

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

    return TRUE;
}

wxCalendarCtrl::~wxCalendarCtrl()
{
#if 0
    m_comboMonth->PopEventHandler();
    m_spinYear->PopEventHandler();
#endif // 0
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

    m_comboMonth->Show(show);
    m_spinYear->Show(show);

    return TRUE;
}

bool wxCalendarCtrl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return FALSE;
    }

    m_comboMonth->Enable(enable);
    m_spinYear->Enable(enable);

    return TRUE;
}

// ----------------------------------------------------------------------------
// changing date
// ----------------------------------------------------------------------------

void wxCalendarCtrl::SetDate(const wxDateTime& date)
{
    if ( m_date.GetMonth() == date.GetMonth() &&
         m_date.GetYear() == date.GetYear() )
    {
        // just change the day
        ChangeDay(date);
    }
    else
    {
        // change everything
        m_date = date;

        // update the controls
        m_comboMonth->SetSelection(m_date.GetMonth());
        m_spinYear->SetValue(m_date.Format(_T("%Y")));

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

    GenerateEvent(type);
}

// ----------------------------------------------------------------------------
// date helpers
// ----------------------------------------------------------------------------

wxDateTime wxCalendarCtrl::GetStartDate() const
{
    wxDateTime::Tm tm = m_date.GetTm();

    wxDateTime date = wxDateTime(1, tm.mon, tm.year);
    if ( date.GetWeekDay() != wxDateTime::Sun )
    {
        date.SetToPrevWeekDay(wxDateTime::Sun);

        // be sure to do it or it might gain 1 hour if DST changed in between
        date.ResetTime();
    }
    //else: we already have it

    return date;
}

bool wxCalendarCtrl::IsDateShown(const wxDateTime& date) const
{
    return date.GetMonth() == m_date.GetMonth();
}

size_t wxCalendarCtrl::GetWeek(const wxDateTime& date) const
{
    return date.GetWeekOfMonth(wxDateTime::Sunday_First);
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

// this is a composite control and it must arrange its parts each time its
// size or position changes: the combobox and spinctrl are along the top of
// the available area and the calendar takes up therest of the space

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
    m_comboMonth->Move(x, y);

    int xDiff = sizeCombo.x + HORZ_MARGIN;
    m_spinYear->SetSize(x + xDiff, y, width - xDiff, sizeCombo.y);

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
        *y -= m_comboMonth->GetSize().y + VERT_MARGIN;
    }
}

void wxCalendarCtrl::DoGetSize(int *width, int *height) const
{
    wxControl::DoGetSize(width, height);

    // our real height is bigger
    if ( height )
    {
        *height += m_comboMonth->GetSize().y + VERT_MARGIN;
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

void wxCalendarCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    wxDateTime::WeekDay wd;

    dc.SetFont(m_font);

    RecalcGeometry();

#if DEBUG_PAINT
    printf("--- starting to paint, selection: %s, week %u\n",
           m_date.Format("%a %d-%m-%Y %H:%M:%S").c_str(),
           GetWeek(m_date));
#endif

    // first draw the week days
    if ( IsExposed(0, 0, 7*m_widthCol, m_heightRow) )
    {
#if DEBUG_PAINT
        puts("painting the header");
#endif

        dc.SetTextForeground(*wxBLUE);
        dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawRectangle(0, 0, 7*m_widthCol, m_heightRow);
        for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
        {
            dc.DrawText(m_weekdays[wd], wd*m_widthCol + 1, 0);
        }
    }

    // then the calendar itself
    dc.SetTextForeground(*wxBLACK);
    //dc.SetFont(*wxNORMAL_FONT);

    wxCoord y = m_heightRow;

    wxDateTime date = GetStartDate();
#if DEBUG_PAINT
    printf("starting calendar from %s\n",
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
        printf("painting week %d at y = %d\n", nWeek, y);
#endif

        for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
        {
            if ( IsDateShown(date) )
            {
                // don't use wxDate::Format() which prepends 0s
                wxString day = wxString::Format(_T("%u"), date.GetDay());
                wxCoord width;
                dc.GetTextExtent(day, &width, (wxCoord *)NULL);

                bool isSel = m_date == date;
                if ( isSel )
                {
                    dc.SetTextForeground(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
                    dc.SetTextBackground(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT));
                }

                dc.DrawText(day, wd*m_widthCol + (m_widthCol - width) / 2, y);

                if ( isSel )
                {
                    dc.SetTextForeground(m_foregroundColour);
                    dc.SetTextBackground(m_backgroundColour);
                }
            }
            //else: just don't draw it

            date += wxDateSpan::Day();
        }
    }
#if DEBUG_PAINT
    puts("+++ finished painting");
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

#if DEBUG_PAINT
    printf("*** refreshing week %d at (%d, %d)-(%d, %d)\n",
           GetWeek(date),
           rect.x, rect.y,
           rect.x + rect.width, rect.y + rect.height);
#endif

    Refresh(TRUE, &rect);
}

// ----------------------------------------------------------------------------
// mouse handling
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnClick(wxMouseEvent& event)
{
    RecalcGeometry();

    wxDateTime date;
    if ( !HitTest(event.GetPosition(), &date) )
    {
        event.Skip();
    }
    else
    {
        ChangeDay(date);

        GenerateEvent(wxEVT_CALENDAR_DAY_CHANGED);
    }
}

bool wxCalendarCtrl::HitTest(const wxPoint& pos, wxDateTime *date)
{
    RecalcGeometry();

    wxCoord y = pos.y;
    if ( y < m_heightRow )
        return FALSE;

    y -= m_heightRow;
    int week = y / m_heightRow,
        wday = pos.x / m_widthCol;

    if ( week >= 6 || wday >= 7 )
        return FALSE;

    wxCHECK_MSG( date, FALSE, _T("bad pointer in wxCalendarCtrl::HitTest") );

    *date = GetStartDate();
    *date += wxDateSpan::Days(7*week + wday);

    return IsDateShown(*date);
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

    SetDate(wxDateTime(tm.mday, mon, tm.year));

    GenerateEvent(wxEVT_CALENDAR_MONTH_CHANGED);
}

void wxCalendarCtrl::OnYearChange(wxSpinEvent& event)
{
    wxDateTime::Tm tm = m_date.GetTm();

    int year = event.GetInt();
    if ( tm.mday > wxDateTime::GetNumberOfDays(tm.mon, year) )
    {
        tm.mday = wxDateTime::GetNumberOfDays(tm.mon, year);
    }

    SetDate(wxDateTime(tm.mday, tm.mon, year));

    GenerateEvent(wxEVT_CALENDAR_YEAR_CHANGED);
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
            SetDateAndNotify(m_date + wxDateSpan::Day());
            break;

        case WXK_LEFT:
            SetDateAndNotify(m_date - wxDateSpan::Day());
            break;

        case WXK_UP:
            SetDateAndNotify(m_date - wxDateSpan::Week());
            break;

        case WXK_DOWN:
            SetDateAndNotify(m_date + wxDateSpan::Week());
            break;

        case WXK_HOME:
            SetDateAndNotify(wxDateTime::Today());
            break;

        default:
            event.Skip();
    }
}

// ----------------------------------------------------------------------------
// wxCalendarEvent
// ----------------------------------------------------------------------------

void wxCalendarCtrl::GenerateEvent(wxEventType type)
{
    // we're called for a change in some particular date field but we always
    // also generate a generic "changed" event
    wxCalendarEvent event(this, type);
    wxCalendarEvent event2(this, wxEVT_CALENDAR_SEL_CHANGED);

    (void)GetEventHandler()->ProcessEvent(event);
    (void)GetEventHandler()->ProcessEvent(event2);
}

wxCalendarEvent::wxCalendarEvent(wxCalendarCtrl *cal, wxEventType type)
               : wxCommandEvent(type, cal->GetId())
{
    m_date = cal->GetDate();
}
