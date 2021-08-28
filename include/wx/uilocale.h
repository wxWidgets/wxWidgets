///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uilocale.h
// Purpose:     wxUILocale class declaration.
// Author:      Vadim Zeitlin
// Created:     2021-07-31
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UILOCALE_H_
#define _WX_UILOCALE_H_

#include "wx/defs.h"

#if wxUSE_INTL

#include "wx/localedefs.h"
#include "wx/string.h"

class wxUILocaleImpl;

// Flags for wxUILocale::CompareStrings().
enum
{
    wxCompare_CaseSensitive   = 0,
    wxCompare_CaseInsensitive = 1
};

// ----------------------------------------------------------------------------
// wxUILocale allows to use the default UI locale and get information about it
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxUILocale
{
public:
    // Configure the UI to use the default user locale.
    static bool UseDefault();

    // Use the locale corresponding to the given language.
    //
    // This is a compatibility function used by wxWidgets itself, don't use it
    // in the new code.
    static bool UseLanguage(const wxLanguageInfo& info);

    // Get the object corresponding to the currently used locale.
    static const wxUILocale& GetCurrent();

    // Get the platform-dependent name of the current locale.
    wxString GetName() const;

    // Query the locale for the specified information.
    wxString GetInfo(wxLocaleInfo index,
                     wxLocaleCategory cat = wxLOCALE_CAT_DEFAULT) const;

    // Compares two strings, for a locale specified by wxLocaleIdent.
    static int CompareStrings(const wxString& lhs, const wxString& rhs,
                              const wxLocaleIdent& localeId = wxLocaleIdent(),
                              int flags = wxCompare_CaseSensitive);

    // Note that this class is not supposed to be used polymorphically, hence
    // its dtor is not virtual.
    ~wxUILocale();

private:
    // Ctor is private, use static accessor to get objects of this class.
    wxUILocale() : m_impl(NULL) { }

    // Used by UseDefault().
    //
    // Note that this object takes ownership of the provided pointer and will
    // delete it in dtor.
    void SetImpl(wxUILocaleImpl* impl);


    static wxUILocale ms_current;

    wxUILocaleImpl* m_impl;

    wxDECLARE_NO_COPY_CLASS(wxUILocale);
};

inline wxString wxGetUIDateFormat()
{
    return wxUILocale::GetCurrent().GetInfo(wxLOCALE_SHORT_DATE_FMT);
}

#else // !wxUSE_INTL

inline wxString wxGetUIDateFormat()
{
    return wxString(wxS("%x"));
}

#endif // wxUSE_INTL/!wxUSE_INTL

#endif // _WX_UILOCALE_H_
