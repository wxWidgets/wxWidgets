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
 * python.cxx
 *
 * Python module.
 **************************************************************************/


#include "swig.h"
#include "python.h"

// Structures for managing doc strings

struct DocString {
  DocEntry    *de;
  char        *name;
  DocString    *next;
};

static   int          doc_index = 0;
static   DocString   *doc_strings = 0;

static char *usage = "\
Python Options (available with -python)\n\
     -docstring      - Produce docstrings (only applies to shadow classes)\n\
     -globals name   - Set name used to access C global variable ('cvar' by default).\n\
     -module name    - Set module name\n\
     -keyword        - Use keyword arguments\n\
     -shadow         - Generate shadow classes. \n\n";

static String pragma_include;

// ---------------------------------------------------------------------
// PYTHON::parse_args(int argc, char *argv[])
//
// ---------------------------------------------------------------------

void PYTHON::parse_args(int argc, char *argv[]) {

  int i = 1;

  sprintf(LibDir,"%s",path);

  docstring = 0;

  // Look for additional command line options.
  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if(strcmp(argv[i],"-module") == 0) {
	    if (argv[i+1]) {
	      module = new char[strlen(argv[i+1])+2];
	      strcpy(module, argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i+=1;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-globals") == 0) {
	    if (argv[i+1]) {
	      global_name = new char[strlen(argv[i+1])+1];
	      strcpy(global_name, argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-shadow") == 0) {
	    shadow = 1;
	    mark_arg(i);
          } else if (strcmp(argv[i],"-docstring") == 0) {
	    docstring = 1;
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-keyword") == 0) {
	    use_kw = 1;
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(usage,stderr);
	  }
      }
  }
  // Create a symbol for this language
  add_symbol("SWIGPYTHON",0,0);

  // Set name of typemaps

  typemap_lang = "python";

}

// ---------------------------------------------------------------------
// PYTHON::parse()
//
// Parse the interface file
// ---------------------------------------------------------------------

void
PYTHON::parse() {

  printf("Generating wrappers for Python\n");
  headers();

  // Run the SWIG parser

  yyparse();
}

// ---------------------------------------------------------------------
// PYTHON::set_module(char *mod_name, char **mod_list)
//
// Sets the module name.
// Does nothing if it's already set (so it can be overridden as a command
// line option).
//
//----------------------------------------------------------------------

void PYTHON::set_module(char *mod_name, char **mod_list) {
  int i;

  // If an "import" method has been set and we're in shadow class mode,
  // output a python command to load the module

  if (import_file) {
    if (!(strcmp(import_file,input_file+strlen(input_file)-strlen(import_file)))) {
      if (shadow) {
	fprintf(f_shadow,"\nfrom %s import *\n", mod_name);
      }
      delete import_file;
      import_file = 0;
    }
  }

  if (module) return;

  module = new char[strlen(mod_name)+1];
  strcpy(module,mod_name);

  // If there was a mod_list specified, make this incredible hack
  if  (mod_list) {
    modinit << "#define SWIGMODINIT ";
    modextern << "#ifdef __cplusplus\n"
	      << "extern \"C\" {\n"
	      << "#endif\n";
    i = 0;
    while(mod_list[i]) {
      modinit << "swig_add_module(\"" << mod_list[i] << "\",init"
	      << mod_list[i] << "); \\\n";

      modextern << "extern void init" << mod_list[i] << "();\n";
      i++;
    }
    modextern << "#ifdef __cplusplus\n"
	      << "}\n"
	      << "#endif\n";
    modinit << "/* End of extern module initialization */\n";

  }
}

// ---------------------------------------------------------------------
// PYTHON::set_init(char *iname)
//
// Sets the initialization function name.
// Does nothing if it's already set
//
//----------------------------------------------------------------------

void PYTHON::set_init(char *iname) {
  set_module(iname,0);
}


// ---------------------------------------------------------------------
// PYTHON::import(char *filename)
//
// Imports a SWIG module as a separate file.
//----------------------------------------------------------------------

void PYTHON::import(char *filename) {
  if (import_file) delete import_file;
  import_file = copy_string(filename);
}

// ----------------------------------------------------------------------
// PYTHON::add_method(char *name, char *function)
//
// Add some symbols to the methods table
// ----------------------------------------------------------------------

void PYTHON::add_method(char *name, char *function) {

  Method *n;

  n = new Method;
  n->name = new char[strlen(name)+1];
  strcpy(n->name,name);
  n->function = new char[strlen(function)+1];
  strcpy(n->function, function);

  n->next = head;
  head = n;
}

// ---------------------------------------------------------------------
// PYTHON::print_methods()
//
// Prints out the method array.
// ---------------------------------------------------------------------

void PYTHON::print_methods() {

  Method *n;

  fprintf(f_wrappers,"static PyMethodDef %sMethods[] = {\n", module);
  n = head;
  while (n) {
    if (!use_kw) {
      fprintf(f_wrappers,"\t { \"%s\", %s, METH_VARARGS },\n", n->name, n->function);
    } else {
      fprintf(f_wrappers,"\t { \"%s\", (PyCFunction) %s, METH_VARARGS | METH_KEYWORDS },\n", n->name, n->function);
    }
    n = n->next;
  }
  fprintf(f_wrappers,"\t { NULL, NULL }\n");
  fprintf(f_wrappers,"};\n");
  fprintf(f_wrappers,"#ifdef __cplusplus\n");
  fprintf(f_wrappers,"}\n");
  fprintf(f_wrappers,"#endif\n");
}

// ---------------------------------------------------------------------
// char *PYTHON::add_docstring(DocEntry *de)
//
// Adds a documentation entry to the doc-string generator.   Returns a
// unique character symbol that will be used to fill in the doc-string
// at a later time.
// ---------------------------------------------------------------------

char *PYTHON::add_docstring(DocEntry *de) {
  DocString *s;
  String str;

  str = "@doc";
  str << doc_index << "@";

  s = new DocString();
  s->de = de;
  s->name = copy_string(str);
  s->next = doc_strings;
  doc_strings = s;
  doc_index++;
  return s->name;
}

// ---------------------------------------------------------------------
// PYTHON::headers(void)
//
// ----------------------------------------------------------------------

void PYTHON::headers(void)
{

  emit_banner(f_header);

  fprintf(f_header,"/* Implementation : PYTHON */\n\n");
  fprintf(f_header,"#define SWIGPYTHON\n");

  if (!NoInclude) {
    if (insert_file("python.swg", f_header) == -1) {
      fprintf(stderr,"SWIG : Fatal error. Unable to locate python.swg. (Possible installation problem).\n");
      SWIG_exit(1);
    }
  } else {
    if (insert_file("pyexp.swg", f_header) == -1) {
      fprintf(stderr,"SWIG : Fatal error. Unable to locate pyexp.swg. (Possible installation problem).\n");
      SWIG_exit(1);
    }
  }
}


// --------------------------------------------------------------------
// PYTHON::initialize(void)
//
// This function outputs the starting code for a function to initialize
// your interface.   It is only called once by the parser.
//
// ---------------------------------------------------------------------

void PYTHON::initialize(void)
{

  char  filen[256];
  char  *temp;
  char  *oldmodule = 0;

  if (!module) {
    module = "swig";
    fprintf(stderr,"SWIG : *** Warning. No module name specified.\n");
  }

  // If shadow classing is enabled, we're going to change the module
  // name to "modulec"

  if (shadow) {
    temp = new char[strlen(module)+2];
    sprintf(temp,"%sc",module);
    oldmodule = module;
    module = temp;
  }
  /* Initialize the C code for the module */
  initialize_cmodule();
  /* Create a shadow file (if enabled).*/
  if (shadow) {
    sprintf(filen,"%s%s.py", output_dir, oldmodule);
    if ((f_shadow = fopen(filen,"w")) == 0) {
      fprintf(stderr,"Unable to open %s\n", filen);
      SWIG_exit(0);
    }
    fprintf(f_shadow,"# This file was created automatically by SWIG.\n");
    fprintf(f_shadow,"import %s\n", module);
  }

  // Dump out external module declarations

  if (strlen(modinit.get()) > 0) {
    fprintf(f_header,"%s\n",modinit.get());
  }
  if (strlen(modextern.get()) > 0) {
    fprintf(f_header,"%s\n",modextern.get());
  }
  fprintf(f_wrappers,"#ifdef __cplusplus\n");
  fprintf(f_wrappers,"extern \"C\" {\n");
  fprintf(f_wrappers,"#endif\n");
}

// ---------------------------------------------------------------------
// PYTHON::initialize_cmodule(void)
//
// Initializes the C module.
//
// ---------------------------------------------------------------------
void PYTHON::initialize_cmodule(void)
{
  int i;
  fprintf(f_header,"#define SWIG_init    init%s\n\n", module);
  fprintf(f_header,"#define SWIG_name    \"%s\"\n", module);

  // Output the start of the init function.
  // Modify this to use the proper return type and arguments used
  // by the target Language

  fprintf(f_init,"static PyObject *SWIG_globals;\n");

  fprintf(f_init,"#ifdef __cplusplus\n");
  fprintf(f_init,"extern \"C\" \n");
  fprintf(f_init,"#endif\n");

  fprintf(f_init,"SWIGEXPORT(void) init%s() {\n",module);
  fprintf(f_init,"\t PyObject *m, *d;\n");

  if (InitNames) {
    i = 0;
    while (InitNames[i]) {
      fprintf(f_init,"\t %s();\n", InitNames[i]);
      i++;
    }
  }
  fprintf(f_init,"\t SWIG_globals = SWIG_newvarlink();\n");
  fprintf(f_init,"\t m = Py_InitModule(\"%s\", %sMethods);\n", module, module);
  fprintf(f_init,"\t d = PyModule_GetDict(m);\n");
}


// ---------------------------------------------------------------------
// PYTHON::close(void)
//
// Called when the end of the interface file is reached.  Closes the
// initialization function and performs cleanup as necessary.
// ---------------------------------------------------------------------

void PYTHON::close(void)
{

  print_methods();
  close_cmodule();
  if ((doc_entry) && (module)){
    String temp;
    temp << "Python Module : ";
    if (shadow) {
      module[strlen(module)-1] = 0;
    }
    temp << module;
    doc_entry->cinfo << temp;
  }
  if (shadow) {
    String  fullshadow;
    fullshadow << classes
               << "\n\n#-------------- FUNCTION WRAPPERS ------------------\n\n"
               << func
               << "\n\n#-------------- VARIABLE WRAPPERS ------------------\n\n"
               << vars;

    if (strlen(pragma_include) > 0) {
      fullshadow << "\n\n#-------------- USER INCLUDE -----------------------\n\n"
                 << pragma_include;
    }

    // Go through all of the docstrings and replace the docstrings

    DocString *s;
    s = doc_strings;
    while (s) {
      fullshadow.replace(s->name, s->de->text);
      s = s->next;
    }
    /*
    fprintf(f_shadow,"\n\n#-------------- FUNCTION WRAPPERS ------------------\n\n");
    fprintf(f_shadow,"%s",func.get());
    fprintf(f_shadow,"\n\n#-------------- VARIABLE WRAPPERS ------------------\n\n");
    fprintf(f_shadow,"%s",vars.get());
    if (strlen(pragma_include) > 0) {
      fprintf(f_shadow,"\n\n#-------------- USER INCLUDE -----------------------\n\n");
      fprintf(f_shadow,"%s",pragma_include.get());
    }
    */
    fprintf(f_shadow, "%s", fullshadow.get());
    fclose(f_shadow);
  }
}

// --------------------------------------------------------------------
// PYTHON::close_cmodule(void)
//
// Called to cleanup the C module code
// --------------------------------------------------------------------
void PYTHON::close_cmodule(void)
{
  emit_ptr_equivalence(f_init);
  fprintf(f_init,"}\n");
}

// ----------------------------------------------------------------------
// PYTHON::get_pointer(char *iname, char *srcname, char *src, char *target,
//                     DataType *t, WrapperFunction &f, char *ret)
//
// Emits code to get a pointer and do type checking.
//      iname = name of the function/method  (used for error messages)
//      srcname = Name of source (used for error message reporting).
//      src   = name of variable where source string is located.
//      dest  = name of variable where pointer value is stored.
//      t     = Expected datatype of the parameter
//      f     = Wrapper function object being used to generate code.
//      ret   = return code upon failure.
//
// Note : pointers are stored as strings so you first need to get
// a string and then call _swig_get_hex() to extract a point.
//
// This module is pretty ugly, but type checking is kind of nasty
// anyways.
// ----------------------------------------------------------------------

void
PYTHON::get_pointer(char *iname, char *srcname, char *src, char *dest,
		    DataType *t, String &f, char *ret)
{

  // Now get the pointer value from the string and save in dest

    if (t->is_reference)
        f << tab4 << "if (" << src << ") {\n"
          << tab8 << "if (SWIG_GetPtrObj(" << src << ",(void **) &" << dest << ",";
    else
        f << tab4 << "if (" << src << ") {\n"
          << tab8 << "if (" << src << " == Py_None) { " << dest << " = NULL; }\n"
          << tab8 << "else if (SWIG_GetPtrObj(" << src << ",(void **) &" << dest << ",";

  // If we're passing a void pointer, we give the pointer conversion a NULL
  // pointer, otherwise pass in the expected type.

  if (t->type == T_VOID) f << "(char *) 0 )) {\n";
  else
    f << "\"" << t->print_mangle() << "\")) {\n";

  // This part handles the type checking according to three different
  // levels.   0 = no checking, 1 = warning message, 2 = strict.

  switch(TypeStrict) {
  case 0: // No type checking
    f << tab8 << "}\n";
    break;

  case 1: // Warning message only

    // Change this part to how you want to handle a type-mismatch warning.
    // By default, it will just print to stderr.

    f << tab8 << tab4 << "fprintf(stderr,\"Warning : type mismatch in " << srcname
	   << " of " << iname << ". Expected " << t->print_mangle()
	   << ", received %s\\n\"," << src << ");\n"
	   << tab8 << "}\n";

    break;
  case 2: // Super strict mode.

    // Change this part to return an error.

    f << tab8 << tab4 << "PyErr_SetString(PyExc_TypeError,\"Type error in " << srcname
	   << " of " << iname << ". Expected " << t->print_mangle() << ".\");\n"
	   << tab8 << "return " << ret << ";\n"
	   << tab8 << "}\n";
    break;

  default :
    fprintf(stderr,"SWIG Error. Unknown strictness level\n");
    break;
  }
  f << tab4 << "}\n";
}

// ----------------------------------------------------------------------
// PYTHON::emit_function_header()
//
// Return the code to be used as a function header
// ----------------------------------------------------------------------
void PYTHON::emit_function_header(WrapperFunction &emit_to, char *wname)
{
  if (!use_kw) {
    emit_to.def << "static PyObject *" << wname
		<< "(PyObject *self, PyObject *args) {";
  } else {
    emit_to.def << "static PyObject *" << wname
                << "(PyObject *self, PyObject *args, PyObject *kwargs) {";
  }
  emit_to.code << tab4 << "self = self;\n";
}

// ----------------------------------------------------------------------
// PYTHON::convert_self()
//
// Called during the function generation process, to determine what to
// use as the "self" variable during the call.  Derived classes may emit code
// to convert the real self pointer into a usable pointer.
//
// Returns the name of the variable to use as the self pointer
// ----------------------------------------------------------------------
char *PYTHON::convert_self(WrapperFunction &)
{
  // Default behaviour is no translation
  return "";
}

// ----------------------------------------------------------------------
// PYTHON::make_funcname_wrapper()
//
// Called to create a name for a wrapper function
// ----------------------------------------------------------------------
char *PYTHON::make_funcname_wrapper(char *fnName)
{
  return name_wrapper(fnName,"");
}

// ----------------------------------------------------------------------
// PYTHON::create_command(char *cname, char *iname)
//
// Create a new command in the interpreter.  Used for C++ inheritance
// stuff.
// ----------------------------------------------------------------------

void PYTHON::create_command(char *cname, char *iname) {

  // Create the name of the wrapper function

  char *wname = name_wrapper(cname,"");

  // Now register the function with the interpreter.

  add_method(iname, wname);

}

// ----------------------------------------------------------------------
// PYTHON::create_function(char *name, char *iname, DataType *d,
//                             ParmList *l)
//
// This function creates a wrapper function and registers it with the
// interpreter.
//
// Inputs :
//     name  = actual name of the function that's being wrapped
//    iname  = name of the function in the interpreter (may be different)
//        d  = Return datatype of the functions.
//        l  = A linked list containing function parameter information.
//
// ----------------------------------------------------------------------

void PYTHON::create_function(char *name, char *iname, DataType *d, ParmList *l)
{
  Parm    *p;
  int     pcount,i,j;
  String  wname, self_name, call_name;
  char    source[64], target[64], temp[256], argnum[20];
  char    *usage = 0;
  WrapperFunction f;
  String   parse_args;
  String   arglist;
  String   get_pointers;
  String   cleanup, outarg;
  String   check;
  String   build;
  String   kwargs;

  int      have_build = 0;
  char     *tm;
  int      numopt = 0;

  have_output = 0;

  // Make a valid name for this function.   This removes special symbols
  // that would cause problems in the C compiler.

  wname = make_funcname_wrapper(iname);

  // Now emit the function declaration for the wrapper function.  You
  // should modify this to return the appropriate types and use the
  // appropriate parameters.

  emit_function_header(f, wname);

  f.add_local("PyObject *","_resultobj");

  // Get the function usage string for later use

  usage = usage_func(iname,d,l);

  // Write code to extract function parameters.
  // This is done in one pass, but we need to construct three independent
  // pieces.
  //      1.    Python format string such as "iis"
  //      2.    The actual arguments to put values into
  //      3.    Pointer conversion code.
  //
  // If there is a type mapping, we will extract the Python argument
  // as a raw PyObject and let the user deal with it.
  //

  pcount = emit_args(d, l, f);
  if (!use_kw) {
    parse_args << tab4 << "if(!PyArg_ParseTuple(args,\"";
  } else {
    parse_args << tab4 << "if(!PyArg_ParseTupleAndKeywords(args,kwargs,\"";
    arglist << ",_kwnames";
  }

  i = 0;
  j = 0;
  numopt = l->numopt();        // Get number of optional arguments
  if (numopt) have_defarg = 1;
  p = l->get_first();

  kwargs << "{ ";
  while (p != 0) {

    // Generate source and target strings
    sprintf(source,"_obj%d",i);
    sprintf(target,"_arg%d",i);
    sprintf(argnum,"%d",j+1);

    // Only consider this argument if it's not ignored

    if (!p->ignore) {
      arglist << ",";
      // Add an optional argument separator if needed

      if (j == pcount-numopt) {
	parse_args << "|";
      }

      if (strlen(p->name)) {
	kwargs << "\"" << p->name << "\",";
      } else {
	kwargs << "\"arg" << j+1 << "\",";
	//	kwargs << "\"\",";
      }

      // Look for input typemap

      if ((tm = typemap_lookup("in","python",p->t,p->name,source,target,&f))) {
	parse_args << "O";        // Grab the argument as a raw PyObject
	f.add_local("PyObject *",source,"0");
	arglist << "&" << source;
	if (i >= (pcount-numopt))
	  get_pointers << tab4 << "if (" << source << ")\n";
	get_pointers << tm << "\n";
	get_pointers.replace("$argnum", argnum);
	get_pointers.replace("$arg",source);
      } else {

	// Check if this parameter is a pointer.  If not, we'll get values

	if (!p->t->is_pointer) {
	  // Extract a parameter by "value"

	  switch(p->t->type) {

	    // Handle integers here.  Usually this can be done as a single
	    // case if you appropriate cast things.   However, if you have
	    // special cases, you'll need to add more code.

	  case T_INT : case T_UINT: case T_SINT:
	    parse_args << "i";
	    break;
	  case T_SHORT: case T_USHORT: case T_SSHORT:
	    parse_args << "h";
	    break;
	  case T_LONG : case T_ULONG: case T_SLONG :
	    parse_args << "l";
	    break;
	  case T_SCHAR : case T_UCHAR :
	    parse_args << "b";
	    break;
	  case T_CHAR:
	    parse_args << "c";
	    break;
	  case T_FLOAT :
	    parse_args << "f";
	    break;
	  case T_DOUBLE:
	    parse_args << "d";
	    break;

	  case T_BOOL:
	    {
	      String tempb;
	      String tempval;
	      if (p->defvalue) {
		tempval << "(int) " << p->defvalue;
	      }
	      tempb << "tempbool" << i;
	      parse_args << "i";
	      if (!p->defvalue)
		f.add_local("int",tempb.get());
	      else
		f.add_local("int",tempb.get(),tempval.get());
	      get_pointers << tab4 << target << " = " << p->t->print_cast() << " " << tempb << ";\n";
	      arglist << "&" << tempb;
	    }
	  break;

	    // Void.. Do nothing.

	  case T_VOID :
	    break;

	    // User defined.   This is usually invalid.   No way to pass a
	    // complex type by "value".  We'll just pass into the unsupported
	    // datatype case.

	  case T_USER:

	    // Unsupported data type

	  default :
	    fprintf(stderr,"%s : Line %d. Unable to use type %s as a function argument.\n",input_file, line_number, p->t->print_type());
	    break;
	  }

	  // Emit code for parameter list

	  if ((p->t->type != T_VOID) && (p->t->type != T_BOOL))
	    arglist << "&_arg" << i;

	} else {

	  // Is some other kind of variable.

	  if ((p->t->type == T_CHAR) && (p->t->is_pointer == 1)) {
	    parse_args << "s";
	    arglist << "&_arg" << i;
	  } else {

	    // Have some sort of pointer variable.  Create a temporary local
	    // variable for the string and read the pointer value into it.

	    parse_args << "O";
	    sprintf(source,"_argo%d", i);
	    sprintf(target,"_arg%d", i);
	    sprintf(temp,"argument %d",i+1);

	    f.add_local("PyObject *", source,"0");
	    arglist << "&" << source;
	    get_pointer(iname, temp, source, target, p->t, get_pointers, "NULL");
	  }
	}
      }
      j++;
    }
    // Check if there was any constraint code
    if ((tm = typemap_lookup("check","python",p->t,p->name,source,target))) {
      check << tm << "\n";
      check.replace("$argnum", argnum);
    }
    // Check if there was any cleanup code
    if ((tm = typemap_lookup("freearg","python",p->t,p->name,target,source))) {
      cleanup << tm << "\n";
      cleanup.replace("$argnum", argnum);
      cleanup.replace("$arg",source);
    }
    if ((tm = typemap_lookup("argout","python",p->t,p->name,target,"_resultobj"))) {
      outarg << tm << "\n";
      outarg.replace("$argnum", argnum);
      outarg.replace("$arg",source);
      have_output++;
    }
    if ((tm = typemap_lookup("build","python",p->t,p->name,source,target))) {
      build << tm << "\n";
      have_build = 1;
    }
    p = l->get_next();
    i++;
  }

  kwargs << " NULL }";
  if (use_kw) {
    f.locals << tab4 << "char *_kwnames[] = " << kwargs << ";\n";
  }

  parse_args << ":" << iname << "\"";     // No additional arguments
  parse_args << arglist << ")) \n"
	     << tab8 << "return NULL;\n";

  self_name = convert_self(f);

  /* Now slap the whole first part of the wrapper function together */

  f.code << parse_args << get_pointers << check;


  // Special handling for build values

  if (have_build) {
    char temp1[256];
    char temp2[256];
    l->sub_parmnames(build);            // Replace all parameter names
    for (i = 0; i < l->nparms; i++) {
      p = l->get(i);
      if (strlen(p->name) > 0) {
	sprintf(temp1,"_in_%s", p->name);
      } else {
	sprintf(temp1,"_in_arg%d", i);
      }
      sprintf(temp2,"_obj%d",i);
      build.replaceid(temp1,temp2);
    }
    f.code << build;
  }

  // This function emits code to call the real function.  Assuming you read
  // the parameters in correctly, this will work.

  call_name = "";
  call_name << self_name << name;
  emit_func_call(call_name,d,l,f);

  // Now emit code to return the functions return value (if any).
  // If there was a result, it was saved in _result.
  // If the function is a void type, don't do anything.

  if ((strncmp(name, "new_", 4) != 0) &&  // don't use the out typemap for constructors
      (tm = typemap_lookup("out","python",d,iname,"_result","_resultobj"))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  } else {

    if ((d->type != T_VOID) || (d->is_pointer)) {
      // Now have return value, figure out what to do with it.

      if (!d->is_pointer) {

	// Function returns a "value"

	switch(d->type) {

	  // Return an integer type

	case T_INT: case T_SINT: case T_UINT: case T_BOOL:
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"i\",_result);\n";
	  break;
	case T_SHORT: case T_SSHORT: case T_USHORT:
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"h\",_result);\n";
	  break;
	case T_LONG : case T_SLONG : case T_ULONG:
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"l\",_result);\n";
	  break;
	case T_SCHAR: case T_UCHAR :
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"b\",_result);\n";
	  break;

	  // Return a floating point value

	case T_DOUBLE :
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"d\",_result);\n";
	  break;
	case T_FLOAT :
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"f\",_result);\n";
	  break;

	  // Return a single ASCII value.  Usually we need to convert
	  // it to a NULL-terminate string and return that instead.

	case T_CHAR :
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"c\",_result);\n";
	  break;

	case T_USER :

	  // Return something by value
	  // We're living dangerously here, but life is short...play hard

	  // Oops.  Need another local variable
	  f.add_local("char","_ptemp[128]");

	  d->is_pointer++;
	  f.code << tab4 << "SWIG_MakePtr(_ptemp, (void *) _result,\""
		 << d->print_mangle() << "\");\n";
	  d->is_pointer--;
	  // Return a character string containing our pointer.

	  f.code << tab4 << "_resultobj = Py_BuildValue(\"s\",_ptemp);\n";
	  break;
	default :
	  fprintf(stderr,"%s: Line %d. Unable to use return type %s in function %s.\n", input_file, line_number, d->print_type(), name);
	  break;
	}
      } else {

	// Return type is a pointer.   We'll see if it's a char * and return
	// a string. Otherwise, we'll convert it into a SWIG pointer and return
	// that.

	if ((d->type == T_CHAR) && (d->is_pointer == 1)) {

	  // Return a character string
	  f.code << tab4 << "_resultobj = Py_BuildValue(\"s\", _result);\n";

	  // If declared as a new object, free the result

	} else {

	  // Build a SWIG pointer.
	  f.add_local("char","_ptemp[128]");
	  f.code << tab4 << "if (_result) {\n"
		 << tab8 << "SWIG_MakePtr(_ptemp, (char *) _result,\""
		 << d->print_mangle() << "\");\n";

	  // Return a character string containing our pointer.
	  f.code << tab8 << "_resultobj = Py_BuildValue(\"s\",_ptemp);\n";
	  f.code << tab4 << "} else {\n"
		 << tab8 << "Py_INCREF(Py_None);\n"
		 << tab8 << "_resultobj = Py_None;\n"
		 << tab4 << "}\n";
	}
      }
    } else {
      // no return value and no output args
      //if (!have_output) {
	f.code << tab4 << "Py_INCREF(Py_None);\n";
	f.code << tab4 << "_resultobj = Py_None;\n";
      //}
    }
  }

  // Check to see if there were any output arguments, if so we're going to
  // create a Python list object out of the current result

  f.code << outarg;

  // If there was any other cleanup needed, do that

  f.code << cleanup;

  // Look to see if there is any newfree cleanup code

  if (NewObject) {
    if ((tm = typemap_lookup("newfree","python",d,iname,"_result",""))) {
      f.code << tm << "\n";
    }
  }

  // See if there is any argument cleanup code

  if ((tm = typemap_lookup("ret","python",d,iname,"_result",""))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  }

  f.code << tab4 << "return _resultobj;\n";
  f.code << "}\n";

  // Substitute the cleanup code
  f.code.replace("$cleanup",cleanup);

  // Substitute the function name
  f.code.replace("$name",iname);

  // Dump the function out
  f.print(f_wrappers);

  // Now register the function with the interpreter.

  add_method(iname, wname);

  // Create a documentation entry for this

  if (doc_entry) {
    static DocEntry *last_doc_entry = 0;
    doc_entry->usage << usage;
    if (last_doc_entry != doc_entry) {
      doc_entry->cinfo << "returns " << d->print_type();
      last_doc_entry = doc_entry;
    }
  }

  // ---------------------------------------------------------------------------
  // Create a shadow for this function (if enabled and not in a member function)
  // ---------------------------------------------------------------------------

  if ((shadow) && (!(shadow & PYSHADOW_MEMBER))) {
    String translate;

    int need_wrapper = 0;
    int munge_return = 0;
    int have_optional = 0;

    // Check return code for modification
    if ((hash.lookup(d->name)) && (d->is_pointer <=1)) {
      need_wrapper = 1;
      munge_return = 1;
    }

    if (docstring && doc_entry)
      need_wrapper = 1;

    // If no modification is needed. We're just going to play some
    // symbol table games instead

    if (!need_wrapper) {
      func << iname << " = " << module << "." << iname << "\n\n";
    } else {
      func << "def " << iname << "(*_args, **_kwargs):\n";

      // Create a docstring for this
      if (docstring && doc_entry) {
	func << tab4 << "\"\"\"" << add_docstring(doc_entry) << "\"\"\"\n";
      }

      func << tab4 << "val = apply(" << module << "." << iname << ",_args,_kwargs)\n";

      if (munge_return) {
	//  If the output of this object has been remapped in any way, we're
	//  going to return it as a bare object.

	if (!typemap_check("out",typemap_lang,d,iname)) {

	  // If there are output arguments, we are going to return the value
          // unchanged.  Otherwise, emit some shadow class conversion code.

	  if (!have_output) {
	    func << tab4 << "if val: val = " << (char *) hash.lookup(d->name) << "Ptr(val)";
	    if (((hash.lookup(d->name)) && (d->is_pointer < 1)) ||
		((hash.lookup(d->name)) && (d->is_pointer == 1) && NewObject))
	      func << "; val.thisown = 1\n";
	    else
	      func << "\n";
	  } else {
	    // Does nothing--returns the value unmolested
	  }
	}
      }
      func << tab4 << "return val\n\n";
    }
  }
}

// -----------------------------------------------------------------------
// PYTHON::link_variable(char *name, char *iname, DataType *d)
//
// Input variables:
//     name = the real name of the variable being linked
//    iname = Name of the variable in the interpreter (may be different)
//        d = Datatype of the variable.
//
// This creates a pair of functions for evaluating/setting the value
// of a variable.   These are then added to the special SWIG global
// variable type.
// -----------------------------------------------------------------------

void PYTHON::link_variable(char *name, char *iname, DataType *t) {

    char   *wname;
    static int have_globals = 0;
    char   *tm;

    WrapperFunction getf, setf;

    // If this is our first call, add the globals variable to the
    // Python dictionary.

    if (!have_globals) {
      fprintf(f_init,"\t PyDict_SetItemString(d,\"%s\", SWIG_globals);\n",global_name);
      have_globals=1;
      if ((shadow) && (!(shadow & PYSHADOW_MEMBER))) {
	vars << global_name << " = " << module << "." << global_name << "\n";
      }
    }
    // First make a sanitized version of the function name (in case it's some
    // funky C++ thing).

    wname = name_wrapper(name,"");

    // ---------------------------------------------------------------------
    // Create a function for setting the value of the variable
    // ---------------------------------------------------------------------

    setf.def << "static int " << wname << "_set(PyObject *val) {";
    if (!(Status & STAT_READONLY)) {
      if ((tm = typemap_lookup("varin","python",t,name,"val",name))) {
	setf.code << tm << "\n";
	setf.code.replace("$name",iname);
      } else {
	if ((t->type != T_VOID) || (t->is_pointer)) {
	  if (!t->is_pointer) {

	    // Have a real value here

	    switch(t->type) {
	    case T_INT: case T_SHORT: case T_LONG :
	    case T_UINT: case T_USHORT: case T_ULONG:
	    case T_SINT: case T_SSHORT: case T_SLONG:
	    case T_SCHAR: case T_UCHAR: case T_BOOL:
	      // Get an integer value
	      setf.add_local(t->print_type(), "tval");
	      setf.code << tab4 << "tval = " << t->print_cast() << "PyInt_AsLong(val);\n"
			<< tab4 << "if (PyErr_Occurred()) {\n"
			<< tab8 << "PyErr_SetString(PyExc_TypeError,\"C variable '"
			<< iname << "'(" << t->print_type() << ")\");\n"
			<< tab8 << "return 1; \n"
			<< tab4 << "}\n"
			<< tab4 << name << " = tval;\n";
	      break;

	    case T_FLOAT: case T_DOUBLE:
	      // Get a floating point value
	      setf.add_local(t->print_type(), "tval");
	      setf.code << tab4 << "tval = " << t->print_cast() << "PyFloat_AsDouble(val);\n"
			<< tab4 << "if (PyErr_Occurred()) {\n"
			<< tab8 << "PyErr_SetString(PyExc_TypeError,\"C variable '"
			<< iname << "'(" << t->print_type() << ")\");\n"
			<< tab8 << "return 1; \n"
			<< tab4 << "}\n"
			<< tab4 << name << " = tval;\n";
	      break;

	      // A single ascii character

	    case T_CHAR:
	      setf.add_local("char *", "tval");
	      setf.code << tab4 << "tval = (char *) PyString_AsString(val);\n"
			<< tab4 << "if (PyErr_Occurred()) {\n"
			<< tab8 << "PyErr_SetString(PyExc_TypeError,\"C variable '"
			<< iname << "'(" << t->print_type() << ")\");\n"
			<< tab8 << "return 1; \n"
			<< tab4 << "}\n"
			<< tab4 << name << " = *tval;\n";
	      break;
	    case T_USER:
	      t->is_pointer++;
	      setf.add_local(t->print_type(),"temp");
	      get_pointer(iname,"value","val","temp",t,setf.code,"1");
	      setf.code << tab4 << name << " = *temp;\n";
	      t->is_pointer--;
	      break;
	    default:
	      fprintf(stderr,"%s : Line %d. Unable to link with type %s.\n", input_file, line_number, t->print_type());
	    }
	  } else {

	    // Parse a pointer value

	    if ((t->type == T_CHAR) && (t->is_pointer == 1)) {
	      setf.add_local("char *", "tval");
	      setf.code << tab4 << "tval = (char *) PyString_AsString(val);\n"
			<< tab4 << "if (PyErr_Occurred()) {\n"
			<< tab8 << "PyErr_SetString(PyExc_TypeError,\"C variable '"
			<< iname << "'(" << t->print_type() << ")\");\n"
			<< tab8 << "return 1; \n"
			<< tab4 << "}\n";

	      if (CPlusPlus) {
		setf.code << tab4 << "if (" << name << ") delete [] " << name << ";\n"
			  << tab4 << name << " = new char[strlen(tval)+1];\n"
			  << tab4 << "strcpy((char *)" << name << ",tval);\n";
	      } else {
		setf.code << tab4 << "if (" << name << ") free(" << name << ");\n"
			  << tab4 << name << " = (char *) malloc(strlen(tval)+1);\n"
			  << tab4 << "strcpy((char *)" << name << ",tval);\n";
	      }
	    } else {

	      // Is a generic pointer value.

	      setf.add_local(t->print_type(),"temp");
	      get_pointer(iname,"value","val","temp",t,setf.code,"1");
	      setf.code << tab4 << name << " = temp;\n";
	    }
	  }
	}
      }
      setf.code << tab4 << "return 0;\n";
    } else {
      // Is a readonly variable.  Issue an error
      setf.code << tab4 << "PyErr_SetString(PyExc_TypeError,\"Variable " << iname
		<< " is read-only.\");\n"
		<< tab4 << "return 1;\n";
    }

    setf.code << "}\n";

    // Dump out function for setting value

    setf.print(f_wrappers);

    // ----------------------------------------------------------------
    // Create a function for getting the value of a variable
    // ----------------------------------------------------------------

    getf.def << "static PyObject *" << wname << "_get() {";
    getf.add_local("PyObject *","pyobj");
    if ((tm = typemap_lookup("varout","python",t,name,name,"pyobj"))) {
      getf.code << tm << "\n";
      getf.code.replace("$name",iname);
    } else if ((tm = typemap_lookup("out","python",t,name,name,"pyobj"))) {
      getf.code << tm << "\n";
      getf.code.replace("$name",iname);
    } else {
      if ((t->type != T_VOID) || (t->is_pointer)) {
	if (!t->is_pointer) {

	  /* Is a normal datatype */
	  switch(t->type) {
	  case T_INT: case T_SINT: case T_UINT:
	  case T_SHORT: case T_SSHORT: case T_USHORT:
	  case T_LONG: case T_SLONG: case T_ULONG:
	  case T_SCHAR: case T_UCHAR: case T_BOOL:
	    getf.code << tab4 << "pyobj = PyInt_FromLong((long) " << name << ");\n";
	    break;
	  case T_FLOAT: case T_DOUBLE:
	    getf.code << tab4 << "pyobj = PyFloat_FromDouble((double) " << name << ");\n";
	    break;
	  case T_CHAR:
	    getf.add_local("char","ptemp[2]");
	    getf.code << tab4 << "ptemp[0] = " << name << ";\n"
		      << tab4 << "ptemp[1] = 0;\n"
		      << tab4 << "pyobj = PyString_FromString(ptemp);\n";
	    break;
	  case T_USER:
	    // Hack this into a pointer
	    getf.add_local("char", "ptemp[128]");
	    t->is_pointer++;
	    getf.code << tab4 << "SWIG_MakePtr(ptemp,(char *) &" << name
		      << "," << quote << t->print_mangle() << quote << ");\n"
		      << tab4 << "pyobj = PyString_FromString(ptemp);\n";
	    t->is_pointer--;
	    break;
	  default:
	    fprintf(stderr,"Unable to link with type %s\n", t->print_type());
	    break;
	  }
	} else {

	  // Is some sort of pointer value
	  if ((t->type == T_CHAR) && (t->is_pointer == 1)) {
	    getf.code << tab4 << "if (" << name << ")\n"
		      << tab8 << "pyobj = PyString_FromString(" << name << ");\n"
		      << tab4 << "else pyobj = PyString_FromString(\"(NULL)\");\n";
	  } else {
	    getf.add_local("char","ptemp[128]");
	    getf.code << tab4 << "SWIG_MakePtr(ptemp, (char *) " << name << ",\""
		      << t->print_mangle() << "\");\n"
		      << tab4 << "pyobj = PyString_FromString(ptemp);\n";
	  }
	}
      }
    }

    getf.code << tab4 << "return pyobj;\n"
	      << "}\n";

    getf.print(f_wrappers);

    // Now add this to the variable linking mechanism

    fprintf(f_init,"\t SWIG_addvarlink(SWIG_globals,\"%s\",%s_get, %s_set);\n", iname, wname, wname);


    // Fill in the documentation entry

    if (doc_entry) {
      doc_entry->usage << usage_var(iname, t);
      doc_entry->cinfo << "Global : " << t->print_type() << " " << name;
    }

    // ----------------------------------------------------------
    // Output a shadow variable.  (If applicable and possible)
    // ----------------------------------------------------------
    if ((shadow) && (!(shadow & PYSHADOW_MEMBER))) {
      if ((hash.lookup(t->name)) && (t->is_pointer <= 1)) {
	vars << iname << " = " << (char *) hash.lookup(t->name) << "Ptr(" << module << "." << global_name
	     << "." << iname << ")\n";
      }
    }
}

// -----------------------------------------------------------------------
// PYTHON::declare_const(char *name, char *iname, DataType *type, char *value)
//
// Makes a constant as defined with #define.  Constants are added to the
// module's dictionary and are **NOT** guaranteed to be read-only,
// sorry.
//
// ------------------------------------------------------------------------

void PYTHON::declare_const(char *name, char *, DataType *type, char *value) {

  char   *tm;

  // Make a static python object

  if ((tm = typemap_lookup("const","python",type,name,value,name))) {
    fprintf(f_init,"%s\n",tm);
  } else {

    if ((type->type == T_USER) && (!type->is_pointer)) {
      fprintf(stderr,"%s : Line %d.  Unsupported constant value.\n", input_file, line_number);
      return;
    }

    if (type->is_pointer == 0) {
      switch(type->type) {
      case T_INT:case T_SINT: case T_UINT: case T_BOOL:
      case T_SHORT: case T_SSHORT: case T_USHORT:
      case T_LONG: case T_SLONG: case T_ULONG:
      case T_SCHAR: case T_UCHAR:
	fprintf(f_init,"\t PyDict_SetItemString(d,\"%s\", PyInt_FromLong((long) %s));\n",name,value);
	break;
      case T_DOUBLE:
      case T_FLOAT:
	fprintf(f_init,"\t PyDict_SetItemString(d,\"%s\", PyFloat_FromDouble((double) %s));\n",name,value);
	break;
      case T_CHAR :
	fprintf(f_init,"\t PyDict_SetItemString(d,\"%s\", PyString_FromString(\"%s\"));\n",name,value);
	break;
      default:
	fprintf(stderr,"%s : Line %d. Unsupported constant value.\n", input_file, line_number);
	break;
      }
    } else {
      if ((type->type == T_CHAR) && (type->is_pointer == 1)) {
	fprintf(f_init,"\t PyDict_SetItemString(d,\"%s\", PyString_FromString(\"%s\"));\n",name,value);
      } else {
	// A funky user-defined type.  We're going to munge it into a string pointer value
	fprintf(f_init,"\t {\n");
	fprintf(f_init,"\t\t char %s_char[%d];\n", name, (int) strlen(type->print_mangle()) + 20);
	fprintf(f_init,"\t\t SWIG_MakePtr(%s_char, (void *) (%s),\"%s\");\n",
		name, value, type->print_mangle());
	fprintf(f_init,"\t\t PyDict_SetItemString(d,\"%s\", PyString_FromString(%s_char));\n",name,name);
	fprintf(f_init,"\t }\n");
      }
    }
  }
  if ((shadow) && (!(shadow & PYSHADOW_MEMBER))) {
    vars << name << " = " << module << "." << name << "\n";
  }
  if (doc_entry) {
    doc_entry->usage = "";
    doc_entry->usage << usage_const(name,type,value);
    doc_entry->cinfo = "";
    doc_entry->cinfo << "Constant: " << type->print_type();
  }
}

// ----------------------------------------------------------------------
// PYTHON::usage_var(char *iname, DataType *t)
//
// This function produces a string indicating how to use a variable.
// It is called by the documentation system to produce syntactically
// correct documentation entires.
//
// s is a pointer to a character pointer.   You should create
// a string and set this pointer to point to it.
// ----------------------------------------------------------------------

char *PYTHON::usage_var(char *iname, DataType *) {

  static String temp;

  temp = "";
  temp << global_name << "." << iname;

  // Create result.  Don't modify this

  return temp.get();
}

// ---------------------------------------------------------------------------
// PYTHON::usage_func(char *iname, DataType *t, ParmList *l)
//
// Produces a string indicating how to call a function in the target
// language.
//
// ---------------------------------------------------------------------------

char *PYTHON::usage_func(char *iname, DataType *, ParmList *l) {

  static String temp;
  Parm  *p;
  int    i;

  temp = "";
  temp << iname << "(";

  // Now go through and print parameters
  // You probably don't need to change this

  i = 0;
  p = l->get_first();
  while (p != 0) {
    if (!p->ignore) {
      i++;
      /* If parameter has been named, use that.   Otherwise, just print a type  */

      if ((p->t->type != T_VOID) || (p->t->is_pointer)) {
	if (strlen(p->name) > 0) {
	  temp << p->name;
	} else {
	  temp << p->t->print_type();
	}
      }
      p = l->get_next();
      if (p != 0) {
	if (!p->ignore)
	  temp << ",";
      }
    } else {
      p = l->get_next();
      if (p) {
	if ((!p->ignore) && (i > 0))
	  temp << ",";
      }
    }
  }

  temp << ")";

  // Create result. Don't change this

  return temp.get();

}


// ----------------------------------------------------------------------
// PYTHON::usage_const(char *iname, DataType *type, char *value)
//
// Produces a string for a constant.   Really about the same as
// usage_var() except we'll indicate the value of the constant.
// ----------------------------------------------------------------------

char *PYTHON::usage_const(char *iname, DataType *, char *value) {

  static String temp;
  temp = "";
  temp << iname << " = " << value;

  return temp.get();
}

// -----------------------------------------------------------------------
// PYTHON::add_native(char *name, char *funcname)
//
// Add a native module name to the methods list.
// -----------------------------------------------------------------------

void PYTHON::add_native(char *name, char *funcname) {
  add_method(name, funcname);
  if (shadow) {
    func << name << " = " << module << "." << name << "\n\n";
  }
}

// -----------------------------------------------------------------------
// PYTHON::cpp_class_decl(char *name, char *rename, char *type)
//
// Treatment of an empty class definition.    Used to handle
// shadow classes across modules.
// -----------------------------------------------------------------------

void PYTHON::cpp_class_decl(char *name, char *rename, char *type) {
    char temp[256];
    if (shadow) {
	hash.add(name,copy_string(rename));
	// Add full name of datatype to the hash table
	if (strlen(type) > 0) {
	  sprintf(temp,"%s %s", type, name);
	  hash.add(temp,copy_string(rename));
	}
    }
}

// -----------------------------------------------------------------------
// PYTHON::pragma(char *name, char *type)
//
// Pragma directive. Used to do various python specific things
// -----------------------------------------------------------------------

void PYTHON::pragma(char *lang, char *cmd, char *value) {

    if (strcmp(lang,"python") == 0) {
	if (strcmp(cmd,"CODE") == 0) {
	  if (shadow) {
	    fprintf(f_shadow,"%s\n",value);
	  }
	} else if (strcmp(cmd,"code") == 0) {
	  if (shadow) {
	    fprintf(f_shadow,"%s\n",value);
	  }
	} else if (strcmp(cmd,"include") == 0) {
	  if (shadow) {
	    if (value) {
	      if (get_file(value,pragma_include) == -1) {
		fprintf(stderr,"%s : Line %d. Unable to locate file %s\n", input_file, line_number, value);
	      }
	    }
	  }
	} else {
	  fprintf(stderr,"%s : Line %d. Unrecognized pragma.\n", input_file, line_number);
	}
    }
}


struct PyPragma {
  PyPragma(char *method, char *text) : m_method(method), m_text(text), next(0) { }
  ~PyPragma() { if (next) delete next; }
  String  m_method;
  String  m_text;
  PyPragma  *next;
};

static PyPragma *pragmas = 0;

// -----------------------------------------------------------------------------
// PYTHON::cpp_pragma(Pragma *plist)
//
// Handle C++ pragmas
// -----------------------------------------------------------------------------

void PYTHON::cpp_pragma(Pragma *plist) {
  PyPragma *pyp1 = 0, *pyp2 = 0;
  if (pragmas) {
    delete pragmas;
    pragmas = 0;
  }
  while (plist) {
    if (strcmp(plist->lang,"python") == 0) {
      if (strcmp(plist->name,"addtomethod") == 0) {
            // parse value, expected to be in the form "methodName:line"
	String temp = plist->value;
	char* txtptr = strchr(temp.get(), ':');
	if (txtptr) {
	  // add name and line to a list in current_class
	  *txtptr = 0;
	  txtptr++;
	  pyp1 = new PyPragma(temp,txtptr);
	  if (pyp2) {
	      pyp2->next = pyp1;
	      pyp2 = pyp1;
	  } else {
	    pragmas = pyp1;
	    pyp2 = pragmas;
	  }
	} else {
	  fprintf(stderr,"%s : Line %d. Malformed addtomethod pragma.  Should be \"methodName:text\"\n",
		  plist->filename.get(),plist->lineno);
	}
      } else if (strcmp(plist->name, "addtoclass") == 0) {
	pyp1 = new PyPragma("__class__",plist->value);
	if (pyp2) {
	  pyp2->next = pyp1;
	  pyp2 = pyp1;
	} else {
	  pragmas = pyp1;
	  pyp2 = pragmas;
	}
      }
    }
    plist = plist->next;
  }
}

// --------------------------------------------------------------------------------
// PYTHON::emitAddPragmas(String& output, char* name, char* spacing);
//
// Search the current class pragma for any text belonging to name.
// Append the text properly spaced to the output string.
// --------------------------------------------------------------------------------

void PYTHON::emitAddPragmas(String& output, char* name, char* spacing)
{
  PyPragma *p = pragmas;
  while (p) {
    if (strcmp(p->m_method,name) == 0) {
      output << spacing << p->m_text << "\n";
    }
    p = p->next;
  }
}
