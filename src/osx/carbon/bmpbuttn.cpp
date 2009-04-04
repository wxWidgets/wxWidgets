/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/bmpbuttn.cpp
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
#include "wx/image.h"

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif

#include "wx/osx/private.h"

//---------------------------------------------------------------------------
// Helper functions

static wxBitmap wxMakeStdSizeBitmap(const wxBitmap& bitmap)
{
    // in Mac OS X the icon controls (which are used for borderless bitmap
    // buttons) can have only one of the few standard sizes and if they
    // don't, the OS rescales them automatically resulting in really ugly
    // images, so centre the image in a square of standard size instead

    // the supported sizes, sorted in decreasng order
    static const int stdSizes[] = { 128, 48, 32, 16, 0 };

    const int width = bitmap.GetWidth();
    const int height = bitmap.GetHeight();

    wxBitmap newBmp(bitmap);
    
    int n;
    for ( n = 0; n < (int)WXSIZEOF(stdSizes); n++ )
    {
        const int sizeStd = stdSizes[n];
        if ( width > sizeStd || height > sizeStd )
        {
            // it will become -1 if the bitmap is larger than the biggest
            // supported size, this is intentional
            n--;

            break;
        }
    }

    if ( n != -1 )
    {
        const int sizeStd = stdSizes[n];
        if ( width != sizeStd || height != sizeStd )
        {
            wxASSERT_MSG( width <= sizeStd && height <= sizeStd,
                          _T("bitmap shouldn't be cropped") );

            wxImage square_image = bitmap.ConvertToImage();
            newBmp = square_image.Size
                     (
                         wxSize(sizeStd, sizeStd),
                         wxPoint((sizeStd - width)/2, (sizeStd-height)/2)
                     );
        }
    }
    //else: let the system rescale the bitmap

    return newBmp;
}

//---------------------------------------------------------------------------

class wxMacBitmapButton : public wxMacControl
{
public:
    wxMacBitmapButton( wxWindowMac* peer ) : wxMacControl(peer)
    {
    }
    
    void SetBitmap(const wxBitmap& bitmap)
    {
        wxBitmap bmp;
        if ( GetWXPeer()->HasFlag( wxBORDER_NONE ) )
        {
            bmp = wxMakeStdSizeBitmap(bitmap);
            // TODO set bitmap in peer as well
        }
        else
            bmp = bitmap;

        ControlButtonContentInfo info;

        if ( GetWXPeer()->HasFlag( wxBORDER_NONE ) )
        {        
            wxMacCreateBitmapButton( &info, bmp, kControlContentIconRef );
            if ( info.contentType != kControlNoContent )
                SetData( kControlIconPart, kControlIconContentTag, info );
        }
        else
        {
            wxMacCreateBitmapButton( &info, bmp );
            if ( info.contentType != kControlNoContent )
                SetData( kControlButtonPart, kControlBevelButtonContentTag, info );
        }

        wxMacReleaseBitmapButton( &info );
    }
};

wxWidgetImplType* wxWidgetImpl::CreateBitmapButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID WXUNUSED(id), 
                                    const wxBitmap& bitmap,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long WXUNUSED(extraStyle))
{
    OSStatus err = noErr;
    ControlButtonContentInfo info;

    Rect bounds = wxMacGetBoundsForControl( wxpeer, pos, size );
    wxMacControl* peer = new wxMacBitmapButton( wxpeer );
    wxBitmap bmp;

    if ( bitmap.Ok() && (style & wxBORDER_NONE) )
    {
        bmp = wxMakeStdSizeBitmap(bitmap);
        // TODO set bitmap in peer as well
    }
    else
        bmp = bitmap;


    if ( style & wxBORDER_NONE )
    {
		// contrary to the docs this control only works with iconrefs
        wxMacCreateBitmapButton( &info, bmp, kControlContentIconRef );
        err = CreateIconControl(
                MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
                &bounds, &info, false, peer->GetControlRefAddr() );
    }
    else
    {
        wxMacCreateBitmapButton( &info, bmp );
        err = CreateBevelButtonControl(
                MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, CFSTR(""),
                ((style & wxBU_AUTODRAW) ? kControlBevelButtonSmallBevel : kControlBevelButtonNormalBevel ),
                kControlBehaviorOffsetContents, &info, 0, 0, 0, peer->GetControlRefAddr() );
    }

    verify_noerr( err );

    wxMacReleaseBitmapButton( &info );
    return peer;
}
#endif
