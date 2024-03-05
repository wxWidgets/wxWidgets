/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/power.mm
// Purpose:     wxPowerResource implementation for OSX
// Author:      Tobias Taschner
// Created:     2014-08-09
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#include "wx/power.h"
#include "wx/atomic.h"
#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

#include <IOKit/pwr_mgt/IOPMLib.h>

// ----------------------------------------------------------------------------
// wxPowerResource
// ----------------------------------------------------------------------------

wxAtomicInt g_powerResourceSystemRefCount = 0;

IOPMAssertionID g_pmAssertionID = 0;
NSObject* g_processInfoActivity = nil;

bool UpdatePowerResourceUsage(wxPowerResourceKind kind, const wxString& reason)
{
    if ( g_powerResourceSystemRefCount >= 1 )
    {
        wxCFStringRef cfreason(reason);
        if( reason.IsEmpty())
            cfreason = wxString("User Activity");

        if ( !g_processInfoActivity )
        {
            NSActivityOptions
                options = NSActivityUserInitiated |
                          NSActivityIdleSystemSleepDisabled;

            if ( kind == wxPOWER_RESOURCE_SCREEN )
                options |= NSActivityIdleDisplaySleepDisabled;

            g_processInfoActivity = [[NSProcessInfo processInfo]
                                     beginActivityWithOptions:options
                                     reason:cfreason.AsNSString()];
            [g_processInfoActivity retain];
            return true;
        }
    }
    else if ( g_powerResourceSystemRefCount == 0 )
    {
        if ( g_processInfoActivity )
        {
            [[NSProcessInfo processInfo]
             endActivity:(id)g_processInfoActivity];
            g_processInfoActivity = nil;

            return true;
        }
    }

    return true;
}

bool wxPowerResource::Acquire(wxPowerResourceKind kind, const wxString& reason)
{
    wxAtomicInc(g_powerResourceSystemRefCount);

    bool success = UpdatePowerResourceUsage(kind, reason);
    if (!success)
        wxAtomicDec(g_powerResourceSystemRefCount);

    return success;
}

void wxPowerResource::Release(wxPowerResourceKind kind)
{
    switch ( kind )
    {
        case wxPOWER_RESOURCE_SCREEN:
        case wxPOWER_RESOURCE_SYSTEM:
            if ( g_powerResourceSystemRefCount > 0 )
            {
                wxAtomicDec(g_powerResourceSystemRefCount);
            }
            else
            {
                wxFAIL_MSG("Power resource was not acquired");
            }

            UpdatePowerResourceUsage(kind, "");
            break;
    }
}
