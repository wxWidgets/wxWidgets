// SWIG Objective-C configuration file
// Dave Beazley
// Copyright (C) 1997

// This file provides support to Objective-C parsing and 
// should be included with just about any Objective-C module

// Base Object class

@interface Object { }

-(char *) name;             // Get object name

@end

typedef Object *id;         // Make 'id' behave like any other "Object"

// Typemaps to make *id work like kind of like a void pointer

%typemap(python,in) id {
   char *temp;
   if (!PyString_Check($source)) {
     PyErr_SetString(PyExc_TypeError,"Expecting an 'id' in argument $argnum of $name");
     return NULL;
   }
   temp = PyString_AsString($source);
   if (SWIG_GetPtr(temp, (void **) &$target, 0)) {
     PyErr_SetString(PyExc_TypeError,"Expecting an 'id' in argument $argnum of $name");
     return NULL;
   }
}

%typemap(tcl,in) id {
  if (SWIG_GetPtr($source,(void **) &$target, 0)) {
    Tcl_SetResult(interp,"Expecting an 'id' in argument $argnum of $name",TCL_STATIC);
    return TCL_ERROR;
  }
}

%typemap(tcl8,in) id {
  if (SWIG_GetPointerObj(interp, $source, (void **) &$target, 0)) {
    Tcl_SetStringObj(result_obj, "Expecting an 'id' in argument $argnum of $name");
  }
}

%typemap(perl5,in) id {
  if (SWIG_GetPtr($source, (void **) &$target, 0)) {
    croak("Expecting an 'id' in argument $argnum of $name");
  }
}


  



