%{
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
 * parser.y
 *
 * YACC parser for parsing function declarations.
 *
 * *** DISCLAIMER ***
 *
 * This is the most ugly, incredibly henious, and completely unintelligible
 * file in SWIG.  While it started out simple, it has grown into a
 * monster that is almost unmaintainable.   A complete parser rewrite is
 * currently in progress that should make this file about 1/4 the size
 * that it is now.   Needless to say, don't modify this file or even look
 * at it for that matter!
 ***********************************************************************/

#define yylex yylex

extern "C" int yylex();
void   yyerror (char *s);       
    
extern int  line_number;
extern int  start_line;
extern void skip_brace(void);
extern void skip_define(void);
extern void skip_decl(void);
extern int  skip_cond(int);
extern void skip_to_end(void);
extern void skip_template(void);
extern void scanner_check_typedef(void);
extern void scanner_ignore_typedef(void);
extern void scanner_clear_start(void);
extern void start_inline(char *, int);
extern void format_string(char *);
extern void swig_pragma(char *, char *);

#include "internal.h"

#ifdef NEED_ALLOC
void *alloca(unsigned n) {
  return((void *) malloc(n));
}
#else
// This redefinition is apparently needed on a number of machines,
// particularly HPUX
#undef  alloca
#define alloca  malloc
#endif

// Initialization flags.   These indicate whether or not certain
// features have been initialized.  These were added to allow
// interface files without the block (required in previous
// versions).

static int     module_init = 0;    /* Indicates whether the %module name was given */
static int     title_init = 0;     /* Indicates whether %title directive has been given */
static int     doc_init = 0;    

static int     lang_init = 0;      /* Indicates if the language has been initialized */

static int            i;
       int            Error = 0;
static char           temp_name[128];
static DataType      *temp_typeptr, temp_type;
static char           yy_rename[256];
static int            Rename_true = 0;
static DataType      *Active_type = 0;         // Used to support variable lists
static int            Active_extern = 0;       // Whether or not list is external
static int            Active_static = 0;
static DataType       *Active_typedef = 0;     // Used for typedef lists
static int            InArray = 0;             // Used when an array declaration is found 
static int            in_then = 0;
static int            in_else = 0;       
static int            allow = 1;               // Used during conditional compilation
static int            doc_scope = 0;           // Documentation scoping
static String         ArrayString;             // Array type attached to parameter names
static String         ArrayBackup;             // Array backup string
static char           *DefArg = 0;             // Default argument hack
static char           *ConstChar = 0;          // Used to store raw character constants
static ParmList       *tm_parm = 0;            // Parameter list used to hold typemap parameters
static Hash            name_hash;              // Hash table containing renamings
       char           *objc_construct = "new"; // Objective-C constructor
       char           *objc_destruct = "free"; // Objective-C destructor

/* Some macros for building constants */

#define E_BINARY(TARGET, SRC1, SRC2, OP)  \
        TARGET = new char[strlen(SRC1) + strlen(SRC2) +strlen(OP)+1];\
	sprintf(TARGET,"%s%s%s",SRC1,OP,SRC2);

/* C++ modes */

#define  CPLUS_PUBLIC    1
#define  CPLUS_PRIVATE   2
#define  CPLUS_PROTECTED 3

int     cplus_mode;

// Declarations of some functions for handling C++ 

extern void cplus_open_class(char *name, char *rname, char *ctype);
extern void cplus_member_func(char *, char *, DataType *, ParmList *, int);
extern void cplus_constructor(char *, char *, ParmList *);
extern void cplus_destructor(char *, char *);
extern void cplus_variable(char *, char *, DataType *);
extern void cplus_static_func(char *, char *, DataType *, ParmList *);
extern void cplus_declare_const(char *, char *, DataType *, char *);
extern void cplus_class_close(char *);
extern void cplus_inherit(int, char **);
extern void cplus_cleanup(void);
extern void cplus_static_var(char *, char *, DataType *);
extern void cplus_register_type(char *);
extern void cplus_register_scope(Hash *);
extern void cplus_inherit_scope(int, char **);
extern void cplus_add_pragma(char *, char *, char *);
extern DocEntry *cplus_set_class(char *);
extern void cplus_unset_class();
extern void cplus_abort();
  
// ----------------------------------------------------------------------
// static init_language()
//
// Initialize the target language.
// Does nothing if this function has already been called.
// ----------------------------------------------------------------------

static void init_language() {
  if (!lang_init) {
    lang->initialize();
    
    // Initialize the documentation system

    if (!doctitle) {
      doctitle = new DocTitle(title,0);
    }
    if (!doc_init)
      doctitle->usage = title;

    doc_stack[0] = doctitle;
    doc_stack_top = 0;
    
    int oldignore = IgnoreDoc;
    IgnoreDoc = 1;
    if (ConfigFile) {
      include_file(ConfigFile);
    }
    IgnoreDoc = oldignore;
  }
  lang_init = 1;
  title_init = 1;
}

// ----------------------------------------------------------------------
// int promote(int t1, int t2)
//
// Promote types (for constant expressions)
// ----------------------------------------------------------------------

int promote(int t1, int t2) {

  if ((t1 == T_ERROR) || (t2 == T_ERROR)) return T_ERROR;
  if ((t1 == T_DOUBLE) || (t2 == T_DOUBLE)) return T_DOUBLE;
  if ((t1 == T_FLOAT) || (t2 == T_FLOAT)) return T_FLOAT;
  if ((t1 == T_ULONG) || (t2 == T_ULONG)) return T_ULONG;
  if ((t1 == T_LONG) || (t2 == T_LONG)) return T_LONG;
  if ((t1 == T_UINT) || (t2 == T_UINT)) return T_UINT;
  if ((t1 == T_INT) || (t2 == T_INT)) return T_INT;
  if ((t1 == T_USHORT) || (t2 == T_USHORT)) return T_SHORT;
  if ((t1 == T_SHORT) || (t2 == T_SHORT)) return T_SHORT;
  if ((t1 == T_UCHAR) || (t2 == T_UCHAR)) return T_UCHAR;
  if (t1 != t2) {
    fprintf(stderr,"%s : Line %d. Type mismatch in constant expression\n",
	    input_file, line_number);
    FatalError();
  }
  return t1;
}

/* Generate the scripting name of an object.  Takes %name directive into 
   account among other things */

static char *make_name(char *name) {
  // Check to see if the name is in the hash
  char *nn = (char *) name_hash.lookup(name);
  if (nn) return nn;        // Yep, return it.

  if (Rename_true) {
    Rename_true = 0;
    return yy_rename;
  } else {
    // Now check to see if the name contains a $
    if (strchr(name,'$')) {
      static String temp;
      temp = "";
      temp << name;
      temp.replace("$","_S_");
      return temp;
    } else {
      return name;
    }
  }
}

/* Return the parent of a documentation entry.   If wrapping externally, this is 0 */

static DocEntry *doc_parent() {
  if (!WrapExtern) 
    return doc_stack[doc_stack_top];
  else
    return 0;
}

// ----------------------------------------------------------------------
// create_function(int ext, char *name, DataType *t, ParmList *l)
//
// Creates a function and manages documentation creation.  Really
// only used internally to the parser.
// ----------------------------------------------------------------------

void create_function(int ext, char *name, DataType *t, ParmList *l) {
  if (Active_static) return;     // Static declaration. Ignore

  init_language();
  if (WrapExtern) return;        // External wrapper file. Ignore

  char *iname = make_name(name);

  // Check if symbol already exists

  if (add_symbol(iname, t, (char *) 0)) {
    fprintf(stderr,"%s : Line %d. Function %s multiply defined (2nd definition ignored).\n",
	    input_file, line_number, iname);
  } else {
    Stat_func++;
    if (Verbose) {
      fprintf(stderr,"Wrapping function : ");
      emit_extern_func(name, t, l, 0, stderr);
    }

    // If extern, make an extern declaration in the SWIG wrapper file

    if (ext) 
      emit_extern_func(name, t, l, ext, f_header);
    else if (ForceExtern) {
      emit_extern_func(name, t, l, 1, f_header);
    }

    // If this function has been declared inline, produce a function

    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
    lang->create_function(name, iname, t, l);
    l->check_defined();
    t->check_defined();
  }
  scanner_clear_start();
}

// -------------------------------------------------------------------
// create_variable(int ext, char *name, DataType *t)
//
// Create a link to a global variable.
// -------------------------------------------------------------------

void create_variable(int ext, char *name, DataType *t) {

  if (WrapExtern) return;        // External wrapper file. Ignore
  int oldstatus = Status;

  if (Active_static) return;  // If static ignore
				   
  init_language();

  char *iname = make_name(name);
  if (add_symbol(iname, t, (char *) 0)) {
    fprintf(stderr,"%s : Line %d. Variable %s multiply defined (2nd definition ignored).\n",
	    input_file, line_number, iname);
  } else {
    Stat_var++;
    if (Verbose) {
      fprintf(stderr,"Wrapping variable : ");
      emit_extern_var(name, t, 0, stderr);
    }

    // If externed, output an external declaration

    if (ext) 
      emit_extern_var(name, t, ext, f_header);
    else if (ForceExtern) {
      emit_extern_var(name, t, 1, f_header);
    }

    // If variable datatype is read-only, we'll force it to be readonly
    if (t->status & STAT_READONLY) Status = Status | STAT_READONLY;

    // Now dump it out
    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
    lang->link_variable(name, iname, t);
    t->check_defined();
    Status = oldstatus;
  }
  scanner_clear_start();
}

// ------------------------------------------------------------------
// create_constant(char *name, DataType *type, char *value)
//
// Creates a new constant.
// -------------------------------------------------------------------

void create_constant(char *name, DataType *type, char *value) {

  if (Active_static) return;
  if (WrapExtern) return;        // External wrapper file. Ignore
  init_language();

  if (Rename_true) {
    fprintf(stderr,"%s : Line %d. %%name directive ignored with #define\n",
	    input_file, line_number);
    Rename_true = 0;
  }

  if ((type->type == T_CHAR) && (!type->is_pointer))
    type->is_pointer++;
  
  if (!value) value = copy_string(name);
  sprintf(temp_name,"const:%s", name);
  if (add_symbol(temp_name, type, value)) {
    fprintf(stderr,"%s : Line %d. Constant %s multiply defined. (2nd definition ignored)\n",
	    input_file, line_number, name);
  } else {
    // Update symbols value if already defined.
    update_symbol(name, type, value);

    if (!WrapExtern) {    // Only wrap the constant if not in %extern mode
      Stat_const++;
      if (Verbose) 
	fprintf(stderr,"Creating constant %s = %s\n", name, value);

      doc_entry = new DocDecl(name,doc_stack[doc_stack_top]);	   
      lang->declare_const(name, name, type, value);
      type->check_defined();
    }
  }
  scanner_clear_start();
}


/* Print out array brackets */
void print_array() {
  int i;
  for (i = 0; i < InArray; i++)
    fprintf(stderr,"[]");
}

/* manipulate small stack for managing if-then-else */

static int then_data[100];
static int else_data[100];
static int allow_data[100];
static int te_index = 0;
static int prev_allow = 1;

void if_push() {
  then_data[te_index] = in_then;
  else_data[te_index] = in_else;
  allow_data[te_index] = allow;
  prev_allow = allow;
  te_index++;
  if (te_index >= 100) {
    fprintf(stderr,"SWIG.  Internal parser error. if-then-else stack overflow.\n");
    SWIG_exit(1);
  }
}

void if_pop() {
  if (te_index > 0) {
    te_index--;
    in_then = then_data[te_index];
    in_else = else_data[te_index];
    allow = allow_data[te_index];
    if (te_index > 0) {
      prev_allow = allow_data[te_index-1];
    } else {
      prev_allow = 1;
    }
  }
}

// Structures for handling code fragments built for nested classes

struct Nested {
  String   code;         // Associated code fragment
  int      line;         // line number where it starts
  char     *name;        // Name associated with this nested class
  DataType *type;        // Datatype associated with the name
  Nested   *next;        // Next code fragment in list
};

// Some internal variables for saving nested class information

static Nested      *nested_list = 0;

// Add a function to the nested list

static void add_nested(Nested *n) {
  Nested *n1;
  if (!nested_list) nested_list = n;
  else {
    n1 = nested_list;
    while (n1->next) n1 = n1->next;
    n1->next = n;
  }
}

// Dump all of the nested class declarations to the inline processor
// However.  We need to do a few name replacements and other munging
// first.  This function must be called before closing a class!

static void dump_nested(char *parent) {
  Nested *n,*n1;
  n = nested_list;
  int oldstatus = Status;

  Status = STAT_READONLY;
  while (n) {
    // Token replace the name of the parent class
    n->code.replace("$classname",parent);

    // Fix up the name of the datatype (for building typedefs and other stuff)
    sprintf(n->type->name,"%s_%s",parent,n->name);
    
    // Add the appropriate declaration to the C++ processor
    doc_entry = new DocDecl(n->name,doc_stack[doc_stack_top]);
    cplus_variable(n->name,(char *) 0, n->type);

    // Dump the code to the scanner
    if (Verbose)
      fprintf(stderr,"Splitting from %s : (line %d) \n%s\n", parent,n->line, n->code.get());

    fprintf(f_header,"\n%s\n", n->code.get());
    start_inline(n->code.get(),n->line);

    n1 = n->next;
    delete n;
    n = n1;
  }
  nested_list = 0;
  Status = oldstatus;
}    

%}

/* The type of each node in the parse tree
   must be one of the elements of the union
   given below.  This is used to derive the
   C++ declaration for "yylval" that appears
   in parser.tab.h. */

%union {         
  char        *id;
  struct Declaration {
    char *id;
    int   is_pointer;
    int   is_reference;
  } decl;
  struct InitList {
    char **names;
    int    count;
  } ilist;
  struct DocList {
    char **names;
    char **values;
    int  count;
  } dlist;
  struct Define {
    char *id;
    int   type;
  } dtype;
  DataType     *type;
  Parm         *p;
  TMParm       *tmparm;
  ParmList     *pl;
  int           ivalue;
};

%token <id> ID 
%token <id> HBLOCK WRAPPER POUND 
%token <id> STRING
%token <id> NUM_INT NUM_FLOAT CHARCONST NUM_UNSIGNED NUM_LONG NUM_ULONG
%token <ivalue> TYPEDEF
%token <type> TYPE_INT TYPE_UNSIGNED TYPE_SHORT TYPE_LONG TYPE_FLOAT TYPE_DOUBLE TYPE_CHAR TYPE_VOID TYPE_SIGNED TYPE_BOOL TYPE_TYPEDEF
%token LPAREN RPAREN COMMA SEMI EXTERN INIT LBRACE RBRACE DEFINE PERIOD
%token CONST STRUCT UNION EQUAL SIZEOF MODULE LBRACKET RBRACKET
%token WEXTERN ILLEGAL
%token READONLY READWRITE NAME RENAME INCLUDE CHECKOUT ADDMETHODS PRAGMA 
%token CVALUE COUT
%token ENUM ENDDEF MACRO 
%token CLASS PRIVATE PUBLIC PROTECTED COLON STATIC VIRTUAL FRIEND OPERATOR THROW TEMPLATE
%token NATIVE INLINE
%token IFDEF IFNDEF ENDIF ELSE UNDEF IF DEFINED ELIF
%token RAW_MODE ALPHA_MODE TEXT DOC_DISABLE DOC_ENABLE STYLE LOCALSTYLE
%token TYPEMAP EXCEPT IMPORT ECHO NEW APPLY CLEAR DOCONLY
%token <ivalue> TITLE SECTION SUBSECTION SUBSUBSECTION 
%token LESSTHAN GREATERTHAN
%token <id> USERDIRECTIVE

