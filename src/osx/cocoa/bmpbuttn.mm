/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: bmpbuttn.cpp 54820 2008-07-29 20:04:11Z SC $
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

class wxBitmapButtonCocoaImpl : public wxWidgetCocoaImpl, public wxBitmapButtonImpl
{
    public :
    wxBitmapButtonCocoaImpl( wxWindowMac* peer , WXWidget w) : wxWidgetCocoaImpl(peer,w)
    {
    }
    
    void SetPressedBitmap( const wxBitmap& bitmap )
    {
        wxNSButton* button = (wxNSButton*) m_osxView;
        [button setAlternateImage: bitmap.GetNSImage()];
        [button setButtonType:NSMomentaryChangeButton];
    }
} ;

wxWidgetImplType* wxWidgetImpl::CreateBitmapButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxBitmap& bitmap,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    // trying to get as close as possible to flags
    if ( style & wxBORDER_NONE )
    {
        [v setBezelStyle:NSShadowlessSquareBezelStyle];
        [v setBordered:NO]; 
    }
    else
    {
        // see trac #11128 for a thorough discussion
        if ( (style & wxBORDER_MASK) == wxBORDER_RAISED )
            [v setBezelStyle:NSRegularSquareBezelStyle];
        else if ( (style & wxBORDER_MASK) == wxBORDER_SUNKEN )
            [v setBezelStyle:NSSmallSquareBezelStyle];
        else
            [v setBezelStyle:NSShadowlessSquareBezelStyle];
    }

    if (bitmap.Ok())
        [v setImage:bitmap.GetNSImage() ];

    [v setButtonType:NSMomentaryPushInButton];
    wxWidgetCocoaImpl* c = new wxBitmapButtonCocoaImpl( wxpeer, v );
    return c;
}

#endif
