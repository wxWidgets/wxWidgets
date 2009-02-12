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

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/nonownedwnd.h"
    #include "wx/log.h"
#endif

#ifdef __WXMAC__
    #include "wx/osx/private.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#include <objc/objc-runtime.h>

NSRect wxOSXGetFrameForControl( wxWindowMac* window , const wxPoint& pos , const wxSize &size , bool adjustForOrigin )
{
    int x, y, w, h ;

    window->MacGetBoundsForControl( pos , size , x , y, w, h , adjustForOrigin ) ;
    wxRect bounds(x,y,w,h);
    NSView* sv = (window->GetParent()->GetHandle() );

    return wxToNSRect( sv, bounds );
}

@interface wxNSView : NSView
{
}

- (BOOL) canBecomeKeyView;

@end // wxNSView

@interface NSView(PossibleMethods) 
- (void)setTitle:(NSString *)aString;
- (void)setStringValue:(NSString *)aString;
- (void)setIntValue:(int)anInt;
- (void)setFloatValue:(float)aFloat;
- (void)setDoubleValue:(double)aDouble;

- (double)minValue;
- (double)maxValue;
- (void)setMinValue:(double)aDouble;
- (void)setMaxValue:(double)aDouble;

- (void)sizeToFit;

- (BOOL)isEnabled;
- (void)setEnabled:(BOOL)flag;

- (void)setImage:(NSImage *)image;
- (void)setControlSize:(NSControlSize)size;

- (id)contentView;

- (void)setTarget:(id)anObject;
- (void)setAction:(SEL)aSelector;
- (void)setDoubleAction:(SEL)aSelector;
@end 

long wxOSXTranslateCocoaKey( int unichar )
{
    long retval = unichar;
    switch( unichar )
    {
        case NSUpArrowFunctionKey :
            retval = WXK_UP;
            break;
        case NSDownArrowFunctionKey :
            retval = WXK_DOWN;
            break;
        case NSLeftArrowFunctionKey :
            retval = WXK_LEFT;
            break;
        case NSRightArrowFunctionKey :
            retval = WXK_RIGHT;
            break;
        case NSInsertFunctionKey  :
            retval = WXK_INSERT;
            break;
        case NSDeleteFunctionKey  :
            retval = WXK_DELETE;
            break;
        case NSHomeFunctionKey  :
            retval = WXK_HOME;
            break;
//        case NSBeginFunctionKey  :
//            retval = WXK_BEGIN;
//            break;
        case NSEndFunctionKey  :
            retval = WXK_END;
            break;
        case NSPageUpFunctionKey  :
            retval = WXK_PAGEUP;
            break;
       case NSPageDownFunctionKey  :
            retval = WXK_PAGEDOWN;
            break;
       case NSHelpFunctionKey  :
            retval = WXK_HELP;
            break;

        default :
            if ( unichar >= NSF1FunctionKey && unichar >= NSF24FunctionKey )
                retval = WXK_F1 + (unichar - NSF1FunctionKey );
            break;
    }
    return retval;
}

void SetupKeyEvent( wxKeyEvent &wxevent , NSEvent * nsEvent )
{
    UInt32 modifiers = [nsEvent modifierFlags] ;
    int eventType = [nsEvent type];

    wxevent.m_shiftDown = modifiers & NSShiftKeyMask;
    wxevent.m_controlDown = modifiers & NSControlKeyMask;
    wxevent.m_altDown = modifiers & NSAlternateKeyMask;
    wxevent.m_metaDown = modifiers & NSCommandKeyMask;
    
    wxevent.m_rawCode = [nsEvent keyCode];
    wxevent.m_rawFlags = modifiers;
    
    wxevent.SetTimestamp( [nsEvent timestamp] * 1000.0 ) ;
    switch (eventType)
    {
        case NSKeyDown :
            wxevent.SetEventType( wxEVT_KEY_DOWN )  ;
            break;
        case NSKeyUp :
            wxevent.SetEventType( wxEVT_KEY_UP )  ;
            break;
        case NSFlagsChanged :
            // setup common code here
            break;
        default :
            break ;
    }

    wxString chars;
    if ( eventType != NSFlagsChanged )
    {
        NSString* nschars = [nsEvent characters];
        if ( nschars )
        {
            wxCFStringRef cfchars((CFStringRef)[nschars retain]);
            chars = cfchars.AsString();
        }
    }
    
    int unichar = chars.Length() > 0 ? chars[0] : 0;
    long keyval = wxOSXTranslateCocoaKey(unichar) ;
    if ( keyval == unichar && ( wxevent.GetEventType() == wxEVT_KEY_UP || wxevent.GetEventType() == wxEVT_KEY_DOWN ) )
        keyval = wxToupper( keyval ) ;

#if wxUSE_UNICODE
    wxevent.m_uniChar = unichar;
#endif
    wxevent.m_keyCode = keyval;
}

