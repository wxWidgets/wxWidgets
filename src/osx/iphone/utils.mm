/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/utils.mm
// Purpose:     various cocoa utility functions
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: utils.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #if wxUSE_GUI
        #include "wx/toplevel.h"
        #include "wx/font.h"
    #endif
#endif

#include "wx/apptrait.h"

#include "wx/osx/private.h"

#if wxUSE_GUI
    #include "wx/osx/private/timer.h"
    #include "wx/osx/dcclient.h"
#endif // wxUSE_GUI

#if wxOSX_USE_IPHONE

#include <AudioToolbox/AudioServices.h>

#if wxUSE_BASE

// Emit a beeeeeep
void wxBell()
{
    // would be kSystemSoundID_UserPreferredAlert but since the headers aren't correct, add it manually
    AudioServicesPlayAlertSound(0x00001000 );
}

// ----------------------------------------------------------------------------
// Common Event Support
// ----------------------------------------------------------------------------

@interface wxAppDelegate : NSObject <UIApplicationDelegate> {
}

@end

@implementation wxAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {	
	wxTheApp->OnInit();
}


- (void)dealloc {
	[super dealloc];
}


@end

bool wxApp::CallOnInit()
{
    return true;
}

int wxApp::OnRun()
{
    wxMacAutoreleasePool pool;
    char* appname = "test";
    UIApplicationMain( 1, &appname, nil, @"wxAppDelegate" );
    return 1;
}

void wxMacWakeUp()
{
    // TODO
}

#endif // wxUSE_BASE

#if wxUSE_GUI

// TODO : reorganize

extern wxFont* CreateNormalFont()
{
    return new wxFont([UIFont systemFontSize] , wxSWISS, wxNORMAL, wxNORMAL, FALSE, "Helvetica" );
}

extern wxFont* CreateSmallFont()
{
    return new wxFont([UIFont smallSystemFontSize] , wxSWISS, wxNORMAL, wxNORMAL, FALSE, "Helvetica" );
}

extern UIFont* CreateUIFont( const wxFont& font )
{
    return [UIFont fontWithName:wxCFStringRef(font.GetFaceName() ).AsNSString() size:font.GetPointSize()];
}

extern void DrawTextInContext( CGContextRef context, CGPoint where, UIFont *font, NSString* text )
{
    bool contextChanged = ( UIGraphicsGetCurrentContext() != context );
    if ( contextChanged )
        UIGraphicsPushContext(context);

    [text drawAtPoint:where withFont:font];

    if ( contextChanged )
        UIGraphicsPopContext();
}

extern CGSize MeasureTextInContext( UIFont *font, NSString* text )
{
    return  [text sizeWithFont:font];
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    CGRect r = [[UIScreen mainScreen] applicationFrame];
    if ( x )
        *x = r.origin.x;
    if ( y )
        *y = r.origin.y;
    if ( width )
        *width = r.size.width;
    if ( height )
        *height = r.size.height;
    
}

void wxGetMousePosition( int* x, int* y )
{
//    wxPoint pt = wxFromNSPoint(NULL, [NSEvent mouseLocation]);
};

// Returns depth of screen
int wxDisplayDepth()
{
    return 32; // TODO can we determine this ?
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
    
    if ( width )
        *width = (int)bounds.size.width ;
    if ( height )
        *height = (int)bounds.size.height;
}

wxTimerImpl* wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxOSXTimerImpl(timer);
}

int gs_wxBusyCursorCount = 0;
extern wxCursor    gMacCurrentCursor;
wxCursor        gMacStoredActiveCursor;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(const wxCursor *cursor)
{
    if (gs_wxBusyCursorCount++ == 0)
    {
        gMacStoredActiveCursor = gMacCurrentCursor;
        cursor->MacInstall();

        wxSetCursor(*cursor);
    }
    //else: nothing to do, already set
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    wxCHECK_RET( gs_wxBusyCursorCount > 0,
        wxT("no matching wxBeginBusyCursor() for wxEndBusyCursor()") );

    if (--gs_wxBusyCursorCount == 0)
    {
        gMacStoredActiveCursor.MacInstall();
        gMacStoredActiveCursor = wxNullCursor;

        wxSetCursor(wxNullCursor);
    }
}

// true if we're between the above two calls
bool wxIsBusy()
{
    return (gs_wxBusyCursorCount > 0);
}

bool wxGetKeyState (wxKeyCode key)
{
    return false;
}

wxBitmap wxWindowDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    // wxScreenDC is derived from wxWindowDC, so a screen dc will
    // call this method when a Blit is performed with it as a source.
    if (!m_window)
        return wxNullBitmap;
        
    wxSize sz = m_window->GetSize();
    
    int left = subrect != NULL ? subrect->x : 0 ;
    int top = subrect != NULL ? subrect->y : 0 ;
    int width = subrect != NULL ? subrect->width : sz.x;
    int height = subrect !=  NULL ? subrect->height : sz.y ;
    
    wxBitmap bmp = wxBitmap(width, height, 32);
    
    CGContextRef context = (CGContextRef)bmp.GetHBITMAP();
    
    CGContextSaveGState(context);
    
    
    CGContextTranslateCTM( context, 0,  height );
    CGContextScaleCTM( context, 1, -1 );

    if ( subrect )
        CGContextTranslateCTM( context, -subrect->x, -subrect->y ) ;

    UIGraphicsPushContext(context);
    [ (NSView*) m_window->GetHandle() drawRect:CGRectMake(left, top, width, height ) ];
    UIGraphicsPopContext();
    CGContextRestoreGState(context);

    return bmp;
}

#endif // wxUSE_GUI

wxOperatingSystemId wxGetOsVersion(int *verMaj, int *verMin)
{
    // get OS version
    int major, minor;

    wxString release = wxCFStringRef( [ [UIDevice currentDevice] systemVersion] ).AsString() ;

    if ( release.empty() ||
         wxSscanf(release.c_str(), wxT("%d.%d"), &major, &minor) != 2 )
    {
        // failed to get version string or unrecognized format
        major =
        minor = -1;
    }

    if ( verMaj )
        *verMaj = major;
    if ( verMin )
        *verMin = minor;

    return wxOS_MAC_OSX_DARWIN;
}

wxString wxGetOsDescription()
{
    wxString release = wxCFStringRef( [ [UIDevice currentDevice] systemName] ).AsString() ;

    return release;
}


#endif // wxOSX_USE_IPHONE