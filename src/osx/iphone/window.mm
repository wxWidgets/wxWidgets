/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/window.mm
// Purpose:     widgets (non tlw) for iphone
// Author:      Stefan Csomor
// Modified by:
// Created:     2008-06-20
// RCS-ID:      $Id: window.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/osx/private.h"

#include "wx/event.h"
#include "wx/nonownedwnd.h"
#include "wx/frame.h"

@interface wxUIView : UIView
{
    wxWidgetImpl* ;
}

- (void)drawRect: (CGRect) rect;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)handleTouchEvent:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)setImplementation: (wxWidgetImpl *) theImplementation;
- (wxWidgetImpl*) implementation;
- (BOOL) isFlipped;
- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;

@end // wxUIView

@interface wxUIContentView : wxUIView
{
}

@end

@interface wxUIContentViewController : UIViewController
{
}

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
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_LEFT_DCLICK : wxEVT_LEFT_DOWN )  ;
                    break ;

                default:
                    break ;
            }
            break ;

        case UITouchPhaseEnded :
            switch ( button )
            {
                case 0 :
                    wxevent.SetEventType( wxEVT_LEFT_UP )  ;
                    break ;

                default:
                    break ;
            }
            break ;

        case UITouchPhaseMoved :
            wxevent.SetEventType( wxEVT_MOTION ) ;
            break;
        default :
            break ;
    }
}

@implementation wxUIView

- (id) initWithFrame: (CGRect) frame
{
    if ( self = [super initWithFrame:frame])
    {
        // additional inits
    }
    return self;
}

- (void) dealloc
{
    // additional deallocs
    [super dealloc];
}

