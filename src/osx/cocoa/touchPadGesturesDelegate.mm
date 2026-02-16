#include "wx/wxprec.h"
#include "wx/osx/cocoa/touchPadGesturesDelegate.h"


static const double     g_pinchDeltaLimit                = 0.010000;
static const double     g_panEventExpireThreshold        = 0.000800;
static const double     g_rotationScaleFactor            = -0.025;
static const double     g_rubberSheetPinchScaleFactor    = 2;
static const double     g_rubberSheetSendThreshold       = 0.01;
static const double     g_basicRSPanThreshold            = 0.004;
static const double     g_basicRSPinchThreshold          = 0.008;
static const double     g_basicRSRotateThreshold         = 0.005;
static const int        g_defaultSensibility             = 5;
static const int        g_maxSensibility                 = 10;

@implementation TrackPadConstants

+ (double) pinchDeltaLimit { return g_pinchDeltaLimit; }
+ (double) panEventExpireThreshold { return g_panEventExpireThreshold; }
+ (double) rotationScaleFactor { return g_rotationScaleFactor; }
+ (double) rubberSheetPinchScaleFactor { return g_rubberSheetPinchScaleFactor; }
+ (double) rubberSheetSendThreshold { return g_rubberSheetSendThreshold; }
+ (double) basicRSPanThreshold { return g_basicRSPanThreshold; }
+ (double) basicRSPinchThreshold { return g_basicRSPinchThreshold; }
+ (double) basicRSRotateThreshold { return g_basicRSRotateThreshold; }
+ (int) defaultSensibility { return g_defaultSensibility; }
+ (int) maxSensibility { return g_maxSensibility; }

@end
