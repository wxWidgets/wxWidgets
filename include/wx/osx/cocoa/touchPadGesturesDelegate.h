#ifndef __TOUCHPADGESTURESDELEGATE_H_
#define __TOUCHPADGESTURESDELEGATE_H_

#include "wx/defs.h"
#import "trackerTouchDouble.h"

WXDLLIMPEXP_CORE
@protocol TouchPadGesturesDelegate

- (void) _initTrackers;
- (void)acceptTouchEvents:(BOOL)status;
- (void)enableRubberSheet:(bool)enable;
- (void)setRubberSheetSensibility:(int)sensibility;
- (void) endGestures;
- (NSMutableArray *)getInputTrackers;

- (void)dualTouchesBegan:(DualTouchTracker*)tracker;
- (void)dualTouchesEnded:(DualTouchTracker*)tracker;

// rubber sheet
- (void)dualTouchesMovedRubberSheet:(DualTouchTracker*)tracker;
- (void)dualTouchesMoved:(DualTouchTracker*)tracker;
- (void)dualTouchPan:(NSEvent*)event;

//handling gesture events for rotation and pinch
- (void)dualGestureRotate:(DualTouchTracker*)tracker;
- (void)dualGestureSmartMagnify:(DualTouchTracker*)tracker;
- (void)dualGestureMagnify:(DualTouchTracker*)tracker;

// other stuff
- (void)dualTouchesEnded:(DualTouchTracker*)tracker;
- (void)enableTrackers;

@property BOOL isTouch;

@end

WXDLLIMPEXP_CORE
@interface TrackPadConstants : NSObject

// delta dist. between fingers for which to send pinch event.
+ (double) pinchDeltaLimit;
// if pan event is handled to late then skip it.
+ (double) panEventExpireThreshold;
+ (double) rotationScaleFactor;
// rubbersheet pinch action scale factor.
+ (double) rubberSheetPinchScaleFactor;
// rubbersheet send event timeframe - seconds.
+ (double) rubberSheetSendThreshold;
// rubbersheet base threshold for pan action.
+ (double) basicRSPanThreshold;
// rubbersheet base threshold for pinch action.
+ (double) basicRSPinchThreshold;
// rubbersheet base threshold for rotate action.
+ (double) basicRSRotateThreshold;
// rubbersheet actions threshold modifier.
+ (int) defaultSensibility;
// maximum allowed rubbersheet activation sensibility.
+ (int) maxSensibility;

@end

#endif // __TOUCHPADGESTURESDELEGATE_H_
