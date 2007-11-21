///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/bstream.cpp
// Purpose:     House the base stream test suite.
// Author:      Hans Van Leemputten
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "bstream.h"

using CppUnit::TestSuite;
using CppUnit::Test;

///////////////////////////////////////////////////////////////////////////////
// Streams main test suite, it houses all stream test suites.
//

class StreamCase : public TestSuite
{
public:
    StreamCase()
        :TestSuite(STREAM_TEST_NAME)
    { /* Nothing extra */ }
    static Test *suite();
};

Test *StreamCase::suite()
{
    TestSuite *suite = new StreamCase;

    /*
     * Register all sub stream test suites.
     */

    STREAM_REGISTER_SUB_SUITE(memStream);
    STREAM_REGISTER_SUB_SUITE(strStream);
    STREAM_REGISTER_SUB_SUITE(fileStream);
    STREAM_REGISTER_SUB_SUITE(ffileStream);
    STREAM_REGISTER_SUB_SUITE(tempStream);
    STREAM_REGISTER_SUB_SUITE(zlibStream);
    STREAM_REGISTER_SUB_SUITE(backStream);

    extern CppUnit::Test* GetlargeFileSuite();
    Test *lfs = GetlargeFileSuite();
    if (lfs)
        suite->addTest(lfs);

    /*
    ** Add more stream subtests here
    */

    return suite;
}

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(StreamCase);
// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(StreamCase, STREAM_TEST_NAME);

