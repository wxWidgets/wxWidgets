/////////////////////////////////////////////////////////////////////////////
// Name:        chartype.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @ingroup group_funcmacro_string */
//@{

/**
    This macro can be used with character and string literals (in other words,
    @c 'x' or @c "foo") to automatically convert them to Unicode in Unicode
    builds of wxWidgets. This macro is simply returns the value passed to it
    without changes in ASCII build. In fact, its definition is:

@code
#ifdef UNICODE
#   define wxT(x) L ## x
#else // !Unicode
#   define wxT(x) x
#endif
@endcode

    @see @ref overview_unicode

    @header{wx/chartype.h}
*/
#define wxT(string)

/**
    wxS is macro which can be used with character and string literals to either
    convert them to wide characters or strings in @c wchar_t-based Unicode
    builds or keep them unchanged in UTF-8 builds. The use of this macro is
    optional as the translation will always be done at run-time even if there
    is a mismatch between the kind of the literal used and string or character
    type used in the current build, but using it can be beneficial in
    performance-sensitive code to do the conversion at compile-time instead.

    @see wxT()

    @header{wx/chartype.h}
*/
#define wxS(string)

/**
    This macro is exactly the same as wxT() and is defined in wxWidgets simply
    because it may be more intuitive for Windows programmers as the standard
    Win32 headers also define it (as well as yet another name for the same
    macro which is _TEXT()).

    Don't confuse this macro with _()!

    @header{wx/chartype.h}
*/
#define _T(string)

//@}
