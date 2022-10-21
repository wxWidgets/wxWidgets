///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/graphmatrix.cpp
// Purpose:     Graphics matrix unit test
// Author:      Artur Wieczorek
// Created:     2016-09-18
// Copyright:   (c) 2016 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_GRAPHICS_CONTEXT

#include "wx/graphics.h"
#include "wx/dcmemory.h"

// ----------------------------------------------------------------------------
// Graphics matrix test classes
// ----------------------------------------------------------------------------

class GraphicsMatrixTestCaseBase : public CppUnit::TestCase
{
public:
    GraphicsMatrixTestCaseBase()
    {
        m_bmp.Create(100, 100);
        m_dc.SelectObject(m_bmp);
        m_rend = nullptr;
        m_ctx = nullptr;
    }

    ~GraphicsMatrixTestCaseBase()
    {
        m_dc.SelectObject(wxNullBitmap);
        m_bmp = wxNullBitmap;
    }

    virtual void setUp() override
    {
        wxASSERT( m_rend );
        m_ctx = m_rend->CreateContext(m_dc);
    }

    virtual void tearDown() override
    {
        delete m_ctx;
        m_ctx = nullptr;
    }

protected:
    void InitState();
    void InvertMatrix();
    void Concat1();
    void Concat2();
    void Concat3();

    wxGraphicsRenderer* m_rend;

private:
    void CheckMatrix(const wxGraphicsMatrix& m,
                     double a, double b, double c, double d,
                     double tx, double ty);

    wxBitmap m_bmp;
    wxMemoryDC m_dc;
    wxGraphicsContext* m_ctx;

    wxDECLARE_NO_COPY_CLASS(GraphicsMatrixTestCaseBase);
};

// ========================
// wxGraphicsContext tests
// ========================

#ifdef __WXMSW__
// GDI+ and Direct2D are available only under MSW.

#if wxUSE_GRAPHICS_GDIPLUS

class GraphicsMatrixTestCaseGDIPlus : public GraphicsMatrixTestCaseBase
{
public:
    GraphicsMatrixTestCaseGDIPlus()
    {
        m_rend = wxGraphicsRenderer::GetGDIPlusRenderer();
    }

    virtual ~GraphicsMatrixTestCaseGDIPlus()
    {
    }

private:
    CPPUNIT_TEST_SUITE( GraphicsMatrixTestCaseGDIPlus );
        CPPUNIT_TEST( InitState );
        CPPUNIT_TEST( InvertMatrix );
        CPPUNIT_TEST( Concat1 );
        CPPUNIT_TEST( Concat2 );
        CPPUNIT_TEST( Concat3 );
    CPPUNIT_TEST_SUITE_END();

protected:
    wxDECLARE_NO_COPY_CLASS(GraphicsMatrixTestCaseGDIPlus);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GraphicsMatrixTestCaseGDIPlus );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GraphicsMatrixTestCaseGDIPlus, "GraphicsMatrixTestCaseGDIPlus" );

#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D

class GraphicsMatrixTestCaseDirect2D : public GraphicsMatrixTestCaseBase
{
public:
    GraphicsMatrixTestCaseDirect2D()
    {
        m_rend = wxGraphicsRenderer::GetDirect2DRenderer();
    }

    virtual ~GraphicsMatrixTestCaseDirect2D()
    {
    }

private:
    CPPUNIT_TEST_SUITE( GraphicsMatrixTestCaseDirect2D );
        CPPUNIT_TEST( InitState );
        CPPUNIT_TEST( InvertMatrix );
        CPPUNIT_TEST( Concat1 );
        CPPUNIT_TEST( Concat2 );
        CPPUNIT_TEST( Concat3 );
    CPPUNIT_TEST_SUITE_END();

protected:
    wxDECLARE_NO_COPY_CLASS(GraphicsMatrixTestCaseDirect2D);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GraphicsMatrixTestCaseDirect2D );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GraphicsMatrixTestCaseDirect2D, "GraphicsMatrixTestCaseDirect2D" );

#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // __WXMSW__

#if wxUSE_CAIRO

class GraphicsMatrixTestCaseCairo : public GraphicsMatrixTestCaseBase
{
public:
    GraphicsMatrixTestCaseCairo()
    {
        m_rend = wxGraphicsRenderer::GetCairoRenderer();
    }

    virtual ~GraphicsMatrixTestCaseCairo()
    {
    }

private:
    CPPUNIT_TEST_SUITE( GraphicsMatrixTestCaseCairo );
        CPPUNIT_TEST( InitState );
        CPPUNIT_TEST( InvertMatrix );
        CPPUNIT_TEST( Concat1 );
        CPPUNIT_TEST( Concat2 );
        CPPUNIT_TEST( Concat3 );
    CPPUNIT_TEST_SUITE_END();

protected:
    wxDECLARE_NO_COPY_CLASS(GraphicsMatrixTestCaseCairo);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GraphicsMatrixTestCaseCairo );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GraphicsMatrixTestCaseCairo, "GraphicsMatrixTestCaseCairo" );

#endif // wxUSE_CAIRO

