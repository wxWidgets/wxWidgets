/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/tglbtn.mm
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxMac.
// Author:      Stefan Csomor
// Modified by:
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

static
void
SetToggleButtonStyle(NSButton *v, long style)
{
    NSButtonType type = NSOnOffButton;

    // This is the appropriate default bezel style for the toggle buttons.
    NSBezelStyle bezel = NSShadowlessSquareBezelStyle;

    switch ( style & wxBORDER_MASK )
    {
        case 0:
            break;

        case wxBORDER_NONE:
            [v setBordered:NO];
            type = NSToggleButton;
            break;

        case wxBORDER_SIMPLE:
            bezel = NSSmallSquareBezelStyle;
            break;

        case wxBORDER_STATIC:
        case wxBORDER_RAISED:
        case wxBORDER_THEME:
        case wxBORDER_SUNKEN:
            break;
    }

    [v setBezelStyle:bezel];
    [v setButtonType:type];
}

wxWidgetImplType* wxWidgetImpl::CreateToggleButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(winid),
                                    const wxString& WXUNUSED(label),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    SetToggleButtonStyle(v, style);

    wxWidgetCocoaImpl* c = new wxButtonCocoaImpl( wxpeer, v );
    return c;
}

wxWidgetImplType* wxWidgetImpl::CreateBitmapToggleButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(winid),
                                    const wxBitmapBundle& label,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSButton* v = [[wxNSButton alloc] initWithFrame:r];

    SetToggleButtonStyle(v, style);
    
    if (label.IsOk())
        [v setImage: wxOSXGetImageFromBundle(label) ];

    [v setButtonType:NSOnOffButton];
    wxWidgetCocoaImpl* c = new wxButtonCocoaImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_TOGGLEBTN

