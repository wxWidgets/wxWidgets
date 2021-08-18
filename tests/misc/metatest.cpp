///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/metatest.cpp
// Purpose:     Test template meta-programming constructs
// Author:      Jaakko Salli
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include "wx/object.h"
#include "wx/utils.h"
#include "wx/meta/pod.h"
#include "wx/meta/movable.h"

#ifndef wxNO_RTTI
#include <typeinfo>
#endif

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
        CPPUNIT_TEST( ImplicitConversion );
        CPPUNIT_TEST( MinMax );
    CPPUNIT_TEST_SUITE_END();

    void IsPod();
    void IsMovable();
    void ImplicitConversion();
    void MinMax();

    wxDECLARE_NO_COPY_CLASS(MetaProgrammingTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MetaProgrammingTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MetaProgrammingTestCase,
                                       "MetaProgrammingTestCase" );


void MetaProgrammingTestCase::IsPod()
{
    CPPUNIT_ASSERT(wxIsPod<bool>::value);
    CPPUNIT_ASSERT(wxIsPod<signed int>::value);
    CPPUNIT_ASSERT(wxIsPod<double>::value);
    CPPUNIT_ASSERT(wxIsPod<wxObject*>::value);
    CPPUNIT_ASSERT(!wxIsPod<wxObject>::value);
}

void MetaProgrammingTestCase::IsMovable()
{
    CPPUNIT_ASSERT(wxIsMovable<bool>::value);
    CPPUNIT_ASSERT(wxIsMovable<signed int>::value);
    CPPUNIT_ASSERT(wxIsMovable<double>::value);
    CPPUNIT_ASSERT(wxIsMovable<wxObject*>::value);
    CPPUNIT_ASSERT(!wxIsMovable<wxObject>::value);
}

void MetaProgrammingTestCase::ImplicitConversion()
{
#ifndef wxNO_RTTI
    CPPUNIT_ASSERT(typeid(wxImplicitConversionType<char,int>::value) == typeid(int));
    CPPUNIT_ASSERT(typeid(wxImplicitConversionType<int,unsigned>::value) == typeid(unsigned));
#ifdef wxLongLong_t
    CPPUNIT_ASSERT(typeid(wxImplicitConversionType<wxLongLong_t,float>::value) == typeid(float));
#endif
#endif // !wxNO_RTTI
}

void MetaProgrammingTestCase::MinMax()
{
    // test that wxMax(1.1,1) returns float, not long int
    float f = wxMax(1.1f, 1l);
    CPPUNIT_ASSERT_EQUAL( 1.1f, f);

    // test that comparing signed and unsigned correctly returns unsigned: this
    // may seem counterintuitive in this case but this is consistent with the
    // standard C conversions
    CPPUNIT_ASSERT_EQUAL( 1, wxMin(-1, 1u) );

    CPPUNIT_ASSERT_EQUAL( -1., wxClip(-1.5, -1, 1) );
    CPPUNIT_ASSERT_EQUAL( 0, wxClip(0, -1, 1) );
    CPPUNIT_ASSERT_EQUAL( 1, wxClip(2l, -1, 1) );
}
