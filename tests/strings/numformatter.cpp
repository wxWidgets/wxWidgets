///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/numformat.cpp
// Purpose:     wxNumberFormatter unit test
// Author:      Vadim Zeitlin
// Created:     2011-01-15
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/numformatter.h"
#include "wx/intl.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class NumFormatterTestCase : public CppUnit::TestCase
{
public:
    NumFormatterTestCase()
    {
        // We need to use a locale with known decimal point and which uses the
        // thousands separator for the tests to make sense.
        wxLanguage lang;
        if ( wxLocale::IsAvailable(wxLANGUAGE_ENGLISH_US) )
            lang = wxLANGUAGE_ENGLISH_US;
        else if ( wxLocale::IsAvailable(wxLANGUAGE_ENGLISH_UK) )
            lang = wxLANGUAGE_ENGLISH_UK;
        else
        {
            m_locale = NULL;
            return;
        }

        m_locale = new wxLocale(lang, wxLOCALE_DONT_LOAD_DEFAULT);
    }

    virtual ~NumFormatterTestCase()
    {
        delete m_locale;
    }

private:
    CPPUNIT_TEST_SUITE( NumFormatterTestCase );
        CPPUNIT_TEST( LongToString );
        CPPUNIT_TEST( DoubleToString );
        CPPUNIT_TEST( NoTrailingZeroes );
        CPPUNIT_TEST( LongFromString );
        CPPUNIT_TEST( DoubleFromString );
    CPPUNIT_TEST_SUITE_END();

    void LongToString();
    void DoubleToString();
    void NoTrailingZeroes();
    void LongFromString();
    void DoubleFromString();

    wxLocale *m_locale;

    wxDECLARE_NO_COPY_CLASS(NumFormatterTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( NumFormatterTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( NumFormatterTestCase, "NumFormatterTestCase" );

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void NumFormatterTestCase::LongToString()
{
    if ( !m_locale )
        return;

    CPPUNIT_ASSERT_EQUAL(          "1", wxNumberFormatter::ToString(         1));
    CPPUNIT_ASSERT_EQUAL(         "12", wxNumberFormatter::ToString(        12));
    CPPUNIT_ASSERT_EQUAL(        "123", wxNumberFormatter::ToString(       123));
    CPPUNIT_ASSERT_EQUAL(      "1,234", wxNumberFormatter::ToString(      1234));
    CPPUNIT_ASSERT_EQUAL(     "12,345", wxNumberFormatter::ToString(     12345));
    CPPUNIT_ASSERT_EQUAL(    "123,456", wxNumberFormatter::ToString(    123456));
    CPPUNIT_ASSERT_EQUAL(  "1,234,567", wxNumberFormatter::ToString(   1234567));
    CPPUNIT_ASSERT_EQUAL( "12,345,678", wxNumberFormatter::ToString(  12345678));
    CPPUNIT_ASSERT_EQUAL("123,456,789", wxNumberFormatter::ToString( 123456789));
}

void NumFormatterTestCase::DoubleToString()
{
    if ( !m_locale )
        return;

    CPPUNIT_ASSERT_EQUAL("1.0", wxNumberFormatter::ToString(1., 1));
    CPPUNIT_ASSERT_EQUAL("0.123456", wxNumberFormatter::ToString(0.123456, 6));
    CPPUNIT_ASSERT_EQUAL("1.234567", wxNumberFormatter::ToString(1.234567, 6));
    CPPUNIT_ASSERT_EQUAL("12.34567", wxNumberFormatter::ToString(12.34567, 5));
    CPPUNIT_ASSERT_EQUAL("123.4567", wxNumberFormatter::ToString(123.4567, 4));
    CPPUNIT_ASSERT_EQUAL("1,234.56", wxNumberFormatter::ToString(1234.56, 2));
    CPPUNIT_ASSERT_EQUAL("12,345.6", wxNumberFormatter::ToString(12345.6, 1));
    CPPUNIT_ASSERT_EQUAL("12,345.6", wxNumberFormatter::ToString(12345.6, 1));
    CPPUNIT_ASSERT_EQUAL("123,456,789.0",
                         wxNumberFormatter::ToString(123456789., 1));
    CPPUNIT_ASSERT_EQUAL("123,456,789.012",
                         wxNumberFormatter::ToString(123456789.012, 3));
}

void NumFormatterTestCase::NoTrailingZeroes()
{
    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::ToString(123L, wxNumberFormatter::Style_NoTrailingZeroes)
    );

    if ( !m_locale )
        return;

    CPPUNIT_ASSERT_EQUAL
    (
        "123.000",
        wxNumberFormatter::ToString(123., 3)
    );

    CPPUNIT_ASSERT_EQUAL
    (
        "123",
        wxNumberFormatter::ToString(123., 3, wxNumberFormatter::Style_NoTrailingZeroes)
    );

    CPPUNIT_ASSERT_EQUAL
    (
        "123",
        wxNumberFormatter::ToString(123., 9, wxNumberFormatter::Style_NoTrailingZeroes)
    );

    CPPUNIT_ASSERT_EQUAL
    (
        "123.456",
        wxNumberFormatter::ToString(123.456, 3, wxNumberFormatter::Style_NoTrailingZeroes)
    );

    CPPUNIT_ASSERT_EQUAL
    (
        "123.456000000",
        wxNumberFormatter::ToString(123.456, 9)
    );

    CPPUNIT_ASSERT_EQUAL
    (
        "123.456",
        wxNumberFormatter::ToString(123.456, 9, wxNumberFormatter::Style_NoTrailingZeroes)
    );
}

void NumFormatterTestCase::LongFromString()
{
    if ( !m_locale )
        return;

    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::FromString("123", static_cast<long *>(0))
    );

    long l;
    CPPUNIT_ASSERT( !wxNumberFormatter::FromString("", &l) );
    CPPUNIT_ASSERT( !wxNumberFormatter::FromString("foo", &l) );
    CPPUNIT_ASSERT( !wxNumberFormatter::FromString("1.234", &l) );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123", &l) );
    CPPUNIT_ASSERT_EQUAL( 123, l );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("1234", &l) );
    CPPUNIT_ASSERT_EQUAL( 1234, l );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("1,234", &l) );
    CPPUNIT_ASSERT_EQUAL( 1234, l );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("12,345", &l) );
    CPPUNIT_ASSERT_EQUAL( 12345, l );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123,456", &l) );
    CPPUNIT_ASSERT_EQUAL( 123456, l );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("1,234,567", &l) );
    CPPUNIT_ASSERT_EQUAL( 1234567, l );
}

