/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/mslu.h
// Purpose:     Fixes for bugs in MSLU
// Author:      Vaclav Slavik
// Modified by:
// Created:     2002/02/17
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSLU_H_
#define _WX_MSLU_H_

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/wxchar.h"

#if !wxUSE_UNICODE_MSLU
inline bool wxUsingUnicowsDll() { return FALSE; }
#else

// Returns true if we are running under Unicode emulation in Win9x environment.
// Workaround hacks take effect only if this condition is met
inline bool wxUsingUnicowsDll()
{
    return (wxGetOsVersion() == wxWIN95);
}

//------------------------------------------------------------------------
// Wrongly implemented functions from unicows.dll
//------------------------------------------------------------------------

#if wxUSE_GUI

WXDLLEXPORT int wxMSLU_DrawStateW(WXHDC dc, WXHBRUSH br, WXFARPROC outputFunc, 
                                  WXLPARAM lData, WXWPARAM wData, 
                                  int x, int y, int cx, int cy, 
                                  unsigned int flags);
#define DrawStateW(dc, br, func, ld, wd, x, y, cx, cy, flags) \
    wxMSLU_DrawStateW((WXHDC)dc,(WXHBRUSH)br,(WXFARPROC)func, \
                      ld, wd, x, y, cx, cy, flags)

WXDLLEXPORT int wxMSLU_GetOpenFileNameW(void *ofn);
#define GetOpenFileNameW(ofn) wxMSLU_GetOpenFileNameW((void*)ofn)

WXDLLEXPORT int wxMSLU_GetSaveFileNameW(void *ofn);
#define GetSaveFileNameW(ofn) wxMSLU_GetSaveFileNameW((void*)ofn)

#endif

//------------------------------------------------------------------------
// Missing libc file manipulation functions in Win9x
//------------------------------------------------------------------------

WXDLLEXPORT int wxMSLU__trename(const wxChar *oldname, const wxChar *newname);
WXDLLEXPORT int wxMSLU__tremove(const wxChar *name);

#if defined( __VISUALC__ ) \
    || ( defined(__MINGW32__) && wxCHECK_W32API_VERSION( 0, 5 ) ) \
    || ( defined(__MWERKS__) && defined(__WXMSW__) ) \
    || ( defined(__BORLANDC__) && (__BORLANDC__ > 0x460) )
WXDLLEXPORT int wxMSLU__wopen(const wxChar *name, int flags, int mode);
WXDLLEXPORT int wxMSLU__waccess(const wxChar *name, int mode);
WXDLLEXPORT int wxMSLU__wmkdir(const wxChar *name);
WXDLLEXPORT int wxMSLU__wrmdir(const wxChar *name);
WXDLLEXPORT int wxMSLU__wstat(const wxChar *name, struct _stat *buffer);
#endif

#endif // wxUSE_UNICODE_MSLU

#endif // _WX_MSLU_H_
