///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/tls.cpp
// Purpose:     wxTlsValue unit test
// Author:      Vadim Zeitlin
// Created:     2008-08-28
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/thread.h"
#include "wx/tls.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

namespace
{

// NB: this struct must be a POD, so don't use wxString as its member
struct PerThreadData
{
    const char *name;
    int number;
};

wxTLS_TYPE(PerThreadData) gs_threadDataVar;
#define gs_threadData wxTLS_VALUE(gs_threadDataVar)

wxTLS_TYPE(int) gs_threadIntVar;
#define gs_threadInt wxTLS_VALUE(gs_threadIntVar)

// ----------------------------------------------------------------------------
// test thread
// ----------------------------------------------------------------------------

// this thread arbitrarily modifies all global thread-specific variables to
// make sure that the changes in it are not visible from the main thread
class TLSTestThread : public wxThread
{
public:
    // ctor both creates and starts the thread
    TLSTestThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }

    virtual void *Entry()
    {
        gs_threadInt = 17;

        gs_threadData.name = "worker";
        gs_threadData.number = 2;

        CPPUNIT_ASSERT_EQUAL( "worker", gs_threadData.name );
        CPPUNIT_ASSERT_EQUAL( 2, gs_threadData.number );

        return NULL;
    }
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TLSTestCase : public CppUnit::TestCase
{
public:
    TLSTestCase() { }

private:
    CPPUNIT_TEST_SUITE( TLSTestCase );
        CPPUNIT_TEST( TestInt );
        CPPUNIT_TEST( TestStruct );
    CPPUNIT_TEST_SUITE_END();

    void TestInt();
    void TestStruct();

    DECLARE_NO_COPY_CLASS(TLSTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TLSTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TLSTestCase, "TLSTestCase" );

void TLSTestCase::TestInt()
{
    CPPUNIT_ASSERT_EQUAL( 0, gs_threadInt );

    gs_threadInt++;
    CPPUNIT_ASSERT_EQUAL( 1, gs_threadInt );

    TLSTestThread().Wait();

    CPPUNIT_ASSERT_EQUAL( 1, gs_threadInt );
}

void TLSTestCase::TestStruct()
{
    CPPUNIT_ASSERT_EQUAL( "", gs_threadData.name );
    CPPUNIT_ASSERT_EQUAL( 0, gs_threadData.number );

    gs_threadData.name = "main";
    gs_threadData.number = 1;

    CPPUNIT_ASSERT_EQUAL( 1, gs_threadData.number );

    TLSTestThread().Wait();

    CPPUNIT_ASSERT_EQUAL( "main", gs_threadData.name );
    CPPUNIT_ASSERT_EQUAL( 1, gs_threadData.number );
}

