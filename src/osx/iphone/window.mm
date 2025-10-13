/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/window.mm
// Purpose:     widgets (non tlw) for iphone
// Author:      Stefan Csomor
// Created:     2008-06-20
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/osx/private.h"

#ifndef WX_PRECOMP
    #include "wx/nonownedwnd.h"
    #include "wx/frame.h"
    #include "wx/event.h"
    #include "wx/log.h"
#endif

#include "wx/textctrl.h"

#include <objc/runtime.h>

#define TRACE_FOCUS "focus"
#define TRACE_KEYS  "keyevent"

// ----------------------------------------------------------------------------
// debugging helpers
// ----------------------------------------------------------------------------

// These functions are called from the code but are also useful in the debugger
// (especially wxDumpUIView(), as selectors can be printed out directly anyhow),
// so make them just static instead of putting them in an anonymous namespace
// to make it easier to call them.

static wxString wxDumpSelector(SEL cmd)
{
    return wxStringWithNSString(NSStringFromSelector(cmd));
}

static wxString wxDumpUIView(UIView* view)
{
    wxWidgetImpl* const impl = wxWidgetImpl::FindFromWXWidget(view);
    if ( !impl )
        return wxStringWithNSString([view description]);

    return wxString::Format("%s belonging to %s",
                            wxStringWithNSString([view className]),
                            wxDumpWindow(impl->GetWXPeer())
                            );
}

// ----------------------------------------------------------------------------
// Focus
// ----------------------------------------------------------------------------

WXWidget wxWidgetImpl::FindFocus()
{
    UIView* focusedView = nil;
    UIWindow* keyWindow = [[UIApplication sharedApplication] keyWindow];
    if ( keyWindow != nil )
    {
    /*
        NSResponder* responder = [keyWindow firstResponder];
        if ( [responder isKindOfClass:[NSTextView class]] &&
            [keyWindow fieldEditor:NO forObject:nil] != nil )
        {
            focusedView = [(NSTextView*)responder delegate];
        }
        else
        {
            if ( [responder isKindOfClass:[NSView class]] )
                focusedView = (NSView*) responder;
        }
    */
    }
    return focusedView;
}

CGRect wxOSXGetFrameForControl( wxWindowMac* window , const wxPoint& pos , const wxSize &size , bool adjustForOrigin )
{
    int x, y, w, h ;

    window->MacGetBoundsForControl( pos , size , x , y, w, h , adjustForOrigin ) ;
    wxRect bounds(x,y,w,h);
    UIView* sv = (window->GetParent()->GetHandle() );

    return wxToNSRect( sv, bounds );
}


@interface wxUIScrollViewDelegate : NSObject <UIScrollViewDelegate>
{
}

-(void)scrollViewDidScroll:(UIScrollView *)scrollView;

@end

@implementation wxUIScrollViewDelegate

- (id) init
{
    self = [super init];
    return self;
}

-(void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    wxWidgetIPhoneImpl* viewimpl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( scrollView );
    if ( viewimpl )
    {
        wxWindowMac* wxpeer = (wxWindowMac*) viewimpl->GetWXPeer();
        if (wxpeer)
        {
            CGPoint position = [scrollView contentOffset];

            viewimpl->SetDeviceLocalOrigin( wxPoint( -position.x, -position.y ) );

            if (viewimpl->GetBlockScrollEvents())
                return;

            // iOS already scrolled the window
            viewimpl->SetBlockScrollWindow( true );

            int unitY = position.y / viewimpl->GetYScrollPixelsPerLine();
            int offsetY = (int)(position.y) % viewimpl->GetYScrollPixelsPerLine();
            wxScrollWinEvent vevent( wxEVT_SCROLLWIN_THUMBTRACK, unitY, wxVERTICAL );
            vevent.SetPixelOffset( offsetY );
            vevent.SetEventObject( wxpeer );
            wxpeer->OSXGetScrollOwnerWindow()->HandleWindowEvent( vevent );

            int unitX = position.x / viewimpl->GetXScrollPixelsPerLine();
            int offsetX = (int)(position.x) % viewimpl->GetXScrollPixelsPerLine();
            wxScrollWinEvent hevent( wxEVT_SCROLLWIN_THUMBTRACK, unitX, wxHORIZONTAL );
            hevent.SetPixelOffset( offsetX );
            hevent.SetEventObject( wxpeer );
            wxpeer->OSXGetScrollOwnerWindow()->HandleWindowEvent( hevent );

            viewimpl->SetBlockScrollWindow( false );
        }
        viewimpl->SetNeedsDisplay();
    }
}

@end



@interface UIView(PossibleMethods)
- (void)setTitle:(NSString *)title forState:(UIControlState)state;

- (void)drawRect: (CGRect) rect;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)handleTouchEvent:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)pressesBegan:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesChanged:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesEnded:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesCancelled:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event;

- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;

- (BOOL)isEnabled;
- (void)setEnabled:(BOOL)flag;
@end

//
//
//

