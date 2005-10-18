/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/libraries.h
// Purpose:     Pragmas for linking libs conditionally
// Author:      Michael Wetherell
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Michael Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_LIBRARIES_H_
#define _WX_MSW_LIBRARIES_H_

// Notes:
//
// In general the preferred place to add libs is in the bakefiles. This file
// can be used where libs must be added conditionally, for those compilers that
// support a way to do that.

// Newer versions of VC++ support the /GS option which add buffer overflow
// checks. This adds into the generated code calls to support functions in in
// the runtime library.
//
// Starting with VC++ 8 this option is on by default, and code such as the
// runtime library that we need to link is compiled with it. Some versions of
// the runtime library don't include the support code, and a separate library
// bufferoverflowu.lib must be linked. If all versions of VC++ 8 that can
// compile wxWidgets have this lib we can link it here, if not then this will
// have to be removed.
//
// More information: http://support.microsoft.com/?id=894573

#if defined __VISUALC__ && __VISUALC__ >= 1400
    #pragma comment(lib,"bufferoverflowu.lib")
#endif

#endif // _WX_MSW_LIBRARIES_H_
