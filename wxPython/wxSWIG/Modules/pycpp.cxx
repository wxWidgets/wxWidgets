/*******************************************************************************
 * Simplified Wrapper and Interface Generator  (SWIG)
 *
 * Author : David Beazley
 *
 * Department of Computer Science
 * University of Chicago
 * 1100 E 58th Street
 * Chicago, IL  60637
 * beazley@cs.uchicago.edu
 *
 * Please read the file LICENSE for the copyright and terms by which SWIG
 * can be used and distributed.
 *******************************************************************************/

/**********************************************************************
 * $Header$
 *
 * pycpp.cxx
 *
 * This module contains code to generate Python shadow classes of C/C++
 * objects.
 **************************************************************************/


#include "swig.h"
#include "python.h"

static  String   *setattr;
static  String   *getattr;
static  String   *pyclass;
static  String   *construct;
static  String   *cinit;
static  String   *additional;
static  int       have_constructor;
static  int       have_destructor;
static  int       have_getattr;
static  int       have_setattr;
static  int       have_repr;
static  char     *class_name;
static  char     *class_type;
static  char     *real_classname;
static  String   *base_class;
static  String   base_getattr;
static  String   base_setattr;
static  int      class_renamed = 0;

// --------------------------------------------------------------------------
// PYTHON::cpp_open_class(char *classname, char *rname, char *ctype, int strip)
//
// Opens a new C++ class or structure.
// --------------------------------------------------------------------------

void PYTHON::cpp_open_class(char *classname, char *rname, char *ctype, int strip) {

  char  temp[256];

  this->Language::cpp_open_class(classname, rname, ctype, strip);

  if (shadow) {
    /* Create new strings for building up a wrapper function */

    setattr   = new String();
    getattr   = new String();
    pyclass   = new String();
    construct = new String();
    cinit     = new String();
    additional= new String();
    base_class = 0;
    base_getattr = "";
    base_setattr = "";


    //  *pyclass << "class " << rname << ":\n";
    *setattr << tab4 << "def __setattr__(self,name,value):\n";
    *getattr << tab4 << "def __getattr__(self,name):\n";
    have_constructor = 0;
    have_destructor = 0;
    have_getattr = 0;
    have_setattr = 0;
    have_repr = 0;
    if (rname) {
      class_name = copy_string(rname);
      class_renamed = 1;
    } else {
      class_name = copy_string(classname);
      class_renamed = 0;
    }
  }

  real_classname = copy_string(classname);
  class_type = copy_string(ctype);

  // Build up the hash table
  hash.add(real_classname,copy_string(class_name));

  sprintf(temp,"%s %s", class_type, real_classname);
  hash.add(temp,copy_string(class_name));

}

// --------------------------------------------------------------------------
// PYTHON::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l)
//
// Creates a C++ member function
// --------------------------------------------------------------------------

void PYTHON::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l) {

  Parm *p;
  int   i;
  char *realname;
  int   oldshadow;
  int   pcount;
  int   numopt;
  int   have_optional;

  String cname = "python:";
  String translate = "";

  // Create the default member function

  oldshadow = shadow;    // Disable shadowing when wrapping member functions
  if (shadow) shadow = shadow | PYSHADOW_MEMBER;
  this->Language::cpp_member_func(name,iname,t,l);
  shadow = oldshadow;
  if (shadow) {
    if (!iname)
      realname = name;
    else
      realname = iname;

    // Check to see if we've already seen this
    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(), 0,0)) {
      return;   // Forget it, already saw it
    }

    if (strcmp(realname,"__repr__") == 0)
      have_repr = 1;

    // Now add it to the class

    *pyclass << tab4 << "def " << realname << "(self, *_args, **_kwargs):\n";
    // Create a doc string
    if (docstring && doc_entry) {
      *pyclass << tab8 << "\"\"\"" << add_docstring(doc_entry) << "\"\"\"\n";
    }
    *pyclass << tab8 << "val = apply(" << module << "." << name_member(realname,class_name) << ",(self,) + _args, _kwargs)\n";

    // Check to see if the return type is an object
    if ((hash.lookup(t->name)) && (t->is_pointer <= 1)) {
      if (!typemap_check("out",typemap_lang,t,name_member(realname,class_name))) {
	if (!have_output) {
	  *pyclass << tab8 << "if val: val = " << (char *) hash.lookup(t->name) << "Ptr(val) ";
	  if (((hash.lookup(t->name)) && (t->is_pointer < 1)) ||
	      ((hash.lookup(t->name)) && (t->is_pointer == 1) && NewObject))
	    *pyclass << "; val.thisown = 1\n";
	  else
	    *pyclass << "\n";
	} else {
	  // Do nothing!
	}
      }
    }
    emitAddPragmas(*pyclass, realname, tab8);
    *pyclass << tab8 << "return val\n";

    // Change the usage string to reflect our shadow class
    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << usage_func(realname,t,l);
    }
  }
}

