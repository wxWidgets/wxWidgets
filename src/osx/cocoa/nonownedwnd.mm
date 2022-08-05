/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/nonownedwnd.mm
// Purpose:     non owned window for cocoa
// Author:      Stefan Csomor
// Modified by:
// Created:     2008-06-20
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/nonownedwnd.h"
    #include "wx/frame.h"
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/menuitem.h"
    #include "wx/menu.h"
#endif

#include "wx/osx/private.h"

NSScreen* wxOSXGetMenuScreen()
{
    if ( [NSScreen screens] == nil )
        return [NSScreen mainScreen];
    else 
    {
        return [[NSScreen screens] objectAtIndex:0];
    }
}

NSRect wxToNSRect( NSView* parent, const wxRect& r )
{
    NSRect frame = parent ? [parent bounds] : [wxOSXGetMenuScreen() frame];
    int y = r.y;
    int x = r.x ;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = (int)(frame.size.height - ( r.y + r.height ));
    return NSMakeRect(x, y, r.width , r.height);
}

wxRect wxFromNSRect( NSView* parent, const NSRect& rect )
{
    NSRect frame = parent ? [parent bounds] : [wxOSXGetMenuScreen() frame];
    int y = (int)rect.origin.y;
    int x = (int)rect.origin.x;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = (int)(frame.size.height - (rect.origin.y + rect.size.height));
    return wxRect( x, y, (int)rect.size.width, (int)rect.size.height );
}

NSPoint wxToNSPoint( NSView* parent, const wxPoint& p )
{
    NSRect frame = parent ? [parent bounds] : [wxOSXGetMenuScreen() frame];
    int x = p.x ;
    int y = p.y;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = (int)(frame.size.height - ( p.y ));
    return NSMakePoint(x, y);
}

wxPoint wxFromNSPoint( NSView* parent, const NSPoint& p )
{
    NSRect frame = parent ? [parent bounds] : [wxOSXGetMenuScreen() frame];
    int x = (int)p.x;
    int y = (int)p.y;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = (int)(frame.size.height - ( p.y ));
    return wxPoint( x, y);
}

bool shouldHandleSelector(SEL selector)
{
    if (selector == @selector(noop:)
            || selector == @selector(complete:)
            || selector == @selector(deleteBackward:)
            || selector == @selector(deleteForward:)
            || selector == @selector(insertNewline:)
            || selector == @selector(insertTab:)
            || selector == @selector(insertBacktab:)
            || selector == @selector(keyDown:)
            || selector == @selector(keyUp:)
            || selector == @selector(scrollPageUp:)
            || selector == @selector(scrollPageDown:)
            || selector == @selector(scrollToBeginningOfDocument:)
            || selector == @selector(scrollToEndOfDocument:))
        return false;

    return true;

}

static bool IsUsingFullScreenApi(WXWindow macWindow)
{
    return ([macWindow collectionBehavior] & NSWindowCollectionBehaviorFullScreenPrimary);
}

//
// wx category for NSWindow (our own and wrapped instances)
//

@interface NSWindow (wxNSWindowSupport)

- (wxNonOwnedWindowCocoaImpl*) WX_implementation;

- (bool) WX_filterSendEvent:(NSEvent *) event;

@end

@implementation NSWindow (wxNSWindowSupport)

- (wxNonOwnedWindowCocoaImpl*) WX_implementation
{
    return (wxNonOwnedWindowCocoaImpl*) wxNonOwnedWindowImpl::FindFromWXWindow( self );
}

// TODO in cocoa everything during a drag is sent to the NSWindow the mouse down occurred,
// this does not conform to the wx behaviour if the window is not captured, so try to resend
// or capture all wx mouse event handling at the tlw as we did for carbon

- (bool) WX_filterSendEvent:(NSEvent *) event
{
    bool handled = false;
    if ( ([event type] >= NSLeftMouseDown) && ([event type] <= NSMouseExited) )
    {
        WXEVENTREF formerEvent = wxTheApp == NULL ? NULL : wxTheApp->MacGetCurrentEvent();
        WXEVENTHANDLERCALLREF formerHandler = wxTheApp == NULL ? NULL : wxTheApp->MacGetCurrentEventHandlerCallRef();

        wxWindow* cw = wxWindow::GetCapture();
        if ( cw != NULL )
        {
            if (wxTheApp)
                wxTheApp->MacSetCurrentEvent(event, NULL);
            ((wxWidgetCocoaImpl*)cw->GetPeer())->DoHandleMouseEvent( event);
            handled = true;
        }
        if ( handled )
        {
            if (wxTheApp)
                wxTheApp->MacSetCurrentEvent(formerEvent , formerHandler);
        }
    }
    return handled;
}
@end

//
// wx native implementation 
//

static NSResponder* s_nextFirstResponder = NULL;
static NSResponder* s_formerFirstResponder = NULL;

@interface wxNSWindow : NSWindow
{
}

- (void) sendEvent:(NSEvent *)event;
- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen;
- (void)noResponderFor: (SEL) selector;
- (BOOL)makeFirstResponder:(NSResponder *)aResponder;
@end

@implementation wxNSWindow

- (void)sendEvent:(NSEvent *) event
{
    if ( ![self WX_filterSendEvent: event] )
    {
        WXEVENTREF formerEvent = wxTheApp == NULL ? NULL : wxTheApp->MacGetCurrentEvent();
        WXEVENTHANDLERCALLREF formerHandler = wxTheApp == NULL ? NULL : wxTheApp->MacGetCurrentEventHandlerCallRef();

        if (wxTheApp)
            wxTheApp->MacSetCurrentEvent(event, NULL);

        [super sendEvent: event];

        if (wxTheApp)
            wxTheApp->MacSetCurrentEvent(formerEvent , formerHandler);
    }
}

