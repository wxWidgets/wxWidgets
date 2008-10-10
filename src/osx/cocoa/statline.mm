/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/statline.mm
// Purpose:     wxStaticLine class
// Author:      Stefan Csomor
// Created:     28.06.99
// Version:     $Id: statline.mm 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) 2008 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATLINE

#include "wx/statline.h"

#ifndef WX_PRECOMP
    #include "wx/statbox.h"
#endif

#include "wx/osx/private.h"

wxWidgetImplType* wxWidgetImpl::CreateStaticLine( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) 
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSBox* v = [[wxNSBox alloc] initWithFrame:r];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif //wxUSE_STATLINE
