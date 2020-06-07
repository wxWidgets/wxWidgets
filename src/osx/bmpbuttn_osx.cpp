/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/bmpbuttn_osx.cpp
// Purpose:     wxBitmapButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"
#include "wx/image.h"

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif

#include "wx/osx/private.h"

//---------------------------------------------------------------------------

bool wxBitmapButton::Create( wxWindow *parent,
                             wxWindowID id,
                             const wxBitmap& bitmap,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString& name )
{
    DontCreatePeer();
    
    if ( !wxBitmapButtonBase::Create(parent, id, pos, size, style,
                                     validator, name) )
        return false;

    m_marginX =
    m_marginY = wxDEFAULT_BUTTON_MARGIN;

    m_bitmaps[State_Normal] = bitmap;

    SetPeer(wxWidgetImpl::CreateBitmapButton( this, parent, id, bitmap, pos, size, style, GetExtraStyle() ));

    MacPostControlCreate( pos, size );

    return true;
}

wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize best(m_marginX, m_marginY);

    best *= 2;

    if ( GetBitmapLabel().IsOk() )
    {
        const wxSize bitmapSize = GetBitmapLabel().GetScaledSize();
        best += bitmapSize;

        // The NSRoundedBezelStyle and NSTexturedRoundedBezelStyle used when
        // the image is less than 20px tall have a small horizontal border,
        // account for it here to prevent part of the image from being cut off.
        //
        // Note that the magic 20px comes from SetBezelStyleFromBorderFlags()
        // defined in src/osx/cocoa/button.mm.
        if ( bitmapSize.y < 20 )
            best += wxSize(4,0);
    }

    return best;
}

#endif // wxUSE_BMPBUTTON
