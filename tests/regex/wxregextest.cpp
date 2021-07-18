///////////////////////////////////////////////////////////////////////////////
// Name:        tests/regex/wxregex.cpp
// Purpose:     Test wxRegEx
// Author:      Vadim Zeitlin, Mike Wetherell
// Copyright:   Vadim Zeitlin, Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#if wxUSE_REGEX

#include "wx/regex.h"
#include "wx/tokenzr.h"
#include <string>

using std::string;

// Display string for the flags
//
static wxString FlagStr(int flags)
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

TEST_CASE("wxRegEx::Compile", "[regex][compile]")
{
    wxRegEx re;

    CHECK      ( re.Compile("foo") );
    CHECK_FALSE( re.Compile("foo(") );
    CHECK_FALSE( re.Compile("foo(bar") );
    CHECK      ( re.Compile("foo(bar)") );
    CHECK_FALSE( re.Compile("foo[") );
    CHECK_FALSE( re.Compile("foo[bar") );
    CHECK      ( re.Compile("foo[bar]") );
    CHECK_FALSE( re.Compile("foo{1") );
    CHECK      ( re.Compile("foo{1}") );
    CHECK      ( re.Compile("foo{1,2}") );
    CHECK      ( re.Compile("foo*") );
    CHECK      ( re.Compile("foo+") );
    CHECK      ( re.Compile("foo?") );
}

static void
CheckMatch(const char* pattern,
           const char* text,
           const char* expected = NULL,
           int flags = wxRE_DEFAULT)
{
    int compileFlags = flags & ~(wxRE_NOTBOL | wxRE_NOTEOL);
    int matchFlags = flags & (wxRE_NOTBOL | wxRE_NOTEOL);

    INFO( "Pattern: " << pattern << FlagStr(flags) << ", match: " << text );

    wxRegEx re(pattern, compileFlags);
    REQUIRE( re.IsValid() );

    bool ok = re.Matches(text, matchFlags);

    if (expected) {
        REQUIRE( ok );

        wxStringTokenizer tkz(wxString(expected, *wxConvCurrent),
                              wxT("\t"), wxTOKEN_RET_EMPTY);
        size_t i;

        for (i = 0; i < re.GetMatchCount() && tkz.HasMoreTokens(); i++) {
            INFO( "Match #" << i );
            CHECK( re.GetMatch(text, i) == tkz.GetNextToken() );
        }

        if ((flags & wxRE_NOSUB) == 0)
            CHECK(re.GetMatchCount() == i);
    }
    else {
        CHECK( !ok );
    }
}

TEST_CASE("wxRegEx::Match", "[regex][match]")
{
    // Match tests
    // pattern, text, expected results (match, followed by submatches
    // tab separated, or NULL for no match expected)

    CheckMatch("foo", "bar");
    CheckMatch("foo", "foobar", "foo");
    CheckMatch("^foo", "foobar", "foo");
    CheckMatch("^foo", "barfoo");
    CheckMatch("bar$", "barbar", "bar");
    CheckMatch("bar$", "barbar ");
    CheckMatch("OoBa", "FoObAr", "oObA", wxRE_ICASE);
    CheckMatch("^[A-Z].*$", "AA\nbb\nCC", "AA\nbb\nCC");
    CheckMatch("^[A-Z].*$", "AA\nbb\nCC", "AA", wxRE_NEWLINE);
    CheckMatch("^[a-z].*$", "AA\nbb\nCC", "bb", wxRE_NEWLINE);
    CheckMatch("^[A-Z].*$", "AA\nbb\nCC", "CC", wxRE_NEWLINE | wxRE_NOTBOL);
    CheckMatch("^[A-Z].*$", "AA\nbb\nCC", NULL, wxRE_NEWLINE | wxRE_NOTBOL | wxRE_NOTEOL);
    CheckMatch("([[:alpha:]]+) ([[:alpha:]]+) ([[:digit:]]+).* ([[:digit:]]+)$",
        "Fri Jul 13 18:37:52 CEST 2001",
        "Fri Jul 13 18:37:52 CEST 2001\tFri\tJul\t13\t2001");
}

static void
CheckReplace(const char* pattern,
             const char* original,
             const char* replacement,
             const char* expected,
             size_t numMatches)
{
    wxRegEx re(pattern);

    wxString text(original);
    CHECK( re.Replace(&text, replacement) == numMatches );
    CHECK( text == expected );
}

TEST_CASE("wxRegEx::Replace", "[regex][replace]")
{
    // Replace tests
    // pattern, text, replacement, expected result and number of matches
    const char *patn = "([a-z]+)[^0-9]*([0-9]+)";
    CheckReplace(patn, "foo123", "bar", "bar", 1);
    CheckReplace(patn, "foo123", "\\2\\1", "123foo", 1);
    CheckReplace(patn, "foo_123", "\\2\\1", "123foo", 1);
    CheckReplace(patn, "123foo", "bar", "123foo", 0);
    CheckReplace(patn, "123foo456foo", "&&", "123foo456foo456foo", 1);
    CheckReplace(patn, "123foo456foo", "\\0\\0", "123foo456foo456foo", 1);
    CheckReplace(patn, "foo123foo123", "bar", "barbar", 2);
    CheckReplace(patn, "foo123_foo456_foo789", "bar", "bar_bar_bar", 3);
}

TEST_CASE("wxRegEx::QuoteMeta", "[regex][meta]")
{
    CHECK( wxRegEx::QuoteMeta("") == "" );
    CHECK( wxRegEx::QuoteMeta("a") == "a" );
    CHECK( wxRegEx::QuoteMeta("?") == "\\?" );
    CHECK( wxRegEx::QuoteMeta("\\") == "\\\\" );
    CHECK( wxRegEx::QuoteMeta("\\?!") == "\\\\\\?!" );
    CHECK( wxRegEx::QuoteMeta(":foo.*bar") == ":foo\\.\\*bar" );
}

#endif // wxUSE_REGEX
