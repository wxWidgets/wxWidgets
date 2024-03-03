/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/window.mm
// Purpose:     widgets (non tlw) for cocoa
// Author:      Stefan Csomor
// Created:     2008-06-20
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
    #include "wx/combobox.h"
    #include "wx/radiobut.h"
    #include "wx/scrolbar.h"
#endif

#ifdef __WXMAC__
    #include "wx/osx/private.h"
    #include "wx/osx/private/available.h"
    #include "wx/osx/private/datatransfer.h"
#endif

#include "wx/private/bmpbndl.h"

#include "wx/evtloop.h"

#if wxUSE_CARET
    #include "wx/caret.h"
#endif

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
    #include "wx/clipbrd.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <objc/objc-runtime.h>

#define TRACE_FOCUS "focus"
#define TRACE_KEYS  "keyevent"

// ----------------------------------------------------------------------------
// debugging helpers
// ----------------------------------------------------------------------------

// These functions are called from the code but are also useful in the debugger
// (especially wxDumpNSView(), as selectors can be printed out directly anyhow),
// so make them just static instead of putting them in an anonymous namespace
// to make it easier to call them.

static wxString wxDumpSelector(SEL cmd)
{
    return wxStringWithNSString(NSStringFromSelector(cmd));
}

static wxString wxDumpNSView(NSView* view)
{
    wxWidgetImpl* const impl = wxWidgetImpl::FindFromWXWidget(view);
    if ( !impl )
        return wxStringWithNSString([view description]);

    return wxString::Format("%s belonging to %s",
                            wxStringWithNSString([view className]),
                            wxDumpWindow(impl->GetWXPeer())
                            );
}


// Get the window with the focus

NSView* wxOSXGetViewFromResponder( NSResponder* responder )
{
    NSView* view = nil;
    if ( [responder isKindOfClass:[NSTextView class]] )
    {
        NSView* delegate = (NSView*) [(NSTextView*)responder delegate];
        if ( [delegate isKindOfClass:[NSTextField class] ] )
            view = delegate;
        else
            view =  (NSView*) responder;
    }
    else
    {
        if ( [responder isKindOfClass:[NSView class]] )
            view = (NSView*) responder;
    }
    return view;
}

NSView* GetFocusedViewInWindow( NSWindow* keyWindow )
{
    NSView* focusedView = nil;
    if ( keyWindow != nil )
        focusedView = wxOSXGetViewFromResponder([keyWindow firstResponder]);

    return focusedView;
}

WXWidget wxWidgetImpl::FindFocus()
{
    NSWindow *key = [NSApp keyWindow];
    if ( key == nil )
    {
        // Application's keyWindow property may still not be updated and be
        // nil when windowDidBecomeKey: is called and even if the
        // just-activated's window isKeyWindow already returns YES. To get
        // accurate information about where the focus is at all times, we have
        // to explicitly check all application windos as well:
        for ( NSWindow *w in [NSApp windows] )
        {
            if ( [w isKeyWindow] )
            {
                key = w;
                break;
            }
        }
    }
    return key ? GetFocusedViewInWindow(key) : nil;
}

wxWidgetImpl* wxWidgetImpl::FindBestFromWXWidget(WXWidget control)
{
    wxWidgetImpl* impl = FindFromWXWidget(control);
    
    // NSScrollViews can have their subviews like NSClipView
    // therefore check and use the NSScrollView peer in that case
    if ( impl == nullptr && [[control superview] isKindOfClass:[NSScrollView class]])
        impl = FindFromWXWidget([control superview]);
    
    return impl;
}


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

@end // wxNSView

@interface wxNSView(TextInput) <NSTextInputClient>

- (void)insertText:(id)aString replacementRange:(NSRange)replacementRange;
- (void)doCommandBySelector:(SEL)aSelector;
- (void)setMarkedText:(id)aString selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange;
- (void)unmarkText;
- (NSRange)selectedRange;
- (NSRange)markedRange;
- (BOOL)hasMarkedText;
- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange;
- (NSArray*)validAttributesForMarkedText;
- (NSRect)firstRectForCharacterRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange;
- (NSUInteger)characterIndexForPoint:(NSPoint)aPoint;

@end

@interface NSView(PossibleMethods)
- (void)setTitle:(NSString *)aString;
- (void)setStringValue:(NSString *)aString;
- (void)setIntValue:(int)anInt;
- (void)setFloatValue:(float)aFloat;
- (void)setDoubleValue:(double)aDouble;

- (double)minValue;
- (double)maxValue;
- (int)increment;
- (void)setMinValue:(double)aDouble;
- (void)setMaxValue:(double)aDouble;
- (void)setIncrement:(int)value;

- (void)sizeToFit;

- (BOOL)isEnabled;
- (void)setEnabled:(BOOL)flag;

- (BOOL)clipsToBounds;
- (void)setClipsToBounds:(BOOL)clipsToBounds;

- (void)setImage:(NSImage *)image;
- (void)setControlSize:(NSControlSize)size;

- (void)setFont:(NSFont *)fontObject;

- (id)contentView;

- (void)setTarget:(id)anObject;
- (void)setAction:(SEL)aSelector;
- (void)setDoubleAction:(SEL)aSelector;
- (void)setBackgroundColor:(NSColor*)aColor;
- (void)setOpaque:(BOOL)opaque;
- (void)setTextColor:(NSColor *)color;
- (void)setImagePosition:(NSCellImagePosition)aPosition;
@end

// The following code is a combination of the code listed here:
// http://lists.apple.com/archives/cocoa-dev/2008/Apr/msg01582.html
// (which can't be used because KLGetCurrentKeyboardLayout etc aren't 64-bit)
// and the code here:
// http://inquisitivecocoa.com/category/objective-c/
@interface NSEvent (OsGuiUtilsAdditions)
- (NSString*) charactersIgnoringModifiersIncludingShift;
@end

@implementation NSEvent (OsGuiUtilsAdditions)
- (NSString*) charactersIgnoringModifiersIncludingShift {
    // First try -charactersIgnoringModifiers and look for keys which UCKeyTranslate translates
    // differently than AppKit.
    NSString* c = [self charactersIgnoringModifiers];
    if ([c length] == 1) {
        unichar codepoint = [c characterAtIndex:0];
        if ((codepoint >= 0xF700 && codepoint <= 0xF8FF) || codepoint == 0x7F) {
            return c;
        }
    }
    // This is not a "special" key, so ask UCKeyTranslate to give us the character with no
    // modifiers attached.  Actually, that's not quite accurate; we attach the Command modifier
    // which hints the OS to use Latin characters where possible, which is generally what we want.
    NSString* result = @"";
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
    CFDataRef uchr = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
    CFRelease(currentKeyboard);
    if (uchr == nullptr) {
        // this can happen for some non-U.S. input methods (eg. Romaji or Hiragana)
        return c;
    }
    const UCKeyboardLayout *keyboardLayout = (const UCKeyboardLayout*)CFDataGetBytePtr(uchr);
    if (keyboardLayout) {
        UInt32 deadKeyState = 0;
        const UniCharCount maxStringLength = 255;
        UniCharCount actualStringLength = 0;
        UniChar unicodeString[maxStringLength];
        
        OSStatus status = UCKeyTranslate(keyboardLayout,
                                         [self keyCode],
                                         kUCKeyActionDown,
                                         cmdKey >> 8,         // force the Command key to "on"
                                         LMGetKbdType(),
                                         kUCKeyTranslateNoDeadKeysMask,
                                         &deadKeyState,
                                         maxStringLength,
                                         &actualStringLength,
                                         unicodeString);
        
        if(status == noErr)
            result = [NSString stringWithCharacters:unicodeString length:(NSInteger)actualStringLength];
    }
    return result;
}
@end

long wxOSXTranslateCocoaKey( NSEvent* event, int eventType )
{
    long retval = 0;

    if ([event type] != NSFlagsChanged)
    {
        NSString* s = [event charactersIgnoringModifiersIncludingShift];
        // backspace char reports as delete w/modifiers for some reason
        if ([s length] == 1)
        {
            if ( eventType == wxEVT_CHAR && ([event modifierFlags] & NSControlKeyMask) && ( [s characterAtIndex:0] >= 'a' && [s characterAtIndex:0] <= 'z' ) )
            {
                retval = WXK_CONTROL_A + ([s characterAtIndex:0] - 'a');
            }
            else
            {
                switch ( [s characterAtIndex:0] )
                {
                    // numpad enter key End-of-text character ETX U+0003
                    case 3:
                        retval = WXK_NUMPAD_ENTER;
                        break;
                    // backspace key
                    case 0x7F :
                    case 8 :
                        retval = WXK_BACK;
                        break;
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
                    case NSBeginFunctionKey  :
                        retval = WXK_NUMPAD_BEGIN;
                        break;
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
                    default:
                        int intchar = [s characterAtIndex: 0];
                        if ( intchar >= NSF1FunctionKey && intchar <= NSF24FunctionKey )
                            retval = WXK_F1 + (intchar - NSF1FunctionKey );
                        else if ( intchar > 0 && intchar < 32 )
                            retval = intchar;
                        break;
                }
            }
        }
    }

    // Some keys don't seem to have constants. The code mimics the approach
    // taken by WebKit. See:
    // http://trac.webkit.org/browser/trunk/WebCore/platform/mac/KeyEventMac.mm
    switch( [event keyCode] )
    {
        // command key
        case 54:
        case 55:
            retval = WXK_CONTROL;
            break;
        // caps locks key
        case 57: // Capslock
            retval = WXK_CAPITAL;
            break;
        // shift key
        case 56: // Left Shift
        case 60: // Right Shift
            retval = WXK_SHIFT;
            break;
        // alt key
        case 58: // Left Alt
        case 61: // Right Alt
            retval = WXK_ALT;
            break;
        // ctrl key
        case 59: // Left Ctrl
        case 62: // Right Ctrl
            retval = WXK_RAW_CONTROL;
            break;
        // clear key
        case 71:
            retval = WXK_CLEAR;
            break;
        // tab key
        case 48:
            retval = WXK_TAB;
            break;
        default:
            break;
    }
    
    // Check for NUMPAD keys.  For KEY_UP/DOWN events we need to use the
    // WXK_NUMPAD constants, but for the CHAR event we want to use the
    // standard ascii values
    if ( eventType != wxEVT_CHAR )
    {
        switch( [event keyCode] )
        {
            case 75: // /
                retval = WXK_NUMPAD_DIVIDE;
                break;
            case 67: // *
                retval = WXK_NUMPAD_MULTIPLY;
                break;
            case 78: // -
                retval = WXK_NUMPAD_SUBTRACT;
                break;
            case 69: // +
                retval = WXK_NUMPAD_ADD;
                break;
            case 65: // .
                retval = WXK_NUMPAD_DECIMAL;
                break;
            case 82: // 0
                retval = WXK_NUMPAD0;
                break;
            case 83: // 1
                retval = WXK_NUMPAD1;
                break;
            case 84: // 2
                retval = WXK_NUMPAD2;
                break;
            case 85: // 3
                retval = WXK_NUMPAD3;
                break;
            case 86: // 4
                retval = WXK_NUMPAD4;
                break;
            case 87: // 5
                retval = WXK_NUMPAD5;
                break;
            case 88: // 6
                retval = WXK_NUMPAD6;
                break;
            case 89: // 7
                retval = WXK_NUMPAD7;
                break;
            case 91: // 8
                retval = WXK_NUMPAD8;
                break;
            case 92: // 9
                retval = WXK_NUMPAD9;
                break;
            default:
                //retval = [event keyCode];
                break;
        }
    }
    return retval;
}

void wxWidgetCocoaImpl::SetupKeyEvent(wxKeyEvent &wxevent , NSEvent * nsEvent, NSString* charString)
{
    UInt32 modifiers = [nsEvent modifierFlags] ;
    int eventType = [nsEvent type];

    wxevent.m_shiftDown = modifiers & NSShiftKeyMask;
    wxevent.m_rawControlDown = modifiers & NSControlKeyMask;
    wxevent.m_altDown = modifiers & NSAlternateKeyMask;
    wxevent.m_controlDown = modifiers & NSCommandKeyMask;

    wxevent.m_rawCode = [nsEvent keyCode];
    wxevent.m_rawFlags = modifiers;

    wxevent.SetTimestamp( (int)([nsEvent timestamp] * 1000) ) ;
    wxevent.m_isRepeat = (eventType == NSKeyDown) && [nsEvent isARepeat];

    wxString chars;
    if ( eventType != NSFlagsChanged )
    {
        NSString* nschars = [[nsEvent charactersIgnoringModifiersIncludingShift] uppercaseString];
        if ( charString )
        {
            // if charString is set, it did not come from key up / key down
            wxevent.SetEventType( wxEVT_CHAR );
            chars = wxCFStringRef::AsString(charString);
        }
        else if ( nschars )
        {
            chars = wxCFStringRef::AsString(nschars);
        }
    }

    int aunichar = chars.Length() > 0 ? chars[0] : 0;
    long keyval = 0;

    if (wxevent.GetEventType() != wxEVT_CHAR)
    {
        keyval = wxOSXTranslateCocoaKey(nsEvent, wxevent.GetEventType()) ;
        switch (eventType)
        {
            case NSKeyDown :
                wxevent.SetEventType( wxEVT_KEY_DOWN )  ;
                break;
            case NSKeyUp :
                wxevent.SetEventType( wxEVT_KEY_UP )  ;
                break;
            case NSFlagsChanged :
                switch (keyval)
                {
                    case WXK_CONTROL:
                        wxevent.SetEventType( wxevent.m_controlDown ? wxEVT_KEY_DOWN : wxEVT_KEY_UP);
                        break;
                    case WXK_SHIFT:
                        wxevent.SetEventType( wxevent.m_shiftDown ? wxEVT_KEY_DOWN : wxEVT_KEY_UP);
                        break;
                    case WXK_ALT:
                        wxevent.SetEventType( wxevent.m_altDown ? wxEVT_KEY_DOWN : wxEVT_KEY_UP);
                        break;
                    case WXK_RAW_CONTROL:
                        wxevent.SetEventType( wxevent.m_rawControlDown ? wxEVT_KEY_DOWN : wxEVT_KEY_UP);
                        break;
                }
                break;
            default :
                break ;
        }
    }
    else
    {
        long keycode = wxOSXTranslateCocoaKey( nsEvent, wxEVT_CHAR );
        if ( (keycode > 0 && keycode < WXK_SPACE) || keycode == WXK_DELETE || keycode >= WXK_START )
        {
            keyval = keycode;
        }
    }

    if ( !keyval && aunichar < 256 ) // only for ASCII characters
    {
        if ( wxevent.GetEventType() == wxEVT_KEY_UP || wxevent.GetEventType() == wxEVT_KEY_DOWN )
            keyval = wxToupper( aunichar ) ;
        else
            keyval = aunichar;
    }

    // OS X generates events with key codes in Unicode private use area for
    // unprintable symbols such as cursor arrows (WXK_UP is mapped to U+F700)
    // and function keys (WXK_F2 is U+F705). We don't want to use them as the
    // result of wxKeyEvent::GetUnicodeKey() however as it's supposed to return
    // WXK_NONE for "non characters" so explicitly exclude them.
    //
    // We only exclude the private use area inside the Basic Multilingual Plane
    // as key codes beyond it don't seem to be currently used.
    if ( !(aunichar >= 0xe000 && aunichar < 0xf900) )
        wxevent.m_uniChar = aunichar;

    wxevent.m_keyCode = keyval;

    wxWindowMac* peer = GetWXPeer();
    if ( peer )
    {
        wxevent.SetEventObject(peer);
        wxevent.SetId(peer->GetId()) ;
    }
}

