//
// SWIG Typemap library
// Dave Beazley
// May 5, 1997
//
// Python implementation
//
// This library provides standard typemaps for modifying SWIG's behavior.
// With enough entries in this file, I hope that very few people actually
// ever need to write a typemap.
//
// Disclaimer : Unless you really understand how typemaps work, this file
// probably isn't going to make much sense.
//
#ifdef AUTODOC
%section "Typemap Library (Python)",info,after,pre,nosort,skip=1,chop_left=3,chop_right=0,chop_top=0,chop_bottom=0
%text %{
%include typemaps.i

The SWIG typemap library provides a language independent mechanism for
supporting output arguments, input values, and other C function
calling mechanisms.  The primary use of the library is to provide a
better interface to certain C function--especially those involving
pointers.
%}

#endif

// ------------------------------------------------------------------------
// Pointer handling
//
// These mappings provide support for input/output arguments and common
// uses for C/C++ pointers.
// ------------------------------------------------------------------------

// INPUT typemaps.
// These remap a C pointer to be an "INPUT" value which is passed by value
// instead of reference.


#ifdef AUTODOC
%subsection "Input Methods"

%text %{
The following methods can be applied to turn a pointer into a simple
"input" value.  That is, instead of passing a pointer to an object,
you would use a real value instead.

         int            *INPUT
         short          *INPUT
         long           *INPUT
         unsigned int   *INPUT
         unsigned short *INPUT
         unsigned long  *INPUT
         unsigned char  *INPUT
         float          *INPUT
         double         *INPUT

To use these, suppose you had a C function like this :

        double fadd(double *a, double *b) {
               return *a+*b;
        }

You could wrap it with SWIG as follows :

        %include typemaps.i
        double fadd(double *INPUT, double *INPUT);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *INPUT { double *a, double *b };
        double fadd(double *a, double *b);

%}
#endif

%typemap(python,in) double *INPUT(double temp)
{
  temp = PyFloat_AsDouble($source);
  $target = &temp;
}

%typemap(python,in) float  *INPUT(float temp)
{
  temp = (float) PyFloat_AsDouble($source);
  $target = &temp;
}

%typemap(python,in) int            *INPUT(int temp)
{
  temp = (int) PyInt_AsLong($source);
  $target = &temp;
}

%typemap(python,in) short          *INPUT(short temp)
{
  temp = (short) PyInt_AsLong($source);
  $target = &temp;
}

%typemap(python,in) long           *INPUT(long temp)
{
  temp = (long) PyInt_AsLong($source);
  $target = &temp;
}
%typemap(python,in) unsigned int   *INPUT(unsigned int temp)
{
  temp = (unsigned int) PyInt_AsLong($source);
  $target = &temp;
}
%typemap(python,in) unsigned short *INPUT(unsigned short temp)
{
  temp = (unsigned short) PyInt_AsLong($source);
  $target = &temp;
}
%typemap(python,in) unsigned long  *INPUT(unsigned long temp)
{
  temp = (unsigned long) PyInt_AsLong($source);
  $target = &temp;
}
%typemap(python,in) unsigned char  *INPUT(unsigned char temp)
{
  temp = (unsigned char) PyInt_AsLong($source);
  $target = &temp;
}

%typemap(python,in) signed char  *INPUT(signed char temp)
{
  temp = (unsigned char) PyInt_AsLong($source);
  $target = &temp;
}

// OUTPUT typemaps.   These typemaps are used for parameters that
// are output only.   The output value is appended to the result as
// a list element.

#ifdef AUTODOC
%subsection "Output Methods"

%text %{
The following methods can be applied to turn a pointer into an "output"
value.  When calling a function, no input value would be given for
a parameter, but an output value would be returned.  In the case of
multiple output values, they are returned in the form of a Python tuple.

         int            *OUTPUT
         short          *OUTPUT
         long           *OUTPUT
         unsigned int   *OUTPUT
         unsigned short *OUTPUT
         unsigned long  *OUTPUT
         unsigned char  *OUTPUT
         float          *OUTPUT
         double         *OUTPUT

A Python List can also be returned by using L_OUTPUT instead of OUTPUT.

For example, suppose you were trying to wrap the modf() function in the
C math library which splits x into integral and fractional parts (and
returns the integer part in one of its parameters).K:

        double modf(double x, double *ip);

You could wrap it with SWIG as follows :

        %include typemaps.i
        double modf(double x, double *OUTPUT);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *OUTPUT { double *ip };
        double modf(double x, double *ip);

The Python output of the function would be a tuple containing both
output values.
%}
#endif

