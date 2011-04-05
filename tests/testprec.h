#ifndef WX_TESTPREC_INCLUDED
#define WX_TESTPREC_INCLUDED 1

#include "wx/wxprec.h"
#include "wx/cppunit.h"

// Custom test macro that is only defined when wxUIActionSimulator is available
// this allows the tests that do not rely on it to run on platforms that don't
// support it.
//
// FIXME: And while OS X does support it, more or less, too many tests
//        currently fail under it so disable all interactive tests there. They
//        should, of course, be reenabled a.s.a.p.
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXOSX__)
    #define WXUISIM_TEST(test) CPPUNIT_TEST(test)
#else
    #define WXUISIM_TEST(test) (void)0
#endif

// define wxHAVE_U_ESCAPE if the compiler supports \uxxxx character constants
#if (defined(__VISUALC__) && (__VISUALC__ >= 1300)) || \
    (defined(__GNUC__) && (__GNUC__ >= 3))
    #define wxHAVE_U_ESCAPE

    // and disable warning that using them results in with MSVC 8+
    #if wxCHECK_VISUALC_VERSION(8)
        // universal-character-name encountered in source
        #pragma warning(disable:4428)
    #endif
#endif

// Define wxUSING_VC_CRT_IO when using MSVC CRT STDIO library as its standard
// functions give different results from glibc ones in several cases (of
// course, any code relying on this is not portable and probably won't work,
// i.e. will result in tests failures, with other platforms/compilers which
// should have checks for them added as well).
//
// Notice that MinGW uses VC CRT by default but may use its own printf()
// implementation if __USE_MINGW_ANSI_STDIO is defined. And finally also notice
// that testing for __USE_MINGW_ANSI_STDIO directly results in a warning with
// -Wundef if it involves an operation with undefined __MINGW_FEATURES__ so
// test for the latter too to avoid it.
#if defined(__VISUALC__) || \
    (defined(__MINGW32__) && \
     (!defined(__MINGW_FEATURES__) || !__USE_MINGW_ANSI_STDIO))
    #define wxUSING_VC_CRT_IO
#endif

// thrown when assert fails in debug build
class TestAssertFailure
{
public:
    TestAssertFailure(const wxString& file,
                      int line,
                      const wxString& func,
                      const wxString& cond,
                      const wxString& msg)
        : m_file(file),
          m_line(line),
          m_func(func),
          m_cond(cond),
          m_msg(msg)
    {
    }

    const wxString m_file;
    const int m_line;
    const wxString m_func;
    const wxString m_cond;
    const wxString m_msg;

    wxDECLARE_NO_ASSIGN_CLASS(TestAssertFailure);
};

// macro to use for the functions which are supposed to fail an assertion
#if wxDEBUG_LEVEL
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
    // there are no assertions in this build so we can't do anything (we used
    // to check that the condition failed but this didn't work well as in
    // normal build with wxDEBUG_LEVEL != 0 we can pass something not
    // evaluating to a bool at all but it then would fail to compile in
    // wxDEBUG_LEVEL == 0 case, so just don't do anything at all now).
    #define WX_ASSERT_FAILS_WITH_ASSERT(cond)
#endif

// these functions can be used to hook into wxApp event processing and are
// currently used by the events propagation test
class WXDLLIMPEXP_FWD_BASE wxEvent;

typedef int (*FilterEventFunc)(wxEvent&);
typedef bool (*ProcessEventFunc)(wxEvent&);

extern void SetFilterEventFunc(FilterEventFunc func);
extern void SetProcessEventFunc(ProcessEventFunc func);

extern bool IsNetworkAvailable();

// Helper class setting the locale to the given one for its lifetime.
class LocaleSetter
{
public:
    LocaleSetter(const char *loc) : m_locOld(setlocale(LC_ALL, loc)) { }
    ~LocaleSetter() { setlocale(LC_ALL, m_locOld); }

private:
    const char * const m_locOld;

    wxDECLARE_NO_COPY_CLASS(LocaleSetter);
};

// An even simpler helper for setting the locale to "C" one during its lifetime.
class CLocaleSetter : private LocaleSetter
{
public:
    CLocaleSetter() : LocaleSetter("C") { }

private:
    wxDECLARE_NO_COPY_CLASS(CLocaleSetter);
};

// Macro that can be used to register the test with the given name in both the
// global unnamed registry so that it is ran by default and a registry with the
// same name as this test to allow running just this test individually.
//
// Notice that the name shouldn't include the "TestCase" suffix, it's added
// automatically by this macro.
//
// Implementation note: CPPUNIT_TEST_SUITE_[NAMED_]REGISTRATION macros can't be
// used here because they both declare the variable with the same name (as the
// "unique" name they generate is based on the line number which is the same
// for both calls inside the macro), so we need to do it manually.
#define wxREGISTER_UNIT_TEST(name) \
    static CPPUNIT_NS::AutoRegisterSuite< name##TestCase > \
        CPPUNIT_MAKE_UNIQUE_NAME( autoRegisterRegistry__ ); \
    static CPPUNIT_NS::AutoRegisterSuite< name##TestCase > \
        CPPUNIT_MAKE_UNIQUE_NAME( autoRegisterNamedRegistry__ )(#name "TestCase")

#endif
