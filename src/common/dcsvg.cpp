/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/svg.cpp
// Purpose:     SVG sample
// Author:      Chris Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#if wxUSE_SVG

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
    #include "wx/icon.h"
    #include "wx/image.h"
    #include "wx/math.h"
#endif

#include "wx/base64.h"
#include "wx/dcsvg.h"
#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/svggc.h"
    #include "wx/dcgraph.h"
    #include "wx/private/svggc.h"
#endif
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/mstream.h"
#include "wx/sstream.h"
#include "wx/scopedarray.h"
#include "wx/private/rescale.h"
#include "wx/private/svg.h"

#if wxUSE_MARKUP
    #include "wx/private/markupparser.h"
#endif

// ----------------------------------------------------------
// Global utilities
// ----------------------------------------------------------

namespace
{
static const wxSize SVG_DPI(96, 96);
} // anonymous namespace

// ----------------------------------------------------------------------------
// wxSVGBitmapEmbedHandler
// ----------------------------------------------------------------------------

bool
wxSVGBitmapEmbedHandler::ProcessBitmap(const wxBitmap& bmp,
                                       wxCoord x, wxCoord y,
                                       wxOutputStream& stream) const
{
#if wxUSE_BASE64
    static int sub_images = 0;

    if ( wxImage::FindHandler(wxBITMAP_TYPE_PNG) == nullptr )
        wxImage::AddHandler(new wxPNGHandler);

    // write the bitmap as a PNG to a memory stream and Base64 encode
    wxMemoryOutputStream mem;
    bmp.ConvertToImage().SaveFile(mem, wxBITMAP_TYPE_PNG);
    wxString data = wxBase64Encode(mem.GetOutputStreamBuffer()->GetBufferStart(),
                                   mem.GetSize());

    // write image meta information
    wxString s;
    s.reserve(data.size());
    s += wxString::Format("  <image x=\"%d\" y=\"%d\" width=\"%dpx\" height=\"%dpx\"",
                          x, y, bmp.GetWidth(), bmp.GetHeight());
    s += wxString::Format(" id=\"image%d\" "
                          "xlink:href=\"data:image/png;base64,\n",
                          sub_images++);

    // Wrap Base64 encoded data on 76 columns boundary (same as Inkscape).
    constexpr unsigned WRAP = 76;
    for ( size_t i = 0; i < data.size(); i += WRAP )
    {
        if ( i < data.size() - WRAP )
            s += data.substr(i, WRAP) + "\n";
        else
            s += data.substr(i) + "\"\n  />\n"; // last line
    }

    // write to the SVG file
    const wxCharBuffer buf = s.utf8_str();
    stream.Write(buf, strlen((const char*)buf));

    return stream.IsOk();
#else
    // to avoid compiler warnings about unused variables
    wxUnusedVar(bmp);
    wxUnusedVar(x); wxUnusedVar(y);
    wxUnusedVar(stream);

    wxFAIL_MSG
    (
        "Embedding bitmaps in SVG is not available because "
        "wxWidgets was built with wxUSE_BASE64 set to 0."
    );

    return false;
#endif // wxUSE_BASE64
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

    if ( wxImage::FindHandler(wxBITMAP_TYPE_PNG) == nullptr )
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

wxString wxSVGFileDC::GetSVGDocument() const
{
    return ((wxSVGFileDCImpl*)GetImpl())->GetSVGDocument();
}

bool wxSVGFileDC::Save()
{
    return ((wxSVGFileDCImpl*)GetImpl())->Save();
}

void wxSVGFileDC::BeginAccessibleGroup(const wxSVGAttributes& attributes,
                                       const wxString& title,
                                       const wxString& desc)
{
    ((wxSVGFileDCImpl*)GetImpl())->BeginAccessibleGroup(attributes, title, desc);
}

void wxSVGFileDC::EndAccessibleGroup()
{
    ((wxSVGFileDCImpl*)GetImpl())->EndAccessibleGroup();
}

void wxSVGFileDC::BeginLayer(double opacity)
{
    ((wxSVGFileDCImpl*)GetImpl())->BeginLayer(opacity);
}

void wxSVGFileDC::EndLayer()
{
    ((wxSVGFileDCImpl*)GetImpl())->EndLayer();
}

// ----------------------------------------------------------
// wxSVGAttributes
// ----------------------------------------------------------

wxSVGAttributes& wxSVGAttributes::Add(const wxString& name, const wxString& value)
{
    // XML attribute names are case-sensitive.
    m_attributes[name] = value;
    return *this;
}

wxString wxSVGAttributes::GetAsString() const
{
    wxString s;
    bool first = true;
    for ( const auto& attr : m_attributes )
    {
        if ( !first )
            s << " ";
        first = false;

        s << attr.first << "=\"";
#if wxUSE_MARKUP
        s << wxMarkupParser::Quote(attr.second);
#else
        s << attr.second;
#endif
        s << "\"";
    }
    return s;
}

// ----------------------------------------------------------
// wxSVGFileDCImpl
// ----------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxSVGFileDCImpl, wxDCImpl);

size_t wxSVGWriter::ms_clipUniqueId = 0;
size_t wxSVGWriter::ms_gradientUniqueId = 0;

// ----------------------------------------------------------
// wxSVGFileDCImpl - string helpers (static)
// ----------------------------------------------------------

namespace wxSVG
{

wxString NumStr(double f)
{
    // Handle this case specially to avoid generating "-0.00" in the output.
    if ( f == 0 )
        return wxS("0.00");

    return wxString::FromCDouble(f, 2);
}

wxString NumStr(float f)
{
    return NumStr(double(f));
}

wxString Col2SVG(wxColour c, float* opacity)
{
    if ( c.Alpha() != wxALPHA_OPAQUE )
    {
        if ( opacity )
            *opacity = c.Alpha() / 255.0f;

        // Remove the alpha before using GetAsString(wxC2S_HTML_SYNTAX) as it
        // doesn't support colours with alpha channel.
        c = wxColour(c.GetRGB());
    }
    else
    {
        if ( opacity )
            *opacity = 1.0f;
    }

    return c.GetAsString(wxC2S_HTML_SYNTAX);
}

wxString GetPenStroke(const wxColour& c, int style)
{
    float opacity;
    wxString s = wxString::Format(wxS("stroke=\"%s\""), Col2SVG(c, &opacity));

    switch ( style )
    {
        case wxPENSTYLE_SOLID:
        case wxPENSTYLE_DOT:
        case wxPENSTYLE_SHORT_DASH:
        case wxPENSTYLE_LONG_DASH:
        case wxPENSTYLE_DOT_DASH:
        case wxPENSTYLE_USER_DASH:
            s += wxString::Format(wxS(" stroke-opacity=\"%s\""), NumStr(opacity));
            break;
        case wxPENSTYLE_TRANSPARENT:
            s += wxS(" stroke-opacity=\"0.0\"");
            break;
        default:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Pen Style not available"));
            break;
    }

    return s;
}

wxString GetBrushFill(const wxColour& c, int style)
{
    float opacity;
    wxString s = wxString::Format(wxS("fill=\"%s\""), Col2SVG(c, &opacity));

    switch ( style )
    {
        case wxBRUSHSTYLE_SOLID:
        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
        case wxBRUSHSTYLE_FDIAGONAL_HATCH:
        case wxBRUSHSTYLE_CROSSDIAG_HATCH:
        case wxBRUSHSTYLE_CROSS_HATCH:
        case wxBRUSHSTYLE_VERTICAL_HATCH:
        case wxBRUSHSTYLE_HORIZONTAL_HATCH:
            s += wxString::Format(wxS(" fill-opacity=\"%s\""), NumStr(opacity));
            break;
        case wxBRUSHSTYLE_TRANSPARENT:
            s += wxS(" fill-opacity=\"0.0\"");
            break;
        default:
            wxASSERT_MSG(false, wxS("wxSVGFileDC::Requested Brush Style not available"));
            break;
    }

    return s;
}

} // namespace wxSVG

wxSVGFileDCImpl::wxSVGFileDCImpl(wxSVGFileDC* owner, const wxString& filename,
                                 int width, int height, double dpi, const wxString& title)
    : wxDCImpl(owner)
{
    Init(filename, width, height, dpi, title);
}

void wxSVGFileDCImpl::Init(const wxString& filename, int width, int height,
                           double dpi, const wxString& title)
{
    m_mm_to_pix_x = dpi / 25.4;
    m_mm_to_pix_y = dpi / 25.4;

    m_backgroundBrush = *wxTRANSPARENT_BRUSH;
    m_textForegroundColour = *wxBLACK;
    m_textBackgroundColour = *wxWHITE;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_writer.reset(new wxSVGWriter(filename, width, height, dpi, title));
}

bool wxSVGFileDCImpl::IsOk() const
{
    return m_writer && m_writer->IsOk();
}

void wxSVGFileDCImpl::DoGetSize(int* width, int* height) const
{
    if ( width )
        *width = m_writer->GetWidth();
    if ( height )
        *height = m_writer->GetHeight();
}

wxString wxSVGFileDCImpl::GetSVGDocument() const
{
    return m_writer->GetDocument();
}

bool wxSVGFileDCImpl::Save()
{
    return m_writer->Save();
}

wxSVGFileDCImpl::~wxSVGFileDCImpl()
{
    if ( m_writer )
        m_writer->Save();
}

#if wxUSE_GRAPHICS_CONTEXT
wxGraphicsContext* wxSVGFileDCImpl::GetGraphicsContext() const
{
    if ( !m_gc )
        m_gc.reset(new wxSVGGraphicsContext(const_cast<wxSVGFileDCImpl*>(this)));
    return m_gc.get();
}
#endif // wxUSE_GRAPHICS_CONTEXT

void wxSVGFileDCImpl::DoGetSizeMM(int* width, int* height) const
{
    if ( width )
        *width = wxRound( (double)m_writer->GetWidth() / GetMMToPXx() );

    if ( height )
        *height = wxRound( (double)m_writer->GetHeight() / GetMMToPXy() );
}

wxSize wxSVGFileDCImpl::GetPPI() const
{
    const double dpi = m_writer->GetDPI();
    return wxSize(wxRound(dpi), wxRound(dpi));
}

wxSize wxSVGFileDCImpl::FromDIP(const wxSize& sz) const
{
    return sz;
}

wxSize wxSVGFileDCImpl::ToDIP(const wxSize& sz) const
{
    return sz;
}

void wxSVGFileDCImpl::Clear()
{
    {
        wxDCBrushChanger setBackground(*GetOwner(), m_backgroundBrush);
        wxDCPenChanger setTransp(*GetOwner(), *wxTRANSPARENT_PEN);
        DoDrawRectangle(0, 0, m_writer->GetWidth(), m_writer->GetHeight());
    }

    NewGraphicsIfNeeded();
}

void wxSVGFileDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    NewGraphicsIfNeeded();

    wxString s;
    s = wxString::Format(wxS("  <path d=\"M%d %d L%d %d\" %s %s/>\n"),
        x1, y1, x2, y2, wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));

    m_writer->Write(s);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(x1, y1, x2, y2);
}

