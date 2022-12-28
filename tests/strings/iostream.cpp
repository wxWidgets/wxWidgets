//////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/iostream.cpp
// Purpose:     unit test of wxString interaction with std::[io]stream
// Author:      Vadim Zeitlin
// Created:     2007-10-09
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif // WX_PRECOMP

#if wxUSE_STD_IOSTREAM

#include <sstream>

#define ASSERT_OSTREAM_EQUAL(p, s) CPPUNIT_ASSERT_EQUAL(std::string(p), s.str())
#define ASSERT_WOSTREAM_EQUAL(p, s) CPPUNIT_ASSERT_EQUAL(std::wstring(p), s.str())

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class StringIostreamTestCase : public CppUnit::TestCase
{
public:
    StringIostreamTestCase() { }

private:
    CPPUNIT_TEST_SUITE( StringIostreamTestCase );
        CPPUNIT_TEST( Out );
    CPPUNIT_TEST_SUITE_END();

    void Out();
};

CPPUNIT_TEST_SUITE_REGISTRATION( StringIostreamTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( StringIostreamTestCase, "StringIostream" );

void StringIostreamTestCase::Out()
{
    std::ostringstream s;
    s << wxString("hello");
    ASSERT_OSTREAM_EQUAL("hello", s);

#if defined(HAVE_WOSTREAM)
    std::wostringstream ws;
    ws << wxString("bye");
    ASSERT_WOSTREAM_EQUAL(L"bye", ws);
#endif
}

#endif // wxUSE_STD_IOSTREAM
