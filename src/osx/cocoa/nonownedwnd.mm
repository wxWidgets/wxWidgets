/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/nonownedwnd.mm
// Purpose:     non owned window for cocoa
// Author:      DavidStefan Csomor
// Modified by:
// Created:     2008-06-20
// RCS-ID:      $Id: nonownedwnd.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/nonownedwnd.h"
    #include "wx/frame.h"
#endif

#include "wx/osx/private.h"

NSRect wxToNSRect( NSView* parent, const wxRect& r )
{
    NSRect frame = parent ? [parent bounds] : [[NSScreen mainScreen] frame];
    int y = r.y;
    int x = r.x ;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = frame.size.height - ( r.y + r.height );
    return NSMakeRect(x, y, r.width , r.height);
}

wxRect wxFromNSRect( NSView* parent, const NSRect& rect )
{
    NSRect frame = parent ? [parent bounds] : [[NSScreen mainScreen] frame];
    int y = rect.origin.y;
    int x = rect.origin.x;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = frame.size.height - (rect.origin.y + rect.size.height);
    return wxRect( x, y, rect.size.width, rect.size.height );
}

NSPoint wxToNSPoint( NSView* parent, const wxPoint& p )
{
    NSRect frame = parent ? [parent bounds] : [[NSScreen mainScreen] frame];
    int x = p.x ;
    int y = p.y;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = frame.size.height - ( p.y );
    return NSMakePoint(x, y);
}

wxPoint wxFromNSPoint( NSView* parent, const NSPoint& p )
{
    NSRect frame = parent ? [parent bounds] : [[NSScreen mainScreen] frame];
    int x = p.x;
    int y = p.y;
    if ( parent == NULL || ![ parent isFlipped ] )
        y = frame.size.height - ( p.y );
    return wxPoint( x, y);
}

//
// wx native implementation classes
//

@interface wxNSWindow : NSWindow

{
    wxNonOwnedWindowCocoaImpl* impl;
}

- (void)setImplementation: (wxNonOwnedWindowCocoaImpl *) theImplementation;
- (wxNonOwnedWindowCocoaImpl*) implementation;

@end

@implementation wxNSWindow

- (void)setImplementation: (wxNonOwnedWindowCocoaImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxNonOwnedWindowCocoaImpl*) implementation
{
    return impl;
}


@end

@interface wxNSPanel : wxNSWindow

{
}

@end

@implementation wxNSPanel 

@end


//
// controller
//

@interface wxNonOwnedWindowController : NSObject
{
}

- (void)windowDidResize:(NSNotification *)notification;
- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize;
- (void)windowDidResignMain:(NSNotification *)notification;
- (void)windowDidBecomeMain:(NSNotification *)notification;
- (void)windowDidMove:(NSNotification *)notification;
- (BOOL)windowShouldClose:(id)window;

@end

@implementation wxNonOwnedWindowController

- (id) init
{
    [super init];
    return self;
}

- (BOOL)windowShouldClose:(id)nwindow
{
    wxNSWindow* window = (wxNSWindow*) nwindow;
    wxNonOwnedWindowCocoaImpl* windowimpl = [window implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->Close();
    }
    return NO;
}

- (NSSize)windowWillResize:(NSWindow *)win
                    toSize:(NSSize)proposedFrameSize
{
    NSRect frame = [win frame];
    wxRect wxframe = wxFromNSRect( NULL, frame );
    wxframe.SetWidth( proposedFrameSize.width );
    wxframe.SetHeight( proposedFrameSize.height );
    wxNSWindow* window = (wxNSWindow*) win;
    wxNonOwnedWindowCocoaImpl* windowimpl = [window implementation];
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
    wxNSWindow* window = (wxNSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window implementation];
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
    wxNonOwnedWindowCocoaImpl* windowimpl = [window implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMoved(0);
    }
}

- (void)windowDidBecomeMain:(NSNotification *)notification
{
    wxNSWindow* window = (wxNSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleActivated(0, true);
    }
}

- (void)windowDidResignMain:(NSNotification *)notification
{
    wxNSWindow* window = (wxNSWindow*) [notification object];
    wxNonOwnedWindowCocoaImpl* windowimpl = [window implementation];
    if ( windowimpl )
    {
        wxNonOwnedWindow* wxpeer = windowimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleActivated(0, false);
    }
}

@end

