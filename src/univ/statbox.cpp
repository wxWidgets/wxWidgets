/////////////////////////////////////////////////////////////////////////////
// Name:        univ/statbox.cpp
// Purpose:     wxStaticBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
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

#if wxUSE_STATBOX

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
    // we never have a border, so don't call the base class version which draws
    // it
    renderer->DrawFrame();
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxRect wxStaticBox::GetBorderGeometry() const
{
    // FIXME should use the renderer here
    wxRect rect;
    rect.width =
    rect.x = GetCharWidth() / 2 + 1;
    rect.y = GetCharHeight() + 1;
    rect.height = rect.y / 2;

    return rect;
}

wxPoint wxStaticBox::GetClientAreaOrigin() const
{
    wxPoint pt = wxControl::GetClientAreaOrigin();
    wxRect rect = GetBorderGeometry();
    pt.x += rect.x;
    pt.y += rect.y;

    return pt;
}

void wxStaticBox::DoSetClientSize(int width, int height)
{
    wxRect rect = GetBorderGeometry();

    wxControl::DoSetClientSize(width + rect.x + rect.width,
                               height + rect.y + rect.height);
}

void wxStaticBox::DoGetClientSize(int *width, int *height) const
{
    wxControl::DoGetClientSize(width, height);

    wxRect rect = GetBorderGeometry();
    if ( width )
        *width -= rect.x + rect.width;
    if ( height )
        *height -= rect.y + rect.height;
}

#endif // wxUSE_STATBOX

