///////////////////////////////////////////////////////////////////////////////
// Name:        tests/regex/regex.cpp
// Purpose:     Test the built-in regex lib and wxRegEx
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

//
// Notes:
//
// To run just one section, say wx_1, do this:
//  test regex.wx_1
//
// To run all the regex tests:
//  test regex
//
// Some tests must be skipped since they use features which we do not make
// available through wxRegEx. To see the list of tests that have been skipped
// turn on verbose logging, e.g.:
//  test --verbose regex
//
// The tests here are for the builtin library, tests for wxRegEx in general
// should go in wxregex.cpp
//
// The tests are generated from Henry Spencer's reg.test, additional test
// can be added in wxreg.test. These test files are then turned into a C++
// include file 'regex.inc' (included below) using a script 'regex.pl'.
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


// many of the tests are specific to the builtin regex lib, so only attempts
// to do them when using the builtin regex lib.
//
#ifdef wxHAS_REGEX_ADVANCED

#include "wx/regex.h"
#include <string>
#include <vector>

using CppUnit::Test;
using CppUnit::TestCase;
using CppUnit::TestSuite;
using CppUnit::Exception;

using std::string;
using std::vector;

///////////////////////////////////////////////////////////////////////////////
// The test case - an instance represents a single test

class RegExTestCase : public TestCase
{
public:
    // constructor - create a single testcase
    RegExTestCase(
        const string& name,
        const char *mode,
        const char *id,
        const char *flags,
        const char *pattern,
        const char *data,
        const vector<const char *>& expected);

protected:
    // run this testcase
    void runTest();

private:
    // workers
    wxString Conv(const char *str);
    void parseFlags(const wxString& flags);
    void doTest(int flavor);
    static wxString quote(const wxString& arg);
    const wxChar *convError() const { return _T("<cannot convert>"); }

    // assertions - adds some information about the test that failed
    void fail(const wxString& msg) const;
    void failIf(bool condition, const wxString& msg) const
        { if (condition) fail(msg); }

    // mode, id, flags, pattern, test data, expected results...
    int m_mode;
    wxString m_id;
    wxString m_flags;
    wxString m_pattern;
    wxString m_data;
    wxArrayString m_expected;

    // the flag decoded
    int m_compileFlags;
    int m_matchFlags;
    bool m_basic;
    bool m_extended;
    bool m_advanced;
};

// constructor - throws Exception on failure
//
RegExTestCase::RegExTestCase(
    const string& name,
    const char *mode,
    const char *id,
    const char *flags,
    const char *pattern,
    const char *data,
    const vector<const char *>& expected)
  :
    TestCase(name),
    m_mode(mode[0]),
    m_id(Conv(id)),
    m_flags(Conv(flags)),
    m_pattern(Conv(pattern)),
    m_data(Conv(data)),
    m_compileFlags(0),
    m_matchFlags(0),
    m_basic(false),
    m_extended(false),
    m_advanced(false)
{
    bool badconv = m_pattern == convError() || m_data == convError();
    //RN:  Removing the std:: here will break MSVC6 compilation
    std::vector<const char *>::const_iterator it;

    for (it = expected.begin(); it != expected.end(); ++it) {
        m_expected.push_back(Conv(*it));
        badconv = badconv || *m_expected.rbegin() == convError();
    }

    failIf(badconv, _T("cannot convert to default character encoding"));

    // the flags need further parsing...
    parseFlags(m_flags);

#ifndef wxHAS_REGEX_ADVANCED
    failIf(!m_basic && !m_extended, _T("advanced regexs not available"));
#endif
}

int wxWcscmp(const wchar_t* s1, const wchar_t* s2)
{
    size_t nLen1 = wxWcslen(s1);
    size_t nLen2 = wxWcslen(s2);

    if (nLen1 != nLen2)
        return nLen1 - nLen2;

    return wxTmemcmp(s1, s2, nLen1);
}

// convert a string from UTF8 to the internal encoding
//
wxString RegExTestCase::Conv(const char *str)
{
    const wxWCharBuffer wstr = wxConvUTF8.cMB2WC(str);
    const wxWC2WXbuf buf = wxConvCurrent->cWC2WX(wstr);

    if (!buf || wxWcscmp(wxConvCurrent->cWX2WC(buf), wstr) != 0)
        return convError();
    else
        return buf;
}

// Parse flags
//
void RegExTestCase::parseFlags(const wxString& flags)
{
    for (const wxChar *p = flags; *p; p++) {
        switch (*p) {
            // noop
            case '-': break;

            // we don't fully support these flags, but they don't stop us
            // checking for success of failure of the match, so treat as noop
            case 'A': case 'B': case 'E': case 'H':
            case 'I': case 'L': case 'M': case 'N':
            case 'P': case 'Q': case 'R': case 'S':
            case 'T': case 'U': case '%':
                break;

            // match options
            case '^': m_matchFlags |= wxRE_NOTBOL; break;
            case '$': m_matchFlags |= wxRE_NOTEOL; break;
#if wxUSE_UNICODE
            case '*': break;
#endif
            // compile options
            case '&': m_advanced = m_basic = true; break;
            case 'b': m_basic = true; break;
            case 'e': m_extended = true; break;
            case 'i': m_compileFlags |= wxRE_ICASE; break;
            case 'o': m_compileFlags |= wxRE_NOSUB; break;
            case 'n': m_compileFlags |= wxRE_NEWLINE; break;
            case 't': if (strchr("ep", m_mode)) break; // else fall through...

            // anything else we must skip the test
            default:
                fail(wxString::Format(
                     _T("requires unsupported flag '%c'"), *p));
        }
    }
}

