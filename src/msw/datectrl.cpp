/////////////////////////////////////////////////////////////////////////////
// Name:        msw/datectrl.cpp
// Purpose:     wxDatePickerCtrl implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-09
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
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

#ifndef WX_PRECOMP
#endif

#if wxUSE_DATEPICKCTRL

#include "wx/datectrl.h"
#include "wx/app.h"
#include "wx/intl.h"
#include "wx/dynlib.h"

#define _WX_DEFINE_DATE_EVENTS_
#include "wx/dateevt.h"

#include "wx/msw/wrapwin.h"
#include "wx/msw/wrapcctl.h"
#include "wx/msw/private.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helpers for wxDateTime <-> SYSTEMTIME conversion
// ----------------------------------------------------------------------------

static inline void wxFromSystemTime(wxDateTime *dt, const SYSTEMTIME& st)
{
    dt->Set(st.wDay,
            wx_static_cast(wxDateTime::Month, wxDateTime::Jan + st.wMonth - 1),
            st.wYear,
            0, 0, 0);
}

static inline void wxToSystemTime(SYSTEMTIME *st, const wxDateTime& dt)
{
    const wxDateTime::Tm tm(dt.GetTm());

    st->wYear = tm.year;
    st->wMonth = tm.mon - wxDateTime::Jan + 1;
    st->wDay = tm.mday;

    st->wDayOfWeek =
    st->wHour =
    st->wMinute =
    st->wSecond =
    st->wMilliseconds = 0;
}

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
    // although we already call InitCommonControls() in app.cpp which is
    // supposed to initialize all common controls, in comctl32.dll 4.72 (and
    // presumably earlier versions 4.70 and 4.71, date time picker not being
    // supported in < 4.70 anyhow) it does not do it and we have to initialize
    // it explicitely
    static bool s_initDone = false; // MT-ok: used from GUI thread only
    if ( !s_initDone )
    {
        if ( wxTheApp->GetComCtl32Version() < 470 )
        {
            wxLogError(_("This system doesn't support date picker control, please upgrade your version of comctl32.dll"));

            return false;
        }

        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(icex);
        icex.dwICC = ICC_DATE_CLASSES;

        wxDynamicLibrary dllComCtl32(_T("comctl32.dll"), wxDL_VERBATIM);

        typedef BOOL (WINAPI *ICCEx_t)(INITCOMMONCONTROLSEX *);
        wxDYNLIB_FUNCTION( ICCEx_t, InitCommonControlsEx, dllComCtl32 );

        if ( pfnInitCommonControlsEx )
        {
            (*pfnInitCommonControlsEx)(&icex);
        }

        s_initDone = true;
    }


    // use wxDP_SPIN if wxDP_DEFAULT (0) was given as style
    if ( !(style & wxDP_DROPDOWN) )
        style |= wxDP_SPIN;

    // initialize the base class
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(DATETIMEPICK_CLASS, _T(""), pos, size) )
        return false;

    if ( dt.IsValid() )
        SetValue(dt);

    return true;
}

WXDWORD wxDatePickerCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD styleMSW = wxDatePickerCtrlBase::MSWGetStyle(style, exstyle);

    // although MSDN doesn't mention it, DTS_UPDOWN doesn't work with
    // comctl32.dll 4.72
    if ( wxTheApp->GetComCtl32Version() > 472 && (style & wxDP_SPIN) )
        styleMSW |= DTS_UPDOWN;
    //else: drop down by default

#ifdef DTS_SHORTDATECENTURYFORMAT
    if ( style & wxDP_SHOWCENTURY )
        styleMSW |= DTS_SHORTDATECENTURYFORMAT;
    else
#endif // DTS_SHORTDATECENTURYFORMAT
        styleMSW |= DTS_SHORTDATEFORMAT;

    return styleMSW;
}

// TODO: handle WM_WININICHANGE

// ----------------------------------------------------------------------------
// wxDatePickerCtrl geometry
// ----------------------------------------------------------------------------

wxSize wxDatePickerCtrl::DoGetBestSize() const
{
    const int y = GetCharHeight();

    return wxSize(DEFAULT_ITEM_WIDTH, EDIT_HEIGHT_FROM_CHAR_HEIGHT(y));
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl operations
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    // as we don't support DTS_SHOWNONE style so far, we don't allow setting
    // the control to an invalid date, but this restriction may be lifted in
    // the future
    wxCHECK_RET( dt.IsValid(), _T("invalid date") );

    SYSTEMTIME st;
    wxToSystemTime(&st, dt);
    if ( !DateTime_SetSystemtime(GetHwnd(), GDT_VALID, &st) )
    {
        wxLogDebug(_T("DateTime_SetSystemtime() failed"));
    }
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
    wxDateTime dt;
    SYSTEMTIME st;
    if ( DateTime_GetSystemtime(GetHwnd(), &st) == GDT_VALID )
    {
        wxFromSystemTime(&dt, st);
    }

    return dt;
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    SYSTEMTIME st[2];

    DWORD flags = 0;
    if ( dt1.IsValid() )
    {
        wxToSystemTime(&st[0], dt1);
        flags |= GDTR_MIN;
    }

    if ( dt2.IsValid() )
    {
        wxToSystemTime(&st[1], dt2);
        flags |= GDTR_MAX;
    }

    if ( !DateTime_SetRange(GetHwnd(), flags, st) )
    {
        wxLogDebug(_T("DateTime_SetRange() failed"));
    }
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    SYSTEMTIME st[2];

    DWORD flags = DateTime_GetRange(GetHwnd(), st);
    if ( dt1 )
    {
        if ( flags & GDTR_MIN )
            wxFromSystemTime(dt1, st[0]);
        else
            *dt1 = wxDefaultDateTime;
    }

    if ( dt2 )
    {
        if ( flags & GDTR_MAX )
            wxFromSystemTime(dt2, st[1]);
        else
            *dt2 = wxDefaultDateTime;
    }

    return flags != 0;
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl events
// ----------------------------------------------------------------------------

bool
wxDatePickerCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR* hdr = (NMHDR *)lParam;
    switch ( hdr->code )
    {
        case DTN_DATETIMECHANGE:
            NMDATETIMECHANGE *dtch = (NMDATETIMECHANGE *)hdr;
            wxDateTime dt;
            if ( dtch->dwFlags == GDT_VALID )
                wxFromSystemTime(&dt, dtch->st);

            wxDateEvent event(this, dt, wxEVT_DATE_CHANGED);
            if ( GetEventHandler()->ProcessEvent(event) )
            {
                *result = 0;
                return true;
            }
    }

    return wxDatePickerCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

#endif // wxUSE_DATEPICKCTRL

