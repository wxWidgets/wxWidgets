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
#include "wx/string.h"

// ----------------------------------------------------------------------------
// wxUILocaleImpl provides the implementation of public wxUILocale functions
// ----------------------------------------------------------------------------

class wxUILocaleImpl
{
public:
    // This function is implemented in platform-specific code and returns the
    // object used by default, i.e. if wxUILocale::UseDefault() is not called.
    // This object corresponds to the traditional "C" locale.
    //
    // It should never return NULL.
    static wxUILocaleImpl* CreateStdC();

    // Similarly, this one returns the object corresponding to the default user
    // locale settings which is used if wxUILocale::UseDefault() was called.
    //
    // It may return NULL in case of failure.
    static wxUILocaleImpl* CreateUserDefault();

    // This function exists only for wxLocale compatibility and creates the
    // locale corresponding to the given language.
    //
    // The language passed to this function is a valid language, i.e. neither
    // wxLANGUAGE_UNKNOWN nor wxLANGUAGE_DEFAULT.
    //
    // It may return NULL in case of failure.
    static wxUILocaleImpl* CreateForLanguage(const wxLanguageInfo& info);

    // Use this locale in the UI.
    //
    // This is not implemented for all platforms, notably not for Mac where the
    // UI locale is determined at application startup, and so this function
    // always returns false there.
    virtual bool Use() = 0;

    // Functions corresponding to wxUILocale ones.
    virtual wxString GetName() const = 0;
    virtual wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const = 0;

    virtual ~wxUILocaleImpl() { }
};

#endif // _WX_PRIVATE_UILOCALE_H_
