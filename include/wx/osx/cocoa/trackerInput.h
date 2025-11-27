#import <Cocoa/Cocoa.h>

@interface InputTracker : NSResponder {
@private
    NSView *_view;
    BOOL _enabled;
}

// The owning view of the input tracker. This is the object that any callbacks are sent to.
@property(assign) NSView *view;

// Is this tracker currently monitoring the events and sending out callbacks. If a tracker's enabled state changes to NO, it will automatically cancel any tracking that it is currently performing.
@property BOOL isEnabled;

// Stop any tracking the tracker may be performing and reset itself.
- (void)cancelTracking;
@end