UInt32 g_lastButton = 0 ;
bool g_lastButtonWasFakeRight = false ;

// better scroll wheel support 
// see http://lists.apple.com/archives/cocoa-dev/2007/Feb/msg00050.html

@interface NSEvent (DeviceDelta)
- (CGFloat)deviceDeltaX;
- (CGFloat)deviceDeltaY;

// 10.7+
- (BOOL)hasPreciseScrollingDeltas;
- (CGFloat)scrollingDeltaX;
- (CGFloat)scrollingDeltaY;
@end

static void
wxSetupCoordinates(NSView* view, wxCoord &x, wxCoord &y, NSEvent* nsEvent)
{
    NSRect locationInWindow = NSZeroRect;
    locationInWindow.origin = [nsEvent locationInWindow];
    
    // adjust coordinates for the window of the target view
    if ( [nsEvent window] != [view window] )
    {
        if ( [nsEvent window] != nil )
            locationInWindow = [[nsEvent window] convertRectToScreen:locationInWindow];
        
        if ( [view window] != nil )
            locationInWindow = [[view window] convertRectFromScreen:locationInWindow];
    }
    
    NSPoint locationInView = [view convertPoint:locationInWindow.origin fromView:nil];
    wxPoint locationInViewWX = wxFromNSPoint( view, locationInView );
        
    x = locationInViewWX.x;
    y = locationInViewWX.y;

}

void wxWidgetCocoaImpl::SetupCoordinates(wxCoord &x, wxCoord &y, NSEvent* nsEvent)
{
    wxSetupCoordinates(m_osxView, x, y, nsEvent);
}

void wxWidgetCocoaImpl::SetupMouseEvent( wxMouseEvent &wxevent , NSEvent * nsEvent )
{
    int eventType = [nsEvent type];
    UInt32 modifiers = [nsEvent modifierFlags] ;
    
    SetupCoordinates(wxevent.m_x, wxevent.m_y, nsEvent);

    // these parameters are not given for all events
    UInt32 button = [nsEvent buttonNumber];
    UInt32 clickCount = 0;

    wxevent.m_shiftDown = modifiers & NSShiftKeyMask;
    wxevent.m_rawControlDown = modifiers & NSControlKeyMask;
    wxevent.m_altDown = modifiers & NSAlternateKeyMask;
    wxevent.m_controlDown = modifiers & NSCommandKeyMask;
    wxevent.SetTimestamp( (int)([nsEvent timestamp] * 1000) ) ;

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

    // Remember value of g_lastButton for later click count adjustment
    UInt32 prevLastButton = g_lastButton;

    // a control click is interpreted as a right click
    bool thisButtonIsFakeRight = false ;
    if ( button == 0 && (modifiers & NSControlKeyMask) )
    {
        button = 1 ;
        thisButtonIsFakeRight = true ;
    }

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

    // Adjust click count when clicking with different buttons,
    // otherwise we report double clicks by connecting a left click with a ctrl-left click
    switch (eventType)
    {
        case NSLeftMouseDown :
        case NSRightMouseDown :
        case NSOtherMouseDown :
        case NSLeftMouseUp :
        case NSRightMouseUp :
        case NSOtherMouseUp :
            clickCount = [nsEvent clickCount];
            if ( clickCount > 1 && button != prevLastButton )
                clickCount = 1 ;
        break;
    }

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
    {
        switch ( button )
        {
            default:
                wxevent.m_middleDown = true ;
                break;
            case 3:
                wxevent.m_aux1Down = true ;
                break;
            case 4:
                wxevent.m_aux2Down = true ;
                break;
        }
    }

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

                case 3 :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_AUX1_DCLICK : wxEVT_AUX1_DOWN ) ;
                    break ;

                case 4 :
                    wxevent.SetEventType( clickCount > 1 ? wxEVT_AUX2_DCLICK : wxEVT_AUX2_DOWN ) ;
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

                case 3 :
                    wxevent.SetEventType( wxEVT_AUX1_UP ) ;
                    break ;

                case 4 :
                    wxevent.SetEventType( wxEVT_AUX2_UP ) ;
                    break ;

                default:
                    break ;
            }
            break ;

     case NSScrollWheel :
        {
            float deltaX = 0.0;
            float deltaY = 0.0;

            wxevent.SetEventType( wxEVT_MOUSEWHEEL ) ;

            if ( [nsEvent hasPreciseScrollingDeltas] )
            {
                deltaX = [nsEvent scrollingDeltaX];
                deltaY = [nsEvent scrollingDeltaY];
            }
            else
            {
                deltaX = [nsEvent scrollingDeltaX] * 10;
                deltaY = [nsEvent scrollingDeltaY] * 10;
            }
            
            wxevent.m_wheelDelta = 10;
            wxevent.m_wheelInverted = [nsEvent isDirectionInvertedFromDevice];
            wxevent.m_linesPerAction = 1;
            wxevent.m_columnsPerAction = 1;

            if ( fabs(deltaX) > fabs(deltaY) )
            {
                // wx conventions for horizontal are inverted from vertical (originating from native msw behavior)
                // right and up are positive values, left and down are negative values, while on OSX right and down
                // are negative and left and up are positive.
                wxevent.m_wheelAxis = wxMOUSE_WHEEL_HORIZONTAL;
                wxevent.m_wheelRotation = -(int)deltaX;
            }
            else
            {
                wxevent.m_wheelRotation = (int)deltaY;
            }

        }
        break ;

        case NSMouseEntered :
            wxevent.SetEventType( wxEVT_ENTER_WINDOW ) ;
            break;
        case NSMouseExited :
            wxevent.SetEventType( wxEVT_LEAVE_WINDOW ) ;
            break;
        case NSLeftMouseDragged :
        case NSRightMouseDragged :
        case NSOtherMouseDragged :
        case NSMouseMoved :
            wxevent.SetEventType( wxEVT_MOTION ) ;
            break;
        
        case NSEventTypeMagnify:
            wxevent.SetEventType( wxEVT_MAGNIFY );
            wxevent.m_magnification = [nsEvent magnification];
            break;
            
        default :
            break ;
    }

    wxevent.m_clickCount = clickCount;
    wxWindowMac* peer = GetWXPeer();
    if ( peer )
    {
        wxevent.SetEventObject(peer);
        wxevent.SetId(peer->GetId()) ;
    }
}

static void SetDrawingEnabledIfFrozenRecursive(wxWidgetCocoaImpl *impl, bool enable)
{
    if (!impl->GetWXPeer())
        return;

    if (impl->GetWXPeer()->IsFrozen())
        impl->SetDrawingEnabled(enable);

    for ( wxWindowList::iterator i = impl->GetWXPeer()->GetChildren().begin();
          i != impl->GetWXPeer()->GetChildren().end();
          ++i )
    {
        wxWindow *child = *i;
        if ( child->IsTopLevel() || !child->IsFrozen() )
            continue;

        // Skip any user panes as they'll handle this themselves
        if ( !child->GetPeer() || child->GetPeer()->IsUserPane() )
            continue;

        SetDrawingEnabledIfFrozenRecursive((wxWidgetCocoaImpl *)child->GetPeer(), enable);
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

/* idea taken from webkit sources: overwrite the methods that (private) NSToolTipManager will use to attach its tracking rectangle 
 * then when changing the tooltip send fake view-exit and view-enter methods which will lead to a tooltip refresh
 */


#if wxOSX_USE_NATIVE_FLIPPED
- (BOOL)isFlipped
{
    return YES;
}
#endif

- (BOOL) canBecomeKeyView
{
    wxWidgetCocoaImpl* viewimpl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( viewimpl && viewimpl->HasUserKeyHandling() && viewimpl->GetWXPeer() )
        return viewimpl->GetWXPeer()->AcceptsFocus();
    return NO;
}

- (NSView *)hitTest:(NSPoint)aPoint
{
    wxWidgetCocoaImpl* viewimpl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( viewimpl && viewimpl->GetWXPeer() && !viewimpl->GetWXPeer()->IsEnabled() )
        return nil;

    return [super hitTest:aPoint];
}

- (void) viewWillMoveToWindow:(NSWindow *)newWindow
{
    wxWidgetCocoaImpl* viewimpl = (wxWidgetCocoaImpl*) wxWidgetImpl::FindFromWXWidget( self );
    if (viewimpl)
        SetDrawingEnabledIfFrozenRecursive(viewimpl, true);

    [super viewWillMoveToWindow:newWindow];
}

- (void) viewDidMoveToWindow
{
    wxWidgetCocoaImpl* viewimpl = (wxWidgetCocoaImpl*) wxWidgetImpl::FindFromWXWidget( self );
    if (viewimpl)
        SetDrawingEnabledIfFrozenRecursive(viewimpl, false);

    [super viewDidMoveToWindow];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
- (void) viewWillDraw
{
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
    {
        CALayer* layer = self.layer;
        layer.contentsFormat = kCAContentsFormatRGBA8Uint;
    }
    
    [super viewWillDraw];
}
#endif

@end // wxNSView

// We need to adopt NSTextInputClient protocol in order to interpretKeyEvents: to work.
// Currently, only insertText:(replacementRange:) is
// implemented here, and the rest of the methods are stubs.
// It is hoped that someday IME-related functionality is implemented in
// wxWidgets and the methods of this protocol are fully working.

@implementation wxNSView(TextInput)

void wxOSX_insertText(NSView* self, SEL _cmd, NSString* text);

- (void)insertText:(id)aString replacementRange:(NSRange)replacementRange
{
    wxUnusedVar(replacementRange);
    wxOSX_insertText(self, @selector(insertText:), aString);
}

- (void)doCommandBySelector:(SEL)aSelector
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl)
        impl->doCommandBySelector(aSelector, self, _cmd);
}

- (void)setMarkedText:(id)aString selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange
{
    wxUnusedVar(aString);
    wxUnusedVar(selectedRange);
    wxUnusedVar(replacementRange);
}

- (void)unmarkText
{
}

- (NSRange)selectedRange
{    
    return NSMakeRange(NSNotFound, 0);
}

- (NSRange)markedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (BOOL)hasMarkedText
{
    return NO;
}

- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange
{
    wxUnusedVar(aRange);
    wxUnusedVar(actualRange);
    return nil;
}

- (NSArray*)validAttributesForMarkedText
{
    return nil;
}

- (NSRect)firstRectForCharacterRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange
{
    wxUnusedVar(aRange);
    wxUnusedVar(actualRange);
    return NSMakeRect(0, 0, 0, 0);
}
- (NSUInteger)characterIndexForPoint:(NSPoint)aPoint
{
    wxUnusedVar(aPoint);
    return NSNotFound;
}

@end // wxNSView(TextInput)


//
// event handlers
//

#if wxUSE_DRAG_AND_DROP

NSDragOperation wxOSX_draggingEntered( id self, SEL _cmd, id <NSDraggingInfo>sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NSDragOperationNone;

    return impl->draggingEntered(sender, self, _cmd);
}

void wxOSX_draggingExited( id self, SEL _cmd, id <NSDraggingInfo> sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return ;

    return impl->draggingExited(sender, self, _cmd);
}

NSDragOperation wxOSX_draggingUpdated( id self, SEL _cmd, id <NSDraggingInfo>sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NSDragOperationNone;

    return impl->draggingUpdated(sender, self, _cmd);
}

BOOL wxOSX_performDragOperation( id self, SEL _cmd, id <NSDraggingInfo> sender )
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NSDragOperationNone;

    return impl->performDragOperation(sender, self, _cmd) ? YES:NO ;
}

#endif

void wxOSX_mouseEvent(NSView* self, SEL _cmd, NSEvent *event)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

    // We shouldn't let disabled windows get mouse events.
    if (impl->GetWXPeer()->IsEnabled())
        impl->mouseEvent(event, self, _cmd);
}

void wxOSX_cursorUpdate(NSView* self, SEL _cmd, NSEvent *event)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;
    
    impl->cursorUpdate(event, self, _cmd);
}

BOOL wxOSX_acceptsFirstMouse(NSView* WXUNUSED(self), SEL WXUNUSED(_cmd), NSEvent *WXUNUSED(event))
{
    // This is needed to support click through, otherwise the first click on a window
    // will not do anything unless it is the active window already.
    return YES;
}

void wxOSX_keyEvent(NSView* self, SEL _cmd, NSEvent *event)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
    {
        wxLogTrace(TRACE_KEYS, "Dropping %s for %s",
                   wxDumpSelector(_cmd), wxDumpNSView(self));
        return;
    }

    impl->keyEvent(event, self, _cmd);
}

void wxOSX_insertText(NSView* self, SEL _cmd, NSString* text)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

    impl->insertText(text, self, _cmd);
}

void wxOSX_panGestureEvent(NSView* self, SEL WXUNUSED(_cmd), NSPanGestureRecognizer* panGestureRecognizer)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl == nullptr )
         return;

    impl->PanGestureEvent(panGestureRecognizer);
}

void wxOSX_zoomGestureEvent(NSView* self, SEL WXUNUSED(_cmd), NSMagnificationGestureRecognizer* magnificationGestureRecognizer)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl == nullptr )
         return;

    impl->ZoomGestureEvent(magnificationGestureRecognizer);
}

void wxOSX_rotateGestureEvent(NSView* self, SEL WXUNUSED(_cmd), NSRotationGestureRecognizer* rotationGestureRecognizer)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl == nullptr )
         return;

    impl->RotateGestureEvent(rotationGestureRecognizer);
}

void wxOSX_longPressEvent(NSView* self, SEL WXUNUSED(_cmd), NSPressGestureRecognizer* pressGestureRecognizer)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl ==nullptr )
        return;

    impl->LongPressEvent(pressGestureRecognizer);
}

void wxOSX_touchesBegan(NSView* self, SEL WXUNUSED(_cmd), NSEvent *event)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl == nullptr )
        return;

    impl->TouchesBegan(event);
}

void wxOSX_touchesMoved(NSView* self, SEL WXUNUSED(_cmd), NSEvent *event)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl == nullptr )
        return;

    impl->TouchesMoved(event);
}

void wxOSX_touchesEnded(NSView* self, SEL WXUNUSED(_cmd), NSEvent *event)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl == nullptr )
        return;

    impl->TouchesEnded(event);
}

BOOL wxOSX_acceptsFirstResponder(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NO;

    return impl->acceptsFirstResponder(self, _cmd);
}

BOOL wxOSX_becomeFirstResponder(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NO;

    return impl->becomeFirstResponder(self, _cmd);
}

BOOL wxOSX_resignFirstResponder(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NO;

    return impl->resignFirstResponder(self, _cmd);
}

#if !wxOSX_USE_NATIVE_FLIPPED

BOOL wxOSX_isFlipped(NSView* self, SEL _cmd)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return NO;

    return impl->isFlipped(self, _cmd) ? YES:NO;
}

#endif

typedef void (*wxOSX_DrawRectHandlerPtr)(NSView* self, SEL _cmd, NSRect rect);

void wxOSX_drawRect(NSView* self, SEL _cmd, NSRect rect)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

