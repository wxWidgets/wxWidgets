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

#include <limits.h>
#include <float.h>

#ifdef __cplusplus
#define SWIG_STATIC_INLINE static inline 
#else
#define SWIG_STATIC_INLINE static 
#endif 

SWIG_STATIC_INLINE long
SPyObj_AsLong(PyObject * obj)
{
  return PyInt_Check(obj) ?  PyInt_AsLong(obj) : PyLong_AsLong(obj);
}

SWIG_STATIC_INLINE unsigned long
SPyObj_AsUnsignedLong(PyObject * obj) 
{
  if (PyLong_Check(obj)) {
    return PyLong_AsUnsignedLong(obj);
  } else {
    long i = PyInt_AsLong(obj);
    if ( !PyErr_Occurred() && (i < 0)) {
      PyErr_SetString(PyExc_TypeError, "negative value for unsigned type");
    }
    return i;
  }
}

#if !defined(_MSC_VER)
SWIG_STATIC_INLINE PyObject* 
SPyObj_FromLongLong(long long value)
{
  return (value > (long)(LONG_MAX)) ?
    PyLong_FromLongLong(value) : PyInt_FromLong((long)value); 
}
#endif

SWIG_STATIC_INLINE PyObject* 
SPyObj_FromUnsignedLong(unsigned long value)
{
  return (value > (unsigned long)(LONG_MAX)) ?
    PyLong_FromUnsignedLong(value) : PyInt_FromLong((long)value); 
}

#if !defined(_MSC_VER)
SWIG_STATIC_INLINE PyObject* 
SPyObj_FromUnsignedLongLong(unsigned long long value)
{
  return (value > (unsigned long long)(LONG_MAX)) ?
    PyLong_FromUnsignedLongLong(value) : PyInt_FromLong((long)value); 
}
#endif

SWIG_STATIC_INLINE long
SPyObj_AsLongInRange(PyObject * obj, long min_value, long max_value)
{
  long value = SPyObj_AsLong(obj);
  if (!PyErr_Occurred()) {
    if (value < min_value) {
      PyErr_SetString(PyExc_OverflowError,"value is smaller than type minimum");
    } else if (value > max_value) {
      PyErr_SetString(PyExc_OverflowError,"value is greater than type maximum");
    }
  }
  return value;
}

SWIG_STATIC_INLINE unsigned long
SPyObj_AsUnsignedLongInRange(PyObject *obj, unsigned long max_value) 
{
  unsigned long value = SPyObj_AsUnsignedLong(obj);
  if (!PyErr_Occurred()) {
    if (value > max_value) {     
      PyErr_SetString(PyExc_OverflowError,"value is greater than type maximum");
    }
  }
  return value;
}

SWIG_STATIC_INLINE signed char
SPyObj_AsSignedChar(PyObject *obj) { 
  return (signed char)SPyObj_AsLongInRange(obj, SCHAR_MIN, SCHAR_MAX);
}

SWIG_STATIC_INLINE short
SPyObj_AsShort(PyObject *obj) { 
  return (short)SPyObj_AsLongInRange(obj, SHRT_MIN, SHRT_MAX);
}

SWIG_STATIC_INLINE int
SPyObj_AsInt(PyObject *obj) { 
  return SPyObj_AsLongInRange(obj, INT_MIN, INT_MAX);
}

SWIG_STATIC_INLINE unsigned char
SPyObj_AsUnsignedChar(PyObject *obj) { 
  return (unsigned char)SPyObj_AsUnsignedLongInRange(obj, UCHAR_MAX);
}

SWIG_STATIC_INLINE unsigned short 
SPyObj_AsUnsignedShort(PyObject *obj) { 
  return (unsigned short)SPyObj_AsUnsignedLongInRange(obj, USHRT_MAX);
}

SWIG_STATIC_INLINE unsigned int
SPyObj_AsUnsignedInt(PyObject *obj) { 
  return SPyObj_AsUnsignedLongInRange(obj, UINT_MAX);
}

#if !defined(_MSC_VER)
SWIG_STATIC_INLINE long long
SPyObj_AsLongLong(PyObject *obj) {
  return PyInt_Check(obj) ?
    PyInt_AsLong(obj) : PyLong_AsLongLong(obj);
}

SWIG_STATIC_INLINE unsigned long long
SPyObj_AsUnsignedLongLong(PyObject *obj) {
  return PyLong_Check(obj) ?
    PyLong_AsUnsignedLongLong(obj) : SPyObj_AsUnsignedLong(obj);
}
#endif

SWIG_STATIC_INLINE double
SPyObj_AsDouble(PyObject *obj) {
  return (PyFloat_Check(obj)) ? PyFloat_AsDouble(obj) :
    (double)((PyInt_Check(obj)) ? PyInt_AsLong(obj) : PyLong_AsLongLong(obj));
}

SWIG_STATIC_INLINE float
SPyObj_AsFloat(PyObject *obj) {
  double value = SPyObj_AsDouble(obj);
  if (!PyErr_Occurred()) {
    if (value < FLT_MIN) {
      PyErr_SetString(PyExc_OverflowError,"float is smaller than flt_min");
    } else if (value > FLT_MAX) {     
      PyErr_SetString(PyExc_OverflowError,"float is greater than flt_max");
    }
  }
  return (float) value;
}

SWIG_STATIC_INLINE char
SPyObj_AsChar(PyObject *obj) { 
  char c = (PyString_Check(obj) && PyString_Size(obj) == 1) ?
    PyString_AsString(obj)[0] 
    : (char) SPyObj_AsLongInRange(obj, CHAR_MIN, CHAR_MAX);
  if (PyErr_Occurred()) {
    PyErr_Clear();
    PyErr_SetString(PyExc_TypeError, "a char is required");
  }
  return c;
}

SWIG_STATIC_INLINE PyObject *
SPyObj_FromChar(char c) { 
  return PyString_FromStringAndSize(&c,1);
}

SWIG_STATIC_INLINE PyObject *
SPyObj_FromCharPtr(const char* cptr) { 
  return cptr ? PyString_FromString(cptr) : Py_BuildValue((char*)"");
}
 
SWIG_STATIC_INLINE int
SPyObj_AsBool(PyObject *obj) {
    return SPyObj_AsLong/*Long*/(obj) ?  1 : 0;
}



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
#define SWIG_NewPackedObj(ptr, sz, type) \
  SWIG_Python_NewPackedObj(ptr, sz, type)
#define SWIG_InstallConstants(d, constants) \
  SWIG_Python_InstallConstants(d, constants)


SWIGIMPORT(int)               SWIG_Python_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGIMPORT(PyObject *)        SWIG_Python_NewPointerObj(void *, swig_type_info *,int own);
SWIGIMPORT(void *)            SWIG_Python_MustGetPtr(PyObject *, swig_type_info *, int, int);
SWIGIMPORT(PyObject *)        SWIG_Python_newvarlink(void);
SWIGIMPORT(void)              SWIG_Python_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGIMPORT(int)               SWIG_Python_ConvertPacked(PyObject *, void *, int sz, swig_type_info *, int);
SWIGIMPORT(PyObject *)        SWIG_Python_NewPackedObj(void *, int sz, swig_type_info *);
SWIGIMPORT(void)              SWIG_Python_InstallConstants(PyObject *d, swig_const_info constants[]);



/* Contract support */

#define SWIG_contract_assert(expr, msg) if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, (char *) msg ); goto fail; } else

#ifdef __cplusplus
}
#endif

