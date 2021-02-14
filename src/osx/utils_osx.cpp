/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/utils_osx.cpp
// Purpose:     Various utilities
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/log.h"
    #if wxUSE_GUI
        #include "wx/toplevel.h"
        #include "wx/font.h"
    #endif
#endif

#include "wx/apptrait.h"

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// #include "MoreFilesX.h"

#include <AudioToolbox/AudioServices.h>

#if wxUSE_GUI
    #include "wx/private/launchbrowser.h"
#endif

#include "wx/osx/private.h"
#include "wx/osx/private/timer.h"

#include "wx/evtloop.h"

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *WXUNUSED(wnd))
{
    // TODO
    return false;
}

#if wxOSX_USE_COCOA_OR_CARBON

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// Launch document with default app
// ----------------------------------------------------------------------------

bool wxLaunchDefaultApplication(const wxString& document, int flags)
{
    wxUnusedVar(flags);

    wxCFRef<CFURLRef> curl(wxOSXCreateURLFromFileSystemPath(document));
    OSStatus err = LSOpenCFURLRef( curl , NULL );

    if (err == noErr)
    {
        return true;
    }
    else
    {
        wxLogDebug(wxT("Default Application Launch error %d"), (int) err);
        return false;
    }
}

// ----------------------------------------------------------------------------
// Launch default browser
// ----------------------------------------------------------------------------

bool wxDoLaunchDefaultBrowser(const wxLaunchBrowserParams& params)
{
    wxCFRef< CFURLRef > curl( CFURLCreateWithString( kCFAllocatorDefault,
                              wxCFStringRef( params.url ), NULL ) );
    OSStatus err = LSOpenCFURLRef( curl , NULL );

    if (err == noErr)
    {
        return true;
    }
    else
    {
        wxLogDebug(wxT("Browser Launch error %d"), (int) err);
        return false;
    }
}

#endif // wxUSE_GUI

#endif

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj,
                                           int *verMin,
                                           int *verMicro) const
{
    // We suppose that toolkit version is the same as OS version under Mac
    wxGetOsVersion(verMaj, verMin, verMicro);

    return wxPORT_OSX;
}

wxEventLoopBase* wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop;
}

wxWindow* wxFindWindowAtPoint(wxWindow* win, const wxPoint& pt);

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint( pt );
}

/*
    Return the generic RGB color space. This is a 'get' function and the caller should
    not release the returned value unless the caller retains it first. Usually callers
    of this routine will immediately use the returned colorspace with CoreGraphics
    so they typically do not need to retain it themselves.

    This function creates the generic RGB color space once and hangs onto it so it can
    return it whenever this function is called.
*/

CGColorSpaceRef wxMacGetGenericRGBColorSpace()
{
    static wxCFRef<CGColorSpaceRef> genericRGBColorSpace;

    if (genericRGBColorSpace == NULL)
    {
#if wxOSX_USE_IPHONE
        genericRGBColorSpace.reset( CGColorSpaceCreateDeviceRGB() );
#else
        genericRGBColorSpace.reset( CGColorSpaceCreateWithName( kCGColorSpaceSRGB ) );
#endif
    }

    return genericRGBColorSpace;
}

#if wxOSX_USE_COCOA_OR_CARBON

CGColorRef wxMacCreateCGColorFromHITheme( ThemeBrush brush )
{
    const int maxcachedbrush = 58+5; // negative indices are for metabrushes, cache down to -5)
    int brushindex = brush+5;
    if ( brushindex < 0 || brushindex > maxcachedbrush )
    {
        CGColorRef color ;
        HIThemeBrushCreateCGColor( brush, &color );
        return color;
    }
    else
    {
        static bool inited = false;
        static CGColorRef themecolors[maxcachedbrush+1];
        if ( !inited )
        {
            for ( int i = 0 ; i <= maxcachedbrush ; ++i )
                HIThemeBrushCreateCGColor( i-5, &themecolors[i] );
            inited = true;
        }
        return CGColorRetain(themecolors[brushindex ]);
    }
}

//---------------------------------------------------------------------------
// Mac Specific string utility functions
//---------------------------------------------------------------------------

void wxMacStringToPascal( const wxString&from , unsigned char * to )
{
    wxCharBuffer buf = from.mb_str( wxConvLocal );
    size_t len = buf.length();

    if ( len > 255 )
        len = 255;
    to[0] = len;
    memcpy( (char*) &to[1] , buf , len );
}

wxString wxMacMakeStringFromPascal( const unsigned char * from )
{
    return wxString(&from[1], wxConvLocal, from[0]);
}

#endif // wxOSX_USE_COCOA_OR_CARBON
