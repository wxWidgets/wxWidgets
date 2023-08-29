/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/utils.mm
// Purpose:     various cocoa utility functions
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "wx/private/display.h"

#include "wx/osx/private.h"

#if wxUSE_GUI
    #include "wx/private/launchbrowser.h"
    #include "wx/osx/private/timer.h"
    #include "wx/osx/dcclient.h"
#endif // wxUSE_GUI

#if wxOSX_USE_IPHONE

#include <AudioToolbox/AudioServices.h>

#if 1 // wxUSE_BASE

// ----------------------------------------------------------------------------
// Common Event Support
// ----------------------------------------------------------------------------

@interface wxAppDelegate : NSObject <UIApplicationDelegate> {
}

@end

@implementation wxAppDelegate

- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    wxUnusedVar(application);
    wxUnusedVar(launchOptions);
    wxTheApp->OSXOnWillFinishLaunching();
    return YES;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    wxTheApp->OSXOnDidFinishLaunching();
}

- (void)applicationWillTerminate:(UIApplication *)application { 
    wxUnusedVar(application);
    wxTheApp->OSXOnWillTerminate();
}

- (void)dealloc {
    [super dealloc];
}


@end

bool wxApp::CallOnInit()
{
    return true;
}

bool wxApp::DoInitGui()
{
    return true;
}

void wxApp::DoCleanUp()
{
}

#endif // wxUSE_BASE

#if wxUSE_GUI

// Emit a beeeeeep
void wxBell()
{
    // would be kSystemSoundID_UserPreferredAlert but since the headers aren't correct, add it manually
    AudioServicesPlayAlertSound(0x00001000 );
}

// ----------------------------------------------------------------------------
// Launch default browser
// ----------------------------------------------------------------------------

bool wxDoLaunchDefaultBrowser(const wxLaunchBrowserParams& params)
{
    return [[UIApplication sharedApplication] openURL:[NSURL URLWithString:wxCFStringRef(params.url).AsNSString()]]
        == YES;
}

// TODO : reorganize

CFArrayRef CopyAvailableFontFamilyNames()
{
    return (CFArrayRef) [[UIFont familyNames] retain];
}

void wxGetMousePosition( int* x, int* y )
{
    if ( x )
        *x = 0;
    if ( y )
        *y = 0;
};

wxMouseState wxGetMouseState()
{
    wxMouseState ms;
    return ms;
}    

// Get size of display

class wxDisplayImplSingleiOS : public wxDisplayImplSingle
{
public:
    virtual wxRect GetGeometry() const override
    {
        CGRect bounds = [[UIScreen mainScreen] bounds];

        int width, height;
        if ( UIInterfaceOrientationIsPortrait([[UIApplication sharedApplication] statusBarOrientation]) )
        {
            // portrait
            width = (int)bounds.size.width ;
            height = (int)bounds.size.height;
        }
        else
        {
            // landscape
            width = (int)bounds.size.height ;
            height = (int)bounds.size.width;
        }

        return wxRect(0, 0, width, height);
    }

    virtual int GetDepth() const override
    {
        return 32; // TODO can we determine this ?
    }

    virtual wxSize GetPPI() const override
    {
        return wxSize(72, 72);
    }
};

class wxDisplayFactorySingleiOS : public wxDisplayFactorySingle
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay() override
    {
        return new wxDisplayImplSingleiOS;
    }
};

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleiOS;
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

    int left = subrect != nullptr ? subrect->x : 0 ;
    int top = subrect != nullptr ? subrect->y : 0 ;
    int width = subrect != nullptr ? subrect->width : sz.x;
    int height = subrect != nullptr ? subrect->height : sz.y ;

    wxBitmap bmp = wxBitmap(width, height, 32);

    CGContextRef context = (CGContextRef)bmp.GetHBITMAP();

    CGContextSaveGState(context);


    CGContextTranslateCTM( context, 0,  height );
    CGContextScaleCTM( context, 1, -1 );

    if ( subrect )
        CGContextTranslateCTM( context, -subrect->x, -subrect->y ) ;

    UIGraphicsPushContext(context);
    [ (UIView*) m_window->GetHandle() drawRect:CGRectMake(left, top, width, height ) ];
    UIGraphicsPopContext();
    CGContextRestoreGState(context);

    return bmp;
}

#endif // wxUSE_GUI

#endif // wxOSX_USE_IPHONE
