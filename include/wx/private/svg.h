/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/svg.h
// Purpose:     Private SVG helpers shared by wxSVGFileDC and wxSVGGraphicsContext
// Author:      Blake Madden
// Copyright:   (c) Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SVG_H_
#define _WX_PRIVATE_SVG_H_

#include "wx/defs.h"

#if wxUSE_SVG

#include "wx/string.h"
#include "wx/colour.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/dcmemory.h"
#include "wx/dcsvg.h"

namespace wxSVG
{

// Formats a double in C locale with 2-digit precision.
wxString NumStr(double f);
wxString NumStr(float f);

// Returns the colour as "#rrggbb" and optionally its alpha as 0..1 opacity.
wxString Col2SVG(wxColour c, float* opacity = nullptr);

// Returns a "stroke=... stroke-opacity=..." attribute fragment.
wxString GetPenStroke(const wxColour& c, int style = wxPENSTYLE_SOLID);

// Returns a "fill=... fill-opacity=..." attribute fragment.
wxString GetBrushFill(const wxColour& c, int style = wxBRUSHSTYLE_SOLID);

// Returns a <pattern> element definition for hatched brushes, or empty.
wxString CreateBrushFill(const wxBrush& brush, wxSVGShapeRenderingMode mode);

// Returns a "shape-rendering=..." attribute fragment.
wxString GetRenderMode(wxSVGShapeRenderingMode style);

// Returns a "fill=url(#...)" attribute for a hatched brush, or empty.
wxString GetBrushPattern(const wxBrush& brush);

// Returns the unique pattern name (id) for a hatched brush, or empty.
wxString GetBrushStyleName(const wxBrush& brush);

// Returns "stroke-width=... stroke-linecap=... stroke-linejoin=..." fragment.
wxString GetPenStyle(const wxPen& pen);

// Returns a "stroke-dasharray=..." attribute fragment for dashed pens, or empty.
wxString GetPenPattern(const wxPen& pen);

// Returns a wxMemoryDC configured with the given font, scaled so that its
// metrics match the SVG's DPI-independent coordinate system.
wxMemoryDC GetTextMetricDC(const wxFont& font);

} // namespace wxSVG

#endif // wxUSE_SVG

#endif // _WX_PRIVATE_SVG_H_
