///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/unicode.cpp
// Purpose:     Unicode unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-04-28
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin, Wlodzimierz Skiba
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

#include "wx/textfile.h"

#include "wx/cppunit.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class UnicodeTestCase : public CppUnit::TestCase
{
public:
    UnicodeTestCase();

private:
    CPPUNIT_TEST_SUITE( UnicodeTestCase );
        CPPUNIT_TEST( ToFromAscii );
        CPPUNIT_TEST( TextFileRead );
    CPPUNIT_TEST_SUITE_END();

    void ToFromAscii();
    void TextFileRead();

    DECLARE_NO_COPY_CLASS(UnicodeTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( UnicodeTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( UnicodeTestCase, "UnicodeTestCase" );

UnicodeTestCase::UnicodeTestCase()
{
}

void UnicodeTestCase::ToFromAscii()
{

#define TEST_TO_FROM_ASCII(txt)                              \
    {                                                        \
        static const char *msg = txt;                        \
        wxString s = wxString::FromAscii(msg);               \
        CPPUNIT_ASSERT( strcmp( s.ToAscii() , msg ) == 0 );  \
    }

    TEST_TO_FROM_ASCII( "Hello, world!" );
    TEST_TO_FROM_ASCII( "additional \" special \t test \\ component \n :-)" );
}

void UnicodeTestCase::TextFileRead()
{
    wxTextFile file;
    bool file_opened = file.Open(_T("testdata.fc"), wxConvLocal);

    CPPUNIT_ASSERT( file_opened );

    static const wxChar *lines[6] = { 
        _T("# this is the test data file for wxFileConfig tests"),
        _T("value1=one"),
        _T("# a comment here"),
        _T("value2=two"),
        _T("value\\ with\\ spaces\\ inside\\ it=nothing special"),
        _T("path=$PATH")
    };

    if( file_opened )
    {
        const size_t count = file.GetLineCount();
        CPPUNIT_ASSERT( count == 6 );
        for ( size_t n = 0; n < count; n++ )
        {
            CPPUNIT_ASSERT( wxStrcmp( file[n].c_str() , lines[n] ) == 0 );
        }
    }
}
