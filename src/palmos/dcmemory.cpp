/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
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

#include "wx/dcmemory.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/log.h"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMemoryDC
// ----------------------------------------------------------------------------

wxMemoryDC::wxMemoryDC( const wxBitmap& bitmap )
{
    if ( bitmap.IsOk() )
        SelectObject(bitmap);
}

wxMemoryDC::wxMemoryDC(wxDC *dc)
{
}

void wxMemoryDC::Init()
{
}

bool wxMemoryDC::CreateCompatible(wxDC *dc)
{
    return false;
}

void wxMemoryDC::SelectObject(const wxBitmap& bitmap)
{
}

void wxMemoryDC::DoGetSize(int *width, int *height) const
{
}

// the rest of this file deals with drawing rectangles workaround, disabled by
// default

#define wxUSE_MEMORY_DC_DRAW_RECTANGLE 0

#if wxUSE_MEMORY_DC_DRAW_RECTANGLE

// For some reason, drawing a rectangle on a memory DC has problems.
// Use this substitute if we can.
static void wxDrawRectangle(wxDC& dc, wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

#endif // wxUSE_MEMORY_DC_DRAW_RECTANGLE

void wxMemoryDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}
