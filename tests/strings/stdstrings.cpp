//////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/stdstrings.cpp
// Purpose:     wxString unit test
// Author:      Vadim Zeitlin, Wlodzimierz ABX Skiba
// Created:     2004-05-07
// Copyright:   (c) 2004 Vadim Zeitlin, Wlodzimierz Skiba
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <algorithm>

TEST_CASE("StdString::Constructors", "[stdstring]")
{
    wxString s1(wxT("abcdefgh")),
             s2(wxT("abcdefghijklm"), 8),
             s3(wxT("abcdefghijklm")),
             s4(8, wxT('a'));
    wxString s5(s1),
             s6(s3, 0, 8),
             s7(s3.begin(), s3.begin() + 8);
    wxString s8(s1, 4, 8);

    CHECK( s1 == wxT("abcdefgh") );
    CHECK( s2 == s1 );
    CHECK( s4 == wxT("aaaaaaaa") );
    CHECK( s5 == wxT("abcdefgh") );
    CHECK( s6 == s1 );
    CHECK( s7 == s1 );
    CHECK( s8 == wxT("efgh") );

    const char *pc = s1.c_str();
    CHECK( wxString(pc + 1, pc + 4) == "bcd" );

    const wchar_t *pw = s2.c_str();
    CHECK( wxString(pw, pw + 1) == "a" );
}

TEST_CASE("StdString::Iterators", "[stdstring]")
{
    // test compilation of default iterators ctors:
    wxString::iterator i1;
    wxString::const_iterator i2;
    wxString::reverse_iterator i3;
    wxString::const_reverse_iterator i4;
}

TEST_CASE("StdString::IteratorsCmp", "[stdstring]")
{
    wxString s("foobar");
    wxString::iterator i = s.begin();
    wxString::const_iterator ci = s.begin();

    CHECK( i == ci );
    CHECK( i >= ci );
    CHECK( i <= ci );
    CHECK( ci == i );
    CHECK( ci >= i );
    CHECK( ci <= i );

    ci++;

    CHECK( i != ci );
    CHECK( i < ci );
    CHECK( !(i > ci) );

    CHECK( ci != i );
    CHECK( ci > i );
    CHECK( !(ci < i) );
}

TEST_CASE("StdString::Append", "[stdstring]")
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8;

    s1 = s2 = s3 = s4 = s5 = s6 = wxT("abc");
    s1.append(wxT("def"));
    s2.append(wxT("defgh"), 3);
    s3.append(wxString(wxT("abcdef")), 3, 6);
    s4.append(s1);
    s5.append(3, wxT('a'));
    s5.append(2, 'x');
    s5.append(1, (unsigned char)'y');
    s6.append(s1.begin() + 3, s1.end());

    CHECK( s1 == wxT("abcdef") );
    CHECK( s2 == wxT("abcdef") );
    CHECK( s3 == wxT("abcdef") );
    CHECK( s4 == wxT("abcabcdef") );
    CHECK( s5 == wxT("abcaaaxxy") );
    CHECK( s6 == wxT("abcdef") );

    const char *pc = s1.c_str() + 2;
    s7.append(pc, pc + 4);
    CHECK( s7 == "cdef" );

    const wchar_t *pw = s2.c_str() + 2;
    s8.append(pw, pw + 4);
    CHECK( s8 == "cdef" );

    s7 = s8 = wxString(wxT("null\0time"), 9);

    s7.append(wxT("def"));
    s8.append(wxT("defgh"), 3);

    CHECK( s7 == wxString(wxT("null\0timedef"), 12) );
    CHECK( s8 == wxString(wxT("null\0timedef"), 12) );
}

