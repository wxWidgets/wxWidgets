/////////////////////////////////////////////////////////////////////////////
// Name:        statpict.cpp
// Purpose:     wxStaticPicture
// Author:      Wade Brainerd (wadeb@wadeb.com)
// Modified by:
// Created:     2003-05-01
// RCS-ID:
// Copyright:   (c) Wade Brainerd
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/defs.h"

#include "wx/gizmos/statpict.h"
#include "wx/dcclient.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticPicture, wxControl)
WXDLLIMPEXP_GIZMOS const wxChar * wxStaticPictureNameStr = wxT("staticPicture");
    
/*
 * wxStaticPicture
 */

BEGIN_EVENT_TABLE(wxStaticPicture, wxControl)
    EVT_PAINT(wxStaticPicture::OnPaint)
END_EVENT_TABLE()

bool wxStaticPicture::Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& s,
           long style,
           const wxString& name)
{
    SetName(name);

    wxSize size = s ;
    if ( bitmap.Ok() )
    {
        if ( size.x == wxDefaultCoord )
            size.x = bitmap.GetWidth() ;
        if ( size.y == wxDefaultCoord )
            size.y = bitmap.GetHeight() ;
    }

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    Bitmap = bitmap;
    Align = 0;
    Scale = 0;
    ScaleX = ScaleY = 1;

#ifndef __WXMSW__
    LastScaleX = LastScaleY = -1;
    if ( Bitmap.Ok() )
        OriginalImage = Bitmap.ConvertToImage();
#endif

    if ( id == wxID_ANY )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    bool ret = wxControl::Create( parent, id, pos, size, style, wxDefaultValidator, name );

    SetInitialSize( size ) ;

    return ret;
}

void wxStaticPicture::SetBitmap( const wxBitmap& bmp )
{
    Bitmap = bmp;
#ifndef __WXMSW__
    if ( Bitmap.Ok() )
        OriginalImage = Bitmap.ConvertToImage();
    LastScaleX = LastScaleY = -1;
#endif
}

void wxStaticPicture::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    if ( !Bitmap.Ok() )
        return;

    wxPaintDC dc( this );
    PrepareDC( dc );

    wxSize sz = GetSize();
    wxSize bmpsz( Bitmap.GetWidth(), Bitmap.GetHeight() );
    float sx = 1.0f, sy = 1.0f;

    if ( Scale & wxSCALE_UNIFORM )
    {
        float _sx = (float)sz.GetWidth() / (float)bmpsz.GetWidth();
        float _sy = (float)sz.GetHeight() / (float)bmpsz.GetHeight();
        sx = sy = _sx < _sy ? _sx : _sy;
    }
    else
    if ( Scale & wxSCALE_CUSTOM )
    {
        sx = ScaleX;
        sy = ScaleY;
    }
    else
    {
        if ( Scale & wxSCALE_HORIZONTAL )
            sx = (float)sz.x/(float)bmpsz.x;
        if ( Scale & wxSCALE_VERTICAL )
            sy = (float)sz.y/(float)bmpsz.y;
    }

    bmpsz = wxSize( (int)(bmpsz.x*sx), (int)(bmpsz.y*sy) );

    wxPoint pos( 0, 0 );

    if ( Align & wxALIGN_CENTER_HORIZONTAL ) pos.x = (sz.x-bmpsz.x)/2;
    else if ( Align & wxALIGN_RIGHT ) pos.x = sz.x-bmpsz.x;

    if ( Align & wxALIGN_CENTER_VERTICAL ) pos.y = (sz.y-bmpsz.y)/2;
    else if ( Align & wxALIGN_BOTTOM ) pos.y = sz.y-bmpsz.y;

    if ( Scale )
    {
#ifdef __WXMSW__
        double ux, uy;
        dc.GetUserScale( &ux, &uy );
        dc.SetUserScale( ux*sx, uy*sy );
        dc.DrawBitmap( Bitmap, (int)((float)pos.x/sx), (int)((float)pos.y/sy) );
        dc.SetUserScale( ux, uy );
#else
        if ( LastScaleX != sx || LastScaleY != sy )
        {
            LastScaleX = sx;
            LastScaleY = sy;
            ScaledBitmap = wxBitmap( OriginalImage.Scale( bmpsz.x, bmpsz.y ) );
        }
        dc.DrawBitmap( ScaledBitmap, pos.x, pos.y );
#endif
    }
    else
        dc.DrawBitmap( Bitmap, pos.x, pos.y );
}

