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
        CPPUNIT_TEST( CaseChanges );
    CPPUNIT_TEST_SUITE_END();

    void String();
    void PChar();
    void Format();
    void Constructors();
    void Extraction();
    void Find();
    void SingleTokenizerTest( wxChar *str, wxChar *delims, size_t count , wxStringTokenizerMode mode );
    void Tokenizer();
    void Replace();
    void Match();
    void CaseChanges();

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

void StringTestCase::SingleTokenizerTest( wxChar *str, wxChar *delims, size_t count , wxStringTokenizerMode mode )
{
    wxStringTokenizer tkz( str, delims, mode);
    CPPUNIT_ASSERT( tkz.CountTokens() == count );

    wxChar *buf, *s = NULL, *last;

    if ( tkz.GetMode() == wxTOKEN_STRTOK )
    {                                             
        buf = new wxChar[wxStrlen(str) + 1];      
        wxStrcpy(buf, str);                       
        s = wxStrtok(buf, delims, &last);         
    }                                             
    else                                          
    {                                             
        buf = NULL;                               
    }                                             
                                                  
    size_t count2 = 0;                            
    while ( tkz.HasMoreTokens() )                 
    {                                             
        wxString token = tkz.GetNextToken();      
        if ( buf )                                
        {                                         
            CPPUNIT_ASSERT( token == s );         
            s = wxStrtok(NULL, delims, &last);    
        }                                         
        count2++;                                 
    }                                             
                                                  
    CPPUNIT_ASSERT( count2 == count );            
    if ( buf )                                    
    {                                             
        delete [] buf;                            
    }                                             
}

void StringTestCase::Tokenizer()
{
    SingleTokenizerTest( _T(""),                                           _T(" "),              0, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("Hello, world"),                               _T(" "),              2, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("Hello,   world  "),                           _T(" "),              2, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("Hello, world"),                               _T(","),              2, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("Hello, world!"),                              _T(",!"),             2, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("Hello,, world!"),                             _T(",!"),             3, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("Hello, world!"),                              _T(",!"),             3, wxTOKEN_RET_EMPTY_ALL );
    SingleTokenizerTest( _T("username:password:uid:gid:gecos:home:shell"), _T(":"),              7, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("1 \t3\t4  6   "),                             wxDEFAULT_DELIMITERS, 4, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("1 \t3\t4  6   "),                             wxDEFAULT_DELIMITERS, 6, wxTOKEN_RET_EMPTY     );
    SingleTokenizerTest( _T("1 \t3\t4  6   "),                             wxDEFAULT_DELIMITERS, 9, wxTOKEN_RET_EMPTY_ALL );
    SingleTokenizerTest( _T("01/02/99"),                                   _T("/-"),             3, wxTOKEN_DEFAULT       );
    SingleTokenizerTest( _T("01-02/99"),                                   _T("/-"),             3, wxTOKEN_RET_DELIMS    );
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


void StringTestCase::CaseChanges()
{
    wxString s1(_T("Hello!"));
    wxString s1u(s1);
    wxString s1l(s1);
    s1u.MakeUpper();
    s1l.MakeLower();
    wxString s2u, s2l;
    s2u.MakeUpper();
    s2l.MakeLower();

    CPPUNIT_ASSERT( s1u == _T("HELLO!") );
    CPPUNIT_ASSERT( s1l == _T("hello!") );
    CPPUNIT_ASSERT( s2u == wxEmptyString );
    CPPUNIT_ASSERT( s2l == wxEmptyString );
}