#if wxUSE_THREADS
    // OS X starts a NSUIHeartBeatThread for animating the default button in a
    // dialog. This causes a drawRect of the active dialog from outside the
    // main UI thread. This causes an occasional crash since the wx drawing
    // objects (like wxPen) are not thread safe.
    //
    // Notice that NSUIHeartBeatThread seems to be undocumented and doing
    // [NSWindow setAllowsConcurrentViewDrawing:NO] does not affect it.
    if ( !wxThread::IsMain() )
    {
        if ( impl->IsUserPane() )
        {
            wxWindow* win = impl->GetWXPeer();
            if ( win->UseBgCol() )
            {
                
                CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
                CGContextSaveGState( context );

                CGContextSetFillColorWithColor( context, win->GetBackgroundColour().GetCGColor());
                CGRect r = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
                CGContextFillRect( context, r );

                CGContextRestoreGState( context );
            }
        }
        else 
        {
            // just call the superclass handler, we don't need any custom wx drawing
            // here and it seems to work fine:
            wxOSX_DrawRectHandlerPtr
            superimpl = (wxOSX_DrawRectHandlerPtr)
            [[self superclass] instanceMethodForSelector:_cmd];
            superimpl(self, _cmd, rect);
        }

      return;
    }
#endif // wxUSE_THREADS

    return impl->drawRect(&rect, self, _cmd);
}

void wxOSX_controlAction(NSView* self, SEL _cmd, id sender)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

    impl->controlAction(self, _cmd, sender);
}

void wxOSX_controlDoubleAction(NSView* self, SEL _cmd, id sender)
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl == nullptr)
        return;

    impl->controlDoubleAction(self, _cmd, sender);
}

#if wxUSE_DRAG_AND_DROP

namespace
{

unsigned int wxOnDraggingEnteredOrUpdated(wxWidgetCocoaImpl* viewImpl,
                                          WXWidget slf, void *_cmd,
                                          void *s, bool entered)
{
    wxWindow* wxpeer = viewImpl->GetWXPeer();
    wxDropTarget* target = wxpeer ? wxpeer->GetDropTarget() : nullptr;
    if ( target == nullptr )
    {
        if ([[slf superclass] instancesRespondToSelector:(SEL)_cmd])
        {
            auto superimpl = (wxOSX_DraggingEnteredOrUpdatedHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
            return superimpl(slf, (SEL)_cmd, s);
        }
        else
            return NSDragOperationNone;
    }

    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;
    wxOSXPasteboard pb([sender draggingPasteboard]);
    /*
    sourceDragMask contains a flag field with drag operations permitted by
    the source:
    NSDragOperationCopy = 1,
    NSDragOperationLink = 2,
    NSDragOperationGeneric = 4,
    NSDragOperationPrivate = 8,
    NSDragOperationMove = 16,
    NSDragOperationDelete = 32

    By default, pressing modifier keys changes sourceDragMask:
    Control ANDs it with NSDragOperationLink (2)
    Option ANDs it with NSDragOperationCopy (1)
    Command ANDs it with NSDragOperationGeneric (4)

    The end result can be a mask that's 0 (NSDragOperationNone).
    */
    NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];

    NSPoint nspoint = [viewImpl->GetWXWidget() convertPoint:[sender draggingLocation] fromView:nil];
    wxPoint pt = wxFromNSPoint( viewImpl->GetWXWidget(), nspoint );

    /*
    Convert the incoming mask to wxDragResult. This is a lossy conversion
    because wxDragResult contains a single value and not a flag field.
    When dragging the bottom part of the DND sample ("Drag text from here!")
    sourceDragMask contains copy, link, generic, and private flags. Formerly
    this would result in wxDragLink which is not what is expected for text.
    Give precedence to the move and copy flag instead.

    TODO:
    In order to respect wxDrag_DefaultMove, access to dnd.mm's
    DropSourceDelegate will be needed which contains the wxDrag value used.
    (The draggingSource method of sender points to a DropSourceDelegate* ).
    */
    wxDragResult result = wxDragNone;

    if (sourceDragMask & NSDragOperationMove)
        result = wxDragMove;
    else if ( sourceDragMask & NSDragOperationCopy
        || sourceDragMask & NSDragOperationGeneric)
        result = wxDragCopy;
    else if (sourceDragMask & NSDragOperationLink)
        result = wxDragLink;

    target->SetCurrentDragSource(&pb);
    if (entered)
    {
        // Drag entered
        result = target->OnEnter(pt.x, pt.y, result);
    }
    else
    {
        // Drag updated
        result = target->OnDragOver(pt.x, pt.y, result);
    }

    NSDragOperation nsresult = NSDragOperationNone;
    switch (result )
    {
        case wxDragLink:
            nsresult = NSDragOperationLink;
            break;

        case wxDragMove:
            nsresult = NSDragOperationMove;
            break;

        case wxDragCopy:
            nsresult = NSDragOperationCopy;
            break;

        default :
            break;
    }
    return nsresult;
}

} // anonymous namespace

unsigned int wxWidgetCocoaImpl::draggingEntered(void* s, WXWidget slf, void *_cmd)
{
    return wxOnDraggingEnteredOrUpdated(this, slf, _cmd, s, true /*entered*/);
}

