///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/glcanvas.mm
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/osx/cocoa/trackerTouchDouble.h"
#include "wx/osx/cocoa/touchPadGesturesDelegate.h"
#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

WXGLContext WXGLCreateContext( WXGLPixelFormat pixelFormat, WXGLContext shareContext, wxGLCanvas* canvas )
{
    WXGLContext context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: shareContext];

    // Bricsys change: we want to hide the OpenGL view behind the NSWindow so we can
    // draw the crosshair on the contentView of the NSWindow (main frame)
    if( canvas != nullptr && canvas->GetParent() && canvas->GetParent()->GetParent() && canvas->GetParent()->GetParent()->GetParent() )
    {
        wxNonOwnedWindow* topLevelWnd = wxDynamicCast( canvas->GetParent()->GetParent()->GetParent(), wxNonOwnedWindow );
        if( topLevelWnd->GetExtraStyle() & wxWS_EX_TRANSP_FRAME_MAC_BCAD )
        {
            GLint order = -1;
            [context setValues:&order forParameter:NSOpenGLCPSurfaceOrder];
        }
    }
    // end Bricsys change
    return context ;
}

void WXGLDestroyContext( WXGLContext context )
{
    if ( context )
    {
        [context release];
    }
}

void WXGLSwapBuffers( WXGLContext context )
{
    [context flushBuffer];
}

WXGLContext WXGLGetCurrentContext()
{
    return [NSOpenGLContext currentContext];
}

bool WXGLSetCurrentContext(WXGLContext context)
{
    [context makeCurrentContext];

    return true;
}

void WXGLDestroyPixelFormat( WXGLPixelFormat pixelFormat )
{
    if ( pixelFormat )
    {
        [pixelFormat release];
    }
}

// Form a list of attributes by joining canvas attributes and context attributes.
// OS X uses just one list to find a suitable pixel format.
WXGLPixelFormat WXGLChoosePixelFormat(const int *GLAttrs,
                                      int n1,
                                      const int *ctxAttrs,
                                      int n2)
{
    NSOpenGLPixelFormatAttribute data[128];
    const NSOpenGLPixelFormatAttribute *attribs;
    unsigned p = 0;

    // The list should have at least one value and the '0' at end. So the
    // minimum size is 2.
    if ( GLAttrs && n1 > 1 )
    {
        n1--; // skip the ending '0'
        while ( p < (unsigned)n1 )
        {
            data[p] = (NSOpenGLPixelFormatAttribute) GLAttrs[p];
            p++;
        }
    }

    if ( ctxAttrs && n2 > 1 )
    {
        n2--; // skip the ending '0'
        unsigned p2 = 0;
        while ( p2 < (unsigned)n2 )
            data[p++] = (NSOpenGLPixelFormatAttribute) ctxAttrs[p2++];
    }

    // End the list
    data[p] = (NSOpenGLPixelFormatAttribute) 0;

    attribs = data;

    return [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*) attribs];
}

// Bricsys change: #22025 add TouchPadGesturesDelegate protocol and all the members
@interface wxNSCustomOpenGLView : NSOpenGLView <TouchPadGesturesDelegate>
 {
    //Touch pad
    NSMutableArray *inputTrackers;
    DualTouchTracker *m_pTracker;

@private
    //used to differentiate Magic Mouse from touchpad.
    //Te gesture source of scroll from Magic Mouse can't be determined in another way.
    //See: http://stackoverflow.com/questions/13807616/mac-cocoa-how-to-differentiate-if-a-nsscrollwheel-event-is-from-a-mouse-or-trac
    //panning states
    wxState _panningStatus;
    wxState _rotateStatus;
    wxState _magnifyStatus;
    wxState _rubberSheetStatus;
    // double  _currentFingersSize;
    // double  _previousFingersSize;
    bool    _isRubberSheet;
    int     _rubberSheetSensibility;        // rubbersheet actions threshold modifier.
    double  _rubberSheetLastSentTime;       // last time a rubbersheet event has been dispatched for processing.
    double  _rubberSheetAngleThreshold;     // real time rubbersheet rotate threshold.
    double  _rubberSheetPanThreshold;       // real time rubbersheet pan threshold.
    double  _rubberSheetPinchThreshold;     // real time rubbersheet pinch threshold.

@public
    // Need to send resize event on the backing prop changes
    wxEvtHandler* eventHandler;
 }

