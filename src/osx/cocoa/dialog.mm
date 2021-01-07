/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/dialog.mm
// Purpose:     wxDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
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

void wxDialog::DoShowWindowModal()
{   
    wxTopLevelWindow* parent = static_cast<wxTopLevelWindow*>(wxGetTopLevelParent(GetParent()));
    
    wxASSERT_MSG(parent, "ShowWindowModal requires the dialog to have a parent.");
    
    NSWindow* parentWindow = parent->GetWXWindow();
    NSWindow* theWindow = GetWXWindow();
    
    [parentWindow beginSheet:theWindow completionHandler:
     ^(NSModalResponse returnCode)
    {
        this->ModalFinishedCallback(theWindow, returnCode);
    }];
}

void wxDialog::EndWindowModal()
{
    [NSApp endSheet: GetWXWindow()];
    [GetWXWindow() orderOut:GetWXWindow()];
}

bool wxDialog::OSXGetWorksWhenModal()
{
    bool worksWhenModal = false;

    NSWindow* nswindow = IsModal() ? GetWXWindow() : nil;
    if ( nswindow != nil )
    {
        if ( [nswindow isKindOfClass:[NSPanel class]] &&  [(NSPanel*)nswindow worksWhenModal] == YES )
        {
            [(NSPanel*)nswindow setWorksWhenModal:NO];
            worksWhenModal = true;
        }
    }
    return worksWhenModal;
}

void wxDialog::OSXSetWorksWhenModal(bool worksWhenModal)
{
    NSWindow* nswindow = IsModal() ? GetWXWindow() : nil;
    if ( nswindow != nil )
    {
        if ( [nswindow isKindOfClass:[NSPanel class]] &&  [(NSPanel*)nswindow worksWhenModal] == YES )
            [(NSPanel*)nswindow setWorksWhenModal:worksWhenModal];
    }
}
