
#import "wx/osx/cocoa/trackerTouchDouble.h"

@interface DualTouchTracker()
  @property BOOL isTracking;
  @property(readwrite) NSPoint initialPoint;
  @property(readwrite) NSUInteger modifiers;
  - (void)releaseTouches;
@end


@implementation DualTouchTracker

+ (NSNumber *)absoluteValue:(NSNumber *)input {
    return [NSNumber numberWithDouble:fabs([input doubleValue])];
}

- (id)init {
    if (self = [super init]) {
        self.threshold = 1;
        self.pannThreshold = 0.0014;
        self.oldDeltaSizeTouches = 0;
        self.oldMidPointCoord = CGPointZero;
    }
    
    return self;
}

- (void)dealloc {
    [self releaseTouches];
    [self releaseEvent];
    [super dealloc];
}

#pragma mark NSResponder

- (void)touchesBeganWithEvent:(NSEvent *)event {
    if (!self.isEnabled) return;
    
    NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self.view];
    
    if (touches.count == 2) {
        self.initialPoint = [self.view convertPointFromBacking:[event locationInWindow]];
        NSArray *array = [touches allObjects];
        _initialTouches[0] = [[array objectAtIndex:0] retain];
        _initialTouches[1] = [[array objectAtIndex:1] retain];
        
        _currentTouches[0] = [_initialTouches[0] retain];
        _currentTouches[1] = [_initialTouches[1] retain];

        _oldTouches[0] = [ _currentTouches[0] retain];
        _oldTouches[1] = [ _currentTouches[1] retain];

        _oldTouchesUsedInRotateEvent[0] = [ _currentTouches[0] retain];
        _oldTouchesUsedInRotateEvent[1] = [ _currentTouches[1] retain];

        _currentTouchEvent = [event retain];

    } else if (touches.count > 2) {
        // More than 2 touches. Only track 2.
        if (self.isTracking) {
            [self cancelTracking];
        } else {
            [self releaseTouches];
            [self releaseEvent];
        }
        
    }
}

- (void)touchesMovedWithEvent:(NSEvent *)event {
    if (!self.isEnabled) return;
    
    self.modifiers = [event modifierFlags];
    NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self.view];
    
    if (touches.count == 2 && _initialTouches[0]) {
        NSArray *array = [touches allObjects];
        [_oldTouches[0] release];
        [_oldTouches[1] release];

        _oldTouches[0] = [_currentTouches[0] retain];
        _oldTouches[1] = [_currentTouches[1] retain];

        [_currentTouches[0] release];
        [_currentTouches[1] release];
        [_currentTouchEvent release];
        
        NSTouch *touch;
        touch = [array objectAtIndex:0];
        if ([touch.identity isEqual:_initialTouches[0].identity]) {
            _currentTouches[0] = [touch retain];
        } else {
            _currentTouches[1] = [touch retain];
        }
        
        touch = [array objectAtIndex:1];
        if ([touch.identity isEqual:_initialTouches[0].identity]) {
            _currentTouches[0] = [touch retain];
        } else {
            _currentTouches[1] = [touch retain];
        }
        _currentTouchEvent = [event retain];
        
        if (!self.isTracking)
        {
            NSPoint deltaOrigin = self.deltaOrigin;
            NSSize  deltaSize   = self.deltaSize;
            
            //identify begining of movement or continue sequence and send actions
            if (fabs(deltaOrigin.x) > _threshold || fabs(deltaOrigin.y) > _threshold || fabs(deltaSize.width) > _threshold || fabs(deltaSize.height) > _threshold) {
                self.isTracking = YES;
                if (self.beginTrackingAction) [NSApp sendAction:self.beginTrackingAction to:self.view from:self];
            }
        }
        else {
            if (self.updateTrackingAction) [NSApp sendAction:self.updateTrackingAction to:self.view from:self];
        }
    }
}

- (void)touchesEndedWithEvent:(NSEvent *)event {
    if (!self.isEnabled) return;
    
    NSSet *touches = [event touchesMatchingPhase:NSTouchPhaseTouching inView:self.view];

    if(touches && touches.count == 0)
    {
        self.modifiers = [event modifierFlags];
        [self cancelTracking];
    }
}

- (void)touchesCancelledWithEvent:(NSEvent *)event {
    [self cancelTracking];
}

- (void)cancelTracking {
    if (self.isTracking) {
        if (self.endTrackingAction) [NSApp sendAction:self.endTrackingAction to:self.view from:self];
        self.isTracking = NO;
        [self releaseTouches];
        [self releaseEvent];
    }
}

