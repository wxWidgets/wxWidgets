/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/svggc.cpp
// Purpose:     wxGraphicsContext/Renderer backed by wxSVGFileDC
// Author:      Blake Madden
// Copyright:   (c) Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/image.h"
    #include "wx/log.h"
    #include "wx/math.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/font.h"
#endif

#include "wx/svggc.h"
#include "wx/dcsvg.h"
#include "wx/private/graphics.h"

#include <cmath>
#include <vector>

namespace
{

// Represents a single command in an SVG path (Move, Line, etc.).
// We store these instead of a raw string so that we can apply
// transformations to the path after it has been recorded.
struct wxSVGPathSegment
{
    enum Type { MoveSegment, LineSegment, CurveSegment, ArcSegment, CloseSegment } type;
    wxDouble x, y;
    wxDouble x1, y1, x2, y2;
    wxDouble r;
    wxDouble startAngle, endAngle;
    bool clockwise = true;

    static wxSVGPathSegment MoveTo(wxDouble x, wxDouble y)
    {
        wxSVGPathSegment s;
        s.type = MoveSegment;
        s.x = x; s.y = y;
        return s;
    }
    static wxSVGPathSegment LineTo(wxDouble x, wxDouble y)
    {
        wxSVGPathSegment s;
        s.type = LineSegment;
        s.x = x;
        s.y = y;
        return s;
    }
    static wxSVGPathSegment CurveTo(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, wxDouble x, wxDouble y)
    {
        wxSVGPathSegment s;
        s.type = CurveSegment;
        s.x1 = x1;
        s.y1 = y1;
        s.x2 = x2;
        s.y2 = y2;
        s.x = x;
        s.y = y;
        return s;
    }
    static wxSVGPathSegment Arc(wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise)
    {
        wxSVGPathSegment s;
        s.type = ArcSegment;
        s.x = x;
        s.y = y;
        s.r = r;
        s.startAngle = startAngle;
        s.endAngle = endAngle;
        s.clockwise = clockwise;
        return s;
    }
    static wxSVGPathSegment Close()
    {
        wxSVGPathSegment s;
        s.type = CloseSegment;
        return s;
    }
};

// ----------------------------------------------------------------------------
// wxSVGGraphicsMatrixData - standard 2D affine transformation matrix
// ----------------------------------------------------------------------------
class wxSVGGraphicsMatrixData : public wxGraphicsMatrixData
{
public:
    wxSVGGraphicsMatrixData(wxGraphicsRenderer* renderer,
                            wxDouble a = 1.0, wxDouble b = 0.0,
                            wxDouble c = 0.0, wxDouble d = 1.0,
                            wxDouble tx = 0.0, wxDouble ty = 0.0)
        : wxGraphicsMatrixData(renderer),
          m_a(a), m_b(b), m_c(c), m_d(d), m_tx(tx), m_ty(ty)
    {
    }

    wxGraphicsObjectRefData* Clone() const override
    {
        return new wxSVGGraphicsMatrixData(GetRenderer(), m_a, m_b, m_c, m_d, m_tx, m_ty);
    }

    void Concat(const wxGraphicsMatrixData* t) override
    {
        const auto* o = static_cast<const wxSVGGraphicsMatrixData*>(t);
        const wxDouble a  = o->m_a * m_a + o->m_b * m_c;
        const wxDouble b  = o->m_a * m_b + o->m_b * m_d;
        const wxDouble c  = o->m_c * m_a + o->m_d * m_c;
        const wxDouble d  = o->m_c * m_b + o->m_d * m_d;
        const wxDouble tx = o->m_tx * m_a + o->m_ty * m_c + m_tx;
        const wxDouble ty = o->m_tx * m_b + o->m_ty * m_d + m_ty;
        m_a = a; m_b = b; m_c = c; m_d = d; m_tx = tx; m_ty = ty;
    }

    void Set(wxDouble a = 1.0, wxDouble b = 0.0, wxDouble c = 0.0, wxDouble d = 1.0,
             wxDouble tx = 0.0, wxDouble ty = 0.0) override
    {
        m_a = a; m_b = b; m_c = c; m_d = d; m_tx = tx; m_ty = ty;
    }

    void Get(wxDouble* a = nullptr, wxDouble* b = nullptr, wxDouble* c = nullptr,
             wxDouble* d = nullptr, wxDouble* tx = nullptr, wxDouble* ty = nullptr) const override
    {
        if ( a != nullptr )
            *a  = m_a;
        if ( b != nullptr )
            *b  = m_b;
        if ( c != nullptr )
            *c  = m_c;
        if ( d != nullptr )
            *d  = m_d;
        if ( tx != nullptr )
            *tx = m_tx;
        if ( ty != nullptr )
            *ty = m_ty;
    }

    void Invert() override
    {
        const wxDouble det = m_a * m_d - m_b * m_c;
        if ( det == 0 )
            return;

        const wxDouble inv = 1.0 / det;
        const wxDouble a  =  m_d * inv;
        const wxDouble b  = -m_b * inv;
        const wxDouble c  = -m_c * inv;
        const wxDouble d  =  m_a * inv;
        const wxDouble tx = (m_c * m_ty - m_d * m_tx) * inv;
        const wxDouble ty = (m_b * m_tx - m_a * m_ty) * inv;
        m_a = a; m_b = b; m_c = c; m_d = d; m_tx = tx; m_ty = ty;
    }

