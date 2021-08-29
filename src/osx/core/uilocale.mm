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

#include "wx/uilocale.h"
#include "wx/private/uilocale.h"

#include "wx/osx/core/cfref.h"
#include "wx/osx/core/cfstring.h"

#include <CoreFoundation/CFLocale.h>
#include <CoreFoundation/CFString.h>

#import <Foundation/NSString.h>
#import <Foundation/NSLocale.h>

extern wxString
wxGetInfoFromCFLocale(CFLocaleRef cfloc, wxLocaleInfo index, wxLocaleCategory cat);

// ----------------------------------------------------------------------------
// wxLocaleIdent::GetName() implementation using Foundation
// ----------------------------------------------------------------------------

wxString wxLocaleIdent::GetName() const
{
    // Construct name in right format:
    // MacOS: <language>-<script>_<REGION>

    wxString name;
    if ( !m_language.empty() )
    {
        name << m_language;

        if ( !m_script.empty() )
        {
            name << "-" << m_script;
        }

        if ( !m_region.empty() )
        {
            name << "_" << m_region;
        }
    }

    return name;
}

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

    static wxUILocaleImplCF* Create(const wxLocaleIdent& locId)
    {
        CFLocaleRef cfloc = CFLocaleCreate(kCFAllocatorDefault,
                                           wxCFStringRef(locId.GetName()));
        if ( !cfloc )
            return NULL;

        return new wxUILocaleImplCF(cfloc);
    }

    bool Use() wxOVERRIDE;
    wxString GetName() const wxOVERRIDE;
    wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat) const wxOVERRIDE;
    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags) const wxOVERRIDE;

private:
    wxCFRef<CFLocaleRef> m_cfloc;

    wxDECLARE_NO_COPY_CLASS(wxUILocaleImplCF);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

bool
wxUILocaleImplCF::Use()
{
    // There is no way to start using a locale other than default.
    return false;
}

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
    return wxUILocaleImplCF::Create(wxLocaleIdent("C"));
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateUserDefault()
{
    return new wxUILocaleImplCF(CFLocaleCopyCurrent());
}

/* static */
wxUILocaleImpl* wxUILocaleImpl::CreateForLocale(const wxLocaleIdent& locId)
{
    return wxUILocaleImplCF::Create(locId);
}

int
wxUILocaleImplCF::CompareStrings(const wxString& lhs, const wxString& rhs,
                                 int flags) const
{
    NSString *ns_lhs = [NSString stringWithCString:lhs.ToStdString(wxConvUTF8).c_str()
                                 encoding:NSUTF8StringEncoding];
    NSString *ns_rhs = [NSString stringWithCString:rhs.ToStdString(wxConvUTF8).c_str()
                                 encoding:NSUTF8StringEncoding];
    NSInteger options = 0;
    if ( flags & wxCompare_CaseInsensitive )
        options |= NSCaseInsensitiveSearch;

    NSComparisonResult ret = [ns_lhs compare:ns_rhs
                                     options:options
                                     range:(NSRange){0, [ns_lhs length]}
                                     locale:(id)(CFLocaleRef)m_cfloc];

    switch (ret)
    {
        case NSOrderedAscending:
            return -1;
        case NSOrderedSame:
            return 0;
        case NSOrderedDescending:
            return 1;
    }

    return 0;
}

#endif // wxUSE_INTL
