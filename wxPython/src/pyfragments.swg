

// There standard t_output_helper has been changed to return a list rather
// than a tuple, we'll replace it with the old implementation here.


%fragment("t_output_helper","header") %{
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



// These fragments are inserted in modules that need to convert PyObjects to
// integer values, my versions allow any numeric type to be used, as long as
// it can be converted to a PyInt.  (Specifically, I allow floats where the
// default SWIG_AsVal_long would just raise an exception.
//


%fragment(SWIG_AsVal_frag(long), "header") {
SWIGINTERN int
SWIG_AsVal(long)(PyObject* obj, long* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyInt_AsLong(obj);
        return 1;
    }
    else {
        SWIG_type_error("number", obj);
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
        SWIG_type_error("unsigned number", obj);
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
        SWIG_type_error("number", obj);
    }
    return 0;
}
}
