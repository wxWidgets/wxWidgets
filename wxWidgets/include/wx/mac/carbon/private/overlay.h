/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_CARBON_PRIVATE_OVERLAY_H_
#define _WX_MAC_CARBON_PRIVATE_OVERLAY_H_

#include "wx/mac/private.h"
#include "wx/toplevel.h"
#include "wx/graphics.h"

class wxOverlayImpl
{
public:
    wxOverlayImpl() ;
    ~wxOverlayImpl() ;
    
    
    // clears the overlay without restoring the former state
    // to be done eg when the window content has been changed and repainted
    void Reset();
    
    // returns true if it has been setup
    bool IsOk();
    
    void Init( wxWindowDC* dc, int x , int y , int width , int height );
    
    void BeginDrawing( wxWindowDC* dc);
    
    void EndDrawing( wxWindowDC* dc);
    
    void Clear( wxWindowDC* dc);

private:
    OSStatus CreateOverlayWindow();
    
    void MacGetBounds( Rect *bounds );
    
    WindowRef m_overlayWindow;
    WindowRef m_overlayParentWindow;
    CGContextRef m_overlayContext ;
    // we store the window in case we would have to issue a Refresh()
    wxWindow* m_window ;
    
    int m_x ;
    int m_y ;
    int m_width ;
    int m_height ;
} ;

#endif // _WX_MAC_CARBON_PRIVATE_OVERLAY_H_