UInt32 g_lastButton = 0 ;
bool g_lastButtonWasFakeRight = false ;

void SetupMouseEvent( wxMouseEvent &wxevent , NSEvent * nsEvent )
{
    int eventType = [nsEvent type];
    UInt32 modifiers = [nsEvent modifierFlags] ;
    wxPoint screenMouseLocation = wxFromNSPoint( NULL, [nsEvent locationInWindow]);

    // these parameters are not given for all events
    UInt32 button = [nsEvent buttonNumber];
    UInt32 clickCount = 0;
    if ( eventType != NSScrollWheel ) 
        [nsEvent clickCount];

    wxevent.m_x = screenMouseLocation.x;
    wxevent.m_y = screenMouseLocation.y;
    wxevent.m_shiftDown = modifiers & NSShiftKeyMask;
    wxevent.m_controlDown = modifiers & NSControlKeyMask;
    wxevent.m_altDown = modifiers & NSAlternateKeyMask;
    wxevent.m_metaDown = modifiers & NSCommandKeyMask;
    wxevent.m_clickCount = clickCount;
    wxevent.SetTimestamp( [nsEvent timestamp] * 1000.0 ) ;

    UInt32 mouseChord = 0; 

    switch (eventType)
    {
        case NSLeftMouseDown :
        case NSLeftMouseDragged :
            mouseChord = 1U;
            break;
        case NSRightMouseDown :
        case NSRightMouseDragged :
            mouseChord = 2U;
            break;
        case NSOtherMouseDown :
        case NSOtherMouseDragged :
            mouseChord = 4U;
            break;
    }

    // a control click is interpreted as a right click
    bool thisButtonIsFakeRight = false ;
    if ( button == 0 && (modifiers & NSControlKeyMask) )
    {
        button = 1 ;
        thisButtonIsFakeRight = true ;
    }

    // otherwise we report double clicks by connecting a left click with a ctrl-left click
    if ( clickCount > 1 && button != g_lastButton )
        clickCount = 1 ;

    // we must make sure that our synthetic 'right' button corresponds in
    // mouse down, moved and mouse up, and does not deliver a right down and left up
    switch (eventType)
    {
        case NSLeftMouseDown :
        case NSRightMouseDown :
        case NSOtherMouseDown :
            g_lastButton = button ;
            g_lastButtonWasFakeRight = thisButtonIsFakeRight ;
            break;
     }

    if ( button == 0 )
    {
        g_lastButton = 0 ;
        g_lastButtonWasFakeRight = false ;
    }
    else if ( g_lastButton == 1 && g_lastButtonWasFakeRight )
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

    // translate into wx types
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
            wxevent.m_wheelDelta = 10;
            wxevent.m_linesPerAction = 1;
            NSLog(@"deltaX %f, deltaY %f",[nsEvent deltaX], [nsEvent deltaY]);
            if ( abs([nsEvent deltaX]) > abs([nsEvent deltaY]) )
            {
                wxevent.m_wheelAxis = 1;
                wxevent.m_wheelRotation = [nsEvent deltaX] * 10.0;
            }
            else
            {
                wxevent.m_wheelRotation = [nsEvent deltaY] * 10.0;
            }
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

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized) 
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (BOOL) canBecomeKeyView
{
    return YES;
}

@end // wxNSView

//
// event handlers
//

#if wxUSE_DRAG_AND_DROP

// see http://lists.apple.com/archives/Cocoa-dev/2005/Jul/msg01244.html
// for details on the NSPasteboard -> PasteboardRef conversion

NSDragOperation wxOSX_draggingEntered( id self, SEL _cmd, id <NSDraggingInfo>sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NSDragOperationNone;
        
    return impl->draggingEntered(sender, self, _cmd);
}

void wxOSX_draggingExited( id self, SEL _cmd, id <NSDraggingInfo> sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return ;
        
    return impl->draggingExited(sender, self, _cmd);
}

NSDragOperation wxOSX_draggingUpdated( id self, SEL _cmd, id <NSDraggingInfo>sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NSDragOperationNone;
        
    return impl->draggingUpdated(sender, self, _cmd);
}

BOOL wxOSX_performDragOperation( id self, SEL _cmd, id <NSDraggingInfo> sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NSDragOperationNone;
        
    return impl->performDragOperation(sender, self, _cmd) ? YES:NO ;
}

