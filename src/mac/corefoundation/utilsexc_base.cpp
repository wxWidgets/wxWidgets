/////////////////////////////////////////////////////////////////////////////
// Name:        mac/corefoundation/utilsexc_base.cpp
// Purpose:     wxMacExecute
// Author:      Ryan Norton
// Modified by:
// Created:     2005-06-21
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
// Notes:       Source was originally in utilsexc_cf.cpp,1.6 then moved
//              to totally unrelated hid.cpp,1.8.
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// WX includes
#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

// Mac Includes
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

// More WX Includes
#include "wx/filename.h"
#include "wx/mac/corefoundation/cfstring.h"

// Default path style
#ifdef __WXMAC_OSX__
#define kDefaultPathStyle kCFURLPOSIXPathStyle
#else
#define kDefaultPathStyle kCFURLHFSPathStyle
#endif

//===========================================================================
//  IMPLEMENTATION
//===========================================================================

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//    wxMacExecute
//
// argv is the command line split up, with the application path first
// flags are the flags from wxExecute
// process is the process passed from wxExecute for pipe streams etc.
// returns -1 on error for wxEXEC_SYNC and 0 on error for wxEXEC_ASYNC
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long wxMacExecute(wxChar **argv,
               int flags,
               wxProcess *process)
{
    // Semi-macros used for return value of wxMacExecute
    const long errorCode = ((flags & wxEXEC_SYNC) ? -1 : 0);
    const long successCode = ((flags & wxEXEC_SYNC) ? 0 : -1); // fake PID

    // Obtains the number of arguments for determining the size of
    // the CFArray used to hold them
    CFIndex cfiCount = 0;
    for(wxChar** argvcopy = argv; *argvcopy != NULL ; ++argvcopy)
    {
        ++cfiCount;
    }

    // If there is not a single argument then there is no application
    // to launch
    if(cfiCount == 0)
    {
        wxLogDebug(wxT("wxMacExecute No file to launch!"));
        return errorCode ;
    }

    // Path to bundle
    wxString path = *argv++;

    // Create a CFURL for the application path
    // Created this way because we are opening a bundle which is a directory
    CFURLRef cfurlApp =
        CFURLCreateWithFileSystemPath(
            kCFAllocatorDefault,
            wxMacCFStringHolder(path),
            kDefaultPathStyle,
            true); //false == not a directory

    // Check for error from the CFURL
    if(!cfurlApp)
    {
        wxLogDebug(wxT("wxMacExecute Can't open path: %s"), path.c_str());
        return errorCode ;
    }

    // Create a CFBundle from the CFURL created earlier
    CFBundleRef cfbApp = CFBundleCreate(kCFAllocatorDefault, cfurlApp);

    // Check to see if CFBundleCreate returned an error,
    // and if it did this was an invalid bundle or not a bundle
    // at all (maybe a simple directory etc.)
    if(!cfbApp)
    {
        wxLogDebug(wxT("wxMacExecute Bad bundle: %s"), path.c_str());
        CFRelease(cfurlApp);
        return errorCode ;
    }

    // Get the bundle type and make sure its an 'APPL' bundle
    // Otherwise we're dealing with something else here...
    UInt32 dwBundleType, dwBundleCreator;
    CFBundleGetPackageInfo(cfbApp, &dwBundleType, &dwBundleCreator);
    if(dwBundleType != 'APPL')
    {
        wxLogDebug(wxT("wxMacExecute Not an APPL bundle: %s"), path.c_str());
        CFRelease(cfbApp);
        CFRelease(cfurlApp);
        return errorCode ;
    }

    // Create a CFArray for dealing with the command line
    // arguments to the bundle
    CFMutableArrayRef cfaFiles = CFArrayCreateMutable(kCFAllocatorDefault,
                                    cfiCount-1, &kCFTypeArrayCallBacks);
    if(!cfaFiles) //This should never happen
    {
        wxLogDebug(wxT("wxMacExecute Could not create CFMutableArray"));
        CFRelease(cfbApp);
        CFRelease(cfurlApp);
        return errorCode ;
    }

    // Loop through command line arguments to the bundle,
    // turn them into CFURLs and then put them in cfaFiles
    // For use to launch services call
        for( ; *argv != NULL ; ++argv)
        {
        // Check for '<' as this will ring true for
        // CFURLCreateWithString but is generally not considered
        // typical on mac but is usually passed here from wxExecute
        if (wxStrcmp(*argv, wxT("<")) == 0)
            continue;


        CFURLRef cfurlCurrentFile;    // CFURL to hold file path
        wxFileName argfn(*argv);     // Filename for path

        if(argfn.DirExists())
        {
            // First, try creating as a directory
            cfurlCurrentFile = CFURLCreateWithFileSystemPath(
                                kCFAllocatorDefault,
                                wxMacCFStringHolder(*argv),
                                kDefaultPathStyle,
                                true); //true == directory
        }
        else if(argfn.FileExists())
        {
            // And if it isn't a directory try creating it
            // as a regular file
            cfurlCurrentFile = CFURLCreateWithFileSystemPath(
                                kCFAllocatorDefault,
                                wxMacCFStringHolder(*argv),
                                kDefaultPathStyle,
                                false); //false == regular file
        }
        else
        {
            // Argument did not refer to
            // an entry in the local filesystem,
            // so try creating it through CFURLCreateWithString
            cfurlCurrentFile = CFURLCreateWithString(
                                kCFAllocatorDefault,
                                wxMacCFStringHolder(*argv),
                                NULL);
        }

        // Continue in the loop if the CFURL could not be created
        if(!cfurlCurrentFile)
        {
            wxLogDebug(
                wxT("wxMacExecute Could not create CFURL for argument:%s"),
                *argv);
            continue;
        }

        // Add the valid CFURL to the argument array and then
        // release it as the CFArray adds a ref count to it
            CFArrayAppendValue(
                cfaFiles,
                cfurlCurrentFile
                            );
            CFRelease(cfurlCurrentFile); // array has retained it
        }

    // Create a LSLaunchURLSpec for use with LSOpenFromURLSpec
    // Note that there are several flag options (launchFlags) such
    // as kLSLaunchDontSwitch etc. and maybe we could be more
    // picky about the flags we choose
    LSLaunchURLSpec launchspec;
    launchspec.appURL = cfurlApp;
    launchspec.itemURLs = cfaFiles;
    launchspec.passThruParams = NULL; //AEDesc*
    launchspec.launchFlags = kLSLaunchDefaults;
    launchspec.asyncRefCon = NULL;

    // Finally, call LSOpenFromURL spec with our arguments
    // 2nd parameter is a pointer to a CFURL that gets
    // the actual path launched by the function
    OSStatus status = LSOpenFromURLSpec(&launchspec, NULL);

    // Cleanup corefoundation references
    CFRelease(cfbApp);
    CFRelease(cfurlApp);
    CFRelease(cfaFiles);

    // Check for error from LSOpenFromURLSpec
    if(status != noErr)
    {
        wxLogDebug(wxT("wxMacExecute LSOpenFromURLSpec Error: %d"),
                   (int)status);
        return errorCode ;
    }

    // No error from LSOpenFromURLSpec, so app was launched
    return successCode;
}