void SetupMouseEvent( wxMouseEvent &wxevent , NSSet* touches, UIEvent * nsEvent )
{
    UInt32 modifiers = 0 ;
    UITouch *touch = [touches anyObject];

    // these parameters are not given for all events
    UInt32 button = 0; // no secondary button
    UInt32 clickCount = [touch tapCount];
    UInt32 mouseChord = 0; // TODO does this exist for cocoa

    // will be overridden
    wxevent.m_x = 0;
    wxevent.m_y = 0;
    wxevent.m_shiftDown = 0;
    wxevent.m_controlDown = 0;
    wxevent.m_altDown = 0;
    wxevent.m_metaDown = 0;
    wxevent.m_clickCount = clickCount;
    wxevent.SetTimestamp( [touch timestamp] ) ;
/*
    // a control click is interpreted as a right click
    bool thisButtonIsFakeRight = false ;
    if ( button == kEventMouseButtonPrimary && (modifiers & controlKey) )
    {
        button = kEventMouseButtonSecondary ;
        thisButtonIsFakeRight = true ;
    }

    // otherwise we report double clicks by connecting a left click with a ctrl-left click
    if ( clickCount > 1 && button != g_lastButton )
        clickCount = 1 ;
    // we must make sure that our synthetic 'right' button corresponds in
    // mouse down, moved and mouse up, and does not deliver a right down and left up

    if ( cEvent.GetKind() == kEventMouseDown )
    {
        g_lastButton = button ;
        g_lastButtonWasFakeRight = thisButtonIsFakeRight ;
    }

    if ( button == 0 )
    {
        g_lastButton = 0 ;
        g_lastButtonWasFakeRight = false ;
    }
    else if ( g_lastButton == kEventMouseButtonSecondary && g_lastButtonWasFakeRight )
        button = g_lastButton ;

    // Adjust the chord mask to remove the primary button and add the
    // secondary button.  It is possible that the secondary button is
    // already pressed, e.g. on a mouse connected to a laptop, but this
    // possibility is ignored here:
    if( thisButtonIsFakeRight && ( mouseChord & 1U ) )
        mouseChord = ((mouseChord & ~1U) | 2U);

    if(mouseChord & 1U)
                wxevent.m_leftDown = true ;
    if(mouseChord & 2U)
                wxevent.m_rightDown = true ;
    if(mouseChord & 4U)
                wxevent.m_middleDown = true ;

*/
    // translate into wx types
    int eventType = [touch phase];
    switch (eventType)
    {
        case UITouchPhaseBegan :
            switch ( button )
            {
                case 0 :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_LEFT_DCLICK : wxEVT_LEFT_DOWN );
                    wxevent.SetLeftDown(true);
                    break ;

                default:
                    break ;
            }
            break ;

        case UITouchPhaseEnded :
            switch ( button )
            {
                case 0 :
                    wxevent.SetEventType( wxEVT_LEFT_UP );
                    wxevent.SetLeftDown(false);
                    break ;

                default:
                    break ;
            }
            break ;

        case UITouchPhaseMoved :
            wxevent.SetEventType( wxEVT_MOTION );
            wxevent.SetLeftDown(true);
            break;
        default :
            break ;
    }
}

@implementation wxUIView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

@end // wxUIView

typedef void (*wxOSX_touchEventHandlerPtr)(UIView* self, SEL _cmd, NSSet* touches, UIEvent *event );

void wxOSX_touchEvent(UIView* self, SEL _cmd, NSSet* touches, UIEvent *event )
{
    // Call the superclass handler first as that is e.g. handling mouse events for UISegmentedControls
    wxOSX_touchEventHandlerPtr superimpl =
        (wxOSX_touchEventHandlerPtr) [[self superclass] instanceMethodForSelector:_cmd];
    superimpl(self, _cmd, touches, event );

    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

    impl->touchEvent(touches, event, self, _cmd);
}

BOOL wxOSX_becomeFirstResponder(UIView* self, SEL _cmd)
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NO;

    return impl->becomeFirstResponder(self, _cmd);
}

BOOL wxOSX_resignFirstResponder(UIView* self, SEL _cmd)
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NO;

    return impl->resignFirstResponder(self, _cmd);
}

void wxOSX_keyEvent(UIView* self, SEL _cmd, NSSet* presses, UIEvent *event)
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
    {
        wxLogTrace(TRACE_KEYS, "Dropping %s for %s",
                   wxDumpSelector(_cmd), wxDumpUIView(self));
        return;
    }

    impl->keyEvent(presses, event, self, _cmd);
}

void wxOSX_drawRect(UIView* self, SEL _cmd, CGRect rect)
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

    return impl->drawRect(&rect, self, _cmd);
}


