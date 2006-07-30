// There is a bug in the standard t_output_helper, this one will replace it
// until it is fixed.


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
// NOTE: This file has to be %included very early in the SWIGging process as
// it no longer allows existing fragments to be replaced with one of the same
// name.  So to make this work I had to bring a copy of python.swg into this
// project and do the %include there before most other of the standard swiglib
// files are %included.  This may change in 1.3.23, so adjust accordingly then.



%fragment(SWIG_AsVal_frag(long), "header") {
// See my_fragments.i
SWIGSTATICINLINE(int)
SWIG_AsVal(long)(PyObject* obj, long* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyInt_AsLong(obj);
        return 1;
    }
    else {
        PyObject* errmsg = PyString_FromFormat("Expected number, got %s",
                                               obj->ob_type->tp_name);
        PyErr_SetObject(PyExc_TypeError, errmsg);
        Py_DECREF(errmsg);
    }
    return 0;
}
}


%fragment(SWIG_AsVal_frag(unsigned long), "header",
          fragment=SWIG_AsVal_frag(long)) {
// See my_fragments.i
SWIGSTATICINLINE(int)
SWIG_AsVal(unsigned long)(PyObject* obj, unsigned long* val)
{
    long v = 0;
    if (SWIG_AsVal_long(obj, &v) && v < 0) {
        PyErr_SetString(PyExc_TypeError, "negative value received for unsigned type");
        return 0;
    }
    else if (val)
        *val = (unsigned long)v;
    return 1;
}
}


%fragment(SWIG_AsVal_frag(double), "header") {
// See my_fragments.i
SWIGSTATICINLINE(int)
SWIG_AsVal(double)(PyObject *obj, double* val)
{
    if (PyNumber_Check(obj)) {
        if (val) *val = PyFloat_AsDouble(obj);
        return 1;
    }
    else {
        PyObject* errmsg = PyString_FromFormat("Expected number, got %s",
                                               obj->ob_type->tp_name);
        PyErr_SetObject(PyExc_TypeError, errmsg);
        Py_DECREF(errmsg);
    }
    return 0;
}
}
