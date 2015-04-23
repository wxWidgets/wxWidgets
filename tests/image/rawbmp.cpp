///////////////////////////////////////////////////////////////////////////////
// Name:        tests/image/rawbmp.cpp
// Purpose:     Test for using raw bitmap access classes with wxImage
// Author:      Vadim Zeitlin
// Created:     2008-05-30
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef wxHAS_RAW_BITMAP

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/image.h"
#include "wx/rawbmp.h"

namespace
{
    const int WIDTH = 10;
    const int HEIGHT = 10;
}

#define ASSERT_COL_EQUAL(x, y) \
    CPPUNIT_ASSERT_EQUAL( (unsigned char)(x), (y) )

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ImageRawTestCase : public CppUnit::TestCase
{
public:
    ImageRawTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ImageRawTestCase );
        CPPUNIT_TEST( RGBImage );
    CPPUNIT_TEST_SUITE_END();

    void RGBImage();

    wxDECLARE_NO_COPY_CLASS(ImageRawTestCase);
};

CPPUNIT_TEST_SUITE_REGISTRATION( ImageRawTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ImageRawTestCase, "ImageRawTestCase" );

void ImageRawTestCase::RGBImage()
{
    // create a check board image
    wxImage image(WIDTH, HEIGHT);

    wxImagePixelData data(image);
    wxImagePixelData::Iterator p(data);
    for ( int y = 0; y < HEIGHT; y++ )
    {
        const wxImagePixelData::Iterator rowStart = p;

        for ( int x = 0; x < WIDTH; x++ )
        {
            p.Red() =
            p.Green() =
            p.Blue() = (x + y) % 2 ? 0 : -1;
            ++p;
        }

        p = rowStart;
        p.OffsetY(data, 1);
    }

    // test a few pixels
    ASSERT_COL_EQUAL( 0xff, image.GetRed(0, 0) );
    ASSERT_COL_EQUAL( 0xff, image.GetBlue(1, 1) );
    ASSERT_COL_EQUAL( 0, image.GetGreen(0, 1) );
    ASSERT_COL_EQUAL( 0, image.GetGreen(1, 0) );
}

#endif // wxHAS_RAW_BITMAP