void wxSVGFileDCImpl::DoDrawLines(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    if ( n > 1 )
    {
        NewGraphicsIfNeeded();
        wxString s;

        s = wxString::Format(wxS("  <path d=\"M%d %d"),
            (points[0].x + xoffset), (points[0].y + yoffset));

        if ( AreAutomaticBoundingBoxUpdatesEnabled() )
            CalcBoundingBox(points[0].x + xoffset, points[0].y + yoffset);

        for ( int i = 1; i < n; ++i )
        {
            s += wxString::Format(wxS(" L%d %d"), (points[i].x + xoffset), (points[i].y + yoffset));
            if ( AreAutomaticBoundingBoxUpdatesEnabled() )
                CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
        }

        s += wxString::Format(wxS("\" fill=\"none\" %s %s/>\n"),
            wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));

        m_writer->Write(s);
    }
}

#if wxUSE_SPLINES
void wxSVGFileDCImpl::DoDrawSpline(const wxPointList* points)
{
    wxCHECK_RET(points, "null pointer to spline points");
    wxCHECK_RET(points->size() >= 2, "incomplete list of spline points");

    NewGraphicsIfNeeded();

    wxPointList::const_iterator itPt = points->begin();
    const wxPoint* pt = *itPt; ++itPt;
    wxPoint2DDouble p1(*pt);

    pt = *itPt; ++itPt;
    wxPoint2DDouble p2(*pt);
    wxPoint2DDouble p3 = (p1 + p2) / 2.0;

    wxString s = "  <path d=\"";

    s += wxString::Format("M %s %s L %s %s",
                          wxSVG::NumStr(p1.m_x), wxSVG::NumStr(p1.m_y), wxSVG::NumStr(p3.m_x), wxSVG::NumStr(p3.m_y));
    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
    {
        CalcBoundingBox(wxRound(p1.m_x), wxRound(p1.m_y));
        CalcBoundingBox(wxRound(p3.m_x), wxRound(p3.m_y));
    }

    while ( itPt != points->end() )
    {
        pt = *itPt; ++itPt;

        wxPoint2DDouble p0 = p3;
        p1 = p2;
        p2 = *pt;
        p3 = (p1 + p2) / 2.0;

        // Calculate using degree elevation to a cubic bezier
        wxPoint2DDouble c1 = (p0 + (p1 * 2.0)) / 3.0;
        wxPoint2DDouble c2 = ((p1 * 2.0) + p3) / 3.0;

        s += wxString::Format(" C %s %s, %s %s, %s %s",
             wxSVG::NumStr(c1.m_x), wxSVG::NumStr(c1.m_y), wxSVG::NumStr(c2.m_x), wxSVG::NumStr(c2.m_y), wxSVG::NumStr(p3.m_x), wxSVG::NumStr(p3.m_y));

        if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        {
            CalcBoundingBox(wxRound(p0.m_x), wxRound(p0.m_y));
            CalcBoundingBox(wxRound(p3.m_x), wxRound(p3.m_y));
        }
    }
    s += wxString::Format(" L %s %s", wxSVG::NumStr(p2.m_x), wxSVG::NumStr(p2.m_y));
    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(wxRound(p2.m_x), wxRound(p2.m_y));

    s += wxString::Format("\" fill=\"none\" %s %s/>\n",
                          wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));
    m_writer->Write(s);
}
#endif // wxUSE_SPLINES

void wxSVGFileDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    NewGraphicsIfNeeded();

    wxString s;

    s = wxS("  <g stroke-width=\"1\" stroke-linecap=\"round\">\n  ");
    m_writer->Write(s);

    DoDrawLine(x, y, x, y);

    s = wxS("  </g>\n");
    m_writer->Write(s);
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
    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
    {
        CalcBoundingBox(x, y);
        CalcBoundingBox((wxCoord)(x + w * cos(rad)), (wxCoord)(y - h * sin(rad)));
        CalcBoundingBox((wxCoord)(x + h * sin(rad)), (wxCoord)(y + h * cos(rad)));
        CalcBoundingBox((wxCoord)(x + h * sin(rad) + w * cos(rad)), (wxCoord)(y + h * cos(rad) - w * sin(rad)));
    }

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
    if ( m_font.GetUnderlined() )
        textDecoration += wxS(" underline");
    if ( m_font.GetStrikethrough() )
        textDecoration += wxS(" line-through");
    if ( textDecoration.empty() )
        textDecoration = wxS(" none");

    wxString style;
    style += wxString::Format(wxS("font-family=\"%s\" "), m_font.GetFaceName());
    style += wxString::Format(wxS("font-weight=\"%d\" "), m_font.GetWeight());
    style += wxString::Format(wxS("font-style=\"%s\" "), fontstyle);
    style += wxString::Format(wxS("font-size=\"%spt\" "), wxSVG::NumStr(m_font.GetFractionalPointSize()));
    style += wxString::Format(wxS("text-decoration=\"%s\" "), textDecoration);
    style += wxString::Format(wxS("%s %s stroke-width=\"0\" "),
                              wxSVG::GetBrushFill(m_textForegroundColour),
                              wxSVG::GetPenStroke(m_textForegroundColour));

    // "xml:space" is deprecated in favour of "white-space: pre", keep it for now to
    // support SVG viewers that do not support the new tag
    style += wxS("xml:space=\"preserve\" ");
    style += wxS("style=\"white-space: pre;\" ");

    // Draw all text line by line
    const wxArrayString lines = wxSplit(sText, '\n', '\0');
    for ( size_t lineNum = 0; lineNum < lines.size(); lineNum++ )
    {
        const double xRect = x + lineNum * dx;
        const double yRect = y + lineNum * dy;

        // convert x,y to SVG text x,y (the coordinates of the text baseline)
        wxCoord ww, hh, desc;
        wxString const& line = lines[lineNum];
        DoGetTextExtent(line, &ww, &hh, &desc);
        const double xText = xRect + (hh - desc) * sin(rad);
        const double yText = yRect + (hh - desc) * cos(rad);

        if ( m_backgroundMode == wxBRUSHSTYLE_SOLID )
        {
            // draw text background
            const wxString rectStyle = wxString::Format(
                wxS("%s %s stroke-width=\"1\""),
                wxSVG::GetBrushFill(m_textBackgroundColour),
                wxSVG::GetPenStroke(m_textBackgroundColour));

            wxString rectTransform = wxString::Format(
                wxS("rotate(%s %s %s)"),
                wxSVG::NumStr(-angle), wxSVG::NumStr(xRect), wxSVG::NumStr(yRect));

#if wxUSE_GRAPHICS_CONTEXT
            if ( m_writer->GetGCTransform().StartsWith(wxS(" transform=\"")) )
            {
                rectTransform.Prepend(m_writer->GetGCTransform().AfterFirst('"').BeforeLast('"') + wxS(" "));
            }
#endif

            s = wxString::Format(
                wxS("  <rect x=\"%s\" y=\"%s\" width=\"%d\" height=\"%d\" %s %s transform=\"%s\"/>\n"),
                wxSVG::NumStr(xRect), wxSVG::NumStr(yRect), ww, hh,
                wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), rectStyle, rectTransform);

            m_writer->Write(s);
        }

        wxString transform = wxString::Format(
            wxS("rotate(%s %s %s)"),
            wxSVG::NumStr(-angle), wxSVG::NumStr(xText), wxSVG::NumStr(yText));

#if wxUSE_GRAPHICS_CONTEXT
        if ( m_writer->GetGCTransform().StartsWith(wxS(" transform=\"")) )
        {
            transform.Prepend(m_writer->GetGCTransform().AfterFirst('"').BeforeLast('"') + wxS(" "));
        }
#endif

        s = wxString::Format(
            wxS("  <text x=\"%s\" y=\"%s\" textLength=\"%d\" %s transform=\"%s\">%s</text>\n"),
            wxSVG::NumStr(xText), wxSVG::NumStr(yText), ww, style, transform,
#if wxUSE_MARKUP
            wxMarkupParser::Quote(line)
#else
            line
#endif
        );

        m_writer->Write(s);
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
        x, y, width, height, wxSVG::NumStr(radius),
        wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen), wxSVG::GetBrushPattern(m_brush));

    m_writer->Write(s);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(wxPoint(x, y), wxSize(width, height));
}

void wxSVGFileDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                                    wxCoord xoffset, wxCoord yoffset,
                                    wxPolygonFillMode fillStyle)
{
    NewGraphicsIfNeeded();

    wxString s;

    s = wxS("  <polygon points=\"");

    for ( int i = 0; i < n; i++ )
    {
        s += wxString::Format(wxS("%d %d "), points[i].x + xoffset, points[i].y + yoffset);
        if ( AreAutomaticBoundingBoxUpdatesEnabled() )
            CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }

    s += wxString::Format(wxS("\" %s %s %s fill-rule=\"%s\"/>\n"),
        wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen), wxSVG::GetBrushPattern(m_brush),
        fillStyle == wxODDEVEN_RULE ? wxS("evenodd") : wxS("nonzero"));

    m_writer->Write(s);
}

