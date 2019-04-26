/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/svg.cpp
// Purpose:     SVG sample
// Author:      Chris Elliott
// Modified by:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SVG

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
    #include "wx/dcscreen.h"
    #include "wx/icon.h"
    #include "wx/image.h"
    #include "wx/math.h"
#endif

#include "wx/base64.h"
#include "wx/dcsvg.h"
#include "wx/wfstream.h"
#include "wx/filename.h"
#include "wx/mstream.h"
#include "wx/scopedarray.h"

#if wxUSE_MARKUP
    #include "wx/private/markupparser.h"
#endif

// ----------------------------------------------------------
// Global utilities
// ----------------------------------------------------------

namespace
{

// This function returns a string representation of a floating point number in
// C locale (i.e. always using "." for the decimal separator) and with the
// fixed precision (which is 2 for some unknown reason but this is what it was
// in this code originally).
inline wxString NumStr(double f)
{
    return wxString::FromCDouble(f, 2);
}

// Return the colour representation as HTML-like "#rrggbb" string and also
// returns its alpha as opacity number in 0..1 range.
wxString Col2SVG(wxColour c, float *opacity)
{
    if ( c.Alpha() != wxALPHA_OPAQUE )
    {
        *opacity = c.Alpha() / 255.0f;

        // Remove the alpha before using GetAsString(wxC2S_HTML_SYNTAX) as it
        // doesn't support colours with alpha channel.
        c = wxColour(c.GetRGB());
    }
    else // No alpha.
    {
        *opacity = 1.;
    }

    return c.GetAsString(wxC2S_HTML_SYNTAX);
}

wxString wxPenString(wxColour c, int style = wxPENSTYLE_SOLID)
{
    float opacity;
    wxString s = wxS("stroke:") + Col2SVG(c, &opacity) + wxS("; ");

    switch ( style )
    {
        case wxPENSTYLE_SOLID:
        case wxPENSTYLE_DOT:
        case wxPENSTYLE_SHORT_DASH:
        case wxPENSTYLE_LONG_DASH:
        case wxPENSTYLE_DOT_DASH:
        case wxPENSTYLE_USER_DASH:
            s += wxString::Format(wxS("stroke-opacity:%s; "), NumStr(opacity));
            break;
        case wxPENSTYLE_TRANSPARENT:
            s += wxS("stroke-opacity:0.0; ");
            break;
        default:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Pen Style not available"));
    }

    return s;
}

wxString wxBrushString(wxColour c, int style = wxBRUSHSTYLE_SOLID)
{
    float opacity;
    wxString s = wxS("fill:") + Col2SVG(c, &opacity) + wxS("; ");

    switch ( style )
    {
        case wxBRUSHSTYLE_SOLID:
        case wxBRUSHSTYLE_FDIAGONAL_HATCH:
        case wxBRUSHSTYLE_CROSSDIAG_HATCH:
        case wxBRUSHSTYLE_CROSS_HATCH:
        case wxBRUSHSTYLE_VERTICAL_HATCH:
        case wxBRUSHSTYLE_HORIZONTAL_HATCH:
            s += wxString::Format(wxS("fill-opacity:%s; "), NumStr(opacity));
            break;
        case wxBRUSHSTYLE_TRANSPARENT:
            s += wxS("fill-opacity:0.0; ");
            break;
        default:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Brush Style not available"));
    }

    return s;
}

static
wxString wxGetPenPattern(wxPen& pen)
{
    wxString s;

    // The length of the dashes and gaps have a constant factor.
    // Dots have a width of 2, short dashes 10, long dashes 15 and gaps 8 (5 for dots).
    // When the pen width increases, lines become thicker and unrecognizable.
    // Multiplying with 1/3th of the width creates line styles matching the appearance of wxDC.
    // The pen width is not used to modify user provided dash styles.
    float w = pen.GetWidth();
    if (pen.GetWidth() == 0)
        w = 1;
    w = w / 3;

    switch (pen.GetStyle())
    {
        case wxPENSTYLE_DOT:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f\" "), w * 2, w * 5);
            break;
        case wxPENSTYLE_SHORT_DASH:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f\" "), w * 10, w * 8);
            break;
        case wxPENSTYLE_LONG_DASH:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f\" "), w * 15, w * 8);
            break;
        case wxPENSTYLE_DOT_DASH:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f,%f,%f\" "), w * 8, w * 8, w * 2, w * 8);
            break;
        case wxPENSTYLE_USER_DASH:
        {
            s = wxS("stroke-dasharray=\"");
            wxDash *dashes;
            int count = pen.GetDashes(&dashes);
            if ((dashes != NULL) && (count > 0))
            {
                for (int i = 0; i < count; ++i)
                {
                    s << dashes[i];
                    if (i < count - 1)
                        s << ",";
                }
            }
            s += wxS("\" ");
            break;
        }
        case wxPENSTYLE_STIPPLE_MASK_OPAQUE:
        case wxPENSTYLE_STIPPLE_MASK:
        case wxPENSTYLE_STIPPLE:
        case wxPENSTYLE_BDIAGONAL_HATCH:
        case wxPENSTYLE_CROSSDIAG_HATCH:
        case wxPENSTYLE_FDIAGONAL_HATCH:
        case wxPENSTYLE_CROSS_HATCH:
        case wxPENSTYLE_HORIZONTAL_HATCH:
        case wxPENSTYLE_VERTICAL_HATCH:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Pen Pattern not available"));
            break;
        case wxPENSTYLE_SOLID:
        case wxPENSTYLE_TRANSPARENT:
        case wxPENSTYLE_INVALID:
            // these penstyles do not need a pattern.
            break;
    }
    return s;
}

