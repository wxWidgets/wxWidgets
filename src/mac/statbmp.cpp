/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

#include "wx/statbmp.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

/*
 * wxStaticBitmap
 */

BEGIN_EVENT_TABLE(wxStaticBitmap, wxControl)
    EVT_PAINT(wxStaticBitmap::OnPaint)
END_EVENT_TABLE()

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    SetName(name);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    m_messageBitmap = bitmap;

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style;

    bool ret = wxControl::Create( parent, id, pos, size, style , wxDefaultValidator , name );
    
	SetSizeOrDefault() ;
	
    return ret;
}

void wxStaticBitmap::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::SetSize( x , y , width , height , sizeFlags ) ;
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_messageBitmap = bitmap;
    SetSizeOrDefault();
}
void wxStaticBitmap::OnPaint( wxPaintEvent &event ) 
{
    wxPaintDC dc(this);
    PrepareDC(dc);
	dc.SetPalette( *m_messageBitmap.GetPalette() ) ;
	dc.DrawBitmap( m_messageBitmap , 0 , 0 ) ;
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    if ( m_messageBitmap.Ok() )
        return wxSize(m_messageBitmap.GetWidth(), m_messageBitmap.GetHeight());
    else
        return wxSize(16, 16);  // completely arbitrary
}