/* Objective C tokens */

%token OC_INTERFACE OC_END OC_PUBLIC OC_PRIVATE OC_PROTECTED OC_CLASS OC_IMPLEMENT OC_PROTOCOL

%left  OR
%left  XOR
%left  AND
%left  LSHIFT RSHIFT 
%left  PLUS MINUS 
%left  STAR SLASH
%left  UMINUS NOT LNOT
%left  DCOLON

%type <ivalue>   extern array array2 parm_specifier parm_specifier_list;
%type <pl>       parms ptail;
%type <p>        parm parm_type;
%type <tmparm>   typemap_parm tm_list tm_tail;
%type <id>       pname cpptype base_specifier access_specifier typemap_name tm_method idstring;
%type <type>     type opt_signed opt_unsigned strict_type;
%type <decl>     declaration nested_decl;
%type <ivalue>   stars cpp_const_expr;
%type <ilist>    initlist base_list inherit;
%type <dtype>    definetype definetail def_args;
%type <dtype>    etype;
%type <dtype>    expr;
%type <id>       ename stylearg objc_inherit;
%type <dlist>    stylelist styletail;
%type <type>     objc_ret_type objc_arg_type; 
%type <id>       objc_protolist objc_separator;
%type <pl>       objc_args;

%%

/* The productions of the grammar with their
   associated semantic actions. */

program        : { 
                    {
		      int ii;
		      for (ii = 0; ii < 256; ii++) {
			handler_stack[ii] = 0;
		      }
		      handler_stack[0] = comment_handler;
		    }
                    doc_stack[0] = doctitle;
                 } command {
		   CommentHandler::cleanup();
                   cplus_cleanup();
		   doc_entry = doctitle;
		   if (lang_init) {
		     lang->close();
		   }
		   if (te_index) {
		     fprintf(stderr,"%s : EOF.  Missing #endif detected.\n", input_file);
		     FatalError();
		   }
               }
               ;
    
command        : command statement { 
		     scanner_clear_start();
                     Error = 0;
                }
               | empty {
	       }
               ;

statement      : INCLUDE idstring {
                  if (allow) {
//		    init_language();
		    doc_entry = 0;
		    // comment_handler->clear();
		    include_file($2);
		  }
                }

/* %extern directive */

               | WEXTERN idstring {
		 if (allow) {
		   int oldextern = WrapExtern;
//		   init_language();
		   doc_entry = 0;
		   // comment_handler->clear();
		   WrapExtern = 1;
		   if (include_file($2) >= 0) {
		     add_symbol("SWIGEXTERN",0,0);
		   } else {
		     WrapExtern = oldextern;
		   }
		 }
	       }

/* %import directive.  Like %extern but calls out to a language module */

                | IMPORT idstring {
		  if (allow) {
		    int oldextern = WrapExtern;
		    init_language();
		    doc_entry = 0;
		    WrapExtern = 1;
		    if (include_file($2) >= 0) {
		      add_symbol("SWIGEXTERN",0,0);
		      lang->import($2);
		    } else {
		      WrapExtern = oldextern;
		    }
		  }
                }

/* %checkout directive.  Like %include, but simply copies the file into the
   current directory */

                | CHECKOUT idstring {
                  if (allow) {
                     if ((checkout_file($2,$2)) == 0) {
                       fprintf(stderr,"%s checked out from the SWIG library.\n",$2);
                      }
                  }
                }

/* An unknown C preprocessor statement.  Just throw it away */

                | POUND {
		 if (allow) {
                  doc_entry = 0;
		  if (Verbose) {
		    fprintf(stderr,"%s : Line %d.  CPP %s ignored.\n", input_file, line_number,$1);
		  }
		 }
		}

/* A variable declaration */

                | extern type declaration array2 def_args {
		  if (allow) {
		    init_language();
		    if (Active_type) delete Active_type;
		    Active_type = new DataType($2);
		    Active_extern = $1;
		    $2->is_pointer += $3.is_pointer;
		    if ($4 > 0) {
		      $2->is_pointer++;
		      $2->status = STAT_READONLY;
                      $2->arraystr = copy_string(ArrayString);
		    }
		    if ($3.is_reference) {
		      fprintf(stderr,"%s : Line %d. Error. Linkage to C++ reference not allowed.\n", input_file, line_number);
		      FatalError();
		    } else {
		      if ($2->qualifier) {
			if ((strcmp($2->qualifier,"const") == 0)) {
			  if ($5.type != T_ERROR)
			    create_constant($3.id, $2, $5.id);
			} else 
			  create_variable($1,$3.id,$2);
		      } else
			create_variable($1,$3.id,$2);
		    }
		  }
		  delete $2;
                } stail { } 

/* Global variable that smells like a function pointer */

                | extern strict_type LPAREN STAR { 
                   skip_decl();
		   fprintf(stderr,"%s : Line %d. Function pointers not currently supported.\n",
			   input_file, line_number);
		}

/* A static variable declaration (Ignored) */

   	        | STATIC type declaration array2 def_args {
		  if (Verbose) {
		    fprintf(stderr,"static variable %s ignored.\n",$3.id);
		  }
		  Active_static = 1;
		  delete $2;
		} stail {
		  Active_static = 0;
		}

/* Global variable that smells like a function pointer */

                | STATIC strict_type LPAREN STAR { 
                   skip_decl();
		   fprintf(stderr,"%s : Line %d. Function pointers not currently supported.\n",
			   input_file, line_number);
		}


/* A function declaration */

                | extern type declaration LPAREN parms RPAREN cpp_const {
		  if (allow) {
		    init_language();
		    if (Active_type) delete Active_type;
		    Active_type = new DataType($2);
		    Active_extern = $1;
		    $2->is_pointer += $3.is_pointer;
		    $2->is_reference = $3.is_reference;
		    create_function($1, $3.id, $2, $5);
		  }
		  delete $2;
		  delete $5;
		} stail { } 
               
/* A function declaration with code after it */

                | extern type declaration LPAREN parms RPAREN func_end {
		  if (allow) {
		    init_language();
		    $2->is_pointer += $3.is_pointer;
		    $2->is_reference = $3.is_reference;
		    create_function($1, $3.id, $2, $5);
		  }
		  delete $2;
		  delete $5;
		};

/* A function declared without any return datatype */

                | extern declaration LPAREN parms RPAREN cpp_const { 
		  if (allow) {
                    init_language();
		    DataType *t = new DataType(T_INT);
                    t->is_pointer += $2.is_pointer;
		    t->is_reference = $2.is_reference;
		    create_function($1,$2.id,t,$4);
		    delete t;
		  }
                } stail { };

/* A static function declaration code after it */

                | STATIC type declaration LPAREN parms RPAREN func_end {
		  if ((allow) && (Inline)) {
		    if (strlen(CCode.get())) {
		      init_language();
		      $2->is_pointer += $3.is_pointer;
		      $2->is_reference = $3.is_reference;
		      create_function(0, $3.id, $2, $5);
		    }
		  }
		  delete $2;
		  delete $5;
		};

/* A function with an explicit inline directive. Not safe to use inside a %inline block */

                | INLINE type declaration LPAREN parms RPAREN func_end {
		  if (allow) {
		    init_language();
		    $2->is_pointer += $3.is_pointer;
		    $2->is_reference = $3.is_reference;
		    if (Inline) {
		      fprintf(stderr,"%s : Line %d. Repeated %%inline directive.\n",input_file,line_number);
		      FatalError();
		    } else {
		      if (strlen(CCode.get())) {
			fprintf(f_header,"static ");
			emit_extern_func($3.id,$2,$5,3,f_header);
			fprintf(f_header,"%s\n",CCode.get());
		      }
		      create_function(0, $3.id, $2, $5);
		    }
		  }
		  delete $2;
		  delete $5;
		};

/* A static function declaration (ignored) */

                | STATIC type declaration LPAREN parms RPAREN cpp_const {
		  if (allow) {
		    if (Verbose) {
		      fprintf(stderr,"static function %s ignored.\n", $3.id);
		    }
		  }
		  Active_static = 1;
		  delete $2;
		  delete $5;
		} stail {
		  Active_static = 0;
		 }

/* Enable Read-only mode */

               | READONLY {
		  if (allow)
		    Status = Status | STAT_READONLY;
	       }

/* Enable Read-write mode */

	       | READWRITE {
		 if (allow)
		   Status = Status & ~STAT_READONLY;
	       }

/* New %name directive */
               | NAME LPAREN ID RPAREN {
		 if (allow) {
                     strcpy(yy_rename,$3);
                     Rename_true = 1;
		 }
               }

/* %rename directive */
               | RENAME ID ID SEMI { 
		 if (name_hash.lookup($2)) {
		   name_hash.remove($2);
		 }
		 name_hash.add($2,copy_string($3));
	       }
          
/* %new directive */

               | NEW {
                     NewObject = 1;
                } statement {
                     NewObject = 0;
               }

/* Empty name directive.  No longer allowed  */

               | NAME LPAREN RPAREN {
		 if (allow) {
		   fprintf(stderr,"%s : Lind %d. Empty %%name() is no longer supported.\n",
			   input_file, line_number);
		   FatalError();
		 }
	       } cpp {
		 Rename_true = 0;
	       }

/* A native wrapper function */

               | NATIVE LPAREN ID RPAREN extern ID SEMI {
		 if (allow && (!WrapExtern)) {
		   init_language();
		   if (add_symbol($3,(DataType *) 0, (char *) 0)) {
		     fprintf(stderr,"%s : Line %d. Name of native function %s conflicts with previous declaration (ignored)\n",
			     input_file, line_number, $3);
		   } else {
		     doc_entry = new DocDecl($3,doc_stack[doc_stack_top]);
		     lang->add_native($3,$6);
		   }
		 }
	       }
               | NATIVE LPAREN ID RPAREN extern type declaration LPAREN parms RPAREN SEMI {
		 if (allow && (!WrapExtern)) {
		   init_language();
		   $6->is_pointer += $7.is_pointer;
		   if (add_symbol($3,(DataType *) 0, (char *) 0)) {
		     fprintf(stderr,"%s : Line %d. Name of native function %s conflicts with previous declaration (ignored)\n",
			     input_file, line_number, $3);
		   } else {
		     if ($5) {
		       emit_extern_func($7.id, $6, $9, $5, f_header);
		     }
		     doc_entry = new DocDecl($3,doc_stack[doc_stack_top]);
		     lang->add_native($3,$7.id);
		   }
		 }
		 delete $6;
		 delete $9;
	       }

/* %title directive */

               | TITLE STRING styletail {
		 if (allow && (!WrapExtern)) {
		   if (!title_init) {
		     title_init = 1;
		     doc_init = 1;
		     if (!comment_handler) {
		       comment_handler = new CommentHandler();
		     }
		     { 
		       int ii;
		       for (ii = 0; ii < $3.count; ii++) {
			 comment_handler->style($3.names[ii],$3.values[ii]);
		       }
		     }
		     // Create a new title for documentation 
		     {
		       int temp = line_number;
		       line_number = $1;
		       if (!doctitle)
			 doctitle = new DocTitle($2,0);
		       else {
			 doctitle->name = copy_string(title);
			 doctitle->line_number = $1;
			 doctitle->end_line = $1;
		       }
		       line_number = temp;
		     }
		     doctitle->usage = $2;
		     doc_entry = doctitle;
		     doc_stack[0] = doc_entry;
		     doc_stack_top = 0;
		     handler_stack[0] = comment_handler;
		     { 
		       int ii;
		       for (ii = 0; ii < $3.count; ii++) {
			 doc_stack[doc_stack_top]->style($3.names[ii],$3.values[ii]);
		       }
		     }

		   } else {
		     // Ignore it
		   }
		 }
	       } 


/* %section directive */

               | SECTION STRING styletail {
		 if (allow && (!WrapExtern) && (!IgnoreDoc)) {
		   // Copy old comment handler
		   // if (handler_stack[1]) delete handler_stack[1];
		   handler_stack[1] = new CommentHandler(handler_stack[0]);  
		   comment_handler = handler_stack[1];
		   { 
		     int ii;
		     for (ii = 0; ii < $3.count; ii++) {
		       comment_handler->style($3.names[ii],$3.values[ii]);
		     }
		   }
		   {
		     int temp = line_number;
		     line_number = $1;
		     doc_entry = new DocSection($2,doc_stack[0]);
		     line_number = temp;
		   }
		   doc_stack_top = 1;
		   doc_stack[1] = doc_entry;
		   { 
		     int ii;
		     for (ii = 0; ii < $3.count; ii++) {
		       doc_stack[doc_stack_top]->style($3.names[ii],$3.values[ii]);
		     }
		   }
		 }
	       }

/* %subsection directive */
               | SUBSECTION STRING styletail {
		 if (allow && (!WrapExtern) && (!IgnoreDoc)) {
		   if (doc_stack_top < 1) {
		     fprintf(stderr,"%s : Line %d. Can't apply %%subsection here.\n", input_file,line_number);
		     FatalError();
		   } else {

		     // Copy old comment handler
		     // if (handler_stack[2]) delete handler_stack[2];
		     handler_stack[2] = new CommentHandler(handler_stack[1]);
		     comment_handler = handler_stack[2];
		     { 
		       int ii;
		       for (ii = 0; ii < $3.count; ii++) {
			 comment_handler->style($3.names[ii],$3.values[ii]);
		       }
		     }
		     {
		       int temp = line_number;
		       line_number = $1;
		       doc_entry = new DocSection($2,doc_stack[1]);
		       line_number = temp;
		     }
		     doc_stack_top = 2;
		     doc_stack[2] = doc_entry;
		     { 
		       int ii;
		       for (ii = 0; ii < $3.count; ii++) {
			 doc_stack[doc_stack_top]->style($3.names[ii],$3.values[ii]);
		       }
		     }
		   }
		 }
	       }

/* %subsubsection directive */
               | SUBSUBSECTION STRING styletail {
		 if (allow && (!WrapExtern) && (!IgnoreDoc)) {
		   if (doc_stack_top < 2) {
		     fprintf(stderr,"%s : Line %d. Can't apply %%subsubsection here.\n", input_file,line_number);
		     FatalError();
		   } else {

		     // Copy old comment handler

		     // if (handler_stack[3]) delete handler_stack[3];
		     handler_stack[3] = new CommentHandler(handler_stack[2]);
		     comment_handler = handler_stack[3];
		     { 
		       int ii;
		       for (ii = 0; ii < $3.count; ii++) {
			 comment_handler->style($3.names[ii],$3.values[ii]);
		       }
		     }
		     {
		       int temp = line_number;
		       line_number = $1;
		       doc_entry = new DocSection($2,doc_stack[2]);
		       line_number = temp;
		     }
		     doc_stack_top = 3;
		     doc_stack[3] = doc_entry;
		     { 
		       int ii;
		       for (ii = 0; ii < $3.count; ii++) {
			 doc_stack[doc_stack_top]->style($3.names[ii],$3.values[ii]);
		       }
		     }
		   }
		 }
	       }

