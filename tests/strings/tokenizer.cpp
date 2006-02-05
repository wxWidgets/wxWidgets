///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/strings.cpp
// Purpose:     wxStringTokenizer unit test
// Author:      Vadim Zeitlin
// Created:     2005-12-20 (extacted from strings.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2004-2005 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/tokenzr.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TokenizerTestCase : public CppUnit::TestCase
{
public:
    TokenizerTestCase() { }

private:
    CPPUNIT_TEST_SUITE( TokenizerTestCase );
        CPPUNIT_TEST( GetCount );
        CPPUNIT_TEST( GetPosition );
        CPPUNIT_TEST( GetString );
        CPPUNIT_TEST( LastDelimiter );
        CPPUNIT_TEST( StrtokCompat );
    CPPUNIT_TEST_SUITE_END();

    void GetCount();
    void GetPosition();
    void GetString();
    void LastDelimiter();
    void StrtokCompat();

    DECLARE_NO_COPY_CLASS(TokenizerTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TokenizerTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TokenizerTestCase, "TokenizerTestCase" );

// ----------------------------------------------------------------------------
// test data
// ----------------------------------------------------------------------------

static const struct TokenizerTestData
{
    // the string to tokenize
    const wxChar *str;

    // the delimiters to use
    const wxChar *delims;

    // the tokenizer mode
    wxStringTokenizerMode mode;

    // expected number of tokens
    size_t count;
}
gs_testData[] =
{
    { _T(""),                   _T(" "),              wxTOKEN_DEFAULT,       0 },
    { _T(""),                   _T(" "),              wxTOKEN_RET_EMPTY,     0 },
    { _T(""),                   _T(" "),              wxTOKEN_RET_EMPTY_ALL, 0 },
    { _T(""),                   _T(" "),              wxTOKEN_RET_DELIMS,    0 },
    { _T(":"),                  _T(":"),              wxTOKEN_RET_EMPTY,     1 },
    { _T(":"),                  _T(":"),              wxTOKEN_RET_DELIMS,    1 },
    { _T(":"),                  _T(":"),              wxTOKEN_RET_EMPTY_ALL, 2 },
    { _T("::"),                 _T(":"),              wxTOKEN_RET_EMPTY,     1 },
    { _T("::"),                 _T(":"),              wxTOKEN_RET_DELIMS,    1 },
    { _T("::"),                 _T(":"),              wxTOKEN_RET_EMPTY_ALL, 3 },

    { _T("Hello, world"),       _T(" "),              wxTOKEN_DEFAULT,       2 },
    { _T("Hello,   world  "),   _T(" "),              wxTOKEN_DEFAULT,       2 },
    { _T("Hello, world"),       _T(","),              wxTOKEN_DEFAULT,       2 },
    { _T("Hello, world!"),      _T(",!"),             wxTOKEN_DEFAULT,       2 },
    { _T("Hello,, world!"),     _T(",!"),             wxTOKEN_DEFAULT,       3 },
    { _T("Hello,, world!"),     _T(",!"),             wxTOKEN_STRTOK,        2 },
    { _T("Hello, world!"),      _T(",!"),             wxTOKEN_RET_EMPTY_ALL, 3 },

    { _T("username:password:uid:gid:gecos:home:shell"),
                                _T(":"),              wxTOKEN_DEFAULT,       7 },

    { _T("1:2::3:"),            _T(":"),              wxTOKEN_DEFAULT,       4 },
    { _T("1:2::3:"),            _T(":"),              wxTOKEN_RET_EMPTY,     4 },
    { _T("1:2::3:"),            _T(":"),              wxTOKEN_RET_EMPTY_ALL, 5 },
    { _T("1:2::3:"),            _T(":"),              wxTOKEN_RET_DELIMS,    4 },
    { _T("1:2::3:"),            _T(":"),              wxTOKEN_STRTOK,        3 },

    { _T("1:2::3::"),           _T(":"),              wxTOKEN_DEFAULT,       4 },
    { _T("1:2::3::"),           _T(":"),              wxTOKEN_RET_EMPTY,     4 },
    { _T("1:2::3::"),           _T(":"),              wxTOKEN_RET_EMPTY_ALL, 6 },
    { _T("1:2::3::"),           _T(":"),              wxTOKEN_RET_DELIMS,    4 },
    { _T("1:2::3::"),           _T(":"),              wxTOKEN_STRTOK,        3 },

    { _T("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_DEFAULT,       4 },
    { _T("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_STRTOK,        4 },
    { _T("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_RET_EMPTY,     6 },
    { _T("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_RET_EMPTY_ALL, 9 },

    { _T("01/02/99"),           _T("/-"),             wxTOKEN_DEFAULT,       3 },
    { _T("01-02/99"),           _T("/-"),             wxTOKEN_RET_DELIMS,    3 },
};

// helper function returning the string showing the index for which the test
// fails in the diagnostic message
static std::string Nth(size_t n)
{
    return std::string(wxString::Format(_T("for loop index %lu"),
                                        (unsigned long)n).mb_str());
}

// ----------------------------------------------------------------------------
// the tests
// ----------------------------------------------------------------------------

void TokenizerTestCase::GetCount()
{
    for ( size_t n = 0; n < WXSIZEOF(gs_testData); n++ )
    {
        const TokenizerTestData& ttd = gs_testData[n];

        wxStringTokenizer tkz(ttd.str, ttd.delims, ttd.mode);
        CPPUNIT_ASSERT_EQUAL_MESSAGE( Nth(n), ttd.count, tkz.CountTokens() );

        size_t count = 0;
        while ( tkz.HasMoreTokens() )
        {
            tkz.GetNextToken();
            count++;
        }

        CPPUNIT_ASSERT_EQUAL_MESSAGE( Nth(n), ttd.count, count );
    }
}

// call this with the string to tokenize, delimeters to use and the expected
// positions (i.e. results of GetPosition()) after each GetNextToken() call,
// terminate positions with 0
static void
DoTestGetPosition(const wxChar *s, const wxChar *delims, int pos, ...)
{
    wxStringTokenizer tkz(s, delims);

    CPPUNIT_ASSERT_EQUAL( (size_t)0, tkz.GetPosition() );

    va_list ap;
    va_start(ap, pos);

    for ( ;; )
    {
        if ( !pos )
        {
            CPPUNIT_ASSERT( !tkz.HasMoreTokens() );
            break;
        }

        tkz.GetNextToken();

        CPPUNIT_ASSERT_EQUAL( (size_t)pos, tkz.GetPosition() );

        pos = va_arg(ap, int);
    }

    va_end(ap);
}

void TokenizerTestCase::GetPosition()
{
    DoTestGetPosition(_T("foo"), _T("_"), 3, 0);
    DoTestGetPosition(_T("foo_bar"), _T("_"), 4, 7, 0);
    DoTestGetPosition(_T("foo_bar_"), _T("_"), 4, 8, 0);
}

// helper for GetString(): the parameters are the same as for DoTestGetPosition
// but it checks GetString() return value instead of GetPosition()
static void
DoTestGetString(const wxChar *s, const wxChar *delims, int pos, ...)
{
    wxStringTokenizer tkz(s, delims);

    CPPUNIT_ASSERT_EQUAL( wxString(s), tkz.GetString() );

    va_list ap;
    va_start(ap, pos);

    for ( ;; )
    {
        if ( !pos )
        {
            CPPUNIT_ASSERT( tkz.GetString().empty() ) ;
            break;
        }

        tkz.GetNextToken();

        CPPUNIT_ASSERT_EQUAL( wxString(s + pos), tkz.GetString() );

        pos = va_arg(ap, int);
    }

    va_end(ap);
}

void TokenizerTestCase::GetString()
{
    DoTestGetString(_T("foo"), _T("_"), 3, 0);
    DoTestGetString(_T("foo_bar"), _T("_"), 4, 7, 0);
    DoTestGetString(_T("foo_bar_"), _T("_"), 4, 8, 0);
}

void TokenizerTestCase::LastDelimiter()
{
    wxStringTokenizer tkz(_T("a+-b=c"), _T("+-="));

    tkz.GetNextToken();
    CPPUNIT_ASSERT_EQUAL( _T('+'), tkz.GetLastDelimiter() );

    tkz.GetNextToken();
    CPPUNIT_ASSERT_EQUAL( _T('-'), tkz.GetLastDelimiter() );

    tkz.GetNextToken();
    CPPUNIT_ASSERT_EQUAL( _T('='), tkz.GetLastDelimiter() );

    tkz.GetNextToken();
    CPPUNIT_ASSERT_EQUAL( _T('\0'), tkz.GetLastDelimiter() );
}

void TokenizerTestCase::StrtokCompat()
{
    for ( size_t n = 0; n < WXSIZEOF(gs_testData); n++ )
    {
        const TokenizerTestData& ttd = gs_testData[n];
        if ( ttd.mode != wxTOKEN_STRTOK )
            continue;

#if wxUSE_UNICODE
        wxWCharBuffer
#else
        wxCharBuffer
#endif
            buf(ttd.str);
        wxChar *last;
        wxChar *s = wxStrtok(buf.data(), ttd.delims, &last);

        wxStringTokenizer tkz(ttd.str, ttd.delims, ttd.mode);
        while ( tkz.HasMoreTokens() )
        {
            CPPUNIT_ASSERT_EQUAL( wxString(s), tkz.GetNextToken() );
            s = wxStrtok(NULL, ttd.delims, &last);
        }
    }
}


