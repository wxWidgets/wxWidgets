///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/stdstrings.cpp
// Purpose:     wxString unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-05-07
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin, Wlodzimierz Skiba
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

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class StdStringTestCase : public CppUnit::TestCase
{
public:
    StdStringTestCase();

private:
    CPPUNIT_TEST_SUITE( StdStringTestCase );
        CPPUNIT_TEST( StdConstructors );
        CPPUNIT_TEST( StdAppend );
        CPPUNIT_TEST( StdAssign );
        CPPUNIT_TEST( StdCompare );
        CPPUNIT_TEST( StdErase );
        CPPUNIT_TEST( StdFind );
        CPPUNIT_TEST( StdFindFirst );
        CPPUNIT_TEST( StdFindLast );
        CPPUNIT_TEST( StdInsert );
        CPPUNIT_TEST( StdReplace );
        CPPUNIT_TEST( StdRFind );
        CPPUNIT_TEST( StdResize );
        CPPUNIT_TEST( StdRiter );
        CPPUNIT_TEST( StdSubstr );
    CPPUNIT_TEST_SUITE_END();

    void StdConstructors();
    void StdAppend();
    void StdAssign();
    void StdCompare();
    void StdErase();
    void StdFind();
    void StdFindFirst();
    void StdFindLast();
    void StdInsert();
    void StdReplace();
    void StdRFind();
    void StdResize();
    void StdRiter();
    void StdSubstr();

    DECLARE_NO_COPY_CLASS(StdStringTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( StdStringTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StdStringTestCase, "StdStringTestCase" );

StdStringTestCase::StdStringTestCase()
{
}

void StdStringTestCase::StdConstructors()
{
    wxString s1(_T("abcdefgh")),
             s2(_T("abcdefghijklm"), 8),
             s3(_T("abcdefghijklm")),
             s4(8, _T('a'));
    wxString s5(s1),
             s6(s3, 0, 8),
             s7(s3.begin(), s3.begin() + 8);
    wxString s8(s1, 4, 8);

    CPPUNIT_ASSERT( s1 == _T("abcdefgh") );
    CPPUNIT_ASSERT( s2 == s1 );
    CPPUNIT_ASSERT( s4 == _T("aaaaaaaa") );
    CPPUNIT_ASSERT( s5 == _T("abcdefgh") );
    CPPUNIT_ASSERT( s6 == s1 );
    CPPUNIT_ASSERT( s7 == s1 );
    CPPUNIT_ASSERT( s8 == _T("efgh") );
}

void StdStringTestCase::StdAppend()
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8;

    s1 = s2 = s3 = s4 = s5 = s6 = _T("abc");
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

    s7 = s8 = wxString(_T("null\0time"), 9);

    s7.append(_T("def"));
    s8.append(_T("defgh"), 3);

    CPPUNIT_ASSERT( s7 == wxString(_T("null\0timedef"), 12) );
    CPPUNIT_ASSERT( s8 == wxString(_T("null\0timedef"), 12) );
}

void StdStringTestCase::StdAssign()
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8;

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
}

void StdStringTestCase::StdCompare()
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8;

    s1 = _T("abcdefgh");
    s2 = _T("abcdefgh");
    s3 = _T("abc");
    s4 = _T("abcdefghi");
    s5 = _T("aaa");
    s6 = _T("zzz");

    CPPUNIT_ASSERT( s1.compare(s2) == 0 );
    CPPUNIT_ASSERT( s1.compare(s3) > 0 );
    CPPUNIT_ASSERT( s1.compare(s4) < 0 );
    CPPUNIT_ASSERT( s1.compare(s5) > 0 );
    CPPUNIT_ASSERT( s1.compare(s6) < 0 );
    CPPUNIT_ASSERT( s1.compare(1, 12, s1) > 0);
    CPPUNIT_ASSERT( s1.compare(_T("abcdefgh")) == 0);
    CPPUNIT_ASSERT( s1.compare(1, 7, _T("bcdefgh")) == 0);
    CPPUNIT_ASSERT( s1.compare(1, 7, _T("bcdefgh"), 7) == 0);
}

void StdStringTestCase::StdErase()
{
    wxString s1, s2, s3, s4, s5, s6, s7;

    s1 = _T("abcdefgh");
    s2 = _T("abcdefgh");
    s3 = _T("abc");
    s4 = _T("abcdefghi");
    s5 = _T("aaa");
    s6 = _T("zzz");
    s7 = _T("zabcdefg");

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
}

