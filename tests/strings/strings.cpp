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
#if wxUSE_WCHAR_T
        CPPUNIT_TEST( ConstructorsWithConversion );
        CPPUNIT_TEST( Conversion );
#endif
#if wxUSE_UNICODE
        CPPUNIT_TEST( ConversionUTF7 );
#endif
        CPPUNIT_TEST( Extraction );
        CPPUNIT_TEST( Find );
        CPPUNIT_TEST( Tokenizer );
        CPPUNIT_TEST( TokenizerGetPosition );
        CPPUNIT_TEST( Replace );
        CPPUNIT_TEST( Match );
        CPPUNIT_TEST( CaseChanges );
        CPPUNIT_TEST( Compare );
        CPPUNIT_TEST( CompareNoCase );
    CPPUNIT_TEST_SUITE_END();

    void String();
    void PChar();
    void Format();
    void Constructors();
#if wxUSE_WCHAR_T
    void ConstructorsWithConversion();
    void Conversion();
#endif
#if wxUSE_UNICODE
    void ConversionUTF7();
#endif
    void Extraction();
    void Find();
    void SingleTokenizerTest( wxChar *str, wxChar *delims, size_t count , wxStringTokenizerMode mode );
    void Tokenizer();
    void TokenizerGetPosition();
    void Replace();
    void Match();
    void CaseChanges();
    void Compare();
    void CompareNoCase();

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

#if wxUSE_WCHAR_T
void StringTestCase::ConstructorsWithConversion()
{
    // the string "Déjà" in UTF-8 and wchar_t:
    const unsigned char utf8Buf[] = {0x44,0xC3,0xA9,0x6A,0xC3,0xA0,0};
    const wchar_t wchar[] = {0x44,0xE9,0x6A,0xE0,0};
    const unsigned char utf8subBuf[] = {0x44,0xC3,0xA9,0x6A,0}; // just "Déj"
    const char *utf8 = (char *)utf8Buf;
    const char *utf8sub = (char *)utf8subBuf;

    wxString s1(utf8, wxConvUTF8);
    wxString s2(wchar, wxConvUTF8);

#if wxUSE_UNICODE
    CPPUNIT_ASSERT( s1 == wchar );
    CPPUNIT_ASSERT( s2 == wchar );
#else
    CPPUNIT_ASSERT( s1 == utf8 );
    CPPUNIT_ASSERT( s2 == utf8 );
#endif

    wxString sub(utf8sub, wxConvUTF8); // "Dej" substring
    wxString s3(utf8, wxConvUTF8, 4);
    wxString s4(wchar, wxConvUTF8, 3);

    CPPUNIT_ASSERT( s3 == sub );
    CPPUNIT_ASSERT( s4 == sub );

#if wxUSE_UNICODE
    CPPUNIT_ASSERT ( wxString("\t[pl]open.format.Sformatuj dyskietkê=gfloppy %f", 
                               wxConvUTF8) == wxT("") ); //Pos 35 (funky e) is invalid UTF8
#else
    CPPUNIT_ASSERT ( wxString(L"\t[pl]open.format.Sformatuj dyskietkê=gfloppy %f", 
                               wxConvUTF8) == wxT("") ); //Pos 35 (funky e) is invalid UTF8
#endif
}

void StringTestCase::Conversion()
{
#if wxUSE_UNICODE
        wxString szTheString(L"The\0String", wxConvLibc, 10);
        wxCharBuffer theBuffer = szTheString.mb_str();

        CPPUNIT_ASSERT( memcmp(theBuffer.data(), "The\0String", 11) == 0 );

        wxString szTheString2("The\0String", wxConvLocal, 10);
        CPPUNIT_ASSERT( wxMemcmp(szTheString2.c_str(), L"The\0String", 11) == 0 );
#else
        wxString szTheString(wxT("TheString"));
        szTheString.insert(3, 1, '\0');
        wxWCharBuffer theBuffer = szTheString.wc_str(wxConvLibc);

        CPPUNIT_ASSERT( memcmp(theBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );

        wxString szLocalTheString(wxT("TheString"));
        szLocalTheString.insert(3, 1, '\0');
        wxWCharBuffer theLocalBuffer = szLocalTheString.wc_str(wxConvLocal);

        CPPUNIT_ASSERT( memcmp(theLocalBuffer.data(), L"The\0String", 11 * sizeof(wchar_t)) == 0 );
#endif
}
#endif // wxUSE_WCHAR_T

#if wxUSE_UNICODE
void StringTestCase::ConversionUTF7()
{
    const wxChar wdata[] = { 0x00A3, 0x00A3, 0x00A3, 0x00A3, 0 }; // pound signs
    const char *utf7 = "+AKM-+AKM-+AKM-+AKM-";
    wxString str(wdata);

    wxCSConv conv(_T("utf-7"));
    CPPUNIT_ASSERT( strcmp(str.mb_str(conv), utf7) == 0 );
}
#endif // wxUSE_UNICODE

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

// call this with the string to tokenize, delimeters to use and the expected
// positions (i.e. results of GetPosition()) after each GetNextToken() call,
// terminate positions with 0
static void
DoTokenizerGetPosition(const wxChar *s, const wxChar *delims, int pos, ...)
{
    wxStringTokenizer tkz(s, delims);

    CPPUNIT_ASSERT( tkz.GetPosition() == 0 );

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

        CPPUNIT_ASSERT( tkz.GetPosition() == (size_t)pos );

        pos = va_arg(ap, int);
    }

    va_end(ap);
}

