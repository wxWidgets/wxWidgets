/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/utils_base.mm
// Purpose:     various OS X utility functions in the base lib
//              (extracted from cocoa/utils.mm)
// Author:      Tobias Taschner
// Created:     2016-02-10
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/utils.h"
#include "wx/platinfo.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/datetime.h"
#endif
#include "wx/filename.h"
#include "wx/apptrait.h"

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

#if wxUSE_SOCKETS
// global pointer which lives in the base library, set from the net one (see
// sockosx.cpp) and used from the GUI code (see utilsexc_cf.cpp) -- ugly but
// needed hack, see the above-mentioned files for more information
class wxSocketManager;
extern WXDLLIMPEXP_BASE wxSocketManager *wxOSXSocketManagerCF;
wxSocketManager *wxOSXSocketManagerCF = nullptr;
#endif // wxUSE_SOCKETS

// our OS version is the same in non GUI and GUI cases
wxOperatingSystemId wxGetOsVersion(int *verMaj, int *verMin, int *verMicro)
{
    NSOperatingSystemVersion osVer = [NSProcessInfo processInfo].operatingSystemVersion;

    if ( verMaj != nullptr )
        *verMaj = osVer.majorVersion;

    if ( verMin != nullptr )
        *verMin = osVer.minorVersion;

    if ( verMicro != nullptr )
        *verMicro = osVer.patchVersion;

    return wxOS_MAC_OSX_DARWIN;
}

bool wxCheckOsVersion(int majorVsn, int minorVsn, int microVsn)
{
    NSOperatingSystemVersion osVer;
    osVer.majorVersion = majorVsn;
    osVer.minorVersion = minorVsn;
    osVer.patchVersion = microVsn;

    return [[NSProcessInfo processInfo] isOperatingSystemAtLeastVersion:osVer] != NO;
}

wxString wxGetOsDescription()
{

    int majorVer, minorVer;
    wxGetOsVersion(&majorVer, &minorVer);

#ifndef __WXOSX_IPHONE__
    // Notice that neither the OS name itself nor the code names seem to be
    // ever translated, OS X itself uses the English words even for the
    // languages not using Roman alphabet.
    // Starting with 10.12 the macOS branding is used
    wxString osBrand = wxCheckOsVersion(10, 12) ? "macOS" : "OS X";
    wxString osName;
    if (majorVer == 10)
    {
        switch (minorVer)
        {
            case 10:
                osName = "Yosemite";
                break;
            case 11:
                osName = "El Capitan";
                break;
            case 12:
                osName = "Sierra";
                break;
            case 13:
                osName = "High Sierra";
                break;
            case 14:
                osName = "Mojave";
                break;
            case 15:
                osName = "Catalina";
                break;
        };
    }
    else if (majorVer > 10)
    {
        switch (majorVer)
        {
            case 11:
                osName = "Big Sur";
                break;
            case 12:
                osName = "Monterey";
                break;
            case 13:
                osName = "Ventura";
                break;
        }
    }
#else
    wxString osBrand = "iOS";
    wxString osName;
#endif

    wxString osDesc = osBrand;
    if (!osName.empty())
        osDesc += " " + osName;

    NSString* osVersionString = [NSProcessInfo processInfo].operatingSystemVersionString;
    if (osVersionString)
        osDesc += " " + wxCFStringRef::AsString((CFStringRef)osVersionString);

    return osDesc;
}

/* static */
#if wxUSE_DATETIME
bool wxDateTime::GetFirstWeekDay(wxDateTime::WeekDay *firstDay)
{
    wxCHECK_MSG( firstDay, false, wxS("output parameter must be non-null") );

    NSCalendar *calendar = [NSCalendar currentCalendar];
    [calendar setLocale:[NSLocale autoupdatingCurrentLocale]];

    *firstDay = wxDateTime::WeekDay(([calendar firstWeekday] - 1) % 7);
    return true;
}
#endif // wxUSE_DATETIME

#ifndef __WXOSX_IPHONE__

