/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/brush.cpp
// Purpose:     wxBrush
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "brush.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/brush.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// ============================================================================
// wxBrush implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)

// ----------------------------------------------------------------------------
// wxBrush ctors/dtor
// ----------------------------------------------------------------------------

wxBrush::wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, int style)
{
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
}

wxBrush::~wxBrush()
{
}

// ----------------------------------------------------------------------------
// wxBrush house keeping stuff
// ----------------------------------------------------------------------------

wxBrush& wxBrush::operator=(const wxBrush& brush)
{
    return *this;
}

bool wxBrush::operator==(const wxBrush& brush) const
{
    return FALSE;
}

wxObjectRefData *wxBrush::CreateRefData() const
{
    return NULL;
}

wxObjectRefData *wxBrush::CloneRefData(const wxObjectRefData *data) const
{
    return NULL;
}

// ----------------------------------------------------------------------------
// wxBrush accessors
// ----------------------------------------------------------------------------

wxColour wxBrush::GetColour() const
{
    return wxNullColour;
}

int wxBrush::GetStyle() const
{
    return -1;
}

wxBitmap *wxBrush::GetStipple() const
{
    return NULL;
}

WXHANDLE wxBrush::GetResourceHandle() const
{
    return (WXHANDLE)0;
}

// ----------------------------------------------------------------------------
// wxBrush setters
// ----------------------------------------------------------------------------

void wxBrush::SetColour(const wxColour& col)
{
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
}

void wxBrush::SetStyle(int style)
{
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
}