/* %alpha directive (obsolete) */
               | ALPHA_MODE {
		 if (allow && (!WrapExtern)) {
		   fprintf(stderr,"%%alpha directive is obsolete.  Use '%%style sort' instead.\n");
		   handler_stack[0]->style("sort",0);
		   doc_stack[0]->style("sort",0);
		 }
	       }
/* %raw directive (obsolete) */
               | RAW_MODE {
		 if (allow && (!WrapExtern)) {
		   fprintf(stderr,"%%raw directive is obsolete. Use '%%style nosort' instead.\n");
		   handler_stack[0]->style("nosort",0);
		   doc_stack[0]->style("nosort",0);
		 }
	       }

               | doc_enable { }

/* %text directive */

               | TEXT HBLOCK {
		 if (allow && (!WrapExtern)) {
		   $2[strlen($2) - 1] = 0;
		   doc_entry = new DocText($2,doc_stack[doc_stack_top]);
		   doc_entry = 0;
		 }
	       }
             

               | typedef_decl { }

/* Code insertion block */

               | HBLOCK {
		 if (allow && (!WrapExtern)) {
		   init_language();
		   $1[strlen($1) - 1] = 0;
//		   fprintf(f_header,"#line %d \"%s\"\n", start_line, input_file);
		   fprintf(f_header, "%s\n", $1);
		 }
	       }

/* Super-secret undocumented for people who really know what's going on feature */

               | WRAPPER HBLOCK {
                 if (allow && (!WrapExtern)) {
		   init_language();
		   $2[strlen($2) - 1] = 0;
		   fprintf(f_wrappers,"%s\n",$2);
		 }
	       }

/* Initialization code */

               | INIT HBLOCK {
		 if (allow && (!WrapExtern)) {
		   init_language();
		   $2[strlen($2) -1] = 0;
		   fprintf(f_init,"%s\n", $2);
		 }
	       }

/* Inline block */
               | INLINE HBLOCK {
		 if (allow && (!WrapExtern)) {
		   init_language();
		   $2[strlen($2) - 1] = 0;
		   fprintf(f_header, "%s\n", $2);
		   start_inline($2,start_line);
		 }
	       }

/* Echo mode */
               | ECHO HBLOCK {
		 if (allow && (!WrapExtern)) {
		   fprintf(stderr,"%s\n", $2);
		 }
	       }

               | ECHO STRING {
                 if (allow && (!WrapExtern)) {
                   fprintf(stderr,"%s\n", $2);
                 }
               }

/* Disable code generation */
               | DOCONLY {
                   DocOnly = 1;
               }
       
/* Init directive--to avoid errors in other modules */

               | INIT ID initlist {
		 if (allow) {
		   if (!module_init) {
		     lang->set_init($2);
		     module_init = 1;
		     init_language();
		   } else {
		     if (Verbose)
		       fprintf(stderr,"%s : Line %d. %%init %s ignored.\n",
			       input_file, line_number, $2);
		   }
		   if ($3.count > 0) {
		     fprintf(stderr,"%s : Line %d. Warning. Init list no longer supported.\n",
			     input_file,line_number);
		   }
		 }
		 for (i = 0; i < $3.count; i++)
		   if ($3.names[i]) delete [] $3.names[i];
		 delete [] $3.names;
	       }
/* Module directive */

               | MODULE ID initlist {
		 if (allow) {
		   if ($3.count)
		     lang->set_module($2,$3.names);
		   else
		     lang->set_module($2,0);
		   module_init = 1;
		   init_language();
		 }
		 for (i = 0; i < $3.count; i++)
		   if ($3.names[i]) delete [] $3.names[i];
		 delete [] $3.names;
	       }

/* #define directive */

               | DEFINE ID definetail {
		 if (allow) {
		   if (($3.type != T_ERROR) && ($3.type != T_SYMBOL)) {
		     init_language();
		     temp_typeptr = new DataType($3.type);
		     create_constant($2, temp_typeptr, $3.id);
		     delete temp_typeptr;
		   } else if ($3.type == T_SYMBOL) {
		     // Add a symbol to the SWIG symbol table
		     if (add_symbol($2,(DataType *) 0, (char *) 0)) {
		       fprintf(stderr,"%s : Line %d. Warning. Symbol %s already defined.\n", 
			       input_file,line_number, $2);
		     }
		   }
		 }
	       }

/* A CPP Macro.   Ignore (hopefully) */

               | DEFINE MACRO {
		 if (Verbose) {
		   fprintf(stderr,"%s : Line %d.  CPP Macro ignored.\n", input_file, line_number);
		 }
	       }

/* An undef directive */
               | UNDEF ID {
		 remove_symbol($2);
	       }

/* Enumerations */

               | extern ENUM ename LBRACE { scanner_clear_start(); } enumlist RBRACE SEMI { 
		 if (allow) {
		   init_language();
		   if ($3) {
		     temp_type.type = T_INT;
		     temp_type.is_pointer = 0;
		     temp_type.implicit_ptr = 0;
		     sprintf(temp_type.name,"int");
		     temp_type.typedef_add($3,1);
		   }
		 }
	       }

/* A typdef'd enum.  Pretty common in C headers */

               | TYPEDEF ENUM ename LBRACE { scanner_clear_start(); } enumlist RBRACE ID {
		 if (allow) {
		   init_language();
		   temp_type.type = T_INT;
		   temp_type.is_pointer = 0;
		   temp_type.implicit_ptr = 0;
		   sprintf(temp_type.name,"int");
		   Active_typedef = new DataType(&temp_type);
		   temp_type.typedef_add($8,1);
		 }
	       } typedeflist { }

/* -----------------------------------------------------------------
   typemap support.

   These constructs are used to support type-maps.
   ----------------------------------------------------------------- */

/* Create a new typemap */

               | TYPEMAP LPAREN ID COMMA tm_method RPAREN tm_list LBRACE {
		   TMParm *p;
                   skip_brace();
		   p = $7;
		   while (p) {
		     typemap_register($5,$3,p->p->t,p->p->name,CCode,p->args);
		     p = p->next;
                   }
		   delete $3;
		   delete $5;
	       }

/* Create a new typemap in current language */
               | TYPEMAP LPAREN tm_method RPAREN tm_list LBRACE {
		 if (!typemap_lang) {
		   fprintf(stderr,"SWIG internal error. No typemap_lang specified.\n");
		   fprintf(stderr,"typemap on %s : Line %d. will be ignored.\n",input_file,line_number);
		   FatalError();
		 } else {
		   TMParm *p;
		   skip_brace();
		   p = $5;
		   while (p) {
		     typemap_register($3,typemap_lang,p->p->t,p->p->name,CCode,p->args);
		     p = p->next;
		   }
		 }
		 delete $3;
	       }

/* Clear a typemap */

               | TYPEMAP LPAREN ID COMMA tm_method RPAREN tm_list SEMI {
		 TMParm *p;
		 p = $7;
		 while (p) {
                   typemap_clear($5,$3,p->p->t,p->p->name);
		   p = p->next;
		 }
		 delete $3;
		 delete $5;
	       }
/* Clear a typemap in current language */

               | TYPEMAP LPAREN tm_method RPAREN tm_list SEMI {
		 if (!typemap_lang) {
		   fprintf(stderr,"SWIG internal error. No typemap_lang specified.\n");
		   fprintf(stderr,"typemap on %s : Line %d. will be ignored.\n",input_file,line_number);
		   FatalError();
		 } else {
		   TMParm *p;
		   p = $5;
		   while (p) {
		     typemap_clear($3,typemap_lang,p->p->t,p->p->name);
		     p = p->next;
		   }
		 }
		 delete $3;
	       }

/* Copy a typemap */

               | TYPEMAP LPAREN ID COMMA tm_method RPAREN tm_list EQUAL typemap_parm SEMI {
                   TMParm *p;
		   p = $7;
		   while (p) {
		     typemap_copy($5,$3,$9->p->t,$9->p->name,p->p->t,p->p->name);
		     p = p->next;
		   }
		   delete $3;
		   delete $5;
		   delete $9->p;
		   delete $9;
	       }

/* Copy typemap in current language */

               | TYPEMAP LPAREN tm_method RPAREN tm_list EQUAL typemap_parm SEMI {
		 if (!typemap_lang) {
		   fprintf(stderr,"SWIG internal error. No typemap_lang specified.\n");
		   fprintf(stderr,"typemap on %s : Line %d. will be ignored.\n",input_file,line_number);
		   FatalError();
		 } else {
                   TMParm *p;
		   p = $5;
		   while (p) {
		     typemap_copy($3,typemap_lang,$7->p->t,$7->p->name,p->p->t,p->p->name);
		     p = p->next;
		   }
		 }
		 delete $3;
		 delete $7->p;
		 delete $7;
	       }
/* -----------------------------------------------------------------
   apply and clear support (for typemaps)
   ----------------------------------------------------------------- */

               | APPLY typemap_parm LBRACE tm_list RBRACE {
		 TMParm *p;
		 p = $4;
		 while(p) {
		   typemap_apply($2->p->t,$2->p->name,p->p->t,p->p->name);
		   p = p->next;
		 }
		 delete $4;
		 delete $2->args;
		 delete $2;
               }
	       | CLEAR tm_list SEMI {
		 TMParm *p;
		 p = $2;
		 while (p) {
		   typemap_clear_apply(p->p->t, p->p->name);
		   p = p->next;
		 }
               }


/* -----------------------------------------------------------------
   exception support

   These constructs are used to define exceptions
   ----------------------------------------------------------------- */

/* An exception definition */
               | EXCEPT LPAREN ID RPAREN LBRACE {
                    skip_brace();
                    fragment_register("except",$3, CCode);
		    delete $3;
	       }

/* A Generic Exception (no language specified */
               | EXCEPT LBRACE {
                    skip_brace();
                    fragment_register("except",typemap_lang, CCode);
               }

/* Clear an exception */

               | EXCEPT LPAREN ID RPAREN SEMI {
                     fragment_clear("except",$3);
               }

/* Generic clear */
               | EXCEPT SEMI {
                     fragment_clear("except",typemap_lang);
	       }
   
/* Miscellaenous stuff */

               | SEMI { }
               | cpp { }
               | objective_c { }
               | error {
		 if (!Error) {
		   {
		     static int last_error_line = -1;
		     if (last_error_line != line_number) {
		       fprintf(stderr,"%s : Line %d. Syntax error in input.\n", input_file, line_number);
		       FatalError();
		       last_error_line = line_number;
                       // Try to make some kind of recovery.
		       skip_decl();
		     }
		     Error = 1;
		   }
		 }
	       }

/* A an extern C type declaration.  Does nothing, but is ignored */

               | EXTERN STRING LBRACE command RBRACE { }
               | cond_compile { }

/* Officially, this directive doesn't exist yet */

               | pragma { }

/* %style directive.   This applies to all current styles */

               | STYLE stylelist {
		 { 
		   int ii,jj;
		   for (ii = 0; ii < $2.count; ii++) {
		     comment_handler->style($2.names[ii],$2.values[ii]);
		     for (jj = 0; jj < doc_stack_top; jj++) 
		       doc_stack[jj]->style($2.names[ii],$2.values[ii]);
		     if (doctitle)
		       doctitle->style($2.names[ii],$2.values[ii]);
		     doc->style($2.names[ii],$2.values[ii]);
		   }
		 }
	       }

/* %localstyle directive.   This applies only to the current style */

               | LOCALSTYLE stylelist {
		 { 
		   int ii;
		   for (ii = 0; ii < $2.count; ii++) {
		     comment_handler = new CommentHandler(comment_handler);
		     handler_stack[doc_stack_top] = comment_handler;
		     comment_handler->style($2.names[ii],$2.values[ii]);
		     doc_stack[doc_stack_top]->style($2.names[ii],$2.values[ii]);
		   }
		 }
	       }

/* User defined directive */
               | user_directive{ }
               ;


/* Dcumentation disable/enable */

doc_enable     : DOC_DISABLE {
		 if (allow) {
		   if (IgnoreDoc) {
		     /* Already in a disabled documentation */
		     doc_scope++;
		   } else {
		     if (Verbose)
		       fprintf(stderr,"%s : Line %d. Documentation disabled.\n", input_file, line_number);
		     IgnoreDoc = 1;
		     doc_scope = 1;
		   }
		 }
	       }
/* %enabledoc directive */
               | DOC_ENABLE {
		 if (allow) {
		   if (IgnoreDoc) {
		     if (doc_scope > 1) {
		       doc_scope--;
		     } else {
		       if (Verbose)
			 fprintf(stderr,"%s : Line %d. Documentation enabled.\n", input_file, line_number);
		       IgnoreDoc = 0;
		       doc_scope = 0;
		     }
		   }
		 }
	       }
               ;

/* Note : This really needs to be re-done */

/* A typedef with pointers */
typedef_decl   : TYPEDEF type declaration {
		 if (allow) {
		   init_language();
		   /* Add a new typedef */
		   Active_typedef = new DataType($2);
		   $2->is_pointer += $3.is_pointer;
		   $2->typedef_add($3.id);
		   /* If this is %typedef, add it to the header */
		   if ($1) 
		     fprintf(f_header,"typedef %s %s;\n", $2->print_full(), $3.id);
		   cplus_register_type($3.id);
		 }
	       } typedeflist { };

/* A rudimentary typedef involving function pointers */

               | TYPEDEF type LPAREN STAR pname RPAREN LPAREN parms RPAREN SEMI {
		 if (allow) {
		   init_language();
		   /* Typedef'd pointer */
		   if ($1) {
		     sprintf(temp_name,"(*%s)",$5);
		     fprintf(f_header,"typedef ");
		     emit_extern_func(temp_name, $2,$8,0,f_header);
		   }
		   strcpy($2->name,"<function ptr>");
		   $2->type = T_USER;
		   $2->is_pointer = 1;
		   $2->typedef_add($5,1);
		   cplus_register_type($5);
		 }
		 delete $2;
		 delete $5;
		 delete $8;
	       }

/* A typedef involving function pointers again */

               | TYPEDEF type stars LPAREN STAR pname RPAREN LPAREN parms RPAREN SEMI {
		 if (allow) {
		   init_language();
		   if ($1) {
		     $2->is_pointer += $3;
		     sprintf(temp_name,"(*%s)",$6);
		     fprintf(f_header,"typedef ");
		     emit_extern_func(temp_name, $2,$9,0,f_header);
		   }

		   /* Typedef'd pointer */
		   strcpy($2->name,"<function ptr>");
		   $2->type = T_USER;
		   $2->is_pointer = 1;
		   $2->typedef_add($6,1);
		   cplus_register_type($6);
		 }
		 delete $2;
		 delete $6;
		 delete $9;
	       }

