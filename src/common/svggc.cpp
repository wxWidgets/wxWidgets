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
#include "wx/private/svg.h"
#include "wx/private/svggc.h"

#include <cmath>
#include <vector>

// ----------------------------------------------------------------------------
// wxSVGPathSegment
// ----------------------------------------------------------------------------

wxSVGPathSegment wxSVGPathSegment::MoveTo(wxDouble x, wxDouble y)
{
    wxSVGPathSegment s;
    s.type = MoveSegment;
    s.x = x; s.y = y;
    return s;
}

wxSVGPathSegment wxSVGPathSegment::LineTo(wxDouble x, wxDouble y)
{
    wxSVGPathSegment s;
    s.type = LineSegment;
    s.x = x;
    s.y = y;
    return s;
}

wxSVGPathSegment wxSVGPathSegment::CurveTo(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, wxDouble x, wxDouble y)
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

wxSVGPathSegment wxSVGPathSegment::QuadCurveTo(wxDouble cx, wxDouble cy, wxDouble x, wxDouble y)
{
    wxSVGPathSegment s;
    s.type = QuadCurveSegment;
    s.x1 = cx;
    s.y1 = cy;
    s.x = x;
    s.y = y;
    return s;
}

wxSVGPathSegment wxSVGPathSegment::Arc(wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise)
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

wxSVGPathSegment wxSVGPathSegment::Close()
{
    wxSVGPathSegment s;
    s.type = CloseSegment;
    return s;
}

// ----------------------------------------------------------------------------
// wxSVGGraphicsMatrixData - standard 2D affine transformation matrix
// ----------------------------------------------------------------------------

wxSVGGraphicsMatrixData::wxSVGGraphicsMatrixData(wxGraphicsRenderer* renderer,
                                                 wxDouble a, wxDouble b,
                                                 wxDouble c, wxDouble d,
                                                 wxDouble tx, wxDouble ty)
    : wxGraphicsMatrixData(renderer),
      m_a(a), m_b(b), m_c(c), m_d(d), m_tx(tx), m_ty(ty)
{
}

wxGraphicsObjectRefData* wxSVGGraphicsMatrixData::Clone() const
{
    return new wxSVGGraphicsMatrixData(GetRenderer(), m_a, m_b, m_c, m_d, m_tx, m_ty);
}

void wxSVGGraphicsMatrixData::Concat(const wxGraphicsMatrixData* t)
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

void wxSVGGraphicsMatrixData::Set(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                                 wxDouble tx, wxDouble ty)
{
    m_a = a; m_b = b; m_c = c; m_d = d; m_tx = tx; m_ty = ty;
}

void wxSVGGraphicsMatrixData::Get(wxDouble* a, wxDouble* b, wxDouble* c,
                                 wxDouble* d, wxDouble* tx, wxDouble* ty) const
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

void wxSVGGraphicsMatrixData::Invert()
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

bool wxSVGGraphicsMatrixData::IsEqual(const wxGraphicsMatrixData* t) const
{
    const auto* o = static_cast<const wxSVGGraphicsMatrixData*>(t);
    return m_a == o->m_a && m_b == o->m_b && m_c == o->m_c
        && m_d == o->m_d && m_tx == o->m_tx && m_ty == o->m_ty;
}

bool wxSVGGraphicsMatrixData::IsIdentity() const
{
    return m_a == 1.0 && m_b == 0.0 && m_c == 0.0 && m_d == 1.0
        && m_tx == 0.0 && m_ty == 0.0;
}

void wxSVGGraphicsMatrixData::Translate(wxDouble dx, wxDouble dy)
{
    m_tx += dx * m_a + dy * m_c;
    m_ty += dx * m_b + dy * m_d;
}

void wxSVGGraphicsMatrixData::Scale(wxDouble xScale, wxDouble yScale)
{
    m_a *= xScale;
    m_b *= xScale;
    m_c *= yScale;
    m_d *= yScale;
}

void wxSVGGraphicsMatrixData::Rotate(wxDouble angle)
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

