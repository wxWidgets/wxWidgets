/////////////////////////////////////////////////////////////////////////////
// Name:        wxcrt.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @addtogroup group_funcmacro_string */
//@{

/**
    @return @true if the pointer is either @NULL or points to an empty string,
             @false otherwise.

    @header{wx/wxcrt.h}
*/
bool wxIsEmpty(const char* p);

/**
    This is a safe version of standard function @e strlen(): it does exactly
    the same thing (i.e. returns the length of the string) except that it
    returns 0 if @a p is the @NULL pointer.

    @header{wx/wxcrt.h}
*/
size_t wxStrlen(const char* p);

/**
    This function complements the standard C function @e stricmp() which
    performs case-insensitive comparison.

    @return A negative value, 0, or positive value if @a p1 is less than,
             equal to or greater than @a p2. The comparison is case-sensitive.

    @header{wx/wxcrt.h}
*/
int wxStrcmp(const char* p1, const char* p2);

/**
    This function complements the standard C function @e strcmp() which performs
    case-sensitive comparison.

    @return A negative value, 0, or positive value if @a p1 is less than,
             equal to or greater than @e p2. The comparison is case-insensitive.

    @header{wx/wxcrt.h}
*/
int wxStricmp(const char* p1, const char* p2);

/**
    @deprecated Use wxString instead.

    This macro is defined as:

    @code
    #define wxStringEq(s1, s2) (s1 && s2 && (strcmp(s1, s2) == 0))
    @endcode

    @header{wx/wxcrt.h}
*/
bool wxStringEq(const wxString& s1, const wxString& s2);

/**
    @deprecated Use wxString::Find() instead.

    Returns @true if the substring @a s1 is found within @a s2, ignoring case
    if @a exact is @false. If @a subString is @false, no substring matching is
    done.

    @header{wx/wxcrt.h}
*/
bool wxStringMatch(const wxString& s1, const wxString& s2,
                   bool subString = true, bool exact = false);

/**
    This is a convenience function wrapping wxStringTokenizer which simply
    returns all tokens found in the given @a string in an array.

    Please see wxStringTokenizer::wxStringTokenizer() for a description of the
    other parameters.

    @header{wx/wxcrt.h}
*/
wxArrayString wxStringTokenize(const wxString& string,
                               const wxString& delims = wxDEFAULT_DELIMITERS,
                               wxStringTokenizerMode mode = wxTOKEN_DEFAULT);

/**
    Safe and more convenient replacement for strncpy().

    This function copies the source string @a src to the destination buffer @a
    dst of size @a n without overflowing the buffer and ensuring that it is
    always @NUL-terminated.

    Example of use:
    @code
        char buf[256];
        if ( wxStrlcpy(buf, GetSomeString(), WXSIZEOF(buf)) > WXSIZEOF(buf) )
            ... handle truncation ...
    @endcode
    Notice that using wxStrncpy() in similar way is wrong, the above is broadly
    equivalent to
    @code
        char buf[256];
        buf[WXSIZEOF(buf) - 1] = '\0';
        wxStrncpy(buf, GetSomeString(), WXSIZEOF(buf) - 1);
        if ( buf[WXSIZEOF(buf) - 1] != '\0' )
        {
            ... truncation occurred ...
            // need to NUL-terminate string manually
            buf[WXSIZEOF(buf) - 1] = '\0';
        }
    @endcode
    which should explain the advantage of using wxStrlcpy().

    Notice that this function is similar to the OpenBSD strlcpy() function.

    The template parameter @a T can be either @c char or @c wchar_t.

    @param dst
        Destination buffer of size (greater or) equal to @a n.
    @param src
        @NUL-terminated source string.
    @param n
        The size of the destination buffer.
    @return
        The length of @a src, if the returned value is greater or equal to @a n
        then there was not enough space in the destination buffer and the
        string was truncated.

    @since{2.9.0}

    @header{wx/wxcrt.h}
 */
template <typename T>
size_t wxStrlcpy(T *dst, const T *src, size_t n);

/**
    This function replaces the dangerous standard function @e sprintf() and is
    like @e snprintf() available on some platforms. The only difference with
    @e sprintf() is that an additional argument - buffer size - is taken and
    the buffer is never overflowed.

    Returns the number of characters copied to the buffer or -1 if there is not
    enough space.

    @see wxVsnprintf(), wxString::Printf()

    @header{wx/wxcrt.h}
*/
int wxSnprintf(wxChar* buf, size_t len, const wxChar* format, ...);

/**
    The same as wxSnprintf() but takes a @c va_list argument instead of an
    arbitrary number of parameters.

    @note If @c wxUSE_PRINTF_POS_PARAMS is set to 1, then this function
          supports positional arguments (see wxString::Printf() for more
          information). However other functions of the same family (wxPrintf(),
          wxSprintf(), wxFprintf(), wxVfprintf(), wxVfprintf(), wxVprintf(),
          wxVsprintf()) currently do not to support positional parameters even
          when @c wxUSE_PRINTF_POS_PARAMS is 1.

    @see wxSnprintf(), wxString::PrintfV()

    @header{wx/wxcrt.h}
*/
int wxVsnprintf(wxChar* buf, size_t len,
                const wxChar* format, va_list argPtr);

//@}

