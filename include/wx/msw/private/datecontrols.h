///////////////////////////////////////////////////////////////////////////////
// Name:        msw/private/datecontrols.h
// Purpose:     implementation helpers for wxDatePickerCtrl and wxCalendarCtrl
// Author:      Vadim Zeitlin
// Created:     2008-04-04
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _MSW_PRIVATE_DATECONTROLS_H_
#define _MSW_PRIVATE_DATECONTROLS_H_

// namespace for the helper functions related to the date controls
namespace wxMSWDateControls
{

// do the one time only initialization of date classes of comctl32.dll, return
// true if ok or log an error and return false if we failed (this can only
// happen with a very old version of common controls DLL, i.e. before 4.70)
extern bool CheckInitialization();

// convert SYSTEMTIME to wxDateTime
inline void FromSystemTime(wxDateTime *dt, const SYSTEMTIME& st)
{
    dt->Set(st.wDay,
            wx_static_cast(wxDateTime::Month, wxDateTime::Jan + st.wMonth - 1),
            st.wYear,
            0, 0, 0);
}

// convert wxDateTime to SYSTEMTIME
inline void ToSystemTime(SYSTEMTIME *st, const wxDateTime& dt)
{
    const wxDateTime::Tm tm(dt.GetTm());

    st->wYear = (WXWORD)tm.year;
    st->wMonth = (WXWORD)(tm.mon - wxDateTime::Jan + 1);
    st->wDay = tm.mday;

    st->wDayOfWeek =
    st->wHour =
    st->wMinute =
    st->wSecond =
    st->wMilliseconds = 0;
}

} // namespace wxMSWDateControls

#endif // _MSW_PRIVATE_DATECONTROLS_H_

