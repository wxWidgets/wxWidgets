/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tls.h
// Purpose:     wxTLS_TYPE()
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    Macro to be used for thread-specific variables declarations.

    This macro can be used to define thread-specific variables of the specified
    @a type. Such variables must be global or static. Example of use:
    @code
    struct PerThreadData
    {
        ... data which will be different for every thread ...
    };

    static wxTLS_TYPE(PerThreadData *) s_threadPtr;
    @endcode

    Currently only types of size less than size of a pointer are supported.
    This limitation will be lifted in the future.
 */
#define wxTLS_TYPE(type)

