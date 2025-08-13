/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/dialog.mm
// Purpose:     wxDialog class
// Author:      Stefan Csomor
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
#include "wx/log.h"

extern wxList wxModalDialogs;

void wxDialog::DoShowWindowModal()
{
    m_modality = wxDIALOG_MODALITY_APP_MODAL;
    ShowModal();
    SendWindowModalDialogEvent ( wxEVT_WINDOW_MODAL_DIALOG_CLOSED  );
}

void wxDialog::EndWindowModal()
{
    wxNonOwnedWindow *parent = dynamic_cast<wxNonOwnedWindow*>( GetParent() );
    wxASSERT_MSG( parent != nullptr, "dialog must have in a toplevel window parent for modal event loop" );

    wxWidgetIPhoneImpl *parentImpl = (wxWidgetIPhoneImpl*) parent->GetPeer();
    UIViewController *parentController = (UIViewController *) parentImpl->GetController();
    wxASSERT_MSG( parentController != nullptr, "dialog must have in a toplevel window parent for modal event loop" );

    wxWidgetIPhoneImpl *impl = (wxWidgetIPhoneImpl*) GetPeer();
    UIViewController *controller = (UIViewController *) impl->GetController();
    wxASSERT_MSG( controller != nullptr, "dialog must have in a toplevel window parent for modal event loop" );

    [parentController dismissViewControllerAnimated: YES completion: nil ];
}
