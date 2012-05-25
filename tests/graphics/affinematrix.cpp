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

#include "wx/graphics.h"
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
#if wxUSE_GRAPHICS_CONTEXT
        CPPUNIT_TEST( CompareToGraphicsContext );
#endif // wxUSE_GRAPHICS_CONTEXT
#endif // wxUSE_DC_TRANSFORM_MATRIX
    CPPUNIT_TEST_SUITE_END();

    void InvertMatrix();
#if wxUSE_DC_TRANSFORM_MATRIX
    void VMirrorAndTranslate();
    void Rotate90Clockwise();
#if wxUSE_GRAPHICS_CONTEXT
    void CompareToGraphicsContext();
#endif // wxUSE_GRAPHICS_CONTEXT

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
        matrix.Translate(0, -m_bmpOrig.GetHeight() + 1);
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
        matrix.Rotate(0.5 * M_PI);
        matrix.Translate(0, -m_bmpOrig.GetHeight());
        dc.SetTransformMatrix(matrix);
        dc.DrawBitmap(m_bmpOrig, 0, 0);
    }

    CPPUNIT_ASSERT_EQUAL( bmpUsingMatrix.ConvertToImage(),
                          m_imgOrig.Rotate90(true) );
}

#if wxUSE_GRAPHICS_CONTEXT
void AffineTransformTestCase::CompareToGraphicsContext()
{
    wxPoint2DDouble pointA1(1.0, 3.0), pointA2(60.0, 50.0),
                    pointG1(1.0, 3.0), pointG2(60.0, 50.0);

    // Create affine matrix and transform it
    wxAffineMatrix2D matrixA1, matrixA2;
    matrixA2.Rotate(M_PI / 3);
    matrixA1.Translate(-m_bmpOrig.GetWidth()/2, -m_bmpOrig.GetHeight()/2);
    matrixA1.Rotate(-M_PI *2/ 6);
    matrixA1.Translate(m_bmpOrig.GetWidth()/2, m_bmpOrig.GetHeight()/2);
    matrixA1.Mirror(wxHORIZONTAL);
    matrixA1.Concat(matrixA2);
    matrixA1.Mirror(wxVERTICAL);
    matrixA1.Translate(m_bmpOrig.GetWidth()/2, -m_bmpOrig.GetHeight()/2);
    matrixA1.Scale(0.9, 0.9);
    matrixA1.Invert();

    // Create image using first matrix
    wxBitmap bmpUsingMatrixA1(m_bmpOrig.GetHeight(), m_bmpOrig.GetWidth());

    // Build the transformed image using the transformation matrix
    {
        wxMemoryDC dc(bmpUsingMatrixA1);

        if ( !dc.CanUseTransformMatrix() )
            return;

        // Draw the bitmap
        dc.SetTransformMatrix(matrixA1);
        dc.DrawBitmap(m_bmpOrig, 0, 0);

        // Draw a line
        matrixA1.TransformPoint(&pointA1.m_x, &pointA1.m_y);
        matrixA1.TransformDistance(&pointA2.m_x, &pointA2.m_y);

        dc.DrawLine(wxRound(pointA1.m_x), wxRound(pointA1.m_y),
            wxRound(pointA1.m_x + pointA2.m_x), wxRound(pointA1.m_x + pointA2.m_y));
    }


    // Create graphics matrix and transform it
    wxMemoryDC mDc;
    wxGraphicsContext* gDc = wxGraphicsContext::Create(mDc);
    wxGraphicsMatrix matrixG1 = gDc->CreateMatrix();
    wxGraphicsMatrix matrixG2 = gDc->CreateMatrix();
    matrixG2.Rotate(M_PI / 3);
    matrixG1.Translate(-m_bmpOrig.GetWidth()/2, -m_bmpOrig.GetHeight()/2);
    matrixG1.Rotate(-M_PI*2 / 6);
    matrixG1.Translate(m_bmpOrig.GetWidth()/2, m_bmpOrig.GetHeight()/2);
    matrixG1.Scale(-1, 1);
    matrixG1.Concat(matrixG2);
    matrixG1.Scale(1, -1);
    matrixG1.Translate(m_bmpOrig.GetWidth()/2, -m_bmpOrig.GetHeight()/2);
    matrixG1.Scale(0.9, 0.9);
    matrixG1.Invert();
    // Create affine matrix from the graphics matrix
    wxMatrix2D mat2D;
    wxPoint2DDouble tr;
    matrixG1.Get(&mat2D.m_11, &mat2D.m_12, &mat2D.m_21, &mat2D.m_22, &tr.m_x, &tr.m_y);
    wxAffineMatrix2D matrixAG;
    matrixAG.Set(mat2D, tr);

    delete gDc;

    // Create image using last matrix
    wxBitmap bmpUsingMatrixAG(m_bmpOrig.GetHeight(), m_bmpOrig.GetWidth());

    // Build the transformed image using the transformation matrix
    {
        wxMemoryDC dc(bmpUsingMatrixAG);

        if ( !dc.CanUseTransformMatrix() )
            return;

        // Draw the bitmap
        dc.SetTransformMatrix(matrixAG);
        dc.DrawBitmap(m_bmpOrig, 0, 0);

        // Draw a line
        matrixG1.TransformPoint(&pointG1.m_x, &pointG1.m_y);
        matrixG1.TransformDistance(&pointG2.m_x, &pointG2.m_y);

        dc.DrawLine(wxRound(pointG1.m_x), wxRound(pointG1.m_y),
            wxRound(pointG1.m_x + pointG2.m_x), wxRound(pointG1.m_x + pointG2.m_y));
    }


    CPPUNIT_ASSERT_EQUAL( bmpUsingMatrixA1.ConvertToImage(),
                          bmpUsingMatrixAG.ConvertToImage() );

    // Save the images to check that something _is_ inside the visible area.
    //bmpUsingMatrixA1.SaveFile("matrixA1.jpg", wxBITMAP_TYPE_JPEG);
    //bmpUsingMatrixAG.SaveFile("matrixAG.jpg", wxBITMAP_TYPE_JPEG);
}
#endif // wxUSE_GRAPHICS_CONTEXT

#endif // wxUSE_DC_TRANSFORM_MATRIX
