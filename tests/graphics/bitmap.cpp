///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/bitmap.cpp
// Purpose:     wxBitmap unit test
// Author:      Vadim Zeitlin
// Created:     2010-03-29
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/bitmap.h"
#include "wx/dcmemory.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class BitmapTestCase : public CppUnit::TestCase
{
public:
    BitmapTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( BitmapTestCase );
        CPPUNIT_TEST( Mask );
    CPPUNIT_TEST_SUITE_END();

    void Mask();

    wxBitmap m_bmp;

    DECLARE_NO_COPY_CLASS(BitmapTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( BitmapTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BitmapTestCase, "BitmapTestCase" );

void BitmapTestCase::setUp()
{
    m_bmp.Create(10, 10);

    wxMemoryDC dc(m_bmp);;
    dc.SetBackground(*wxWHITE);
    dc.Clear();

    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(4, 4, 2, 2);

    dc.SetPen(*wxRED_PEN);
    dc.DrawLine(0, 0, 10, 10);
    dc.DrawLine(10, 0, 0, 10);
}

void BitmapTestCase::tearDown()
{
    m_bmp = wxNullBitmap;
}

void BitmapTestCase::Mask()
{
    wxMask *mask = new wxMask(m_bmp, *wxBLACK);
    m_bmp.SetMask(mask);

    // copying masks should work
    wxMask *mask2 = new wxMask(*mask);
    m_bmp.SetMask(mask2);
}

