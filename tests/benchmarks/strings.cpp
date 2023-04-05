/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/strings.cpp
// Purpose:     String-related benchmarks
// Author:      Vadim Zeitlin
// Created:     2008-07-19
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/string.h"
#include "wx/ffile.h"
#include "wx/arrstr.h"

#include "bench.h"
#include "htmlparser/htmlpars.h"

static const char asciistr[] =
    "This is just the first line of a very long 7 bit ASCII string"
    "This is just the second line of a very long 7 bit ASCII string"
    "This is just the third line of a very long 7 bit ASCII string"
    "This is just the fourth line of a very long 7 bit ASCII string"
    "This is just the fifth line of a very long 7 bit ASCII string"
    "This is just the sixth line of a very long 7 bit ASCII string"
    "This is just the seventh line of a very long 7 bit ASCII string"
    "This is just the eighth line of a very long 7 bit ASCII string"
    "This is just the ninth line of a very long 7 bit ASCII string"
    "This is just the tenth line of a very long 7 bit ASCII string"
    ;

static const char utf8str[] =
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 0"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 1"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 2"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 3"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 4"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 5"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 6"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 7"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 8"
    "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5 \xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE 9"
    ;

namespace
{

const wxString& GetTestAsciiString()
{
    static wxString testString;
    if ( testString.empty() )
    {
        long num = Bench::GetNumericParameter();
        if ( !num )
            num = 1;

        for ( long n = 0; n < num; n++ )
            testString += wxString::FromAscii(asciistr);
    }

    return testString;
}

} // anonymous namespace