void StdStringTestCase::StdFind()
{
    //                0         1         2
    //                01234567890123456789012345
    wxString s1 = _T("abcdefgABCDEFGabcABCabcABC");
    wxString s2 = _T("gAB");

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

    //                   0          1          2
    //                   0123456 78901234567 8901234567
    //wxString _s1 = _T("abcdefg\0ABCDEFGabc\0ABCabcABC");
    //wxString _s2 = _T("g\0AB");
    wxString _s1 = _T("abcdefgABCDEFGabcABCabcABC");
    wxString _s2 = _T("gAB");

    _s1.insert(7, 1, '\0');
    _s1.insert(18, 1, '\0');
    _s2.insert(1, 1, '\0');

    CPPUNIT_ASSERT( _s1.find(_T('A')) == 8u );
    CPPUNIT_ASSERT( _s1.find(_T('A'), 8) == 8u );
    CPPUNIT_ASSERT( _s1.find(_T('Z')) == wxString::npos );
    CPPUNIT_ASSERT( _s1.find(_T('C'), 22) == 27u );

    CPPUNIT_ASSERT( _s1.find(_T("AB")) == 8u );
    CPPUNIT_ASSERT( _s1.find(_T("AB"), 26) == wxString::npos );
    CPPUNIT_ASSERT( _s1.find(_T("AB"), 23) == 25u );

    CPPUNIT_ASSERT( _s1.find(_T("ABZZZ"), 2, 2) == 8u );
    CPPUNIT_ASSERT( _s1.find(_T("ABZZZ"), 26, 2) == wxString::npos );

    CPPUNIT_ASSERT( _s1.find(_s2) == 6u );
    CPPUNIT_ASSERT( _s1.find(_s2, 7) == wxString::npos );
    CPPUNIT_ASSERT( _s1.find(_s2, 6) == 6u );
}

void StdStringTestCase::StdFindFirst()
{
    //                0         1         2         3
    //                01234567890123456789012345678901234
    wxString s1 = _T("aaaaaabcdefghlkjiaaaaaabcdbcdbcdbcd");
    wxString s2 = _T("aaaaaa");

    CPPUNIT_ASSERT( s1.find_first_not_of(_T('a')) == 6u );
    CPPUNIT_ASSERT( s1.find_first_not_of(_T('a'), 7) == 7u );
    CPPUNIT_ASSERT( s2.find_first_not_of(_T('a')) == wxString::npos );

    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abde"), 4) == 7u );
    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abde"), 7) == 7u );
    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abcdefghijkl")) == wxString::npos );

    CPPUNIT_ASSERT( s1.find_first_not_of(_T("abcdefghi"), 0, 4) == 9u );

    CPPUNIT_ASSERT( s1.find_first_of(_T('c')) == 7u );
    CPPUNIT_ASSERT( s1.find_first_of(_T('v')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_first_of(_T('c'), 10) == 24u );

    CPPUNIT_ASSERT( s1.find_first_of(_T("ijkl")) == 13u );
    CPPUNIT_ASSERT( s1.find_first_of(_T("ddcfg"), 17) == 24u );
    CPPUNIT_ASSERT( s1.find_first_of(_T("ddcfga"), 17, 5) == 24u );
}

void StdStringTestCase::StdFindLast()
{
    //                0         1         2         3
    //                01234567890123456789012345678901234
    wxString s1 = _T("aaaaaabcdefghlkjiaaaaaabcdbcdbcdbcd");
    wxString s2 = _T("aaaaaa");

    CPPUNIT_ASSERT( s2.find_last_not_of(_T('a')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T('d')) == 33u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T('d'), 25) == 24u );

    CPPUNIT_ASSERT( s1.find_last_not_of(_T("bcd")) == 22u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T("abc"), 24) == 16u );

    CPPUNIT_ASSERT( s1.find_last_not_of(_T("abcdefghijklmnopqrstuv"), 24, 3) == 16u );

    CPPUNIT_ASSERT( s2.find_last_of(_T('c')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_last_of(_T('a')) == 22u );
    CPPUNIT_ASSERT( s1.find_last_of(_T('b'), 24) == 23u );

    CPPUNIT_ASSERT( s1.find_last_of(_T("ijklm")) == 16u );
    CPPUNIT_ASSERT( s1.find_last_of(_T("ijklma"), 33, 4) == 16u );
    CPPUNIT_ASSERT( s1.find_last_of(_T("a"), 17) == 17u );



    //                  0          1          2         3
    //                  012345 67890123456789 01234567890123456
//    wxString s1 = _T("aaaaaa\0bcdefghlkjiaa\0aaaabcdbcdbcdbcd");
//    wxString s2 = _T("aaaaaa\0");
    s1.insert(6,1,'\0');
    s1.insert(20,1,'\0');
    s2.insert(6,1,'\0');

    CPPUNIT_ASSERT( s2.find_last_not_of(_T('a')) == 6u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T('d')) == 35u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T('d'), 27) == 26u );

    CPPUNIT_ASSERT( s1.find_last_not_of(_T("bcd")) == 24u );
    CPPUNIT_ASSERT( s1.find_last_not_of(_T("abc"), 26) == 20u );

    CPPUNIT_ASSERT( s1.find_last_not_of(_T("abcdefghijklmnopqrstuv"), 26, 3) == 20u );

    CPPUNIT_ASSERT( s2.find_last_of(_T('c')) == wxString::npos );
    CPPUNIT_ASSERT( s1.find_last_of(_T('a')) == 24u );
    CPPUNIT_ASSERT( s1.find_last_of(_T('b'), 26) == 25u );

    CPPUNIT_ASSERT( s1.find_last_of(_T("ijklm")) == 17u );
    CPPUNIT_ASSERT( s1.find_last_of(_T("ijklma"), 35, 4) == 17u );
    CPPUNIT_ASSERT( s1.find_last_of(_T("a"), 18) == 18u );
}