void wxSVGFileDCImpl::DoDrawPolyPolygon(int n, const int count[], const wxPoint points[],
                                        wxCoord xoffset, wxCoord yoffset,
                                        wxPolygonFillMode fillStyle)
{
    if ( n == 1 )
    {
        DoDrawPolygon(count[0], points, xoffset, yoffset, fillStyle);
        return;
    }

    int i, j;
    int totalPts = 0;
    for ( j = 0; j < n; ++j )
        totalPts += count[j];

    wxScopedArray<wxPoint> pts(totalPts + n);

    int polyCounter = 0, polyIndex = 0;
    for ( i = j = 0; i < totalPts; ++i )
    {
        pts[j++] = points[i];
        ++polyCounter;
        if ( polyCounter == count[polyIndex] )
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

    for ( i = j = 0; i < n; i++ )
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
        wxSVG::NumStr(x + rw), wxSVG::NumStr(y + rh), wxSVG::NumStr(rw), wxSVG::NumStr(rh),
        wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));
    s += wxS("/>\n");

    m_writer->Write(s);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(wxPoint(x, y), wxSize(width, height));
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
        m_writer->Write(s);
    }

    double theta1 = atan2((double)(yc - y1), (double)(x1 - xc));
    if ( theta1 < 0 )
        theta1 = theta1 + M_PI * 2;

    double theta2 = atan2((double)(yc - y2), (double)(x2 - xc));
    if ( theta2 < 0 )
        theta2 = theta2 + M_PI * 2;
    if ( theta2 < theta1 ) theta2 = theta2 + M_PI * 2;

    int fArc;                  // flag for large or small arc 0 means less than 180 degrees
    if ( fabs(theta2 - theta1) > M_PI )
        fArc = 1; else fArc = 0;

    int fSweep = 0;             // flag for sweep always 0

    if ( x1 == x2 && y1 == y2 )
    {
        // drawing full circle fails with default arc. Draw two half arcs instead.
        s = wxString::Format(wxS("  <path d=\"M%d %d a%s %s 0 %d %d %s 0 a%s %s 0 %d %d %s 0"),
            x1, y1,
            wxSVG::NumStr(r1), wxSVG::NumStr(r2), fArc, fSweep, wxSVG::NumStr( r1 * 2),
            wxSVG::NumStr(r1), wxSVG::NumStr(r2), fArc, fSweep, wxSVG::NumStr(-r1 * 2));
    }
    else
    {
        // comply to wxDC specs by drawing closing line if brush is not transparent
        wxString line;
        if ( GetBrush().GetStyle() != wxBRUSHSTYLE_TRANSPARENT )
            line = wxString::Format(wxS("L%d %d z"), xc, yc);

        s = wxString::Format(wxS("  <path d=\"M%d %d A%s %s 0 %d %d %d %d %s"),
            x1, y1, wxSVG::NumStr(r1), wxSVG::NumStr(r2), fArc, fSweep, x2, y2, line);
    }

    s += wxString::Format(wxS("\" %s %s/>\n"),
        wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));

    m_writer->Write(s);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
    {
        double r = wxMax(r1, r2);
        CalcBoundingBox(xc - r, yc - r, xc + r, yc + r);
    }
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
    if ( start < 0 )
        start += 360;
    while ( fabs(start) > 360 )
        start -= (start / fabs(start)) * 360;

    double end = (ea - 90);
    if ( end < 0 )
        end += 360;
    while ( fabs(end) > 360 )
        end -= (end / fabs(end)) * 360;

    // svg arcs are in clockwise direction, reverse angle
    double angle = end - start;
    if ( angle <= 0 )
        angle += 360;

    int fArc = angle > 180 ? 1 : 0; // flag for large or small arc
    int fSweep = 0;                 // flag for sweep always 0

    wxString arcPath;
    if ( angle == 360 )
    {
        // Drawing full circle fails with default arc. Draw two half arcs instead.
        fArc = 1;
        arcPath = wxString::Format(wxS("  <path d=\"M%d %s a%s %s 0 %d %d %s 0 a%s %s 0 %d %d %s 0"),
            x, wxSVG::NumStr(y + ry),
            wxSVG::NumStr(rx), wxSVG::NumStr(ry), fArc, fSweep, wxSVG::NumStr( rx * 2),
            wxSVG::NumStr(rx), wxSVG::NumStr(ry), fArc, fSweep, wxSVG::NumStr(-rx * 2));
    }
    else
    {
        arcPath = wxString::Format(wxS("  <path d=\"M%s %s A%s %s 0 %d %d %s %s"),
            wxSVG::NumStr(xs), wxSVG::NumStr(ys),
            wxSVG::NumStr(rx), wxSVG::NumStr(ry), fArc, fSweep, wxSVG::NumStr(xe), wxSVG::NumStr(ye));
    }

    // Workaround so SVG does not draw an extra line from the centre of the drawn arc
    // to the start point of the arc.
    // First draw the arc with the current brush, without a border,
    // then draw the border without filling the arc.
    if ( GetBrush().GetStyle() != wxBRUSHSTYLE_TRANSPARENT )
    {
        wxDCPenChanger setTransp(*GetOwner(), *wxTRANSPARENT_PEN);
        NewGraphicsIfNeeded();

        wxString arcFill = arcPath;
        arcFill += wxString::Format(wxS(" L%s %s z\" %s %s/>\n"),
            wxSVG::NumStr(xc), wxSVG::NumStr(yc),
            wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));
        m_writer->Write(arcFill);
    }

    wxDCBrushChanger setTransp(*GetOwner(), *wxTRANSPARENT_BRUSH);
    NewGraphicsIfNeeded();

    wxString arcLine = wxString::Format(wxS("%s\" %s %s/>\n"),
        arcPath, wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen));
    m_writer->Write(arcLine);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(x, y, x + w, y + h);
}

void wxSVGFileDCImpl::DoGradientFillLinear(const wxRect& rect,
                                           const wxColour& initialColour,
                                           const wxColour& destColour,
                                           wxDirection nDirection)
{
    NewGraphicsIfNeeded();

    float initOpacity;
    float destOpacity;
    wxString initCol = wxSVG::Col2SVG(initialColour, &initOpacity);
    wxString destCol = wxSVG::Col2SVG(destColour, &destOpacity);

    const int x1 = ((nDirection & wxLEFT) > 0) ? 100 : 0;
    const int y1 = ((nDirection & wxUP) > 0) ? 100 : 0;
    const int x2 = ((nDirection & wxRIGHT) > 0) ? 100 : 0;
    const int y2 = ((nDirection & wxDOWN) > 0) ? 100 : 0;

    const size_t gradId = m_writer->GetNextGradientId();

    wxString s;
    s += wxS("  <defs>\n");
    s += wxString::Format(wxS("    <linearGradient id=\"gradient%zu\" x1=\"%d%%\" y1=\"%d%%\" x2=\"%d%%\" y2=\"%d%%\">\n"),
        gradId, x1, y1, x2, y2);
    s += wxString::Format(wxS("      <stop offset=\"0%%\" stop-color=\"%s\" stop-opacity=\"%s\"/>\n"),
        initCol, wxSVG::NumStr(initOpacity));
    s += wxString::Format(wxS("      <stop offset=\"100%%\" stop-color=\"%s\" stop-opacity=\"%s\"/>\n"),
        destCol, wxSVG::NumStr(destOpacity));
    s += wxS("    </linearGradient>\n");
    s += wxS("  </defs>\n");

    s += wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"url(#gradient%zu)\" %s %s %s/>\n"),
        rect.x, rect.y, rect.width, rect.height, gradId,
        wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen), wxSVG::GetBrushPattern(m_brush));

    m_writer->Write(s);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(rect);
}

