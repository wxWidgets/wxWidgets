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
    // Handle this case specially to avoid generating "-0.00" in the output.
    if ( f == 0 )
    {
        return wxS("0.00");
    }

    return wxString::FromCDouble(f, 2);
}

// Return the colour representation as HTML-like "#rrggbb" string and also
// returns its alpha as opacity number in 0..1 range.
wxString Col2SVG(wxColour c, float* opacity = NULL)
{
    if ( c.Alpha() != wxALPHA_OPAQUE )
    {
        if ( opacity )
            *opacity = c.Alpha() / 255.0f;

        // Remove the alpha before using GetAsString(wxC2S_HTML_SYNTAX) as it
        // doesn't support colours with alpha channel.
        c = wxColour(c.GetRGB());
    }
    else // No alpha.
    {
        if ( opacity )
            *opacity = 1.0f;
    }

    return c.GetAsString(wxC2S_HTML_SYNTAX);
}

wxString GetPenStroke(const wxColour& c, int style = wxPENSTYLE_SOLID)
{
    float opacity;
    wxString s = wxS("stroke:") + Col2SVG(c, &opacity) + wxS(";");

    switch ( style )
    {
        case wxPENSTYLE_SOLID:
        case wxPENSTYLE_DOT:
        case wxPENSTYLE_SHORT_DASH:
        case wxPENSTYLE_LONG_DASH:
        case wxPENSTYLE_DOT_DASH:
        case wxPENSTYLE_USER_DASH:
            s += wxString::Format(wxS(" stroke-opacity:%s;"), NumStr(opacity));
            break;
        case wxPENSTYLE_TRANSPARENT:
            s += wxS(" stroke-opacity:0.0;");
            break;
        default:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Pen Style not available"));
            break;
    }

    return s;
}

wxString GetBrushFill(const wxColour& c, int style = wxBRUSHSTYLE_SOLID)
{
    float opacity;
    wxString s = wxS("fill:") + Col2SVG(c, &opacity) + wxS(";");

    switch ( style )
    {
        case wxBRUSHSTYLE_SOLID:
        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
        case wxBRUSHSTYLE_FDIAGONAL_HATCH:
        case wxBRUSHSTYLE_CROSSDIAG_HATCH:
        case wxBRUSHSTYLE_CROSS_HATCH:
        case wxBRUSHSTYLE_VERTICAL_HATCH:
        case wxBRUSHSTYLE_HORIZONTAL_HATCH:
            s += wxString::Format(wxS(" fill-opacity:%s;"), NumStr(opacity));
            break;
        case wxBRUSHSTYLE_TRANSPARENT:
            s += wxS(" fill-opacity:0.0;");
            break;
        default:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Brush Style not available"));
            break;
    }

    return s;
}

