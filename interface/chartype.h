/////////////////////////////////////////////////////////////////////////////
    // Name:        chartype.h
    // Purpose:     documentation for global functions
    // Author:      wxWidgets team
    // RCS-ID:      $Id$
    // Licence:     wxWindows license
    /////////////////////////////////////////////////////////////////////////////
    
    //@{
/**
    wxT() is a macro which can be used with character and string literals (in other
    words, @c 'x' or @c "foo") to automatically convert them to Unicode in
    Unicode build configuration. Please see the
    @ref overview_unicode "Unicode overview" for more information.
    
    This macro is simply returns the value passed to it without changes in ASCII
    build. In fact, its definition is:
    
    @code
    #ifdef UNICODE
    #define wxT(x) L ## x
    #else // !Unicode
    #define wxT(x) x
    #endif
    @endcode
*/
wxChar wxT(char ch);
    const wxChar * wxT(const char * s);
//@}


    //@{
/**
    wxS is macro which can be used with character and string literals to either
    convert them to wide characters or strings in @c wchar_t-based Unicode
    builds or keep them unchanged in UTF-8 builds. The use of this macro is
    optional as the translation will always be done at run-time even if there is a
    mismatch between the kind of the literal used and wxStringCharType used in the
    current build, but using it can be beneficial in performance-sensitive code to
    do the conversion at compile-time instead.
    
    @sa wxT
*/
wxStringCharType wxS(char ch);
    const wxStringCharType * wxS(const char * s);
//@}

