/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/stattext.cpp
// Purpose:     wxStaticText
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native wxStaticText implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/brush.h"
#endif

#include <Field.h>

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if(!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    // note that wxALIGN_LEFT is equal to 0 so we shouldn't
    // test for it using & operator

    JustificationType align = leftAlign;

    if ( style & wxALIGN_CENTRE )
        align = centerAlign ;
    else if ( style & wxALIGN_RIGHT )
        align = rightAlign;

    return wxControl::PalmCreateField(label, pos, size, false, false, align);
}

wxBorder wxStaticText::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

wxSize wxStaticText::DoGetBestSize() const
{
    return wxSize(0,0);
}

bool wxStaticText::SetFont(const wxFont& font)
{
    return false;
}

#endif // wxUSE_STATTEXT