@property BOOL isTouch;
// Bricsys change end: #22025 add TouchPadGesturesDelegate protocol and all the members

@end

@implementation wxNSCustomOpenGLView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        // Bricsys change: #22025 add custom doCommandBySelector as there is new
        // NSStandardKeyBindingResponding, which the OpenGLView doesn't impelement
        wxOSXCocoaClassAddWXMethods( self, wxOSXSKIP_NONE, wxOSXADD_COMMANDBYSELECTOR);
        // end Bricsys change
    }
    // Tracker support
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

// Bricsys change: #22025 add TouchPadGesturesDelegate protocol and all the members
- (id)initWithFrame:(NSRect)rectBox {
    [super initWithFrame:rectBox];
    [self setWantsLayer:YES];
    [self _initTrackers];
    [self acceptTouchEvents:NO];
    _isTouch                    = FALSE;
    _isRubberSheet              = false;
    _rubberSheetSensibility     = 5;
    _rubberSheetLastSentTime    = 0;
    _rubberSheetAngleThreshold  = 0;
    _rubberSheetPanThreshold    = 0;
    _rubberSheetPinchThreshold  = 0;
    // _currentFingersSize         = 0;
    // _previousFingersSize        = 0;
    _panningStatus              = wxNoState;
    _rotateStatus               = wxNoState;
    _magnifyStatus              = wxNoState;
    _rubberSheetStatus          = wxNoState;
    return self;
}
// Bricsys change, refs #72649
- (void)dealloc
{
    [inputTrackers release];
    [super dealloc];
}
// Bricsys change end, refs #72649

- (void)viewDidChangeBackingProperties {
    // use the following line if this resize event should have the physical pixel resolution
    // but that is not recommended, because ordinary resize events won't do so either
    // which would necessitate a case-by-case switch in the resize handler method.
    // NSRect nsrect = [view convertRectToBacking:[view bounds]];
    NSRect nsrect = [self bounds];
    if ( !(nsrect.size.width > 0 && nsrect.size.height > 0) || eventHandler == nullptr)
        return;
    wxRect rect = wxRect(nsrect.origin.x, nsrect.origin.y, nsrect.size.width, nsrect.size.height);
    wxSizeEvent event = wxSizeEvent();
    event.SetRect(rect);
    event.SetSize(rect.GetSize());
    eventHandler->ProcessEvent(event); // need to process it now as the drawRect will trigger onPaint
    [self drawRect: nsrect];
}

// MARK:- TouchPadGesturesDelegate
- (void)_initTrackers
{
    inputTrackers = [NSMutableArray new];
    DualTouchTracker *touchTracker = [DualTouchTracker new];
    touchTracker.beginTrackingAction = @selector(dualTouchesBegan:);

    touchTracker.rotateAction = @selector(dualGestureRotate:);
    touchTracker.magnifyAction = @selector(dualGestureMagnify:);
    touchTracker.doubleTapAction = @selector(dualGestureSmartMagnify:);

    touchTracker.view = self;
    [inputTrackers addObject:touchTracker];
    [touchTracker release];
}

- (void)acceptTouchEvents:(BOOL)status
{
    [self setAcceptsTouchEvents:status];
}

- (void)enableRubberSheet:(bool)enable
{
    _isRubberSheet = enable;
}

-(void)setRubberSheetSensibility:(int)sensibility
{
    sensibility = [TrackPadConstants maxSensibility] - sensibility;

    if(sensibility < 0 || sensibility > [TrackPadConstants maxSensibility])
        sensibility = [TrackPadConstants defaultSensibility];

    _rubberSheetSensibility = sensibility;
}

