///////////////////////////////////////////////////////////////////////////////
// Name:        tests/regex/wxregex.cpp
// Purpose:     Test wxRegEx
// Author:      Vadim Zeitlin, Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   Vadim Zeitlin, Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#if wxUSE_REGEX

#include "wx/regex.h"
#include "wx/tokenzr.h"
#include <string>

using CppUnit::Test;
using CppUnit::TestCase;
using CppUnit::TestSuite;
using std::string;


///////////////////////////////////////////////////////////////////////////////
// Compile Test

class RegExCompileTestCase : public TestCase
{
public:
    RegExCompileTestCase(const char *name, const wxString& pattern,
                         bool correct, int flags)
    :   TestCase(name),
        m_pattern(pattern),
        m_correct(correct),
        m_flags(flags)
    { }

protected:
    void runTest();

private:
    wxString m_pattern;
    bool m_correct;
    int m_flags;
};

void RegExCompileTestCase::runTest()
{
    wxRegEx re;
    bool ok = re.Compile(m_pattern, m_flags);

    if (m_correct)
        CPPUNIT_ASSERT_MESSAGE("compile failed", ok);
    else
        CPPUNIT_ASSERT_MESSAGE("compile succeeded (should fail)", !ok);
}


///////////////////////////////////////////////////////////////////////////////
// Match Test

class RegExMatchTestCase : public TestCase
{
public:
    RegExMatchTestCase(const char *name, const wxString& pattern,
                       const wxString& text, const char *expected,
                       int flags)
    :   TestCase(name),
        m_pattern(pattern),
        m_text(text),
        m_expected(expected),
        m_flags(flags)
    { }

protected:
    void runTest();

private:
    wxString m_pattern;
    wxString m_text;
    const char *m_expected;
    int m_flags;
};