    bool IsEqual(const wxGraphicsMatrixData* t) const override
    {
        const auto* o = static_cast<const wxSVGGraphicsMatrixData*>(t);
        return m_a == o->m_a && m_b == o->m_b && m_c == o->m_c
            && m_d == o->m_d && m_tx == o->m_tx && m_ty == o->m_ty;
    }

    bool IsIdentity() const override
    {
        return m_a == 1.0 && m_b == 0.0 && m_c == 0.0 && m_d == 1.0
            && m_tx == 0.0 && m_ty == 0.0;
    }

    void Translate(wxDouble dx, wxDouble dy) override
    {
        m_tx += dx * m_a + dy * m_c;
        m_ty += dx * m_b + dy * m_d;
    }

    void Scale(wxDouble xScale, wxDouble yScale) override
    {
        m_a *= xScale;
        m_b *= xScale;
        m_c *= yScale;
        m_d *= yScale;
    }

    void Rotate(wxDouble angle) override
    {
        const wxDouble s = std::sin(angle);
        const wxDouble c = std::cos(angle);
        const wxDouble a =  m_a * c + m_c * s;
        const wxDouble b =  m_b * c + m_d * s;
        const wxDouble cc = -m_a * s + m_c * c;
        const wxDouble dd = -m_b * s + m_d * c;
        m_a = a;
        m_b = b;
        m_c = cc;
        m_d = dd;
    }

    void TransformPoint(wxDouble* x, wxDouble* y) const override
    {
        const wxDouble xx = *x * m_a + *y * m_c + m_tx;
        const wxDouble yy = *x * m_b + *y * m_d + m_ty;
        *x = xx;
        *y = yy;
    }

    void TransformDistance(wxDouble* dx, wxDouble* dy) const override
    {
        const wxDouble xx = *dx * m_a + *dy * m_c;
        const wxDouble yy = *dx * m_b + *dy * m_d;
        *dx = xx;
        *dy = yy;
    }

    void* GetNativeMatrix() const override { return nullptr; }

    wxDouble m_a, m_b, m_c, m_d, m_tx, m_ty;
};

// ----------------------------------------------------------------------------
// wxSVGGraphicsPathData - records path commands for deferred SVG generation
// ----------------------------------------------------------------------------
class wxSVGGraphicsPathData : public wxGraphicsPathData
{
public:
    explicit wxSVGGraphicsPathData(wxGraphicsRenderer* renderer)
        : wxGraphicsPathData(renderer),
          m_curX(0), m_curY(0),
          m_hasPoints(false),
          m_minX(0), m_minY(0), m_maxX(0), m_maxY(0)
    {
    }

    wxGraphicsObjectRefData* Clone() const override
    {
        auto* p = new wxSVGGraphicsPathData(GetRenderer());
        p->m_segments = m_segments;
        p->m_curX = m_curX;
        p->m_curY = m_curY;
        p->m_hasPoints = m_hasPoints;
        p->m_minX = m_minX;
        p->m_minY = m_minY;
        p->m_maxX = m_maxX;
        p->m_maxY = m_maxY;
        return p;
    }

    void MoveToPoint(wxDouble x, wxDouble y) override
    {
        m_segments.push_back(wxSVGPathSegment::MoveTo(x, y));
        m_curX = x; m_curY = y;
        Extend(x, y);
    }

    void AddLineToPoint(wxDouble x, wxDouble y) override
    {
        m_segments.push_back(wxSVGPathSegment::LineTo(x, y));
        m_curX = x; m_curY = y;
        Extend(x, y);
    }

    void AddCurveToPoint(wxDouble cx1, wxDouble cy1,
                         wxDouble cx2, wxDouble cy2,
                         wxDouble x, wxDouble y) override
    {
        m_segments.push_back(wxSVGPathSegment::CurveTo(cx1, cy1, cx2, cy2, x, y));
        m_curX = x; m_curY = y;
        Extend(cx1, cy1);
        Extend(cx2, cy2);
        Extend(x, y);
    }

    void AddPath(const wxGraphicsPathData* path) override
    {
        const auto* o = static_cast<const wxSVGGraphicsPathData*>(path);
        m_segments.insert(m_segments.end(), o->m_segments.begin(), o->m_segments.end());
        m_curX = o->m_curX;
        m_curY = o->m_curY;
        if ( o->m_hasPoints )
        {
            Extend(o->m_minX, o->m_minY);
            Extend(o->m_maxX, o->m_maxY);
        }
    }

    void CloseSubpath() override
    {
        m_segments.push_back(wxSVGPathSegment::Close());
    }

    void GetCurrentPoint(wxDouble* x, wxDouble* y) const override
    {
        if ( x != nullptr )
            *x = m_curX;
        if ( y != nullptr )
            *y = m_curY;
    }

