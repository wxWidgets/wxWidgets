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

- (id)init;
- (id)initWithWxTimer:(wxTimer*)theTimer;
- (wxTimer*)timer;
@end // interface wxNSTimerData : NSObject

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

    wxNSTimerData *userInfo = [[wxNSTimerData alloc] initWithWxTimer:this];
    m_cocoaNSTimer =     [[NSTimer 
            scheduledTimerWithTimeInterval: millisecs / 1000.0 //seconds
            target:		wxTimer::sm_cocoaDelegate
            selector:	@selector(onNotify:) 
            userInfo:	userInfo
            repeats:	oneShot == false] retain];
    [userInfo release];
                       
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

