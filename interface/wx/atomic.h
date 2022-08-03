/////////////////////////////////////////////////////////////////////////////
// Name:        atomic.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ============================================================================
// Global functions/macros
// ============================================================================

/** @addtogroup group_funcmacro_atomic */
///@{

/**
    This function increments @a value in an atomic manner.

    @note It is recommended to use @c std::atomic available in C++11 and later
    instead of this function in any new code.

    Whenever possible wxWidgets provides an efficient, CPU-specific,
    implementation of this function. If such implementation is available, the
    symbol wxHAS_ATOMIC_OPS is defined. Otherwise this function still exists
    but is implemented in a generic way using a critical section which can be
    prohibitively expensive for use in performance-sensitive code.

    Returns the new value after the increment (the return value is only
    available since wxWidgets 3.1.7, this function doesn't return anything in
    previous versions of the library).

    @header{wx/atomic.h}
*/
wxInt32 wxAtomicInc(wxAtomicInt& value);

/**
    This function decrements value in an atomic manner.

    Returns the new value after decrementing it.

    @see wxAtomicInc

    @header{wx/atomic.h}
*/
wxInt32 wxAtomicDec(wxAtomicInt& value);

///@}