// The default implementation always moves the window back onto the screen,
// even when the programmer explicitly wants to hide it.
- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
    wxUnusedVar(screen);
    return frameRect;
}

- (void)doCommandBySelector:(SEL)selector
{
    if (shouldHandleSelector(selector) &&
        !(selector == @selector(cancel:) || selector == @selector(cancelOperation:)) )
        [super doCommandBySelector:selector];
}


// NB: if we don't do this, all key downs that get handled lead to a NSBeep
- (void)noResponderFor: (SEL) selector
{
    if (selector != @selector(keyDown:) && selector != @selector(keyUp:))
    {
        [super noResponderFor:selector];
    }
}

// We need this for borderless windows, i.e. shaped windows or windows without  
// a title bar. For more info, see:
// http://lists.apple.com/archives/cocoa-dev/2008/May/msg02091.html
- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (BOOL)makeFirstResponder:(NSResponder *)aResponder
{
    NSResponder* tempFormer = s_formerFirstResponder;
    NSResponder* tempNext = s_nextFirstResponder;
    s_nextFirstResponder = aResponder;
    s_formerFirstResponder = [[NSApp keyWindow] firstResponder];
    BOOL retval = [super makeFirstResponder:aResponder];
    s_nextFirstResponder = tempNext;
    s_formerFirstResponder = tempFormer;
    return retval;
}

@end

@interface wxNSPanel : NSPanel
{
}

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen;
- (void)noResponderFor: (SEL) selector;
- (void)sendEvent:(NSEvent *)event;
- (BOOL)makeFirstResponder:(NSResponder *)aResponder;
@end

@implementation wxNSPanel

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
    wxNonOwnedWindowCocoaImpl* impl = (wxNonOwnedWindowCocoaImpl*) wxNonOwnedWindowImpl::FindFromWXWindow( self );
    if (impl && impl->IsFullScreen())
        return frameRect;
    else
        return [super constrainFrameRect:frameRect toScreen:screen];
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (void)doCommandBySelector:(SEL)selector
{
    if (shouldHandleSelector(selector))
        [super doCommandBySelector:selector];
}

// NB: if we don't do this, it seems that all events that end here lead to a NSBeep
- (void)noResponderFor: (SEL) selector
{
    if (selector != @selector(keyDown:) && selector != @selector(keyUp:))
    {
        [super noResponderFor:selector];
    }
}

- (void)sendEvent:(NSEvent *) event
{
    if ( ![self WX_filterSendEvent: event] )
    {
        WXEVENTREF formerEvent = wxTheApp == NULL ? NULL : wxTheApp->MacGetCurrentEvent();
        WXEVENTHANDLERCALLREF formerHandler = wxTheApp == NULL ? NULL : wxTheApp->MacGetCurrentEventHandlerCallRef();
        
        if (wxTheApp)
            wxTheApp->MacSetCurrentEvent(event, NULL);
        
        [super sendEvent: event];
        
        if (wxTheApp)
            wxTheApp->MacSetCurrentEvent(formerEvent , formerHandler);
    }
}

- (BOOL)makeFirstResponder:(NSResponder *)aResponder
{
    NSResponder* tempFormer = s_formerFirstResponder;
    NSResponder* tempNext = s_nextFirstResponder;
    s_nextFirstResponder = aResponder;
    s_formerFirstResponder = [[NSApp keyWindow] firstResponder];
    BOOL retval = [super makeFirstResponder:aResponder];
    s_nextFirstResponder = tempNext;
    s_formerFirstResponder = tempFormer;
    return retval;
}

@end


//
// controller
//

static void *EffectiveAppearanceContext = &EffectiveAppearanceContext;

@interface wxNonOwnedWindowController : NSObject <NSWindowDelegate>
{
}

- (void)windowDidResize:(NSNotification *)notification;
- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize;
- (void)windowDidResignKey:(NSNotification *)notification;
- (void)windowDidBecomeKey:(NSNotification *)notification;
- (void)windowDidMove:(NSNotification *)notification;
- (void)windowDidMiniaturize:(NSNotification *)notification;
- (void)windowDidDeminiaturize:(NSNotification *)notification;
- (BOOL)windowShouldClose:(id)window;
- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame;
- (void)windowWillEnterFullScreen:(NSNotification *)notification;
- (void)windowDidChangeBackingProperties:(NSNotification *)notification;
- (NSApplicationPresentationOptions)window:(NSWindow *)window
      willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions;

@end

extern int wxOSXGetIdFromSelector(SEL action );

@implementation wxNonOwnedWindowController

- (id) init
{
    self = [super init];
    return self;
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window
      willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    NSApplicationPresentationOptions options =
        NSApplicationPresentationFullScreen | NSApplicationPresentationHideDock;

    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        if (windowimpl->m_macFullscreenStyle & wxFULLSCREEN_NOMENUBAR)
            options |= NSApplicationPresentationAutoHideMenuBar;

        // Auto hide toolbar requires auto hide menu
        if (windowimpl->m_macFullscreenStyle & wxFULLSCREEN_NOTOOLBAR)
            options |= NSApplicationPresentationAutoHideToolbar |
                NSApplicationPresentationAutoHideMenuBar;
    }

    return options;
}