    void AddArc(wxDouble x, wxDouble y, wxDouble r,
                wxDouble startAngle, wxDouble endAngle, bool clockwise) override
    {
        // Calculate start and end points of the arc for SVG path generation.
        const wxDouble sx = x + r * std::cos(startAngle);
        const wxDouble sy = y + r * std::sin(startAngle);
        const wxDouble ex = x + r * std::cos(endAngle);
        const wxDouble ey = y + r * std::sin(endAngle);

        if ( !m_hasPoints )
            MoveToPoint(sx, sy);
        else
            AddLineToPoint(sx, sy);

        m_segments.push_back(wxSVGPathSegment::Arc(x, y, r, startAngle, endAngle, clockwise));
        m_curX = ex; m_curY = ey;

        Extend(x - r, y - r);
        Extend(x + r, y + r);
    }

    void* GetNativePath() const override
    {
        // Regenerate the "d" string from segments on demand.
        const_cast<wxSVGGraphicsPathData*>(this)->m_d = GetDString();
        return const_cast<wxString*>(&m_d);
    }

    void UnGetNativePath(void*) const override {}

    void Transform(const wxGraphicsMatrixData* matrix) override
    {
        const auto* m = static_cast<const wxSVGGraphicsMatrixData*>(matrix);
        std::vector<wxSVGPathSegment> oldSegments = m_segments;
        m_segments.clear();
        m_hasPoints = false;

        // Arcs must be converted to cubic Beziers before transformation
        // because a transformed elliptical arc is generally a generic conic.
        for ( const auto& s : oldSegments )
        {
            if ( s.type == wxSVGPathSegment::ArcSegment )
            {
                const wxDouble sx = s.x + s.r * std::cos(s.startAngle);
                const wxDouble sy = s.y + s.r * std::sin(s.startAngle);
                if (!m_hasPoints)
                    MoveToPoint(sx, sy);
                else
                    AddLineToPoint(sx, sy);

                AddArcToCubics(s.x, s.y, s.r, s.startAngle, s.endAngle, s.clockwise);
            }
            else
            {
                m_segments.push_back(s);
                if (s.type != wxSVGPathSegment::CloseSegment)
                    m_hasPoints = true;
            }
        }

        // Apply the transformation matrix to all points in the path.
        m_hasPoints = false;
        for ( auto& s : m_segments )
        {
            switch ( s.type )
            {
                case wxSVGPathSegment::MoveSegment:
                case wxSVGPathSegment::LineSegment:
                    m->TransformPoint(&s.x, &s.y);
                    Extend(s.x, s.y);
                    break;
                case wxSVGPathSegment::CurveSegment:
                    m->TransformPoint(&s.x1, &s.y1);
                    m->TransformPoint(&s.x2, &s.y2);
                    m->TransformPoint(&s.x, &s.y);
                    Extend(s.x1, s.y1);
                    Extend(s.x2, s.y2);
                    Extend(s.x, s.y);
                    break;
                case wxSVGPathSegment::ArcSegment:
                    break;
                case wxSVGPathSegment::CloseSegment:
                    break;
            }
        }

        if ( !m_segments.empty() )
        {
            const auto& last = m_segments.back();
            if ( last.type != wxSVGPathSegment::CloseSegment )
            {
                m_curX = last.x;
                m_curY = last.y;
            }
        }
    }

    void GetBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) const override
    {
        if ( !m_hasPoints )
        {
            if ( x != nullptr )
                *x = 0;
            if ( y != nullptr)
                *y = 0;
            if ( w != nullptr )
                *w = 0;
            if ( h != nullptr )
                *h = 0;
            return;
        }
        if ( x != nullptr )
            *x = m_minX;
        if ( y != nullptr )
            *y = m_minY;
        if ( w != nullptr )
            *w = m_maxX - m_minX;
        if ( h != nullptr )
            *h = m_maxY - m_minY;
    }

    bool Contains(wxDouble WXUNUSED(x), wxDouble WXUNUSED(y),
                  wxPolygonFillMode WXUNUSED(fillStyle) = wxODDEVEN_RULE) const override
    {
        /* TODO */
         return false;
    }

