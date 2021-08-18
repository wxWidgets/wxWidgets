/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/datectrl.cpp
// Purpose:     wxDatePickerCtrl implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-09
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_DATEPICKCTRL

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/msw/private.h"
#endif

#include "wx/datectrl.h"
#include "wx/dateevt.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDatePickerCtrl creation
// ----------------------------------------------------------------------------

bool
wxDatePickerCtrl::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxDateTime& dt,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    // use wxDP_SPIN if wxDP_DEFAULT (0) was given as style
    if ( !(style & wxDP_DROPDOWN) )
        style |= wxDP_SPIN;

    return MSWCreateDateTimePicker(parent, id, dt,
                                   pos, size, style,
                                   validator, name);
}

WXDWORD wxDatePickerCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD styleMSW = wxDatePickerCtrlBase::MSWGetStyle(style, exstyle);

    if ( style & wxDP_SPIN )
        styleMSW |= DTS_UPDOWN;
    //else: drop down by default

    if ( style & wxDP_SHOWCENTURY )
        styleMSW |= DTS_SHORTDATECENTURYFORMAT;
    else
        styleMSW |= DTS_SHORTDATEFORMAT;

    if ( style & wxDP_ALLOWNONE )
        styleMSW |= DTS_SHOWNONE;

    return styleMSW;
}

// TODO: handle WM_WININICHANGE

#if wxUSE_INTL

wxLocaleInfo wxDatePickerCtrl::MSWGetFormat() const
{
    return wxLOCALE_SHORT_DATE_FMT;
}

#endif // wxUSE_INTL

// ----------------------------------------------------------------------------
// wxDatePickerCtrl operations
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    if ( dt.IsValid() )
    {
        // Don't try setting the date if it's out of range: calendar control
        // under XP (and presumably all the other pre-Vista Windows versions)
        // doesn't return false from DateTime_SetSystemtime() in this case but
        // doesn't actually change the date, so we can't update our m_date
        // unconditionally and would need to check whether it was changed
        // before doing it. It looks simpler to just check whether it's in
        // range here instead.
        //
        // If we ever drop support for XP we could rely on the return value of
        // DateTime_SetSystemtime() but this probably won't happen in near
        // future.
        wxDateTime dtStart, dtEnd;
        GetRange(&dtStart, &dtEnd);
        if ( (dtStart.IsValid() && dt < dtStart) ||
                (dtEnd.IsValid() && dt > dtEnd) )
        {
            // Fail silently, some existing code relies on SetValue() with an
            // out of range value simply doing nothing -- so don't.
            return;
        }
    }

    wxDateTimePickerCtrl::SetValue(dt);

    // we need to keep only the date part, times don't make sense for this
    // control (in particular, comparisons with other dates would fail)
    if ( m_date.IsValid() )
        m_date.ResetTime();
}

wxDateTime wxDatePickerCtrl::MSWGetControlValue() const
{
    wxDateTime dt;
    SYSTEMTIME st;
    if ( DateTime_GetSystemtime(GetHwnd(), &st) == GDT_VALID )
    {
        dt.SetFromMSWSysDate(st);
    }

    return dt;
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
#if wxDEBUG_LEVEL
    const wxDateTime dt = MSWGetControlValue();

    wxASSERT_MSG( m_date.IsValid() == dt.IsValid() &&
                    (!dt.IsValid() || dt == m_date),
                  wxT("bug in wxDateTimePickerCtrl: m_date not in sync") );
#endif // wxDEBUG_LEVEL

    return wxDateTimePickerCtrl::GetValue();
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
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

    if ( !DateTime_SetRange(GetHwnd(), flags, st) )
    {
        wxLogDebug(wxT("DateTime_SetRange() failed"));
        return;
    }

    // Setting the range could have changed the current control value if the
    // old one wasn't inside the new range, so update it.
    m_date = MSWGetControlValue();
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    SYSTEMTIME st[2];

    DWORD flags = DateTime_GetRange(GetHwnd(), st);
    if ( dt1 )
    {
        // Workaround for https://bugs.winehq.org/show_bug.cgi?id=40301: WINE
        // returns GDTR_MIN even if there is no lower range bound. Luckily we
        // can check for it easily as the SYSTEMTIME contains only zeroes in
        // this case and 0 is invalid value for wMonth field.
        if ( (flags & GDTR_MIN) && (st[0].wMonth != 0) )
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
// wxDatePickerCtrl events
// ----------------------------------------------------------------------------

bool wxDatePickerCtrl::MSWOnDateTimeChange(const NMDATETIMECHANGE& dtch)
{
    wxDateTime dt;
    if ( dtch.dwFlags == GDT_VALID )
        dt.SetFromMSWSysDate(dtch.st);

    // filter out duplicate DTN_DATETIMECHANGE events which the native
    // control sends us when using wxDP_DROPDOWN style
    if ( (m_date.IsValid() == dt.IsValid()) &&
            (!m_date.IsValid() || dt == m_date) )
        return false;

    m_date = dt;
    wxDateEvent event(this, dt, wxEVT_DATE_CHANGED);
    return HandleWindowEvent(event);
}

#endif // wxUSE_DATEPICKCTRL