- (void)drawRect: (CGRect) rect
{
    if (  )
    {
        CGContextRef context = (CGContextRef) UIGraphicsGetCurrentContext();
        CGContextSaveGState( context );
        // draw background
        
        CGContextSetFillColorWithColor( context, ->GetWXPeer()->GetBackgroundColour().GetCGColor());
        CGContextFillRect(context, rect );

        if ( [ self isFlipped ] == NO )
        {
            CGContextTranslateCTM( context, 0,  [self bounds].size.height );
            CGContextScaleCTM( context, 1, -1 );
        }

        ->GetWXPeer()->MacSetCGContextRef( context );

        ->GetWXPeer()->GetUpdateRegion() = 
            wxRegion(rect.origin.x,rect.origin.y,rect.size.width,rect.size.height) ;

        wxPaintEvent event;
        event.SetTimestamp(0); //  todo
        event.SetEventObject(->GetWXPeer());
        ->GetWXPeer()->HandleWindowEvent(event);

        CGContextRestoreGState( context );
    }

}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
    [self handleTouchEvent:touches withEvent:event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{
    [self handleTouchEvent:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
    [self handleTouchEvent:touches withEvent:event];
}

-(void)handleTouchEvent:(NSSet *)touches withEvent:(UIEvent *)event 
{ 
    CGPoint clickLocation; 
    UITouch *touch = [touches anyObject];
    clickLocation = [touch locationInView:self];

    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
    SetupMouseEvent( wxevent , touches, event ) ;
    wxevent.m_x = clickLocation.x;
    wxevent.m_y = clickLocation.y;
    wxevent.SetEventObject( ->GetWXPeer() ) ;
    wxevent.SetId( ->GetWXPeer()->GetId() ) ;
    ->GetWXPeer()->HandleWindowEvent(wxevent);
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation
{
     = theImplementation;
}

- (wxWidgetImpl*) implementation
{
    return ;
}

- (BOOL) isFlipped
{
    return YES;
}

- (BOOL) becomeFirstResponder
{
    BOOL r = [super becomeFirstResponder];
    if ( r )
    {
    }
    return r;
}

- (BOOL) resignFirstResponder
{
    BOOL r = [super resignFirstResponder];
    if ( r )
    {
    }
    return r;
}


@end // wxUIView

@implementation wxUIContentView

@end

@implementation wxUIContentViewController

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation) interfaceOrientation
{
    return YES;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    CGRect fr = [self.view frame];
//    CGRect cv = [[self.view superview] frame];
//    CGRect bounds = CGRectMake( 0,0,fr.size.width, fr.size.height);
//    [[self.view superview] setFrame: fr ];
//    [self.view setFrame: bounds];
//    [self.view setNeedsDisplayInRect:bounds];
}

@end


IMPLEMENT_DYNAMIC_CLASS( wxWidgetIPhoneImpl , wxWidgetImpl )

wxWidgetIPhoneImpl::wxWidgetIPhoneImpl( wxWindowMac* peer , WXWidget w, bool isRootControl ) :
    wxWidgetImpl( peer, isRootControl ), m_osxView(w)
{
}

wxWidgetIPhoneImpl::wxWidgetIPhoneImpl() 
{
}

void wxWidgetIPhoneImpl::Init()
{
    m_osxView = NULL;
}

wxWidgetIPhoneImpl::~wxWidgetIPhoneImpl()
{
    [m_osxView setImplementation:NULL];
    [m_osxView release];
}
    
bool wxWidgetIPhoneImpl::IsVisible() const 
{
    // TODO Superviews
    return [m_osxView isHidden] == NO;
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

void wxWidgetIPhoneImpl::ScrollRect( const wxRect *rect, int dx, int dy )
{
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
    GetSize( width, height );
}

void wxWidgetIPhoneImpl::SetNeedsDisplay( const wxRect* where )
{
    if ( where )
    {
        CGRect r = CGRectMake( where->x, where->y, where->width, where->height) ;
        [m_osxView setNeedsDisplayInRect:r];
    }
    else
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
    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;
    [container addSubview:m_osxView];
}

void  wxWidgetImpl::Convert( wxPoint *pt , wxWidgetImpl *from , wxWidgetImpl *to )
{
    CGPoint p = CGPointMake( pt->x , pt->y );
    p = [from->GetWXWidget() convertPoint:p toView:to->GetWXWidget() ]; 
    pt->x = (int)p.x;
    pt->y = (int)p.y;
}

void wxWidgetIPhoneImpl::SetBackgroundColour( const wxColour &col )
{
    // m_osxView.backgroundColor = [[UIColor alloc] initWithCGColor:col.GetCGColor()];
}

//
// Factory methods
//

wxWidgetImpl* wxWidgetImpl::CreateUserPane( wxWindowMac* wxpeer, const wxPoint& pos, const wxSize& size,
                            long style, long extraStyle)
{
    UIView* sv = (wxpeer->GetParent()->GetHandle() );
    
    CGRect r = CGRectMake( pos.x, pos.y, size.x, size.y) ;
    // Rect bounds = wxMacGetBoundsForControl( wxpeer, pos , size ) ;
    wxUIView* v = [[wxUIView alloc] initWithFrame:r];
    sv.clipsToBounds = YES;
    sv.contentMode =  UIViewContentModeRedraw;
    sv.clearsContextBeforeDrawing = NO;
    // sv.backgroundColor = [[UIColor alloc] initWithCGColor:wxpeer->GetBackgroundColour().GetCGColor()];
    [sv addSubview:v];
    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

wxWidgetImpl* wxWidgetImpl::CreateContentView( wxNonOwnedWindow* now ) 
{
    UIWindow* toplevelwindow = now->GetWXWindow();

    wxUIContentView* contentview = [[wxUIContentView alloc] initWithFrame:[toplevelwindow bounds]];
    contentview.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    wxUIContentViewController* controller = [[wxUIContentViewController alloc] init];
    controller.view = contentview;
    /*
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    // left orientation not yet implemented !
    if (orientation == UIInterfaceOrientationLandscapeRight ) 
    {
        CGAffineTransform transform = v.transform;
 
        // Use the status bar frame to determine the center point of the window's content area.
        CGRect statusBarFrame = [[UIApplication sharedApplication] statusBarFrame];
        CGRect bounds = CGRectMake(0, 0, statusBarFrame.size.height, statusBarFrame.origin.x);
        CGPoint center = CGPointMake(bounds.size.height / 2.0, bounds.size.width / 2.0);
 
        // Set the center point of the view to the center point of the window's content area.
        v.center = center;
 
        // Rotate the view 90 degrees around its new center point.
        transform = CGAffineTransformRotate(transform, ( M_PI / 2.0));
        v.transform = transform;
    }
    */
    wxWidgetIPhoneImpl* impl = new wxWidgetIPhoneImpl( now, contentview, true );
    [contentview setImplementation:impl];
    [toplevelwindow addSubview:contentview];
    return impl;
}

