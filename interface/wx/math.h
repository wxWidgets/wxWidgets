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

//@}

