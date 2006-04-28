/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/msgdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
#endif

#include "wx/mac/uma.h"

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption,
                                 long style, const wxPoint& pos)
{
    m_caption = caption;
    m_message = message;
    m_parent = parent;
    SetMessageDialogStyle(style);
}

int wxMessageDialog::ShowModal()
{
    int resultbutton = wxID_CANCEL ;

    short result ;

    const long style = GetMessageDialogStyle();

    wxASSERT_MSG( ( style & 0x3F ) != wxYES , wxT("this style is not supported on mac") ) ;

    AlertType alertType = kAlertPlainAlert ;
    if (style & wxICON_EXCLAMATION)
        alertType = kAlertNoteAlert ;
    else if (style & wxICON_HAND)
        alertType = kAlertStopAlert ;
    else if (style & wxICON_INFORMATION)
        alertType = kAlertNoteAlert ;
    else if (style & wxICON_QUESTION)
        alertType = kAlertCautionAlert ;

#if TARGET_CARBON
    if ( UMAGetSystemVersion() >= 0x1000 )
    {
        AlertStdCFStringAlertParamRec param ;
        wxMacCFStringHolder cfNoString(_("No") , m_font.GetEncoding()) ;
        wxMacCFStringHolder cfYesString( _("Yes") , m_font.GetEncoding()) ;

        wxMacCFStringHolder cfTitle(m_caption , m_font.GetEncoding());
        wxMacCFStringHolder cfText(m_message , m_font.GetEncoding());

        param.movable = true;
        param.flags = 0 ;

        bool skipDialog = false ;

        if (style & wxYES_NO)
        {
            if (style & wxCANCEL)
            {
                param.defaultText     = cfYesString ;
                param.cancelText     = (CFStringRef) kAlertDefaultCancelText;
                param.otherText     = cfNoString ;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = kAlertStdAlertCancelButton;
            }
            else
            {
                param.defaultText     = cfYesString ;
                param.cancelText     = NULL;
                param.otherText     = cfNoString ;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = 0;
            }
        }
        // the msw implementation even shows an ok button if it is not specified, we'll do the same
        else
        {
            if (style & wxCANCEL)
            {
                // thats a cancel missing
                param.defaultText     = (CFStringRef) kAlertDefaultOKText ;
                param.cancelText     = (CFStringRef) kAlertDefaultCancelText ;
                param.otherText     = NULL;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = 0;
            }
            else
            {
                param.defaultText     = (CFStringRef) kAlertDefaultOKText ;
                param.cancelText     = NULL;
                param.otherText     = NULL;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = 0;
            }
        }
        /*
        else
        {
            skipDialog = true ;
        }
        */

        param.position = kWindowDefaultPosition;
        if ( !skipDialog )
        {
            DialogRef alertRef ;
            CreateStandardAlert( alertType , cfTitle , cfText , &param , &alertRef ) ;
            RunStandardAlert( alertRef , NULL , &result ) ;
        }
        if ( skipDialog )
            return wxID_CANCEL ;
    }
    else
#endif
    {
        AlertStdAlertParamRec    param;

        Str255 yesPString ;
        Str255 noPString ;

        Str255 pascalTitle ;
        Str255 pascalText ;
        wxMacStringToPascal( m_caption , pascalTitle ) ;
        wxMacStringToPascal( _("Yes") , yesPString ) ;
        wxMacStringToPascal(  _("No") , noPString ) ;
        wxMacStringToPascal( m_message , pascalText ) ;

        param.movable         = true;
        param.filterProc     = NULL ;
        if (style & wxYES_NO)
        {
            if (style & wxCANCEL)
            {
                param.defaultText     = yesPString ;
                param.cancelText     = (StringPtr) kAlertDefaultCancelText;
                param.otherText     = noPString ;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = kAlertStdAlertCancelButton;
            }
            else
            {
                param.defaultText     = yesPString ;
                param.cancelText     = NULL;
                param.otherText     = noPString ;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = 0;
            }
        }
        else if (style & wxOK)
        {
            if (style & wxCANCEL)
            {
                param.defaultText     = (StringPtr) kAlertDefaultOKText ;
                param.cancelText     = (StringPtr) kAlertDefaultCancelText ;
                param.otherText     = NULL;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = 0;
            }
            else
            {
                param.defaultText     = (StringPtr) kAlertDefaultOKText ;
                param.cancelText     = NULL;
                param.otherText     = NULL;
                param.helpButton     = false ;
                param.defaultButton = kAlertStdAlertOKButton;
                param.cancelButton     = 0;
            }
        }
        else
        {
            return resultbutton ;
        }

        param.position         = 0;

        StandardAlert( alertType, pascalTitle, pascalText, &param, &result );
    }

    if (style & wxOK)
    {
        if (style & wxCANCEL)
        {
            //TODO add Cancelbutton
            switch( result )
            {
            case 1 :
                resultbutton = wxID_OK ;
                break ;
            case 2 :
                break ;
            case 3 :
                break ;
            }
        }
        else
        {
            switch( result )
            {
            case 1 :
                resultbutton = wxID_OK ;
                break ;
            case 2 :
                break ;
            case 3 :
                break ;
            }
        }
    }
    else if (style & wxYES_NO)
    {
        if (style & wxCANCEL)
        {
            switch( result )
            {
            case 1 :
                resultbutton = wxID_YES ;
                break ;
            case 2 :
                resultbutton = wxID_CANCEL ;
                break ;
            case 3 :
                resultbutton = wxID_NO ;
                break ;
            }
        }
        else
        {
            switch( result )
            {
            case 1 :
                resultbutton = wxID_YES ;
                break ;
            case 2 :
                break ;
            case 3 :
                resultbutton = wxID_NO ;
                break ;
            }
        }
    }

    return resultbutton ;
}