void wxOSXIPhoneClassAddWXMethods(Class c)
{
    class_addMethod(c, @selector(touchesBegan:withEvent:), (IMP) wxOSX_touchEvent, "v@:@@");
    class_addMethod(c, @selector(touchesMoved:withEvent:), (IMP) wxOSX_touchEvent, "v@:@@");
    class_addMethod(c, @selector(touchesEnded:withEvent:), (IMP) wxOSX_touchEvent, "v@:@@");
    class_addMethod(c, @selector(becomeFirstResponder), (IMP) wxOSX_becomeFirstResponder, "c@:" );
    class_addMethod(c, @selector(resignFirstResponder), (IMP) wxOSX_resignFirstResponder, "c@:" );

    class_addMethod(c, @selector(pressesBegan:withEvent:), (IMP) wxOSX_keyEvent, "v@:@@" );
    class_addMethod(c, @selector(pressesChanged:withEvent:), (IMP) wxOSX_keyEvent, "v@:@@" );
    class_addMethod(c, @selector(pressesEnded:withEvent:), (IMP) wxOSX_keyEvent, "v@:@@" );
    class_addMethod(c, @selector(pressesCancelled:withEvent:withEvent:), (IMP) wxOSX_keyEvent, "v@:@@" );

    class_addMethod(c, @selector(drawRect:), (IMP) wxOSX_drawRect, "v@:{_CGRect={_CGPoint=ff}{_CGSize=ff}}" );
}

//
// UIControl extensions
//

@interface UIControl (wxUIControlActionSupport)

- (void) WX_touchUpInsideAction:(id)sender event:(UIEvent*)event;
- (void) WX_valueChangedAction:(id)sender event:(UIEvent*)event;

@end

@implementation UIControl (wxUIControlActionSupport)

- (void) WX_touchUpInsideAction:(id)sender event:(UIEvent*)event
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl != nullptr)
        impl->controlAction(sender, UIControlEventTouchUpInside, event);
}

- (void) WX_valueChangedAction:(id)sender event:(UIEvent*)event
{
    wxWidgetIPhoneImpl* impl = (wxWidgetIPhoneImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl != nullptr)
        impl->controlAction(sender, UIControlEventValueChanged, event);
}

@end

wxIMPLEMENT_DYNAMIC_CLASS(wxWidgetIPhoneImpl , wxWidgetImpl);

wxWidgetIPhoneImpl::wxWidgetIPhoneImpl( wxWindowMac* peer , WXWidget w, int flags, void* controller ) :
    wxWidgetImpl( peer, flags ), m_osxView(w), m_controller(controller),
    m_blockScrollEvents(false), m_blockScrollWindow(false),
    m_xScrollPixelsPerLine(0), m_yScrollPixelsPerLine(0)

{
}

wxWidgetIPhoneImpl::wxWidgetIPhoneImpl()
{
}

void wxWidgetIPhoneImpl::Init()
{
    m_osxView = nullptr;
}

wxWidgetIPhoneImpl::~wxWidgetIPhoneImpl()
{
    RemoveAssociations( this );

    if ( !IsRootControl() )
    {
        UIView *sv = [m_osxView superview];
        if ( sv != nil )
            [m_osxView removeFromSuperview];
    }
    [m_osxView release];
}

bool wxWidgetIPhoneImpl::IsVisible() const
{
    UIView* view = m_osxView;
    while ( view != nil && [view isHidden] == NO )
    {
        view = [view superview];
    }
    return view == nil;
}

void wxWidgetIPhoneImpl::SetVisibility( bool visible )
{
    [m_osxView setHidden:(visible ? NO:YES)];
}

void wxWidgetIPhoneImpl::Raise()
{
    [[m_osxView superview] bringSubviewToFront:m_osxView];
}

void wxWidgetIPhoneImpl::Lower()
{
    [[m_osxView superview] sendSubviewToBack:m_osxView];
}

void wxWidgetIPhoneImpl::Move(int x, int y, int width, int height)
{
    CGRect r = CGRectMake( x, y, width, height) ;
    [m_osxView setFrame:r];
}



void wxWidgetIPhoneImpl::GetPosition( int &x, int &y ) const
{
    CGRect r = [m_osxView frame];
    x = r.origin.x;
    y = r.origin.y;
}

void wxWidgetIPhoneImpl::GetSize( int &width, int &height ) const
{
    CGRect rect = [m_osxView frame];
    width = rect.size.width;
    height = rect.size.height;
}

void wxWidgetIPhoneImpl::GetContentArea( int&left, int &top, int &width, int &height ) const
{
    left = top = 0;
    CGRect rect = [m_osxView bounds];
    width = rect.size.width;
    height = rect.size.height;
}

void wxWidgetIPhoneImpl::SetNeedsDisplay( const wxRect* WXUNUSED(where) )
{
    // TODO: Use "where" if non-null after adapting it to DeviceLocalOrigin
    [m_osxView setNeedsDisplay];
}

bool wxWidgetIPhoneImpl::GetNeedsDisplay() const
{
    return false;
//    return [m_osxView needsDisplay];
}

bool wxWidgetIPhoneImpl::CanFocus() const
{
    return [m_osxView canBecomeFirstResponder] == YES;
    // ? return [m_osxView isUserInteractionEnabled] == YES;
}

bool wxWidgetIPhoneImpl::HasFocus() const
{
    return [m_osxView isFirstResponder] == YES;
}

bool wxWidgetIPhoneImpl::SetFocus()
{
//    [m_osxView makeKeyWindow] ;
//  TODO
    return false;
}


void wxWidgetIPhoneImpl::RemoveFromParent()
{
    [m_osxView removeFromSuperview];
}

