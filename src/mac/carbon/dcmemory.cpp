/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dcmemory.h"
#endif

#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/mac/private.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxPaintDC)

wxMemoryDC::wxMemoryDC(void)
: m_selected()
{
    m_ok = TRUE;
    SetBackground(*wxWHITE_BRUSH);
    SetBrush(*wxWHITE_BRUSH);
    SetPen(*wxBLACK_PEN);
    SetFont(*wxNORMAL_FONT) ;
    m_ok = FALSE;
};

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
: m_selected()
{
    m_ok = TRUE;
    SetBackground(*wxWHITE_BRUSH);
    SetBrush(*wxWHITE_BRUSH);
    SetPen(*wxBLACK_PEN);
    SetFont(*wxNORMAL_FONT) ;
    m_ok = FALSE;
};

wxMemoryDC::~wxMemoryDC()
{
    if ( m_selected.Ok() )
    {
#if wxMAC_USE_CORE_GRAPHICS
        m_selected.EndRawAccess() ;
        CGContextRef bmCtx = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
        delete m_graphicContext ;
        m_graphicContext = NULL ;
        CGContextRelease( bmCtx ) ;
#else
// TODO        UnlockPixels( GetGWorldPixMap(MAC_WXHBITMAP(m_selected.GetHBITMAP())) );
#endif
    }
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    if ( m_selected.Ok() )
    {
#if wxMAC_USE_CORE_GRAPHICS
        m_selected.EndRawAccess() ;
        CGContextRef bmCtx = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
        delete m_graphicContext ;
        m_graphicContext = NULL ;
        CGContextRelease( bmCtx ) ;
#else
// TODO        UnlockPixels( GetGWorldPixMap(MAC_WXHBITMAP(m_selected.GetHBITMAP())) );
#endif
    }
    m_selected = bitmap;
    if (m_selected.Ok())
    {
#if wxMAC_USE_CORE_GRAPHICS
        m_selected.UseAlpha() ;
        void * data = m_selected.BeginRawAccess() ;

        int bitsPerComp = 8 ;
        int bytesPerPixel = 4 ;
        int w = bitmap.GetWidth() ;
        int h = bitmap.GetHeight() ;
        CGImageAlphaInfo a = kCGImageAlphaNoneSkipFirst ; 
        CGColorSpaceRef genericColorSpace  = wxMacGetGenericRGBColorSpace();
        CGContextRef bmCtx = CGBitmapContextCreate(data , w, h, bitsPerComp , bytesPerPixel * w , genericColorSpace, a);
        wxASSERT_MSG( bmCtx , wxT("Unable to create bitmap context") ) ;

        CGContextSetFillColorSpace(bmCtx, genericColorSpace); 
        CGContextSetStrokeColorSpace(bmCtx, genericColorSpace); 

        if( bmCtx )
        {
            CGContextTranslateCTM( bmCtx , 0 ,  m_selected.GetHeight() ) ;
            CGContextScaleCTM( bmCtx , 1 , -1 ) ;
            m_graphicContext = new wxMacCGContext( bmCtx ) ;  
            m_graphicContext->SetPen( m_pen ) ;
            m_graphicContext->SetBrush( m_brush ) ;     
        }
        m_ok = (m_graphicContext != NULL) ;  
#else
        if ( ( m_macPort = m_selected.GetHBITMAP( &m_macMask ) ) != NULL )
        {
            LockPixels( GetGWorldPixMap(  (CGrafPtr)  m_macPort ) ) ;
            /*
            wxMask * mask = bitmap.GetMask() ;
            if ( mask )
            {
                m_macMask = mask->GetHBITMAP() ;
            }
            */
            SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , 0 , 0 , m_selected.GetWidth() , m_selected.GetHeight() ) ;
            CopyRgn( (RgnHandle) m_macBoundaryClipRgn ,(RgnHandle)  m_macCurrentClipRgn ) ;
            m_ok = TRUE ;
        }
        else
        {
            m_ok = FALSE;
        }
#endif
    }
    else
    {
        m_ok = FALSE;
    }
}

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
    if (m_selected.Ok())
    {
        if (width) (*width) = m_selected.GetWidth();
        if (height) (*height) = m_selected.GetHeight();
    }
    else
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
    }
}


