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
 * perl5.c
 *
 * Definitions for adding functions to Perl 5
 *
 * How to extend perl5 (note : this is totally different in Perl 4) :
 *
 * 1.   Variable linkage
 *
 *      Must declare two functions :
 *
 *          _var_set(SV *sv, MAGIC *mg);
 *          _var_get(SV *sv, MAGIC *mg);
 *
 *      These functions must set/get the values of a variable using
 *      Perl5 internals.
 *
 *      To add these to Perl5 (which isn't entirely clear), need to
 *      do the following :
 *
 *            SV  *sv;
 *            MAGIC  *m;
 *            sv = perl_get_sv("varname",TRUE);
 *            sv_magic(sv,sv, 'U', "varname", strlen("varname));
 *            m = mg_find(sv, 'U');
 *            m->mg_virtual = (MGVTBL *) malloc(sizeof(MGVTBL));
 *            m->mg_virtual.svt_get = _var_set;
 *            m->mg_virtual.svt_set = _var_get;
 *            m->mg_virtual.svt_len = 0;
 *            m->mg_virtual.svt_free = 0;
 *            m->mg_virtual.svt_clear = 0;
 *
 *
 * 2.   Function extension
 *
 *      Functions are declared as :
 *             XS(_wrap_func) {
 *                 dXSARGS;
 *                 if (items != parmcount) {
 *                     croak("Usage :");
 *                 }
 *              ... get arguments ...
 *
 *              ... call function ...
 *              ... set return value in ST(0) 
 *                 XSRETURN(1);
 *              }
 *      To extract function arguments, use the following :
 *              _arg = (int) SvIV(ST(0))
 *              _arg = (double) SvNV(ST(0))
 *              _arg = (char *) SvPV(ST(0),na);
 *
 *      For return values, use :
 *              ST(0) = sv_newmortal();
 *              sv_setiv(ST(0), (IV) RETVAL);     // Integers
 *              sv_setnv(ST(0), (double) RETVAL); // Doubles
 *              sv_setpv((SV*) ST(0), RETVAL);    // Strings
 *
 *      New functions are added using 
 *              newXS("name", _wrap_func, file)
 *
 *    
 * 3.   Compilation.
 *
 *      Code should be compiled into an object file for dynamic
 *      loading into Perl.
 ***********************************************************************/

#include "swig.h"
#include "perl5.h"

static String pragma_include;

static char *usage = "\
Perl5 Options (available with -perl5)\n\
     -module name    - Set module name\n\
     -package name   - Set package prefix\n\
     -static         - Omit code related to dynamic loading.\n\
     -shadow         - Create shadow classes.\n\
     -compat         - Compatibility mode.\n\
     -alt-header file- Use an alternate header.\n\n";

static char *import_file = 0;
static char *smodule = 0;
static int   compat = 0;

// ---------------------------------------------------------------------
// PERL5::parse_args(int argc, char *argv[])
//
// Parse command line options.
// ---------------------------------------------------------------------

void
PERL5::parse_args(int argc, char *argv[]) {

  int i = 1;

  export_all = 0;
  sprintf(LibDir,"%s", perl_path);

  // Look for certain command line options

  // Get options
  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if(strcmp(argv[i],"-package") == 0) {
	    if (argv[i+1]) {
	      package = new char[strlen(argv[i+1])+1];
	      strcpy(package, argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-module") == 0) {
	    if (argv[i+1]) {
	      module = new char[strlen(argv[i+1])+1];
	      strcpy(module, argv[i+1]);
	      cmodule = module;
	      cmodule.replace(":","_");
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-exportall") == 0) {
	      export_all = 1;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-static") == 0) {
	      is_static = 1;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-shadow") == 0) {
	    blessed = 1;
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-alt-header") == 0) {
	    if (argv[i+1]) {
	      alt_header = copy_string(argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-compat") == 0) {
	    compat = 1;
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(usage,stderr);
	  }
      }
  }
  // Add a symbol for this module

  add_symbol("SWIGPERL",0,0);
  add_symbol("SWIGPERL5",0,0);

  // Set name of typemaps

  typemap_lang = "perl5";

}

// ------------------------------------------------------------------
// PERL5::parse()
//
// Parse an interface file
// ------------------------------------------------------------------

void
PERL5::parse() {


  printf("Generating wrappers for Perl 5\n");

  // Print out PERL5 specific headers
  
  headers();
  
  // Run the parser
  
  yyparse();
  fputs(vinit.get(),f_wrappers);
}


// ---------------------------------------------------------------------
// PERL5::set_module(char *mod_name, char **mod_list)
//
// Sets the module name.
// Does nothing if it's already set (so it can be overridden as a command
// line option).
//
//----------------------------------------------------------------------
static String modinit, modextern;

void PERL5::set_module(char *mod_name, char **mod_list) {
  int i;
  if (import_file) {
    if (!(strcmp(import_file,input_file+strlen(input_file)-strlen(import_file)))) {
      if (blessed) {
	fprintf(f_pm,"require %s;\n", mod_name);
      }
      delete [] import_file;
      import_file = 0;
    }
  }

  if (module) return;
  
  module = new char[strlen(mod_name)+1];
  strcpy(module,mod_name);

  // if there was a mod_list specified, make this big hack
  if (mod_list) {
    modinit << "#define SWIGMODINIT ";
    modextern << "#ifdef __cplusplus\n"
	      << "extern \"C\" {\n"
	      << "#endif\n";
    i = 0;
    while(mod_list[i]) {
      modinit << "newXS(\"" << mod_list[i] << "::boot_" << mod_list[i] << "\", boot_" << mod_list[i] << ", file);\\\n";
      modextern << "extern void boot_" << mod_list[i] << "(CV *);\n";
      i++;
    }
    modextern << "#ifdef __cplusplus\n"
	      << "}\n"
	      << "#endif\n";
    modinit << "/* End of extern module initialization */\n";
  }

  // Create a C module name and put it in 'cmodule'

  cmodule = module;
  cmodule.replace(":","_");
}

// ---------------------------------------------------------------------
// PERL5::set_init(char *iname)
//
// Sets the initialization function name.
// Does nothing if it's already set
//
//----------------------------------------------------------------------

void PERL5::set_init(char *iname) {
  set_module(iname,0);
}

// ---------------------------------------------------------------------
// PERL5::headers(void)
//
// Generate the appropriate header files for PERL5 interface.
// ----------------------------------------------------------------------

void PERL5::headers(void)
{

  emit_banner(f_header);

  if (!alt_header) {
    if (insert_file("headers.swg", f_header) == -1) {
      fprintf(stderr,"Perl5 : Fatal error. Unable to locate headers.swg. Possible installation problem.\n");
      SWIG_exit(1);
    }
  } else {
    if (insert_file(alt_header, f_header) == -1) {
      fprintf(stderr,"SWIG : Fatal error.  Unable to locate %s.\n",alt_header);
      SWIG_exit(1);
    }
  }

  if (NoInclude) {
    fprintf(f_header,"#define SWIG_NOINCLUDE\n");
  }

  // Get special SWIG related declarations
  if (insert_file("perl5.swg", f_header) == -1) {
    fprintf(stderr,"SWIG : Fatal error.  Unable to locate 'perl5.swg' in SWIG library.\n");
    SWIG_exit(1);
  }

  // Get special SWIG related declarations
  if (insert_file("perl5mg.swg", f_header) == -1) {
    fprintf(stderr,"SWIG : Fatal error.  Unable to locate 'perl5mg.swg' in SWIG library.\n");
    SWIG_exit(1);
  }

}

// --------------------------------------------------------------------
// PERL5::initialize()
//
// Output initialization code that registers functions with the
// interface.
// ---------------------------------------------------------------------

void PERL5::initialize()
{

  char filen[256];

  if (!module){
    module = "swig";
    fprintf(stderr,"SWIG : *** Warning. No module name specified.\n");
  }

  if (!package) {
    package = new char[strlen(module)+1];
    strcpy(package,module);
  }

  // If we're in blessed mode, change the package name to "packagec"

  if (blessed) {
    char *newpackage = new char[strlen(package)+2];
    sprintf(newpackage,"%sc",package);
    realpackage = package;
    package = newpackage;
  } else {
    realpackage = package;
  }

  // Create a .pm file 
  // Need to strip off any prefixes that might be found in
  // the module name

  {
    char *m = module + strlen(module);
    while (m != module) {
      if (*m == ':') {
	m++;
	break;
      }
      m--;
    }
    sprintf(filen,"%s%s.pm", output_dir,m);
    if ((f_pm = fopen(filen,"w")) == 0) {
      fprintf(stderr,"Unable to open %s\n", filen);
      SWIG_exit(0);
    }
  }
  if (!blessed) {
    smodule = module;
  } else if (is_static) {
    smodule = new char[strlen(module)+2];
    strcpy(smodule,module);
    strcat(smodule,"c");
    cmodule << "c";
  } else {
    smodule = module;
  }

  fprintf(f_header,"#define SWIG_init    boot_%s\n\n", cmodule.get());
  fprintf(f_header,"#define SWIG_name   \"%s::boot_%s\"\n", package, cmodule.get());
  fprintf(f_header,"#define SWIG_varinit \"%s::var_%s_init();\"\n", package, cmodule.get());
  fprintf(f_header,"#ifdef __cplusplus\n");
  fprintf(f_header,"extern \"C\"\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"#ifndef PERL_OBJECT\n");
  fprintf(f_header,"SWIGEXPORT(void) boot_%s(CV* cv);\n", cmodule.get());
  fprintf(f_header,"#else\n");
  fprintf(f_header,"SWIGEXPORT(void) boot_%s(CV *cv, CPerlObj *);\n",cmodule.get());
  fprintf(f_header,"#endif\n");
  fprintf(f_init,"#ifdef __cplusplus\n");
  fprintf(f_init,"extern \"C\"\n");
  fprintf(f_init,"#endif\n");
  fprintf(f_init,"XS(boot_%s) {\n", cmodule.get());
  fprintf(f_init,"\t dXSARGS;\n");
  fprintf(f_init,"\t char *file = __FILE__;\n");
  fprintf(f_init,"\t cv = cv; items = items;\n");
  fprintf(f_init,"\t newXS(\"%s::var_%s_init\", _wrap_perl5_%s_var_init, file);\n",package,cmodule.get(), cmodule.get());
  vinit << "XS(_wrap_perl5_" << cmodule << "_var_init) {\n"
        << tab4 << "dXSARGS;\n"
	<< tab4 << "SV *sv;\n"
	<< tab4 << "cv = cv; items = items;\n";

  fprintf(f_pm,"# This file was automatically generated by SWIG\n");
  fprintf(f_pm,"package %s;\n",module);
  fprintf(f_pm,"require Exporter;\n");
  if (!is_static) {
    fprintf(f_pm,"require DynaLoader;\n");
    fprintf(f_pm,"@ISA = qw(Exporter DynaLoader);\n");
  } else {
    fprintf(f_pm,"@ISA = qw(Exporter);\n");
  }    

  // Start creating magic code


  magic << "#ifdef PERL_OBJECT\n"
	<< "#define MAGIC_CLASS _wrap_" << module << "_var::\n"
	<< "class _wrap_" << module << "_var : public CPerlObj {\n"
	<< "public:\n"
	<< "#else\n"
	<< "#define MAGIC_CLASS\n"
	<< "#endif\n"
        << "SWIGCLASS_STATIC int swig_magic_readonly(SV *sv, MAGIC *mg) {\n"
	<< tab4 << "MAGIC_PPERL\n"
	<< tab4 << "sv = sv; mg = mg;\n"
	<< tab4 << "croak(\"Value is read-only.\");\n"
	<< tab4 << "return 0;\n"
	<< "}\n";  // Dump out external module declarations

  /* Process additional initialization files here */

  if (strlen(modinit.get()) > 0) {
    fprintf(f_header,"%s\n",modinit.get());
  }
  if (strlen(modextern.get()) > 0) {
    fprintf(f_header,"%s\n",modextern.get());
  }
}

// ---------------------------------------------------------------------
// PERL5::import(char *filename)
//
// Import directive
// ---------------------------------------------------------------------

void PERL5::import(char *filename) {
  if (import_file) delete [] import_file;
  import_file = copy_string(filename);
}


// ---------------------------------------------------------------------
// PERL5::close(void)
//
// Wrap things up.  Close initialization function.
// ---------------------------------------------------------------------

void PERL5::close(void)
{
  String base;

  // Dump out variable wrappers

  magic << "\n\n#ifdef PERL_OBJECT\n"
	<< "};\n"
	<< "#endif\n";

  fprintf(f_header,"%s\n", magic.get());
  
  emit_ptr_equivalence(f_init);

  fprintf(f_init,"\t ST(0) = &PL_sv_yes;\n");
  fprintf(f_init,"\t XSRETURN(1);\n");
  fprintf(f_init,"}\n");

  vinit << tab4 << "XSRETURN(1);\n"
        << "}\n";

  fprintf(f_pm,"package %s;\n", package);	

  if (!is_static) {
    fprintf(f_pm,"bootstrap %s;\n", smodule);
  } else {
    fprintf(f_pm,"boot_%s();\n", smodule);
  }
  fprintf(f_pm,"var_%s_init();\n", cmodule.get());
  fprintf(f_pm,"%s",pragma_include.get());
  fprintf(f_pm,"package %s;\n", realpackage);
  fprintf(f_pm,"@EXPORT = qw(%s );\n",exported.get());

  if (blessed) {

    base << "\n# ---------- BASE METHODS -------------\n\n"
	 << "package " << realpackage << ";\n\n";

    // Write out the TIE method

    base << "sub TIEHASH {\n"
	 << tab4 << "my ($classname,$obj) = @_;\n"
	 << tab4 << "return bless $obj, $classname;\n"
	 << "}\n\n";

    // Output a CLEAR method.   This is just a place-holder, but by providing it we 
    // can make declarations such as
    //     %$u = ( x => 2, y=>3, z =>4 );
    //
    // Where x,y,z are the members of some C/C++ object.

    base << "sub CLEAR { }\n\n";

    // Output default firstkey/nextkey methods

    base << "sub FIRSTKEY { }\n\n";
    base << "sub NEXTKEY { }\n\n";

    // Output a 'this' method

    base << "sub this {\n"
	 << tab4 << "my $ptr = shift;\n"
	 << tab4 << "return tied(%$ptr);\n"
	 << "}\n\n";

    fprintf(f_pm,"%s",base.get());

    // Emit function stubs for stand-alone functions

    fprintf(f_pm,"\n# ------- FUNCTION WRAPPERS --------\n\n");
    fprintf(f_pm,"package %s;\n\n",realpackage);
    fprintf(f_pm,"%s",func_stubs.get());


    // Emit package code for different classes

    fprintf(f_pm,"%s",pm.get());

    // Emit variable stubs

    fprintf(f_pm,"\n# ------- VARIABLE STUBS --------\n\n");
    fprintf(f_pm,"package %s;\n\n",realpackage);
    fprintf(f_pm,"%s",var_stubs.get());

  }

  fprintf(f_pm,"1;\n");
  fclose(f_pm);

  // Patch up documentation title

  if ((doc_entry) && (module)) {
    doc_entry->cinfo << "Module  : " << module << ", "
	 << "Package : " << realpackage;
  }

}

// ----------------------------------------------------------------------
// char *PERL5::type_mangle(DataType *t)
//
// Mangles a datatype into a Perl5 name compatible with xsubpp type
// T_PTROBJ.
// ----------------------------------------------------------------------

char *
PERL5::type_mangle(DataType *t) {
  static char result[128];
  int   i;
  char *r, *c;

  if (blessed) {

    // Check to see if we've blessed this datatype

    if ((classes.lookup(t->name)) && (t->is_pointer <= 1)) {

      // This is a blessed class.  Return just the type-name 
      strcpy(result,(char *) classes.lookup(t->name));
      return result;
    }
  }
      
  r = result;
  c = t->name;

  for ( c = t->name; *c; c++,r++) {
      *r = *c;
  }
  for (i = 0; i < (t->is_pointer-t->implicit_ptr); i++, r++) {
    strcpy(r,"Ptr");
    r+=2;
  }
  *r = 0;
  return result;
}

// ----------------------------------------------------------------------
// PERL5::get_pointer(char *iname, char *srcname, char *src, char *target,
//                     DataType *t, String &f, char *ret)
//
// Emits code to get a pointer from a parameter and do type checking.
// ----------------------------------------------------------------------

void PERL5::get_pointer(char *iname, char *srcname, char *src, char *dest,
			DataType *t, String &f, char *ret) {

  // Now get the pointer value from the string and save in dest
  
  f << tab4 << "if (SWIG_GetPtr(" << src << ",(void **) &" << dest << ",";

  // If we're passing a void pointer, we give the pointer conversion a NULL
  // pointer, otherwise pass in the expected type.
  
  if (t->type == T_VOID) f << "(char *) 0 )) {\n";
  else
    f << "\"" << t->print_mangle() << "\")) {\n";

  // This part handles the type checking according to three different
  // levels.   0 = no checking, 1 = warning message, 2 = strict.

  switch(TypeStrict) {
  case 0: // No type checking
    f << tab4 << "}\n";
    break;

  case 1: // Warning message only

    // Change this part to how you want to handle a type-mismatch warning.
    // By default, it will just print to stderr.

    f << tab8 << "fprintf(stderr,\"Warning : type mismatch in " << srcname
      << " of " << iname << ". Expected " << t->print_mangle()
      << ", received %s\\n\"," << src << ");\n"
      << tab4 << "}\n";

    break;
  case 2: // Super strict mode.

    // Change this part to return an error.

    f << tab8 << "croak(\"Type error in " << srcname
	   << " of " << iname << ". Expected " << t->print_mangle() << ".\");\n"
	   << tab8 << ret << ";\n"
	   << tab4 << "}\n";

    break;
    
  default :
    fprintf(stderr,"SWIG Error. Unknown strictness level\n");
    break;
  }
}

// ----------------------------------------------------------------------
// PERL5::create_command(char *cname, char *iname)
//
// Create a command and register it with the interpreter
// ----------------------------------------------------------------------

void PERL5::create_command(char *cname, char *iname) {
  fprintf(f_init,"\t newXS(\"%s::%s\", %s, file);\n", package, iname, name_wrapper(cname,""));
  if (export_all) {
    exported << iname << " ";
  }
}

// ----------------------------------------------------------------------
// PERL5::create_function(char *name, char *iname, DataType *d,
//                             ParmList *l)
//
// Create a function declaration and register it with the interpreter.
// ----------------------------------------------------------------------

void PERL5::create_function(char *name, char *iname, DataType *d, ParmList *l)
{
  Parm *p;
  int   pcount,i,j;
  char  *wname;
  char *usage = 0;
  WrapperFunction f;
  char  source[256],target[256],temp[256], argnum[32];
  char  *tm;
  String cleanup,outarg,build;
  int    numopt = 0;
  int    need_save, num_saved = 0;             // Number of saved arguments.
  int    have_build = 0;

  // Make a wrapper name for this

  wname = name_wrapper(iname,"");
  
  // Now write the wrapper function itself....this is pretty ugly

  f.def << "XS(" << wname << ") {\n";
  f.code << tab4 << "cv = cv;\n";

  pcount = emit_args(d, l, f);
  numopt = l->numopt();

  f.add_local("int","argvi = 0");

  // Check the number of arguments

  usage = usage_func(iname,d,l);
  f.code << tab4 << "if ((items < " << (pcount-numopt) << ") || (items > " << l->numarg() << ")) \n"
	 << tab8 << "croak(\"Usage: " << usage << "\");\n";

  // Write code to extract parameters.
  // This section should be able to extract virtually any kind 
  // parameter, represented as a string

  i = 0;
  j = 0;
  p = l->get_first();
  while (p != 0) {
    // Produce string representation of source and target arguments
    sprintf(source,"ST(%d)",j);
    sprintf(target,"_arg%d",i);
    sprintf(argnum,"%d",j+1);

    // Check to see if this argument is being ignored

    if (!p->ignore) {
      
      // If there are optional arguments, check for this

      if (j>= (pcount-numopt))
	f.code << tab4 << "if (items > " << j << ") {\n";

      // See if there is a type-map
      if ((tm = typemap_lookup("in","perl5",p->t,p->name,source,target,&f))) {
	f.code << tm << "\n";
	f.code.replace("$argnum",argnum);
	f.code.replace("$arg",source);
      } else {

	if (!p->t->is_pointer) {
	  
	  // Extract a parameter by "value"
	  
	  switch(p->t->type) {
	    
	    // Integers
	    
	  case T_BOOL:
	  case T_INT :
	  case T_SHORT :
	  case T_LONG :
	  case T_SINT :
	  case T_SSHORT:
	  case T_SLONG:
	  case T_SCHAR:
	  case T_UINT:
	  case T_USHORT:
	  case T_ULONG:
	  case T_UCHAR:
	    f.code << tab4 << "_arg" << i << " = " << p->t->print_cast()
	      << "SvIV(ST(" << j << "));\n";
	    break;
	  case T_CHAR :



	    f.code << tab4 << "_arg" << i << " = (char) *SvPV(ST(" << j << "),PL_na);\n";
	    break;
	  
	  // Doubles
	  
	  case T_DOUBLE :
	  case T_FLOAT :
	    f.code << tab4 << "_arg" << i << " = " << p->t->print_cast()
	      << " SvNV(ST(" << j << "));\n";
	    break;
	  
	  // Void.. Do nothing.
	  
	  case T_VOID :
	    break;
	  
	    // User defined.   This is invalid here.   Note, user-defined types by
	    // value are handled in the parser.
	    
	  case T_USER:
	    
	    // Unsupported data type
	    
	  default :
	    fprintf(stderr,"%s : Line %d. Unable to use type %s as a function argument.\n",input_file, line_number, p->t->print_type());
	    break;
	  }
	} else {
	  
	  // Argument is a pointer type.   Special case is for char *
	  // since that is usually a string.
	  
	  if ((p->t->type == T_CHAR) && (p->t->is_pointer == 1)) {
	    f.code << tab4 << "if (! SvOK((SV*) ST(" << j << "))) { "
		   << "_arg" << i << " = 0; }\n";
	    f.code << tab4 << "else { _arg"
		   << i << " = (char *) SvPV(ST(" << j << "),PL_na); }\n";
	  } else {
	    
	    // Have a generic pointer type here.    Read it in as a swig
	    // typed pointer.
	    
	    sprintf(temp,"argument %d", i+1);
	    get_pointer(iname,temp,source,target, p->t, f.code, "XSRETURN(1)");
	  }
	}
      }
      // The source is going to be an array of saved values.

      sprintf(temp,"_saved[%d]",num_saved);
      if (j>= (pcount-numopt))
	f.code << tab4 << "} \n";
      j++;
    } else {
      temp[0] = 0;
    }
    // Check to see if there is any sort of "build" typemap (highly complicated)

    if ((tm = typemap_lookup("build","perl5",p->t,p->name,source,target))) {
      build << tm << "\n";
      have_build = 1;
    }

    // Check if there is any constraint code
    if ((tm = typemap_lookup("check","perl5",p->t,p->name,source,target))) {
      f.code << tm << "\n";
      f.code.replace("$argnum",argnum);
    }
    need_save = 0;

    if ((tm = typemap_lookup("freearg","perl5",p->t,p->name,target,temp))) {
      cleanup << tm << "\n";
      cleanup.replace("$argnum",argnum);
      cleanup.replace("$arg",temp);
      need_save = 1;
    }
    if ((tm = typemap_lookup("argout","perl5",p->t,p->name,target,"ST(argvi)"))) {
      String tempstr;
      tempstr = tm;
      tempstr.replace("$argnum",argnum);
      tempstr.replace("$arg",temp);
      outarg << tempstr << "\n";
      need_save = 1;
    }
    // If we needed a saved variable, we need to emit to emit some code for that
    // This only applies if the argument actually existed (not ignore)
    if ((need_save) && (!p->ignore)) {
      f.code << tab4 << temp << " = " << source << ";\n";
      num_saved++;
    }
    p = l->get_next();
    i++;
  }

  // If there were any saved arguments, emit a local variable for them

  if (num_saved) {
    sprintf(temp,"_saved[%d]",num_saved);
    f.add_local("SV *",temp);
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

  // See if there was a typemap
    
  if ((tm = typemap_lookup("out","perl5",d,iname,"_result","ST(argvi)"))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  } else if ((d->type != T_VOID) || (d->is_pointer)) {
    if (!d->is_pointer) {
      
      // Function returns a "value"
      f.code << tab4 << "ST(argvi) = sv_newmortal();\n";
      switch(d->type) {
      case T_INT: case T_BOOL: case T_SINT: case T_UINT:
      case T_SHORT: case T_SSHORT: case T_USHORT:
      case T_LONG : case T_SLONG : case T_ULONG:
      case T_SCHAR: case T_UCHAR :
	f.code << tab4 << "sv_setiv(ST(argvi++),(IV) _result);\n";
	break;
      case T_DOUBLE :
      case T_FLOAT :
	f.code << tab4 << "sv_setnv(ST(argvi++), (double) _result);\n";
	break;
      case T_CHAR :
	f.add_local("char", "_ctemp[2]");
	f.code << tab4 << "_ctemp[0] = _result;\n"
	       << tab4 << "_ctemp[1] = 0;\n"
	       << tab4 << "sv_setpv((SV*)ST(argvi++),_ctemp);\n";
	break;
	
	// Return a complex type by value
	
      case T_USER:
	d->is_pointer++;
	f.code << tab4 << "sv_setref_pv(ST(argvi++),\"" << d->print_mangle()
	       << "\", (void *) _result);\n";
	d->is_pointer--;
	break;
	
      default :
	fprintf(stderr,"%s: Line %d. Unable to use return type %s in function %s.\n", input_file, line_number, d->print_type(), name);
	break;
      }
    } else {
      
      // Is a pointer return type
      f.code << tab4 << "ST(argvi) = sv_newmortal();\n";
      if ((d->type == T_CHAR) && (d->is_pointer == 1)) {
	
	// Return a character string
	f.code << tab4 << "sv_setpv((SV*)ST(argvi++),(char *) _result);\n";
	
      } else {
	// Is an ordinary pointer type.
	f.code << tab4 << "sv_setref_pv(ST(argvi++),\"" << d->print_mangle()
	       << "\", (void *) _result);\n";
      }
    }
  }

  // If there were any output args, take care of them.
  
  f.code << outarg;

  // If there was any cleanup, do that.

  f.code << cleanup;

  if (NewObject) {
    if ((tm = typemap_lookup("newfree","perl5",d,iname,"_result",""))) {
      f.code << tm << "\n";
    }
  }

  if ((tm = typemap_lookup("ret","perl5",d,iname,"_result",""))) {
      // Yep.  Use it instead of the default
      f.code << tm << "\n";
  }

  // Wrap things up (in a manner of speaking)

  f.code << tab4 << "XSRETURN(argvi);\n}\n";

  // Add the dXSARGS last

  f.add_local("dXSARGS","");

  // Substitute the cleanup code
  f.code.replace("$cleanup",cleanup);
  f.code.replace("$name",iname);

  // Dump this function out

  f.print(f_wrappers);

  // Create a first crack at a documentation entry

  if (doc_entry) {
    static DocEntry *last_doc_entry = 0;
    doc_entry->usage << usage;
    if (last_doc_entry != doc_entry) {
      doc_entry->cinfo << "returns " << d->print_type();
      last_doc_entry = doc_entry;
    }
  }

  // Now register the function

  fprintf(f_init,"\t newXS(\"%s::%s\", %s, file);\n", package, iname, wname);

  if (export_all) {
    exported << iname << " ";
  }


  // --------------------------------------------------------------------
  // Create a stub for this function, provided it's not a member function
  //
  // Really we only need to create a stub if this function involves
  // complex datatypes.   If it does, we'll make a small wrapper to 
  // process the arguments.   If it doesn't, we'll just make a symbol
  // table entry.
  // --------------------------------------------------------------------

  if ((blessed) && (!member_func)) {
    int    need_stub = 0;
    String func;
    
    // We'll make a stub since we may need it anyways

    func << "sub " << iname << " {\n"
	 << tab4 << "my @args = @_;\n";


    // Now we have to go through and patch up the argument list.  If any
    // arguments to our function correspond to other Perl objects, we
    // need to extract them from a tied-hash table object.

    Parm *p = l->get_first();
    int i = 0;
    while(p) {

      if (!p->ignore) {
	// Look up the datatype name here

	if ((classes.lookup(p->t->name)) && (p->t->is_pointer <= 1)) {
	  if (i >= (pcount - numopt))
	    func << tab4 << "if (scalar(@args) >= " << i << ") {\n" << tab4;
	  
	  func << tab4 << "$args[" << i << "] = tied(%{$args[" << i << "]});\n";

	  if (i >= (pcount - numopt))
	    func << tab4 << "}\n";

	  need_stub = 1;
	}
	i++;
      }
      p = l->get_next();
    }

    func << tab4 << "my $result = " << package << "::" << iname << "(@args);\n";

    // Now check to see what kind of return result was found.
    // If this function is returning a result by 'value', SWIG did an 
    // implicit malloc/new.   We'll mark the object like it was created
    // in Perl so we can garbage collect it.

    if ((classes.lookup(d->name)) && (d->is_pointer <=1)) {

      func << tab4 << "return undef if (!defined($result));\n";

      // If we're returning an object by value, put it's reference
      // into our local hash table

      if ((d->is_pointer == 0) || ((d->is_pointer == 1) && NewObject)) {
	func << tab4 << "$" << (char *) classes.lookup(d->name) << "::OWNER{$result} = 1;\n";
      }

      // We're returning a Perl "object" of some kind.  Turn it into
      // a tied hash

      func << tab4 << "my %resulthash;\n"
	/*	   << tab4 << "tie %resulthash, \"" << (char *) classes.lookup(d->name) << "\", $result;\n"
	   << tab4 << "return bless \\%resulthash, \"" << (char *) classes.lookup(d->name) << "\";\n"
	*/
	   << tab4 << "tie %resulthash, ref($result), $result;\n"
	   << tab4 << "return bless \\%resulthash, ref($result);\n"
	   << "}\n";

      need_stub = 1;
    } else {

      // Hmmm.  This doesn't appear to be anything I know about so just 
      // return it unmolested.

      func << tab4 <<"return $result;\n"
	   << "}\n";

    }

    // Now check if we needed the stub.  If so, emit it, otherwise
    // Emit code to hack Perl's symbol table instead

    if (need_stub) {
      func_stubs << func;
    } else {
      func_stubs << "*" << iname << " = *" << package << "::" << iname << ";\n";
    }
  }
}

// -----------------------------------------------------------------------
// PERL5::link_variable(char *name, char *iname, DataType *d)
//
// Create a link to a C variable.
// -----------------------------------------------------------------------

void PERL5::link_variable(char *name, char *iname, DataType *t)
{
  char  set_name[256];
  char  val_name[256];
  WrapperFunction  getf, setf;
  char  *tm;
  sprintf(set_name,"_wrap_set_%s",iname);
  sprintf(val_name,"_wrap_val_%s",iname);

  // Create a new scalar that we will attach magic to

  vinit << tab4 << "sv = perl_get_sv(\"" << package << "::" << iname << "\",TRUE | 0x2);\n";

  // Create a Perl function for setting the variable value

  if (!(Status & STAT_READONLY)) {
    setf.def << "SWIGCLASS_STATIC int " << set_name << "(SV* sv, MAGIC *mg) {\n";

    setf.code << tab4 << "MAGIC_PPERL\n";
    setf.code << tab4 << "mg = mg;\n";

    /* Check for a few typemaps */
    if ((tm = typemap_lookup("varin","perl5",t,"","sv",name))) {
      setf.code << tm << "\n";
    } else if ((tm = typemap_lookup("in","perl5",t,"","sv",name))) {
      setf.code << tm << "\n";
    } else {
      if (!t->is_pointer) {
	
	// Set the value to something 
	
	switch(t->type) {
	case T_INT : case T_BOOL: case T_SINT : case T_UINT:
	case T_SHORT : case T_SSHORT : case T_USHORT:
	case T_LONG : case T_SLONG : case T_ULONG:
	case T_UCHAR: case T_SCHAR:
	  setf.code << tab4 << name << " = " << t->print_cast() << " SvIV(sv);\n";
	  break;
	case T_DOUBLE :
	case T_FLOAT :
	  setf.code << tab4 << name << " = " << t->print_cast() << " SvNV(sv);\n";
	  break;
	case T_CHAR :
	  setf.code << tab4 << name << " = (char) *SvPV(sv,PL_na);\n";
	  break;
	  
	case T_USER:
	  
	  // Add support for User defined type here
	  // Get as a pointer value
	  
	  t->is_pointer++;
	  setf.add_local("void","*_temp");
	  get_pointer(iname,"value","sv","_temp", t, setf.code, "return(1)");
	  setf.code << tab4 << name << " = *(" << t->print_cast() << " _temp);\n";
	  t->is_pointer--;
	  break;
	  
	default :
	  fprintf(stderr,"%s : Line %d.  Unable to link with datatype %s (ignored).\n", input_file, line_number, t->print_type());
	  return;
	}
      } else {
	// Have some sort of pointer type here, Process it differently
	if ((t->type == T_CHAR) && (t->is_pointer == 1)) {
	  setf.add_local("char","*_a");
	  setf.code << tab4 << "_a = (char *) SvPV(sv,PL_na);\n";
	  
	  if (CPlusPlus)
	    setf.code << tab4 << "if (" << name << ") delete [] " << name << ";\n"
		      << tab4 << name << " = new char[strlen(_a)+1];\n";
	  else
	    setf.code << tab4 << "if (" << name << ") free(" << name << ");\n"
		      << tab4 << name << " = (char *) malloc(strlen(_a)+1);\n";
	  setf.code << "strcpy(" << name << ",_a);\n";
	} else {
	  // Set the value of a pointer
	  
	  setf.add_local("void","*_temp");
	  get_pointer(iname,"value","sv","_temp", t, setf.code, "return(1)");
	  setf.code << tab4 << name << " = " << t->print_cast() << " _temp;\n";
	}
      }
    }
    setf.code << tab4 << "return 1;\n"
	      << "}\n";
    
    setf.code.replace("$name",iname);
    setf.print(magic);
    
  }

  // Now write a function to evaluate the variable
  
  getf.def << "SWIGCLASS_STATIC int " << val_name << "(SV *sv, MAGIC *mg) {\n";
  getf.code << tab4 << "MAGIC_PPERL\n";
  getf.code << tab4 << "mg = mg;\n";

  // Check for a typemap
  
  if ((tm = typemap_lookup("varout","perl5",t,"",name, "sv"))) {
    getf.code << tm << "\n";
  } else  if ((tm = typemap_lookup("out","perl5",t,"",name,"sv"))) {
    setf.code << tm << "\n";
  } else {
    if (!t->is_pointer) {
      switch(t->type) {
      case T_INT : case T_BOOL: case T_SINT: case T_UINT:
      case T_SHORT : case T_SSHORT: case T_USHORT:
      case T_LONG : case T_SLONG : case T_ULONG:
      case T_UCHAR: case T_SCHAR:
	getf.code << tab4 << "sv_setiv(sv, (IV) " << name << ");\n";
	vinit << tab4 << "sv_setiv(sv,(IV)" << name << ");\n";
	break;
      case T_DOUBLE :
      case T_FLOAT :
	getf.code << tab4 << "sv_setnv(sv, (double) " << name << ");\n";
	vinit << tab4 << "sv_setnv(sv,(double)" << name << ");\n";
	break;
      case T_CHAR :
	getf.add_local("char","_ptemp[2]");
	getf.code << tab4 << "_ptemp[0] = " << name << ";\n"
		  << tab4 << "_ptemp[1] = 0;\n"
		  << tab4 << "sv_setpv((SV*) sv, _ptemp);\n";
	break;
      case T_USER:
	t->is_pointer++;
	getf.code << tab4 << "rsv = SvRV(sv);\n"
		  << tab4 << "sv_setiv(rsv,(IV) &" << name << ");\n";

	// getf.code << tab4 << "sv_setref_pv((SV*) sv,\"" << t->print_mangle()
	//  << "\", (void *) &" << name << ");\n";

	getf.add_local("SV","*rsv");
	vinit << tab4 << "sv_setref_pv(sv,\"" << t->print_mangle() << "\",(void *) &" << name << ");\n";
	t->is_pointer--;
	
	break;
      default :
	break;
      }
    } else {
      
      // Have some sort of arbitrary pointer type.  Return it as a string
      
      if ((t->type == T_CHAR) && (t->is_pointer == 1))
	getf.code << tab4 << "sv_setpv((SV*) sv, " << name << ");\n";
      else {
	getf.code << tab4 << "rsv = SvRV(sv);\n"
		  << tab4 << "sv_setiv(rsv,(IV) " << name << ");\n";
	getf.add_local("SV","*rsv");
	vinit << tab4 << "sv_setref_pv(sv,\"" << t->print_mangle() << "\",(void *) 1);\n";

	//getf.code << tab4 << "sv_setref_pv((SV*) sv,\"" << t->print_mangle()
	//	  << "\", (void *) " << name << ");\n";
      }
    }
  }
  getf.code << tab4 << "return 1;\n"
	    << "}\n";

  getf.code.replace("$name",iname);
  getf.print(magic);
  
  // Now add symbol to the PERL interpreter
  if (Status & STAT_READONLY) {
    vinit << tab4 << "swig_create_magic(sv,\"" << package << "::" << iname << "\",MAGIC_CAST MAGIC_CLASS swig_magic_readonly, MAGIC_CAST MAGIC_CLASS " << val_name << ");\n";
  } else {
    vinit << tab4 << "swig_create_magic(sv,\"" << package << "::" << iname << "\", MAGIC_CAST MAGIC_CLASS " << set_name << ", MAGIC_CAST MAGIC_CLASS " << val_name << ");\n";
  }      
  // Add a documentation entry
  
  if (doc_entry) {
    doc_entry->usage << usage_var(iname,t);
    doc_entry->cinfo << "Global : " << t->print_type() << " " << name;
  }
  
  // If we're blessed, try to figure out what to do with the variable
  //     1.  If it's a Perl object of some sort, create a tied-hash
  //         around it.
  //     2.  Otherwise, just hack Perl's symbol table
  
  if (blessed) {
    if ((classes.lookup(t->name)) && (t->is_pointer <= 1)) {
      var_stubs << "\nmy %__" << iname << "_hash;\n"
		<< "tie %__" << iname << "_hash,\"" << (char *) classes.lookup(t->name) << "\", $"
		<< package << "::" << iname << ";\n"
		<< "$" << iname << "= \\%__" << iname << "_hash;\n"
		<< "bless $" << iname << ", " << (char *) classes.lookup(t->name) << ";\n";
    } else {
      var_stubs << "*" << iname << " = *" << package << "::" << iname << ";\n";
    }
    if (export_all)
      exported << "$" << name << " ";
  }
}

// -----------------------------------------------------------------------
// PERL5::declare_const(char *name, char *iname, DataType *type, char *value)
//
// Makes a constant.  Really just creates a variable and creates a read-only
// link to it.
// ------------------------------------------------------------------------

// Functions used to create constants

static const char *setiv = "#ifndef PERL_OBJECT\
\n#define swig_setiv(a,b) _swig_setiv(a,b)\
\nstatic void _swig_setiv(char *name, long value) { \
\n#else\
\n#define swig_setiv(a,b) _swig_setiv(pPerl,a,b)\
\nstatic void _swig_setiv(CPerlObj *pPerl, char *name, long value) { \
\n#endif\
\n     SV *sv; \
\n     sv = perl_get_sv(name,TRUE | 0x2);\
\n     sv_setiv(sv, (IV) value);\
\n     SvREADONLY_on(sv);\
\n}\n";

static const char *setnv = "#ifndef PERL_OBJECT\
\n#define swig_setnv(a,b) _swig_setnv(a,b)\
\nstatic void _swig_setnv(char *name, double value) { \
\n#else\
\n#define swig_setnv(a,b) _swig_setnv(pPerl,a,b)\
\nstatic void _swig_setnv(CPerlObj *pPerl, char *name, double value) { \
\n#endif\
\n     SV *sv; \
\n     sv = perl_get_sv(name,TRUE | 0x2);\
\n     sv_setnv(sv, value);\
\n     SvREADONLY_on(sv);\
\n}\n";

static const char *setpv = "#ifndef PERL_OBJECT\
\n#define swig_setpv(a,b) _swig_setpv(a,b)\
\nstatic void _swig_setpv(char *name, char *value) { \
\n#else\
\n#define swig_setpv(a,b) _swig_setpv(pPerl,a,b)\
\nstatic void _swig_setpv(CPerlObj *pPerl, char *name, char *value) { \
\n#endif\
\n     SV *sv; \
\n     sv = perl_get_sv(name,TRUE | 0x2);\
\n     sv_setpv(sv, value);\
\n     SvREADONLY_on(sv);\
\n}\n";

static const char *setrv = "#ifndef PERL_OBJECT\
\n#define swig_setrv(a,b,c) _swig_setrv(a,b,c)\
\nstatic void _swig_setrv(char *name, void *value, char *type) { \
\n#else\
\n#define swig_setrv(a,b,c) _swig_setrv(pPerl,a,b,c)\
\nstatic void _swig_setrv(CPerlObj *pPerl, char *name, void *value, char *type) { \
\n#endif\
\n     SV *sv; \
\n     sv = perl_get_sv(name,TRUE | 0x2);\
\n     sv_setref_pv(sv, type, value);\
\n     SvREADONLY_on(sv);\
\n}\n";

void
PERL5::declare_const(char *name, char *, DataType *type, char *value)
  {

  char   *tm;
  static  int have_int_func = 0;
  static  int have_double_func = 0;
  static  int have_char_func = 0;
  static  int have_ref_func = 0;

  if ((tm = typemap_lookup("const","perl5",type,name,value,name))) {
    fprintf(f_init,"%s\n",tm);
  } else {
    if ((type->type == T_USER) && (!type->is_pointer)) {
      fprintf(stderr,"%s : Line %d.  Unsupported constant value.\n", input_file, line_number);
      return;
    }
    // Generate a constant 
    //    vinit << tab4 << "sv = perl_get_sv(\"" << package << "::" << name << "\",TRUE);\n";	
    if (type->is_pointer == 0) {
      switch(type->type) {
      case T_INT:case T_SINT: case T_UINT: case T_BOOL:
      case T_SHORT: case T_SSHORT: case T_USHORT:
      case T_LONG: case T_SLONG: case T_ULONG:
      case T_SCHAR: case T_UCHAR:
	if (!have_int_func) {
	  fprintf(f_header,"%s\n",setiv);
	  have_int_func = 1;
	}
	vinit << tab4 << "swig_setiv(\"" << package << "::" << name << "\", (long) " << value << ");\n";
	break;
      case T_DOUBLE:
      case T_FLOAT:
	if (!have_double_func) {
	  fprintf(f_header,"%s\n",setnv);
	  have_double_func = 1;
	}
	vinit << tab4 << "swig_setnv(\"" << package << "::" << name << "\", (double) (" << value << "));\n";
	break;
      case T_CHAR :
	if (!have_char_func) {
	  fprintf(f_header,"%s\n",setpv);
	  have_char_func = 1;
	}
	vinit << tab4 << "swig_setpv(\"" << package << "::" << name << "\", \"" << value << "\");\n";
	break;
      default:
	fprintf(stderr,"%s : Line %d. Unsupported constant value.\n", input_file, line_number);
	break;
      }
    } else {
      if ((type->type == T_CHAR) && (type->is_pointer == 1)) {
	if (!have_char_func) {
	  fprintf(f_header,"%s\n",setpv);
	  have_char_func = 1;
	}
	vinit << tab4 << "swig_setpv(\"" << package << "::" << name << "\", \"" << value << "\");\n";
      } else {
	// A user-defined type.  We're going to munge it into a string pointer value
	if (!have_ref_func) {
	  fprintf(f_header,"%s\n",setrv);
	  have_ref_func = 1;
	}
	vinit << tab4 << "swig_setrv(\"" << package << "::" << name << "\", (void *) " << value << ", \"" 
	      << type->print_mangle() << "\");\n";
      }
    }
  }

  // Patch up the documentation entry

  if (doc_entry) {
    doc_entry->usage = "";
    doc_entry->usage << usage_const(name,type,value);
    doc_entry->cinfo = "";
    doc_entry->cinfo << "Constant: " << type->print_type();
  }

  if (blessed) {
    if ((classes.lookup(type->name)) && (type->is_pointer <= 1)) {
      var_stubs << "\nmy %__" << name << "_hash;\n"
		<< "tie %__" << name << "_hash,\"" << (char *) classes.lookup(type->name) << "\", $"
		<< package << "::" << name << ";\n"
		<< "$" << name << "= \\%__" << name << "_hash;\n"
		<< "bless $" << name << ", " << (char *) classes.lookup(type->name) << ";\n";
    } else {
      var_stubs << "*" << name << " = *" << package << "::" << name << ";\n";
    }
  }
  if (export_all)
    exported << "$" << name << " ";
}

// ----------------------------------------------------------------------
// PERL5::usage_var(char *iname, DataType *t)
//
// Produces a usage string for a Perl 5 variable.
// ----------------------------------------------------------------------

char *PERL5::usage_var(char *iname, DataType *) {

  static char temp[1024];
  char *c;

  sprintf(temp,"$%s", iname);
  c = temp + strlen(temp);
  return temp;
}

// ---------------------------------------------------------------------------
// char *PERL5::usage_func(pkg, char *iname, DataType *t, ParmList *l)
// 
// Produces a usage string for a function in Perl
// ---------------------------------------------------------------------------

char *PERL5::usage_func(char *iname, DataType *, ParmList *l) {

  static String temp;
  Parm  *p;
  int    i;

  temp = "";
  temp << iname << "(";
  
  /* Now go through and print parameters */

  p = l->get_first();
  i = 0;
  while (p != 0) {
    if (!p->ignore) {
      /* If parameter has been named, use that.   Otherwise, just print a type  */

      if ((p->t->type != T_VOID) || (p->t->is_pointer)) {
	if (strlen(p->name) > 0) {
	  temp << p->name;
	} else {
	  temp << p->t->print_type();
	}
      }
      i++;
      p = l->get_next();
      if (p)
	if (!p->ignore)
	  temp << ",";
    } else {
      p = l->get_next();
      if (p) 
	if ((i>0) && (!p->ignore))
	  temp << ",";
    }
  }
  temp << ");";
  return temp.get();
}

// ----------------------------------------------------------------------
// PERL5::usage_const(char *iname, DataType *type, char *value)
//
// Produces a usage string for a Perl 5 constant
// ----------------------------------------------------------------------

char *PERL5::usage_const(char *iname, DataType *, char *value) {

  static char temp[1024];
  if (value) {
    sprintf(temp,"$%s = %s", iname, value);
  } else {
    sprintf(temp,"$%s", iname);
  }
  return temp;
}

// -----------------------------------------------------------------------
// PERL5::add_native(char *name, char *funcname)
//
// Add a native module name to Perl5.
// -----------------------------------------------------------------------

void PERL5::add_native(char *name, char *funcname) {
  fprintf(f_init,"\t newXS(\"%s::%s\", %s, file);\n", package,name, funcname);
  if (export_all)
    exported << name << " ";
  if (blessed) {
    func_stubs << "*" << name << " = *" << package << "::" << name << ";\n";
  }
}

/****************************************************************************
 ***                      OBJECT-ORIENTED FEATURES                        
 ****************************************************************************
 *** These extensions provide a more object-oriented interface to C++     
 *** classes and structures.    The code here is based on extensions      
 *** provided by David Fletcher and Gary Holt.
 ***                                                                      
 *** I have generalized these extensions to make them more general purpose   
 *** and to resolve object-ownership problems.                            
 ***
 *** The approach here is very similar to the Python module :             
 ***       1.   All of the original methods are placed into a single      
 ***            package like before except that a 'c' is appended to the  
 ***            package name.                                             
 ***
 ***       2.   All methods and function calls are wrapped with a new     
 ***            perl function.   While possibly inefficient this allows   
 ***            us to catch complex function arguments (which are hard to
 ***            track otherwise).
 ***
 ***       3.   Classes are represented as tied-hashes in a manner similar
 ***            to Gary Holt's extension.   This allows us to access
 ***            member data.
 ***
 ***       4.   Stand-alone (global) C functions are modified to take
 ***            tied hashes as arguments for complex datatypes (if
 ***            appropriate).
 ***
 ***       5.   Global variables involving a class/struct is encapsulated
 ***            in a tied hash.
 ***
 ***       6.   Object ownership is maintained by having a hash table
 ***            within in each package called "this".  It is unlikely
 ***            that C++ program will use this so it's a somewhat 
 ***            safe variable name.
 ***
 ****************************************************************************/

static int class_renamed = 0;
static String fullclassname;

// --------------------------------------------------------------------------
// PERL5::cpp_open_class(char *classname, char *rname, int strip)
//
// Opens a new C++ class or structure.   Basically, this just records
// the class name and clears a few variables.
// --------------------------------------------------------------------------

void PERL5::cpp_open_class(char *classname, char *rname, char *ctype, int strip) {

  char temp[256];
  extern void typeeq_addtypedef(char *, char *);

  // Register this with the default class handler

  this->Language::cpp_open_class(classname, rname, ctype, strip);
  
  if (blessed) {
    have_constructor = 0;
    have_destructor = 0;
    have_data_members = 0;

    // If the class is being renamed to something else, use the renaming

    if (rname) {
      class_name = copy_string(rname);
      class_renamed = 1;
      // Now things get even more hideous.   Need to register an equivalence
      // between the renamed name and the new name. Yuck!
      //      printf("%s %s\n", classname, rname);
        typeeq_addtypedef(classname,rname);
        typeeq_addtypedef(rname,classname);
     /*
      fprintf(f_init,"\t SWIG_RegisterMapping(\"%s\",\"%s\",0);\n",classname,rname);
      fprintf(f_init,"\t SWIG_RegisterMapping(\"%s\",\"%s\",0);\n",rname,classname);
      */
    } else {
      class_name = copy_string(classname);
      class_renamed = 0;
    }

    // A highly experimental feature.  This is the fully qualified
    // name of the Perl class

    if (!compat) {
      fullclassname = realpackage;
      fullclassname << "::" << class_name;
    } else {
      fullclassname = class_name;
    }

    fullclassname = class_name;

    real_classname = copy_string(classname);
    if (base_class) delete base_class;
    base_class =  0;
    class_type = copy_string(ctype);
    pcode = new String();
    blessedmembers = new String();
    member_keys = new String();

    // Add some symbols to the hash tables

    //    classes.add(real_classname,copy_string(class_name));   /* Map original classname to class */
    classes.add(real_classname,copy_string(fullclassname));   /* Map original classname to class */

    // Add full name of datatype to the hash table just in case the user uses it

    sprintf(temp,"%s %s", class_type, fullclassname.get());
    //    classes.add(temp,copy_string(class_name));             /* Map full classname to classs    */
  }
}

// -------------------------------------------------------------------------------
// PERL5::cpp_close_class()
//
// These functions close a class definition.   
//
// This also sets up the hash table of classes we've seen go by.
// -------------------------------------------------------------------------------

void PERL5::cpp_close_class() {

  // We need to check to make sure we got constructors, and other
  // stuff here.

  if (blessed) {
    pm << "\n############# Class : " << fullclassname << " ##############\n";
    pm << "\npackage " << fullclassname << ";\n";

    // If we are inheriting from a base class, set that up

    if (strcmp(class_name,realpackage))
      pm << "@ISA = qw( " << realpackage;
    else 
      pm << "@ISA = qw( ";

    if (base_class) {
      pm << " " << *base_class;
    }
    pm << " );\n";

    // Dump out a hash table containing the pointers that we own

    pm << "%OWNER = ();\n";
    if (have_data_members) {
      pm << "%BLESSEDMEMBERS = (\n"
	 << blessedmembers->get() 
	   << ");\n\n";
    }
    if (have_data_members || have_destructor)
      pm << "%ITERATORS = ();\n";


    // Dump out the package methods

    pm << *pcode;
    delete pcode;

    // Output methods for managing ownership

    pm << "sub DISOWN {\n"
       << tab4 << "my $self = shift;\n"
       << tab4 << "my $ptr = tied(%$self);\n"
       << tab4 << "delete $OWNER{$ptr};\n"
       << tab4 << "};\n\n"
       << "sub ACQUIRE {\n"
       << tab4 << "my $self = shift;\n"
       << tab4 << "my $ptr = tied(%$self);\n"
       << tab4 << "$OWNER{$ptr} = 1;\n"
       << tab4 << "};\n\n";

    // Only output the following methods if a class has member data

    if (have_data_members) {

      // Output a FETCH method.  This is actually common to all classes
      pm << "sub FETCH {\n"
	 << tab4 << "my ($self,$field) = @_;\n"
	 << tab4 << "my $member_func = \"" << package << "::" << name_get(name_member("${field}",class_name,AS_IS),AS_IS) << "\";\n"
	 << tab4 << "my $val = &$member_func($self);\n"
	 << tab4 << "if (exists $BLESSEDMEMBERS{$field}) {\n"
	 << tab8 << "return undef if (!defined($val));\n"
	 << tab8 << "my %retval;\n"
	 << tab8 << "tie %retval,$BLESSEDMEMBERS{$field},$val;\n"
	 << tab8 << "return bless \\%retval, $BLESSEDMEMBERS{$field};\n"
	 << tab4 << "}\n"
	 << tab4 << "return $val;\n"
	 << "}\n\n";
      
      // Output a STORE method.   This is also common to all classes (might move to base class)
      
      pm << "sub STORE {\n"
	 << tab4 << "my ($self,$field,$newval) = @_;\n"
	 << tab4 << "my $member_func = \"" << package << "::" << name_set(name_member("${field}",class_name,AS_IS),AS_IS) << "\";\n"
	 << tab4 << "if (exists $BLESSEDMEMBERS{$field}) {\n"
	 << tab8 << "&$member_func($self,tied(%{$newval}));\n"
	 << tab4 << "} else {\n"
	 << tab8 << "&$member_func($self,$newval);\n"
	 << tab4 << "}\n"
	 << "}\n\n";

      // Output a FIRSTKEY method.   This is to allow iteration over a structure's keys.

      pm << "sub FIRSTKEY {\n"
	 << tab4 << "my $self = shift;\n"
	 << tab4 << "$ITERATORS{$self} = [" << member_keys->get() << "];\n"
	 << tab4 << "my $first = shift @{$ITERATORS{$self}};\n"
	 << tab4 << "return $first;\n"
	 << "}\n\n";

      // Output a NEXTKEY method.   This is the iterator so that each and keys works

      pm << "sub NEXTKEY {\n"
	 << tab4 << "my $self = shift;\n"
	 << tab4 << "$nelem = scalar @{$ITERATORS{$self}};\n"
	 << tab4 << "if ($nelem > 0) {\n"
	 << tab8 << "my $member = shift @{$ITERATORS{$self}};\n"
	 << tab8 << "return $member;\n"
	 << tab4 << "} else {\n"
	 << tab8 << "$ITERATORS{$self} = [" << member_keys->get() << "];\n"
	 << tab8 << "return ();\n"
	 << tab4 << "}\n"
	 << "}\n\n";
    }
  }
}

// --------------------------------------------------------------------------
// PERL5::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l)
//
// Handles a C++ member function.    This basically does the same thing as
// the non-C++ version, but we set up a few status variables that affect
// the function generation function.
//
// --------------------------------------------------------------------------

void PERL5::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l) {

  String  func;
  char    *realname;
  Parm    *p;
  int      i;
  String  cname = "perl5:";
  int      pcount, numopt;

  // First emit normal member function

  member_func = 1;
  this->Language::cpp_member_func(name,iname,t,l);
  member_func = 0;

  if (!blessed) return;

  // Now emit a Perl wrapper function around our member function, we might need
  // to patch up some arguments along the way

  if (!iname)
    realname = name;
  else
    realname = iname;

  cname << class_name << "::" << realname;
  if (add_symbol(cname.get(),0,0)) {
    return;    // Forget it, we saw this function already
  }

  func << "sub " << realname << " {\n"
       << tab4 << "my @args = @_;\n" 
       << tab4 << "$args[0] = tied(%{$args[0]});\n";

  // Now we have to go through and patch up the argument list.  If any
  // arguments to our function correspond to other Perl objects, we
  // need to extract them from a tied-hash table object.

  p = l->get_first();
  pcount = l->nparms;
  numopt = l->numopt();
  i = 1;
  while(p) {
    if (!p->ignore) {
	
      // Look up the datatype name here
      if ((classes.lookup(p->t->name)) && (p->t->is_pointer <= 1)) {
	// Yep.   This smells alot like an object, patch up the arguments

	if (i >= (pcount - numopt))
	  func << tab4 << "if (scalar(@args) >= " << i << ") {\n";

	func << tab4 << "$args[" << i << "] = tied(%{$args[" << i << "]});\n";

	if (i >= (pcount - numopt))
	  func << tab4 << "}\n";
      }
      i++;
    }
    p = l->get_next();
  }
  
  // Okay.  We've made argument adjustments, now call into the package

  func << tab4 << "my $result = " << package << "::" << name_member(realname,class_name)
       << "(@args);\n";
  
  // Now check to see what kind of return result was found.
  // If this function is returning a result by 'value', SWIG did an 
  // implicit malloc/new.   We'll mark the object like it was created
  // in Perl so we can garbage collect it.

  if ((classes.lookup(t->name)) && (t->is_pointer <=1)) {

    func << tab4 << "return undef if (!defined($result));\n";

    // If we're returning an object by value, put it's reference
    // into our local hash table

    if ((t->is_pointer == 0) || ((t->is_pointer == 1) && NewObject)) {
      func << tab4 << "$" << (char *) classes.lookup(t->name) << "::OWNER{$result} = 1;\n";
    }

    // We're returning a Perl "object" of some kind.  Turn it into
    // a tied hash

    func << tab4 << "my %resulthash;\n"
      /*	 << tab4 << "tie %resulthash, \"" << (char *) classes.lookup(t->name) << "\", $result;\n"
		 << tab4 << "return bless \\%resulthash, \"" << (char *) classes.lookup(t->name) << "\";\n" */
	 << tab4 << "tie %resulthash, ref($result), $result;\n"
	 << tab4 << "return bless \\%resulthash, ref($result);\n"

	 << "}\n";

  } else {

    // Hmmm.  This doesn't appear to be anything I know about so just 
    // return it unmolested.

    func << tab4 <<"return $result;\n"
	 << "}\n";

  }

  // Append our function to the pcode segment

  *pcode << func;

  // Create a new kind of documentation entry for the shadow class

  if (doc_entry) {
    doc_entry->usage = "";            // Blow away whatever was there before
    doc_entry->usage << usage_func(realname,t,l);
  }
}

// --------------------------------------------------------------------------------
// PERL5::cpp_variable(char *name, char *iname, DataType *t)
//
// Adds an instance member.   This is a little hairy because data members are
// really added with a tied-hash table that is attached to the object.
//
// On the low level, we will emit a pair of get/set functions to retrieve
// values just like before.    These will then be encapsulated in a FETCH/STORE
// method associated with the tied-hash.
//
// In the event that a member is an object that we have already wrapped, then
// we need to retrieve the data a tied-hash as opposed to what SWIG normally
// returns.   To determine this, we build an internal hash called 'BLESSEDMEMBERS'
// that contains the names and types of tied data members.  If a member name
// is in the list, we tie it, otherwise, we just return the normal SWIG value.
// --------------------------------------------------------------------------------

void PERL5::cpp_variable(char *name, char *iname, DataType *t) {

  char *realname;
  String cname = "perl5:";

  // Emit a pair of get/set functions for the variable

  member_func = 1;
  this->Language::cpp_variable(name, iname, t);  
  member_func = 0;

  if (iname) realname = iname;
  else realname = name;

  if (blessed) {
    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(),0,0)) {
      return;    // Forget it, we saw this already
    }
	
    // Store name of key for future reference

    *member_keys << "'" << realname << "', ";

    // Now we need to generate a little Perl code for this

    if ((classes.lookup(t->name)) && (t->is_pointer <= 1)) {

      // This is a Perl object that we have already seen.  Add an
      // entry to the members list

      *blessedmembers << tab4 << realname << " => '" << (char *) classes.lookup(t->name) << "',\n";
      
     }

    // Patch up the documentation entry

    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << "$this->{" << realname << "}";
    }
  }
  have_data_members++;
}


// -----------------------------------------------------------------------------
// void PERL5::cpp_constructor(char *name, char *iname, ParmList *l)
//
// Emits a blessed constructor for our class.    In addition to our construct
// we manage a Perl hash table containing all of the pointers created by
// the constructor.   This prevents us from accidentally trying to free 
// something that wasn't necessarily allocated by malloc or new
// -----------------------------------------------------------------------------

void PERL5::cpp_constructor(char *name, char *iname, ParmList *l) {
  Parm *p;
  int   i;
  char  *realname;
  String cname="perl5:constructor:";

  // Emit an old-style constructor for this class

  member_func = 1;
  this->Language::cpp_constructor(name, iname, l);

  if (blessed) {

    if (iname) 
      realname = iname;
    else {
      if (class_renamed) realname = class_name;
      else realname = class_name;
    }

    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(),0,0)) {
      return;    // Forget it, we saw this already
    }
    if ((strcmp(realname,class_name) == 0) || ((!iname) && (ObjCClass)) ){
      
      // Emit a blessed constructor 

      *pcode << "sub new {\n";

    } else {
      
      // Constructor doesn't match classname so we'll just use the normal name 

      *pcode << "sub " << name_construct(realname) << " () {\n";
	
    }
    
    *pcode << tab4 << "my $self = shift;\n"
	   << tab4 << "my @args = @_;\n";

    // We are going to need to patch up arguments here if necessary
    // Now we have to go through and patch up the argument list.  If any
    // arguments to our function correspond to other Perl objects, we
    // need to extract them from a tied-hash table object.
    
    p = l->get_first();
    i = 0;
    while(p) {
      
      // Look up the datatype name here
      
      if ((classes.lookup(p->t->name)) && (p->t->is_pointer <= 1)) {
	
	// Yep.   This smells alot like an object, patch up the arguments
	*pcode << tab4 << "$args[" << i << "] = tied(%{$args[" << i << "]});\n";
      }
      p = l->get_next();
      i++;
    }
    
    *pcode << tab4 << "$self = " << package << "::" << name_construct(realname) << "(@args);\n"
	   << tab4 << "return undef if (!defined($self));\n"
	   << tab4 << "bless $self, \"" << fullclassname << "\";\n"
	   << tab4 << "$OWNER{$self} = 1;\n"
	   << tab4 << "my %retval;\n"
	   << tab4 << "tie %retval, \"" << fullclassname << "\", $self;\n"
	   << tab4 << "return bless \\%retval,\"" << fullclassname << "\";\n"
	   << "}\n\n";
    have_constructor = 1;

    // Patch up the documentation entry
    
    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << usage_func("new",0,l);
    }
  }
  member_func = 0;
}


// ------------------------------------------------------------------------------
// void PERL5::cpp_destructor(char *name, char *newname)
//
// Creates a destructor for a blessed object
// ------------------------------------------------------------------------------

void PERL5::cpp_destructor(char *name, char *newname) {

  char *realname;
  member_func = 1;
  this->Language::cpp_destructor(name, newname);

  if (blessed) {
    if (newname) realname = newname;
    else {
      if (class_renamed) realname = class_name;
      else realname = name;
    }

    // Emit a destructor for this object

    *pcode << "sub DESTROY {\n"
	   << tab4 << "my $self = tied(%{$_[0]});\n"
           << tab4 << "delete $ITERATORS{$self};\n"
	   << tab4 << "if (exists $OWNER{$self}) {\n"
	   << tab8 <<  package << "::" << name_destroy(realname) << "($self);\n"
	   << tab8 << "delete $OWNER{$self};\n"
	   << tab4 << "}\n}\n\n";
    
    have_destructor = 1;
    
    if (doc_entry) {
      doc_entry->usage = "DESTROY";
      doc_entry->cinfo = "Destructor";
    }
  }
  member_func = 0;
}
// -----------------------------------------------------------------------------
// void PERL5::cpp_static_func(char *name, char *iname, DataType *t, ParmList *l)
//
// Emits a wrapper for a static class function.   Basically, we just call the
// appropriate method in the module package.
// ------------------------------------------------------------------------------
void PERL5::cpp_static_func(char *name, char *iname, DataType *t, ParmList *l) {
  this->Language::cpp_static_func(name,iname,t,l);
  char *realname;
  if (iname) realname = name;
  else realname = iname;

  if (blessed) {
    *pcode << "*" << realname << " = *" << realpackage << "::" << name_member(realname,class_name) << ";\n";
  }
}
  
// ------------------------------------------------------------------------------
// void PERL5::cpp_inherit(char **baseclass, int mode) 
//
// This sets the Perl5 baseclass (if possible).
// ------------------------------------------------------------------------------

void PERL5::cpp_inherit(char **baseclass, int) {

  char *bc;
  int  i = 0, have_first = 0;
  if (!blessed) {
    this->Language::cpp_inherit(baseclass);
    return;
  }

  // Inherit variables and constants from base classes, but not 
  // functions (since Perl can handle that okay).

  this->Language::cpp_inherit(baseclass, INHERIT_CONST | INHERIT_VAR);

  // Now tell the Perl5 module that we're inheriting from base classes

  base_class = new String;
  while (baseclass[i]) {
    // See if this is a class we know about
    bc = (char *) classes.lookup(baseclass[i]);
    if (bc) {
      if (have_first) *base_class << " ";
      *base_class << bc;
      have_first = 1;
    }
    i++;
  }
  if (!have_first) {
    delete base_class;
    base_class = 0;
  }
}

// --------------------------------------------------------------------------------
// PERL5::cpp_declare_const(char *name, char *iname, DataType *type, char *value)
//
// Add access to a C++ constant.  We can really just do this by hacking
// the symbol table
// --------------------------------------------------------------------------------

void PERL5::cpp_declare_const(char *name, char *iname, DataType *type, char *value) {
  char *realname;
  int   oldblessed = blessed;
  String cname;
  
  // Create a normal constant
  blessed = 0;
  this->Language::cpp_declare_const(name, iname, type, value);
  blessed = oldblessed;

  if (blessed) {
    if (!iname)
      realname = name;
    else
      realname = iname;

    cname << class_name << "::" << realname;
    if (add_symbol(cname.get(),0,0)) {
      return;    // Forget it, we saw this already
    }

    // Create a symbol table entry for it
    *pcode << "*" << realname << " = *" << package << "::" << name_member(realname,class_name) << ";\n";

    // Fix up the documentation entry

    if (doc_entry) {
      doc_entry->usage = "";
      doc_entry->usage << realname;
      if (value) {
	doc_entry->usage << " = " << value;
      }
    }
  }
}

// -----------------------------------------------------------------------
// PERL5::cpp_class_decl(char *name, char *rename, char *type)
//
// Treatment of an empty class definition.    Used to handle
// shadow classes across modules.
// -----------------------------------------------------------------------

void PERL5::cpp_class_decl(char *name, char *rename, char *type) {
    char temp[256];
    if (blessed) {
	classes.add(name,copy_string(rename));
	// Add full name of datatype to the hash table
	if (strlen(type) > 0) {
	  sprintf(temp,"%s %s", type, name);
	  classes.add(temp,copy_string(rename));
	}
    }
}

// --------------------------------------------------------------------------------
// PERL5::add_typedef(DataType *t, char *name)
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

void PERL5::add_typedef(DataType *t, char *name) {

  if (!blessed) return;

  // First check to see if there aren't too many pointers

  if (t->is_pointer > 1) return;

  if (classes.lookup(name)) return;      // Already added

  // Now look up the datatype in our shadow class hash table

  if (classes.lookup(t->name)) {

    // Yep.   This datatype is in the hash
    
    // Put this types 'new' name into the hash

    classes.add(name,copy_string((char *) classes.lookup(t->name)));
  }
}


// --------------------------------------------------------------------------------
// PERL5::pragma(char *, char *, char *)
//
// Pragma directive.
//
// %pragma(perl5) code="String"              # Includes a string in the .pm file
// %pragma(perl5) include="file.pl"          # Includes a file in the .pm file
// 
// --------------------------------------------------------------------------------

void PERL5::pragma(char *lang, char *code, char *value) {
  if (strcmp(lang,"perl5") == 0) {
    if (strcmp(code,"code") == 0) {
      // Dump the value string into the .pm file
      if (value) {
	pragma_include << value << "\n";
      }
    } else if (strcmp(code,"include") == 0) {
      // Include a file into the .pm file
      if (value) {
	if (get_file(value,pragma_include) == -1) {
	  fprintf(stderr,"%s : Line %d. Unable to locate file %s\n", input_file, line_number,value);
	}
      }
    } else {
      fprintf(stderr,"%s : Line %d. Unrecognized pragma.\n", input_file,line_number);
    }
  }
}
