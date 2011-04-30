///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/affinetransform.cpp
// Purpose:     Unit test for transformations implemented for wxAffineMatrix2D
// Author:      Catalin Raceanu
// Created:     2011-04-14
// Copyright:   (c) 2011 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcmemory.h"
#include "wx/affinematrix2d.h"
#include "wx/math.h"

#include "testimage.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class AffineTransformTestCase : public CppUnit::TestCase
{
public:
    AffineTransformTestCase()
    {
        wxImage::AddHandler(new wxJPEGHandler);
    }

    virtual void setUp();

private:
    CPPUNIT_TEST_SUITE( AffineTransformTestCase );
        CPPUNIT_TEST( InvertMatrix );
#if wxUSE_DC_TRANSFORM_MATRIX
        CPPUNIT_TEST( VMirrorAndTranslate );
        CPPUNIT_TEST( Rotate90Clockwise );
#endif // wxUSE_DC_TRANSFORM_MATRIX
    CPPUNIT_TEST_SUITE_END();

    void InvertMatrix();
#if wxUSE_DC_TRANSFORM_MATRIX
    void VMirrorAndTranslate();
    void Rotate90Clockwise();

    wxImage m_imgOrig;
    wxBitmap m_bmpOrig;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    DECLARE_NO_COPY_CLASS(AffineTransformTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( AffineTransformTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( AffineTransformTestCase, "AffineTransformTestCase" );

void AffineTransformTestCase::setUp()
{
#if wxUSE_DC_TRANSFORM_MATRIX
    m_imgOrig.LoadFile("horse.jpg");

    CPPUNIT_ASSERT( m_imgOrig.IsOk() );

    m_bmpOrig = wxBitmap(m_imgOrig);
#endif // wxUSE_DC_TRANSFORM_MATRIX
}

void AffineTransformTestCase::InvertMatrix()
{
    wxAffineMatrix2D matrix1;
    matrix1.Set(wxMatrix2D(2, 1, 1, 1), wxPoint2DDouble(1, 1));

    wxAffineMatrix2D matrix2(matrix1);

    matrix2.Invert();

    wxMatrix2D m;
    wxPoint2DDouble p;
    matrix2.Get(&m, &p);
    CPPUNIT_ASSERT_EQUAL( 1, (int)m.m_11 );
    CPPUNIT_ASSERT_EQUAL( -1, (int)m.m_12 );
    CPPUNIT_ASSERT_EQUAL( -1, (int)m.m_21 );
    CPPUNIT_ASSERT_EQUAL( 2, (int)m.m_22 );
    CPPUNIT_ASSERT_EQUAL( 0, (int)p.m_x );
    CPPUNIT_ASSERT_EQUAL( -1, (int)p.m_y );

    matrix2.Concat(matrix1);
    CPPUNIT_ASSERT( matrix2.IsIdentity() );
}

#if wxUSE_DC_TRANSFORM_MATRIX

void AffineTransformTestCase::VMirrorAndTranslate()
{
    wxBitmap bmpUsingMatrix(m_bmpOrig.GetWidth(), m_bmpOrig.GetHeight());

    // build the mirrored image using the transformation matrix
    {
        wxMemoryDC dc(bmpUsingMatrix);

        if ( !dc.CanUseTransformMatrix() )
            return;

        wxAffineMatrix2D matrix;
        matrix.Mirror(wxVERTICAL);
        matrix.Translate(0, m_bmpOrig.GetHeight() - 1);
        dc.SetTransformMatrix(matrix);
        dc.DrawBitmap(m_bmpOrig, 0, 0);
    }

    CPPUNIT_ASSERT_EQUAL( bmpUsingMatrix.ConvertToImage(),
                          m_imgOrig.Mirror(false) );
}

void AffineTransformTestCase::Rotate90Clockwise()
{
    wxBitmap bmpUsingMatrix(m_bmpOrig.GetHeight(), m_bmpOrig.GetWidth());

    // build the rotated image using the transformation matrix
    {
        wxMemoryDC dc(bmpUsingMatrix);

        if ( !dc.CanUseTransformMatrix() )
            return;

        wxAffineMatrix2D matrix;
        matrix.Rotate(-0.5 * M_PI);
        matrix.Translate(m_bmpOrig.GetHeight(), 0);
        dc.SetTransformMatrix(matrix);
        dc.DrawBitmap(m_bmpOrig, 0, 0);
    }

    CPPUNIT_ASSERT_EQUAL( bmpUsingMatrix.ConvertToImage(),
                          m_imgOrig.Rotate90(true) );
}

#endif // wxUSE_DC_TRANSFORM_MATRIX