void wxWidgetIPhoneImpl::Embed( wxWidgetImpl *parent )
{
    UIView* container = parent->GetWXWidget() ;
    wxASSERT_MSG( container != nullptr , wxT("No valid mac container control") ) ;
    [container addSubview:m_osxView];
}

void  wxWidgetImpl::Convert( wxPoint *pt , wxWidgetImpl *from , wxWidgetImpl *to )
{
    // TODO: adapt to DeviceLocalOrigin
    CGPoint p = CGPointMake( pt->x , pt->y );
    p = [from->GetWXWidget() convertPoint:p toView:to->GetWXWidget() ];
    pt->x = (int)p.x;
    pt->y = (int)p.y;
}

void wxWidgetIPhoneImpl::SetBackgroundColour( const wxColour &col )
{
    m_osxView.backgroundColor = [UIColor colorWithCGColor:col.GetCGColor()];
}

void wxWidgetIPhoneImpl::SetForegroundColour( const wxColour &col )
{
    // TODO: use textColor if available?
}

bool wxWidgetIPhoneImpl::SetBackgroundStyle(wxBackgroundStyle style)
{
    if ( style == wxBG_STYLE_PAINT )
        [m_osxView setOpaque: YES ];
    else
    {
        [m_osxView setOpaque: NO ];
        m_osxView.backgroundColor = [UIColor clearColor];
    }
    return true;
}

void wxWidgetIPhoneImpl::SetLabel(const wxString& title)
{
    if ( [m_osxView respondsToSelector:@selector(setTitle:forState:) ] )
    {
        wxCFStringRef cf( title );
        [m_osxView setTitle:cf.AsNSString() forState:UIControlStateNormal ];
    }
#if 0 // nonpublic API problems
    else if ( [m_osxView respondsToSelector:@selector(setStringValue:) ] )
    {
        wxCFStringRef cf( title );
        [m_osxView setStringValue:cf.AsNSString()];
    }
#endif
}


void wxWidgetIPhoneImpl::SetCursor( const wxCursor & cursor )
{
}

void wxWidgetIPhoneImpl::CaptureMouse()
{
}

void wxWidgetIPhoneImpl::ReleaseMouse()
{
}

wxInt32 wxWidgetIPhoneImpl::GetValue() const
{
    return 0;
}

void wxWidgetIPhoneImpl::SetValue( wxInt32 v )
{
}

void wxWidgetIPhoneImpl::SetBitmap( const wxBitmapBundle& bitmap )
{
}

wxBitmap wxWidgetIPhoneImpl::GetBitmap() const
{
    wxBitmap bmp;
    return bmp;
}

void wxWidgetIPhoneImpl::SetBitmapPosition( wxDirection dir )
{
}

void wxWidgetIPhoneImpl::SetupTabs( const wxNotebook &notebook )
{
}

void wxWidgetIPhoneImpl::GetBestRect( wxRect *r ) const
{
    r->x = r->y = r->width = r->height = 0;

    if (  [m_osxView respondsToSelector:@selector(sizeToFit)] )
    {
        CGRect former = [m_osxView frame];
        [m_osxView sizeToFit];
        CGRect best = [m_osxView frame];
        [m_osxView setFrame:former];
        r->width = best.size.width;
        r->height = best.size.height;
    }
}

bool wxWidgetIPhoneImpl::IsEnabled() const
{
    UIView* targetView = m_osxView;
    // TODO add support for documentViews

    if ( [targetView respondsToSelector:@selector(isEnabled) ] )
        return [targetView isEnabled];

    return true;
}

void wxWidgetIPhoneImpl::Enable( bool enable )
{
    UIView* targetView = m_osxView;
    // TODO add support for documentViews

    if ( [targetView respondsToSelector:@selector(setEnabled:) ] )
        [targetView setEnabled:enable];
}

void wxWidgetIPhoneImpl::SetMinimum( wxInt32 v )
{
}

void wxWidgetIPhoneImpl::SetMaximum( wxInt32 v )
{
}

wxInt32 wxWidgetIPhoneImpl::GetMinimum() const
{
    return 0;
}

wxInt32 wxWidgetIPhoneImpl::GetMaximum() const
{
    return 0;
}

void wxWidgetIPhoneImpl::PulseGauge()
{
}

void wxWidgetIPhoneImpl::SetScrollPos(int orient, int pos)
{
    if (m_blockScrollWindow)
        return;
    wxUIView* view = (wxUIView*)m_osxView;  // wxUIView derives from UIScrollView

    CGPoint position = [view contentOffset];

    if (orient == wxVERTICAL)
    {
        if (m_yScrollPixelsPerLine == 0)
            return;
        position = CGPointMake( position.x, pos*m_yScrollPixelsPerLine );
    }
    else if (orient == wxHORIZONTAL)
    {
        if (m_xScrollPixelsPerLine == 0)
            return;
        position = CGPointMake( pos*m_xScrollPixelsPerLine, position.y );
    }

    m_blockScrollEvents = true;
    [view setContentOffset: position];
    m_blockScrollEvents = false;

    SetNeedsDisplay();
}

