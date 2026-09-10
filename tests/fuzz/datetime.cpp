///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/datetime.cpp
// Purpose:     wxDateTime parsing and formatting code fuzzing test
// Author:      Arthur Chan
// Created:     2026-09-02
// Copyright:   (c) 2026 Arthur Chan
///////////////////////////////////////////////////////////////////////////////

#include "wx/log.h"
#include "wx/datetime.h"

#if wxDEBUG_LEVEL

static void exitAssertHandler(const wxString& file,
                              int line,
                              const wxString& func,
                              const wxString& cond,
                              const wxString& msg);

static volatile wxAssertHandler_t
    origAssertHandler = wxSetAssertHandler(exitAssertHandler);

static void exitAssertHandler(const wxString& file,
                              int line,
                              const wxString& func,
                              const wxString& cond,
                              const wxString& msg)
{
    origAssertHandler(file, line, func, cond, msg);

    exit(1);
}

#endif // wxDEBUG_LEVEL

// Formats exercising a spread of specifiers. These are fixed rather than taken
// from the input because Format() and ParseFormat() treat an unknown specifier
// as a programmer error and assert on it, so a fuzzed format string would only
// ever report wxWidgets telling us the format is wrong.
static const char* const s_formats[] =
{
    "%Y-%m-%d %H:%M:%S",
    "%d/%m/%Y",
    "%a, %d %b %Y %H:%M:%S %z",
    "%c",
    "%x %X",
    "%j %U %W %p %I:%M",
};

static void RoundTrip(const wxDateTime& dt)
{
    // Format() falls back to an internal remapping for years outside the range
    // strftime() handles, and that path asserts its own invariant, so keep the
    // round-trip to years where it does not apply.
    const int year = dt.GetYear();
    if ( year < 1970 || year >= 2038 )
        return;

    for ( size_t n = 0; n < WXSIZEOF(s_formats); ++n )
        dt.Format(wxString::FromAscii(s_formats[n]));
}

extern "C" int LLVMFuzzerTestOneInput(const wxUint8 *data, size_t size)
{
    wxLogNull noLog;

    const wxString str =
        wxString::FromUTF8(reinterpret_cast<const char*>(data), size);

    wxString::const_iterator end;
    wxDateTime dt;

    for ( size_t n = 0; n < WXSIZEOF(s_formats); ++n )
    {
        if ( dt.ParseFormat(str, wxString::FromAscii(s_formats[n]), &end) &&
                dt.IsValid() )
            RoundTrip(dt);
    }

    if ( dt.ParseDateTime(str, &end) && dt.IsValid() )
        RoundTrip(dt);

    if ( dt.ParseDate(str, &end) && dt.IsValid() )
        RoundTrip(dt);

    if ( dt.ParseTime(str, &end) && dt.IsValid() )
        RoundTrip(dt);

    if ( dt.ParseRfc822Date(str, &end) && dt.IsValid() )
        RoundTrip(dt);

    if ( dt.ParseISODate(str) && dt.IsValid() )
        RoundTrip(dt);

    if ( dt.ParseISOTime(str) && dt.IsValid() )
        RoundTrip(dt);

    if ( dt.ParseISOCombined(str) && dt.IsValid() )
        RoundTrip(dt);

    return 0;
}