    // Generates the SVG "d" attribute string from recorded segments.
    wxString GetDString() const
    {
        wxString d;
        for ( const auto& s : m_segments )
        {
            switch ( s.type )
            {
                case wxSVGPathSegment::MoveSegment:
                    d += wxString::Format(wxS("M %s %s "),
                        wxSVGFileDCImpl::NumStr(s.x), wxSVGFileDCImpl::NumStr(s.y));
                    break;
                case wxSVGPathSegment::LineSegment:
                    d += wxString::Format(wxS("L %s %s "),
                        wxSVGFileDCImpl::NumStr(s.x), wxSVGFileDCImpl::NumStr(s.y));
                    break;
                case wxSVGPathSegment::CurveSegment:
                    d += wxString::Format(wxS("C %s %s %s %s %s %s "),
                        wxSVGFileDCImpl::NumStr(s.x1), wxSVGFileDCImpl::NumStr(s.y1),
                        wxSVGFileDCImpl::NumStr(s.x2), wxSVGFileDCImpl::NumStr(s.y2),
                        wxSVGFileDCImpl::NumStr(s.x), wxSVGFileDCImpl::NumStr(s.y));
                    break;
                case wxSVGPathSegment::ArcSegment:
                {
                    wxDouble sweep = s.endAngle - s.startAngle;
                    if ( s.clockwise )
                    {
                        while ( sweep < 0 )
                            sweep += 2 * M_PI;
                    }
                    else
                    {
                        while ( sweep > 0 )
                            sweep -= 2 * M_PI;
                        sweep = -sweep;
                    }

                    if ( sweep >= 2 * M_PI - 1e-9 )
                    {
                        const wxDouble ox = s.x - s.r, oy = s.y;
                        d += wxString::Format(wxS("A %s %s 0 1 %d %s %s "),
                            wxSVGFileDCImpl::NumStr(s.r), wxSVGFileDCImpl::NumStr(s.r),
                            s.clockwise ? 1 : 0,
                            wxSVGFileDCImpl::NumStr(s.x + s.r), wxSVGFileDCImpl::NumStr(s.y));
                        d += wxString::Format(wxS("A %s %s 0 1 %d %s %s "),
                            wxSVGFileDCImpl::NumStr(s.r), wxSVGFileDCImpl::NumStr(s.r),
                            s.clockwise ? 1 : 0,
                            wxSVGFileDCImpl::NumStr(ox), wxSVGFileDCImpl::NumStr(oy));
                    }
                    else
                    {
                        const wxDouble ex = s.x + s.r * std::cos(s.endAngle);
                        const wxDouble ey = s.y + s.r * std::sin(s.endAngle);
                        const int large = sweep > M_PI ? 1 : 0;
                        d += wxString::Format(wxS("A %s %s 0 %d %d %s %s "),
                            wxSVGFileDCImpl::NumStr(s.r), wxSVGFileDCImpl::NumStr(s.r),
                            large, s.clockwise ? 1 : 0,
                            wxSVGFileDCImpl::NumStr(ex), wxSVGFileDCImpl::NumStr(ey));
                    }
                    break;
                }
                case wxSVGPathSegment::CloseSegment:
                    d += wxS("Z ");
                    break;
            }
        }
        return d;
    }

private:
    void Extend(wxDouble x, wxDouble y)
    {
        if ( !m_hasPoints )
        {
            m_minX = m_maxX = x;
            m_minY = m_maxY = y;
            m_hasPoints = true;
            return;
        }
        if ( x < m_minX )
            m_minX = x;
        if ( x > m_maxX )
            m_maxX = x;
        if ( y < m_minY )
            m_minY = y;
        if ( y > m_maxY )
            m_maxY = y;
    }

    // Approximates a circular arc with a series of cubic Beziers.
    void AddArcToCubics(wxDouble xc, wxDouble yc, wxDouble r,
                        wxDouble startAngle, wxDouble endAngle, bool clockwise)
    {
        wxDouble sweep = endAngle - startAngle;
        if ( clockwise )
        {
            while (sweep < 0) sweep += 2 * M_PI;
            while (sweep > 2 * M_PI) sweep -= 2 * M_PI;
        }
        else
        {
            while (sweep > 0) sweep -= 2 * M_PI;
            while (sweep < -2 * M_PI) sweep += 2 * M_PI;
        }

        if ( std::abs(sweep) < 1e-9 )
            return;

        int n = std::ceil(std::abs(sweep) / (M_PI / 2.0));
        wxDouble angleStep = sweep / n;
        wxDouble alpha = angleStep / 2.0;
        wxDouble kappa = 4.0 / 3.0 * (1.0 - std::cos(alpha)) / std::sin(alpha);

        wxDouble a1 = startAngle;
        for ( int i = 0; i < n; ++i )
        {
            wxDouble a2 = a1 + angleStep;

            wxDouble cos_a1 = std::cos(a1);
            wxDouble sin_a1 = std::sin(a1);
            wxDouble cos_a2 = std::cos(a2);
            wxDouble sin_a2 = std::sin(a2);

            wxDouble cx1 = xc + r * (cos_a1 - kappa * sin_a1);
            wxDouble cy1 = yc + r * (sin_a1 + kappa * cos_a1);
            wxDouble cx2 = xc + r * (cos_a2 + kappa * sin_a2);
            wxDouble cy2 = yc + r * (sin_a2 - kappa * cos_a2);
            wxDouble x = xc + r * cos_a2;
            wxDouble y = yc + r * sin_a2;

            m_segments.push_back(wxSVGPathSegment::CurveTo(cx1, cy1, cx2, cy2, x, y));

            a1 = a2;
        }
    }

    std::vector<wxSVGPathSegment> m_segments;
    wxString m_d;
    wxDouble m_curX, m_curY;
    bool m_hasPoints;
    wxDouble m_minX, m_minY, m_maxX, m_maxY;
};

// ----------------------------------------------------------------------------
// wxSVGGraphicsPenData / BrushData / FontData
// ----------------------------------------------------------------------------

class wxSVGGraphicsPenData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsPenData(wxGraphicsRenderer* renderer,
                         const wxGraphicsPenInfo& info)
        : wxGraphicsObjectRefData(renderer), m_info(info)
    {
        m_pen.SetColour(info.GetColour());
        m_pen.SetWidth(static_cast<int>(info.GetWidth() + 0.5));
        m_pen.SetStyle(info.GetStyle());
    }

    const wxPen& GetPen() const { return m_pen; }
    const wxGraphicsPenInfo& GetInfo() const { return m_info; }