void wxSVGGraphicsMatrixData::TransformPoint(wxDouble* x, wxDouble* y) const
{
    const wxDouble xx = *x * m_a + *y * m_c + m_tx;
    const wxDouble yy = *x * m_b + *y * m_d + m_ty;
    *x = xx;
    *y = yy;
}

void wxSVGGraphicsMatrixData::TransformDistance(wxDouble* dx, wxDouble* dy) const
{
    const wxDouble xx = *dx * m_a + *dy * m_c;
    const wxDouble yy = *dx * m_b + *dy * m_d;
    *dx = xx;
    *dy = yy;
}

// ----------------------------------------------------------------------------
// wxSVGGraphicsPathData - records path commands for deferred SVG generation
// ----------------------------------------------------------------------------

wxSVGGraphicsPathData::wxSVGGraphicsPathData(wxGraphicsRenderer* renderer)
    : wxGraphicsPathData(renderer),
      m_curX(0), m_curY(0),
      m_hasPoints(false),
      m_minX(0), m_minY(0), m_maxX(0), m_maxY(0)
{
}

wxGraphicsObjectRefData* wxSVGGraphicsPathData::Clone() const
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

void wxSVGGraphicsPathData::MoveToPoint(wxDouble x, wxDouble y)
{
    m_segments.push_back(wxSVGPathSegment::MoveTo(x, y));
    m_curX = x; m_curY = y;
    Extend(x, y);
}

void wxSVGGraphicsPathData::AddLineToPoint(wxDouble x, wxDouble y)
{
    m_segments.push_back(wxSVGPathSegment::LineTo(x, y));
    m_curX = x; m_curY = y;
    Extend(x, y);
}

void wxSVGGraphicsPathData::AddCurveToPoint(wxDouble cx1, wxDouble cy1,
                                           wxDouble cx2, wxDouble cy2,
                                           wxDouble x, wxDouble y)
{
    m_segments.push_back(wxSVGPathSegment::CurveTo(cx1, cy1, cx2, cy2, x, y));
    m_curX = x; m_curY = y;
    Extend(cx1, cy1);
    Extend(cx2, cy2);
    Extend(x, y);
}

void wxSVGGraphicsPathData::AddQuadCurveToPoint(wxDouble cx, wxDouble cy,
                                               wxDouble x, wxDouble y)
{
    m_segments.push_back(wxSVGPathSegment::QuadCurveTo(cx, cy, x, y));
    m_curX = x; m_curY = y;
    Extend(cx, cy);
    Extend(x, y);
}

void wxSVGGraphicsPathData::AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    wxGraphicsPathData::AddRectangle(x, y, w, h);
}

void wxSVGGraphicsPathData::AddCircle(wxDouble x, wxDouble y, wxDouble r)
{
    // A full circle is drawn as two arcs to avoid the SVG arc command's ambiguity
    // with 360-degree sweeps.
    MoveToPoint(x + r, y);
    AddArc(x, y, r, 0.0, M_PI, true);
    AddArc(x, y, r, M_PI, 2.0 * M_PI, true);
    CloseSubpath();
}

void wxSVGGraphicsPathData::AddArcToPoint(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, wxDouble r)
{
    wxGraphicsPathData::AddArcToPoint(x1, y1, x2, y2, r);
}

void wxSVGGraphicsPathData::AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    wxGraphicsPathData::AddEllipse(x, y, w, h);
}

void wxSVGGraphicsPathData::AddRoundedRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius)
{
    wxGraphicsPathData::AddRoundedRectangle(x, y, w, h, radius);
}

void wxSVGGraphicsPathData::AddPath(const wxGraphicsPathData* path)
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

void wxSVGGraphicsPathData::CloseSubpath()
{
    m_segments.push_back(wxSVGPathSegment::Close());
}

void wxSVGGraphicsPathData::GetCurrentPoint(wxDouble* x, wxDouble* y) const
{
    if ( x != nullptr )
        *x = m_curX;
    if ( y != nullptr )
        *y = m_curY;
}

void wxSVGGraphicsPathData::AddArc(wxDouble x, wxDouble y, wxDouble r,
                                  wxDouble startAngle, wxDouble endAngle, bool clockwise)
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

