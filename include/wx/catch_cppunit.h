/////////////////////////////////////////////////////////////////////////////
// Name:        wx/catch_cppunit.h
// Purpose:     Reimplementation of CppUnit macros in terms of CATCH
// Author:      Vadim Zeitlin
// Created:     2017-10-30
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CATCH_CPPUNIT_H_
#define _WX_CATCH_CPPUNIT_H_

#include "catch2/catch.hpp"

// CppUnit-compatible macros.

// CPPUNIT_ASSERTs are mapped to REQUIRE(), not CHECK(), as this is how CppUnit
// works but in many cases they really should be CHECK()s instead, i.e. the
// test should continue to run even if one assert failed. Unfortunately there
// is no automatic way to know it, so the existing code will need to be
// reviewed and CHECK() used explicitly where appropriate.
//
// Also notice that we don't use parentheses around x and y macro arguments in
// the macro expansion, as usual. This is because these parentheses would then
// appear in CATCH error messages if the assertion fails, making them much less
// readable and omitting should be fine here, exceptionally, as the arguments
// of these macros are usually just simple expressions.
#define CPPUNIT_ASSERT(cond) REQUIRE(cond)
#define CPPUNIT_ASSERT_EQUAL(x, y) REQUIRE(x == y)

// Using INFO() disallows putting more than one of these macros on the same
// line but this can happen if they're used inside another macro, so wrap it
// inside a scope.
#define CPPUNIT_ASSERT_MESSAGE(msg, cond) \
    wxSTATEMENT_MACRO_BEGIN               \
    INFO(msg);                            \
    REQUIRE(cond);                        \
    wxSTATEMENT_MACRO_END

#define CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, x, y) \
    wxSTATEMENT_MACRO_BEGIN                     \
    INFO(msg);                                  \
    REQUIRE(x == y);                            \
    wxSTATEMENT_MACRO_END

// CATCH Approx class uses the upper bound of "epsilon*(scale + max(|x|, |y|))"
// for |x - y| which is not really compatible with our fixed delta, so we can't
// use it here.
#define CPPUNIT_ASSERT_DOUBLES_EQUAL(x, y, delta) \
    REQUIRE(std::abs(x - y) < delta)

#define CPPUNIT_FAIL(msg) FAIL(msg)

