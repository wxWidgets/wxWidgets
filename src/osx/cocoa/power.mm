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

#ifndef WX_PRECOMP
  #include "wx/module.h"
  #include "wx/window.h"
#endif

#include "wx/power.h"
#include "wx/atomic.h"
#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

#include <IOKit/pwr_mgt/IOPMLib.h>

#import <Cocoa/Cocoa.h>

static wxAtomicInt g_powerResourceSystemRefCount = 0;

// ----------------------------------------------------------------------------
// wxCocoaPowerModule
// ----------------------------------------------------------------------------

@interface wxCocoaPowerEventsObserver : NSObject
@end

@implementation wxCocoaPowerEventsObserver

- (id)init {
    self = [super init];
    if (self) {
        NSNotificationCenter *center = [[NSWorkspace sharedWorkspace] notificationCenter];

        // Register for "will sleep" notification
        [center addObserver:self
                   selector:@selector(handleWillSleep:)
                       name:NSWorkspaceWillSleepNotification
                     object:nil];

        // Register for "did wake" notification
        [center addObserver:self
                   selector:@selector(handleDidWake:)
                       name:NSWorkspaceDidWakeNotification
                     object:nil];
    }
    return self;
}

- (void)dealloc {
    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
    [super dealloc];
}

- (void)handleWillSleep:(NSNotification *)notification {
    wxPowerEvent event(wxEVT_POWER_SUSPENDED);

    for ( auto tlw : wxTopLevelWindows )
        tlw->HandleWindowEvent(event);
}

- (void)handleDidWake:(NSNotification *)notification {
    wxPowerEvent event(wxEVT_POWER_RESUME);

    for ( auto tlw : wxTopLevelWindows )
        tlw->HandleWindowEvent(event);
}

@end

namespace {
// the global power events observer
static wxCFRef<wxCocoaPowerEventsObserver*> g_powerEventsObserver;

// add power observers on init and remove on exit
class wxCocoaPowerModule : public wxModule
{
public:
    bool OnInit() override { return true; }
    void OnExit() override
    {
        // There should be no other threads by now, so no locking is needed.
        if ( g_powerResourceSystemRefCount )
        {
            g_powerEventsObserver.reset();

            g_powerResourceSystemRefCount = 0;
        }
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxCocoaPowerModule);
};
}; // anonymous namespace

// ----------------------------------------------------------------------------
// wxPowerResource
// ----------------------------------------------------------------------------

static NSObject* g_processInfoActivity = nil;

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
        }

        g_powerEventsObserver.reset();
    }

    return true;
}

bool
wxPowerResource::Acquire(wxPowerResourceKind kind,
                         const wxString& reason,
                         wxPowerBlockKind blockKind)
{
    if ( blockKind == wxPOWER_DELAY )
    {
        if ( kind == wxPOWER_RESOURCE_SYSTEM )
        {
            wxAtomicInc(g_powerResourceSystemRefCount);

            if ( g_powerEventsObserver.get() == nullptr )
            {
                g_powerEventsObserver = [[wxCocoaPowerEventsObserver alloc] init];
            }
        }

        return true;
    }

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
