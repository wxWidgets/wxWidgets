#ifndef WX_TESTPREC_INCLUDED
#define WX_TESTPREC_INCLUDED 1

#include "wx/wxprec.h"
#include "wx/evtloop.h"

// This provides std::make_unique() even when using C++11 compilers.
#include "wx/private/make_unique.h"

// It is used often enough in the tests to make it worth avoiding the need to
// qualify it with "std::" every time.
using std::make_unique;

// This header must be included before catch.hpp to be taken into account.
#include "asserthelper.h"

// This needs to be defined before including catch.hpp for PCH support.
#define CATCH_CONFIG_ALL_PARTS

#include "catch2/catch.hpp"

// Define conversions to strings for some common wxWidgets types.
namespace Catch
{
    template <>
    struct StringMaker<wxUniChar>
    {
        static std::string convert(wxUniChar uc)
        {
            return wxString(uc).ToStdString(wxConvUTF8);
        }
    };

    template <>
    struct StringMaker<wxUniCharRef>
    {
        static std::string convert(wxUniCharRef ucr)
        {
            return wxString(ucr).ToStdString(wxConvUTF8);
        }
    };

    // While this conversion already works due to the existence of the stream
    // insertion operator for wxString, define a custom one making it more
    // obvious when strings containing non-printable characters differ.
    template <>
    struct StringMaker<wxString>
    {
        static std::string convert(const wxString& wxs)
        {
            std::string s;
            s.reserve(wxs.length() + 2);
            s += '"';
            for ( auto c : wxs )
            {
                if ( c >= 128 || !iswprint(c) )
                    s += wxString::Format(wxASCII_STR("\\u%04X"), c).ToAscii();
                else
                    s += c;
            }
            s += '"';

            return s;
        }
    };
}

// Return the name of the test being currently executed or an empty string if
// no test is running, e.g. if we're called before or after the tests.
//
// Note that we can't use Catch::getResultCapture() here because it throws if
// there is no active test run and this function is called from the callbacks
// which can't let exceptions escape from them.
inline std::string wxGetCurrentTestName()
{
    auto* const capture = Catch::getCurrentContext().getResultCapture();

    return capture ? capture->getCurrentTestName() : std::string();
}

// Convenient variant of INFO() which uses wxString::Format() internally.
#define wxINFO_FMT_HELPER(fmt, ...) \
    wxString::Format(fmt, __VA_ARGS__).ToStdString(wxConvUTF8)

#define wxINFO_FMT(...) INFO(wxINFO_FMT_HELPER(__VA_ARGS__))

// Use these macros to check the given condition with the given formatted
// message (it should contain the format string and arguments in a separate
// pair of parentheses).
//
// Note that using INFO() disallows putting more than one of these macros on
// the same line but this can happen if they're used inside another macro, so
// wrap it inside a scope.
#define WX_ASSERT_MESSAGE(msg, cond)                             \
    wxSTATEMENT_MACRO_BEGIN                                      \
    INFO(std::string(wxString::Format msg .mb_str(wxConvLibc))); \
    REQUIRE(cond);                                               \
    wxSTATEMENT_MACRO_END

#define WX_ASSERT_EQUAL_MESSAGE(msg, expected, actual)           \
    wxSTATEMENT_MACRO_BEGIN                                      \
    INFO(std::string(wxString::Format msg .mb_str(wxConvLibc))); \
    REQUIRE((actual) == (expected));                             \
    wxSTATEMENT_MACRO_END

// Define a test case with the given name and tags simply calling the given
// method of the given test class.
//
// This is useful for running the same tests for several different fixtures,
// e.g. for testing both single and multi-line wxTextCtrl.
#define wxTEST_CASE_FOR_METHOD(testclass, prefix, name, tags) \
    TEST_CASE_METHOD(testclass, prefix "::" #name, tags)      \
    {                                                         \
        name();                                               \
    }

#if defined(__VISUALC__)
    #if _MSC_VER < 1910
        // MSVS 2015 doesn't handle literal Unicode characters in wide strings
        // correctly, so use \uxxxx escapes for it instead.
        //
        // When support for MSVS 2015 is dropped, this symbol and all code
        // guarded by it should be removed.
        #define wxMUST_USE_U_ESCAPE
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
#if (defined(__MINGW32__) && !defined(wxMINGW_WITH_FIXED_MANTISSA) && \
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

    TestAssertFailure(const TestAssertFailure&) = default;

    const wxString m_file;
    const int m_line;
    const wxString m_func;
    const wxString m_cond;
    const wxString m_msg;

    wxDECLARE_NO_ASSIGN_CLASS(TestAssertFailure);
};

// macro to use for the functions which are supposed to fail an assertion
#if wxDEBUG_LEVEL
    #define WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE(msg, code) \
        wxSTATEMENT_MACRO_BEGIN \
            bool throwsAssert = false; \
            try { code ; } \
            catch ( const TestAssertFailure& ) { throwsAssert = true; } \
            if ( throwsAssert ) \
                SUCCEED("assert triggered"); \
            else \
                FAIL_CHECK(msg); \
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
    #define WX_ASSERT_FAILS_WITH_ASSERT_MESSAGE(msg, code)
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

#if wxUSE_LOG
// Logging is disabled by default when running the tests, but sometimes it can
// be helpful to see the errors in case of unexpected failure, so this class
// re-enables logs in its scope.
//
// It's a counterpart to wxLogNull.
class TestLogEnabler
{
public:
    TestLogEnabler();
    ~TestLogEnabler();

private:
    wxDECLARE_NO_COPY_CLASS(TestLogEnabler);
};
#else // !wxUSE_LOG
class TestLogEnabler { };
#endif // wxUSE_LOG/!wxUSE_LOG

#if wxUSE_GUI

// Return true if the UI tests, i.e. the ones using wxUIActionSimulator, are
// enabled: the tests using it must check for this and return early if not.
extern bool EnableUITests();

// Helper function deleting the window without asserts (and hence exceptions
// thrown from its dtor!) even if it has mouse capture.
void DeleteTestWindow(wxWindow* win);

#endif // wxUSE_GUI

#endif
