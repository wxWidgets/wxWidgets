///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/vararg.cpp
// Purpose:     Test for wx vararg look-alike macros
// Author:      Vaclav Slavik
// Created:     2007-02-20
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
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

#include "wx/string.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class VarArgTestCase : public CppUnit::TestCase
{
public:
    VarArgTestCase() {}

private:
    CPPUNIT_TEST_SUITE( VarArgTestCase );
        CPPUNIT_TEST( StringPrintf );
#if wxUSE_STD_STRING
        CPPUNIT_TEST( StdString );
#endif
    CPPUNIT_TEST_SUITE_END();

    void StringPrintf();
#if wxUSE_STD_STRING
    void StdString();
#endif

    DECLARE_NO_COPY_CLASS(VarArgTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VarArgTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VarArgTestCase, "VarArgTestCase" );

void VarArgTestCase::StringPrintf()
{
    wxString s, s2;

    // test passing literals:
    s.Printf("%s %i", "foo", 42);
    CPPUNIT_ASSERT( s == "foo 42" );
    s.Printf("%s %s %i", _T("bar"), "=", 11);

    // test passing c_str():
    CPPUNIT_ASSERT( s == "bar = 11" );
    s2.Printf("(%s)", s.c_str());
    CPPUNIT_ASSERT( s2 == "(bar = 11)" );
    s2.Printf(_T("[%s](%s)"), s.c_str(), "str");
    CPPUNIT_ASSERT( s2 == "[bar = 11](str)" );

    // test passing wxString directly:
    s2.Printf(_T("[%s](%s)"), s, "str");
    CPPUNIT_ASSERT( s2 == "[bar = 11](str)" );

    // test passing wxCharBufferType<T>:
    s = "FooBar";
    s2.Printf(_T("(%s)"), s.mb_str());
    CPPUNIT_ASSERT( s2 == "(FooBar)" );
    s2.Printf(_T("value=%s;"), s.wc_str());
    CPPUNIT_ASSERT( s2 == "value=FooBar;" );

    // this tests correct passing of wxCStrData constructed from string
    // literal:
    bool cond = true;
    s2.Printf(_T("foo %s"), !cond ? s.c_str() : _T("bar"));

}

#if wxUSE_STD_STRING
void VarArgTestCase::StdString()
{
    // test passing std::[w]string
    wxString s;

    std::string mb("multi-byte");
    std::string wc("widechar");

    s.Printf("string %s(%i).", mb, 1);
    CPPUNIT_ASSERT( s == "string multi-byte(1)." );

    s.Printf("string %s(%i).", wc, 2);
    CPPUNIT_ASSERT( s == "string widechar(2)." );
}
#endif // wxUSE_STD_STRING
