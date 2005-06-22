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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP


#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>

#include "wx/uri.h"
#include "wx/mac/corefoundation/cfstring.h"

long wxMacExecute(wxChar **argv,
               int flags,
               wxProcess *process)
{
	const long errorCode = ((flags & wxEXEC_SYNC) ? -1 : 0);
	const long successCode = ((flags & wxEXEC_SYNC) ? 0 : -1); // fake PID

    CFIndex cfiCount = 0;
    //get count
    for(wxChar** argvcopy = argv; *argvcopy != NULL ; ++argvcopy)
    {
        ++cfiCount;
    }

    if(cfiCount == 0) //no file to launch?
    {
        wxLogDebug(wxT("wxMacExecute No file to launch!"));
        return errorCode ;
    }
    
    CFURLRef cfurlApp = CFURLCreateWithString(
            kCFAllocatorDefault,
            wxMacCFStringHolder(*argv++, wxLocale::GetSystemEncoding()),
            NULL);
    wxASSERT(cfurlApp);

    CFBundleRef cfbApp = CFBundleCreate(kCFAllocatorDefault, cfurlApp);
    if(!cfbApp)
    {
        wxLogDebug(wxT("wxMacExecute Bad bundle"));
        CFRelease(cfurlApp);
        return errorCode ;
    }
    
    
    UInt32 dwBundleType, dwBundleCreator;
    CFBundleGetPackageInfo(cfbApp, &dwBundleType, &dwBundleCreator);

    //Only call wxMacExecute for .app bundles - others could be actual unix programs
    if(dwBundleType != 'APPL')
    {
        CFRelease(cfurlApp);
        return errorCode ;
    }
    
    //
    // We have a good bundle - so let's launch it!
    //
    
    CFMutableArrayRef cfaFiles =
        CFArrayCreateMutable(kCFAllocatorDefault, cfiCount - 1, &kCFTypeArrayCallBacks);
            
    wxASSERT(cfaFiles);
    
    if(--cfiCount)
    {
        for( ; *argv != NULL ; ++argv)
        {
//            wxLogDebug(*argv);
            wxString sCurrentFile;
            
            if(wxURI(*argv).IsReference())
                sCurrentFile = wxString(wxT("file://")) + *argv;
            else
                sCurrentFile = *argv;
                
            CFURLRef cfurlCurrentFile =   CFURLCreateWithString(
                    kCFAllocatorDefault,
                    wxMacCFStringHolder(sCurrentFile, wxLocale::GetSystemEncoding()),
                    NULL);
            wxASSERT(cfurlCurrentFile);

            CFArrayAppendValue(
                cfaFiles,
                cfurlCurrentFile
                            );
            CFRelease(cfurlCurrentFile); // array has retained it
        }
    }
    
    LSLaunchURLSpec launchspec;
    launchspec.appURL = cfurlApp;
    launchspec.itemURLs = cfaFiles;
    launchspec.passThruParams = NULL; //AEDesc* 
    launchspec.launchFlags = kLSLaunchDefaults | kLSLaunchDontSwitch;  //TODO:  Possibly be smarter with flags
    launchspec.asyncRefCon = NULL;
    
    OSStatus status = LSOpenFromURLSpec(&launchspec,
                        NULL); //2nd is CFURLRef* really launched

    //cleanup
    CFRelease(cfurlApp);
    CFRelease(cfaFiles);
    
    //check for error
    if(status != noErr)
    {
        wxLogDebug(wxT("wxMacExecute ERROR: %d"), (int)status);
        return errorCode ;
    }
    return successCode; //success
}