// Helper function for List output

%{
static PyObject* l_output_helper(PyObject* target, PyObject* o) {
    PyObject*   o2;
    if (!target) {
        target = o;
    } else if (target == Py_None) {
        Py_DECREF(Py_None);
        target = o;
    } else {
        if (!PyList_Check(target)) {
            o2 = target;
            target = PyList_New(0);
            PyList_Append(target, o2);
	    Py_XDECREF(o2);
        }
        PyList_Append(target,o);
	Py_XDECREF(o);
    }
    return target;
}
%}

// Force the argument to be ignored.

%typemap(python,ignore) int            *L_OUTPUT(int temp),
                        short          *L_OUTPUT(short temp),
                        long           *L_OUTPUT(long temp),
                        unsigned int   *L_OUTPUT(unsigned int temp),
                        unsigned short *L_OUTPUT(unsigned short temp),
                        unsigned long  *L_OUTPUT(unsigned long temp),
                        unsigned char  *L_OUTPUT(unsigned char temp),
                        signed char    *L_OUTPUT(signed char temp),
                        float          *L_OUTPUT(float temp),
                        double         *L_OUTPUT(double temp)
{
  $target = &temp;
}

%typemap(python,argout) int            *L_OUTPUT,
                        short          *L_OUTPUT,
                        long           *L_OUTPUT,
                        unsigned int   *L_OUTPUT,
                        unsigned short *L_OUTPUT,
                        unsigned long  *L_OUTPUT,
                        unsigned char  *L_OUTPUT,
                        signed char    *L_OUTPUT
{
    PyObject *o;
    o = PyInt_FromLong((long) (*$source));
    l_output_helper($target,o);
}

%typemap(python,argout) float    *L_OUTPUT,
                        double   *L_OUTPUT
{
    PyObject *o;
    o = PyFloat_FromDouble((double) (*$source));
    $target = l_output_helper($target,o);
}

// These typemaps contributed by Robin Dunn
//----------------------------------------------------------------------
//
// T_OUTPUT typemap (and helper function) to return multiple argouts as
// a tuple instead of a list.
//
// Author: Robin Dunn
//----------------------------------------------------------------------