void wxSVGFileDCImpl::DoGradientFillConcentric(const wxRect& rect,
                                               const wxColour& initialColour,
                                               const wxColour& destColour,
                                               const wxPoint& circleCenter)
{
    NewGraphicsIfNeeded();

    float initOpacity;
    float destOpacity;
    wxString initCol = wxSVG::Col2SVG(initialColour, &initOpacity);
    wxString destCol = wxSVG::Col2SVG(destColour, &destOpacity);

    const double cx = circleCenter.x * 100.0 / rect.GetWidth();
    const double cy = circleCenter.y * 100.0 / rect.GetHeight();
    const double fx = cx;
    const double fd = cy;

    const size_t gradId = m_writer->GetNextGradientId();

    wxString s;
    s += wxS("  <defs>\n");
    s += wxString::Format(wxS("    <radialGradient id=\"gradient%zu\" cx=\"%s%%\" cy=\"%s%%\" fx=\"%s%%\" fy=\"%s%%\">\n"),
        gradId, wxSVG::NumStr(cx), wxSVG::NumStr(cy), wxSVG::NumStr(fx), wxSVG::NumStr(fd));
    s += wxString::Format(wxS("      <stop offset=\"0%%\" stop-color=\"%s\" stop-opacity=\"%s\" />\n"),
        initCol, wxSVG::NumStr(initOpacity));
    s += wxString::Format(wxS("      <stop offset=\"100%%\" stop-color=\"%s\" stop-opacity=\"%s\" />\n"),
        destCol, wxSVG::NumStr(destOpacity));
    s += wxS("    </radialGradient>\n");
    s += wxS("  </defs>\n");

    s += wxString::Format(wxS("  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"url(#gradient%zu)\" %s %s %s/>\n"),
        rect.x, rect.y, rect.width, rect.height, gradId,
        wxSVG::GetRenderMode(m_writer->GetShapeRenderingMode()), wxSVG::GetPenPattern(m_pen), wxSVG::GetBrushPattern(m_brush));

    m_writer->Write(s);

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(rect);
}

void wxSVGFileDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    wxRect clipBox = region.GetBox();
    wxPoint logPos = DeviceToLogical(clipBox.x, clipBox.y);
    wxSize logDim = DeviceToLogicalRel(clipBox.width, clipBox.height);
    DoSetClippingRegion(logPos.x, logPos.y, logDim.x, logDim.y);
}

void wxSVGFileDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    // We need to have box definition in the standard form with (x,y)
    // pointing to the top-left corner of the box and with non-negative
    // width and height because SVG doesn't accept negative values
    // of width/height and we need this standard form in the internal
    // calculations in wxDCImpl.
    if ( width < 0 )
    {
        width = -width;
        x -= (width - 1);
    }
    if ( height < 0 )
    {
        height = -height;
        y -= (height - 1);
    }

    const size_t clipId = m_writer->GetNextClipId();

    wxString svg;

    // End current graphics group to ensure proper xml nesting (e.g. so that
    // graphics can be subsequently changed inside the clipping region)
    svg << "</g>\n"
           "<defs>\n"
           "  <clipPath id=\"clip" << clipId << "\">\n"
           "    <rect id=\"cliprect" << clipId << "\" "
                "x=\"" << x << "\" "
                "y=\"" << y << "\" "
                "width=\"" << width << "\" "
                "height=\"" << height << "\" "
                "stroke=\"none\" fill=\"none\"/>\n"
           "  </clipPath>\n"
           "</defs>\n"
           "<g clip-path=\"url(#clip" << clipId << ")\">\n";

    m_writer->Write(svg);

    // Re-apply current graphics to ensure proper xml nesting
    DoStartNewGraphics();

    m_writer->IncrementClipNestingLevel();

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
    for ( int i = 0; i < m_writer->GetClipNestingLevel(); i++ )
    {
        svg << "</g>\n";
    }

    m_writer->Write(svg);

    // Re-apply current graphics (e.g. brush may have been changed inside one
    // of the clipped regions - that change will have been lost after xml
    // elements for the clipped region have been closed).
    DoStartNewGraphics();

    m_writer->SetClipNestingLevel(0);

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
    wxFont font = theFont ? *theFont : m_font;

    // A bitmap must be selected before wrapping the memory DC in a wxGCDC
    // (size is irrelevant).
    wxBitmap metricBmp(1, 1);
    wxMemoryDC memDc(metricBmp);
    const double scale = memDc.GetPPI().y / SVG_DPI.y;
    if ( scale != 1.0 && scale != 0.0 )
    {
        // SVG is DPI-independent, so we want text metrics at the default (96) DPI.
        // Text does not scale linearly (at least on MSW), so scale the font size,
        // rather than dividing the returned extent.
        font.SetFractionalPointSize(font.GetFractionalPointSize() / scale);
    }

#if wxUSE_GRAPHICS_CONTEXT
    // Measure through a wxGCDC so widths come from the platform's modern font engine
    // (DirectWrite / Core Graphics / Cairo).
    wxGCDC gcdc(memDc);
    gcdc.SetFont(font);
    gcdc.GetTextExtent(string, x, y, descent, externalLeading);
#else
    memDc.SetFont(font);
    memDc.GetTextExtent(string, x, y, descent, externalLeading);
#endif
}

wxCoord wxSVGFileDCImpl::GetCharHeight() const
{
    return wxSVG::GetTextMetricDC(m_font).GetCharHeight();
}

wxCoord wxSVGFileDCImpl::GetCharWidth() const
{
    return wxSVG::GetTextMetricDC(m_font).GetCharWidth();
}

void wxSVGFileDCImpl::ComputeScaleAndOrigin()
{
    wxDCImpl::ComputeScaleAndOrigin();
    m_writer->MarkGraphicsChanged();
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
    m_writer->SetBitmapHandler(handler);
}

void wxSVGFileDCImpl::SetShapeRenderingMode(wxSVGShapeRenderingMode renderingMode)
{
    m_writer->SetShapeRenderingMode(renderingMode);
}