class GraphicsMatrixTestCaseDefault : public GraphicsMatrixTestCaseBase
{
public:
    GraphicsMatrixTestCaseDefault()
    {
        m_rend = wxGraphicsRenderer::GetDefaultRenderer();
    }

    virtual ~GraphicsMatrixTestCaseDefault()
    {
    }

private:
    CPPUNIT_TEST_SUITE( GraphicsMatrixTestCaseDefault );
        CPPUNIT_TEST( InitState );
        CPPUNIT_TEST( InvertMatrix );
        CPPUNIT_TEST( Concat1 );
        CPPUNIT_TEST( Concat2 );
        CPPUNIT_TEST( Concat3 );
    CPPUNIT_TEST_SUITE_END();

protected:
    wxDECLARE_NO_COPY_CLASS(GraphicsMatrixTestCaseDefault);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GraphicsMatrixTestCaseDefault );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GraphicsMatrixTestCaseDefault, "GraphicsMatrixTestCaseDefault" );

// =====  Implementation  =====
static inline double RoundVal(double v)
{
    wxString s = wxString::Format(wxS("%g"), v);
    s.ToDouble(&v);
    return v;
}

void GraphicsMatrixTestCaseBase::CheckMatrix(const wxGraphicsMatrix& m,
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
            msg += wxS("\n- ");
        }
        msg += wxString::Format(wxS("Invalid m11 value: Actual: %g  Expected: %g"),
                                cur_a, a );
    }

    if ( RoundVal(b) != RoundVal(cur_b) )
    {
        if ( !msg.empty() )
        {
            msg += wxS("\n- ");
        }
        msg += wxString::Format(wxS("Invalid m12 value: Actual: %g  Expected: %g"),
                                cur_b, b );
    }

    if ( RoundVal(c) != RoundVal(cur_c) )
    {
        if ( !msg.empty() )
        {
            msg += wxS("\n- ");
        }
        msg += wxString::Format(wxS("Invalid m21 value: Actual: %g  Expected: %g"),
                                cur_c, c );
    }

    if ( RoundVal(d) != RoundVal(cur_d) )
    {
        if ( !msg.empty() )
        {
            msg += wxS("\n- ");
        }
        msg += wxString::Format(wxS("Invalid m22 value: Actual: %g  Expected: %g"),
                                cur_d, d );
    }

    if ( RoundVal(tx) != RoundVal(cur_tx) )
    {
        if ( !msg.empty() )
        {
            msg += wxS("\n- ");
        }
        msg += wxString::Format(wxS("Invalid tx value: Actual: %g  Expected: %g"),
                                cur_tx, tx );
    }

    if ( RoundVal(ty) != RoundVal(cur_ty) )
    {
        if ( !msg.empty() )
        {
            msg += wxS("\n- ");
        }
        msg += wxString::Format(wxS("Invalid ty value: Actual: %g  Expected: %g"),
                                cur_ty, ty );
    }

    if( !msg.empty() )
    {
        wxCharBuffer buffer = msg.ToUTF8();
        CPPUNIT_FAIL( buffer.data() );
    }
}

void GraphicsMatrixTestCaseBase::InitState()
{
    wxGraphicsMatrix m = m_ctx->CreateMatrix();

    CheckMatrix(m, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
}

void GraphicsMatrixTestCaseBase::InvertMatrix()
{
    wxGraphicsMatrix m = m_ctx->CreateMatrix(2.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    m.Invert();

    CheckMatrix(m, 1.0, -1.0, -1.0, 2.0, 0.0, -1.0);
}

void GraphicsMatrixTestCaseBase::Concat1()
{
    wxGraphicsMatrix m1 = m_ctx->CreateMatrix(0.9, 0.4, -0.4, 0.9, 0.0, 0.0);
    wxGraphicsMatrix m2 = m_ctx->CreateMatrix(1.0, 0.0, 0.0, 1.0, 3.0, 5.0);
    m1.Concat(m2);

    CheckMatrix(m1, 0.9, 0.4, -0.4, 0.9, 0.7, 5.7);
}

void GraphicsMatrixTestCaseBase::Concat2()
{
    wxGraphicsMatrix m1 = m_ctx->CreateMatrix(0.9, 0.4, -0.4, 0.9, 0.0, 0.0);
    wxGraphicsMatrix m2 = m_ctx->CreateMatrix(1.0, 0.0, 0.0, 1.0, 3.0, 5.0);
    m2.Concat(m1);

    CheckMatrix(m2, 0.9, 0.4, -0.4, 0.9, 3.0, 5.0);
}

void GraphicsMatrixTestCaseBase::Concat3()
{
    wxGraphicsMatrix m1 = m_ctx->CreateMatrix(0.9, 0.4, -0.4, 0.9, 0.0, 0.0);
    wxGraphicsMatrix m2 = m_ctx->CreateMatrix(1.0, 0.0, 0.0, 1.0, 3.0, 5.0);
    wxGraphicsMatrix m = m1;
    m.Concat(m2);

    CheckMatrix(m, 0.9, 0.4, -0.4, 0.9, 0.7, 5.7);
}

#endif // wxUSE_GRAPHICS_CONTEXT
