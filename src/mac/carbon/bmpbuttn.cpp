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
    m_bmpNormal = bitmap;
 
    m_marginX = 0;
    m_marginY = 0;

    // int x = pos.x;
    // int y = pos.y;
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
    m_bmpNormal = bitmap;
	wxBitmapRefData * bmap = NULL ;
	
	if ( m_bmpNormal.Ok() )
		bmap = (wxBitmapRefData*) ( m_bmpNormal.GetRefData()) ;
	
	MacPreControlCreate( parent , id ,  "" , pos , wxSize( width , height ) ,style, validator , name , &bounds , title ) ;

	m_macControl = ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 
		kControlBehaviorOffsetContents + 
		    ( bmap && bmap->m_bitmapType == kMacBitmapTypeIcon ? 
		    kControlContentCIconHandle : kControlContentPictHandle ) , 0, 
	  	(( style & wxBU_AUTODRAW ) ? kControlBevelButtonSmallBevelProc : kControlBevelButtonNormalBevelProc ), (long) this ) ;
	wxASSERT_MSG( (ControlHandle) m_macControl != NULL , "No valid mac control" ) ;
	
	ControlButtonContentInfo info ;
	wxMacCreateBitmapButton( &info , m_bmpNormal ) ;
	if ( info.contentType != kControlNoContent )
	{
	    ::SetControlData( (ControlHandle) m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
    }
	MacPostControlCreate() ;

    return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_bmpNormal = bitmap;

    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , m_bmpNormal ) ;
    if ( info.contentType != kControlNoContent )
    {
        ::SetControlData( (ControlHandle) m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
    }
}

