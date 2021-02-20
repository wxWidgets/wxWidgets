///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/numformat.cpp
// Purpose:     wxNumberFormatter unit test
// Author:      Vadim Zeitlin
// Created:     2011-01-15
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/numformatter.h"
#include "wx/intl.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class NumFormatterTestCase
{
public:
    NumFormatterTestCase() :
        // We need to use a locale with known decimal point and which uses the
        // thousands separator for the tests to make sense.
        m_locale(wxLANGUAGE_ENGLISH_UK, wxLOCALE_DONT_LOAD_DEFAULT)
    {
    }

protected:
    bool CanRunTest() const { return m_locale.IsOk(); }

private:
    wxLocale m_locale;

    wxDECLARE_NO_COPY_CLASS(NumFormatterTestCase);
};

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::LongToString", "[numformatter]")
{
    if ( !CanRunTest() )
        return;

    CHECK( wxNumberFormatter::ToString(         1L) ==           "1" );
    CHECK( wxNumberFormatter::ToString(        -1L) ==          "-1" );
    CHECK( wxNumberFormatter::ToString(        12L) ==          "12" );
    CHECK( wxNumberFormatter::ToString(       -12L) ==         "-12" );
    CHECK( wxNumberFormatter::ToString(       123L) ==         "123" );
    CHECK( wxNumberFormatter::ToString(      -123L) ==        "-123" );
    CHECK( wxNumberFormatter::ToString(      1234L) ==       "1,234" );
    CHECK( wxNumberFormatter::ToString(     -1234L) ==      "-1,234" );
    CHECK( wxNumberFormatter::ToString(     12345L) ==      "12,345" );
    CHECK( wxNumberFormatter::ToString(    -12345L) ==     "-12,345" );
    CHECK( wxNumberFormatter::ToString(    123456L) ==     "123,456" );
    CHECK( wxNumberFormatter::ToString(   -123456L) ==    "-123,456" );
    CHECK( wxNumberFormatter::ToString(   1234567L) ==   "1,234,567" );
    CHECK( wxNumberFormatter::ToString(  -1234567L) ==  "-1,234,567" );
    CHECK( wxNumberFormatter::ToString(  12345678L) ==  "12,345,678" );
    CHECK( wxNumberFormatter::ToString( -12345678L) == "-12,345,678" );
    CHECK( wxNumberFormatter::ToString( 123456789L) == "123,456,789" );
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::LongLongToString", "[numformatter]")
{
    if ( !CanRunTest() )
        return;

    CHECK( wxNumberFormatter::ToString(wxLL(         1)) ==           "1" );
    CHECK( wxNumberFormatter::ToString(wxLL(        12)) ==          "12" );
    CHECK( wxNumberFormatter::ToString(wxLL(       123)) ==         "123" );
    CHECK( wxNumberFormatter::ToString(wxLL(      1234)) ==       "1,234" );
    CHECK( wxNumberFormatter::ToString(wxLL(     12345)) ==      "12,345" );
    CHECK( wxNumberFormatter::ToString(wxLL(    123456)) ==     "123,456" );
    CHECK( wxNumberFormatter::ToString(wxLL(   1234567)) ==   "1,234,567" );
    CHECK( wxNumberFormatter::ToString(wxLL(  12345678)) ==  "12,345,678" );
    CHECK( wxNumberFormatter::ToString(wxLL( 123456789)) == "123,456,789" );
}

#endif // wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::DoubleToString", "[numformatter]")
{
    if ( !CanRunTest() )
        return;

    CHECK( wxNumberFormatter::ToString(1., 1) == "1.0" );
    CHECK( wxNumberFormatter::ToString(0.123456, 6) == "0.123456" );
    CHECK( wxNumberFormatter::ToString(1.234567, 6) == "1.234567" );
    CHECK( wxNumberFormatter::ToString(12.34567, 5) == "12.34567" );
    CHECK( wxNumberFormatter::ToString(123.4567, 4) == "123.4567" );
    CHECK( wxNumberFormatter::ToString(1234.56, 2) == "1,234.56" );
    CHECK( wxNumberFormatter::ToString(12345.6, 1) == "12,345.6" );
    CHECK( wxNumberFormatter::ToString(123456789.012, 3) == "123,456,789.012" );
    CHECK( wxNumberFormatter::ToString(12345.012, -1) == "12,345" );

    CHECK( wxNumberFormatter::ToString(-123.123, 4, wxNumberFormatter::Style_None)
            == "-123.1230" );
    CHECK( wxNumberFormatter::ToString( 0.02, 1, wxNumberFormatter::Style_None)
            == "0.0" );
    CHECK( wxNumberFormatter::ToString(-0.02, 1, wxNumberFormatter::Style_None)
            == "-0.0" );
}

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::NoTrailingZeroes", "[numformatter]")
{
    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::ToString(123L, wxNumberFormatter::Style_NoTrailingZeroes)
    );

    if ( !CanRunTest() )
        return;

    CHECK( wxNumberFormatter::ToString(123., 3) == "123.000" );

    CHECK( wxNumberFormatter::ToString(123., 3, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123" );

    CHECK( wxNumberFormatter::ToString(123., 9, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123" );

    CHECK( wxNumberFormatter::ToString(123.456, 3, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123.456" );

    CHECK( wxNumberFormatter::ToString(123.456, 9) == "123.456000000" );

    CHECK( wxNumberFormatter::ToString(123.456, 9, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123.456" );

    CHECK( wxNumberFormatter::ToString(123.123, 2, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123.12" );

    CHECK( wxNumberFormatter::ToString(123.123, 0, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123" );

    CHECK( wxNumberFormatter::ToString(-0.000123, 3, wxNumberFormatter::Style_NoTrailingZeroes)
            == "0" );

    CHECK( wxNumberFormatter::ToString(123., -1, wxNumberFormatter::Style_NoTrailingZeroes)
            == "123" );

    CHECK( wxNumberFormatter::ToString(1e-120, -1, wxNumberFormatter::Style_NoTrailingZeroes)
            == "1e-120" );
}

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::LongFromString", "[numformatter]")
{
    if ( !CanRunTest() )
        return;

    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::FromString("123", static_cast<long *>(0))
    );

    long l;
    CHECK( !wxNumberFormatter::FromString("", &l) );
    CHECK( !wxNumberFormatter::FromString("foo", &l) );
    CHECK( !wxNumberFormatter::FromString("1.234", &l) );

    CHECK( wxNumberFormatter::FromString("123", &l) );
    CHECK( l == 123 );

    CHECK( wxNumberFormatter::FromString("1234", &l) );
    CHECK( l == 1234 );

    CHECK( wxNumberFormatter::FromString("1,234", &l) );
    CHECK( l == 1234 );

    CHECK( wxNumberFormatter::FromString("12,345", &l) );
    CHECK( l == 12345 );

    CHECK( wxNumberFormatter::FromString("123,456", &l) );
    CHECK( l == 123456 );

    CHECK( wxNumberFormatter::FromString("1,234,567", &l) );
    CHECK( l == 1234567 );
}

#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::LongLongFromString", "[numformatter]")
{
    if ( !CanRunTest() )
        return;

    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::FromString("123", static_cast<wxLongLong_t *>(0))
    );

    wxLongLong_t l;
    CHECK( !wxNumberFormatter::FromString("", &l) );
    CHECK( !wxNumberFormatter::FromString("foo", &l) );
    CHECK( !wxNumberFormatter::FromString("1.234", &l) );

    CHECK( wxNumberFormatter::FromString("123", &l) );
    CHECK( l == 123 );

    CHECK( wxNumberFormatter::FromString("1234", &l) );
    CHECK( l == 1234 );

    CHECK( wxNumberFormatter::FromString("1,234", &l) );
    CHECK( l == 1234 );

    CHECK( wxNumberFormatter::FromString("12,345", &l) );
    CHECK( l == 12345 );

    CHECK( wxNumberFormatter::FromString("123,456", &l) );
    CHECK( l == 123456 );

    CHECK( wxNumberFormatter::FromString("1,234,567", &l) );
    CHECK( l == 1234567 );
}

#endif // wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG

TEST_CASE_METHOD(NumFormatterTestCase, "NumFormatter::DoubleFromString", "[numformatter]")
{
    if ( !CanRunTest() )
        return;

    WX_ASSERT_FAILS_WITH_ASSERT
    (
        wxNumberFormatter::FromString("123", static_cast<double *>(0))
    );

    double d;
    CHECK( !wxNumberFormatter::FromString("", &d) );
    CHECK( !wxNumberFormatter::FromString("bar", &d) );

    CHECK( wxNumberFormatter::FromString("123", &d) );
    CHECK( d == 123. );

    CHECK( wxNumberFormatter::FromString("123.456789012", &d) );
    CHECK( d == 123.456789012 );

    CHECK( wxNumberFormatter::FromString("1,234.56789012", &d) );
    CHECK( d == 1234.56789012 );

    CHECK( wxNumberFormatter::FromString("12,345.6789012", &d) );
    CHECK( d == 12345.6789012 );

    CHECK( wxNumberFormatter::FromString("123,456.789012", &d) );
    CHECK( d == 123456.789012 );

    CHECK( wxNumberFormatter::FromString("1,234,567.89012", &d) );
    CHECK( d == 1234567.89012 );

    CHECK( wxNumberFormatter::FromString("12,345,678.9012", &d) );
    CHECK( d == 12345678.9012 );

    CHECK( wxNumberFormatter::FromString("123,456,789.012", &d) );
    CHECK( d == 123456789.012 );

    CHECK( wxNumberFormatter::FromString("123456789.012", &d) );
    CHECK( d == 123456789.012 );
}
