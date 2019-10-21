///////////////////////////////////////////////////////////////////////////////
// Name:        wx/beforestd.h
// Purpose:     #include before STL headers
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07/07/03
// Copyright:   (c) 2003 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Unfortunately, when compiling at maximum warning level, the standard
    headers themselves may generate warnings -- and really lots of them. So
    before including them, this header should be included to temporarily
    suppress the warnings and after this the header afterstd.h should be
    included to enable them back again.

    Note that there are intentionally no inclusion guards in this file, because
    it can be included several times.
 */

/**
    GCC's visibility support is broken for libstdc++ in some older versions
    (namely Debian/Ubuntu's GCC 4.1, see
    https://bugs.launchpad.net/ubuntu/+source/gcc-4.1/+bug/109262). We fix it
    here by mimicking newer versions' behaviour of using default visibility
    for libstdc++ code.
 */
#if defined(HAVE_VISIBILITY) && defined(HAVE_BROKEN_LIBSTDCXX_VISIBILITY)
    #pragma GCC visibility push(default)
#endif
