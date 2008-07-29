/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/cocoa/utils.mm
// Purpose:     various cocoa utility functions
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: utils.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #if wxUSE_GUI
        #include "wx/toplevel.h"
        #include "wx/font.h"
    #endif
#endif

#include "wx/apptrait.h"

#include "wx/osx/private.h"

#if wxUSE_GUI
#if wxOSX_USE_COCOA_OR_CARBON
    #include "wx/osx/uma.h"
    #include <CoreServices/CoreServices.h>
    #include <Carbon/Carbon.h>
    #include "wx/osx/private/timer.h"
#endif
#endif // wxUSE_GUI

#if wxOSX_USE_COCOA

#if wxUSE_BASE

// Emit a beeeeeep
void wxBell()
{
    NSBeep();
}

// ----------------------------------------------------------------------------
// Common Event Support
// ----------------------------------------------------------------------------

void wxMacWakeUp()
{
    // TODO
}

#endif // wxUSE_BASE

#if wxUSE_GUI

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    NSRect displayRect = [[NSScreen mainScreen] visibleFrame];
    wxRect r = wxFromNSRect( NULL, displayRect );
    if ( x )
        *x = r.x;
    if ( y )
        *y = r.y;
    if ( width )
        *width = r.GetWidth();
    if ( height )
        *height = r.GetHeight();
    
}

void wxGetMousePosition( int* x, int* y )
{
    wxPoint pt = wxFromNSPoint(NULL, [NSEvent mouseLocation]);
};

wxTimerImpl* wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxCarbonTimerImpl(timer);
}

int gs_wxBusyCursorCount = 0;
extern wxCursor    gMacCurrentCursor;
wxCursor        gMacStoredActiveCursor;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(const wxCursor *cursor)
{
    if (gs_wxBusyCursorCount++ == 0)
    {
        gMacStoredActiveCursor = gMacCurrentCursor;
        cursor->MacInstall();

        wxSetCursor(*cursor);
    }
    //else: nothing to do, already set
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    wxCHECK_RET( gs_wxBusyCursorCount > 0,
        wxT("no matching wxBeginBusyCursor() for wxEndBusyCursor()") );

    if (--gs_wxBusyCursorCount == 0)
    {
        gMacStoredActiveCursor.MacInstall();
        gMacStoredActiveCursor = wxNullCursor;

        wxSetCursor(wxNullCursor);
    }
}

// true if we're between the above two calls
bool wxIsBusy()
{
    return (gs_wxBusyCursorCount > 0);
}

void wxMacGlobalToLocal( WindowRef window , Point*pt )
{
}

void wxMacLocalToGlobal( WindowRef window , Point*pt )
{
}

#endif // wxUSE_GUI



#endif // wxOSX_USE_COCOA