void StringTestCase::TokenizerGetPosition()
{
    DoTokenizerGetPosition(_T("foo"), _T("_"), 3, 0);
    DoTokenizerGetPosition(_T("foo_bar"), _T("_"), 4, 7, 0);
    DoTokenizerGetPosition(_T("foo_bar_"), _T("_"), 4, 8, 0);
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

#if !wxUSE_UNICODE
    wxLocale locRu(wxLANGUAGE_RUSSIAN, 0 /* flags */);
    if ( locRu.IsOk() )
    {
        // try upper casing 8bit strings
        wxString sUpper("\xdf"),
                 sLower("\xff");

        CPPUNIT_ASSERT( sUpper.Lower() == sLower );
        CPPUNIT_ASSERT( sLower.Upper() == sUpper );
    }
#endif // !wxUSE_UNICODE
}

void StringTestCase::Compare()
{
    wxString s1 = wxT("AHH");
    wxString eq = wxT("AHH");
    wxString neq1 = wxT("HAH");
    wxString neq2 = wxT("AH");
    wxString neq3 = wxT("AHHH");
    wxString neq4 = wxT("AhH");

    CPPUNIT_ASSERT( s1 == eq );
    CPPUNIT_ASSERT( s1 != neq1 );
    CPPUNIT_ASSERT( s1 != neq2 );
    CPPUNIT_ASSERT( s1 != neq3 );
    CPPUNIT_ASSERT( s1 != neq4 );

//    wxString _s1 = wxT("A\0HH");
//    wxString _eq = wxT("A\0HH");
//    wxString _neq1 = wxT("H\0AH");
//    wxString _neq2 = wxT("A\0H");
//    wxString _neq3 = wxT("A\0HHH");
//    wxString _neq4 = wxT("A\0hH");
    s1.insert(1,1,'\0');
    eq.insert(1,1,'\0');
    neq1.insert(1,1,'\0');
    neq2.insert(1,1,'\0');
    neq3.insert(1,1,'\0');
    neq4.insert(1,1,'\0');

    CPPUNIT_ASSERT( s1 == eq );
    CPPUNIT_ASSERT( s1 != neq1 );
    CPPUNIT_ASSERT( s1 != neq2 );
    CPPUNIT_ASSERT( s1 != neq3 );
    CPPUNIT_ASSERT( s1 != neq4 );
}

void StringTestCase::CompareNoCase()
{
    wxString s1 = wxT("AHH");
    wxString eq = wxT("AHH");
    wxString eq2 = wxT("AhH");
    wxString eq3 = wxT("ahh");
    wxString neq = wxT("HAH");
    wxString neq2 = wxT("AH");
    wxString neq3 = wxT("AHHH");

    #define CPPUNIT_CNCEQ_ASSERT(s1, s2) CPPUNIT_ASSERT( s1.CmpNoCase(s2) == 0)
    #define CPPUNIT_CNCNEQ_ASSERT(s1, s2) CPPUNIT_ASSERT( s1.CmpNoCase(s2) != 0)

    CPPUNIT_CNCEQ_ASSERT( s1, eq );
    CPPUNIT_CNCEQ_ASSERT( s1, eq2 );
    CPPUNIT_CNCEQ_ASSERT( s1, eq3 );

    CPPUNIT_CNCNEQ_ASSERT( s1, neq );
    CPPUNIT_CNCNEQ_ASSERT( s1, neq2 );
    CPPUNIT_CNCNEQ_ASSERT( s1, neq3 );


//    wxString _s1 = wxT("A\0HH");
//    wxString _eq = wxT("A\0HH");
//    wxString _eq2 = wxT("A\0hH");
//    wxString _eq3 = wxT("a\0hh");
//    wxString _neq = wxT("H\0AH");
//    wxString _neq2 = wxT("A\0H");
//    wxString _neq3 = wxT("A\0HHH");

    s1.insert(1,1,'\0');
    eq.insert(1,1,'\0');
    eq2.insert(1,1,'\0');
    eq3.insert(1,1,'\0');
    neq.insert(1,1,'\0');
    neq2.insert(1,1,'\0');
    neq3.insert(1,1,'\0');

    CPPUNIT_CNCEQ_ASSERT( s1, eq );
    CPPUNIT_CNCEQ_ASSERT( s1, eq2 );
    CPPUNIT_CNCEQ_ASSERT( s1, eq3 );

    CPPUNIT_CNCNEQ_ASSERT( s1, neq );
    CPPUNIT_CNCNEQ_ASSERT( s1, neq2 );
    CPPUNIT_CNCNEQ_ASSERT( s1, neq3 );
}

