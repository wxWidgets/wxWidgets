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

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)

#include <wx/mac/uma.h>

BEGIN_EVENT_TABLE(wxStaticText, wxControl)
    EVT_PAINT(wxStaticText::OnPaint)
END_EVENT_TABLE()

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    SetName(name);
    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style;
    m_label = label ;

	bool ret = wxControl::Create( parent, id, pos, size, style , wxDefaultValidator , name );
	SetSizeOrDefault( size ) ;
    
    return ret;
}

void wxStaticText::OnPaint( wxPaintEvent &event ) 
{
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.Clear() ;
    dc.DrawText( m_label , 0 , 0 ) ;
}

wxSize wxStaticText::DoGetBestSize() const
{
	int x , y  ;
	GetTextExtent( m_label , &x , &y ) ;
	return wxSize( x , y ) ;
}

void wxStaticText::SetLabel(const wxString& st , bool resize )
{
	SetTitle( st ) ;
	m_label = st ;
	if ( resize )
		SetSizeOrDefault() ;
	else
		Refresh() ;
}
/*
void wxStaticText::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::SetSize( x , y , width , height , sizeFlags ) ;
}

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
*/

