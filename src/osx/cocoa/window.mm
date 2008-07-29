/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/window.mm
// Purpose:     widgets (non tlw) for cocoa
// Author:      Stefan Csomor
// Modified by:
// Created:     2008-06-20
// RCS-ID:      $Id: window.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include <Cocoa/Cocoa.h>

#ifdef __WXMAC__
#include "wx/osx/private.h"
#endif

#if wxOSX_USE_COCOA

@interface wxNSView : NSView
{
    wxWidgetImpl* m_impl;
}

- (void)drawRect: (NSRect) rect;

-(void)mouseDown:(NSEvent *)event ;
-(void)rightMouseDown:(NSEvent *)event ;
-(void)otherMouseDown:(NSEvent *)event ;
-(void)mouseUp:(NSEvent *)event ;
-(void)rightMouseUp:(NSEvent *)event ;
-(void)otherMouseUp:(NSEvent *)event ;
-(void)handleMouseEvent:(NSEvent *)event;

- (void)setImplementation: (wxWidgetImpl *) theImplementation;
- (wxWidgetImpl*) implementation;
- (BOOL) isFlipped;
- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;

@end // wxNSView

void SetupMouseEvent( wxMouseEvent &wxevent , NSEvent * nsEvent )
{
    UInt32 modifiers = [nsEvent modifierFlags] ;
    wxPoint screenMouseLocation = wxFromNSPoint( NULL, [nsEvent locationInWindow]);

    // these parameters are not given for all events
    UInt32 button = [nsEvent buttonNumber];
    UInt32 clickCount = [nsEvent clickCount];
    UInt32 mouseChord = 0; // TODO does this exist for cocoa

    wxevent.m_x = screenMouseLocation.x;
    wxevent.m_y = screenMouseLocation.y;
    wxevent.m_shiftDown = modifiers & NSShiftKeyMask;
    wxevent.m_controlDown = modifiers & NSControlKeyMask;
    wxevent.m_altDown = modifiers & NSAlternateKeyMask;
    wxevent.m_metaDown = modifiers & NSCommandKeyMask;
    wxevent.m_clickCount = clickCount;
    wxevent.SetTimestamp( [nsEvent timestamp] ) ;
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
    int eventType = [nsEvent type];
    switch (eventType)
    {
        case NSLeftMouseDown :
        case NSRightMouseDown :
        case NSOtherMouseDown :
            switch ( button )
            {
                case 0 :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_LEFT_DCLICK : wxEVT_LEFT_DOWN )  ;
                    break ;

                case 1 :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_RIGHT_DCLICK : wxEVT_RIGHT_DOWN ) ;
                    break ;

                case 2 :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_MIDDLE_DCLICK : wxEVT_MIDDLE_DOWN ) ;
                    break ;

                default:
                    break ;
            }
            break ;

        case NSLeftMouseUp :
        case NSRightMouseUp :
        case NSOtherMouseUp :
            switch ( button )
            {
                case 0 :
                    wxevent.SetEventType( wxEVT_LEFT_UP )  ;
                    break ;

                case 1 :
                    wxevent.SetEventType( wxEVT_RIGHT_UP ) ;
                    break ;

                case 2 :
                    wxevent.SetEventType( wxEVT_MIDDLE_UP ) ;
                    break ;

                default:
                    break ;
            }
            break ;

     case NSScrollWheel :
        {
            wxevent.SetEventType( wxEVT_MOUSEWHEEL ) ;
            /*
            EventMouseWheelAxis axis = cEvent.GetParameter<EventMouseWheelAxis>(kEventParamMouseWheelAxis, typeMouseWheelAxis) ;
            SInt32 delta = cEvent.GetParameter<SInt32>(kEventParamMouseWheelDelta, typeSInt32) ;
            */
            wxevent.m_wheelRotation = 10; // delta;
            wxevent.m_wheelDelta = 1;
            wxevent.m_linesPerAction = 1;
            if ( 0 /* axis == kEventMouseWheelAxisX*/ )
                wxevent.m_wheelAxis = 1;
        }
        break ;

        case NSMouseEntered :
        case NSMouseExited :
        case NSLeftMouseDragged :
        case NSRightMouseDragged :
        case NSOtherMouseDragged :
        case NSMouseMoved :
            wxevent.SetEventType( wxEVT_MOTION ) ;
            break;
        default :
            break ;
    }
}

@implementation wxNSView

- (void)drawRect: (NSRect) rect
{
    if ( m_impl )
    {
        CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
        CGContextSaveGState( context );
        CGContextBeginPath( context );
        CGContextMoveToPoint(context, 0, 0);
        NSRect bounds = [self bounds];
        CGContextAddLineToPoint(context, 10, 0);
        CGContextMoveToPoint(context, 0, 0);
        CGContextAddLineToPoint(context, 0, 10);
        CGContextMoveToPoint(context, bounds.size.width, bounds.size.height);
        CGContextAddLineToPoint(context, bounds.size.width, bounds.size.height-10);
        CGContextMoveToPoint(context, bounds.size.width, bounds.size.height);
        CGContextAddLineToPoint(context, bounds.size.width-10, bounds.size.height);
        CGContextClosePath( context );
        CGContextStrokePath(context);

        if ( [ self isFlipped ] == NO )
        {
            CGContextTranslateCTM( context, 0,  [self bounds].size.height );
            CGContextScaleCTM( context, 1, -1 );
        }
        m_impl->GetWXPeer()->MacSetCGContextRef( context );

        wxPaintEvent event;
        event.SetTimestamp(0); //  todo
        event.SetEventObject(m_impl->GetWXPeer());
        m_impl->GetWXPeer()->HandleWindowEvent(event);
        
        CGContextRestoreGState( context );
    }
}