- (void)endGestures
{
    wxWidgetCocoaImpl* impl = nullptr;

    if(_rotateStatus != wxNoState)
    {
        impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
        wxTrackPadEvent tpevent(wxEVT_ROTATE);
        _rotateStatus = wxRotationEnd;
        tpevent.SetState(_rotateStatus);
        impl->trackpadEvent(tpevent, nullptr, self, _cmd);
        _rotateStatus = wxNoState;
    }

    if(_magnifyStatus != wxNoState )
        _magnifyStatus = wxNoState;
}

-(NSMutableArray *)getInputTrackers
{
    return inputTrackers;
}

- (void)dualTouchesBegan:(DualTouchTracker*)tracker
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );

    if (impl == NULL){
        return;
    }

    CGPoint trackerLocation = NSPointToCGPoint(tracker.initialPoint);
    //we can manage which type of tracking we are performing by simply changing the tracking action methods.
    if(_isRubberSheet)
    {
        tracker.updateTrackingAction = @selector(dualTouchesMovedRubberSheet:);
        _rubberSheetLastSentTime = [[NSProcessInfo processInfo] systemUptime];

        _rubberSheetPanThreshold = [TrackPadConstants basicRSPanThreshold] * _rubberSheetSensibility / [TrackPadConstants defaultSensibility];
        _rubberSheetPinchThreshold = [TrackPadConstants basicRSPinchThreshold] * _rubberSheetSensibility / [TrackPadConstants defaultSensibility];
        _rubberSheetAngleThreshold = [TrackPadConstants basicRSRotateThreshold] * _rubberSheetSensibility / [TrackPadConstants defaultSensibility];
    }
    else
    {
        tracker.updateTrackingAction = @selector(dualTouchesMoved:);
    }

    tracker.endTrackingAction = @selector(dualTouchesEnded:);
}

// rubber sheet
//
- (void)dualTouchesMovedRubberSheet:(DualTouchTracker*)tracker
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (!impl)
        return;

    NSTimeInterval currentTime = [[NSProcessInfo processInfo] systemUptime];

    if((currentTime - _rubberSheetLastSentTime) > [TrackPadConstants rubberSheetSendThreshold])
    {
        wxTrackPadEvent tpevent(wxEVT_RUBBER_SHEET);

        bool oppositeDirection = [tracker GetRotationTouchesMovedInOpposition];
        bool shiftDown = tracker.modifiers & NSShiftKeyMask;

        // pinch data
        //
        double deltaTouches = tracker.deltaSizeOfCurrentTouches;
        double deltaMove    = deltaTouches - tracker.oldDeltaSizeTouches;

        if(_magnifyStatus == wxNoState)
        {
            if(std::abs(deltaMove) > _rubberSheetPinchThreshold && !oppositeDirection)
            {
                _magnifyStatus = wxRubberSheetMove;

                if (_panningStatus == wxNoState &&
                    _rotateStatus == wxNoState)
                {
                    _rubberSheetPanThreshold += _rubberSheetPanThreshold / 2;
                }
            }
        }
        else
        {
            double deltaMv = deltaMove * [TrackPadConstants rubberSheetPinchScaleFactor] + 1;
            tpevent.SetPinchMagnitude(deltaMv);
        }

        // pan data
        //
        CGPoint midPointCoord = tracker.midPointCoordOfCurrentTouches;

        if(_panningStatus == wxNoState && _rubberSheetStatus != wxNoState && !shiftDown)
        {
            double xDist = midPointCoord.x - tracker.oldMidPointCoord.x;
            double yDist = midPointCoord.y - tracker.oldMidPointCoord.y;
            double panDist = std::sqrt(xDist * xDist + yDist * yDist);

            if(panDist > _rubberSheetPanThreshold && !oppositeDirection)
            {
                _panningStatus = wxRubberSheetMove;

                if(_rotateStatus == wxNoState &&
                   _magnifyStatus == wxNoState)
                {
                    _rubberSheetPinchThreshold += _rubberSheetPinchThreshold / 2;
                }
            }
        }
        else
        {
            CGPoint meanPoint = midPointCoord;
            meanPoint.x *= self.frame.size.width;

            // because the origin of trackpad starts from bottom-left point
            // and the frame origin is top-left we have to adapt to frame origin adapt y
            //
            meanPoint.y *= self.frame.size.height;
            meanPoint.y = self.frame.size.height - meanPoint.y;
            tpevent.SetPosition(wxRealPoint(meanPoint.x, meanPoint.y));
        }

        // rotate data
        //
        double angle = [tracker GetRotationAngle];

        if(_rotateStatus == wxNoState)
        {
            if((std::abs(angle) > _rubberSheetAngleThreshold / deltaTouches) && oppositeDirection)
            {
                _rotateStatus = wxRubberSheetMove;

                if(_panningStatus == wxNoState &&
                   _magnifyStatus == wxNoState)
                {
                    _rubberSheetPanThreshold += _rubberSheetPanThreshold / 2;
                    _rubberSheetPinchThreshold += _rubberSheetPinchThreshold / 2;
                }
            }
        }
        else
            tpevent.SetAngle(angle);

        if(_rubberSheetStatus == wxNoState)
        {
            _rubberSheetStatus = wxRubberSheetStart;
            tpevent.SetState(_rubberSheetStatus);
        }
        else
        {
            _rubberSheetStatus = wxRubberSheetMove;
            tpevent.SetState(_rubberSheetStatus);
        }

        if(_rotateStatus != wxNoState ||
           _panningStatus != wxNoState ||
           _magnifyStatus != wxNoState ||
           _rubberSheetStatus == wxRubberSheetStart ||
           shiftDown)
        {
            NSEvent *event = tracker.getCurrentTouchEvent;

            impl->trackpadEvent(tpevent, event, self, _cmd);
        }

        tracker.oldMidPointCoord = midPointCoord;
        tracker.oldDeltaSizeTouches = deltaTouches;
        _rubberSheetLastSentTime = [[NSProcessInfo processInfo] systemUptime];
    }
}

