///////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/beforestd.h
// Purpose:     #include before STL headers
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07/07/03
// RCS-ID:      $Id$
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

#ifdef _MSC_VER
    // these warning have to be disabled and not just temporarily disabled
    // because they will be given at the end of the compilation of the current
    // source -- and there is absolutely nothing we can do about them

    // 'foo': unreferenced inline function has been removed
    #pragma warning(disable:4514)

    // 'function' : function not inlined
    #pragma warning(disable:4710)

    // 'id': identifier was truncated to 'num' characters in the debug info
    #pragma warning(disable:4786)

    #pragma warning(push, 1)
#endif
