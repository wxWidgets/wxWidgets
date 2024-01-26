/////////////////////////////////////////////////////////////////////////////
// Name:        chartype.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/** @addtogroup group_funcmacro_string */
///@{

/**
    Macro taking a literal string and expanding into a wide string.

    This macro should not be used in the new code any more as it is simply
    equivalent to using `L` string prefix now, i.e. its simplified definition
    could be just

    @code
    #define wxT(x)  L##x
    @endcode

    It used to be required when converting literal strings to wxString in
    wxWidgets versions prior to 2.9.0, and so can be found in a lot of existing
    code, but can be simply removed in any code using more recent versions of
    wxWidgets.

    @see @ref overview_unicode, wxS()

    @header{wx/chartype.h}
*/
#define wxT(string)

/**
    Obsolete macro which simply expands to its argument.

    This macro could be used in the code which needed to compile with both
    wxWidgets 2 and 3 versions in some rare circumstances. It is still provided
    for compatibility but serves no purpose any longer.

    @see wxT()

    @since 2.8.12, 2.9.2

    @header{wx/chartype.h}
 */
#define wxT_2(string)

/**
    wxS is a macro which can be used with character and string literals (in other words,
    @c 'x' or @c "foo") to convert them either to wide characters or wide strings
    in @c wchar_t-based (UTF-16) builds, or to keep them unchanged in @c char-based
    (UTF-8) builds.

    Basically this macro produces characters or strings of type wxStringCharType.

    The use of this macro is optional as the translation will always be done at
    run-time even if there is a mismatch between the kind of the literal used
    and the string or character type used in the current build.
    However using it can be beneficial in <b>performance-sensitive code</b> to
    do the conversion at compile-time instead.

    @see @ref overview_unicode, wxT()

    @header{wx/chartype.h}
*/
#define wxS(string)

/**
    This macro is exactly the same as wxT() and is defined in wxWidgets simply
    because it may be more intuitive for Windows programmers as the standard
    Win32 headers also define it (as well as yet another name for the same
    macro which is _TEXT()).

    Don't confuse this macro with _()!

    Note that since wxWidgets 2.9.0 the use of _T() is discouraged just like
    for wxT() and also that this macro may conflict with identifiers defined in
    standard headers of some compilers (such as Sun CC) so its use should
    really be avoided.

    @header{wx/chartype.h}
*/
#define _T(string)

/**
    wxChar is a compatibility typedef always defined as @c wchar_t now.

    Note that it is not affected by @c wxUSE_UNICODE_UTF8 option.
*/
typedef wchar_t wxChar;

/**
    wxSChar is a compatibility typedef always defined as @c wchar_t now.
*/
typedef wchar_t wxSChar;

/**
    wxUChar is a compatibility typedef always defined as @c wchar_t now.
*/
typedef wchar_t wxUChar;

/**
    wxStringCharType is defined to be:
    \- @c char when <tt>wxUSE_UNICODE_WCHAR==0</tt>
    \- @c wchar_t when <tt>wxUSE_UNICODE_WCHAR==1</tt>

    The @c wxUSE_UNICODE_WCHAR symbol is defined by default, but may be turned
    off in which case @c wxUSE_UNICODE_UTF8 is turned on.

    Note that wxStringCharType (as the name says) is the type used by wxString
    for internal storage of the characters.
*/
typedef wxUSE_UNICODE_WCHAR_dependent wxStringCharType;

///@}
