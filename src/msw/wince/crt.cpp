/////////////////////////////////////////////////////////////////////////////
// Name:        wince/crt.cpp
// Purpose:     Implementation of CRT functions missing under Windows CE
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.04.04
// RCS-ID:
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/msw/wince/missing.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

extern "C" void *
bsearch(const void *key, const void *base, size_t num, size_t size,
        int (wxCMPFUNC_CONV *cmp)(const void *, const void *))
{
    int res;
    char *mid;

    char *lo = (char *)base;
    char *hi = lo + num*size;
    while ( lo < hi )
    {
        mid = lo + (hi - lo)/2;

        res = (*cmp)(key, mid);
        if ( res < 0 )
            hi = mid;
        else if ( res > 0 )
            lo = mid + size;
        else // res == 0
            return mid;
    }

    return NULL;
}