- (void)magnifyWithEvent:(NSEvent *)event
{
    if (self.magnifyAction)
    {
        _currentGestureEvent = [event retain];
        [NSApp sendAction:self.magnifyAction to:self.view from:self];
    }
}

- (void)rotateWithEvent:(NSEvent *)event
{
    if (self.rotateAction)
    {
        _currentGestureEvent = [event retain];
        [NSApp sendAction:self.rotateAction to:self.view from:self];
    }
}

- (void)smartMagnifyWithEvent:(NSEvent *)event
{
    if (self.doubleTapAction)
    {
        _currentGestureEvent = [event retain];
        [NSApp sendAction:self.doubleTapAction to:self.view from:self];
    }
}

#pragma mark API

@synthesize threshold = _threshold;
@synthesize isTracking = _tracking;
@synthesize initialPoint = _initialPoint;
@synthesize pannThreshold = _pannThreshold;
@synthesize oldDeltaSizeTouches = _oldDeltaSizeTouches;
@synthesize oldMidPointCoord = _oldMidPointCoord;

@synthesize beginTrackingAction = _beginTrackingAction;
@synthesize updateTrackingAction = _updateTrackingAction;
@synthesize endTrackingAction = _endTrackingAction;
@synthesize rotateAction = _rotateAction;
@synthesize magnifyAction = _magnifyAction;
@synthesize doubleTapAction = _doubleTapAction;

- (NSEvent *) getCurrentTouchEvent{
    return _currentTouchEvent;
}

- (NSEvent *) getCurrentGestureEvent{
    return _currentGestureEvent;
}

- (NSPoint)deltaOrigin {
    if (!(_initialTouches[0] && _initialTouches[1] && _currentTouches[0] && _currentTouches[1])) return NSZeroPoint;
    
    CGFloat x1 = MIN(_initialTouches[0].normalizedPosition.x, _initialTouches[1].normalizedPosition.x);
    CGFloat x2 = MIN(_currentTouches[0].normalizedPosition.x, _currentTouches[1].normalizedPosition.x);
    CGFloat y1 = MIN(_initialTouches[0].normalizedPosition.y, _initialTouches[1].normalizedPosition.y);
    CGFloat y2 = MIN(_currentTouches[0].normalizedPosition.y, _currentTouches[1].normalizedPosition.y);
    
    NSSize deviceSize = _initialTouches[0].deviceSize;
    NSPoint delta;
    delta.x = (x2 - x1) * deviceSize.width;
    delta.y = (y2 - y1) * deviceSize.height;
    return delta;
}

//size between touches( fingers )
- (double)deltaSizeOfCurrentTouches {
     if (!(_currentTouches[0] && _currentTouches[1])) return 0;
    double dx,dy,delta;
    CGPoint p1,p2;

    p1.x = (double)_currentTouches[0].normalizedPosition.x;
    p2.x = (double)_currentTouches[1].normalizedPosition.x;
    p1.y = (double)_currentTouches[0].normalizedPosition.y;
    p2.y = (double)_currentTouches[1].normalizedPosition.y;

    delta = [DualTouchTracker calcPointsDelta:p1 :p2];
    if(_oldDeltaSizeTouches == 0)
        _oldDeltaSizeTouches = delta;
    return delta;
}

- (double)GetRotationAngle
{
    if (!(_initialTouches[0] && _initialTouches[1]) && !(_currentTouches[0] && _currentTouches[1])) return 0;

    //consider vectors
    double angle = 0.;

    //determine the progections of this vectors
    CGPoint oldV = (CGPoint){_oldTouchesUsedInRotateEvent[1].normalizedPosition.x - _oldTouchesUsedInRotateEvent[0].normalizedPosition.x,
                            _oldTouchesUsedInRotateEvent[1].normalizedPosition.y - _oldTouchesUsedInRotateEvent[0].normalizedPosition.y};

    CGPoint currentV = (CGPoint){_currentTouches[1].normalizedPosition.x - _currentTouches[0].normalizedPosition.x,
                                 _currentTouches[1].normalizedPosition.y - _currentTouches[0].normalizedPosition.y};

    [self GetRotationTouchesMovedInOpposition];

    //keep track of what touches we used, some of them will be skipped
    [_oldTouchesUsedInRotateEvent[0] release];
    [_oldTouchesUsedInRotateEvent[1] release];
    _oldTouchesUsedInRotateEvent[0] = [_currentTouches[0] retain];
    _oldTouchesUsedInRotateEvent[1] = [_currentTouches[1] retain];

    //calc dot product
    double dotProd = (oldV.x*currentV.x) + (oldV.y*currentV.y);
    //calc cross product
    double crossProd = oldV.x*currentV.y - oldV.y*currentV.x;
    //calc the angle
    angle = atan2(fabs(crossProd),dotProd);

    if(crossProd > 0)
        angle = -angle;

    return angle;
}

