/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:     wxStaticBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)


bool wxStaticBox::Create( wxWindow *parent,
    wxWindowID id,
    const wxString& label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name )
{
    m_macIsUserPane = false;

    if ( !wxControl::Create( parent, id, pos, size, style, wxDefaultValidator, name ) )
        return false;

    m_label = label;

    Rect bounds = wxMacGetBoundsForControl( this, pos, size );

    m_peer = new wxMacControl( this );
    OSStatus err = CreateGroupBoxControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, CFSTR(""),
        true /*primary*/, m_peer->GetControlRefAddr() );
    verify_noerr( err );

    MacPostControlCreate( pos, size );

    return true;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    static int extraTop = -1; // Uninitted
    static int other = 5;

    if ( extraTop == -1 )
    {
        // The minimal border used for the top.
        // Later on, the staticbox's font height is added to this.
        extraTop = 0;

        if ( UMAGetSystemVersion() >= 0x1030 ) // Panther
        {
            // As indicated by the HIG, Panther needs an extra border of 11
            // pixels (otherwise overlapping occurs at the top). The "other"
            // border has to be 11.
            extraTop = 11;
            other = 11;
        }
    }

    *borderTop = extraTop;
    if ( !m_label.empty() )
        *borderTop += GetCharHeight();

    *borderOther = other;
}

#endif // wxUSE_STATBOX

