///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/filetime.h
// Purpose:     Helpers for converting FILETIME values.
// Author:      Vadim Zeitlin
// Created:     2026-10-17
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_FILETIME_H_
#define _WX_MSW_PRIVATE_FILETIME_H_

#include "wx/msw/wrapwin.h"
#include "wx/longlong.h"

namespace wxMSWImpl
{

// FILETIME is a 64-bit value representing the number of 100ns intervals since
// 1601-01-01 UTC and this is the number of these intervals between this epoch
// and the Unix one, 1970-01-01 UTC.
constexpr wxInt64 FILETIME_UNIX_EPOCH_OFFSET = wxLL(116444736000000000);

// FILETIME is formally unsigned, but Windows functions don't accept values
// with the highest bit set, so this is the maximal valid FILETIME value.
constexpr wxInt64 FILETIME_MAX = wxINT64_MAX;

inline bool IsValidFileTime(const FILETIME& ft)
{
    return !(ft.dwHighDateTime & 0x80000000);
}

// Convert FILETIME, which must be valid, to the number of 100ns intervals
// since the Unix epoch.
inline wxLongLong FileTimeToUnix100ns(const FILETIME& ft)
{
    return wxLongLong(ft.dwHighDateTime, ft.dwLowDateTime) -
            FILETIME_UNIX_EPOCH_OFFSET;
}

// Convert the number of 100ns intervals since the Unix epoch to FILETIME.
//
// The value must be in [-FILETIME_UNIX_EPOCH_OFFSET, FILETIME_MAX -
// FILETIME_UNIX_EPOCH_OFFSET] range, i.e. correspond to a valid FILETIME.
inline FILETIME Unix100nsToFileTime(wxLongLong t100ns)
{
    t100ns += FILETIME_UNIX_EPOCH_OFFSET;

    FILETIME ft;
    ft.dwHighDateTime = t100ns.GetHi();
    ft.dwLowDateTime = t100ns.GetLo();
    return ft;
}

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_FILETIME_H_