IMPLEMENT_DYNAMIC_CLASS( wxNonOwnedWindowCocoaImpl , wxNonOwnedWindowImpl )

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
    [m_macWindow setImplementation:nil];
    [m_macWindow setDelegate:nil];
    [m_macWindow release];
}

void wxNonOwnedWindowCocoaImpl::Destroy()
{
    wxPendingDelete.Append( new wxDeferredObjectDeleter( this ) );
}

void wxNonOwnedWindowCocoaImpl::Create( wxWindow* parent, const wxPoint& pos, const wxSize& size,
long style, long extraStyle, const wxString& name )
{
    static wxNonOwnedWindowController* controller = NULL;
    
    if ( !controller )
        controller =[[wxNonOwnedWindowController alloc] init];


    int windowstyle = NSBorderlessWindowMask;
    
    if ( style & wxFRAME_TOOL_WINDOW )
        m_macWindow = [wxNSPanel alloc];
    else
        m_macWindow = [wxNSWindow alloc];
    
    CGWindowLevel level = kCGNormalWindowLevel;
    
    if ( style & wxFRAME_TOOL_WINDOW )
    {
        if ( ( style & wxSTAY_ON_TOP ) )
            level = kCGUtilityWindowLevel;
        else
            level = kCGFloatingWindowLevel ;
        
    }
    else if ( ( style & wxPOPUP_WINDOW ) )
    {
        level = kCGPopUpMenuWindowLevel;
        /*
        if ( ( style & wxBORDER_NONE ) )
        {
            wclass = kHelpWindowClass ;   // has no border
            attr |= kWindowNoShadowAttribute;
        }
        else
        {
            wclass = kPlainWindowClass ;  // has a single line border, it will have to do for now
        }
        */
    }
    else if ( ( style & wxCAPTION ) )
    {
        windowstyle |= NSTitledWindowMask ;
    }
    else if ( ( style & wxFRAME_DRAWER ) )
    {
        /*
        wclass = kDrawerWindowClass;
        */
    }
    else
    {
        // set these even if we have no title, otherwise the controls won't be visible
        if ( ( style & wxMINIMIZE_BOX ) || ( style & wxMAXIMIZE_BOX ) ||
            ( style & wxCLOSE_BOX ) || ( style & wxSYSTEM_MENU ) )
        {
            windowstyle |= NSTitledWindowMask ;
        }
        /*
        else if ( ( style & wxNO_BORDER ) )
        {
            wclass = kSimpleWindowClass ;
        }
        else
        {
            wclass = kPlainWindowClass ;
        }
        */
    }

    if ( windowstyle & NSTitledWindowMask )
    {
        if ( ( style & wxMINIMIZE_BOX ) )
            windowstyle |= NSMiniaturizableWindowMask ;

        if ( ( style & wxMAXIMIZE_BOX ) )
            windowstyle |= NSResizableWindowMask ; // TODO showing ZOOM ?

        if ( ( style & wxRESIZE_BORDER ) )
            windowstyle |= NSResizableWindowMask ;

        if ( ( style & wxCLOSE_BOX) )
            windowstyle |= NSClosableWindowMask ;
    }
    if ( extraStyle & wxFRAME_EX_METAL)
        windowstyle |= NSTexturedBackgroundWindowMask;

    if ( ( style & wxSTAY_ON_TOP ) )
        level = kCGUtilityWindowLevel;
/*
    if ( ( style & wxFRAME_FLOAT_ON_PARENT ) )
        group = GetWindowGroupOfClass(kFloatingWindowClass);
        */
    
    NSRect r = wxToNSRect( NULL, wxRect( pos, size) );
    
    [m_macWindow setImplementation:this];
    
    [m_macWindow initWithContentRect:r
        styleMask:windowstyle
        backing:NSBackingStoreBuffered
        defer:NO 
        ];
        
    [m_macWindow setLevel:level];

    [m_macWindow setDelegate:controller];
    
    // [m_macWindow makeKeyAndOrderFront:nil];
}


WXWindow wxNonOwnedWindowCocoaImpl::GetWXWindow() const
{
    return m_macWindow;
}

void wxNonOwnedWindowCocoaImpl::Raise()
{
    [m_macWindow orderWindow:NSWindowAbove relativeTo:0];
}
    
void wxNonOwnedWindowCocoaImpl::Lower()
{
    [m_macWindow orderWindow:NSWindowBelow relativeTo:0];
}

