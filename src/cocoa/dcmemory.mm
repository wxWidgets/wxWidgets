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

#import <AppKit/NSImage.h>

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