int wxWidgetIPhoneImpl::GetScrollPos(int orient) const
{
    wxUIView* view = (wxUIView*)m_osxView;  // wxUIView derives from UIScrollView

    CGPoint position = [view contentOffset];

    if ((orient == wxVERTICAL) && (m_yScrollPixelsPerLine != 0))
    {
        return position.y / m_yScrollPixelsPerLine;
    }
    if ((orient == wxHORIZONTAL) && (m_xScrollPixelsPerLine != 0))
    {
        return position.x / m_xScrollPixelsPerLine;
    }

    return 0;
}


void wxWidgetIPhoneImpl::ScrollRect( const wxRect *rect, int dx, int dy )
{
}

void wxWidgetIPhoneImpl::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    if (m_blockScrollWindow)
        return;

    wxUIView* view = (wxUIView*)m_osxView;  // wxUIView derives from UIScrollView

    CGPoint position = [view contentOffset];

    position.x -= dx;
    position.y -= dy;

    m_blockScrollEvents = true;
    [view setContentOffset: position];
    m_blockScrollEvents = false;

    SetNeedsDisplay();
}



void wxWidgetIPhoneImpl::SetScrollbar(int orient, int pos, int thumb,
                                     int range, bool refresh)
{
    wxUIView* view = (wxUIView*)m_osxView;  // wxUIView derives from UIScrollView

    if ( thumb == 0 )
        return;

    CGPoint position = [view contentOffset];
    CGSize size = [view contentSize];
    CGSize viewSize = [view bounds].size;

    if (orient == wxVERTICAL)
    {
        CGFloat yScrollPixelsPerLine = viewSize.height/thumb;
        size = CGSizeMake( size.width, range*yScrollPixelsPerLine );
        position = CGPointMake( position.x, pos*yScrollPixelsPerLine );
        m_yScrollPixelsPerLine = (int) yScrollPixelsPerLine;
    }
    else if (orient == wxHORIZONTAL)
    {
        CGFloat xScrollPixelsPerLine = viewSize.width/thumb;
        size = CGSizeMake( range*xScrollPixelsPerLine, size.height );
        position = CGPointMake( pos*xScrollPixelsPerLine, position.y );
        m_xScrollPixelsPerLine = (int) xScrollPixelsPerLine;
    }

    m_blockScrollEvents = true;
    [view setContentSize: size];
    [view setContentOffset: position];
    m_blockScrollEvents = false;

    SetNeedsDisplay();
}

void wxWidgetIPhoneImpl::SetScrollThumb( wxInt32 value, wxInt32 thumbSize )
{
}

void wxWidgetIPhoneImpl::SetControlSize( wxWindowVariant variant )
{
}

double wxWidgetIPhoneImpl::GetContentScaleFactor() const
{
    if ( [m_osxView respondsToSelector:@selector(contentScaleFactor) ])
        return [m_osxView contentScaleFactor];
    else
        return 1.0;
}

void wxWidgetIPhoneImpl::SetFont(const wxFont & font)
{
}

void wxWidgetIPhoneImpl::InstallEventHandler( WXWidget control )
{
    WXWidget c =  control ? control : (WXWidget) m_osxView;
    wxWidgetImpl::Associate( c, this ) ;

    if ([c isKindOfClass:[UIControl class] ])
    {
        UIControl* cc = (UIControl*) c;
        [cc addTarget:cc action:@selector(WX_touchUpInsideAction:event:) forControlEvents:UIControlEventTouchUpInside];
        [cc addTarget:cc action:@selector(WX_valueChangedAction:event:) forControlEvents:UIControlEventValueChanged];
    }
}

void wxWidgetIPhoneImpl::DoNotifyFocusEvent(bool receivedFocus, wxWidgetImpl* otherWindow)
{
    wxWindow* thisWindow = GetWXPeer();

    if ( receivedFocus )
    {
        wxLogTrace(wxT("Focus"), wxT("focus set(%p)"), static_cast<void*>(thisWindow));
        wxChildFocusEvent eventFocus((wxWindow*)thisWindow);
        thisWindow->HandleWindowEvent(eventFocus);

#if wxUSE_CARET
        if ( thisWindow->GetCaret() )
            thisWindow->GetCaret()->OnSetFocus();
#endif

        wxFocusEvent event(wxEVT_SET_FOCUS, thisWindow->GetId());
        event.SetEventObject(thisWindow);
        if (otherWindow)
            event.SetWindow(otherWindow->GetWXPeer());
        thisWindow->HandleWindowEvent(event) ;
    }
    else // !receivedFocuss
    {
#if wxUSE_CARET
        if ( thisWindow->GetCaret() )
            thisWindow->GetCaret()->OnKillFocus();
#endif

        wxLogTrace(wxT("Focus"), wxT("focus lost(%p)"), static_cast<void*>(thisWindow));

        wxFocusEvent event( wxEVT_KILL_FOCUS, thisWindow->GetId());
        event.SetEventObject(thisWindow);
        if (otherWindow)
            event.SetWindow(otherWindow->GetWXPeer());
        thisWindow->HandleWindowEvent(event) ;
    }
}