/* A typedef involving arrays */

               | TYPEDEF type declaration array {
		 if (allow) {
		   init_language();
		   Active_typedef = new DataType($2);
		   // This datatype is going to be readonly
			
		   $2->status = STAT_READONLY | STAT_REPLACETYPE;
		   $2->is_pointer += $3.is_pointer;
		   // Turn this into a "pointer" corresponding to the array
		   $2->is_pointer++;
		   $2->arraystr = copy_string(ArrayString);
		   $2->typedef_add($3.id);
		   fprintf(stderr,"%s : Line %d. Warning. Array type %s will be read-only without a typemap\n",input_file,line_number, $3.id);
		   cplus_register_type($3.id);

		 }
	       } typedeflist { }
               ;

/* ------------------------------------------------------------------------
   Typedef list
   
   The following rules are used to manage typedef lists.  Only a temporary
   hack until the SWIG 2.0 parser gets online.

   Active_typedef contains the datatype of the last typedef (if applicable)
   ------------------------------------------------------------------------ */
             

typedeflist   : COMMA declaration typedeflist {
                if (allow) {
		  if (Active_typedef) {
		    DataType *t;
		    t = new DataType(Active_typedef);
		    t->is_pointer += $2.is_pointer;
		    t->typedef_add($2.id);
		    cplus_register_type($2.id);
		    delete t;
		  }
		}
              }
              | COMMA declaration array {
		    DataType *t;
		    t = new DataType(Active_typedef);
		    t->status = STAT_READONLY | STAT_REPLACETYPE;
		    t->is_pointer += $2.is_pointer + 1;
		    t->arraystr = copy_string(ArrayString);
		    t->typedef_add($2.id);
		    cplus_register_type($2.id);
		    delete t;
    		    fprintf(stderr,"%s : Line %d. Warning. Array type %s will be read-only without a typemap.\n",input_file,line_number, $2.id);
	      }
              | empty { }
              ;

/* ----------------------------------------------------------------------------------
   Conditional Compilation

   SWIG supports the following constructs
           #ifdef
	   #ifndef
	   #else
	   #endif
	   #if defined(ID)
	   #if ! defined(ID)
	   #elif
	  
   #if, and #elif are a little weak in this implementation
   ---------------------------------------------------------------------------------- */


/* #ifdef directive */
cond_compile   : IFDEF ID {
		 /* Push old if-then-else status */
		 if_push();
		 /* Look a symbol up in the symbol table */
		 if (lookup_symbol($2)) {
		   in_then = 1;
		   in_else = 0;
		   allow = 1 & prev_allow;
		 } else {
		   /* Condition is false.   Skip over whatever is in this block */
		   in_else = skip_cond(1);
		   if (in_else == -1) {
		     /* Unrecoverable error */
		     SWIG_exit(1);
		   }
		   if (!in_else) {
		     if_pop();        // Pop out. Reached end of block
		   } else {
		     allow = prev_allow;
		     in_then = 0;
		   }
		 }
                }

/* #ifndef directive */

               | IFNDEF ID {
		 if_push();
		 if (lookup_symbol($2)) {
		   /* Condition is false.   Skip over whatever is in this block */
		   in_else = skip_cond(1);
		   if (in_else == -1) {
		     /* Unrecoverable error */
		     SWIG_exit(1);
		   }
		   if (!in_else) {
		     if_pop();        // Pop out. Reached end of block
		   } else {
		     allow = prev_allow;
		     in_then = 0;
		   }
		 } else {
		   in_then = 1;
		   in_else = 0;		   
		   allow = 1 & prev_allow;
		 }
	       }

/* #else directive */
               | ELSE {
		 if ((!in_then) || (in_else)) {
		   fprintf(stderr,"%s : Line %d. Misplaced else\n", input_file, line_number);
		   FatalError();
		 } else {
		   in_then = 0;
		   in_else = 1;
		   if (allow) {
		     allow = 0;
		     /* Skip over rest of the conditional */
		     skip_cond(0);
		     if_pop();
		   } else {
		     allow = 1;
		   }
		   allow = allow & prev_allow;
		 }
	       }
/* #endif directive */
               | ENDIF {
		 if ((!in_then) && (!in_else)) {
		   fprintf(stderr,"%s : Line %d. Misplaced endif\n", input_file, line_number);
		   FatalError();
		 } else {
		   if_pop();
		 }
	       }

/* #if */
               | IF cpp_const_expr {
		 /* Push old if-then-else status */
		 if_push();
		 if ($2) {
		   in_then = 1;
		   in_else = 0;
		   allow = 1 & prev_allow;
		 } else {
		   /* Condition is false.   Skip over whatever is in this block */
		   in_else = skip_cond(1);
		   if (in_else == -1) {
		     /* Unrecoverable error */
		     SWIG_exit(1);
		   }
		   if (!in_else) {
		     if_pop();        // Pop out. Reached end of block
		   } else {
		     allow = prev_allow;
		     in_then = 0;
		   }
		 }
	       }

/* #elif.  We treat this identical to an #if.  Abit of a hack, but what
   the hell. */

               | ELIF cpp_const_expr {
		 /* have to pop old if clause off */
		 if_pop();

		 /* Push old if-then-else status */
		 if_push();
		 if ($2) {
		   in_then = 1;
		   in_else = 0;
		   allow = 1 & prev_allow;
		 } else {
		   /* Condition is false.   Skip over whatever is in this block */
		   in_else = skip_cond(1);
		   if (in_else == -1) {
		     /* Unrecoverable error */
		     SWIG_exit(1);
		   }
		   if (!in_else) {
		     if_pop();        // Pop out. Reached end of block
		   } else {
		     allow = prev_allow;
		     in_then = 0;
		   }
		 }
	       }
               ;

/* C preprocessor expression (only used for conditional compilation */

cpp_const_expr : DEFINED LPAREN ID RPAREN {

                 /* Look ID up in the symbol table */
                    if (lookup_symbol($3)) {
		      $$ = 1;
		    } else {
		      $$ = 0;
		    }
               }
               | DEFINED ID {
		 if (lookup_symbol($2)) {
		   $$ = 1;
		 } else {
		   $$ = 0;
		 }
	       }
               | LNOT cpp_const_expr {
                      if ($2) $$ = 0;
		      else $$ = 1;
	       }
               ;

pragma         : PRAGMA LPAREN ID COMMA ID stylearg RPAREN {
		 if (allow && (!WrapExtern))
		   lang->pragma($3,$5,$6);
		   fprintf(stderr,"%s : Line %d. Warning. '%%pragma(lang,opt=value)' syntax is obsolete.\n",
			   input_file,line_number);
		   fprintf(stderr,"        Use '%%pragma(lang) opt=value' instead.\n");
	       }

               | PRAGMA ID stylearg {
                 if (allow && (!WrapExtern)) 
		   swig_pragma($2,$3);
    	       }
               | PRAGMA LPAREN ID RPAREN ID stylearg {
		 if (allow && (!WrapExtern))
		   lang->pragma($3,$5,$6);
	       }
               ;

/* Allow lists of variables and functions to be built up */

stail          : SEMI { }
               | COMMA declaration array2 def_args {
		 if (allow) {
		   init_language();
		   temp_typeptr = new DataType(Active_type);
		   temp_typeptr->is_pointer += $2.is_pointer;
		   if ($3 > 0) {
		     temp_typeptr->is_pointer++;
		     temp_typeptr->status = STAT_READONLY;
		     temp_typeptr->arraystr = copy_string(ArrayString);
		   }
		   if ($2.is_reference) {
		     fprintf(stderr,"%s : Line %d. Error. Linkage to C++ reference not allowed.\n", input_file, line_number);
		     FatalError();
		   } else {
		     if (temp_typeptr->qualifier) {
		       if ((strcmp(temp_typeptr->qualifier,"const") == 0)) {
			 /* Okay.  This is really some sort of C++ constant here. */
			 if ($4.type != T_ERROR)
			   create_constant($2.id, temp_typeptr, $4.id);
		       } else 
			 create_variable(Active_extern,$2.id, temp_typeptr);
		     } else
		       create_variable(Active_extern, $2.id, temp_typeptr);
		   }
		   delete temp_typeptr;
		 }
	       } stail { } 
               | COMMA declaration LPAREN parms RPAREN cpp_const {
		 if (allow) {
		   init_language();
		   temp_typeptr = new DataType(Active_type);
		   temp_typeptr->is_pointer += $2.is_pointer;
		   temp_typeptr->is_reference = $2.is_reference;
		   create_function(Active_extern, $2.id, temp_typeptr, $4);
		   delete temp_typeptr;
		 }
		 delete $4;
	       } stail { }
              ;

definetail     : definetype ENDDEF {
                   $$ = $1;
                 } 
               | ENDDEF {
                   $$.type = T_SYMBOL;
	       }
               | error ENDDEF {
		 if (Verbose) 
		   fprintf(stderr,"%s : Line %d.  Warning. Unable to parse #define (ignored)\n", input_file, line_number);
		 $$.type = T_ERROR;
	       }

               ;

extern         : EXTERN { $$ = 1; }
               | empty {$$ = 0; }
               | EXTERN STRING {
		 if (strcmp($2,"C") == 0) {
		   $$ = 2;
		 } else {
		   fprintf(stderr,"%s : Line %d.  Unrecognized extern type \"%s\" (ignored).\n", input_file, line_number, $2);
		   FatalError();
		 }
	       }
               ;

/* End of a function declaration.  Allows C++ "const" directive and inline code */

func_end       : cpp_const LBRACE { skip_brace(); }
/*               | LBRACE { skip_brace(); } */
               ;

/* ------------------------------------------------------------------------------
   Function parameter lists

   ------------------------------------------------------------------------------ */

parms          : parm ptail {
                 if (($1->t->type != T_VOID) || ($1->t->is_pointer))
		   $2->insert($1,0);
		 $$ = $2;
		 delete $1;
		}
               | empty { $$ = new ParmList;}
               ;

ptail          : COMMA parm ptail {
		 $3->insert($2,0);
		 $$ = $3;
		 delete $2;
                }
               | empty { $$ = new ParmList;}
               ;

parm           : parm_type {
                  $$ = $1;
		  if (typemap_check("ignore",typemap_lang,$$->t,$$->name))
		    $$->ignore = 1;
               }
               | parm_specifier_list parm_type {
                  $$ = $2;
                  $$->call_type = $$->call_type | $1;
		  if (InArray && ($$->call_type & CALL_VALUE)) {
		     fprintf(stderr,"%s : Line %d. Error. Can't use %%val with an array.\n", input_file, line_number);
		     FatalError();
		  }
		  if (!$$->t->is_pointer) {
		     fprintf(stderr,"%s : Line %d. Error. Can't use %%val or %%out with a non-pointer argument.\n", input_file, line_number);
		     FatalError();
		  } else {
		    $$->t->is_pointer--;
		  }
		}

parm_type      : type pname {
		    if (InArray) {
		      $1->is_pointer++;
		      if (Verbose) {
			fprintf(stderr,"%s : Line %d. Warning. Array %s", input_file, line_number, $1->print_type());
			print_array();
			fprintf(stderr," has been converted to %s.\n", $1->print_type());
		      }
		      // Add array string to the type
		      $1->arraystr = copy_string(ArrayString.get());
		    } 
		    $$ = new Parm($1,$2);
		    $$->call_type = 0;
		    $$->defvalue = DefArg;
		    if (($1->type == T_USER) && !($1->is_pointer)) {
		      if (Verbose)
			fprintf(stderr,"%s : Line %d. Warning : Parameter of type '%s'\nhas been remapped to '%s *' and will be called using *((%s *) ptr).\n",
				input_file, line_number, $1->name, $1->name, $1->name);

		      $$->call_type = CALL_REFERENCE;
		      $$->t->is_pointer++;
		    }
		    delete $1;
		    delete $2;
                 }

                | type stars pname {
		   $$ = new Parm($1,$3);
		   $$->t->is_pointer += $2;
		   $$->call_type = 0;
		   $$->defvalue = DefArg;
		   if (InArray) {
		     $$->t->is_pointer++;
		     if (Verbose) {
		       fprintf(stderr,"%s : Line %d. Warning. Array %s", input_file, line_number, $$->t->print_type());
		       print_array();
		       fprintf(stderr," has been converted to %s.\n", $$->t->print_type());
		     }
		     // Add array string to the type
		     $$->t->arraystr = copy_string(ArrayString.get());
		    }
		   delete $1;
		   delete $3;
		}

                | type AND pname {
		  $$ = new Parm($1,$3);
		  $$->t->is_reference = 1;
		  $$->call_type = 0;
		  $$->t->is_pointer++;
		  $$->defvalue = DefArg;
		  if (!CPlusPlus) {
			fprintf(stderr,"%s : Line %d. Warning.  Use of C++ Reference detected.  Use the -c++ option.\n", input_file, line_number);
		  }
		  delete $1;
		  delete $3;
		}
                | type LPAREN stars pname RPAREN LPAREN parms RPAREN {
                  fprintf(stderr,"%s : Line %d. Error. Function pointer not allowed (remap with typedef).\n", input_file, line_number);
		  FatalError();
		  $$ = new Parm($1,$4);
		  $$->t->type = T_ERROR;
		  $$->name = copy_string($4);
		  strcpy($$->t->name,"<function ptr>");
		  delete $1;
		  delete $4;
		  delete $7;
		}
                | PERIOD PERIOD PERIOD {
                  fprintf(stderr,"%s : Line %d. Variable length arguments not supported (ignored).\n", input_file, line_number);
		  $$ = new Parm(new DataType(T_INT),"varargs");
		  $$->t->type = T_ERROR;
		  $$->name = copy_string("varargs");
		  strcpy($$->t->name,"<varargs>");
		  FatalError();
		}
		;

pname          : ID def_args {
                    $$ = $1; 
                    InArray = 0;
		    if ($2.type == T_CHAR)
		      DefArg = copy_string(ConstChar);
		    else
		      DefArg = copy_string($2.id);
                    if ($2.id) delete $2.id;
                }
               | ID array {
                    $$ = $1; 
                    InArray = $2; 
                    DefArg = 0;
               }
               | array {
                         $$ = new char[1];
                         $$[0] = 0;
                         InArray = $1;
                         DefArg = 0;
               }
               | empty { $$ = new char[1];
	                 $$[0] = 0;
                         InArray = 0;
                         DefArg = 0;
               }
               ;

def_args       : EQUAL definetype { $$ = $2; }
               | EQUAL AND ID {
		 $$.id = new char[strlen($3)+2];
		 $$.id[0] = '&';
		 strcpy(&$$.id[1], $3);
		 $$.type = T_USER;
	       }
               | EQUAL LBRACE {
		 skip_brace();
		 $$.id = 0; $$.type = T_INT;
	       }
               | COLON NUM_INT {
               }
               | empty {$$.id = 0; $$.type = T_INT;}
               ;

parm_specifier : CVALUE { $$ = CALL_VALUE; }
               | COUT { $$ = CALL_OUTPUT; }
               ;
                