// -----------------------------------------------------------------------------
// void PYTHON::cpp_constructor(char *name, char *iname, ParmList *l)
//
// Make a constructor for our class
// -----------------------------------------------------------------------------

void PYTHON::cpp_constructor(char *name, char *iname, ParmList *l) {
  char *realname;
  Parm *p;
  int   i;
  int   oldshadow = shadow;
  String cname = "python:constructor:";
  String translate = "";
  int pcount, numopt;
  int have_optional;

  if (shadow) shadow = shadow | PYSHADOW_MEMBER;
  this->Language::cpp_constructor(name,iname,l);
  shadow = oldshadow;

  if (shadow) {
    if (iname)
      realname = iname;
    else {
      if (class_renamed) realname = class_name;
      else realname = class_name;
    }

    // Check to see if we've already seen this
    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(), 0,0)) {
      return;   // Forget it, already seen it
    }

    if (!have_constructor) {

      // Create a new constructor

      *construct << tab4 << "def __init__(self,*_args,**_kwargs):\n";
      if (docstring && doc_entry)
	*construct << tab8 << "\"\"\"" << add_docstring(doc_entry) << "\"\"\"\n";

      *construct << tab8 << "self.this = apply(" << module << "." << name_construct(realname) << ",_args,_kwargs)\n";
      *construct << tab8 << "self.thisown = 1\n";
      emitAddPragmas(*construct,"__init__",tab8);
      have_constructor = 1;
    } else {

      // Hmmm. We seem to be creating a different constructor.  We're just going to create a
      // function for it.

      *additional << "def " << realname << "(*_args,**_kwargs):\n";
      *additional << tab4 << "val = " << class_name << "Ptr(apply("
		  << module << "." << name_construct(realname) << ",_args,_kwargs))\n"
                  << tab4 << "val.thisown = 1\n";
      emitAddPragmas(*additional, realname, tab4);
      *additional << tab4 << "return val\n\n";
    }
    // Patch up the documentation entry
    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << usage_func(class_name,0,l);
    }
  }
}

// ------------------------------------------------------------------------------
// void PYTHON::cpp_destructor(char *name, char *newname)
//
// Creates a destructor for this object
// ------------------------------------------------------------------------------

void PYTHON::cpp_destructor(char *name, char *newname) {
  char *realname;
  int oldshadow = shadow;

  if (shadow) shadow = shadow | PYSHADOW_MEMBER;
  this->Language::cpp_destructor(name,newname);
  shadow = oldshadow;
  if (shadow) {
    if (newname) realname = newname;
    else {
      if (class_renamed) realname = class_name;
      else realname = name;
    }

    char* dfname = name_destroy(realname);

    *pyclass << tab4 << "def __del__(self, " << "delfunc=" << module<< "." << dfname << "):\n";
    emitAddPragmas(*pyclass,"__del__",tab8);
    *pyclass << tab8 << "if self.thisown == 1:\n"
             << tab8 << tab4 << "try:\n"
	     << tab8 << tab8 << "delfunc(self)\n"
             << tab8 << tab4 << "except:\n"
	     << tab8 << tab8 << "pass\n";

    have_destructor = 1;
    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << "del this";
    }
  }
}

// -------------------------------------------------------------------------------
// PYTHON::cpp_close_class()
//
// Closes a Python class and writes out a wrapper
// -------------------------------------------------------------------------------

void PYTHON::cpp_close_class() {
  String    ptrclass;
  String    repr;

  if (shadow) {

    if (!have_constructor) {
      // Build a constructor that takes a pointer to this kind of object
      *construct << tab4 << "def __init__(self,this):\n";
      *construct << tab8 << "self.this = this\n";
    }

    // First, build the pointer base class
    if (base_class) {
      ptrclass << "class " << class_name << "Ptr(" << *base_class << "):\n";
    } else {
    ptrclass << "class " << class_name << "Ptr :\n";
    }

    //    *getattr << tab8 << "return self.__dict__[name]\n";
    *getattr << tab8 << "raise AttributeError,name\n";
    *setattr << tab8 << "self.__dict__[name] = value\n";

    ptrclass << *cinit
	     << tab4 << "def __init__(self,this):\n"
	     << tab8 << "self.this = this\n"
	     << tab8 << "self.thisown = 0\n";

    classes << ptrclass
	    << *pyclass;
    if (have_setattr)
      classes << *setattr;
    if (have_getattr)
      classes << *getattr;

    if (!have_repr) {
      // Supply a repr method for this class
      repr << tab4 << "def __repr__(self):\n"
	   << tab8 << "return \"<C " << class_name <<" instance at %s>\" % (self.this,)\n";

      classes << repr;
      emitAddPragmas(classes,"__class__",tab4);
    }

    // Now build the real class with a normal constructor

    classes << "class " << class_name << "(" << class_name << "Ptr):\n";

    if (docstring && doc_entry) {
      classes << tab4 << "\"\"\"" << add_docstring(doc_entry) << "\"\"\"\n";
    }

    classes << *construct << "\n\n"
	    << "\n" << *additional << "\n";

    delete pyclass;
    delete setattr;
    delete getattr;
    delete additional;
  }
}

