//
// SWIG Pointer manipulation library
// 
// This library can be used to manipulate C pointers.
%title "SWIG Pointer Library"

%module pointer


%section "Pointer Handling Library",noinfo,after,pre,nosort,skip=1,chop_left=3,chop_right=0,chop_top=0,chop_bottom=0

%text %{
%include pointer.i

The pointer.i library provides run-time support for managing and 
manipulating a variety of C/C++ pointer values.  In particular,
you can create various kinds of objects and dereference common
pointer types.  This is done through a common set of functions:

    ptrcast      - Casts a pointer to a new type
    ptrvalue     - Dereferences a pointer 
    ptrset       - Set the value of an object referenced by 
                   a pointer.
    ptrcreate    - Create a new object and return a pointer.
    ptrfree      - Free the memory allocated by ptrcreate.
    ptradd       - Increment/decrement a pointer value.
    ptrmap       - Make two datatypes equivalent to each other.
                   (Is a runtime equivalent of typedef).

When creating, dereferencing, or setting the value of pointer
variable, only the common C datatypes of int, short, long, float,
double, char, and char * are currently supported.   Other
datatypes may generate an error.

One of the more interesting aspects of this library is that
it operates with a wide range of datatypes.  For example,
the "ptrvalue" function can dereference "double *", "int *",
"long *", "char *", and other datatypes. Since SWIG encodes
pointers with type information, this can be done transparently
and in most cases, you can dereference a pointer without
ever knowing what type it actually is.

This library is primarily designed for utility, not high 
performance (the dynamic determination of pointer types takes
more work than most normal wrapper functions).  As a result,
you may achieve better performance by writing customized
"helper" functions if you're making lots of calls to these
functions in inner loops or other intensive operations.
%}

// This library is a pretty hideous mess of language dependent code.
// Grab the implementation from the appropriate libray

%include ptrlang.i