%{
static PyObject* t_output_helper(PyObject* target, PyObject* o) {
    PyObject*   o2;
    PyObject*   o3;

    if (!target) {
        target = o;
    } else if (target == Py_None) {
        Py_DECREF(Py_None);
        target = o;
    } else {
        if (!PyTuple_Check(target)) {
            o2 = target;
            target = PyTuple_New(1);
            PyTuple_SetItem(target, 0, o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3, 0, o);

        o2 = target;
        target = PySequence_Concat(o2, o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
    return target;
}
%}

// Force the argument to be ignored.
%typemap(python,ignore) int            *T_OUTPUT(int temp),
                        short          *T_OUTPUT(short temp),
                        long           *T_OUTPUT(long temp),
                        unsigned int   *T_OUTPUT(unsigned int temp),
                        unsigned short *T_OUTPUT(unsigned short temp),
                        unsigned long  *T_OUTPUT(unsigned long temp),
                        unsigned char  *T_OUTPUT(unsigned char temp),
                        float          *T_OUTPUT(float temp),
                        double         *T_OUTPUT(double temp)
{
  $target = &temp;
}

%typemap(python,argout) int            *T_OUTPUT,
                        short          *T_OUTPUT,
                        long           *T_OUTPUT,
                        unsigned int   *T_OUTPUT,
                        unsigned short *T_OUTPUT,
                        unsigned long  *T_OUTPUT,
                        unsigned char  *T_OUTPUT
{
    PyObject *o;
    o = PyInt_FromLong((long) (*$source));
    $target = t_output_helper($target, o);
}

%typemap(python,argout) float    *T_OUTPUT,
                        double   *T_OUTPUT
{
    PyObject *o;
    o = PyFloat_FromDouble((double) (*$source));
    $target = t_output_helper($target, o);
}

// Set the default output typemap

#ifdef OUTPUT_LIST
%typemap(python,ignore) int                *OUTPUT = int   *L_OUTPUT;
%typemap(python,ignore) short              *OUTPUT = short *L_OUTPUT;
%typemap(python,ignore) long               *OUTPUT = long  *L_OUTPUT;
%typemap(python,ignore) unsigned           *OUTPUT = unsigned *L_OUTPUT;
%typemap(python,ignore) unsigned short     *OUTPUT = unsigned short *L_OUTPUT;
%typemap(python,ignore) unsigned long      *OUTPUT = unsigned long  *L_OUTPUT;
%typemap(python,ignore) unsigned char      *OUTPUT = unsigned char  *L_OUTPUT;
%typemap(python,ignore) signed char        *OUTPUT = signed char    *L_OUTPUT;
%typemap(python,ignore) double             *OUTPUT = double         *L_OUTPUT;
%typemap(python,ignore) float              *OUTPUT = float          *L_OUTPUT;

%typemap(python,argout) int                *OUTPUT = int   *L_OUTPUT;
%typemap(python,argout) short              *OUTPUT = short *L_OUTPUT;
%typemap(python,argout) long               *OUTPUT = long  *L_OUTPUT;
%typemap(python,argout) unsigned           *OUTPUT = unsigned *L_OUTPUT;
%typemap(python,argout) unsigned short     *OUTPUT = unsigned short *L_OUTPUT;
%typemap(python,argout) unsigned long      *OUTPUT = unsigned long  *L_OUTPUT;
%typemap(python,argout) unsigned char      *OUTPUT = unsigned char  *L_OUTPUT;
%typemap(python,argout) signed char        *OUTPUT = signed char    *L_OUTPUT;
%typemap(python,argout) double             *OUTPUT = double         *L_OUTPUT;
%typemap(python,argout) float              *OUTPUT = float          *L_OUTPUT;
#else
%typemap(python,ignore) int                *OUTPUT = int   *T_OUTPUT;
%typemap(python,ignore) short              *OUTPUT = short *T_OUTPUT;
%typemap(python,ignore) long               *OUTPUT = long  *T_OUTPUT;
%typemap(python,ignore) unsigned           *OUTPUT = unsigned *T_OUTPUT;
%typemap(python,ignore) unsigned short     *OUTPUT = unsigned short *T_OUTPUT;
%typemap(python,ignore) unsigned long      *OUTPUT = unsigned long  *T_OUTPUT;
%typemap(python,ignore) unsigned char      *OUTPUT = unsigned char  *T_OUTPUT;
%typemap(python,ignore) signed char        *OUTPUT = signed char    *T_OUTPUT;
%typemap(python,ignore) double             *OUTPUT = double         *T_OUTPUT;
%typemap(python,ignore) float              *OUTPUT = float          *T_OUTPUT;

%typemap(python,argout) int                *OUTPUT = int   *T_OUTPUT;
%typemap(python,argout) short              *OUTPUT = short *T_OUTPUT;
%typemap(python,argout) long               *OUTPUT = long  *T_OUTPUT;
%typemap(python,argout) unsigned           *OUTPUT = unsigned *T_OUTPUT;
%typemap(python,argout) unsigned short     *OUTPUT = unsigned short *T_OUTPUT;
%typemap(python,argout) unsigned long      *OUTPUT = unsigned long  *T_OUTPUT;
%typemap(python,argout) unsigned char      *OUTPUT = unsigned char  *T_OUTPUT;
%typemap(python,argout) signed char        *OUTPUT = signed char    *T_OUTPUT;
%typemap(python,argout) double             *OUTPUT = double         *T_OUTPUT;
%typemap(python,argout) float              *OUTPUT = float          *T_OUTPUT;
#endif

// INOUT
// Mappings for an argument that is both an input and output
// parameter


#ifdef AUTODOC
%subsection "Input/Output Methods"

%text %{
The following methods can be applied to make a function parameter both
an input and output value.  This combines the behavior of both the
"INPUT" and "OUTPUT" methods described earlier.  Output values are
returned in the form of a Python tuple.  To return a Python list,
using L_INOUT instead.

         int            *INOUT
         short          *INOUT
         long           *INOUT
         unsigned int   *INOUT
         unsigned short *INOUT
         unsigned long  *INOUT
         unsigned char  *INOUT
         float          *INOUT
         double         *INOUT

For example, suppose you were trying to wrap the following function :

        void neg(double *x) {
             *x = -(*x);
        }

You could wrap it with SWIG as follows :

        %include typemaps.i
        void neg(double *INOUT);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *INOUT { double *x };
        void neg(double *x);

Unlike C, this mapping does not directly modify the input value (since
this makes no sense in Python).  Rather, the modified input value shows
up as the return value of the function.  Thus, to apply this function
to a Python variable you might do this :

       x = neg(x)

Note : previous versions of SWIG used the symbol 'BOTH' to mark
input/output arguments.   This is still supported, but will be slowly
phased out in future releases.
%}

#endif

%typemap(python,in) int *INOUT = int *INPUT;
%typemap(python,in) short *INOUT = short *INPUT;
%typemap(python,in) long *INOUT = long *INPUT;
%typemap(python,in) unsigned *INOUT = unsigned *INPUT;
%typemap(python,in) unsigned short *INOUT = unsigned short *INPUT;
%typemap(python,in) unsigned long *INOUT = unsigned long *INPUT;
%typemap(python,in) unsigned char *INOUT = unsigned char *INPUT;
%typemap(python,in) float *INOUT = float *INPUT;
%typemap(python,in) double *INOUT = double *INPUT;

%typemap(python,argout) int *INOUT = int *OUTPUT;
%typemap(python,argout) short *INOUT = short *OUTPUT;
%typemap(python,argout) long *INOUT = long *OUTPUT;
%typemap(python,argout) unsigned *INOUT = unsigned *OUTPUT;
%typemap(python,argout) unsigned short *INOUT = unsigned short *OUTPUT;
%typemap(python,argout) unsigned long *INOUT = unsigned long *OUTPUT;
%typemap(python,argout) unsigned char *INOUT = unsigned char *OUTPUT;
%typemap(python,argout) float *INOUT = float *OUTPUT;
%typemap(python,argout) double *INOUT = double *OUTPUT;

%typemap(python,in) int *T_INOUT = int *INPUT;
%typemap(python,in) short *T_INOUT = short *INPUT;
%typemap(python,in) long *T_INOUT = long *INPUT;
%typemap(python,in) unsigned *T_INOUT = unsigned *INPUT;
%typemap(python,in) unsigned short *T_INOUT = unsigned short *INPUT;
%typemap(python,in) unsigned long *T_INOUT = unsigned long *INPUT;
%typemap(python,in) unsigned char *T_INOUT = unsigned char *INPUT;
%typemap(python,in) float *T_INOUT = float *INPUT;
%typemap(python,in) double *T_INOUT = double *INPUT;

%typemap(python,argout) int *T_INOUT = int *T_OUTPUT;
%typemap(python,argout) short *T_INOUT = short *T_OUTPUT;
%typemap(python,argout) long *T_INOUT = long *T_OUTPUT;
%typemap(python,argout) unsigned *T_INOUT = unsigned *T_OUTPUT;
%typemap(python,argout) unsigned short *T_INOUT = unsigned short *T_OUTPUT;
%typemap(python,argout) unsigned long *T_INOUT = unsigned long *T_OUTPUT;
%typemap(python,argout) unsigned char *T_INOUT = unsigned char *T_OUTPUT;
%typemap(python,argout) float *T_INOUT = float *T_OUTPUT;
%typemap(python,argout) double *T_INOUT = double *T_OUTPUT;

%typemap(python,in) int *L_INOUT = int *INPUT;
%typemap(python,in) short *L_INOUT = short *INPUT;
%typemap(python,in) long *L_INOUT = long *INPUT;
%typemap(python,in) unsigned *L_INOUT = unsigned *INPUT;
%typemap(python,in) unsigned short *L_INOUT = unsigned short *INPUT;
%typemap(python,in) unsigned long *L_INOUT = unsigned long *INPUT;
%typemap(python,in) unsigned char *L_INOUT = unsigned char *INPUT;
%typemap(python,in) float *L_INOUT = float *INPUT;
%typemap(python,in) double *L_INOUT = double *INPUT;

%typemap(python,argout) int *L_INOUT = int *L_OUTPUT;
%typemap(python,argout) short *L_INOUT = short *L_OUTPUT;
%typemap(python,argout) long *L_INOUT = long *L_OUTPUT;
%typemap(python,argout) unsigned *L_INOUT = unsigned *L_OUTPUT;
%typemap(python,argout) unsigned short *L_INOUT = unsigned short *L_OUTPUT;
%typemap(python,argout) unsigned long *L_INOUT = unsigned long *L_OUTPUT;
%typemap(python,argout) unsigned char *L_INOUT = unsigned char *L_OUTPUT;
%typemap(python,argout) float *L_INOUT = float *L_OUTPUT;
%typemap(python,argout) double *L_INOUT = double *L_OUTPUT;

// Backwards compatibility

%typemap(python,in) int *BOTH = int *INOUT;
%typemap(python,in) short *BOTH = short *INOUT;
%typemap(python,in) long *BOTH = long *INOUT;
%typemap(python,in) unsigned *BOTH = unsigned *INOUT;
%typemap(python,in) unsigned short *BOTH = unsigned short *INOUT;
%typemap(python,in) unsigned long *BOTH = unsigned long *INOUT;
%typemap(python,in) unsigned char *BOTH = unsigned char *INOUT;
%typemap(python,in) float *BOTH = float *INOUT;
%typemap(python,in) double *BOTH = double *INOUT;

%typemap(python,argout) int *BOTH = int *INOUT;
%typemap(python,argout) short *BOTH = short *INOUT;
%typemap(python,argout) long *BOTH = long *INOUT;
%typemap(python,argout) unsigned *BOTH = unsigned *INOUT;
%typemap(python,argout) unsigned short *BOTH = unsigned short *INOUT;
%typemap(python,argout) unsigned long *BOTH = unsigned long *INOUT;
%typemap(python,argout) unsigned char *BOTH = unsigned char *INOUT;
%typemap(python,argout) float *BOTH = float *INOUT;
%typemap(python,argout) double *BOTH = double *INOUT;

%typemap(python,in) int *T_BOTH = int *T_INOUT;
%typemap(python,in) short *T_BOTH = short *T_INOUT;
%typemap(python,in) long *T_BOTH = long *T_INOUT;
%typemap(python,in) unsigned *T_BOTH = unsigned *T_INOUT;
%typemap(python,in) unsigned short *T_BOTH = unsigned short *T_INOUT;
%typemap(python,in) unsigned long *T_BOTH = unsigned long *T_INOUT;
%typemap(python,in) unsigned char *T_BOTH = unsigned char *T_INOUT;
%typemap(python,in) float *T_BOTH = float *T_INOUT;
%typemap(python,in) double *T_BOTH = double *T_INOUT;

%typemap(python,argout) int *T_BOTH = int *T_INOUT;
%typemap(python,argout) short *T_BOTH = short *T_INOUT;
%typemap(python,argout) long *T_BOTH = long *T_INOUT;
%typemap(python,argout) unsigned *T_BOTH = unsigned *T_INOUT;
%typemap(python,argout) unsigned short *T_BOTH = unsigned short *T_INOUT;
%typemap(python,argout) unsigned long *T_BOTH = unsigned long *T_INOUT;
%typemap(python,argout) unsigned char *T_BOTH = unsigned char *T_INOUT;
%typemap(python,argout) float *T_BOTH = float *T_INOUT;
%typemap(python,argout) double *T_BOTH = double *T_INOUT;

// --------------------------------------------------------------------
// Special types
//
// --------------------------------------------------------------------

#ifdef AUTODOC
%subsection "Special Methods"

%text %{
The typemaps.i library also provides the following mappings :

PyObject *

      When a PyObject * appears as either an input value or return
      value of a function, SWIG passes it through unmodified.  Thus,
      if you want to write a C function that operates on PyObjects,
      it is easy to write.  For example :

           %include typemaps.i
           PyObject *spam(PyObject *obj1, int n);

      Unlike normal Python wrapper functions, These functions can
      use any combination of parameters that you wish.

%}

#endif


// If a PyObject * appears as either an argument or a function return
// value, simply pass it straight through.

%typemap(python,in) PyObject * {
  $target = $source;
}

%typemap(python,out) PyObject * {
  $target = $source;
}

