/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/statbmp_osx.cpp
// Purpose:     wxStaticBitmap
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBMP

#include "wx/statbmp.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

#include <stdio.h>


bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmap& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name )
{
    m_scaleMode = Scale_None;

    DontCreatePeer();

    if (! wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name))
        return false;

    SetPeer(wxWidgetImpl::CreateStaticBitmap( this, parent, id, bitmap , pos, size, style, GetExtraStyle() ));

    MacPostControlCreate( pos, size );

    SetBitmap(bitmap);

    return true;
}


void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    SetInitialSize(GetBitmapSize());

    GetPeer()->SetBitmap(bitmap);

    Refresh();
}

#endif //if wxUSE_STATBMP