- (BOOL)GetRotationTouchesMovedInOpposition
{
    static bool touchesMovedInOpposition = false;

    if (_oldTouchesUsedInRotateEvent[0] == _currentTouches[0] &&
        _oldTouchesUsedInRotateEvent[1] == _currentTouches[1])
    {
        return touchesMovedInOpposition;
    }

    double xDiff1 = _currentTouches[0].normalizedPosition.x - _oldTouchesUsedInRotateEvent[0].normalizedPosition.x;
    double xDiff2 = _currentTouches[1].normalizedPosition.x - _oldTouchesUsedInRotateEvent[1].normalizedPosition.x;
    double yDiff1 = _currentTouches[0].normalizedPosition.y - _oldTouchesUsedInRotateEvent[0].normalizedPosition.y;
    double yDiff2 = _currentTouches[1].normalizedPosition.y - _oldTouchesUsedInRotateEvent[1].normalizedPosition.y;

    touchesMovedInOpposition = (xDiff1 * xDiff2 < 0) && (yDiff1 * yDiff2 < 0) && (xDiff1 * yDiff1 < 0);

    return touchesMovedInOpposition;
}

- (BOOL)DoLinesIntersect
{

    TLine l1 = (TLine){(CGPoint){_oldTouches[0].normalizedPosition.x, _oldTouches[0].normalizedPosition.y},
                       (CGPoint){_oldTouches[1].normalizedPosition.x, _oldTouches[1].normalizedPosition.y}};
    TLine l2 = (TLine){(CGPoint){_currentTouches[0].normalizedPosition.x, _currentTouches[0].normalizedPosition.y},
                       (CGPoint){_currentTouches[1].normalizedPosition.x, _currentTouches[1].normalizedPosition.y}};
    //Check if lines intersect
    CGPoint p = l1.start;
    CGPoint q = l2.start;
    CGPoint r = [DualTouchTracker CGPointSubtract:l1.end :l1.start];
    CGPoint s = [DualTouchTracker CGPointSubtract:l2.end :l2.start];

    double s_r_crossProduct = [DualTouchTracker CGPointCrossProductZComponent:r :s];
    double t = [DualTouchTracker CGPointCrossProductZComponent:
                                                            [DualTouchTracker CGPointSubtract:q :p]
                                                            :s]/s_r_crossProduct;
    double u = [DualTouchTracker CGPointCrossProductZComponent:
                                                            [DualTouchTracker CGPointSubtract:q :p]
                                                            :r]/s_r_crossProduct;

    if(t < 0 || t > 1.0 || u < 0 || u > 1.0)
        return NO;
    else
        return YES;
}

- (double)deltaSizeOfInitialTouches {
    if (!(_currentTouches[0] && _currentTouches[1])) return 0;
    double dx,dy,delta;
    CGPoint p1,p2;

    p1.x = (double)_initialTouches[0].normalizedPosition.x;
    p2.x = (double)_initialTouches[1].normalizedPosition.x;
    p1.y = (double)_initialTouches[0].normalizedPosition.y;
    p2.y = (double)_initialTouches[1].normalizedPosition.y;

    delta = [DualTouchTracker calcPointsDelta:p1 :p2];
    if(_oldDeltaSizeTouches == 0)
        _oldDeltaSizeTouches = delta;
    return delta;
}

- (CGPoint)midPointCoordOfCurrentTouches
{
    if (!(_currentTouches[0] && _currentTouches[1])) return NSZeroPoint;
    double x1,x2,y1,y2,mx,my;

    /*When you are finding the coordinates of the midpoint of a segment,
     you are actually finding the average (mean) of the x-coordinates
     and the average (mean) of the y-coordinates.*/
    //finding the mid point on the segment determined by the 2 touches
    x1 = (double)_currentTouches[0].normalizedPosition.x;
    x2 = (double)_currentTouches[1].normalizedPosition.x;
    y1 = (double)_currentTouches[0].normalizedPosition.y;
    y2 = (double)_currentTouches[1].normalizedPosition.y;

    mx = (x1+x2)/2;
    my = (y1+y2)/2;

    CGPoint mp; //mean point
    mp.x = mx; //* deviceSize.width;
    mp.y = my; //* deviceSize.height;

    if(_oldMidPointCoord.x == CGPointZero.x && _oldMidPointCoord.y == CGPointZero.y)
        _oldMidPointCoord = mp;

    return mp;
}

