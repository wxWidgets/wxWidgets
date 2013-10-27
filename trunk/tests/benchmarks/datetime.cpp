/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/datetime.cpp
// Purpose:     wxDateTime benchmarks
// Author:      Vadim Zeitlin
// Created:     2011-05-23
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/datetime.h"

#include "bench.h"

BENCHMARK_FUNC(ParseDate)
{
    wxDateTime dt;
    return dt.ParseDate("May 23, 2011") && dt.GetMonth() == wxDateTime::May;
}

