///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/seh.h
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
// wxSEHReport: this class is used as a namespace for the SEH-related functions
// ----------------------------------------------------------------------------

struct WXDLLIMPEXP_BASE wxSEHReport
{
    // set the name of the file to which the report is written, it is
    // constructed from the .exe name by default
    static void SetFileName(const wxChar *filename);

    // return the current file name
    static const wxChar *GetFileName();

    // write the exception report to the file, return true if it could be done
    // or false otherwise
    static bool Generate();
};

#endif // wxUSE_ON_FATAL_EXCEPTION

#endif // _WX_MSW_SEH_H_