wxString wxGetBrushStyleName(wxBrush& brush)
{
    wxString brushStyle;

    switch (brush.GetStyle())
    {
        case wxBRUSHSTYLE_FDIAGONAL_HATCH:
            brushStyle = wxS("FdiagonalHatch");
            break;
        case wxBRUSHSTYLE_CROSSDIAG_HATCH:
            brushStyle = wxS("CrossDiagHatch");
            break;
        case wxBRUSHSTYLE_CROSS_HATCH:
            brushStyle = wxS("CrossHatch");
            break;
        case wxBRUSHSTYLE_VERTICAL_HATCH:
            brushStyle = wxS("VerticalHatch");
            break;
        case wxBRUSHSTYLE_HORIZONTAL_HATCH:
            brushStyle = wxS("HorizontalHatch");
            break;
        case wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE:
        case wxBRUSHSTYLE_STIPPLE_MASK:
        case wxBRUSHSTYLE_STIPPLE:
        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Brush Fill not available"));
            break;
        case wxBRUSHSTYLE_SOLID:
        case wxBRUSHSTYLE_TRANSPARENT:
        case wxBRUSHSTYLE_INVALID:
            // these brushstyles do not need a fill.
            break;
    }

    return brushStyle;
}

wxString wxGetBrushFill(wxBrush& brush)
{
    wxString s;
    wxString brushStyle = wxGetBrushStyleName(brush);

    if (!brushStyle.IsEmpty())
        s = wxS(" fill=\"url(#") + brushStyle + brush.GetColour().GetAsString(wxC2S_HTML_SYNTAX).substr(1) + wxS(")\"");

    return s;
}

