/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/timer.mm
// Purpose:     wxTimer for wxCocoa
// Author:      Ryan Norton
// Modified by:
// Created:     2005-02-04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TIMER

#ifndef WX_PRECOMP
    #include "wx/timer.h"
#endif

#include "wx/cocoa/autorelease.h"

#import <Foundation/NSTimer.h>

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_CLASS(wxTimer, wxTimerBase)

// ========================================================================
// wxNSTimerDelegate
// ========================================================================
@interface wxNSTimerDelegate : NSObject
{
}

- (void)onNotify:(NSTimer *)theTimer;
@end // interface wxNSTimerDelegate : NSObject

// ========================================================================
// wxNSTimerData
// ========================================================================
@interface wxNSTimerData : NSObject
{
    wxTimer* m_timer;
}

- (id)setTimer:(wxTimer*)theTimer;
- (wxTimer*)timer;
@end // interface wxNSTimerData : NSObject

@implementation wxNSTimerData : NSObject
- (id)setTimer:(wxTimer*)theTimer;
{
    m_timer = theTimer;
    return self;
}
- (wxTimer*)timer
{
    return m_timer;
}
@end 

@implementation wxNSTimerDelegate : NSObject
- (void)onNotify:(NSTimer *)theTimer
{
    wxNSTimerData* theData = [theTimer userInfo];
    [theData timer]->Notify(); //wxTimerBase method
}
@end 

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

const wxObjcAutoRefFromAlloc<struct objc_object*> wxTimer::sm_cocoaDelegate = [[wxNSTimerDelegate alloc] init];

wxTimer::~wxTimer()
{
    Stop();
}

void wxTimer::Init()
{
    m_cocoaNSTimer = NULL;
}

bool wxTimer::Start(int millisecs, bool oneShot)
{
    Stop();
    
    wxAutoNSAutoreleasePool thePool;

    m_cocoaNSTimer =     [[NSTimer 
            scheduledTimerWithTimeInterval: millisecs / 1000.0 //seconds
            target:		wxTimer::sm_cocoaDelegate
            selector:	@selector(onNotify:) 
            userInfo:	[[wxNSTimerData alloc] setTimer:this]
            repeats:	oneShot == false] retain];
                       
    return IsRunning();
}

void wxTimer::Stop()
{
    if (m_cocoaNSTimer)
    {
        NSObject* theUserInfo = [m_cocoaNSTimer userInfo];
        [m_cocoaNSTimer invalidate];
        [theUserInfo release];
        [m_cocoaNSTimer release];
        m_cocoaNSTimer = NULL;
    }
}

bool wxTimer::IsRunning() const
{
    return m_cocoaNSTimer != NULL && [m_cocoaNSTimer isValid];
}

#endif // wxUSE_TIMER

