/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

#include "wx/app.h"
#include "wx/stattext.h"
#include "wx/notebook.h"
#include "wx/tabctrl.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/settings.h"

#include <stdio.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
#endif

#include "wx/mac/uma.h"

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    m_label = wxStripMenuCodes(label) ;

    if ( !wxControl::Create( parent, id, pos, size, style,
                             wxDefaultValidator , name ) )
    {
        return false;
    }

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;
    wxMacCFStringHolder str(m_label,m_font.GetEncoding() ) ;
    verify_noerr(CreateStaticTextControl(MAC_WXHWND(parent->MacGetTopLevelWindowRef()),&bounds, str , 
        NULL , (ControlRef*)&m_macControl ) ) ;  

    MacPostControlCreate(pos,size) ;

    return true;
}

wxSize wxStaticText::DoGetBestSize() const
{
    ControlFontStyleRec controlFont ;
    Size outSize ;
    verify_noerr( GetControlData( (ControlRef) m_macControl , kControlEntireControl , kControlFontStyleTag , sizeof(controlFont) , &controlFont , &outSize ) ) ;
    
    Point bounds ;
    SInt16 baseline ;
    wxMacCFStringHolder str(m_label ,  m_font.GetEncoding() ) ;
    if ( m_font.MacGetThemeFontID() != kThemeCurrentPortFont )
        verify_noerr( GetThemeTextDimensions( (m_label.Length() > 0 ? ((CFStringRef) str ) : CFSTR(" ") ) , m_font.MacGetThemeFontID() , kThemeStateActive , false , &bounds , &baseline ) ) ;
    else
    {
        wxMacWindowStateSaver sv( this ) ;
        ::TextFont( m_font.MacGetFontNum() ) ;
        ::TextSize( (short)( m_font.MacGetFontSize()) ) ;
        ::TextFace( m_font.MacGetFontStyle() ) ;        
        verify_noerr( GetThemeTextDimensions( (m_label.Length() > 0 ? ((CFStringRef) str ) : CFSTR(" ") ) , kThemeCurrentPortFont , kThemeStateActive , false , &bounds , &baseline ) ) ;
    }
    if ( m_label.Length() == 0 )
        bounds.h = 0 ;
    return wxSize(bounds.h, bounds.v);
}

void wxStaticText::SetLabel(const wxString& st )
{

    m_label = wxStripMenuCodes(st) ;
    
    wxMacCFStringHolder str(m_label,m_font.GetEncoding() ) ;
    CFStringRef ref = str ;
    SetControlData( (ControlRef) m_macControl, kControlEntireControl , kControlStaticTextCFStringTag, sizeof( CFStringRef ),
		&ref );

    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        SetSize( GetBestSize() ) ;
        SetSizeHints(GetSize());
    }

    Update() ;
}

bool wxStaticText::SetFont(const wxFont& font)
{
    bool ret = wxControl::SetFont(font);

	if ( ret )
	{
	    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
            {
	        SetSize( GetBestSize() );
                SetSizeHints(GetSize());
            }
	}

    return ret;
}