private:
    wxGraphicsPenInfo m_info;
    wxPen m_pen;
};

class wxSVGGraphicsBrushData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer, const wxBrush& brush)
        : wxGraphicsObjectRefData(renderer), m_brush(brush)
    {
    }

    const wxBrush& GetBrush() const { return m_brush; }

private:
    wxBrush m_brush;
};

class wxSVGGraphicsFontData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsFontData(wxGraphicsRenderer* renderer,
                          const wxFont& font, const wxColour& colour)
        : wxGraphicsObjectRefData(renderer), m_font(font), m_colour(colour)
    {
    }

    const wxFont& GetFont() const { return m_font; }
    const wxColour& GetColour() const { return m_colour; }

private:
    wxFont m_font;
    wxColour m_colour;
};

class wxSVGGraphicsBitmapData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsBitmapData(wxGraphicsRenderer* renderer, const wxBitmap& bmp)
        : wxGraphicsObjectRefData(renderer), m_bitmap(bmp)
    {
    }

    wxGraphicsObjectRefData* Clone() const override
    {
        return new wxSVGGraphicsBitmapData(GetRenderer(), m_bitmap);
    }

    const wxBitmap& GetBitmap() const { return m_bitmap; }

private:
    wxBitmap m_bitmap;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxSVGGraphicsRenderer
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxSVGGraphicsRenderer, wxGraphicsRenderer);

wxGraphicsRenderer* wxSVGGraphicsRenderer::Get()
{
    static wxSVGGraphicsRenderer s_renderer;
    return &s_renderer;
}

// These methods are not applicable to a text-based SVG renderer.
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContext(const wxWindowDC&) { return nullptr; }
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContext(const wxMemoryDC&) { return nullptr; }
#if wxUSE_PRINTING_ARCHITECTURE
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContext(const wxPrinterDC&) { return nullptr; }
#endif
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContext(const wxEnhMetaFileDC&) { return nullptr; }
#endif
#endif
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContextFromNativeContext(void*) { return nullptr; }
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContextFromNativeWindow(void*) { return nullptr; }
#ifdef __WXMSW__
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContextFromNativeHDC(WXHDC) { return nullptr; }
#endif
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContext(wxWindow*) { return nullptr; }
#if wxUSE_IMAGE
wxGraphicsContext* wxSVGGraphicsRenderer::CreateContextFromImage(wxImage&) { return nullptr; }
#endif
wxGraphicsContext* wxSVGGraphicsRenderer::CreateMeasuringContext() { return nullptr; }

wxGraphicsPath wxSVGGraphicsRenderer::CreatePath()
{
    wxGraphicsPath p;
    p.SetRefData(new wxSVGGraphicsPathData(this));
    return p;
}

wxGraphicsMatrix wxSVGGraphicsRenderer::CreateMatrix(wxDouble a, wxDouble b,
                                                     wxDouble c, wxDouble d,
                                                     wxDouble tx, wxDouble ty)
{
    wxGraphicsMatrix m;
    m.SetRefData(new wxSVGGraphicsMatrixData(this, a, b, c, d, tx, ty));
    return m;
}

wxGraphicsPen wxSVGGraphicsRenderer::CreatePen(const wxGraphicsPenInfo& info)
{
    if ( info.GetStyle() == wxPENSTYLE_TRANSPARENT )
        return wxNullGraphicsPen;

    wxGraphicsPen p;
    p.SetRefData(new wxSVGGraphicsPenData(this, info));
    return p;
}

wxGraphicsBrush wxSVGGraphicsRenderer::CreateBrush(const wxBrush& brush)
{
    if ( !brush.IsOk() || brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT )
        return wxNullGraphicsBrush;

    wxGraphicsBrush b;
    b.SetRefData(new wxSVGGraphicsBrushData(this, brush));
    return b;
}

wxGraphicsBrush
wxSVGGraphicsRenderer::CreateLinearGradientBrush(wxDouble, wxDouble, wxDouble, wxDouble,
                                                 const wxGraphicsGradientStops&,
                                                 const wxGraphicsMatrix&)
{
    /* TODO */
    return wxNullGraphicsBrush;
}

wxGraphicsBrush
wxSVGGraphicsRenderer::CreateRadialGradientBrush(wxDouble, wxDouble, wxDouble, wxDouble, wxDouble,
                                                 const wxGraphicsGradientStops&,
                                                 const wxGraphicsMatrix&)
{
    /* TODO */
    return wxNullGraphicsBrush;
}

wxGraphicsFont wxSVGGraphicsRenderer::CreateFont(const wxFont& font, const wxColour& col)
{
    if ( !font.IsOk() )
        return wxNullGraphicsFont;

    wxGraphicsFont f;
    f.SetRefData(new wxSVGGraphicsFontData(this, font, col));
    return f;
}

wxGraphicsFont wxSVGGraphicsRenderer::CreateFont(double sizeInPixels,
                                                 const wxString& facename,
                                                 int flags,
                                                 const wxColour& col)
{
    return CreateFont(
        wxFont(wxFontInfo(sizeInPixels).FaceName(facename).AllFlags(flags)), col);
}

