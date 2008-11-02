/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/calctrl.cpp
// Purpose:     wxCalendarCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2008-04-04
// RCS-ID:      $Id$
// Copyright:   (C) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CALENDARCTRL

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/msw/private.h"
#endif

#include "wx/calctrl.h"

#include "wx/msw/private/datecontrols.h"

IMPLEMENT_DYNAMIC_CLASS(wxCalendarCtrl, wxControl)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCalendarCtrl creation
// ----------------------------------------------------------------------------

void wxCalendarCtrl::Init()
{
    m_marks =
    m_holidays = 0;
}

bool
wxCalendarCtrl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxDateTime& dt,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    if ( !wxMSWDateControls::CheckInitialization() )
        return false;

    // we need the arrows for the navigation
    style |= wxWANTS_CHARS;

    // initialize the base class
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    // create the native control: this is a bit tricky as we want to receive
    // double click events but the MONTHCAL_CLASS doesn't use CS_DBLCLKS style
    // and so we create our own copy of it which does
    static ClassRegistrar s_clsMonthCal;
    if ( !s_clsMonthCal.IsInitialized() )
    {
        // get a copy of standard class and modify it
        WNDCLASS wc;
        if ( ::GetClassInfo(NULL, MONTHCAL_CLASS, &wc) )
        {
            wc.lpszClassName = wxT("_wx_SysMonthCtl32");
            wc.style |= CS_DBLCLKS;
            s_clsMonthCal.Register(wc);
        }
        else
        {
            wxLogLastError(_T("GetClassInfoEx(SysMonthCal32)"));
        }
    }

    const wxChar * const clsname = s_clsMonthCal.IsRegistered()
        ? s_clsMonthCal.GetName().wx_str()
        : MONTHCAL_CLASS;

    if ( !MSWCreateControl(clsname, wxEmptyString, pos, size) )
        return false;

    // initialize the control 
    UpdateFirstDayOfWeek();

    SetDate(dt.IsValid() ? dt : wxDateTime::Today());

    if ( SetHolidayAttrs() )
        UpdateMarks();

    Connect(wxEVT_LEFT_DOWN,
            wxMouseEventHandler(wxCalendarCtrl::MSWOnClick));
    Connect(wxEVT_LEFT_DCLICK,
            wxMouseEventHandler(wxCalendarCtrl::MSWOnDoubleClick));

    return true;
}

WXDWORD wxCalendarCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD styleMSW = wxCalendarCtrlBase::MSWGetStyle(style, exstyle);

    // right now we don't support all native styles but we should add wx styles
    // corresponding to MCS_NOTODAY and MCS_NOTODAYCIRCLE probably (TODO)

    // for compatibility with the other versions, just turn off today display
    // unconditionally for now
    styleMSW |= MCS_NOTODAY;

    // we also need this style for Mark() to work
    styleMSW |= MCS_DAYSTATE;

    if ( style & wxCAL_SHOW_WEEK_NUMBERS )
       styleMSW |= MCS_WEEKNUMBERS;

    return styleMSW;
}

void wxCalendarCtrl::SetWindowStyleFlag(long style)
{
    const bool hadMondayFirst = HasFlag(wxCAL_MONDAY_FIRST);

    wxCalendarCtrlBase::SetWindowStyleFlag(style);

    if ( HasFlag(wxCAL_MONDAY_FIRST) != hadMondayFirst )
        UpdateFirstDayOfWeek();
}

// ----------------------------------------------------------------------------
// wxCalendarCtrl geometry
// ----------------------------------------------------------------------------

// TODO: handle WM_WININICHANGE
wxSize wxCalendarCtrl::DoGetBestSize() const
{
    RECT rc;
    if ( !GetHwnd() || !MonthCal_GetMinReqRect(GetHwnd(), &rc) )
    {
        return wxCalendarCtrlBase::DoGetBestSize();
    }

    const wxSize best = wxRectFromRECT(rc).GetSize() + GetWindowBorderSize();
    CacheBestSize(best);
    return best;
}

