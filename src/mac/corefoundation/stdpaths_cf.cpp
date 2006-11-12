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

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#if wxUSE_STDPATHS

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif //ndef WX_PRECOMP

#include "wx/stdpaths.h"
#include "wx/filename.h"
#ifdef __WXMAC__
#include "wx/mac/private.h"
#endif
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

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStandardPathsCF ctors/dtor
// ----------------------------------------------------------------------------

wxStandardPathsCF::wxStandardPathsCF()
                 : m_bundle(CFBundleGetMainBundle())
{
    CFRetain(m_bundle);
}

wxStandardPathsCF::wxStandardPathsCF(wxCFBundleRef bundle)
                 : m_bundle(bundle)
{
    CFRetain(m_bundle);
}

wxStandardPathsCF::~wxStandardPathsCF()
{
    CFRelease(m_bundle);
}

// ----------------------------------------------------------------------------
// wxStandardPathsCF Mac-specific methods
// ----------------------------------------------------------------------------

void wxStandardPathsCF::SetBundle(wxCFBundleRef bundle)
{
    CFRetain(bundle);
    CFRelease(m_bundle);
    m_bundle = bundle;
}

// ----------------------------------------------------------------------------
// generic functions in terms of which the other ones are implemented
// ----------------------------------------------------------------------------

static wxString BundleRelativeURLToPath(CFURLRef relativeURL)
{
    CFURLRef absoluteURL = CFURLCopyAbsoluteURL(relativeURL);
    wxCHECK_MSG(absoluteURL, wxEmptyString, wxT("Failed to resolve relative URL to absolute URL"));
    CFStringRef cfStrPath = CFURLCopyFileSystemPath(absoluteURL,kDefaultPathStyle);
    CFRelease(absoluteURL);
    return wxMacCFStringHolder(cfStrPath).AsString(wxLocale::GetSystemEncoding());
}

wxString wxStandardPathsCF::GetFromFunc(wxCFURLRef (*func)(wxCFBundleRef)) const
{
    wxCHECK_MSG(m_bundle, wxEmptyString,
                wxT("wxStandardPaths for CoreFoundation only works with bundled apps"));
    CFURLRef relativeURL = (*func)(m_bundle);
    wxCHECK_MSG(relativeURL, wxEmptyString, wxT("Couldn't get URL"));
    wxString ret(BundleRelativeURLToPath(relativeURL));
    CFRelease(relativeURL);
    return ret;
}

wxString wxStandardPathsCF::GetDocumentsDir() const
{
#ifdef __WXMAC__
    return wxMacFindFolderNoSeparator
        (
#if TARGET_API_MAC_OSX
        kUserDomain,
#else
        kOnSystemDisk,
#endif
        kDocumentsFolderType,
        kCreateFolder
        );
#else
    return wxFileName::GetHomeDir() + wxT("/Documents");
#endif
}

// ----------------------------------------------------------------------------
// wxStandardPathsCF public API
// ----------------------------------------------------------------------------

wxString wxStandardPathsCF::GetConfigDir() const
{
#ifdef __WXMAC__
    return wxMacFindFolder((short)kLocalDomain, kPreferencesFolderType, kCreateFolder);
#else
    return wxT("/Library/Preferences");
#endif
}

wxString wxStandardPathsCF::GetUserConfigDir() const
{
#ifdef __WXMAC__
    return wxMacFindFolder((short)kUserDomain, kPreferencesFolderType, kCreateFolder);
#else
    return wxFileName::GetHomeDir() + wxT("/Library/Preferences");
#endif
}

wxString wxStandardPathsCF::GetDataDir() const
{
    return GetFromFunc(CFBundleCopySharedSupportURL);
}

wxString wxStandardPathsCF::GetExecutablePath() const
{
    ProcessInfoRec processinfo;
    ProcessSerialNumber procno ;
    FSSpec fsSpec;

    procno.highLongOfPSN = 0 ;
    procno.lowLongOfPSN = kCurrentProcess ;
    processinfo.processInfoLength = sizeof(ProcessInfoRec);
    processinfo.processName = NULL;
    processinfo.processAppSpec = &fsSpec;

    GetProcessInformation( &procno , &processinfo ) ;
    return wxMacFSSpec2MacFilename(&fsSpec);
}

wxString wxStandardPathsCF::GetLocalDataDir() const
{
#ifdef __WXMAC__
    return AppendAppName(wxMacFindFolder((short)kLocalDomain, kApplicationSupportFolderType, kCreateFolder));
#else
    return AppendAppName(wxT("/Library/Application Support"));
#endif
}

wxString wxStandardPathsCF::GetUserDataDir() const
{
#ifdef __WXMAC__
    return AppendAppName(wxMacFindFolder((short)kUserDomain, kApplicationSupportFolderType, kCreateFolder));
#else
    return AppendAppName(wxFileName::GetHomeDir() + _T("/Library/Application Support"));
#endif
}

wxString wxStandardPathsCF::GetPluginsDir() const
{
    return GetFromFunc(CFBundleCopyBuiltInPlugInsURL);
}

wxString wxStandardPathsCF::GetResourcesDir() const
{
    return GetFromFunc(CFBundleCopyResourcesDirectoryURL);
}

wxString
wxStandardPathsCF::GetLocalizedResourcesDir(const wxChar *lang,
                                            ResourceCat category) const
{
    return wxStandardPathsBase::
            GetLocalizedResourcesDir(lang, category) + _T(".lproj");
}

#endif // wxUSE_STDPATHS
