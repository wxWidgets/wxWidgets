/////////////////////////////////////////////////////////////////////////////
// Name:        math.h
// Purpose:     Declarations/definitions of common math functions
// Author:      John Labenski and others
// Modified by:
// Created:     02/02/03
// RCS-ID:
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MATH_H_
#define _WX_MATH_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "math.h"
#endif

#include "wx/defs.h"

// unknown __VISAGECC__, __SYMANTECCC__

#if defined(__VISUALC__) || defined(__BORLANDC__) || defined(__WATCOMC__)
    #include <float.h>
    #define wxFinite(x) _finite(x)
#elif defined(__GNUG__)||defined(__GNUWIN32__)||defined(__DJGPP__)|| \
      defined(__SGI_CC__)||defined(__SUNCC__)||defined(__XLC__)|| \
      defined(__HPUX__)||defined(__MWERKS__)
    #define wxFinite(x) finite(x)
#else
    #define wxFinite(x) ((x) == (x))
#endif


#if defined(__VISUALC__)||defined(__BORLAND__)
    #define wxIsNaN(x) _isnan(x)
#elif defined(__GNUG__)||defined(__GNUWIN32__)||defined(__DJGPP__)|| \
      defined(__SGI_CC__)||defined(__SUNCC__)||defined(__XLC__)|| \
      defined(__HPUX__)||defined(__MWERKS__)
    #define wxIsNaN(x) isnan(x)
#else
    #define wxIsNaN(x) ((x) != (x))
#endif


#endif // _WX_MATH_H_
