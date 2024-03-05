/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/msgdlg.mm
// Purpose:     wxMessageDialog
// Author:      Stefan Csomor
// Created:     04/01/98
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/msgdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
#endif

#include "wx/control.h"
#include "wx/thread.h"
#include "wx/evtloop.h"
#include "wx/modalhook.h"
#include "wx/osx/private.h"


wxIMPLEMENT_CLASS(wxMessageDialog, wxDialog);


namespace 
{
    NSAlertStyle GetAlertStyleFromWXStyle( long style )
    {
        if (style & (wxICON_WARNING | wxICON_ERROR))
        {
            // NSCriticalAlertStyle should only be used for questions where
            // caution is needed per the OS X HIG. wxICON_WARNING alone doesn't
            // warrant it, but a question with a warning (rather than question)
            // icon is something serious.
            if (style & (wxYES_NO | wxCANCEL))
                return NSCriticalAlertStyle;
            else
                return NSWarningAlertStyle;
        }
        else
            return NSInformationalAlertStyle;
    }
}

wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
               : wxMessageDialogBase(parent, message, caption, style)
{
}

int wxMessageDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxCFEventLoopPauseIdleEvents pause;
    
    const long style = GetMessageDialogStyle();

    wxASSERT_MSG( (style & 0x3F) != wxYES, wxT("this style is not supported on Mac") );

    // work out what to display
    // if the extended text is empty then we use the caption as the title
    // and the message as the text (for backwards compatibility)
    // but if the extended message is not empty then we use the message as the title
    // and the extended message as the text because that makes more sense

    wxString msgtitle,msgtext;
    if(m_extendedMessage.IsEmpty())
    {
        msgtitle = m_caption;
        msgtext  = m_message;
    }
    else
    {
        msgtitle = m_message;
        msgtext  = m_extendedMessage;
    }


    if ( !wxIsMainThread() )
    {
        CFStringRef defaultButtonTitle = nullptr;
        CFStringRef alternateButtonTitle = nullptr;
        CFStringRef otherButtonTitle = nullptr;

        wxCFStringRef cfTitle( msgtitle );
        wxCFStringRef cfText( msgtext );

        wxCFStringRef cfNoString( wxControl::GetLabelText(GetNoLabel()) );
        wxCFStringRef cfYesString( wxControl::GetLabelText(GetYesLabel()) );
        wxCFStringRef cfOKString( wxControl::GetLabelText(GetOKLabel()) ) ;
        wxCFStringRef cfCancelString( wxControl::GetLabelText(GetCancelLabel()) );

        NSAlertStyle alertType = GetAlertStyleFromWXStyle(style);
                
        int m_buttonId[4] = { 0, 0, 0, wxID_CANCEL /* time-out */ };

        if (style & wxYES_NO)
        {
            if ( style & wxNO_DEFAULT )
            {
                defaultButtonTitle = cfNoString;
                alternateButtonTitle = cfYesString;
                m_buttonId[0] = wxID_NO;
                m_buttonId[1] = wxID_YES;
            }
            else
            {
                defaultButtonTitle = cfYesString;
                alternateButtonTitle = cfNoString;
                m_buttonId[0] = wxID_YES;
                m_buttonId[1] = wxID_NO;
            }
            if (style & wxCANCEL)
            {
                otherButtonTitle = cfCancelString;
                m_buttonId[2] = wxID_CANCEL;
            }
        }
        else
        {
            // the MSW implementation even shows an OK button if it is not specified, we'll do the same
            m_buttonId[0] = wxID_OK;
            // using null as default title does not work on earlier systems
            defaultButtonTitle = cfOKString;
            if (style & wxCANCEL)
            {
                alternateButtonTitle = cfCancelString;
                m_buttonId[1] = wxID_CANCEL;
            }
        }

        wxASSERT_MSG( !(style & wxHELP), "wxHELP not supported in non-GUI thread" );

        CFOptionFlags exitButton;
        OSStatus err = CFUserNotificationDisplayAlert(
            0, alertType, nullptr, nullptr, nullptr, cfTitle, cfText,
            defaultButtonTitle, alternateButtonTitle, otherButtonTitle, &exitButton );
        if (err == noErr)
            SetReturnCode( m_buttonId[exitButton] );
        else
            SetReturnCode( wxID_CANCEL );
    }
    else
    {
        NSAlert* alert = (NSAlert*)ConstructNSAlert();

        OSXBeginModalDialog();

        int button = -1;
        button = [alert runModal];
        
        OSXEndModalDialog();

        ModalFinishedCallback(alert, button);
        [alert release];
    }

    return GetReturnCode();
}