TEST_CASE("StdString::Assign", "[stdstring]")
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8;

    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = wxT("abc");
    s1.assign(wxT("def"));
    s2.assign(wxT("defgh"), 3);
    s3.assign(wxString(wxT("abcdef")), 3, 6);
    s4.assign(s1);
    s5.assign(3, wxT('a'));
    s6.assign(s1.begin() + 1, s1.end());

    CHECK( s1 == wxT("def") );
    CHECK( s2 == wxT("def") );
    CHECK( s3 == wxT("def") );
    CHECK( s4 == wxT("def") );
    CHECK( s5 == wxT("aaa") );
    CHECK(  s6 == wxT("ef") );

    const char *pc = s1.c_str();
    s7.assign(pc, pc + 2);
    CHECK( s7 == "de" );

    const wchar_t *pw = s1.c_str();
    s8.assign(pw + 2, pw + 3);
    CHECK( s8 == "f" );

    s1.assign(s1, 1, 1);
    CHECK( s1 == "e" );
}

TEST_CASE("StdString::Compare", "[stdstring]")
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8;

    s1 = wxT("abcdefgh");
    s2 = wxT("abcdefgh");
    s3 = wxT("abc");
    s4 = wxT("abcdefghi");
    s5 = wxT("aaa");
    s6 = wxT("zzz");

    CHECK( s1.compare(s2) == 0 );
    CHECK( s1.compare(s3) > 0 );
    CHECK( s1.compare(s4) < 0 );
    CHECK( s1.compare(s5) > 0 );
    CHECK( s1.compare(s6) < 0 );
    CHECK( s1.compare(1, 12, s1) > 0);
    CHECK( s1.compare(wxT("abcdefgh")) == 0);
    CHECK( s1.compare(1, 7, wxT("bcdefgh")) == 0);
    CHECK( s1.compare(1, 7, wxT("bcdefgh"), 7) == 0);
}

TEST_CASE("StdString::Erase", "[stdstring]")
{
    wxString s1, s2, s3, s4, s5, s6, s7;

    s1 = wxT("abcdefgh");
    s2 = wxT("abcdefgh");
    s3 = wxT("abc");
    s4 = wxT("abcdefghi");
    s5 = wxT("aaa");
    s6 = wxT("zzz");
    s7 = wxT("zabcdefg");

    s1.erase(1, 1);
    s2.erase(4, 12);
    wxString::iterator it = s3.erase(s3.begin() + 1);
    wxString::iterator it2 = s4.erase(s4.begin() + 4, s4.begin() + 6);
    wxString::iterator it3 = s7.erase(s7.begin() + 4, s7.begin() + 8);

    CHECK( s1 == wxT("acdefgh") );
    CHECK( s2 == wxT("abcd") );
    CHECK( s3 == wxT("ac") );
    CHECK( s4 == wxT("abcdghi") );
    CHECK( s7 == wxT("zabc") );
    CHECK( *it == wxT('c') );
    CHECK( *it2 == wxT('g') );
    CHECK( it3 == s7.end() );
}

TEST_CASE("StdString::Find", "[stdstring]")
{
    //                0         1         2
    //                01234567890123456789012345
    wxString s1 = wxT("abcdefgABCDEFGabcABCabcABC");
    wxString s2 = wxT("gAB");

    CHECK( s1.find(wxT('A')) == 7u );
    CHECK( s1.find(wxT('A'), 7) == 7u );
    CHECK( s1.find(wxT('Z')) == wxString::npos );
    CHECK( s1.find(wxT('C'), 22) == 25u );

    CHECK( s1.find(wxT("gAB")) == 6u );
    CHECK( s1.find(wxT("gAB"), 7) == wxString::npos );
    CHECK( s1.find(wxT("gAB"), 6) == 6u );

    CHECK( s1.find(wxT("gABZZZ"), 2, 3) == 6u );
    CHECK( s1.find(wxT("gABZZZ"), 7, 3) == wxString::npos );

    CHECK( s1.find(s2) == 6u );
    CHECK( s1.find(s2, 7) == wxString::npos );
    CHECK( s1.find(s2, 6) == 6u );

    //                   0          1          2
    //                   0123456 78901234567 8901234567
    //wxString _s1 = wxT("abcdefg\0ABCDEFGabc\0ABCabcABC");
    //wxString _s2 = wxT("g\0AB");
    wxString _s1 = wxT("abcdefgABCDEFGabcABCabcABC");
    wxString _s2 = wxT("gAB");

    _s1.insert(7, 1, '\0');
    _s1.insert(18, 1, '\0');
    _s2.insert(1, 1, '\0');

    CHECK( _s1.find(wxT('A')) == 8u );
    CHECK( _s1.find(wxT('A'), 8) == 8u );
    CHECK( _s1.find(wxT('Z')) == wxString::npos );
    CHECK( _s1.find(wxT('C'), 22) == 27u );

    CHECK( _s1.find(wxT("AB")) == 8u );
    CHECK( _s1.find(wxT("AB"), 26) == wxString::npos );
    CHECK( _s1.find(wxT("AB"), 23) == 25u );

    CHECK( _s1.find(wxT("ABZZZ"), 2, 2) == 8u );
    CHECK( _s1.find(wxT("ABZZZ"), 26, 2) == wxString::npos );

    CHECK( _s1.find(_s2) == 6u );
    CHECK( _s1.find(_s2, 7) == wxString::npos );
    CHECK( _s1.find(_s2, 6) == 6u );
}

