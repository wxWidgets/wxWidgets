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

// Helper class changing the global locale to the one specified by the
// environment variables in its ctor and restoring it in its dtor.
namespace
{

class TempLocaleSetter
{
public:
    explicit TempLocaleSetter(int localeCategory,
                              const wxString& localeId = wxString())
        : m_localeCategory(localeCategory),
          m_localeOrig(strdup(setlocale(localeCategory, nullptr)))
    {
        setlocale(localeCategory, localeId.mb_str());
    }

    ~TempLocaleSetter()
    {
        setlocale(m_localeCategory, m_localeOrig);
        free(m_localeOrig);
    }

private:
    const int m_localeCategory;
    char* const m_localeOrig;

    wxDECLARE_NO_COPY_CLASS(TempLocaleSetter);
};

} // anonymous namespace

#endif // _WX_UNIX_PRIVATE_UILOCALE_H_