void wxSVGFileDCImpl::SetBrush(const wxBrush& brush)
{
    m_brush = brush;
#if wxUSE_GRAPHICS_CONTEXT
    m_writer->SetGraphicsBrush(wxNullGraphicsBrush);
#endif

    m_writer->MarkGraphicsChanged();

    wxString pattern = wxSVG::CreateBrushFill(m_brush, m_writer->GetShapeRenderingMode());
    if ( !pattern.empty() )
    {
        NewGraphicsIfNeeded();

        m_writer->Write(pattern);
    }
}

void wxSVGFileDCImpl::SetPen(const wxPen& pen)
{
    m_pen = pen;
#if wxUSE_GRAPHICS_CONTEXT
    m_writer->SetGraphicsPen(wxNullGraphicsPen);
#endif

    m_writer->MarkGraphicsChanged();
}

void wxSVGFileDCImpl::NewGraphicsIfNeeded()
{
    if ( !m_writer->IsGraphicsChanged() )
        return;

    m_writer->ClearGraphicsChanged();

    m_writer->Write(wxS("</g>\n"));

    DoStartNewGraphics();
}


void wxSVGFileDCImpl::DoStartNewGraphics()
{
    wxString s;
    wxString brushFill;
    wxString penStroke;
    wxString style;

#if wxUSE_GRAPHICS_CONTEXT
    if ( !m_writer->GetGraphicsBrush().IsNull() )
    {
        brushFill = m_writer->WriteGraphicsBrushFill(m_writer->GetGraphicsBrush());
    }
    if ( !m_writer->GetGraphicsPen().IsNull() )
    {
        penStroke = m_writer->WriteGraphicsPenStroke(m_writer->GetGraphicsPen());
    }

    const wxCompositionMode composition = m_writer->GetCompositionMode();
    if ( composition != wxCOMPOSITION_INVALID && composition != wxCOMPOSITION_OVER )
    {
        wxString modeStr;
        switch ( composition )
        {
            case wxCOMPOSITION_ADD:  modeStr = wxS("plus-lighter"); break;
            case wxCOMPOSITION_DIFF: modeStr = wxS("difference"); break;
            default: break;
        }

        if ( !modeStr.empty() )
            style = wxString::Format(wxS("style=\"mix-blend-mode: %s\""), modeStr);
    }
#endif

    if ( brushFill.empty() )
        brushFill = wxSVG::GetBrushFill(m_brush.GetColour(), m_brush.GetStyle());

    if ( penStroke.empty() )
        penStroke = wxSVG::GetPenStroke(m_pen.GetColour(), m_pen.GetStyle());

    s = wxString::Format(wxS("<g %s %s %s %s transform=\"translate(%d %d) scale(%s %s)\">\n"),
        wxSVG::GetPenStyle(m_pen),
        brushFill,
        penStroke,
        style,
        (m_deviceOriginX - m_logicalOriginX) * m_signX,
        (m_deviceOriginY - m_logicalOriginY) * m_signY,
        wxSVG::NumStr(m_scaleX * m_signX),
        wxSVG::NumStr(m_scaleY * m_signY));

    m_writer->Write(s);
}

void wxSVGFileDCImpl::BeginAccessibleGroup(const wxSVGAttributes& attributes,
                                           const wxString& title,
                                           const wxString& desc)
{
    // Close the currently-open pen/brush group. There is always one open
    // at this point: either the initial default group written by Init()
    // or a group written by DoStartNewGraphics() during an earlier draw.
    m_writer->Write(wxS("</g>\n"));

    // Open the accessible group.
    wxString s = wxS("<g");
    if ( !attributes.IsEmpty() )
    {
        s += wxS(" ");
        s += attributes.GetAsString();
    }
    s += wxS(">\n");
    m_writer->Write(s);

    // Emit optional <title> and <desc> children. These must be the first
    // children of the accessible group for assistive technology to pick
    // them up, so write them before reopening the pen/brush group below.
    if ( !title.empty() )
    {
        m_writer->Write(wxString::Format(wxS("<title>%s</title>\n"),
#if wxUSE_MARKUP
                               wxMarkupParser::Quote(title)
#else
                               title
#endif
                               ));
    }
    if ( !desc.empty() )
    {
        m_writer->Write(wxString::Format(wxS("<desc>%s</desc>\n"),
#if wxUSE_MARKUP
                               wxMarkupParser::Quote(desc)
#else
                               desc
#endif
                               ));
    }

    m_writer->IncrementAccessibleGroupDepth();

    // Reopen a pen/brush group inside the accessible group so the invariant
    // "a pen/brush <g> is currently open" is preserved for later drawing.
    DoStartNewGraphics();
    m_writer->ClearGraphicsChanged();
}

void wxSVGFileDCImpl::EndAccessibleGroup()
{
    if ( m_writer->GetAccessibleGroupDepth() == 0 )
    {
        wxFAIL_MSG(wxS("wxSVGFileDC::EndAccessibleGroup() called without a matching BeginAccessibleGroup()"));
        return;
    }

    // Close the current pen/brush group (nested inside the accessible group),
    // then the accessible group itself.
    m_writer->Write(wxS("</g>\n"));
    m_writer->Write(wxS("</g>\n"));

    m_writer->DecrementAccessibleGroupDepth();

    // Reopen a pen/brush group at the now-outer nesting level so subsequent
    // drawing has somewhere to go.
    DoStartNewGraphics();
    m_writer->ClearGraphicsChanged();
}

void wxSVGFileDCImpl::BeginLayer(double opacity)
{
    // Close the currently-open pen/brush group.
    m_writer->Write(wxS("</g>\n"));

    // Open the layer group.
    m_writer->Write(wxString::Format(wxS("<g opacity=\"%s\">\n"), wxSVG::NumStr(opacity)));

    m_writer->IncrementLayerDepth();

    // Reopen a pen/brush group inside the layer group.
    DoStartNewGraphics();
    m_writer->ClearGraphicsChanged();
}

void wxSVGFileDCImpl::EndLayer()
{
    if ( m_writer->GetLayerDepth() == 0 )
    {
        wxFAIL_MSG(wxS("wxSVGFileDC::EndLayer() called without a matching BeginLayer()"));
        return;
    }

    // Close the current pen/brush group, then the layer group itself.
    m_writer->Write(wxS("</g>\n"));
    m_writer->Write(wxS("</g>\n"));

    m_writer->DecrementLayerDepth();

    // Reopen a pen/brush group at the now-outer nesting level.
    DoStartNewGraphics();
    m_writer->ClearGraphicsChanged();
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
    if ( rop != wxCOPY )
    {
        wxASSERT_MSG(false, wxS("wxSVGFileDC::DoBlit Call requested nonCopy mode; this is not possible"));
        return false;
    }
    if ( useMask )
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

    if ( AreAutomaticBoundingBoxUpdatesEnabled() )
        CalcBoundingBox(x, y, x + bmp.GetWidth(), y + bmp.GetHeight());

#if wxUSE_GRAPHICS_CONTEXT
    const bool wrapInGCTransform = !m_writer->GetGCTransform().empty();
    if ( wrapInGCTransform )
        m_writer->Write(wxString::Format(wxS("<g%s>\n"), m_writer->GetGCTransform()));
#endif

    m_writer->WriteBitmap(bmp, x, y);

#if wxUSE_GRAPHICS_CONTEXT
    if ( wrapInGCTransform )
    {
        m_writer->Write(wxS("</g>\n"));
        m_writer->MarkGraphicsChanged();
    }
#endif
}