TEST_CASE("StdString::FindFirst", "[stdstring]")
{
    //                0         1         2         3
    //                01234567890123456789012345678901234
    wxString s1 = wxT("aaaaaabcdefghlkjiaaaaaabcdbcdbcdbcd");
    wxString s2 = wxT("aaaaaa");

    CHECK( s1.find_first_not_of(wxT('a')) == 6u );
    CHECK( s1.find_first_not_of(wxT('a'), 7) == 7u );
    CHECK( s2.find_first_not_of(wxT('a')) == wxString::npos );

    CHECK( s1.find_first_not_of(wxT("abde"), 4) == 7u );
    CHECK( s1.find_first_not_of(wxT("abde"), 7) == 7u );
    CHECK( s1.find_first_not_of(wxT("abcdefghijkl")) == wxString::npos );

    CHECK( s1.find_first_not_of(wxT("abcdefghi"), 0, 4) == 9u );

    CHECK( s1.find_first_of(wxT('c')) == 7u );
    CHECK( s1.find_first_of(wxT('v')) == wxString::npos );
    CHECK( s1.find_first_of(wxT('c'), 10) == 24u );

    CHECK( s1.find_first_of(wxT("ijkl")) == 13u );
    CHECK( s1.find_first_of(wxT("ddcfg"), 17) == 24u );
    CHECK( s1.find_first_of(wxT("ddcfga"), 17, 5) == 24u );
}

TEST_CASE("StdString::FindLast", "[stdstring]")
{
    //                0         1         2         3
    //                01234567890123456789012345678901234
    wxString s1 = wxT("aaaaaabcdefghlkjiaaaaaabcdbcdbcdbcd");
    wxString s2 = wxT("aaaaaa");

    CHECK( s2.find_last_not_of(wxT('a')) == wxString::npos );
    CHECK( s1.find_last_not_of(wxT('d')) == 33u );
    CHECK( s1.find_last_not_of(wxT('d'), 25) == 24u );

    CHECK( s1.find_last_not_of(wxT("bcd")) == 22u );
    CHECK( s1.find_last_not_of(wxT("abc"), 24) == 16u );

    CHECK( s1.find_last_not_of(wxT("abcdefghijklmnopqrstuv"), 24, 3) == 16u );

    CHECK( s2.find_last_of(wxT('c')) == wxString::npos );
    CHECK( s1.find_last_of(wxT('a')) == 22u );
    CHECK( s1.find_last_of(wxT('b'), 24) == 23u );

    CHECK( s1.find_last_of(wxT("ijklm")) == 16u );
    CHECK( s1.find_last_of(wxT("ijklma"), 33, 4) == 16u );
    CHECK( s1.find_last_of(wxT("a"), 17) == 17u );



    //                  0          1          2         3
    //                  012345 67890123456789 01234567890123456
//    wxString s1 = wxT("aaaaaa\0bcdefghlkjiaa\0aaaabcdbcdbcdbcd");
//    wxString s2 = wxT("aaaaaa\0");
    s1.insert(6,1,'\0');
    s1.insert(20,1,'\0');
    s2.insert(6,1,'\0');

    CHECK( s2.find_last_not_of(wxT('a')) == 6u );
    CHECK( s1.find_last_not_of(wxT('d')) == 35u );
    CHECK( s1.find_last_not_of(wxT('d'), 27) == 26u );

    CHECK( s1.find_last_not_of(wxT("bcd")) == 24u );
    CHECK( s1.find_last_not_of(wxT("abc"), 26) == 20u );

    CHECK( s1.find_last_not_of(wxT("abcdefghijklmnopqrstuv"), 26, 3) == 20u );

    CHECK( s2.find_last_of(wxT('c')) == wxString::npos );
    CHECK( s1.find_last_of(wxT('a')) == 24u );
    CHECK( s1.find_last_of(wxT('b'), 26) == 25u );

    CHECK( s1.find_last_of(wxT("ijklm")) == 17u );
    CHECK( s1.find_last_of(wxT("ijklma"), 35, 4) == 17u );
    CHECK( s1.find_last_of(wxT("a"), 18) == 18u );
}

