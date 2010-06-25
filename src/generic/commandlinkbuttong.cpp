/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/commandlinkbuttong.cpp
// Purpose:     wxGenericCommandLinkButton
// Author:      Rickard Westerlund
// Created:     2010-06-23
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMMANDLINKBUTTON

#include "wx/commandlinkbutton.h"
#include "wx/artprov.h"

// ----------------------------------------------------------------------------
// Generic command link button
// ----------------------------------------------------------------------------

bool wxGenericCommandLinkButton::Create(wxWindow *parent,
                                        wxWindowID id,
                                        const wxString& mainLabel,
                                        const wxString& note,
                                        const wxPoint& pos,
                                        const wxSize& size,
                                        long style,
                                        const wxValidator& validator,
                                        const wxString& name)
{
    bool created = wxButton::Create(parent,
                                    id,
                                    mainLabel + '\n' + note,
                                    pos,
                                    size,
                                    style,
                                    validator,
                                    name);

    if ( created && !HasNativeBitmap() )
        SetDefaultBitmap();

    return created;

}

void wxGenericCommandLinkButton::SetDefaultBitmap()
{
    SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_BUTTON));
}

#endif // wxUSE_COMMANDLINKBUTTON