parm_specifier_list : parm_specifier_list parm_specifier {
                 $$ = $1 | $2;
               }
               | parm_specifier {
                 $$ = $1;
	       }
               ;

/* Declaration must be an identifier, possibly preceded by a * for pointer types  */

declaration    : ID { $$.id = $1;
                      $$.is_pointer = 0;
		      $$.is_reference = 0;
                }
               | stars ID {
                      $$.id = $2;
		      $$.is_pointer = $1;
		      $$.is_reference = 0;
	       }
               | AND ID {
		      $$.id = $2;
		      $$.is_pointer = 1;
		      $$.is_reference = 1;
		      if (!CPlusPlus) {
			fprintf(stderr,"%s : Line %d. Warning.  Use of C++ Reference detected.  Use the -c++ option.\n", input_file, line_number);
		      }
	       }
               ;

stars :          STAR empty { $$ = 1; }
               | STAR stars { $$ = $2 + 1;}
               ;


array :        LBRACKET  RBRACKET array2 {
		 $$ = $3 + 1;
		 "[]" >> ArrayString;
              }
              | LBRACKET expr RBRACKET array2 {
                 $$ = $4 + 1;
		 "]" >> ArrayString;
		 $2.id >> ArrayString;
		 "[" >> ArrayString;
              }
	      ;
array2 :       array {
                 $$ = $1;
              }
              | empty { $$ = 0;
                        ArrayString = "";
              }
	      ;

/* Data type must be a built in type or an identifier for user-defined types
   This type can be preceded by a modifier. */

type           : TYPE_INT {
                   $$ = $1;
               }
               | TYPE_SHORT opt_int {
                   $$ = $1;
	       }
               | TYPE_LONG opt_int {
                   $$ = $1;
	       }
               | TYPE_CHAR {
                   $$ = $1;
	       }
               | TYPE_BOOL {
                   $$ = $1;
	       }
               | TYPE_FLOAT {
                   $$ = $1;
	       }
               | TYPE_DOUBLE {
                   $$ = $1;
	       }
               | TYPE_VOID {
                   $$ = $1;
	       }
               | TYPE_SIGNED opt_signed {
                   if ($2) $$ = $2;
		   else $$ = $1;
	       }
               | TYPE_UNSIGNED opt_unsigned {
                   if ($2) $$ = $2;
		   else $$ = $1;
	       }
               | TYPE_TYPEDEF objc_protolist {
		 $$ = $1;
		 if (strlen($2) > 0) {
		    if ((strlen($2) + strlen($$->name)) >= MAX_NAME) {
		      fprintf(stderr,"%s : Line %d. Fatal error. Type-name is too long!\n", 
			      input_file, line_number);
		    } else {
		      strcat($$->name,$2);
		    }
		  }
	       }
               | ID objc_protolist {
		  $$ = new DataType;
		  strcpy($$->name,$1);
		  $$->type = T_USER;
		  /* Do a typedef lookup */
		  $$->typedef_resolve();
		  if (strlen($2) > 0) {
		    if ((strlen($2) + strlen($$->name)) >= MAX_NAME) {
		      fprintf(stderr,"%s : Line %d. Fatal error. Type-name is too long!\n", 
			      input_file, line_number);
		    } else {
		      strcat($$->name,$2);
		    }
		  }
	       }
               | CONST type {
		  $$ = $2;
                  $$->qualifier = new char[6];
		  strcpy($$->qualifier,"const");
     	       }
               | cpptype ID {
                  $$ = new DataType;
		  sprintf($$->name,"%s %s",$1, $2);
		  $$->type = T_USER;
	       }
               | ID DCOLON ID {
                  $$ = new DataType;
                  sprintf($$->name,"%s::%s",$1,$3);
                  $$->type = T_USER;
		  $$->typedef_resolve();
               }
/* This declaration causes a shift-reduce conflict.  Unresolved for now */


               | DCOLON ID {
                  $$ = new DataType;
                  sprintf($$->name,"%s", $2);
                  $$->type = T_USER;
                  $$->typedef_resolve(1);
               }
               | ENUM ID {
                  $$ = new DataType;
                  sprintf($$->name,"enum %s", $2);
                  $$->type = T_INT;
                  $$->typedef_resolve(1);
               }
               ;

/* type specification without ID symbol.   Used in some cases to prevent shift-reduce conflicts */

strict_type    : TYPE_INT {
                   $$ = $1;
               }
               | TYPE_SHORT opt_int {
                   $$ = $1;
	       }
               | TYPE_LONG opt_int {
                   $$ = $1;
	       }
               | TYPE_CHAR {
                   $$ = $1;
	       }
               | TYPE_BOOL {
                   $$ = $1;
	       }
               | TYPE_FLOAT {
                   $$ = $1;
	       }
               | TYPE_DOUBLE {
                   $$ = $1;
	       }
               | TYPE_VOID {
                   $$ = $1;
	       }
               | TYPE_SIGNED opt_signed {
                   if ($2) $$ = $2;
		   else $$ = $1;
	       }
               | TYPE_UNSIGNED opt_unsigned {
                   if ($2) $$ = $2;
		   else $$ = $1;
	       }
               | TYPE_TYPEDEF objc_protolist {
		   $$ = $1;
		   strcat($$->name,$2);
	       }
               | CONST type {
		  $$ = $2;
                  $$->qualifier = new char[6];
		  strcpy($$->qualifier,"const");
     	       }
               | cpptype ID {
                  $$ = new DataType;
		  sprintf($$->name,"%s %s",$1, $2);
		  $$->type = T_USER;
	       }
               ;

/* Optional signed types */

