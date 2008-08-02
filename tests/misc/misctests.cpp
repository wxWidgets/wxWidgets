///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous stuff
// Author:      Peter Most
// Created:     2008-07-10
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Peter Most
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MiscTestCase : public CppUnit::TestCase
{
public:
    MiscTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MiscTestCase );
        CPPUNIT_TEST( Delete );
    CPPUNIT_TEST_SUITE_END();

    void Delete();

    DECLARE_NO_COPY_CLASS(MiscTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MiscTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MiscTestCase, "MiscTestCase" );

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

