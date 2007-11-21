/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/utils.cpp
// Purpose:     Various utilities
// Author:      AUTHOR
// Modified by:
// Created:     2003/??/??
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/display.h"

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "wx/cocoa/string.h"

#import <Foundation/NSURL.h>
#import <AppKit/NSWorkspace.h>

void wxDisplaySize(int *width, int *height)
{
    // TODO
    if(width)
        *width = 1024;
    if(height)
        *height = 768;
}

void wxDisplaySizeMM(int*,int*)
{
    // TODO
}

void wxClientDisplayRect(int *x,int *y,int *width,int *height)
{
    // TODO
    if(x)
        *x = 0;
    if(y)
        *y = 0;
    if(width)
        *width=1024;
    if(height)
        *height=768;
}

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj, int *verMin) const
{
    // We suppose that toolkit version is the same as OS version under Mac
    wxGetOsVersion(verMaj, verMin);

    return wxPORT_COCOA;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

// Return true if we have a colour display
bool wxColourDisplay()
{
    // TODO
    return true;
}

void wxGetMousePosition( int* x, int* y )
{
    // TODO
};

// Returns depth of screen
int wxDisplayDepth()
{
    // TODO
    return 0;
}

// Emit a beeeeeep
void wxBell()
{
    // TODO
}

#include "wx/private/browserhack28.h"

// Private helper method for wxLaunchDefaultBrowser
static bool wxCocoaLaunchDefaultBrowser(const wxString& url, int flags)
{
    // NOTE: We ignore the flags
    return [[NSWorkspace sharedWorkspace] openURL: [NSURL URLWithString:wxNSStringWithWxString(url)]] != NO;
}

// For this to work the linker (particularly the static one) must link in this object file and the C++
// runtime must call the static initializer.  It is basically guaranteed that the linker won't throw us
// away because some of the wxGUIAppTraits virtual methods are implemented in this file.
static class DoFixLaunchDefaultBrowser
{
public:
    DoFixLaunchDefaultBrowser()
    {
        // Tell the base library we can launch a default browser better.
        wxSetLaunchDefaultBrowserImpl(&wxCocoaLaunchDefaultBrowser);
    }
} s_launchDefaultBrowserFixer;

#if 0
// DFE: These aren't even implemented by wxGTK, and no wxWidgets code calls
// them.  If someone needs them, then they'll get a link error

// Consume all events until no more left
void wxFlushEvents()
{
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
    // TODO
    return false;
}

#endif

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
    // TODO
    return false;
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
    char buf[50];
    sprintf(buf, "%.4f", value);
    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
    char buf[50];
    sprintf(buf, "%ld", value);
    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
    char buf[50];
    sprintf(buf, "%d", value);
    return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file)
{
    // TODO
    return false;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
    char *s = NULL;
    bool succ = wxGetResource(section, entry, (char **)&s, file);
    if (succ)
    {
        *value = (float)strtod(s, NULL);
        delete[] s;
        return true;
    }
    else return false;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
    char *s = NULL;
    bool succ = wxGetResource(section, entry, (char **)&s, file);
    if (succ)
    {
        *value = strtol(s, NULL, 10);
        delete[] s;
        return true;
    }
    else return false;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
    char *s = NULL;
    bool succ = wxGetResource(section, entry, (char **)&s, file);
    if (succ)
    {
        *value = (int)strtol(s, NULL, 10);
        delete[] s;
        return true;
    }
    else return false;
}
#endif // wxUSE_RESOURCES
