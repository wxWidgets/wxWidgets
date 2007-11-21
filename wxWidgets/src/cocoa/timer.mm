/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/timer.mm
// Purpose:     wxTimer for wxCocoa
// Author:      Ryan Norton
// Modified by: David Elliott
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

#include "wx/timer.h"

#ifndef WX_PRECOMP
#endif

#include "wx/cocoa/autorelease.h"

#include "wx/cocoa/objc/objc_uniquifying.h"

#import <Foundation/NSTimer.h>

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_CLASS(wxTimer, wxTimerBase)

// ========================================================================
// wxNSTimerData
// ========================================================================
@interface wxNSTimerData : NSObject
{
    wxTimer* m_timer;
}

- (id)init;
- (id)initWithWxTimer:(wxTimer*)theTimer;
- (wxTimer*)timer;
- (void)onNotify:(NSTimer *)theTimer;
@end // interface wxNSTimerData : NSObject
WX_DECLARE_GET_OBJC_CLASS(wxNSTimerData,NSObject)

@implementation wxNSTimerData : NSObject
- (id)init
{
    if(!(self = [super init]))
        return nil;
    m_timer = NULL;
    return self;
}

- (id)initWithWxTimer:(wxTimer*)theTimer;
{
    if(!(self = [super init]))
        return nil;
    m_timer = theTimer;
    return self;
}

- (wxTimer*)timer
{
    return m_timer;
}

- (void)onNotify:(NSTimer *)theTimer
{
    m_timer->Notify(); //wxTimerBase method
}
@end
WX_IMPLEMENT_GET_OBJC_CLASS(wxNSTimerData,NSObject)

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

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

    wxNSTimerData *timerData = [[WX_GET_OBJC_CLASS(wxNSTimerData) alloc] initWithWxTimer:this];
    m_cocoaNSTimer =     [[NSTimer
            scheduledTimerWithTimeInterval: millisecs / 1000.0 //seconds
            target:     timerData
            selector:   @selector(onNotify:)
            userInfo:   nil
            repeats:    oneShot == false] retain];
    [timerData release];

    return IsRunning();
}

void wxTimer::Stop()
{
    if (m_cocoaNSTimer)
    {
        // FIXME: Is this safe to do if !isValid ?
        [m_cocoaNSTimer invalidate];
        [m_cocoaNSTimer release];
        m_cocoaNSTimer = NULL;
    }
}

bool wxTimer::IsRunning() const
{
    return m_cocoaNSTimer != NULL && [m_cocoaNSTimer isValid];
}

#endif // wxUSE_TIMER

