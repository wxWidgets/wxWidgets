// 
// SWIG constraint library
// Dave Beazley
// May 4, 1997
//
// This library file contains typemaps for implementing various kinds of 
// constraints.  Depends upon the SWIG exception library for generating
// errors in a language-independent manner.

#ifdef AUTODOC
%section "Constraint Library",info,after,pre,nosort,skip=1,chop_left=3,chop_right=0,chop_top=0,chop_bottom=0

%text %{
%include constraints.i

This library provides support for applying constraints to function
arguments.  Using a constraint, you can restrict arguments to be
positive numbers, non-NULL pointers, and so on.   The following
constraints are available :

      Number  POSITIVE        - Positive number (not zero)
      Number  NEGATIVE        - Negative number (not zero)
      Number  NONZERO         - Nonzero number
      Number  NONNEGATIVE     - Positive number (including zero)
      Number  NONPOSITIVE     - Negative number (including zero)
      Pointer NONNULL         - Non-NULL pointer
      Pointer ALIGN8          - 8-byte aligned pointer
      Pointer ALIGN4          - 4-byte aligned pointer
      Pointer ALIGN2          - 2-byte aligned pointer

To use the constraints, you need to "apply" them to specific
function arguments in your code.  This is done using the %apply
directive.   For example :

  %apply Number NONNEGATIVE { double nonneg };
  double sqrt(double nonneg);         // Name of argument must match
  
  %apply Pointer NONNULL { void *ptr };
  void *malloc(int POSITIVE);       // May return a NULL pointer
  void free(void *ptr);             // May not accept a NULL pointer

Any function argument of the type you specify with the %apply directive
will be checked with the appropriate constraint.   Multiple types may
be specified as follows :

  %apply Pointer NONNULL { void *, Vector *, List *, double *};

In this case, all of the types listed would be checked for non-NULL 
pointers.

The common datatypes of int, short, long, unsigned int, unsigned long,
unsigned short, unsigned char, signed char, float, and double can be
checked without using the %apply directive by simply using the 
constraint name as the parameter name. For example :

  double sqrt(double NONNEGATIVE);
  double log(double POSITIVE);

If you have used typedef to change type-names, you can also do this :

  %apply double { Real };       // Make everything defined for doubles
                                // work for Reals.
  Real sqrt(Real NONNEGATIVE);
  Real log(Real POSITIVE);

%}
#endif

%include exception.i

// Positive numbers

%typemap(check) int               POSITIVE,
                short             POSITIVE,
                long              POSITIVE,
                unsigned int      POSITIVE,
                unsigned short    POSITIVE,
                unsigned long     POSITIVE,
                signed char       POSITIVE,
                unsigned char     POSITIVE,
                float             POSITIVE,
                double            POSITIVE,
                Number            POSITIVE
{
  if ($target <= 0) {
    SWIG_exception(SWIG_ValueError,"Expected a positive value.");
  }
}

// Negative numbers

%typemap(check) int               NEGATIVE,
                short             NEGATIVE,
                long              NEGATIVE,
                unsigned int      NEGATIVE,
                unsigned short    NEGATIVE,
                unsigned long     NEGATIVE,
                signed char       NEGATIVE,
                unsigned char     NEGATIVE,
                float             NEGATIVE,
                double            NEGATIVE,
                Number            NEGATIVE
{
  if ($target >= 0) {
    SWIG_exception(SWIG_ValueError,"Expected a negative value.");
  }
}

// Nonzero numbers

%typemap(check) int               NONZERO,
                short             NONZERO,
                long              NONZERO,
                unsigned int      NONZERO,
                unsigned short    NONZERO,
                unsigned long     NONZERO,
                signed char       NONZERO,
                unsigned char     NONZERO,
                float             NONZERO,
                double            NONZERO,
                Number            NONZERO
{
  if ($target == 0) {
    SWIG_exception(SWIG_ValueError,"Expected a nonzero value.");
  }
}

// Nonnegative numbers

%typemap(check) int               NONNEGATIVE,
                short             NONNEGATIVE,
                long              NONNEGATIVE,
                unsigned int      NONNEGATIVE,
                unsigned short    NONNEGATIVE,
                unsigned long     NONNEGATIVE,
                signed char       NONNEGATIVE,
                unsigned char     NONNEGATIVE,
                float             NONNEGATIVE,
                double            NONNEGATIVE,
                Number            NONNEGATIVE
{
  if ($target < 0) {
    SWIG_exception(SWIG_ValueError,"Expected a non-negative value.");
  }
}

// Nonpositive numbers

%typemap(check) int               NONPOSITIVE,
                short             NONPOSITIVE,
                long              NONPOSITIVE,
                unsigned int      NONPOSITIVE,
                unsigned short    NONPOSITIVE,
                unsigned long     NONPOSITIVE,
                signed char       NONPOSITIVE,
                unsigned char     NONPOSITIVE,
                float             NONPOSITIVE,
                double            NONPOSITIVE,
                Number            NONPOSITIVE
{
  if ($target < 0) {
    SWIG_exception(SWIG_ValueError,"Expected a non-positive value.");
  }
}
                
// Non-NULL pointer

%typemap(check) void *            NONNULL,
                Pointer           NONNULL
{
  if (!$target) {
    SWIG_exception(SWIG_ValueError,"Received a NULL pointer.");
  }
}

// Aligned pointers

%typemap(check) void *            ALIGN8,
                Pointer           ALIGN8
{
   long tmp;
   tmp = (long) $target;
   if (tmp & 7) {
     SWIG_exception(SWIG_ValueError,"Pointer must be 8-byte aligned.");
   }
}

%typemap(check) void *            ALIGN4,
                Pointer           ALIGN4
{
   long tmp;
   tmp = (long) $target;
   if (tmp & 3) {
     SWIG_exception(SWIG_ValueError,"Pointer must be 4-byte aligned.");
   }
}

%typemap(check) void *            ALIGN2,
                Pointer           ALIGN2
{
   long tmp;
   tmp = (long) $target;
   if (tmp & 1) {
     SWIG_exception(SWIG_ValueError,"Pointer must be 2-byte aligned.");
   }
}


