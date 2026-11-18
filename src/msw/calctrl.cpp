/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/calctrl.cpp
// Purpose:     wxCalendarCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2008-04-04
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


#if wxUSE_CALENDARCTRL

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/msw/private.h"
#endif

#include "wx/calctrl.h"

#include "wx/msw/private/datecontrols.h"
#include "wx/msw/private/custompaint.h"
#include "wx/msw/private/darkmode.h"
#include "wx/renderer.h"
#include <uxtheme.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

// values of week days used by the native control
enum
{
    MonthCal_Monday,
    MonthCal_Tuesday,
    MonthCal_Wednesday,
    MonthCal_Thursday,
    MonthCal_Friday,
    MonthCal_Saturday,
    MonthCal_Sunday
};

} // anonymous namespace

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
        if ( ::GetClassInfo(nullptr, MONTHCAL_CLASS, &wc) )
        {
            wc.lpszClassName = wxT("_wx_SysMonthCtl32");
            wc.style |= CS_DBLCLKS;
            s_clsMonthCal.Register(wc);
        }
        else
        {
            wxLogLastError(wxT("GetClassInfoEx(SysMonthCal32)"));
        }
    }

    const wxChar * const clsname = s_clsMonthCal.IsRegistered()
        ? static_cast<const wxChar*>(s_clsMonthCal.GetName().t_str())
        : MONTHCAL_CLASS;

    if ( !MSWCreateControl(clsname, wxEmptyString, pos, size) )
        return false;

    // initialize the control
    UpdateFirstDayOfWeek();

    SetDate(dt.IsValid() ? dt : wxDateTime::Today());

    SetHolidayAttrs();
    UpdateMarks();

    Bind(wxEVT_LEFT_DOWN, &wxCalendarCtrl::MSWOnClick, this);
    Bind(wxEVT_LEFT_DCLICK, &wxCalendarCtrl::MSWOnDoubleClick, this);
    Bind(wxEVT_MOTION, &wxCalendarCtrl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW , &wxCalendarCtrl::OnMouseLeave, this);

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
    const bool hadMondayFirst = WeekStartsOnMonday();

    wxCalendarCtrlBase::SetWindowStyleFlag(style);

    if ( WeekStartsOnMonday() != hadMondayFirst )
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

    return wxRectFromRECT(rc).GetSize() + GetWindowBorderSize();
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
            wxFALLTHROUGH;

        case MCHT_NOWHERE:
        case MCHT_CALENDARBK:
        case MCHT_TITLEBK:
        case MCHT_TITLEMONTH:
        case MCHT_TITLEYEAR:
            return wxCAL_HITTEST_NOWHERE;

        case MCHT_CALENDARDATE:
            if ( date )
                date->SetFromMSWSysDate(hti.st);
            return wxCAL_HITTEST_DAY;

        case MCHT_CALENDARDAY:
            if ( wd )
            {
                int day = hti.st.wDayOfWeek;

                // the native control returns incorrect day of the week when
                // the first day isn't Monday, i.e. the first column is always
                // "Monday" even if its label is "Sunday", compensate for it
                const int first = LOWORD(MonthCal_GetFirstDayOfWeek(GetHwnd()));
                if ( first == MonthCal_Monday )
                {
                    // as MonthCal_Monday is 0 while wxDateTime::Mon is 1,
                    // normally we need to do this to transform from MSW
                    // convention to wx one
                    day++;
                    day %= 7;
                }
                //else: but when the first day is MonthCal_Sunday, the native
                //      control still returns 0 (i.e. MonthCal_Monday) for the
                //      first column which looks like a bug in it but to work
                //      around it it's enough to not apply the correction above

                *wd = static_cast<wxDateTime::WeekDay>(day);
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
    dt.GetAsMSWSysDate(&st);
    if ( !MonthCal_SetCurSel(GetHwnd(), &st) )
    {
        wxLogDebug(wxT("DateTime_SetSystemtime() failed"));

        return false;
    }

    m_date = dt.GetDateOnly();

    return true;
}

