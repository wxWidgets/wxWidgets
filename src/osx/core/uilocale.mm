///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/uilocale.mm
// Purpose:     wxUILocale implementation for macOS
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

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#include <CoreFoundation/CFLocale.h>
#include <CoreFoundation/CFString.h>

extern wxString
wxGetInfoFromCFLocale(CFLocaleRef cfloc, wxLocaleInfo index, wxLocaleCategory cat);

namespace
{

// ----------------------------------------------------------------------------
// wxUILocale implementation using Core Foundation
// ----------------------------------------------------------------------------

class wxUILocaleImplCF : public wxUILocaleImpl
{
public:
    explicit wxUILocaleImplCF(const wxCFRef<CFLocaleRef>& cfloc)
        : m_cfloc(cfloc)
    {
    }

    static wxUILocaleImplCF* Create(const wxString& name)
    {
        CFLocaleRef cfloc = CFLocaleCreate(kCFAllocatorDefault, wxCFStringRef(name));
        if ( !cfloc )
            return NULL;

        return new wxUILocaleImplCF(cfloc);
    }

    wxString GetName() const wxOVERRIDE;
    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const wxOVERRIDE;

private:
    wxCFRef<CFLocaleRef> m_cfloc;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplCF);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

wxString
wxUILocaleImplCF::GetName() const
{
    return wxCFStringRef::AsString(CFLocaleGetIdentifier(m_cfloc));
}

wxString
wxUILocaleImplCF::GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const
{
    return wxGetInfoFromCFLocale(m_cfloc, index, cat);
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateStdC()
{
    return wxUILocaleImplCF::Create("C");
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplCF(CFLocaleCopyCurrent());
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLanguage(const wxLanguageInfo& info)
{
    return wxUILocaleImplCF::Create(info.CanonicalName);
}

#endif // wxUSE_INTL
