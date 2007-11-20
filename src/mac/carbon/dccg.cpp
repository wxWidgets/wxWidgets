/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dccg.cpp
// Purpose:     wxDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dc.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/region.h"
#endif

#include "wx/mac/uma.h"

#ifdef __MSL__
    #if __MSL__ >= 0x6000
        #include "math.h"
        // in case our functions were defined outside std, we make it known all the same
        namespace std { }
        using namespace std ;
    #endif
#endif

#include "wx/mac/private.h"

#ifndef __LP64__

// TODO: update
// The textctrl implementation still needs that (needs what?) for the non-HIView implementation
//
wxMacWindowClipper::wxMacWindowClipper( const wxWindow* win ) :
    wxMacPortSaver( (GrafPtr) GetWindowPort( (WindowRef) win->MacGetTopLevelWindowRef() ) )
{
    m_newPort = (GrafPtr) GetWindowPort( (WindowRef) win->MacGetTopLevelWindowRef() ) ;
    m_formerClip = NewRgn() ;
    m_newClip = NewRgn() ;
    GetClip( m_formerClip ) ;

    if ( win )
    {
        // guard against half constructed objects, this just leads to a empty clip
        if ( win->GetPeer() )
        {
            int x = 0 , y = 0;
            win->MacWindowToRootWindow( &x, &y ) ;

            // get area including focus rect
            CopyRgn( (RgnHandle) ((wxWindow*)win)->MacGetVisibleRegion(true).GetWXHRGN() , m_newClip ) ;
            if ( !EmptyRgn( m_newClip ) )
                OffsetRgn( m_newClip , x , y ) ;
        }

        SetClip( m_newClip ) ;
    }
}

wxMacWindowClipper::~wxMacWindowClipper()
{
    SetPort( m_newPort ) ;
    SetClip( m_formerClip ) ;
    DisposeRgn( m_newClip ) ;
    DisposeRgn( m_formerClip ) ;
}

wxMacWindowStateSaver::wxMacWindowStateSaver( const wxWindow* win ) :
    wxMacWindowClipper( win )
{
    // the port is already set at this point
    m_newPort = (GrafPtr) GetWindowPort( (WindowRef) win->MacGetTopLevelWindowRef() ) ;
    GetThemeDrawingState( &m_themeDrawingState ) ;
}

wxMacWindowStateSaver::~wxMacWindowStateSaver()
{
    SetPort( m_newPort ) ;
    SetThemeDrawingState( m_themeDrawingState , true ) ;
}

#endif
