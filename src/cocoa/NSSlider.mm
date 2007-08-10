/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/NSSlider.mm
// Purpose:     wxCocoaNSSlider class
// Author:      Mark Oxenham
// Modified by:
// Created:     2007/08/10
// RCS-ID:      $Id: $
// Copyright:   (c) 2007 Software 2000 Ltd. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cocoa/ObjcPose.h"
#include "wx/cocoa/NSSlider.h"

#import <Foundation/NSNotification.h>
#import <Foundation/NSString.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSSlider.h>

WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSSlider)

// ============================================================================
// @class wxNSSliderTarget
// ============================================================================
@interface wxNSSliderTarget : NSObject
{
}

- (void)wxNSSliderUpArrowKeyDown: (id)sender;
- (void)wxNSSliderDownArrowKeyDown: (id)sender;
- (void)wxNSSliderLeftArrowKeyDown: (id)sender;
- (void)wxNSSliderRightArrowKeyDown: (id)sender;
- (void)wxNSSliderPageUpKeyDown: (id)sender;
- (void)wxNSSliderPageDownKeyDown: (id)sender;
- (void)wxNSSliderMoveUp: (id)sender;
- (void)wxNSSliderMoveDown: (id)sender;
- (void)wxNSSliderMoveLeft: (id)sender;
- (void)wxNSSliderMoveRight: (id)sender;
- (void)wxNSSliderPageUp: (id)sender;
- (void)wxNSSliderPageDown: (id)sender;
@end // wxNSSliderTarget

@implementation wxNSSliderTarget : NSObject

- (void)wxNSSliderUpArrowKeyDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderUpArrowKeyDown received without associated wx object"));
    slider->Cocoa_wxNSSliderUpArrowKeyDown();
}

- (void)wxNSSliderDownArrowKeyDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderDownArrowKeyDown received without associated wx object"));
    slider->Cocoa_wxNSSliderDownArrowKeyDown();
}

- (void)wxNSSliderLeftArrowKeyDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderLeftArrowKeyDown received without associated wx object"));
    slider->Cocoa_wxNSSliderLeftArrowKeyDown();
}

- (void)wxNSSliderRightArrowKeyDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderRightArrowKeyDown received without associated wx object"));
    slider->Cocoa_wxNSSliderRightArrowKeyDown();
}

- (void)wxNSSliderPageUpKeyDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderPageUpKeyDown received without associated wx object"));
    slider->Cocoa_wxNSSliderPageUpKeyDown();
}

- (void)wxNSSliderPageDownKeyDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderPageDownKeyDown received without associated wx object"));
    slider->Cocoa_wxNSSliderPageDownKeyDown();
}

- (void)wxNSSliderMoveUp: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderMoveUp received without associated wx object"));
    slider->Cocoa_wxNSSliderMoveUp();
}

- (void)wxNSSliderMoveDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderMoveDown received without associated wx object"));
    slider->Cocoa_wxNSSliderMoveDown();
}

- (void)wxNSSliderMoveLeft: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderMoveLeft received without associated wx object"));
    slider->Cocoa_wxNSSliderMoveLeft();
}

- (void)wxNSSliderMoveRight: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderMoveRight received without associated wx object"));
    slider->Cocoa_wxNSSliderMoveRight();
}

- (void)wxNSSliderPageUp: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderPageUp received without associated wx object"));
    slider->Cocoa_wxNSSliderPageUp();
}

- (void)wxNSSliderPageDown: (id)sender
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(sender);
    wxCHECK_RET(slider,wxT("wxNSSliderPageDown received without associated wx object"));
    slider->Cocoa_wxNSSliderPageDown();
}

@end // implementation wxNSSliderTarget

// ============================================================================
// @class wxPoserNSSlider
// ============================================================================

@interface wxPoserNSSlider : NSSlider
@end

WX_IMPLEMENT_POSER(wxPoserNSSlider);
@implementation wxPoserNSSlider : NSSlider

- (void)keyDown:(NSEvent *)theEvent
{
    SEL originalAction = [self action];
    SEL newAction = originalAction;
    NSString *theEventCharacters = [theEvent charactersIgnoringModifiers];

    if ([theEventCharacters length] == 1)
    {
        switch ([theEventCharacters characterAtIndex:0])
        {
            case NSUpArrowFunctionKey:      newAction = @selector(wxNSSliderUpArrowKeyDown:);       break;
            case NSDownArrowFunctionKey:    newAction = @selector(wxNSSliderDownArrowKeyDown:);     break;
            case NSLeftArrowFunctionKey:    newAction = @selector(wxNSSliderLeftArrowKeyDown:);     break;
            case NSRightArrowFunctionKey:   newAction = @selector(wxNSSliderRightArrowKeyDown:);    break;
            case NSPageUpFunctionKey:       newAction = @selector(wxNSSliderPageUpKeyDown:);        break;
            case NSPageDownFunctionKey:     newAction = @selector(wxNSSliderPageDownKeyDown:);      break;
            default:                                                                                break;
        }
    }

    [self setAction:newAction];
    [super keyDown:theEvent];
    [self setAction:originalAction];
}

- (void)moveUp:(id)sender
{
    SEL originalAction = [self action];

    [self setAction:@selector(wxNSSliderMoveUp:)];
    [super moveUp:sender];
    [self setAction:originalAction];
}

