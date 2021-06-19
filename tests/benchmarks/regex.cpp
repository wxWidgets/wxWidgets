/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/regex.cpp
// Purpose:     wxRegEx benchmarks
// Author:      Vadim Zeitlin
// Created:     2018-11-15
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/ffile.h"
#include "wx/regex.h"

#include "bench.h"

// ----------------------------------------------------------------------------
// Benchmark relative costs of compiling and matching for a simple regex
// ----------------------------------------------------------------------------

static const char* const RE_SIMPLE = ".";

BENCHMARK_FUNC(RECompile)
{
    return wxRegEx(RE_SIMPLE).IsValid();
}

BENCHMARK_FUNC(REMatch)
{
    static wxRegEx re(RE_SIMPLE);
    return re.Matches("foo");
}

BENCHMARK_FUNC(RECompileAndMatch)
{
    return wxRegEx(RE_SIMPLE).Matches("foo");
}

// ----------------------------------------------------------------------------
// Benchmark the cost of using a more complicated regex
// ----------------------------------------------------------------------------

namespace
{

// Use the contents of an already existing test file.
const wxString& GetTestText()
{
    static wxString text;
    if ( text.empty() )
    {
        wxFFile("htmltest.html").ReadAll(&text);
    }

    return text;
}

} // anonymous namespace

BENCHMARK_FUNC(REFindTD)
{
    // This is too simplistic, but good enough for benchmarking.
    static wxRegEx re("<td>[^<]*</td>", wxRE_ICASE | wxRE_NEWLINE);

    int matches = 0;
    for ( const wxChar* p = GetTestText().c_str(); re.Matches(p); ++matches )
    {
        size_t start, len;
        if ( !re.GetMatch(&start, &len) )
            return false;

        p += start + len;
    }

    return matches == 21; // result of "grep -c"
}