- (BOOL) triggerMenu:(SEL) action sender:(id)sender
{
    // feed back into menu item, if it is ours
    if ( [sender isKindOfClass:wxNSMenuItem.class] )
    {
        wxNSMenuItem* nsMenuItem = (wxNSMenuItem*) sender;
        wxMenuItemImpl* impl = [nsMenuItem implementation];
        if ( impl )
        {
            wxMenuItem* menuitem = impl->GetWXPeer();
            return menuitem->GetMenu()->HandleCommandProcess(menuitem);
        }
    }
    // otherwise feed back command into common menubar
    wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar();
    if ( mbar )
    {
        wxMenu* menu = NULL;
        wxMenuItem* menuitem = mbar->FindItem(wxOSXGetIdFromSelector(action), &menu);
        if ( menu != NULL && menuitem != NULL)
            return menu->HandleCommandProcess(menuitem);
    }
    return NO;
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem 
{    
    SEL action = [menuItem action];

    wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar();
    if ( mbar )
    {
        wxMenu* menu = NULL;
        wxMenuItem* menuitem = mbar->FindItem(wxOSXGetIdFromSelector(action), &menu);
        if ( menu != NULL && menuitem != NULL)
        {
            menu->HandleCommandUpdateStatus(menuitem);
            return menuitem->IsEnabled();
        }
    }
    return YES;
}

- (void)undo:(id)sender 
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)redo:(id)sender 
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)cut:(id)sender 
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)copy:(id)sender
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)paste:(id)sender
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)delete:(id)sender 
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)selectAll:(id)sender 
{
    wxUnusedVar(sender);
    [self triggerMenu:_cmd sender:sender];
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        if ( wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer() )
            wxpeer->OSXHandleMiniaturize(0, [window isMiniaturized]);
    }
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        if ( wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer() )
            wxpeer->OSXHandleMiniaturize(0, [window isMiniaturized]);
    }
}

- (BOOL)windowShouldClose:(id)nwindow
{
    wxNonOwnedWindowCocoaImpl* windowimpl = [(NSWindow*) nwindow WX_implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->Close();
    }
    return NO;
}

- (NSSize)windowWillResize:(NSWindow *)window
                    toSize:(NSSize)proposedFrameSize
{
    NSRect frame = [window frame];
    wxRect wxframe = wxFromNSRect( NULL, frame );
    wxframe.SetWidth( (int)proposedFrameSize.width );
    wxframe.SetHeight( (int)proposedFrameSize.height );

    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
        {
            wxpeer->HandleResizing( 0, &wxframe );
            NSSize newSize = NSMakeSize(wxframe.GetWidth(), wxframe.GetHeight());
            return newSize;
        }
    }

    return proposedFrameSize;
}

- (void)windowDidResize:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleResized(0);
    }
}

- (void)windowDidMove:(NSNotification *)notification
{
    wxNSWindow* window = (wxNSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMoved(0);
    }
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        // See windowDidResignKey: -- we emulate corresponding focus set
        // event for the first responder here as well:
        NSResponder *firstResponder = [window firstResponder];
        wxWidgetCocoaImpl *focused = firstResponder
                ? (wxWidgetCocoaImpl*)wxWidgetImpl::FindFromWXWidget(wxOSXGetViewFromResponder(firstResponder))
                : NULL;
        if ( focused )
            focused->DoNotifyFocusSet();

        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleActivated(0, true);
    }
}

- (void)windowDidResignKey:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
        {
            wxpeer->HandleActivated(0, false);

            // As for wx the deactivation also means losing focus, we
            // must emulate focus events _without_ resetting first responder
            // (because that would subtly break other things in Cocoa/macOS):
            NSResponder *firstResponder = [window firstResponder];
            wxWidgetCocoaImpl *focused = firstResponder
                    ? (wxWidgetCocoaImpl*)wxWidgetImpl::FindFromWXWidget(wxOSXGetViewFromResponder(firstResponder))
                    : NULL;
            if ( focused )
                focused->DoNotifyFocusLost();

            // TODO Remove if no problems arise with Popup Windows
#if 0
            // Needed for popup window since the firstResponder
            // (focus in wx) doesn't change when this
            // TLW becomes inactive.
            wxFocusEvent event( wxEVT_KILL_FOCUS, wxpeer->GetId());
            event.SetEventObject(wxpeer);
            wxpeer->HandleWindowEvent(event);
#endif
        }
    }
}

- (id)windowWillReturnFieldEditor:(NSWindow *)sender toObject:(id)anObject
{
    wxUnusedVar(sender);

    if ([anObject isKindOfClass:[wxNSTextField class]])
    {
        wxNSTextField* tf = (wxNSTextField*) anObject;
        wxNSTextFieldEditor* editor = [tf fieldEditor];
        if ( editor == nil )
        {
            editor = [[wxNSTextFieldEditor alloc] init];
            [editor setFieldEditor:YES];
            [editor setTextField:tf];
            [tf setFieldEditor:editor];
            [editor release];
        }
        return editor;
    } 
    else if ([anObject isKindOfClass:[wxNSSearchField class]])
    {
        wxNSSearchField* sf = (wxNSSearchField*) anObject;
        wxNSTextFieldEditor* editor = [sf fieldEditor];
        if ( editor == nil )
        {
            editor = [[wxNSTextFieldEditor alloc] init];
            [editor setFieldEditor:YES];
            [editor setTextField:sf];
            [sf setFieldEditor:editor];
            [editor release];
        }
        return editor;
    }
    else if ([anObject isKindOfClass:[wxNSComboBox class]])
    {
        wxNSComboBox * cb = (wxNSComboBox*) anObject;
        wxNSTextFieldEditor* editor = [cb fieldEditor];
        if ( editor == nil )
        {
            editor = [[wxNSTextFieldEditor alloc] init];
            [editor setFieldEditor:YES];
            [editor setTextField:cb];
            [cb setFieldEditor:editor];
            [editor release];
        }
        return editor;
    }    
 
    return nil;
}

- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame
{
    wxUnusedVar(newFrame);
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        wxMaximizeEvent event(wxpeer->GetId());
        event.SetEventObject(wxpeer);
        return !wxpeer->HandleWindowEvent(event);
    }
    return true;
}