wxCalendarHitTestResult
wxCalendarCtrl::HitTest(const wxPoint& pos,
                        wxDateTime *date,
                        wxDateTime::WeekDay *wd)
{
    WinStruct<MCHITTESTINFO> hti;
    hti.pt.x = pos.x;
    hti.pt.y = pos.y;
    switch ( MonthCal_HitTest(GetHwnd(), &hti) )
    {
        default:
        case MCHT_CALENDARWEEKNUM:
            wxFAIL_MSG( "unexpected" );
            // fall through

        case MCHT_NOWHERE:
        case MCHT_CALENDARBK:
        case MCHT_TITLEBK:
        case MCHT_TITLEMONTH:
        case MCHT_TITLEYEAR:
            return wxCAL_HITTEST_NOWHERE;

        case MCHT_CALENDARDATE:
            if ( date )
                date->SetFromMSWSysTime(hti.st);
            return wxCAL_HITTEST_DAY;

        case MCHT_CALENDARDAY:
            if ( wd )
            {
                *wd = static_cast<wxDateTime::WeekDay>(hti.st.wDayOfWeek);
            }
            return wxCAL_HITTEST_HEADER;

        case MCHT_TITLEBTNNEXT:
            return wxCAL_HITTEST_INCMONTH;

        case MCHT_TITLEBTNPREV:
            return wxCAL_HITTEST_DECMONTH;

        case MCHT_CALENDARDATENEXT:
        case MCHT_CALENDARDATEPREV:
            return wxCAL_HITTEST_SURROUNDING_WEEK;
    }
}

// ----------------------------------------------------------------------------
// wxCalendarCtrl operations
// ----------------------------------------------------------------------------

bool wxCalendarCtrl::SetDate(const wxDateTime& dt)
{
    wxCHECK_MSG( dt.IsValid(), false, "invalid date" );

    SYSTEMTIME st;
    dt.GetAsMSWSysTime(&st);
    if ( !MonthCal_SetCurSel(GetHwnd(), &st) )
    {
        wxLogDebug(_T("DateTime_SetSystemtime() failed"));

        return false;
    }

    m_date = dt;

    return true;
}

wxDateTime wxCalendarCtrl::GetDate() const
{
#ifdef __WXDEBUG__
    SYSTEMTIME st;
    if ( !MonthCal_GetCurSel(GetHwnd(), &st) )
    {
        wxASSERT_MSG( !m_date.IsValid(), "mismatch between data and control" );

        return wxDefaultDateTime;
    }

    wxDateTime dt(st);

    wxASSERT_MSG( dt == m_date, "mismatch between data and control" );
#endif // __WXDEBUG__

    return m_date;
}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    SYSTEMTIME st[2];

    DWORD flags = 0;
    if ( dt1.IsValid() )
    {
        dt1.GetAsMSWSysTime(st + 0);
        flags |= GDTR_MIN;
    }

    if ( dt2.IsValid() )
    {
        dt2.GetAsMSWSysTime(st + 1);
        flags |= GDTR_MAX;
    }

    if ( !MonthCal_SetRange(GetHwnd(), flags, st) )
    {
        wxLogDebug(_T("MonthCal_SetRange() failed"));
    }

    return flags != 0;
}

bool wxCalendarCtrl::GetDateRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    SYSTEMTIME st[2];

    DWORD flags = MonthCal_GetRange(GetHwnd(), st);
    if ( dt1 )
    {
        if ( flags & GDTR_MIN )
            dt1->SetFromMSWSysTime(st[0]);
        else
            *dt1 = wxDefaultDateTime;
    }

    if ( dt2 )
    {
        if ( flags & GDTR_MAX )
            dt2->SetFromMSWSysTime(st[1]);
        else
            *dt2 = wxDefaultDateTime;
    }

    return flags != 0;
}

// ----------------------------------------------------------------------------
// other wxCalendarCtrl operations
// ----------------------------------------------------------------------------