void wxWidgetCocoaImpl::draggingExited(void* s, WXWidget slf, void *_cmd)
{
    wxWindow* wxpeer = GetWXPeer();
    wxDropTarget* target = wxpeer ? wxpeer->GetDropTarget() : nullptr;
    if ( target == nullptr )
    {
        if ([[slf superclass] instancesRespondToSelector:(SEL)_cmd])
        {
            auto superimpl = (wxOSX_DraggingExitedHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
            superimpl(slf, (SEL)_cmd, s);
        }
        return;
    }

    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;
    wxOSXPasteboard pb([sender draggingPasteboard]);

    target->SetCurrentDragSource(&pb);
    target->OnLeave();
 }

unsigned int wxWidgetCocoaImpl::draggingUpdated(void* s, WXWidget slf, void *_cmd)
{
    return wxOnDraggingEnteredOrUpdated(this, slf, _cmd, s, false /*updated*/);
}

bool wxWidgetCocoaImpl::performDragOperation(void* s, WXWidget slf, void *_cmd)
{
    wxWindow* wxpeer = GetWXPeer();
    wxDropTarget* target = wxpeer ? wxpeer->GetDropTarget() : nullptr;
    if ( target == nullptr )
    {
        if ([[slf superclass] instancesRespondToSelector:(SEL)_cmd])
        {
            auto superimpl = (wxOSX_PerformDragOperationHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
            return superimpl(slf, (SEL)_cmd, s);
        }
        else
            return false;
    }

    id <NSDraggingInfo>sender = (id <NSDraggingInfo>) s;
    wxOSXPasteboard pb([sender draggingPasteboard]);
    NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];

    wxDragResult result = wxDragNone;
    NSPoint nspoint = [m_osxView convertPoint:[sender draggingLocation] fromView:nil];
    wxPoint pt = wxFromNSPoint( m_osxView, nspoint );

    if (sourceDragMask & NSDragOperationMove)
        result = wxDragMove;
    else if ( sourceDragMask & NSDragOperationCopy
        || sourceDragMask & NSDragOperationGeneric)
        result = wxDragCopy;
    else if (sourceDragMask & NSDragOperationLink)
        result = wxDragLink;

    target->SetCurrentDragSource(&pb);

    if (target->OnDrop(pt.x, pt.y))
        result = target->OnData(pt.x, pt.y, result);

    return result != wxDragNone;
}
#endif // wxUSE_DRAG_AND_DROP

void wxWidgetCocoaImpl::mouseEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    // we are getting moved events for all windows in the hierarchy, not something wx expects
    // therefore we only handle it for the deepest child in the hierarchy
    if ( [event type] == NSMouseMoved )
    {
        NSView* hitview = [[[slf window] contentView] hitTest:[event locationInWindow]];
        if ( hitview == nullptr || hitview != slf)
            return;
    }

    // The Infinity IN-USB-2 V15 foot pedal on OS 11 produces spurious mouse
    // button events with button number = 10.
    // We cannot do anything useful with button numbers > 4, so throw them away.
    switch ( [event type] )
    {
        case NSLeftMouseDown:
        case NSRightMouseDown:
        case NSOtherMouseDown:
        case NSLeftMouseUp:
        case NSRightMouseUp:
        case NSOtherMouseUp:
            if ( [event buttonNumber] > 4 )
                return;
            break;

        default:
            // Just to avoid -Wswitch.
            break;
    }

    if ( !DoHandleMouseEvent(event) )
    {
        // for plain NSView mouse events would propagate to parents otherwise
        // scrollwheel events have to be propagated if not handled in all cases
        if (!HasUserMouseHandling() || [event type] == NSScrollWheel )
        {
            wxOSX_EventHandlerPtr superimpl = (wxOSX_EventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
            superimpl(slf, (SEL)_cmd, event);

            // super of built-ins keeps the mouse up, as wx expects this event, we have to synthesize it
            // only trigger if at this moment the mouse is already up, and the control is still existing after the event has
            // been handled (we do this by looking up the native NSView's peer from the hash map, that way we are sure the info
            // is current - even when the instance memory of ourselves may have been freed ...

            wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( slf );
            if ( [ event type]  == NSLeftMouseDown && !wxGetMouseState().LeftIsDown() && impl != nullptr )
            {
                wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
                SetupMouseEvent(wxevent , event) ;
                wxevent.SetEventType(wxEVT_LEFT_UP);

                GetWXPeer()->HandleWindowEvent(wxevent);
            }
        }
    }
}

void wxWidgetCocoaImpl::cursorUpdate(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    if ( !SetupCursor(event) )
    {
        wxOSX_EventHandlerPtr superimpl = (wxOSX_EventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
            superimpl(slf, (SEL)_cmd, event);
    }
 }

bool wxWidgetCocoaImpl::SetupCursor(WX_NSEvent event)
{
    extern wxCursor gGlobalCursor;
    
    if ( gGlobalCursor.IsOk() )
    {
        gGlobalCursor.MacInstall();
        return true;
    }
    else
    {
        wxWindow* cursorTarget = GetWXPeer();
        wxCoord x,y;
        SetupCoordinates(x, y, event);
        wxPoint cursorPoint( x , y ) ;
        
        while ( cursorTarget && !cursorTarget->MacSetupCursor( cursorPoint ) )
        {
            // at least in GTK cursor events are not propagated either ...
#if 1
            cursorTarget = nullptr;
#else
            cursorTarget = cursorTarget->GetParent() ;
            if ( cursorTarget )
                cursorPoint += cursorTarget->GetPosition();
#endif
        }
        
        return cursorTarget != nullptr;
    }
}

void wxWidgetCocoaImpl::keyEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    wxLogTrace(TRACE_KEYS, "Got %s for %s",
               wxDumpSelector((SEL)_cmd), wxDumpNSView(slf));

    if ( !m_wxPeer->IsEnabled() )
        return;

    if ( [event type] == NSKeyDown )
    {
        // there are key equivalents that are not command-combos and therefore not handled by cocoa automatically, 
        // therefore we call the menubar directly here, exit if the menu is handling the shortcut
        if ( [[[NSApplication sharedApplication] mainMenu] performKeyEquivalent:event] )
        {
            wxLogTrace(TRACE_KEYS, "%s processed as key equivalent by the menu",
                       wxDumpSelector((SEL)_cmd));
            return;
        }

        BeginNativeKeyDownEvent(event);
    }
    
    if ( GetFocusedViewInWindow([slf window]) != slf || m_hasEditor || !DoHandleKeyEvent(event) )
    {
        wxOSX_EventHandlerPtr superimpl = (wxOSX_EventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        superimpl(slf, (SEL)_cmd, event);
    }
    
    if ( [event type] == NSKeyDown )
    {
        EndNativeKeyDownEvent();
    }
}

// Class containing data used for gestures support.
class wxCocoaGesturesImpl
{
public:
    wxCocoaGesturesImpl(wxWidgetCocoaImpl* impl, NSView* view, int eventsMask)
        : m_win(impl->GetWXPeer()),
          m_view(view)
    {
        m_touchCount = 0;
        m_lastTouchTime = 0;
        m_allowedGestures = 0;
        m_activeGestures = 0;
        m_initialTouch = nullptr;

        Class cls = [m_view class];

        if ( eventsMask & wxTOUCH_PAN_GESTURES )
        {
            eventsMask &= ~wxTOUCH_PAN_GESTURES;

            wxCLANG_WARNING_SUPPRESS(undeclared-selector)
            m_panGestureRecognizer =
            [[NSPanGestureRecognizer alloc] initWithTarget:m_view action: @selector(handlePanGesture:)];
            if ( !class_respondsToSelector(cls, @selector(handlePanGesture:)) )
                class_addMethod(cls, @selector(handlePanGesture:), (IMP) wxOSX_panGestureEvent, "v@:@" );
            wxCLANG_WARNING_RESTORE(undeclared-selector)

            [m_view addGestureRecognizer:m_panGestureRecognizer];
        }
        else
        {
            m_panGestureRecognizer = nil;
        }

        if ( eventsMask & wxTOUCH_ZOOM_GESTURE )
        {
            eventsMask &= ~wxTOUCH_ZOOM_GESTURE;

            wxCLANG_WARNING_SUPPRESS(undeclared-selector)
            m_magnificationGestureRecognizer =
            [[NSMagnificationGestureRecognizer alloc] initWithTarget:m_view action: @selector(handleZoomGesture:)];
            if ( !class_respondsToSelector(cls, @selector(handleZoomGesture:)) )
                class_addMethod(cls, @selector(handleZoomGesture:), (IMP) wxOSX_zoomGestureEvent, "v@:@" );
            wxCLANG_WARNING_RESTORE(undeclared-selector)

            [m_view addGestureRecognizer:m_magnificationGestureRecognizer];
        }
        else
        {
            m_magnificationGestureRecognizer = nil;
        }

        if ( eventsMask & wxTOUCH_ROTATE_GESTURE )
        {
            eventsMask &= ~wxTOUCH_ROTATE_GESTURE;

            wxCLANG_WARNING_SUPPRESS(undeclared-selector)
            m_rotationGestureRecognizer =
            [[NSRotationGestureRecognizer alloc] initWithTarget:m_view action: @selector(handleRotateGesture:)];
            if ( !class_respondsToSelector(cls, @selector(handleRotateGesture:)) )
                class_addMethod(cls, @selector(handleRotateGesture:), (IMP) wxOSX_rotateGestureEvent, "v@:@" );
            wxCLANG_WARNING_RESTORE(undeclared-selector)

            [m_view addGestureRecognizer:m_rotationGestureRecognizer];
        }
        else
        {
            m_rotationGestureRecognizer = nil;
        }

        if ( eventsMask & wxTOUCH_PRESS_GESTURES )
        {
            eventsMask &= ~wxTOUCH_PRESS_GESTURES;

            wxCLANG_WARNING_SUPPRESS(undeclared-selector)
            m_pressGestureRecognizer =
            [[NSPressGestureRecognizer alloc] initWithTarget:m_view action: @selector(handleLongPressGesture:)];
            if ( !class_respondsToSelector(cls, @selector(handleLongPressGesture:)) )
                class_addMethod(cls, @selector(handleLongPressGesture:), (IMP) wxOSX_longPressEvent, "v@:@" );
            wxCLANG_WARNING_RESTORE(undeclared-selector)

            [m_view addGestureRecognizer:m_pressGestureRecognizer];
        }
        else
        {
            m_pressGestureRecognizer = nil;
        }

        wxASSERT_MSG( eventsMask == 0, "Unknown touch event mask bit specified" );

        if ( !class_respondsToSelector(cls, @selector(touchesBeganWithEvent:)) )
            class_addMethod(cls, @selector(touchesBeganWithEvent:), (IMP) wxOSX_touchesBegan, "v@:@" );
        if ( !class_respondsToSelector(cls, @selector(touchesMovedWithEvent:)) )
            class_addMethod(cls, @selector(touchesMovedWithEvent:), (IMP) wxOSX_touchesMoved, "v@:@" );
        if ( !class_respondsToSelector(cls, @selector(touchesEndedWithEvent:)) )
            class_addMethod(cls, @selector(touchesEndedWithEvent:), (IMP) wxOSX_touchesEnded, "v@:@" );
    }

    ~wxCocoaGesturesImpl()
    {
        [m_panGestureRecognizer release];
        [m_magnificationGestureRecognizer release];
        [m_rotationGestureRecognizer release];
        [m_pressGestureRecognizer release];
        [m_initialTouch release];
    }

    void TouchesBegan(NSEvent* event);
    void TouchesMoved(NSEvent* event);
    void TouchesEnded(NSEvent* event);

private:
    wxWindowMac* const m_win;
    NSView* const m_view;

    NSPanGestureRecognizer *m_panGestureRecognizer;
    NSMagnificationGestureRecognizer *m_magnificationGestureRecognizer;
    NSRotationGestureRecognizer *m_rotationGestureRecognizer;
    NSPressGestureRecognizer *m_pressGestureRecognizer;

    int m_allowedGestures;
    int m_activeGestures;
    unsigned int m_touchCount;
    unsigned int m_lastTouchTime;

    // Used to keep track of the touch corresponding to "press" in Press and Tap gesture
    NSTouch* m_initialTouch;

    wxDECLARE_NO_COPY_CLASS(wxCocoaGesturesImpl);
};

// We keep all existing wxCocoaGesturesImpl objects in a
// wxWidgetCocoaImpl-indexed map. We do this instead of just having a data
// member containing wxCocoaGesturesImpl pointer in wxWidgetCocoaImpl
// itself because most windows don't need it and it seems wasteful to
// always increase their size unnecessarily.

// wxCocoaGesturesImpl is only used under 10.10+ and so clang warns about
// wxCocoaGesturesImplMap not having 10.10 availability attribute, but there is
// no simple way to make it pass through the macro, so just suppress the
// warning instead.
wxCLANG_WARNING_SUPPRESS(unguarded-availability)

#include <unordered_map>
using wxCocoaGesturesImplMap = std::unordered_map<wxWidgetCocoaImpl*, wxCocoaGesturesImpl*>;

#include "wx/private/extfield.h"
typedef wxExternalField<wxWidgetCocoaImpl,
                        wxCocoaGesturesImpl,
                        wxCocoaGesturesImplMap> wxCocoaGestures;

wxCLANG_WARNING_RESTORE(unguarded-availability)

void wxWidgetCocoaImpl::PanGestureEvent(NSPanGestureRecognizer* panGestureRecognizer)
{
    NSGestureRecognizerState gestureState;

    switch ( [panGestureRecognizer state] )
    {
        case NSGestureRecognizerStateBegan:
             gestureState = NSGestureRecognizerStateBegan;
             break;
        case NSGestureRecognizerStateChanged:
             gestureState = NSGestureRecognizerStateChanged;
             break;
        case NSGestureRecognizerStateEnded:
        case NSGestureRecognizerStateCancelled:
             gestureState = NSGestureRecognizerStateEnded;
             break;
        // Do not process any other states
        default:
             return;
    }

    wxPanGestureEvent wxevent(GetWXPeer()->GetId());
    wxevent.SetEventObject(GetWXPeer());

    NSPoint nspoint = [panGestureRecognizer locationInView:m_osxView];
    wxPoint pt = wxFromNSPoint(m_osxView, nspoint);

    wxevent.SetPosition(pt);

    nspoint = [panGestureRecognizer translationInView:m_osxView];
    pt = wxFromNSPoint(m_osxView, nspoint);

    static wxPoint s_lastLocation;

    if ( gestureState == NSGestureRecognizerStateBegan )
    {
        wxevent.SetGestureStart();
        s_lastLocation = wxPoint(0, 0);
    }

    if ( gestureState == NSGestureRecognizerStateEnded )
    {
        wxevent.SetGestureEnd();
    }

    // Set the offset
    wxevent.SetDelta(pt - s_lastLocation);

    s_lastLocation = pt;

    GetWXPeer()->HandleWindowEvent(wxevent);
}

void wxWidgetCocoaImpl::ZoomGestureEvent(NSMagnificationGestureRecognizer* magnificationGestureRecognizer)
{
    NSGestureRecognizerState gestureState;

    switch ( [magnificationGestureRecognizer state] )
    {
        case NSGestureRecognizerStateBegan:
             gestureState = NSGestureRecognizerStateBegan;
             break;
        case NSGestureRecognizerStateChanged:
             gestureState = NSGestureRecognizerStateChanged;
             break;
        case NSGestureRecognizerStateEnded:
        case NSGestureRecognizerStateCancelled:
             gestureState = NSGestureRecognizerStateEnded;
             break;
        // Do not process any other states
        default:
             return;
    }

    wxZoomGestureEvent wxevent(GetWXPeer()->GetId());
    wxevent.SetEventObject(GetWXPeer());

    NSPoint nspoint = [magnificationGestureRecognizer locationInView:m_osxView];
    wxPoint pt = wxFromNSPoint(m_osxView, nspoint);

    wxevent.SetPosition(pt);

    if ( gestureState == NSGestureRecognizerStateBegan )
    {
        wxevent.SetGestureStart();
    }

    if ( gestureState == NSGestureRecognizerStateEnded )
    {
        wxevent.SetGestureEnd();
    }

    double magnification = [magnificationGestureRecognizer magnification];

    // Add 1.0 get the magnification.
    magnification += 1.0;

    wxevent.SetZoomFactor(magnification);

    GetWXPeer()->HandleWindowEvent(wxevent);
}

void wxWidgetCocoaImpl::RotateGestureEvent(NSRotationGestureRecognizer* rotationGestureRecognizer)
{
    NSGestureRecognizerState gestureState;

    switch ( [rotationGestureRecognizer state] )
    {
        case NSGestureRecognizerStateBegan:
             gestureState = NSGestureRecognizerStateBegan;
             break;
        case NSGestureRecognizerStateChanged:
             gestureState = NSGestureRecognizerStateChanged;
             break;
        case NSGestureRecognizerStateEnded:
        case NSGestureRecognizerStateCancelled:
             gestureState = NSGestureRecognizerStateEnded;
             break;
        // Do not process any other states
        default:
             return;
    }

    wxRotateGestureEvent wxevent(GetWXPeer()->GetId());
    wxevent.SetEventObject(GetWXPeer());

    NSPoint nspoint = [rotationGestureRecognizer locationInView:m_osxView];
    wxPoint pt = wxFromNSPoint(m_osxView, nspoint);

    wxevent.SetPosition(pt);

    if ( gestureState == NSGestureRecognizerStateBegan )
    {
        wxevent.SetGestureStart();
    }

    if ( gestureState == NSGestureRecognizerStateEnded )
    {
        wxevent.SetGestureEnd();
    }
    // Multiply the returned rotation angle with -1 to obtain the angle in a clockwise sense.
    double angle = -[rotationGestureRecognizer rotation];

    // If the rotation is anti-clockwise convert the angle to its corresponding positive value in a clockwise sense.
    if ( angle < 0 )
    {
        angle += 2 * M_PI;
    }

    wxevent.SetRotationAngle(angle);

    GetWXPeer()->HandleWindowEvent(wxevent);
}

void wxWidgetCocoaImpl::LongPressEvent(NSPressGestureRecognizer* pressGestureRecognizer)
{
    NSGestureRecognizerState gestureState;

    switch ( [pressGestureRecognizer state] )
    {
        case NSGestureRecognizerStateBegan:
            gestureState = NSGestureRecognizerStateBegan;
            break;
        case NSGestureRecognizerStateChanged:
            gestureState = NSGestureRecognizerStateChanged;
            break;
        case NSGestureRecognizerStateEnded:
        case NSGestureRecognizerStateCancelled:
            gestureState = NSGestureRecognizerStateEnded;
            break;
            // Do not process any other states
        default:
            return;
    }

    wxLongPressEvent wxevent(GetWXPeer()->GetId());
    wxevent.SetEventObject(GetWXPeer());

    NSPoint nspoint = [pressGestureRecognizer locationInView:m_osxView];
    wxPoint pt = wxFromNSPoint(m_osxView, nspoint);

    wxevent.SetPosition(pt);

    if ( gestureState == NSGestureRecognizerStateBegan )
    {
        wxevent.SetGestureStart();
    }

    if ( gestureState == NSGestureRecognizerStateEnded )
    {
        wxevent.SetGestureEnd();
    }

    GetWXPeer()->HandleWindowEvent(wxevent);
}

enum TrackedGestures
{
    two_finger_tap = 0x0001,
    press_and_tap  = 0x0002
};

void wxWidgetCocoaImpl::TouchesBegan(WX_NSEvent event)
{
    if ( wxCocoaGesturesImpl* gestures = wxCocoaGestures::FromObject(this) )
        gestures->TouchesBegan(event);
}

void wxCocoaGesturesImpl::TouchesBegan(NSEvent* event)
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:m_view];

    m_touchCount += touches.count;
    m_allowedGestures &= ~two_finger_tap;

    // Check if 2 fingers are placed together
    if ( m_touchCount == 2 && touches.count == 2 )
    {
        // Two Finger Tap Event may occur in future
        m_allowedGestures |= two_finger_tap;

        // Cancel Press and Tap Event
        m_allowedGestures &= ~press_and_tap;

        return;
    }

    // Time of event in milliseconds
    const unsigned int eventTimeStamp = event.timestamp * 1000 + 0.5;

    if ( m_touchCount == 1 )
    {
        // Save the time of event
        m_lastTouchTime = eventTimeStamp;

        // Press and Tap may occur in future
        m_allowedGestures |= press_and_tap;

        NSArray* array = [touches allObjects];

        // Save the touch corresponding to "press"
        m_initialTouch = [[array objectAtIndex:0] copy];
    }

    touches = [event touchesMatchingPhase:NSTouchPhaseStationary inView:m_view];

    // Check if 2 fingers are placed within the time interval of 200 milliseconds
    if ( m_touchCount == 2 && touches.count == 1 && eventTimeStamp - m_lastTouchTime <= wxTwoFingerTimeInterval )
    {
        // Two Finger Tap Event may occur in future
        m_allowedGestures |= two_finger_tap;

        // Cancel Press and Tap
        m_allowedGestures &= ~press_and_tap;

        [m_initialTouch release];
    }
}

void wxWidgetCocoaImpl::TouchesMoved(WX_NSEvent event)
{
    if ( wxCocoaGesturesImpl* gestures = wxCocoaGestures::FromObject(this) )
        gestures->TouchesMoved(event);
}

void wxCocoaGesturesImpl::TouchesMoved(NSEvent* event)
{
    // Cancel Two Finger Tap Event if there is any movement
    m_allowedGestures &= ~two_finger_tap;

    if ( !(m_allowedGestures & press_and_tap) )
    {
        return;
    }

    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:m_view];

    NSArray* array = [touches allObjects];

    // Iterate through all moving touches to check if the touch corresponding to "press"
    // in Press and Tap event is moving.
    for ( unsigned i = 0; i < [array count]; ++i )
    {
        NSTouch* touch = [array objectAtIndex:i];

        // Check if this touch and m_initialTouch are same
        if ( [touch.identity isEqual:m_initialTouch.identity] )
        {
            // Process Press and Tap Event if the touch corresponding to "press" is moving
            // and the gesture is active.
            if ( m_activeGestures & press_and_tap )
            {
                wxPressAndTapEvent wxevent(m_win->GetId());
                wxevent.SetEventObject(m_win);

                // Get the mouse coordinates
                wxCoord x, y;
                wxSetupCoordinates(m_view, x, y, event);
                wxevent.SetPosition(wxPoint (x,y));

                m_win->HandleWindowEvent(wxevent);
            }

            // Cancel Press and Tap Event if the touch corresponding to "press" is moving
            // and the gesture is not active.
            else
            {
                m_allowedGestures &= ~press_and_tap;
            }

            return;
        }
    }
}

void wxWidgetCocoaImpl::TouchesEnded(WX_NSEvent event)
{
    if ( wxCocoaGesturesImpl* gestures = wxCocoaGestures::FromObject(this) )
        gestures->TouchesEnded(event);
}

void wxCocoaGesturesImpl::TouchesEnded(NSEvent* event)
{
    NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:m_view];

    m_touchCount -= touches.count;

    // Time of event in milliseconds
    const unsigned int eventTimeStamp = event.timestamp * 1000 + 0.5;

    // Check if 2 fingers are lifted off together or if 2 fingers are lifted off within the time interval of 200 milliseconds
    if ( (!m_touchCount && (m_allowedGestures & two_finger_tap)) &&
         (touches.count == 2 ||
         (touches.count == 1 && eventTimeStamp - m_lastTouchTime <= wxTwoFingerTimeInterval)) )
    {
        wxTwoFingerTapEvent wxevent(m_win->GetId());
        wxevent.SetEventObject(m_win);
        wxevent.SetGestureStart();
        wxevent.SetGestureEnd();

        // Get the mouse coordinates
        wxCoord x, y;
        wxSetupCoordinates(m_view, x, y, event);
        wxevent.SetPosition(wxPoint (x,y));

        m_win->HandleWindowEvent(wxevent);
    }

    // If Two Finger Tap Event is possible in future then save the timestamp to use it when the other touch
    // leaves the surface.
    else if ( m_touchCount == 1 && (m_allowedGestures & two_finger_tap) )
    {
        m_lastTouchTime = eventTimeStamp;
    }

    // Check if Press and Tap event is possible.
    else if ( m_allowedGestures & press_and_tap )
    {
        NSArray* array = [touches allObjects];

        // True if touch that ended is the touch corresponding to "press"
        bool isPressTouch = false;

        // Iterate through all ended touches
        for( unsigned i = 0; i < [array count]; ++i )
        {
            NSTouch* touch = [array objectAtIndex:i];

            // Check if touch that ended is the touch corresponding to "press"
            if ( [touch.identity isEqual:m_initialTouch.identity] )
            {
                isPressTouch = true;
                break;
            }
        }

        // Cancel Press and Tap Event if the touch corresponding to press is ended
        // and Press and Tap was not active
        if ( isPressTouch && !(m_activeGestures & press_and_tap) )
        {
            m_allowedGestures &= ~press_and_tap;
            return;
        }

        wxPressAndTapEvent wxevent(m_win->GetId());
        wxevent.SetEventObject(m_win);

        // Get the mouse coordinates
        wxCoord x, y;
        wxSetupCoordinates(m_view, x, y, event);
        wxevent.SetPosition(wxPoint (x,y));

        if ( !(m_activeGestures & press_and_tap) )
        {
            wxevent.SetGestureStart();
            m_activeGestures |= press_and_tap;
        }

        // End Press and Tap Event if the touch corresponding to "press" is lifted off
        else if ( isPressTouch )
        {
            wxevent.SetGestureEnd();

            m_activeGestures &= ~press_and_tap;
            m_allowedGestures &= ~press_and_tap;
            [m_initialTouch release];
        }

        m_win->HandleWindowEvent(wxevent);
    }

    else
    {
        m_allowedGestures &= ~press_and_tap;
        m_allowedGestures &= ~two_finger_tap;
        m_activeGestures &= ~press_and_tap;
    }
}