wxString wxCreateBrushFill(wxBrush& brush)
{
    wxString s;
    wxString patternName = wxGetBrushStyleName(brush);

    if (!patternName.IsEmpty())
    {
        patternName += brush.GetColour().GetAsString(wxC2S_HTML_SYNTAX).substr(1);
        s = wxS("<pattern id=\"") + patternName + wxS("\" patternUnits=\"userSpaceOnUse\" width=\"8\" height=\"8\">\n");
        s += wxS("  <path style=\"stroke:") + brush.GetColour().GetAsString(wxC2S_HTML_SYNTAX) + wxS(";\" ");

        switch (brush.GetStyle())
        {
            case wxBRUSHSTYLE_FDIAGONAL_HATCH:
                s += wxS("d=\"M7,-1 l2,2 M0,0 l8,8 M-1,7 l2,2\"");
                break;
            case wxBRUSHSTYLE_CROSSDIAG_HATCH:
                s += wxS("d=\"M7,-1 l2,2 M0,0 l8,8 M-1,7 l2,2 M-1,1 l2,-2 M0,8 l8,-8 M7,9 l2,-2\"");
                break;
            case wxBRUSHSTYLE_CROSS_HATCH:
                s += wxS("d=\"M4,0 l0,8 M0,4 l8,0\"");
                break;
            case wxBRUSHSTYLE_VERTICAL_HATCH:
                s += wxS("d=\"M4,0 l0,8\"");
                break;
            case wxBRUSHSTYLE_HORIZONTAL_HATCH:
                s += wxS("d=\"M0,4 l8,0\"");
                break;
            default:
                break;
        }

        s += wxS("/>\n</pattern>\n");
    }

    return s;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxSVGBitmapEmbedHandler
// ----------------------------------------------------------------------------

bool
wxSVGBitmapEmbedHandler::ProcessBitmap(const wxBitmap& bmp,
                                       wxCoord x, wxCoord y,
                                       wxOutputStream& stream) const
{
    static int sub_images = 0;

    if ( wxImage::FindHandler(wxBITMAP_TYPE_PNG) == NULL )
        wxImage::AddHandler(new wxPNGHandler);

    // write the bitmap as a PNG to a memory stream and Base64 encode
    wxMemoryOutputStream mem;
    bmp.ConvertToImage().SaveFile(mem, wxBITMAP_TYPE_PNG);
    wxString data = wxBase64Encode(mem.GetOutputStreamBuffer()->GetBufferStart(),
                                   mem.GetSize());

    // write image meta information
    wxString s;
    s += wxString::Format("  <image x=\"%d\" y=\"%d\" width=\"%dpx\" height=\"%dpx\"",
                          x, y, bmp.GetWidth(), bmp.GetHeight());
    s += wxString::Format(" id=\"image%d\" "
                          "xlink:href=\"data:image/png;base64,\n",
                          sub_images++);

    // Wrap Base64 encoded data on 76 columns boundary (same as Inkscape).
    const unsigned WRAP = 76;
    for ( size_t i = 0; i < data.size(); i += WRAP )
    {
        if (i < data.size() - WRAP)
            s += data.Mid(i, WRAP) + "\n";
        else
            s += data.Mid(i, s.size() - i) + "\"\n  />\n"; // last line
    }

    // write to the SVG file
    const wxCharBuffer buf = s.utf8_str();
    stream.Write(buf, strlen((const char *)buf));

    return stream.IsOk();
}

// ----------------------------------------------------------
// wxSVGBitmapFileHandler
// ----------------------------------------------------------

bool
wxSVGBitmapFileHandler::ProcessBitmap(const wxBitmap& bmp,
                                      wxCoord x, wxCoord y,
                                      wxOutputStream& stream) const
{
    static int sub_images = 0;

    if ( wxImage::FindHandler(wxBITMAP_TYPE_PNG) == NULL )
        wxImage::AddHandler(new wxPNGHandler);

    // find a suitable file name
    wxFileName sPNG = m_path;
    do
    {
        sPNG.SetFullName(wxString::Format("%s%simage%d.png",
                         sPNG.GetName(),
                         sPNG.GetName().IsEmpty() ? "" : "_",
                         sub_images++));
    }
    while ( sPNG.FileExists() );

    if ( !bmp.SaveFile(sPNG.GetFullPath(), wxBITMAP_TYPE_PNG) )
        return false;

    // reference the bitmap from the SVG doc
    wxString s;
    s += wxString::Format("  <image x=\"%d\" y=\"%d\" width=\"%dpx\" height=\"%dpx\"",
                          x, y, bmp.GetWidth(), bmp.GetHeight());
    s += wxString::Format(" xlink:href=\"%s\"/>\n", sPNG.GetFullName());

    // write to the SVG file
    const wxCharBuffer buf = s.utf8_str();
    stream.Write(buf, strlen((const char*)buf));

    return stream.IsOk();
}

// ----------------------------------------------------------
// wxSVGFileDC (specialisations)
// ----------------------------------------------------------

void wxSVGFileDC::SetBitmapHandler(wxSVGBitmapHandler* handler)
{
    ((wxSVGFileDCImpl*)GetImpl())->SetBitmapHandler(handler);
}

// ----------------------------------------------------------
// wxSVGFileDCImpl
// ----------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxSVGFileDCImpl, wxDC);

wxSVGFileDCImpl::wxSVGFileDCImpl(wxSVGFileDC *owner, const wxString &filename,
                                 int width, int height, double dpi, const wxString &title)
    : wxDCImpl(owner)
{
    Init(filename, width, height, dpi, title);
}

void wxSVGFileDCImpl::Init(const wxString &filename, int Width, int Height,
                           double dpi, const wxString &title)
{
    m_width = Width;
    m_height = Height;

    m_dpi = dpi;

    m_OK = true;

    m_clipUniqueId = 0;
    m_clipNestingLevel = 0;

    m_mm_to_pix_x = dpi / 25.4;
    m_mm_to_pix_y = dpi / 25.4;

    m_backgroundBrush = *wxTRANSPARENT_BRUSH;
    m_textForegroundColour = *wxBLACK;
    m_textBackgroundColour = *wxWHITE;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_filename = filename;
    m_graphics_changed = true;

    ////////////////////code here

    m_bmp_handler.reset();
    m_outfile.reset(new wxFileOutputStream(m_filename));

    wxString s;
    s += wxS("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    s += wxS("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n\n");
    s += wxS("<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");
    s += wxString::Format(wxS(" width=\"%scm\" height=\"%scm\" viewBox=\"0 0 %d %d\">\n"), NumStr(float(Width) / dpi * 2.54), NumStr(float(Height) / dpi * 2.54), Width, Height);
    s += wxString::Format(wxS("<title>%s</title>\n"), title);
    s += wxString(wxS("<desc>Picture generated by wxSVG ")) + wxSVGVersion + wxS("</desc>\n\n");
    s += wxS("<g style=\"fill:black; stroke:black; stroke-width:1\">\n");
    write(s);
}

wxSVGFileDCImpl::~wxSVGFileDCImpl()
{
    wxString s;

    // Close remaining clipping group elements
    for (size_t i = 0; i < m_clipUniqueId; i++)
        s += wxS("</g>\n");

    s += wxS("</g>\n</svg>\n");
    write(s);
}

void wxSVGFileDCImpl::DoGetSizeMM(int *width, int *height) const
{
    if (width)
        *width = wxRound( (double)m_width / GetMMToPXx() );

    if (height)
        *height = wxRound( (double)m_height / GetMMToPXy() );
}

wxSize wxSVGFileDCImpl::GetPPI() const
{
    return wxSize(wxRound(m_dpi), wxRound(m_dpi));
}

void wxSVGFileDCImpl::Clear()
{
    {
        wxDCBrushChanger setBackground(*GetOwner(), m_backgroundBrush);
        wxDCPenChanger setTransp(*GetOwner(), *wxTRANSPARENT_PEN);
        DoDrawRectangle(0, 0, m_width, m_height);
    }

    NewGraphicsIfNeeded();
}

void wxSVGFileDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    NewGraphicsIfNeeded();

    wxString s;
    s = wxString::Format(wxS("  <path %sd=\"M%d %d L%d %d\"/>\n"), wxGetPenPattern(m_pen), x1, y1, x2, y2);

    write(s);

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxSVGFileDCImpl::DoDrawLines(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    if (n > 1)
    {
        NewGraphicsIfNeeded();
        wxString s;

        s = wxS("  <path ") + wxGetPenPattern(m_pen);
        s += wxString::Format(wxS("d=\"M%d %d"), (points[0].x + xoffset), (points[0].y + yoffset));
        CalcBoundingBox(points[0].x + xoffset, points[0].y + yoffset);

        for (int i = 1; i < n; ++i)
        {
            s += wxString::Format(wxS(" L%d %d"), (points[i].x + xoffset), (points[i].y + yoffset));
            CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
        }

        s += wxS("\" style=\"fill:none\"/>\n");

        write(s);
    }
}

void wxSVGFileDCImpl::DoDrawPoint(wxCoord x1, wxCoord y1)
{
    wxString s;
    NewGraphicsIfNeeded();
    s = wxS("<g style=\"stroke-linecap:round;\">\n");
    write(s);
    DoDrawLine(x1, y1, x1, y1);
    s = wxS("</g>\n");
    write(s);
}

void wxSVGFileDCImpl::DoDrawCheckMark(wxCoord x1, wxCoord y1, wxCoord width, wxCoord height)
{
    wxDCImpl::DoDrawCheckMark(x1, y1, width, height);
}

void wxSVGFileDCImpl::DoDrawText(const wxString& text, wxCoord x1, wxCoord y1)
{
    DoDrawRotatedText(text, x1, y1, 0.0);
}

void wxSVGFileDCImpl::DoDrawRotatedText(const wxString& sText, wxCoord x, wxCoord y, double angle)
{
    //known bug; if the font is drawn in a scaled DC, it will not behave exactly as wxMSW
    NewGraphicsIfNeeded();
    wxString s;

    // Get extent of whole text.
    wxCoord w, h, heightLine;
    GetOwner()->GetMultiLineTextExtent(sText, &w, &h, &heightLine);

    // Compute the shift for the origin of the next line.
    const double rad = wxDegToRad(angle);
    const double dx = heightLine * sin(rad);
    const double dy = heightLine * cos(rad);

    // wxS("upper left") and wxS("upper right")
    CalcBoundingBox(x, y);
    CalcBoundingBox((wxCoord)(x + w * cos(rad)), (wxCoord)(y - h * sin(rad)));

    // wxS("bottom left") and wxS("bottom right")
    CalcBoundingBox((wxCoord)(x + h * sin(rad)), (wxCoord)(y + h * cos(rad)));
    CalcBoundingBox((wxCoord)(x + h * sin(rad) + w * cos(rad)), (wxCoord)(y + h * cos(rad) - w * sin(rad)));

    if (m_backgroundMode == wxBRUSHSTYLE_SOLID)
    {
        // draw background first
        // just like DoDrawRectangle except we pass the text color to it and set the border to a 1 pixel wide text background
        s += wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "), x, y, w, h);
        s += wxS("style=\"") + wxBrushString(m_textBackgroundColour);
        s += wxS("stroke-width:1; ") + wxPenString(m_textBackgroundColour);
        s += wxString::Format(wxS("\" transform=\"rotate(%s %d %d)\"/>"), NumStr(-angle), x, y);
        s += wxS("\n");
        write(s);
    }

    // Draw all text line by line
    const wxArrayString lines = wxSplit(sText, '\n', '\0');
    for (size_t lineNum = 0; lineNum < lines.size(); lineNum++)
    {
        // convert x,y to SVG text x,y (the coordinates of the text baseline)
        wxCoord ww, hh, desc;
        DoGetTextExtent(lines[lineNum], &ww, &hh, &desc);
        int xx = x + wxRound(lineNum * dx) + (hh - desc) * sin(rad);
        int yy = y + wxRound(lineNum * dy) + (hh - desc) * cos(rad);

        //now do the text itself
        s += wxString::Format(wxS("  <text x=\"%d\" y=\"%d\" textLength=\"%d\" "), xx, yy, ww);

        wxString fontName(m_font.GetFaceName());
        if (fontName.Len() > 0)
            s += wxS("style=\"font-family:") + fontName + wxS("; ");
        else
            s += wxS("style=\" ");

        wxString fontweight = wxString::Format(wxS("%d"), m_font.GetWeight());

        s += wxS("font-weight:") + fontweight + wxS("; ");

        wxString fontstyle;
        switch (m_font.GetStyle())
        {
            case wxFONTSTYLE_MAX:
                wxFAIL_MSG(wxS("invalid font style value"));
                wxFALLTHROUGH;

            case wxFONTSTYLE_NORMAL:
                fontstyle = wxS("normal");
                break;

            case wxFONTSTYLE_ITALIC:
                fontstyle = wxS("italic");
                break;

            case wxFONTSTYLE_SLANT:
                fontstyle = wxS("oblique");
                break;
        }

        wxASSERT_MSG(!fontstyle.empty(), wxS("unknown font style value"));

        s += wxS("font-style:") + fontstyle + wxS("; ");

        wxString textDecoration;
        if (m_font.GetUnderlined())
            textDecoration += wxS(" underline");
        if (m_font.GetStrikethrough())
            textDecoration += wxS(" line-through");
        if (textDecoration.IsEmpty())
            textDecoration = wxS(" none");

        s += wxS("text-decoration:") + textDecoration + wxS("; ");

        s += wxString::Format(wxS("font-size:%dpt; "), m_font.GetPointSize());
        //text will be solid, unless alpha value isn't opaque in the foreground colour
        s += wxBrushString(m_textForegroundColour) + wxPenString(m_textForegroundColour);
        s += wxString::Format(wxS("stroke-width:0;\" transform=\"rotate(%s %d %d)\""), NumStr(-angle), xx, yy);
        s += wxS(" xml:space=\"preserve\">");
#if wxUSE_MARKUP
        s += wxMarkupParser::Quote(lines[lineNum]) + wxS("</text>\n");
#else
        s += lines[lineNum] + wxS("</text>\n");
#endif

        write(s);
    }
}

void wxSVGFileDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    DoDrawRoundedRectangle(x, y, width, height, 0);
}

void wxSVGFileDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    NewGraphicsIfNeeded();
    wxString s;

    s = wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" rx=\"%s\"%s"),
                         x, y, width, height, NumStr(radius), wxGetBrushFill(m_brush));

    s += wxS("/>\n");
    write(s);

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + width, y + height);
}

void wxSVGFileDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                                    wxCoord xoffset, wxCoord yoffset,
                                    wxPolygonFillMode fillStyle)
{
    NewGraphicsIfNeeded();
    wxString s;

    s = wxS("  <polygon style=\"");
    if (fillStyle == wxODDEVEN_RULE)
        s += wxS("fill-rule:evenodd;");
    else
        s += wxS("fill-rule:nonzero;");

    s += wxS("\"") + wxGetBrushFill(m_brush) + wxS(" points=\"");

    for (int i = 0; i < n; i++)
    {
        s += wxString::Format(wxS("%d %d "), points[i].x + xoffset, points[i].y + yoffset);
        CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }
    s += wxS("\"/>\n");
    write(s);
}

void wxSVGFileDCImpl::DoDrawPolyPolygon(int n, const int count[], const wxPoint points[],
                                        wxCoord xoffset, wxCoord yoffset,
                                        wxPolygonFillMode fillStyle)
{
    if (n == 1)
    {
        DoDrawPolygon(count[0], points, xoffset, yoffset, fillStyle);
        return;
    }

    int i, j;
    int totalPts = 0;
    for (j = 0; j < n; ++j)
        totalPts += count[j];

    wxScopedArray<wxPoint> pts(totalPts + n);

    int polyCounter = 0, polyIndex = 0;
    for (i = j = 0; i < totalPts; ++i)
    {
        pts[j++] = points[i];
        ++polyCounter;
        if (polyCounter == count[polyIndex])
        {
            pts[j++] = points[i - count[polyIndex] + 1];
            ++polyIndex;
            polyCounter = 0;
        }
    }

    {
        wxDCPenChanger setTransp(*GetOwner(), *wxTRANSPARENT_PEN);
        DoDrawPolygon(j, pts.get(), xoffset, yoffset, fillStyle);
    }

    for (i = j = 0; i < n; i++)
    {
        DoDrawLines(count[i] + 1, pts.get() + j, xoffset, yoffset);
        j += count[i] + 1;
    }
}