void RegExMatchTestCase::runTest()
{
    int compileFlags = m_flags & ~(wxRE_NOTBOL | wxRE_NOTEOL);
    int matchFlags = m_flags & (wxRE_NOTBOL | wxRE_NOTEOL);

    wxRegEx re(m_pattern, compileFlags);
    CPPUNIT_ASSERT_MESSAGE("compile failed", re.IsValid());

    bool ok = re.Matches(m_text, matchFlags);

    if (m_expected) {
        CPPUNIT_ASSERT_MESSAGE("match failed", ok);

        wxStringTokenizer tkz(wxString(m_expected, *wxConvCurrent),
                              wxT("\t"), wxTOKEN_RET_EMPTY);
        size_t i;

        for (i = 0; i < re.GetMatchCount() && tkz.HasMoreTokens(); i++) {
            wxString expected = tkz.GetNextToken();
            wxString result = re.GetMatch(m_text, i);

            wxString msgstr;
            msgstr.Printf(wxT("\\%d == '%s' (expected '%s')"),
                          (int)i, result.c_str(), expected.c_str());

            CPPUNIT_ASSERT_MESSAGE((const char*)msgstr.mb_str(),
                                   result == expected);
        }

        if ((m_flags & wxRE_NOSUB) == 0)
            CPPUNIT_ASSERT(re.GetMatchCount() == i);
    }
    else {
        CPPUNIT_ASSERT_MESSAGE("match succeeded (should fail)", !ok);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Replacement Test

class RegExReplaceTestCase : public TestCase
{
public:
    RegExReplaceTestCase(const char *name, const wxString& pattern,
                         const wxString& text, const wxString& repl,
                         const wxString& expected, size_t count, int flags)
    :   TestCase(name),
        m_pattern(pattern),
        m_text(text),
        m_repl(repl),
        m_expected(expected),
        m_count(count),
        m_flags(flags)
    { }

protected:
    void runTest();

private:
    wxString m_pattern;
    wxString m_text;
    wxString m_repl;
    wxString m_expected;
    size_t m_count;
    int m_flags;
};

void RegExReplaceTestCase::runTest()
{
    wxRegEx re(m_pattern, m_flags);

    wxString text(m_text);
    size_t nRepl = re.Replace(&text, m_repl);

    wxString msgstr;
    msgstr.Printf(wxT("returns '%s' (expected '%s')"), text.c_str(), m_expected.c_str());
    CPPUNIT_ASSERT_MESSAGE((const char*)msgstr.mb_str(), text == m_expected);

    msgstr.Printf(wxT("matches %d times (expected %d)"), (int)nRepl, (int)m_count);
    CPPUNIT_ASSERT_MESSAGE((const char*)msgstr.mb_str(), nRepl == m_count);
}


///////////////////////////////////////////////////////////////////////////////
// The suite

class wxRegExTestSuite : public TestSuite
{
public:
    wxRegExTestSuite() : TestSuite("wxRegExTestSuite") { }
    static Test *suite();

private:
    void add(const char *pattern, bool correct, int flags = wxRE_DEFAULT);
    void add(const char *pattern, const char *text,
             const char *expected = NULL, int flags = wxRE_DEFAULT);
    void add(const char *pattern, const char *text, const char *replacement,
             const char *expected, size_t count, int flags = wxRE_DEFAULT);

    static wxString FlagStr(int flags);
    static wxString Conv(const char *str) { return wxString(str, *wxConvCurrent); }
};

// Build the suite (static)
//
Test *wxRegExTestSuite::suite()
{
    wxRegExTestSuite *suite = new wxRegExTestSuite;

    // Compile tests
    // pattern, expected result
    suite->add("foo", true);
    suite->add("foo(", false);
    suite->add("foo(bar", false);
    suite->add("foo(bar)", true);
    suite->add("foo[", false);
    suite->add("foo[bar", false);
    suite->add("foo[bar]", true);
    suite->add("foo{1", false);
    suite->add("foo{1}", true);
    suite->add("foo{1,2}", true);
    suite->add("foo*", true);
    suite->add("foo+", true);
    suite->add("foo?", true);

    // Match tests
    // pattern, text, expected results (match, followed by submatches
    // tab separated, or NULL for no match expected)
    suite->add("foo", "bar");
    suite->add("foo", "foobar", "foo");
    suite->add("^foo", "foobar", "foo");
    suite->add("^foo", "barfoo");
    suite->add("bar$", "barbar", "bar");
    suite->add("bar$", "barbar ");
    suite->add("OoBa", "FoObAr", "oObA", wxRE_ICASE);
    suite->add("^[A-Z].*$", "AA\nbb\nCC", "AA\nbb\nCC");
    suite->add("^[A-Z].*$", "AA\nbb\nCC", "AA", wxRE_NEWLINE);
    suite->add("^[a-z].*$", "AA\nbb\nCC", "bb", wxRE_NEWLINE);
    suite->add("^[A-Z].*$", "AA\nbb\nCC", "CC", wxRE_NEWLINE | wxRE_NOTBOL);
    suite->add("^[A-Z].*$", "AA\nbb\nCC", NULL, wxRE_NEWLINE | wxRE_NOTBOL | wxRE_NOTEOL);
    suite->add("([[:alpha:]]+) ([[:alpha:]]+) ([[:digit:]]+).* ([[:digit:]]+)$",
        "Fri Jul 13 18:37:52 CEST 2001",
        "Fri Jul 13 18:37:52 CEST 2001\tFri\tJul\t13\t2001");

    // Replace tests
    // pattern, text, replacement, expected result and number of matches
    const char *patn = "([a-z]+)[^0-9]*([0-9]+)";
    suite->add(patn, "foo123", "bar", "bar", 1);
    suite->add(patn, "foo123", "\\2\\1", "123foo", 1);
    suite->add(patn, "foo_123", "\\2\\1", "123foo", 1);
    suite->add(patn, "123foo", "bar", "123foo", 0);
    suite->add(patn, "123foo456foo", "&&", "123foo456foo456foo", 1);
    suite->add(patn, "123foo456foo", "\\0\\0", "123foo456foo456foo", 1);
    suite->add(patn, "foo123foo123", "bar", "barbar", 2);
    suite->add(patn, "foo123_foo456_foo789", "bar", "bar_bar_bar", 3);

    return suite;
}

// Add a compile test
//
void wxRegExTestSuite::add(
    const char *pattern,
    bool correct,
    int flags /*=wxRE_DEFAULT*/)
{
    addTest(new RegExCompileTestCase(
                (wxT("/") + Conv(pattern) + wxT("/") + FlagStr(flags)).mb_str(),
                Conv(pattern), correct, flags));
}

// Add a match test
//
void wxRegExTestSuite::add(
    const char *pattern,
    const char *text,
    const char *expected /*=NULL*/,
    int flags /*=wxRE_DEFAULT*/)
{
    wxString name;

    name << wxT("'") << Conv(text) << wxT("' =~ /") << Conv(pattern) << wxT("/")
         << FlagStr(flags);
    name.Replace(wxT("\n"), wxT("\\n"));

    addTest(new RegExMatchTestCase(name.mb_str(), Conv(pattern),
                                   Conv(text), expected, flags));
}

// Add a replace test
//
void wxRegExTestSuite::add(
    const char *pattern,
    const char *text,
    const char *replacement,
    const char *expected,
    size_t count,
    int flags /*=wxRE_DEFAULT*/)
{
    wxString name;

    name << wxT("'") << Conv(text) << wxT("' =~ s/") << Conv(pattern) << wxT("/")
         << Conv(replacement) << wxT("/g") << FlagStr(flags);
    name.Replace(wxT("\n"), wxT("\\n"));

    addTest(new RegExReplaceTestCase(
                    name.mb_str(), Conv(pattern), Conv(text),
                    Conv(replacement), Conv(expected), count, flags));
}

// Display string for the flags
//
wxString wxRegExTestSuite::FlagStr(int flags)
{
    wxString str;

    if (!flags)
        return str;

    for (int i = 0; (unsigned)flags >> i; i++) {
        switch (flags & (1 << i)) {
            case 0: break;
#ifdef wxHAS_REGEX_ADVANCED
            case wxRE_ADVANCED: str += wxT(" | wxRE_ADVANCED"); break;
#endif
            case wxRE_BASIC:    str += wxT(" | wxRE_BASIC"); break;
            case wxRE_ICASE:    str += wxT(" | wxRE_ICASE"); break;
            case wxRE_NOSUB:    str += wxT(" | wxRE_NOSUB"); break;
            case wxRE_NEWLINE:  str += wxT(" | wxRE_NEWLINE"); break;
            case wxRE_NOTBOL:   str += wxT(" | wxRE_NOTBOL"); break;
            case wxRE_NOTEOL:   str += wxT(" | wxRE_NOTEOL"); break;
            default: wxFAIL; break;
        }
    }

    return wxT(" (") + str.Mid(3) + wxT(")");
}

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(wxRegExTestSuite);

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(wxRegExTestSuite, "wxRegExTestSuite");


#endif // wxUSE_REGEX
