/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/pen.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/pen.h"
#include "wx/colour.h"

wxPen::wxPen()
{
}

wxPen::wxPen( const wxColour &colour, int width, wxPenStyle style)
{
}

#if FUTURE_WXWIN_COMPATIBILITY_3_0
wxPen::wxPen(const wxColour& col, int width, int style)
{
}

#endif

bool wxPen::operator==(const wxPen& pen) const
{
    return false;
}

bool wxPen::operator!=(const wxPen& pen) const
{
    return false;
}

void wxPen::SetColour(const wxColour& col)
{
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
}

void wxPen::SetWidth(int width)
{
}

void wxPen::SetStyle(wxPenStyle style)
{
}

void wxPen::SetStipple(const wxBitmap& stipple)
{
}

void wxPen::SetDashes(int nb_dashes, const wxDash *dash)
{
}

void wxPen::SetJoin(wxPenJoin join)
{
}

void wxPen::SetCap(wxPenCap cap)
{
}

wxColour wxPen::GetColour() const
{
    return wxColour();
}

wxBitmap *wxPen::GetStipple() const
{
    return NULL;
}

wxPenStyle wxPen::GetStyle() const
{
    return wxPenStyle();
}

wxPenJoin wxPen::GetJoin() const
{
    return wxPenJoin();
}

wxPenCap wxPen::GetCap() const
{
    return wxPenCap();
}

int wxPen::GetWidth() const
{
    return 0;
}

int wxPen::GetDashes(wxDash **ptr) const
{
    return 0;
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}