void PYTHON::cpp_cleanup() { };

void PYTHON::cpp_inherit(char **baseclass,int) {

  char *bc;
  int   i = 0, first_base = 0;

  if (!shadow) {
    this->Language::cpp_inherit(baseclass);
    return;
  }

  // We'll inherit variables and constants, but not methods

  this->Language::cpp_inherit(baseclass, INHERIT_VAR);

  if (!baseclass) return;
  base_class = new String;

  // Now tell the Python module that we're inheriting from a base class

  while (baseclass[i]) {
    bc = (char *) hash.lookup(baseclass[i]);
    if (bc) {
      if (first_base) *base_class << ",";
      *base_class << bc << "Ptr";
      first_base = 1;
    }
    i++;
  }
  if (!first_base) {
    delete base_class;
    base_class = 0;
  }
}

// --------------------------------------------------------------------------------
// PYTHON::cpp_variable(char *name, char *iname, DataType *t)
//
// Adds an instance member.
// --------------------------------------------------------------------------------

void PYTHON::cpp_variable(char *name, char *iname, DataType *t) {
  char *realname;
  int   inhash = 0;
  int   oldshadow = shadow;
  String cname = "python:";

  if (shadow) shadow = shadow | PYSHADOW_MEMBER;
  this->Language::cpp_variable(name,iname,t);
  shadow = oldshadow;

  if (shadow) {
    have_getattr = 1;
    have_setattr = 1;
    if (!iname)
      realname = name;
    else
      realname = iname;

    // Check to see if we've already seen this

    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(), 0,0)) {
      return;   // Forget it, already seen it
    }

    // Figure out if we've seen this datatype before

    if ((hash.lookup(t->name)) && (t->is_pointer <= 1)) inhash = 1;

    // Now write some code to set the variable
    *setattr << tab8 << "if name == \"" << realname << "\" :\n";
    if (inhash) {
      *setattr << tab8 << tab4 << module << "." << name_set(name_member(realname,class_name)) << "(self,value.this)\n";
    } else {
      *setattr << tab8 << tab4 << module << "." << name_set(name_member(realname,class_name)) << "(self,value)\n";
    }
    *setattr << tab8 << tab4 << "return\n";

    // Write some code to get the variable
    *getattr << tab8 << "if name == \"" << realname << "\" : \n";
    if (inhash) {
      *getattr << tab8 << tab4 << "return " << (char *) hash.lookup(t->name) << "Ptr(" << module << "."
	       << name_get(name_member(realname,class_name)) << "(self))\n";
    } else {
      *getattr << tab8 << tab4 << "return " << module << "." << name_get(name_member(realname,class_name)) << "(self)\n";
    }

    // Patch up ye old documentation entry

    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << "self." << realname;
    }
  }
}

// --------------------------------------------------------------------------------
// PYTHON::cpp_declare_const(char *name, char *iname, DataType *type, char *value)
//
// Add access to a C++ constant
// --------------------------------------------------------------------------------

void PYTHON::cpp_declare_const(char *name, char *iname, DataType *type, char *value) {
  char *realname;
  int   oldshadow = shadow;
  String cname = "python:";

  if (shadow) shadow = shadow | PYSHADOW_MEMBER;
  this->Language::cpp_declare_const(name,iname,type,value);
  shadow = oldshadow;

  if (shadow) {
    if (!iname)
      realname = name;
    else
      realname = iname;

    // Check to see if we've already seen this

    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(), 0,0)) {
      return;   // Forget it, already seen it
    }

    *cinit << tab4 << realname << " = " << module << "." << name_member(realname,class_name) << "\n";

    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << "self." << realname;
      if (value) {
	doc_entry->usage << " = " << value;
      }
    }
  }
}

// --------------------------------------------------------------------------------
// PYTHON::add_typedef(DataType *t, char *name)
//
// This is called whenever a typedef is encountered.   When shadow classes are
// used, this function lets us discovered hidden uses of a class.  For example :
//
//     struct FooBar {
//            ...
//     }
//
// typedef FooBar *FooBarPtr;
//
// --------------------------------------------------------------------------------

void PYTHON::add_typedef(DataType *t, char *name) {

  if (!shadow) return;

  // First check to see if there aren't too many pointers

  if (t->is_pointer > 1) return;

  if (hash.lookup(name)) return;      // Already added


  // Now look up the datatype in our shadow class hash table

  if (hash.lookup(t->name)) {

    // Yep.   This datatype is in the hash

    // Put this types 'new' name into the hash

    hash.add(name,copy_string((char *) hash.lookup(t->name)));
  }
}
