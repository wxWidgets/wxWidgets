///////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/crashrpt.h
// Purpose:     helpers for the structured exception handling (SEH)
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
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
    //
    // this results in dump/crash report as small as possible, this is the
    // default flag
    wxCRASH_REPORT_STACK = 1,

    // if this flag is given, the values of the local variables are dumped
    //
    // note that this will result in huge file containing the dump of the
    // entire process memory space when using mini dumps!
    wxCRASH_REPORT_LOCALS = 2,

    // if this flag is given, the values of all global variables are dumped
    //
    // this creates a much larger mini dump and also takes more time to
    // generate if our own crash reporting code is used
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
                                     wxCRASH_REPORT_STACK);
};

#endif // wxUSE_ON_FATAL_EXCEPTION

#endif // _WX_MSW_SEH_H_

