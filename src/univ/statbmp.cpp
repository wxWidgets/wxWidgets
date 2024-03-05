/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/statbmp.cpp
// Purpose:     wxStaticBitmap implementation
// Author:      Vadim Zeitlin
// Created:     25.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_STATBMP

#include "wx/statbmp.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/icon.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBitmap
// ----------------------------------------------------------------------------

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmapBundle &label,
                            const wxPoint &pos,
                            const wxSize &size,
                            long style,
                            const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    // set bitmap first
    SetBitmap(label);

    // and adjust our size to fit it after this
    SetInitialSize(size);

    return true;
}

// ----------------------------------------------------------------------------
// bitmap/icon setting/getting and converting between
// ----------------------------------------------------------------------------

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    m_bitmapBundle = bitmap;

    InvalidateBestSize();
    SetSize(GetBestSize());
    Refresh();
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxStaticBitmap::DoDraw(wxControlRenderer *renderer)
{
    wxControl::DoDraw(renderer);
    renderer->DrawBitmap(GetBitmap());
}

#endif // wxUSE_STATBMP