opt_signed     : empty {
                   $$ = (DataType *) 0;
               }
               | TYPE_INT {
                   $$ = $1;
		   $$->type = T_INT;
		   sprintf(temp_name,"signed %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               | TYPE_SHORT opt_int {
                   $$ = $1;
		   $$->type = T_SHORT;
		   sprintf(temp_name,"signed %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               | TYPE_LONG opt_int {
                   $$ = $1;
		   $$->type = T_LONG;
		   sprintf(temp_name,"signed %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               | TYPE_CHAR {
                   $$ = $1;
		   $$->type = T_SCHAR;
		   sprintf(temp_name,"signed %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               ;

/* Optional unsigned types */

opt_unsigned   : empty {
                   $$ = (DataType *) 0;
               }
               | TYPE_INT {
                   $$ = $1;
		   $$->type = T_UINT;
		   sprintf(temp_name,"unsigned %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               | TYPE_SHORT opt_int {
                   $$ = $1;
		   $$->type = T_USHORT;
		   sprintf(temp_name,"unsigned %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               | TYPE_LONG opt_int {
                   $$ = $1;
		   $$->type = T_ULONG;
		   sprintf(temp_name,"unsigned %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               | TYPE_CHAR {
                   $$ = $1;
		   $$->type = T_UCHAR;
		   sprintf(temp_name,"unsigned %s",$1->name);
		   strcpy($$->name,temp_name);
	       }
               ;

opt_int        : TYPE_INT { }
               | empty { }
               ;

definetype     : { scanner_check_typedef(); } expr {
                   $$ = $2;
		   scanner_ignore_typedef();
		   if (ConstChar) delete ConstChar;
		   ConstChar = 0;
                }
                | STRING {
                   $$.id = $1;
                   $$.type = T_CHAR;
		   if (ConstChar) delete ConstChar;
		   ConstChar = new char[strlen($1)+3];
		   sprintf(ConstChar,"\"%s\"",$1);
		}
                | CHARCONST {
                   $$.id = $1;
		   $$.type = T_CHAR;
		   if (ConstChar) delete ConstChar;
		   ConstChar = new char[strlen($1)+3];
		   sprintf(ConstChar,"'%s'",$1);
		 }
                ;	       


/* Initialization function links */

initlist       : initlist COMMA ID {
                 $$ = $1;
		 $$.names[$$.count] = copy_string($3);
		 $$.count++;
		 $$.names[$$.count] = (char *) 0;
                }
               | empty {
                 $$.names = new char *[NI_NAMES];
		 $$.count = 0;
		 for (i = 0; i < NI_NAMES; i++) 
		   $$.names[i] = (char *) 0;
	       }
               ;

/* Some stuff for handling enums */

ename          :  ID { $$ = $1; } 
               |  empty { $$ = (char *) 0;}
               ;

/* SWIG enum list.   
*/

enumlist       :  enumlist COMMA edecl {}
               |  edecl {}
               ;


edecl          :  ID {
		   temp_typeptr = new DataType(T_INT);
		   create_constant($1, temp_typeptr, $1);
		   delete temp_typeptr;
		 } 
                 | ID EQUAL { scanner_check_typedef();} etype {
		   temp_typeptr = new DataType($4.type);
// Use enum name instead of value
// OLD		   create_constant($1, temp_typeptr, $4.id);
                   create_constant($1, temp_typeptr, $1);
		   delete temp_typeptr;
                 }
                 | cond_compile edecl { }
                 | empty { }
                 ;
		   
etype            : expr {
                   $$ = $1;
		   if (($$.type != T_INT) && ($$.type != T_UINT) &&
		       ($$.type != T_LONG) && ($$.type != T_ULONG) &&
		       ($$.type != T_SHORT) && ($$.type != T_USHORT) && 
		       ($$.type != T_SCHAR) && ($$.type != T_UCHAR)) {
		     fprintf(stderr,"%s : Lind %d. Type error. Expecting an int\n",
			     input_file, line_number);
		     FatalError();
		   }

                }
                | CHARCONST {
                   $$.id = $1;
		   $$.type = T_CHAR;
		 }
                ;	       

/* Arithmetic expressions.   Used for constants and other cool stuff.
   Really, we're not doing anything except string concatenation, but
   this does allow us to parse many constant declarations.
 */

expr           :  NUM_INT { 
                  $$.id = $1;
                  $$.type = T_INT;
                 }
               |  NUM_FLOAT {
                  $$.id = $1;
                  $$.type = T_DOUBLE;
               }
               |  NUM_UNSIGNED {
                  $$.id = $1;
		  $$.type = T_UINT;
	       }
               |  NUM_LONG {
                  $$.id = $1;
		  $$.type = T_LONG;
	       } 
               |  NUM_ULONG {
                  $$.id = $1;
		  $$.type = T_ULONG;
	       }
               |  SIZEOF LPAREN type RPAREN {
	          $$.id = new char[strlen($3->name)+9];
		  sprintf($$.id,"sizeof(%s)", $3->name);
		  $$.type = T_INT;
	       }
               |  LPAREN strict_type RPAREN expr %prec UMINUS {
		  $$.id = new char[strlen($4.id)+strlen($2->name)+3];
		  sprintf($$.id,"(%s)%s",$2->name,$4.id);
		  $$.type = $2->type;
	       } 
               | ID {
		 $$.id = lookup_symvalue($1);
		 if ($$.id == (char *) 0)
		   $$.id = $1;
		 else {
		   $$.id = new char[strlen($$.id)+3];
		   sprintf($$.id,"(%s)",lookup_symvalue($1));
		 }
		 temp_typeptr = lookup_symtype($1);
		 if (temp_typeptr) $$.type = temp_typeptr->type;
		 else $$.type = T_INT;
               }
               | ID DCOLON ID {
                  $$.id = new char[strlen($1)+strlen($3)+3];
		  sprintf($$.id,"%s::%s",$1,$3);
                  $$.type = T_INT;
		  delete $1;
		  delete $3;
               }
               | expr PLUS expr {
	         E_BINARY($$.id,$1.id,$3.id,"+");
		 $$.type = promote($1.type,$3.type);
		 delete $1.id;
		 delete $3.id;
	       }
               | expr MINUS expr {
	         E_BINARY($$.id,$1.id,$3.id,"-");
		 $$.type = promote($1.type,$3.type);
		 delete $1.id;
		 delete $3.id;
	       }
               | expr STAR expr {
	         E_BINARY($$.id,$1.id,$3.id,"*");
		 $$.type = promote($1.type,$3.type);
		 delete $1.id;
		 delete $3.id;

	       }
               | expr SLASH expr {
	         E_BINARY($$.id,$1.id,$3.id,"/");
		 $$.type = promote($1.type,$3.type);
		 delete $1.id;
		 delete $3.id;

	       }
               | expr AND expr {
	         E_BINARY($$.id,$1.id,$3.id,"&");
		 $$.type = promote($1.type,$3.type);
		 if (($1.type == T_DOUBLE) || ($3.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 delete $1.id;
		 delete $3.id;

	       }
               | expr OR expr {
	         E_BINARY($$.id,$1.id,$3.id,"|");
		 $$.type = promote($1.type,$3.type);
		 if (($1.type == T_DOUBLE) || ($3.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 $$.type = T_INT;
		 delete $1.id;
		 delete $3.id;

	       }
               | expr XOR expr {
	         E_BINARY($$.id,$1.id,$3.id,"^");
		 $$.type = promote($1.type,$3.type);
		 if (($1.type == T_DOUBLE) || ($3.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 $$.type = T_INT;
		 delete $1.id;
		 delete $3.id;

	       }
               | expr LSHIFT expr {
	         E_BINARY($$.id,$1.id,$3.id,"<<");
		 $$.type = promote($1.type,$3.type);
		 if (($1.type == T_DOUBLE) || ($3.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 $$.type = T_INT;
		 delete $1.id;
		 delete $3.id;

	       }
               | expr RSHIFT expr {
	         E_BINARY($$.id,$1.id,$3.id,">>");
		 $$.type = promote($1.type,$3.type);
		 if (($1.type == T_DOUBLE) || ($3.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 $$.type = T_INT;
		 delete $1.id;
		 delete $3.id;

	       }
               |  MINUS expr %prec UMINUS {
	          $$.id = new char[strlen($2.id)+2];
		  sprintf($$.id,"-%s",$2.id);
		  $$.type = $2.type;
		 delete $2.id;

	       }
               |  NOT expr {
	          $$.id = new char[strlen($2.id)+2];
		  sprintf($$.id,"~%s",$2.id);
		  if ($2.type == T_DOUBLE) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		  }
		  $$.type = $2.type;
		  delete $2.id;
	       }
               |  LPAREN expr RPAREN {
	          $$.id = new char[strlen($2.id)+3];
	          sprintf($$.id,"(%s)", $2.id);
		  $$.type = $2.type;
		  delete $2.id;
	       }
               ;
/****************************************************************/
/* C++ Support                                                  */
/****************************************************************/

cpp          : cpp_class { }
             | cpp_other {}
             ;
  
cpp_class    : 

/* A class/struct/union  definition */
             extern cpptype ID inherit LBRACE {
	       char *iname;
	       if (allow) {
		 init_language();
		 DataType::new_scope();

		 sprintf(temp_name,"CPP_CLASS:%s\n",$3);
		 if (add_symbol(temp_name, (DataType *) 0, (char *) 0)) {
		   fprintf(stderr,"%s : Line %d. Error. %s %s is multiply defined.\n", input_file, line_number, $2, $3);
		   FatalError();
		 }
		 if ((!CPlusPlus) && (strcmp($2,"class") == 0))
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);

		 iname = make_name($3);
		 doc_entry = new DocClass(iname, doc_parent());
		 if (iname == $3) 
		   cplus_open_class($3, 0, $2);
		 else
		   cplus_open_class($3, iname, $2);
		 if (strcmp($2,"class") == 0)
		   cplus_mode = CPLUS_PRIVATE;
		 else
		   cplus_mode = CPLUS_PUBLIC;
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
		 nested_list = 0;
		 // Merge in scope from base classes
		 cplus_inherit_scope($4.count,$4.names);
	       }
              } cpp_members RBRACE {
		if (allow) {
		  if ($4.names) {
		    if (strcmp($2,"union") != 0)
		      cplus_inherit($4.count, $4.names);
		    else {
		      fprintf(stderr,"%s : Line %d.  Inheritance not allowed for unions.\n",input_file, line_number);
		      FatalError();
		    }
		  }
		  // Clean up the inheritance list
		  if ($4.names) {
		    int j;
		    for (j = 0; j < $4.count; j++) {
		      if ($4.names[j]) delete [] $4.names[j];
		    }
		    delete [] $4.names;
		  }

		  // Dumped nested declarations (if applicable)
		  dump_nested($3);

		  // Save and collapse current scope
		  cplus_register_scope(DataType::collapse_scope($3));

		  // Restore the original doc entry for this class
		  doc_entry = doc_stack[doc_stack_top];
		  cplus_class_close((char *) 0); 
		  doc_entry = 0;
		  // Bump the documentation stack back down
		  doc_stack_top--;
		  cplus_mode = CPLUS_PUBLIC;
		}
	      }

/* Class with a typedef */
		
             | TYPEDEF cpptype ID inherit LBRACE {
	       if (allow) {
		 char *iname;
		 init_language();
		 DataType::new_scope();

		 sprintf(temp_name,"CPP_CLASS:%s\n",$3);
		 if (add_symbol(temp_name, (DataType *) 0, (char *) 0)) {
		   fprintf(stderr,"%s : Line %d. Error. %s %s is multiply defined.\n", input_file, line_number, $2, $3);
		   FatalError();
		 }
		 if ((!CPlusPlus) && (strcmp($2,"class") == 0))
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);
		 
		 iname = make_name($3);
		 doc_entry = new DocClass(iname, doc_parent());
		 if ($3 == iname) 
		   cplus_open_class($3, 0, $2);
		 else
		   cplus_open_class($3, iname, $2);
		 if (strcmp($2,"class") == 0)
		   cplus_mode = CPLUS_PRIVATE;
		 else
		   cplus_mode = CPLUS_PUBLIC;
		 // Create a documentation entry for the class
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
		 nested_list = 0;

		 // Merge in scope from base classes
		 cplus_inherit_scope($4.count,$4.names);

	       }
              } cpp_members RBRACE declaration {
		if (allow) {
		  if ($4.names) {
		    if (strcmp($2,"union") != 0)
		      cplus_inherit($4.count, $4.names);
		    else {
		      fprintf(stderr,"%s : Line %d.  Inheritance not allowed for unions.\n",input_file, line_number);
		      FatalError();
		    }
		  }
		  // Create a datatype for correctly processing the typedef
		  Active_typedef = new DataType();
		  Active_typedef->type = T_USER;
		  sprintf(Active_typedef->name,"%s %s", $2,$3);
		  Active_typedef->is_pointer = 0;
		  Active_typedef->implicit_ptr = 0;

		  // Clean up the inheritance list
		  if ($4.names) {
		    int j;
		    for (j = 0; j < $4.count; j++) {
		      if ($4.names[j]) delete [] $4.names[j];
		    }
		    delete [] $4.names;
		  }

		  if ($9.is_pointer > 0) {
		    fprintf(stderr,"%s : Line %d.  typedef struct { } *id not supported properly. Winging it...\n", input_file, line_number);

		  }
		  // Create dump nested class code
		  if ($9.is_pointer > 0) {
		    dump_nested($3);
		  } else {
		    dump_nested($9.id);
		  }
		    
		  // Collapse any datatypes created in the the class

		  cplus_register_scope(DataType::collapse_scope($3));

		  doc_entry = doc_stack[doc_stack_top];
		  if ($9.is_pointer > 0) {
		    cplus_class_close($3);
		  } else {
		    cplus_class_close($9.id); 
		  }
		  doc_stack_top--;
		  doc_entry = 0;

		  // Create a typedef in global scope

		  if ($9.is_pointer == 0)
		    Active_typedef->typedef_add($9.id);
		  else {
		    DataType *t = new DataType(Active_typedef);
		    t->is_pointer += $9.is_pointer;
		    t->typedef_add($9.id);
		    cplus_register_type($9.id);
		    delete t;
		  }
		  cplus_mode = CPLUS_PUBLIC;
		}
	      } typedeflist { };

/* An unnamed struct with a typedef */

             | TYPEDEF cpptype LBRACE {
	       char *iname;
	       if (allow) {
		 init_language();
		 DataType::new_scope();
		 if ((!CPlusPlus) && (strcmp($2,"class") == 0))
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);
		 
		 iname = make_name("");
		 doc_entry = new DocClass(iname,doc_parent());
		 if (strlen(iname))
		   cplus_open_class("", iname, $2);
		 else
		   cplus_open_class("",0,$2);
		 if (strcmp($2,"class") == 0)
		   cplus_mode = CPLUS_PRIVATE;
		 else
		   cplus_mode = CPLUS_PUBLIC;
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
		 nested_list = 0;
	       }
              } cpp_members RBRACE declaration {
		if (allow) {
		  if ($7.is_pointer > 0) {
		    fprintf(stderr,"%s : Line %d. typedef %s {} *%s not supported correctly. Will be ignored.\n", input_file, line_number, $2, $7.id);
		    cplus_abort();
		  } else {
		    sprintf(temp_name,"CPP_CLASS:%s\n",$7.id);
		    if (add_symbol(temp_name, (DataType *) 0, (char *) 0)) {
		      fprintf(stderr,"%s : Line %d. Error. %s %s is multiply defined.\n", input_file, line_number, $2, $7.id);
		      FatalError();
		    }
		  }
		  // Create a datatype for correctly processing the typedef
		  Active_typedef = new DataType();
		  Active_typedef->type = T_USER;
		  sprintf(Active_typedef->name,"%s",$7.id);
		  Active_typedef->is_pointer = 0;
		  Active_typedef->implicit_ptr = 0;
		  
		  // Dump nested classes
		  if ($7.is_pointer == 0)  
		    dump_nested($7.id);

		  // Go back to previous scope

		  cplus_register_scope(DataType::collapse_scope((char *) 0));
		  
		  doc_entry = doc_stack[doc_stack_top];
		  // Change name of doc_entry
		  doc_entry->name = copy_string($7.id);
		  if ($7.is_pointer == 0) 
		    cplus_class_close($7.id); 
		  doc_entry = 0;
		  doc_stack_top--;
		  cplus_mode = CPLUS_PUBLIC;
		}
	      } typedeflist { }
             ;

cpp_other    :/* A dummy class name */

             extern cpptype ID SEMI {
	       char *iname;
		 if (allow) {
		   init_language();
		   iname = make_name($3);
		   lang->cpp_class_decl($3,iname,$2);
		 }
	     }   

/* A static C++ member function (declared out of scope)  */
            
              | extern type declaration DCOLON ID LPAREN parms RPAREN SEMI {
	       if (allow) {
		 init_language();
		 if (!CPlusPlus) 
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);
		 
		 $2->is_pointer += $3.is_pointer;
		 $2->is_reference = $3.is_reference;
		 // Fix up the function name
		 sprintf(temp_name,"%s::%s",$3.id,$5);
		 if (!Rename_true) {
		   Rename_true = 1;
		   sprintf(yy_rename,"%s_%s",$3.id,$5);
		 }
		 create_function($1, temp_name, $2, $7);
	       }
	       delete $2;
	       delete $7;
	      }
 
/* A static C++ member data */
             | extern type declaration DCOLON ID SEMI {
	       if (allow) {
		 init_language();
		 if (!CPlusPlus) 
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);

		 $2->is_pointer += $3.is_pointer;
		 // Fix up the function name
		 sprintf(temp_name,"%s::%s",$3.id,$5);
		 if (!Rename_true) {
		   Rename_true = 1;
		   sprintf(yy_rename,"%s_%s",$3.id,$5);
		 }
		 create_variable($1,temp_name, $2);
	       }
	       delete $2;
	     }

/* Operator overloading catch */

             | extern type declaration DCOLON OPERATOR {
	       fprintf(stderr,"%s : Line %d. Operator overloading not supported (ignored).\n", input_file, line_number);
		skip_decl();
		delete $2;
	     } 


/* Template catch */
             | TEMPLATE {
	       fprintf(stderr,"%s : Line %d. Templates not currently supported (ignored).\n",
		       input_file, line_number);
	       skip_decl();
	     }

/* %addmethods directive used outside of a class definition */

             | ADDMETHODS ID LBRACE {
	       cplus_mode = CPLUS_PUBLIC;
               doc_entry = cplus_set_class($2);
	       if (!doc_entry) {
		 doc_entry = new DocClass($2,doc_parent());
	       };
	       doc_stack_top++;
	       doc_stack[doc_stack_top] = doc_entry;
	       scanner_clear_start();
	       AddMethods = 1;
	     } added_members RBRACE {
	       cplus_unset_class();
	       doc_entry = 0;
	       doc_stack_top--;
	       AddMethods = 0;
	     }
             ;

added_members : cpp_member cpp_members { }
              | objc_method objc_methods { } 
              | empty { }
              ;
      
cpp_members  : cpp_member cpp_members {}
             | ADDMETHODS LBRACE {
	           AddMethods = 1;
	     } cpp_members RBRACE {
	           AddMethods = 0;
	     } cpp_members { }
	     | error {
	       skip_decl();
		   {
		     static int last_error_line = -1;
		     if (last_error_line != line_number) {
		       fprintf(stderr,"%s : Line %d. Syntax error in input.\n", input_file, line_number);
		       FatalError();
		       last_error_line = line_number;
		     }
		   }
	     } cpp_members { }
             | empty { }
             ;

cpp_member   :  type declaration LPAREN parms RPAREN cpp_end {
                char *iname;
                if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    $1->is_pointer += $2.is_pointer;
		    $1->is_reference = $2.is_reference;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping member function : %s\n",$2.id);
		    }
		    iname = make_name($2.id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == $2.id) iname = 0;
		    cplus_member_func($2.id, iname, $1,$4,0);
		  }
		  scanner_clear_start();
		}
		delete $1;
		delete $4;
              }
 
/* Virtual member function */
                      
             |  VIRTUAL type declaration LPAREN parms RPAREN cpp_vend {
	       char *iname;
	       if (allow) {
		 init_language();
		 if (cplus_mode == CPLUS_PUBLIC) {
		   Stat_func++;
		   $2->is_pointer += $3.is_pointer;
		   $2->is_reference = $3.is_reference;
		   if (Verbose) {
		     fprintf(stderr,"Wrapping virtual member function : %s\n",$3.id);
		   }
		   iname = make_name($3.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $3.id) iname = 0;
		   cplus_member_func($3.id,iname,$2,$5,1);
		 }
		 scanner_clear_start();
	       }
	       delete $2;
	       delete $5;
	     }

/* Possibly a constructor */
              | ID LPAREN parms RPAREN ctor_end {
		char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping C++ constructor %s\n", $1);
		    }
		    iname = make_name($1);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == $1) iname = 0;
		    cplus_constructor($1,iname, $3);
		  }
		  scanner_clear_start();
		}
		delete $3;
	      }

/* A destructor (hopefully) */

              | NOT ID LPAREN parms RPAREN cpp_end {
		char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping C++ destructor %s\n", $2);
		    }
		    iname = make_name($2);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == $2) iname = 0;
		    cplus_destructor($2,iname);
		  }
		}
		scanner_clear_start();
	      }

/* A virtual destructor */

              | VIRTUAL NOT ID LPAREN RPAREN cpp_end {
 	        char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping C++ destructor %s\n", $3);
		    }
		    iname = make_name($3);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == $3) iname = 0;
		    cplus_destructor($3,iname);
		  }
		}
		scanner_clear_start();
	      }

/* Member data */

              | type declaration def_args {
		if (allow) {
		  char *iname;
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    if (Active_type) delete Active_type;
		    Active_type = new DataType($1);
		    $1->is_pointer += $2.is_pointer;
		    $1->is_reference = $2.is_reference;
		    if ($1->qualifier) {
		      if ((strcmp($1->qualifier,"const") == 0) && ($1->is_pointer == 0)) {
			// Okay.  This is really some sort of C++ constant here.
	  	          if ($3.type != T_ERROR) {
			    iname = make_name($2.id);
			    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
			    if (iname == $2.id) iname = 0;
			    cplus_declare_const($2.id,iname, $1, $3.id);
			  }
		      } else {
			int oldstatus = Status;
			char *tm;
			if ($1->status & STAT_READONLY) {
			  if (!(tm = typemap_lookup("memberin",typemap_lang,$1,$2.id,"",""))) 
			    Status = Status | STAT_READONLY;
			}
			iname = make_name($2.id);
			doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
			if (iname == $2.id) iname = 0;
			cplus_variable($2.id,iname,$1);
			Status = oldstatus;
		      }
		    } else {
		      char *tm = 0;
		      int oldstatus = Status;
		      if ($1->status & STAT_READONLY) {
			if (!(tm = typemap_lookup("memberin",typemap_lang,$1,$2.id,"",""))) 
			  Status = Status | STAT_READONLY;
		      }
		      iname = make_name($2.id);
		      doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		      if (iname == $2.id) iname = 0;
		      cplus_variable($2.id,iname,$1);
		      Status = oldstatus;
		      if (Verbose) {
			fprintf(stderr,"Wrapping member data %s\n", $2.id);
		      }
		    }
		  }
		  scanner_clear_start();
		}
		delete $1;
	      } cpp_tail { }

              | type declaration array def_args {
		char *iname;
		if (allow) {
		  int oldstatus = Status;
		  char *tm = 0;
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    if (Active_type) delete Active_type;
		    Active_type = new DataType($1);
		    $1->is_pointer += $2.is_pointer + 1;
		    $1->is_reference = $2.is_reference;
		    $1->arraystr = copy_string(ArrayString);
		    if (!(tm = typemap_lookup("memberin",typemap_lang,$1,$2.id,"",""))) 
		      Status = STAT_READONLY;

		    iname = make_name($2.id);
		    doc_entry = new DocDecl(iname, doc_stack[doc_stack_top]);
		    if (iname == $2.id) iname = 0;
		    cplus_variable($2.id,iname,$1);
		    Status = oldstatus;
		    if (!tm)
		      fprintf(stderr,"%s : Line %d. Warning. Array member will be read-only.\n",input_file,line_number);
		  }
		scanner_clear_start();
		}
		delete $1;
	      }
  

