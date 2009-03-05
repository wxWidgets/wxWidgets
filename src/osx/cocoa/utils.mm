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
#if wxOSX_USE_COCOA_OR_CARBON
    #include <CoreServices/CoreServices.h>
    #include "wx/osx/dcclient.h"
    #include "wx/osx/private/timer.h"
#endif
#endif // wxUSE_GUI

#if wxOSX_USE_COCOA

#if wxUSE_BASE

// Emit a beeeeeep
void wxBell()
{
    NSBeep();
}

// ----------------------------------------------------------------------------
// Common Event Support
// ----------------------------------------------------------------------------

void wxMacWakeUp()
{
    // TODO
}

#endif // wxUSE_BASE

#if wxUSE_GUI

@interface wxNSAppController : NSObject
{
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;
- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename;
- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender;
- (BOOL)application:(NSApplication *)sender printFile:(NSString *)filename;
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event
    withReplyEvent:(NSAppleEventDescriptor *)replyEvent;
@end

@implementation wxNSAppController

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    // let wx do this, not cocoa
    return NO;
}

- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename;
{
    wxCFStringRef cf(wxCFRetain(filename));
    wxTheApp->MacOpenFile(cf.AsString()) ;
    return YES;
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender;
{
    wxTheApp->MacNewFile() ;
    return NO;
}

- (BOOL)application:(NSApplication *)sender printFile:(NSString *)filename
{
    wxCFStringRef cf(wxCFRetain(filename));
    wxTheApp->MacPrintFile(cf.AsString()) ;
    return YES;
}

/*
    Allowable return values are:
        NSTerminateNow - it is ok to proceed with termination
        NSTerminateCancel - the application should not be terminated
        NSTerminateLater - it may be ok to proceed with termination later.  The application must call -replyToApplicationShouldTerminate: with YES or NO once the answer is known
            this return value is for delegates who need to provide document modal alerts (sheets) in order to decide whether to quit.
*/
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    wxWindow* win = wxTheApp->GetTopWindow() ;
    if ( win )
    {
        wxCommandEvent exitEvent(wxEVT_COMMAND_MENU_SELECTED, wxApp::s_macExitMenuItemId);
        if (!win->GetEventHandler()->ProcessEvent(exitEvent))
            win->Close(true) ;
    }
    else
    {
         wxTheApp->ExitMainLoop() ;
    }
    return NSTerminateCancel;
}

- (BOOL)applicationShouldHandleReopen:(NSApplication *)sender hasVisibleWindows:(BOOL)flag
{
    wxTheApp->MacReopenApp() ;
    return NO;
}

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event
    withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
    NSString* url = [[event descriptorAtIndex:1] stringValue];
    wxCFStringRef cf(wxCFRetain(url));
    wxTheApp->MacOpenURL(cf.AsString()) ;
}
@end

/* 
    allows ShowModal to work when using sheets.
    see include/wx/osx/cocoa/private.h for more info 
*/
@implementation ModalDialogDelegate
- (id)init
{
    [super init];
    sheetFinished = NO;
    resultCode = -1;
    return self;
}

- (BOOL)finished 
{
    return sheetFinished;
}

- (int)code
{
    return resultCode;
}

- (void)waitForSheetToFinish
{
    while (!sheetFinished)
    {
        wxSafeYield();
    }
}

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
    resultCode = returnCode;
    sheetFinished = YES;
    [sheet orderOut: self];
}
@end

bool wxApp::DoInitGui()
{
    [NSApplication sharedApplication];

    if (!sm_isEmbedded)
    {
        wxNSAppController* controller = [[wxNSAppController alloc] init];
        [[NSApplication sharedApplication] setDelegate:controller];

        NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
        [appleEventManager setEventHandler:controller andSelector:@selector(handleGetURLEvent:withReplyEvent:)
            forEventClass:kInternetEventClass andEventID:kAEGetURL];
    }
    [NSApp finishLaunching];
    return true;
}

void wxApp::DoCleanUp()
{
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    NSRect displayRect = [[NSScreen mainScreen] visibleFrame];
    wxRect r = wxFromNSRect( NULL, displayRect );
    if ( x )
        *x = r.x;
    if ( y )
        *y = r.y;
    if ( width )
        *width = r.GetWidth();
    if ( height )
        *height = r.GetHeight();

}

void wxGetMousePosition( int* x, int* y )
{
    wxPoint pt = wxFromNSPoint(NULL, [NSEvent mouseLocation]);
    if ( x )
        *x = pt.x;
    if ( y )
        *y = pt.y;
};

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

void wxMacGlobalToLocal( WindowRef window , Point*pt )
{
}

void wxMacLocalToGlobal( WindowRef window , Point*pt )
{
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

    NSRect rect = NSMakeRect(left, top, width, height );
    NSView* view = (NSView*) m_window->GetHandle();
    [view lockFocus];
    // we use this method as other methods force a repaint, and this method can be
    // called from OnPaint, even with the window's paint dc as source (see wxHTMLWindow)
    NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc] initWithFocusedViewRect: [view bounds]] retain];
    [view unlockFocus];

    CGImageRef cgImageRef = (CGImageRef)[rep CGImage];

    wxBitmap bitmap(CGImageGetWidth(cgImageRef)  , CGImageGetHeight(cgImageRef) );
    CGRect r = CGRectMake( 0 , 0 , CGImageGetWidth(cgImageRef)  , CGImageGetHeight(cgImageRef) );
    // since our context is upside down we dont use CGContextDrawImage
    wxMacDrawCGImage( (CGContextRef) bitmap.GetHBITMAP() , &r, cgImageRef ) ;
    CGImageRelease(cgImageRef);
    cgImageRef = NULL;
    [rep release];

    return bitmap;
}

#endif // wxUSE_GUI

#endif // wxOSX_USE_COCOA