void wxOSX_mouseEvent(NSView* self, SEL _cmd, NSEvent *event) 
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return;
        
    impl->mouseEvent(event, self, _cmd);
}

void wxOSX_keyEvent(NSView* self, SEL _cmd, NSEvent *event) 
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return;
        
    impl->keyEvent(event, self, _cmd);
}

BOOL wxOSX_performKeyEquivalent(NSView* self, SEL _cmd, NSEvent *event) 
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NO;
        
    return impl->performKeyEquivalent(event, self, _cmd);
}

BOOL wxOSX_becomeFirstResponder(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NO;
        
    return impl->becomeFirstResponder(self, _cmd);
}

BOOL wxOSX_resignFirstResponder(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NO;
        
    return impl->resignFirstResponder(self, _cmd);
}

void wxOSX_resetCursorRects(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return;
        
    impl->resetCursorRects(self, _cmd);
}

BOOL wxOSX_isFlipped(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return NO;
        
    return impl->isFlipped(self, _cmd) ? YES:NO;
}

void wxOSX_drawRect(NSView* self, SEL _cmd, NSRect rect)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return;
        
    return impl->drawRect(&rect, self, _cmd);
}

void wxOSX_controlAction(NSView* self, SEL _cmd, id sender)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return;
        
    impl->controlAction(self, _cmd, sender);
}

void wxOSX_controlDoubleAction(NSView* self, SEL _cmd, id sender)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == NULL)
        return;
        
    impl->controlDoubleAction(self, _cmd, sender);
}

unsigned int wxWidgetCocoaImpl::draggingEntered(void* s, WXWidget slf, void *_cmd)
{
    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;
    NSPasteboard *pboard = [sender draggingPasteboard];
    NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
    
    wxWindow* wxpeer = GetWXPeer();
    if ( wxpeer == NULL )
        return NSDragOperationNone;
        
    wxDropTarget* target = wxpeer->GetDropTarget();
    if ( target == NULL )
        return NSDragOperationNone;

    wxDragResult result = wxDragNone;
    wxPoint pt = wxFromNSPoint( m_osxView, [sender draggingLocation] );

    if ( sourceDragMask & NSDragOperationLink )
        result = wxDragLink;
    else if ( sourceDragMask & NSDragOperationCopy )
        result = wxDragCopy;
    else if ( sourceDragMask & NSDragOperationMove )
        result = wxDragMove;

    PasteboardRef pboardRef;    
    PasteboardCreate((CFStringRef)[pboard name], &pboardRef);
    target->SetCurrentDragPasteboard(pboardRef);
    result = target->OnEnter(pt.x, pt.y, result);
    CFRelease(pboardRef);
     
    NSDragOperation nsresult = NSDragOperationNone;
    switch (result )
    {
        case wxDragLink:
            nsresult = NSDragOperationLink;
        case wxDragMove:
            nsresult = NSDragOperationMove;
        case wxDragCopy:
            nsresult = NSDragOperationCopy;
        default :
            break;
    }
    return nsresult;
}

void wxWidgetCocoaImpl::draggingExited(void* s, WXWidget slf, void *_cmd)
{
    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;
    NSPasteboard *pboard = [sender draggingPasteboard];
    
    wxWindow* wxpeer = GetWXPeer();
    if ( wxpeer == NULL )
        return;
        
    wxDropTarget* target = wxpeer->GetDropTarget();
    if ( target == NULL )
        return;
        
    PasteboardRef pboardRef;    
    PasteboardCreate((CFStringRef)[pboard name], &pboardRef);
    target->SetCurrentDragPasteboard(pboardRef);
    target->OnLeave();
    CFRelease(pboardRef);
 }

unsigned int wxWidgetCocoaImpl::draggingUpdated(void* s, WXWidget slf, void *_cmd)
{
    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;
    NSPasteboard *pboard = [sender draggingPasteboard];
    NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
        
    wxWindow* wxpeer = GetWXPeer();
    if ( wxpeer == NULL )
        return NSDragOperationNone;
        
    wxDropTarget* target = wxpeer->GetDropTarget();
    if ( target == NULL )
        return NSDragOperationNone;

    wxDragResult result = wxDragNone;
    wxPoint pt = wxFromNSPoint( m_osxView, [sender draggingLocation] );

    if ( sourceDragMask & NSDragOperationLink )
        result = wxDragLink;
    else if ( sourceDragMask & NSDragOperationCopy )
        result = wxDragCopy;
    else if ( sourceDragMask & NSDragOperationMove )
        result = wxDragMove;

    PasteboardRef pboardRef;    
    PasteboardCreate((CFStringRef)[pboard name], &pboardRef);
    target->SetCurrentDragPasteboard(pboardRef);
    result = target->OnDragOver(pt.x, pt.y, result);
    CFRelease(pboardRef);
     
    NSDragOperation nsresult = NSDragOperationNone;
    switch (result )
    {
        case wxDragLink:
            nsresult = NSDragOperationLink;
        case wxDragMove:
            nsresult = NSDragOperationMove;
        case wxDragCopy:
            nsresult = NSDragOperationCopy;
        default :
            break;
    }
    return nsresult;
}

