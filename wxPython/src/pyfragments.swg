

//----------------------------------------------------------------------

// The standard t_output_helper has been changed to return a list rather than
// a tuple, we'll replace it with the old implementation here.  In SWIG 1.3.27
// and earlier it is implemented as a $fragment, so it is only inserted into
// the modules that need it.  For SWIG 1.3.28+ we just need to add a -D on the
// compile command line to turn on the tuple version of the AppendOuput
// function.
#if SWIG_VERSION < 0x010328
%fragment("t_output_helper","header")
%{
    static PyObject* t_output_helper(PyObject* result, PyObject* obj)
    {
        PyObject*   o2;
        PyObject*   o3;
        if (!result) {
            result = obj;
        } else if (result == Py_None) {
            Py_DECREF(result);
            result = obj;
        } else {
            if (!PyTuple_Check(result)) {
                o2 = result;
                result = PyTuple_New(1);
                PyTuple_SET_ITEM(result, 0, o2);
            }
            o3 = PyTuple_New(1);            
            PyTuple_SetItem(o3, 0, obj);      
            o2 = result;
            result = PySequence_Concat(o2, o3); 
            Py_DECREF(o2);                      
            Py_DECREF(o3);
        }
        return result;
    }
%}

#endif




//----------------------------------------------------------------------
// These fragments are inserted in modules that need to convert PyObjects to
// integer values, my versions allow any numeric type to be used, as long as
// it can be converted to a PyInt.  (Specifically, I allow floats where the
// default SWIG_AsVal_long would just raise an exception.
//

#if SWIG_VERSION < 0x010328

%fragment(SWIG_AsVal_frag(long), "header") {
SWIGINTERN int
SWIG_AsVal(long)(PyObject* obj, long* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyInt_AsLong(obj);
        return 1;
    }
    else {
        SWIG_Python_TypeError("number", obj);
    }
    return 0;
}
}


%fragment(SWIG_AsVal_frag(unsigned long), "header",
          fragment=SWIG_AsVal_frag(long)) {
SWIGINTERN int 
SWIG_AsVal(unsigned long)(PyObject* obj, unsigned long* val)
{
    long v = 0;
    if (SWIG_AsVal_long(obj, &v) && v < 0) {
        SWIG_Python_TypeError("unsigned number", obj);
    }
    else if (val)
        *val = (unsigned long)v;
    return 1;
}
}


%fragment(SWIG_AsVal_frag(double), "header") {
SWIGINTERN int
SWIG_AsVal(double)(PyObject *obj, double* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyFloat_AsDouble(obj);
        return 1;
    }
    else {
        SWIG_Python_TypeError("number", obj);
    }
    return 0;
}
}


#else  // SWIG_VERSION >= 1.3.28

%fragment(SWIG_AsVal_frag(long), "header") {
SWIGINTERN int
SWIG_AsVal(long)(PyObject* obj, long* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyInt_AsLong(obj);
        return SWIG_OK;
    }
    return SWIG_TypeError;
}
}


%fragment(SWIG_AsVal_frag(unsigned long), "header",
          fragment=SWIG_AsVal_frag(long)) {
SWIGINTERN int 
SWIG_AsVal(unsigned long)(PyObject* obj, unsigned long* val)
{
    long v = 0;
    if (SWIG_AsVal_long(obj, &v) && v < 0) {
        return SWIG_TypeError;
    }
    else if (val)
        *val = (unsigned long)v;
    return SWIG_OK;
}
}


%fragment(SWIG_AsVal_frag(double), "header") {
SWIGINTERN int
SWIG_AsVal(double)(PyObject *obj, double* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyFloat_AsDouble(obj);
        return SWIG_OK;
    }
    return SWIG_TypeError;
}
}


#endif // SWIG_VERSION