typedef void (*wxOSX_DrawRectHandlerPtr)(UIView* self, SEL _cmd, CGRect rect);
typedef BOOL (*wxOSX_FocusHandlerPtr)(UIView* self, SEL _cmd);
typedef void (*wxOSX_PressesHandlerPtr)(UIView* self, SEL _cmd, NSSet* presses, UIEvent* event);

bool wxWidgetIPhoneImpl::becomeFirstResponder(WXWidget slf, void *_cmd)
{
    wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    // get the current focus before running becomeFirstResponder
    UIView* otherView = FindFocus();
    wxWidgetImpl* otherWindow = FindFromWXWidget(otherView);
    BOOL r = superimpl(slf, (SEL)_cmd);
    if ( r )
    {
        DoNotifyFocusEvent( true, otherWindow );
    }
    return r;
}

bool wxWidgetIPhoneImpl::resignFirstResponder(WXWidget slf, void *_cmd)
{
    wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    BOOL r = superimpl(slf, (SEL)_cmd);
    // get the current focus after running resignFirstResponder
    UIView* otherView = FindFocus();
    wxWidgetImpl* otherWindow = FindFromWXWidget(otherView);
    // NSTextViews have an editor as true responder, therefore the might get the
    // resign notification if their editor takes over, don't trigger any event hen
    if ( r && otherWindow != this)
    {
        DoNotifyFocusEvent( false, otherWindow );
    }
    return r;
}

void wxWidgetIPhoneImpl::drawRect(CGRect* rect, WXWidget slf, void *WXUNUSED(_cmd))
{
    wxRegion updateRgn( wxFromNSRect( slf, *rect ) );
    updateRgn.Offset( GetDeviceLocalOrigin() );

    wxWindow* wxpeer = GetWXPeer();
    wxpeer->GetUpdateRegion() = updateRgn;

    // note that context may be null in certain views
    CGContextRef context = (CGContextRef) UIGraphicsGetCurrentContext();
    if ( context != nullptr )
    {
        CGContextSaveGState( context );
    // draw background
/*
        CGContextSetFillColorWithColor( context, GetWXPeer()->GetBackgroundColour().GetCGColor());
        CGContextFillRect(context, *rect );
*/
    }
        GetWXPeer()->MacSetCGContextRef( context );

    wxpeer->MacSetCGContextRef( context );

    bool handled = wxpeer->MacDoRedraw( 0 );

    if ( context != nullptr )
    {
        CGContextRestoreGState( context );
        CGContextSaveGState( context );
    }

    if ( !handled )
    {
        // call super
        SEL _cmd = @selector(drawRect:);
        wxOSX_DrawRectHandlerPtr superimpl = (wxOSX_DrawRectHandlerPtr) [[slf superclass] instanceMethodForSelector:_cmd];
        if ( superimpl != wxOSX_drawRect )
        {
            superimpl(slf, _cmd, *rect);
            if ( context != nullptr )
            {
                CGContextRestoreGState( context );
                CGContextSaveGState( context );
            }
        }
    }

    if ( context != nullptr )
    {
        wxpeer->MacPaintChildrenBorders();
        CGContextRestoreGState( context );
    }

    wxpeer->MacSetCGContextRef( nullptr );
}

void wxWidgetIPhoneImpl::touchEvent(NSSet* touches, UIEvent *event, WXWidget slf, void *WXUNUSED(_cmd))
{
    bool inRecursion = false;
    if ( inRecursion )
        return;

    UITouch *touch = [touches anyObject];
    CGPoint clickLocation;
    if ( [touch view] != slf && IsRootControl() )
    {
        NSLog(@"self is %@ and touch view is %@",slf,[touch view]);
        inRecursion = true;
        inRecursion = false;
    }
    else
    {
        clickLocation = [touch locationInView:slf];
        wxPoint pt = wxFromNSPoint( m_osxView, clickLocation );

        wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
        SetupMouseEvent( wxevent , touches, event ) ;
        wxevent.m_x = pt.x + m_deviceLocalOrigin.x;
        wxevent.m_y = pt.y + m_deviceLocalOrigin.y;
        wxevent.SetEventObject( GetWXPeer() ) ;
        //?wxevent.SetId( GetWXPeer()->GetId() ) ;

        GetWXPeer()->HandleWindowEvent(wxevent);
    }
}

void wxWidgetIPhoneImpl::controlAction(void* sender, wxUint32 controlEvent, WX_UIEvent rawEvent)
{
    if ( controlEvent == UIControlEventTouchUpInside )
        GetWXPeer()->OSXHandleClicked(0);
    else if ( controlEvent == UIControlEventValueChanged )
        GetWXPeer()->OSXHandleClicked(0);
}

void wxWidgetIPhoneImpl::controlTextDidChange()
{
    wxTextCtrl* wxpeer = wxDynamicCast((wxWindow*)GetWXPeer(),wxTextCtrl);
    if ( wxpeer )
    {
        wxCommandEvent event(wxEVT_TEXT, wxpeer->GetId());
        event.SetEventObject( wxpeer );
        event.SetString( wxpeer->GetValue() );
        wxpeer->HandleWindowEvent( event );
    }
}