- (void)moveDown:(id)sender
{
    SEL originalAction = [self action];

    [self setAction:@selector(wxNSSliderMoveDown:)];
    [super moveDown:sender];
    [self setAction:originalAction];
}

- (void)moveLeft:(id)sender
{
    SEL originalAction = [self action];

    [self setAction:@selector(wxNSSliderMoveLeft:)];
    [super moveLeft:sender];
    [self setAction:originalAction];
}

- (void)moveRight:(id)sender
{
    SEL originalAction = [self action];

    [self setAction:@selector(wxNSSliderMoveRight:)];
    [super moveRight:sender];
    [self setAction:originalAction];
}

- (void)pageUp:(id)sender
{
    SEL originalAction = [self action];

    [self setAction:@selector(wxNSSliderPageUp:)];
    [super pageUp:sender];
    [self setAction:originalAction];
}

- (void)pageDown:(id)sender
{
    SEL originalAction = [self action];

    [self setAction:@selector(wxNSSliderPageDown:)];
    [super pageDown:sender];
    [self setAction:originalAction];
}

@end

// ============================================================================
// @class wxPoserNSSliderCell
// ============================================================================

#define kwxNSSliderStartTracking    @"wxNSSliderStartTracking"
#define kwxNSSliderContinueTracking @"wxNSSliderContinueTracking"
#define kwxNSSliderStopTracking     @"wxNSSliderStopTracking"

@interface wxPoserNSSliderCell : NSSliderCell
@end

WX_IMPLEMENT_POSER(wxPoserNSSliderCell);
@implementation wxPoserNSSliderCell : NSSliderCell
- (BOOL)startTrackingAt:(NSPoint)startPoint inView:(NSView *)controlView
{
    BOOL result = [super startTrackingAt:startPoint inView:controlView];
    [[NSNotificationCenter defaultCenter] postNotificationName:kwxNSSliderStartTracking object:controlView];
    return result;
}

- (BOOL)continueTracking:(NSPoint)lastPoint at:(NSPoint)currentPoint inView:(NSView *)controlView
{
    BOOL result = [super continueTracking:lastPoint at:currentPoint inView:controlView];
    [[NSNotificationCenter defaultCenter] postNotificationName:kwxNSSliderContinueTracking object:controlView];
    return result;
}

- (void)stopTracking:(NSPoint)lastPoint at:(NSPoint)stopPoint inView:(NSView *)controlView mouseIsUp:(BOOL)flag
{
    [super stopTracking:lastPoint at:stopPoint inView:controlView mouseIsUp:flag];
    [[NSNotificationCenter defaultCenter] postNotificationName:kwxNSSliderStopTracking object:controlView];
}
@end

// ============================================================================
// @class wxNSSliderNotificationObserver
// ============================================================================
@interface wxNSSliderNotificationObserver : NSObject
{
}

struct objc_object *wxCocoaNSSlider::sm_cocoaObserver = [[wxNSSliderNotificationObserver alloc] init];

- (void)startTracking: (NSNotification *)notification;
- (void)continueTracking: (NSNotification *)notification;
- (void)stopTracking: (NSNotification *)notification;
@end // interface wxNSSliderNotificationObserver

@implementation wxNSSliderNotificationObserver : NSObject

- (void)startTracking: (NSNotification *)notification;
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa([notification object]);
    wxCHECK_RET(slider,wxT("startTracking received but no wxSlider exists"));
    slider->CocoaNotification_startTracking(notification);
}

- (void)continueTracking: (NSNotification *)notification;
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa([notification object]);
    wxCHECK_RET(slider,wxT("continueTracking received but no wxSlider exists"));
    slider->CocoaNotification_continueTracking(notification);
}

- (void)stopTracking: (NSNotification *)notification;
{
    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa([notification object]);
    wxCHECK_RET(slider,wxT("stopTracking received but no wxSlider exists"));
    slider->CocoaNotification_stopTracking(notification);
}

@end // implementation wxNSSliderNotificationObserver

// ============================================================================
// class wxCocoaNSSlider
// ============================================================================
const wxObjcAutoRefFromAlloc<struct objc_object*> wxCocoaNSSlider::sm_cocoaTarget = [[wxNSSliderTarget alloc] init];


void wxCocoaNSSlider::AssociateNSSlider(WX_NSSlider cocoaNSSlider)
{
    if(cocoaNSSlider)
    {
        sm_cocoaHash.insert(wxCocoaNSSliderHash::value_type(cocoaNSSlider,this));
        [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(startTracking:) name:kwxNSSliderStartTracking object:cocoaNSSlider];
        [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(continueTracking:) name:kwxNSSliderContinueTracking object:cocoaNSSlider];
        [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(stopTracking:) name:kwxNSSliderStopTracking object:cocoaNSSlider];
        [cocoaNSSlider setTarget:sm_cocoaTarget];
    }
}

void wxCocoaNSSlider::DisassociateNSSlider(WX_NSSlider cocoaNSSlider)
{
    if(cocoaNSSlider)
    {
        sm_cocoaHash.erase(cocoaNSSlider);
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:kwxNSSliderStartTracking object:cocoaNSSlider];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:kwxNSSliderContinueTracking object:cocoaNSSlider];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:kwxNSSliderStopTracking object:cocoaNSSlider];
    }
}