// Try test for all flavours of expression specified
//
void RegExTestCase::runTest()
{
    if (m_basic)
        doTest(wxRE_BASIC);
    if (m_extended)
        doTest(wxRE_EXTENDED);
#ifdef wxHAS_REGEX_ADVANCED
    if (m_advanced || (!m_basic && !m_extended))
        doTest(wxRE_ADVANCED);
#endif
}

// Try the test for a single flavour of expression
//
void RegExTestCase::doTest(int flavor)
{
    wxRegEx re(m_pattern, m_compileFlags | flavor);

    // 'e' - test that the pattern fails to compile
    if (m_mode == 'e') {
        failIf(re.IsValid(), _T("compile succeeded (should fail)"));
        return;
    }
    failIf(!re.IsValid(), _T("compile failed"));

    bool matches = re.Matches(m_data, m_matchFlags);

    // 'f' or 'p' - test that the pattern does not match
    if (m_mode == 'f' || m_mode == 'p') {
        failIf(matches, _T("match succeeded (should fail)"));
        return;
    }

    // otherwise 'm' or 'i' - test the pattern does match
    failIf(!matches, _T("match failed"));

    if (m_compileFlags & wxRE_NOSUB)
        return;

    // check wxRegEx has correctly counted the number of subexpressions
    wxString msg;
    msg << _T("GetMatchCount() == ") << re.GetMatchCount()
        << _T(", expected ") << m_expected.size();
    failIf(m_expected.size() != re.GetMatchCount(), msg);

    for (size_t i = 0; i < m_expected.size(); i++) {
        wxString result;
        size_t start, len;

        msg.clear();
        msg << _T("wxRegEx::GetMatch failed for match ") << i;
        failIf(!re.GetMatch(&start, &len, i), msg);

        // m - check the match returns the strings given
        if (m_mode == 'm')
            if (start < INT_MAX)
                result = m_data.substr(start, len);
            else
                result = _T("");

        // i - check the match returns the offsets given
        else if (m_mode == 'i')
            if (start > INT_MAX)
                result = _T("-1 -1");
            else if (start + len > 0)
                result << start << _T(" ") << start + len - 1;
            else
                result << start << _T(" -1");

        msg.clear();
        msg << _T("match(") << i << _T(") == ") << quote(result)
            << _T(", expected == ") << quote(m_expected[i]);
        failIf(result != m_expected[i], msg);
    }
}

// assertion - adds some information about the test that failed
//
void RegExTestCase::fail(const wxString& msg) const
{
    wxString str;
    wxArrayString::const_iterator it;

    str << (wxChar)m_mode << _T(" ") << m_id << _T(" ") << m_flags << _T(" ")
        << quote(m_pattern) << _T(" ") << quote(m_data);

    for (it = m_expected.begin(); it != m_expected.end(); ++it)
        str << _T(" ") << quote(*it);

    if (str.length() > 77)
        str = str.substr(0, 74) + _T("...");

    str << _T("\n ") << msg;

    // no lossy convs so using utf8
    CPPUNIT_FAIL(string(str.mb_str(wxConvUTF8)));
}

// quote a string so that it can be displayed (static)
//
wxString RegExTestCase::quote(const wxString& arg)
{
    const wxChar *needEscape = _T("\a\b\t\n\v\f\r\"\\");
    const wxChar *escapes = _T("abtnvfr\"\\");
    wxString str;

    for (size_t i = 0; i < arg.length(); i++) {
        wxUChar ch = arg[i];
        const wxChar *p = wxStrchr(needEscape, ch);

        if (p)
            str += wxString::Format(_T("\\%c"), escapes[p - needEscape]);
        else if (wxIscntrl(ch))
            str += wxString::Format(_T("\\%03o"), ch);
        else
            str += ch;
    }

    return str.length() == arg.length() && str.find(' ') == wxString::npos ?
        str : _T("\"") + str + _T("\"");
}


///////////////////////////////////////////////////////////////////////////////
// Test suite

class RegExTestSuite : public TestSuite
{
public:
    RegExTestSuite(string name) : TestSuite(name) { }
    void add(const char *mode, const char *id, const char *flags,
             const char *pattern, const char *data, const char *expected, ...);
};

// Add a testcase to the suite
//
void RegExTestSuite::add(
    const char *mode,
    const char *id,
    const char *flags,
    const char *pattern,
    const char *data,
    const char *expected, ...)
{
    string name = getName() + "." + id;

    vector<const char *> expected_results;
    va_list ap;

    for (va_start(ap, expected); expected; expected = va_arg(ap, const char *))
        expected_results.push_back(expected);

    va_end(ap);

    try {
        addTest(new RegExTestCase(
            name, mode, id, flags, pattern, data, expected_results));
    }
    catch (Exception& e) {
        wxLogInfo(wxString::Format(_T("skipping: %s\n %s\n"),
            wxString(name.c_str(), wxConvUTF8).c_str(),
            wxString(e.what(), wxConvUTF8).c_str()));
    }
}


// Include the generated tests
//
#include "regex.inc"


#endif // wxHAS_REGEX_ADVANCED
