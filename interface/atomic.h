/////////////////////////////////////////////////////////////////////////////
// Name:        atomic.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_atomic */
//@{

/**
    This function increments @a value in an atomic manner.

    @header{wx/atomic.h}
*/
void wxAtomicInc(wxAtomicInt& value);

/**
    This function decrements value in an atomic manner. Returns 0 if value is 0
    after decrementation or any non-zero value (not necessarily equal to the
    value of the variable) otherwise.

    @header{wx/atomic.h}
*/
wxInt32 wxAtomicDec(wxAtomicInt& value);

//@}