- (CGPoint)midPointCoordOfInitialTouches
{
    if (!(_currentTouches[0] && _currentTouches[1])) return NSZeroPoint;
    double x1,x2,y1,y2,mx,my;

    /*When you are finding the coordinates of the midpoint of a segment,
     you are actually finding the average (mean) of the x-coordinates
     and the average (mean) of the y-coordinates.*/
    //finding the mid point on the segment determined by the 2 touches
    x1 = (double)_initialTouches[0].normalizedPosition.x;
    x2 = (double)_initialTouches[1].normalizedPosition.x;
    y1 = (double)_initialTouches[0].normalizedPosition.y;
    y2 = (double)_initialTouches[1].normalizedPosition.y;

    mx = (x1+x2)/2;
    my = (y1+y2)/2;

    CGPoint mp; //mean point
    mp.x = mx; //* deviceSize.width;
    mp.y = my; //* deviceSize.height;

    if(_oldMidPointCoord.x == CGPointZero.x && _oldMidPointCoord.y == CGPointZero.y)
        _oldMidPointCoord = mp;

    return mp;
}

- (NSSize)deltaSize {
    if (!(_initialTouches[0] && _initialTouches[1] && _currentTouches[0] && _currentTouches[1])) return NSZeroSize;
    
    double x1,x2,y1,y2,width1,width2,height1,height2;
    
    x1 = MIN(_initialTouches[0].normalizedPosition.x, _initialTouches[1].normalizedPosition.x);
    x2 = MAX(_initialTouches[0].normalizedPosition.x, _initialTouches[1].normalizedPosition.x);
    width1 = x2 - x1;
    
    y1 = MIN(_initialTouches[0].normalizedPosition.y, _initialTouches[1].normalizedPosition.y);
    y2 = MAX(_initialTouches[0].normalizedPosition.y, _initialTouches[1].normalizedPosition.y);
    height1 = y2 - y1;
    
    x1 = MIN(_currentTouches[0].normalizedPosition.x, _currentTouches[1].normalizedPosition.x);
    x2 = MAX(_currentTouches[0].normalizedPosition.x, _currentTouches[1].normalizedPosition.x);
    width2 = x2 - x1;
    
    y1 = MIN(_currentTouches[0].normalizedPosition.y, _currentTouches[1].normalizedPosition.y);
    y2 = MAX(_currentTouches[0].normalizedPosition.y, _currentTouches[1].normalizedPosition.y);
    height2 = y2 - y1;
    
    NSSize deviceSize = _initialTouches[0].deviceSize;
    NSSize delta;
    delta.width = (width2 - width1) * deviceSize.width;
    delta.height = (height2 - height1) * deviceSize.height;
    return delta;
}

//calc distance between 2 points.
+ (double)calcPointsDelta:(CGPoint)point1 :(CGPoint)point2
{
    double dx = point1.x-point2.x;
    double dy = point1.y-point2.y;
    double delta = sqrt(dx*dx + dy*dy);
    return delta;
}

//line intersection functions

+ (CGFloat) CGPointCrossProductZComponent:(CGPoint)p1 :(CGPoint)p2
{
    return p1.x * p2.y - p1.y * p2.x;
}

+ (CGPoint) CGPointSubtract:(CGPoint)p1 :(CGPoint)p2
{
    return (CGPoint){p1.x - p2.x, p1.y - p2.y};
}

+ (CGPoint) CGPointAdd:(CGPoint)p1 :(CGPoint)p2
{
    return (CGPoint){p1.x + p2.x, p1.y + p2.y};
}

+ (CGPoint) CGPointMultiply:(CGPoint)p1 :(CGFloat)factor
{
    return (CGPoint){p1.x * factor, p1.y * factor};
}

//END of line intersection functions

- (void)releaseTouches {
    [_initialTouches[0] release];
    [_initialTouches[1] release];
    _initialTouches[0] = nil;
    _initialTouches[1] = nil;

    [_oldTouches[0] release];
    [_oldTouches[1] release];
    _oldTouches[0] = nil;
    _oldTouches[1] = nil;

    [_currentTouches[0] release];
    [_currentTouches[1] release];
    _currentTouches[0] = nil;
    _currentTouches[1] = nil;

    [_oldTouchesUsedInRotateEvent[0] release];
    [_oldTouchesUsedInRotateEvent[1] release];
    _oldTouchesUsedInRotateEvent[0] = nil;
    _oldTouchesUsedInRotateEvent[1] = nil;

}

- (void)releaseEvent {
    [_currentTouchEvent release];
    _currentTouchEvent = nil;
}

@end // end DualTouchTracker implementation