wxDateTime wxCalendarCtrl::GetDate() const
{
#if wxDEBUG_LEVEL
    SYSTEMTIME st;

    if ( !MonthCal_GetCurSel(GetHwnd(), &st) )
    {
        wxASSERT_MSG( !m_date.IsValid(), "mismatch between data and control" );

        return wxDefaultDateTime;
    }

    wxDateTime dt;
    dt.SetFromMSWSysDate(st);

    // Windows XP and earlier didn't include the time component into the
    // returned date but Windows 7 does, so we can't compare the full objects
    // in the same way under all the Windows versions, just compare their date
    // parts
    wxASSERT_MSG( dt.IsSameDate(m_date), "mismatch between data and control" );
#endif // wxDEBUG_LEVEL

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
        wxLogDebug(wxT("MonthCal_SetRange() failed"));
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
            dt1->SetFromMSWSysDate(st[0]);
        else
            *dt1 = wxDefaultDateTime;
    }

    if ( dt2 )
    {
        if ( flags & GDTR_MAX )
            dt2->SetFromMSWSysDate(st[1]);
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
    // Currently the native control may show more than one month if its size is
    // big enough. Ideal would be to prevent this from happening but there
    // doesn't seem to be any obvious way to do it, so for now just handle the
    // possibility that we can display several of them: one before the current
    // one and up to 12 after it.
    MONTHDAYSTATE states[14] = { 0 };
    const DWORD nMonths = MonthCal_GetMonthRange(GetHwnd(), GMR_DAYSTATE, nullptr);

    // although in principle the calendar might not show any days from the
    // preceding months, it seems like it always does, consider e.g. Feb 2010
    // which starts on Monday and ends on Sunday and so could fit on 4 lines
    // without showing any subsequent months -- the standard control still
    // shows it on 6 lines and the number of visible months is still 3
    //
    // OTOH Windows 7 control can show all 12 months or even years or decades
    // in its window if you "zoom out" of it by double clicking on free areas
    // so the return value can be (much, in case of decades view) greater than
    // 3 but in this case marks are not visible anyhow so simply ignore it
    if ( nMonths >= 2 && nMonths <= WXSIZEOF(states) )
    {
        // The current, fully visible month is always the second one.
        states[1] = m_marks | m_holidays;

        if ( !MonthCal_SetDayState(GetHwnd(), nMonths, states) )
        {
            wxLogLastError(wxT("MonthCal_SetDayState"));
        }
    }
    //else: not a month view at all
}

void wxCalendarCtrl::UpdateFirstDayOfWeek()
{
    MonthCal_SetFirstDayOfWeek(GetHwnd(),
                               WeekStartsOnMonday() ? MonthCal_Monday
                                                           : MonthCal_Sunday);
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
                m_date.SetFromMSWSysDate(sch->stSelStart);

                // changing the year or the month results in a second dummy
                // MCN_SELCHANGE event on this system which doesn't really
                // change anything -- filter it out
                if ( m_date != dateOld )
                {
                    if ( GenerateAllChangeEvents(dateOld) )
                    {
                        // month changed, need to update the holidays if we use
                        // them
                        SetHolidayAttrs();
                        UpdateMarks();
                    }
                }
            }
            break;

        case MCN_GETDAYSTATE:
            {
                const NMDAYSTATE * const ds = (NMDAYSTATE *)lParam;

                wxDateTime startDate;
                startDate.SetFromMSWSysDate(ds->stStart);

                // Ensure we have a valid date to work with.
                wxDateTime currentDate = m_date.IsValid() ? m_date : startDate;

                // Set to the start of month for comparison with startDate to
                // work correctly.
                currentDate.SetDay(1);

                for ( int i = 0; i < ds->cDayState; i++ )
                {
                    // set holiday/marks only for the "current" month
                    if ( startDate == currentDate )
                        ds->prgDayState[i] = m_marks | m_holidays;
                    else
                        ds->prgDayState[i] = 0;

                    startDate += wxDateSpan::Month();
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

void wxCalendarCtrl::OnMouseMove(wxMouseEvent& event)
{
    if ( wxMSWDarkMode::IsActive() )
    {
        // Get mouse position as cell column and row.
        WinStruct<MCHITTESTINFO> ht;
        ht.pt.x = event.GetPosition().x;
        ht.pt.y = event.GetPosition().y;
        ::SendMessage(m_hWnd, MCM_HITTEST, 0, (LPARAM)&ht);
        if ( ht.uHit == MCHT_CALENDARDATE )
        {
            // The mouse is over the body. If it changed cells, repaint.
            if ( ht.iOffset != m_msCal || ht.iCol != m_msCol || ht.iRow != m_msRow )
                Refresh(false);
            m_msCal = ht.iOffset;
            m_msCol = ht.iCol;
            m_msRow = ht.iRow;
        }
        else
        {
            // The mouse is outside the body. If previously inside, repaint.
            if ( m_msCal != -1 )
                Refresh(false);
            m_msCal = -1;
        }
    }
    event.Skip();
}

void wxCalendarCtrl::OnMouseLeave(wxMouseEvent& event)
{
    if ( wxMSWDarkMode::IsActive() )
    {
        // If the mouse was previously inside the body, repaint.
        if ( m_msCal != -1 )
            Refresh(false);
        m_msCal = -1;
    }
    event.Skip();
}

void wxCalendarCtrl::MSWSetDarkOrLightMode()
{
    wxCalendarCtrlBase::MSWSetDarkOrLightMode();

    auto bg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetPixel();
    if ( wxMSWDarkMode::IsActive() )
    {
        // Disable visual themes so that MCM_SETCOLOR takes effect.
        ::SetWindowTheme(m_hWnd, L"", L"");
        auto fg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetPixel();
        ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_MONTHBK, bg);
        ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_TEXT, fg);
        ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_TITLETEXT, fg);
        auto grayText = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT).GetPixel();
        ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_TRAILINGTEXT, grayText);
        // Unfortunately, MCSC_TITLEBK specifies both a background and text
        // colour. It controls the text for the days of the week, the week
        // numbers, the background of the header and the background of the
        // selected cell. We redraw those background areas ourselves.
        ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_TITLEBK, fg);
    }
    // The MCSC_BACKGROUND setting takes affect both with and without visual
    // themes.
    ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_BACKGROUND, bg);
}