void wxSVGFileDCImpl::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    NewGraphicsIfNeeded();

    int rh = height / 2;
    int rw = width / 2;

    wxString s;
    s = wxString::Format(wxS("  <ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\""), x + rw, y + rh, rw, rh);
    s += wxS("/>\n");

    write(s);

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + width, y + height);
}

void wxSVGFileDCImpl::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
    /* Draws an arc of a circle, centred on (xc, yc), with starting point
    (x1, y1) and ending at (x2, y2). The current pen is used for the outline
    and the current brush for filling the shape.

    The arc is drawn in an anticlockwise direction from the start point to
    the end point.

    Might be better described as Pie drawing */

    NewGraphicsIfNeeded();
    wxString s;

    // we need the radius of the circle which has two estimates
    double r1 = sqrt( double( (x1 - xc)*(x1 - xc) ) + double( (y1 - yc)*(y1 - yc) ) );
    double r2 = sqrt( double( (x2 - xc)*(x2 - xc) ) + double( (y2 - yc)*(y2 - yc) ) );

    wxASSERT_MSG((fabs( r2 - r1 ) <= 3), wxS("wxSVGFileDC::DoDrawArc Error in getting radii of circle"));
    if ( fabs( r2 - r1 ) > 3 )    //pixels
    {
        s = wxS("<!--- wxSVGFileDC::DoDrawArc Error in getting radii of circle -->\n");
        write(s);
    }

    double theta1 = atan2((double)(yc - y1), (double)(x1 - xc));
    if (theta1 < 0)
        theta1 = theta1 + M_PI * 2;

    double theta2 = atan2((double)(yc - y2), (double)(x2 - xc));
    if (theta2 < 0)
        theta2 = theta2 + M_PI * 2;
    if (theta2 < theta1) theta2 = theta2 + M_PI * 2;

    int fArc;                  // flag for large or small arc 0 means less than 180 degrees
    if (fabs(theta2 - theta1) > M_PI)
        fArc = 1; else fArc = 0;

    int fSweep = 0;             // flag for sweep always 0

    if (x1 == x2 && y1 == y2)
    {
        // drawing full circle fails with default arc. Draw two half arcs instead.
        s = wxString::Format(wxS("  <path d=\"M%d %d a%s %s 0 %d %d %s %s a%s %s 0 %d %d %s %s"),
            x1, y1,
            NumStr(r1), NumStr(r2), fArc, fSweep, NumStr( r1 * 2), NumStr(0),
            NumStr(r1), NumStr(r2), fArc, fSweep, NumStr(-r1 * 2), NumStr(0));
    }
    else
    {
        // comply to wxDC specs by drawing closing line if brush is not transparent
        wxString line;
        if (GetBrush().GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
            line = wxString::Format(wxS("L%d %d z"), xc, yc);

        s = wxString::Format(wxS("  <path d=\"M%d %d A%s %s 0 %d %d %d %d %s"),
            x1, y1, NumStr(r1), NumStr(r2), fArc, fSweep, x2, y2, line);
    }

    s += wxS("\"/>\n");

    write(s);
}

void wxSVGFileDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{
    /*
    Draws an arc of an ellipse. The current pen is used for drawing the arc
    and the current brush is used for drawing the pie.

    x and y specify the x and y coordinates of the upper-left corner of the
    rectangle that contains the ellipse.

    width and height specify the width and height of the rectangle that
    contains the ellipse.

    start and end specify the start and end of the arc relative to the
    three-o'clock position from the center of the rectangle. Angles are
    specified in degrees (360 is a complete circle). Positive values mean
    counter-clockwise motion. If start is equal to end, a complete ellipse
    will be drawn. */

    //radius
    double rx = w / 2.0;
    double ry = h / 2.0;
    // center
    double xc = x + rx;
    double yc = y + ry;

    // start and end coords
    double xs, ys, xe, ye;
    xs = xc + rx * cos(wxDegToRad(sa));
    xe = xc + rx * cos(wxDegToRad(ea));
    ys = yc - ry * sin(wxDegToRad(sa));
    ye = yc - ry * sin(wxDegToRad(ea));

    // svg arcs have 0 degrees at 12-o'clock instead of 3-o'clock
    double start = (sa - 90);
    if (start < 0)
        start += 360;
    while (fabs(start) > 360)
        start -= (start / fabs(start)) * 360;

    double end = (ea - 90);
    if (end < 0)
        end += 360;
    while (fabs(end) > 360)
        end -= (end / fabs(end)) * 360;

    // svg arcs are in clockwise direction, reverse angle
    double angle = end - start;
    if (angle <= 0)
        angle += 360;

    int fArc = angle > 180 ? 1 : 0; // flag for large or small arc
    int fSweep = 0;                 // flag for sweep always 0

    wxString arcPath;
    if (angle == 360)
    {
        // Drawing full circle fails with default arc. Draw two half arcs instead.
        fArc = 1;
        arcPath = wxString::Format(wxS("  <path d=\"M%s %s a%s %s 0 %d %d %s %s a%s %s 0 %d %d %s %s"),
            NumStr(x), NumStr(y + ry),
            NumStr(rx), NumStr(ry), fArc, fSweep, NumStr( rx * 2), NumStr(0),
            NumStr(rx), NumStr(ry), fArc, fSweep, NumStr(-rx * 2), NumStr(0));
    }
    else
    {
        arcPath = wxString::Format(wxS("  <path d=\"M%s %s A%s %s 0 %d %d %s %s"),
            NumStr(xs), NumStr(ys),
            NumStr(rx), NumStr(ry), fArc, fSweep, NumStr(xe), NumStr(ye));
    }

    // Workaround so SVG does not draw an extra line from the centre of the drawn arc
    // to the start point of the arc.
    // First draw the arc with the current brush, without a border,
    // then draw the border without filling the arc.
    if (GetBrush().GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
    {
        wxDCPenChanger setTransp(*GetOwner(), *wxTRANSPARENT_PEN);
        NewGraphicsIfNeeded();

        wxString arcFill = arcPath;
        arcFill += wxString::Format(wxS(" L%s %s z"), NumStr(xc), NumStr(yc));
        arcFill += wxS("\"/>\n");
        write(arcFill);
    }

    wxDCBrushChanger setTransp(*GetOwner(), *wxTRANSPARENT_BRUSH);
    NewGraphicsIfNeeded();

    wxString arcLine = arcPath + wxS("\"/>\n");
    write(arcLine);
}

void wxSVGFileDCImpl::DoSetClippingRegion(int x, int y, int width, int height)
{
    wxString svg;

    // End current graphics group to ensure proper xml nesting (e.g. so that
    // graphics can be subsequently changed inside the clipping region)
    svg << "</g>\n"
           "<defs>\n"
           "  <clipPath id=\"clip" << m_clipNestingLevel << "\">\n"
           "    <rect id=\"cliprect" << m_clipNestingLevel << "\" "
                "x=\"" << x << "\" "
                "y=\"" << y << "\" "
                "width=\"" << width << "\" "
                "height=\"" << height << "\" "
                "style=\"stroke: gray; fill: none;\"/>\n"
           "  </clipPath>\n"
           "</defs>\n"
           "<g style=\"clip-path: url(#clip" << m_clipNestingLevel << ");\">\n";

    write(svg);

    // Re-apply current graphics to ensure proper xml nesting
    DoStartNewGraphics();

    m_clipUniqueId++;
    m_clipNestingLevel++;

    // Update the base class m_clip[XY][12] fields too.
    wxDCImpl::DoSetClippingRegion(x, y, width, height);
}

void wxSVGFileDCImpl::DestroyClippingRegion()
{
    wxString svg;

    // End current graphics element to ensure proper xml nesting (e.g. graphics
    // might have been changed inside the clipping region)
    svg << "</g>\n";

    // Close clipping group elements
    for (size_t i = 0; i < m_clipUniqueId; i++)
    {
        svg << "</g>\n";
    }

    write(svg);

    // Re-apply current graphics (e.g. brush may have been changed inside one
    // of the clipped regions - that change will have been lost after xml
    // elements for the clipped region have been closed).
    DoStartNewGraphics();

    m_clipUniqueId = 0;

    // Also update the base class clipping region information.
    wxDCImpl::DestroyClippingRegion();
}

void wxSVGFileDCImpl::DoGetTextExtent(const wxString& string, wxCoord *w, wxCoord *h, wxCoord *descent, wxCoord *externalLeading, const wxFont *font) const
{
    wxScreenDC sDC;

    sDC.SetFont(m_font);
    if (font != NULL)
        sDC.SetFont(*font);
    sDC.GetTextExtent(string, w, h, descent, externalLeading);
}

wxCoord wxSVGFileDCImpl::GetCharHeight() const
{
    wxScreenDC sDC;
    sDC.SetFont(m_font);

    return sDC.GetCharHeight();

}

wxCoord wxSVGFileDCImpl::GetCharWidth() const
{
    wxScreenDC sDC;
    sDC.SetFont(m_font);

    return sDC.GetCharWidth();
}


// ----------------------------------------------------------
// wxSVGFileDCImpl - set functions
// ----------------------------------------------------------

void wxSVGFileDCImpl::SetBackground(const wxBrush &brush)
{
    m_backgroundBrush = brush;
}

void wxSVGFileDCImpl::SetBackgroundMode(int mode)
{
    m_backgroundMode = mode;
}

void wxSVGFileDCImpl::SetBitmapHandler(wxSVGBitmapHandler* handler)
{
    m_bmp_handler.reset(handler);
}

void wxSVGFileDCImpl::SetBrush(const wxBrush& brush)
{
    m_brush = brush;

    m_graphics_changed = true;

    wxString pattern = wxCreateBrushFill(m_brush);
    write(pattern);
}

void wxSVGFileDCImpl::SetPen(const wxPen& pen)
{
    m_pen = pen;

    m_graphics_changed = true;
}

void wxSVGFileDCImpl::NewGraphicsIfNeeded()
{
    if ( !m_graphics_changed )
        return;

    m_graphics_changed = false;

    write(wxS("</g>\n"));

    DoStartNewGraphics();
}

void wxSVGFileDCImpl::DoStartNewGraphics()
{
    wxString s, sBrush, sPenCap, sPenJoin, sPenStyle, sLast;

    sBrush = wxS("<g style=\"") + wxBrushString(m_brush.GetColour(), m_brush.GetStyle())
           + wxPenString(m_pen.GetColour(), m_pen.GetStyle());

    switch ( m_pen.GetCap() )
    {
        case wxCAP_PROJECTING:
            sPenCap = wxS("stroke-linecap:square; ");
            break;
        case wxCAP_BUTT:
            sPenCap = wxS("stroke-linecap:butt; ");
            break;
        case wxCAP_ROUND:
        default:
            sPenCap = wxS("stroke-linecap:round; ");
    }

    switch (m_pen.GetJoin())
    {
        case wxJOIN_BEVEL:
            sPenJoin = wxS("stroke-linejoin:bevel; ");
            break;
        case wxJOIN_MITER:
            sPenJoin = wxS("stroke-linejoin:miter; ");
            break;
        case wxJOIN_ROUND:
        default:
            sPenJoin = wxS("stroke-linejoin:round; ");
    }

    sLast = wxString::Format(wxS("stroke-width:%d\" transform=\"translate(%s %s) scale(%s %s)\">"),
                             m_pen.GetWidth(),
                             NumStr((m_deviceOriginX - m_logicalOriginX)* m_signX),
                             NumStr((m_deviceOriginY - m_logicalOriginY)* m_signY),
                             NumStr(m_scaleX * m_signX),
                             NumStr(m_scaleY * m_signY));

    s = sBrush + sPenCap + sPenJoin + sPenStyle + sLast + wxS("\n");
    write(s);
}

void wxSVGFileDCImpl::SetFont(const wxFont& font)
{
    m_font = font;
}

// export a bitmap as a raster image in png
bool wxSVGFileDCImpl::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                             wxDC* source, wxCoord xsrc, wxCoord ysrc,
                             wxRasterOperationMode logicalFunc /*= wxCOPY*/, bool useMask /*= false*/,
                             wxCoord /*xsrcMask = -1*/, wxCoord /*ysrcMask = -1*/)
{
    if (logicalFunc != wxCOPY)
    {
        wxASSERT_MSG(false, wxS("wxSVGFileDC::DoBlit Call requested nonCopy mode; this is not possible"));
        return false;
    }
    if (useMask != false)
    {
        wxASSERT_MSG(false, wxS("wxSVGFileDC::DoBlit Call requested false mask; this is not possible"));
        return false;
    }
    wxBitmap myBitmap(width, height);
    wxMemoryDC memDC;
    memDC.SelectObject(myBitmap);
    memDC.Blit(0, 0, width, height, source, xsrc, ysrc);
    memDC.SelectObject(wxNullBitmap);
    DoDrawBitmap(myBitmap, xdest, ydest);
    return false;
}