void StdStringTestCase::StdInsert()
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = _T("aaaa");
    s9 = s10 = _T("cdefg");

    s1.insert(1, _T("cc") );
    s2.insert(2, _T("cdef"), 3);
    s3.insert(2, s10);
    s4.insert(2, s10, 3, 7);
    s5.insert(1, 2, _T('c'));
    s6.insert(s6.begin() + 3, _T('X'));
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
}

void StdStringTestCase::StdReplace()
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8, s9;

    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = _T("QWERTYUIOP");
    s9 = _T("werty");

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
}

void StdStringTestCase::StdRFind()
{
    //                0         1         2
    //                01234567890123456789012345
    wxString s1 = _T("abcdefgABCDEFGabcABCabcABC");
    wxString s2 = _T("gAB");
    wxString s3 = _T("ab");

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


    //                  0          1          2
    //                  01234 56789012 345678901234567
//    wxString s1 = _T("abcde\0fgABCDE\0FGabcABCabcABC");
//    wxString s2 = _T("gAB");
//    wxString s3 = _T("ab");

    s1.insert(5,1,'\0');
    s1.insert(13,1,'\0');

    CPPUNIT_ASSERT( s1.rfind(_T('A')) == 25u );
    CPPUNIT_ASSERT( s1.rfind(_T('A'), 8) == 8u );
    CPPUNIT_ASSERT( s1.rfind(_T('Z')) == wxString::npos );
    CPPUNIT_ASSERT( s1.rfind(_T('C'), 22) == 21u );

    CPPUNIT_ASSERT( s1.rfind(_T("cAB")) == 24u );
    CPPUNIT_ASSERT( s1.rfind(_T("cAB"), 15) == wxString::npos );
    CPPUNIT_ASSERT( s1.rfind(_T("cAB"), 21) == 18u );

    CPPUNIT_ASSERT( s1.rfind(_T("gABZZZ"), 8, 3) == 7u );
    CPPUNIT_ASSERT( s1.rfind(_T("gABZZZ"), 5, 3) == wxString::npos );
}

void StdStringTestCase::StdResize()
{
    wxString s1, s2, s3, s4;

    s1 = s2 = s3 = s4 = _T("abcABCdefDEF");

    s1.resize( 12 );
    s2.resize( 10 );
    s3.resize( 14, _T(' ') );
    s4.resize( 14, _T('W') );

    CPPUNIT_ASSERT( s1 == _T("abcABCdefDEF") );
    CPPUNIT_ASSERT( s2 == _T("abcABCdefD") );
    CPPUNIT_ASSERT( s3 == _T("abcABCdefDEF  ") );
    CPPUNIT_ASSERT( s4 == _T("abcABCdefDEFWW") );
}

void StdStringTestCase::StdRiter()
{
    const wxString s(_T("fozbar"));

    wxString::const_reverse_iterator ri(s.rbegin());
    CPPUNIT_ASSERT( _T('r') == *ri );
    CPPUNIT_ASSERT( _T('a') == *++ri );
    CPPUNIT_ASSERT( _T('r') == *--ri );

    ri = s.rend();
    ri--;
    CPPUNIT_ASSERT( _T('f') == *ri );

    --ri;
    CPPUNIT_ASSERT( _T('o') == *ri );

    wxString::const_iterator i = ri.base();
    CPPUNIT_ASSERT( _T('z') == *i );
}

void StdStringTestCase::StdSubstr()
{
    wxString s1 = _T("abcdefgABCDEFG");

    CPPUNIT_ASSERT( s1.substr( 0, 14 ) == s1 );
    CPPUNIT_ASSERT( s1.substr( 1, 13 ) == _T("bcdefgABCDEFG") );
    CPPUNIT_ASSERT( s1.substr( 1, 20 ) == _T("bcdefgABCDEFG") );
    CPPUNIT_ASSERT( s1.substr( 14, 30 ) == _T("") );

    s1.insert(3,1,'\0');
    s1.insert(8,1,'\0');
    s1.insert(13,1,'\0');

    wxString s2 = wxT("bcdefgABCDEFG");
    s2.insert(2,1,'\0');
    s2.insert(7,1,'\0');
    s2.insert(12,1,'\0');

    CPPUNIT_ASSERT( s1.substr( 0, 17 ) == s1 );
    CPPUNIT_ASSERT( s1.substr( 1, 17 ) == s2 );
    CPPUNIT_ASSERT( s1.substr( 1, 20 ) == s2 );
    CPPUNIT_ASSERT( s1.substr( 17, 30 ) == _T("") );
}