bool wxNonOwnedWindowCocoaImpl::Show(bool show)
{
    if ( show )
    {
        [m_macWindow orderFront:nil];
        [[m_macWindow contentView] setNeedsDisplay:YES];
    }
    else 
        [m_macWindow orderOut:nil];
    return true;
}
    
bool wxNonOwnedWindowCocoaImpl::ShowWithEffect(bool show, wxShowEffect effect, unsigned timeout)
{
    return Show(show);
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
            wxFAIL_MSG( _T("Metal Style cannot be changed after creation") );
        }
        else if ( !metal && (windowStyle & NSTexturedBackgroundWindowMask ) )
        {
            wxFAIL_MSG( _T("Metal Style cannot be changed after creation") );
        }        
    }
}
    
bool wxNonOwnedWindowCocoaImpl::SetBackgroundStyle(wxBackgroundStyle style)
{
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
    width = rect.size.width;
    height = rect.size.height;
}

void wxNonOwnedWindowCocoaImpl::GetContentArea( int& left, int &right, int &width, int &height ) const
{
    NSRect outer = NSMakeRect(100,100,100,100);
    NSRect content = [NSWindow contentRectForFrameRect:outer styleMask:[m_macWindow styleMask] ];
    NSRect rect = [[m_macWindow contentView] frame];
    width = rect.size.width;
    height = rect.size.height;
}
    
bool wxNonOwnedWindowCocoaImpl::SetShape(const wxRegion& region)
{
    return false;
}

void wxNonOwnedWindowCocoaImpl::SetTitle( const wxString& title, wxFontEncoding encoding ) 
{
    [m_macWindow setTitle:wxCFStringRef( title , encoding ).AsNSString()];
}
    
bool wxNonOwnedWindowCocoaImpl::IsMaximized() const
{
    return [m_macWindow isZoomed];
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
    
void wxNonOwnedWindowCocoaImpl::Maximize(bool maximize)
{
    [m_macWindow zoom:nil];
}
    

// http://cocoadevcentral.com/articles/000028.php

typedef struct
{
    int m_formerLevel;
    NSRect m_formerFrame;
} FullScreenData ;

bool wxNonOwnedWindowCocoaImpl::IsFullScreen() const
{
    return m_macFullScreenData != NULL ;
}
    
bool wxNonOwnedWindowCocoaImpl::ShowFullScreen(bool show, long style)
{
    if ( show )
    {
        FullScreenData *data = (FullScreenData *)m_macFullScreenData ;
        delete data ;
        data = new FullScreenData();

        m_macFullScreenData = data ;
        data->m_formerLevel = [m_macWindow level];
        data->m_formerFrame = [m_macWindow frame];
        CGDisplayCapture( kCGDirectMainDisplay );
        [m_macWindow setLevel:CGShieldingWindowLevel()];
        [m_macWindow setFrame:[[NSScreen mainScreen] frame] display:YES];
    }
    else if ( m_macFullScreenData != NULL )
    {
        FullScreenData *data = (FullScreenData *) m_macFullScreenData ;
        CGDisplayRelease( kCGDirectMainDisplay );
        [m_macWindow setLevel:data->m_formerLevel];
        [m_macWindow setFrame:data->m_formerFrame display:YES];
        delete data ;
        m_macFullScreenData = NULL ;
    }
    
    return true;
}

void wxNonOwnedWindowCocoaImpl::RequestUserAttention(int WXUNUSED(flags))
{
}

void wxNonOwnedWindowCocoaImpl::ScreenToWindow( int *x, int *y )
{
    wxPoint p((x ? *x : 0), (y ? *y : 0) );
    NSPoint nspt = wxToNSPoint( NULL, p );
    nspt = [m_macWindow convertScreenToBase:nspt];
    nspt = [[m_macWindow contentView] convertPoint:nspt fromView:nil];
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
    nspt = [m_macWindow convertBaseToScreen:nspt];
    p = wxFromNSPoint( NULL, nspt);
    if ( x )
        *x = p.x;
    if ( y )
        *y = p.y;
}

wxNonOwnedWindowImpl* wxNonOwnedWindowImpl::CreateNonOwnedWindow( wxNonOwnedWindow* wxpeer, wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name )
{
    wxNonOwnedWindowImpl* now = new wxNonOwnedWindowCocoaImpl( wxpeer );
    now->Create( parent, pos, size, style , extraStyle, name );
    return now;
}
