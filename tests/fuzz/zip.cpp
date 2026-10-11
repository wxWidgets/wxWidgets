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

#include "exitassert.cpp"

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