-(void)mouseDown:(NSEvent *)event 
{
    [self handleMouseEvent:event];
}

-(void)rightMouseDown:(NSEvent *)event 
{
    [self handleMouseEvent:event];
}

-(void)otherMouseDown:(NSEvent *)event 
{
    [self handleMouseEvent:event];
}

-(void)mouseUp:(NSEvent *)event 
{
    [self handleMouseEvent:event];
}

-(void)rightMouseUp:(NSEvent *)event 
{
    [self handleMouseEvent:event];
}

-(void)otherMouseUp:(NSEvent *)event 
{
    [self handleMouseEvent:event];
}

-(void)handleMouseEvent:(NSEvent *)event 
{ 
    NSPoint clickLocation; 
    clickLocation = [self convertPoint:[event locationInWindow] fromView:nil]; 
    wxPoint pt = wxFromNSPoint( self, clickLocation );
    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
    SetupMouseEvent( wxevent , event ) ;
    wxevent.m_x = pt.x;
    wxevent.m_y = pt.y;
    m_impl->GetWXPeer()->HandleWindowEvent(wxevent);
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation
{
    m_impl = theImplementation;
}

- (wxWidgetImpl*) implementation
{
    return m_impl;
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


@end // wxNSView

IMPLEMENT_DYNAMIC_CLASS( wxWidgetCocoaImpl , wxWidgetImpl )

wxWidgetCocoaImpl::wxWidgetCocoaImpl( wxWindowMac* peer , WXWidget w, bool isRootControl ) :
    wxWidgetImpl( peer, isRootControl ), m_osxView(w)
{
}

wxWidgetCocoaImpl::wxWidgetCocoaImpl() 
{
}

void wxWidgetCocoaImpl::Init()
{
    m_osxView = NULL;
}

wxWidgetCocoaImpl::~wxWidgetCocoaImpl()
{
    [m_osxView setImplementation:NULL];
    [m_osxView release];
}
    
bool wxWidgetCocoaImpl::IsVisible() const 
{
    return [m_osxView isHiddenOrHasHiddenAncestor] == NO;
}

void wxWidgetCocoaImpl::Raise()
{
}
    
void wxWidgetCocoaImpl::Lower()
{
}

void wxWidgetCocoaImpl::ScrollRect( const wxRect *rect, int dx, int dy )
{
}

void wxWidgetCocoaImpl::Move(int x, int y, int width, int height)
{
    NSRect r = wxToNSRect( [m_osxView superview], wxRect(x,y,width, height) );
    [m_osxView setFrame:r];
}

void wxWidgetCocoaImpl::GetPosition( int &x, int &y ) const
{
    wxRect r = wxFromNSRect( [m_osxView superview], [m_osxView frame] );
    x = r.GetLeft();
    y = r.GetTop();
}

void wxWidgetCocoaImpl::GetSize( int &width, int &height ) const
{
    NSRect rect = [m_osxView frame];
    width = rect.size.width;
    height = rect.size.height;
}

void wxWidgetCocoaImpl::GetContentArea( int&left, int &top, int &width, int &height )
{
    left = top = 0;
    GetSize( width, height );
}

void wxWidgetCocoaImpl::SetNeedsDisplay( const wxRect* where )
{
    if ( where )
        [m_osxView setNeedsDisplayInRect:wxToNSRect(m_osxView, *where )];
    else
        [m_osxView setNeedsDisplay:YES];
}

bool wxWidgetCocoaImpl::GetNeedsDisplay() const
{
    return [m_osxView needsDisplay];
}

void wxWidgetCocoaImpl::CanFocus() const
{
    return [m_osxView acceptsFirstResponder] == YES;
}

bool wxWidgetCocoaImpl::HasFocus() const
{
    return [m_osxView isFirstResponder] == YES;
}

bool wxWidgetCocoaImpl::SetFocus() 
{
    [m_osxView makeKeyWindow] ;
}


void wxWidgetCocoaImpl::RemoveFromParent()
{
    [m_osxView removeFromSuperview];
}

void wxWidgetCocoaImpl::Embed( wxWidgetImpl *parent )
{
    NSView* container = parent->GetWXWidget() ;
    wxASSERT_MSG( container != NULL , wxT("No valid mac container control") ) ;
    [container addSubview:m_osxView];
}


//
// Factory methods
//

wxWidgetImpl* wxWidgetImpl::CreateUserPane( wxWindowMac* wxpeer, const wxPoint& pos, const wxSize& size,
                            long style, long extraStyle, const wxString& name)
{
    NSView* sv = (wxpeer->GetParent()->GetHandle() );
    
    NSRect r = wxToNSRect( sv, wxRect( pos, size) );
    // Rect bounds = wxMacGetBoundsForControl( wxpeer, pos , size ) ;
    wxNSView* v = [[wxNSView alloc] initWithFrame:r];
    [sv addSubview:v];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

wxWidgetImpl* wxWidgetImpl::CreateContentView( wxNonOwnedWindow* now ) 
{
    NSWindow* tlw = now->GetWXWindow();
    wxNSView* v = [[wxNSView alloc] initWithFrame:[[tlw contentView] frame]];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( now, v, true );
    [v setImplementation:c];
    [tlw setContentView:v];
    return c;
}


#endif
