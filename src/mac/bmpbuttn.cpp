/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/bmpbuttn.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#include <wx/mac/uma.h>

PicHandle MakePict(GWorldPtr wp) ;

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_buttonBitmap = bitmap;
 
    m_marginX = 0;
    m_marginY = 0;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    if ( width == -1 && bitmap.Ok())
	width = bitmap.GetWidth() + 2*m_marginX;

    if ( height == -1 && bitmap.Ok())
	height = bitmap.GetHeight() + 2*m_marginY;

	m_macHorizontalBorder = 0 ; // additional pixels around the real control
	m_macVerticalBorder = 0 ;
	Rect bounds ;
	Str255 title ;
	MacPreControlCreate( parent , id ,  "" , pos , wxSize( width , height ) ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 
		kControlBehaviorOffsetContents + kControlContentPictHandle , 0, 
	  	(( style & wxBU_AUTODRAW ) ? kControlBevelButtonSmallBevelProc : kControlBevelButtonNormalBevelProc ), (long) this ) ;
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
	
    m_buttonBitmap = bitmap;
			PicHandle	icon = NULL ;
	if ( m_buttonBitmap.Ok() )
	{
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( m_buttonBitmap.GetRefData()) ;
		if ( bmap->m_bitmapType == kMacBitmapTypePict )
			icon = bmap->m_hPict ;
		else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
		{
			icon = MakePict( bmap->m_hBitmap ) ;
		}
	}
	ControlButtonContentInfo info ;
	
	info.contentType = kControlContentPictHandle ;
	info.u.picture = icon ;
	
	UMASetControlData( m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;

	MacPostControlCreate() ;

    return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_buttonBitmap = bitmap;
			PicHandle	icon = NULL ;
	if ( m_buttonBitmap.Ok() )
	{
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( m_buttonBitmap.GetRefData()) ;
		if ( bmap->m_bitmapType == kMacBitmapTypePict )
			icon = bmap->m_hPict ;
		else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
		{
			icon = MakePict( bmap->m_hBitmap ) ;
		}
	}
	ControlButtonContentInfo info ;
	
	info.contentType = kControlContentPictHandle ;
	info.u.picture = icon ;
	
	UMASetControlData( m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
}

