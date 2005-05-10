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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "statbox.h"
#endif

#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

/*
 * Static box
 */

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_macIsUserPane = FALSE ;

    if ( !wxControl::Create(parent, id, pos, size,
                            style, wxDefaultValidator, name) )
        return false;

    m_label = label ;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

    m_peer = new wxMacControl(this) ;
    verify_noerr(CreateGroupBoxControl(MAC_WXHWND(parent->MacGetTopLevelWindowRef()),&bounds, CFSTR("") ,
        true /*primary*/ , m_peer->GetControlRefAddr() ) ) ;

    MacPostControlCreate(pos,size) ;

    return TRUE;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    static int extraTop = -1; // Uninitted
    static int other = 5;

    if ( extraTop == -1 )
    {
        // The minimal border used for the top. Later on the staticbox'
        // font height is added to this.
        extraTop = 0;

        if ( UMAGetSystemVersion() >= 0x1030 /*Panther*/ )
        {
            // As indicated by the HIG, Panther needs an extra border of 11
            // pixels (otherwise overlapping occurs at the top). The "other"
            // border has to be 11.
            extraTop = 11;
            other = 11;
        }

    }

    *borderTop = extraTop + GetCharHeight();
    *borderOther = other;
}

#endif // wxUSE_STATBOX