/* Static Member data */

              | STATIC type declaration {
		char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    $2->is_pointer += $3.is_pointer;
		    iname = make_name($3.id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == $3.id) iname = 0;
		    cplus_static_var($3.id,iname,$2);
		    if (Active_type) delete Active_type;
		    Active_type = new DataType($2);
		    if (Verbose) {
		      fprintf(stderr,"Wrapping static member data %s\n", $3.id);
		    }
		  }
		  scanner_clear_start();
		}
		delete $2;
	      } cpp_tail { }

/* Static member function */

              | STATIC type declaration LPAREN parms RPAREN cpp_end {
		char *iname;
		if (allow) {
		  $2->is_pointer += $3.is_pointer;
		  $2->is_reference = $3.is_reference;
		  if (cplus_mode == CPLUS_PUBLIC) {
		    iname = make_name($3.id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == $3.id) iname = 0;
		    cplus_static_func($3.id, iname, $2, $5);
		    if (Verbose)
		      fprintf(stderr,"Wrapping static member function %s\n",$3.id);
		  }
		  scanner_clear_start();
		}
		delete $2;
		delete $5;
	      }
/* Turn on public: mode */

              | PUBLIC COLON {
		if (allow) {
		  cplus_mode = CPLUS_PUBLIC;
		  if (Verbose)
		    fprintf(stderr,"Public mode\n");
		  scanner_clear_start();
		}
	      }

/* Turn on private: mode */

              | PRIVATE COLON {
		if (allow) {
		  cplus_mode = CPLUS_PRIVATE;
		  if (Verbose)
		    fprintf(stderr,"Private mode\n");
		  scanner_clear_start();
		}
	      }

/* Turn on protected mode */

              | PROTECTED COLON {
		if (allow) {
		  cplus_mode = CPLUS_PROTECTED;
		  if (Verbose)
		    fprintf(stderr,"Protected mode\n");
		  scanner_clear_start();
		}
	      }

/* This is the new style rename */

             | NAME LPAREN ID RPAREN {
	       if (allow) {
		 strcpy(yy_rename,$3);
		 Rename_true = 1;
	       }
	     } 

/* New mode */
             | NEW {
                 NewObject = 1;
             } cpp_member {
                 NewObject = 0;
             }

/* C++ Enum */
              | ENUM ename LBRACE {scanner_clear_start();} cpp_enumlist RBRACE SEMI {

		 // if ename was supplied.  Install it as a new integer datatype.

		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		   if ($2) {
		     cplus_register_type($2);
		     temp_type.type = T_INT;
		     temp_type.is_pointer = 0;
		     temp_type.implicit_ptr = 0;
		     sprintf(temp_type.name,"int");
		     temp_type.typedef_add($2,1); 
		   }
		 }
	       }
	      }
              | READONLY {
		if (allow)
		  Status = Status | STAT_READONLY;
		scanner_clear_start();
              }
              | READWRITE {
		if (allow) 
		  Status = Status & ~(STAT_READONLY);
		scanner_clear_start();
	      }
/* A friend :   Illegal */
              | FRIEND {
		if (allow)
		  fprintf(stderr,"%s : Line %d. Friends are not allowed--members only! (ignored)\n", input_file, line_number);
		skip_decl();
		scanner_clear_start();
	      }  

/* An operator: Illegal */
              | type type_extra OPERATOR {
		if (allow)
		  fprintf(stderr,"%s : Line %d. Operator overloading not supported (ignored).\n", input_file, line_number);
		skip_decl();
		scanner_clear_start();
	      }
              | cond_compile { 
		scanner_clear_start();
	      }

/* A typedef inside a class */
              | typedef_decl { }

/* Pragma directive */

              | cpp_pragma {
	      		scanner_clear_start();
	      }


cpp_pragma    : PRAGMA ID stylearg {
                 if (allow && (!WrapExtern)) { }
    	       }
               | PRAGMA LPAREN ID RPAREN ID stylearg {
		 if (allow && (!WrapExtern))
                   cplus_add_pragma($3,$5,$6);
	       }
               ;



/* ----------------------------------------------------------------------
   Nested structure.    This is a big ugly "hack".   If we encounter
   a nested structure, we're going to grab the text of its definition and
   feed it back into the scanner.  In the meantime, we need to grab
   variable declaration information and generate the associated wrapper
   code later.  Yikes!

   This really only works in a limited sense.   Since we use the 
   code attached to the nested class to generate both C/C++ code,
   it can't have any SWIG directives in it.  It also needs to be parsable
   by SWIG or this whole thing is going to puke.
   ---------------------------------------------------------------------- */

/* A struct sname { } id;  declaration */

              | cpptype ID LBRACE { start_line = line_number; skip_brace(); 
	      } nested_decl SEMI { 

		if (cplus_mode == CPLUS_PUBLIC) {
		  cplus_register_type($2);
		  if ($5.id) {
		    if (strcmp($1,"class") == 0) {
		      fprintf(stderr,"%s : Line %d.  Warning. Nested classes not currently supported (ignored).\n", input_file, line_number);
		      /* Generate some code for a new class */
		    } else {
		      Nested *n = new Nested;
		      n->code << "typedef " << $1 << " " 
			      << CCode.get() << " $classname_" << $5.id << ";\n";
		      n->name = copy_string($5.id);
		      n->line = start_line;
		      n->type = new DataType;
		      n->type->type = T_USER;
		      n->type->is_pointer = $5.is_pointer;
		      n->type->is_reference = $5.is_reference;
		      n->next = 0;
		      add_nested(n);
		    }
		  }
		}
	      }
/* An unnamed structure definition */
              | cpptype LBRACE { start_line = line_number; skip_brace();
              } declaration SEMI { 
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (strcmp($1,"class") == 0) {
		    fprintf(stderr,"%s : Line %d.  Warning. Nested classes not currently supported (ignored)\n", input_file, line_number);
		    /* Generate some code for a new class */
		  } else {
		    /* Generate some code for a new class */

		    Nested *n = new Nested;
		    n->code << "typedef " << $1 << " " 
			    << CCode.get() << " $classname_" << $4.id << ";\n";
		    n->name = copy_string($4.id);
		    n->line = start_line;
		    n->type = new DataType;
		    n->type->type = T_USER;
		    n->type->is_pointer = $4.is_pointer;
		    n->type->is_reference = $4.is_reference;
		    n->next = 0;
		    add_nested(n);

		  }
		}
	      }
/* An empty class declaration */
              | cpptype ID SEMI {
  		    if (cplus_mode == CPLUS_PUBLIC) {
                       cplus_register_type($2);
                    }
              }

/* Other miscellaneous errors */
              | type stars LPAREN { 
                     skip_decl();
                     fprintf(stderr,"%s : Line %d. Function pointers not currently supported (ignored).\n", input_file, line_number);
		     
	      }
              | strict_type LPAREN STAR {
                     skip_decl();
                     fprintf(stderr,"%s : Line %d. Function pointers not currently supported (ignored).\n", input_file, line_number);
		     
	      }
              | ID LPAREN STAR { 
                     skip_decl();
                     fprintf(stderr,"%s : Line %d. Function pointers not currently supported (ignored).\n", input_file, line_number);
		     
	      }
              | doc_enable { }
              | SEMI { }
              ;

nested_decl   : declaration { $$ = $1;}
              | empty { $$.id = 0; }
              ;

type_extra    : stars {}
              | AND {}
              | empty {}
              ; 

cpp_tail      : SEMI { }
               | COMMA declaration def_args {
		 if (allow) {
		   int oldstatus = Status;
		   char *tm;

		   init_language();
		   if (cplus_mode == CPLUS_PUBLIC) {
		     temp_typeptr = new DataType(Active_type);
		     temp_typeptr->is_pointer += $2.is_pointer;
		     if (Verbose) {
		       fprintf(stderr,"Wrapping member variable : %s\n",$2.id);
		     }
		     Stat_var++;
		     doc_entry = new DocDecl($2.id,doc_stack[doc_stack_top]);
		     if (temp_typeptr->status & STAT_READONLY) {
		       if (!(tm = typemap_lookup("memberin",typemap_lang,temp_typeptr,$2.id,"",""))) 
			 Status = Status | STAT_READONLY;
		     }
		     cplus_variable($2.id,(char *) 0,temp_typeptr);		
		     Status = oldstatus;
		     delete temp_typeptr;
		   }
		   scanner_clear_start();
		 }
	       } cpp_tail { } 
               | COMMA declaration array def_args {
		 if (allow) {
		   int oldstatus = Status;
		   char *tm;

		   init_language();
		   if (cplus_mode == CPLUS_PUBLIC) {
		     temp_typeptr = new DataType(Active_type);
		     temp_typeptr->is_pointer += $2.is_pointer;
		     if (Verbose) {
		       fprintf(stderr,"Wrapping member variable : %s\n",$2.id);
		     }
		     Stat_var++;
		     if (!(tm = typemap_lookup("memberin",typemap_lang,temp_typeptr,$2.id,"",""))) 
		       Status = Status | STAT_READONLY;
		     doc_entry = new DocDecl($2.id,doc_stack[doc_stack_top]);
		     if (temp_typeptr->status & STAT_READONLY) Status = Status | STAT_READONLY;
		     cplus_variable($2.id,(char *) 0,temp_typeptr);		
		     Status = oldstatus;
		     if (!tm)
		       fprintf(stderr,"%s : Line %d. Warning. Array member will be read-only.\n",input_file,line_number);
		     delete temp_typeptr;
		   }
		   scanner_clear_start();
		 }
	       } cpp_tail { } 
               ;

cpp_end        : cpp_const SEMI { 
                    CCode = "";
               } 
               | cpp_const LBRACE { skip_brace(); }
               ;

cpp_vend       : cpp_const SEMI { CCode = ""; }
               | cpp_const EQUAL definetype SEMI { CCode = ""; }
               | cpp_const LBRACE { skip_brace(); }
               ;

cpp_enumlist   :  cpp_enumlist COMMA cpp_edecl {}
               |  cpp_edecl {}
               ;

cpp_edecl      :  ID {
                    if (allow) {
		      if (cplus_mode == CPLUS_PUBLIC) {
			if (Verbose) {
			  fprintf(stderr,"Creating enum value %s\n", $1);
			}
			Stat_const++;
			temp_typeptr = new DataType(T_INT);
			doc_entry = new DocDecl($1,doc_stack[doc_stack_top]);
			cplus_declare_const($1, (char *) 0, temp_typeptr, (char *) 0);
			delete temp_typeptr;
			scanner_clear_start();
		      }
		    }
                  }
                 | ID EQUAL etype {
		   if (allow) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       if (Verbose) {
			 fprintf(stderr, "Creating enum value %s = %s\n", $1, $3.id);
		       }
		       Stat_const++;
		       temp_typeptr = new DataType(T_INT);
		       doc_entry = new DocDecl($1,doc_stack[doc_stack_top]);
		       cplus_declare_const($1,(char *) 0, temp_typeptr,(char *) 0);
// OLD : Bug with value     cplus_declare_const($1,(char *) 0, temp_typeptr,$3.id);
		       delete temp_typeptr;
		       scanner_clear_start();
		     }
		   }
		 }
                 | NAME LPAREN ID RPAREN ID {
		   if (allow) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       if (Verbose) {
			 fprintf(stderr,"Creating enum value %s\n", $5);
		       }
		       Stat_const++;
		       temp_typeptr = new DataType(T_INT);
		       doc_entry = new DocDecl($3,doc_stack[doc_stack_top]);
		       cplus_declare_const($5, $3, temp_typeptr, (char *) 0);
		       delete temp_typeptr;
		       scanner_clear_start();
		     }
		   }
		 }
                 | NAME LPAREN ID RPAREN ID EQUAL etype {
		   if (allow) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       if (Verbose) {
			 fprintf(stderr, "Creating enum value %s = %s\n", $5, $7.id);
		       }
		       Stat_const++;
		       temp_typeptr = new DataType(T_INT);
		       doc_entry = new DocDecl($3,doc_stack[doc_stack_top]);
		       cplus_declare_const($5,$3, temp_typeptr, (char *) 0);
// Old : bug with value	       cplus_declare_const($5,$3, temp_typeptr,$7.id);
		       delete temp_typeptr;
		       scanner_clear_start();
		     }
		   }
		 }
                 | cond_compile cpp_edecl { }
                 | empty { }
                 ;

inherit        : COLON base_list {
		   $$ = $2;
                }
                | empty {
                   $$.names = (char **) 0;
		   $$.count = 0;
                }
                ;

base_list      : base_specifier { 
                   int i;
                   $$.names = new char *[NI_NAMES];
		   $$.count = 0;
		   for (i = 0; i < NI_NAMES; i++){
		     $$.names[i] = (char *) 0;
		   }
                   if ($1) {
                       $$.names[$$.count] = copy_string($1);
                       $$.count++;
		   }
               }

               | base_list COMMA base_specifier { 
                   $$ = $1;
                   if ($3) {
		     $$.names[$$.count] = copy_string($3);
		     $$.count++;
		   }
               }
               ;
                                 
base_specifier : ID {     
                  fprintf(stderr,"%s : Line %d. No access specifier given for base class %s (ignored).\n",
			  input_file,line_number,$1);
		  $$ = (char *) 0;
               }
               | VIRTUAL ID { 
                  fprintf(stderr,"%s : Line %d. No access specifier given for base class %s (ignored).\n",
			  input_file,line_number,$2);
		  $$ = (char *) 0;
	       }
	       | VIRTUAL access_specifier ID {
		 if (strcmp($2,"public") == 0) {
		   $$ = $3;
		 } else {
		   fprintf(stderr,"%s : Line %d. %s inheritance not supported (ignored).\n",
			   input_file,line_number,$2);
		   $$ = (char *) 0;
		 }
               }
               | access_specifier ID {
		 if (strcmp($1,"public") == 0) {
		   $$ = $2;
		 } else {
		   fprintf(stderr,"%s : Line %d. %s inheritance not supported (ignored).\n",
			   input_file,line_number,$1);
		   $$ = (char *) 0;
		 }
	       }                          
               | access_specifier VIRTUAL ID {
                 if (strcmp($1,"public") == 0) {
		   $$ = $3;
		 } else {
		   fprintf(stderr,"%s : Line %d. %s inheritance not supported (ignored).\n",
			   input_file,line_number,$1);
		   $$ = (char *) 0;
		 }
               }
               ;                               

access_specifier :  PUBLIC { $$ = "public"; }
               | PRIVATE { $$ = "private"; }
               | PROTECTED { $$ = "protected"; }
               ;
              
  
cpptype        : CLASS { $$ = "class"; }
               | STRUCT { $$ = "struct"; }
               | UNION {$$ = "union"; }
               ;

cpp_const      : CONST {} 
               | THROW LPAREN parms RPAREN { delete $3;}
               | empty {}
               ;

/* Constructor initializer */

ctor_end       : cpp_const ctor_initializer SEMI { 
                    CCode = "";
               } 
               | cpp_const ctor_initializer LBRACE { skip_brace(); }
               ;

ctor_initializer : COLON mem_initializer_list {}
               | empty {}
               ;

mem_initializer_list : mem_initializer { }
               | mem_initializer_list COMMA mem_initializer { }
               ;

mem_initializer : ID LPAREN expr_list RPAREN { }
               | ID LPAREN RPAREN { }
                ;
 
