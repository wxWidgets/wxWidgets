///////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     native implementation of wxStatusBar (optional)
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbrma.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/statusbr.h"
#include "wx/dc.h"
#include "wx/dcclient.h"

BEGIN_EVENT_TABLE(wxStatusBarMac, wxStatusBarGeneric)
	EVT_PAINT(wxStatusBarMac::OnPaint)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBarMac class
// ----------------------------------------------------------------------------

wxStatusBarMac::wxStatusBarMac()
{
    SetParent(NULL);
}

wxStatusBarMac::~wxStatusBarMac()
{
}

bool wxStatusBarMac::Create(wxWindow *parent, wxWindowID id,
           long style ,
           const wxString& name)
{
	return wxStatusBarGeneric::Create( parent , id , style , name ) ;
}

void wxStatusBarMac::DrawFieldText(wxDC& dc, int i)
{
  int leftMargin = 2;

  wxRect rect;
  GetFieldRect(i, rect);
  
  if ( !IsWindowHilited( MacGetRootWindow() ) )
  {
    dc.SetTextForeground( wxColour( 0x80 , 0x80 , 0x80 ) ) ;
  }

  wxString text(GetStatusText(i));

  long x, y;

  dc.GetTextExtent(text, &x, &y);

  int xpos = rect.x + leftMargin + 1 ;
  int ypos = 2 ;
  
  dc.SetClippingRegion(rect.x, 0, rect.width, m_height);

  dc.DrawText(text, xpos, ypos);

  dc.DestroyClippingRegion();
}

void wxStatusBarMac::DrawField(wxDC& dc, int i)
{
    DrawFieldText(dc, i);
}

void wxStatusBarMac::SetStatusText(const wxString& text, int number)
{
    wxCHECK_RET( (number >= 0) && (number < m_nFields),
                 _T("invalid status bar field index") );

    m_statusStrings[number] = text;
    wxRect rect;
    GetFieldRect(number, rect);
    Refresh( TRUE , &rect ) ;
    /*
    // TODO make clear work again also when using themes
    wxClientDC dc(this);
    dc.SetBackground( wxBrush(GetBackgroundColour(), wxSOLID) );
    dc.SetClippingRegion( rect.x+1, rect.y+1, rect.width-1, rect.height-1 );
    dc.Clear();
    dc.DestroyClippingRegion();
    DrawFieldText( dc, number );
    */

}

void wxStatusBarMac::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
  	wxPaintDC dc(this);
  	
  if ( IsWindowHilited( MacGetRootWindow() ) )
  {
  	wxPen black( wxBLACK , 1 , wxSOLID ) ;
	wxPen white( wxWHITE , 1 , wxSOLID ) ;
	
    dc.SetPen(black);
    dc.DrawLine(0, 0 ,
           m_width , 0);
   	dc.SetPen(white);
    dc.DrawLine(0, 1 ,
           m_width , 1);
  }
  else
  {
    dc.SetPen(wxPen(wxColour(0x80,0x80,0x80),1,wxSOLID));
    dc.DrawLine(0, 0 ,
           m_width , 0);
  }

  int i;
  if ( GetFont().Ok() )
    dc.SetFont(GetFont());
  dc.SetBackgroundMode(wxTRANSPARENT);

  for ( i = 0; i < m_nFields; i ++ )
    DrawField(dc, i);

#   ifdef __WXMSW__
        dc.SetFont(wxNullFont);
#   endif // MSW
}

void wxStatusBarMac::MacSuperEnabled( bool enabled ) 
{
    Refresh(FALSE) ;
    wxWindow::MacSuperEnabled( enabled ) ;
}