/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlg.h"
#endif

#include "wx/app.h"
#include "wx/msgdlg.h"
#include "wx/intl.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxMessageDialog, wxDialog)
#endif

short language = 0 ;

void wxMacConvertNewlines( const char *source , char * destination ) ;
void wxMacConvertNewlines( const char *source , char * destination )
{
	const char *s = source ;
	char *d = destination ;
	
	while( *s  )
	{
		switch( *s )
		{
			case 0x0a :
				*d++ = 0x0d ;
				++s ;
				break ;
			case 0x0d :
				*d++ = 0x0d ;
				++s ;
				if ( *s == 0x0a )
					++s ;
				break ;
			default :
				*d++ = *s++ ;
				break ;
		}
	}
	*d = 0 ;
}

wxMessageDialog::wxMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        long style, const wxPoint& pos)
{
    m_caption = caption;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
}

int wxMessageDialog::ShowModal()
{
	int resultbutton = wxID_CANCEL ;

	short result ;
	Str255 pascalTitle ;
	Str255 pascalText ;
	char   cText[2048] ;

	Str255 yesPString ;
	Str255 noPString ;
	
	wxMacStringToPascal( m_caption , pascalTitle ) ;
	wxMacStringToPascal( _("Yes") , yesPString ) ;
	wxMacStringToPascal(  _("No") , noPString ) ;
	
	if (wxApp::s_macDefaultEncodingIsPC)
	{
		strcpy(cText , wxMacMakeMacStringFromPC( m_message) ) ;
	}
	else
	{
		strcpy( cText , m_message ) ;
	}

	wxMacConvertNewlines( cText , cText ) ;
	CopyCStringToPascal( cText , pascalText ) ;

	wxASSERT_MSG( ( m_dialogStyle & 0x3F ) != wxYES , "this style is not supported on mac" ) ;


	AlertStdAlertParamRec	param;

	param.movable 		= true;
	param.filterProc 	= NULL ;
	
  if (m_dialogStyle & wxYES_NO)
  {
    if (m_dialogStyle & wxCANCEL)
    {
			param.defaultText 	= yesPString ;
			param.cancelText 	= (StringPtr) kAlertDefaultCancelText;
			param.otherText 	= noPString ;
			param.helpButton 	= false ;
			param.defaultButton = kAlertStdAlertOKButton;
			param.cancelButton 	= kAlertStdAlertCancelButton;
    }
    else
    {
			param.defaultText 	= yesPString ;
			param.cancelText 	= NULL;
			param.otherText 	= noPString ;
			param.helpButton 	= false ;
			param.defaultButton = kAlertStdAlertOKButton;
			param.cancelButton 	= 0;
    }
  }
  else if (m_dialogStyle & wxOK)
  {
    if (m_dialogStyle & wxCANCEL)
    {
    	// thats a cancel missing
			param.defaultText 	= (StringPtr) kAlertDefaultOKText ;
			param.cancelText 	= NULL;
			param.otherText 	= NULL;
			param.helpButton 	= false ;
			param.defaultButton = kAlertStdAlertOKButton;
			param.cancelButton 	= 0;
    }
    else
    {
			param.defaultText 	= (StringPtr) kAlertDefaultOKText ;
			param.cancelText 	= NULL;
			param.otherText 	= NULL;
			param.helpButton 	= false ;
			param.defaultButton = kAlertStdAlertOKButton;
			param.cancelButton 	= 0;
    }
  }
	else
	{
		return resultbutton ;
	}

	param.position 		= 0;

  if (m_dialogStyle & wxICON_EXCLAMATION)
			StandardAlert( kAlertNoteAlert, pascalTitle, pascalText, &param, &result );
  else if (m_dialogStyle & wxICON_HAND)
			StandardAlert( kAlertStopAlert, pascalTitle, pascalText, &param, &result );
  else if (m_dialogStyle & wxICON_INFORMATION)
		StandardAlert( kAlertNoteAlert, pascalTitle, pascalText, &param, &result );
  else if (m_dialogStyle & wxICON_QUESTION)
			StandardAlert( kAlertCautionAlert, pascalTitle, pascalText, &param, &result );
	else
			StandardAlert( kAlertPlainAlert, pascalTitle, pascalText, &param, &result );

	if (m_dialogStyle & wxOK)
  {
    if (m_dialogStyle & wxCANCEL)				
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
  else if (m_dialogStyle & wxYES_NO)
  {
    if (m_dialogStyle & wxCANCEL)
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