TEST_CASE("StdString::StartsEndsWith", "[stdstring]")
{
    const wxString s(wxT("Hello, world!"));
    CHECK( s.starts_with(wxT("Hello")) == true );
    CHECK( s.starts_with(wxT("Hello, ")) == true );
    CHECK( s.starts_with(wxT("Hello, world!")) == true );
    CHECK( s.starts_with(wxT("Hello, world!!!")) == false );
    CHECK( s.starts_with(wxT("")) == true );
    CHECK( s.starts_with(wxT("Goodbye")) == false );
    CHECK( s.starts_with(wxT("Hi")) == false );

    CHECK( s.ends_with(wxT("Hello, world!")) == true );
    CHECK( s.ends_with(wxT("world!")) == true );
    CHECK( s.ends_with(wxT("Hello")) == false );
    CHECK( s.ends_with(wxT("!")) == true );
    CHECK( s.ends_with(wxT("")) == true );
    CHECK( s.ends_with(wxT("very long string")) == false );
    CHECK( s.ends_with(wxT("?")) == false );
    CHECK( s.ends_with(wxT("Hello, world")) == false );
    CHECK( s.ends_with(wxT("Gello, world!")) == false );
}

TEST_CASE("StdString::Insert", "[stdstring]")
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = wxT("aaaa");
    s9 = s10 = wxT("cdefg");

    s1.insert(1, wxT("cc") );
    s2.insert(2, wxT("cdef"), 3);
    s3.insert(2, s10);
    s4.insert(2, s10, 3, 7);
    s5.insert(1, 2, wxT('c'));
    s6.insert(s6.begin() + 3, wxT('X'));
    s7.insert(s7.begin(), s9.begin(), s9.end() - 1);
    s8.insert(s8.begin(), 2, wxT('c'));

    CHECK( s1 == wxT("accaaa")    );
    CHECK( s2 == wxT("aacdeaa")   );
    CHECK( s3 == wxT("aacdefgaa") );
    CHECK( s4 == wxT("aafgaa")    );
    CHECK( s5 == wxT("accaaa")    );
    CHECK( s6 == wxT("aaaXa")     );
    CHECK( s7 == wxT("cdefaaaa")  );
    CHECK( s8 == wxT("ccaaaa")    );

    s1 = s2 = s3 = wxT("aaaa");
    s1.insert(0, wxT("ccc"), 2);
    s2.insert(4, wxT("ccc"), 2);

    CHECK( s1 == wxT("ccaaaa") );
    CHECK( s2 == wxT("aaaacc") );
}

