// Copyright (c) 2007 John Wilmes

#include "wx/colour.h"
#include "wx/string.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include <Magick++.h>

Magick::Color GetMagickColor(const wxColour& wxcol);

wxColour GetWxColour(const Magick::Color& mcol);

char * GetMagick(wxBitmapType type);

wxString GetJsonPoints(int n, wxPoint points[]);

wxString GetJsonBrush(const wxBrush& brush);

wxString GetJsonPen(const wxPen& pen);

wxString GetJsonFont(const wxFont& font);

Magick::LineCap GetMagickCap(int cap);

Magick::LineJoin GetMagickJoin(int join);

Magick::CompositeOperator GetMagickCompositeOp(int op);