bool wxCalendarCtrl::EnableMonthChange(bool enable)
{
    if ( !wxCalendarCtrlBase::EnableMonthChange(enable) )
        return false;

    wxDateTime dtStart, dtEnd;
    if ( !enable )
    {
        dtStart = GetDate();
        dtStart.SetDay(1);

        dtEnd = dtStart.GetLastMonthDay();
    }
    //else: leave them invalid to remove the restriction

    SetDateRange(dtStart, dtEnd);

    return true;
}

void wxCalendarCtrl::Mark(size_t day, bool mark)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    int mask = 1 << (day - 1);
    if ( mark )
        m_marks |= mask;
    else
        m_marks &= ~mask;

    // calling Refresh() here is not enough to change the day appearance
    UpdateMarks();
}

void wxCalendarCtrl::SetHoliday(size_t day)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    m_holidays |= 1 << (day - 1);
}

void wxCalendarCtrl::UpdateMarks()
{
    // we show only one full month but there can be some days from the month
    // before it and from the one after it so days from 3 different months can
    // be partially shown
    MONTHDAYSTATE states[3] = { 0 };
    const int nMonths = MonthCal_GetMonthRange(GetHwnd(), GMR_DAYSTATE, NULL);

    // although in principle the calendar might not show any days from the
    // preceding months, it seems like it always does, consider e.g. Feb 2010
    // which starts on Monday and ends on Sunday and so could fit on 4 lines
    // without showing any subsequent months -- the standard control still
    // shows it on 6 lines and the number of visible months is still 3
    wxCHECK_RET( nMonths == (int)WXSIZEOF(states), "unexpected months range" );

    // the fully visible month is the one in the middle
    states[1] = m_marks | m_holidays;

    if ( !MonthCal_SetDayState(GetHwnd(), nMonths, states) )
    {
        wxLogLastError(_T("MonthCal_SetDayState"));
    }
}

void wxCalendarCtrl::UpdateFirstDayOfWeek()
{
    MonthCal_SetFirstDayOfWeek(GetHwnd(), HasFlag(wxCAL_MONDAY_FIRST) ? 0 : 6);
}

// ----------------------------------------------------------------------------
// wxCalendarCtrl events
// ----------------------------------------------------------------------------

bool wxCalendarCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR* hdr = (NMHDR *)lParam;
    switch ( hdr->code )
    {
        case MCN_SELCHANGE:
            {
                // we need to update m_date first, before calling the user code
                // which expects GetDate() to return the new date
                const wxDateTime dateOld = m_date;
                const NMSELCHANGE * const sch = (NMSELCHANGE *)lParam;
                m_date.SetFromMSWSysTime(sch->stSelStart);

                // changing the year or the month results in a second dummy
                // MCN_SELCHANGE event on this system which doesn't really
                // change anything -- filter it out
                if ( m_date != dateOld )
                {
                    if ( GenerateAllChangeEvents(dateOld) )
                    {
                        // month changed, need to update the holidays if we use
                        // them
                        if ( SetHolidayAttrs() )
                            UpdateMarks();
                    }
                }
            }
            break;

        case MCN_GETDAYSTATE:
            {
                const NMDAYSTATE * const ds = (NMDAYSTATE *)lParam;
                for ( int i = 0; i < ds->cDayState; i++ )
                {
                    ds->prgDayState[i] = m_marks | m_holidays;
                }
            }
            break;

        default:
            return wxCalendarCtrlBase::MSWOnNotify(idCtrl, lParam, result);
    }

    *result = 0;
    return true;
}

void wxCalendarCtrl::MSWOnDoubleClick(wxMouseEvent& event)
{
    if ( HitTest(event.GetPosition()) == wxCAL_HITTEST_DAY )
    {
        if ( GenerateEvent(wxEVT_CALENDAR_DOUBLECLICKED) )
            return; // skip event.Skip() below
    }

    event.Skip();
}

void wxCalendarCtrl::MSWOnClick(wxMouseEvent& event)
{
    // for some reason, the control doesn't get focus on its own when the user
    // clicks in it
    SetFocus();

    event.Skip();
}

#endif // wxUSE_CALENDARCTRL