- (void)dualTouchesMoved:(DualTouchTracker*)tracker
{
    m_pTracker = tracker;
}

- (void)dualTouchPan:(NSEvent*)event
{
    if(_rotateStatus == wxNoState && _magnifyStatus == wxNoState && !_isRubberSheet)
    {
        wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );

        if (impl == NULL)
            return;

        wxTrackPadEvent tpevent(wxEVT_PAN);
        CGPoint meanPoint = m_pTracker.midPointCoordOfCurrentTouches;
        meanPoint.x *= self.frame.size.width;
        //because the origin of trackpad starts from bottom-left point
        //and the frame origin is top-left we have to adapt to frame origin
        //adapt y
        meanPoint.y *= self.frame.size.height;
        meanPoint.y = self.frame.size.height - meanPoint.y;


        switch([event phase])
        {
            case NSEventPhaseBegan:
                //[event touches:NSEventPhaseBegan in:m_osxView];
                _panningStatus = wxPanningStartMove;
                tpevent.SetState(_panningStatus);
                tpevent.SetPosition(wxRealPoint(meanPoint.x, meanPoint.y));
                impl->trackpadEvent(tpevent, event, self, _cmd);

            break;
            case NSEventPhaseChanged:
                _panningStatus = wxPanningMove;
                tpevent.SetState(_panningStatus);
                tpevent.SetPosition(wxRealPoint(meanPoint.x, meanPoint.y));
                impl->trackpadEvent(tpevent, event, self, _cmd);
                break;
            case NSEventPhaseEnded:
            case NSEventPhaseCancelled:
                if(_panningStatus != wxNoState)
                {
                    wxTrackPadEvent tpevent(wxEVT_PAN);
                    _panningStatus = wxPanningEnd;
                    tpevent.SetState(_panningStatus);
                    impl->trackpadEvent(tpevent, event, self, _cmd);
                    _panningStatus = wxNoState;
                }
                break;
        }

    }
    else
    {
        return;
    }
}

