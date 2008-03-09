/////////////////////////////////////////////////////////////////////////////
// Name:        cpp.h
// Purpose:     documentation for global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

//@{
/**
    These macro return the concatenation of the tokens passed as their arguments.
    Unlike when using the preprocessor @c  operator, the arguments undergo
    the macro expansion before being concatenated.
*/
wxCONCAT(x1, x2);
wxCONCAT3(x1, x2, x3);
wxCONCAT4(x1, x2, x3, x4);
wxCONCAT5(x1, x2, x3, x4, x5);
//@}


/**
Returns the string representation of the given symbol which can be either a
literal or a macro (hence the advantage of using this macro instead of the
standard preprocessor @c # operator which doesn't work with macros).
Notice that this macro always produces a @c char string, use
wxSTRINGIZE_T to build a wide string Unicode build.

@see wxCONCAT
*/
#define wxSTRINGIZE(x)     /* implementation is private */

/**
    Returns the string representation of the given symbol as either an ASCII or
    Unicode string, depending on the current build. This is the Unicode-friendly
    equivalent of wxSTRINGIZE.
*/
#define wxSTRINGIZE_T(x)     /* implementation is private */

