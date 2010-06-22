///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/metatest.cpp
// Purpose:     Test template meta-programming constructs
// Author:      Jaakko Salli
// RCS-ID:      $Id$
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#include "wx/object.h"
#include "wx/meta/pod.h"
#include "wx/meta/movable.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MetaProgrammingTestCase : public CppUnit::TestCase
{
public:
    MetaProgrammingTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MetaProgrammingTestCase );
        CPPUNIT_TEST( IsPod );
        CPPUNIT_TEST( IsMovable );
    CPPUNIT_TEST_SUITE_END();

    void IsPod();
    void IsMovable();

    DECLARE_NO_COPY_CLASS(MetaProgrammingTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MetaProgrammingTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MetaProgrammingTestCase,
                                       "MetaProgrammingTestCase" );


void MetaProgrammingTestCase::IsPod()
{
    CPPUNIT_ASSERT(wxIsPod<bool>::value);
    CPPUNIT_ASSERT(wxIsPod<signed int>::value);
    CPPUNIT_ASSERT(wxIsPod<double>::value);
#if !defined(__VISUALC__) || wxCHECK_VISUALC_VERSION(7)
    CPPUNIT_ASSERT(wxIsPod<wxObject*>::value);
#endif
    CPPUNIT_ASSERT(!wxIsPod<wxObject>::value);
}

void MetaProgrammingTestCase::IsMovable()
{
    CPPUNIT_ASSERT(wxIsMovable<bool>::value);
    CPPUNIT_ASSERT(wxIsMovable<signed int>::value);
    CPPUNIT_ASSERT(wxIsMovable<double>::value);
#if !defined(__VISUALC__) || wxCHECK_VISUALC_VERSION(7)
    CPPUNIT_ASSERT(wxIsMovable<wxObject*>::value);
#endif
    CPPUNIT_ASSERT(!wxIsMovable<wxObject>::value);
}