void wxWidgetCocoaImpl::insertText(NSString* text, WXWidget slf, void *_cmd)
{
    wxLogTrace(TRACE_KEYS, "Insert text \"%s\" for %s",
               wxStringWithNSString(text), wxDumpNSView(slf));

    bool result = false;
    if ( HasUserKeyHandling() && !m_hasEditor && [text length] > 0)
    {
        if ( IsInNativeKeyDown() && [text isEqualToString:[GetLastNativeKeyDownEvent() characters]])
        {
            // If we have a corresponding key event, send wxEVT_KEY_DOWN now.
            // (see also: wxWidgetCocoaImpl::DoHandleKeyEvent)
            {
                wxKeyEvent wxevent(wxEVT_KEY_DOWN);
                SetupKeyEvent( wxevent, GetLastNativeKeyDownEvent() );
                result = GetWXPeer()->OSXHandleKeyEvent(wxevent);
                SetKeyDownSent();
            }

            // ...and wxEVT_CHAR.
            result = result || DoHandleCharEvent(GetLastNativeKeyDownEvent(), text);
        }
        else
        {
            // If we don't have a corresponding key event (e.g. IME-composed
            // characters), send wxEVT_CHAR without sending wxEVT_KEY_DOWN.
            result = DoHandleCharEvent(nullptr,text);
        }
    }
    if ( !result )
    {
        wxOSX_TextEventHandlerPtr superimpl = (wxOSX_TextEventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        superimpl(slf, (SEL)_cmd, text);
    }
}

bool wxWidgetCocoaImpl::doCommandBySelector(void* sel, WXWidget slf, void* WXUNUSED(_cmd))
{
    wxLogTrace(TRACE_KEYS, "Selector %s for %s",
               wxDumpSelector((SEL)sel), wxDumpNSView(slf));

    // keystrokes can be translated on macos to selectors, see
    // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/TextDefaultsBindings/TextDefaultsBindings.html
    // it is also possible to map 1 keystroke to multiple commands, eg Ctrl-O on mac is translated to the bash-equivalent of
    // execute and move back in history, since this results in two commands, Ctrl-O was sent twice as a wx key down event.
    // we now track the sending of the events to avoid duplicates.

    bool handled = false;

    if ( IsInNativeKeyDown() && !WasKeyDownSent())
    {
        // If we have a corresponding key event, send wxEVT_KEY_DOWN now.
        // (see also: wxWidgetCocoaImpl::DoHandleKeyEvent)
        wxKeyEvent wxevent(wxEVT_KEY_DOWN);
        SetupKeyEvent( wxevent, GetLastNativeKeyDownEvent() );
        handled = GetWXPeer()->OSXHandleKeyEvent(wxevent);

        if (!handled)
        {
            // Generate wxEVT_CHAR if wxEVT_KEY_DOWN is not handled.

            wxKeyEvent wxevent2(wxevent) ;
            wxevent2.SetEventType(wxEVT_CHAR);
            SetupKeyEvent( wxevent2, GetLastNativeKeyDownEvent() );
            handled = GetWXPeer()->OSXHandleKeyEvent(wxevent2);
        }
        SetKeyDownSent();
    }
    else
    {
        wxLogTrace(TRACE_KEYS, "Doing nothing in doCommandBySelector:");
    }

    return handled;
}

bool wxWidgetCocoaImpl::acceptsFirstResponder(WXWidget slf, void *_cmd)
{
    if ( HasUserKeyHandling() )
        return m_wxPeer->AcceptsFocus();
    else
    {
        wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        return superimpl(slf, (SEL)_cmd);
    }
}

bool wxWidgetCocoaImpl::becomeFirstResponder(WXWidget slf, void *_cmd)
{
    wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    BOOL r = superimpl(slf, (SEL)_cmd);
    if ( r )
        DoNotifyFocusSet();

    return r;
}

bool wxWidgetCocoaImpl::resignFirstResponder(WXWidget slf, void *_cmd)
{
    wxOSX_FocusHandlerPtr superimpl = (wxOSX_FocusHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    BOOL r = superimpl(slf, (SEL)_cmd);
    
    // wxNSTextFields and wxNSComboBoxes have an editor as real responder, therefore they get
    // a resign notification when their editor takes over, don't trigger  event here, the control
    // gets a controlTextDidEndEditing notification which will send a focus kill.
    if ( r && !m_hasEditor)
        DoNotifyFocusLost();

    return r;
}

#if !wxOSX_USE_NATIVE_FLIPPED

bool wxWidgetCocoaImpl::isFlipped(WXWidget slf, void *WXUNUSED(_cmd))
{
    return m_isFlipped;
}

#endif

#define OSX_DEBUG_DRAWING 0

void wxWidgetCocoaImpl::drawRect(void* rect, WXWidget slf, void *WXUNUSED(_cmd))
{
    // preparing the update region
    
    wxRegion updateRgn;

    // since adding many rects to a region is a costly process, by default use the bounding rect
#if 0
    const NSRect *rects;
    NSInteger count;
    [slf getRectsBeingDrawn:&rects count:&count];
    for ( int i = 0 ; i < count ; ++i )
    {
        updateRgn.Union(wxFromNSRect(slf, rects[i]));
    }
#else
    updateRgn.Union(wxFromNSRect(slf,*(NSRect*)rect));
#endif
    
    wxWindow* wxpeer = GetWXPeer();

    if ( wxpeer->MacGetLeftBorderSize() != 0 || wxpeer->MacGetTopBorderSize() != 0 )
    {
        // as this update region is in native window locals we must adapt it to wx window local
        updateRgn.Offset( wxpeer->MacGetLeftBorderSize() , wxpeer->MacGetTopBorderSize() );
    }
    
    // Restrict the update region to the shape of the window, if any, and also
    // remember the region that we need to clear later.
    wxNonOwnedWindow* const tlwParent = wxpeer->MacGetTopLevelWindow();
    if ( tlwParent == nullptr )
        return;
    
    const bool isTopLevel = tlwParent == wxpeer;
    wxRegion clearRgn;
    if ( tlwParent->GetWindowStyle() & wxFRAME_SHAPED )
    {
        wxRegion rgn = tlwParent->GetShape();
        if ( rgn.IsOk() )
        {
            if ( isTopLevel )
                clearRgn = updateRgn;

            int xoffset = 0, yoffset = 0;
            wxpeer->MacRootWindowToWindow( &xoffset, &yoffset );
            rgn.Offset( xoffset, yoffset );
            updateRgn.Intersect(rgn);

            if ( isTopLevel )
            {
                // Exclude the window shape from the region to be cleared below.
                rgn.Xor(wxpeer->GetSize());
                clearRgn.Intersect(rgn);
            }
        }
    }
    
    wxpeer->GetUpdateRegion() = updateRgn;

    // setting up the drawing context
    // note that starting from 10.14 this may be null in certain views
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    wxpeer->MacSetCGContextRef( context );
    if ( context != nullptr )
    {
        CGContextSaveGState( context );
        
#if OSX_DEBUG_DRAWING
        CGContextBeginPath( context );
        CGContextMoveToPoint(context, 0, 0);
        NSRect bounds = [slf bounds];
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
        
        if ( ![slf isFlipped] )
        {
            CGContextTranslateCTM( context, 0,  [m_osxView bounds].size.height );
            CGContextScaleCTM( context, 1, -1 );
        }
    }

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
        superimpl(slf, _cmd, *(NSRect*)rect);
        if ( context != nullptr )
        {
            CGContextRestoreGState( context );
            CGContextSaveGState( context );
        }
    }
    
    if ( context != nullptr )
    {
        // as we called restore above, we have to flip again if necessary
        if ( ![slf isFlipped] )
        {
            CGContextTranslateCTM( context, 0,  [m_osxView bounds].size.height );
            CGContextScaleCTM( context, 1, -1 );
        }
        
        if ( isTopLevel )
        {
            // We also need to explicitly draw the part of the top level window
            // outside of its region with transparent colour to ensure that it is
            // really transparent.
            if ( clearRgn.IsOk() )
            {
                wxMacCGContextStateSaver saveState(context);
                wxWindowDC dc(wxpeer);
                dc.SetBackground(wxBrush(wxTransparentColour));
                dc.SetDeviceClippingRegion(clearRgn);
                dc.Clear();
            }
            
#if wxUSE_GRAPHICS_CONTEXT
            // If the window shape is defined by a path, stroke the path to show
            // the window border.
            const wxGraphicsPath& path = tlwParent->GetShapePath();
            if ( !path.IsNull() )
            {
                CGContextSetLineWidth(context, 1);
                CGContextSetStrokeColorWithColor(context, wxLIGHT_GREY->GetCGColor());
                CGContextAddPath(context, (CGPathRef) path.GetNativePath());
                CGContextStrokePath(context);
            }
#endif // wxUSE_GRAPHICS_CONTEXT
        }
        
        wxpeer->MacPaintChildrenBorders();
        CGContextRestoreGState( context );
    }
    wxpeer->MacSetCGContextRef( nullptr );
}

void wxWidgetCocoaImpl::controlAction( WXWidget WXUNUSED(slf), void *WXUNUSED(_cmd), void *WXUNUSED(sender))
{
    wxWindow* wxpeer = (wxWindow*) GetWXPeer();
    if ( wxpeer )
    {
        wxpeer->OSXSimulateFocusEvents();
        wxpeer->OSXHandleClicked(0);
    }
}

void wxWidgetCocoaImpl::controlDoubleAction( WXWidget WXUNUSED(slf), void *WXUNUSED(_cmd), void *WXUNUSED(sender))
{
}

void wxWidgetCocoaImpl::controlTextDidChange()
{
    wxWindow* wxpeer = (wxWindow*)GetWXPeer();
    if ( wxpeer ) 
    {
        // since native rtti doesn't have to be enabled and wx' rtti is not aware of the mixin wxTextEntry, workaround is needed
        if ( wxTextCtrl *tc = wxDynamicCast( wxpeer , wxTextCtrl ) )
        {
            tc->MarkDirty();
            tc->SendTextUpdatedEventIfAllowed();
        }
        else if ( wxComboBox *cb = wxDynamicCast( wxpeer , wxComboBox ) )
            cb->SendTextUpdatedEventIfAllowed();
        else 
        {
            wxFAIL_MSG("Unexpected class for controlTextDidChange event");
        }
    }
}

//

#if OBJC_API_VERSION >= 2

#define wxOSX_CLASS_ADD_METHOD( c, s, i, t ) \
    class_addMethod(c, s, i, t );

#else

#define wxOSX_CLASS_ADD_METHOD( c, s, i, t ) \
    { s, (char*) t, i },

#endif

void wxOSXCocoaClassAddWXMethods(Class c, wxOSXSkipOverrides skipFlags)
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
    
    wxOSX_CLASS_ADD_METHOD(c, @selector(acceptsFirstMouse:), (IMP) wxOSX_acceptsFirstMouse, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(scrollWheel:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseEntered:), (IMP) wxOSX_mouseEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(mouseExited:), (IMP) wxOSX_mouseEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(magnifyWithEvent:), (IMP)wxOSX_mouseEvent, "v@:@")

    wxOSX_CLASS_ADD_METHOD(c, @selector(cursorUpdate:), (IMP) wxOSX_cursorUpdate, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(keyDown:), (IMP) wxOSX_keyEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(keyUp:), (IMP) wxOSX_keyEvent, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(flagsChanged:), (IMP) wxOSX_keyEvent, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(insertText:), (IMP) wxOSX_insertText, "v@:@" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(acceptsFirstResponder), (IMP) wxOSX_acceptsFirstResponder, "c@:" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(becomeFirstResponder), (IMP) wxOSX_becomeFirstResponder, "c@:" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(resignFirstResponder), (IMP) wxOSX_resignFirstResponder, "c@:" )

#if !wxOSX_USE_NATIVE_FLIPPED
    wxOSX_CLASS_ADD_METHOD(c, @selector(isFlipped), (IMP) wxOSX_isFlipped, "c@:" )
#endif
        
    if ( !(skipFlags & wxOSXSKIP_DRAW) )
        wxOSX_CLASS_ADD_METHOD(c, @selector(drawRect:), (IMP) wxOSX_drawRect, "v@:{_NSRect={_NSPoint=ff}{_NSSize=ff}}" )

    wxOSX_CLASS_ADD_METHOD(c, @selector(controlAction:), (IMP) wxOSX_controlAction, "v@:@" )
    wxOSX_CLASS_ADD_METHOD(c, @selector(controlDoubleAction:), (IMP) wxOSX_controlDoubleAction, "v@:@" )

#if wxUSE_DRAG_AND_DROP
    if ( !(skipFlags & wxOSXSKIP_DND) )
    {
        wxOSX_CLASS_ADD_METHOD(c, @selector(draggingEntered:), (IMP) wxOSX_draggingEntered, "I@:@" )
        wxOSX_CLASS_ADD_METHOD(c, @selector(draggingUpdated:), (IMP) wxOSX_draggingUpdated, "I@:@" )
        wxOSX_CLASS_ADD_METHOD(c, @selector(draggingExited:), (IMP) wxOSX_draggingExited, "v@:@" )
        wxOSX_CLASS_ADD_METHOD(c, @selector(performDragOperation:), (IMP) wxOSX_performDragOperation, "c@:@" )
    }
#endif

#if OBJC_API_VERSION < 2
    } ;
    static int method_count = WXSIZEOF( wxmethods );
    static objc_method_list *wxmethodlist = nullptr;
    if ( wxmethodlist == nullptr )
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

wxIMPLEMENT_DYNAMIC_CLASS(wxWidgetCocoaImpl , wxWidgetImpl);

wxWidgetCocoaImpl::wxWidgetCocoaImpl( wxWindowMac* peer , WXWidget w, int flags ) :
wxWidgetImpl( peer, flags )
{
    Init();
    m_osxView = w;
    m_osxClipView = nil;

    // check if the user wants to create the control initially hidden
    if ( !peer->IsShown() )
        SetVisibility(false);
    
    // gc aware handling
    if ( m_osxView )
        CFRetain(m_osxView);
    [m_osxView release];
    m_osxView.clipsToBounds = YES;
}


wxWidgetCocoaImpl::wxWidgetCocoaImpl()
{
    Init();
}

void wxWidgetCocoaImpl::Init()
{
    m_osxView = nullptr;
#if !wxOSX_USE_NATIVE_FLIPPED
    m_isFlipped = true;
#endif
    m_lastKeyDownEvent = nil;
    m_lastKeyDownWXSent = false;
    m_hasEditor = false;
}

