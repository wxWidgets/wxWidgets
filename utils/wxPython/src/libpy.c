/***********************************************************************
 * $Header$
 * swig_lib/python/python.cfg
 *
 * This file contains coded needed to add variable linking to the
 * Python interpreter.   C variables are added as a new kind of Python
 * datatype.
 *
 * Also contains supporting code for building python under Windows
 * and things like that.
 *
 * $Log$
 * Revision 1.1  1999/08/04 05:25:58  RD
 * New Makefile/Setup files supporting multiple dynamic extension modules
 * for unix systems.
 *
 * Fixes for the wxGLCanvas demo to work around a strange bug in gtk.
 *
 * SWIG support routines now compiled separately instead of being bundled
 * in wx.cpp.
 *
 ************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include "Python.h"
#ifdef __cplusplus
}
#endif

/* Definitions for Windows/Unix exporting */
#if defined(__WIN32__)
#   if defined(_MSC_VER)
#	define SWIGEXPORT(a,b) __declspec(dllexport) a b
#   else
#	if defined(__BORLANDC__)
#	    define SWIGEXPORT(a,b) a _export b
#	else
#	    define SWIGEXPORT(a,b) a b
#	endif
#   endif
#else
#   define SWIGEXPORT(a,b) a b
#endif

#ifdef SWIG_GLOBAL
#ifdef __cplusplus
#define SWIGSTATIC extern "C"
#else
#define SWIGSTATIC
#endif
#endif

#ifndef SWIGSTATIC
#define SWIGSTATIC static
#endif

typedef struct {
  char  *name;
  PyObject *(*get_attr)(void);
  int (*set_attr)(PyObject *);
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar **vars;
  int      nvars;
  int      maxvars;
} swig_varlinkobject;

/* ----------------------------------------------------------------------
   swig_varlink_repr()

   Function for python repr method
   ---------------------------------------------------------------------- */

static PyObject *
swig_varlink_repr(swig_varlinkobject *v)
{
  v = v;
  return PyString_FromString("<Global variables>");
}

/* ---------------------------------------------------------------------
   swig_varlink_print()

   Print out all of the global variable names
   --------------------------------------------------------------------- */

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags)
{

  int i = 0;
  flags = flags;
  fprintf(fp,"Global variables { ");
  while (v->vars[i]) {
    fprintf(fp,"%s", v->vars[i]->name);
    i++;
    if (v->vars[i]) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

/* --------------------------------------------------------------------
   swig_varlink_getattr
 
   This function gets the value of a variable and returns it as a
   PyObject.   In our case, we'll be looking at the datatype and
   converting into a number or string
   -------------------------------------------------------------------- */

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n)
{
  int i = 0;
  char temp[128];

  while (v->vars[i]) {
    if (strcmp(v->vars[i]->name,n) == 0) {
      return (*v->vars[i]->get_attr)();
    }
    i++;
  }
  sprintf(temp,"C global variable %s not found.", n);
  PyErr_SetString(PyExc_NameError,temp);
  return NULL;
}

/* -------------------------------------------------------------------
   swig_varlink_setattr()

   This function sets the value of a variable.
   ------------------------------------------------------------------- */

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p)
{
  char temp[128];
  int i = 0;
  while (v->vars[i]) {
    if (strcmp(v->vars[i]->name,n) == 0) {
      return (*v->vars[i]->set_attr)(p);
    }
    i++;
  }
  sprintf(temp,"C global variable %s not found.", n);
  PyErr_SetString(PyExc_NameError,temp);
  return 1;
}

statichere PyTypeObject varlinktype = {
/*  PyObject_HEAD_INIT(&PyType_Type)  Note : This doesn't work on some machines */
  PyObject_HEAD_INIT(0)              
  0,
  "varlink",                          /* Type name    */
  sizeof(swig_varlinkobject),         /* Basic size   */
  0,                                  /* Itemsize     */
  0,                                  /* Deallocator  */ 
  (printfunc) swig_varlink_print,     /* Print        */
  (getattrfunc) swig_varlink_getattr, /* get attr     */
  (setattrfunc) swig_varlink_setattr, /* Set attr     */
  0,                                  /* tp_compare   */
  (reprfunc) swig_varlink_repr,       /* tp_repr      */    
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_mapping*/
  0,                                  /* tp_hash      */
};

/* Create a variable linking object for use later */

SWIGSTATIC PyObject *
SWIG_newvarlink(void)
{
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  /*  _Py_NewReference(result);  Does not seem to be necessary */
  result->nvars = 0;
  result->maxvars = 64;
  result->vars = (swig_globalvar **) malloc(64*sizeof(swig_globalvar *));
  result->vars[0] = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

SWIGSTATIC void
SWIG_addvarlink(PyObject *p, char *name,
	   PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p))
{
  swig_varlinkobject *v;
  v= (swig_varlinkobject *) p;
	
  if (v->nvars >= v->maxvars -1) {
    v->maxvars = 2*v->maxvars;
    v->vars = (swig_globalvar **) realloc(v->vars,v->maxvars*sizeof(swig_globalvar *));
    if (v->vars == NULL) {
      fprintf(stderr,"SWIG : Fatal error in initializing Python module.\n");
      exit(1);
    }
  }
  v->vars[v->nvars] = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  v->vars[v->nvars]->name = (char *) malloc(strlen(name)+1);
  strcpy(v->vars[v->nvars]->name,name);
  v->vars[v->nvars]->get_attr = get_attr;
  v->vars[v->nvars]->set_attr = set_attr;
  v->nvars++;
  v->vars[v->nvars] = 0;
}



