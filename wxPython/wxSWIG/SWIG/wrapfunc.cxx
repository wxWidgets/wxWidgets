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

// ------------------------------------------------------------------
// wrapfunc.cxx
//
// Created : June 22, 1996
// Dave Beazley
//
// This file defines a class for writing wrappers.  Instead of worrying
// about I/O problems, this wrapper class can be used to write functions
// out of order.
// 
// Defines 3 string objects.
//     def      - Wrapper function definition (function name and arguments)
//     locals   - Local variable definitions
//     code     - The actual wrapper function code
//
//-------------------------------------------------------------------

#include "internal.h"
#include <ctype.h>

// -------------------------------------------------------------------
// Print out a wrapper function.
//
// -------------------------------------------------------------------

void WrapperFunction::print(FILE *f) {
  fprintf(f,"%s\n",def.get());
  fprintf(f,"%s\n",locals.get());
  fprintf(f,"%s\n",code.get());
}

// -------------------------------------------------------------------
// Print out a wrapper function.
//
// -------------------------------------------------------------------

void WrapperFunction::print(String &f) {
  f << def << "\n"
    << locals << "\n"
    << code << "\n";
}

// -------------------------------------------------------------------
// Safely add a local variable.
//
// Maintains a hash table to prevent double adding.
// -------------------------------------------------------------------

void WrapperFunction::add_local(char *type, char *name, char *defarg) {
  char *stored_type;
  char *new_type;
  char temp[256],*c,*t;

  new_type = new char[strlen(type)+1];
  strcpy(new_type,type);

  // Figure out what the name of this variable is

  c = name;
  t = temp;
  while ((isalnum(*c) || (*c == '_') || (*c == '$')) && (*c)) {
    *(t++) = *c;
    c++;
  }
  *t = 0;
  if (h.add(temp,new_type,WrapperFunction::del_type) == -1) {
    // Check to see if a type mismatch has occurred
    stored_type = (char *) h.lookup(temp);
    if (strcmp(type,stored_type) != 0) 
      fprintf(stderr,"Error. Type %s conflicts with previously declared type of %s\n",
	      type, stored_type);
    return;    
  }

  // Successful, write some wrapper code

  if (!defarg)
    locals << tab4 << type << " " << name << ";\n";
  else
    locals << tab4 << type << " " << name << " = " << defarg << ";\n";
}


// -------------------------------------------------------------------
// char *WrapperFunction::new_local(char *type, char *name, char *defarg) {
//
// A safe way to add a new local variable.  type and name are used as
// a starting point, but a new local variable will be created if these
// are already in use.
// -------------------------------------------------------------------

char *WrapperFunction::new_local(char *type, char *name, char *defarg) {
  char *new_type;
  static String new_name;
  char *c;
  new_type = new char[strlen(type)+1];

  strcpy(new_type,type);
  new_name = "";
  c = name;
  for (c = name; ((isalnum(*c) || (*c == '_') || (*c == '$')) && (*c)); c++)
    new_name << *c;

  // Try to add a new local variable
  if (h.add(new_name,new_type,WrapperFunction::del_type) == -1) {
    // Local variable already exists, try to generate a new name
    int i = 0;
    new_name = "";
    // This is a little funky.  We copy characters until we reach a nonvalid
    // identifier symbol, add a number, then append the rest.   This is
    // needed to properly handle arrays.
    c = name;
    for (c = name; ((isalnum(*c) || (*c == '_') || (*c == '$')) && (*c)); c++)
      new_name << *c;
    new_name << i;
    while (h.add(new_name,new_type,WrapperFunction::del_type) == -1) {
      i++;
      c = name;
      new_name = "";
      for (c = name; ((isalnum(*c) || (*c == '_') || (*c == '$')) && (*c)); c++)
	new_name << *c;
      new_name << i;
    }
  }
  new_name << c;
  // Successful, write some wrapper code
  if (!defarg)
    locals << tab4 << type << " " << new_name << ";\n";
  else
    locals << tab4 << type << " " << new_name << " = " << defarg << ";\n";

  // Need to strip off the array symbols now

  c = new_name.get();
  while ((isalnum(*c) || (*c == '_') || (*c == '$')) && (*c))
    c++;
  *c = 0;
  return new_name;
}

// ------------------------------------------------------------------
// static WrapperFunction::del_type(void *obj)
//
// Callback function used when cleaning up the hash table.
// ------------------------------------------------------------------

void WrapperFunction::del_type(void *obj) {
  delete (char *) obj;
}
