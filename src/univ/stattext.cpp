/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/stattext.cpp
// Purpose:     wxStaticText
// Author:      Vadim Zeitlin
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
    if ( str == m_labelOrig )
        return;

    // save original label
    m_labelOrig = str;

    // draw as real label the abbreviated version of it
    WXSetVisibleLabel(GetEllipsizedLabel());
}

void wxStaticText::WXSetVisibleLabel(const wxString& str)
{
    UnivDoSetLabel(str);
}

wxString wxStaticText::WXGetVisibleLabel() const
{
    return wxControl::GetLabel();
}

/*
   FIXME: UpdateLabel() should be called on size events to allow correct
          dynamic ellipsizing of the label
*/

#endif // wxUSE_STATTEXT
