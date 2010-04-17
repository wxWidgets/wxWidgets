/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/brush.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/brush.h"
#include "wx/colour.h"

wxBrush::wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, wxBrushStyle style )
{
}

#if FUTURE_WXWIN_COMPATIBILITY_3_0
wxBrush::wxBrush(const wxColour& col, int style)
{
}

#endif
wxBrush::wxBrush(const wxBitmap& stipple)
{
}


void wxBrush::SetColour(const wxColour& col)
{
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
}

void wxBrush::SetStyle(wxBrushStyle style)
{
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
}


bool wxBrush::operator==(const wxBrush& brush) const
{
    return false;
}


wxColour wxBrush::GetColour() const
{
    return wxColour();
}

wxBrushStyle wxBrush::GetStyle() const
{
    return wxBrushStyle();
}

wxBitmap *wxBrush::GetStipple() const
{
    return NULL;
}


wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}

