/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/calctrl.cpp
// Purpose:     wxCalendarCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2008-04-04
// RCS-ID:      $Id:$
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

    // initialize the base class
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(MONTHCAL_CLASS, wxEmptyString, pos, size) )
        return false;

    SetDate(dt.IsValid() ? dt : wxDateTime::Today());

    return true;
}

WXDWORD wxCalendarCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD styleMSW = wxCalendarCtrlBase::MSWGetStyle(style, exstyle);

    // right now we don't support any native styles but we should add wx styles
    // corresponding to MCS_NOTODAY, MCS_NOTODAYCIRCLE and MCS_WEEKNUMBERS
    // probably (TODO)

    // for compatibility with the other versions, just turn off today display
    // unconditionally for now
    styleMSW |= MCS_NOTODAY;

    return styleMSW;
}

// TODO: handle WM_WININICHANGE

// ----------------------------------------------------------------------------
// wxCalendarCtrl geometry
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// wxCalendarCtrl operations
// ----------------------------------------------------------------------------

bool wxCalendarCtrl::SetDate(const wxDateTime& dt)
{
    wxCHECK_MSG( dt.IsValid(), false, "invalid date" );

    SYSTEMTIME st;
    wxMSWDateControls::ToSystemTime(&st, dt);
    if ( !MonthCal_SetCurSel(GetHwnd(), &st) )
    {
        wxLogDebug(_T("DateTime_SetSystemtime() failed"));

        return false;
    }

    return true;
}

wxDateTime wxCalendarCtrl::GetDate() const
{
    SYSTEMTIME st;
    if ( !MonthCal_GetCurSel(GetHwnd(), &st) )
        return wxDefaultDateTime;

    wxDateTime dt;
    wxMSWDateControls::FromSystemTime(&dt, st);
    return dt;
}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    SYSTEMTIME st[2];

    DWORD flags = 0;
    if ( dt1.IsValid() )
    {
        wxMSWDateControls::ToSystemTime(&st[0], dt1);
        flags |= GDTR_MIN;
    }

    if ( dt2.IsValid() )
    {
        wxMSWDateControls::ToSystemTime(&st[1], dt2);
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
            wxMSWDateControls::FromSystemTime(dt1, st[0]);
        else
            *dt1 = wxDefaultDateTime;
    }

    if ( dt2 )
    {
        if ( flags & GDTR_MAX )
            wxMSWDateControls::FromSystemTime(dt2, st[1]);
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
    wxFAIL_MSG( "not implemented" );

    return false;
}

void wxCalendarCtrl::Mark(size_t day, bool mark)
{
    wxFAIL_MSG( "not implemented" );
}

// ----------------------------------------------------------------------------
// wxCalendarCtrl events
// ----------------------------------------------------------------------------

bool wxCalendarCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR* hdr = (NMHDR *)lParam;
    switch ( hdr->code )
    {
        case MCN_SELECT:
            NMSELCHANGE *sch = (NMSELCHANGE *)hdr;
            GenerateEvent(wxEVT_CALENDAR_SEL_CHANGED);
            *result = 0;
            return true;
    }

    return wxCalendarCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

#endif // wxUSE_CALENDARCTRL
