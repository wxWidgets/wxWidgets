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

#include "wx/msgdlg.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxMessageDialog, wxDialog)
#endif

#define kMacOKAlertResourceID 128
#define kMacYesNoAlertResourceID 129
#define kMacYesNoCancelAlertResourceID 130
#define kMacNoYesAlertResourceID 131
#define kMacNoYesCancelAlertResourceID 132

short language = 0 ;

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
	
	strcpy( (char*) pascalTitle , m_caption ) ;
	c2pstr( (char*) pascalTitle ) ;
	strcpy( (char*) pascalText , m_message ) ;
	wxMacConvertNewlines( (char*)pascalText ,(char*) pascalText) ;
	c2pstr( (char*) pascalText ) ;

	if ( !UMAHasAppearance() )
	{
		int resourceID ;
		
	  if (m_dialogStyle & wxYES_NO)
	  {
	    if (m_dialogStyle & wxCANCEL)
	      resourceID = kMacYesNoCancelAlertResourceID;
	    else
	      resourceID = kMacYesNoAlertResourceID + language * 10 ;
	  }
	  else if (m_dialogStyle & wxOK)
	  {
	    if (m_dialogStyle & wxCANCEL)
	      resourceID = kMacOKAlertResourceID; // wrong
	    else
	      resourceID = kMacOKAlertResourceID;
	  }
		else
		{
			return resultbutton ;
		}
	/*
	  if (hWnd)
	    msStyle |= MB_APPLMODAL;
	  else
	    msStyle |= MB_TASKMODAL;
	*/

		ParamText( pascalTitle , pascalText , NULL , NULL ) ;

	  if (m_dialogStyle & wxICON_EXCLAMATION)
				result = Alert( resourceID , NULL ) ;
	  else if (m_dialogStyle & wxICON_HAND)
			result = StopAlert( resourceID , NULL ) ;
	  else if (m_dialogStyle & wxICON_INFORMATION)
				result = NoteAlert( resourceID , NULL ) ;
	  else if (m_dialogStyle & wxICON_QUESTION)
				result = CautionAlert( resourceID , NULL ) ;
		else
				result = Alert( resourceID , NULL ) ;

		resultbutton = result ;
		
	  if (m_dialogStyle & wxYES_NO)
	  {
	    if (m_dialogStyle & wxCANCEL)
	    {
				switch( result )
				{
					case 1 :
						resultbutton = wxID_YES ;
						break ;
					case 2 :
						resultbutton = wxID_NO ;
						break ;
					case 3 :
						resultbutton = wxID_CANCEL ;
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
						resultbutton = wxID_NO ;
						break ;
					case 3 :
						break ;
				}
			}
		}
	}
	else
	{
		AlertStdAlertParamRec	param;

		param.movable 		= true;
		param.filterProc 	= NULL ;
		
	  if (m_dialogStyle & wxYES_NO)
	  {
	    if (m_dialogStyle & wxCANCEL)
	    {
				param.defaultText 	= "\pYes" ;
				param.cancelText 	= (StringPtr) kAlertDefaultCancelText;
				param.otherText 	= "\pNo";
				param.helpButton 	= false ;
				param.defaultButton = kAlertStdAlertOKButton;
				param.cancelButton 	= kAlertStdAlertCancelButton;
	    }
	    else
	    {
				param.defaultText 	= "\pYes" ;
				param.cancelText 	= NULL;
				param.otherText 	= "\pNo";
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
	}
	return resultbutton ;
}

