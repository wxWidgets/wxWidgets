//
// SWIG Typemap library
// Dave Beazley
// May 5, 1997
//
// Perl5 implementation
//
// This library provides standard typemaps for modifying SWIG's behavior.
// With enough entries in this file, I hope that very few people actually
// ever need to write a typemap.
//

#ifdef AUTODOC
%section "Typemap Library (Perl 5)",info,after,pre,nosort,skip=1,chop_left=3,chop_right=0,chop_top=0,chop_bottom=0
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

%typemap(perl5,in) double *INPUT(double temp)
{
  temp = (double) SvNV($source);
  $target = &temp;
}

%typemap(perl5,in) float  *INPUT(float temp)
{
  temp = (float) SvNV($source);
  $target = &temp;
}

%typemap(perl5,in) int            *INPUT(int temp)
{
  temp = (int) SvIV($source);
  $target = &temp;
}

%typemap(perl5,in) short          *INPUT(short temp)
{
  temp = (short) SvIV($source);
  $target = &temp;
}

%typemap(perl5,in) long           *INPUT(long temp)
{
  temp = (long) SvIV($source);
  $target = &temp;
}
%typemap(perl5,in) unsigned int   *INPUT(unsigned int temp)
{
  temp = (unsigned int) SvIV($source);
  $target = &temp;
}
%typemap(perl5,in) unsigned short *INPUT(unsigned short temp)
{
  temp = (unsigned short) SvIV($source);
  $target = &temp;
}
%typemap(perl5,in) unsigned long  *INPUT(unsigned long temp)
{
  temp = (unsigned long) SvIV($source);
  $target = &temp;
}
%typemap(perl5,in) unsigned char  *INPUT(unsigned char temp)
{
  temp = (unsigned char) SvIV($source);
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
multiple output values, functions will return a Perl array.

         int            *OUTPUT
         short          *OUTPUT
         long           *OUTPUT
         unsigned int   *OUTPUT
         unsigned short *OUTPUT
         unsigned long  *OUTPUT
         unsigned char  *OUTPUT
         float          *OUTPUT
         double         *OUTPUT
         
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

The Perl output of the function would be an array containing both
output values. 

%}

#endif

// Force the argument to be ignored.

%typemap(perl5,ignore) int            *OUTPUT(int temp),
                       short          *OUTPUT(short temp),
                       long           *OUTPUT(long temp),
                       unsigned int   *OUTPUT(unsigned int temp),
                       unsigned short *OUTPUT(unsigned short temp),
                       unsigned long  *OUTPUT(unsigned long temp),
                       unsigned char  *OUTPUT(unsigned char temp),
                       float          *OUTPUT(float temp),
                       double         *OUTPUT(double temp)
{
  $target = &temp;
}

%typemap(perl5,argout) int            *OUTPUT,
                       short          *OUTPUT,
                       long           *OUTPUT,
                       unsigned int   *OUTPUT,
                       unsigned short *OUTPUT,
                       unsigned long  *OUTPUT,
                       unsigned char  *OUTPUT
{
  if (argvi >= items) {
    EXTEND(sp,1);
  }
  $target = sv_newmortal();
  sv_setiv($target,(IV) *($source));
  argvi++;
}

%typemap(perl5,argout) float    *OUTPUT,
                       double   *OUTPUT
{
  if (argvi >= items) {
    EXTEND(sp,1);
  }
  $target = sv_newmortal();
  sv_setnv($target,(double) *($source));
  argvi++;
}

// BOTH
// Mappings for an argument that is both an input and output
// parameter


#ifdef AUTODOC
%subsection "Input/Output Methods"

%text %{
The following methods can be applied to make a function parameter both
an input and output value.  This combines the behavior of both the
"INPUT" and "OUTPUT" methods described earlier.  Output values are
returned in the form of a Tcl list.

         int            *BOTH
         short          *BOTH
         long           *BOTH
         unsigned int   *BOTH
         unsigned short *BOTH
         unsigned long  *BOTH
         unsigned char  *BOTH
         float          *BOTH
         double         *BOTH
         
For example, suppose you were trying to wrap the following function :

        void neg(double *x) {
             *x = -(*x);
        }

You could wrap it with SWIG as follows :

        %include typemaps.i
        void neg(double *BOTH);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *BOTH { double *x };
        void neg(double *x);

Unlike C, this mapping does not directly modify the input value.
Rather, the modified input value shows up as the return value of the
function.  Thus, to apply this function to a Perl variable you might
do this :

       $x = neg($x);

%}

#endif

%typemap(perl5,in) int *BOTH = int *INPUT;
%typemap(perl5,in) short *BOTH = short *INPUT;
%typemap(perl5,in) long *BOTH = long *INPUT;
%typemap(perl5,in) unsigned *BOTH = unsigned *INPUT;
%typemap(perl5,in) unsigned short *BOTH = unsigned short *INPUT;
%typemap(perl5,in) unsigned long *BOTH = unsigned long *INPUT;
%typemap(perl5,in) unsigned char *BOTH = unsigned char *INPUT;
%typemap(perl5,in) float *BOTH = float *INPUT;
%typemap(perl5,in) double *BOTH = double *INPUT;