//handling gesture events for rotation and pinch
- (void)dualGestureRotate:(DualTouchTracker*)tracker
{
    if(_isRubberSheet)
    {
        if(_rubberSheetStatus == wxRubberSheetMove &&
           _rotateStatus == wxNoState &&
           _magnifyStatus == wxNoState)
        {
            _rotateStatus = wxRubberSheetMove;

            _rubberSheetPanThreshold += _rubberSheetPanThreshold / 2;
            _rubberSheetPinchThreshold += _rubberSheetPinchThreshold / 2;
        }
        return;
    }

    NSEvent * event = tracker.getCurrentGestureEvent;
    wxWidgetCocoaImpl* impl = nullptr;

    switch ([event phase])
    {
        case NSEventPhaseBegan:
            if(event.type == NSEventTypeRotate && _magnifyStatus == wxNoState && _panningStatus == wxNoState)
            {
                impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );

                if (impl == nullptr)
                {
                    return;
                }

                wxTrackPadEvent tpevent(wxEVT_ROTATE);

                double angle = (double)[event rotation] * [TrackPadConstants rotationScaleFactor];
                tpevent.SetAngle(angle);

                _rotateStatus = wxRotationStart;
                tpevent.SetState(_rotateStatus);

                NSPoint mousePosition = [NSEvent mouseLocation];
                mousePosition = [self convertPoint:mousePosition fromView:nil];
                tpevent.SetPosition(wxRealPoint(mousePosition.x, mousePosition.y));

                impl->trackpadEvent(tpevent, event, self, _cmd);
            }

            [event release];
            break;

        case NSEventPhaseChanged:
            if(event.type == NSEventTypeRotate && _magnifyStatus == wxNoState && _panningStatus == wxNoState)
            {
                impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );

                if (impl == nullptr)
                {
                    return;
                }

                wxTrackPadEvent tpevent(wxEVT_ROTATE);

                double angle = (double)[event rotation] * [TrackPadConstants rotationScaleFactor];
                tpevent.SetAngle(angle);

                _rotateStatus = wxRotationMove;
                tpevent.SetState(_rotateStatus);

                impl->trackpadEvent(tpevent, event, self, _cmd);
            }

            [event release];
            break;

        case NSEventPhaseEnded:
        case NSEventPhaseCancelled:
            if(event.type == NSEventTypeRotate)
            {
                impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );

                if (impl == nullptr)
                {
                    return;
                }

                wxTrackPadEvent tpevent(wxEVT_ROTATE);

                _rotateStatus = wxRotationEnd;
                tpevent.SetState(_rotateStatus);

                impl->trackpadEvent(tpevent, event, self, _cmd);

                _rotateStatus = wxNoState;
            }

            [event release];
            break;
    }
}

- (void)dualGestureSmartMagnify:(DualTouchTracker*)tracker
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL){
        return;
    }

    wxTrackPadEvent tpevent(wxEVT_TAP);
    NSEvent * event = tracker.getCurrentGestureEvent;
    impl->trackpadEvent(tpevent, event, self, _cmd);
    [event release];
}

- (void)dualGestureMagnify:(DualTouchTracker*)tracker
{
    if(_isRubberSheet)
        return;

    wxWidgetCocoaImpl* impl = nullptr;
    NSEvent * event = tracker.getCurrentGestureEvent;

    switch ([event phase])
    {
        case NSEventPhaseBegan:
        case NSEventPhaseChanged:
            if(event.type == NSEventTypeMagnify && _rotateStatus == wxNoState && _panningStatus == wxNoState)
            {
                impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );

                if (impl == nullptr)
                {
                    return;
                }

                _magnifyStatus = wxMagnifying;

                wxTrackPadEvent tpevent(wxEVT_PINCH);

                double magValue = 1.0 + (double)[event magnification];

                tpevent.SetPinchMagnitude(magValue);

                NSPoint mousePosition = [NSEvent mouseLocation];
                mousePosition = [self convertPoint:mousePosition fromView:nil];
                tpevent.SetPosition(wxRealPoint(mousePosition.x, mousePosition.y));

                impl->trackpadEvent(tpevent, event, self, _cmd);
            }

            [event release];
            break;

        case NSEventPhaseEnded:
        case NSEventPhaseCancelled:
            if(event.type == NSEventTypeMagnify)
            {
                _magnifyStatus = wxNoState;
            }

            [event release];
            break;
    }
}
//end gestures

