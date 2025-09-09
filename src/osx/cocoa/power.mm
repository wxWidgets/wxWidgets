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

// ----------------------------------------------------------------------------
// wxPowerResource
// ----------------------------------------------------------------------------

static wxAtomicInt g_powerResourceSystemRefCount = 0;
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

            return true;
        }
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
        // We don't support this mode under macOS because it's not needed there.
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

#import <Cocoa/Cocoa.h>
#import "wx/power.h" // Assuming you have wxPowerEvent defined or a custom event

@interface CocoaPowerEventsObserver : NSObject
@end

@implementation CocoaPowerEventsObserver

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
// add power observers on init and remove on exit
class wxCocoaPowerModule : public wxModule
{
wxDECLARE_DYNAMIC_CLASS(wxCocoaPowerModule);

public:
    bool OnInit() override
    {
         m_powerObserver = [[CocoaPowerEventsObserver alloc] init];
         return true;
    }

    void OnExit() override
    {
         if (m_powerObserver != nullptr)
         {
             [m_powerObserver release];
             m_powerObserver = nullptr;
         }
    }

private:
    CocoaPowerEventsObserver* m_powerObserver;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxCocoaPowerModule, wxModule)

}; // anonymous namespace
