/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/NSSlider.mm
// Purpose:     wxCocoaNSSlider class
// Author:      Mark Oxenham
// Modified by: David Elliott
// Created:     2007/08/10
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Software 2000 Ltd. All rights reserved.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSSlider.h"

#import <Foundation/NSNotification.h>
#import <Foundation/NSString.h>
#import <AppKit/NSEvent.h>
#include "wx/cocoa/objc/NSSlider.h"

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
// @class WXNSSlider
// ============================================================================


@implementation WXNSSlider : NSSlider

// Override to ensure that WXNSSlider gets created with a WXNSSliderCell
+ (Class)cellClass
{
    return [WX_GET_OBJC_CLASS(WXNSSliderCell) class];
}

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
WX_IMPLEMENT_GET_OBJC_CLASS(WXNSSlider,NSSlider)

// ============================================================================
// @class WXNSSliderCell
// ============================================================================

#define kwxNSSliderStartTracking    @"wxNSSliderStartTracking"
#define kwxNSSliderContinueTracking @"wxNSSliderContinueTracking"
#define kwxNSSliderStopTracking     @"wxNSSliderStopTracking"

@implementation WXNSSliderCell : NSSliderCell
- (BOOL)startTrackingAt:(NSPoint)startPoint inView:(NSView *)controlView
{
    BOOL result = [super startTrackingAt:startPoint inView:controlView];

    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(controlView);
    if(slider)
        slider->CocoaNotification_startTracking(NULL);

    return result;
}

- (BOOL)continueTracking:(NSPoint)lastPoint at:(NSPoint)currentPoint inView:(NSView *)controlView
{
    BOOL result = [super continueTracking:lastPoint at:currentPoint inView:controlView];

    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(controlView);
    if(slider)
        slider->CocoaNotification_continueTracking(NULL);

    return result;
}

- (void)stopTracking:(NSPoint)lastPoint at:(NSPoint)stopPoint inView:(NSView *)controlView mouseIsUp:(BOOL)flag
{
    [super stopTracking:lastPoint at:stopPoint inView:controlView mouseIsUp:flag];

    wxCocoaNSSlider *slider = wxCocoaNSSlider::GetFromCocoa(controlView);
    if(slider)
        slider->CocoaNotification_stopTracking(NULL);
}
@end
WX_IMPLEMENT_GET_OBJC_CLASS(WXNSSliderCell,NSSliderCell)

// ============================================================================
// class wxCocoaNSSlider
// ============================================================================
const wxObjcAutoRefFromAlloc<struct objc_object*> wxCocoaNSSlider::sm_cocoaTarget = [[wxNSSliderTarget alloc] init];


void wxCocoaNSSlider::AssociateNSSlider(WX_NSSlider cocoaNSSlider)
{
    if(cocoaNSSlider)
    {
        sm_cocoaHash.insert(wxCocoaNSSliderHash::value_type(cocoaNSSlider,this));
        [cocoaNSSlider setTarget:sm_cocoaTarget];
    }
}

void wxCocoaNSSlider::DisassociateNSSlider(WX_NSSlider cocoaNSSlider)
{
    if(cocoaNSSlider)
    {
        sm_cocoaHash.erase(cocoaNSSlider);
    }
}
