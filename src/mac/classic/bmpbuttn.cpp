/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/bitmap.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#include "wx/mac/uma.h"

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    // since bitmapbuttonbase is subclass of button calling wxBitmapButtonBase::Create
    // essentially creates an additional button
    if ( !wxControl::Create(parent, id, pos, size,
                                     style, validator, name) )
        return false;

    m_bmpNormal = bitmap;

    if (style & wxBU_AUTODRAW)
    {
        m_marginX = wxDEFAULT_BUTTON_MARGIN;
        m_marginY = wxDEFAULT_BUTTON_MARGIN;
    }
    else
    {
        m_marginX = 0;
        m_marginY = 0;
    }

    int width = size.x;
    int height = size.y;

    if ( bitmap.Ok() )
    {
        wxSize newSize = DoGetBestSize();
        if ( width == -1 )
            width = newSize.x;
        if ( height == -1 )
            height = newSize.y;
    }

    Rect bounds ;
    Str255 title ;
    m_bmpNormal = bitmap;
    wxBitmapRefData * bmap = NULL ;

    if ( m_bmpNormal.Ok() )
        bmap = (wxBitmapRefData*) ( m_bmpNormal.GetRefData()) ;

    MacPreControlCreate( parent , id ,  wxEmptyString , pos , wxSize( width , height ) ,style, validator , name , &bounds , title ) ;

    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 ,
        kControlBehaviorOffsetContents +
            ( bmap && bmap->m_bitmapType == kMacBitmapTypeIcon ?
            kControlContentCIconHandle : kControlContentPictHandle ) , 0,
          (( style & wxBU_AUTODRAW ) ? kControlBevelButtonSmallBevelProc : kControlBevelButtonNormalBevelProc ), (long) this ) ;
    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;

    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , m_bmpNormal ) ;
    if ( info.contentType != kControlNoContent )
    {
        ::SetControlData( (ControlHandle) m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
    }
    MacPostControlCreate() ;

    return true;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_bmpNormal = bitmap;
    InvalidateBestSize();

    ControlButtonContentInfo info ;
    wxMacCreateBitmapButton( &info , m_bmpNormal ) ;
    if ( info.contentType != kControlNoContent )
    {
        ::SetControlData( (ControlHandle) m_macControl , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
    }
}


wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize best;
    if (m_bmpNormal.Ok())
    {
        best.x = m_bmpNormal.GetWidth() + 2*m_marginX;
        best.y = m_bmpNormal.GetHeight() + 2*m_marginY;
    }
    return best;
}

#endif // wxUSE_BMPBUTTON
