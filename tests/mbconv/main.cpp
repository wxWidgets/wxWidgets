///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/main.cpp
// Purpose:     wxMBConv unit test
// Author:      Vadim Zeitlin
// Created:     14.02.04
// RCS-ID:      $Id$
// Copyright:   (c) 2003 TT-Solutions
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/strconv.h"
#include "wx/string.h"

#include "wx/cppunit.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MBConvTestCase : public CppUnit::TestCase
{
public:
    MBConvTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MBConvTestCase );
        CPPUNIT_TEST( WC2CP1250 );
    CPPUNIT_TEST_SUITE_END();

    void WC2CP1250();

    DECLARE_NO_COPY_CLASS(MBConvTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MBConvTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MBConvTestCase, "MBConvTestCase" );

void MBConvTestCase::WC2CP1250()
{
    static const struct Data
    {
        const wchar_t *wc;
        const char *cp1250;
    } data[] =
    {
        { L"hello", "hello" },  // test that it works in simplest case
        { L"\xBD of \xBD is \xBC", NULL }, // this should fail as cp1250 doesn't have 1/2
    };

    wxCSConv cs1250(wxFONTENCODING_CP1250);
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        if (d.cp1250)
        {
            CPPUNIT_ASSERT( strcmp(cs1250.cWC2MB(d.wc), d.cp1250) == 0 );
        }
        else
        {
            CPPUNIT_ASSERT( (const char*)cs1250.cWC2MB(d.wc) == NULL );
        }
    }
}