long wxOSXTranslateKey( UIPress* press, int eventType )
{
    long retval = 0;
    UIKey* key = press.key;

    if ( key == nil )
        return 0;

    UIKeyboardHIDUsage keyCode = [key keyCode];
    switch( keyCode )
    {
        // command key
        case UIKeyboardHIDUsageKeyboardLeftGUI:
        case UIKeyboardHIDUsageKeyboardRightGUI:
            retval = WXK_CONTROL;
            break;
        // caps locks key
        case UIKeyboardHIDUsageKeyboardCapsLock: // Capslock
            retval = WXK_CAPITAL;
            break;
        // shift key
        case UIKeyboardHIDUsageKeyboardLeftShift: // Left Shift
        case UIKeyboardHIDUsageKeyboardRightShift: // Right Shift
            retval = WXK_SHIFT;
            break;
        // alt key
        case UIKeyboardHIDUsageKeyboardLeftAlt: // Left Alt
        case UIKeyboardHIDUsageKeyboardRightAlt: // Right Alt
            retval = WXK_ALT;
            break;
        // ctrl key
        case UIKeyboardHIDUsageKeyboardLeftControl: // Left Ctrl
        case UIKeyboardHIDUsageKeyboardRightControl: // Right Ctrl
            retval = WXK_RAW_CONTROL;
            break;
        // clear key
        case UIKeyboardHIDUsageKeyboardClear:
            retval = WXK_CLEAR;
            break;
        // tab key
        case UIKeyboardHIDUsageKeyboardTab:
            retval = WXK_TAB;
            break;

        // numpad enter key End-of-text character ETX U+0003
        case UIKeyboardHIDUsageKeypadEnter:
            retval = WXK_NUMPAD_ENTER;
            break;
        // backspace key
        case UIKeyboardHIDUsageKeyboardDeleteOrBackspace :
            retval = WXK_BACK;
            break;
        case UIKeyboardHIDUsageKeyboardUpArrow :
            retval = WXK_UP;
            break;
        case UIKeyboardHIDUsageKeyboardDownArrow :
            retval = WXK_DOWN;
            break;
        case UIKeyboardHIDUsageKeyboardLeftArrow :
            retval = WXK_LEFT;
            break;
        case UIKeyboardHIDUsageKeyboardRightArrow :
            retval = WXK_RIGHT;
            break;
        case UIKeyboardHIDUsageKeyboardInsert  :
            retval = WXK_INSERT;
            break;
        case UIKeyboardHIDUsageKeyboardDeleteForward  :
            retval = WXK_DELETE;
            break;
        case UIKeyboardHIDUsageKeyboardHome  :
            retval = WXK_HOME;
            break;
        case UIKeyboardHIDUsageKeyboardEnd  :
            retval = WXK_END;
            break;
        case UIKeyboardHIDUsageKeyboardPageUp  :
            retval = WXK_PAGEUP;
            break;
       case UIKeyboardHIDUsageKeyboardPageDown  :
            retval = WXK_PAGEDOWN;
            break;
       case UIKeyboardHIDUsageKeyboardHelp  :
            retval = WXK_HELP;
            break;
        case UIKeyboardHIDUsageKeyboard0:
            retval = '0';
            break;
        default:
            if ( keyCode >= UIKeyboardHIDUsageKeyboardA && keyCode <= UIKeyboardHIDUsageKeyboardZ )
            {
                retval = 'a' + (keyCode - UIKeyboardHIDUsageKeyboardA );
            }
            else if ( keyCode >= UIKeyboardHIDUsageKeyboard1 && keyCode <= UIKeyboardHIDUsageKeyboard9 )
            {
                retval = '1' + (keyCode - UIKeyboardHIDUsageKeyboard1 );
            }
            else if ( keyCode >= UIKeyboardHIDUsageKeyboardF1 && keyCode <= UIKeyboardHIDUsageKeyboardF12 )
            {
                retval = WXK_F1 + (keyCode - UIKeyboardHIDUsageKeyboardF1 );
            }
            break;
    }

    // Check for NUMPAD keys.  For KEY_UP/DOWN events we need to use the
    // WXK_NUMPAD constants, but for the CHAR event we want to use the
    // standard ascii values
    if ( eventType != wxEVT_CHAR )
    {
        switch( [key keyCode] )
        {
            case UIKeyboardHIDUsageKeypadSlash: // /
                retval = WXK_NUMPAD_DIVIDE;
                break;
            case UIKeyboardHIDUsageKeypadAsterisk: // *
                retval = WXK_NUMPAD_MULTIPLY;
                break;
            case UIKeyboardHIDUsageKeypadHyphen: // -
                retval = WXK_NUMPAD_SUBTRACT;
                break;
            case UIKeyboardHIDUsageKeypadPlus: // +
                retval = WXK_NUMPAD_ADD;
                break;
            case UIKeyboardHIDUsageKeypadPeriod: // .
                retval = WXK_NUMPAD_DECIMAL;
                break;
            case UIKeyboardHIDUsageKeypad0: // 0
                retval = WXK_NUMPAD0;
                break;
            case UIKeyboardHIDUsageKeypad1: // 1
                retval = WXK_NUMPAD1;
                break;
            case UIKeyboardHIDUsageKeypad2: // 2
                retval = WXK_NUMPAD2;
                break;
            case UIKeyboardHIDUsageKeypad3: // 3
                retval = WXK_NUMPAD3;
                break;
            case UIKeyboardHIDUsageKeypad4: // 4
                retval = WXK_NUMPAD4;
                break;
            case UIKeyboardHIDUsageKeypad5: // 5
                retval = WXK_NUMPAD5;
                break;
            case UIKeyboardHIDUsageKeypad6: // 6
                retval = WXK_NUMPAD6;
                break;
            case UIKeyboardHIDUsageKeypad7: // 7
                retval = WXK_NUMPAD7;
                break;
            case UIKeyboardHIDUsageKeypad8: // 8
                retval = WXK_NUMPAD8;
                break;
            case UIKeyboardHIDUsageKeypad9: // 9
                retval = WXK_NUMPAD9;
                break;
            default:
                //retval = [event keyCode];
                break;
        }
    }
    return retval;
}
void wxWidgetIPhoneImpl::SetupKeyEvent(wxKeyEvent &wxevent , UIEvent* nsEvent, UIPress* press, NSString* charString)
{
    UIPressPhase pressPhase = press.phase;

    wxevent.SetTimestamp( static_cast<long>([press timestamp] * 1000) ) ;

    long keyval = 0;

    if ( press.key )
    {
        UIKeyModifierFlags modifiers = [press.key modifierFlags];

        wxevent.m_shiftDown = modifiers & UIKeyModifierShift; // UIKeyModifierAlphaShift for capslock
        wxevent.m_rawControlDown = modifiers & UIKeyModifierControl;
        wxevent.m_altDown = modifiers & UIKeyModifierAlternate;
        wxevent.m_controlDown = modifiers & UIKeyModifierCommand;
        wxevent.m_rawCode = (wxUint32) ([press.key keyCode]);
        wxevent.m_rawFlags =(wxUint32) modifiers;

    }

    if (wxevent.GetEventType() != wxEVT_CHAR)
    {
        keyval = wxOSXTranslateKey(press, wxevent.GetEventType()) ;
        switch (pressPhase)
        {
            case UIPressPhaseBegan :
                wxevent.SetEventType( wxEVT_KEY_DOWN )  ;
                break;
            case UIPressPhaseStationary:
                wxevent.SetEventType( wxEVT_KEY_DOWN )  ;
                wxevent.m_isRepeat = true;
                break;
            case UIPressPhaseEnded :
                wxevent.SetEventType( wxEVT_KEY_UP )  ;
                break;
            default :
                break ;
        }
    }
    else
    {
        long keycode = wxOSXTranslateKey( press, wxEVT_CHAR );
        if ( (keycode > 0 && keycode < WXK_SPACE) || keycode == WXK_DELETE || keycode >= WXK_START )
        {
            keyval = keycode;
        }
    }

    wxevent.m_keyCode = keyval;

    wxWindowMac* peer = GetWXPeer();
    if ( peer )
    {
        wxevent.SetEventObject(peer);
        wxevent.SetId(peer->GetId()) ;
    }
}