bool wxWidgetCocoaImpl::performDragOperation(void* s, WXWidget slf, void *_cmd)
{
    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;

    NSPasteboard *pboard = [sender draggingPasteboard];
    NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
    
    wxWindow* wxpeer = GetWXPeer();
    wxDropTarget* target = wxpeer->GetDropTarget();
    wxDragResult result = wxDragNone;
    wxPoint pt = wxFromNSPoint( m_osxView, [sender draggingLocation] );

    if ( sourceDragMask & NSDragOperationLink )
        result = wxDragLink;
    else if ( sourceDragMask & NSDragOperationCopy )
        result = wxDragCopy;
    else if ( sourceDragMask & NSDragOperationMove )
        result = wxDragMove;

    PasteboardRef pboardRef;    
    PasteboardCreate((CFStringRef)[pboard name], &pboardRef);
    target->SetCurrentDragPasteboard(pboardRef);
    result = target->OnData(pt.x, pt.y, result);
    CFRelease(pboardRef);
     
    return result != wxDragNone;
}

#endif

typedef void (*wxOSX_EventHandlerPtr)(NSView* self, SEL _cmd, NSEvent *event);
typedef BOOL (*wxOSX_PerformKeyEventHandlerPtr)(NSView* self, SEL _cmd, NSEvent *event);
typedef BOOL (*wxOSX_FocusHandlerPtr)(NSView* self, SEL _cmd);
typedef BOOL (*wxOSX_ResetCursorRectsHandlerPtr)(NSView* self, SEL _cmd);
typedef BOOL (*wxOSX_DrawRectHandlerPtr)(NSView* self, SEL _cmd, NSRect rect);

void wxWidgetCocoaImpl::mouseEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    if ( !DoHandleMouseEvent(event) )
    {
        wxOSX_EventHandlerPtr superimpl = (wxOSX_EventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        superimpl(slf, (SEL)_cmd, event);
    }
}

void wxWidgetCocoaImpl::keyEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    if ( !DoHandleKeyEvent(event) )
    {
        wxOSX_EventHandlerPtr superimpl = (wxOSX_EventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        superimpl(slf, (SEL)_cmd, event);
    }
}

bool wxWidgetCocoaImpl::performKeyEquivalent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    if ( !DoHandleKeyEvent(event) )
    {
        wxOSX_PerformKeyEventHandlerPtr superimpl = (wxOSX_PerformKeyEventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        return superimpl(slf, (SEL)_cmd, event);
    }

    return YES;
}

bool wxWidgetCocoaImpl::becomeFirstResponder(WXWidget slf, void *_cmd)
{
    wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    BOOL r = superimpl(slf, (SEL)_cmd);
    if ( r )
        DoNotifyFocusEvent( true );
    return r;
}

bool wxWidgetCocoaImpl::resignFirstResponder(WXWidget slf, void *_cmd)
{
    wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    BOOL r = superimpl(slf, (SEL)_cmd);
    if ( r )
        DoNotifyFocusEvent( false );
    return r;
}

