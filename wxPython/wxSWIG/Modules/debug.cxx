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
/**************************************************************************
 * $Header$
 *
 * debug.cxx
 *
 * This is a dummy language module that is used only for testing the SWIG
 * parser.
 *
 * It creates a wrapper file, but only containing descriptions of what
 * was wrapped.
 *
 ***************************************************************************/

#include "swig.h"
#include "debug.h"

void DEBUGLANG::parse_args(int, char **) {
  sprintf(LibDir,"%s",path);
  typemap_lang = "debug";
}

void DEBUGLANG::parse() { 
  headers();
  yyparse();
}

void DEBUGLANG::set_module(char *mod_name, char **) { 
  if (module) return;
  module = new char[strlen(mod_name)+1];
  strcpy(module,mod_name);
}

void DEBUGLANG::set_init(char *init_name) { 
  set_module(init_name,0);
}

void DEBUGLANG::headers(void) { 
  fprintf(f_header,"/* DEBUG : Language specific headers go here */\n\n");
  fprintf(f_header,"/* DEBUG : Pointer conversion function here */\n\n");
  fprintf(f_header,"/* DEBUG : Language specific code here */\n\n");
}

void DEBUGLANG::initialize(void) {
  
  fprintf(f_header,"#define   SWIG_init     %s_init\n\n", module);
  fprintf(f_header,"#define   SWIG_name    \"%s\"\n", module);
  
  fprintf(f_init,"\n/* MODULE INITIALIZATION */\n\n");
  fprintf(f_init,"void %s_init() {\n", module);

}

void DEBUGLANG::close(void) { 
  fprintf(f_init,"}  /* END INIT */\n");
  
  fprintf(f_wrappers,"SWIG POINTER-MAPPING TABLE\n\n");
  emit_ptr_equivalence(f_init);
}

void DEBUGLANG::create_function(char *name, char *iname, DataType *d, ParmList *l) {
  
  fprintf(f_wrappers,"WRAPPER : ");
  emit_extern_func(name,d,l,0,f_wrappers);
  fprintf(f_wrappers,"\n");

  fprintf(f_init,"     ADD COMMAND    : %s --> ", iname);
  emit_extern_func(name,d,l,0,f_init);

}
 
void DEBUGLANG::link_variable(char *name, char *iname, DataType *t)  { 
  fprintf(f_wrappers,"WRAPPER : ");
  emit_extern_var(name,t,0,f_wrappers);
  
  fprintf(f_init,"     ADD VARIABLE   : %s --> ", iname);
  emit_extern_var(name,t,0,f_init);

}

void DEBUGLANG::declare_const(char *name, char *, DataType *type, char *value) { 
  if (!value) value = "[None]";
  fprintf(f_init,"     ADD CONSTANT   : %s %s = %s\n", type->print_cast(),name,value);
}

void DEBUGLANG::add_native(char *name, char *funcname) { 
  fprintf(f_init,"     ADD NATIVE     : %s --> %s\n", name, funcname);
}

void DEBUGLANG::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l) {
  fprintf(f_wrappers,"        MEMBER FUNC   : ");
  emit_extern_func(name,t,l,0,f_wrappers);
  fprintf(f_wrappers,"\n");
  if (!iname) iname = name;
  fprintf(f_init,"     ADD MEMBER FUN : %s --> ", iname);
  emit_extern_func(name,t,l,0,f_init);
}

void DEBUGLANG::cpp_constructor(char *name, char *iname, ParmList *l) { 
  DataType *t;
  fprintf(f_wrappers,"        CONSTRUCTOR   : ");
  t = new DataType(T_USER);
  sprintf(t->name,"%s",name);
  t->is_pointer=1;
  emit_extern_func(name,t,l,0,f_wrappers);
  if (!iname) iname = name;
  fprintf(f_init,"     ADD CONSTRUCT  : %s --> ", iname);
  emit_extern_func(name,t,l,0,f_init);
}

void DEBUGLANG::cpp_destructor(char *name, char *iname) {
  fprintf(f_wrappers,"        DESTRUCTOR    : ~%s();\n", name);
  if (!iname) iname = name;
  fprintf(f_init,"     ADD DESTRUCT  : %s --> ~%s();\n",iname,name);
}

void DEBUGLANG::cpp_open_class(char *name, char *iname, char *ctype, int strip) { 
  this->Language::cpp_open_class(name, iname, ctype,strip);
  fprintf(f_wrappers,"C++ CLASS START : %s %s  ========================================\n\n",ctype,name);
  fprintf(f_init,"\n     // C++ CLASS START : %s %s\n",ctype,name);
}

void DEBUGLANG::cpp_close_class() { 
  fprintf(f_wrappers,"C++ CLASS END ===================================================\n\n");
  fprintf(f_init,"     // C++ CLASS END \n\n");
}

void DEBUGLANG::cpp_inherit(char **baseclass, int) { 
  int i = 0;
  if (baseclass) {
    fprintf(f_wrappers,"inheriting from baseclass :");
    while (baseclass[i]) {
      fprintf(f_wrappers," %s",baseclass[i]);
      i++;
    }
    fprintf(f_wrappers,"\n");
  }
  this->Language::cpp_inherit(baseclass);
}

void DEBUGLANG::cpp_variable(char *name, char *iname, DataType *t) { 
  fprintf(f_wrappers,"        ATTRIBUTE     : ");
  emit_extern_var(name,t,0,f_wrappers);
  if (!iname) iname = name;
  fprintf(f_init,"     ADD MEMBER     : %s --> ", iname);
  emit_extern_var(name,t,0,f_init);
}
void DEBUGLANG::cpp_static_func(char *name, char *iname, DataType *t, ParmList *l) {

  fprintf(f_wrappers,"        STATIC FUNC   : ");
  emit_extern_func(name,t,l,0,f_wrappers);
  fprintf(f_init,"     ADD STATIC FUNC: %s --> ", iname);
  emit_extern_func(name,t,l,0,f_init);

}

void DEBUGLANG::cpp_declare_const(char *name, char *iname, DataType *t, char *value) { 
  if (!value) value = "[None]";
  fprintf(f_wrappers,"        C++ CONST     : %s %s = %s\n", t->print_cast(), name, value);
  if (!iname) iname = name;
  fprintf(f_init,"     ADD C++ CONST  : %s --> %s = %s\n", iname, t->print_cast(), value);
}

void DEBUGLANG::cpp_static_var(char *name, char *iname, DataType *t) { 
  fprintf(f_wrappers,"        C++ STATIC VAR: ");
  emit_extern_var(name,t,0,f_wrappers);
  if (!iname) iname = name;
  fprintf(f_init,"     ADD STATIC VAR : %s --> ",iname);
  emit_extern_var(name,t,0,f_init);
}

void DEBUGLANG::pragma(char *lname, char *name, char *value) { 
  fprintf(f_wrappers,"PRAGMA : LANG = %s, NAME = %s ", lname, name);
  if (value) {
    fprintf(f_wrappers,", VALUE = %s\n", value);
  } else {
    fprintf(f_wrappers,"\n");
  }
}

void DEBUGLANG::cpp_class_decl(char *name, char *, char *type) { 
  fprintf(f_wrappers,"C++ CLASS DECLARATION : %s %s\n", type,name);
}