bool wxCocoaLaunch(const char* const* argv, pid_t &pid)
{
    // If there is not a single argument then there is no application
    // to launch
    if(!argv)
    {
        wxLogDebug(wxT("wxCocoaLaunch No file to launch!"));
        return false ;
    }

    // Path to bundle
    wxString path = *argv++;
    NSError *error = nil;
    NSURL *url = [NSURL fileURLWithPath:wxCFStringRef(path).AsNSString() isDirectory:YES];

    // Check the URL validity
    if( url == nil )
    {
        wxLogDebug(wxT("wxCocoaLaunch Can't open path: %s"), path);
        return false ;
    }

    // Don't try running non-bundled applications here, we don't support output
    // redirection, which is important for them, unlike for the bundled apps,
    // so let the generic Unix code handle them.
    if ( [NSBundle bundleWithURL:url] == nil )
    {
        return false;
    }

    NSMutableArray *params = [[NSMutableArray alloc] init];

    // Loop through command line arguments to the bundle,
    // turn them into CFURLs and then put them in cfaFiles
    // For use to launch services call
    for( ; *argv != nullptr; ++argv )
    {
        NSURL *cfurlCurrentFile;
        wxString dir( *argv );
        if( wxFileName::DirExists(dir) )
        {
            // First, try creating as a directory
            cfurlCurrentFile = [NSURL fileURLWithPath:wxCFStringRef(dir).AsNSString() isDirectory:YES];
        }
        else if( wxFileName::FileExists(dir) )
        {
            // And if it isn't a directory try creating it
            // as a regular file
            cfurlCurrentFile = [NSURL fileURLWithPath:wxCFStringRef(dir).AsNSString() isDirectory:NO];
        }
        else
        {
            // Argument did not refer to
            // an entry in the local filesystem,
            // so try creating it through CFURLCreateWithString
            cfurlCurrentFile = [NSURL URLWithString:wxCFStringRef(dir).AsNSString()];
        }

        // Continue in the loop if the CFURL could not be created
        if(cfurlCurrentFile == nil)
        {
            wxLogDebug(
                       wxT("wxCocoaLaunch Could not create NSURL for argument:%s"),
                       *argv);
            continue;
        }

        // Add the valid CFURL to the argument array and then
        // release it as the CFArray adds a ref count to it
        [params addObject:cfurlCurrentFile];
    }

    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    
    
    NSRunningApplication *app = nil;
    
    if ( [params count] > 0 )
        app = [ws openURLs:params withApplicationAtURL:url
                   options:NSWorkspaceLaunchAsync
             configuration:[NSDictionary dictionary]
                     error:&error];
    
    if ( app == nil )
    {
        app = [ws launchApplicationAtURL:url
                                 options:NSWorkspaceLaunchAsync
                           configuration:[NSDictionary dictionary]
                                   error:&error];

        // this was already processed argv is null and nothing bad will happen
        for( ; *argv != nullptr; ++argv )
        {
            wxString currfile(*argv);
            if( [ws openFile:wxCFStringRef(currfile).AsNSString()
             withApplication:wxCFStringRef(path).AsNSString()] == NO )
            {
                wxLogDebug(wxT("wxCocoaLaunch Could not open argument:%s"), *argv);
                return false;
            }
        }
    }
    
    [params release];

    if( app != nil )
        pid = [app processIdentifier];
    else
    {
        wxString errorDesc = wxCFStringRef::AsString([error localizedDescription]);
        wxLogDebug( "wxCocoaLaunch failure: error is %s", errorDesc );
        return false;
    }
    return true;
}

#endif

int wxCMPFUNC_CONV wxCmpNatural(const wxString& s1, const wxString& s2)
{
    // The values of NSOrdered{Ascending,Same,Descending} are the same as
    // expected return values of wxCmpNatural(), so we don't need to convert.
    return [wxCFStringRef(s1).AsNSString() localizedStandardCompare: wxCFStringRef(s2).AsNSString()];
}
