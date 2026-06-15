///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/fileconf.cpp
// Purpose:     wxFileConfig parsing code fuzzing test
// Author:      Arthur Chan
// Created:     2026-06-15
// Copyright:   (c) 2026 Arthur Chan
///////////////////////////////////////////////////////////////////////////////

#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/fileconf.h"

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

extern "C" int LLVMFuzzerTestOneInput(const wxUint8 *data, size_t size)
{
    wxLogNull noLog;

    wxMemoryInputStream mis(data, size);
    // The wxInputStream ctor reads the whole stream and runs the INI parser.
    wxFileConfig config(mis);

    // Walk the parsed entries and groups to exercise the read-back paths.
    wxString name;
    long index;
    for ( bool more = config.GetFirstEntry(name, index);
          more;
          more = config.GetNextEntry(name, index) ) {
        wxString value;
        config.Read(name, &value);
    }

    for ( bool more = config.GetFirstGroup(name, index);
          more;
          more = config.GetNextGroup(name, index) ) {
    }

    return 0;
}