namespace wxSVG
{

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
        if ((dashes != nullptr) && (count > 0))
        {
            for ( int i = 0; i < count; ++i )
            {
                s << dashes[i];
                if (i < count - 1)
                    s << wxS(",");
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

    penStyle += wxString::Format(wxS("stroke-width=\"%d\""), pen.GetWidth());

    switch (pen.GetCap())
    {
    case wxCAP_PROJECTING:
        penStyle += wxS(" stroke-linecap=\"square\"");
        break;
    case wxCAP_BUTT:
        penStyle += wxS(" stroke-linecap=\"butt\"");
        break;
    case wxCAP_ROUND:
    default:
        penStyle += wxS(" stroke-linecap=\"round\"");
        break;
    }

    switch (pen.GetJoin())
    {
    case wxJOIN_BEVEL:
        penStyle += wxS(" stroke-linejoin=\"bevel\"");
        break;
    case wxJOIN_MITER:
        penStyle += wxS(" stroke-linejoin=\"miter\"");
        break;
    case wxJOIN_ROUND:
    default:
        penStyle += wxS(" stroke-linejoin=\"round\"");
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

    if ( !brushStyle.empty() )
        brushStyle += wxString::Format(wxS("%s%02X"), Col2SVG(brush.GetColour()).substr(1), brush.GetColour().Alpha());

    return brushStyle;
}

wxString GetBrushPattern(const wxBrush& brush)
{
    wxString s;
    wxString brushStyle = GetBrushStyleName(brush);

    if ( !brushStyle.empty() )
        s = wxString::Format(wxS("fill=\"url(#%s)\""), brushStyle);

    return s;
}

wxString GetRenderMode(wxSVGShapeRenderingMode style)
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

    if ( !patternName.empty() )
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
        wxString brushStrokeStr = wxS("stroke-width=\"1\" stroke-linecap=\"round\" stroke-linejoin=\"round\"");

        s += wxString::Format(wxS("  <pattern id=\"%s\" patternUnits=\"userSpaceOnUse\" width=\"8\" height=\"8\">\n"),
            patternName);
        s += wxString::Format(wxS("    <path stroke=\"%s\" stroke-opacity=\"%s\" %s %s %s/>\n"),
            brushColourStr, NumStr(opacity), brushStrokeStr, pattern, GetRenderMode(mode));
        s += wxS("  </pattern>\n");
    }

    return s;
}

wxMemoryDC GetTextMetricDC(const wxFont& font)
{
    wxMemoryDC dc;
    const double dcDPI = dc.GetPPI().y;
    const double scale = dcDPI / SVG_DPI.y;
    if (scale != 1)
    {
        // The SVG is DPI-independent so we want text metrics for the default (96) DPI.
        //
        // We can't just divide the returned sizes by the scale factor, because
        // text does not scale linear (at least on Windows). Therefore, we scale
        // the font size instead.
        wxFont scaledFont = font;
        scaledFont.SetFractionalPointSize(scaledFont.GetFractionalPointSize() / scale);
        dc.SetFont(scaledFont);
    }
    else
    {
        dc.SetFont(font);
    }

    return dc;
}

} // namespace wxSVG

// ----------------------------------------------------------------------------
// wxSVGWriter
// ----------------------------------------------------------------------------

wxSVGWriter::wxSVGWriter(const wxString& filename,
                         int width, int height,
                         double dpi,
                         const wxString& title)
    : m_filename(filename),
      m_width(width),
      m_height(height),
      m_dpi(dpi)
{
    WriteHeader(title);
}

void wxSVGWriter::Write(const wxString& s)
{
    m_svgDocument += s;
}