void wxSVGFileDCImpl::DoDrawIcon(const class wxIcon & myIcon, wxCoord x, wxCoord y)
{
    wxBitmap myBitmap(myIcon.GetWidth(), myIcon.GetHeight());
    wxMemoryDC memDC;
    memDC.SelectObject(myBitmap);
    memDC.DrawIcon(myIcon, 0, 0);
    memDC.SelectObject(wxNullBitmap);
    DoDrawBitmap(myBitmap, x, y);
}

void wxSVGFileDCImpl::DoDrawBitmap(const class wxBitmap & bmp, wxCoord x, wxCoord y, bool WXUNUSED(bTransparent) /*=0*/)
{
    NewGraphicsIfNeeded();

    // If we don't have any bitmap handler yet, use the default one.
    if ( !m_bmp_handler )
        m_bmp_handler.reset(new wxSVGBitmapFileHandler(m_filename));

    m_bmp_handler->ProcessBitmap(bmp, x, y, *m_outfile);
}

void wxSVGFileDCImpl::write(const wxString &s)
{
    m_OK = m_outfile->IsOk();
    if (!m_OK)
        return;
    const wxCharBuffer buf = s.utf8_str();
    m_outfile->Write(buf, strlen((const char *)buf));
    m_OK = m_outfile->IsOk();
}

#endif // wxUSE_SVG
