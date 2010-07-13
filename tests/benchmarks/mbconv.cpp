/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/mbconv.cpp
// Purpose:     MB<->WC conversion benchmarks
// Author:      Vadim Zeitlin
// Created:     2008-10-17
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/strconv.h"
#include "wx/string.h"

#include "bench.h"

namespace
{

const wchar_t *TEST_STRING =
    L"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod"
    L"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim"
    L"veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea"
    L"commodo consequat. Duis aute irure dolor in reprehenderit in voluptate"
    L"velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint"
    L"occaecat cupidatat non proident, sunt in culpa qui officia deserunt"
    L"mollit anim id est laborum."
    ;

// just compute the length of the resulting multibyte string
bool ComputeMBLength(const wxMBConv& conv)
{
    // we suppose a fixed length encoding here (which happens to cover UTF-8
    // too as long as the test string is ASCII)
    return conv.FromWChar(NULL, 0, TEST_STRING)
            == (wcslen(TEST_STRING) + 1)*conv.GetMBNulLen();
}

// perform the conversion
bool ConvertToMB(const wxMBConv& conv)
{
    const size_t outlen = (wcslen(TEST_STRING) + 1)*conv.GetMBNulLen();
    wxCharBuffer buf(outlen - 1); // it adds 1 internally
    return conv.FromWChar(buf.data(), outlen, TEST_STRING) == outlen;
}

} // anonymous namespace

BENCHMARK_FUNC(UTF16InitWX)
{
    wxMBConvUTF16 conv;
    return true;
}

BENCHMARK_FUNC(UTF16InitSys)
{
    wxCSConv conv("UTF-16LE");
    return conv.IsOk();
}

BENCHMARK_FUNC(UTF16LenWX)
{
    return ComputeMBLength(wxMBConvUTF16());
}

BENCHMARK_FUNC(UTF16LenSys)
{
    return ComputeMBLength(wxCSConv("UTF-16LE"));
}

BENCHMARK_FUNC(UTF16WX)
{
    return ConvertToMB(wxMBConvUTF16());
}

BENCHMARK_FUNC(UTF16Sys)
{
    return ConvertToMB(wxCSConv("UTF-16LE"));
}