void wxSVGWriter::WriteHeader(const wxString& title)
{
    wxString s;
    s += wxS("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    s += wxS("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n\n");
    s += wxS("<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"");
    s += wxString::Format(wxS(" width=\"%scm\" height=\"%scm\" viewBox=\"0 0 %d %d\">\n"),
                          wxSVG::NumStr(m_width / m_dpi * 2.54),
                          wxSVG::NumStr(m_height / m_dpi * 2.54),
                          m_width, m_height);
    s += wxString::Format(wxS("<title>%s</title>\n"),
#if wxUSE_MARKUP
                          wxMarkupParser::Quote(title)
#else
                          title
#endif
                          );
    s += wxString::Format(wxS("<desc>Created with %s</desc>\n\n"), wxVERSION_STRING);
    s += wxS("<g fill=\"black\" stroke=\"black\" stroke-width=\"1\">\n");
    Write(s);
}

wxString wxSVGWriter::GetDocument() const
{
    wxString doc(m_svgDocument);

    // Close remaining clipping group elements
    for ( int i = 0; i < m_clipNestingLevel; i++ )
        doc += wxS("</g>\n");

    // Close remaining layer group elements
    for ( int i = 0; i < m_layerDepth; i++ )
        doc += wxS("</g>\n");

    // Close the currently-open pen/brush group.
    doc += wxS("</g>\n");

    // Close any accessible groups left open by missing EndAccessibleGroup()
    // calls, so the output is still well-formed XML.
    for ( int i = 0; i < m_accessibleGroupDepth; i++ )
        doc += wxS("</g>\n");

    doc += wxS("</svg>\n");

    return doc;
}

bool wxSVGWriter::Save()
{
    if ( m_saved || m_filename.empty() )
        return m_saved;

    wxFile file(m_filename, wxFile::write);
    if ( file.IsOpened() )
        m_saved = file.Write(GetDocument(), wxConvUTF8) && file.Close();

    if ( !m_saved )
        m_writeError = true;

    return m_saved;
}

void wxSVGWriter::SetBitmapHandler(wxSVGBitmapHandler* handler)
{
    m_bmpHandler.reset(handler);
}

void wxSVGWriter::WriteBitmap(const wxBitmap& bmp, wxCoord x, wxCoord y)
{
    // If we don't have any bitmap handler yet, use the default one.
    if ( !m_bmpHandler )
        m_bmpHandler.reset(new wxSVGBitmapFileHandler(m_filename));

    wxStringOutputStream stream(&m_svgDocument);
    if ( !m_bmpHandler->ProcessBitmap(bmp, x, y, stream) )
        m_writeError = true;
}

#if wxUSE_GRAPHICS_CONTEXT

bool wxSVGWriter::SetCompositionMode(wxCompositionMode mode)
{
    if ( m_compositionMode == mode )
        return false;

    m_compositionMode = mode;
    m_graphicsChanged = true;
    return true;
}

wxString wxSVGWriter::WriteGraphicsBrushFill(const wxGraphicsBrush& brush)
{
    if ( brush.IsNull() )
        return wxString();

    auto* data = static_cast<wxSVGGraphicsBrushData*>(brush.GetRefData());
    if ( !data || !data->IsGradient() )
        return wxString();

    wxString s;
    s += wxS("  <defs>\n");

    wxString gradId = wxString::Format(wxS("gradient%zu"), GetNextGradientId());

    wxString transform;
    const wxGraphicsMatrix& matrix = data->GetMatrix();
    if ( !matrix.IsNull() && !matrix.IsIdentity() )
    {
        wxDouble a, b, c, d, tx, ty;
        matrix.Get(&a, &b, &c, &d, &tx, &ty);
        transform = wxString::Format(wxS(" gradientTransform=\"matrix(%s %s %s %s %s %s)\""),
            wxSVG::NumStr(a), wxSVG::NumStr(b), wxSVG::NumStr(c), wxSVG::NumStr(d), wxSVG::NumStr(tx), wxSVG::NumStr(ty));
    }

    if ( data->IsRadial() )
    {
        wxDouble startX, startY, endX, endY, radius;
        data->GetRadialParameters(&startX, &startY, &endX, &endY, &radius);
        s += wxString::Format(wxS("    <radialGradient id=\"%s\" cx=\"%s\" cy=\"%s\" r=\"%s\" fx=\"%s\" fy=\"%s\"%s gradientUnits=\"userSpaceOnUse\">\n"),
            gradId, wxSVG::NumStr(endX), wxSVG::NumStr(endY), wxSVG::NumStr(radius), wxSVG::NumStr(startX), wxSVG::NumStr(startY), transform);
    }
    else
    {
        wxDouble x1, y1, x2, y2;
        data->GetLinearParameters(&x1, &y1, &x2, &y2);
        s += wxString::Format(wxS("    <linearGradient id=\"%s\" x1=\"%s\" y1=\"%s\" x2=\"%s\" y2=\"%s\"%s gradientUnits=\"userSpaceOnUse\">\n"),
            gradId, wxSVG::NumStr(x1), wxSVG::NumStr(y1), wxSVG::NumStr(x2), wxSVG::NumStr(y2), transform);
    }

    const wxGraphicsGradientStops& stops = data->GetStops();
    for ( size_t i = 0; i < stops.GetCount(); ++i )
    {
        wxGraphicsGradientStop stop = stops.Item(static_cast<unsigned>(i));
        float opacity;
        wxString col = wxSVG::Col2SVG(stop.GetColour(), &opacity);
        s += wxString::Format(wxS("      <stop offset=\"%s%%\" stop-color=\"%s\" stop-opacity=\"%s\"/>\n"),
            wxSVG::NumStr(stop.GetPosition() * 100), col, wxSVG::NumStr(opacity));
    }

    if ( data->IsRadial() )
        s += wxS("    </radialGradient>\n");
    else
        s += wxS("    </linearGradient>\n");

    s += wxS("  </defs>\n");
    Write(s);

    return wxString::Format(wxS("fill=\"url(#%s)\""), gradId);
}

wxString wxSVGWriter::WriteGraphicsPenStroke(const wxGraphicsPen& pen)
{
    if ( pen.IsNull() )
        return wxString();

    auto* data = static_cast<wxSVGGraphicsPenData*>(pen.GetRefData());
    if ( !data || data->GetInfo().GetGradientType() == wxGRADIENT_NONE )
        return wxString();

    const wxGraphicsPenInfo& info = data->GetInfo();

    wxString s;
    s += wxS("  <defs>\n");

    wxString gradId = wxString::Format(wxS("gradient%zu"), GetNextGradientId());

    wxString transform;
    const wxGraphicsMatrix& matrix = info.GetMatrix();
    if ( !matrix.IsNull() && !matrix.IsIdentity() )
    {
        wxDouble a, b, c, d, tx, ty;
        matrix.Get(&a, &b, &c, &d, &tx, &ty);
        transform = wxString::Format(wxS(" gradientTransform=\"matrix(%s %s %s %s %s %s)\""),
            wxSVG::NumStr(a), wxSVG::NumStr(b), wxSVG::NumStr(c), wxSVG::NumStr(d), wxSVG::NumStr(tx), wxSVG::NumStr(ty));
    }

    if ( info.GetGradientType() == wxGRADIENT_RADIAL )
    {
        s += wxString::Format(wxS("    <radialGradient id=\"%s\" cx=\"%s\" cy=\"%s\" r=\"%s\" fx=\"%s\" fy=\"%s\"%s gradientUnits=\"userSpaceOnUse\">\n"),
            gradId, wxSVG::NumStr(info.GetEndX()), wxSVG::NumStr(info.GetEndY()), wxSVG::NumStr(info.GetRadius()), wxSVG::NumStr(info.GetStartX()), wxSVG::NumStr(info.GetStartY()), transform);
    }
    else
    {
        s += wxString::Format(wxS("    <linearGradient id=\"%s\" x1=\"%s\" y1=\"%s\" x2=\"%s\" y2=\"%s\"%s gradientUnits=\"userSpaceOnUse\">\n"),
            gradId, wxSVG::NumStr(info.GetX1()), wxSVG::NumStr(info.GetY1()), wxSVG::NumStr(info.GetX2()), wxSVG::NumStr(info.GetY2()), transform);
    }

    const wxGraphicsGradientStops& stops = info.GetStops();
    for ( size_t i = 0; i < stops.GetCount(); ++i )
    {
        wxGraphicsGradientStop stop = stops.Item(static_cast<unsigned>(i));
        float opacity;
        wxString col = wxSVG::Col2SVG(stop.GetColour(), &opacity);
        s += wxString::Format(wxS("      <stop offset=\"%s%%\" stop-color=\"%s\" stop-opacity=\"%s\"/>\n"),
            wxSVG::NumStr(stop.GetPosition() * 100), col, wxSVG::NumStr(opacity));
    }

    if ( info.GetGradientType() == wxGRADIENT_RADIAL )
        s += wxS("    </radialGradient>\n");
    else
        s += wxS("    </linearGradient>\n");

    s += wxS("  </defs>\n");
    Write(s);

    return wxString::Format(wxS("stroke=\"url(#%s)\""), gradId);
}

#endif // wxUSE_GRAPHICS_CONTEXT

#endif // wxUSE_SVG
