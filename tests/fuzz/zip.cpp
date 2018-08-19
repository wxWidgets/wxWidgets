///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/zip.cpp
// Purpose:     ZIP archives reading code fuzzing test
// Author:      Vadim Zeitlin
// Created:     2017-10-24
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/zipstrm.h"

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
    wxZipInputStream zis(mis);
    while ( wxZipEntry* const ze = zis.GetNextEntry() ) {
        zis.OpenEntry(*ze);
        delete ze;
    }

    return 0;
}
