///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/crashrpt.h
// Purpose:     helpers for the structured exception handling (SEH) under Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_SEH_H_
#define _WX_MSW_SEH_H_

#include "wx/defs.h"

#if wxUSE_ON_FATAL_EXCEPTION

// ----------------------------------------------------------------------------
// report generation flags
// ----------------------------------------------------------------------------

enum
{
    // we always report where the crash occured
    wxCRASH_REPORT_LOCATION = 0,

    // if this flag is given, the call stack is dumped
    wxCRASH_REPORT_STACK = 1,

    // if this flag is given, the values of the local variables are dumped
    wxCRASH_REPORT_LOCALS = 2,

    // if this flag is given, the values of all global variables are dumped
    //
    // WARNING: this may take a very long time and generate megabytes of output
    //          in a big program, this is why it is off by default
    wxCRASH_REPORT_GLOBALS = 4
};

// ----------------------------------------------------------------------------
// wxCrashReport: this class is used to create crash reports
// ----------------------------------------------------------------------------

struct WXDLLIMPEXP_BASE wxCrashReport
{
    // set the name of the file to which the report is written, it is
    // constructed from the .exe name by default
    static void SetFileName(const wxChar *filename);

    // return the current file name
    static const wxChar *GetFileName();

    // write the exception report to the file, return true if it could be done
    // or false otherwise
    static bool Generate(int flags = wxCRASH_REPORT_LOCATION |
                                     wxCRASH_REPORT_STACK |
                                     wxCRASH_REPORT_LOCALS);
};

#endif // wxUSE_ON_FATAL_EXCEPTION

#endif // _WX_MSW_SEH_H_

