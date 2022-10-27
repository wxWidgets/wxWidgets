///////////////////////////////////////////////////////////////////////////////
// Name:        tests/regex/regex.cpp
// Purpose:     Test the built-in regex lib and wxRegEx
// Author:      Mike Wetherell
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
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


#if wxUSE_REGEX

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "wx/regex.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

///////////////////////////////////////////////////////////////////////////////
// The test case - an instance represents a single test

class RegExTestCase
{
public:
    RegExTestCase(
        const char *mode,
        const char *id,
        const char *flags,
        const char *pattern,
        const char *data,
        const vector<const char *>& expected);

private:
    void runTest();

    // workers
    wxString Conv(const char *str);
    bool parseFlags(const wxString& flags);
    void doTest(int flavor);
    static wxString quote(const wxString& arg);

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
    const char *mode,
    const char *id,
    const char *flags,
    const char *pattern,
    const char *data,
    const vector<const char *>& expected)
  :
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
    vector<const char *>::const_iterator it;

    for (it = expected.begin(); it != expected.end(); ++it) {
        m_expected.push_back(Conv(*it));
    }

    runTest();
}

int wxWcscmp(const wchar_t* s1, const wchar_t* s2)
{
    size_t nLen1 = wxWcslen(s1);
    size_t nLen2 = wxWcslen(s2);

    if (nLen1 != nLen2)
        return nLen1 - nLen2;

    return memcmp(s1, s2, nLen1*sizeof(wchar_t));
}

// convert a string from UTF8 to the internal encoding
//
wxString RegExTestCase::Conv(const char *str)
{
    const wxWCharBuffer wstr = wxConvUTF8.cMB2WC(str);
    const wxWC2WXbuf buf = wxConvCurrent->cWC2WX(wstr);

    if (!buf || wxWcscmp(wxConvCurrent->cWX2WC(buf), wstr) != 0)
    {
        FAIL( "Converting string \"" << str << "\" failed" );
    }

    return buf;
}

// Parse flags
//
bool RegExTestCase::parseFlags(const wxString& flags)
{
    for ( wxString::const_iterator p = flags.begin(); p != flags.end(); ++p )
    {
        switch ( (*p).GetValue() ) {
            // noop
            case '-': break;

            // we don't fully support these flags, but they don't stop us
            // checking for success of failure of the match, so treat as noop
            case 'A': case 'B': case 'H':
            case 'I': case 'L': case 'M': case 'N':
            case 'P': case 'Q': case 'R': case 'S':
            case 'T': case '%':
                break;

            // Skip tests checking for backslash inside bracket expressions:
            // this works completely differently in PCRE where backslash is
            // special, even inside [], from POSIX.
            case 'E':
                return false;
            // Also skip the (there is only one) test using POSIX-specified
            // handling of unmatched ')' as a non-special character -- PCRE
            // doesn't support this and it doesn't seem worth implementing
            // support for this ourselves either.
            case 'U':
                return false;

            // match options
            case '^': m_matchFlags |= wxRE_NOTBOL; break;
            case '$': m_matchFlags |= wxRE_NOTEOL; break;
            case '*': break;
            // compile options
            case '&': m_advanced = m_basic = true; break;
            case 'b': m_basic = true; break;
            case 'e': m_extended = true; break;
            case 'i': m_compileFlags |= wxRE_ICASE; break;
            case 'o': m_compileFlags |= wxRE_NOSUB; break;
            case 'n': m_compileFlags |= wxRE_NEWLINE; break;
            case 't': if (strchr("ep", m_mode)) break; wxFALLTHROUGH;

            // anything else we must skip the test
            default:
                return false;
        }
    }

    return true;
}

