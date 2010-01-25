/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/dialog.cpp
// Purpose:     wxDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
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
#include "wx/evtloop.h"

void wxDialog::EndWindowModal()
{
    // Nothing to do for now.
}

void wxDialog::DoShowWindowModal()
{
    // If someone wants to add support for this to wxOSX Carbon, here would 
    // be the place to start: http://trac.wxwidgets.org/ticket/9459
    // Unfortunately, supporting sheets in Carbon isn't as straightforward
    // as with Cocoa, so it will probably take some tweaking.
    wxDialogBase::ShowWindowModal();
}

void wxDialog::DoShowModal()
{

    SetFocus() ;

    WindowRef windowRef = (WindowRef) GetWXWindow();
    WindowGroupRef windowGroup = NULL;
    WindowGroupRef formerParentGroup = NULL;
    bool resetGroupParent = false;

    if ( GetParent() == NULL )
    {
        windowGroup = GetWindowGroup(windowRef) ;
        if ( windowGroup != GetWindowGroupOfClass( kMovableModalWindowClass ) )
        {
            formerParentGroup = GetWindowGroupParent( windowGroup );
            SetWindowGroupParent( windowGroup, GetWindowGroupOfClass( kMovableModalWindowClass ) );
            resetGroupParent = true;
        }
    }
    BeginAppModalStateForWindow(windowRef) ;

#if wxUSE_CONSOLE_EVENTLOOP
    wxEventLoopGuarantor ensureHasLoop;
#endif
    wxEventLoopBase * const loop = wxEventLoop::GetActive();
    while ( IsModal() )
        loop->Dispatch();

    EndAppModalStateForWindow(windowRef) ;
    if ( resetGroupParent )
    {
        SetWindowGroupParent( windowGroup , formerParentGroup );
    }
}
