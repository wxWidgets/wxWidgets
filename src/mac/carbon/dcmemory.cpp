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
    m_ok = FALSE;
};

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
: m_selected()
{
    m_ok = TRUE;
    SetBackground(*wxWHITE_BRUSH);
    SetBrush(*wxWHITE_BRUSH);
    SetPen(*wxBLACK_PEN);
    m_ok = FALSE;
};

wxMemoryDC::~wxMemoryDC()
{
    if ( m_selected.Ok() )
    {
        UnlockPixels( GetGWorldPixMap(MAC_WXHBITMAP(m_selected.GetHBITMAP())) );
    }
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    if ( m_selected.Ok() )
    {
        UnlockPixels( GetGWorldPixMap(MAC_WXHBITMAP(m_selected.GetHBITMAP())) );
    }
    m_selected = bitmap;
    if (m_selected.Ok())
    {
        if ( m_selected.GetHBITMAP() )
        {
            m_macPort = (GrafPtr) m_selected.GetHBITMAP() ;
            LockPixels( GetGWorldPixMap(  (CGrafPtr)  m_macPort ) ) ;
            wxMask * mask = bitmap.GetMask() ;
            if ( mask )
            {
                m_macMask = mask->GetMaskBitmap() ;
            }
            SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , 0 , 0 , m_selected.GetWidth() , m_selected.GetHeight() ) ;
            CopyRgn( (RgnHandle) m_macBoundaryClipRgn ,(RgnHandle)  m_macCurrentClipRgn ) ;
            m_ok = TRUE ;
        }
        else
        {
            m_ok = FALSE;
        }
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


