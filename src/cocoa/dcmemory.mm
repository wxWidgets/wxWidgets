/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dcmemory.mm
// Purpose:     wxMemoryDC class
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/dcmemory.h"
#include "wx/log.h"

#import <AppKit/NSImage.h>
#import <AppKit/NSAffineTransform.h>
#import <AppKit/NSGraphicsContext.h>

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxDC)

wxMemoryDC::wxMemoryDC(void)
{
    m_cocoaNSImage = NULL;
    m_ok = false;
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
        [m_cocoaNSImage lockFocusOnRepresentation: m_selectedBitmap.GetNSBitmapImageRep()];
        sm_cocoaDCStack.Insert(this);
        return true;
    }
    return false;
}

bool wxMemoryDC::CocoaUnlockFocus()
{
    [m_cocoaNSImage unlockFocus];
    return true;
}

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    CocoaUnwindStackAndLoseFocus();
    [m_cocoaNSImage release];
    m_cocoaNSImage = nil;
    m_selectedBitmap = bitmap;
    if(m_selectedBitmap.Ok())
    {
        m_cocoaNSImage = [[NSImage alloc]
                initWithSize:NSMakeSize(m_selectedBitmap.GetWidth(),
                    m_selectedBitmap.GetHeight())];
        [m_cocoaNSImage addRepresentation: m_selectedBitmap.GetNSBitmapImageRep()];
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

    wxLogDebug("[m_cocoaNSImage isFlipped]=%d", [m_cocoaNSImage isFlipped]);
    [m_cocoaNSImage drawAtPoint: NSMakePoint(0,0)
        fromRect: NSMakeRect(xsrc,
            m_selectedBitmap.GetHeight()-height-ysrc,
            width, height)
        operation: NSCompositeCopy // FIXME: raster ops
        fraction: 1.0];
        
    [context restoreGraphicsState];
    return false;
}

