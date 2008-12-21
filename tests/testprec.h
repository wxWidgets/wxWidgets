#include "wx/wxprec.h"
#include "wx/cppunit.h"

// define wxHAVE_U_ESCAPE if the compiler supports \uxxxx character constants
#if (defined(__VISUALC__) && (__VISUALC__ >= 1300)) || \
    (defined(__GNUC__) && (__GNUC__ >= 3))
    #define wxHAVE_U_ESCAPE
#endif

// thrown when assert fails in debug build
class TestAssertFailure { };

// macro to use for the functions which are supposed to fail an assertion
#ifdef __WXDEBUG__
    // some old cppunit versions don't define CPPUNIT_ASSERT_THROW so roll our
    // own
    #define WX_ASSERT_FAILS_WITH_ASSERT(cond) \
        { \
            bool throwsAssert = false; \
            try { cond ; } \
            catch ( const TestAssertFailure& ) { throwsAssert = true; } \
            if ( !throwsAssert ) \
                CPPUNIT_FAIL("expected assertion not generated"); \
        }
#else
    // there are no assertions in non-debug build so just check that it fails
    #define WX_ASSERT_FAILS_WITH_ASSERT(cond) CPPUNIT_ASSERT(!(cond))
#endif

