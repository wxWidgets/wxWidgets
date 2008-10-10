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

wxWidgetImplType* wxWidgetImpl::CreateBitmapButton( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxBitmap& bitmap,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) 
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];
    
    [v setBezelStyle:NSRegularSquareBezelStyle];
    [v setImage:bitmap.GetNSImage() ];
    [v setButtonType:NSMomentaryPushInButton];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif
