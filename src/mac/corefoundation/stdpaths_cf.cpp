///////////////////////////////////////////////////////////////////////////////
// Name:        mac/corefoundation/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for CoreFoundation systems
// Author:      David Elliott
// Modified by:
// Created:     2004-10-27
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott <dfe@cox.net>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif //ndef WX_PRECOMP

#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/mac/corefoundation/cfstring.h"

#if defined(__DARWIN__)
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>
#else
#include <CFBundle.h>
#include <CFURL.h>
#endif

#if defined(__WXCOCOA__) || defined(__WXMAC_OSX__)
#define kDefaultPathStyle kCFURLPOSIXPathStyle
#else
#define kDefaultPathStyle kCFURLHFSPathStyle
#endif

static wxString BundleRelativeURLToPath(CFURLRef relativeURL)
{
    CFURLRef absoluteURL = CFURLCopyAbsoluteURL(relativeURL);
    wxCHECK_MSG(absoluteURL, wxEmptyString, wxT("Failed to resolve relative URL to absolute URL"));
    CFStringRef cfStrPath = CFURLCopyFileSystemPath(absoluteURL,kDefaultPathStyle);
    CFRelease(absoluteURL);
    return wxMacCFStringHolder(cfStrPath).AsString(wxLocale::GetSystemEncoding());
}

wxStandardPathsCF::wxStandardPathsCF()
:   m_bundle(CFBundleGetMainBundle())
{
    CFRetain(m_bundle);
}

wxStandardPathsCF::wxStandardPathsCF(struct __CFBundle *bundle)
:   m_bundle(bundle)
{
    CFRetain(m_bundle);
}

wxStandardPathsCF::~wxStandardPathsCF()
{
    CFRelease(m_bundle);
}

void wxStandardPathsCF::SetBundle(struct __CFBundle *bundle)
{
    CFRetain(bundle);
    CFRelease(m_bundle);
    m_bundle = bundle;
}

wxString wxStandardPathsCF::GetConfigDir() const
{
    // TODO: What do we do for pure Carbon?
    return wxT("/Library/Preferences");
}

wxString wxStandardPathsCF::GetUserConfigDir() const
{
    // TODO: What do we do for pure Carbon?
    return wxFileName::GetHomeDir() + wxT("/Library/Preferences");
}

wxString wxStandardPathsCF::GetDataDir() const
{
    wxCHECK_MSG(m_bundle, wxEmptyString, wxT("wxStandardPaths for CoreFoundation only works with bundled apps"));
    CFURLRef relativeURL = CFBundleCopySharedSupportURL(m_bundle);
    wxCHECK_MSG(relativeURL, wxEmptyString, wxT("Couldn't get SharedSupport URL"));
    wxString ret(BundleRelativeURLToPath(relativeURL));
    CFRelease(relativeURL);
    return ret;
}

wxString wxStandardPathsCF::GetLocalDataDir() const
{
    return AppendAppName(wxT("/Library/Application Support"));
}

wxString wxStandardPathsCF::GetUserDataDir() const
{
    return AppendAppName(wxFileName::GetHomeDir() + _T("/Library/Application Support"));
}

wxString wxStandardPathsCF::GetPluginsDir() const
{
    wxCHECK_MSG(m_bundle, wxEmptyString, wxT("wxStandardPaths for CoreFoundation only works with bundled apps"));
    CFURLRef relativeURL = CFBundleCopyBuiltInPlugInsURL(m_bundle);
    wxCHECK_MSG(relativeURL, wxEmptyString, wxT("Couldn't get BuiltInPlugIns URL"));
    wxString ret(BundleRelativeURLToPath(relativeURL));
    CFRelease(relativeURL);
    return ret;
}