TEST_CASE("StdString::Replace", "[stdstring]")
{
    wxString s1, s2, s3, s4, s5, s6, s7, s8, s9;

    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = wxT("QWERTYUIOP");
    s9 = wxT("werty");

    s1.replace(3, 4, wxT("rtyu"));
    s1.replace(8, 7, wxT("opopop"));
    s2.replace(10, 12, wxT("WWWW"));
    s3.replace(1, 5, s9);
    s4.replace(1, 4, s9, 0, 4);
    s5.replace(1, 2, s9, 1, 12);
    s6.replace(0, 123, s9, 0, 123);
    s7.replace(2, 7, s9);

    CHECK( s1 == wxT("QWErtyuIopopop") );
    CHECK( s2 == wxT("QWERTYUIOPWWWW") );
    CHECK( s3 == wxT("QwertyUIOP")     );
    CHECK( s4 == wxT("QwertYUIOP")     );
    CHECK( s5 == wxT("QertyRTYUIOP")   );
    CHECK( s6 == s9 );
    CHECK( s7 == wxT("QWwertyP") );
}

TEST_CASE("StdString::RFind", "[stdstring]")
{
    //                0         1         2
    //                01234567890123456789012345
    wxString s1 = wxT("abcdefgABCDEFGabcABCabcABC");
    wxString s2 = wxT("gAB");
    wxString s3 = wxT("ab");

    CHECK( s1.rfind(wxT('A')) == 23u );
    CHECK( s1.rfind(wxT('A'), 7) == 7u );
    CHECK( s1.rfind(wxT('Z')) == wxString::npos );
    CHECK( s1.rfind(wxT('C'), 22) == 19u );

    CHECK( s1.rfind(wxT("cAB")) == 22u );
    CHECK( s1.rfind(wxT("cAB"), 15) == wxString::npos );
    CHECK( s1.rfind(wxT("cAB"), 21) == 16u );

    CHECK( s1.rfind(wxT("gABZZZ"), 7, 3) == 6u );
    CHECK( s1.rfind(wxT("gABZZZ"), 5, 3) == wxString::npos );

    CHECK( s1.rfind(s2) == 6u );
    CHECK( s1.rfind(s2, 5) == wxString::npos );
    CHECK( s1.rfind(s2, 6) == 6u );
    CHECK( s1.rfind(s3, 1) == 0u );


    //                  0          1          2
    //                  01234 56789012 345678901234567
//    wxString s1 = wxT("abcde\0fgABCDE\0FGabcABCabcABC");
//    wxString s2 = wxT("gAB");
//    wxString s3 = wxT("ab");

    s1.insert(5,1,'\0');
    s1.insert(13,1,'\0');

    CHECK( s1.rfind(wxT('A')) == 25u );
    CHECK( s1.rfind(wxT('A'), 8) == 8u );
    CHECK( s1.rfind(wxT('Z')) == wxString::npos );
    CHECK( s1.rfind(wxT('C'), 22) == 21u );

    CHECK( s1.rfind(wxT("cAB")) == 24u );
    CHECK( s1.rfind(wxT("cAB"), 15) == wxString::npos );
    CHECK( s1.rfind(wxT("cAB"), 21) == 18u );

    CHECK( s1.rfind(wxT("gABZZZ"), 8, 3) == 7u );
    CHECK( s1.rfind(wxT("gABZZZ"), 5, 3) == wxString::npos );
}

