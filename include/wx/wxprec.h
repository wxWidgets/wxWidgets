/////////////////////////////////////////////////////////////////////////////
// Name:        wxprec.h
// Purpose:     Includes the appropriate files for precompiled headers
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// compiler detection; includes setup.h
#include "wx/defs.h"

// check if to use precompiled headers: do it for most Windows compilers unless
// explicitly disabled by defining NOPCH
#if ((defined(__BORLANDC__) || defined(__VISUALC__) || defined(__WATCOMC__)) && defined(__WXMSW__)) || defined(__VISAGECPP__)
    #if !defined(NOPCH)
        #define WX_PRECOMP
    #endif
#endif

// For some reason, this must be defined for common dialogs to work.
#ifdef __WATCOMC__
    #define INCLUDE_COMMDLG_H  1
#endif

#ifdef WX_PRECOMP

// include standard Windows headers
#if defined(__WXMSW__) && !wxUSE_MFC
    #include <windows.h>
    #include "wx/msw/winundef.h"
#endif

// include the most common wx headers
#include "wx/wx.h"

#endif // WX_PRECOMP
