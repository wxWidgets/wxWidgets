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

    NO_COPY_CLASS(MBConvTestCase);
};

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
        { L"½ of ½ is ¼", "" }, // this should fail as cp1250 doesn't have 1/2
    };

    wxCSConv cs1250(wxFONTENCODING_CP1250);
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        CPPUNIT_ASSERT( wxString(d.wc, cs1250) == d.cp1250 );
    }
}

// ----------------------------------------------------------------------------
// program entry point
// ----------------------------------------------------------------------------

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(MBConvTestCase::suite());

  return runner.run("") ? 0 : 1;
}

