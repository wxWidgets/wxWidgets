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

#include "internal.h"

/*******************************************************************************
 * $Header$
 *
 * File : symbol.cxx
 *
 * Symbol table management.
 *
 *******************************************************************************/

// -----------------------------------------------------------------------------
// Symbol object
// -----------------------------------------------------------------------------

struct   Symbol {
  ~Symbol() {
    if (name) delete name;
    if (type) delete type;
    if (value) delete value;
  }
  char      *name;
  DataType  *type;        // Optional datatype
  char      *value;       // Optional value (for constant expressions)
};

static Hash    SymHash;   // SWIG Symbol table

// -----------------------------------------------------------------------------
// int add_symbol(char *name, DataType *type, char *value)
// 
// Adds a symbol to the symbol table. Returns -1 if symbol is already in the
// table.
//
// Inputs :
//          name     =  Symbol name
//          type     =  Datatype (for constants).  Optional.
//          value    =  Value string. Optional.
//
// Output : 0 on success, -1 if symbol already exists.
//
// Side Effects : None
// -----------------------------------------------------------------------------

int add_symbol(char *name, DataType *type, char *value) {

  Symbol *s;
  int ret;
  
  s = new Symbol;
  s->name = copy_string(name);
  if (type)
    s->type = new DataType(type);
  else s->type = (DataType *) 0;
  if (value) 
    s->value = copy_string(value);
  else s->value = (char *) 0;

  // Add this to the symbol table

  ret = SymHash.add(s->name, s);
  if (ret == -1) {
    delete s;
  } 
  return ret;
}

// -----------------------------------------------------------------------------
// int lookup_symbol(char *name)
// 
// Checks to see if a symbol is in the symbol table.
//
// Inputs : name   = Symbol name
//
// Output : 0 if not found, 1 if found.
//
// Side Effects : None
// -----------------------------------------------------------------------------

int lookup_symbol(char *name) {
  Symbol *s;

  s = (Symbol *) SymHash.lookup(name);
  if (s) return 1;
  else return 0;
}

// -----------------------------------------------------------------------------
// DataType *lookup_symtype(char *name)
// 
// Returns the datatype of a symbol or NULL if not found.
//
// Inputs : name   = Symbol name
//
// Output : Datatype of symbol, NULL if not found.
//
// Side Effects : None
// -----------------------------------------------------------------------------

DataType *lookup_symtype(char *name) {

  Symbol *s;

  s = (Symbol *) SymHash.lookup(name);
  if (s) return s->type;
  else return (DataType *) 0;
}

// -----------------------------------------------------------------------------
// char *lookup_symvalue(char *name)
// 
// Returns the value associate with a symbol.
//
// Inputs : name = Symbol name
//
// Output : Symbol value (or NULL if not present).
//
// Side Effects : None
// -----------------------------------------------------------------------------

char *lookup_symvalue(char *name) {

  Symbol *s;

  s = (Symbol *) SymHash.lookup(name);
  if (s) return s->value;
  else return (char *) 0;
}

// -----------------------------------------------------------------------------
// int update_symbol(char *name, DataType *type, char *value)
//
// Updates a symbol (or create it) in the hash table.
//
// Inputs : 
//          name   = Name of symbol
//          type   = Datatype of symbol (optional)
//          value  = Symbol value (optional)
//
// Output : 0
//
// Side Effects : None
// -----------------------------------------------------------------------------

int update_symbol(char *name, DataType *type, char *value) {

  Symbol *s;

  s = (Symbol *) SymHash.lookup(name);
  if (s) {
    if (s->type) delete s->type;
    if (s->value) delete s->value;
    if (type) 
      s->type = new DataType(type);
    else
      s->type = (DataType *) 0;
    if (value) 
      s->value = copy_string(value);
    else
      s->value = (char *) 0;
    return 0;
  } else {
    return add_symbol(name, type, value);
  }
}

// -----------------------------------------------------------------------------
// void remove_symbol(char *name)
// 
// Removes a symbol from the symbol table.
//
// Inputs : name = Symbol name.
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void remove_symbol(char *name) {
  SymHash.remove(name);
}
