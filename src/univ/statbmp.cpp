/////////////////////////////////////////////////////////////////////////////
// Name:        univ/statbmp.cpp
// Purpose:     wxStaticBitmap implementation
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
    #pragma implementation "univstatbmp.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBMP

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/icon.h"
    #include "wx/statbmp.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

// ----------------------------------------------------------------------------
// wxStaticBitmap
// ----------------------------------------------------------------------------

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmap &label,
                            const wxPoint &pos,
                            const wxSize &size,
                            long style,
                            const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    // set bitmap first
    SetBitmap(label);

    // and adjust our size to fit it after this
    if ( size.x == -1 || size.y == -1 )
    {
        wxSize sizeBest = DoGetBestSize();
        if ( size.x != -1 )
            sizeBest.x = size.x;
        if ( size.y != -1 )
            sizeBest.y = size.y;

        SetSize(sizeBest);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// bitmap/icon setting/getting and converting between
// ----------------------------------------------------------------------------

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{
#ifdef __WXMSW__
    m_bitmap.CopyFromIcon(icon);
#else
    m_bitmap = (const wxBitmap&)icon;
#endif
}

wxIcon wxStaticBitmap::GetIcon() const
{
    wxIcon icon;
#ifdef __WXMSW__
    icon.CopyFromBitmap(m_bitmap);
#else
    icon = (const wxIcon&)m_bitmap;
#endif
    return icon;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

wxSize wxStaticBitmap::DoGetBestSize() const
{
    wxSize sz = wxStaticBitmapBase::DoGetBestSize();
    wxTheme::Get()->GetRenderer()->AdjustSize(&sz, this);
    return sz;
}

void wxStaticBitmap::DoDraw(wxControlRenderer *renderer)
{
    wxControl::DoDraw(renderer);
    renderer->DrawBitmap(GetBitmap());
}

#endif // wxUSE_STATBMP