static void SendFullScreenWindowEvent(NSNotification* notification, bool fullscreen)
{
    NSWindow* window = (NSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window WX_implementation];
    if ( windowimpl )
    {
        if (windowimpl->m_macIgnoreNextFullscreenChange)
        {
            windowimpl->m_macIgnoreNextFullscreenChange = false;
            return;
        }

        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        wxFullScreenEvent event(wxpeer->GetId(), fullscreen);
        event.SetEventObject(wxpeer);
        wxpeer->HandleWindowEvent(event);
    }
}

// work around OS X bug, on a secondary monitor an already fully sized window
// (eg maximized) will not be correctly put to full screen size and keeps a 22px
// title band at the top free, therefore we force the correct content size

- (void)windowWillEnterFullScreen:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*) [notification object];

    NSView* view = [window contentView];
    NSRect windowframe = [window frame];
    NSRect viewframe = [view frame];
    NSUInteger stylemask = [window styleMask] | NSFullScreenWindowMask;
    NSRect expectedframerect = [NSWindow contentRectForFrameRect: windowframe styleMask: stylemask];
    
    if ( !NSEqualSizes(expectedframerect.size, viewframe.size) )
    {
        [view setFrameSize: expectedframerect.size];
    }

    SendFullScreenWindowEvent(notification, true);
}

- (void)windowWillExitFullScreen:(NSNotification *)notification
{
    SendFullScreenWindowEvent(notification, false);
}

// from https://developer.apple.com/library/archive/documentation/GraphicsAnimation/Conceptual/HighResolutionOSX/CapturingScreenContents/CapturingScreenContents.html