wxString GetPenPattern(const wxPen& pen)
{
    wxString s;

    // The length of the dashes and gaps have a constant factor.
    // Dots have a width of 2, short dashes 10, long dashes 15 and gaps 8 (5 for dots).
    // When the pen width increases, lines become thicker and unrecognizable.
    // Multiplying with 1/3th of the width creates line styles matching the appearance of wxDC.
    // The pen width is not used to modify user provided dash styles.
    double w = pen.GetWidth();
    if (pen.GetWidth() == 0)
        w = 1;
    w = w / 3;

    switch (pen.GetStyle())
    {
        case wxPENSTYLE_DOT:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f\""), w * 2, w * 5);
            break;
        case wxPENSTYLE_SHORT_DASH:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f\""), w * 10, w * 8);
            break;
        case wxPENSTYLE_LONG_DASH:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f\""), w * 15, w * 8);
            break;
        case wxPENSTYLE_DOT_DASH:
            s = wxString::Format(wxS("stroke-dasharray=\"%f,%f,%f,%f\""), w * 8, w * 8, w * 2, w * 8);
            break;
        case wxPENSTYLE_USER_DASH:
        {
            s = wxS("stroke-dasharray=\"");
            wxDash* dashes;
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
            s += wxS("\"");
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

wxString GetPenStyle(const wxPen& pen)
{
    wxString penStyle;

    penStyle += wxString::Format(wxS("stroke-width:%d;"), pen.GetWidth());

    switch (pen.GetCap())
    {
        case wxCAP_PROJECTING:
            penStyle += wxS(" stroke-linecap:square;");
            break;
        case wxCAP_BUTT:
            penStyle += wxS(" stroke-linecap:butt;");
            break;
        case wxCAP_ROUND:
        default:
            penStyle += wxS(" stroke-linecap:round;");
            break;
    }

    switch (pen.GetJoin())
    {
        case wxJOIN_BEVEL:
            penStyle += wxS(" stroke-linejoin:bevel;");
            break;
        case wxJOIN_MITER:
            penStyle += wxS(" stroke-linejoin:miter;");
            break;
        case wxJOIN_ROUND:
        default:
            penStyle += wxS(" stroke-linejoin:round;");
            break;
    }

    return penStyle;
}

wxString GetBrushStyleName(const wxBrush& brush)
{
    wxString brushStyle;

    switch (brush.GetStyle())
    {
        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
            brushStyle = wxS("BdiagonalHatch");
            break;
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
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Brush Fill not available"));
            break;
        case wxBRUSHSTYLE_SOLID:
        case wxBRUSHSTYLE_TRANSPARENT:
        case wxBRUSHSTYLE_INVALID:
            // these brushstyles do not need a fill.
            break;
    }

    if (!brushStyle.empty())
        brushStyle += wxString::Format(wxS("%s%02X"), Col2SVG(brush.GetColour()).substr(1), brush.GetColour().Alpha());

    return brushStyle;
}

wxString GetBrushPattern(const wxBrush& brush)
{
    wxString s;
    wxString brushStyle = GetBrushStyleName(brush);

    if (!brushStyle.empty())
        s = wxString::Format(wxS("fill=\"url(#%s)\""), brushStyle);

    return s;
}

wxString GetRenderMode(const wxSVGShapeRenderingMode style)
{
    wxString mode;
    switch (style)
    {
        case wxSVG_SHAPE_RENDERING_OPTIMIZE_SPEED:
            mode = wxS("optimizeSpeed");
            break;
        case wxSVG_SHAPE_RENDERING_CRISP_EDGES:
            mode = wxS("crispEdges");
            break;
        case wxSVG_SHAPE_RENDERING_GEOMETRIC_PRECISION:
            mode = wxS("geometricPrecision");
            break;
        case wxSVG_SHAPE_RENDERING_AUTO:
            mode = wxS("auto");
            break;
    }

    wxString s = wxString::Format(wxS("shape-rendering=\"%s\""), mode);
    return s;
}

wxString CreateBrushFill(const wxBrush& brush, wxSVGShapeRenderingMode mode)
{
    wxString s;
    wxString patternName = GetBrushStyleName(brush);

    if (!patternName.empty())
    {
        wxString pattern;
        switch (brush.GetStyle())
        {
            case wxBRUSHSTYLE_BDIAGONAL_HATCH:
                pattern = wxS("d=\"M-1,1 l2,-2 M0,8 l8,-8 M7,9 l2,-2\"");
                break;
            case wxBRUSHSTYLE_FDIAGONAL_HATCH:
                pattern = wxS("d=\"M7,-1 l2,2 M0,0 l8,8 M-1,7 l2,2\"");
                break;
            case wxBRUSHSTYLE_CROSSDIAG_HATCH:
                pattern = wxS("d=\"M7,-1 l2,2 M0,0 l8,8 M-1,7 l2,2 M-1,1 l2,-2 M0,8 l8,-8 M7,9 l2,-2\"");
                break;
            case wxBRUSHSTYLE_CROSS_HATCH:
                pattern = wxS("d=\"M4,0 l0,8 M0,4 l8,0\"");
                break;
            case wxBRUSHSTYLE_VERTICAL_HATCH:
                pattern = wxS("d=\"M4,0 l0,8\"");
                break;
            case wxBRUSHSTYLE_HORIZONTAL_HATCH:
                pattern = wxS("d=\"M0,4 l8,0\"");
                break;
            default:
                break;
        }

        float opacity;
        wxString brushColourStr = Col2SVG(brush.GetColour(), &opacity);
        wxString brushStrokeStr = wxS("stroke-width:1; stroke-linecap:round; stroke-linejoin:round;");

        s += wxString::Format(wxS("  <pattern id=\"%s\" patternUnits=\"userSpaceOnUse\" width=\"8\" height=\"8\">\n"),
            patternName);
        s += wxString::Format(wxS("    <path style=\"stroke:%s; stroke-opacity:%s; %s\" %s %s/>\n"),
            brushColourStr, NumStr(opacity), brushStrokeStr, pattern, GetRenderMode(mode));
        s += wxS("  </pattern>\n");
    }

    return s;
}

void SetScaledScreenDCFont(wxScreenDC& sDC, const wxFont& font)
{
    // When using DPI-independent pixels, the results of GetTextExtent() and
    // similar don't depend on DPI anyhow.
#ifndef wxHAVE_DPI_INDEPENDENT_PIXELS
    static const int SVG_DPI = 96;

    const double screenDPI = sDC.GetPPI().y;
    const double scale = screenDPI / SVG_DPI;
    if ( scale > 1 )
    {
        // wxScreenDC uses the DPI of the main screen to determine the text
        // extent and character width/height. Because the SVG should be
        // DPI-independent we want the text extent of the default (96) DPI.
        //
        // We can't just divide the returned sizes by the scale factor, because
        // text does not scale linear (at least on Windows). Therefore, we scale
        // the font size instead.
        wxFont scaledFont = font;
        scaledFont.SetFractionalPointSize(scaledFont.GetFractionalPointSize() / scale);
        sDC.SetFont(scaledFont);
    }
    else
#endif // !wxHAVE_DPI_INDEPENDENT_PIXELS
    {
        sDC.SetFont(font);
    }
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
    stream.Write(buf, strlen((const char*)buf));

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
                         sPNG.GetName().empty() ? "" : "_",
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

wxIMPLEMENT_ABSTRACT_CLASS(wxSVGFileDC, wxDC);

void wxSVGFileDC::SetBitmapHandler(wxSVGBitmapHandler* handler)
{
    ((wxSVGFileDCImpl*)GetImpl())->SetBitmapHandler(handler);
}

void wxSVGFileDC::SetShapeRenderingMode(wxSVGShapeRenderingMode renderingMode)
{
    ((wxSVGFileDCImpl*)GetImpl())->SetShapeRenderingMode(renderingMode);
}

// ----------------------------------------------------------
// wxSVGFileDCImpl
// ----------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxSVGFileDCImpl, wxDCImpl);

wxSVGFileDCImpl::wxSVGFileDCImpl(wxSVGFileDC* owner, const wxString& filename,
                                 int width, int height, double dpi, const wxString& title)
    : wxDCImpl(owner)
{
    Init(filename, width, height, dpi, title);
}

void wxSVGFileDCImpl::Init(const wxString& filename, int width, int height,
                           double dpi, const wxString& title)
{
    m_width = width;
    m_height = height;

    m_dpi = dpi;

    m_OK = true;

    m_clipUniqueId = 0;
    m_clipNestingLevel = 0;

    m_gradientUniqueId = 0;

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

    m_renderingMode = wxSVG_SHAPE_RENDERING_AUTO;

    ////////////////////code here

    m_bmp_handler.reset();

    if ( m_filename.empty() )
        m_outfile.reset();
    else
        m_outfile.reset(new wxFileOutputStream(m_filename));

    wxString s;
    s += wxS("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    s += wxS("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n\n");
    s += wxS("<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");
    s += wxString::Format(wxS(" width=\"%scm\" height=\"%scm\" viewBox=\"0 0 %d %d\">\n"), NumStr(float(m_width) / dpi * 2.54), NumStr(float(m_height) / dpi * 2.54), m_width, m_height);
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

void wxSVGFileDCImpl::DoGetSizeMM(int* width, int* height) const
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
    s = wxString::Format(wxS("  <path d=\"M%d %d L%d %d\" %s %s/>\n"),
        x1, y1, x2, y2, GetRenderMode(m_renderingMode), GetPenPattern(m_pen));

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

        s = wxString::Format(wxS("  <path d=\"M%d %d"),
            (points[0].x + xoffset), (points[0].y + yoffset));

        CalcBoundingBox(points[0].x + xoffset, points[0].y + yoffset);

        for (int i = 1; i < n; ++i)
        {
            s += wxString::Format(wxS(" L%d %d"), (points[i].x + xoffset), (points[i].y + yoffset));
            CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
        }

        s += wxString::Format(wxS("\" style=\"fill:none\" %s %s/>\n"),
            GetRenderMode(m_renderingMode), GetPenPattern(m_pen));

        write(s);
    }
}

void wxSVGFileDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    NewGraphicsIfNeeded();

    wxString s;

    s = wxS("  <g style=\"stroke-width:1; stroke-linecap:round;\">\n  ");
    write(s);

    DoDrawLine(x, y, x, y);

    s = wxS("  </g>\n");
    write(s);
}

void wxSVGFileDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    DoDrawRotatedText(text, x, y, 0.0);
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

    // Update bounding box: upper left, upper right, bottom left, bottom right
    CalcBoundingBox(x, y);
    CalcBoundingBox((wxCoord)(x + w * cos(rad)), (wxCoord)(y - h * sin(rad)));
    CalcBoundingBox((wxCoord)(x + h * sin(rad)), (wxCoord)(y + h * cos(rad)));
    CalcBoundingBox((wxCoord)(x + h * sin(rad) + w * cos(rad)), (wxCoord)(y + h * cos(rad) - w * sin(rad)));

    // Create text style string
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

    wxString textDecoration;
    if (m_font.GetUnderlined())
        textDecoration += wxS(" underline");
    if (m_font.GetStrikethrough())
        textDecoration += wxS(" line-through");
    if (textDecoration.empty())
        textDecoration = wxS(" none");

    wxString style = wxS("style=\"");
    style += wxString::Format(wxS("font-family:%s; "), m_font.GetFaceName());
    style += wxString::Format(wxS("font-weight:%d; "), m_font.GetWeight());
    style += wxString::Format(wxS("font-style:%s; "), fontstyle);
    style += wxString::Format(wxS("font-size:%spt; "), NumStr(m_font.GetFractionalPointSize()));
    style += wxString::Format(wxS("text-decoration:%s; "), textDecoration);
    style += wxString::Format(wxS("%s %s stroke-width:0; "),
                              GetBrushFill(m_textForegroundColour),
                              GetPenStroke(m_textForegroundColour));
    style += wxS("white-space: pre;");
    style += wxS("\"");

    // this is deprecated in favour of "white-space: pre", keep it for now to
    // support SVG viewers that do not support the new tag
    style += wxS(" xml:space=\"preserve\"");

    // Draw all text line by line
    const wxArrayString lines = wxSplit(sText, '\n', '\0');
    for (size_t lineNum = 0; lineNum < lines.size(); lineNum++)
    {
        const double xRect = x + lineNum * dx;
        const double yRect = y + lineNum * dy;

        // convert x,y to SVG text x,y (the coordinates of the text baseline)
        wxCoord ww, hh, desc;
        wxString const& line = lines[lineNum];
        DoGetTextExtent(line, &ww, &hh, &desc);
        const double xText = xRect + (hh - desc) * sin(rad);
        const double yText = yRect + (hh - desc) * cos(rad);

        if (m_backgroundMode == wxBRUSHSTYLE_SOLID)
        {
            // draw text background
            const wxString rectStyle = wxString::Format(
                wxS("style=\"%s %s stroke-width:1;\""),
                GetBrushFill(m_textBackgroundColour),
                GetPenStroke(m_textBackgroundColour));

            const wxString rectTransform = wxString::Format(
                wxS("transform=\"rotate(%s %s %s)\""),
                NumStr(-angle), NumStr(xRect), NumStr(yRect));

            s = wxString::Format(
                wxS("  <rect x=\"%s\" y=\"%s\" width=\"%d\" height=\"%d\" %s %s %s/>\n"),
                NumStr(xRect), NumStr(yRect), ww, hh,
                GetRenderMode(m_renderingMode), rectStyle, rectTransform);

            write(s);
        }

        const wxString transform = wxString::Format(
            wxS("transform=\"rotate(%s %s %s)\""),
            NumStr(-angle), NumStr(xText), NumStr(yText));

        s = wxString::Format(
            wxS("  <text x=\"%s\" y=\"%s\" textLength=\"%d\" %s %s>%s</text>\n"),
            NumStr(xText), NumStr(yText), ww, style, transform,
#if wxUSE_MARKUP
            wxMarkupParser::Quote(line)
#else
            line
#endif
        );

        write(s);
    }
}

void wxSVGFileDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    DoDrawRoundedRectangle(x, y, width, height, 0.0);
}

void wxSVGFileDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    NewGraphicsIfNeeded();
    wxString s;

    s = wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" rx=\"%s\" %s %s %s/>\n"),
        x, y, width, height, NumStr(radius),
        GetRenderMode(m_renderingMode), GetPenPattern(m_pen), GetBrushPattern(m_brush));

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

    s = wxS("  <polygon points=\"");

    for (int i = 0; i < n; i++)
    {
        s += wxString::Format(wxS("%d %d "), points[i].x + xoffset, points[i].y + yoffset);
        CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }

    s += wxString::Format(wxS("\" %s %s %s style=\"fill-rule:%s;\"/>\n"),
        GetRenderMode(m_renderingMode), GetPenPattern(m_pen), GetBrushPattern(m_brush),
        fillStyle == wxODDEVEN_RULE ? wxS("evenodd") : wxS("nonzero"));

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

    const double rh = height / 2.0;
    const double rw = width / 2.0;

    wxString s;
    s = wxString::Format(wxS("  <ellipse cx=\"%s\" cy=\"%s\" rx=\"%s\" ry=\"%s\" %s %s"),
        NumStr(x + rw), NumStr(y + rh), NumStr(rw), NumStr(rh),
        GetRenderMode(m_renderingMode), GetPenPattern(m_pen));
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

    s += wxString::Format(wxS("\" %s %s/>\n"),
        GetRenderMode(m_renderingMode), GetPenPattern(m_pen));

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
        arcFill += wxString::Format(wxS(" L%s %s z\" %s %s/>\n"),
            NumStr(xc), NumStr(yc),
            GetRenderMode(m_renderingMode), GetPenPattern(m_pen));
        write(arcFill);
    }

    wxDCBrushChanger setTransp(*GetOwner(), *wxTRANSPARENT_BRUSH);
    NewGraphicsIfNeeded();

    wxString arcLine = wxString::Format(wxS("%s\" %s %s/>\n"),
        arcPath, GetRenderMode(m_renderingMode), GetPenPattern(m_pen));
    write(arcLine);
}