wxWidgetCocoaImpl::~wxWidgetCocoaImpl()
{
    if ( GetWXPeer() && GetWXPeer()->IsFrozen() )
        SetDrawingEnabled(true);
    
    RemoveAssociations( this );

    if ( !IsRootControl() )
    {
        NSView *sv = [m_osxView superview];
        if ( sv != nil )
            [m_osxView removeFromSuperview];
    }
    // gc aware handling
    if ( m_osxView )
        CFRelease(m_osxView);

    wxCocoaGestures::EraseForObject(this);
}

void wxWidgetCocoaImpl::BeginNativeKeyDownEvent( NSEvent* event )
{
    m_lastKeyDownEvent = event;
    m_lastKeyDownWXSent = false;
}

void wxWidgetCocoaImpl::EndNativeKeyDownEvent()
{
    m_lastKeyDownEvent = nil;
    m_lastKeyDownWXSent = false;
}

bool wxWidgetCocoaImpl::IsInNativeKeyDown() const
{
    return m_lastKeyDownEvent != nil;
}

NSEvent* wxWidgetCocoaImpl::GetLastNativeKeyDownEvent()
{
    wxASSERT( m_lastKeyDownEvent != nil);
    return m_lastKeyDownEvent;
}

void wxWidgetCocoaImpl::SetKeyDownSent()
{
    wxASSERT( !m_lastKeyDownWXSent );
    m_lastKeyDownWXSent = true;
}

bool wxWidgetCocoaImpl::WasKeyDownSent() const
{
    return m_lastKeyDownWXSent;
}

wxWidgetCocoaNativeKeyDownSuspender::wxWidgetCocoaNativeKeyDownSuspender( wxWidgetCocoaImpl *target ) : m_target(target)
{
    m_nsevent = m_target->m_lastKeyDownEvent;
    m_wxsent = m_target->m_lastKeyDownWXSent;
    
    m_target->m_lastKeyDownEvent = nil;
}

wxWidgetCocoaNativeKeyDownSuspender::~wxWidgetCocoaNativeKeyDownSuspender()
{
    m_target->m_lastKeyDownEvent = m_nsevent;
    m_target->m_lastKeyDownWXSent = m_wxsent;
}

bool wxWidgetCocoaImpl::IsVisible() const
{
    return [m_osxView isHiddenOrHasHiddenAncestor] == NO;
}

void wxWidgetCocoaImpl::SetVisibility( bool visible )
{
    [m_osxView setHidden:(visible ? NO:YES)];
    
    // trigger redraw upon shown for layer-backed views
    if ( WX_IS_MACOS_AVAILABLE(10, 14 ) )
        if( !m_osxView.isHiddenOrHasHiddenAncestor )
            SetNeedsDisplay(nullptr);
}

double wxWidgetCocoaImpl::GetContentScaleFactor() const
{
    NSWindow* tlw = [m_osxView window];
    if ( tlw )
        return [tlw backingScaleFactor];
    else
        return wxOSXGetMainScreenContentScaleFactor();
}

// ----------------------------------------------------------------------------
// window animation stuff
// ----------------------------------------------------------------------------

// define a delegate used to refresh the window during animation
@interface wxNSAnimationDelegate : NSObject <NSAnimationDelegate>
{
    wxWindow *m_win;
    bool m_isDone;
}

- (id)init:(wxWindow *)win;

- (bool)isDone;

// NSAnimationDelegate methods
- (void)animationDidEnd:(NSAnimation*)animation;
- (void)animation:(NSAnimation*)animation
        didReachProgressMark:(NSAnimationProgress)progress;
@end

@implementation wxNSAnimationDelegate

- (id)init:(wxWindow *)win
{
    if ( self = [super init] )
    {
        m_win = win;
        m_isDone = false;
    }
    return self;
}

- (bool)isDone
{
    return m_isDone;
}

- (void)animation:(NSAnimation*)animation
        didReachProgressMark:(NSAnimationProgress)progress
{
    wxUnusedVar(animation);
    wxUnusedVar(progress);

    m_win->SendSizeEvent();
}

- (void)animationDidEnd:(NSAnimation*)animation
{
    wxUnusedVar(animation);
    m_isDone = true;
}

@end

/* static */
bool
wxWidgetCocoaImpl::ShowViewOrWindowWithEffect(wxWindow *win,
                                              bool show,
                                              wxShowEffect effect,
                                              unsigned timeout)
{
    // create the dictionary describing the animation to perform on this view
    NSObject * const
        viewOrWin = static_cast<NSObject *>(win->OSXGetViewOrWindow());
    NSMutableDictionary * const
        dict = [NSMutableDictionary dictionaryWithCapacity:4];
    [dict setObject:viewOrWin forKey:NSViewAnimationTargetKey];

    // determine the start and end rectangles assuming we're hiding the window
    const wxRect rectOrig = win->GetRect();
    wxRect rectStart,
           rectEnd;
    rectStart =
    rectEnd = rectOrig;

    if ( show )
    {
        if ( effect == wxSHOW_EFFECT_ROLL_TO_LEFT ||
                effect == wxSHOW_EFFECT_SLIDE_TO_LEFT )
            effect = wxSHOW_EFFECT_ROLL_TO_RIGHT;
        else if ( effect == wxSHOW_EFFECT_ROLL_TO_RIGHT ||
                    effect == wxSHOW_EFFECT_SLIDE_TO_RIGHT )
            effect = wxSHOW_EFFECT_ROLL_TO_LEFT;
        else if ( effect == wxSHOW_EFFECT_ROLL_TO_TOP ||
                    effect == wxSHOW_EFFECT_SLIDE_TO_TOP )
            effect = wxSHOW_EFFECT_ROLL_TO_BOTTOM;
        else if ( effect == wxSHOW_EFFECT_ROLL_TO_BOTTOM ||
                    effect == wxSHOW_EFFECT_SLIDE_TO_BOTTOM )
            effect = wxSHOW_EFFECT_ROLL_TO_TOP;
    }

    switch ( effect )
    {
        case wxSHOW_EFFECT_ROLL_TO_LEFT:
        case wxSHOW_EFFECT_SLIDE_TO_LEFT:
            rectEnd.width = 0;
            break;

        case wxSHOW_EFFECT_ROLL_TO_RIGHT:
        case wxSHOW_EFFECT_SLIDE_TO_RIGHT:
            rectEnd.x = rectStart.GetRight();
            rectEnd.width = 0;
            break;

        case wxSHOW_EFFECT_ROLL_TO_TOP:
        case wxSHOW_EFFECT_SLIDE_TO_TOP:
            rectEnd.height = 0;
            break;

        case wxSHOW_EFFECT_ROLL_TO_BOTTOM:
        case wxSHOW_EFFECT_SLIDE_TO_BOTTOM:
            rectEnd.y = rectStart.GetBottom();
            rectEnd.height = 0;
            break;

        case wxSHOW_EFFECT_EXPAND:
            rectEnd.x = rectStart.x + rectStart.width / 2;
            rectEnd.y = rectStart.y + rectStart.height / 2;
            rectEnd.width =
            rectEnd.height = 0;
            break;

        case wxSHOW_EFFECT_BLEND:
            [dict setObject:(show ? NSViewAnimationFadeInEffect
                                  : NSViewAnimationFadeOutEffect)
                  forKey:NSViewAnimationEffectKey];
            break;

        case wxSHOW_EFFECT_NONE:
        case wxSHOW_EFFECT_MAX:
            wxFAIL_MSG( "unexpected animation effect" );
            return false;

        default:
            wxFAIL_MSG( "unknown animation effect" );
            return false;
    };

    if ( show )
    {
        // we need to restore it to the original rectangle instead of making it
        // disappear
        wxSwap(rectStart, rectEnd);

        // and as the window is currently hidden, we need to show it for the
        // animation to be visible at all (but don't restore it at its full
        // rectangle as it shouldn't appear immediately)
        win->SetSize(rectStart);
        win->Show();
    }

    NSView * const parentView = [viewOrWin isKindOfClass:[NSView class]]
                                    ? [(NSView *)viewOrWin superview]
                                    : nil;
    const NSRect rStart = wxToNSRect(parentView, rectStart);
    const NSRect rEnd = wxToNSRect(parentView, rectEnd);

    [dict setObject:[NSValue valueWithRect:rStart]
          forKey:NSViewAnimationStartFrameKey];
    [dict setObject:[NSValue valueWithRect:rEnd]
          forKey:NSViewAnimationEndFrameKey];

    // create an animation using the values in the above dictionary
    NSViewAnimation * const
        anim = [[NSViewAnimation alloc]
                initWithViewAnimations:[NSArray arrayWithObject:dict]];

    if ( !timeout )
    {
        // what is a good default duration? Windows uses 200ms, Web frameworks
        // use anything from 250ms to 1s... choose something in the middle
        timeout = 200;
    }

    [anim setDuration:timeout/1000.];   // duration is in seconds here

    // if the window being animated changes its layout depending on its size
    // (which is almost always the case) we need to redo it during animation
    //
    // the number of layouts here is arbitrary, but 10 seems like too few (e.g.
    // controls in wxInfoBar visibly jump around)
    const int NUM_LAYOUTS = 20;
    for ( float f = 1./NUM_LAYOUTS; f < 1.; f += 1./NUM_LAYOUTS )
        [anim addProgressMark:f];

    wxNSAnimationDelegate * const
        animDelegate = [[wxNSAnimationDelegate alloc] init:win];
    [anim setDelegate:animDelegate];
    [anim startAnimation];

    // Cocoa is capable of doing animation asynchronously or even from separate
    // thread but wx API doesn't provide any way to be notified about the
    // animation end and without this we really must ensure that the window has
    // the expected (i.e. the same as if a simple Show() had been used) size
    // when we return, so block here until the animation finishes
    //
    // notice that because the default animation mode is NSAnimationBlocking,
    // no user input events ought to be processed from here
    {
        wxEventLoopGuarantor ensureEventLoopExistence;
        wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
        while ( ![animDelegate isDone] )
            loop->Dispatch();
    }

    if ( !show )
    {
        // NSViewAnimation is smart enough to hide the NSView being animated at
        // the end but we also must ensure that it's hidden for wx too
        win->Hide();

        // and we must also restore its size because it isn't expected to
        // change just because the window was hidden
        win->SetSize(rectOrig);
    }
    else
    {
        // refresh it once again after the end to ensure that everything is in
        // place
        win->SendSizeEvent();
    }

    [anim setDelegate:nil];
    [animDelegate release];
    [anim release];

    return true;
}

bool wxWidgetCocoaImpl::ShowWithEffect(bool show,
                                       wxShowEffect effect,
                                       unsigned timeout)
{
    return ShowViewOrWindowWithEffect(m_wxPeer, show, effect, timeout);
}

typedef __kindof NSView* KindOfView;

class CocoaWindowCompareContext
{
    wxDECLARE_NO_COPY_CLASS(CocoaWindowCompareContext);
public:
    CocoaWindowCompareContext(); // Not implemented
    CocoaWindowCompareContext(NSView *target, NSArray *subviews)
    {
        m_target = target;
        // Cocoa sorts subviews in-place.. make a copy
        m_subviews = [subviews copy];
    }

    ~CocoaWindowCompareContext()
    {   // release the copy
        [m_subviews release];
    }
    NSView* target()
    {   return m_target; }

    NSArray* subviews()
    {   return m_subviews; }

    /* Helper function that returns the comparison based off of the original ordering */
    NSComparisonResult CompareUsingOriginalOrdering(KindOfView first,
            KindOfView second)
    {
        NSUInteger firstI = [m_subviews indexOfObjectIdenticalTo:first];
        NSUInteger secondI = [m_subviews indexOfObjectIdenticalTo:second];
        // NOTE: If either firstI or secondI is NSNotFound then it will be NSIntegerMax and thus will
        // likely compare higher than the other view which is reasonable considering the only way that
        // can happen is if the subview was added after our call to subviews but before the call to
        // sortSubviewsUsingFunction:context:.  Thus we don't bother checking.  Particularly because
        // that case should never occur anyway because that would imply a multi-threaded GUI call
        // which is a big no-no with Cocoa.

        // Subviews are ordered from back to front meaning one that is already lower will have an lower index.
        NSComparisonResult result = (firstI < secondI)
        ?   NSOrderedAscending /* -1 */
        :   (firstI > secondI)
        ?   NSOrderedDescending /* 1 */
        :   NSOrderedSame /* 0 */;

        return result;
    }
private:
    /* The subview we are trying to Raise or Lower */
    NSView *m_target;
    /* A copy of the original array of subviews */
    NSArray *m_subviews;
};

/* Causes Cocoa to raise the target view to the top of the Z-Order by telling the sort function that
 * the target view is always higher than every other view.  When comparing two views neither of
 * which is the target, it returns the correct response based on the original ordering
 */
static NSComparisonResult CocoaRaiseWindowCompareFunction(KindOfView first, KindOfView second, void *ctx)
{
    CocoaWindowCompareContext *compareContext = (CocoaWindowCompareContext*)ctx;
    // first should be ordered higher
    if(first==compareContext->target())
        return NSOrderedDescending;
    // second should be ordered higher
    if(second==compareContext->target())
        return NSOrderedAscending;
    return compareContext->CompareUsingOriginalOrdering(first,second);
}

void wxWidgetCocoaImpl::Raise()
{
    NSView* nsview = m_osxView;

    NSView *superview = [nsview superview];
    CocoaWindowCompareContext compareContext(nsview, [superview subviews]);

    [superview sortSubviewsUsingFunction: CocoaRaiseWindowCompareFunction
                                 context: &compareContext];

}

/* Causes Cocoa to lower the target view to the bottom of the Z-Order by telling the sort function that
 * the target view is always lower than every other view.  When comparing two views neither of
 * which is the target, it returns the correct response based on the original ordering
 */
static NSComparisonResult CocoaLowerWindowCompareFunction(KindOfView first, KindOfView second, void *ctx)
{
    CocoaWindowCompareContext *compareContext = (CocoaWindowCompareContext*)ctx;
    // first should be ordered lower
    if(first==compareContext->target())
        return NSOrderedAscending;
    // second should be ordered lower
    if(second==compareContext->target())
        return NSOrderedDescending;
    return compareContext->CompareUsingOriginalOrdering(first,second);
}

void wxWidgetCocoaImpl::Lower()
{
    NSView* nsview = m_osxView;

    NSView *superview = [nsview superview];
    CocoaWindowCompareContext compareContext(nsview, [superview subviews]);

    [superview sortSubviewsUsingFunction: CocoaLowerWindowCompareFunction
                                 context: &compareContext];
}

void wxWidgetCocoaImpl::ScrollRect( const wxRect *WXUNUSED(rect), int WXUNUSED(dx), int WXUNUSED(dy) )
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
    if (parent && [m_osxView superview] != parent->GetHandle() )
    {
        int cx = 0,cy = 0,cw = 0,ch = 0;
        if ( parent->GetPeer() )
        {
            parent->GetPeer()->GetContentArea(cx, cy, cw, ch);
            x -= cx;
            y -= cy;
        }
    }
    [[m_osxView superview] setNeedsDisplayInRect:[m_osxView frame]];
    NSRect r = wxToNSRect( [m_osxView superview], wxRect(x,y,width, height) );
    [m_osxView setFrame:r];
    [[m_osxView superview] setNeedsDisplayInRect:r];
}

