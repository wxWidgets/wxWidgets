///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/uilocale.h
// Purpose:     MSW-specific locale-related helpers
// Author:      Vadim Zeitlin
// Created:     2021-08-14
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_UILOCALE_H_
#define _WX_MSW_PRIVATE_UILOCALE_H_

#include "wx/msw/private.h"     // Include <windows.h> to get LCID.

#ifndef LOCALE_SNAME
#define LOCALE_SNAME 0x5c
#endif
#ifndef LOCALE_CUSTOM_UI_DEFAULT
#define LOCALE_CUSTOM_UI_DEFAULT 0x1400
#endif

// Use the specific LCID for the current thread.
void wxUseLCID(LCID lcid);

// These functions are defined in src/common/intl.cpp
wxString wxGetInfoFromLCID(LCID lcid, wxLocaleInfo index, wxLocaleCategory cat);
LCTYPE wxGetLCTYPEFormatFromLocalInfo(wxLocaleInfo index);

WXDLLIMPEXP_BASE wxString wxTranslateFromUnicodeFormat(const wxString& fmt);

#endif // _WX_MSW_PRIVATE_UILOCALE_H_
