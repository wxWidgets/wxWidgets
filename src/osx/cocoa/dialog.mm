/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dialog.cpp
// Purpose:     wxDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: dialog.cpp 54820 2008-07-29 20:04:11Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dialog.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

extern wxList wxModalDialogs;

void wxDialog::DoShowModal()
{
    wxCHECK_RET( !IsModal(), wxT("DoShowModal() called twice") );

    // If the app hasn't started, flush the event queue
    // If we don't do this, the Dock doesn't get the message that
    // the app has started so will refuse to activate it.
    NSApplication *theNSApp = [NSApplication sharedApplication];
    if (![theNSApp isRunning])
    {
        wxMacAutoreleasePool pool;
        while(NSEvent *event = [theNSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES])
        {
            [theNSApp sendEvent:event];
        }
    }

    wxModalDialogs.Append(this);

    SetFocus() ;
/*
    WindowGroupRef windowGroup;
    WindowGroupRef formerParentGroup;
    bool resetGroupParent = false;

    if ( GetParent() == NULL )
    {
        windowGroup = GetWindowGroup(windowRef) ;
        formerParentGroup = GetWindowGroupParent( windowGroup );
        SetWindowGroupParent( windowGroup, GetWindowGroupOfClass( kMovableModalWindowClass ) );
        resetGroupParent = true;
    }
*/
    NSWindow* theWindow = GetWXWindow();

    NSModalSession session = [NSApp beginModalSessionForWindow:theWindow];
    while (IsModal())
    {
        wxMacAutoreleasePool autoreleasepool;
        // we cannot break based on the return value, because nested
        // alerts might set this to stopped as well, so it would be
        // unsafe
        [NSApp runModalSession:session];

        // break if ended, perform no further idle processing
        if (!IsModal())
            break;

        // do some idle processing
        bool needMore = false;
        if (wxTheApp)
        {
            wxTheApp->ProcessPendingEvents();
            needMore = wxTheApp->ProcessIdle();
        }
        
        if (!needMore)
        {
            // no more idle processing wanted - block until the next event
            [theNSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantFuture] inMode:NSDefaultRunLoopMode dequeue:NO];
        }
    }
    [NSApp endModalSession:session];

/*
    if ( resetGroupParent )
    {
        SetWindowGroupParent( windowGroup , formerParentGroup );
    }
*/
}
