/////////////////////////////////////////////////////////////////////////////
// Name:        math.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @addtogroup group_funcmacro_math */
//@{

/**
    Returns a non-zero value if @a x is neither infinite nor NaN (not a
    number), returns 0 otherwise.

    @header{wx/math.h}
*/
int wxFinite(double x);

/**
    Returns a non-zero value if x is NaN (not a number), returns 0 otherwise.

    @header{wx/math.h}
*/
bool wxIsNaN(double x);

/**
    Converts the given array of 10 bytes (corresponding to 80 bits) to
    a float number according to the IEEE floating point standard format
    (aka IEEE standard 754).

    @see wxConvertToIeeeExtended() to perform the opposite operation
*/
wxFloat64 wxConvertFromIeeeExtended(const wxInt8 *bytes);

/**
    Converts the given floating number @a num in a sequence of 10 bytes
    which are stored in the given array @a bytes (which must be large enough)
    according to the IEEE floating point standard format
    (aka IEEE standard 754).

    @see wxConvertFromIeeeExtended() to perform the opposite operation
*/
void wxConvertToIeeeExtended(wxFloat64 num, wxInt8 *bytes);

//@}