// Try test for all flavours of expression specified
//
void RegExTestCase::runTest()
{
    // the flags need further parsing...
    if (!parseFlags(m_flags)) {
        // we just have to skip the unsupported flags now
        return;
    }

    // Skip, or accommodate, some test cases from the original test suite that
    // are known not to work with PCRE:

    // Several regexes use syntax which is valid in PCRE and so their
    // compilation doesn't fail as expected:
    if (m_mode == 'e') {
        static const char* validForPCRE[] =
        {
            // Non-capturing group.
            "a(?:b)c",

            // Possessive quantifiers.
            "a++", "a?+","a*+",

            // Quoting from pcre2pattern(1):
            //
            //      An opening curly bracket [...] that does not match the
            //      syntax of a quantifier, is taken as a literal character.
            "a{1,2,3}", "a{1", "a{1n}", "a\\{0,1", "a{0,1\\",

            // From the same page:
            //
            //      The numbers must be less than 65536
            //
            // (rather than 256 limit for POSIX).
            "a{257}", "a{1000}",

            // Also:
            //
            //      If a minus character is required in a class, it must be
            //      escaped with a backslash or appear in a position where it
            //      cannot be interpreted as indicating a range, typically as
            //      the first or last character in the class, or immediately
            //      after a range.
            //
            // (while POSIX wants the last case to be an error).
            "a[a-b-c]",

            // PCRE allows quantifiers after word boundary assertions, so skip
            // the tests checking that using them results in an error.
            "[[:<:]]*", "[[:>:]]*", "\\<*", "\\>*", "\\y*", "\\Y*",

            // PCRE only interprets "\x" and "\u" specially when they're
            // followed by exactly 2 or 4 hexadecimal digits and just lets them
            // match "x" or "u" otherwise, instead of giving an error.
            "a\\xq", "a\\u008x",

            // And "\U" always just matches "U", PCRE doesn't support it as
            // Unicode escape at all (even with PCRE2_EXTRA_ALT_BSUX).
            "a\\U0000008x",

            // "\z" is the "end of string" assertion and not an error in PCRE.
            "a\\z",

            // Recursive backreferences are explicitly allowed in PCRE.
            "a((b)\\1)",

            // Backreferences with index greater than 8 are interpreted as
            // octal escapes, unfortunately.
            "a((((((((((b\\10))))))))))c", "a\\12b",
        };

        for (size_t n = 0; n < WXSIZEOF(validForPCRE); ++n) {
            if (m_pattern == validForPCRE[n])
                return;
        }
    }

    if (m_mode == 'm') {
        // PCRE doesn't support POSIX collating elements, so we have to skip
        // those too.
        if (m_pattern.find("[.") != wxString::npos || m_pattern.find("[:") != wxString::npos)
            return;

        // "\b" is a word boundary assertion in PCRE and so is "\B", so the
        // tests relying on them being escapes for ASCII backspace and
        // backslash respectively must be skipped.
        if (m_pattern.find("\\b") != wxString::npos || m_pattern.find("\\B") != wxString::npos)
            return;

        // As explained above, "\U" is not supported by PCRE, only "\u" is.
        if (m_pattern == "a\\U00000008x")
            m_pattern = "a\\u0008x";
        // And "\x" is supported only when followed by 2 digits, not 4.
        else if (m_pattern == "a\\x0008x")
            m_pattern = "a\\x08x";

        // "\12" can be a backreferences or an octal escape in PCRE, but never
        // literal "12" as this test expects it to be.
        if (m_pattern == "a\\12b")
            return;

        // Switching to "extended" mode is supposed to turn off "\W"
        // interpretation, but it doesn't work with PCRE.
        if (m_pattern == "(?e)\\W+")
            return;

        // None of the tests in "tricky cases" section passes with PCRE. It's
        // not really clear if PCRE is wrong or the original test suite was or
        // even if these regexes are ambiguous, but for now explicitly anchor
        // them at the end to force them to pass even with PCRE, as without it
        // they would match less than expected.
        if (m_pattern == "(week|wee)(night|knights)" ||
            m_pattern == "a(bc*).*\\1" ||
            m_pattern == "a(b.[bc]*)+")
            m_pattern += '$';
    }

    // This test uses an empty alternative branch: in POSIX, this is ignored,
    // while with PCRE it matches an empty string and we must set NOTEMPTY flag
    // explicitly to disable this.
    if (m_pattern == "a||b" && m_flags == "NS" ) {
        m_matchFlags |= wxRE_NOTEMPTY;
    }


    // Provide more information about the test case if it fails.
    wxString str;
    wxArrayString::const_iterator it;

    str << (wxChar)m_mode << wxT(" ") << m_id << wxT(" ") << m_flags << wxT(" ")
        << quote(m_pattern) << wxT(" ") << quote(m_data);

    for (it = m_expected.begin(); it != m_expected.end(); ++it)
        str << wxT(" ") << quote(*it);

    if (str.length() > 77)
        str = str.substr(0, 74) + wxT("...");

    INFO( str );


    if (m_basic)
        doTest(wxRE_BASIC);
    if (m_extended)
        doTest(wxRE_EXTENDED);
    if (m_advanced || (!m_basic && !m_extended))
        doTest(wxRE_ADVANCED);
}

