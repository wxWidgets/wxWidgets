// constarray.i
//
// This module changes SWIG to place constant values into a Tcl array


#ifdef AUTODOC
%subsection "Array Constants",pre
%text %{
%include constarray.i

This module changes SWIG so that constant values are placed into a Tcl
array instead of global variables.   The array is given the same name as
the SWIG module (specified with the %module directive).

This module should generally be included at the top of an interface
file before any declarations appear.   Furthermore, this module changes
the default handling of basic datatypes including integers, floats,
and character strings.

When this module is used, constants are simply accessed through the
module name.  For example :

     %module example
     ...
     #define FOO 42

would be accessed as '$example(FOO)'

Note : This module replaces the existing mechanism for creating constants.
The method used by this module is based on a set of typemaps supplied
by Tim Medley.
%}
#endif

%typemap(tcl,const) int SWIG_DEFAULT_TYPE, 
                    unsigned int SWIG_DEFAULT_TYPE,
                    long SWIG_DEFAULT_TYPE,
                    unsigned long SWIG_DEFAULT_TYPE,
                    short SWIG_DEFAULT_TYPE,
                    unsigned short SWIG_DEFAULT_TYPE,
                    unsigned char  SWIG_DEFAULT_TYPE,
                    signed char    SWIG_DEFAULT_TYPE
{
  static int ivalue = (int) $source;
  Tcl_LinkVar(interp,SWIG_name "($target)",(char *) &ivalue, TCL_LINK_INT | TCL_LINK_READ_ONLY);
}

%typemap(tcl,const) float  SWIG_DEFAULT_TYPE, 
                    double SWIG_DEFAULT_TYPE
{
  static double dvalue = (double) $source;
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &dvalue, TCL_LINK_DOUBLE | TCL_LINK_READ_ONLY);
}

%typemap(tcl,const) char  *SWIG_DEFAULT_TYPE
{
  static char *cvalue = $source;
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &cvalue, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
}

%typemap(tcl,const) Pointer *SWIG_DEFAULT_TYPE
{
  static char *pvalue;
  pvalue = (char *) malloc(20+strlen("$mangle"));
  SWIG_MakePtr(pvalue, (void *) ($source), "$mangle");
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &pvalue, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
}  

// ----------------------------------------------------------------------------------
// Tcl 8 Object versions
// ----------------------------------------------------------------------------------

%typemap(tcl8,const) int SWIG_DEFAULT_TYPE, 
                    unsigned int SWIG_DEFAULT_TYPE,
                    long SWIG_DEFAULT_TYPE,
                    unsigned long SWIG_DEFAULT_TYPE,
                    short SWIG_DEFAULT_TYPE,
                    unsigned short SWIG_DEFAULT_TYPE,
                    unsigned char  SWIG_DEFAULT_TYPE,
                    signed char    SWIG_DEFAULT_TYPE
{
  static int ivalue = (int) $source;
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &ivalue, TCL_LINK_INT | TCL_LINK_READ_ONLY);
}

%typemap(tcl8,const) float  SWIG_DEFAULT_TYPE, 
                    double SWIG_DEFAULT_TYPE
{
  static double dvalue = (double) $source;
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &dvalue, TCL_LINK_DOUBLE | TCL_LINK_READ_ONLY);
}

%typemap(tcl8,const) char  *SWIG_DEFAULT_TYPE
{
  static char *cvalue = $source;
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &cvalue, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
}

%typemap(tcl8,const) Pointer *SWIG_DEFAULT_TYPE
{
  static char *pvalue;
  pvalue = (char *) malloc(20+strlen("$mangle"));
  SWIG_MakePtr(pvalue, (void *) ($source), "$mangle");
  Tcl_LinkVar(interp, SWIG_prefix SWIG_name "($target)",(char *) &pvalue, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
}  


   
