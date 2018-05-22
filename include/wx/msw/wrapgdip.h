///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/wrapgdip.h
// Purpose:     wrapper around <gdiplus.h> header
// Author:      Vadim Zeitlin
// Created:     2007-03-15
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WRAPGDIP_H_
#define _WX_MSW_WRAPGDIP_H_

#include "wx/msw/wrapwin.h"

// min and max must be available for gdiplus.h but we cannot define them as
// macros because they conflict with std::numeric_limits<T>::min and max when
// compiling with mingw-w64 and -std=c++17. This happens because with c++17,
// math.h includes bessel_function which requires std::numeric_limits.

#include <cmath>
using std::min;
using std::max;

// There are many clashes between the names of the member fields and parameters
// in the standard gdiplus.h header and each of them results in C4458 with
// VC14, so disable this warning for this file as there is no other way to
// avoid it.
#ifdef __VISUALC__
    #pragma warning(push)
    #pragma warning(disable:4458) // declaration of 'xxx' hides class member
#endif

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef __VISUALC__
    #pragma warning(pop)
#endif

#endif // _WX_MSW_WRAPGDIP_H_

