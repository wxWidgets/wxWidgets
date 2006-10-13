/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dcmemory.mm
// Purpose:     wxMemoryDC class
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcmemory.h"
#endif //WX_PRECOMP

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSImage.h>
#import <AppKit/NSAffineTransform.h>
#import <AppKit/NSGraphicsContext.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSBezierPath.h>

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxDC)

wxMemoryDC::wxMemoryDC( const wxBitmap& bitmap )
{
    m_cocoaNSImage = NULL;
    m_ok = false;

    if ( bitmap.IsOk() )
        SelectObject(bitmap);
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
{
    m_cocoaNSImage = NULL;
    m_ok = false;
}

wxMemoryDC::~wxMemoryDC(void)
{
    CocoaUnwindStackAndLoseFocus();
    [m_cocoaNSImage release];
}

bool wxMemoryDC::CocoaLockFocus()
{
    if(m_cocoaNSImage)
    {
        [m_cocoaNSImage lockFocus];
        sm_cocoaDCStack.Insert(this);
        NSAffineTransform *newTransform = CocoaGetWxToBoundsTransform([m_cocoaNSImage isFlipped], [m_cocoaNSImage size].height);
        [newTransform retain];
        [m_cocoaWxToBoundsTransform release];
        m_cocoaWxToBoundsTransform = newTransform;
        CocoaApplyTransformations();
        return true;
    }
    return false;
}

bool wxMemoryDC::CocoaUnlockFocus()
{
    [m_cocoaNSImage unlockFocus];
    return true;
}

// NOTE: The AppKit is unable to draw onto an NSBitmapImageRep so we must
// instead copy the data to an offscreen window, then copy it back
void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    wxAutoNSAutoreleasePool pool;
    if(m_selectedBitmap.Ok())
    {
        CocoaTakeFocus();
        wxASSERT(m_cocoaNSImage);
        m_selectedBitmap.SetNSBitmapImageRep(
            [[NSBitmapImageRep alloc]
                initWithFocusedViewRect:NSMakeRect(0.0,0.0,
                    m_selectedBitmap.GetWidth(),
                    m_selectedBitmap.GetHeight())]);
    }
    CocoaUnwindStackAndLoseFocus();
    [m_cocoaNSImage release];
    m_cocoaNSImage = nil;
    m_selectedBitmap = bitmap;
    if(m_selectedBitmap.Ok())
    {
        // Create an offscreen window of the same size
        m_cocoaNSImage = [[NSImage alloc]
                initWithSize:NSMakeSize(m_selectedBitmap.GetWidth(),
                    m_selectedBitmap.GetHeight())];

        // Now copy the data
        NSImage *nsimage = [m_selectedBitmap.GetNSImage(false) retain];
        [m_cocoaNSImage lockFocus];
        [nsimage drawAtPoint: NSMakePoint(0,0)
            fromRect: NSMakeRect(0.0,0.0,m_selectedBitmap.GetWidth(),m_selectedBitmap.GetHeight())
            operation: NSCompositeCopy
            fraction: 1.0];
        [m_cocoaNSImage unlockFocus];

        [nsimage release];
    }
}

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
    if(width)
        *width = m_selectedBitmap.GetWidth();
    if(height)
        *height = m_selectedBitmap.GetHeight();
}

bool wxMemoryDC::CocoaDoBlitOnFocusedDC(wxCoord xdest, wxCoord ydest,
    wxCoord width, wxCoord height, wxCoord xsrc, wxCoord ysrc,
    int logicalFunc, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{
    if(!m_selectedBitmap.Ok())
        return false;

    NSAffineTransform *transform = [NSAffineTransform transform];
    [transform translateXBy:xdest yBy:ydest];

    NSAffineTransform *flipTransform = [NSAffineTransform transform];
    /*  x' = 1x + 0y + 0
        y' = 0x + -1y + window's height
    */
    NSAffineTransformStruct matrix = {
        1,  0
    ,   0, -1
    ,   0, height
    };
    [flipTransform setTransformStruct: matrix];

    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    [context saveGraphicsState];
    [transform concat];
    [flipTransform concat];

    wxLogTrace(wxTRACE_COCOA,wxT("[m_cocoaNSImage isFlipped]=%d"), [m_cocoaNSImage isFlipped]);
    [m_cocoaNSImage drawAtPoint: NSMakePoint(0,0)
        fromRect: NSMakeRect(xsrc,
            m_selectedBitmap.GetHeight()-height-ysrc,
            width, height)
        operation: NSCompositeCopy // FIXME: raster ops
        fraction: 1.0];

    [context restoreGraphicsState];
    return false;
}

bool wxMemoryDC::CocoaGetBounds(void *rectData)
{
    if(!rectData)
        return false;
    if(!m_cocoaNSImage)
        return false;
    NSRect *pRect = (NSRect*)rectData;
    pRect->origin.x = 0.0;
    pRect->origin.y = 0.0;
    pRect->size = [m_cocoaNSImage size];
    return true;
}

