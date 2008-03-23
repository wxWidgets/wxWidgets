/////////////////////////////////////////////////////////////////////////////
// Name:        defs.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @ingroup group_funcmacro_byteorder */
//@{

/**
    This macro will swap the bytes of the @a value variable from little endian
    to big endian or vice versa unconditionally, i.e. independently of the
    current platform.

    @header{wx/defs.h}
*/
#define wxINT32_SWAP_ALWAYS( wxInt32 value )
#define wxUINT32_SWAP_ALWAYS( wxUint32 value )
#define wxINT16_SWAP_ALWAYS( wxInt16 value )
#define wxUINT16_SWAP_ALWAYS( wxUint16 value )

//@}

/** @ingroup group_funcmacro_byteorder */
//@{

/**
    This macro will swap the bytes of the @a value variable from little endian
    to big endian or vice versa if the program is compiled on a big-endian
    architecture (such as Sun work stations). If the program has been compiled
    on a little-endian architecture, the value will be unchanged.

    Use these macros to read data from and write data to a file that stores
    data in little-endian (for example Intel i386) format.

    @header{wx/defs.h}
*/
#define wxINT32_SWAP_ON_BE( wxInt32 value )
#define wxUINT32_SWAP_ON_BE( wxUint32 value )
#define wxINT16_SWAP_ON_BE( wxInt16 value )
#define wxUINT16_SWAP_ON_BE( wxUint16 value )

//@}

/** @ingroup group_funcmacro_byteorder */
//@{

/**
    This macro will swap the bytes of the @a value variable from little endian
    to big endian or vice versa if the program is compiled on a little-endian
    architecture (such as Intel PCs). If the program has been compiled on a
    big-endian architecture, the value will be unchanged.

    Use these macros to read data from and write data to a file that stores
    data in big-endian format.

    @header{wx/defs.h}
*/
#define wxINT32_SWAP_ON_LE( wxInt32 value )
#define wxUINT32_SWAP_ON_LE( wxUint32 value )
#define wxINT16_SWAP_ON_LE( wxInt16 value )
#define wxUINT16_SWAP_ON_LE( wxUint16 value )

//@}



/** @ingroup group_funcmacro_misc */
//@{

/**
    This macro can be used around a function declaration to generate warnings
    indicating that this function is deprecated (i.e. obsolete and planned to
    be removed in the future) when it is used. Only Visual C++ 7 and higher and
    g++ compilers currently support this functionality.

    Example of use:

    @code
    // old function, use wxString version instead
    wxDEPRECATED( void wxGetSomething(char *buf, size_t len) );

    // ...
    wxString wxGetSomething();
    @endcode

    @header{wx/defs.h}
*/
#define wxDEPRECATED(function)

/**
    This is a special version of wxDEPRECATED() macro which only does something
    when the deprecated function is used from the code outside wxWidgets itself
    but doesn't generate warnings when it is used from wxWidgets.

    It is used with the virtual functions which are called by the library
    itself -- even if such function is deprecated the library still has to call
    it to ensure that the existing code overriding it continues to work, but
    the use of this macro ensures that a deprecation warning will be generated
    if this function is used from the user code or, in case of Visual C++, even
    when it is simply overridden.

    @header{wx/defs.h}
*/
#define wxDEPRECATED_BUT_USED_INTERNALLY(function)

/**
    This macro is similar to wxDEPRECATED() but can be used to not only declare
    the function @a function as deprecated but to also provide its (inline)
    implementation @a body.

    It can be used as following:

    @code
    class wxFoo
    {
    public:
        // OldMethod() is deprecated, use NewMethod() instead
        void NewMethod();
        wxDEPRECATED_INLINE( void OldMethod(), NewMethod() );
    };
    @endcode

    @header{wx/defs.h}
*/
#define wxDEPRECATED_INLINE(func, body)

/**
    @c wxEXPLICIT is a macro which expands to the C++ @c explicit keyword if
    the compiler supports it or nothing otherwise. Thus, it can be used even in
    the code which might have to be compiled with an old compiler without
    support for this language feature but still take advantage of it when it is
    available.

    @header{wx/defs.h}
*/
#define wxEXPLICIT

/**
    GNU C++ compiler gives a warning for any class whose destructor is private
    unless it has a friend. This warning may sometimes be useful but it doesn't
    make sense for reference counted class which always delete themselves
    (hence destructor should be private) but don't necessarily have any
    friends, so this macro is provided to disable the warning in such case. The
    @a name parameter should be the name of the class but is only used to
    construct a unique friend class name internally.

    Example of using the macro:

    @code
    class RefCounted
    {
    public:
        RefCounted() { m_nRef = 1; }
        void IncRef() { m_nRef++ ; }
        void DecRef() { if ( !--m_nRef ) delete this; }

    private:
        ~RefCounted() { }

        wxSUPPRESS_GCC_PRIVATE_DTOR(RefCounted)
    };
    @endcode

    Notice that there should be no semicolon after this macro.

    @header{wx/defs.h}
*/
#define wxSUPPRESS_GCC_PRIVATE_DTOR_WARNING(name)

/**
    This macro is the same as the standard C99 @c va_copy for the compilers
    which support it or its replacement for those that don't. It must be used
    to preserve the value of a @c va_list object if you need to use it after
    passing it to another function because it can be modified by the latter.

    As with @c va_start, each call to @c wxVaCopy must have a matching
    @c va_end.

    @header{wx/defs.h}
*/
void wxVaCopy(va_list argptrDst, va_list argptrSrc);

//@}

