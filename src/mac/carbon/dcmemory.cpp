/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/graphics.h"

#include "wx/mac/private.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxPaintDC)

void wxMemoryDC::Init()
{
    m_ok = true;
    SetBackground(*wxWHITE_BRUSH);
    SetBrush(*wxWHITE_BRUSH);
    SetPen(*wxBLACK_PEN);
    SetFont(*wxNORMAL_FONT);
    m_ok = false;
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
: m_selected()
{
    Init();
}

wxMemoryDC::~wxMemoryDC()
{
    if ( m_selected.Ok() )
    {
#if wxMAC_USE_CORE_GRAPHICS
        m_selected.EndRawAccess() ;
        CGContextRef bmCtx = (CGContextRef) m_graphicContext->GetNativeContext() ;
        delete m_graphicContext ;
        m_graphicContext = NULL ;
        CGContextRelease( bmCtx ) ;
#else
// TODO: UnlockPixels( GetGWorldPixMap(MAC_WXHBITMAP(m_selected.GetHBITMAP())) );
#endif
    }
}

void wxMemoryDC::DoSelect( const wxBitmap& bitmap )
{
    if ( m_selected.Ok() )
    {
#if wxMAC_USE_CORE_GRAPHICS
        m_selected.EndRawAccess() ;
        CGContextRef bmCtx = (CGContextRef) m_graphicContext->GetNativeContext() ;
        delete m_graphicContext ;
        m_graphicContext = NULL ;
        CGContextRelease( bmCtx ) ;
#else
// TODO: UnlockPixels( GetGWorldPixMap(MAC_WXHBITMAP(m_selected.GetHBITMAP())) );
#endif
    }

    m_selected = bitmap;
    if (m_selected.Ok())
    {
#if wxMAC_USE_CORE_GRAPHICS
        if ( m_selected.GetDepth() != 1 )
            m_selected.UseAlpha() ;
        void * data = m_selected.BeginRawAccess() ;

        int bitsPerComp = 8 ;
        int bytesPerPixel = 4 ;
        int w = bitmap.GetWidth() ;
        int h = bitmap.GetHeight() ;
		m_width = w;
		m_height = h;

        // TODO: should this be kCGImageAlphaPremultiplied[First,Last] ?
        CGImageAlphaInfo a = kCGImageAlphaNoneSkipFirst ;

        CGColorSpaceRef genericColorSpace  = wxMacGetGenericRGBColorSpace();
        CGContextRef bmCtx = CGBitmapContextCreate( data , w, h, bitsPerComp , bytesPerPixel * w , genericColorSpace, a );
        wxASSERT_MSG( bmCtx , wxT("Unable to create bitmap context") ) ;

        if ( bmCtx )
        {
            CGContextSetFillColorSpace( bmCtx, genericColorSpace );
            CGContextSetStrokeColorSpace( bmCtx, genericColorSpace );

            CGContextTranslateCTM( bmCtx , 0 ,  m_selected.GetHeight() ) ;
            CGContextScaleCTM( bmCtx , 1 , -1 ) ;

			SetGraphicsContext( wxGraphicsContext::CreateFromNative( bmCtx ) );
        }
        m_ok = (m_graphicContext != NULL) ;

#else
        m_macPort = m_selected.GetHBITMAP( &m_macMask ) ;
        m_ok = (m_macPort != NULL) ;
        if (m_ok)
        {
            LockPixels( GetGWorldPixMap( (CGrafPtr) m_macPort ) ) ;
            SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , 0 , 0 , m_selected.GetWidth() , m_selected.GetHeight() ) ;
            CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
        }
#endif
    }
    else
    {
        m_ok = false;
    }
}

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
    if (m_selected.Ok())
    {
        if (width)
            (*width) = m_selected.GetWidth();
        if (height)
            (*height) = m_selected.GetHeight();
    }
    else
    {
        if (width)
            (*width) = 0;
        if (height)
            (*height) = 0;
    }
}
