/* -----------------------------------------------------------------------------
 * python.swg
 *
 * Python configuration module.
 * ----------------------------------------------------------------------------- */

/* Python.h has to appear first */

%insert(runtime) %{
#include "Python.h"
%}

%insert(runtime) "precommon.swg";
%insert(runtime) "common.swg";          /* Common type-checking code */
%insert(runtime) "pyrun.swg";           /* Python run-time code */

/* Special directive for shadow code */

#define %shadow      %insert("shadow")
#define %pythoncode  %insert("python")

%include "pymacros.swg"


/* -----------------------------------------------------------------------------
 *  SWIGTYPE typemaps
 * ----------------------------------------------------------------------------- */

%include "pyswigtype.swg"


/* -----------------------------------------------------------------------------
 *  Check for local fragment defintions
 * ----------------------------------------------------------------------------- */

%include "my_fragments.i"


/* -----------------------------------------------------------------------------
 *  Typemap specializations
 * ----------------------------------------------------------------------------- */

%include "pyinout.swg"
%include "pyvoid.swg"
%include "pyobject.swg"
%include "pystrbase.swg"
%include "pystrings.swg"
%include "pyvaltypes.swg"
%include "pyptrtypes.swg"
%include "pyprimtypes.swg"
%include "pymisctypes.swg"
%include "pyenum.swg"

/* ------------------------------------------------------------
 *  Overloaded operator support
 * ------------------------------------------------------------ */
%include "pyopers.swg"

/* ------------------------------------------------------------
 * Warnings for Python keywords 
 * ------------------------------------------------------------ */
%include "pythonkw.swg"

/* ------------------------------------------------------------
 * The start of the Python initialization function 
 * ------------------------------------------------------------ */

%init %{
#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT(void) SWIG_init(void) {
  static PyObject *SWIG_globals = 0; 
  static int       typeinit = 0;
  PyObject *m, *d;
  int       i;
  if (!SWIG_globals) SWIG_globals = SWIG_newvarlink();
  m = Py_InitModule((char *) SWIG_name, SwigMethods);
  d = PyModule_GetDict(m);

  if (!typeinit) {
     for (i = 0; swig_types_initial[i]; i++) {
        swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
     }
     typeinit = 1;
  }
  SWIG_InstallConstants(d,swig_const_table);
%}


