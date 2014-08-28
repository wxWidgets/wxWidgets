/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/stattext.cpp
// Purpose:     wxStaticText
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString &label,
                          const wxPoint &pos,
                          const wxSize &size,
                          long style,
                          const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    SetLabel(label);
    SetInitialSize(size);

    return true;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxStaticText::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawLabel();
}

void wxStaticText::SetLabel(const wxString& str)
{
    // save original label
    m_labelOrig = str;

    // draw as real label the abbreviated version of it
    DoSetLabel(GetEllipsizedLabel());
}

void wxStaticText::DoSetLabel(const wxString& str)
{
    UnivDoSetLabel(str);
}

wxString wxStaticText::DoGetLabel() const
{
    return wxControl::GetLabel();
}

/*
   FIXME: UpdateLabel() should be called on size events to allow correct
          dynamic ellipsizing of the label
*/

#endif // wxUSE_STATTEXT
