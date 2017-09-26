/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/overlay.mm
// Purpose:     common wxOverlay code
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// Copyright:   (c) wxWidgets team
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

#include "wx/overlay.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

#include "wx/dcgraph.h"

#include "wx/private/overlay.h"

#ifdef wxHAS_NATIVE_OVERLAY

// ============================================================================
// implementation
// ============================================================================

wxOverlayImpl::wxOverlayImpl()
{
    m_window = NULL ;
    m_overlayContext = NULL ;
    m_overlayWindow = NULL ;
}

wxOverlayImpl::~wxOverlayImpl()
{
    Reset();
}

bool wxOverlayImpl::IsOk()
{
    return m_overlayWindow != NULL ;
}

void wxOverlayImpl::CreateOverlayWindow( wxDC* dc )
{
    if (m_window)
    {
        m_overlayParentWindow = m_window->MacGetTopLevelWindowRef();
        [m_overlayParentWindow makeKeyAndOrderFront:nil];

        wxPoint origin(m_x, m_y);
        if (!dc->IsKindOf(CLASSINFO(wxClientDC)))
            origin -= m_window->GetClientAreaOrigin();

        origin = m_window->ClientToScreen(origin);

        wxSize size(m_width, m_height);
        NSRect overlayRect = wxToNSRect(NULL, wxRect(origin, size));
        overlayRect = [NSWindow contentRectForFrameRect:overlayRect styleMask:NSBorderlessWindowMask];

        m_overlayWindow = [[NSWindow alloc] initWithContentRect:overlayRect
                                                      styleMask:NSBorderlessWindowMask
                                                        backing:NSBackingStoreBuffered
                                                          defer:YES];
        [m_overlayParentWindow addChildWindow:m_overlayWindow ordered:NSWindowAbove];
    }
    else
    {
        m_overlayParentWindow = NULL;
        CGRect cgbounds;
        cgbounds = CGDisplayBounds(CGMainDisplayID());

        m_overlayWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(cgbounds.origin.x, cgbounds.origin.y,
                                                                    cgbounds.size.width,
                                                                    cgbounds.size.height)
                                                      styleMask:NSBorderlessWindowMask
                                                        backing:NSBackingStoreBuffered
                                                          defer:YES];
    }
    [m_overlayWindow setOpaque:NO];
    [m_overlayWindow setIgnoresMouseEvents:YES];
    [m_overlayWindow setBackgroundColor:[NSColor clearColor]];
    [m_overlayWindow setAlphaValue:1.0];

    [m_overlayWindow orderFront:nil];
}

void wxOverlayImpl::Init( wxDC* dc, int x , int y , int width , int height )
{
    wxASSERT_MSG( !IsOk() , _("You cannot Init an overlay twice") );

    m_window = dc->GetWindow();
    m_x = x ;
    m_y = y ;
    m_width = width ;
    m_height = height ;

    CreateOverlayWindow(dc);
    wxASSERT_MSG(m_overlayWindow != NULL, _("Couldn't create the overlay window"));
    m_overlayContext = (CGContextRef) [[m_overlayWindow graphicsContext] graphicsPort];
    wxASSERT_MSG(  m_overlayContext != NULL , _("Couldn't init the context on the overlay window") );
}

void wxOverlayImpl::BeginDrawing( wxDC* dc)
{
    wxDCImpl *impl = dc->GetImpl();
    wxGCDCImpl *win_impl = wxDynamicCast(impl,wxGCDCImpl);
    if (win_impl)
    {
        int ySize = 0;
        if ( m_window )
        {
            ySize = m_height;
        }
        else
        {
            CGRect cgbounds ;
            cgbounds = CGDisplayBounds(CGMainDisplayID());
            ySize = cgbounds.size.height;
        }

        wxGraphicsContext* ctx = wxGraphicsContext::CreateFromNative( m_overlayContext );
        ctx->Translate(0, ySize);
        ctx->Scale(1,-1);
        
        win_impl->SetGraphicsContext( ctx );
        
        if (m_window)
            dc->SetDeviceOrigin(dc->LogicalToDeviceX(-m_x), dc->LogicalToDeviceY(-m_y));
        dc->SetClippingRegion(m_x, m_y, m_width, m_height);
    }
}

void wxOverlayImpl::EndDrawing( wxDC* dc)
{
    wxDCImpl *impl = dc->GetImpl();
    wxGCDCImpl *win_impl = wxDynamicCast(impl,wxGCDCImpl);
    if (win_impl)
        win_impl->SetGraphicsContext(NULL);

    CGContextFlush( m_overlayContext );
}

void wxOverlayImpl::Clear(wxDC* dc)
{
    wxASSERT_MSG( IsOk() , _("You cannot Clear an overlay that is not inited") );

    dc->GetGraphicsContext()->ClearRectangle(m_x - 1, m_y - 1, m_width + 2, m_height + 2);
}

void wxOverlayImpl::Reset()
{
    if ( m_overlayContext )
    {
        m_overlayContext = NULL ;
    }

    // todo : don't dispose, only hide and reposition on next run
    if (m_overlayWindow)
    {
        [m_overlayParentWindow removeChildWindow:m_overlayWindow];
        [m_overlayWindow release];
        m_overlayWindow = NULL ;
    }
}

#endif // wxHAS_NATIVE_OVERLAY