wxGraphicsFont wxSVGGraphicsRenderer::CreateFontAtDPI(const wxFont& font,
                                                      const wxRealPoint&,
                                                      const wxColour& col)
{
    return CreateFont(font, col);
}

wxGraphicsBitmap wxSVGGraphicsRenderer::CreateBitmap(const wxBitmap& bmp)
{
    if ( !bmp.IsOk() )
        return wxNullGraphicsBitmap;

    wxGraphicsBitmap b;
    b.SetRefData(new wxSVGGraphicsBitmapData(this, bmp));
    return b;
}

#if wxUSE_IMAGE
wxGraphicsBitmap wxSVGGraphicsRenderer::CreateBitmapFromImage(const wxImage& image)
{
    if ( !image.IsOk() )
        return wxNullGraphicsBitmap;

    return CreateBitmap(wxBitmap(image));
}

wxImage wxSVGGraphicsRenderer::CreateImageFromBitmap(const wxGraphicsBitmap& bmp)
{
    auto* data = static_cast<wxSVGGraphicsBitmapData*>(bmp.GetRefData());
    if ( data != nullptr )
        return data->GetBitmap().ConvertToImage();
    return wxNullImage;
}
#endif

wxGraphicsBitmap wxSVGGraphicsRenderer::CreateBitmapFromNativeBitmap(void*)
{
    // Not applicable for SVG.
    return wxNullGraphicsBitmap;
}

wxGraphicsBitmap wxSVGGraphicsRenderer::CreateSubBitmap(const wxGraphicsBitmap& bmp,
                                                        wxDouble x, wxDouble y,
                                                        wxDouble w, wxDouble h)
{
    auto* data = static_cast<wxSVGGraphicsBitmapData*>(bmp.GetRefData());
    if ( data == nullptr )
        return wxNullGraphicsBitmap;

    wxBitmap sub = data->GetBitmap().GetSubBitmap(wxRect(wxRound(x), wxRound(y), wxRound(w), wxRound(h)));
    return CreateBitmap(sub);
}

wxString wxSVGGraphicsRenderer::GetName() const { return wxS("svg"); }

void wxSVGGraphicsRenderer::GetVersion(int* major, int* minor, int* micro) const
{
    if ( major != nullptr )
        *major = 1;
    if ( minor != nullptr )
        *minor = 0;
    if ( micro != nullptr )
        *micro = 0;
}

// ----------------------------------------------------------------------------
// wxSVGGraphicsContext
// ----------------------------------------------------------------------------

wxSVGGraphicsContext::wxSVGGraphicsContext(wxSVGFileDCImpl* impl)
    : wxGraphicsContext(wxSVGGraphicsRenderer::Get()),
      m_impl(impl)
{
    int w = 0, h = 0;
    impl->DoGetSize(&w, &h);
    m_width = w;
    m_height = h;

    m_currentPen = impl->m_pen;
    m_currentBrush = impl->m_brush;
    m_currentFont = impl->m_font;
    m_currentTextColour = impl->m_textForegroundColour;

    m_transform = GetRenderer()->CreateMatrix();
}

wxSVGGraphicsContext::~wxSVGGraphicsContext() = default;

void wxSVGGraphicsContext::PushState()
{
    m_stateStack.push(m_transform);
}

void wxSVGGraphicsContext::PopState()
{
    if ( m_stateStack.empty() )
        return;
    m_transform = m_stateStack.top();
    m_stateStack.pop();
}

void wxSVGGraphicsContext::Clip(const wxRegion& region)
{
    m_impl->DoSetDeviceClippingRegion(region);
}

void wxSVGGraphicsContext::Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    m_impl->DoSetClippingRegion(wxRound(x), wxRound(y), wxRound(w), wxRound(h));
}

void wxSVGGraphicsContext::ResetClip()
{
    m_impl->DestroyClippingRegion();
}

void wxSVGGraphicsContext::GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h)
{
    if ( x != nullptr )
        *x = 0;
    if ( y != nullptr )
        *y = 0;
    if ( w != nullptr )
        *w = m_width;
    if ( h != nullptr )
        *h = m_height;
}

void* wxSVGGraphicsContext::GetNativeContext()
{
    // A text-based SVG context has no native platform handle.
    return nullptr;
}

bool wxSVGGraphicsContext::SetAntialiasMode(wxAntialiasMode antialias)
{
    m_antialias = antialias;
    // Map wxAntialiasMode to SVG shape-rendering attribute.
    if ( antialias == wxANTIALIAS_NONE )
        m_impl->SetShapeRenderingMode(wxSVG_SHAPE_RENDERING_CRISP_EDGES);
    else
        m_impl->SetShapeRenderingMode(wxSVG_SHAPE_RENDERING_AUTO);
    return true;
}

