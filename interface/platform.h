/////////////////////////////////////////////////////////////////////////////
    // Name:        platform.h
    // Purpose:     documentation for global functions
    // Author:      wxWidgets team
    // RCS-ID:      $Id$
    // Licence:     wxWindows license
    /////////////////////////////////////////////////////////////////////////////
    
    /**
    Same as wxCHECK_VERSION but also checks that
    @c wxSUBRELEASE_NUMBER is at least @e subrel.
*/
#define bool wxCHECK_VERSION_FULL(major, minor, release, subrel)     /* implementation is private */


    /**
    This is a macro which evaluates to @true if the current wxWidgets version is at
    least major.minor.release.
    
    For example, to test if the program is compiled with wxWidgets 2.2 or higher,
    the following can be done:
    @code
    wxString s;
    #if wxCHECK_VERSION(2, 2, 0)
        if ( s.StartsWith("foo") )
    #else // replacement code for old version
        if ( strncmp(s, "foo", 3) == 0 )
    #endif
        {
            ...
        }
    @endcode
*/
#define bool wxCHECK_VERSION(major, minor, release)     /* implementation is private */

/**
    Returns 1 if the compiler being used to compile the code is Visual C++
    compiler version @e major or greater. Otherwise, and also if
    the compiler is not Visual C++ at all, returns 0.
*/
#define bool wxCHECK_VISUALC_VERSION(major)     /* implementation is private */

/**
    Returns 1 if the compiler being used to compile the code is GNU C++
    compiler (g++) version major.minor or greater. Otherwise, and also if
    the compiler is not GNU C++ at all, returns 0.
*/
#define bool wxCHECK_GCC_VERSION(major, minor)     /* implementation is private */

/**
    Returns 1 if the compiler being used to compile the code is Sun CC Pro
    compiler and its version is at least @c major.minor. Otherwise returns
    0.
*/
#define bool wxCHECK_SUNCC_VERSION(major, minor)     /* implementation is private */