- (void)dualTouchesEnded:(DualTouchTracker*)tracker {

    tracker.updateTrackingAction = nil;
    tracker.endTrackingAction    = nil;
    tracker.oldDeltaSizeTouches  = 0;
    [self enableTrackers];

    if(_rubberSheetStatus != wxNoState)
    {
        _rubberSheetStatus = wxNoState;
        _panningStatus = wxNoState;
        _rotateStatus = wxNoState;
        _magnifyStatus = wxNoState;

        wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
        NSEvent * event = tracker.getCurrentTouchEvent;
        wxTrackPadEvent tpevent(wxEVT_RUBBER_SHEET);
        tpevent.SetState(wxRubberSheetEnd);
        impl->trackpadEvent(tpevent, event, self, _cmd);
    }
}

- (void)enableTrackers {
    for (InputTracker *tracker in inputTrackers) {
        tracker.isEnabled = YES;
    }
}

@end

// Bricsys change end: #22025 add TouchPadGesturesDelegate protocol and all the members

bool wxGLCanvas::DoCreate(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    DontCreatePeer();

    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    NSRect r = wxOSXGetFrameForControl( this, pos , size ) ;
    wxNSCustomOpenGLView* v = [[wxNSCustomOpenGLView alloc] initWithFrame:r];
    [v setWantsBestResolutionOpenGLSurface:YES];

    //Bricsys change, refs #22025 add false, true - otherwise key events are not processed
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( this, v, wxWidgetImpl::Widget_IsUserPane);
    //Bricsys change end
    SetPeer(c);
    MacPostControlCreate(pos, size) ;
    // Bricsys change, refs #22025 add event handler for backing properties change events
    // enable events for backing properties if done before SetPeer events would be
    // sent too early (with tiny size)
    v->eventHandler = this->GetEventHandler();
    // Bricsys change end
    return true;
}

wxGLCanvas::~wxGLCanvas()
{
    if ( m_glFormat )
        WXGLDestroyPixelFormat(m_glFormat);
}

bool wxGLCanvas::SwapBuffers()
{
    WXGLContext context = WXGLGetCurrentContext();
    wxCHECK_MSG(context, false, wxT("should have current context"));

    [context flushBuffer];

    return true;
}

class RAII_lockFocus
{
public:
    RAII_lockFocus(NSView* view)
    {
        m_pView = view;
        [m_pView lockFocus];
    }
    ~RAII_lockFocus()
    {
        [m_pView unlockFocus];
    }
private:
    NSView* m_pView;
};

// Bricsys change: add crosshair draw function
// helper functions
static void getContentViewPoints( const wxCursorLineArray& lines, const NSView* view, NSView* contentView, int index, NSPoint& pt1, NSPoint& pt2)
{
    NSPoint viewPt1 = NSMakePoint( lines[index].m_pt1.x, lines[index].m_pt1.y );
    NSPoint viewPt2 = NSMakePoint( lines[index].m_pt2.x, lines[index].m_pt2.y );
    
    pt1 = [view convertPoint: viewPt1 toView: contentView];
    pt2 = [view convertPoint: viewPt2 toView: contentView];
}

static void getMidPoint( const wxCursorLineArray& lines, const NSView* view, NSView* contentView, int index, NSPoint& midPoint)
{
    NSPoint contentViewPt1, contentViewPt2;
    getContentViewPoints( lines, view, contentView, index, contentViewPt1, contentViewPt2 );
    
    // calculate mid point of the current line
    CGFloat minX = fmin( contentViewPt1.x, contentViewPt2.x );
    CGFloat minY = fmin( contentViewPt1.y, contentViewPt2.y );
    CGFloat maxX = fmax( contentViewPt1.x, contentViewPt2.x );
    CGFloat maxY = fmax( contentViewPt1.y, contentViewPt2.y );
    midPoint = NSMakePoint( minX + (maxX - minX)/2, minY + (minY - minY)/2 );
}

