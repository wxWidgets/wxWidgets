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

#include "wx/defs.h"

#include "wx/statbmp.h"
#include "wx/dcclient.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

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

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    m_bitmap = bitmap;
    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style;

    bool ret = wxControl::Create( parent, id, pos, size, style , wxDefaultValidator , name );
	SetBestSize( size ) ;
    
    return ret;
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    Refresh() ;
    SetBestSize(wxSize(bitmap.GetWidth(), bitmap.GetHeight()));
}

void wxStaticBitmap::OnPaint( wxPaintEvent &event ) 
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    dc.DrawBitmap( m_bitmap , 0 , 0 , TRUE ) ;
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
   if ( m_bitmap.Ok() )
       return wxSize(m_bitmap.GetWidth(), m_bitmap.GetHeight());
   else
       return wxSize(16, 16);  // completely arbitrary
}

