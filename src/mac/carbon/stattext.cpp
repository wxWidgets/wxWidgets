/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

#include "wx/app.h"
#include "wx/stattext.h"

#include <stdio.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
#endif

#include <wx/mac/uma.h>

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  label , pos , size ,style, *((wxValidator*)NULL) , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
	  	kControlStaticTextProc , (long) this ) ;
	::UMASetControlData( m_macControl, kControlLabelPart, kControlStaticTextTextTag , (long) title[0] , (char*) &title[1] ) ;
	
	MacPostControlCreate() ;

  return TRUE;
}

void wxStaticText::SetLabel(const wxString& st , bool resize )
{
	SetTitle( st ) ;
	wxString label ;
	
	if( wxApp::s_macDefaultEncodingIsPC )
		label = wxMacMakeMacStringFromPC( st ) ;
	else
		label = st ;
		
	::UMASetControlData( m_macControl, kControlLabelPart, kControlStaticTextTextTag , (long) label.Length() , (char*)(const char*) label ) ;
	Refresh() ;
}