#define CPPUNIT_ASSERT_THROW(expr, exception)         \
    try                                               \
    {                                                 \
        expr;                                         \
        FAIL("Expected exception of type " #exception \
                   " not thrown by " #expr);          \
    }                                                 \
    catch ( exception& ) {}

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

// Use a different namespace for our mock ups of the real declarations in
// CppUnit namespace to avoid clashes if we end up being linked with the real
// CppUnit library, but bring it into scope with a using directive below to
// make it possible to compile the original code using CppUnit unmodified.
namespace CatchCppUnit
{

namespace CppUnit
{

// These classes don't really correspond to the real CppUnit ones, but contain
// just the minimum we need to make CPPUNIT_TEST() macro and our mock up of
// TestSuite class work.

class Test
{
public:
    // Name argument exists only for compatibility with the real CppUnit but is
    // not used here.
    explicit Test(const std::string& name = std::string()) : m_name(name) { }

    virtual ~Test() { }

    virtual void runTest() = 0;

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
};

class TestCase : public Test
{
public:
    explicit TestCase(const std::string& name = std::string()) : Test(name) { }

    virtual void setUp() {}
    virtual void tearDown() {}
};

class TestSuite : public Test
{
public:
    explicit TestSuite(const std::string& name = std::string()) : Test(name) { }

    ~TestSuite()
    {
        for ( size_t n = 0; n < m_tests.size(); ++n )
        {
            delete m_tests[n];
        }
    }

    void addTest(Test* test) { m_tests.push_back(test); }
    size_t getChildTestCount() const { return m_tests.size(); }

    void runTest() override
    {
        for ( size_t n = 0; n < m_tests.size(); ++n )
        {
            m_tests[n]->runTest();
        }
    }

private:
    std::vector<Test*> m_tests;

    wxDECLARE_NO_COPY_CLASS(TestSuite);
};

} // namespace CppUnit

} // namespace CatchCppUnit

using namespace CatchCppUnit;

// Helpers used in the implementation of the macros below.
namespace wxPrivate
{

// An object which resets a string to its old value when going out of scope.
class TempStringAssign
{
public:
    explicit TempStringAssign(std::string& str, const char* value)
        : m_str(str),
          m_orig(str)
    {
        str = value;
    }

    ~TempStringAssign()
    {
        m_str = m_orig;
    }

private:
    std::string& m_str;
    const std::string m_orig;

    wxDECLARE_NO_COPY_CLASS(TempStringAssign);
};

// These two strings are used to implement wxGetCurrentTestName() and must be
// defined in the test driver.
extern std::string wxTheCurrentTestClass, wxTheCurrentTestMethod;

} // namespace wxPrivate

inline std::string wxGetCurrentTestName()
{
    std::string s = wxPrivate::wxTheCurrentTestClass;
    if ( !s.empty() && !wxPrivate::wxTheCurrentTestMethod.empty() )
        s += "::";
    s += wxPrivate::wxTheCurrentTestMethod;

    return s;
}

// Notice that the use of this macro unconditionally changes the protection for
// everything that follows it to "public". This is necessary to allow taking
// the address of the runTest() method in CPPUNIT_TEST_SUITE_REGISTRATION()
// below and there just doesn't seem to be any way around it.
#define CPPUNIT_TEST_SUITE(testclass)   \
    public:                             \
    void runTest() override           \
    {                                   \
        using namespace wxPrivate;      \
        TempStringAssign setClass(wxTheCurrentTestClass, #testclass)

#define CPPUNIT_TEST(testname)                                             \
        SECTION(#testname)                                                 \
        {                                                                  \
            TempStringAssign setMethod(wxTheCurrentTestMethod, #testname); \
            setUp();                                                       \
            try                                                            \
            {                                                              \
                testname();                                                \
            }                                                              \
            catch ( ... )                                                  \
            {                                                              \
                tearDown();                                                \
                throw;                                                     \
            }                                                              \
            tearDown();                                                    \
        }

#define CPPUNIT_TEST_SUITE_END()        \
    }                                   \
    struct EatNextSemicolon

#define wxREGISTER_UNIT_TEST_WITH_TAGS(testclass, tags)       \
    METHOD_AS_TEST_CASE(testclass::runTest, #testclass, tags) \
    struct EatNextSemicolon

#define CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(testclass, suitename) \
    wxREGISTER_UNIT_TEST_WITH_TAGS(testclass, "[" suitename "]")

// Existings tests always use both this macro and the named registration one
// above, but we can't register the same test case twice with CATCH, so simply
// ignore this one.
#define CPPUNIT_TEST_SUITE_REGISTRATION(testclass) \
    struct EatNextSemicolon

// ----------------------------------------------------------------------------
// wxWidgets-specific macros
// ----------------------------------------------------------------------------

// Convenient variant of INFO() which uses wxString::Format() internally.
#define wxINFO_FMT_HELPER(fmt, ...) \
    wxString::Format(fmt, __VA_ARGS__).ToStdString(wxConvUTF8)

#define wxINFO_FMT(...) INFO(wxINFO_FMT_HELPER(__VA_ARGS__))

// Use this macro to assert with the given formatted message (it should contain
// the format string and arguments in a separate pair of parentheses)
#define WX_ASSERT_MESSAGE(msg, cond) \
    CPPUNIT_ASSERT_MESSAGE(std::string(wxString::Format msg .mb_str(wxConvLibc)), (cond))

#define WX_ASSERT_EQUAL_MESSAGE(msg, expected, actual) \
    CPPUNIT_ASSERT_EQUAL_MESSAGE(std::string(wxString::Format msg .mb_str(wxConvLibc)), \
                                 (expected), (actual))

#endif // _WX_CATCH_CPPUNIT_H_
