/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/tglbtn.mm
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxMac.
// Author:      Stefan Csomor
// Created:     08.02.01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declatations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"
#include "wx/osx/private.h"
#include "wx/private/bmpbndl.h"

wxWidgetImplType* wxWidgetImpl::CreateToggleButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID winid,
                                    const wxString& label,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    wxOSXSetBezelStyleFromBorderFlags(v, style, winid, label, wxBitmapBundle(), wxpeer);

    wxWidgetCocoaImpl* c = new wxButtonCocoaImpl( wxpeer, v );
    return c;
}

wxWidgetImplType* wxWidgetImpl::CreateBitmapToggleButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID winid,
                                    const wxBitmapBundle& bundle,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    wxOSXSetBezelStyleFromBorderFlags(v, style, winid, wxEmptyString, bundle, wxpeer);

    if (bundle.IsOk())
        [v setImage: wxOSXGetImageFromBundle(bundle) ];

    wxWidgetCocoaImpl* c = new wxButtonCocoaImpl( wxpeer, v );

    return c;
}

#endif // wxUSE_TOGGLEBTN