void wxSVGFileDCImpl::DoGradientFillLinear(const wxRect& rect,
                                           const wxColour& initialColour,
                                           const wxColour& destColour,
                                           wxDirection nDirection)
{
    NewGraphicsIfNeeded();

    float initOpacity;
    float destOpacity;
    wxString initCol = Col2SVG(initialColour, &initOpacity);
    wxString destCol = Col2SVG(destColour, &destOpacity);

    const int x1 = ((nDirection & wxLEFT) > 0) ? 100 : 0;
    const int y1 = ((nDirection & wxUP) > 0) ? 100 : 0;
    const int x2 = ((nDirection & wxRIGHT) > 0) ? 100 : 0;
    const int y2 = ((nDirection & wxDOWN) > 0) ? 100 : 0;

    wxString s;
    s += wxS("  <defs>\n");
    s += wxString::Format(wxS("    <linearGradient id=\"gradient%zu\" x1=\"%d%%\" y1=\"%d%%\" x2=\"%d%%\" y2=\"%d%%\">\n"),
        m_gradientUniqueId, x1, y1, x2, y2);
    s += wxString::Format(wxS("      <stop offset=\"0%%\" style=\"stop-color:%s; stop-opacity:%s\"/>\n"),
        initCol, NumStr(initOpacity));
    s += wxString::Format(wxS("      <stop offset=\"100%%\" style=\"stop-color:%s; stop-opacity:%s\"/>\n"),
        destCol, NumStr(destOpacity));
    s += wxS("    </linearGradient>\n");
    s += wxS("  </defs>\n");

    s += wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"url(#gradient%zu)\" %s %s %s/>\n"),
        rect.x, rect.y, rect.width, rect.height, m_gradientUniqueId,
        GetRenderMode(m_renderingMode), GetPenPattern(m_pen), GetBrushPattern(m_brush));

    m_gradientUniqueId++;

    write(s);

    CalcBoundingBox(rect.x, rect.y);
    CalcBoundingBox(rect.x + rect.width, rect.y + rect.height);
}