expr_list      : expr { }
               | expr_list COMMA expr { }
               ;


/**************************************************************/
/* Objective-C parsing                                        */
/**************************************************************/

objective_c    : OC_INTERFACE ID objc_inherit { 
                   ObjCClass = 1;
                   init_language();
		   cplus_mode = CPLUS_PROTECTED;
		   sprintf(temp_name,"CPP_CLASS:%s\n",$2);
		   if (add_symbol(temp_name,(DataType *) 0, (char *) 0)) {
		     fprintf(stderr,"%s : Line %d.  @interface %s is multiple defined.\n",
			     input_file,line_number,$2);
		     FatalError();
		   }
		   // Create a new documentation entry
		   doc_entry = new DocClass($2,doc_parent());
		   doc_stack_top++;
		   doc_stack[doc_stack_top] = doc_entry;
		   scanner_clear_start();
		   cplus_open_class($2, (char *) 0, "");     // Open up a new C++ class
                } LBRACE objc_data RBRACE objc_methods OC_END { 
		  if ($3) {
		      char *inames[1];
		      inames[0] = $3;
		      cplus_inherit(1,inames);
		  }
		  // Restore original doc entry for this class
		  doc_entry = doc_stack[doc_stack_top];
		  cplus_class_close($2);
		  doc_entry = 0;
		  doc_stack_top--;
		  cplus_mode = CPLUS_PUBLIC;
		  ObjCClass = 0;
		  delete $2;
		  delete $3;
                }
/* An obj-c category declaration */
               | OC_INTERFACE ID LPAREN ID RPAREN objc_protolist {
                 ObjCClass = 1;
		 init_language();
                 cplus_mode = CPLUS_PROTECTED;
                 doc_entry = cplus_set_class($2);
		 if (!doc_entry) {
		   doc_entry = new DocClass($2,doc_parent());
		 }
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
	       } objc_methods OC_END {
                 cplus_unset_class();
                 doc_entry = 0;
                 doc_stack_top--;
               }
               | OC_IMPLEMENT { skip_to_end(); }
               | OC_PROTOCOL { skip_to_end(); }
               | OC_CLASS ID initlist SEMI {
		 char *iname = make_name($2);
                 init_language();
                 lang->cpp_class_decl($2,iname,"");
		 for (int i = 0; i <$3.count; i++) {
		   if ($3.names[i]) {
		     iname = make_name($3.names[i]);
		     lang->cpp_class_decl($3.names[i],iname,"");
		     delete [] $3.names[i];
		   }
		 } 
		 delete [] $3.names;
	       }
               ;

objc_inherit   : COLON ID objc_protolist { $$ = $2;}
               | objc_protolist empty { $$ = 0; }
               ;


objc_protolist : LESSTHAN { skip_template(); 
                   CCode.strip();           // Strip whitespace
		   CCode.replace("<","< ");
		   CCode.replace(">"," >");
                   $$ = CCode.get();
                 }
               | empty {
                   $$ = "";
               }
               ;
                 
objc_data      : objc_vars objc_data { }
               | OC_PUBLIC { 
                    cplus_mode = CPLUS_PUBLIC;
                 } objc_data { }
               | OC_PRIVATE {
                    cplus_mode = CPLUS_PRIVATE;
                 } objc_data { }
               | OC_PROTECTED { 
                    cplus_mode = CPLUS_PROTECTED;
                 } objc_data { }
               | error {
		 if (!Error) {
		   skip_decl();
		   {
		     static int last_error_line = -1;
		     if (last_error_line != line_number) {
		       fprintf(stderr,"%s : Line %d. Syntax error in input.\n", input_file, line_number);
		       FatalError();
		       last_error_line = line_number;
		     }
		     Error = 1;
		   }
		 }
	       } objc_data { }
               | empty { }
               ;

objc_vars      : objc_var objc_vartail SEMI {
  
                }
               ;

/* An objective-C member variable */

objc_var       : type declaration { 
                 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm;
		   char *iname;
		   if (Active_type) delete Active_type;
		   Active_type = new DataType($1);
		   $1->is_pointer += $2.is_pointer;
		   $1->is_reference = $2.is_reference;
		   if ($1->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,$1,$2.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name($2.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $2.id) iname = 0;
		   cplus_variable($2.id,iname,$1);
		   Status = oldstatus; 
		 }
		 scanner_clear_start();
		 delete $1;
               }
               | type declaration array { 
		 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm, *iname;
		   if (Active_type) delete Active_type;
		   Active_type = new DataType($1);
		   $1->is_pointer += $2.is_pointer;
		   $1->is_reference = $2.is_reference;
		   $1->arraystr = copy_string(ArrayString);
		   if ($1->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,$1,$2.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name($2.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $2.id) iname = 0;
		   cplus_variable($2.id,iname,$1);
		   Status = oldstatus; 
		 }
		 scanner_clear_start();
		 delete $1;
	       }
               | NAME LPAREN ID RPAREN {
                    strcpy(yy_rename,$3);
                    Rename_true = 1;
	       } objc_var { };

objc_vartail   : COMMA declaration objc_vartail { 
                 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm, *iname;
		   DataType *t = new DataType (Active_type);
		   t->is_pointer += $2.is_pointer;
		   t->is_reference = $2.is_reference;
		   if (t->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,t,$2.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name($2.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $2.id) iname = 0;
		   cplus_variable($2.id,iname,t);
		   Status = oldstatus; 
		   delete t;
		 }
		 scanner_clear_start();
               }
               | COMMA declaration array objc_vartail {
		 char *iname;
                 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm;
		   DataType *t = new DataType (Active_type);
		   t->is_pointer += $2.is_pointer;
		   t->is_reference = $2.is_reference;
		   t->arraystr = copy_string(ArrayString);
		   if (t->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,t,$2.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name($2.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $2.id) iname = 0;
		   cplus_variable($2.id,iname,t);
		   Status = oldstatus; 
		   delete t;
		 }
		 scanner_clear_start();
               }
               | empty { }
               ;

objc_methods   : objc_method objc_methods { };
               | ADDMETHODS LBRACE {
                   AddMethods = 1;
	       } objc_methods RBRACE {
                   AddMethods = 0;
               }
               | NAME LPAREN ID RPAREN {
                     strcpy(yy_rename,$3);
                     Rename_true = 1;
	       } objc_methods { }
               | error {
                 skip_decl();		                
		 if (!Error) {
		   {
		     static int last_error_line = -1;
		     if (last_error_line != line_number) {
		       fprintf(stderr,"%s : Line %d. Syntax error in input.\n", input_file, line_number);
		       FatalError();
		       last_error_line = line_number;
		     }
		     Error = 1;
		   }
		 }
	       } objc_methods { }
               | empty { }
               ;

objc_method    : MINUS objc_ret_type ID objc_args objc_end {
                 char *iname;
                 // An objective-C instance function
                 // This is like a C++ member function

		 if (strcmp($3,objc_destruct) == 0) {
		   // This is an objective C destructor
                   doc_entry = new DocDecl($3,doc_stack[doc_stack_top]);
                   cplus_destructor($3,(char *) 0);
		 } else {
		   iname = make_name($3);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $3) iname = 0;
		   cplus_member_func($3,iname,$2,$4,0);
		   scanner_clear_start();
		   delete $2;
		   delete $3;
		   delete $4;
		 }
               }
               | PLUS objc_ret_type ID objc_args objc_end { 
                 char *iname;
                 // An objective-C class function
                 // This is like a c++ static member function
                 if (strcmp($3,objc_construct) == 0) {
		   // This is an objective C constructor
		   doc_entry = new DocDecl($3,doc_stack[doc_stack_top]);
                   cplus_constructor($3,0,$4);
		 } else {
		   iname = make_name($3);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == $3) iname = 0;
		   cplus_static_func($3,iname,$2,$4);
		 }
                 scanner_clear_start();
                 delete $2;
                 delete $3;
                 delete $4;
               }
               ;

objc_end       : SEMI { CCode = ""; }
               | LBRACE { skip_brace(); }
               ;

objc_ret_type  : LPAREN type RPAREN { 
                  $$ = $2;
                }
               | LPAREN type stars RPAREN { 
                  $$ = $2;
                  $$->is_pointer += $3;
               }
               | empty {       /* Empty type means "id" type */
                  $$ = new DataType(T_VOID);
		  sprintf($$->name,"id");
                  $$->is_pointer = 1;
                  $$->implicit_ptr = 1;
               }
               ;

objc_arg_type  : LPAREN parm RPAREN { 
                  $$ = new DataType($2->t);
                  delete $2;
                 }
               | empty { 
                  $$ = new DataType(T_VOID);
		  sprintf($$->name,"id");
                  $$->is_pointer = 1;
                  $$->implicit_ptr = 1;
               }
               ;
 
objc_args      : objc_args objc_separator objc_arg_type ID { 
                   Parm *p= new Parm($3,$4);
		   p->objc_separator = $2;
                   $$ = $1;
                   $$->append(p);
               }
               | empty { 
                 $$ = new ParmList;
               }
               ;

objc_separator : COLON { $$ = copy_string(":"); }
               | ID COLON { $$ = new char[strlen($1)+2]; 
                    strcpy($$,$1);
		    strcat($$,":");
		    delete $1;
	        }
               ;

/* Miscellaneous stuff */

/* Documentation style list */

stylelist      : ID stylearg styletail {
                    $$ = $3;
		    $$.names[$$.count] = copy_string($1);
		    $$.values[$$.count] = copy_string($2);
		    format_string($$.values[$$.count]);
		    $$.count++;
                 }
               ;


styletail      : styletail COMMA ID stylearg {
                    $$ = $1;
		    $$.names[$$.count] = copy_string($3);
		    $$.values[$$.count] = copy_string($4);
		    format_string($$.values[$$.count]);
		    $$.count++;
                 }
               | empty {
                    $$.names = new char *[NI_NAMES];
		    $$.values = new char *[NI_NAMES];
		    $$.count = 0;
	       }
               ;

stylearg       : EQUAL NUM_INT {
                     $$ = $2;
                 }
               | EQUAL STRING {
                     $$ = $2;
	       }
               | empty { 
                     $$ = 0;
                }
               ;


/* --------------------------------------------------------------
 * Type-map parameters 
 * -------------------------------------------------------------- */

tm_method      : ID {
                 $$ = $1;
               } 
               | CONST {
                 $$ = copy_string("const");
               }
               ;

tm_list        : typemap_parm tm_tail {
                 $$ = $1;
                 $$->next = $2;
		}
               ;

tm_tail        : COMMA typemap_parm tm_tail {
                 $$ = $2;
                 $$->next = $3;
                }
               | empty { $$ = 0;}
               ;

typemap_parm   : type typemap_name {
		    if (InArray) {
		      $1->is_pointer++;
		      $1->arraystr = copy_string(ArrayString);
		    }
		    $$ = new TMParm;
                    $$->p = new Parm($1,$2);
		    $$->p->call_type = 0;
		    $$->args = tm_parm;
		    delete $1;
		    delete $2;
                 }

                | type stars typemap_name {
		  $$ = new TMParm;
		   $$->p = new Parm($1,$3);
		   $$->p->t->is_pointer += $2;
		   $$->p->call_type = 0;
		   if (InArray) {
		     $$->p->t->is_pointer++;
		     $$->p->t->arraystr = copy_string(ArrayString);
		    }
		   $$->args = tm_parm;
		   delete $1;
		   delete $3;
		}

                | type AND typemap_name {
                  $$ = new TMParm;
		  $$->p = new Parm($1,$3);
		  $$->p->t->is_reference = 1;
		  $$->p->call_type = 0;
		  $$->p->t->is_pointer++;
		  if (!CPlusPlus) {
			fprintf(stderr,"%s : Line %d. Warning.  Use of C++ Reference detected.  Use the -c++ option.\n", input_file, line_number);
		  }
		  $$->args = tm_parm;
		  delete $1;
		  delete $3;
		}
                | type LPAREN stars typemap_name RPAREN LPAREN parms RPAREN {
                  fprintf(stderr,"%s : Line %d. Error. Function pointer not allowed (remap with typedef).\n", input_file, line_number);
		  FatalError();
                  $$ = new TMParm;
		  $$->p = new Parm($1,$4);
		  $$->p->t->type = T_ERROR;
		  $$->p->name = copy_string($4);
		  strcpy($$->p->t->name,"<function ptr>");
		  $$->args = tm_parm;
		  delete $1;
		  delete $4;
		  delete $7;
		}
		;

typemap_name    : ID typemap_args {
                    $$ = $1; 
                    InArray = 0;
                }
                | ID array  { 
                    ArrayBackup = "";
		    ArrayBackup << ArrayString;
                  } typemap_args {
                    $$ = $1;
                    InArray = $2;
                    ArrayString = "";
		    ArrayString << ArrayBackup;
                }
                | array { 
                    ArrayBackup = "";
		    ArrayBackup << ArrayString;
		} typemap_args {
		    $$ = new char[1];
		    $$[0] = 0;
		    InArray = $1;
                    ArrayString = "";
                    ArrayString << ArrayBackup;
		}
                | typemap_args { $$ = new char[1];
  	                  $$[0] = 0;
                          InArray = 0;
                }
                ;

typemap_args    : LPAREN parms RPAREN {
                  tm_parm = $2;
                }
                | empty {
                  tm_parm = 0;
                }
                ;

idstring       : ID {$$ = $1;}
               | STRING { $$ = $1;}
               ;


/* User defined directive */

user_directive : USERDIRECTIVE LPAREN parms RPAREN uservalue { }
               | USERDIRECTIVE uservalue { }
               ;

uservalue      : ID SEMI { }
               | STRING SEMI { }
               | LBRACE RBRACE { }
               ;
 
                     
                          
/* Parsing of expressions, but only for throw away code */

/* Might need someday 
dummyexpr      :  NUM_INT { }
               |  NUM_FLOAT { }
               |  NUM_UNSIGNED { }
               |  NUM_LONG { } 
               |  NUM_ULONG { }
               |  SIZEOF LPAREN type RPAREN { }
               |  ID { }
               |  dummyexpr PLUS dummyexpr {	       }
               |  dummyexpr MINUS dummyexpr {	       }
               |  dummyexpr STAR dummyexpr {	       }
               |  dummyexpr SLASH dummyexpr {	       }
               |  dummyexpr AND dummyexpr {	       }
               |  dummyexpr OR dummyexpr {	       }
               |  dummyexpr XOR dummyexpr {	       }
               |  dummyexpr LSHIFT dummyexpr {	       }
               |  dummyexpr RSHIFT dummyexpr {	       }
               |  MINUS dummyexpr %prec UMINUS {	       }
               |  NOT dummyexpr {	       }
               |  LPAREN dummyexpr RPAREN {	       }
               ;

	       */


empty          :   ;

%%

void error_recover() {
  int c;
  c = yylex();
  while ((c > 0) && (c != SEMI)) 
    c = yylex();
}

/* Called by the parser (yyparse) when an error is found.*/
void yyerror (char *) {
  //  Fprintf(stderr,"%s : Line %d. Syntax error.\n", input_file, line_number);
  //  error_recover();
}