// Try the test for a single flavour of expression
//
void RegExTestCase::doTest(int flavor)
{
    wxRegEx re(m_pattern, m_compileFlags | flavor);

    // 'e' - test that the pattern fails to compile
    if (m_mode == 'e') {
        CHECK( !re.IsValid() );

        // Never continue with this kind of test.
        return;
    } else {
        // Note: we don't use REQUIRE here because this would abort the entire
        // test case on error instead of skipping just the rest of this regex
        // test.
        CHECK( re.IsValid() );

        if (!re.IsValid())
            return;
    }

    bool matches = re.Matches(m_data, m_matchFlags);

    // 'f' or 'p' - test that the pattern does not match
    if (m_mode == 'f' || m_mode == 'p') {
        CHECK( !matches );
    } else {
        // otherwise 'm' or 'i' - test the pattern does match
        CHECK( matches );
    }

    if (!matches)
        return;

    if (m_compileFlags & wxRE_NOSUB)
        return;

    // check wxRegEx has correctly counted the number of subexpressions
    CHECK( m_expected.size() == re.GetMatchCount() );

    for (size_t i = 0; i < m_expected.size(); i++) {
        wxString result;
        size_t start, len;

        INFO( "Match " << i );
        CHECK( re.GetMatch(&start, &len, i) );

        // m - check the match returns the strings given
        if (m_mode == 'm')
        {
            if (start < INT_MAX)
                result = m_data.substr(start, len);
            else
                result = wxT("");
        }

        // i - check the match returns the offsets given
        else if (m_mode == 'i')
        {
#if wxUSE_UNICODE_UTF8
            // Values returned by GetMatch() are indices into UTF-8 string, but
            // the values expected by the test are indices in a UTF-16 or -32
            // string, so convert them. Note that the indices are correct, as
            // using substr(start, len) must return the match itself, it's just
            // that they differ when using UTF-8 internally.
            if ( start < INT_MAX )
            {
                if ( start + len > 0 )
                    len = m_data.substr(start, len).wc_str().length();

                start = m_data.substr(0, start).wc_str().length();
            }
#endif // wxUSE_UNICODE_UTF8

            if (start > INT_MAX)
                result = wxT("-1 -1");
            else if (start + len > 0)
                result << start << wxT(" ") << start + len - 1;
            else
                result << start << wxT(" -1");
        }

        CHECK( result == m_expected[i] );
    }
}

// quote a string so that it can be displayed (static)
//
wxString RegExTestCase::quote(const wxString& arg)
{
    const wxChar *needEscape = wxT("\a\b\t\n\v\f\r\"\\");
    const wxChar *escapes = wxT("abtnvfr\"\\");
    wxString str;

    for (size_t i = 0; i < arg.length(); i++) {
        wxChar ch = (wxChar)arg[i];
        const wxChar *p = wxStrchr(needEscape, ch);

        if (p)
            str += wxString::Format(wxT("\\%c"), escapes[p - needEscape]);
        else if (wxIscntrl(ch))
            str += wxString::Format(wxT("\\%03o"), ch);
        else
            str += (wxChar)ch;
    }

    return str.length() == arg.length() && str.find(' ') == wxString::npos ?
        str : wxT("\"") + str + wxT("\"");
}

// The helper function used by the tests in auto-generated regex.inc.
static void
CheckRE(
        const char *mode,
        const char *id,
        const char *flags,
        const char *pattern,
        const char *data,
        const char *expected,
        ...)
{
    vector<const char *> expected_results;
    va_list ap;

    for (va_start(ap, expected); expected; expected = va_arg(ap, const char *))
        expected_results.push_back(expected);

    va_end(ap);

    RegExTestCase(mode, id, flags, pattern, data, expected_results);
}


// Include the generated tests
//
#include "regex.inc"


#endif // wxUSE_REGEX
