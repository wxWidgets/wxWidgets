/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cocoa/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_COCOA_PRIVATE_OVERLAY_H_
#define _WX_OSX_COCOA_PRIVATE_OVERLAY_H_

#include "wx/osx/private.h"
#include "wx/toplevel.h"
#include "wx/graphics.h"
#include "wx/private/overlay.h"


class wxOverlayCocoaImpl : public wxOverlayImpl
{
public:
    wxOverlayCocoaImpl() ;
    ~wxOverlayCocoaImpl() ;


    // clears the overlay without restoring the former state
    // to be done eg when the window content has been changed and repainted
    virtual void Reset() wxOVERRIDE;

    // returns true if it has been setup
    virtual bool IsOk() wxOVERRIDE;

    virtual void InitFromDC( wxDC* dc, int x , int y , int width , int height ) wxOVERRIDE;

    virtual void InitFromWindow(wxWindow* win, bool fullscreen) wxOVERRIDE;

    virtual void BeginDrawing( wxDC* dc) wxOVERRIDE;

    virtual void EndDrawing( wxDC* dc) wxOVERRIDE;

    virtual void Clear( wxDC* dc) wxOVERRIDE;

private:
    void CreateOverlayWindow( wxDC* dc );

    WXWindow m_overlayWindow;
    WXWindow m_overlayParentWindow;
    CGContextRef m_overlayContext ;
    // we store the window in case we would have to issue a Refresh()
    wxWindow* m_window ;

    int m_x ;
    int m_y ;
    int m_width ;
    int m_height ;
} ;

#endif // _WX_MAC_CARBON_PRIVATE_OVERLAY_H_