void wxMessageDialog::ShowWindowModal()
{
    wxNonOwnedWindow* parentWindow = nullptr;

    m_modality = wxDIALOG_MODALITY_WINDOW_MODAL;

    if (GetParent())
        parentWindow = dynamic_cast<wxNonOwnedWindow*>(wxGetTopLevelParent(GetParent()));

    wxASSERT_MSG(parentWindow, "Window modal display requires parent.");

    if (parentWindow)
    {
        NSAlert* alert = (NSAlert*)ConstructNSAlert();
        
        NSWindow* nativeParent = parentWindow->GetWXWindow();
        [alert beginSheetModalForWindow:nativeParent  completionHandler:
         ^(NSModalResponse returnCode)
        {
            this->ModalFinishedCallback(alert, returnCode);
        }];
    }
}

void wxMessageDialog::ModalFinishedCallback(void* WXUNUSED(panel), int resultCode)
{
    int resultbutton = wxID_CANCEL;
    if ( resultCode < NSAlertFirstButtonReturn )
        resultbutton = wxID_CANCEL;
    else
    {
        if ( resultCode - NSAlertFirstButtonReturn < m_buttonCount )
            resultbutton = m_buttonId[ resultCode - NSAlertFirstButtonReturn ];
        else
            resultbutton = wxID_CANCEL;
    }
    SetReturnCode(resultbutton);
    
    if (GetModality() == wxDIALOG_MODALITY_WINDOW_MODAL)
        SendWindowModalDialogEvent ( wxEVT_WINDOW_MODAL_DIALOG_CLOSED  );
}

void* wxMessageDialog::ConstructNSAlert()
{
    const long style = GetMessageDialogStyle();

    wxASSERT_MSG( (style & 0x3F) != wxYES, wxT("this style is not supported on Mac") );

    // work out what to display
    // if the extended text is empty then we use the caption as the title
    // and the message as the text (for backwards compatibility)
    // but if the extended message is not empty then we use the message as the title
    // and the extended message as the text because that makes more sense

    wxString msgtitle,msgtext;
    if(m_extendedMessage.IsEmpty())
    {
        msgtitle = m_caption;
        msgtext  = m_message;
    }
    else
    {
        msgtitle = m_message;
        msgtext  = m_extendedMessage;
    }

    NSAlert* alert = [[NSAlert alloc] init];
    NSAlertStyle alertType = GetAlertStyleFromWXStyle(style);

    wxCFStringRef cfNoString( wxControl::GetLabelText(GetNoLabel()) );
    wxCFStringRef cfYesString( wxControl::GetLabelText(GetYesLabel()) );
    wxCFStringRef cfOKString( wxControl::GetLabelText(GetOKLabel()) );
    wxCFStringRef cfCancelString( wxControl::GetLabelText(GetCancelLabel()) );

    wxCFStringRef cfTitle( msgtitle );
    wxCFStringRef cfText( msgtext );

    [alert setMessageText:cfTitle.AsNSString()];
    [alert setInformativeText:cfText.AsNSString()];
    [alert setAlertStyle:alertType];

    m_buttonCount = 0;

    if (style & wxYES_NO)
    {
        if ( style & wxNO_DEFAULT )
        {
            [alert addButtonWithTitle:cfNoString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_NO;
            [alert addButtonWithTitle:cfYesString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_YES;
        }
        else
        {
            [alert addButtonWithTitle:cfYesString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_YES;
            [alert addButtonWithTitle:cfNoString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_NO;
        }

        if (style & wxCANCEL)
        {
            [alert addButtonWithTitle:cfCancelString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_CANCEL;
        }
    }
    // the MSW implementation even shows an OK button if it is not specified, we'll do the same
    else
    {
        if ( style & wxCANCEL_DEFAULT )
        {
            [alert addButtonWithTitle:cfCancelString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_CANCEL;

            [alert addButtonWithTitle:cfOKString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_OK;
        }
        else 
        {
            [alert addButtonWithTitle:cfOKString.AsNSString()];
            m_buttonId[ m_buttonCount++ ] = wxID_OK;
            if (style & wxCANCEL)
            {
                [alert addButtonWithTitle:cfCancelString.AsNSString()];
                m_buttonId[ m_buttonCount++ ] = wxID_CANCEL;
            }
        }

    }

    if ( style & wxHELP )
    {
        wxCFStringRef cfHelpString( GetHelpLabel() );
        [alert addButtonWithTitle:cfHelpString.AsNSString()];
        m_buttonId[ m_buttonCount++ ] = wxID_HELP;
    }

    wxASSERT_MSG( m_buttonCount <= (int)WXSIZEOF(m_buttonId), "Too many buttons" );

    return alert;
}