void NumFormatterTestCase::DoubleFromString()
{
    if ( !m_locale )
        return;

    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::FromString("123", static_cast<double *>(0))
    );

    double d;
    CPPUNIT_ASSERT( !wxNumberFormatter::FromString("", &d) );
    CPPUNIT_ASSERT( !wxNumberFormatter::FromString("bar", &d) );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123", &d) );
    CPPUNIT_ASSERT_EQUAL( 123., d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123.456789012", &d) );
    CPPUNIT_ASSERT_EQUAL( 123.456789012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("1,234.56789012", &d) );
    CPPUNIT_ASSERT_EQUAL( 1234.56789012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("12,345.6789012", &d) );
    CPPUNIT_ASSERT_EQUAL( 12345.6789012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123,456.789012", &d) );
    CPPUNIT_ASSERT_EQUAL( 123456.789012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("1,234,567.89012", &d) );
    CPPUNIT_ASSERT_EQUAL( 1234567.89012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("12,345,678.9012", &d) );
    CPPUNIT_ASSERT_EQUAL( 12345678.9012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123,456,789.012", &d) );
    CPPUNIT_ASSERT_EQUAL( 123456789.012, d );

    CPPUNIT_ASSERT( wxNumberFormatter::FromString("123456789.012", &d) );
    CPPUNIT_ASSERT_EQUAL( 123456789.012, d );
}