void wxWidgetIPhoneImpl::keyEvent( NSSet *presses, UIEvent* event, WXWidget slf, void *_cmd)
{
    wxLogTrace(TRACE_KEYS, "Got %s for %s",
               wxDumpSelector((SEL)_cmd), wxDumpUIView(slf));

    if ( !m_wxPeer->IsEnabled() )
        return;

    wxKeyEvent wxevent;

    bool handled = false;

    for (UIPress *press in (NSSet<UIPress *> *) presses)
    {
        SetupKeyEvent(wxevent, event, press , nil);
        m_wxPeer->OSXHandleKeyEvent(wxevent);
    }

    if ( !handled )
    {
        wxOSX_PressesHandlerPtr superimpl = (wxOSX_PressesHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        superimpl(slf, (SEL)_cmd, presses, event);
    }
}
//
// Factory methods
//

wxWidgetImpl* wxWidgetImpl::CreateUserPane( wxWindowMac* wxpeer, wxWindowMac* WXUNUSED(parent),
    wxWindowID WXUNUSED(id), const wxPoint& pos, const wxSize& size,
    long WXUNUSED(style), long WXUNUSED(extraStyle))
{
    UIView* sv = (wxpeer->GetParent()->GetHandle() );

    CGRect r = CGRectMake( pos.x, pos.y, size.x, size.y) ;
    wxUIView* v = [[wxUIView alloc] initWithFrame:r];
    v.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
    wxUIScrollViewDelegate *delegate = [[wxUIScrollViewDelegate alloc] init];
    [v setDelegate: delegate];
    sv.clipsToBounds = YES;
    sv.contentMode =  UIViewContentModeRedraw;
    sv.clearsContextBeforeDrawing = NO;
    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl( wxpeer, v, Widget_IsUserPane );
    return c;
}

