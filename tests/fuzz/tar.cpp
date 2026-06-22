///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/tar.cpp
// Purpose:     TAR archives reading code fuzzing test
// Author:      Arthur Chan
// Created:     2026-06-11
// Copyright:   (c) 2026 Arthur Chan
///////////////////////////////////////////////////////////////////////////////

#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/tarstrm.h"

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
    wxTarInputStream tis(mis);
    while ( wxTarEntry* const te = tis.GetNextEntry() ) {
        if ( tis.OpenEntry(*te) ) {
            // Read the entry data to exercise the size/offset parsing and OnSysRead
            char buf[4096];
            while ( tis.IsOk() && !tis.Eof() ) {
                tis.Read(buf, sizeof(buf));
                if ( tis.LastRead() == 0 )
                    break;
            }
            tis.CloseEntry();
        }
        delete te;
    }

    return 0;
}
