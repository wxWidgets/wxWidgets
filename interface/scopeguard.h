/////////////////////////////////////////////////////////////////////////////
// Name:        scopeguard.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @ingroup group_funcmacro_misc */
//@{
/**
    This macro ensures that the global @a function with 0, 1, 2 or more
    parameters (up to some implementaton-defined limit) is executed on scope
    exit, whether due to a normal function return or because an exception has
    been thrown. A typical example of its usage:

    @code
    void *buf = malloc(size);
    wxON_BLOCK_EXIT1(free, buf);
    @endcode

    Please see the original article by Andrei Alexandrescu and Petru Marginean
    published in December 2000 issue of C/C++ Users Journal for more details.

    @see wxON_BLOCK_EXIT_OBJ0()

    @header{wx/scopeguard.h}
*/
#define wxON_BLOCK_EXIT0(function)
#define wxON_BLOCK_EXIT1(function, p1)
#define wxON_BLOCK_EXIT2(function, p1, p2)
//@}

/** @ingroup group_funcmacro_misc */
//@{
/**
    This family of macros is similar to wxON_BLOCK_EXIT0(), but calls a method
    of the given object instead of a free function.

    @header{wx/scopeguard.h}
*/
#define wxON_BLOCK_EXIT_OBJ0(object, method)
#define wxON_BLOCK_EXIT_OBJ1(object, method, p1)
#define wxON_BLOCK_EXIT_OBJ2(object, method, p1, p2)
//@}

