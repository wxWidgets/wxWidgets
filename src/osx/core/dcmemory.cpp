/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Stefan Csomor
// Created:     01/02/97
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/graphics.h"
#include "wx/osx/dcmemory.h"

#include "wx/osx/private.h"

//-----------------------------------------------------------------------------
// wxMemoryDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxMemoryDCImpl, wxPaintDCImpl);


wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner )
  : wxPaintDCImpl( owner )
{
    Init();
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
  : wxPaintDCImpl( owner )
{
    Init();
    DoSelect(bitmap);
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC * WXUNUSED(dc) )
  : wxPaintDCImpl( owner )
{
    Init();
}

void wxMemoryDCImpl::Init()
{
    m_ok = true;
    SetBackground(*wxWHITE_BRUSH);
    SetBrush(*wxWHITE_BRUSH);
    SetPen(*wxBLACK_PEN);
    SetFont(*wxNORMAL_FONT);
    m_ok = false;
}

wxMemoryDCImpl::~wxMemoryDCImpl()
{
    if ( m_selected.IsOk() )
    {
        m_selected.SetSelectedInto(nullptr);
        wxDELETE(m_graphicContext);
    }
}

void wxMemoryDCImpl::DoSelect( const wxBitmap& bitmap )
{
    if ( m_selected.IsOk() )
    {
        m_selected.SetSelectedInto(nullptr);
        wxDELETE(m_graphicContext);
    }

    m_selected = bitmap;
    if (m_selected.IsOk())
    {
        wxASSERT_MSG( !bitmap.GetSelectedInto() ||
                     (bitmap.GetSelectedInto() == GetOwner()),
                     "Bitmap is selected in another wxMemoryDC, delete the first wxMemoryDC or use SelectObject(nullptr)" );

        m_selected.SetSelectedInto(GetOwner());
        m_width = bitmap.GetLogicalWidth();
        m_height = bitmap.GetLogicalHeight();
        m_contentScaleFactor = bitmap.GetScaleFactor();
        CGColorSpaceRef genericColorSpace  = wxMacGetGenericRGBColorSpace();
        CGContextRef bmCtx = (CGContextRef) m_selected.GetHBITMAP();

        if ( bmCtx )
        {
            CGContextSetFillColorSpace( bmCtx, genericColorSpace );
            CGContextSetStrokeColorSpace( bmCtx, genericColorSpace );
            SetGraphicsContext( wxGraphicsContext::CreateFromNative( bmCtx ) );
            if (m_graphicContext)
            {
                m_graphicContext->SetContentScaleFactor(m_contentScaleFactor);
            }
        }
        m_ok = (m_graphicContext != nullptr) ;
    }
    else
    {
        m_ok = false;
    }
}

void wxMemoryDCImpl::DoGetSize( int *width, int *height ) const
{
    if (m_selected.IsOk())
    {
        if (width)
            (*width) = m_selected.GetLogicalWidth();
        if (height)
            (*height) = m_selected.GetLogicalHeight();
    }
    else
    {
        if (width)
            (*width) = 0;
        if (height)
            (*height) = 0;
    }
}