void* wxSVGGraphicsPathData::GetNativePath() const
{
    // Regenerate the "d" string from segments on demand.
    const_cast<wxSVGGraphicsPathData*>(this)->m_d = GetDString();
    return const_cast<wxString*>(&m_d);
}

void wxSVGGraphicsPathData::Transform(const wxGraphicsMatrixData* matrix)
{
    if ( matrix == nullptr )
        return;

    const auto* m = static_cast<const wxSVGGraphicsMatrixData*>(matrix);
    std::vector<wxSVGPathSegment> oldSegments = m_segments;
    m_segments.clear();
    m_hasPoints = false;

    // Arcs must be converted to cubic Béziers before transformation
    // because a transformed elliptical arc is generally a generic conic.
    for ( const auto& s : oldSegments )
    {
        if ( s.type == wxSVGPathSegment::ArcSegment )
        {
            const wxDouble sx = s.x + s.r * std::cos(s.startAngle);
            const wxDouble sy = s.y + s.r * std::sin(s.startAngle);
            if ( !m_hasPoints )
                MoveToPoint(sx, sy);
            else
                AddLineToPoint(sx, sy);

            AddArcToCubics(s.x, s.y, s.r, s.startAngle, s.endAngle, s.clockwise);
        }
        else
        {
            m_segments.push_back(s);
            if ( s.type != wxSVGPathSegment::CloseSegment )
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
            case wxSVGPathSegment::QuadCurveSegment:
                m->TransformPoint(&s.x1, &s.y1);
                m->TransformPoint(&s.x, &s.y);
                Extend(s.x1, s.y1);
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

void wxSVGGraphicsPathData::GetBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) const
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

bool wxSVGGraphicsPathData::Contains(wxDouble x, wxDouble y,
                                     wxPolygonFillMode fillStyle) const
{
    if ( m_segments.empty() )
        return false;

    // Fast bounding box rejection.
    wxDouble bx, by, bw, bh;
    GetBox(&bx, &by, &bw, &bh);
    if ( x < bx || x > bx + bw || y < by || y > by + bh )
        return false;

    int windingNumber = 0;
    wxPoint2DDouble curPt, startPt;
    bool hasStartPt = false;

    // Helper to process a single line segment for the winding number algorithm.
    const auto processEdge = [&](const wxPoint2DDouble& p1, const wxPoint2DDouble& p2)
    {
        if ( p1.m_y <= y )
        {
            if ( p2.m_y > y && (p2.m_x - p1.m_x) * (y - p1.m_y) - (x - p1.m_x) * (p2.m_y - p1.m_y) > 0 )
                windingNumber++;
        }
        else
        {
            if ( p2.m_y <= y && (p2.m_x - p1.m_x) * (y - p1.m_y) - (x - p1.m_x) * (p2.m_y - p1.m_y) < 0 )
                windingNumber--;
        }
    };

    for ( const auto& seg : m_segments )
    {
        switch ( seg.type )
        {
            case wxSVGPathSegment::MoveSegment:
                if ( hasStartPt )
                    processEdge(curPt, startPt);
                curPt = { seg.x, seg.y };
                startPt = curPt;
                hasStartPt = true;
                break;

            case wxSVGPathSegment::LineSegment:
                processEdge(curPt, { seg.x, seg.y });
                curPt = { seg.x, seg.y };
                break;

            case wxSVGPathSegment::CurveSegment:
            {
                const wxPoint2DDouble p0 = curPt;
                const wxPoint2DDouble p1 = { seg.x1, seg.y1 };
                const wxPoint2DDouble p2 = { seg.x2, seg.y2 };
                const wxPoint2DDouble p3 = { seg.x, seg.y };
                constexpr int steps = 10;
                for ( int i = 1; i <= steps; ++i )
                {
                    const double t = i / (double)steps;
                    const double tInv = 1.0 - t;
                    const wxPoint2DDouble next = {
                        tInv * tInv * tInv * p0.m_x + 3 * tInv * tInv * t * p1.m_x + 3 * tInv * t * t * p2.m_x + t * t * t * p3.m_x,
                        tInv * tInv * tInv * p0.m_y + 3 * tInv * tInv * t * p1.m_y + 3 * tInv * t * t * p2.m_y + t * t * t * p3.m_y
                    };
                    processEdge(curPt, next);
                    curPt = next;
                }
                break;
            }

            case wxSVGPathSegment::QuadCurveSegment:
            {
                const wxPoint2DDouble p0 = curPt;
                const wxPoint2DDouble p1 = { seg.x1, seg.y1 };
                const wxPoint2DDouble p2 = { seg.x, seg.y };
                constexpr int steps = 10;
                for ( int i = 1; i <= steps; ++i )
                {
                    const double t = i / static_cast<double>(steps);
                    const double tInv = 1.0 - t;
                    const wxPoint2DDouble next = {
                        tInv * tInv * p0.m_x + 2 * tInv * t * p1.m_x + t * t * p2.m_x,
                        tInv * tInv * p0.m_y + 2 * tInv * t * p1.m_y + t * t * p2.m_y
                    };
                    processEdge(curPt, next);
                    curPt = next;
                }
                break;
            }

            case wxSVGPathSegment::ArcSegment:
            {
                wxDouble sweep = seg.endAngle - seg.startAngle;
                constexpr wxDouble twoPi = 2.0 * M_PI;
                if ( std::abs(sweep) >= twoPi - 1e-9 )
                {
                    sweep = seg.clockwise ? twoPi : -twoPi;
                }
                else
                {
                    if ( seg.clockwise )
                    {
                        while ( sweep <= 0 ) sweep += twoPi;
                        while ( sweep > twoPi ) sweep -= twoPi;
                    }
                    else
                    {
                        while ( sweep >= 0 ) sweep -= twoPi;
                        while ( sweep < -twoPi ) sweep += twoPi;
                    }
                }

                const int steps = std::max(1, (int)std::ceil(std::abs(sweep) / (M_PI / 10.0)));
                const double angleStep = sweep / steps;
                for ( int i = 1; i <= steps; ++i )
                {
                    const double ang = seg.startAngle + i * angleStep;
                    const wxPoint2DDouble next = { seg.x + seg.r * std::cos(ang), seg.y + seg.r * std::sin(ang) };
                    processEdge(curPt, next);
                    curPt = next;
                }
                break;
            }

            case wxSVGPathSegment::CloseSegment:
                if ( hasStartPt )
                {
                    processEdge(curPt, startPt);
                    hasStartPt = false;
                }
                break;
        }
    }

    if ( hasStartPt )
        processEdge(curPt, startPt);

    if ( fillStyle == wxODDEVEN_RULE )
        return (windingNumber % 2) != 0;
    return windingNumber != 0;
}

// Generates the SVG "d" attribute string from recorded segments.
wxString wxSVGGraphicsPathData::GetDString() const
{
    wxString d;
    for ( const auto& s : m_segments )
    {
        switch ( s.type )
        {
            case wxSVGPathSegment::MoveSegment:
                d += wxString::Format(wxS("M %s %s "),
                    wxSVG::NumStr(s.x), wxSVG::NumStr(s.y));
                break;
            case wxSVGPathSegment::LineSegment:
                d += wxString::Format(wxS("L %s %s "),
                    wxSVG::NumStr(s.x), wxSVG::NumStr(s.y));
                break;
            case wxSVGPathSegment::CurveSegment:
                d += wxString::Format(wxS("C %s %s %s %s %s %s "),
                    wxSVG::NumStr(s.x1), wxSVG::NumStr(s.y1),
                    wxSVG::NumStr(s.x2), wxSVG::NumStr(s.y2),
                    wxSVG::NumStr(s.x), wxSVG::NumStr(s.y));
                break;
            case wxSVGPathSegment::QuadCurveSegment:
                d += wxString::Format(wxS("Q %s %s %s %s "),
                    wxSVG::NumStr(s.x1), wxSVG::NumStr(s.y1),
                    wxSVG::NumStr(s.x), wxSVG::NumStr(s.y));
                break;
            case wxSVGPathSegment::ArcSegment:
            {
                wxDouble sweep = s.endAngle - s.startAngle;
                constexpr wxDouble twoPi = 2 * M_PI;
                if ( std::abs(sweep) >= twoPi - 1e-9 )
                {
                    sweep = twoPi;
                }
                else
                {
                    if ( s.clockwise )
                    {
                        while ( sweep <= 0 )
                            sweep += twoPi;
                        while ( sweep > twoPi )
                            sweep -= twoPi;
                    }
                    else
                    {
                        while ( sweep >= 0 )
                            sweep -= twoPi;
                        while ( sweep < -twoPi )
                            sweep += twoPi;
                        sweep = -sweep;
                    }
                }

                if ( sweep >= twoPi - 1e-9 )
                {
                    const wxDouble ox = s.x - s.r, oy = s.y;
                    d += wxString::Format(wxS("A %s %s 0 1 %d %s %s "),
                        wxSVG::NumStr(s.r), wxSVG::NumStr(s.r),
                        s.clockwise ? 1 : 0,
                        wxSVG::NumStr(s.x + s.r), wxSVG::NumStr(s.y));
                    d += wxString::Format(wxS("A %s %s 0 1 %d %s %s "),
                        wxSVG::NumStr(s.r), wxSVG::NumStr(s.r),
                        s.clockwise ? 1 : 0,
                        wxSVG::NumStr(ox), wxSVG::NumStr(oy));
                }
                else
                {
                    const wxDouble ex = s.x + s.r * std::cos(s.endAngle);
                    const wxDouble ey = s.y + s.r * std::sin(s.endAngle);
                    const int large = sweep > M_PI ? 1 : 0;
                    d += wxString::Format(wxS("A %s %s 0 %d %d %s %s "),
                        wxSVG::NumStr(s.r), wxSVG::NumStr(s.r),
                        large, s.clockwise ? 1 : 0,
                        wxSVG::NumStr(ex), wxSVG::NumStr(ey));
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

void wxSVGGraphicsPathData::Extend(wxDouble x, wxDouble y)
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

// Approximates a circular arc with a series of cubic Béziers.
void wxSVGGraphicsPathData::AddArcToCubics(wxDouble xc, wxDouble yc, wxDouble r,
                                          wxDouble startAngle, wxDouble endAngle, bool clockwise)
{
    wxDouble sweep = endAngle - startAngle;
    constexpr wxDouble twoPi = 2.0 * M_PI;
    if ( std::abs(sweep) >= twoPi - 1e-9 )
    {
        sweep = clockwise ? twoPi : -twoPi;
    }
    else
    {
        if ( clockwise )
        {
            while ( sweep <= 0 ) sweep += twoPi;
            while ( sweep > twoPi ) sweep -= twoPi;
        }
        else
        {
            while ( sweep >= 0 ) sweep -= twoPi;
            while ( sweep < -twoPi ) sweep += twoPi;
        }
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

// ----------------------------------------------------------------------------
// wxSVGGraphicsPenData / BrushData / FontData / BitmapData
// ----------------------------------------------------------------------------

wxSVGGraphicsPenData::wxSVGGraphicsPenData(wxGraphicsRenderer* renderer,
                                           const wxGraphicsPenInfo& info)
    : wxGraphicsObjectRefData(renderer), m_info(info)
{
    m_pen.SetColour(info.GetColour());
    m_pen.SetWidth(static_cast<int>(info.GetWidth() + 0.5));
    m_pen.SetStyle(info.GetStyle());
}

wxSVGGraphicsBrushData::wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer, const wxBrush& brush)
    : wxGraphicsObjectRefData(renderer), m_brush(brush)
{
}

wxSVGGraphicsBrushData::wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer,
                                               wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                                               const wxGraphicsGradientStops& stops,
                                               const wxGraphicsMatrix& matrix)
    : wxGraphicsObjectRefData(renderer),
      m_isGradient(true),
      m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2),
      m_stops(stops), m_matrix(matrix)
{
}

wxSVGGraphicsBrushData::wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer,
                                               wxDouble startX, wxDouble startY, wxDouble endX, wxDouble endY,
                                               wxDouble radius,
                                               const wxGraphicsGradientStops& stops,
                                               const wxGraphicsMatrix& matrix)
    : wxGraphicsObjectRefData(renderer),
      m_isGradient(true), m_isRadial(true),
      m_x1(startX), m_y1(startY), m_x2(endX), m_y2(endY), m_radius(radius),
      m_stops(stops), m_matrix(matrix)
{
}

wxSVGGraphicsFontData::wxSVGGraphicsFontData(wxGraphicsRenderer* renderer,
                                             const wxFont& font, const wxColour& colour)
    : wxGraphicsObjectRefData(renderer), m_font(font), m_colour(colour)
{
}

wxSVGGraphicsBitmapData::wxSVGGraphicsBitmapData(wxGraphicsRenderer* renderer, const wxBitmap& bmp)
    : wxGraphicsObjectRefData(renderer), m_bitmap(bmp)
{
}

wxGraphicsObjectRefData* wxSVGGraphicsBitmapData::Clone() const
{
    return new wxSVGGraphicsBitmapData(GetRenderer(), m_bitmap);
}

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

wxGraphicsContext* wxSVGGraphicsRenderer::CreateContext(wxSVGFileDC& dc)
{
    return wxSVGGraphicsContext::Create(dc);
}

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
wxSVGGraphicsRenderer::CreateLinearGradientBrush(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                                                 const wxGraphicsGradientStops& stops,
                                                 const wxGraphicsMatrix& matrix)
{
    wxGraphicsBrush b;
    b.SetRefData(new wxSVGGraphicsBrushData(this, x1, y1, x2, y2, stops, matrix));
    return b;
}

wxGraphicsBrush
wxSVGGraphicsRenderer::CreateRadialGradientBrush(wxDouble startX, wxDouble startY, wxDouble endX, wxDouble endY,
                                                 wxDouble radius,
                                                 const wxGraphicsGradientStops& stops,
                                                 const wxGraphicsMatrix& matrix)
{
    wxGraphicsBrush b;
    b.SetRefData(new wxSVGGraphicsBrushData(this, startX, startY, endX, endY, radius, stops, matrix));
    return b;
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
      m_impl(impl),
      m_writer(impl->m_writer.get())
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

wxSVGGraphicsContext::wxSVGGraphicsContext(wxSVGFileDC& dc)
    : wxSVGGraphicsContext(static_cast<wxSVGFileDCImpl*>(dc.GetImpl()))
{
}

wxGraphicsContext* wxSVGGraphicsContext::Create(wxSVGFileDC& dc)
{
    return new wxSVGGraphicsContext(dc);
}

wxSVGGraphicsContext::~wxSVGGraphicsContext() = default;

void wxSVGGraphicsContext::PushState()
{
    m_stateStack.push({ m_transform, m_writer->GetClipNestingLevel(), m_composition });
}

void wxSVGGraphicsContext::PopState()
{
    if ( m_stateStack.empty() )
        return;

    const State& s = m_stateStack.top();
    m_transform = s.transform;
    SyncTransformToDC();

    if ( m_composition != s.compositionMode )
    {
        m_composition = s.compositionMode;
        m_writer->SetCompositionMode(m_composition);
    }

    if ( m_writer->GetClipNestingLevel() > s.clipNestingLevel )
    {
        // Close the clipping groups that were opened since PushState.
        m_writer->Write(wxS("</g>\n")); // Close the attribute group
        auto level = m_writer->GetClipNestingLevel();
        while ( level > s.clipNestingLevel )
        {
            m_writer->Write(wxS("</g>\n"));
            --level;
        }
        m_writer->SetClipNestingLevel(level);
        // Restart the attribute group
        m_impl->DoStartNewGraphics();
    }

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
    wxRect r;
    if ( m_impl->DoGetClippingRect(r) )
    {
        if ( x != nullptr )
            *x = r.x;
        if ( y != nullptr )
            *y = r.y;
        if ( w != nullptr )
            *w = r.width;
        if ( h != nullptr )
            *h = r.height;
    }
    else
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

bool wxSVGGraphicsContext::SetCompositionMode(wxCompositionMode op)
{
    if ( op == wxCOMPOSITION_ADD || op == wxCOMPOSITION_DIFF || op == wxCOMPOSITION_OVER )
    {
        m_writer->SetCompositionMode(op);
        m_composition = op;
        return true;
    }
    return false;
}

void wxSVGGraphicsContext::BeginLayer(wxDouble opacity)
{
    m_impl->BeginLayer(opacity);
}

void wxSVGGraphicsContext::EndLayer()
{
    m_impl->EndLayer();
}

void wxSVGGraphicsContext::Translate(wxDouble dx, wxDouble dy)
{
    m_transform.Translate(dx, dy);
    SyncTransformToDC();
}

void wxSVGGraphicsContext::Scale(wxDouble xScale, wxDouble yScale)
{
    m_transform.Scale(xScale, yScale);
    SyncTransformToDC();
}

void wxSVGGraphicsContext::Rotate(wxDouble angle)
{
    m_transform.Rotate(angle);
    SyncTransformToDC();
}

void wxSVGGraphicsContext::ConcatTransform(const wxGraphicsMatrix& matrix)
{
    m_transform.Concat(matrix);
    SyncTransformToDC();
}

void wxSVGGraphicsContext::SetTransform(const wxGraphicsMatrix& matrix)
{
    m_transform = matrix;
    SyncTransformToDC();
}

wxGraphicsMatrix wxSVGGraphicsContext::GetTransform() const
{
    return m_transform;
}

void wxSVGGraphicsContext::SetPen(const wxGraphicsPen& pen)
{
    wxGraphicsContext::SetPen(pen);
    m_writer->SetGraphicsPen(pen);
    m_writer->MarkGraphicsChanged();

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
    m_writer->SetGraphicsBrush(brush);
    m_writer->MarkGraphicsChanged();

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
    m_writer->MarkGraphicsChanged();
}

void wxSVGGraphicsContext::SyncBrushToDC(const wxBrush& brush)
{
    // Mirror the brush to the parent DC.
    m_currentBrush = brush;
    m_impl->m_brush = brush;
    m_writer->MarkGraphicsChanged();
}

void wxSVGGraphicsContext::SyncTransformToDC()
{
    m_writer->SetGCTransform(GetCurrentTransformAttr());
}

wxString wxSVGGraphicsContext::GetCurrentTransformAttr() const
{
    if ( m_transform.IsNull() || m_transform.IsIdentity() )
        return wxString();

    wxDouble a, b, c, d, tx, ty;
    m_transform.Get(&a, &b, &c, &d, &tx, &ty);
    return wxString::Format(
        wxS(" transform=\"matrix(%s,%s,%s,%s,%s,%s)\""),
        wxSVG::NumStr(a),
        wxSVG::NumStr(b),
        wxSVG::NumStr(c),
        wxSVG::NumStr(d),
        wxSVG::NumStr(tx),
        wxSVG::NumStr(ty));
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
        if ( !m_transform.IsNull() )
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

    wxString stroke;
    if ( !m_pen.IsNull() )
        stroke = m_writer->WriteGraphicsPenStroke(m_pen);

    if ( stroke.empty() )
    {
        stroke = wxSVG::GetPenStroke(m_currentPen.GetColour(),
                                     m_currentPen.GetStyle());
    }

    const wxString transform = GetCurrentTransformAttr();

    const wxString s = wxString::Format(
        wxS("  <path d=\"%s\" fill=\"none\" %s stroke-width=\"%d\"%s/>\n"),
        data->GetDString(), stroke, m_currentPen.GetWidth(), transform);

    m_writer->Write(s);

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

    wxString fill;
    if ( !m_brush.IsNull() )
        fill = m_writer->WriteGraphicsBrushFill(m_brush);

    if ( fill.empty() )
    {
        fill = m_currentBrush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT
            ? wxString(wxS("fill=\"none\""))
            : wxSVG::GetBrushFill(m_currentBrush.GetColour(),
                                  m_currentBrush.GetStyle());
    }

    const wxString transform = GetCurrentTransformAttr();
    const wxString rule = (fillStyle == wxODDEVEN_RULE)
        ? wxS("evenodd") : wxS("nonzero");

    const wxString s = wxString::Format(
        wxS("  <path d=\"%s\" %s fill-rule=\"%s\" stroke=\"none\"%s/>\n"),
        data->GetDString(), fill, rule, transform);

    m_writer->Write(s);

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
    for ( size_t i = 1; i <= text.length(); ++i )
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