void wxWidgetCocoaImpl::GetPosition( int &x, int &y ) const
{
    wxRect r = wxFromNSRect( [m_osxView superview], [m_osxView frame] );
    x = r.GetLeft();
    y = r.GetTop();
    
    // under Cocoa we might have a contentView in the wxParent to which we have to
    // adjust the coordinates
    wxWindowMac* parent = GetWXPeer()->GetParent();
    if (parent && [m_osxView superview] != parent->GetHandle() )
    {
        int cx = 0,cy = 0,cw = 0,ch = 0;
        if ( parent->GetPeer() )
        {
            parent->GetPeer()->GetContentArea(cx, cy, cw, ch);
            x += cx;
            y += cy;
        }
    }
}

void wxWidgetCocoaImpl::GetSize( int &width, int &height ) const
{
    NSRect rect = [m_osxView frame];
    width = (int)rect.size.width;
    height = (int)rect.size.height;
}

void wxWidgetCocoaImpl::GetContentArea( int&left, int &top, int &width, int &height ) const
{
    if ( [m_osxView respondsToSelector:@selector(contentView) ] && [m_osxView contentView] )
    {
        NSView* cv = [m_osxView contentView];

        NSRect bounds = [m_osxView bounds];
        NSRect rect = [cv frame];

        int y = (int)rect.origin.y;
        int x = (int)rect.origin.x;
        if ( ![ m_osxView isFlipped ] )
            y = (int)(bounds.size.height - (rect.origin.y + rect.size.height));
        left = x;
        top = y;
        width = (int)rect.size.width;
        height = (int)rect.size.height;
    }
    else
    {
        left = top = 0;
        GetSize( width, height );

        int leftinset, topinset, rightinset, bottominset;
        GetLayoutInset( leftinset, topinset, rightinset, bottominset);
        left += leftinset;
        top += topinset;
        width -= leftinset + rightinset;
        height -= topinset + bottominset;
    }
}

void wxWidgetCocoaImpl::GetLayoutInset(int &left , int &top , int &right, int &bottom) const
{
    NSEdgeInsets insets = [m_osxView alignmentRectInsets];
    left = insets.left;
    top = insets.top;
    right = insets.right;
    bottom = insets.bottom;
}

namespace
{
    
void SetSubviewsNeedDisplay( NSView *view )
{
    for ( NSView *sub in view.subviews )
    {
        if ( sub.isHidden )
            continue;

        [sub setNeedsDisplay:YES];
        SetSubviewsNeedDisplay(sub);
    }
}

void SetSubviewsNeedDisplay( NSView *view, NSRect rect )
{
    for ( NSView *sub in view.subviews )
    {
        if ( sub.isHidden )
            continue;

        NSRect intersect = NSIntersectionRect(sub.frame, rect);
        if ( !NSIsEmptyRect(intersect) )
        {
            intersect = [view convertRect:intersect toView:sub];
            [sub setNeedsDisplayInRect:intersect];
            SetSubviewsNeedDisplay(sub, intersect);
        }
    }
}

}


void wxWidgetCocoaImpl::SetNeedsDisplay( const wxRect* where )
{
    wxMacAutoreleasePool autoreleasepool;

    if ( where )
        [m_osxView setNeedsDisplayInRect:wxToNSRect(m_osxView, *where )];
    else
        [m_osxView setNeedsDisplay:YES];

    // Layer-backed views (which are all in Mojave's Dark Mode) may not have
    // their children implicitly redrawn with the parent. For compatibility,
    // do it manually here:
    if ( WX_IS_MACOS_AVAILABLE(10, 14 ) )
    {
        if ( where )
            SetSubviewsNeedDisplay(m_osxView, wxToNSRect(m_osxView, *where ));
        else
            SetSubviewsNeedDisplay(m_osxView);
    }
}

bool wxWidgetCocoaImpl::GetNeedsDisplay() const
{
    return [m_osxView needsDisplay];
}

bool wxWidgetCocoaImpl::CanFocus() const
{
    if ( !IsVisible() )
    {
        // canBecomeKeyView always returns false for hidden windows, but this
        // could be wrong because the window could still accept focus once it
        // becomes visible, so we have no choice but to return true here to
        // avoid situations in which the expected window doesn't get the focus.
        return true;
    }

    NSView* targetView = m_osxView;
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        targetView = [(NSScrollView*) m_osxView documentView];
    const bool canFocus = [targetView canBecomeKeyView] == YES;
    wxLogTrace(TRACE_FOCUS, "CanFocus(%s) -> %s",
               wxDumpNSView(m_osxView), canFocus ? "YES" : "NO");
    return canFocus;
}

@interface wxNSClipView : NSClipView

@end

@implementation wxNSClipView

#if wxOSX_USE_NATIVE_FLIPPED
- (BOOL)isFlipped
{
    return YES;
}
#endif

@end

bool wxWidgetCocoaImpl::HasFocus() const
{
    NSView* targetView = m_osxView;
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        targetView = [(NSScrollView*) m_osxView documentView];
    return ( FindFocus() == targetView );
}

bool wxWidgetCocoaImpl::SetFocus()
{
    NSView* targetView = m_osxView;
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        targetView = [(NSScrollView*) m_osxView documentView];

    if ( [targetView canBecomeKeyView] )
    {
        wxLogTrace(TRACE_FOCUS, "Setting focus to %s", wxDumpNSView(m_osxView));

        [[m_osxView window] makeFirstResponder: targetView] ;
    }
    else // can't become key view
    {
        // This most commonly happens because the window is still hidden, as
        // canBecomeKeyView: always returns NO in this case, so schedule this
        // window to become focused when it's shown later in this case.
        //
        // Note that this may still fail to work: if full keyboard access is
        // off and this window is not a text control or similar, setting
        // initial first responder won't do anything. But this is not really a
        // problem and at least it will do the right thing if the window turns
        // out to be focusable (i.e. it's a text control or full keyboard
        // access is on).
        if ( !IsVisible() )
        {
            wxLogTrace(TRACE_FOCUS, "Setting initial focus to %s",
                       wxDumpNSView(m_osxView));

            [[m_osxView window] setInitialFirstResponder: targetView] ;
        }
        else // window is shown but doesn't accept focus
        {
            // Not sure when exactly can this happen, for now just don't do
            // anything in this case.
            wxLogTrace(TRACE_FOCUS, "Not setting focus to %s",
                       wxDumpNSView(m_osxView));
            return false;
        }
    }

    return true;
}

#if wxUSE_DRAG_AND_DROP
void wxWidgetCocoaImpl::SetDropTarget(wxDropTarget* target)
{
    [m_osxView unregisterDraggedTypes];

    if (target == nullptr)
        return;

    wxDataObject* dobj = target->GetDataObject();

    if (dobj)
    {
        wxCFMutableArrayRef<CFStringRef> typesarray;
        /*
            In macOS the view controls the types we can drop onto it, so we have to collect
            all formats that can be put into dataObject instead of only ones that can be get.
        */
        dobj->AddSupportedTypes(typesarray, wxDataObjectBase::Direction::Set);
        NSView* targetView = m_osxView;
        if ([m_osxView isKindOfClass:[NSScrollView class]])
            targetView = [(NSScrollView*)m_osxView documentView];

        [targetView registerForDraggedTypes:typesarray];
    }
}
#endif // wxUSE_DRAG_AND_DROP

void wxWidgetCocoaImpl::RemoveFromParent()
{
    // User panes will be thawed in the removeFromSuperview call below
    if (!IsUserPane() && m_wxPeer->IsFrozen())
        SetDrawingEnabled(true);

    [m_osxView removeFromSuperview];
}

void wxWidgetCocoaImpl::Embed( wxWidgetImpl *parent )
{
    NSView* container = nil;

    if ( m_wxPeer->MacIsWindowScrollbar( parent->GetWXPeer()))
        container = parent->GetWXWidget();
    else
        container = parent->GetContainer();

    wxASSERT_MSG( container != nullptr , wxT("No valid mac container control") ) ;
    [container addSubview:m_osxView];
    
    // User panes will be frozen elsewhere
    if( m_wxPeer->IsFrozen() && !IsUserPane() )
        SetDrawingEnabled(false);
}

void wxWidgetCocoaImpl::SetBackgroundColour( const wxColour &col )
{
    NSView* targetView = m_osxView;
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        targetView = [(NSScrollView*) m_osxView documentView];

    if ( [targetView respondsToSelector:@selector(setBackgroundColor:) ] )
    {
        wxWindow* peer = GetWXPeer();
        if ( peer->GetBackgroundStyle() != wxBG_STYLE_TRANSPARENT )
        {
            wxNonOwnedWindow* toplevel = dynamic_cast<wxNonOwnedWindow*>(peer);

            if ( toplevel == nullptr || toplevel->GetShape().IsEmpty() )
                [targetView setBackgroundColor:
                        col.IsOk() ? col.OSXGetNSColor() : nil];
        }
    }
}

bool wxWidgetCocoaImpl::SetBackgroundStyle( wxBackgroundStyle style )
{
    BOOL opaque = ( style == wxBG_STYLE_PAINT );
    
    if ( [m_osxView respondsToSelector:@selector(setOpaque:) ] )
    {
        [m_osxView setOpaque: opaque];
        if ( style == wxBG_STYLE_TRANSPARENT )
            [m_osxView setBackgroundColor:[NSColor clearColor]];
    }
    
    return true ;
}

void wxWidgetCocoaImpl::SetLabel( const wxString& title )
{
    if ( [m_osxView respondsToSelector:@selector(setAttributedTitle:) ] )
    {
        wxFont f = GetWXPeer()->GetFont();
        // we should not override system font colors unless explicitly specified
        wxColour col = GetWXPeer()->UseForegroundColour() ? GetWXPeer()->GetForegroundColour() : wxNullColour;
        if ( f.GetStrikethrough() || f.GetUnderlined() || col.IsOk() )
        {
            wxCFStringRef cf(title );

            NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc]
                                                     initWithString:cf.AsNSString()];

            [attrString beginEditing];

            NSTextAlignment textAlign;
            if ( [m_osxView isKindOfClass:[NSControl class]] )
                textAlign = [(id)m_osxView alignment];
            else
                textAlign = NSCenterTextAlignment;
            [attrString setAlignment:textAlign
                               range:NSMakeRange(0, [attrString length])];

            [attrString addAttribute:NSFontAttributeName
                               value:f.OSXGetNSFont()
                               range:NSMakeRange(0, [attrString length])];
            if ( f.GetStrikethrough() )
            {
                [attrString addAttribute:NSStrikethroughStyleAttributeName
                                   value:@(NSUnderlineStyleSingle)
                                   range:NSMakeRange(0, [attrString length])];
            }

            if ( f.GetUnderlined() )
            {
                [attrString addAttribute:NSUnderlineStyleAttributeName
                                   value:@(NSUnderlineStyleSingle)
                                   range:NSMakeRange(0, [attrString length])];

            }

            if ( col.IsOk() )
            {
                [attrString addAttribute:NSForegroundColorAttributeName
                                   value:col.OSXGetNSColor()
                                   range:NSMakeRange(0, [attrString length])];
            }

            [attrString endEditing];

            [(id)m_osxView setAttributedTitle:attrString];
            
            [attrString release];

            return;
        }
    }

    if ( [m_osxView respondsToSelector:@selector(setTitle:) ] )
    {
        wxCFStringRef cf( title );
        [m_osxView setTitle:cf.AsNSString()];
    }
    else if ( [m_osxView respondsToSelector:@selector(setStringValue:) ] )
    {
        wxCFStringRef cf( title );
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
    if (  [m_osxView respondsToSelector:@selector(minValue)] )
    {
        return (int)[m_osxView minValue];
    }
    return 0;
}

void wxWidgetCocoaImpl::SetIncrement(int value)
{
    if (  [m_osxView respondsToSelector:@selector(setIncrement:)] )
    {
        [m_osxView setIncrement:value];
    }
}

int wxWidgetCocoaImpl::GetIncrement() const
{
    if(  [m_osxView respondsToSelector:@selector(increment)] )
    {
        return (int) [m_osxView increment];
    }
    return 0;
}

wxInt32 wxWidgetCocoaImpl::GetMaximum() const
{
    if (  [m_osxView respondsToSelector:@selector(maxValue)] )
    {
        return (int)[m_osxView maxValue];
    }
    return 0;
}

wxBitmap wxWidgetCocoaImpl::GetBitmap() const
{
    wxBitmap bmp;

    // TODO: how to create a wxBitmap from NSImage?
#if 0
    if ( [m_osxView respondsToSelector:@selector(image:)] )
        bmp = [m_osxView image];
#endif

    return bmp;
}

void wxWidgetCocoaImpl::SetBitmap( const wxBitmapBundle& bitmap )
{
    if (  [m_osxView respondsToSelector:@selector(setImage:)] )
    {
        [m_osxView setImage: wxOSXGetImageFromBundle(bitmap)];
        [m_osxView setNeedsDisplay:YES];
    }
}

void wxWidgetCocoaImpl::SetBitmapPosition( wxDirection dir )
{
    if ( [m_osxView respondsToSelector:@selector(setImagePosition:)] )
    {
        NSCellImagePosition pos;
        switch ( dir )
        {
            case wxLEFT:
                pos = NSImageLeft;
                break;

            case wxRIGHT:
                pos = NSImageRight;
                break;

            case wxTOP:
                pos = NSImageAbove;
                break;

            case wxBOTTOM:
                pos = NSImageBelow;
                break;

            default:
                wxFAIL_MSG( "invalid image position" );
                pos = NSNoImage;
        }

        [m_osxView setImagePosition:pos];
    }
}

void wxWidgetCocoaImpl::SetupTabs( const wxNotebook& WXUNUSED(notebook))
{
    // implementation in subclass
}

void wxWidgetCocoaImpl::GetBestRect( wxRect *r ) const
{
    r->x = r->y = r->width = r->height = 0;

    if (  [m_osxView respondsToSelector:@selector(sizeToFit)] )
    {
        NSRect former = [m_osxView frame];
        [m_osxView sizeToFit];
        NSRect best = [m_osxView frame];
        [m_osxView setFrame:former];
        r->width = (int)ceil(best.size.width);
        r->height = (int)ceil(best.size.height);
    }
}

bool wxWidgetCocoaImpl::IsEnabled() const
{
    NSView* targetView = m_osxView;
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        targetView = [(NSScrollView*) m_osxView documentView];

    if ( [targetView respondsToSelector:@selector(isEnabled) ] )
        return [targetView isEnabled];
    return true;
}

void wxWidgetCocoaImpl::Enable( bool enable )
{
    NSView* targetView = m_osxView;
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        targetView = [(NSScrollView*) m_osxView documentView];

    if ( [targetView respondsToSelector:@selector(setEnabled:) ] )
        [targetView setEnabled:enable];

    if ( !enable && HasFocus() )
        m_wxPeer->Navigate();
}

void wxWidgetCocoaImpl::PulseGauge()
{
}

void wxWidgetCocoaImpl::SetScrollThumb( wxInt32 WXUNUSED(val), wxInt32 WXUNUSED(view) )
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
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_16
            if ( WX_IS_MACOS_AVAILABLE( 10, 16 ))
                size = NSControlSizeLarge;
            else
