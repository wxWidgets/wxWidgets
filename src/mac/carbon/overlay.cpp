/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/overlay.cpp
// Purpose:     common wxOverlay code
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/overlay.h"
#include "wx/private/overlay.h"
#include "wx/dcclient.h"

#if wxHAS_NATIVE_OVERLAY

// ============================================================================
// implementation
// ============================================================================

wxOverlayImpl::wxOverlayImpl()
{
    m_window = NULL ;
    m_overlayContext = NULL ;
    m_overlayWindow = NULL ;
}

wxOverlayImpl::~wxOverlayImpl()
{
    Reset();
}

bool wxOverlayImpl::IsOk() 
{
    return m_overlayWindow != NULL ;
}

void wxOverlayImpl::MacGetBounds( Rect *bounds )
{
    wxPoint origin(0,0);
    origin = m_window->ClientToScreen( origin );
    bounds->top = origin.y;
    bounds->left = origin.x;
    bounds->bottom = origin.y+m_y+m_height;
    bounds->right = origin.x+m_x+m_width;
}

OSStatus wxOverlayImpl::CreateOverlayWindow()
{
    OSStatus err;

    WindowAttributes overlayAttributes  = kWindowIgnoreClicksAttribute;
        
	if ( m_window )
	{
		m_overlayParentWindow =(WindowRef) m_window->MacGetTopLevelWindowRef();
    
		Rect bounds ;
		MacGetBounds(&bounds);
		err  = CreateNewWindow( kOverlayWindowClass, overlayAttributes, &bounds, &m_overlayWindow );  
		if ( err == noErr ) 
		{
        SetWindowGroup( m_overlayWindow, GetWindowGroup(m_overlayParentWindow));    //  Put them in the same group so that their window layers are consistent
 		}
	}
	else
	{
		m_overlayParentWindow = NULL ;
		CGRect cgbounds ;
		cgbounds = CGDisplayBounds(CGMainDisplayID());
		Rect bounds;
		bounds.top = cgbounds.origin.y;
		bounds.left = cgbounds.origin.x;
		bounds.bottom = bounds.top + cgbounds.size.height;
		bounds.right = bounds.left  + cgbounds.size.width;
		err  = CreateNewWindow( kOverlayWindowClass, overlayAttributes, &bounds, &m_overlayWindow );  		
	}
	ShowWindow(m_overlayWindow);
	return err;
}

void wxOverlayImpl::Init( wxWindowDC* dc, int x , int y , int width , int height )
{
    wxASSERT_MSG( !IsOk() , _("You cannot Init an overlay twice") );

    m_window = dc->GetWindow(); 
    m_x = x ;
    m_y = y ;
    m_width = width ;
    m_height = height ;
    
    OSStatus err = CreateOverlayWindow();
    wxASSERT_MSG(  err == noErr , _("Couldn't create the overlay window") );
#ifndef __LP64__
    err = QDBeginCGContext(GetWindowPort(m_overlayWindow), &m_overlayContext);
#endif
    CGContextTranslateCTM( m_overlayContext, 0, m_height+m_y );
    CGContextScaleCTM( m_overlayContext, 1, -1 );
    wxASSERT_MSG(  err == noErr , _("Couldn't init the context on the overlay window") );
}

void wxOverlayImpl::BeginDrawing( wxWindowDC* dc)
{
// TODO CS
	dc->SetGraphicsContext( wxGraphicsContext::CreateFromNative( m_overlayContext ) );
/*
    delete dc->m_graphicContext ;
    dc->m_graphicContext = new wxMacCGContext( m_overlayContext );
    // we are right now startin at 0,0 not at the wxWindow's origin, so most of the calculations 
    // int dc are already corect
    // just to make sure :
    dc->m_macLocalOrigin.x = 0 ;
    dc->m_macLocalOrigin.y = 0 ;
	*/
    wxSize size = dc->GetSize() ;
    dc->SetClippingRegion( 0 , 0 , size.x , size.y ) ;
}

void wxOverlayImpl::EndDrawing( wxWindowDC* dc)
{
	dc->SetGraphicsContext(NULL);
}

void wxOverlayImpl::Clear(wxWindowDC* dc) 
{
    wxASSERT_MSG( IsOk() , _("You cannot Clear an overlay that is not inited") );
    CGRect box  = CGRectMake( m_x - 1, m_y - 1 , m_width + 2 , m_height + 2 );
    CGContextClearRect( m_overlayContext, box );
}

void wxOverlayImpl::Reset()
{
    if ( m_overlayContext )
    {
#ifndef __LP64__
        OSStatus err = QDEndCGContext(GetWindowPort(m_overlayWindow), &m_overlayContext);
        wxASSERT_MSG(  err == noErr , _("Couldn't end the context on the overlay window") );
#endif
        m_overlayContext = NULL ;
    }    
    
    // todo : don't dispose, only hide and reposition on next run
    if (m_overlayWindow)
    {
        DisposeWindow(m_overlayWindow);
        m_overlayWindow = NULL ;
    }
}

#endif // wxHAS_NATIVE_OVERLAY
