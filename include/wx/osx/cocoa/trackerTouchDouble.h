#ifndef __TRACKER_TOUCH_DOUBLE_H_
#define __TRACKER_TOUCH_DOUBLE_H_

#include "wx/defs.h"
#import "trackerInput.h"

typedef struct TLine {
    CGPoint start, end;
} TLine;

const TLine TLineZero = (TLine){(CGPoint){0.0, 0.0}, (CGPoint){0.0, 0.0}};

WXDLLIMPEXP_CORE
@interface DualTouchTracker : InputTracker {
@private
    BOOL _tracking;
    NSPoint _initialPoint;
    NSUInteger _modifiers;
    CGFloat _threshold;
    double _pinchThreshold;
    double _oldDeltaSizeTouches;
    CGPoint _oldMidPointCoord;
    NSEvent * _currentTouchEvent;
    NSEvent * _currentGestureEvent;

    //Arrays to keep track of touches and identify if they are the same touches during touch events.
    NSTouch *_initialTouches[2];
    NSTouch *_oldTouches[2];
    NSTouch *_currentTouches[2];
    NSTouch *_oldTouchesUsedInRotateEvent[2];

    SEL _beginTrackingAction;
    SEL _updateTrackingAction;
    SEL _endTrackingAction;
    SEL _rotateAction;
    SEL _magnifyAction;
    SEL _doubleTapAction;
}

// The amount of dual touch movement before tracking begins. This value is in points (72ppi). Defaults to 1.
@property CGFloat threshold;

// Ammount of dual touch diff between fingers in order to detect panning movement or pinching.
@property double pinchThreshold;

// Delta used to compare with pannThreshold
@property double oldDeltaSizeTouches;

@property CGPoint oldMidPointCoord;

// The location of the cursor in the view's coordinate space when the second touch began.
@property(readonly) NSPoint initialPoint;

// The modifier flags of the last event processed by the tracker. The returned value outside of the begin and end tracking actions are undefined.
@property(readonly) NSUInteger modifiers;

// The two tracked touches are considered the bounds of a rectangle. THe following methods allow you to get the change in origin or size from the inital tracking values to the current values of said rectangle. The values are in points (72ppi)
@property(readonly) NSPoint deltaOrigin;
@property(readonly) NSSize deltaSize;
@property(readonly) double deltaSizeOfCurrentTouches;
@property(readonly) double deltaSizeOfInitialTouches;
@property(readonly) CGPoint midPointCoordOfCurrentTouches;
@property(readonly) CGPoint midPointCoordOfInitialTouches;

// Get the current NSEvent for attributes
- (NSEvent *) getCurrentTouchEvent;
- (NSEvent *) getCurrentGestureEvent;
+ (NSNumber *)absoluteValue:(NSNumber *)input;
- (double) GetRotationAngle;
- (BOOL) GetRotationTouchesMovedInOpposition;
- (BOOL)   DoLinesIntersect;
+ (double) calcPointsDelta:(CGPoint)point1 :(CGPoint)point2;
+ (NSPoint) calcPointsDelta:(NSPoint)point1 :(NSPoint)point2;
+ (CGFloat) CGPointCrossProductZComponent:(CGPoint)p1 :(CGPoint)p2;

+ (CGPoint) CGPointSubtract:(CGPoint)p1 :(CGPoint)p2;

+ (CGPoint) CGPointAdd:(CGPoint)p1 :(CGPoint)p2;

+ (CGPoint) CGPointMultiply:(CGPoint)p1 :(CGFloat)factor;

@property SEL dualTapDownAction;
@property SEL dualTapUpAction;

// The following properties hold the tracking callbacks on the view. Each method should have one paramenter (DragTracker *) and a void return.
@property SEL doubleTapVerificationAction;
@property SEL beginTrackingAction;
@property SEL updateTrackingAction;
@property SEL endTrackingAction;
@property SEL rotateAction;
@property SEL magnifyAction;
@property SEL doubleTapAction;

@end

#endif // __TRACKER_TOUCH_DOUBLE_H_
