/***********************************************************************
 * pyrun.swg for wxPython
 *
 * Include only the function prototypes and such from SWIG's pyrun.swg,
 * but not the runtime functions themselves.  This helps keep the
 * wrapper files clean of unnecessary stuff that is in the libpy.c file
 * anyway.
 *
 ************************************************************************/


#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWIG_PY_INT     1
#define SWIG_PY_FLOAT   2
#define SWIG_PY_STRING  3
#define SWIG_PY_POINTER 4
#define SWIG_PY_BINARY  5

/* Flags for pointer conversion */

#define SWIG_POINTER_EXCEPTION     0x1
#define SWIG_POINTER_DISOWN        0x2

/* Exception handling in wrappers */
#define SWIG_fail   goto fail

/* Constant information structure */
typedef struct swig_const_info {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    swig_type_info **ptype;
} swig_const_info;



/* Common SWIG API */
#define SWIG_ConvertPtr(obj, pp, type, flags) \
  SWIG_Python_ConvertPtr(obj, pp, type, flags)
#define SWIG_NewPointerObj(p, type, flags) \
  SWIG_Python_NewPointerObj(p, type, flags)
#define SWIG_MustGetPtr(p, type, argnum, flags) \
  SWIG_Python_MustGetPtr(p, type, argnum, flags)

/* Python-specific SWIG API */
#define SWIG_newvarlink() \
  SWIG_Python_newvarlink()
#define SWIG_addvarlink(p, name, get_attr, set_attr) \
  SWIG_Python_addvarlink(p, name, get_attr, set_attr)
#define SWIG_ConvertPacked(obj, ptr, sz, ty, flags) \
  SWIG_Python_ConvertPacked(obj, ptr, sz, ty, flags)
#define SWIG_PackData(c, ptr, sz) \
  SWIG_Python_PackData(c, ptr, sz)
#define SWIG_UnpackData(c, ptr, sz) \
  SWIG_Python_UnpackData(c, ptr, sz)
#define SWIG_NewPackedObj(ptr, sz, type) \
  SWIG_Python_NewPackedObj(ptr, sz, type)
#define SWIG_InstallConstants(d, constants) \
  SWIG_Python_InstallConstants(d, constants)


SWIGEXPORT(int)               SWIG_Python_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGEXPORT(PyObject *)        SWIG_Python_NewPointerObj(void *, swig_type_info *,int own);
SWIGEXPORT(void *)	      SWIG_Python_MustGetPtr(PyObject *, swig_type_info *, int, int);

SWIGEXPORT(PyObject *)        SWIG_Python_newvarlink(void);
SWIGEXPORT(void)              SWIG_Python_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGEXPORT(int)               SWIG_Python_ConvertPacked(PyObject *, void *, int sz, swig_type_info *, int);
SWIGEXPORT(char *)            SWIG_Python_PackData(char *c, void *, int);
SWIGEXPORT(char *)            SWIG_Python_UnpackData(char *c, void *, int);
SWIGEXPORT(PyObject *)        SWIG_Python_NewPackedObj(void *, int sz, swig_type_info *);
SWIGEXPORT(void)              SWIG_Python_InstallConstants(PyObject *d, swig_const_info constants[]);


/* Contract support */

#define SWIG_preassert(expr, msg)  if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else
#define SWIG_postassert(expr, msg) if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else

#define SWIG_inherit_preassert(expr, msg)  if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else
#define SWIG_inherit_postassert(expr, msg) if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else

#define SWIG_invariant(expr, msg)       if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else
#define SWIG_invariant_begin(expr, msg) if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else
#define SWIG_invariant_end(expr, msg)   if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, msg #expr ); goto fail; } else

#ifdef __cplusplus
}
#endif

