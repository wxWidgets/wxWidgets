///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/strings.cpp
// Purpose:     wxString unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-04-19
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

#include "wx/tokenzr.h"

#include "wx/cppunit.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class StringTestCase : public CppUnit::TestCase
{
public:
    StringTestCase();

private:
    CPPUNIT_TEST_SUITE( StringTestCase );
        CPPUNIT_TEST( String );
        CPPUNIT_TEST( PChar );
        CPPUNIT_TEST( Format );
        CPPUNIT_TEST( Constructors );
        CPPUNIT_TEST( Extraction );
        CPPUNIT_TEST( Find );
        CPPUNIT_TEST( Tokenizer );
        CPPUNIT_TEST( Replace );
        CPPUNIT_TEST( Match );
        CPPUNIT_TEST( Std );
    CPPUNIT_TEST_SUITE_END();

    void String();
    void PChar();
    void Format();
    void Constructors();
    void Extraction();
    void Find();
    void Tokenizer();
    void Replace();
    void Match();
    void Std();

    DECLARE_NO_COPY_CLASS(StringTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( StringTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StringTestCase, "StringTestCase" );

StringTestCase::StringTestCase()
{
}

void StringTestCase::String()
{
    wxString a, b, c;

    a.reserve (128);
    b.reserve (128);
    c.reserve (128);

    for (int i = 0; i < 2; ++i)
    {
        a = _T("Hello");
        b = _T(" world");
        c = _T("! How'ya doin'?");
        a += b;
        a += c;
        c = _T("Hello world! What's up?");
        CPPUNIT_ASSERT( c != a );
    }
}

void StringTestCase::PChar()
{
    wxChar a [128];
    wxChar b [128];
    wxChar c [128];

    for (int i = 0; i < 2; ++i)
    {
        wxStrcpy (a, _T("Hello"));
        wxStrcpy (b, _T(" world"));
        wxStrcpy (c, _T("! How'ya doin'?"));
        wxStrcat (a, b);
        wxStrcat (a, c);
        wxStrcpy (c, _T("Hello world! What's up?"));
        CPPUNIT_ASSERT( wxStrcmp (c, a) != 0 );
    }
}

void StringTestCase::Format()
{
    wxString s1,s2;
    s1.Printf(_T("%03d"), 18);
    CPPUNIT_ASSERT( s1 == wxString::Format(_T("%03d"), 18) );
    s2.Printf(_T("Number 18: %s\n"), s1.c_str());
    CPPUNIT_ASSERT( s2 == wxString::Format(_T("Number 18: %s\n"), s1.c_str()) );
}

void StringTestCase::Constructors()
{
    #define TEST_CTOR(args, res)                                               \
        {                                                                      \
            wxString s args ;                                                  \
            CPPUNIT_ASSERT( s == res );                                        \
        }

    TEST_CTOR((_T('Z'), 4), _T("ZZZZ"));
    TEST_CTOR((_T("Hello"), 4), _T("Hell"));
    TEST_CTOR((_T("Hello"), 5), _T("Hello"));

    static const wxChar *s = _T("?really!");
    const wxChar *start = wxStrchr(s, _T('r'));
    const wxChar *end = wxStrchr(s, _T('!'));
    TEST_CTOR((start, end), _T("really"));
}

void StringTestCase::Extraction()
{
    wxString s(_T("Hello, world!"));

    CPPUNIT_ASSERT( wxStrcmp( s.c_str() , _T("Hello, world!") ) == 0 );
    CPPUNIT_ASSERT( wxStrcmp( s.Left(5).c_str() , _T("Hello") ) == 0 );
    CPPUNIT_ASSERT( wxStrcmp( s.Right(6).c_str() , _T("world!") ) == 0 );
    CPPUNIT_ASSERT( wxStrcmp( s(3, 5).c_str() , _T("lo, w") ) == 0 );
    CPPUNIT_ASSERT( wxStrcmp( s.Mid(3).c_str() , _T("lo, world!") ) == 0 );
    CPPUNIT_ASSERT( wxStrcmp( s.substr(3, 5).c_str() , _T("lo, w") ) == 0 );
    CPPUNIT_ASSERT( wxStrcmp( s.substr(3).c_str() , _T("lo, world!") ) == 0 );

    wxString rest;

    #define TEST_STARTS_WITH( prefix , correct_rest, result ) \
        CPPUNIT_ASSERT( \
            ( s.StartsWith( prefix, &rest ) == result ) && \
            ( ( result == false ) || ( wxStrcmp( correct_rest , rest ) == 0 ) ) \
        )

    TEST_STARTS_WITH( _T("Hello"),           _T(", world!"),      true  );
    TEST_STARTS_WITH( _T("Hello, "),         _T("world!"),        true  );
    TEST_STARTS_WITH( _T("Hello, world!"),   _T(""),              true  );
    TEST_STARTS_WITH( _T("Hello, world!!!"), _T(""),              false );
    TEST_STARTS_WITH( _T(""),                _T("Hello, world!"), true  );
    TEST_STARTS_WITH( _T("Goodbye"),         _T(""),              false );
    TEST_STARTS_WITH( _T("Hi"),              _T(""),              false );

    #undef TEST_STARTS_WITH
}

void StringTestCase::Find()
{
    #define TEST_FIND( str , start , result )  \
        CPPUNIT_ASSERT( wxString(str).find(_T("ell"), start) == result );

    TEST_FIND( _T("Well, hello world"),  0, 1              );
    TEST_FIND( _T("Well, hello world"),  6, 7              );
    TEST_FIND( _T("Well, hello world"),  9, wxString::npos );

    #undef TEST_FIND
}

void StringTestCase::Tokenizer()
{
    #define TEST_TOKENIZER( str , delims, count , mode )  \
        {                                                 \
            wxStringTokenizer tkz( str, delims, mode);    \
            CPPUNIT_ASSERT( tkz.CountTokens() == count ); \
                                                          \
            wxChar *buf, *s = NULL, *last;                \
                                                          \
            if ( tkz.GetMode() == wxTOKEN_STRTOK )        \
            {                                             \
                buf = new wxChar[wxStrlen(str) + 1];      \
                wxStrcpy(buf, str);                       \
                s = wxStrtok(buf, delims, &last);         \
            }                                             \
            else                                          \
            {                                             \
                buf = NULL;                               \
            }                                             \
                                                          \
            size_t count2 = 0;                            \
            while ( tkz.HasMoreTokens() )                 \
            {                                             \
                wxString token = tkz.GetNextToken();      \
                if ( buf )                                \
                {                                         \
                    CPPUNIT_ASSERT( token == s );         \
                    s = wxStrtok(NULL, delims, &last);    \
                }                                         \
                count2++;                                 \
            }                                             \
                                                          \
            CPPUNIT_ASSERT( count2 == count );            \
            if ( buf )                                    \
            {                                             \
                delete [] buf;                            \
            }                                             \
        }

    TEST_TOKENIZER( _T(""),                                           _T(" "),              0, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("Hello, world"),                               _T(" "),              2, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("Hello,   world  "),                           _T(" "),              2, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("Hello, world"),                               _T(","),              2, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("Hello, world!"),                              _T(",!"),             2, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("Hello,, world!"),                             _T(",!"),             3, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("Hello, world!"),                              _T(",!"),             3, wxTOKEN_RET_EMPTY_ALL );
    TEST_TOKENIZER( _T("username:password:uid:gid:gecos:home:shell"), _T(":"),              7, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("1 \t3\t4  6   "),                             wxDEFAULT_DELIMITERS, 4, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("1 \t3\t4  6   "),                             wxDEFAULT_DELIMITERS, 6, wxTOKEN_RET_EMPTY     );
    TEST_TOKENIZER( _T("1 \t3\t4  6   "),                             wxDEFAULT_DELIMITERS, 9, wxTOKEN_RET_EMPTY_ALL );
    TEST_TOKENIZER( _T("01/02/99"),                                   _T("/-"),             3, wxTOKEN_DEFAULT       );
    TEST_TOKENIZER( _T("01-02/99"),                                   _T("/-"),             3, wxTOKEN_RET_DELIMS    );

    #undef TEST_TOKENIZER
}

void StringTestCase::Replace()
{
    #define TEST_REPLACE( original , pos , len , replacement , result ) \
        { \
            wxString s = original; \
            s.replace( pos , len , replacement ); \
            CPPUNIT_ASSERT( s == result ); \
        }

    TEST_REPLACE( _T("012-AWORD-XYZ"), 4, 5, _T("BWORD"),  _T("012-BWORD-XYZ") );
    TEST_REPLACE( _T("increase"),      0, 2, _T("de"),     _T("decrease")      );
    TEST_REPLACE( _T("wxWindow"),      8, 0, _T("s"),      _T("wxWindows")     );
    TEST_REPLACE( _T("foobar"),        3, 0, _T("-"),      _T("foo-bar")       );
    TEST_REPLACE( _T("barfoo"),        0, 6, _T("foobar"), _T("foobar")        );

    #undef TEST_REPLACE
}

void StringTestCase::Match()
{
    #define TEST_MATCH( s1 , s2 , result ) \
        CPPUNIT_ASSERT( wxString(s1).Matches(s2) == result )

    TEST_MATCH( _T("foobar"),    _T("foo*"),   true  );
    TEST_MATCH( _T("foobar"),    _T("*oo*"),   true  );
    TEST_MATCH( _T("foobar"),    _T("*bar"),   true  );
    TEST_MATCH( _T("foobar"),    _T("??????"), true  );
    TEST_MATCH( _T("foobar"),    _T("f??b*"),  true  );
    TEST_MATCH( _T("foobar"),    _T("f?b*"),   false );
    TEST_MATCH( _T("foobar"),    _T("*goo*"),  false );
    TEST_MATCH( _T("foobar"),    _T("*foo"),   false );
    TEST_MATCH( _T("foobarfoo"), _T("*foo"),   true  );
    TEST_MATCH( _T(""),          _T("*"),      true  );
    TEST_MATCH( _T(""),          _T("?"),      false );

    #undef TEST_MATCH
}

void StringTestCase::Std()
{
    wxString s1(_T("abcdefgh")),
             s2(_T("abcdefghijklm"), 8),
             s3(_T("abcdefghijklm")),
             s4(8, _T('a'));
    wxString s5(s1),
             s6(s3, 0, 8),
             s7(s3.begin(), s3.begin() + 8);
    wxString s8(s1, 4, 8), s9, s10, s11;

    CPPUNIT_ASSERT( s1 == _T("abcdefgh") );
    CPPUNIT_ASSERT( s2 == s1 );
    CPPUNIT_ASSERT( s4 == _T("aaaaaaaa") );
    CPPUNIT_ASSERT( s5 == _T("abcdefgh") );
    CPPUNIT_ASSERT( s6 == s1 );
    CPPUNIT_ASSERT( s7 == s1 );
    CPPUNIT_ASSERT( s8 == _T("efgh") );

    // test append
    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = _T("abc");
    s1.append(_T("def"));
    s2.append(_T("defgh"), 3);
    s3.append(wxString(_T("abcdef")), 3, 6);
    s4.append(s1);
    s5.append(3, _T('a'));
    s6.append(s1.begin() + 3, s1.end());

    CPPUNIT_ASSERT( s1 == _T("abcdef") );
    CPPUNIT_ASSERT( s2 == _T("abcdef") );
    CPPUNIT_ASSERT( s3 == _T("abcdef") );
    CPPUNIT_ASSERT( s4 == _T("abcabcdef") );
    CPPUNIT_ASSERT( s5 == _T("abcaaa") );
    CPPUNIT_ASSERT( s6 == _T("abcdef") );

    // test assign
    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = _T("abc");
    s1.assign(_T("def"));
    s2.assign(_T("defgh"), 3);
    s3.assign(wxString(_T("abcdef")), 3, 6);
    s4.assign(s1);
    s5.assign(3, _T('a'));
    s6.assign(s1.begin() + 1, s1.end());

    CPPUNIT_ASSERT( s1 == _T("def") );
    CPPUNIT_ASSERT( s2 == _T("def") );
    CPPUNIT_ASSERT( s3 == _T("def") );
    CPPUNIT_ASSERT( s4 == _T("def") );
    CPPUNIT_ASSERT( s5 == _T("aaa") );
    CPPUNIT_ASSERT( s6 == _T("ef") );

    // test compare
    s1 = _T("abcdefgh");
    s2 = _T("abcdefgh");
    s3 = _T("abc");
    s4 = _T("abcdefghi");
    s5 = _T("aaa");
    s6 = _T("zzz");
    s7 = _T("zabcdefg");

    CPPUNIT_ASSERT( s1.compare(s2) == 0 );
    CPPUNIT_ASSERT( s1.compare(s3) > 0 );
    CPPUNIT_ASSERT( s1.compare(s4) < 0 );
    CPPUNIT_ASSERT( s1.compare(s5) > 0 );
    CPPUNIT_ASSERT( s1.compare(s6) < 0 );
    CPPUNIT_ASSERT( s1.compare(1, 12, s1) > 0);
    CPPUNIT_ASSERT( s1.compare(_T("abcdefgh")) == 0);
    CPPUNIT_ASSERT( s1.compare(1, 7, _T("bcdefgh")) == 0);
    CPPUNIT_ASSERT( s1.compare(1, 7, _T("bcdefgh"), 7) == 0);

    // test erase
    s1.erase(1, 1);
    s2.erase(4, 12);
    wxString::iterator it = s3.erase(s3.begin() + 1);
    wxString::iterator it2 = s4.erase(s4.begin() + 4, s4.begin() + 6);
    wxString::iterator it3 = s7.erase(s7.begin() + 4, s7.begin() + 8);

    CPPUNIT_ASSERT( s1 == _T("acdefgh") );
    CPPUNIT_ASSERT( s2 == _T("abcd") );
    CPPUNIT_ASSERT( s3 == _T("ac") );
    CPPUNIT_ASSERT( s4 == _T("abcdghi") );
    CPPUNIT_ASSERT( s7 == _T("zabc") );
    CPPUNIT_ASSERT( *it == _T('c') );
    CPPUNIT_ASSERT( *it2 == _T('g') );
    CPPUNIT_ASSERT( it3 == s7.end() );

    // find
    //       0         1         2
    //       01234567890123456789012345
    s1 = _T("abcdefgABCDEFGabcABCabcABC");
    s2 = _T("gAB");

    CPPUNIT_ASSERT( s1.find(_T('A')) == 7u );
    CPPUNIT_ASSERT( s1.find(_T('A'), 7) == 7u );
    CPPUNIT_ASSERT( s1.find(_T('Z')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find(_T('C'), 22) == 25u );

    CPPUNIT_ASSERT( s1.find(_T("gAB")) == 6u );
    CPPUNIT_ASSERT( s1.find(_T("gAB"), 7) == wxString::npos );
    CPPUNIT_ASSERT( s1.find(_T("gAB"), 6) == 6u );

    CPPUNIT_ASSERT( s1.find(_T("gABZZZ"), 2, 3) == 6u );
    CPPUNIT_ASSERT( s1.find(_T("gABZZZ"), 7, 3) == wxString::npos );

    CPPUNIT_ASSERT( s1.find(s2) == 6u );
    CPPUNIT_ASSERT( s1.find(s2, 7) == wxString::npos );
    CPPUNIT_ASSERT( s1.find(s2, 6) == 6u );

    // find_first_not_of
    //       0         1         2         3
    //       01234567890123456789012345678901234
    s1 = _T("aaaaaabcdefghlkjiaaaaaabcdbcdbcdbcd");
    s2 = _T("aaaaaa");

    CPPUNIT_ASSERT( s1.find_first_not_of(_T('a')) == 6u );
    CPPUNIT_ASSERT( s1.find_first_not_of(_T('a'), 7) == 7u );
    CPPUNIT_ASSERT( s2.find_first_not_of(_T('a')) == wxString::npos );

    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abde"), 4) == 7u );
    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abde"), 7) == 7u );
    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abcdefghijkl")) == wxString::npos );

    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abcdefghi"), 0, 4) == 9u );

    // find_first_of
    CPPUNIT_ASSERT( s1.find_first_of(_T('c')) == 7u );
    CPPUNIT_ASSERT( s1.find_first_of(_T('v')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_first_of(_T('c'), 10) == 24u );

    CPPUNIT_ASSERT( s1.find_first_of(_T("ijkl")) == 13u );
    CPPUNIT_ASSERT( s1.find_first_of(_T("ddcfg"), 17) == 24u );
    CPPUNIT_ASSERT( s1.find_first_of(_T("ddcfga"), 17, 5) == 24u );

    // find_last_not_of
    //       0         1         2         3
    //       01234567890123456789012345678901234
    s1 = _T("aaaaaabcdefghlkjiaaaaaabcdbcdbcdbcd");
    s2 = _T("aaaaaa");

    CPPUNIT_ASSERT( s2.find_last_not_of(_T('a')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T('d')) == 33u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T('d'), 25) == 24u );

    CPPUNIT_ASSERT( s1.find_last_not_of(_T("bcd")) == 22u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T("abc"), 24) == 16u );

    CPPUNIT_ASSERT( s1.find_last_not_of(_T("abcdefghijklmnopqrstuv"), 24, 3) == 16u );

    // find_last_of
    CPPUNIT_ASSERT( s2.find_last_of(_T('c')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_last_of(_T('a')) == 22u );
    CPPUNIT_ASSERT( s1.find_last_of(_T('b'), 24) == 23u );

    CPPUNIT_ASSERT( s1.find_last_of(_T("ijklm")) == 16u );
    CPPUNIT_ASSERT( s1.find_last_of(_T("ijklma"), 33, 4) == 16u );
    CPPUNIT_ASSERT( s1.find_last_of(_T("a"), 17) == 17u );

    // test insert
    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = _T("aaaa");
    s9 = s10 = _T("cdefg");

    s1.insert(1, _T("cc") );
    s2.insert(2, _T("cdef"), 3);
    s3.insert(2, s10);
    s4.insert(2, s10, 3, 7);
    s5.insert(1, 2, _T('c'));
    it = s6.insert(s6.begin() + 3, _T('X'));
    s7.insert(s7.begin(), s9.begin(), s9.end() - 1);
    s8.insert(s8.begin(), 2, _T('c'));

    CPPUNIT_ASSERT( s1 == _T("accaaa") );
    CPPUNIT_ASSERT( s2 == _T("aacdeaa") );
    CPPUNIT_ASSERT( s3 == _T("aacdefgaa") );
    CPPUNIT_ASSERT( s4 == _T("aafgaa") );
    CPPUNIT_ASSERT( s5 == _T("accaaa") );
    CPPUNIT_ASSERT( s6 == _T("aaaXa") );
    CPPUNIT_ASSERT( s7 == _T("cdefaaaa") );
    CPPUNIT_ASSERT( s8 == _T("ccaaaa") );

    s1 = s2 = s3 = _T("aaaa");
    s1.insert(0, _T("ccc"), 2);
    s2.insert(4, _T("ccc"), 2);

    CPPUNIT_ASSERT( s1 == _T("ccaaaa") );
    CPPUNIT_ASSERT( s2 == _T("aaaacc") );

    // test replace
    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = _T("QWERTYUIOP");
    s9 = s10 = _T("werty");

    s1.replace(3, 4, _T("rtyu"));
    s1.replace(8, 7, _T("opopop"));
    s2.replace(10, 12, _T("WWWW"));
    s3.replace(1, 5, s9);
    s4.replace(1, 4, s9, 0, 4);
    s5.replace(1, 2, s9, 1, 12);
    s6.replace(0, 123, s9, 0, 123);
    s7.replace(2, 7, s9);

    CPPUNIT_ASSERT( s1 == _T("QWErtyuIopopop") );
    CPPUNIT_ASSERT( s2 == _T("QWERTYUIOPWWWW") );
    CPPUNIT_ASSERT( s3 == _T("QwertyUIOP") );
    CPPUNIT_ASSERT( s4 == _T("QwertYUIOP") );
    CPPUNIT_ASSERT( s5 == _T("QertyRTYUIOP") );
    CPPUNIT_ASSERT( s6 == s9);
    CPPUNIT_ASSERT( s7 == _T("QWwertyP") );

    // rfind
    //       0         1         2
    //       01234567890123456789012345
    s1 = _T("abcdefgABCDEFGabcABCabcABC");
    s2 = _T("gAB");
    s3 = _T("ab");

    CPPUNIT_ASSERT( s1.rfind(_T('A')) == 23u );
    CPPUNIT_ASSERT( s1.rfind(_T('A'), 7) == 7u );
    CPPUNIT_ASSERT( s1.rfind(_T('Z')) == wxString::npos );
    CPPUNIT_ASSERT( s1.rfind(_T('C'), 22) == 19u );

    CPPUNIT_ASSERT( s1.rfind(_T("cAB")) == 22u );
    CPPUNIT_ASSERT( s1.rfind(_T("cAB"), 15) == wxString::npos );
    CPPUNIT_ASSERT( s1.rfind(_T("cAB"), 21) == 16u );

    CPPUNIT_ASSERT( s1.rfind(_T("gABZZZ"), 7, 3) == 6u );
    CPPUNIT_ASSERT( s1.rfind(_T("gABZZZ"), 5, 3) == wxString::npos );

    CPPUNIT_ASSERT( s1.rfind(s2) == 6u );
    CPPUNIT_ASSERT( s1.rfind(s2, 5) == wxString::npos );
    CPPUNIT_ASSERT( s1.rfind(s2, 6) == 6u );
    CPPUNIT_ASSERT( s1.rfind(s3, 1) == 0u );

    // resize
    s1 = s2 = s3 = s4 = _T("abcABCdefDEF");

    s1.resize( 12 );
    s2.resize( 10 );
    s3.resize( 14, _T(' ') );
    s4.resize( 14, _T('W') );

    CPPUNIT_ASSERT( s1 == _T("abcABCdefDEF") );
    CPPUNIT_ASSERT( s2 == _T("abcABCdefD") );
    CPPUNIT_ASSERT( s3 == _T("abcABCdefDEF  ") );
    CPPUNIT_ASSERT( s4 == _T("abcABCdefDEFWW") );

    // substr
    s1 = _T("abcdefgABCDEFG");

    CPPUNIT_ASSERT( s1.substr( 0, 14 ) == s1 );
    CPPUNIT_ASSERT( s1.substr( 1, 13 ) == _T("bcdefgABCDEFG") );
    CPPUNIT_ASSERT( s1.substr( 1, 20 ) == _T("bcdefgABCDEFG") );
    CPPUNIT_ASSERT( s1.substr( 14, 30 ) == _T("") );
}

