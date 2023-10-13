///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/uilocale.h
// Purpose:     wxUILocaleImpl class declaration
// Author:      Vadim Zeitlin
// Created:     2021-08-01
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_UILOCALE_H_
#define _WX_PRIVATE_UILOCALE_H_

#include "wx/localedefs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/vector.h"

typedef wxVector<wxLanguageInfo> wxLanguageInfos;

// Return the vector of all languages known to wx.
const wxLanguageInfos& wxGetLanguageInfos();

// Function returning hard-coded values for the "C" locale.
wxString wxGetStdCLocaleInfo(wxLocaleInfo index, wxLocaleCategory cat);

// ----------------------------------------------------------------------------
// wxUILocaleImpl provides the implementation of public wxUILocale functions
// ----------------------------------------------------------------------------

class wxUILocaleImpl : public wxRefCounter
{
public:
    // This function is implemented in platform-specific code and returns the
    // object used by default, i.e. if wxUILocale::UseDefault() is not called.
    // This object corresponds to the traditional "C" locale.
    //
    // It should never return nullptr.
    static wxUILocaleImpl* CreateStdC();

    // Similarly, this one returns the object corresponding to the default user
    // locale settings which is used if wxUILocale::UseDefault() was called.
    //
    // It may return nullptr in case of failure.
    static wxUILocaleImpl* CreateUserDefault();

    // Create locale object for the given locale.
    //
    // It may return nullptr in case of failure.
    static wxUILocaleImpl* CreateForLocale(const wxLocaleIdent& locId);

    // This function exists only for wxLocale compatibility and creates the
    // locale corresponding to the given language. It shouldn't be used
    // anywhere else.
    //
    // It is implemented in terms of CreateForLocale() for non-MSW platforms,
    // but under MSW it is different for compatibility reasons.
    //
    // The language passed to this function is a valid language, i.e. neither
    // wxLANGUAGE_UNKNOWN nor wxLANGUAGE_DEFAULT.
    //
    // It may return nullptr in case of failure, but never does so for English
    // languages because wxLocale(wxLANGUAGE_ENGLISH) is always supposed to
    // work, so it just falls back on CreateStdC() if it fails to create it.
    static wxUILocaleImpl* CreateForLanguage(const wxLanguageInfo& info);

    // This function retrieves a list of preferred UI languages.
    // The list is in the order of preference, if it has more than one entry.
    // The entries contain platform-dependent identifiers.
    static wxVector<wxString> GetPreferredUILanguages();

    // Helper function used by GetMonthName/GetWeekDayName(): returns 0 if flags is
    // wxDateTime::Name_Full, 1 if it is wxDateTime::Name_Abbr, and 2 if it is
    // wxDateTime::Name_Shortest or -1 if the flags is incorrect (and asserts in this case)
    //
    // the return value of this function is used as an index into 2D array
    // containing full names in its first row and abbreviated ones in the 2nd one
    static int ArrayIndexFromFlag(wxDateTime::NameFlags flags);

    // Use this locale in the UI.
    //
    // This is not implemented for all platforms, notably not for Mac where the
    // UI locale is determined at application startup, but we can't do anything
    // about it anyhow, so we don't even bother returning an error code from it.
    virtual void Use() = 0;

    // Functions corresponding to wxUILocale ones.
    virtual wxString GetName() const = 0;
    virtual wxLocaleIdent GetLocaleId() const = 0;
    virtual wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const = 0;
    virtual wxString GetLocalizedName(wxLocaleName name, wxLocaleForm form) const = 0;
    virtual wxString GetMonthName(wxDateTime::Month month, wxDateTime::NameForm form) const = 0;
    virtual wxString GetWeekDayName(wxDateTime::WeekDay weekday, wxDateTime::NameForm form) const = 0;

    virtual wxLayoutDirection GetLayoutDirection() const = 0;
    virtual int CompareStrings(const wxString& lhs, const wxString& rhs,
                               int flags) const = 0;

    virtual ~wxUILocaleImpl() { }
};

#endif // _WX_PRIVATE_UILOCALE_H_
