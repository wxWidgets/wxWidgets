///////////////////////////////////////////////////////////////////////////////
// Name:        osx/cocoa/aboutdlg.cpp
// Purpose:     native wxAboutBox() implementation for wxMac
// Author:      Vadim Zeitlin
// Created:     2006-10-08
// RCS-ID:      $Id: aboutdlg.cpp 54820 2008-07-29 20:04:11Z SC $
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
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

#if wxUSE_ABOUTDLG

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/aboutdlg.h"
#include "wx/generic/aboutdlgg.h"

#include "wx/osx/private.h"

/*
@"Credits": An NSAttributedString displayed in the info area of the panel. If not specified, this method then looks for a file named “Credits.html”, “Credits.rtf”, and “Credits.rtfd”, in that order, in the bundle returned by the NSBundle class method mainBundle. The first file found is used. If none is found, the info area is left blank.
@"ApplicationName": An NSString object displayed as the application’s name. If not specified, this method then uses the value of CFBundleName (localizable). If neither is found, this method uses [[NSProcessInfo processInfo] processName].
@"ApplicationIcon": An NSImage object displayed as the application’s icon. If not specified, this method then looks for an image named “NSApplicationIcon”, using [NSImage imageNamed:@"NSApplicationIcon"]. If neither is available, this method uses the generic application icon.
@"Version": An NSString object with the build version number of the application (“58.4”), displayed as “(v58.4)”. If not specified, obtain from the CFBundleVersion key in infoDictionary; if not specified, leave blank (the “(v)” is not displayed).
@"Copyright": An NSString object with a line of copyright information. If not specified, this method then looks for the value of NSHumanReadableCopyright in the localized version infoDictionary. If neither is available, this method leaves the space blank.
@"ApplicationVersion": An NSString object with the application version (“Mac OS X”, “3”, “WebObjects 4.5”, “AppleWorks 6”,...). If not specified, obtain from the CFBundleShortVersionString key in infoDictionary. If neither is available, the build version, if available, is printed alone, as “Version x.x”.
*/

// helper class for HIAboutBox options
class AboutBoxOptions : public wxCFRef<CFMutableDictionaryRef>
{
public:
    AboutBoxOptions() : wxCFRef<CFMutableDictionaryRef>
                        (
                          CFDictionaryCreateMutable
                          (
                           kCFAllocatorDefault,
                           4, // there are at most 4 values
                           &kCFTypeDictionaryKeyCallBacks,
                           &kCFTypeDictionaryValueCallBacks
                          )
                        )
    {
    }

    void Set(CFStringRef key, const wxString& value)
    {
        CFDictionarySetValue(*this, key, wxCFStringRef(value));
    }

    void SetAttributedString( CFStringRef key, const wxString& value )
    {
          wxCFRef<CFAttributedStringRef> attrString(
            CFAttributedStringCreate(kCFAllocatorDefault, wxCFStringRef(value), NULL) );
        CFDictionarySetValue(*this, key, attrString);
    }
};

// ============================================================================
// implementation
// ============================================================================

void wxAboutBox(const wxAboutDialogInfo& info)
{
    // Mac native about box currently can show only name, version, copyright
    // and description fields and we also shoehorn the credits text into the
    // description but if we have anything else we must use the generic version

    if ( info.IsSimple() )
    {
        AboutBoxOptions opts;

        opts.Set(CFSTR("ApplicationName"), info.GetName());

        if ( info.HasVersion() )
        {
            opts.Set(CFSTR("Version"),info.GetVersion());
            opts.Set(CFSTR("ApplicationVersion"),
                     wxString::Format(_("Version %s"), info.GetVersion()));
        }

        if ( info.HasCopyright() )
            opts.Set(CFSTR("Copyright"), info.GetCopyrightToDisplay());

        opts.SetAttributedString(CFSTR("Credits"), info.GetDescriptionAndCredits());

        [[NSApplication sharedApplication] orderFrontStandardAboutPanelWithOptions:((NSDictionary*)(CFDictionaryRef) opts)];
    }
    else // simple "native" version is not enough
    {
        // we need to use the full-blown generic version
        wxGenericAboutBox(info);
    }
}

#endif // wxUSE_ABOUTDLG
