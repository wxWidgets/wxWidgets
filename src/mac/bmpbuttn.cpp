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

#include "wx/window.h"
#include "wx/bmpbuttn.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)
#endif

#include "wx/mac/uma.h"
#include "wx/bitmap.h"

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

	Rect bounds ;
	Str255 title ;
    m_buttonBitmap = bitmap;
	wxBitmapRefData * bmap = (wxBitmapRefData*) ( m_buttonBitmap.GetRefData()) ;
	
	MacPreControlCreate( parent , id ,  "" , pos , wxSize( width , height ) ,style, validator , name , &bounds , title ) ;

	m_macControl = ::NewControl( parent->MacGetRootWindow() , &bounds , title , false , 0 , 
		kControlBehaviorOffsetContents + 
		    ( bmap->m_bitmapType == kMacBitmapTypeIcon ? kControlContentCIconHandle : kControlContentPictHandle ) , 0, 
	  	(( style & wxBU_AUTODRAW ) ? kControlBevelButtonSmallBevelProc : kControlBevelButtonNormalBevelProc ), (long) this ) ;
	wxASSERT_MSG( m_macControl != NULL , "No valid mac control" ) ;
	
	ControlButtonContentInfo info ;
	

	if ( m_buttonBitmap.Ok() )
	{
		if ( bmap->m_bitmapType == kMacBitmapTypePict ) {
	    info.contentType = kControlContentPictHandle ;
			info.u.picture = bmap->m_hPict ;
		}
		else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
		{
			if ( m_buttonBitmap.GetMask() )
			{
		    info.contentType = kControlContentCIconHandle ;
				info.u.cIconHandle = wxMacCreateCIcon( bmap->m_hBitmap , m_buttonBitmap.GetMask()->GetMaskBitmap() ,
				    8 , 16 ) ;
			}
			else
			{
		    info.contentType = kControlContentCIconHandle ;
				info.u.cIconHandle = wxMacCreateCIcon( bmap->m_hBitmap , NULL ,
				    8 , 16 ) ;
			}
		}
		else if ( bmap->m_bitmapType == kMacBitmapTypeIcon )
		{
	        info.contentType = kControlContentCIconHandle ;
	        info.u.cIconHandle = bmap->m_hIcon ;
		}
	}
	
	::SetControlData( m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;

	MacPostControlCreate() ;

    return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
	ControlButtonContentInfo info ;
    m_buttonBitmap = bitmap;

	if ( m_buttonBitmap.Ok() )
	{
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( m_buttonBitmap.GetRefData()) ;
		if ( bmap->m_bitmapType == kMacBitmapTypePict ) {
	    info.contentType = kControlContentPictHandle ;
			info.u.picture = bmap->m_hPict ;
		}
		else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
		{
			if ( m_buttonBitmap.GetMask() )
			{
		    info.contentType = kControlContentCIconHandle ;
				info.u.cIconHandle = wxMacCreateCIcon( bmap->m_hBitmap , m_buttonBitmap.GetMask()->GetMaskBitmap() ,
				    8 , 16 ) ;
			}
			else
			{
		    info.contentType = kControlContentCIconHandle ;
				info.u.cIconHandle = wxMacCreateCIcon( bmap->m_hBitmap , NULL ,
				    8 , 16 ) ;
			}
		}
		else if ( bmap->m_bitmapType == kMacBitmapTypeIcon )
		{
	        info.contentType = kControlContentCIconHandle ;
	        info.u.cIconHandle = bmap->m_hIcon ;
		}

		
	    ::SetControlData( m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
    }
}