- (void)windowDidChangeBackingProperties:(NSNotification *)notification
{
    NSWindow* theWindow = (NSWindow*)[notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [theWindow WX_implementation];
    wxNonOwnedWindow* wxpeer = windowimpl ? windowimpl->GetWXPeer() : NULL;
    if (wxpeer)
    {
        CGFloat newBackingScaleFactor = [theWindow backingScaleFactor];
        CGFloat oldBackingScaleFactor = [[[notification userInfo]
                                          objectForKey:@"NSBackingPropertyOldScaleFactorKey"]
                                         doubleValue];
        if (newBackingScaleFactor != oldBackingScaleFactor)
        {
            const wxSize oldDPI = wxWindow::OSXMakeDPIFromScaleFactor(oldBackingScaleFactor);
            const wxSize newDPI = wxWindow::OSXMakeDPIFromScaleFactor(newBackingScaleFactor);

            wxDPIChangedEvent event(oldDPI, newDPI);
            event.SetEventObject(wxpeer);
            wxpeer->HandleWindowEvent(event);

        }

        NSColorSpace *newColorSpace = [theWindow colorSpace];
        NSColorSpace *oldColorSpace = [[notification userInfo]
                                       objectForKey:@"NSBackingPropertyOldColorSpaceKey"];
        if (![newColorSpace isEqual:oldColorSpace])
        {
            wxSysColourChangedEvent event;
            event.SetEventObject(wxpeer);
            wxpeer->HandleWindowEvent(event);
        }
    }
}

- (void)addObservers:(NSWindow*)win
{
    [win addObserver:self forKeyPath:@"effectiveAppearance"
             options:0 context:EffectiveAppearanceContext];
}

- (void)removeObservers:(NSWindow*)win
{
    [win removeObserver:self forKeyPath:@"effectiveAppearance" context:EffectiveAppearanceContext];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    wxUnusedVar(keyPath);
    wxUnusedVar(change);

    if (context == EffectiveAppearanceContext)
    {
        wxNonOwnedWindowCocoaImpl* windowimpl = [(NSWindow*)object WX_implementation];
        wxNonOwnedWindow* wxpeer = windowimpl ? windowimpl->GetWXPeer() : NULL;
        if (wxpeer)
        {
            wxSysColourChangedEvent event;
            event.SetEventObject(wxpeer);
            wxpeer->HandleWindowEvent(event);
        }
    }
}

@end

wxIMPLEMENT_DYNAMIC_CLASS(wxNonOwnedWindowCocoaImpl , wxNonOwnedWindowImpl);

wxNonOwnedWindowCocoaImpl::wxNonOwnedWindowCocoaImpl( wxNonOwnedWindow* nonownedwnd) :
    wxNonOwnedWindowImpl(nonownedwnd)
{
    m_macWindow = NULL;
    m_macFullScreenData = NULL;
}

wxNonOwnedWindowCocoaImpl::wxNonOwnedWindowCocoaImpl()
{
    m_macWindow = NULL;
    m_macFullScreenData = NULL;
}

wxNonOwnedWindowCocoaImpl::~wxNonOwnedWindowCocoaImpl()
{
    if ( !m_wxPeer->IsNativeWindowWrapper() )
    {
        [(wxNonOwnedWindowController*)[m_macWindow delegate] removeObservers:m_macWindow];
        [m_macWindow setDelegate:nil];
     
        // make sure we remove this first, otherwise the ref count will not lead to the 
        // native window's destruction
        if ([m_macWindow parentWindow] != 0)
            [[m_macWindow parentWindow] removeChildWindow: m_macWindow];

        [m_macWindow setReleasedWhenClosed:YES];
        [m_macWindow close];
    }
}

void wxNonOwnedWindowCocoaImpl::WillBeDestroyed()
{
    if ( !m_wxPeer->IsNativeWindowWrapper() )
    {
        [(wxNonOwnedWindowController*)[m_macWindow delegate] removeObservers:m_macWindow];
        [m_macWindow setDelegate:nil];
    }
}

void wxNonOwnedWindowCocoaImpl::Create( wxWindow* WXUNUSED(parent), const wxPoint& pos, const wxSize& size,
long style, long extraStyle, const wxString& WXUNUSED(name) )
{
    static wxNonOwnedWindowController* controller = NULL;

    if ( !controller )
        controller =[[wxNonOwnedWindowController alloc] init];


    int windowstyle = NSBorderlessWindowMask;

    if ( style & wxFRAME_TOOL_WINDOW || ( style & wxPOPUP_WINDOW ) ||
            GetWXPeer()->GetExtraStyle() & wxTOPLEVEL_EX_DIALOG )
        m_macWindow = [wxNSPanel alloc];
    else
        m_macWindow = [wxNSWindow alloc];

    [m_macWindow setAcceptsMouseMovedEvents:YES];

    NSInteger level = NSNormalWindowLevel;

    if ( style & wxFRAME_TOOL_WINDOW )
    {
        windowstyle |= NSUtilityWindowMask;
    }
    else if ( ( style & wxFRAME_DRAWER ) )
    {
        /*
        wclass = kDrawerWindowClass;
        */
    }
 
    if ( ( style & wxMINIMIZE_BOX ) || ( style & wxMAXIMIZE_BOX ) ||
        ( style & wxCLOSE_BOX ) || ( style & wxSYSTEM_MENU ) || ( style & wxCAPTION ) )
    {
        windowstyle |= NSTitledWindowMask ;
        if ( ( style & wxMINIMIZE_BOX ) )
            windowstyle |= NSMiniaturizableWindowMask ;
        
        if ( ( style & wxMAXIMIZE_BOX ) )
            windowstyle |= NSResizableWindowMask ;
        
        if ( ( style & wxCLOSE_BOX) )
            windowstyle |= NSClosableWindowMask ;
    }
    
    if ( ( style & wxRESIZE_BORDER ) )
        windowstyle |= NSResizableWindowMask ;

    if ( extraStyle & wxFRAME_EX_METAL)
        windowstyle |= NSTexturedBackgroundWindowMask;

    if ( ( style & wxFRAME_FLOAT_ON_PARENT ) || ( style & wxFRAME_TOOL_WINDOW ) )
        level = NSFloatingWindowLevel;

    if ( ( style & wxSTAY_ON_TOP ) )
        level = NSModalPanelWindowLevel;

    if ( ( style & wxPOPUP_WINDOW ) )
        level = NSPopUpMenuWindowLevel;

    NSRect frameRect = wxToNSRect( NULL, wxRect( pos, size) );

    NSRect contentRect = [NSWindow contentRectForFrameRect:frameRect styleMask:windowstyle];

    [m_macWindow initWithContentRect:contentRect
        styleMask:windowstyle
        backing:NSBackingStoreBuffered
        defer:NO
        ];

    if (!NSEqualRects([m_macWindow frame], frameRect))
    {
        [m_macWindow setFrame:frameRect display:NO];
    }

    // if we just have a title bar with no buttons needed, hide them
    if ( (windowstyle & NSTitledWindowMask) && 
        !(style & wxCLOSE_BOX) && !(style & wxMAXIMIZE_BOX) && !(style & wxMINIMIZE_BOX) )
    {
        [[m_macWindow standardWindowButton:NSWindowZoomButton] setHidden:YES];
        [[m_macWindow standardWindowButton:NSWindowCloseButton] setHidden:YES];
        [[m_macWindow standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
    }
    
    m_macWindowLevel = level;
    SetUpForModalParent();
    [m_macWindow setLevel:m_macWindowLevel];

    [m_macWindow setDelegate:controller];
    [controller addObservers:m_macWindow];
    
    if ( !(style & wxFRAME_TOOL_WINDOW) )
        [m_macWindow setHidesOnDeactivate:NO];
    
    if ( GetWXPeer()->GetBackgroundStyle() == wxBG_STYLE_TRANSPARENT )
    {
        [m_macWindow setOpaque:NO];
        [m_macWindow setAlphaValue:1.0];
        [m_macWindow setBackgroundColor:[NSColor clearColor]];
    }
}

void wxNonOwnedWindowCocoaImpl::Create( wxWindow* WXUNUSED(parent), WXWindow nativeWindow )
{
    m_macWindow = nativeWindow;
}

WXWindow wxNonOwnedWindowCocoaImpl::GetWXWindow() const
{
    return m_macWindow;
}

void wxNonOwnedWindowCocoaImpl::Raise()
{
    [m_macWindow makeKeyAndOrderFront:nil];
}

void wxNonOwnedWindowCocoaImpl::Lower()
{
    [m_macWindow orderWindow:NSWindowBelow relativeTo:0];
}

void wxNonOwnedWindowCocoaImpl::SetUpForModalParent()
{
    wxNonOwnedWindow* wxpeer = GetWXPeer();
    if (wxpeer)
    {
        // If the parent is modal, windows with wxFRAME_FLOAT_ON_PARENT style need
        // to be in NSModalPanelWindowLevel and not NSFloatingWindowLevel to stay
        // above the parent.
        wxDialog* const parentDialog = wxDynamicCast(wxGetTopLevelParent(wxpeer->GetParent()), wxDialog);
        if (parentDialog && parentDialog->IsModal())
        {
            if (m_macWindowLevel == NSFloatingWindowLevel)
            {
                m_macWindowLevel = NSModalPanelWindowLevel;
                if ([m_macWindow level] == NSFloatingWindowLevel)
                    [m_macWindow setLevel:m_macWindowLevel];
            }

            // Cocoa's modal loop does not process other windows by default, but
            // don't call this on normal window levels so nested modal dialogs will
            // still behave modally.
            if (m_macWindowLevel != NSNormalWindowLevel)
            {
                if ([m_macWindow isKindOfClass:[NSPanel class]])
                {
                    [(NSPanel*)m_macWindow setWorksWhenModal:YES];
                }
            }
        }
    }
}

void wxNonOwnedWindowCocoaImpl::ShowWithoutActivating()
{
    SetUpForModalParent();
    [m_macWindow setHidesOnDeactivate:YES];
    [m_macWindow orderFront:nil];
    [[m_macWindow contentView] setNeedsDisplay: YES];
}

bool wxNonOwnedWindowCocoaImpl::Show(bool show)
{
    if ( show )
    {
        wxNonOwnedWindow* wxpeer = GetWXPeer(); 
        if ( wxpeer )
        {
            // add to parent window before showing
            wxDialog * const dialog = wxDynamicCast(wxpeer, wxDialog);
            if ( wxpeer->GetParent() && dialog )
            {
                NSView * parentView = wxpeer->GetParent()->GetPeer()->GetWXWidget();
                if ( parentView )
                {
                    NSWindow* parentNSWindow = [parentView window];
                    if ( parentNSWindow ) {
                        // we used to call [parentNSWindow addChildWindow:m_macWindow here
                        // but this lead to problems with Spaces (modal windows disappeared
                        // when dragged to a different space)

                        // If the parent is modal, windows with wxFRAME_FLOAT_ON_PARENT style need
                        // to be in NSModalPanelWindowLevel and not NSFloatingWindowLevel to stay
                        // above the parent.
                        if ([m_macWindow level] == NSFloatingWindowLevel ||
                            [m_macWindow level] == NSModalPanelWindowLevel) {
                            m_macWindowLevel = NSModalPanelWindowLevel;
                            [m_macWindow setLevel:m_macWindowLevel];
                        }
                    }
                }
            }
            
            SetUpForModalParent();
            if (!(wxpeer->GetWindowStyle() & wxFRAME_TOOL_WINDOW))
                [m_macWindow makeKeyAndOrderFront:nil];
            else 
                [m_macWindow orderFront:nil]; 
        }
        [[m_macWindow contentView] setNeedsDisplay: YES];
    }
    else
    {
        // avoid propagation of orderOut to parent 
        if ([m_macWindow parentWindow] != 0)
            [[m_macWindow parentWindow] removeChildWindow: m_macWindow];
        [m_macWindow orderOut:nil];
    }
    return true;
}

bool wxNonOwnedWindowCocoaImpl::ShowWithEffect(bool show,
                                               wxShowEffect effect,
                                               unsigned timeout)
{
    return wxWidgetCocoaImpl::
            ShowViewOrWindowWithEffect(m_wxPeer, show, effect, timeout);
}

void wxNonOwnedWindowCocoaImpl::Update()
{
    [m_macWindow displayIfNeeded];
}

bool wxNonOwnedWindowCocoaImpl::SetTransparent(wxByte alpha)
{
    [m_macWindow setAlphaValue:(CGFloat) alpha/255.0];
    return true;
}

bool wxNonOwnedWindowCocoaImpl::SetBackgroundColour(const wxColour& col )
{
    [m_macWindow setBackgroundColor:col.OSXGetNSColor()];
    return true;
}

void wxNonOwnedWindowCocoaImpl::SetExtraStyle( long exStyle )
{
    if ( m_macWindow )
    {
        bool metal = exStyle & wxFRAME_EX_METAL ;
        int windowStyle = [ m_macWindow styleMask];
        if ( metal && !(windowStyle & NSTexturedBackgroundWindowMask) )
        {
            wxFAIL_MSG( wxT("Metal Style cannot be changed after creation") );
        }
        else if ( !metal && (windowStyle & NSTexturedBackgroundWindowMask ) )
        {
            wxFAIL_MSG( wxT("Metal Style cannot be changed after creation") );
        }
    }
}

void wxNonOwnedWindowCocoaImpl::SetWindowStyleFlag( long style )
{
    // don't mess with native wrapped windows, they might throw an exception when their level is changed
    if (!m_wxPeer->IsNativeWindowWrapper() && m_macWindow)
    {
        NSInteger level = NSNormalWindowLevel;
        
        if (style & wxSTAY_ON_TOP)
            level = NSModalPanelWindowLevel;
        else if (( style & wxFRAME_FLOAT_ON_PARENT ) || ( style & wxFRAME_TOOL_WINDOW ))
            level = NSFloatingWindowLevel;

        // Only update the level when it has changed, setting a level can cause the OS to reorder the windows in the level
        if ( level != m_macWindowLevel )
        {
            [m_macWindow setLevel: level];
            m_macWindowLevel = level;
        }
    }
}

bool wxNonOwnedWindowCocoaImpl::SetBackgroundStyle(wxBackgroundStyle style)
{
    if ( style == wxBG_STYLE_TRANSPARENT )
    {
        [m_macWindow setOpaque:NO];
        [m_macWindow setBackgroundColor:[NSColor clearColor]];
    }

    return true;
}

bool wxNonOwnedWindowCocoaImpl::CanSetTransparent()
{
    return true;
}

void wxNonOwnedWindowCocoaImpl::MoveWindow(int x, int y, int width, int height)
{
    NSRect r = wxToNSRect( NULL, wxRect(x,y,width, height) );
    // do not trigger refreshes upon invisible and possible partly created objects
    [m_macWindow setFrame:r display:GetWXPeer()->IsShownOnScreen()];
}

void wxNonOwnedWindowCocoaImpl::GetPosition( int &x, int &y ) const
{
    wxRect r = wxFromNSRect( NULL, [m_macWindow frame] );
    x = r.GetLeft();
    y = r.GetTop();
}

void wxNonOwnedWindowCocoaImpl::GetSize( int &width, int &height ) const
{
    NSRect rect = [m_macWindow frame];
    width = (int)rect.size.width;
    height = (int)rect.size.height;
}

void wxNonOwnedWindowCocoaImpl::GetContentArea( int& left, int &top, int &width, int &height ) const
{
    NSRect rect = [[m_macWindow contentView] frame];
    left = (int)rect.origin.x;
    top = (int)rect.origin.y;
    width = (int)rect.size.width;
    height = (int)rect.size.height;
}

bool wxNonOwnedWindowCocoaImpl::SetShape(const wxRegion& WXUNUSED(region))
{
    // macOS caches the contour of the drawn area, so when the region is changed and the content view redrawn
    // the shape of the tlw does not change, this is a workaround I found that leads to a contour-refresh ...
    NSRect formerFrame = [m_macWindow frame];
    NSSize formerSize = [NSWindow contentRectForFrameRect:formerFrame styleMask:[m_macWindow styleMask]].size;
    [m_macWindow setContentSize:NSMakeSize(10,10)];
    [m_macWindow setContentSize:formerSize];

    [m_macWindow setOpaque:NO];
    [m_macWindow setBackgroundColor:[NSColor clearColor]];

    return true;
}

void wxNonOwnedWindowCocoaImpl::SetTitle( const wxString& title, wxFontEncoding encoding )
{
    [m_macWindow setTitle:wxCFStringRef( title , encoding ).AsNSString()];
}

wxContentProtection wxNonOwnedWindowCocoaImpl::GetContentProtection() const
{
    return (m_macWindow.sharingType == NSWindowSharingNone) ?
        wxCONTENT_PROTECTION_ENABLED : wxCONTENT_PROTECTION_NONE;
}

bool wxNonOwnedWindowCocoaImpl::SetContentProtection(wxContentProtection contentProtection)
{
    m_macWindow.sharingType = (contentProtection == wxCONTENT_PROTECTION_ENABLED) ?
        NSWindowSharingNone : NSWindowSharingReadOnly;

    return true;
}

bool wxNonOwnedWindowCocoaImpl::EnableCloseButton(bool enable)
{
    [[m_macWindow standardWindowButton:NSWindowCloseButton] setEnabled:enable];

    return true;
}

bool wxNonOwnedWindowCocoaImpl::EnableMaximizeButton(bool enable)
{
    [[m_macWindow standardWindowButton:NSWindowZoomButton] setEnabled:enable];

    return true;
}

bool wxNonOwnedWindowCocoaImpl::EnableMinimizeButton(bool enable)
{
    [[m_macWindow standardWindowButton:NSWindowMiniaturizeButton] setEnabled:enable];

    return true;
}

bool wxNonOwnedWindowCocoaImpl::IsMaximized() const
{
    if (([m_macWindow styleMask] & NSResizableWindowMask) != 0)
    {
        // isZoomed internally calls windowWillResize which would trigger
        // an wxEVT_SIZE. Setting ignore resizing supresses the event
        m_wxPeer->OSXSetIgnoreResizing(true);
        BOOL result = [m_macWindow isZoomed];
        m_wxPeer->OSXSetIgnoreResizing(false);
        return result;
    }
    else
    {
        NSRect rectScreen = [[NSScreen mainScreen] visibleFrame];
        NSRect rectWindow = [m_macWindow frame];
        return (rectScreen.origin.x == rectWindow.origin.x &&
                rectScreen.origin.y == rectWindow.origin.y &&
                rectScreen.size.width == rectWindow.size.width &&
                rectScreen.size.height == rectWindow.size.height);
    }
}

bool wxNonOwnedWindowCocoaImpl::IsIconized() const
{
    return [m_macWindow isMiniaturized];
}

void wxNonOwnedWindowCocoaImpl::Iconize( bool iconize )
{
    if ( iconize )
        [m_macWindow miniaturize:nil];
    else
        [m_macWindow deminiaturize:nil];
}

void wxNonOwnedWindowCocoaImpl::Maximize(bool WXUNUSED(maximize))
{
    [m_macWindow zoom:nil];
}


// http://cocoadevcentral.com/articles/000028.php

typedef struct
{
    NSUInteger m_formerStyleMask;
    int m_formerLevel;
    NSRect m_formerFrame;
} FullScreenData ;

bool wxNonOwnedWindowCocoaImpl::IsFullScreen() const
{
    if ( IsUsingFullScreenApi(m_macWindow) )
    {
        return [m_macWindow styleMask] & NSFullScreenWindowMask;
    }

    return m_macFullScreenData != NULL ;
}

bool wxNonOwnedWindowCocoaImpl::EnableFullScreenView(bool enable, long style)
{
    m_macFullscreenStyle = style;
    NSUInteger collectionBehavior = [m_macWindow collectionBehavior];
    if (enable)
    {
        collectionBehavior |= NSWindowCollectionBehaviorFullScreenPrimary;
        collectionBehavior &= ~NSWindowCollectionBehaviorFullScreenAuxiliary;
    }
    else
    {
        // Note that just turning "Full Screen Primary" is not enough, the
        // window would still be made full screen when the green button in the
        // title bar is pressed, and we need to explicitly turn on the "Full
        // Screen Auxiliary" style to prevent this from happening. This works,
        // at least under 10.11 and 10.14, even though it's not really clear
        // from the documentation that it should.
        collectionBehavior &= ~NSWindowCollectionBehaviorFullScreenPrimary;
        collectionBehavior |= NSWindowCollectionBehaviorFullScreenAuxiliary;
    }
    [m_macWindow setCollectionBehavior: collectionBehavior];

    return true;
}

bool wxNonOwnedWindowCocoaImpl::ShowFullScreen(bool show, long style)
{
    if ( IsUsingFullScreenApi(m_macWindow) )
    {
        if ( show != IsFullScreen() )
        {
            m_macFullscreenStyle = style;
            m_macIgnoreNextFullscreenChange = true;
            [m_macWindow toggleFullScreen: nil];
        }

        return true;
    }

    if ( show )
    {
        FullScreenData *data = (FullScreenData *)m_macFullScreenData ;
        delete data ;
        data = new FullScreenData();

        m_macFullScreenData = data ;
        data->m_formerLevel = [m_macWindow level];
        data->m_formerFrame = [m_macWindow frame];
        data->m_formerStyleMask = [m_macWindow styleMask];

        NSRect screenframe = [[NSScreen mainScreen] frame];
        NSRect frame = NSMakeRect (0, 0, 100, 100);
        NSRect contentRect;

        [m_macWindow setStyleMask:data->m_formerStyleMask & ~ NSResizableWindowMask];
        
        contentRect = [NSWindow contentRectForFrameRect: frame
                                styleMask: [m_macWindow styleMask]];
        screenframe.origin.y += (frame.origin.y - contentRect.origin.y);
        screenframe.size.height += (frame.size.height - contentRect.size.height);
        [m_macWindow setFrame:screenframe display:YES];

        SetSystemUIMode(kUIModeAllHidden,
                                kUIOptionDisableAppleMenu
                        /*
                                | kUIOptionDisableProcessSwitch
                                | kUIOptionDisableForceQuit
                         */); 
    }
    else if ( m_macFullScreenData != NULL )
    {
        FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
        
        [m_macWindow setFrame:data->m_formerFrame display:YES];
        [m_macWindow setStyleMask:data->m_formerStyleMask];

        delete data ;
        m_macFullScreenData = NULL ;

        SetSystemUIMode(kUIModeNormal, 0); 
    }

    return true;
}

void wxNonOwnedWindowCocoaImpl::RequestUserAttention(int flagsWX)
{
    NSRequestUserAttentionType flagsOSX;
    switch ( flagsWX )
    {
        case wxUSER_ATTENTION_INFO:
            flagsOSX = NSInformationalRequest;
            break;

        case wxUSER_ATTENTION_ERROR:
            flagsOSX = NSCriticalRequest;
            break;

        default:
            wxFAIL_MSG( "invalid RequestUserAttention() flags" );
            return;
    }

    [NSApp requestUserAttention:flagsOSX];
}

void wxNonOwnedWindowCocoaImpl::ScreenToWindow( int *x, int *y )
{
    wxPoint p((x ? *x : 0), (y ? *y : 0) );
    NSRect nsrect = NSZeroRect;
    nsrect.origin = wxToNSPoint( NULL, p );
    nsrect = [m_macWindow convertRectFromScreen:nsrect];
    NSPoint nspt = [[m_macWindow contentView] convertPoint:nsrect.origin fromView:nil];
    p = wxFromNSPoint([m_macWindow contentView], nspt);
    if ( x )
        *x = p.x;
    if ( y )
        *y = p.y;
}

void wxNonOwnedWindowCocoaImpl::WindowToScreen( int *x, int *y )
{
    wxPoint p((x ? *x : 0), (y ? *y : 0) );
    NSPoint nspt = wxToNSPoint( [m_macWindow contentView], p );
    nspt = [[m_macWindow contentView] convertPoint:nspt toView:nil];
    NSRect nsrect = NSZeroRect;
    nsrect.origin = nspt;
    nsrect = [m_macWindow convertRectToScreen:nsrect];
    p = wxFromNSPoint( NULL, nsrect.origin);
    if ( x )
        *x = p.x;
    if ( y )
        *y = p.y;
}

bool wxNonOwnedWindowCocoaImpl::IsActive()
{
    return [m_macWindow isKeyWindow];
}

void wxNonOwnedWindowCocoaImpl::SetModified(bool modified)
{
    [m_macWindow setDocumentEdited:modified];
}

bool wxNonOwnedWindowCocoaImpl::IsModified() const
{
    return [m_macWindow isDocumentEdited];
}

void wxNonOwnedWindowCocoaImpl::SetRepresentedFilename(const wxString& filename)
{
    [m_macWindow setRepresentedFilename:wxCFStringRef(filename).AsNSString()];
}

void wxNonOwnedWindowCocoaImpl::SetBottomBorderThickness(int thickness)
{
    [m_macWindow setAutorecalculatesContentBorderThickness:(thickness ? NO : YES) forEdge:NSMinYEdge];
    [m_macWindow setContentBorderThickness:thickness forEdge:NSMinYEdge];
}

void wxNonOwnedWindowCocoaImpl::RestoreWindowLevel()
{
    if ( [m_macWindow level] != m_macWindowLevel )
        [m_macWindow setLevel:m_macWindowLevel];
}

WX_NSResponder wxNonOwnedWindowCocoaImpl::GetNextFirstResponder()
{
    return s_nextFirstResponder;
}

WX_NSResponder wxNonOwnedWindowCocoaImpl::GetFormerFirstResponder()
{
    return s_formerFirstResponder;
}

//
//
//

wxNonOwnedWindowImpl* wxNonOwnedWindowImpl::CreateNonOwnedWindow( wxNonOwnedWindow* wxpeer, wxWindow* parent, WXWindow nativeWindow)
{
    wxNonOwnedWindowCocoaImpl* now = new wxNonOwnedWindowCocoaImpl( wxpeer );
    now->Create( parent, nativeWindow );
    return now;
}

wxNonOwnedWindowImpl* wxNonOwnedWindowImpl::CreateNonOwnedWindow( wxNonOwnedWindow* wxpeer, wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name )
{
    wxNonOwnedWindowImpl* now = new wxNonOwnedWindowCocoaImpl( wxpeer );
    now->Create( parent, pos, size, style , extraStyle, name );
    return now;
}

