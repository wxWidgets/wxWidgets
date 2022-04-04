///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous stuff
// Author:      Peter Most, Vadim Zeitlin
// Created:     2008-07-10
// Copyright:   (c) 2008 Peter Most
//              (c) 2009 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/defs.h"

#include "wx/math.h"

// just some classes using wxRTTI for wxStaticCast() test
#include "wx/tarstrm.h"
#include "wx/zipstrm.h"

#ifdef __WINDOWS__
    // Needed for wxMulDivInt32().
    #include "wx/msw/wrapwin.h"
#endif

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
#ifdef HAVE_VARIADIC_MACROS
        CPPUNIT_TEST( CallForEach );
#endif // HAVE_VARIADIC_MACROS
        CPPUNIT_TEST( Delete );
        CPPUNIT_TEST( StaticCast );
    CPPUNIT_TEST_SUITE_END();

    void Assert();
    void CallForEach();
    void Delete();
    void StaticCast();

    wxDECLARE_NO_COPY_CLASS(MiscTestCase);
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

#ifdef HAVE_VARIADIC_MACROS
void MiscTestCase::CallForEach()
{
    #define MY_MACRO(pos, str) s += str;

    wxString s;
    wxCALL_FOR_EACH(MY_MACRO, "foo", "bar", "baz");

    CPPUNIT_ASSERT_EQUAL( "foobarbaz", s );

    #undef MY_MACRO
}
#endif // HAVE_VARIADIC_MACROS

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
    wxDELETEA( array );
    CPPUNIT_ASSERT( array == NULL );

    // this results in compilation error, as it should
#if 0
    struct SomeUnknownStruct *p = NULL;
    wxDELETE(p);
#endif
}

namespace
{

#ifdef __WXDEBUG__

// helper function used just to avoid warnings about value computed not being
// used in WX_ASSERT_FAILS_WITH_ASSERT() in StaticCast() below
bool IsNull(void *p)
{
    return p == NULL;
}

#endif // __WXDEBUG__

} // anonymous namespace

void MiscTestCase::StaticCast()
{
#if wxUSE_TARSTREAM
    wxTarEntry tarEntry;
    CPPUNIT_ASSERT( wxStaticCast(&tarEntry, wxArchiveEntry) );

    wxArchiveEntry *entry = &tarEntry;
    CPPUNIT_ASSERT( wxStaticCast(entry, wxTarEntry) );

#if wxUSE_ZIPSTREAM
    wxZipEntry zipEntry;
    entry = &zipEntry;
    CPPUNIT_ASSERT( wxStaticCast(entry, wxZipEntry) );
    WX_ASSERT_FAILS_WITH_ASSERT( IsNull(wxStaticCast(&zipEntry, wxTarEntry)) );
#endif // wxUSE_ZIPSTREAM

    WX_ASSERT_FAILS_WITH_ASSERT( IsNull(wxStaticCast(entry, wxTarEntry)) );
#endif // wxUSE_TARSTREAM
}

TEST_CASE("RTTI::ClassInfo", "[rtti]")
{
    wxObject obj;
    CHECK( obj.GetClassInfo()->IsKindOf(wxCLASSINFO(wxObject)) );
    CHECK( !obj.GetClassInfo()->IsKindOf(wxCLASSINFO(wxArchiveEntry)) );

#if wxUSE_ZIPSTREAM
    wxZipEntry zipEntry;
    CHECK( zipEntry.GetClassInfo()->IsKindOf(wxCLASSINFO(wxArchiveEntry)) );
#endif // wxUSE_ZIPSTREAM
}

TEST_CASE("wxCTZ", "[math]")
{
    CHECK( wxCTZ(1) == 0 );
    CHECK( wxCTZ(4) == 2 );
    CHECK( wxCTZ(17) == 0 );
    CHECK( wxCTZ(0x80000000) == 31 );

    WX_ASSERT_FAILS_WITH_ASSERT( wxCTZ(0) );
}

TEST_CASE("wxRound", "[math]")
{
    CHECK( wxRound(2.3) == 2 );
    CHECK( wxRound(3.7) == 4 );
    CHECK( wxRound(-0.5f) == -1 );

    WX_ASSERT_FAILS_WITH_ASSERT( wxRound(2.0*INT_MAX) );
    WX_ASSERT_FAILS_WITH_ASSERT( wxRound(1.1*INT_MIN) );

    // For compatibility reasons, we allow using wxRound() with integer types
    // as well, even if this doesn't really make sense/
#if WXWIN_COMPATIBILITY_3_0
    #ifdef __VISUALC__
        #pragma warning(push)
        #pragma warning(disable:4996)
    #endif
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)

    CHECK( wxRound(-9) == -9 );
    CHECK( wxRound((size_t)17) == 17 );
    CHECK( wxRound((short)289) == 289 );

    wxGCC_WARNING_RESTORE(deprecated-declarations)
    #ifdef __VISUALC__
        #pragma warning(pop)
    #endif
#endif // WXWIN_COMPATIBILITY_3_0
}

TEST_CASE("wxMulDivInt32", "[math]")
{
    // Check that it rounds correctly.
    CHECK( wxMulDivInt32(15, 3, 2) == 23 );

    // Check that it doesn't overflow.
    CHECK( wxMulDivInt32((INT_MAX - 1)/2, 200, 100) == INT_MAX - 1 );
}
