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



SWIGIMPORT(PyObject *)        SWIG_newvarlink(void);
SWIGIMPORT(void)              SWIG_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGIMPORT(int)               SWIG_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGIMPORT(int)               SWIG_ConvertPacked(PyObject *, void *, int sz, swig_type_info *, int);
SWIGIMPORT(char *)            SWIG_PackData(char *c, void *, int);
SWIGIMPORT(char *)            SWIG_UnpackData(char *c, void *, int);
SWIGIMPORT(PyObject *)        SWIG_NewPointerObj(void *, swig_type_info *,int own);
SWIGIMPORT(PyObject *)        SWIG_NewPackedObj(void *, int sz, swig_type_info *);
SWIGIMPORT(void)              SWIG_InstallConstants(PyObject *d, swig_const_info constants[]);


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

