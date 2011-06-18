///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous stuff
// Author:      Peter Most, Vadim Zeitlin
// Created:     2008-07-10
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Peter Most
//              (c) 2009 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"

// just some classes using wxRTTI for wxStaticCast() test
#include "wx/tarstrm.h"
#include "wx/zipstrm.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MiscTestCase : public CppUnit::TestCase
{
public:
    MiscTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MiscTestCase );
        CPPUNIT_TEST( Assert );
        CPPUNIT_TEST( Delete );
        CPPUNIT_TEST( StaticCast );
    CPPUNIT_TEST_SUITE_END();

    void Assert();
    void Delete();
    void StaticCast();

    DECLARE_NO_COPY_CLASS(MiscTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MiscTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MiscTestCase, "MiscTestCase" );

namespace
{

bool AssertIfOdd(int n)
{
    wxCHECK_MSG( !(n % 2), false, "parameter must be even" );

    return true;
}

} // anonymous namespace

void MiscTestCase::Assert()
{
    AssertIfOdd(0);
    WX_ASSERT_FAILS_WITH_ASSERT(AssertIfOdd(1));

    // doesn't fail any more
    wxAssertHandler_t oldHandler = wxSetAssertHandler(NULL);
    AssertIfOdd(17);
    wxSetAssertHandler(oldHandler);
}

void MiscTestCase::Delete()
{
    // Allocate some arbitrary memory to get a valid pointer:
    long *pointer = new long;
    CPPUNIT_ASSERT( pointer != NULL );

    // Check that wxDELETE sets the pointer to NULL:
    wxDELETE( pointer );
    CPPUNIT_ASSERT( pointer == NULL );

    // Allocate some arbitrary array to get a valid pointer:
    long *array = new long[ 3 ];
    CPPUNIT_ASSERT( array != NULL );

    // Check that wxDELETEA sets the pointer to NULL:
    wxDELETE( array );
    CPPUNIT_ASSERT( array == NULL );

    // this results in compilation error, as it should
#if 0
    struct SomeUnknownStruct *p = NULL;
    wxDELETE(p);
#endif
}

namespace
{

// helper function used just to avoid warnings about value computed not being
// used in WX_ASSERT_FAILS_WITH_ASSERT() in StaticCast() below
bool IsNull(void *p)
{
    return p == NULL;
}

} // anonymous namespace

void MiscTestCase::StaticCast()
{
    wxTarEntry tarEntry;
    CPPUNIT_ASSERT( wxStaticCast(&tarEntry, wxArchiveEntry) );

    wxArchiveEntry *entry = &tarEntry;
    CPPUNIT_ASSERT( wxStaticCast(entry, wxTarEntry) );

    wxZipEntry zipEntry;
    entry = &zipEntry;
    CPPUNIT_ASSERT( wxStaticCast(entry, wxZipEntry) );

    WX_ASSERT_FAILS_WITH_ASSERT( IsNull(wxStaticCast(entry, wxTarEntry)) );
    WX_ASSERT_FAILS_WITH_ASSERT( IsNull(wxStaticCast(&zipEntry, wxTarEntry)) );
}

