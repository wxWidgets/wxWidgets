///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/graphmatrix.cpp
// Purpose:     Graphics matrix unit test
// Author:      Artur Wieczorek
// Created:     2016-09-18
// Copyright:   (c) 2016 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#if wxUSE_GRAPHICS_CONTEXT

#include "wx/graphics.h"
#include "wx/dcmemory.h"

#include <memory>

static void InitState(wxGraphicsContext* gc);
static void InvertMatrix(wxGraphicsContext* gc);
static void Concat1(wxGraphicsContext* gc);
static void Concat2(wxGraphicsContext* gc);
static void Concat3(wxGraphicsContext* gc);

TEST_CASE("GraphicsMatrixTestCase::DefaultRenderer", "[graphmatrix][default]")
{
    wxBitmap bmp(100, 100);
    wxMemoryDC dc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDefaultRenderer();
    REQUIRE(rend);
    std::unique_ptr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());

    SECTION("InitState")
    {
        InitState(gc.get());
    }

    SECTION("InvertMatrix")
    {
        InvertMatrix(gc.get());
    }

    SECTION("Concat1")
    {
        Concat1(gc.get());
    }

    SECTION("Concat2")
    {
        Concat2(gc.get());
    }

    SECTION("Concat3")
    {
        Concat3(gc.get());
    }
}

#ifdef __WXMSW__

#if wxUSE_GRAPHICS_GDIPLUS
TEST_CASE("GraphicsMatrixTestCase::GDIPlusRenderer", "[graphmatrix][gdiplus]")
{
    wxBitmap bmp(100, 100);
    wxMemoryDC dc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    REQUIRE(rend);
    std::unique_ptr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());

    SECTION("InitState")
    {
        InitState(gc.get());
    }

    SECTION("InvertMatrix")
    {
        InvertMatrix(gc.get());
    }

    SECTION("Concat1")
    {
        Concat1(gc.get());
    }

    SECTION("Concat2")
    {
        Concat2(gc.get());
    }

    SECTION("Concat3")
    {
        Concat3(gc.get());
    }
}
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
TEST_CASE("GraphicsMatrixTestCase::Direct2DRenderer", "[graphmatrix][direct2d]")
{
    wxBitmap bmp(100, 100);
    wxMemoryDC dc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetDirect2DRenderer();
    REQUIRE(rend);
    std::unique_ptr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());

    SECTION("InitState")
    {
        InitState(gc.get());
    }

    SECTION("InvertMatrix")
    {
        InvertMatrix(gc.get());
    }

    SECTION("Concat1")
    {
        Concat1(gc.get());
    }

    SECTION("Concat2")
    {
        Concat2(gc.get());
    }

    SECTION("Concat3")
    {
        Concat3(gc.get());
    }
}
#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // __WXMSW__

#if wxUSE_CAIRO
TEST_CASE("GraphicsMatrixTestCase::CairoRenderer", "[graphmatrix][cairo]")
{
    wxBitmap bmp(100, 100);
    wxMemoryDC dc(bmp);
    wxGraphicsRenderer* rend = wxGraphicsRenderer::GetCairoRenderer();
    REQUIRE(rend);
    std::unique_ptr<wxGraphicsContext> gc(rend->CreateContext(dc));
    REQUIRE(gc.get());

    SECTION("InitState")
    {
        InitState(gc.get());
    }

    SECTION("InvertMatrix")
    {
        InvertMatrix(gc.get());
    }

    SECTION("Concat1")
    {
        Concat1(gc.get());
    }

    SECTION("Concat2")
    {
        Concat2(gc.get());
    }

    SECTION("Concat3")
    {
        Concat3(gc.get());
    }
}
#endif // wxUSE_CAIRO

// =====  Implementation  =====

static inline double RoundVal(double v)
{
    wxString s = wxString::Format("%g", v);
    s.ToDouble(&v);
    return v;
}