void wxSVGFileDCImpl::DoGradientFillConcentric(const wxRect& rect,
                                               const wxColour& initialColour,
                                               const wxColour& destColour,
                                               const wxPoint& circleCenter)
{
    NewGraphicsIfNeeded();

    float initOpacity;
    float destOpacity;
    wxString initCol = Col2SVG(initialColour, &initOpacity);
    wxString destCol = Col2SVG(destColour, &destOpacity);

    const double cx = circleCenter.x * 100.0 / rect.GetWidth();
    const double cy = circleCenter.y * 100.0 / rect.GetHeight();
    const double fx = cx;
    const double fd = cy;

    wxString s;
    s += wxS("  <defs>\n");
    s += wxString::Format(wxS("    <radialGradient id=\"gradient%zu\" cx=\"%s%%\" cy=\"%s%%\" fx=\"%s%%\" fy=\"%s%%\">\n"),
        m_gradientUniqueId, NumStr(cx), NumStr(cy), NumStr(fx), NumStr(fd));
    s += wxString::Format(wxS("      <stop offset=\"0%%\" style=\"stop-color:%s; stop-opacity:%s\" />\n"),
        initCol, NumStr(initOpacity));
    s += wxString::Format(wxS("      <stop offset=\"100%%\" style=\"stop-color:%s; stop-opacity:%s\" />\n"),
        destCol, NumStr(destOpacity));
    s += wxS("    </radialGradient>\n");
    s += wxS("  </defs>\n");

    s += wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"url(#gradient%zu)\" %s %s %s/>\n"),
        rect.x, rect.y, rect.width, rect.height, m_gradientUniqueId,
        GetRenderMode(m_renderingMode), GetPenPattern(m_pen), GetBrushPattern(m_brush));

    m_gradientUniqueId++;

    write(s);

    CalcBoundingBox(rect.x, rect.y);
    CalcBoundingBox(rect.x + rect.width, rect.y + rect.height);
}

void wxSVGFileDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
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

void wxSVGFileDCImpl::DoGetTextExtent(const wxString& string,
                                      wxCoord* x,
                                      wxCoord* y,
                                      wxCoord* descent,
                                      wxCoord* externalLeading,
                                      const wxFont* theFont) const
{
    wxScreenDC sDC;
    SetScaledScreenDCFont(sDC, theFont ? *theFont : m_font);

    sDC.GetTextExtent(string, x, y, descent, externalLeading);
}

wxCoord wxSVGFileDCImpl::GetCharHeight() const
{
    wxScreenDC sDC;
    SetScaledScreenDCFont(sDC, m_font);

    return sDC.GetCharHeight();

}

wxCoord wxSVGFileDCImpl::GetCharWidth() const
{
    wxScreenDC sDC;
    SetScaledScreenDCFont(sDC, m_font);

    return sDC.GetCharWidth();
}


// ----------------------------------------------------------
// wxSVGFileDCImpl - set functions
// ----------------------------------------------------------

void wxSVGFileDCImpl::SetBackground(const wxBrush& brush)
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

void wxSVGFileDCImpl::SetShapeRenderingMode(wxSVGShapeRenderingMode renderingMode)
{
    m_renderingMode = renderingMode;
}

void wxSVGFileDCImpl::SetBrush(const wxBrush& brush)
{
    m_brush = brush;

    m_graphics_changed = true;

    wxString pattern = CreateBrushFill(m_brush, m_renderingMode);
    if ( !pattern.empty() )
    {
        NewGraphicsIfNeeded();

        write(pattern);
    }
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
    wxString s;

    s = wxString::Format(wxS("<g style=\"%s %s %s\" transform=\"translate(%s %s) scale(%s %s)\">\n"),
        GetPenStyle(m_pen),
        GetBrushFill(m_brush.GetColour(), m_brush.GetStyle()),
        GetPenStroke(m_pen.GetColour(), m_pen.GetStyle()),
        NumStr((m_deviceOriginX - m_logicalOriginX) * m_signX),
        NumStr((m_deviceOriginY - m_logicalOriginY) * m_signY),
        NumStr(m_scaleX * m_signX),
        NumStr(m_scaleY * m_signY));

    write(s);
}

