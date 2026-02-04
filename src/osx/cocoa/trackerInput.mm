#import "wx/osx/cocoa/trackerInput.h"


@implementation InputTracker

- (id)init {
    if(self = [super init]) {
        self.isEnabled = YES;
    }
    return self;
}


@synthesize view = _view;
@synthesize isEnabled = _enabled;

- (void)setIsEnabled:(BOOL)enabled {
    if (_enabled && !enabled)  {
        [self cancelTracking];
    }
    _enabled = enabled;
}

- (void)cancelTracking {
}
@end