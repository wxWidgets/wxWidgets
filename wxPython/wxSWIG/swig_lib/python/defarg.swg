/* This file defines an internal function for processing default arguments
   with shadow classes.

   There seems to be no straightforward way to write a shadow functions
   involving default arguments. For example :

             def foo(arg1,arg2,*args):
                     shadowc.foo(arg1,arg2,args)

   This fails because args is now a tuple and SWIG doesn't know what to
   do with it.

   This file allows a different approach :

            def foo(arg1,arg2,*args):
                    shadowc.__call_defarg(shadowc.foo,(arg1,arg2,)+args)

   Basically, we form a new tuple from the object, call this special
   __call_defarg method and it passes control to the real wrapper function.
   An ugly hack, but it works.
*/

static PyObject *swig_call_defargs(PyObject *self, PyObject *args) {
	PyObject *func;
	PyObject *parms;

	if (!PyArg_ParseTuple(args,"OO",&func,&parms))
	  return NULL;

	if (!PyCallable_Check(func)) {
             PyErr_SetString(PyExc_TypeError, "__call_defarg : Need a callable object!");
	     return NULL;
	}
	return PyEval_CallObject(func,parms);

}