#endif
            size = NSRegularControlSize;
            break ;

        default:
            wxFAIL_MSG(wxT("unexpected window variant"));
            break ;
    }
    if ( [m_osxView respondsToSelector:@selector(setControlSize:)] )
        [m_osxView setControlSize:size];
    else if ([m_osxView respondsToSelector:@selector(cell)])
    {
        id cell = [(id)m_osxView cell];
        if ([cell respondsToSelector:@selector(setControlSize:)])
            [cell setControlSize:size];
    }

    // we need to propagate this to inner views as well
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
    {
        NSView* targetView = [(NSScrollView*) m_osxView documentView];
    
        if ( [targetView respondsToSelector:@selector(setControlSize:)] )
            [targetView setControlSize:size];
        else if ([targetView respondsToSelector:@selector(cell)])
        {
            id cell = [(id)targetView cell];
            if ([cell respondsToSelector:@selector(setControlSize:)])
                [cell setControlSize:size];
        }
    }
}

NSView* wxWidgetCocoaImpl::GetViewWithText() const
{
    if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
        return [(NSScrollView*) m_osxView documentView];
    else if ( [m_osxView isKindOfClass:[NSBox class] ] )
        return [(NSBox*) m_osxView titleCell];

    return m_osxView;
}

void wxWidgetCocoaImpl::SetFont(wxFont const& font)
{
    NSView* const targetView = GetViewWithText();

    if ([targetView respondsToSelector:@selector(setFont:)])
        [targetView setFont: font.OSXGetNSFont()];
    if ([m_osxView respondsToSelector:@selector(setAttributedTitle:)])
        SetLabel(wxStripMenuCodes(GetWXPeer()->GetLabel(), wxStrip_Mnemonics));
}

void wxWidgetCocoaImpl::SetForegroundColour(const wxColour& col)
{
    NSView* const targetView = GetViewWithText();

    if ([targetView respondsToSelector:@selector(setTextColor:)])
    {
        [targetView setTextColor: col.IsOk() ? col.OSXGetNSColor() : nil];
    }
}

void wxWidgetCocoaImpl::SetToolTip(wxToolTip* tooltip)
{
    if ( tooltip )
    {
        wxCFStringRef cf( tooltip->GetTip() );
        [m_osxView setToolTip: cf.AsNSString()];
    }
    else 
    {
        [m_osxView setToolTip:nil];
    }
}

void wxWidgetCocoaImpl::InstallEventHandler( WXWidget control )
{
    WXWidget c =  control ? control : (WXWidget) m_osxView;
    wxWidgetImpl::Associate( c, this ) ;
    if ([c respondsToSelector:@selector(setAction:)])
    {
        [c setTarget: c];
        if ( dynamic_cast<wxRadioButton*>(GetWXPeer()) )
        {
            // everything already set up
        }
        else
            [c setAction: @selector(controlAction:)];
        
        if ([c respondsToSelector:@selector(setDoubleAction:)])
        {
            [c setDoubleAction: @selector(controlDoubleAction:)];
        }

    }
    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited|NSTrackingCursorUpdate|NSTrackingMouseMoved|NSTrackingActiveAlways|NSTrackingInVisibleRect;
    NSTrackingArea* area = [[NSTrackingArea alloc] initWithRect: NSZeroRect options: options owner: m_osxView userInfo: nil];
    [m_osxView addTrackingArea: area];
    [area release];
}

bool wxWidgetCocoaImpl::EnableTouchEvents(int eventsMask)
{
    if ( HasUserMouseHandling() )
    {
        if ( eventsMask == wxTOUCH_NONE )
        {
            if ( wxCocoaGestures::EraseForObject(this) )
            {
                [m_osxView setAcceptsTouchEvents:NO];
            }
            //else: we didn't have any gesture data anyhow
        }
        else // We do want to have gesture events.
        {
            // clang does not see that the owning object always destroys its extra field
#ifndef __clang_analyzer__
            wxCocoaGestures::StoreForObject
            (
                this,
                new wxCocoaGesturesImpl(this, m_osxView, eventsMask)
            );
#endif
            
            [m_osxView setAcceptsTouchEvents:YES];
        }

        return true;
    }

    wxUnusedVar(eventsMask);
    return false;
}

bool wxWidgetCocoaImpl::DoHandleCharEvent(NSEvent *event, NSString *text)
{
    bool result = false;
    wxWindowMac* peer = GetWXPeer();
    int length = [text length];
    if ( peer )
    {
        const wxString str = wxCFStringRef([text retain]).AsString();
        for ( wxString::const_iterator it = str.begin();
              it != str.end();
              ++it )
        {
            wxKeyEvent wxevent(wxEVT_CHAR);
            
            // if we have exactly one character resulting from the event, then
            // set the corresponding modifiers and raw data from the nsevent
            // otherwise leave these at defaults, as they probably would be incorrect
            // anyway (IME input)
            
            if ( event != nil && length == 1)
            {
                SetupKeyEvent(wxevent,event,text);
            }
            else
            {
                wxevent.m_shiftDown = wxevent.m_controlDown = wxevent.m_altDown = wxevent.m_metaDown = false;
                wxevent.m_rawCode = 0;
                wxevent.m_rawFlags = 0;

                const wxChar aunichar = *it;
                wxevent.m_uniChar = aunichar;
                wxevent.m_keyCode = aunichar < 0x80 ? aunichar : WXK_NONE;
                
                wxevent.SetEventObject(peer);
                wxevent.SetId(peer->GetId());

                if ( event )
                    wxevent.SetTimestamp( (int)([event timestamp] * 1000) ) ;
            }
            
            result = peer->OSXHandleKeyEvent(wxevent) || result;
        }
    }
    return result;
}

bool wxWidgetCocoaImpl::ShouldHandleKeyNavigation(const wxKeyEvent &WXUNUSED(event)) const
{
    // If the window wants to have all keys, let it have it and don't process
    // TAB as key navigation event.
    return !m_wxPeer->HasFlag(wxWANTS_CHARS);
}

bool wxWidgetCocoaImpl::DoHandleKeyNavigation(const wxKeyEvent &event)
{
    wxLogTrace(TRACE_KEYS, "Handling key navigation event for %s",
               wxDumpNSView(m_osxView));

    bool handled = false;
    wxWindow *focus = GetWXPeer();
    if (focus && event.GetKeyCode() == WXK_TAB)
    {
        if (ShouldHandleKeyNavigation(event))
        {
            wxWindow* iter = focus->GetParent() ;
            while (iter && !handled)
            {
                if (iter->HasFlag(wxTAB_TRAVERSAL))
                {
                    wxNavigationKeyEvent new_event;
                    new_event.SetEventObject( focus );
                    new_event.SetDirection( !event.ShiftDown() );
                    /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
                    new_event.SetWindowChange( event.ControlDown() );
                    new_event.SetCurrentFocus( focus );
                    handled = iter->HandleWindowEvent( new_event );
                }

                iter = iter->GetParent() ;
            }
        }
    }
    return handled;
}

bool wxWidgetCocoaImpl::DoHandleKeyEvent(NSEvent *event)
{
    wxLogTrace(TRACE_KEYS, "Handling key event for %s",
               wxDumpNSView(m_osxView));

    wxKeyEvent wxevent(wxEVT_KEY_DOWN);
    SetupKeyEvent( wxevent, event );

    // Generate wxEVT_CHAR_HOOK before sending any other events but only when
    // the key is pressed, not when it's released (the type of wxevent is
    // changed by SetupKeyEvent() so it can be wxEVT_KEY_UP too by now).
    if ( wxevent.GetEventType() == wxEVT_KEY_DOWN )
    {
        wxKeyEvent eventHook(wxEVT_CHAR_HOOK, wxevent);
        if ( GetWXPeer()->OSXHandleKeyEvent(eventHook)
                && !eventHook.IsNextEventAllowed() )
        {
            wxLogTrace(TRACE_KEYS, "Key down event handled");
            return true;
        }

        if (DoHandleKeyNavigation(wxevent))
        {
            wxLogTrace(TRACE_KEYS, "Key down event handled as navigation event");
            return true;
        }
    }

    if ( HasUserKeyHandling() && [event type] == NSKeyDown)
    {
        // Don't fire wxEVT_KEY_DOWN here in order to allow IME to intercept
        // some key events. If the event is not handled by IME, either
        // insertText: or doCommandBySelector: is called, so we send
        // wxEVT_KEY_DOWN and wxEVT_CHAR there.

        if ( [m_osxView isKindOfClass:[NSScrollView class] ] )
            [[(NSScrollView*)m_osxView documentView] interpretKeyEvents:[NSArray arrayWithObject:event]];
        else
            [m_osxView interpretKeyEvents:[NSArray arrayWithObject:event]];

        if ( IsInNativeKeyDown() && !WasKeyDownSent())
        {
            wxLogTrace(TRACE_KEYS, "Emulating missing key down event");

            wxKeyEvent wxevent(wxEVT_KEY_DOWN);
            SetupKeyEvent( wxevent, GetLastNativeKeyDownEvent() );
            GetWXPeer()->OSXHandleKeyEvent(wxevent);
        }
        return true;
    }
    else
    {
        return GetWXPeer()->OSXHandleKeyEvent(wxevent);
    }
}

bool wxWidgetCocoaImpl::DoHandleMouseEvent(NSEvent *event)
{
    // Call this before handling the event in case the event handler destroys
    // this window.
    (void)SetupCursor(event);

    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
    SetupMouseEvent(wxevent , event) ;
    return GetWXPeer()->HandleWindowEvent(wxevent);
}

void wxWidgetCocoaImpl::DoNotifyFocusSet()
{
    NSResponder* responder = wxNonOwnedWindowCocoaImpl::GetFormerFirstResponder();
    NSView* otherView = wxOSXGetViewFromResponder(responder);
    wxWidgetImpl* otherWindow = FindFromWXWidget(otherView);
    
    // It doesn't make sense to notify about the focus set if it's the same
    // control in the end, and just a different subview
    if ( otherWindow != this )
        DoNotifyFocusEvent(true, otherWindow);
}

void wxWidgetCocoaImpl::DoNotifyFocusLost()
{
    NSResponder * responder = wxNonOwnedWindowCocoaImpl::GetNextFirstResponder();
    NSView* otherView = wxOSXGetViewFromResponder(responder);
    wxWidgetImpl* otherWindow = FindBestFromWXWidget(otherView);
    
    // It doesn't make sense to notify about the loss of focus if it's the same
    // control in the end, and just a different subview
    if ( otherWindow != this )
        DoNotifyFocusEvent( false, otherWindow );
}

void wxWidgetCocoaImpl::DoNotifyFocusEvent(bool receivedFocus, wxWidgetImpl* otherWindow)
{
    wxWindow* thisWindow = GetWXPeer();

    if ( receivedFocus )
    {
        wxLogTrace(TRACE_FOCUS, "Set focus for %s", wxDumpWindow(thisWindow));
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
    else // !receivedFocus
    {
#if wxUSE_CARET
        if ( thisWindow->GetCaret() )
            thisWindow->GetCaret()->OnKillFocus();
#endif

        wxLogTrace(TRACE_FOCUS, "Lost focus in %s", wxDumpWindow(thisWindow));

        wxFocusEvent event( wxEVT_KILL_FOCUS, thisWindow->GetId());
        event.SetEventObject(thisWindow);
        if (otherWindow)
            event.SetWindow(otherWindow->GetWXPeer());
        thisWindow->HandleWindowEvent(event) ;
    }
}

void wxWidgetCocoaImpl::SetCursor(const wxCursor& cursor)
{
    if ( !wxIsBusy() )
    {
        NSRect location = NSZeroRect;
        location.origin = [NSEvent mouseLocation];
        location = [[m_osxView window] convertRectFromScreen:location];
        NSPoint locationInView = [m_osxView convertPoint:location.origin fromView:nil];        
        
        if( NSMouseInRect(locationInView, [m_osxView bounds], YES) )
        {
            [(NSCursor*)cursor.GetHCURSOR() set];
        }
    }
}

void wxWidgetCocoaImpl::CaptureMouse()
{
    // TODO remove if we don't get into problems with cursor settings
    //    [[m_osxView window] disableCursorRects];
}

void wxWidgetCocoaImpl::ReleaseMouse()
{
    // TODO remove if we don't get into problems with cursor settings
    //    [[m_osxView window] enableCursorRects];
}

#if !wxOSX_USE_NATIVE_FLIPPED

void wxWidgetCocoaImpl::SetFlipped(bool flipped)
{
    m_isFlipped = flipped;
}

#endif

void wxWidgetCocoaImpl::EnableFocusRing(bool enabled)
{
    [m_osxView setFocusRingType:enabled ? NSFocusRingTypeExterior : NSFocusRingTypeNone];
}

void wxWidgetCocoaImpl::SetDrawingEnabled(bool enabled)
{
    if ( [m_osxView window] == nil )
        return;

    if ( enabled )
    {
        [[m_osxView window] enableFlushWindow];
        [m_osxView setNeedsDisplay:YES];
    }
    else
    {
        [[m_osxView window] disableFlushWindow];
    }
}

void wxWidgetCocoaImpl::AdjustClippingView(wxScrollBar* horizontal, wxScrollBar* vertical)
{
    if( m_osxClipView )
    {
        NSRect bounds = m_osxView.bounds;
        if( horizontal && horizontal->IsShown() )
        {
            int sz = horizontal->GetSize().y;
            bounds.size.height -= sz;
        }
        if( vertical && vertical->IsShown() )
        {
            int sz = vertical->GetSize().x;
            bounds.size.width -= sz;
        }
        m_osxClipView.frame = bounds;
    }
}

void wxWidgetCocoaImpl::UseClippingView()
{
// starting from Sonoma child windows are bleeding through under the scrollbar
// use native scrollviews therefore
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_14_0
    if ( WX_IS_MACOS_AVAILABLE(14, 0) )
    {
        wxWindow* peer = m_wxPeer;

        if ( peer && m_osxClipView == nil)
        {
            m_osxClipView = [[wxNSClipView alloc] initWithFrame: m_osxView.bounds];
            [(NSClipView*)m_osxClipView setDrawsBackground: NO];
            [m_osxView addSubview:m_osxClipView];

            // TODO check for additional subwindows which might have to be moved to the clip view ?
        }
    }
#endif
}


//
// Factory methods
//

wxWidgetImpl* wxWidgetImpl::CreateUserPane( wxWindowMac* wxpeer, wxWindowMac* WXUNUSED(parent),
    wxWindowID WXUNUSED(id), const wxPoint& pos, const wxSize& size,
    long WXUNUSED(style), long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSView* v = [[wxNSView alloc] initWithFrame:r];

    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v, Widget_IsUserPane );
    return c;
}

wxWidgetImpl* wxWidgetImpl::CreateContentView( wxNonOwnedWindow* now )
{
    NSWindow* tlw = now->GetWXWindow();
    
    wxWidgetCocoaImpl* c = nullptr;
    if ( now->IsNativeWindowWrapper() )
    {
        NSView* cv = [tlw contentView];
        c = new wxWidgetCocoaImpl( now, cv, Widget_IsRoot );
        if ( cv != nil )
        {
            // increase ref count, because the impl destructor will decrement it again
            CFRetain(cv);
            if ( !now->IsShown() )
                [cv setHidden:NO];
        }
    }
    else
    {
        wxNSView* v = [[wxNSView alloc] initWithFrame:[[tlw contentView] frame]];
        c = new wxWidgetCocoaImpl( now, v, Widget_IsRoot );
        c->InstallEventHandler();
        [tlw setContentView:v];
    }
    return c;
}
