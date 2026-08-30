///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/strings.cpp
// Purpose:     wxStringTokenizer unit test
// Author:      Vadim Zeitlin
// Created:     2005-12-20 (extacted from strings.cpp)
// Copyright:   (c) 2004-2005 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/tokenzr.h"

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
    { wxT(""),                   wxT(" "),              wxTOKEN_DEFAULT,       0 },
    { wxT(""),                   wxT(" "),              wxTOKEN_RET_EMPTY,     0 },
    { wxT(""),                   wxT(" "),              wxTOKEN_RET_EMPTY_ALL, 0 },
    { wxT(""),                   wxT(" "),              wxTOKEN_RET_DELIMS,    0 },
    { wxT(":"),                  wxT(":"),              wxTOKEN_RET_EMPTY,     1 },
    { wxT(":"),                  wxT(":"),              wxTOKEN_RET_DELIMS,    1 },
    { wxT(":"),                  wxT(":"),              wxTOKEN_RET_EMPTY_ALL, 2 },
    { wxT("::"),                 wxT(":"),              wxTOKEN_RET_EMPTY,     1 },
    { wxT("::"),                 wxT(":"),              wxTOKEN_RET_DELIMS,    1 },
    { wxT("::"),                 wxT(":"),              wxTOKEN_RET_EMPTY_ALL, 3 },

    { wxT("Hello, world"),       wxT(" "),              wxTOKEN_DEFAULT,       2 },
    { wxT("Hello,   world  "),   wxT(" "),              wxTOKEN_DEFAULT,       2 },
    { wxT("Hello, world"),       wxT(","),              wxTOKEN_DEFAULT,       2 },
    { wxT("Hello, world!"),      wxT(",!"),             wxTOKEN_DEFAULT,       2 },
    { wxT("Hello,, world!"),     wxT(",!"),             wxTOKEN_DEFAULT,       3 },
    { wxT("Hello,, world!"),     wxT(",!"),             wxTOKEN_STRTOK,        2 },
    { wxT("Hello, world!"),      wxT(",!"),             wxTOKEN_RET_EMPTY_ALL, 3 },

    { wxT("username:password:uid:gid:gecos:home:shell"),
                                wxT(":"),              wxTOKEN_DEFAULT,       7 },

    { wxT("1:2::3:"),            wxT(":"),              wxTOKEN_DEFAULT,       4 },
    { wxT("1:2::3:"),            wxT(":"),              wxTOKEN_RET_EMPTY,     4 },
    { wxT("1:2::3:"),            wxT(":"),              wxTOKEN_RET_EMPTY_ALL, 5 },
    { wxT("1:2::3:"),            wxT(":"),              wxTOKEN_RET_DELIMS,    4 },
    { wxT("1:2::3:"),            wxT(":"),              wxTOKEN_STRTOK,        3 },

    { wxT("1:2::3::"),           wxT(":"),              wxTOKEN_DEFAULT,       4 },
    { wxT("1:2::3::"),           wxT(":"),              wxTOKEN_RET_EMPTY,     4 },
    { wxT("1:2::3::"),           wxT(":"),              wxTOKEN_RET_EMPTY_ALL, 6 },
    { wxT("1:2::3::"),           wxT(":"),              wxTOKEN_RET_DELIMS,    4 },
    { wxT("1:2::3::"),           wxT(":"),              wxTOKEN_STRTOK,        3 },

    { wxT("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_DEFAULT,       4 },
    { wxT("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_STRTOK,        4 },
    { wxT("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_RET_EMPTY,     6 },
    { wxT("1 \t3\t4  6   "),     wxDEFAULT_DELIMITERS, wxTOKEN_RET_EMPTY_ALL, 9 },

    { wxT("01/02/99"),           wxT("/-"),             wxTOKEN_DEFAULT,       3 },
    { wxT("01-02/99"),           wxT("/-"),             wxTOKEN_RET_DELIMS,    3 },
};

// ----------------------------------------------------------------------------
// the tests
// ----------------------------------------------------------------------------

TEST_CASE("Tokenizer::GetCount", "[tokenizer]")
{
    for ( size_t n = 0; n < WXSIZEOF(gs_testData); n++ )
    {
        const TokenizerTestData& ttd = gs_testData[n];

        wxStringTokenizer tkz(ttd.str, ttd.delims, ttd.mode);
        INFO( "for loop index " << n );
        CHECK( tkz.CountTokens() == ttd.count );

        size_t count = 0;
        while ( tkz.HasMoreTokens() )
        {
            tkz.GetNextToken();
            count++;
        }

        CHECK( count == ttd.count );
    }
}

// call this with the string to tokenize, delimeters to use and the expected
// positions (i.e. results of GetPosition()) after each GetNextToken() call,
// terminate positions with 0
static void
DoTestGetPosition(const wxChar *s, const wxChar *delims, int pos, ...)
{
    wxStringTokenizer tkz(s, delims);

    CHECK( tkz.GetPosition() == (size_t)0 );

    va_list ap;
    va_start(ap, pos);

    for ( ;; )
    {
        if ( !pos )
        {
            CHECK( !tkz.HasMoreTokens() );
            break;
        }

        tkz.GetNextToken();

        CHECK( tkz.GetPosition() == (size_t)pos );

        pos = va_arg(ap, int);
    }

    va_end(ap);
}

TEST_CASE("Tokenizer::GetPosition", "[tokenizer]")
{
    DoTestGetPosition(wxT("foo"), wxT("_"), 3, 0);
    DoTestGetPosition(wxT("foo_bar"), wxT("_"), 4, 7, 0);
    DoTestGetPosition(wxT("foo_bar_"), wxT("_"), 4, 8, 0);
}

// helper for GetString(): the parameters are the same as for DoTestGetPosition
// but it checks GetString() return value instead of GetPosition()
static void
DoTestGetString(const wxChar *s, const wxChar *delims, int pos, ...)
{
    wxStringTokenizer tkz(s, delims);

    CHECK( tkz.GetString() == wxString(s) );

    va_list ap;
    va_start(ap, pos);

    for ( ;; )
    {
        if ( !pos )
        {
            CHECK( tkz.GetString().empty() ) ;
            break;
        }

        tkz.GetNextToken();

        CHECK( tkz.GetString() == wxString(s + pos) );

        pos = va_arg(ap, int);
    }

    va_end(ap);
}

TEST_CASE("Tokenizer::GetString", "[tokenizer]")
{
    DoTestGetString(wxT("foo"), wxT("_"), 3, 0);
    DoTestGetString(wxT("foo_bar"), wxT("_"), 4, 7, 0);
    DoTestGetString(wxT("foo_bar_"), wxT("_"), 4, 8, 0);
}

TEST_CASE("Tokenizer::LastDelimiter", "[tokenizer]")
{
    wxStringTokenizer tkz(wxT("a+-b=c"), wxT("+-="));

    tkz.GetNextToken();
    CHECK( tkz.GetLastDelimiter() == wxT('+') );

    tkz.GetNextToken();
    CHECK( tkz.GetLastDelimiter() == wxT('-') );

    tkz.GetNextToken();
    CHECK( tkz.GetLastDelimiter() == wxT('=') );

    tkz.GetNextToken();
    CHECK( tkz.GetLastDelimiter() == wxT('\0') );
}

TEST_CASE("Tokenizer::StrtokCompat", "[tokenizer]")
{
    for ( size_t n = 0; n < WXSIZEOF(gs_testData); n++ )
    {
        const TokenizerTestData& ttd = gs_testData[n];
        if ( ttd.mode != wxTOKEN_STRTOK )
            continue;

        wxWCharBuffer
            buf(ttd.str);
        wxChar *last;
        wxChar *s = wxStrtok(buf.data(), ttd.delims, &last);

        wxStringTokenizer tkz(ttd.str, ttd.delims, ttd.mode);
        while ( tkz.HasMoreTokens() )
        {
            CHECK( tkz.GetNextToken() == wxString(s) );
            s = wxStrtok(nullptr, ttd.delims, &last);
        }
    }
}

TEST_CASE("Tokenizer::CopyObj", "[tokenizer]")
{
    // Test copy ctor
    wxStringTokenizer tkzSrc(wxT("first:second:third:fourth"), wxT(":"));
    while ( tkzSrc.HasMoreTokens() )
    {
        tkzSrc.GetNextToken();
        wxStringTokenizer tkz = tkzSrc;

        CHECK( tkz.GetPosition() == tkzSrc.GetPosition() );
        CHECK( tkz.GetString() == tkzSrc.GetString() );

        // Change the state of both objects and compare again...
        tkzSrc.GetNextToken();
        tkz.GetNextToken();

        CHECK( tkz.GetPosition() == tkzSrc.GetPosition() );
        CHECK( tkz.GetString() == tkzSrc.GetString() );
    }
}

TEST_CASE("Tokenizer::AssignObj", "[tokenizer]")
{
    // Test assignment
    wxStringTokenizer tkzSrc(wxT("first:second:third:fourth"), wxT(":"));
    wxStringTokenizer tkz;
    while ( tkzSrc.HasMoreTokens() )
    {
        tkzSrc.GetNextToken();
        tkz = tkzSrc;

        CHECK( tkz.GetPosition() == tkzSrc.GetPosition() );
        CHECK( tkz.GetString() == tkzSrc.GetString() );

        // Change the state of both objects and compare again...
        tkzSrc.GetNextToken();
        tkz.GetNextToken();

        CHECK( tkz.GetPosition() == tkzSrc.GetPosition() );
        CHECK( tkz.GetString() == tkzSrc.GetString() );
    }
}