TEST_CASE("StdString::Resize", "[stdstring]")
{
    wxString s1, s2, s3, s4;

    s1 = s2 = s3 = s4 = wxT("abcABCdefDEF");

    s1.resize( 12 );
    s2.resize( 10 );
    s3.resize( 14, wxT(' ') );
    s4.resize( 14, wxT('W') );

    CHECK( s1 == wxT("abcABCdefDEF") );
    CHECK( s2 == wxT("abcABCdefD") );
    CHECK( s3 == wxT("abcABCdefDEF  ") );
    CHECK( s4 == wxT("abcABCdefDEFWW") );

#if wxUSE_UNICODE
    wxString s =
        wxString::FromUTF8("\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
    s.resize(3);
    CHECK( s == wxString::FromUTF8("\xd0\x9f\xd1\x80\xd0\xb8") );
#endif // wxUSE_UNICODE
}

TEST_CASE("StdString::Riter", "[stdstring]")
{
    const wxString s(wxT("fozbar"));

    wxString::const_reverse_iterator ri(s.rbegin());
    CHECK( wxT('r') == *ri );
    CHECK( wxT('a') == *++ri );
    CHECK( wxT('r') == *--ri );

    ri = s.rend();
    ri--;
    CHECK( wxT('f') == *ri );

    --ri;
    CHECK( wxT('o') == *ri );

    wxString::const_iterator i = ri.base();
    CHECK( wxT('z') == *i );
}

TEST_CASE("StdString::Substr", "[stdstring]")
{
    wxString s1 = wxT("abcdefgABCDEFG");

    CHECK( s1.substr( 0, 14 ) == s1 );
    CHECK( s1.substr( 1, 13 ) == wxT("bcdefgABCDEFG") );
    CHECK( s1.substr( 1, 20 ) == wxT("bcdefgABCDEFG") );
    CHECK( s1.substr( 14, 30 ) == wxT("") );

    s1.insert(3,1,'\0');
    s1.insert(8,1,'\0');
    s1.insert(13,1,'\0');

    wxString s2 = wxT("bcdefgABCDEFG");
    s2.insert(2,1,'\0');
    s2.insert(7,1,'\0');
    s2.insert(12,1,'\0');

    CHECK( s1.substr( 0, 17 ) == s1 );
    CHECK( s1.substr( 1, 17 ) == s2 );
    CHECK( s1.substr( 1, 20 ) == s2 );
    CHECK( s1.substr( 17, 30 ) == wxT("") );
}

#if wxUSE_STD_STRING
TEST_CASE("StdString::Conversion", "[stdstring]")
{
    std::string strStd("std::string value");
    wxStdWideString strStdWide(L"std::wstring value");

    wxString s1(strStd);
    CHECK( s1 == "std::string value" );

    wxString s2(strStdWide);
    CHECK( s2 == "std::wstring value" );

    wxString s3;
    s3 = strStd;
    CHECK( s3 == "std::string value" );
    s3 = strStdWide;
    CHECK( s3 == "std::wstring value" );

    wxString s4("hello");

    // notice that implicit wxString -> std::string conversion is only
    // available in wxUSE_STL case, because it conflicts with conversion to
    // const char*/wchar_t*
#if wxUSE_STL && wxUSE_UNSAFE_WXSTRING_CONV
    std::string s5 = s4;
#else
    std::string s5 = s4.ToStdString();
#endif
    CHECK( s5 == "hello" );

#if wxUSE_STL
    wxStdWideString s6 = s4;
#else
    wxStdWideString s6 = s4.ToStdWstring();
#endif
    CHECK( s6 == "hello" );

#if wxUSE_UNSAFE_WXSTRING_CONV
    std::string s7(s4);
    CHECK( s7 == "hello" );
#endif

    wxStdWideString s8(s4);
    CHECK( s8 == "hello" );

#if wxUSE_UNICODE
    std::string s9("\xF0\x9F\x90\xB1\0\xE7\x8C\xAB", 9); /* U+1F431 U+0000 U+732B */
    wxString s10 = wxString::FromUTF8(s9);
    CHECK( s10.ToStdString(wxConvUTF8) == s9 );
    CHECK( s10.utf8_string() == s9 );
#endif // wxUSE_UNICODE

    std::string s11("xyz\0\xFF", 5); /* an invalid UTF-8 sequence */
    CHECK( "" == wxString::FromUTF8(s11) );

    CHECK( wxString("bye").utf8_string() == std::string("bye") );
}
#endif // wxUSE_STD_STRING

TEST_CASE("StdString::Algo", "[stdstring]")
{
    wxString s("AB");
    std::reverse(s.begin(), s.end());
    CHECK( s == "BA" );
}
