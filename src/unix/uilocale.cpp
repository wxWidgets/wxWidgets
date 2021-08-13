///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/uilocale.cpp
// Purpose:     wxUILocale implementation for Unix systems
// Author:      Vadim Zeitlin
// Created:     2021-08-01
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_INTL

#include "wx/private/uilocale.h"

#include "wx/intl.h"

#include <locale.h>

namespace
{

// ----------------------------------------------------------------------------
// wxUILocale implementation using standard Unix/C functions
// ----------------------------------------------------------------------------

class wxUILocaleImplUnix : public wxUILocaleImpl
{
public:
    // Locale argument may be NULL to not change it at all.
    explicit wxUILocaleImplUnix(const char* locale);

    wxString GetName() const wxOVERRIDE;
    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const wxOVERRIDE;

private:
    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplUnix);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

wxUILocaleImplUnix::wxUILocaleImplUnix(const char* locale)
{
    if ( locale )
        setlocale(LC_ALL, locale);
}

wxString
wxUILocaleImplUnix::GetName() const
{
    return wxString::FromAscii(setlocale(LC_ALL, NULL));
}

wxString
wxUILocaleImplUnix::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
    // Currently we rely on the user code not calling setlocale() itself, so
    // that the current locale is still the same as was set in the ctor.
    //
    // If this assumption turns out to be wrong, we could use wxLocaleSetter to
    // temporarily change the locale here (maybe only if setlocale(NULL) result
    // differs from the expected one).
    return wxLocale::GetInfo(index, cat);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    return new wxUILocaleImplUnix(NULL);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplUnix("");
}

#endif // wxUSE_INTL
