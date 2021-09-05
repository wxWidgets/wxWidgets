///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/uilocale.h
// Purpose:     Various locale-related helpers used under Unix systems only
// Author:      Vadim Zeitlin
// Created:     2021-08-14 (extracted from src/common/intl.cpp)
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_UILOCALE_H_
#define _WX_UNIX_PRIVATE_UILOCALE_H_

#include "wx/string.h"

// get just the language part ("en" in "en_GB")
inline wxString ExtractLang(const wxString& langFull)
{
    return langFull.BeforeFirst('_');
}

// get everything else (including the leading '_')
inline wxString ExtractNotLang(const wxString& langFull)
{
    size_t pos = langFull.find('_');
    if ( pos != wxString::npos )
        return langFull.substr(pos);
    else
        return wxString();
}

const char *wxSetlocaleTryAll(int c, const wxLocaleIdent& lc);

// Extract date format from D_T_FMT value.
wxString wxGetDateFormatOnly(const wxString& fmt);

#endif // _WX_UNIX_PRIVATE_UILOCALE_H_
