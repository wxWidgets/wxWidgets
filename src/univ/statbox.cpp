/////////////////////////////////////////////////////////////////////////////
// Name:        univ/statbox.cpp
// Purpose:     wxStaticBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univstatbox.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATTEXT

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/statbox.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString &label,
                         const wxPoint &pos,
                         const wxSize &size,
                         long style,
                         const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetLabel(label);

    return TRUE;
}

void wxStaticBox::DoDraw(wxControlRenderer *renderer)
{
    // we never have a border, so don't call the base class version whcih draws
    // it
    renderer->DrawFrame();
}

#endif // wxUSE_STATTEXT

