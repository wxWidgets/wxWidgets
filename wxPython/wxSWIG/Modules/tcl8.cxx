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

/***********************************************************************
 * $Header$
 *
 * tcl8.cxx
 *
 * Module for creating Tcl 8.0 native wrapper functions.  Older SWIG
 * modules will work with Tcl 8.0, but this one provides a significant
 * boost in performance.
 ***********************************************************************/

#include "swig.h"
#include "tcl8.h"
#include <ctype.h>

static char *Tcl_config="swigtcl.swg";
static char *usage = "\
Tcl 8.0 Options (available with -tcl)\n\
     -module name    - Set name of module\n\
     -prefix name    - Set a prefix to be appended to all names\n\
     -namespace      - Build module into a Tcl 8 namespace. \n\
     -noobject       - Omit code for object oriented interface.\n\
     -old            - Use old SWIG interface (same as -noobject).\n\n";

static char *ns_name = 0;

static String mod_init;
static String mod_extern;

// ---------------------------------------------------------------------
// TCL8::parse_args(int argc, char *argv[])
//
// Parse tcl specific command line options
// ---------------------------------------------------------------------

void TCL8::parse_args(int argc, char *argv[]) {
  
  int i = 1;
  sprintf(LibDir,"%s",tcl_path);

  // Look for certain command line options

  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if (strcmp(argv[i],"-prefix") == 0) {
	    if (argv[i+1]) {
	      prefix = new char[strlen(argv[i+1])+2];
	      strcpy(prefix, argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-module") == 0) {
	    if (argv[i+1]) {
	      set_module(argv[i+1],0);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-namespace") == 0) {
	    nspace = 1;
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-old") == 0) {
	    shadow = 0;
	    mark_arg(i);
          } else if (strcmp(argv[i],"-noobject") == 0) {
	    shadow = 0;
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(usage,stderr);
	  }
      }
  }

  // If a package has been specified, make sure it ends with a '_'

  if (prefix) {
    ns_name = copy_string(prefix);
    if (prefix[strlen(prefix)] != '_') {
      prefix[strlen(prefix)+1] = 0;
      prefix[strlen(prefix)] = '_';
    }
  } else 
    prefix = "";

  // Create a symbol SWIGTCL

  add_symbol("SWIGTCL",0,0);
  add_symbol("SWIGTCL8",0,0);

  // Set name of typemaps

  typemap_lang = "tcl8";

  // Attempt to load up the C++ configuration files

  get_file("delcmd8.swg",delcmd);
  get_file("methodcmd8.swg",methodcmd);
  get_file("objcmd8.swg",objcmd);

}

// ---------------------------------------------------------------------
// void TCL8::parse()
//
// Start parsing an interface file for Tcl.
// ---------------------------------------------------------------------

void TCL8::parse() {

  fprintf(stderr,"Making wrappers for Tcl 8.x\n");

  // Print out TCL specific headers
  
  headers();
  
  // Run the parser
  
  yyparse();

}

// ---------------------------------------------------------------------
// TCL8::set_module(char *mod_name,char **mod_list)
//
// Sets the module name.
// Does nothing if it's already set (so it can be overridden as a command
// line option).
//
// mod_list is a NULL-terminated list of additional modules.  This
// is really only useful when building static executables and other
// things.
//----------------------------------------------------------------------

void TCL8::set_module(char *mod_name, char **mod_list) {

  char temp[256], *c;
  int  i;

  if (module) return;
  
  module = new char[strlen(mod_name)+1];
  strcpy(module,mod_name);

  // Fix capitalization for Tcl 

  c = module;
  while (*c) {
    *c = (char) tolower(*c);
    c++;
  }

  // Now create an initialization function

  sprintf(temp,"%s_Init", module);
  init_name = new char[strlen(temp) + 1];
  strcpy(init_name, temp);
  *init_name = toupper(*init_name);

  if (!ns_name) ns_name = copy_string(module);

  // If namespaces have been specified, set the prefix to the module name

  if ((nspace) && (strlen(prefix) < 1)) {
    prefix = new char[strlen(module)+2];
    strcpy(prefix,module);
    prefix[strlen(module)] = '_';
    prefix[strlen(module)+1] = 0;
  }

  // If additional modules have been specified, create some code for
  // initializing them.

  if (mod_list) {
    i = 0;
    while (mod_list[i]) {
      c = mod_list[i];
      while (*c) {
	*c = (char) tolower(*c);
	c++;
      }
      sprintf(temp,"%s_Init",mod_list[i]);
      temp[0] = toupper(temp[0]);
      
      // Dump out some initialization code
      
      mod_init << tab4 << "if (" << temp << "(" << interp_name << ") == TCL_ERROR) {\n"
	       << tab8 << "return TCL_ERROR;\n"
	       << tab4 << "}\n\n";
      mod_extern << "extern int " << temp << "(Tcl_Interp *);\n";
      i++;
    }
  }
}


// ---------------------------------------------------------------------
// TCL8::set_init(char *iname)
//
// Sets the initialization function name.
// Does nothing if it's already set
//
//----------------------------------------------------------------------

void TCL8::set_init(char *iname) {

  if (init_name) return;
  init_name = new char[strlen(iname)+1];
  strcpy(init_name, iname);

}

// ---------------------------------------------------------------------
// TCL8::headers(void)
//
// Generate the appropriate header files for TCL interface.
// ----------------------------------------------------------------------

void TCL8::headers(void)
{

  emit_banner(f_header);
  fprintf(f_header,"/* Implementation : TCL 8.0 */\n\n");
  fprintf(f_header,"#include <tcl.h>\n");
  fprintf(f_header,"#include <string.h>\n");
  fprintf(f_header,"#include <stdlib.h>\n");
  fprintf(f_header,"#define SWIGTCL\n");
  fprintf(f_header,"#define SWIGTCL8\n");
  
  // Include a Tcl configuration file for Unix,Mac,Wintel.

  if (NoInclude) {
    fprintf(f_header,"#define SWIG_NOINCLUDE\n");
  }

  if (insert_file("swigtcl8.swg",f_header) == -1) {
    fprintf(stderr,"SWIG : Fatal error. Unable to locate 'swigtcl8.swg' in SWIG library.\n");
    SWIG_exit(1);
  }
}

// --------------------------------------------------------------------
// TCL8::initialize(void)
//
// Produces an initialization function.   Assumes that the init function
// name has already been specified.
// ---------------------------------------------------------------------

void TCL8::initialize() 
{

  if ((!ns_name) && (nspace)) {
    fprintf(stderr,"Tcl error.   Must specify a namespace.\n");
    SWIG_exit(1);
  }

  if (!init_name) {
    init_name = "Swig_Init";
    fprintf(stderr,"SWIG : *** Warning. No module name specified.\n");
  }

  fprintf(f_header,"#define SWIG_init    %s\n", init_name);
  if (!module) module = "swig";
  fprintf(f_header,"#define SWIG_name    \"%s\"\n", module);
  if (nspace) {
    fprintf(f_header,"#define SWIG_prefix  \"%s::\"\n", ns_name);
    fprintf(f_header,"#define SWIG_namespace \"%s\"\n\n", ns_name);
  } else {
    fprintf(f_header,"#define SWIG_prefix  \"%s\"\n", prefix);
    fprintf(f_header,"#define SWIG_namespace \"\"\n\n");
  }
  fprintf(f_header,"#ifdef __cplusplus\n");
  fprintf(f_header,"extern \"C\" {\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"#ifdef MAC_TCL\n");
  fprintf(f_header,"#pragma export on\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"SWIGEXPORT(int) %s(Tcl_Interp *);\n", init_name);
  fprintf(f_header,"#ifdef MAC_TCL\n");
  fprintf(f_header,"#pragma export off\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"#ifdef __cplusplus\n");
  fprintf(f_header,"}\n");
  fprintf(f_header,"#endif\n");
  

  fprintf(f_init,"SWIGEXPORT(int) %s(Tcl_Interp *%s) {\n", init_name, interp_name);
  if (nspace) {
    fprintf(f_init,"#ifdef ITCL_NAMESPACES\n");
    fprintf(f_init,"\t Itcl_Namespace spaceId;\n");
    fprintf(f_init,"#endif\n");
  }
  
  fprintf(f_init,"\t if (%s == 0) \n", interp_name);
  fprintf(f_init,"\t\t return TCL_ERROR;\n");

  /* Set up SwigPtrType table */

  fprintf(f_init,"\t SWIG_RegisterType();\n");

  /* Check to see if other initializations need to be performed */

  if (strlen(mod_extern.get())) {
    fprintf(f_init,"%s\n",mod_init.get());
    fprintf(f_header,"#ifdef __cplusplus\n");
    fprintf(f_header,"extern \"C\" {\n");
    fprintf(f_header,"#endif\n");
    fprintf(f_header,"%s\n",mod_extern.get());
    fprintf(f_header,"#ifdef __cplusplus\n");
    fprintf(f_header,"}\n");
    fprintf(f_header,"#endif\n");
  }


  /* Check to see if we're adding support for Tcl8 nspaces */
  if (nspace) {
    fprintf(f_init,"#if (TCL_MAJOR_VERSION >= 8)\n");
    fprintf(f_init,"\t Tcl_Eval(%s,\"namespace eval %s { }\");\n", interp_name, ns_name);
    fprintf(f_init,"#endif\n");
  }
}

// ---------------------------------------------------------------------
// TCL8::close(void)
//
// Wrap things up.  Close initialization function.
// ---------------------------------------------------------------------

void TCL8::close(void)
{

  // Dump the pointer equivalency table

  emit_ptr_equivalence(f_init);

  // Close the init file and quit

  fprintf(f_init,"%s",postinit.get());
  fprintf(f_init,"\t return TCL_OK;\n");
  fprintf(f_init,"}\n");

}

// ----------------------------------------------------------------------
// TCL8::get_pointer(char *iname, char *srcname, char *src, char *dest,
//                  DataType *t, String &f, char *ret)
//
// iname     = name of function or variable
// srcname   = name of source
// src       = source variable in wrapper code
// dest      = destination variable in wrapper code
// t         = Datatype 
// f         = String where output is going to go
// ret       = Return action
// ----------------------------------------------------------------------

void TCL8::get_pointer(char *iname, char *srcname, char *src, char *dest,
		      DataType *t, String &f, char *ret) {

  // Pointers are read as hex-strings with encoded type information
  
  f << tab4 << "if ((rettype = SWIG_GetPointerObj(interp," << src << ",(void **) &" << dest << ",";

  if (t->type == T_VOID) f << "(char *) 0))) {\n";
  else 
    f << "\"" << t->print_mangle() << "\"))) {\n";

  // Now emit code according to the level of strictness desired

  switch(TypeStrict) {
  case 0: // No type checking
    f << tab4 << "}\n";
    break;
  case 1: // Warning message only
    f << tab8 << "fprintf(stderr,\"Warning : type mismatch in " << srcname 
      << " of " << iname << ". Expected " << t->print_mangle() 
      << ", received %s\\n\", rettype);\n"
      << tab4 << "}\n";
  case 2: // Super strict mode.
    f << tab8 << "Tcl_SetStringObj(tcl_result, \"Type error in " << srcname << " of " << iname
      << ". Expected " << t->print_mangle() << ", received \", -1);\n"
      << tab8 << "Tcl_AppendToObj(tcl_result, rettype, -1);\n"
      << tab8 << ret << ";\n"
      << tab4 << "}\n";
    break;
  default :
    fprintf(stderr,"Unknown strictness level\n");
    break;
  }
}


// ----------------------------------------------------------------------
// TCL8::create_command(char *cname, char *iname)
//
// Creates a Tcl command from a C function.
// ----------------------------------------------------------------------

void TCL8::create_command(char *cname, char *iname) {

  char *wname = name_wrapper(cname,prefix);

  fprintf(f_init,"\t Tcl_CreateObjCommand(%s, SWIG_prefix \"%s\",%s, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);\n", interp_name, iname, wname);

  // Add interpreter name to repeatcmd hash table.  This hash is used in C++ code
  // generation to try and find repeated wrapper functions.

  repeatcmd.add(iname,copy_string(wname));
}

// ----------------------------------------------------------------------
// TCL8::create_function(char *name, char *iname, DataType *d, ParmList *l)
//
// Create a function declaration and register it with the interpreter.
// ----------------------------------------------------------------------

void TCL8::create_function(char *name, char *iname, DataType *d, ParmList *l)
{
  Parm            *p;
  int              pcount,i,j;
  char            *wname;
  char            *usage = 0, *tm;
  char             source[64];
  char             target[64];
  char             argnum[32];
  WrapperFunction  f;
  String           cleanup, outarg, build;
  int              numopt= 0;
  int              have_build = 0;

  // Make a wrapper name for this function

  wname = name_wrapper(iname,prefix);

  // Now write the wrapper function itself....this is pretty ugly

  f.def << "static int " << wname << "(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {\n";

  f.code << tab4 << "clientData = clientData; objv = objv;\n";

  // Print out variables for storing arguments.

  pcount = emit_args(d, l, f);
  numopt = l->numopt();

  // Create a local variable for holding the interpreter result value

  f.add_local("Tcl_Obj *", "tcl_result");

  // Extract the tcl result object

  f.code << tab4 << "tcl_result = Tcl_GetObjResult(interp);\n";

  // Check the number of arguments

  usage = usage_func(iname,d,l);       // Create a usage string
  f.code << tab4 << "if ((objc < " << (pcount-numopt) +1 << ") || (objc > " << l->numarg()+1 << ")) {\n"
	 << tab8 << "Tcl_SetStringObj(tcl_result,\"Wrong # args. " << usage << "\",-1);\n"
	 << tab8 << "return TCL_ERROR;\n"
	 << tab4 << "}\n";

  // Extract parameters.    This case statement should be used to extract
  // Function parameters.   Add more cases if you want to do more.

  i = 0;
  j = 0;
  p = l->get_first();
  while (p != 0) {
    // Produce string representations of the source and target arguments
    sprintf(source,"objv[%d]",j+1);
    sprintf(target,"_arg%d",i);
    sprintf(argnum,"%d",j+1);

    // See if this argument is being ignored
    
    if (!p->ignore) {
      if (j >= (pcount-numopt)) 
	f.code << tab4 << "if (objc >" << j+1 << ") { \n";

      if ((tm = typemap_lookup("in","tcl8",p->t,p->name,source,target,&f))) {
	// Yep.  Use it instead of the default
	f.code << tm << "\n";
	f.code.replace("$argnum",argnum);
	f.code.replace("$arg",source);
      } else {
	if (!p->t->is_pointer) {
	
	  // Extract a parameter by value.
	
	  switch(p->t->type) {
	    
	    // Signed Integers
	  
	  case T_BOOL:
	  case T_INT:
	  case T_SINT:
	  case T_SHORT:
	  case T_SSHORT:
	  case T_LONG:
	  case T_SLONG:
	  case T_SCHAR:
	    
	    // Unsigned integers
	  
	  case T_UINT:
	  case T_USHORT:
	  case T_ULONG:
	  case T_UCHAR:
	    f.add_local("int","tempint");
	    f.code << tab4 << "if (Tcl_GetIntFromObj(interp,objv[" << j+1 << "],&tempint) == TCL_ERROR) return TCL_ERROR;\n";
	    f.code << tab4 << "_arg" << i << " = " << p->t->print_cast() << " tempint;\n";
	  break;
	  
	  // Floating point

	  case T_FLOAT:
	  case T_DOUBLE:
	    f.add_local("double","tempdouble");
	    f.add_local("Tcl_Obj *", "dupobj");
	    f.code << tab4 << "dupobj = Tcl_DuplicateObj(objv[" << j+1 << "]);\n"
		   << tab4 << "if (Tcl_GetDoubleFromObj(interp,dupobj,&tempdouble) == TCL_ERROR) {\n"
		   << tab8 << "Tcl_DecrRefCount(dupobj);\n"
		   << tab8 << "return TCL_ERROR;\n"
		   << tab4 << "}\n"
		   << tab4 << "Tcl_DecrRefCount(dupobj);\n"
		   << tab4 << "_arg" << i << " = " << p->t->print_cast() << " tempdouble;\n";
	    break;
	  
	    // A single character
	    
	  case T_CHAR :
	    f.add_local("char *","tempstr");
	    f.add_local("int","templength");
	    f.code << tab4 << "if ((tempstr = Tcl_GetStringFromObj(objv[" << j+1 << "],&templength)) == NULL) return TCL_ERROR;\n"
		   << tab4 << "_arg" << i << " = *tempstr;\n";
	    break;
	  
	    // Void.. Do nothing.
	  
	  case T_VOID :
	    break;
	    
	    // User defined.   This is an error.
	    
	  case T_USER:
	    
	    // Unsupported data type
	    
	  default :
	    fprintf(stderr,"%s : Line %d: Unable to use type %s as a function argument.\n",
		    input_file, line_number, p->t->print_type());
	    break;
	  }
	} else {
	  
	  // Function argument is some sort of pointer
	  // Look for a string.   Otherwise, just pull off a pointer.
	  
	  if ((p->t->type == T_CHAR) && (p->t->is_pointer == 1)) {
	    f.add_local("int","templength");
	    f.code << tab4 << "if ((_arg" << i << " = Tcl_GetStringFromObj(objv[" << j+1 << "], &templength)) == NULL) return TCL_ERROR;\n";
	  } else {
	  
	    // Have a generic pointer type here.    Read it in as
	    // a hex-string
	    char arg_temp[256];
	    
	    // Try to parse pointer value directly
	    
#ifdef OLD
	    f.add_local("char *", "tempstr");
	    f.add_local("int","templength");
	    f.code << tab4 << "if ((tempstr = Tcl_GetStringFromObj(objv[" << j+1 << "],&templength)) == NULL) return TCL_ERROR;\n";
	    get_pointer(iname,arg_temp,"tempstr",target,p->t,f.code,"return TCL_ERROR");
#endif
	    sprintf(arg_temp,"argument %d",j+1);
	    f.add_local("char *", "rettype");
	    get_pointer(iname,arg_temp,source,target,p->t,f.code,"return TCL_ERROR");
	  }
	}
      }
      if (j >= (pcount-numopt))
	f.code << tab4 << "}\n";
      j++;
    }


    // Check to see if there is any sort of "build" typemap (highly complicated)

    if ((tm = typemap_lookup("build","tcl8",p->t,p->name,source,target))) {
      build << tm << "\n";
      have_build = 1;
    }

    // Check to see if there was any sort of a constaint typemap
    if ((tm = typemap_lookup("check","tcl8",p->t,p->name,source,target))) {
      // Yep.  Use it instead of the default
      f.code << tm << "\n";
      f.code.replace("$argnum",argnum);
      f.code.replace("$arg",source);
    }

    // Check if there was any cleanup code (save it for later)
    if ((tm = typemap_lookup("freearg","tcl8",p->t,p->name,target,"tcl_result"))) {
      // Yep.  Use it instead of the default
      cleanup << tm << "\n";
      cleanup.replace("$argnum",argnum);
      cleanup.replace("$arg",source);
    }
    // Look for output arguments
    if ((tm = typemap_lookup("argout","tcl8",p->t,p->name,target,"tcl_result"))) {
      outarg << tm << "\n";
      outarg.replace("$argnum",argnum);
      outarg.replace("$arg",source);
    }
    i++;
    p = l->get_next();   // Get next parameter and continue
  }


  // If there was a "build" typemap, we need to go in and perform a serious hack
  
  if (have_build) {
    char temp1[32];
    char temp2[256];
    l->sub_parmnames(build);            // Replace all parameter names
    j = 1;
    for (i = 0; i < l->nparms; i++) {
      p = l->get(i);
      if (strlen(p->name) > 0) {
	sprintf(temp1,"_in_%s", p->name);
      } else {
	sprintf(temp1,"_in_arg%d", i);
      }
      sprintf(temp2,"argv[%d]",j);
      build.replaceid(temp1,temp2);
      if (!p->ignore) 
	j++;
    }
    f.code << build;
  }

  // Now write code to make the function call

  emit_func_call(name,d,l,f);

  // Extract the tcl result object

  f.code << tab4 << "tcl_result = Tcl_GetObjResult(interp);\n";


  // Return value if necessary 

  if ((tm = typemap_lookup("out","tcl8",d,name,"_result","tcl_result"))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  } else if ((d->type != T_VOID) || (d->is_pointer)) {
    if (!d->is_pointer) {
	
      // Function returns a "value"
	
      switch(d->type) {
	// Is an integer
      case T_BOOL:
      case T_INT:
      case T_SINT:
      case T_SHORT:
      case T_SSHORT:
      case T_LONG :
      case T_SLONG:
      case T_SCHAR:
      case T_UINT:
      case T_USHORT:
      case T_ULONG:
      case T_UCHAR:
	f.code << tab4 << "Tcl_SetIntObj(tcl_result,(long) _result);\n";
	break;
	
	// Is a single character.  Assume we return it as a string
      case T_CHAR :
	f.code << tab4 << "Tcl_SetStringObj(tcl_result,&_result,1);\n";
	break;
	
	// Floating point number
      case T_DOUBLE :
      case T_FLOAT :
	f.code << tab4 << "Tcl_SetDoubleObj(tcl_result,(double) _result);\n";
	break;
	
	// User defined type
      case T_USER :
	
	// Okay. We're returning malloced memory at this point.
	// Probably dangerous, but who said safety was a good thing?
	
	//	f.add_local("char","resultchar[256]");
	d->is_pointer++;
#ifdef OLD
	f.code << tab4 << "SWIG_MakePtr(resultchar, (void *) _result,\"" << d->print_mangle() << "\");\n"
	       << tab4 << "Tcl_SetStringObj(tcl_result,resultchar,-1);\n";
#endif
	f.code << tab4 << "SWIG_SetPointerObj(tcl_result,(void *) _result,\"" << d->print_mangle() << "\");\n";
	
	d->is_pointer--;
	break;
	
	// Unknown type
      default :
	fprintf(stderr,"%s : Line %d: Unable to use return type %s in function %s.\n",
		input_file, line_number, d->print_type(), name);
	break;
      }
    } else {
	
      // Is a pointer return type
      
      if ((d->type == T_CHAR) && (d->is_pointer == 1)) {
	// Return a character string
	f.code << tab4 << "Tcl_SetStringObj(tcl_result,_result,-1);\n";
      } else {
#ifdef OLD
	f.add_local("char","resultchar[256]");
	f.code << tab4 << "SWIG_MakePtr(resultchar, (void *) _result,\"" << d->print_mangle() << "\");\n"
	       << tab4 << "Tcl_SetStringObj(tcl_result,resultchar,-1);\n";
#endif
	
	f.code << tab4 << "SWIG_SetPointerObj(tcl_result,(void *) _result,\"" << d->print_mangle() << "\");\n";	
      }
    }
  }


  // Dump output argument code
  f.code << outarg;

  // Dump the argument cleanup code
  f.code << cleanup;

  // Look for any remaining cleanup

  if (NewObject) {
    if ((tm = typemap_lookup("newfree","tcl8",d,iname,"_result",""))) {
      f.code << tm << "\n";
    }
  }

  if ((tm = typemap_lookup("ret","tcl8",d,name,"_result",""))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  }
  
  // Wrap things up (in a manner of speaking)

  f.code << tab4 << "return TCL_OK;\n}";

  // Substitute the cleanup code
  f.code.replace("$cleanup",cleanup);
  f.code.replace("$name",iname);

  // Dump out the function

  f.print(f_wrappers);

  // Now register the function with Tcl

  fprintf(f_init,"\t Tcl_CreateObjCommand(%s, SWIG_prefix \"%s\", %s, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);\n",interp_name, iname, wname);

  // If there's a documentation entry, produce a usage string

  if (doc_entry) {

    static DocEntry *last_doc_entry = 0;

    // Use usage as description
    doc_entry->usage << usage;

    // Set the cinfo field to specific a return type 

    if (last_doc_entry != doc_entry) {
      doc_entry->cinfo << "returns " << d->print_type();
      last_doc_entry = doc_entry;
    }
  }
}

// -----------------------------------------------------------------------
// TCL8::link_variable(char *name, char *iname, DataType *t,
//                           int ex)
//
// Create a TCL link to a variable.
// -----------------------------------------------------------------------

void TCL8::link_variable(char *name, char *iname, DataType *t)
{

  String    s;
  char      *tm, *tm1;

  // See if there were any typemaps

  tm = typemap_lookup("varin","tcl8",t,name,"","");
  tm1 = typemap_lookup("varout","tcl8",t,name,"","");
  if (tm || tm1) {
    fprintf(stderr,"%s : Line %d. Warning. varin/varout typemap methods not supported.",
	    input_file, line_number);
  }

  // Check the datatype.  Must be a valid Tcl type (there aren't many)

  if (((t->type == T_INT) && (!t->is_pointer)) ||
      ((t->type == T_UINT) && (!t->is_pointer)) ||
      ((t->type == T_SINT) && (!t->is_pointer)) ||
      ((t->type == T_DOUBLE) && (!t->is_pointer)) ||
      ((t->type == T_BOOL) && (!t->is_pointer)) ||
      ((t->type == T_CHAR) && (t->is_pointer == 1))) {

    // This is a valid TCL type. 

    if (t->type == T_UINT)
      fprintf(stderr,"%s : Line %d : ** Warning. Linkage of unsigned type may be unsafe.\n",
              input_file,  line_number);

    // Now add symbol to the TCL interpreter

    switch(t->type) {
    case T_CHAR :
      if (t->arraystr) {
	// Is an array.  We have to do something different
	fprintf(f_wrappers,"static char *tclvar%s = %s;\n",name,name);
	s << "(char *) &tclvar" << name << ", TCL_LINK_STRING";
      } else {
	s << "(char *) &" << name << ", TCL_LINK_STRING";
      }
      break;
    case T_BOOL:
    case T_INT :
    case T_UINT:
    case T_SINT:
      s << "(char *) &" << name << ", TCL_LINK_INT";
      break;
    case T_DOUBLE :
      s << "(char *) &" << name << ", TCL_LINK_DOUBLE";
      break;
    default :
      fprintf(f_init,"Fatal error. Internal error (Tcl:link_variable)\n");
      break;
    }

    if (Status & STAT_READONLY)
      s << " | TCL_LINK_READ_ONLY);\n";
    else
      s << ");\n";

    fprintf(f_init,"\t Tcl_LinkVar(%s, SWIG_prefix \"%s\", %s",interp_name, iname, s.get());    
    
    // Make a usage string for it

    if (doc_entry) {
      doc_entry->usage << usage_var(iname,t);
      doc_entry->cinfo = "";
      doc_entry->cinfo << "Global : " << t->print_type() << " " << name;
    }
  } else {

    // Have some sort of "other" type.
    // We're going to emit some functions to set/get it's value instead
    
    emit_set_get(name,iname, t);
    if (doc_entry) {
      doc_entry->cinfo = "";
      doc_entry->cinfo << "Global : " << t->print_type() << " " << iname;
    }

    // If shadow classes are enabled and we have a user-defined type
    // that we know about, create a command for it.  

    if (shadow) {
      if ((t->type == T_USER) && (t->is_pointer < 1)) {
	// See if the datatype is in our hash table
	if (hash.lookup(t->name)) {
	  // Yep.  Try to create a command for it

	  postinit << tab4 << "{\n"
		   << tab8 << "char cmd[] = \"" 
		   << (char *) hash.lookup(t->name) << " " << iname << " -this ["
		   << iname << "_get ]\";\n"
		   << tab8 << "Tcl_GlobalEval(interp,cmd);\n"
		   << tab4 << "}\n";
	}
      }
    }
  }  
}

// -----------------------------------------------------------------------
// TCL8::declare_const(char *name, char *iname, DataType *type, char *value)
//
// Makes a constant.  Really just creates a variable and links to it.
// Tcl variable linkage allows read-only variables so we'll use that
// instead of just creating a Tcl variable.
// ------------------------------------------------------------------------

void TCL8::declare_const(char *name, char *, DataType *type, char *value) {

  int OldStatus = Status;      // Save old status flags
  DataType *t; 
  char   var_name[256];
  char     *tm;
  String  rvalue;
  Status = STAT_READONLY;      // Enable readonly mode.

  // Make a static variable;

  sprintf(var_name,"_wrap_const_%s",name);
  
  // See if there's a typemap
  rvalue = value;
  if ((type->type == T_CHAR) && (type->is_pointer == 1)) {
    rvalue << "\""; 
    "\"" >> rvalue;
  } 
  if ((type->type == T_CHAR) && (type->is_pointer == 0)) {
    rvalue << "'";
    "'" >> rvalue;
  }
  if ((tm = typemap_lookup("const","tcl8",type,name,rvalue.get(),name))) {
    // Yep.  Use it instead of the default
    fprintf(f_init,"%s\n",tm);
  } else {

    // Create variable and assign it a value
    
    if (type->is_pointer == 0) {
      switch(type->type) {
      case T_BOOL: case T_INT: case T_SINT: case T_DOUBLE:
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	link_variable(var_name,name,type);
	break;
      case T_SHORT:
      case T_LONG:
      case T_SSHORT:
      case T_SCHAR:
      case T_SLONG:
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	fprintf(f_header,"static char *%s_char;\n", var_name);
	if (CPlusPlus)
	  fprintf(f_init,"\t %s_char = new char[32];\n",var_name);
	else
	  fprintf(f_init,"\t %s_char = (char *) malloc(32);\n",var_name);
	
	fprintf(f_init,"\t sprintf(%s_char,\"%%ld\", (long) %s);\n", var_name, var_name);
	sprintf(var_name,"%s_char",var_name);
	t = new DataType(T_CHAR);
	t->is_pointer = 1;
	link_variable(var_name,name,t);
	delete t;
	break;
      case T_UINT:
      case T_USHORT:
      case T_ULONG:
      case T_UCHAR:
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	fprintf(f_header,"static char *%s_char;\n", var_name);
	if (CPlusPlus)
	  fprintf(f_init,"\t %s_char = new char[32];\n",var_name);
	else
	  fprintf(f_init,"\t %s_char = (char *) malloc(32);\n",var_name);
	
	fprintf(f_init,"\t sprintf(%s_char,\"%%lu\", (unsigned long) %s);\n", var_name, var_name);
	sprintf(var_name,"%s_char",var_name);
	t = new DataType(T_CHAR);
	t->is_pointer = 1;
	link_variable(var_name,name,t);
	delete t;
	break;
      case T_FLOAT:
	type->type = T_DOUBLE;
	strcpy(type->name,"double");
	fprintf(f_header,"static %s %s = %s (%s);\n", type->print_type(), var_name, type->print_cast(), value);
	link_variable(var_name,name,type);
	break;
	
      case T_CHAR:
	type->is_pointer++;
	fprintf(f_header,"static %s %s = \"%s\";\n", type->print_type(), var_name, value);
	link_variable(var_name,name,type);
	type->is_pointer--;
	break;
      default:
	fprintf(stderr,"%s : Line %d. Unsupported constant value.\n", input_file, line_number);
	break;
      }
    } else {
      // Have some sort of pointer value here
      if ((type->type == T_CHAR) && (type->is_pointer == 1)) {
	// Character string
	fprintf(f_header,"static %s %s = \"%s\";\n", type->print_type(), var_name, value);
	link_variable(var_name,name,type);
      } else {
	// Something else.   Some sort of pointer value
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	fprintf(f_header,"static char *%s_char;\n", var_name);
	if (CPlusPlus)
	  fprintf(f_init,"\t %s_char = new char[%d];\n",var_name,(int) strlen(type->print_mangle())+ 20);
	else
	  fprintf(f_init,"\t %s_char = (char *) malloc(%d);\n",var_name, (int) strlen(type->print_mangle())+ 20);
	
	t = new DataType(T_CHAR);
	t->is_pointer = 1;
	fprintf(f_init,"\t SWIG_MakePtr(%s_char, (void *) %s,\"%s\");\n",
		var_name, var_name, type->print_mangle());
	sprintf(var_name,"%s_char",var_name);
	link_variable(var_name,name,t);
	delete t;
      }
    }
  }

  // Create a documentation entry for this

  if (doc_entry) {
    doc_entry->usage = "";       // Destroy any previous information from linking
    doc_entry->usage << usage_const(name, type, value);
    doc_entry->cinfo = "";
    doc_entry->cinfo << "Constant : " << type->print_type();
  }

  Status = OldStatus;
}

// ----------------------------------------------------------------------
// TCL8::usage_var(char *iname, DataType *t, char **s)
//
// Produces a usage string for a tcl variable.  Stores it in s
// ----------------------------------------------------------------------

char *TCL8::usage_var(char *iname, DataType *t) {

  static char temp[1024];

  if (!nspace) {
    sprintf(temp,"$%s%s", prefix, iname);
  } else { 
    sprintf(temp,"%s::%s", ns_name, iname);
  }
  if (!(((t->type == T_INT) && (!t->is_pointer)) ||
	((t->type == T_UINT) && (!t->is_pointer)) ||
	((t->type == T_DOUBLE) && (!t->is_pointer)) ||
	((t->type == T_BOOL) && (!t->is_pointer)) ||
	((t->type == T_CHAR) && (t->is_pointer)))) {
    /* We emitted a pair of set/get functions instead.  Doc will be generated for that */
    return temp;
  }
  return temp;
}



// ---------------------------------------------------------------------------
// char *TCL8::usage_string(char *iname, DataType *t, ParmList *l),
// 
// Generates a generic usage string for a Tcl function.
// ---------------------------------------------------------------------------

char * TCL8::usage_string(char *iname, DataType *, ParmList *l) {

  static String temp;
  Parm  *p;
  int   i, numopt,pcount;

  temp = "";
  temp << iname << " ";
  
  /* Now go through and print parameters */
  i = 0;
  pcount = l->nparms;
  numopt = l->numopt();
  p = l->get_first();
  while (p != 0) {

    // Only print an argument if not ignored

    if (!typemap_check("ignore","tcl8",p->t,p->name)) {
      if (i >= (pcount-numopt))
	temp << "?";

      /* If parameter has been named, use that.   Otherwise, just print a type  */

      if ((p->t->type != T_VOID) || (p->t->is_pointer)) {
	if (strlen(p->name) > 0) {
	  temp << p->name;
	}
	else {
	  temp << "{ " << p->t->print_type() << " }";
	}
      }
      if (i >= (pcount-numopt))
	temp << "?";
      temp << " ";
      i++;
    }
    p = l->get_next();
  }
  return temp;
}

// ---------------------------------------------------------------------------
// char *TCL8::usage_func(char *iname, DataType *t, ParmList *l),
// 
// Produces a usage string for a function in Tcl
// ---------------------------------------------------------------------------

char * TCL8::usage_func(char *iname, DataType *t, ParmList *l) {

  String temp;

  if (nspace) {
    temp << ns_name << "::" << iname;
  } else {
    temp << prefix << iname;
  }
  return usage_string(temp,t,l);
}

// -----------------------------------------------------------------
// TCL8::usage_const(char *name, DataType *type, char *value)
//                         char **s)
//
// Makes a usage string and returns it
// -----------------------------------------------------------------

char *TCL8::usage_const(char *name, DataType *, char *value) {
  static String temp;
  temp = "";
  if (nspace) {
    temp << ns_name << "::" << name << " = " << value;
  } else {
    temp << "$" << prefix << name << " = " << value;
  }
  return temp.get();
}
    
// -------------------------------------------------------------------
// TCL8::add_native(char *name, char *funcname)
//
// This adds an already written Tcl wrapper function to our
// initialization function.
// -------------------------------------------------------------------


void TCL8::add_native(char *name, char *funcname) {

  fprintf(f_init,"\t Tcl_CreateCommand(%s, SWIG_prefix \"%s\", %s, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);\n",interp_name, name, funcname);
  
  if (doc_entry) {
    if (nspace) 
      doc_entry->usage << ns_name << "::" << name << " args";
    else
      doc_entry->usage << prefix << name << " args";

    doc_entry->cinfo << "Native method : " << funcname;
  }
    
}
 
// -------------------------------------------------------------------
// TCL8::pragma(char *lname, char *name, char *value)
//
// Handle pragmas.
// --------------------------------------------------------------------

void TCL8::pragma(char *, char *, char *) {

}

// ---------------------------------------------------------------------
// C++ Handling
//
// The following functions provide some support for C++ classes and
// C structs.   
// ---------------------------------------------------------------------

void TCL8::cpp_open_class(char *classname, char *rename, char *ctype, int strip) {

  this->Language::cpp_open_class(classname,rename,ctype,strip);

  if (shadow) {
  
    config = "";
    cget = "";
    methods = "";
    options = "";
    config_options = "";
    methodnames = "";
    
    have_constructor = 0;
    have_destructor = 0;
    have_methods = 0;
    have_config = 0;
    have_cget = 0;

    if (rename)
      class_name = copy_string(rename);
    else
      class_name = copy_string(classname);
  
    base_class = (char *) 0;
    if (!strip) {
      class_type = new char[strlen(ctype)+2];
      sprintf(class_type,"%s ", ctype);
    } else
      class_type = "";

    real_classname = copy_string(classname);
  }
}

void TCL8::cpp_close_class() {
  String code,temp;
  DataType *t;

  this->Language::cpp_close_class();
  if (shadow) {

    t = new DataType;
    sprintf(t->name,"%s%s", class_type, real_classname);
    t->type = T_USER;
    t->is_pointer = 1;
    
    // Note : The object oriented interface is defined by three files
    //     delcmd8.swg          - Object deletion wrapper
    //     methodcmd8.swg       - Method invocation command
    //     objcmd8.swg          - Command to create a new object
    //
    // These files are located in the SWIG library.  This module
    // grabs the files and performs marker replacements to
    // build the wrapper function.
    
    // Generate a Tcl function for object destruction
    
    if (have_destructor) {
      code << delcmd;
    }

    // Dump out method code
    code << methodcmd;

    // Dump out object creation command
    code << objcmd;

    // Now perform marker replacements
    code.replace("@CLASS@",class_name);
    temp = "";
    temp << name_destroy(class_name);
    code.replace("@DESTRUCTOR@",temp);
    code.replace("@CLASSTYPE@",t->print_type());
    "configure " >> methodnames;
    "cget " >> methodnames;
    code.replace("@METHODLIST@", methodnames);
    code.replace("@CLASSMANGLE@",t->print_mangle());
    code.replace("@METHODS@",methods);
    code.replace("@CONFIGMETHODS@",config);
    code.replace("@CGETMETHODS@",cget);
    if (have_constructor) {
      temp = "";
      temp << name_wrapper(name_construct(class_name),prefix);
    } else {
      temp = "0";
    }
    code.replace("@TCLCONSTRUCTOR@",temp);
    code.replace("@CONFIGLIST@",config_options);
    code.replace("@CGETLIST@",options);
    if (have_destructor) {
      temp = "TclDelete";
      temp << class_name;
    } else {
      temp = "0";
    }
    code.replace("@TCLDESTRUCTOR@",temp);
    fprintf(f_wrappers,"%s\n", code.get());

    fprintf(f_init,"\t Tcl_CreateObjCommand(interp,SWIG_prefix \"%s\",Tcl%sCmd, (ClientData) NULL, NULL);\n", class_name, class_name);
  }
}

void TCL8::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l) {

  char *realname;
  String temp;
  char  *rname;

  this->Language::cpp_member_func(name,iname,t,l);

  if (shadow) {
    if (iname) 
      realname = iname;
    else
      realname = name;
    
    // Add stubs for this member to our class handler function

    if (have_methods) 
      methods << tab4 << "else ";
    else 
      methods << tab4;

    temp = "";
    temp << name_member(realname,class_name);
    rname = (char *) repeatcmd.lookup(temp);
    if (!rname)
      rname = name_wrapper(temp.get(),prefix);
    
    methods << "if (strcmp(_str,\"" << realname << "\") == 0) {\n"
	    << tab4 << tab4 << "cmd = " << rname << ";\n"
	    << tab4 << "}";

    have_methods = 1;
    methodnames << realname << " ";

    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << usage_string(realname,t,l);
    }
  }
}

void TCL8::cpp_variable(char *name, char *iname, DataType *t) {
  char *realname;
  String temp;
  char *rname;

  this->Language::cpp_variable(name, iname, t);

  if (shadow) {
    if (iname)
      realname = iname;
    else
      realname = name;
    
    char *bc = class_name;

    // Write config code
    
    if (!(Status & STAT_READONLY)) {
      if (!have_config) {
	config << tab8 << tab8;
      } else {
	config << " else ";
      }

      // Try to figure out if there is already a wrapper for this
      
      temp = "";
      temp << name_set(name_member(realname,bc));
      rname = (char *) repeatcmd.lookup(temp);
      if (!rname) 
	rname = name_wrapper(temp.get(),prefix);

      config << "if (strcmp(_str,\"-" << realname << "\") == 0) {\n"
	     << tab8 << tab8 << tab4 << "cmd = " << rname << ";\n"
	     << tab8 << tab8 << "} ";
      
      have_config = 1;
    }
    
    // Write cget code
    
    if (!have_cget) {
      cget << tab8 << tab8;
    } else {
      cget << " else ";
    }


    // Try to figure out if there is a wrapper for this function
    temp = "";
    temp << name_get(name_member(realname,bc));
    rname = (char *) repeatcmd.lookup(temp);
    if (!rname) 
      rname = name_wrapper(temp.get(),prefix);

    cget << "if (strcmp(_str,\"-" << realname << "\") == 0) {\n"
	 << tab8 << tab8 << tab4 << "cmd = " << rname << ";\n"
	 << tab8 << tab8 << "} ";
    have_cget = 1;
    
    options << "-" << realname << " ";
    if (!(Status & STAT_READONLY)) {
      config_options << "-" << realname << " ";
    }
    if (doc_entry){
      doc_entry->usage = "";
      doc_entry->usage << "-" << realname << "\n";
    }
  }
}

void TCL8::cpp_constructor(char *name, char *iname, ParmList *l) {
  this->Language::cpp_constructor(name,iname,l);

  if (shadow) {
    if ((!have_constructor) && (doc_entry)) {
      doc_entry->usage = "";
      doc_entry->usage << class_name << usage_string(" name",0,l);
    }
    have_constructor = 1;
  }
}
void TCL8::cpp_destructor(char *name, char *newname) {
  this->Language::cpp_destructor(name,newname);
  if (shadow) {
    if (!have_destructor) {
      if (doc_entry) {
	doc_entry->usage = "rename obj {}";
      }
    }
    have_destructor = 1;
  }
}

void TCL8::cpp_inherit(char **baseclass, int) {
  this->Language::cpp_inherit(baseclass);
}

void TCL8::cpp_declare_const(char *name, char *iname, DataType *type, char *value) {
  this->Language::cpp_declare_const(name,iname,type,value);
}

// --------------------------------------------------------------------------------
// TCL8::add_typedef(DataType *t, char *name)
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

void TCL8::add_typedef(DataType *t, char *name) {

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

// -----------------------------------------------------------------------
// TCL8::cpp_class_decl(char *name, char *rename, char *type)
//
// Treatment of an empty class definition.    Used to handle
// shadow classes across modules.
// -----------------------------------------------------------------------

void TCL8::cpp_class_decl(char *name, char *rename, char *type) {
  char temp[256];
  this->Language::cpp_class_decl(name,rename, type);

  if (shadow) {
    hash.add(name,copy_string(rename));
    // Add full name of datatype to the hash table
    if (strlen(type) > 0) {
      sprintf(temp,"%s %s", type, name);
      hash.add(temp,copy_string(rename));
    }
  }
}
