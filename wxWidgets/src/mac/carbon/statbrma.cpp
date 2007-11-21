///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/statbarma.cpp
// Purpose:     native implementation of wxStatusBar (optional)
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/mac/private.h"


BEGIN_EVENT_TABLE(wxStatusBarMac, wxStatusBarGeneric)
    EVT_PAINT(wxStatusBarMac::OnPaint)
END_EVENT_TABLE()


wxStatusBarMac::wxStatusBarMac(wxWindow *parent,
        wxWindowID id,
        long style,
        const wxString& name)
        :
        wxStatusBarGeneric()
{
    SetParent( NULL );
    Create( parent, id, style, name );
}

wxStatusBarMac::wxStatusBarMac()
        :
        wxStatusBarGeneric()
{
    SetParent( NULL );
}

wxStatusBarMac::~wxStatusBarMac()
{
}

bool wxStatusBarMac::Create(wxWindow *parent, wxWindowID id,
                            long style ,
                            const wxString& name)
{
    if ( !wxStatusBarGeneric::Create( parent, id, style, name ) )
        return false;

    if ( parent->MacGetTopLevelWindow()->MacGetMetalAppearance() )
        MacSetBackgroundBrush( wxNullBrush );

    // normal system font is too tall for fitting into the standard height
    SetWindowVariant( wxWINDOW_VARIANT_SMALL );

    return true;
}

void wxStatusBarMac::DrawFieldText(wxDC& dc, int i)
{
    int w, h;
    GetSize( &w , &h );
    wxRect rect;
    GetFieldRect( i, rect );

    if ( !MacIsReallyHilited() )
        dc.SetTextForeground( wxColour( 0x80, 0x80, 0x80 ) );

    wxString text(GetStatusText( i ));

    long x, y;

    dc.GetTextExtent(text, &x, &y);

    int leftMargin = 2;
    int xpos = rect.x + leftMargin + 1;
    int ypos = 1;

    if ( MacGetTopLevelWindow()->MacGetMetalAppearance() )
        ypos++;

    dc.SetClippingRegion(rect.x, 0, rect.width, h);

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
        wxT("invalid status bar field index") );

    if ( m_statusStrings[number] == text )
        return ;

    m_statusStrings[number] = text;
    wxRect rect;
    GetFieldRect(number, rect);
    int w, h;
    GetSize( &w, &h );
    rect.y = 0;
    rect.height = h ;
    Refresh( true, &rect );
    Update();
}

void wxStatusBarMac::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.Clear();

    int major, minor;
    wxGetOsVersion( &major, &minor );
    int w, h;
    GetSize( &w, &h );

    if ( MacIsReallyHilited() )
    {
        wxPen white( *wxWHITE , 1 , wxSOLID );
        if (major >= 10)
        {
            // Finder statusbar border color: (Project Builder similar is 9B9B9B)
            if ( MacGetTopLevelWindow()->MacGetMetalAppearance() )
                dc.SetPen(wxPen(wxColour(0x40, 0x40, 0x40), 1, wxSOLID));
            else
                dc.SetPen(wxPen(wxColour(0xB1, 0xB1, 0xB1), 1, wxSOLID));
        }
        else
        {
            wxPen black( *wxBLACK , 1 , wxSOLID );
            dc.SetPen(black);
        }

        dc.DrawLine(0, 0, w, 0);
        dc.SetPen(white);
        dc.DrawLine(0, 1, w, 1);
    }
    else
    {
        if (major >= 10)
            // Finder statusbar border color: (Project Builder similar is 9B9B9B)
            dc.SetPen(wxPen(wxColour(0xB1, 0xB1, 0xB1), 1, wxSOLID));
        else
            dc.SetPen(wxPen(wxColour(0x80, 0x80, 0x80), 1, wxSOLID));

        dc.DrawLine(0, 0, w, 0);
    }

    int i;
    if ( GetFont().Ok() )
        dc.SetFont(GetFont());
    dc.SetBackgroundMode(wxTRANSPARENT);

    for ( i = 0; i < m_nFields; i ++ )
        DrawField(dc, i);
}

void wxStatusBarMac::MacHiliteChanged()
{
    Refresh();
    Update();
}

#endif // wxUSE_STATUSBAR

