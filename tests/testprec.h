#ifndef WX_TESTPREC_INCLUDED
#define WX_TESTPREC_INCLUDED 1

#include "wx/wxprec.h"
#include "wx/evtloop.h"

// This needs to be included before catch.hpp to be taken into account.
#include "testdate.h"

#include "wx/catch_cppunit.h"

// Custom test macro that is only defined when wxUIActionSimulator is available
// this allows the tests that do not rely on it to run on platforms that don't
// support it.
//
// Unfortunately, currently too many of the UI tests fail on non-MSW platforms,
// so they're disabled there by default. This really, really needs to be fixed,
// but for now having the UI tests always failing is not helpful as it prevents
// other test failures from being noticed, so disable them there.
#if wxUSE_UIACTIONSIMULATOR
    #define WXUISIM_TEST(test) if ( EnableUITests() ) { CPPUNIT_TEST(test) }
#else
    #define WXUISIM_TEST(test)
#endif

// define wxHAVE_U_ESCAPE if the compiler supports \uxxxx character constants
#if defined(__VISUALC__) || \
    (defined(__GNUC__) && (__GNUC__ >= 3))
    #define wxHAVE_U_ESCAPE

    // and disable warning that using them results in with MSVC 8+
    #if wxCHECK_VISUALC_VERSION(8)
        // universal-character-name encountered in source
        #pragma warning(disable:4428)
    #endif
#endif

// Define wxUSING_MANTISSA_SIZE_3 for certain versions of MinGW and MSVC.
// These use a CRT which prints the exponent with a minimum of 3
// digits instead of 2.
//
// This happens for all MSVC compilers before version 14 (VS2015).
// And for MinGW when it does not define or set __USE_MINGW_ANSI_STDIO.
// Since MinGW 5.0.4 it uses at least 2 digits for the exponent:
// https://sourceforge.net/p/mingw-w64/mailman/message/36333746/

#if (defined(__MINGW64_VERSION_MAJOR) && (__MINGW64_VERSION_MAJOR > 5 || \
        (__MINGW64_VERSION_MAJOR == 5 && __MINGW64_VERSION_MINOR >= 0) || \
        (__MINGW64_VERSION_MAJOR == 5 && __MINGW64_VERSION_MINOR == 0 && __MINGW64_VERSION_BUGFIX >= 4)))
#define wxMINGW_WITH_FIXED_MANTISSA
#endif
#if (defined(__VISUALC__) && !wxCHECK_VISUALC_VERSION(14)) || \
        (defined(__MINGW32__) && !defined(wxMINGW_WITH_FIXED_MANTISSA) && \
        (!defined(__USE_MINGW_ANSI_STDIO) || !__USE_MINGW_ANSI_STDIO))
    #define wxDEFAULT_MANTISSA_SIZE_3
#endif

// Many tests use wide characters or wide strings inside Catch macros, which
// requires converting them to string if the check fails. This falls back to
// std::ostream::operator<<() by default, which never worked correctly, as there
// never was any overload for wchar_t and so it used something else, but in C++
// 20 this overload is explicitly deleted, so it results in compile-time error.
//
// Hence define this specialization to allow compiling such comparisons.
namespace Catch
{

template <>
struct StringMaker<wchar_t>
{
    static std::string convert(wchar_t wc)
    {
        if ( wc < 0x7f )
            return std::string(static_cast<char>(wc), 1);

        return wxString::Format(wxASCII_STR("U+%06X"), wc).ToStdString(wxConvLibc);
    }
};

} // namespace Catch


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
    #define WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE(msg, code) \
        wxSTATEMENT_MACRO_BEGIN \
            bool throwsAssert = false; \
            try { code ; } \
            catch ( const TestAssertFailure& ) { throwsAssert = true; } \
            if ( !throwsAssert ) \
                CPPUNIT_FAIL(msg); \
        wxSTATEMENT_MACRO_END

    #define WX_ASSERT_FAILS_WITH_ASSERT(code) \
        WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE( \
            "expected assertion not generated", code)

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

extern bool IsAutomaticTest();

extern bool IsRunningUnderXVFB();

#ifdef __LINUX__
extern bool IsRunningInLXC();
#endif // __LINUX__

// Helper class setting the locale to the given one for its lifetime.
class LocaleSetter
{
public:
    LocaleSetter(const char *loc)
        : m_locOld(wxStrdupA(setlocale(LC_ALL, NULL)))
    {
        setlocale(LC_ALL, loc);
    }

    ~LocaleSetter()
    {
        setlocale(LC_ALL, m_locOld);
        free(m_locOld);
    }

private:
    char * const m_locOld;

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

#if wxUSE_GUI

// Return true if the UI tests are enabled, used by WXUISIM_TEST().
extern bool EnableUITests();

// Helper function deleting the window without asserts (and hence exceptions
// thrown from its dtor!) even if it has mouse capture.
void DeleteTestWindow(wxWindow* win);

#endif // wxUSE_GUI

// Convenience macro which registers a test case using just its "base" name,
// i.e. without the common "TestCase" suffix, as its tag.
#define wxREGISTER_UNIT_TEST(testclass) \
    wxREGISTER_UNIT_TEST_WITH_TAGS(testclass ## TestCase, "[" #testclass "]")

#endif