%typemap(perl5,argout) int *BOTH = int *OUTPUT;
%typemap(perl5,argout) short *BOTH = short *OUTPUT;
%typemap(perl5,argout) long *BOTH = long *OUTPUT;
%typemap(perl5,argout) unsigned *BOTH = unsigned *OUTPUT;
%typemap(perl5,argout) unsigned short *BOTH = unsigned short *OUTPUT;
%typemap(perl5,argout) unsigned long *BOTH = unsigned long *OUTPUT;
%typemap(perl5,argout) unsigned char *BOTH = unsigned char *OUTPUT;
%typemap(perl5,argout) float *BOTH = float *OUTPUT;
%typemap(perl5,argout) double *BOTH = double *OUTPUT;

// REFERENCE
// Accept Perl references as pointers


#ifdef AUTODOC
%subsection "Reference Methods"

%text %{
The following methods make Perl references work like simple C
pointers.  References can only be used for simple input/output
values, not C arrays however.  It should also be noted that 
REFERENCES are specific to Perl and not supported in other
scripting languages at this time.

         int            *REFERENCE
         short          *REFERENCE
         long           *REFERENCE
         unsigned int   *REFERENCE
         unsigned short *REFERENCE
         unsigned long  *REFERENCE
         unsigned char  *REFERENCE
         float          *REFERENCE
         double         *REFERENCE
         
For example, suppose you were trying to wrap the following function :

        void neg(double *x) {
             *x = -(*x);
        }

You could wrap it with SWIG as follows :

        %include typemaps.i
        void neg(double *REFERENCE);

or you can use the %apply directive :

        %include typemaps.i
        %apply double *REFERENCE { double *x };
        void neg(double *x);

Unlike the BOTH mapping described previous, this approach directly
modifies the value of a Perl reference.  Thus, you could use it
as follows :

       $x = 3;
       neg(\$x);
       print "$x\n";         # Should print out -3.
%}

#endif

%typemap(perl5,in) double *REFERENCE (double dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if ((!SvNOK(tempsv)) && (!SvIOK(tempsv))) {
	printf("Received %d\n", SvTYPE(tempsv));
	croak("Expected a double reference.");
  }
  dvalue = SvNV(tempsv);
  $target = &dvalue;
}

%typemap(perl5,in) float *REFERENCE (float dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if ((!SvNOK(tempsv)) && (!SvIOK(tempsv))) {
    croak("expected a double reference");
  }
  dvalue = (float) SvNV(tempsv);
  $target = &dvalue;
}

%typemap(perl5,in) int *REFERENCE (int dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if (!SvIOK(tempsv)) {
    croak("expected a integer reference");
  }
  dvalue = SvIV(tempsv);
  $target = &dvalue;
}

%typemap(perl5,in) short *REFERENCE (short dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if (!SvIOK(tempsv)) {
    croak("expected a integer reference");
  }
  dvalue = (short) SvIV(tempsv);
  $target = &dvalue;
}
%typemap(perl5,in) long *REFERENCE (long dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if (!SvIOK(tempsv)) {
    croak("expected a integer reference");
  }
  dvalue = (long) SvIV(tempsv);
  $target = &dvalue;
}
%typemap(perl5,in) unsigned int *REFERENCE (unsigned int dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if (!SvIOK(tempsv)) {
    croak("expected a integer reference");
  }
  dvalue = (unsigned int) SvIV(tempsv);
  $target = &dvalue;
}
%typemap(perl5,in) unsigned short *REFERENCE (unsigned short dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if (!SvIOK(tempsv)) {
    croak("expected a integer reference");
  }
  dvalue = (unsigned short) SvIV(tempsv);
  $target = &dvalue;
}
%typemap(perl5,in) unsigned long *REFERENCE (unsigned long dvalue)
{
  SV *tempsv;
  if (!SvROK($source)) {
    croak("expected a reference");
  }
  tempsv = SvRV($source);
  if (!SvIOK(tempsv)) {
    croak("expected a integer reference");
  }
  dvalue = (unsigned long) SvIV(tempsv);
  $target = &dvalue;
}

%typemap(perl5,argout) double *REFERENCE,
                       float  *REFERENCE
{
  SV *tempsv;
  tempsv = SvRV($arg);
  sv_setnv(tempsv, (double) *$source);
}

%typemap(perl5,argout) int            *REFERENCE,
                       short          *REFERENCE,
                       long           *REFERENCE,
                       unsigned int   *REFERENCE,
                       unsigned short *REFERENCE,
                       unsigned long  *REFERENCE
{
  SV *tempsv;
  tempsv = SvRV($arg);
  sv_setiv(tempsv, (int) *$source);
}

// --------------------------------------------------------------------
// Special types
//
// --------------------------------------------------------------------