bool wxSVGGraphicsContext::SetInterpolationQuality(wxInterpolationQuality interpolation)
{
    m_interpolation = interpolation;
    // Map wxInterpolationQuality to SVG shape-rendering attribute as a quality hint.
    switch ( interpolation )
    {
        case wxINTERPOLATION_FAST:
            m_impl->SetShapeRenderingMode(wxSVG_SHAPE_RENDERING_OPTIMIZE_SPEED);
            break;
        case wxINTERPOLATION_GOOD:
        case wxINTERPOLATION_BEST:
            m_impl->SetShapeRenderingMode(wxSVG_SHAPE_RENDERING_GEOMETRIC_PRECISION);
            break;
        case wxINTERPOLATION_NONE:
            m_impl->SetShapeRenderingMode(wxSVG_SHAPE_RENDERING_CRISP_EDGES);
            break;
        case wxINTERPOLATION_DEFAULT:
            m_impl->SetShapeRenderingMode(wxSVG_SHAPE_RENDERING_AUTO);
            break;
    }
    return true;
}

bool wxSVGGraphicsContext::SetCompositionMode(wxCompositionMode) { /* TODO */ return false; }

void wxSVGGraphicsContext::BeginLayer(wxDouble) { /* TODO */ }
void wxSVGGraphicsContext::EndLayer() { /* TODO */ }

void wxSVGGraphicsContext::Translate(wxDouble dx, wxDouble dy)
{
    m_transform.Translate(dx, dy);
}

void wxSVGGraphicsContext::Scale(wxDouble xScale, wxDouble yScale)
{
    m_transform.Scale(xScale, yScale);
}

void wxSVGGraphicsContext::Rotate(wxDouble angle)
{
    m_transform.Rotate(angle);
}

void wxSVGGraphicsContext::ConcatTransform(const wxGraphicsMatrix& matrix)
{
    m_transform.Concat(matrix);
}

void wxSVGGraphicsContext::SetTransform(const wxGraphicsMatrix& matrix)
{
    m_transform = matrix;
}

wxGraphicsMatrix wxSVGGraphicsContext::GetTransform() const
{
    return m_transform;
}

void wxSVGGraphicsContext::SetPen(const wxGraphicsPen& pen)
{
    wxGraphicsContext::SetPen(pen);
    if ( pen.IsNull() )
    {
        SyncPenToDC(*wxTRANSPARENT_PEN);
        return;
    }

    auto* data = static_cast<wxSVGGraphicsPenData*>(pen.GetRefData());
    if ( data != nullptr )
        SyncPenToDC(data->GetPen());
}

void wxSVGGraphicsContext::SetBrush(const wxGraphicsBrush& brush)
{
    wxGraphicsContext::SetBrush(brush);
    if ( brush.IsNull() )
    {
        SyncBrushToDC(*wxTRANSPARENT_BRUSH);
        return;
    }

    auto* data = static_cast<wxSVGGraphicsBrushData*>(brush.GetRefData());
    if ( data != nullptr )
        SyncBrushToDC(data->GetBrush());
}

void wxSVGGraphicsContext::SetFont(const wxGraphicsFont& font)
{
    wxGraphicsContext::SetFont(font);
    if ( font.IsNull() )
        return;

    auto* data = static_cast<wxSVGGraphicsFontData*>(font.GetRefData());
    if ( data == nullptr )
        return;

    m_currentFont = data->GetFont();
    m_currentTextColour = data->GetColour();
    m_impl->m_font = m_currentFont;
    m_impl->m_textForegroundColour = m_currentTextColour;
}

void wxSVGGraphicsContext::SyncPenToDC(const wxPen& pen)
{
    // Mirror the pen to the parent DC so its NewGraphicsIfNeeded() closes
    // and reopens the current <g> group before the next write.
    m_currentPen = pen;
    m_impl->m_pen = pen;
    m_impl->m_graphics_changed = true;
}

void wxSVGGraphicsContext::SyncBrushToDC(const wxBrush& brush)
{
    // Mirror the brush to the parent DC.
    m_currentBrush = brush;
    m_impl->m_brush = brush;
    m_impl->m_graphics_changed = true;
}

wxString wxSVGGraphicsContext::GetCurrentTransformAttr() const
{
    if ( m_transform.IsIdentity() )
        return wxString();

    wxDouble a, b, c, d, tx, ty;
    m_transform.Get(&a, &b, &c, &d, &tx, &ty);
    return wxString::Format(
        wxS(" transform=\"matrix(%s,%s,%s,%s,%s,%s)\""),
        wxSVGFileDCImpl::NumStr(a),
        wxSVGFileDCImpl::NumStr(b),
        wxSVGFileDCImpl::NumStr(c),
        wxSVGFileDCImpl::NumStr(d),
        wxSVGFileDCImpl::NumStr(tx),
        wxSVGFileDCImpl::NumStr(ty));
}

void wxSVGGraphicsContext::AccumulatePathBounds(wxGraphicsPathData* data)
{
    // Push the path's local-coordinate bounding box through the current
    // transform and feed the four corners to the parent DC's bounding box,
    // so wxSVGFileDC::MaxX/MaxY include GC-rendered geometry.
    if ( data == nullptr )
        return;

    wxDouble bx, by, bw, bh;
    data->GetBox(&bx, &by, &bw, &bh);

    wxDouble cx[4] = { bx, bx + bw, bx + bw, bx };
    wxDouble cy[4] = { by, by, by + bh, by + bh };

    for ( int i = 0; i < 4; ++i )
    {
        m_transform.TransformPoint(&cx[i], &cy[i]);
        m_impl->CalcBoundingBox(wxRound(cx[i]), wxRound(cy[i]));
    }
}