void wxSVGFileDCImpl::SetFont(const wxFont& font)
{
    m_font = font;
}

bool wxSVGFileDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                             wxCoord width, wxCoord height,
                             wxDC* source,
                             wxCoord xsrc, wxCoord ysrc,
                             wxRasterOperationMode rop,
                             bool useMask,
                             wxCoord WXUNUSED(xsrcMask), wxCoord WXUNUSED(ysrcMask))
{
    if (rop != wxCOPY)
    {
        wxASSERT_MSG(false, wxS("wxSVGFileDC::DoBlit Call requested nonCopy mode; this is not possible"));
        return false;
    }
    if (useMask)
    {
        wxASSERT_MSG(false, "wxSVGFileDC::DoBlit Call requested mask; this is not possible");
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

void wxSVGFileDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    wxBitmap myBitmap(icon.GetWidth(), icon.GetHeight());
    wxMemoryDC memDC;
    memDC.SelectObject(myBitmap);
    memDC.DrawIcon(icon, 0, 0);
    memDC.SelectObject(wxNullBitmap);
    DoDrawBitmap(myBitmap, x, y);
}

void wxSVGFileDCImpl::DoDrawBitmap(const wxBitmap& bmp, wxCoord x, wxCoord y,
                                   bool WXUNUSED(useMask))
{
    NewGraphicsIfNeeded();

    // If we don't have any bitmap handler yet, use the default one.
    if ( !m_bmp_handler )
        m_bmp_handler.reset(new wxSVGBitmapFileHandler(m_filename));

    m_OK = m_outfile && m_outfile->IsOk();
    if (!m_OK)
        return;

    m_bmp_handler->ProcessBitmap(bmp, x, y, *m_outfile);
    m_OK = m_outfile->IsOk();
}

void wxSVGFileDCImpl::write(const wxString& s)
{
    m_OK = m_outfile && m_outfile->IsOk();
    if (!m_OK)
        return;

    const wxCharBuffer buf = s.utf8_str();
    m_outfile->Write(buf, strlen((const char*)buf));
    m_OK = m_outfile->IsOk();
}

#endif // wxUSE_SVG
