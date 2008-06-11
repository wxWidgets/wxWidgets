/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/msgdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
#endif

#include "wx/thread.h"
#include "wx/mac/uma.h"


IMPLEMENT_CLASS(wxMessageDialog, wxDialog)


wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
               : wxMessageDialogBase(parent, message, caption, style)
{
    m_yes = _("Yes");
    m_no  = _("No");
    m_ok  = _("OK");
    m_cancel = _("Cancel");
}

bool wxMessageDialog::SetYesNoLabels(const wxString& yes,const wxString& no)
{
    m_yes = yes;
    m_no = no;
    return true;
}

bool wxMessageDialog::SetYesNoCancelLabels(const wxString& yes, const wxString& no, const wxString& cancel)
{
    m_yes = yes;
    m_no = no;
    m_cancel = cancel;
    return true;
}

bool wxMessageDialog::SetOKLabel(const wxString& ok)
{
    m_ok = ok;
    return true;
}

bool wxMessageDialog::SetOKCancelLabels(const wxString& ok, const wxString& cancel)
{
    m_ok = ok;
    m_cancel = cancel;
    return true;
}

int wxMessageDialog::ShowModal()
{
    int resultbutton = wxID_CANCEL;

    const long style = GetMessageDialogStyle();

    wxASSERT_MSG( (style & 0x3F) != wxYES, wxT("this style is not supported on Mac") );

    AlertType alertType = kAlertPlainAlert;
    if (style & wxICON_EXCLAMATION)
        alertType = kAlertCautionAlert;
    else if (style & wxICON_HAND)
        alertType = kAlertStopAlert;
    else if (style & wxICON_INFORMATION)
        alertType = kAlertNoteAlert;
    else if (style & wxICON_QUESTION)
        alertType = kAlertNoteAlert;


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
        CFStringRef defaultButtonTitle = NULL;
        CFStringRef alternateButtonTitle = NULL;
        CFStringRef otherButtonTitle = NULL;

        wxCFStringRef cfTitle( msgtitle, GetFont().GetEncoding() );
        wxCFStringRef cfText( msgtext, GetFont().GetEncoding() );

        wxCFStringRef cfNoString( m_no.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfYesString( m_yes.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfOKString( m_ok.c_str() , GetFont().GetEncoding()) ;
        wxCFStringRef cfCancelString( m_cancel.c_str(), GetFont().GetEncoding() );

        int buttonId[4] = { 0, 0, 0, wxID_CANCEL /* time-out */ };

        if (style & wxYES_NO)
        {
            if ( style & wxNO_DEFAULT )
            {
                defaultButtonTitle = cfNoString;
                alternateButtonTitle = cfYesString;
                buttonId[0] = wxID_NO;
                buttonId[1] = wxID_YES;
            }
            else
            {
                defaultButtonTitle = cfYesString;
                alternateButtonTitle = cfNoString;
                buttonId[0] = wxID_YES;
                buttonId[1] = wxID_NO;
            }
            if (style & wxCANCEL)
            {
                otherButtonTitle = cfCancelString;
                buttonId[2] = wxID_CANCEL;
            }
        }
        else
        {
            // the MSW implementation even shows an OK button if it is not specified, we'll do the same
            buttonId[0] = wxID_OK;
            // using null as default title does not work on earlier systems
            defaultButtonTitle = cfOKString;
            if (style & wxCANCEL)
            {
                alternateButtonTitle = cfCancelString;
                buttonId[1] = wxID_CANCEL;
            }
        }

        CFOptionFlags exitButton;
        OSStatus err = CFUserNotificationDisplayAlert(
            0, alertType, NULL, NULL, NULL, cfTitle, cfText,
            defaultButtonTitle, alternateButtonTitle, otherButtonTitle, &exitButton );
        if (err == noErr)
            resultbutton = buttonId[exitButton];
    }
    else
    {
        short result;

        AlertStdCFStringAlertParamRec param;
        wxCFStringRef cfNoString( m_no.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfYesString( m_yes.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfOKString( m_ok.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfCancelString( m_cancel.c_str(), GetFont().GetEncoding() );

        wxCFStringRef cfTitle( msgtitle, GetFont().GetEncoding() );
        wxCFStringRef cfText( msgtext, GetFont().GetEncoding() );

        param.movable = true;
        param.flags = 0;
        param.version = kStdCFStringAlertVersionOne;

        bool skipDialog = false;

        if (style & wxYES_NO)
        {
            if (style & wxCANCEL)
            {
                param.defaultText = cfYesString;
                param.cancelText = cfCancelString;
                param.otherText = cfNoString;
                param.helpButton = false;
                param.defaultButton = style & wxNO_DEFAULT ? kAlertStdAlertOtherButton : kAlertStdAlertOKButton;
                param.cancelButton = kAlertStdAlertCancelButton;
            }
            else
            {
                param.defaultText = cfYesString;
                param.cancelText = NULL;
                param.otherText = cfNoString;
                param.helpButton = false;
                param.defaultButton = style & wxNO_DEFAULT ? kAlertStdAlertOtherButton : kAlertStdAlertOKButton;
                param.cancelButton = 0;
            }
        }
        // the MSW implementation even shows an OK button if it is not specified, we'll do the same
        else
        {
            if (style & wxCANCEL)
            {
                // that's a cancel missing
                param.defaultText = cfOKString;
                param.cancelText = cfCancelString;
                param.otherText = NULL;
                param.helpButton = false;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton = 0;
            }
            else
            {
                param.defaultText = cfOKString;
                param.cancelText = NULL;
                param.otherText = NULL;
                param.helpButton = false;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton = 0;
            }
        }

        param.position = kWindowDefaultPosition;
        if ( !skipDialog )
        {
            DialogRef alertRef;
            CreateStandardAlert( alertType, cfTitle, cfText, &param, &alertRef );
            RunStandardAlert( alertRef, NULL, &result );
        }
        else
        {
            return wxID_CANCEL;
        }

        if (style & wxOK)
        {
            switch ( result )
            {
            case 1:
                resultbutton = wxID_OK;
                break;

            case 2:
                // TODO: add Cancel button
                // if (style & wxCANCEL)
                //     resultbutton = wxID_CANCEL;
                break;

            case 3:
            default:
                break;
            }
        }
        else if (style & wxYES_NO)
        {
            switch ( result )
            {
            case 1:
                resultbutton = wxID_YES;
                break;

            case 2:
                if (!(style & wxCANCEL))
                    resultbutton = wxID_CANCEL;
                break;

            case 3:
                resultbutton = wxID_NO;
                break;

            default:
                break;
            }
        }
    }

    return resultbutton;
}
