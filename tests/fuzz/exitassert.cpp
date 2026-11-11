///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/exitassert.cpp
// Purpose:     Fragment exiting on asserting failure in debug builds
// Author:      Vadim Zeitlin
// Created:     2026-10-11
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

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