// this is just a baseline
BENCHMARK_FUNC(Strlen)
{
    if ( strlen(utf8str) != WXSIZEOF(utf8str) - 1 )
        return false;

    if ( strlen(asciistr) != WXSIZEOF(asciistr) - 1 )
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// FromUTF8() benchmarks
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(FromUTF8)
{
    wxString s = wxString::FromUTF8(utf8str);
    if ( s.empty() )
        return false;

    s = wxString::FromUTF8(asciistr);
    if ( s.empty() )
        return false;

    return true;
}

BENCHMARK_FUNC(FromUTF8WithNpos)
{
    wxString s = wxString::FromUTF8(utf8str, wxString::npos);
    if ( s.empty() )
        return false;

    s = wxString::FromUTF8(asciistr, wxString::npos);
    if ( s.empty() )
        return false;

    return true;
}

BENCHMARK_FUNC(FromUTF8WithLen)
{
    wxString s = wxString::FromUTF8(utf8str, WXSIZEOF(utf8str));
    if ( s.empty() )
        return false;

    s = wxString::FromUTF8(asciistr, WXSIZEOF(asciistr));
    if ( s.empty() )
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// FromUTF8Unchecked() benchmarks
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(FromUTF8Unchecked)
{
    wxString s = wxString::FromUTF8Unchecked(utf8str);
    if ( s.empty() )
        return false;

    s = wxString::FromUTF8Unchecked(asciistr);
    if ( s.empty() )
        return false;

    return true;
}

BENCHMARK_FUNC(FromUTF8UncheckedWithNpos)
{
    wxString s = wxString::FromUTF8Unchecked(utf8str, wxString::npos);
    if ( s.empty() )
        return false;

    s = wxString::FromUTF8Unchecked(asciistr, wxString::npos);
    if ( s.empty() )
        return false;

    return true;
}

BENCHMARK_FUNC(FromUTF8UncheckedWithLen)
{
    wxString s = wxString::FromUTF8Unchecked(utf8str, WXSIZEOF(utf8str));
    if ( s.empty() )
        return false;

    s = wxString::FromUTF8Unchecked(asciistr, WXSIZEOF(asciistr));
    if ( s.empty() )
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// FromAscii() benchmarks
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(FromAscii)
{
    wxString s = wxString::FromAscii(asciistr);
    if ( s.empty() )
        return false;

    return true;
}

BENCHMARK_FUNC(FromAsciiWithNpos)
{
    wxString s = wxString::FromAscii(asciistr);
    if ( s.empty() )
        return false;

    return true;
}

BENCHMARK_FUNC(FromAsciiWithLen)
{
    wxString s = wxString::FromAscii(asciistr, WXSIZEOF(asciistr));
    if ( s.empty() )
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// simple string iteration
// ----------------------------------------------------------------------------

// baseline
BENCHMARK_FUNC(ForCString)
{
    for ( size_t n = 0; n < WXSIZEOF(asciistr); n++ )
    {
        if ( asciistr[n] == '~' )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(ForStringIndex)
{
    const wxString& s = GetTestAsciiString();
    const size_t len = s.length();
    for ( size_t n = 0; n < len; n++ )
    {
        if ( s[n] == '~' )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(ForStringIter)
{
    const wxString& s = GetTestAsciiString();
    const wxString::const_iterator end = s.end();
    for ( wxString::const_iterator i = s.begin(); i != end; ++i )
    {
        if ( *i == '~' )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(ForStringRIter)
{
    const wxString& s = GetTestAsciiString();
    const wxString::const_reverse_iterator rend = s.rend();
    for ( wxString::const_reverse_iterator i = s.rbegin(); i != rend; ++i )
    {
        if ( *i == '~' )
            return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxString::Replace()
// ----------------------------------------------------------------------------

const size_t ASCIISTR_LEN = strlen(asciistr);

BENCHMARK_FUNC(ReplaceLoop)
{
    wxString str('x', ASCIISTR_LEN);
    for ( size_t n = 0; n < ASCIISTR_LEN; n++ )
    {
        if ( str[n] == 'a' )
            str[n] = 'z';
    }

    return str.length() != 0;
}

BENCHMARK_FUNC(ReplaceNone)
{
    wxString str('x', ASCIISTR_LEN);
    return str.Replace("a", "z") == 0;
}

BENCHMARK_FUNC(ReplaceSome)
{
    wxString str(asciistr);
    return str.Replace("7", "8") != 0;
}

BENCHMARK_FUNC(ReplaceAll)
{
    wxString str('x', ASCIISTR_LEN);
    return str.Replace("x", "y") != 0;
}

BENCHMARK_FUNC(ReplaceLonger)
{
    wxString str('x', ASCIISTR_LEN);
    return str.Replace("x", "yy") != 0;
}

BENCHMARK_FUNC(ReplaceShorter)
{
    wxString str('x', ASCIISTR_LEN);
    return str.Replace("xx", "y") != 0;
}

// ----------------------------------------------------------------------------
// string arrays
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(ArrStrPushBack)
{
    wxArrayString a;
    for (int i = 0; i < 100; ++i)
    {
        a.push_back(wxString(asciistr));
        a.push_back(wxString(utf8str));
    }
    return !a.empty();
}

BENCHMARK_FUNC(ArrStrInsert)
{
    wxArrayString a;
    for (int i = 0; i < 100; ++i)
    {
        a.insert(a.begin(), wxString(asciistr));
        a.insert(a.begin(), wxString(utf8str));
    }
    return !a.empty();
}

BENCHMARK_FUNC(ArrStrSort)
{
    wxArrayString a;
    a.reserve(100);
    for (int i = 0; i < 100; ++i)
        a.push_back(wxString(asciistr + i));
    a.Sort();
    return !a.empty();
}

BENCHMARK_FUNC(VectorStrPushBack)
{
    std::vector<wxString> v;
    for (int i = 0; i < 100; ++i)
    {
        v.push_back(wxString(asciistr));
        v.push_back(wxString(utf8str));
    }
    return !v.empty();
}

BENCHMARK_FUNC(VectorStrInsert)
{
    std::vector<wxString> v;
    for (int i = 0; i < 100; ++i)
    {
        v.insert(v.begin(), wxString(asciistr));
        v.insert(v.begin(), wxString(utf8str));
    }
    return !v.empty();
}

BENCHMARK_FUNC(VectorStrSort)
{
    std::vector<wxString> v;
    v.reserve(100);
    for (int i = 0; i < 100; ++i)
        v.push_back(wxString(asciistr + i));
    std::sort(v.begin(), v.end());
    return !v.empty();
}

// ----------------------------------------------------------------------------
// string case conversion
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(Lower)
{
    return GetTestAsciiString().Lower().length() > 0;
}

BENCHMARK_FUNC(Upper)
{
    return GetTestAsciiString().Upper().length() > 0;
}

// ----------------------------------------------------------------------------
// string comparison
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(StrcmpA)
{
    const wxString& s = GetTestAsciiString();

    return wxCRT_StrcmpA(s.c_str(), s.c_str()) == 0;
}

BENCHMARK_FUNC(StrcmpW)
{
    const wxString& s = GetTestAsciiString();

    return wxCRT_StrcmpW(s.wc_str(), s.wc_str()) == 0;
}

BENCHMARK_FUNC(StricmpA)
{
    const wxString& s = GetTestAsciiString();

    return wxCRT_StricmpA(s.c_str(), s.c_str()) == 0;
}

BENCHMARK_FUNC(StricmpW)
{
    const wxString& s = GetTestAsciiString();

    return wxCRT_StricmpW(s.wc_str(), s.wc_str()) == 0;
}

BENCHMARK_FUNC(StringCmp)
{
    const wxString& s = GetTestAsciiString();

    return s.Cmp(s) == 0;
}

BENCHMARK_FUNC(StringCmpNoCase)
{
    const wxString& s = GetTestAsciiString();

    return s.CmpNoCase(s) == 0;
}

// Also benchmark various native functions under MSW. Surprisingly/annoyingly
// they sometimes have vastly better performance than alternatives, especially
// for case-sensitive comparison (see #10375).
#ifdef __WINDOWS__

#include "wx/msw/wrapwin.h"

BENCHMARK_FUNC(MSWlstrcmp)
{
    const wxString& s = GetTestAsciiString();

    return lstrcmp(s.t_str(), s.t_str()) == 0;
}

BENCHMARK_FUNC(MSWlstrcmpi)
{
    const wxString& s = GetTestAsciiString();

    return lstrcmpi(s.t_str(), s.t_str()) == 0;
}

BENCHMARK_FUNC(MSWCompareString)
{
    const wxString& s = GetTestAsciiString();

    return ::CompareString
             (
                LOCALE_USER_DEFAULT,
                0,
                s.t_str(), s.length(),
                s.t_str(), s.length()
             ) == CSTR_EQUAL;
}

BENCHMARK_FUNC(MSWCompareStringIgnoreCase)
{
    const wxString& s = GetTestAsciiString();

    return ::CompareString
             (
                LOCALE_USER_DEFAULT,
                NORM_IGNORECASE,
                s.t_str(), s.length(),
                s.t_str(), s.length()
             ) == CSTR_EQUAL;
}

#endif // __WINDOWS__

// ----------------------------------------------------------------------------
// string buffers: wx[W]CharBuffer
// ----------------------------------------------------------------------------

BENCHMARK_FUNC(CharBuffer)
{
    wxString str(asciistr);

    // NB: wxStrlen() is here to simulate some use of the returned buffer.
    //     Both mb_str() and wc_str() are used so that this code does something
    //     nontrivial in any build.
    return wxStrlen(str.mb_str()) == ASCIISTR_LEN &&
           wxStrlen(str.wc_str()) == ASCIISTR_LEN;
}


// ----------------------------------------------------------------------------
// wxString::operator[] - parse large HTML page
// ----------------------------------------------------------------------------

class DummyParser : public wx28HtmlParser
{
public:
    virtual wxObject* GetProduct() { return nullptr; }
    virtual void AddText(const wxChar*) {}
};


BENCHMARK_FUNC(ParseHTML)
{
    // static so that construction time is not counted
    static DummyParser parser;
    static wxString html;
    if ( html.empty() )
    {
        wxString html1;
        wxFFile("htmltest.html").ReadAll(&html1, wxConvUTF8);

        // this is going to make for some invalid HTML, of course, but it
        // doesn't really matter
        long num = Bench::GetNumericParameter();
        if ( !num )
            num = 1;

        for ( long n = 0; n < num; n++ )
            html += html1;
    }

    parser.Parse(html);

    return true;
}

// ----------------------------------------------------------------------------
// conversions between strings and numbers
// ----------------------------------------------------------------------------

namespace
{

const struct ToDoubleData
{
    const char *str;
    double value;
    bool ok;
} toDoubleData[] =
{
    { "1", 1, true },
    { "1.23", 1.23, true },
    { ".1", .1, true },
    { "1.", 1, true },
    { "1..", 0, false },
    { "0", 0, true },
    { "a", 0, false },
    { "12345", 12345, true },
    { "-1", -1, true },
    { "--1", 0, false },
    { "-3E-5", -3E-5, true },
    { "-3E-abcde5", 0, false },
};

const struct FromDoubleData
{
    double value;
    int prec;
    const char *str;
} fromDoubleData[] =
{
    { 1.23,             -1, "1.23" },
    { -0.45678,         -1, "-0.45678" },
    { 1.2345678,         0, "1" },
    { 1.2345678,         1, "1.2" },
    { 1.2345678,         2, "1.23" },
    { 1.2345678,         3, "1.235" },
};

} // anonymous namespace

BENCHMARK_FUNC(StringToDouble)
{
    double d = 0.;
    for ( const auto& data : toDoubleData )
    {
        if ( wxString(data.str).ToDouble(&d) != data.ok )
            return false;

        if ( data.ok && d != data.value )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(StringToCDouble)
{
    double d = 0.;
    for ( const auto& data : toDoubleData )
    {
        if ( wxString(data.str).ToCDouble(&d) != data.ok )
            return false;

        if ( data.ok && d != data.value )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(StringFromDouble)
{
    for ( const auto& data : fromDoubleData )
    {
        const wxString& s = wxString::FromDouble(data.value, data.prec);
        if ( wxStrcmp(s.utf8_str(), data.str) != 0 )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(StringFromCDouble)
{
    for ( const auto& data : fromDoubleData )
    {
        const wxString& s = wxString::FromCDouble(data.value, data.prec);
        if ( wxStrcmp(s.utf8_str(), data.str) != 0 )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(Strtod)
{
    double d = 0.;
    char* end = nullptr;
    for ( const auto& data : toDoubleData )
    {
        d = strtod(data.str, &end);

        if ( (end && *end == '\0') != data.ok )
            return false;

        if ( data.ok && d != data.value )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(PrintfDouble)
{
    char buf[64];
    for ( const auto& data : fromDoubleData )
    {
        if ( data.prec == -1 )
        {
            if ( !snprintf(buf, sizeof(buf), "%g", data.value) )
                return false;
        }
        else
        {
            if ( !snprintf(buf, sizeof(buf), "%.*f", data.prec, data.value) )
                return false;
        }

        if ( strcmp(buf, data.str) != 0 )
            return false;
    }

    return true;
}

#if wxCHECK_CXX_STD(201703L)

#include <charconv>

#ifdef __cpp_lib_to_chars
BENCHMARK_FUNC(StdFromChars)
{
    double d = 0.;
    for ( const auto& data : toDoubleData )
    {
        const auto end = data.str + strlen(data.str);
        const auto res = std::from_chars(data.str, end, d);

        if ( (res.ptr == end && res.ec == std::errc{}) != data.ok )
            return false;

        if ( data.ok && d != data.value )
            return false;
    }

    return true;
}

BENCHMARK_FUNC(StdToChars)
{
    char buf[64];
    std::to_chars_result res;
    for ( const auto& data : fromDoubleData )
    {
        if ( data.prec == -1 )
        {
            res = std::to_chars(buf, buf + sizeof(buf), data.value);
        }
        else
        {
            res = std::to_chars(buf, buf + sizeof(buf), data.value,
                                std::chars_format::fixed, data.prec);
        }

        if ( res.ec != std::errc{} )
            return false;

        *res.ptr = '\0';

        if ( strcmp(buf, data.str) != 0 )
            return false;
    }

    return true;
}
#endif // __cpp_lib_to_chars

#endif // C++17
