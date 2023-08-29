/////////////////////////////////////////////////////////////////////////////
// Name:        wx/memory.h
// Purpose:     Obsolete legacy header for memory debugging: don't include it
//              it any longer, it is only preserved to avoid breaking existing
//              code including it.
// Author:      Arthur Seaton, Julian Smart
// Modified by:
// Created:     29/01/98
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MEMORY_H_
#define _WX_MEMORY_H_

// These obsolete macros are still defined just in case anybody is using them,
// but nobody should.
#define WXDEBUG_DUMPDELAYCOUNTER

#define wxTrace(fmt)
#define wxTraceLevel(l, fmt)

#define WXTRACE true ? (void)0 : wxTrace
#define WXTRACELEVEL true ? (void)0 : wxTraceLevel

#endif // _WX_MEMORY_H_
