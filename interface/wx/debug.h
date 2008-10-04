/////////////////////////////////////////////////////////////////////////////
// Name:        debug.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @ingroup group_funcmacro_debug */
//@{

/**
    Assert macro. An error message will be generated if the condition is @false in
    debug mode, but nothing will be done in the release build.
    Please note that the condition in wxASSERT() should have no side effects
    because it will not be executed in release mode at all.

    @see wxASSERT_MSG(), wxCOMPILE_TIME_ASSERT()

    @header{wx/debug.h}
*/
#define wxASSERT( condition )

/**
    This macro results in a @ref wxCOMPILE_TIME_ASSERT "compile time assertion failure"
    if the size of the given @c type is less than @c size bits.

    You may use it like this, for example:

    @code
    // we rely on the int being able to hold values up to 2^32
    wxASSERT_MIN_BITSIZE(int, 32);

    // can't work with the platforms using UTF-8 for wchar_t
    wxASSERT_MIN_BITSIZE(wchar_t, 16);
    @endcode

    @header{wx/debug.h}
*/
#define wxASSERT_MIN_BITSIZE( type, size )

/**
    Assert macro with message.
    An error message will be generated if the condition is @false.

    @see wxASSERT(), wxCOMPILE_TIME_ASSERT()

    @header{wx/debug.h}
*/
#define wxASSERT_MSG( condition, message )

/**
    Checks that the condition is @true, returns with the given return value if
    not (stops execution in debug mode). This check is done even in release mode.

    @header{wx/debug.h}
*/
#define wxCHECK( condition, retValue )

/**
    Checks that the condition is @true, returns with the given return value if
    not (stops execution in debug mode). This check is done even in release mode.

    This macro may be only used in non-void functions, see also wxCHECK_RET().

    @header{wx/debug.h}
*/
#define wxCHECK_MSG( condition, retValue, message )

/**
    Checks that the condition is @true, and returns if not (stops execution
    with the given error message in debug mode). This check is done even in
    release mode.

    This macro should be used in void functions instead of wxCHECK_MSG().

    @header{wx/debug.h}
*/
#define wxCHECK_RET( condition, message )

/**
    Checks that the condition is @true, and if not, it will wxFAIL() and
    execute the given @c operation if it is not. This is a generalisation of
    wxCHECK() and may be used when something else than just returning from the
    function must be done when the @c condition is @false. This check is done
    even in release mode.

    @header{wx/debug.h}
*/
#define wxCHECK2(condition, operation)

/**
    This is the same as wxCHECK2(), but wxFAIL_MSG() with the specified
    @c message is called instead of wxFAIL() if the @c condition is @false.

    @header{wx/debug.h}
*/
#define wxCHECK2_MSG( condition, operation, message )

/**
    Using wxCOMPILE_TIME_ASSERT() results in a compilation error if the
    specified @c condition is @false. The compiler error message should include
    the @c message identifier - please note that it must be a valid C++
    identifier and not a string unlike in the other cases.

    This macro is mostly useful for testing the expressions involving the
    @c sizeof operator as they can't be tested by the preprocessor but it is
    sometimes desirable to test them at the compile time.

    Note that this macro internally declares a struct whose name it tries to
    make unique by using the @c __LINE__ in it but it may still not work if you
    use it on the same line in two different source files. In this case you may
    either change the line in which either of them appears on or use the
    wxCOMPILE_TIME_ASSERT2() macro.

    Also note that Microsoft Visual C++ has a bug which results in compiler
    errors if you use this macro with 'Program Database For Edit And Continue'
    (@c /ZI) option, so you shouldn't use it ('Program Database' (@c /Zi) is ok
    though) for the code making use of this macro.

    @see wxASSERT_MSG(), wxASSERT_MIN_BITSIZE()

    @header{wx/debug.h}
*/
#define wxCOMPILE_TIME_ASSERT( condition, message )

/**
    This macro is identical to wxCOMPILE_TIME_ASSERT() except that it allows
    you to specify a unique @c name for the struct internally defined by this
    macro to avoid getting the compilation errors described for
    wxCOMPILE_TIME_ASSERT().

    @header{wx/debug.h}
*/
#define wxCOMPILE_TIME_ASSERT2(condition, message, name)

/**
    Will always generate an assert error if this code is reached (in debug
    mode).

    @see wxFAIL_MSG()

    @header{wx/debug.h}
*/
#define wxFAIL()

/**
    Will always generate an assert error with specified message if this code is
    reached (in debug mode).

    This macro is useful for marking unreachable" code areas, for example it
    may be used in the "default:" branch of a switch statement if all possible
    cases are processed above.

    @see wxFAIL()

    @header{wx/debug.h}
*/
#define wxFAIL_MSG( message )

/**
    Returns @true if the program is running under debugger, @false otherwise.

    Please note that this function is currently only implemented for Win32 and
    Mac builds using CodeWarrior and always returns @false elsewhere.

    @header{wx/debug.h}
*/
bool wxIsDebuggerRunning();

/**
    This function is called whenever one of debugging macros fails (i.e.
    condition is @false in an assertion). It is only defined in the debug mode,
    in release builds the wxCHECK() failures don't result in anything.

    To override the default behaviour in the debug builds which is to show the
    user a dialog asking whether he wants to abort the program, continue or
    continue ignoring any subsequent assert failures, you may override
    wxApp::OnAssertFailure() which is called by this function if the global
    application object exists.

    @header{wx/debug.h}
*/
void wxOnAssert( const char* fileName,
                  int lineNumber,
                  const char* function,
                  const char* condition,
                  const char* message = NULL );

/**
    In debug mode (when @c __WXDEBUG__ is defined) this function generates a
    debugger exception meaning that the control is passed to the debugger if
    one is attached to the process. Otherwise the program just terminates
    abnormally. In release mode this function does nothing.

    @header{wx/debug.h}
*/
void wxTrap();

//@}



/** @ingroup group_funcmacro_misc */
//@{

/**
    This macro expands to the name of the current function if the compiler
    supports any of @c __FUNCTION__, @c __func__ or equivalent variables or
    macros or to @NULL if none of them is available.

    @header{wx/debug.h}
*/
#define __WXFUNCTION__

//@}

