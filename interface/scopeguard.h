/////////////////////////////////////////////////////////////////////////////
// Name:        scopeguard.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

//@{
/**
    This family of macros is similar to wxON_BLOCK_EXIT()
    but calls a method of the given object instead of a free function.
*/
wxON_BLOCK_EXIT_OBJ0(obj, method);
wxON_BLOCK_EXIT_OBJ1(obj, method, p1);
wxON_BLOCK_EXIT_OBJ2(obj, method, p1, p2);
//@}


//@{
/**
    This family of macros allows to ensure that the global function @e func
    with 0, 1, 2 or more parameters (up to some implementaton-defined limit) is
    executed on scope exit, whether due to a normal function return or because an
    exception has been thrown. A typical example of its usage:

    @code
    void *buf = malloc(size);
        wxON_BLOCK_EXIT1(free, buf);
    @endcode

    Please see the original article by Andrei Alexandrescu and Petru Marginean
    published in December 2000 issue of C/C++ Users Journal for more
    details.

    @see wxON_BLOCK_EXIT_OBJ()
*/
wxON_BLOCK_EXIT0(func);
wxON_BLOCK_EXIT1(func, p1);
wxON_BLOCK_EXIT2(func, p1, p2);
//@}

