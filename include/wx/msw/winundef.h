/////////////////////////////////////////////////////////////////////////////
// Name:        winundef.h
// Purpose:     undefine the common symbols #define'd by <windows.h>
// Author:      Vadim Zeitlin
// Modified by:
// Created:     16.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUNDEF_H_
#define _WX_WINUNDEF_H_

// windows.h #defines the following identifiers which are also used in wxWin
#ifdef GetCharWidth
    #undef GetCharWidth
#endif

#ifdef FindWindow
    #undef FindWindow
#endif

#ifdef GetClassName
    #undef GetClassName
#endif

#ifdef GetClassInfo
    #undef GetClassInfo
#endif

#ifdef LoadAccelerators
    #undef LoadAccelerators
#endif

#ifdef GetWindowProc
    #undef GetWindowProc
#endif

#ifdef DrawText
    #undef DrawText
#endif

#ifdef StartDoc
    #undef StartDoc
#endif

#ifdef GetFirstChild
    #undef GetFirstChild
#endif

#ifdef GetNextChild
    #undef GetNextChild
#endif

#ifdef GetNextSibling
    #undef GetNextSibling
#endif

#ifdef GetObject
    #undef GetObject
#endif

#endif // _WX_WINUNDEF_H_