void wxWidgetCocoaImpl::resetCursorRects(WXWidget slf, void *_cmd)
{
    wxWindow* wxpeer = GetWXPeer();
    if ( wxpeer )
    {
        NSCursor *cursor = (NSCursor*)wxpeer->GetCursor().GetHCURSOR();
        if (cursor == NULL)
        {
            wxOSX_ResetCursorRectsHandlerPtr superimpl = (wxOSX_ResetCursorRectsHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
            superimpl(slf, (SEL)_cmd);
        }
        else
            [slf addCursorRect: [slf bounds]
                cursor: cursor];
    }
}
  
bool wxWidgetCocoaImpl::isFlipped(WXWidget slf, void *_cmd)
{
    return m_isFlipped;
}


#define OSX_DEBUG_DRAWING 0

void wxWidgetCocoaImpl::drawRect(void* rect, WXWidget slf, void *_cmd)
{
    CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState( context );
    
#if OSX_DEBUG_DRAWING
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
#endif

    if ( !m_isFlipped )
    {
        CGContextTranslateCTM( context, 0,  [m_osxView bounds].size.height );
        CGContextScaleCTM( context, 1, -1 );
    }
    
    wxRegion updateRgn;
    const NSRect *rects;
    NSInteger count;

    [slf getRectsBeingDrawn:&rects count:&count];
    for ( int i = 0 ; i < count ; ++i )
    {
        updateRgn.Union(wxFromNSRect(slf, rects[i]) );
    }

    wxWindow* wxpeer = GetWXPeer();
    wxpeer->GetUpdateRegion() = updateRgn;
    wxpeer->MacSetCGContextRef( context );
    
    bool handled = wxpeer->MacDoRedraw( 0 );
            
    CGContextRestoreGState( context );

    CGContextSaveGState( context );
    if ( !handled )
    {
        // call super
        SEL _cmd = @selector(drawRect:);
        wxOSX_DrawRectHandlerPtr superimpl = (wxOSX_DrawRectHandlerPtr) [[slf superclass] instanceMethodForSelector:_cmd];
        superimpl(slf, _cmd, *(NSRect*)rect);
        CGContextRestoreGState( context );
        CGContextSaveGState( context );
    }
    wxpeer->MacPaintChildrenBorders();
    wxpeer->MacSetCGContextRef( NULL );
    CGContextRestoreGState( context );
}

void wxWidgetCocoaImpl::controlAction( WXWidget slf, void *_cmd, void *sender)
{
    wxWindow* wxpeer = (wxWindow*) GetWXPeer();
    if ( wxpeer )
        wxpeer->OSXHandleClicked(0);
}

void wxWidgetCocoaImpl::controlDoubleAction( WXWidget slf, void *_cmd, void *sender)
{
}

// 

#if OBJC_API_VERSION >= 2

#define wxOSX_CLASS_ADD_METHOD( c, s, i, t ) \
    class_addMethod(c, s, i, t );

#else

#define wxOSX_CLASS_ADD_METHOD( c, s, i, t ) \
    { s, t, i },

#endif

void wxOSXCocoaClassAddWXMethods(Class c)
{

#if OBJC_API_VERSION < 2
    static objc_method wxmethods[] =
    {
#endif

    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseDown:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(rightMouseDown:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(otherMouseDown:), (IMP) wxOSX_mouseEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseUp:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(rightMouseUp:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(otherMouseUp:), (IMP) wxOSX_mouseEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseMoved:), (IMP) wxOSX_mouseEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseDragged:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(rightMouseDragged:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(otherMouseDragged:), (IMP) wxOSX_mouseEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(scrollWheel:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseEntered:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseExited:), (IMP) wxOSX_mouseEvent, "v@:@" )
    
    wxOSX_CLASS_ADD_METHOD(c, @selector(keyDown:), (IMP) wxOSX_keyEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(keyUp:), (IMP) wxOSX_keyEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(flagsChanged:), (IMP) wxOSX_keyEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(performKeyEquivalent:), (IMP) wxOSX_performKeyEquivalent, "v@:@" )


    wxOSX_CLASS_ADD_METHOD(c, @selector(becomeFirstResponder), (IMP) wxOSX_becomeFirstResponder, "c@:" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(resignFirstResponder), (IMP) wxOSX_resignFirstResponder, "c@:" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(resetCursorRects), (IMP) wxOSX_resetCursorRects, "v@:" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(isFlipped), (IMP) wxOSX_isFlipped, "c@:" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(drawRect:), (IMP) wxOSX_drawRect, "v@:{_NSRect={_NSPoint=ff}{_NSSize=ff}}" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(controlAction:), (IMP) wxOSX_controlAction, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(controlDoubleAction:), (IMP) wxOSX_controlDoubleAction, "v@:@" )

#if wxUSE_DRAG_AND_DROP
    wxOSX_CLASS_ADD_METHOD(c, @selector(draggingEntered:), (IMP) wxOSX_draggingEntered, "I@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(draggingUpdated:), (IMP) wxOSX_draggingUpdated, "I@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(draggingExited:), (IMP) wxOSX_draggingExited, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(performDragOperation:), (IMP) wxOSX_performDragOperation, "c@:@" )
#endif     
        
#if OBJC_API_VERSION < 2
    } ;
    static int method_count = WXSIZEOF( wxmethods );
    static objc_method_list *wxmethodlist = NULL;
    if ( wxmethodlist == NULL )
    {
        wxmethodlist = (objc_method_list*) malloc(sizeof(objc_method_list) + sizeof(wxmethods) );
        memcpy( &wxmethodlist->method_list[0], &wxmethods[0], sizeof(wxmethods) );
        wxmethodlist->method_count = method_count;
        wxmethodlist->obsolete = 0;
    }
    class_addMethods( c, wxmethodlist );
#endif
}

//
// C++ implementation class
//

IMPLEMENT_DYNAMIC_CLASS( wxWidgetCocoaImpl , wxWidgetImpl )

wxWidgetCocoaImpl::wxWidgetCocoaImpl( wxWindowMac* peer , WXWidget w, bool isRootControl ) :
    wxWidgetImpl( peer, isRootControl )
{
    Init();
    m_osxView = w;
}

wxWidgetCocoaImpl::wxWidgetCocoaImpl() 
{
    Init();
}

void wxWidgetCocoaImpl::Init()
{
    m_osxView = NULL;
    m_isFlipped = true;
}

wxWidgetCocoaImpl::~wxWidgetCocoaImpl()
{
    RemoveAssociations( this );

    if ( !IsRootControl() )
    {
        NSView *sv = [m_osxView superview];
        if ( sv != nil )
            [m_osxView removeFromSuperview];
    }
    [m_osxView release];
}
    
bool wxWidgetCocoaImpl::IsVisible() const 
{
    return [m_osxView isHiddenOrHasHiddenAncestor] == NO;
}

void wxWidgetCocoaImpl::SetVisibility( bool visible )
{
    [m_osxView setHidden:(visible ? NO:YES)];
}

void wxWidgetCocoaImpl::Raise()
{
}
    
void wxWidgetCocoaImpl::Lower()
{
}

void wxWidgetCocoaImpl::ScrollRect( const wxRect *rect, int dx, int dy )
{
#if 1
    SetNeedsDisplay() ;
#else
    // We should do something like this, but it wasn't working in 10.4.
    if (GetNeedsDisplay() )
    {
        SetNeedsDisplay() ;
    }
    NSRect r = wxToNSRect( [m_osxView superview], *rect );
    NSSize offset = NSMakeSize((float)dx, (float)dy);
    [m_osxView scrollRect:r by:offset];
#endif
}

void wxWidgetCocoaImpl::Move(int x, int y, int width, int height)
{
    wxWindowMac* parent = GetWXPeer()->GetParent();
    // under Cocoa we might have a contentView in the wxParent to which we have to 
    // adjust the coordinates
    if (parent)
    {
        wxPoint pt(parent->GetClientAreaOrigin());
        x -= pt.x;
        y -= pt.y;
    }
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

void wxWidgetCocoaImpl::GetContentArea( int&left, int &top, int &width, int &height ) const
{
    if ( [m_osxView respondsToSelector:@selector(contentView) ] )
    {
        NSView* cv = [m_osxView contentView];
     
        NSRect bounds = [m_osxView bounds];
        NSRect rect = [cv frame];
        
        int y = rect.origin.y;
        int x = rect.origin.x;
        if ( ![ m_osxView isFlipped ] )
            y = bounds.size.height - (rect.origin.y + rect.size.height);
        left = x;
        top = y;
        width = rect.size.width;
        height = rect.size.height;
    }
    else
    {
        left = top = 0;
        GetSize( width, height );
    }
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

bool wxWidgetCocoaImpl::CanFocus() const
{
    return [m_osxView canBecomeKeyView] == YES;
}

bool wxWidgetCocoaImpl::HasFocus() const
{
    return ( [[m_osxView window] firstResponder] == m_osxView );
}

bool wxWidgetCocoaImpl::SetFocus() 
{
    if ( [m_osxView canBecomeKeyView] == NO )
        return false;
        
    [[m_osxView window] makeFirstResponder: m_osxView] ;
    return true;
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

void wxWidgetCocoaImpl::SetBackgroundColour( const wxColour &WXUNUSED(col) )
{
    // m_osxView.backgroundColor = [[UIColor alloc] initWithCGColor:col.GetCGColor()];
}

void wxWidgetCocoaImpl::SetLabel( const wxString& title, wxFontEncoding encoding )
{
    if ( [m_osxView respondsToSelector:@selector(setTitle:) ] )
    {
        wxCFStringRef cf( title , m_wxPeer->GetFont().GetEncoding() );
        [m_osxView setTitle:cf.AsNSString()];
    }
    else if ( [m_osxView respondsToSelector:@selector(setStringValue:) ] )
    {
        wxCFStringRef cf( title , m_wxPeer->GetFont().GetEncoding() );
        [m_osxView setStringValue:cf.AsNSString()];
    }
}
    

void  wxWidgetImpl::Convert( wxPoint *pt , wxWidgetImpl *from , wxWidgetImpl *to )
{
    NSPoint p = wxToNSPoint( from->GetWXWidget(), *pt );
    p = [from->GetWXWidget() convertPoint:p toView:to->GetWXWidget() ]; 
    *pt = wxFromNSPoint( to->GetWXWidget(), p );
}

wxInt32 wxWidgetCocoaImpl::GetValue() const 
{
    return [(NSControl*)m_osxView intValue];
}

void wxWidgetCocoaImpl::SetValue( wxInt32 v ) 
{
    if (  [m_osxView respondsToSelector:@selector(setIntValue:)] )
    {
        [m_osxView setIntValue:v];
    }
    else if (  [m_osxView respondsToSelector:@selector(setFloatValue:)] )
    {
        [m_osxView setFloatValue:(double)v];
    }
    else if (  [m_osxView respondsToSelector:@selector(setDoubleValue:)] )
    {
        [m_osxView setDoubleValue:(double)v];
    }
}

void wxWidgetCocoaImpl::SetMinimum( wxInt32 v ) 
{
    if (  [m_osxView respondsToSelector:@selector(setMinValue:)] )
    {
        [m_osxView setMinValue:(double)v];
    }
}

void wxWidgetCocoaImpl::SetMaximum( wxInt32 v ) 
{
    if (  [m_osxView respondsToSelector:@selector(setMaxValue:)] )
    {
        [m_osxView setMaxValue:(double)v];
    }
}

wxInt32 wxWidgetCocoaImpl::GetMinimum() const 
{
    if (  [m_osxView respondsToSelector:@selector(getMinValue:)] )
    {
        return [m_osxView minValue];
    }
    return 0;
}

wxInt32 wxWidgetCocoaImpl::GetMaximum() const 
{
    if (  [m_osxView respondsToSelector:@selector(getMaxValue:)] )
    {
        return [m_osxView maxValue];
    }
    return 0;
}

void wxWidgetCocoaImpl::SetBitmap( const wxBitmap& bitmap )
{
    if (  [m_osxView respondsToSelector:@selector(setImage:)] )
    {
        [m_osxView setImage:bitmap.GetNSImage()];
    }
}

void wxWidgetCocoaImpl::SetupTabs( const wxNotebook& notebook)
{
    // implementation in subclass
}

void wxWidgetCocoaImpl::GetBestRect( wxRect *r ) const
{
    r->x = r->y = r->width = r->height = 0;
//    if (  [m_osxView isKindOfClass:[NSControl class]] )
    if (  [m_osxView respondsToSelector:@selector(sizeToFit)] )
    {
        NSRect former = [m_osxView frame];
        [m_osxView sizeToFit];
        NSRect best = [m_osxView frame];
        [m_osxView setFrame:former];
        r->width = best.size.width;
        r->height = best.size.height;
    }
}

bool wxWidgetCocoaImpl::IsEnabled() const
{
    if ( [m_osxView respondsToSelector:@selector(isEnabled) ] )
        return [m_osxView isEnabled];
    return true;
}

void wxWidgetCocoaImpl::Enable( bool enable )
{
    if ( [m_osxView respondsToSelector:@selector(setEnabled:) ] )
        [m_osxView setEnabled:enable];
}

void wxWidgetCocoaImpl::PulseGauge()
{
}

void wxWidgetCocoaImpl::SetScrollThumb( wxInt32 val, wxInt32 view )
{
}

void wxWidgetCocoaImpl::SetControlSize( wxWindowVariant variant ) 
{
    NSControlSize size = NSRegularControlSize;
    
    switch ( variant )
    {
        case wxWINDOW_VARIANT_NORMAL :
            size = NSRegularControlSize;
            break ;

        case wxWINDOW_VARIANT_SMALL :
            size = NSSmallControlSize;
            break ;

        case wxWINDOW_VARIANT_MINI :
            size = NSMiniControlSize;
            break ;

        case wxWINDOW_VARIANT_LARGE :
            size = NSRegularControlSize;
            break ;

        default:
            wxFAIL_MSG(_T("unexpected window variant"));
            break ;
    }
    if ( [m_osxView respondsToSelector:@selector(setControlSize:)] )
        [m_osxView setControlSize:size];
}

void wxWidgetCocoaImpl::SetFont(wxFont const&, wxColour const&, long, bool)
{
    // TODO
}

void wxWidgetCocoaImpl::InstallEventHandler( WXWidget control )
{
    WXWidget c =  control ? control : (WXWidget) m_osxView;
    wxWidgetImpl::Associate( c, this ) ;
    if ([c respondsToSelector:@selector(setAction:)])
    {
        [c setTarget: c];
        [c setAction: @selector(controlAction:)];
        if ([c respondsToSelector:@selector(setDoubleAction:)])
        {
            [c setDoubleAction: @selector(controlDoubleAction:)];
        }
        
    }
}

bool wxWidgetCocoaImpl::DoHandleKeyEvent(NSEvent *event)
{
    wxKeyEvent wxevent(wxEVT_KEY_DOWN);
    SetupKeyEvent( wxevent, event );

    return GetWXPeer()->OSXHandleKeyEvent(wxevent);
}

bool wxWidgetCocoaImpl::DoHandleMouseEvent(NSEvent *event)
{
    NSPoint clickLocation; 
    clickLocation = [m_osxView convertPoint:[event locationInWindow] fromView:nil]; 
    wxPoint pt = wxFromNSPoint( m_osxView, clickLocation );
    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
    SetupMouseEvent( wxevent , event ) ;
    wxevent.m_x = pt.x;
    wxevent.m_y = pt.y;

    return GetWXPeer()->HandleWindowEvent(wxevent);
}

void wxWidgetCocoaImpl::DoNotifyFocusEvent(bool receivedFocus)
{
    wxWindow* thisWindow = GetWXPeer();
    if ( thisWindow->MacGetTopLevelWindow() && NeedsFocusRect() )
    {
        thisWindow->MacInvalidateBorders();
    }

    if ( receivedFocus )
    {
        wxLogTrace(_T("Focus"), _T("focus set(%p)"), static_cast<void*>(thisWindow));
        wxChildFocusEvent eventFocus((wxWindow*)thisWindow);
        thisWindow->HandleWindowEvent(eventFocus);

#if wxUSE_CARET
        if ( thisWindow->GetCaret() )
            thisWindow->GetCaret()->OnSetFocus();
#endif

        wxFocusEvent event(wxEVT_SET_FOCUS, thisWindow->GetId());
        event.SetEventObject(thisWindow);
        // TODO how to find out the targetFocusWindow ?
        // event.SetWindow(targetFocusWindow);
        thisWindow->HandleWindowEvent(event) ;
    }
    else // !receivedFocuss
    {
#if wxUSE_CARET
        if ( thisWindow->GetCaret() )
            thisWindow->GetCaret()->OnKillFocus();
#endif

        wxLogTrace(_T("Focus"), _T("focus lost(%p)"), static_cast<void*>(thisWindow));
                    
        wxFocusEvent event( wxEVT_KILL_FOCUS, thisWindow->GetId());
        event.SetEventObject(thisWindow);
        // TODO how to find out the targetFocusWindow ?
        // event.SetWindow(targetFocusWindow);
        thisWindow->HandleWindowEvent(event) ;
    }
}

void wxWidgetCocoaImpl::SetCursor(const wxCursor& cursor)
{
    NSPoint location = [NSEvent mouseLocation];
    location = [[m_osxView window] convertScreenToBase:location];
    NSPoint locationInView = [m_osxView convertPoint:location fromView:nil];

    if( NSMouseInRect(locationInView, [m_osxView bounds], YES) )
    {
        [(NSCursor*)cursor.GetHCURSOR() set];
    }
    [[m_osxView window] invalidateCursorRectsForView:m_osxView];
}

void wxWidgetCocoaImpl::CaptureMouse()
{
    [[m_osxView window] disableCursorRects];
}

void wxWidgetCocoaImpl::ReleaseMouse()
{
    [[m_osxView window] enableCursorRects];
}

void wxWidgetCocoaImpl::SetFlipped(bool flipped)
{
    m_isFlipped = flipped;
}

//
// Factory methods
//

wxWidgetImpl* wxWidgetImpl::CreateUserPane( wxWindowMac* wxpeer, wxWindowMac* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                            long style, long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSView* v = [[wxNSView alloc] initWithFrame:r];

    // temporary hook for dnd
    [v registerForDraggedTypes:[NSArray arrayWithObjects:
        NSStringPboardType, NSFilenamesPboardType, NSTIFFPboardType, NSPICTPboardType, NSPDFPboardType, nil]];
        
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    return c;
}

wxWidgetImpl* wxWidgetImpl::CreateContentView( wxNonOwnedWindow* now ) 
{
    NSWindow* tlw = now->GetWXWindow();
    wxNSView* v = [[wxNSView alloc] initWithFrame:[[tlw contentView] frame]];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( now, v, true );
    [tlw setContentView:v];
    return c;
}
