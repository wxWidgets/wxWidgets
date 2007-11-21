/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/statbmp.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

/*
 * wxStaticBitmap
 */

BEGIN_EVENT_TABLE(wxStaticBitmap, wxStaticBitmapBase)
    EVT_PAINT(wxStaticBitmap::OnPaint)
END_EVENT_TABLE()

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
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
        if ( size.x == -1 )
            size.x = bitmap.GetWidth() ;
        if ( size.y == -1 )
            size.y = bitmap.GetHeight() ;
    }

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    m_bitmap = bitmap;
    if ( id == wxID_ANY )
          m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    bool ret = wxControl::Create( parent, id, pos, size, style , wxDefaultValidator , name );
    SetInitialSize( size ) ;

    return ret;
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    InvalidateBestSize();
    SetSize(wxSize(bitmap.GetWidth(), bitmap.GetHeight()));
    Refresh() ;
}

void wxStaticBitmap::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    dc.DrawBitmap( m_bitmap , 0 , 0 , true ) ;
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    return wxWindow::DoGetBestSize() ;
}
