///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/utf8literals.cpp
// Purpose:     Test using string literals with wxALLOW_UTF8_LITERALS
// Author:      Vadim Zeitlin
// Created:     2026-10-19
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// Include this header first to be able to check wxUSE_UTF8_LOCALE_ONLY, but
// note that it doesn't include wx/string.h, so we can still define the
// symbols affecting it below.
#include "wx/platform.h"

// wxNO_IMPLICIT_WXSTRING_ENCODING can't be used in UTF-8-only build and MSVC
// uses UTF-8 for string literals only with /utf-8 (or equivalent) option,
// which is not used by all build systems, so only test wxALLOW_UTF8_LITERALS
// if it can be used.
#if !wxUSE_UTF8_LOCALE_ONLY && \
    (!defined(_MSC_VER) || \
        (defined(_MSVC_EXECUTION_CHARACTER_SET) && \
            _MSVC_EXECUTION_CHARACTER_SET == 65001))
    #define wxNO_IMPLICIT_WXSTRING_ENCODING
    #define wxALLOW_UTF8_LITERALS
#endif

#include "testprec.h"

#ifdef wxALLOW_UTF8_LITERALS

#include "wx/any.h"
#include "wx/crt.h"
#include "wx/fileconf.h"
#include "wx/sstream.h"

#include <type_traits>

// Check that literals, but only them, can be converted to wxString: this also
// verifies that the symbols defined above were really taken into account,
// which wouldn't be the case if precompiled headers were used for this file.
static_assert(std::is_convertible<const char (&)[4], wxString>::value,
              "string literals should be convertible to wxString");
static_assert(!std::is_convertible<char (&)[4], wxString>::value,
              "non-const char arrays shouldn't be convertible to wxString");
static_assert(!std::is_convertible<const char*, wxString>::value,
              "char pointers shouldn't be convertible to wxString");
static_assert(!std::is_convertible<std::string, wxString>::value,
              "std::string shouldn't be convertible to wxString");

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

// Note that the checks comparing with non-ASCII narrow string literals use
// extra parentheses to prevent Catch from seeing the literal: otherwise it
// would instantiate its comparison templates for it, and these templates may
// also be instantiated in the other test files, compiled without
// wxALLOW_UTF8_LITERALS, with a different meaning, and the linker could use
// their version instead of ours (this is an ODR violation). The checks using
// only ASCII literals don't need the parentheses because, although the same
// problem exists for them, both versions of the template give the same result
// for them, as ASCII strings are the same in UTF-8 and the current locale
// encoding.

namespace
{

// Use escapes in the literals to avoid depending on the source file encoding.
const wchar_t* const GRUSSE = L"Gr\u00fc\u00dfe";

void TakeString(const wxString& s)
{
    CHECK( s == GRUSSE );
}

} // anonymous namespace

TEST_CASE("wxALLOW_UTF8_LITERALS::String", "[string][utf8]")
{
    wxString s = "Gr\u00fc\u00dfe";
    CHECK( s == GRUSSE );

    wxString s2("Gr\u00fc\u00dfe");
    CHECK( s2 == GRUSSE );

    TakeString("Gr\u00fc\u00dfe");

    s = "x";
    CHECK( s == L"x" );

    s += "\u00e9";
    s << "y";
    s.Append("z");
    s.Prepend("<");
    CHECK( s == L"<x\u00e9yz" );

    CHECK( s + ">" == L"<x\u00e9yz>" );
    CHECK( "[" + s == L"[<x\u00e9yz" );

    UNSCOPED_INFO( "s = " << s );
    CHECK( (s == "<x\u00e9yz") );
    UNSCOPED_INFO( "s = " << s );
    CHECK( ("<x\u00e9yz" == s) );
    CHECK( s != "x" );
    CHECK( s < "z" );
    CHECK( s.IsSameAs("<X\u00e9YZ", false) );

    CHECK( s.Find("\u00e9") == 2 );
    CHECK( s.StartsWith("<x") );
    CHECK( s.EndsWith("yz") );

    CHECK( s.Replace("\u00e9", "e") == 1 );
    CHECK( s == "<xeyz" );

    // These overloads take the number of characters, just as the ones taking
    // const char* in the other build modes.
    CHECK( wxString("abcdef", 3) == "abc" );

    s.assign("abcdef", 2);
    CHECK( s == "ab" );

    s.replace(0, 1, "xyz", 2);
    CHECK( s == "xyb" );

    // String literals must be valid UTF-8.
    WX_ASSERT_FAILS_WITH_ASSERT( (void)wxString("\xff") );
}

TEST_CASE("wxALLOW_UTF8_LITERALS::Format", "[string][utf8]")
{
    CHECK( wxString::Format("%s=%d", L"x", 17) == "x=17" );
    CHECK( wxString::Format("Gr\u00fc\u00dfe %d", 1) == L"Gr\u00fc\u00dfe 1" );

    wxString s;
    s.Printf("%s!", GRUSSE);
    UNSCOPED_INFO( "s = " << s );
    CHECK( (s == "Gr\u00fc\u00dfe!") );

    // Format strings too.
    WX_ASSERT_FAILS_WITH_ASSERT( (void)wxString::Format("\xff %d", 1) );

    // Literals and other const char arrays can be used as arguments too.
    CHECK( wxString::Format("%s=%d", "x", 17) == "x=17" );
    CHECK( wxString::Format("%s!", "Gr\u00fc\u00dfe") == s );

    static constexpr char grusse[] = "Gr\u00fc\u00dfe";
    s.Printf("%s %s", grusse, wxString(GRUSSE));
    CHECK( s == wxString(GRUSSE) + " " + GRUSSE );

    wchar_t buf[64];
    wxSnprintf(buf, WXSIZEOF(buf), "<%s>", grusse);
    UNSCOPED_INFO( "buf = " << wxString(buf) );
    CHECK( (wxString(buf) == "<Gr\u00fc\u00dfe>") );

    // And they must be valid UTF-8 too.
    WX_ASSERT_FAILS_WITH_ASSERT( (void)wxString::Format("%s", "\xff") );
}

TEST_CASE("wxALLOW_UTF8_LITERALS::Other", "[string][utf8]")
{
    SECTION("wxAny")
    {
        wxAny a = "Gr\u00fc\u00dfe";
        CHECK( a.As<wxString>() == GRUSSE );
        CHECK( (a == "Gr\u00fc\u00dfe") );

        a = "x";
        CHECK( a.As<wxString>() == L"x" );
    }

#if wxUSE_FILECONFIG && wxUSE_STREAMS
    SECTION("wxConfig")
    {
        wxStringInputStream sis(wxString{});
        wxFileConfig fc(sis);

        CHECK( fc.Write("key", "Gr\u00fc\u00dfe") );
        CHECK( fc.Read("key", "default") == GRUSSE );
        CHECK( fc.Read("nokey", "d\u00e9faut") == L"d\u00e9faut" );
    }
#endif // wxUSE_FILECONFIG && wxUSE_STREAMS
}

#endif // wxALLOW_UTF8_LITERALS