static void CheckMatrix(const wxGraphicsMatrix& m,
                        double a, double b, double c, double d,
                        double tx, double ty)
{
    double cur_a, cur_b, cur_c, cur_d, cur_tx, cur_ty;
    m.Get(&cur_a, &cur_b, &cur_c, &cur_d, &cur_tx, &cur_ty);

    wxString msg;

    if ( RoundVal(a) != RoundVal(cur_a) )
    {
        if ( !msg.empty() )
        {
            msg += "\n- ";
        }
        msg += wxString::Format("Invalid m11 value: Actual: %g  Expected: %g",
                                cur_a, a );
    }

    if ( RoundVal(b) != RoundVal(cur_b) )
    {
        if ( !msg.empty() )
        {
            msg += "\n- ";
        }
        msg += wxString::Format("Invalid m12 value: Actual: %g  Expected: %g",
                                cur_b, b );
    }

    if ( RoundVal(c) != RoundVal(cur_c) )
    {
        if ( !msg.empty() )
        {
            msg += "\n- ";
        }
        msg += wxString::Format("Invalid m21 value: Actual: %g  Expected: %g",
                                cur_c, c );
    }

    if ( RoundVal(d) != RoundVal(cur_d) )
    {
        if ( !msg.empty() )
        {
            msg += "\n- ";
        }
        msg += wxString::Format("Invalid m22 value: Actual: %g  Expected: %g",
                                cur_d, d );
    }

    if ( RoundVal(tx) != RoundVal(cur_tx) )
    {
        if ( !msg.empty() )
        {
            msg += "\n- ";
        }
        msg += wxString::Format("Invalid tx value: Actual: %g  Expected: %g",
                                cur_tx, tx );
    }

    if ( RoundVal(ty) != RoundVal(cur_ty) )
    {
        if ( !msg.empty() )
        {
            msg += "\n- ";
        }
        msg += wxString::Format("Invalid ty value: Actual: %g  Expected: %g",
                                cur_ty, ty );
    }

    if( !msg.empty() )
    {
        wxCharBuffer buffer = msg.ToUTF8();
        FAIL_CHECK( buffer.data() );
    }
}

static void InitState(wxGraphicsContext* gc)
{
    wxGraphicsMatrix m = gc->CreateMatrix();

    CheckMatrix(m, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
}

static void InvertMatrix(wxGraphicsContext* gc)
{
    wxGraphicsMatrix m = gc->CreateMatrix(2.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    m.Invert();

    CheckMatrix(m, 1.0, -1.0, -1.0, 2.0, 0.0, -1.0);
}

static void Concat1(wxGraphicsContext* gc)
{
    wxGraphicsMatrix m1 = gc->CreateMatrix(0.9, 0.4, -0.4, 0.9, 0.0, 0.0);
    wxGraphicsMatrix m2 = gc->CreateMatrix(1.0, 0.0, 0.0, 1.0, 3.0, 5.0);
    m1.Concat(m2);

    CheckMatrix(m1, 0.9, 0.4, -0.4, 0.9, 0.7, 5.7);
}

static void Concat2(wxGraphicsContext* gc)
{
    wxGraphicsMatrix m1 = gc->CreateMatrix(0.9, 0.4, -0.4, 0.9, 0.0, 0.0);
    wxGraphicsMatrix m2 = gc->CreateMatrix(1.0, 0.0, 0.0, 1.0, 3.0, 5.0);
    m2.Concat(m1);

    CheckMatrix(m2, 0.9, 0.4, -0.4, 0.9, 3.0, 5.0);
}

static void Concat3(wxGraphicsContext* gc)
{
    wxGraphicsMatrix m1 = gc->CreateMatrix(0.9, 0.4, -0.4, 0.9, 0.0, 0.0);
    wxGraphicsMatrix m2 = gc->CreateMatrix(1.0, 0.0, 0.0, 1.0, 3.0, 5.0);
    wxGraphicsMatrix m = m1;
    m.Concat(m2);

    CheckMatrix(m, 0.9, 0.4, -0.4, 0.9, 0.7, 5.7);
}

#endif // wxUSE_GRAPHICS_CONTEXT