WXLRESULT wxCalendarCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // In dark mode, redraw the header to make the background dark and to
    // improve the appearance of the buttons. The control draws light-mode
    // buttons.
    if ( nMsg == WM_PAINT && wxMSWDarkMode::IsActive() )
    {
        // Paint the control into a memory DC, specified by wParam.
        auto defPaint = [this](HWND WXUNUSED(hwnd), WPARAM wParam) {
            // For the decade and century views, make the text colour
            // consistent. In these views, the native control uses the
            // gray MCSC_TRAILINGTEXT colour in odd places. This is not
            // noticeable in light mode but it looks weird in dark mode.
            auto view = ::SendMessage(m_hWnd, MCM_GETCURRENTVIEW, 0, 0);
            if ( view == MCMV_DECADE || view == MCMV_CENTURY )
            {
                // Save MCSC_TRAILINGTEXT, then set it to MCSC_TEXT (white).
                auto tt = ::SendMessage(m_hWnd, MCM_GETCOLOR, MCSC_TRAILINGTEXT, 0);
                auto fg = ::SendMessage(m_hWnd, MCM_GETCOLOR, MCSC_TEXT, 0);
                ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_TRAILINGTEXT, fg);
                // Paint.
                 MSWDefWindowProc(WM_PAINT, wParam, 0);
                // Restore MCSC_TRAILINGTEXT.
                ::SendMessage(m_hWnd, MCM_SETCOLOR, MCSC_TRAILINGTEXT, tt);
            }
            else
                MSWDefWindowProc(WM_PAINT, wParam, 0);
        };
        auto postProcess = [this](const wxBitmap& bmpIn) {
            return MSWPaintDarkMode(bmpIn);
        };
        wxMSWImpl::CustomPaint(m_hWnd, defPaint, postProcess);
        return 0;
    }

    return wxCalendarCtrlBase::MSWWindowProc(nMsg, wParam, lParam);
}

wxBitmap wxCalendarCtrl::MSWPaintDarkMode(const wxBitmap& bmpIn)
{
    wxBitmap bmpOut(bmpIn);
    bmpOut.SetScaleFactor(GetDPIScaleFactor());
    wxMemoryDC dc(bmpOut);
    dc.SetFont(GetFont());
    dc.SetLayoutDirection(GetLayoutDirection());

    // Initialize MCGRIDINFO with text buffer.
    WinStruct<MCGRIDINFO> info;
    wchar_t text[128] = { };
    info.pszName = text;
    info.cchName = WXSIZEOF(text);
    wxString textPrevious;

    // Loop over each calendar.
    int nCal = ::SendMessage(m_hWnd, MCM_GETCALENDARCOUNT, 0, 0);
    for ( info.iCalendar = 0; info.iCalendar < nCal; info.iCalendar++ )
    {
        // Erase the header to dark background instead of the MCSC_TITLEBK
        // colour, which is white.
        info.dwPart = MCGIP_CALENDARHEADER;
        info.dwFlags = MCGIF_NAME | MCGIF_RECT;
        ::SendMessage(m_hWnd, MCM_GETCALENDARGRIDINFO, 0, (LPARAM)&info);
        COLORREF bg = ::SendMessage(m_hWnd, MCM_GETCOLOR, MCSC_BACKGROUND, 0);
        dc.SetBrush(wxBrush(wxColour(bg)));
        dc.SetPen(*wxTRANSPARENT_PEN);
        wxRect rcHead = wxRectFromRECT(info.rc);
        dc.DrawRectangle(rcHead);

        // If the heading text does not change, this calendar is past the end
        // of the date range.
        if ( textPrevious == text )
            continue;
        textPrevious = text;

        // Redraw header text.
        COLORREF fg = ::SendMessage(m_hWnd, MCM_GETCOLOR, MCSC_TITLETEXT, 0);
        dc.SetTextForeground(wxColour(fg));
        dc.DrawLabel(text, rcHead, wxALIGN_CENTER);

        // The rest of this loop redraws the selected cell and the hot cell.

        // Iterate over all the cells.
        info.dwPart = MCGIP_CALENDARCELL;
        info.dwFlags = MCGIF_DATE | MCGIF_NAME | MCGIF_RECT;
        bool isMonthView = ::SendMessage(m_hWnd, MCM_GETCURRENTVIEW, 0, 0) ==
            MCMV_MONTH;
        int nCol = isMonthView ? 7 : 4;
        int nRow = isMonthView ? 6 : 3;
        for ( info.iRow = 0; info.iRow < nRow; info.iRow++ )
        {
            for ( info.iCol = 0; info.iCol < nCol; info.iCol++ )
            {
                // Get cell info.
                text[0] = L'\0';
                ::SendMessage(m_hWnd, MCM_GETCALENDARGRIDINFO, 0, (LPARAM)&info);

                // If the cell is empty, skip it. It is outside the date range.
                if ( text[0] == L'\0' )
                    continue;

                bool isHot = (info.iCalendar == m_msCal &&
                    info.iCol == m_msCol && info.iRow == m_msRow);
                int flags = isHot ? wxCONTROL_CURRENT : 0;
                if ( info.bSelected )
                    flags = wxCONTROL_SELECTED | wxCONTROL_FOCUSED;

                // If the cell is either hot or selected, redraw the
                // background and text.
                if ( flags )
                {
                    // Draw background and focus rectangle.
                    const wxRect rect = wxRectFromRECT(info.rc);
                    wxRendererNative::Get().DrawItemSelectionRect(this, dc,
                        rect, flags);

                    // Draw cell text.
                    if ( isMonthView )
                    {
                        // Set bold font if text is "marked" or is a holiday.
                        bool isBold = false;
                        if ( nCal == 1 )
                        {
                            auto bit = 1 << (info.stStart.wDay - 1);
                            isBold = ((m_marks | m_holidays) & bit) != 0;
                        }
                        if ( isBold )
                            dc.SetFont(GetFont().Bold());

                        // Position the text within the cell like the native control.
                        auto rectText = rect;
                        rectText.Deflate(1);
                        rectText.width -= FromDIP(6);
                        dc.DrawLabel(text, rectText, wxALIGN_RIGHT);

                        // Restore font.
                        if ( isBold )
                            dc.SetFont(GetFont());
                    }
                    else
                        dc.DrawLabel(text, rect, wxALIGN_CENTER);
                }
            }
        }
    }

    // Draw the left button.
    info.dwFlags = MCGIF_RECT;
    info.dwPart = MCGIP_PREV;
    ::SendMessage(m_hWnd, MCM_GETCALENDARGRIDINFO, 0, (LPARAM)&info);
    // Draw a left-pointing triangle.
    dc.DrawLabel(L"\u25C0", wxRectFromRECT(info.rc), wxALIGN_CENTER);

    // Draw the right button.
    info.dwPart = MCGIP_NEXT;
    ::SendMessage(m_hWnd, MCM_GETCALENDARGRIDINFO, 0, (LPARAM)&info);
    // Draw a right-pointing triangle.
    dc.DrawLabel(L"\u25B6", wxRectFromRECT(info.rc), wxALIGN_CENTER);

    return bmpOut;
}

#endif // wxUSE_CALENDARCTRL