void clearLines(NSGraphicsContext *nsContext, const wxCursorLineArray& lines, const NSView* view, NSView* contentView)
{
    [nsContext setCompositingOperation:NSCompositeClear];
    [[NSColor clearColor] set];
    NSBezierPath *line = [NSBezierPath bezierPath];
    [line setLineWidth:2];
    for( size_t i = 0; i < lines.size(); i++ )
    {
        NSPoint contentViewPt1, contentViewPt2;
        getContentViewPoints( lines, view, contentView, i, contentViewPt1, contentViewPt2 );
        
        [line moveToPoint: contentViewPt1];
        [line lineToPoint: contentViewPt2];
    }
    [line stroke];
}

void undrawLastCrosshair(NSGraphicsContext *nsContext, const wxCursorLineArray& lines, const NSView* view, NSView* contentView)
{
    bool lastUndraw = false;
    
    for( size_t i = 0; i < lines.size(); i++ )
    {
        NSPoint contentViewMid;
        getMidPoint( lines, view, contentView, i, contentViewMid );
        
        // check mid point's color
        NSColor* color = NSReadPixel(contentViewMid);
        CGFloat red, green, blue, alpha;
        [color getRed:&red  green:&green blue:&blue alpha:&alpha];
        if( red != 0.0 || green != 0.0 || blue != 0.0 || alpha != 0.0 )
        {
            lastUndraw = true;
            break;
        }
    }
    
    if( lastUndraw )
    {
        clearLines(nsContext, lines, view, contentView);
    }
}
// end helper functions

// we hide the openGL view behind the NSWindow's contentView and then we draw
// on the contentView (the default view of an NSWindow)
void wxGLCanvas::updateCrosshairCanvasMac( const wxCursorLineArray& lines, bool undraw )
{
    WXGLContext context = WXGLGetCurrentContext();
    NSView* openglView = [context view];
    NSWindow* window = openglView.window;
    NSView* contentView = [window contentView];
    if( lines.size() < 1 )
    {
        if(undraw)
        {
            RAII_lockFocus raii( contentView );
            NSGraphicsContext *nsContext = [NSGraphicsContext currentContext];
            
            undrawLastCrosshair( nsContext, m_lastLinesDrawn, openglView, contentView );
        }
        return;
    }
    
    RAII_lockFocus raii( contentView );
    NSGraphicsContext *nsContext = [NSGraphicsContext currentContext];
    
    // AB: sometimes there are multiple DRAW commands that arrive in succession (this still works in XOR mode on Linux/Win)
    // but here, we need to undraw the last draw, so we need to record it (like in a static variable)
    if( !undraw )
    {
        undrawLastCrosshair( nsContext, m_lastLinesDrawn, openglView, contentView );
    }
    
    if( undraw )
    {
        clearLines(nsContext, lines, openglView, contentView);
    }
    else
    {
        [nsContext setCompositingOperation:NSCompositeCopy];
        for( size_t i = 0; i < lines.size(); i++ )
        {
            NSBezierPath *line = [NSBezierPath bezierPath];
            
            CGFloat redC = 1.0-lines[i].m_color.Red()/255.0;
            CGFloat greenC = 1.0-lines[i].m_color.Green()/255.0;
            CGFloat blueC = 1.0-lines[i].m_color.Blue()/255.0;
            
            [[NSColor colorWithCalibratedRed:redC green:greenC blue:blueC alpha:1.0] set];
            
            NSPoint contentViewPt1, contentViewPt2;
            getContentViewPoints( lines, openglView, contentView, i, contentViewPt1, contentViewPt2 );
            
            [line moveToPoint: contentViewPt1];
            [line lineToPoint: contentViewPt2];
            [line stroke];
        }
        
        // record the last drawn lines
        m_lastLinesDrawn = lines;
    }
}
// end Bricsys change

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;

    [m_glContext setView: win.GetHandle() ];
    [m_glContext update];

    [m_glContext makeCurrentContext];

    return true;
}