void wxSVGGraphicsContext::StrokePath(const wxGraphicsPath& path)
{
    if ( path.IsNull() || m_currentPen.GetStyle() == wxPENSTYLE_TRANSPARENT )
        return;

    auto* data = static_cast<wxSVGGraphicsPathData*>(path.GetRefData());
    if ( data == nullptr )
        return;

    m_impl->NewGraphicsIfNeeded();

    const wxString stroke = wxSVGFileDCImpl::GetPenStroke(m_currentPen.GetColour(),
                                                          m_currentPen.GetStyle());
    const wxString transform = GetCurrentTransformAttr();

    const wxString s = wxString::Format(
        wxS("  <path d=\"%s\" fill=\"none\" %s stroke-width=\"%d\"%s/>\n"),
        data->GetDString(), stroke, m_currentPen.GetWidth(), transform);

    m_impl->write(s);

    AccumulatePathBounds(data);
}

void wxSVGGraphicsContext::FillPath(const wxGraphicsPath& path, wxPolygonFillMode fillStyle)
{
    // Writes an SVG <path> element for the given path using the current brush.
    if ( path.IsNull() )
        return;

    auto* data = static_cast<wxSVGGraphicsPathData*>(path.GetRefData());
    if ( data == nullptr )
        return;

    m_impl->NewGraphicsIfNeeded();

    const wxString fill = m_currentBrush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT
        ? wxString(wxS("fill=\"none\""))
        : wxSVGFileDCImpl::GetBrushFill(m_currentBrush.GetColour(),
                                        m_currentBrush.GetStyle());
    const wxString transform = GetCurrentTransformAttr();
    const wxString rule = (fillStyle == wxODDEVEN_RULE)
        ? wxS("evenodd") : wxS("nonzero");

    const wxString s = wxString::Format(
        wxS("  <path d=\"%s\" %s fill-rule=\"%s\" stroke=\"none\"%s/>\n"),
        data->GetDString(), fill, rule, transform);

    m_impl->write(s);

    AccumulatePathBounds(data);
}

void wxSVGGraphicsContext::GetTextExtent(const wxString& text,
                                         wxDouble* width, wxDouble* height,
                                         wxDouble* descent,
                                         wxDouble* externalLeading) const
{
    wxCoord w = 0, h = 0, d = 0, e = 0;
    m_impl->DoGetTextExtent(text, &w, &h, &d, &e, &m_currentFont);
    if ( width != nullptr )
        *width = w;
    if ( height != nullptr )
        *height = h;
    if ( descent != nullptr )
        *descent = d;
    if ( externalLeading != nullptr )
        *externalLeading = e;
}

void wxSVGGraphicsContext::GetPartialTextExtents(const wxString& text,
                                                 wxArrayDouble& widths) const
{
    widths.clear();
    widths.reserve(text.length());
    for (size_t i = 1; i <= text.length(); ++i)
    {
        wxCoord w = 0, h = 0;
        m_impl->DoGetTextExtent(text.substr(0, i), &w, &h, nullptr, nullptr, &m_currentFont);
        widths.push_back(w);
    }
}

void wxSVGGraphicsContext::DrawBitmap(const wxGraphicsBitmap& bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    auto* data = static_cast<wxSVGGraphicsBitmapData*>(bmp.GetRefData());
    if ( data != nullptr )
    {
        wxBitmap b = data->GetBitmap();
        if ( w != b.GetWidth() || h != b.GetHeight() )
        {
            wxImage img = b.ConvertToImage();
            img.Rescale(wxRound(w), wxRound(h));
            b = wxBitmap(img);
        }
        DrawBitmap(b, x, y, w, h);
    }
}

void wxSVGGraphicsContext::DrawBitmap(const wxBitmap& bmp,
                                      wxDouble x, wxDouble y,
                                      wxDouble WXUNUSED(w), wxDouble WXUNUSED(h))
{
    // Delegate to the DC; its DoDrawBitmap() will call NewGraphicsIfNeeded().
    m_impl->GetOwner()->DrawBitmap(bmp, wxCoord(x), wxCoord(y), false);
}

void wxSVGGraphicsContext::DrawIcon(const wxIcon& icon,
                                    wxDouble x, wxDouble y,
                                    wxDouble WXUNUSED(w), wxDouble WXUNUSED(h))
{
    m_impl->GetOwner()->DrawIcon(icon, wxCoord(x), wxCoord(y));
}

#ifdef __WXMSW__
// Not analogous to SVG output.
WXHDC wxSVGGraphicsContext::GetNativeHDC() { return WXHDC(0); }
void wxSVGGraphicsContext::ReleaseNativeHDC(WXHDC) {}
#endif

void wxSVGGraphicsContext::DoDrawText(const wxString& str, wxDouble x, wxDouble y)
{
    m_impl->GetOwner()->DrawText(str, wxCoord(x), wxCoord(y));
}

#endif // wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT
