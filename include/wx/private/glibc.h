///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/glibc.h
// Purpose:     glibc-specific private wx header
// Author:      Vadim Zeitlin
// Created:     2022-06-23
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_GLIBC_H_
#define _WX_PRIVATE_GLIBC_H_

// Ensure that a header include __GLIBC__ is defined.
#include <string.h>

// Macro for testing glibc version similar to wxCHECK_GCC_VERSION().
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
    #define wxCHECK_GLIBC_VERSION( major, minor ) \
        ( ( __GLIBC__ > (major) ) \
            || ( __GLIBC__ == (major) && __GLIBC_MINOR__ >= (minor) ) )
#else
    #define wxCHECK_GLIBC_VERSION( major, minor ) 0
#endif

#endif // _WX_PRIVATE_GLIBC_H_