//Bricsys change:
@interface wxCocoaRetinaHelperImpl : NSObject
{
    NSView *view;
    wxEvtHandler* handler;
}
-(id)initWithView:(NSView *)view handler:(wxEvtHandler *)handler;
-(void)setViewWantsBestResolutionOpenGLSurface:(BOOL)value;
-(BOOL)getViewWantsBestResolutionOpenGLSurface;
-(float)getBackingScaleFactor;
@end

@implementation wxCocoaRetinaHelperImpl

wxRetinaHelper::wxRetinaHelper(wxWindow* canvas) : m_pCanvas(canvas)
{
    if(m_pCanvas)
        m_pCocoaRetinaHelperImpl = [[wxCocoaRetinaHelperImpl alloc] initWithView:m_pCanvas->GetHandle() handler:m_pCanvas->GetEventHandler()];
    else
        m_pCocoaRetinaHelperImpl = nullptr;
}

wxRetinaHelper::~wxRetinaHelper()
{
    [(id)m_pCocoaRetinaHelperImpl release];
}

void wxRetinaHelper::setViewWantsBestResolutionOpenGLSurface (bool aValue)
{
    if(m_pCocoaRetinaHelperImpl)
        [(id)m_pCocoaRetinaHelperImpl setViewWantsBestResolutionOpenGLSurface:aValue];
}

bool wxRetinaHelper::getViewWantsBestResolutionOpenGLSurface()
{
    if(m_pCocoaRetinaHelperImpl)
        return [(id)m_pCocoaRetinaHelperImpl getViewWantsBestResolutionOpenGLSurface];
    else
        return false;
}

float wxRetinaHelper::getBackingScaleFactor()
{
    if(m_pCocoaRetinaHelperImpl)
        return [(id)m_pCocoaRetinaHelperImpl getBackingScaleFactor];
    else
        return 1;
}


-(id)initWithView:(NSView *)aView handler:(wxEvtHandler *)aHandler
{
    self = [super init];
    if(self)
    {
        handler = aHandler;
        view = aView;
        // register for backing change notifications
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        if(nc){
            [nc addObserver:self
                   selector:@selector(windowDidChangeBackingProperties:)
                       name:NSWindowDidChangeBackingPropertiesNotification
                     object:nil];
        }
    }
    return self;
}

-(void) dealloc
{
    // unregister from all notifications
    NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
    if(nc){
        [nc removeObserver:self];
    }
    [super dealloc];
}

-(void)setViewWantsBestResolutionOpenGLSurface:(BOOL)value
{
    [view setWantsBestResolutionOpenGLSurface:value];
}

-(BOOL)getViewWantsBestResolutionOpenGLSurface
{
    return [view wantsBestResolutionOpenGLSurface];
}

-(float)getBackingScaleFactor
{
    return [[view window] backingScaleFactor];
}

//scaling is adjusted depending on monitor(retina or non-retina displays) where window is currently placed on
- (void)windowDidChangeBackingProperties:(NSNotification *)notification {
    NSWindow *theWindow = (NSWindow *)[notification object];
    if(theWindow == [view window])
    {
        CGFloat newBackingScaleFactor = [theWindow backingScaleFactor];
        CGFloat oldBackingScaleFactor = [[[notification userInfo]
                                          objectForKey:@"NSBackingPropertyOldScaleFactorKey"]
                                         doubleValue];
        if (newBackingScaleFactor != oldBackingScaleFactor) {
            // generate a wx resize event and pass it to the handler's queue
            wxSizeEvent *event = new wxSizeEvent();
            // use the following line if this resize event should have the physical pixel resolution
            // but that is not recommended, because ordinary resize events won't do so either
            // which would necessitate a case-by-case switch in the resize handler method.
            // NSRect nsrect = [view convertRectToBacking:[view bounds]];
            NSRect nsrect = [view bounds];
            wxRect rect = wxRect(nsrect.origin.x, nsrect.origin.y, nsrect.size.width, nsrect.size.height);
            event->SetRect(rect);
            event->SetSize(rect.GetSize());
            handler->QueueEvent(event);
        }
    }
}
@end
// end Bricsys change
#endif // wxUSE_GLCANVAS
