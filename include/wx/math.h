/**
* Name:        math.h
* Purpose:     Declarations/definitions of common math functions
* Author:      John Labenski and others
* Modified by:
* Created:     02/02/03
* RCS-ID:
* Copyright:   (c)
* Licence:     wxWindows licence
*/

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_MATH_H_
#define _WX_MATH_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "math.h"
#endif

#include "wx/defs.h"

#include <math.h>

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif

/* Scaling factors for various unit conversions */
#ifndef METRIC_CONVERSION_CONSTANT
    #define METRIC_CONVERSION_CONSTANT 0.0393700787
#endif

#ifndef mm2inches
    #define mm2inches (METRIC_CONVERSION_CONSTANT)
#endif

#ifndef inches2mm
    #define inches2mm (1/(mm2inches))
#endif

#ifndef mm2twips
    #define mm2twips (METRIC_CONVERSION_CONSTANT*1440)
#endif

#ifndef twips2mm
    #define twips2mm (1/(mm2twips))
#endif

#ifndef mm2pt
    #define mm2pt (METRIC_CONVERSION_CONSTANT*72)
#endif

#ifndef pt2mm
    #define pt2mm (1/(mm2pt))
#endif


/* unknown __VISAGECC__, __SYMANTECCC__ */

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


#endif /* _WX_MATH_H_ */
