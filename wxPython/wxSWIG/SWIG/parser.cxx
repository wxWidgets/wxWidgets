
/*  A Bison parser, made from parser.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ID	258
#define	HBLOCK	259
#define	WRAPPER	260
#define	POUND	261
#define	STRING	262
#define	NUM_INT	263
#define	NUM_FLOAT	264
#define	CHARCONST	265
#define	NUM_UNSIGNED	266
#define	NUM_LONG	267
#define	NUM_ULONG	268
#define	TYPEDEF	269
#define	TYPE_INT	270
#define	TYPE_UNSIGNED	271
#define	TYPE_SHORT	272
#define	TYPE_LONG	273
#define	TYPE_FLOAT	274
#define	TYPE_DOUBLE	275
#define	TYPE_CHAR	276
#define	TYPE_VOID	277
#define	TYPE_SIGNED	278
#define	TYPE_BOOL	279
#define	TYPE_TYPEDEF	280
#define	LPAREN	281
#define	RPAREN	282
#define	COMMA	283
#define	SEMI	284
#define	EXTERN	285
#define	INIT	286
#define	LBRACE	287
#define	RBRACE	288
#define	DEFINE	289
#define	PERIOD	290
#define	CONST	291
#define	STRUCT	292
#define	UNION	293
#define	EQUAL	294
#define	SIZEOF	295
#define	MODULE	296
#define	LBRACKET	297
#define	RBRACKET	298
#define	WEXTERN	299
#define	ILLEGAL	300
#define	READONLY	301
#define	READWRITE	302
#define	NAME	303
#define	RENAME	304
#define	INCLUDE	305
#define	CHECKOUT	306
#define	ADDMETHODS	307
#define	PRAGMA	308
#define	CVALUE	309
#define	COUT	310
#define	ENUM	311
#define	ENDDEF	312
#define	MACRO	313
#define	CLASS	314
#define	PRIVATE	315
#define	PUBLIC	316
#define	PROTECTED	317
#define	COLON	318
#define	STATIC	319
#define	VIRTUAL	320
#define	FRIEND	321
#define	OPERATOR	322
#define	THROW	323
#define	TEMPLATE	324
#define	NATIVE	325
#define	INLINE	326
#define	IFDEF	327
#define	IFNDEF	328
#define	ENDIF	329
#define	ELSE	330
#define	UNDEF	331
#define	IF	332
#define	DEFINED	333
#define	ELIF	334
#define	RAW_MODE	335
#define	ALPHA_MODE	336
#define	TEXT	337
#define	DOC_DISABLE	338
#define	DOC_ENABLE	339
#define	STYLE	340
#define	LOCALSTYLE	341
#define	TYPEMAP	342
#define	EXCEPT	343
#define	IMPORT	344
#define	ECHO	345
#define	NEW	346
#define	APPLY	347
#define	CLEAR	348
#define	DOCONLY	349
#define	TITLE	350
#define	SECTION	351
#define	SUBSECTION	352
#define	SUBSUBSECTION	353
#define	LESSTHAN	354
#define	GREATERTHAN	355
#define	USERDIRECTIVE	356
#define	OC_INTERFACE	357
#define	OC_END	358
#define	OC_PUBLIC	359
#define	OC_PRIVATE	360
#define	OC_PROTECTED	361
#define	OC_CLASS	362
#define	OC_IMPLEMENT	363
#define	OC_PROTOCOL	364
#define	OR	365
#define	XOR	366
#define	AND	367
#define	LSHIFT	368
#define	RSHIFT	369
#define	PLUS	370
#define	MINUS	371
#define	STAR	372
#define	SLASH	373
#define	UMINUS	374
#define	NOT	375
#define	LNOT	376
#define	DCOLON	377

#line 1 "parser.y"

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


#line 475 "parser.y"
typedef union {         
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
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		907
#define	YYFLAG		-32768
#define	YYNTBASE	123

#define YYTRANSLATE(x) ((unsigned)(x) <= 377 ? yytranslate[x] : 258)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,   118,   119,   120,   121,   122
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     7,     9,    12,    15,    18,    21,    23,
    24,    32,    37,    38,    46,    51,    52,    62,    70,    71,
    80,    88,    96,    97,   107,   109,   111,   116,   121,   122,
   126,   127,   133,   141,   153,   157,   161,   165,   169,   171,
   173,   175,   178,   180,   182,   185,   188,   191,   194,   197,
   199,   203,   207,   211,   214,   217,   218,   227,   228,   229,
   240,   249,   256,   265,   272,   283,   292,   298,   302,   308,
   311,   317,   320,   322,   324,   326,   328,   334,   336,   338,
   341,   344,   346,   348,   350,   351,   357,   368,   380,   381,
   388,   392,   396,   398,   401,   404,   406,   408,   411,   414,
   419,   422,   425,   433,   437,   444,   446,   447,   454,   455,
   464,   467,   469,   472,   474,   476,   479,   482,   485,   487,
   491,   493,   495,   498,   501,   505,   509,   518,   522,   525,
   528,   530,   532,   535,   539,   542,   545,   547,   549,   551,
   554,   556,   558,   561,   564,   567,   570,   574,   579,   581,
   583,   585,   588,   591,   593,   595,   597,   599,   601,   604,
   607,   610,   613,   616,   619,   623,   626,   629,   631,   634,
   637,   639,   641,   643,   645,   647,   650,   653,   656,   659,
   662,   664,   666,   669,   672,   674,   676,   678,   681,   684,
   686,   688,   690,   691,   694,   696,   698,   702,   704,   706,
   708,   712,   714,   716,   717,   722,   725,   727,   729,   731,
   733,   735,   737,   739,   741,   746,   751,   753,   757,   761,
   765,   769,   773,   777,   781,   785,   789,   793,   796,   799,
   803,   805,   807,   808,   817,   818,   819,   831,   832,   833,
   843,   848,   858,   865,   871,   873,   874,   881,   884,   887,
   889,   892,   893,   894,   902,   903,   907,   909,   916,   924,
   930,   937,   944,   945,   951,   956,   957,   963,   971,   974,
   977,   980,   985,   986,   990,   991,   999,  1001,  1003,  1005,
  1009,  1011,  1013,  1015,  1019,  1026,  1027,  1034,  1035,  1041,
  1045,  1049,  1053,  1057,  1059,  1061,  1063,  1065,  1067,  1069,
  1071,  1073,  1074,  1080,  1081,  1088,  1091,  1094,  1097,  1102,
  1105,  1109,  1111,  1113,  1117,  1123,  1131,  1134,  1136,  1139,
  1141,  1143,  1147,  1149,  1152,  1156,  1159,  1163,  1165,  1167,
  1169,  1171,  1173,  1175,  1177,  1182,  1184,  1188,  1192,  1195,
  1197,  1199,  1203,  1208,  1212,  1214,  1218,  1219,  1229,  1230,
  1240,  1242,  1244,  1249,  1253,  1256,  1258,  1260,  1263,  1264,
  1268,  1269,  1273,  1274,  1278,  1279,  1283,  1285,  1289,  1292,
  1296,  1297,  1304,  1308,  1313,  1315,  1318,  1319,  1325,  1326,
  1333,  1334,  1338,  1340,  1346,  1352,  1354,  1356,  1360,  1365,
  1367,  1371,  1373,  1378,  1380,  1382,  1385,  1389,  1394,  1396,
  1399,  1402,  1404,  1406,  1408,  1411,  1415,  1417,  1420,  1424,
  1428,  1437,  1440,  1441,  1446,  1447,  1451,  1453,  1457,  1459,
  1461,  1463,  1469,  1472,  1475,  1478,  1481
};

static const short yyrhs[] = {    -1,
   124,   125,     0,   125,   126,     0,   257,     0,    50,   254,
     0,    44,   254,     0,    89,   254,     0,    51,   254,     0,
     6,     0,     0,   149,   163,   159,   162,   156,   127,   145,
     0,   149,   164,    26,   117,     0,     0,    64,   163,   159,
   162,   156,   128,   145,     0,    64,   164,    26,   117,     0,
     0,   149,   163,   159,    26,   151,    27,   213,   129,   145,
     0,   149,   163,   159,    26,   151,    27,   150,     0,     0,
   149,   159,    26,   151,    27,   213,   130,   145,     0,    64,
   163,   159,    26,   151,    27,   150,     0,    71,   163,   159,
    26,   151,    27,   150,     0,     0,    64,   163,   159,    26,
   151,    27,   213,   131,   145,     0,    46,     0,    47,     0,
    48,    26,     3,    27,     0,    49,     3,     3,    29,     0,
     0,    91,   132,   126,     0,     0,    48,    26,    27,   133,
   177,     0,    70,    26,     3,    27,   149,     3,    29,     0,
    70,    26,     3,    27,   149,   163,   159,    26,   151,    27,
    29,     0,    95,     7,   244,     0,    96,     7,   244,     0,
    97,     7,   244,     0,    98,     7,   244,     0,    81,     0,
    80,     0,   137,     0,    82,     4,     0,   138,     0,     4,
     0,     5,     4,     0,    31,     4,     0,    71,     4,     0,
    90,     4,     0,    90,     7,     0,    94,     0,    31,     3,
   170,     0,    41,     3,   170,     0,    34,     3,   148,     0,
    34,    58,     0,    76,     3,     0,     0,   149,    56,   171,
    32,   134,   172,    33,    29,     0,     0,     0,    14,    56,
   171,    32,   135,   172,    33,     3,   136,   141,     0,    87,
    26,     3,    28,   246,    27,   247,    32,     0,    87,    26,
   246,    27,   247,    32,     0,    87,    26,     3,    28,   246,
    27,   247,    29,     0,    87,    26,   246,    27,   247,    29,
     0,    87,    26,     3,    28,   246,    27,   247,    39,   249,
    29,     0,    87,    26,   246,    27,   247,    39,   249,    29,
     0,    92,   249,    32,   247,    33,     0,    93,   247,    29,
     0,    88,    26,     3,    27,    32,     0,    88,    32,     0,
    88,    26,     3,    27,    29,     0,    88,    29,     0,    29,
     0,   177,     0,   219,     0,     1,     0,    30,     7,    32,
   125,    33,     0,   142,     0,   144,     0,    85,   243,     0,
    86,   243,     0,   255,     0,    83,     0,    84,     0,     0,
    14,   163,   159,   139,   141,     0,    14,   163,    26,   117,
   155,    27,    26,   151,    27,    29,     0,    14,   163,   160,
    26,   117,   155,    27,    26,   151,    27,    29,     0,     0,
    14,   163,   159,   161,   140,   141,     0,    28,   159,   141,
     0,    28,   159,   161,     0,   257,     0,    72,     3,     0,
    73,     3,     0,    75,     0,    74,     0,    77,   143,     0,
    79,   143,     0,    78,    26,     3,    27,     0,    78,     3,
     0,   121,   143,     0,    53,    26,     3,    28,     3,   245,
    27,     0,    53,     3,   245,     0,    53,    26,     3,    27,
     3,   245,     0,    29,     0,     0,    28,   159,   162,   156,
   146,   145,     0,     0,    28,   159,    26,   151,    27,   213,
   147,   145,     0,   168,    57,     0,    57,     0,     1,    57,
     0,    30,     0,   257,     0,    30,     7,     0,   213,    32,
     0,   153,   152,     0,   257,     0,    28,   153,   152,     0,
   257,     0,   154,     0,   158,   154,     0,   163,   155,     0,
   163,   160,   155,     0,   163,   112,   155,     0,   163,    26,
   160,   155,    27,    26,   151,    27,     0,    35,    35,    35,
     0,     3,   156,     0,     3,   161,     0,   161,     0,   257,
     0,    39,   168,     0,    39,   112,     3,     0,    39,    32,
     0,    63,     8,     0,   257,     0,    54,     0,    55,     0,
   158,   157,     0,   157,     0,     3,     0,   160,     3,     0,
   112,     3,     0,   117,   257,     0,   117,   160,     0,    42,
    43,   162,     0,    42,   176,    43,   162,     0,   161,     0,
   257,     0,    15,     0,    17,   167,     0,    18,   167,     0,
    21,     0,    24,     0,    19,     0,    20,     0,    22,     0,
    23,   165,     0,    16,   166,     0,    25,   223,     0,     3,
   223,     0,    36,   163,     0,   212,     3,     0,     3,   122,
     3,     0,   122,     3,     0,    56,     3,     0,    15,     0,
    17,   167,     0,    18,   167,     0,    21,     0,    24,     0,
    19,     0,    20,     0,    22,     0,    23,   165,     0,    16,
   166,     0,    25,   223,     0,    36,   163,     0,   212,     3,
     0,   257,     0,    15,     0,    17,   167,     0,    18,   167,
     0,    21,     0,   257,     0,    15,     0,    17,   167,     0,
    18,   167,     0,    21,     0,    15,     0,   257,     0,     0,
   169,   176,     0,     7,     0,    10,     0,   170,    28,     3,
     0,   257,     0,     3,     0,   257,     0,   172,    28,   173,
     0,   173,     0,     3,     0,     0,     3,    39,   174,   175,
     0,   142,   173,     0,   257,     0,   176,     0,    10,     0,
     8,     0,     9,     0,    11,     0,    12,     0,    13,     0,
    40,    26,   163,    27,     0,    26,   164,    27,   176,     0,
     3,     0,     3,   122,     3,     0,   176,   115,   176,     0,
   176,   116,   176,     0,   176,   117,   176,     0,   176,   118,
   176,     0,   176,   112,   176,     0,   176,   110,   176,     0,
   176,   111,   176,     0,   176,   113,   176,     0,   176,   114,
   176,     0,   116,   176,     0,   120,   176,     0,    26,   176,
    27,     0,   178,     0,   184,     0,     0,   149,   212,     3,
   208,    32,   179,   187,    33,     0,     0,     0,    14,   212,
     3,   208,    32,   180,   187,    33,   159,   181,   141,     0,
     0,     0,    14,   212,    32,   182,   187,    33,   159,   183,
   141,     0,   149,   212,     3,    29,     0,   149,   163,   159,
   122,     3,    26,   151,    27,    29,     0,   149,   163,   159,
   122,     3,    29,     0,   149,   163,   159,   122,    67,     0,
    69,     0,     0,    52,     3,    32,   185,   186,    33,     0,
   191,   187,     0,   237,   233,     0,   257,     0,   191,   187,
     0,     0,     0,    52,    32,   188,   187,    33,   189,   187,
     0,     0,     1,   190,   187,     0,   257,     0,   163,   159,
    26,   151,    27,   204,     0,    65,   163,   159,    26,   151,
    27,   205,     0,     3,    26,   151,    27,   214,     0,   120,
     3,    26,   151,    27,   204,     0,    65,   120,     3,    26,
    27,   204,     0,     0,   163,   159,   156,   192,   201,     0,
   163,   159,   161,   156,     0,     0,    64,   163,   159,   193,
   201,     0,    64,   163,   159,    26,   151,    27,   204,     0,
    61,    63,     0,    60,    63,     0,    62,    63,     0,    48,
    26,     3,    27,     0,     0,    91,   194,   191,     0,     0,
    56,   171,    32,   195,   206,    33,    29,     0,    46,     0,
    47,     0,    66,     0,   163,   200,    67,     0,   142,     0,
   138,     0,   196,     0,    53,     3,   245,     0,    53,    26,
     3,    27,     3,   245,     0,     0,   212,     3,    32,   197,
   199,    29,     0,     0,   212,    32,   198,   159,    29,     0,
   212,     3,    29,     0,   163,   160,    26,     0,   164,    26,
   117,     0,     3,    26,   117,     0,   137,     0,    29,     0,
   159,     0,   257,     0,   160,     0,   112,     0,   257,     0,
    29,     0,     0,    28,   159,   156,   202,   201,     0,     0,
    28,   159,   161,   156,   203,   201,     0,   213,    29,     0,
   213,    32,     0,   213,    29,     0,   213,    39,   168,    29,
     0,   213,    32,     0,   206,    28,   207,     0,   207,     0,
     3,     0,     3,    39,   175,     0,    48,    26,     3,    27,
     3,     0,    48,    26,     3,    27,     3,    39,   175,     0,
   142,   207,     0,   257,     0,    63,   209,     0,   257,     0,
   210,     0,   209,    28,   210,     0,     3,     0,    65,     3,
     0,    65,   211,     3,     0,   211,     3,     0,   211,    65,
     3,     0,    61,     0,    60,     0,    62,     0,    59,     0,
    37,     0,    38,     0,    36,     0,    68,    26,   151,    27,
     0,   257,     0,   213,   215,    29,     0,   213,   215,    32,
     0,    63,   216,     0,   257,     0,   217,     0,   216,    28,
   217,     0,     3,    26,   218,    27,     0,     3,    26,    27,
     0,   176,     0,   218,    28,   176,     0,     0,   102,     3,
   222,   220,    32,   224,    33,   233,   103,     0,     0,   102,
     3,    26,     3,    27,   223,   221,   233,   103,     0,   108,
     0,   109,     0,   107,     3,   170,    29,     0,    63,     3,
   223,     0,   223,   257,     0,    99,     0,   257,     0,   229,
   224,     0,     0,   104,   225,   224,     0,     0,   105,   226,
   224,     0,     0,   106,   227,   224,     0,     0,     1,   228,
   224,     0,   257,     0,   230,   232,    29,     0,   163,   159,
     0,   163,   159,   161,     0,     0,    48,    26,     3,    27,
   231,   230,     0,    28,   159,   232,     0,    28,   159,   161,
   232,     0,   257,     0,   237,   233,     0,     0,    52,    32,
   234,   233,    33,     0,     0,    48,    26,     3,    27,   235,
   233,     0,     0,     1,   236,   233,     0,   257,     0,   116,
   239,     3,   241,   238,     0,   115,   239,     3,   241,   238,
     0,    29,     0,    32,     0,    26,   163,    27,     0,    26,
   163,   160,    27,     0,   257,     0,    26,   153,    27,     0,
   257,     0,   241,   242,   240,     3,     0,   257,     0,    63,
     0,     3,    63,     0,     3,   245,   244,     0,   244,    28,
     3,   245,     0,   257,     0,    39,     8,     0,    39,     7,
     0,   257,     0,     3,     0,    36,     0,   249,   248,     0,
    28,   249,   248,     0,   257,     0,   163,   250,     0,   163,
   160,   250,     0,   163,   112,   250,     0,   163,    26,   160,
   250,    27,    26,   151,    27,     0,     3,   253,     0,     0,
     3,   161,   251,   253,     0,     0,   161,   252,   253,     0,
   253,     0,    26,   151,    27,     0,   257,     0,     3,     0,
     7,     0,   101,    26,   151,    27,   256,     0,   101,   256,
     0,     3,    29,     0,     7,    29,     0,    32,    33,     0,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   559,   568,   582,   586,   590,   601,   618,   636,   646,   657,
   684,   688,   696,   702,   708,   717,   729,   733,   746,   755,
   759,   774,   797,   806,   812,   819,   825,   833,   842,   844,
   850,   856,   862,   874,   895,   943,   973,  1009,  1046,  1054,
  1062,  1066,  1075,  1079,  1090,  1100,  1109,  1119,  1125,  1132,
  1138,  1160,  1176,  1195,  1202,  1208,  1208,  1223,  1223,  1233,
  1243,  1256,  1275,  1287,  1305,  1320,  1341,  1352,  1369,  1376,
  1383,  1388,  1394,  1395,  1396,  1397,  1415,  1416,  1420,  1424,
  1440,  1453,  1459,  1473,  1492,  1504,  1508,  1530,  1554,  1570,
  1583,  1595,  1606,  1626,  1652,  1675,  1694,  1704,  1730,  1759,
  1768,  1775,  1781,  1789,  1793,  1801,  1802,  1828,  1829,  1839,
  1842,  1845,  1848,  1856,  1857,  1858,  1870,  1879,  1885,  1888,
  1893,  1896,  1901,  1916,  1942,  1961,  1973,  1984,  1994,  2003,
  2008,  2014,  2021,  2022,  2028,  2032,  2034,  2037,  2038,  2041,
  2044,  2051,  2055,  2060,  2070,  2071,  2075,  2079,  2086,  2089,
  2097,  2100,  2103,  2106,  2109,  2112,  2115,  2118,  2121,  2125,
  2129,  2140,  2155,  2160,  2165,  2174,  2180,  2190,  2193,  2196,
  2199,  2202,  2205,  2208,  2211,  2214,  2218,  2222,  2226,  2231,
  2240,  2243,  2249,  2255,  2261,  2271,  2274,  2280,  2286,  2292,
  2300,  2301,  2304,  2304,  2310,  2317,  2329,  2335,  2345,  2346,
  2352,  2353,  2357,  2362,  2362,  2369,  2370,  2373,  2385,  2396,
  2400,  2404,  2408,  2412,  2416,  2421,  2426,  2438,  2445,  2451,
  2457,  2464,  2471,  2482,  2494,  2506,  2518,  2530,  2537,  2547,
  2558,  2559,  2562,  2596,  2633,  2667,  2730,  2734,  2757,  2793,
  2796,  2809,  2830,  2850,  2858,  2866,  2876,  2884,  2885,  2886,
  2889,  2890,  2892,  2894,  2895,  2905,  2906,  2909,  2933,  2956,
  2977,  2997,  3017,  3068,  3070,  3101,  3120,  3124,  3144,  3155,
  3166,  3177,  3185,  3187,  3192,  3192,  3210,  3215,  3221,  3229,
  3235,  3240,  3244,  3249,  3252,  3275,  3276,  3301,  3302,  3326,
  3333,  3338,  3343,  3348,  3349,  3352,  3353,  3356,  3357,  3358,
  3361,  3362,  3386,  3387,  3412,  3415,  3418,  3421,  3422,  3423,
  3426,  3427,  3430,  3445,  3461,  3476,  3492,  3493,  3496,  3499,
  3505,  3518,  3527,  3532,  3537,  3546,  3555,  3566,  3567,  3568,
  3572,  3573,  3574,  3577,  3578,  3579,  3584,  3587,  3590,  3591,
  3594,  3595,  3598,  3599,  3602,  3603,  3611,  3627,  3644,  3655,
  3660,  3661,  3662,  3677,  3678,  3682,  3688,  3693,  3694,  3696,
  3697,  3699,  3700,  3702,  3703,  3716,  3717,  3720,  3727,  3749,
  3771,  3774,  3776,  3796,  3818,  3821,  3822,  3824,  3827,  3830,
  3831,  3844,  3845,  3848,  3868,  3889,  3890,  3893,  3896,  3900,
  3908,  3912,  3920,  3926,  3931,  3932,  3943,  3953,  3960,  3967,
  3970,  3973,  3983,  3986,  3991,  3997,  4001,  4004,  4017,  4031,
  4044,  4059,  4063,  4066,  4072,  4075,  4082,  4088,  4091,  4096,
  4097,  4103,  4104,  4107,  4108,  4109,  4141
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","ID","HBLOCK",
"WRAPPER","POUND","STRING","NUM_INT","NUM_FLOAT","CHARCONST","NUM_UNSIGNED",
"NUM_LONG","NUM_ULONG","TYPEDEF","TYPE_INT","TYPE_UNSIGNED","TYPE_SHORT","TYPE_LONG",
"TYPE_FLOAT","TYPE_DOUBLE","TYPE_CHAR","TYPE_VOID","TYPE_SIGNED","TYPE_BOOL",
"TYPE_TYPEDEF","LPAREN","RPAREN","COMMA","SEMI","EXTERN","INIT","LBRACE","RBRACE",
"DEFINE","PERIOD","CONST","STRUCT","UNION","EQUAL","SIZEOF","MODULE","LBRACKET",
"RBRACKET","WEXTERN","ILLEGAL","READONLY","READWRITE","NAME","RENAME","INCLUDE",
"CHECKOUT","ADDMETHODS","PRAGMA","CVALUE","COUT","ENUM","ENDDEF","MACRO","CLASS",
"PRIVATE","PUBLIC","PROTECTED","COLON","STATIC","VIRTUAL","FRIEND","OPERATOR",
"THROW","TEMPLATE","NATIVE","INLINE","IFDEF","IFNDEF","ENDIF","ELSE","UNDEF",
"IF","DEFINED","ELIF","RAW_MODE","ALPHA_MODE","TEXT","DOC_DISABLE","DOC_ENABLE",
"STYLE","LOCALSTYLE","TYPEMAP","EXCEPT","IMPORT","ECHO","NEW","APPLY","CLEAR",
"DOCONLY","TITLE","SECTION","SUBSECTION","SUBSUBSECTION","LESSTHAN","GREATERTHAN",
"USERDIRECTIVE","OC_INTERFACE","OC_END","OC_PUBLIC","OC_PRIVATE","OC_PROTECTED",
"OC_CLASS","OC_IMPLEMENT","OC_PROTOCOL","OR","XOR","AND","LSHIFT","RSHIFT","PLUS",
"MINUS","STAR","SLASH","UMINUS","NOT","LNOT","DCOLON","program","@1","command",
"statement","@2","@3","@4","@5","@6","@7","@8","@9","@10","@11","doc_enable",
"typedef_decl","@12","@13","typedeflist","cond_compile","cpp_const_expr","pragma",
"stail","@14","@15","definetail","extern","func_end","parms","ptail","parm",
"parm_type","pname","def_args","parm_specifier","parm_specifier_list","declaration",
"stars","array","array2","type","strict_type","opt_signed","opt_unsigned","opt_int",
"definetype","@16","initlist","ename","enumlist","edecl","@17","etype","expr",
"cpp","cpp_class","@18","@19","@20","@21","@22","cpp_other","@23","added_members",
"cpp_members","@24","@25","@26","cpp_member","@27","@28","@29","@30","cpp_pragma",
"@31","@32","nested_decl","type_extra","cpp_tail","@33","@34","cpp_end","cpp_vend",
"cpp_enumlist","cpp_edecl","inherit","base_list","base_specifier","access_specifier",
"cpptype","cpp_const","ctor_end","ctor_initializer","mem_initializer_list","mem_initializer",
"expr_list","objective_c","@35","@36","objc_inherit","objc_protolist","objc_data",
"@37","@38","@39","@40","objc_vars","objc_var","@41","objc_vartail","objc_methods",
"@42","@43","@44","objc_method","objc_end","objc_ret_type","objc_arg_type","objc_args",
"objc_separator","stylelist","styletail","stylearg","tm_method","tm_list","tm_tail",
"typemap_parm","typemap_name","@45","@46","typemap_args","idstring","user_directive",
"uservalue","empty", NULL
};
#endif

static const short yyr1[] = {     0,
   124,   123,   125,   125,   126,   126,   126,   126,   126,   127,
   126,   126,   128,   126,   126,   129,   126,   126,   130,   126,
   126,   126,   131,   126,   126,   126,   126,   126,   132,   126,
   133,   126,   126,   126,   126,   126,   126,   126,   126,   126,
   126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
   126,   126,   126,   126,   126,   134,   126,   135,   136,   126,
   126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
   126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
   126,   126,   137,   137,   139,   138,   138,   138,   140,   138,
   141,   141,   141,   142,   142,   142,   142,   142,   142,   143,
   143,   143,   144,   144,   144,   145,   146,   145,   147,   145,
   148,   148,   148,   149,   149,   149,   150,   151,   151,   152,
   152,   153,   153,   154,   154,   154,   154,   154,   155,   155,
   155,   155,   156,   156,   156,   156,   156,   157,   157,   158,
   158,   159,   159,   159,   160,   160,   161,   161,   162,   162,
   163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
   163,   163,   163,   163,   163,   163,   163,   164,   164,   164,
   164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
   165,   165,   165,   165,   165,   166,   166,   166,   166,   166,
   167,   167,   169,   168,   168,   168,   170,   170,   171,   171,
   172,   172,   173,   174,   173,   173,   173,   175,   175,   176,
   176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
   176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
   177,   177,   179,   178,   180,   181,   178,   182,   183,   178,
   184,   184,   184,   184,   184,   185,   184,   186,   186,   186,
   187,   188,   189,   187,   190,   187,   187,   191,   191,   191,
   191,   191,   192,   191,   191,   193,   191,   191,   191,   191,
   191,   191,   194,   191,   195,   191,   191,   191,   191,   191,
   191,   191,   191,   196,   196,   197,   196,   198,   196,   196,
   196,   196,   196,   196,   196,   199,   199,   200,   200,   200,
   201,   202,   201,   203,   201,   204,   204,   205,   205,   205,
   206,   206,   207,   207,   207,   207,   207,   207,   208,   208,
   209,   209,   210,   210,   210,   210,   210,   211,   211,   211,
   212,   212,   212,   213,   213,   213,   214,   214,   215,   215,
   216,   216,   217,   217,   218,   218,   220,   219,   221,   219,
   219,   219,   219,   222,   222,   223,   223,   224,   225,   224,
   226,   224,   227,   224,   228,   224,   224,   229,   230,   230,
   231,   230,   232,   232,   232,   233,   234,   233,   235,   233,
   236,   233,   233,   237,   237,   238,   238,   239,   239,   239,
   240,   240,   241,   241,   242,   242,   243,   244,   244,   245,
   245,   245,   246,   246,   247,   248,   248,   249,   249,   249,
   249,   250,   251,   250,   252,   250,   250,   253,   253,   254,
   254,   255,   255,   256,   256,   256,   257
};

static const short yyr2[] = {     0,
     0,     2,     2,     1,     2,     2,     2,     2,     1,     0,
     7,     4,     0,     7,     4,     0,     9,     7,     0,     8,
     7,     7,     0,     9,     1,     1,     4,     4,     0,     3,
     0,     5,     7,    11,     3,     3,     3,     3,     1,     1,
     1,     2,     1,     1,     2,     2,     2,     2,     2,     1,
     3,     3,     3,     2,     2,     0,     8,     0,     0,    10,
     8,     6,     8,     6,    10,     8,     5,     3,     5,     2,
     5,     2,     1,     1,     1,     1,     5,     1,     1,     2,
     2,     1,     1,     1,     0,     5,    10,    11,     0,     6,
     3,     3,     1,     2,     2,     1,     1,     2,     2,     4,
     2,     2,     7,     3,     6,     1,     0,     6,     0,     8,
     2,     1,     2,     1,     1,     2,     2,     2,     1,     3,
     1,     1,     2,     2,     3,     3,     8,     3,     2,     2,
     1,     1,     2,     3,     2,     2,     1,     1,     1,     2,
     1,     1,     2,     2,     2,     2,     3,     4,     1,     1,
     1,     2,     2,     1,     1,     1,     1,     1,     2,     2,
     2,     2,     2,     2,     3,     2,     2,     1,     2,     2,
     1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
     1,     1,     2,     2,     1,     1,     1,     2,     2,     1,
     1,     1,     0,     2,     1,     1,     3,     1,     1,     1,
     3,     1,     1,     0,     4,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     4,     4,     1,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
     1,     1,     0,     8,     0,     0,    11,     0,     0,     9,
     4,     9,     6,     5,     1,     0,     6,     2,     2,     1,
     2,     0,     0,     7,     0,     3,     1,     6,     7,     5,
     6,     6,     0,     5,     4,     0,     5,     7,     2,     2,
     2,     4,     0,     3,     0,     7,     1,     1,     1,     3,
     1,     1,     1,     3,     6,     0,     6,     0,     5,     3,
     3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     0,     5,     0,     6,     2,     2,     2,     4,     2,
     3,     1,     1,     3,     5,     7,     2,     1,     2,     1,
     1,     3,     1,     2,     3,     2,     3,     1,     1,     1,
     1,     1,     1,     1,     4,     1,     3,     3,     2,     1,
     1,     3,     4,     3,     1,     3,     0,     9,     0,     9,
     1,     1,     4,     3,     2,     1,     1,     2,     0,     3,
     0,     3,     0,     3,     0,     3,     1,     3,     2,     3,
     0,     6,     3,     4,     1,     2,     0,     5,     0,     6,
     0,     3,     1,     5,     5,     1,     1,     3,     4,     1,
     3,     1,     4,     1,     1,     2,     3,     4,     1,     2,
     2,     1,     1,     1,     2,     3,     1,     2,     3,     3,
     8,     2,     0,     4,     0,     3,     1,     3,     1,     1,
     1,     5,     2,     2,     2,     2,     0
};

static const short yydefact[] = {     1,
   427,     0,     4,    76,    44,     0,     9,     0,    73,   114,
     0,     0,     0,     0,    25,    26,     0,     0,     0,     0,
     0,     0,     0,   245,     0,     0,     0,     0,    97,    96,
     0,     0,     0,    40,    39,     0,    83,    84,     0,     0,
     0,     0,     0,     0,    29,     0,     0,    50,     0,     0,
     0,     0,     0,     0,     0,   351,   352,     3,    41,    43,
    78,    79,     0,    74,   231,   232,    75,    82,   115,    45,
   427,   151,   427,   427,   427,   156,   157,   154,   158,   427,
   155,   427,     0,   332,   333,   427,   331,     0,     0,     0,
   116,   427,    46,     0,    54,   427,   420,   421,     6,     0,
     0,     5,     8,     0,   427,     0,   151,   427,   427,   427,
   156,   157,   154,   158,   427,   155,   427,     0,     0,     0,
     0,     0,     0,    47,     0,     0,    94,    95,    55,     0,
     0,    98,    99,    42,   427,    80,    81,     0,     0,    72,
    70,     7,    48,    49,     0,   427,     0,     0,   427,   427,
   427,   427,   427,     0,     0,   427,     0,   423,   427,   427,
   427,   427,     0,   427,     0,     0,     0,     0,     0,   356,
     0,   162,   357,   187,   427,   427,   190,   160,   186,   191,
   152,   192,   153,   182,   427,   427,   185,   159,   181,   161,
   163,   167,     0,   200,   166,   142,     0,    85,     0,   164,
   238,   427,    51,   198,     0,   195,   196,   112,    53,     0,
     0,    52,     0,    31,     0,   246,     0,   104,   402,     0,
   160,   152,   153,   159,   161,   163,   167,   427,     0,   164,
     0,     0,   164,   101,     0,   102,   427,   403,   404,     0,
     0,    30,   427,   427,     0,   427,   427,   415,   408,   417,
   419,     0,    68,     0,   405,   407,    35,   399,    36,    37,
    38,   424,   425,     0,   138,   139,     0,   427,   122,   141,
     0,   427,   119,   426,     0,     0,   347,   427,     0,     0,
   144,   146,   145,   427,   143,   427,     0,   164,   165,   188,
   189,   183,   184,    58,   427,   427,    89,     0,     0,     0,
   320,     0,     0,     0,   113,   111,   217,   210,   211,   212,
   213,   214,     0,     0,     0,     0,   194,    27,   427,    28,
   427,   401,   400,     0,     0,   427,   149,   427,   150,    15,
   427,   427,     0,   397,     0,     0,     0,   427,   413,   412,
     0,   427,   427,     0,   410,   409,   427,     0,   427,     0,
     0,     0,     0,   118,   121,   123,   140,   427,     0,   427,
   124,   427,   131,   132,     0,   427,     0,   355,   353,    56,
     0,   427,     0,   427,    12,   241,     0,   427,     0,     0,
    86,    93,   427,   427,   323,   329,   328,   330,     0,   319,
   321,     0,   235,   255,   427,     0,   295,   277,   278,     0,
     0,     0,   427,     0,     0,     0,     0,     0,   279,   273,
     0,   294,   282,   281,   427,     0,     0,     0,   283,     0,
   257,    77,   197,     0,   168,   427,   427,   427,   173,   174,
   171,   175,   427,   172,   427,     0,     0,     0,     0,     0,
   228,   229,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   114,     0,    32,   427,   427,     0,     0,     0,
   250,   427,   427,     0,   193,     0,    13,   137,     0,     0,
   100,   403,     0,     0,    71,    69,   427,   418,     0,   147,
   427,   416,    67,   406,   427,   128,   422,   427,   129,   130,
   427,   126,   125,   427,   354,     0,   427,   427,     0,     0,
   244,    10,   233,   203,   427,     0,   202,   207,     0,   427,
    90,     0,   324,     0,     0,   326,     0,     0,     0,   427,
     0,   252,   427,     0,     0,   270,   269,   271,     0,     0,
     0,     0,     0,   299,   427,   298,     0,   300,     0,     0,
   251,   164,   288,   218,   177,   169,   170,   176,   178,   179,
     0,   230,   180,     0,   224,   225,   223,   226,   227,   219,
   220,   221,   222,     0,   116,     0,     0,     0,     0,   390,
     0,   247,   248,   381,     0,     0,   249,     0,   383,   105,
     0,   427,   135,     0,   133,   136,     0,   427,     0,   427,
     0,    64,    62,     0,   414,     0,   148,   398,   120,     0,
   349,   365,     0,   359,   361,   363,     0,     0,     0,   427,
   367,     0,   334,     0,    19,   336,   427,   427,   243,     0,
     0,   204,   206,   427,     0,   427,    91,    92,     0,   325,
   322,   327,     0,   256,   293,     0,     0,     0,   284,     0,
   275,   266,     0,     0,   274,   427,   427,   263,   427,   291,
   280,   292,   239,   290,   286,     0,   216,   215,   427,     0,
   164,     0,   427,   427,     0,     0,   377,   376,   103,    21,
    23,   134,     0,   106,    14,    33,     0,    22,     0,     0,
     0,   427,     0,     0,     0,     0,     0,     0,     0,   369,
     0,   358,     0,     0,   375,     0,   427,     0,    18,    16,
     0,    11,     0,     0,   201,    59,     0,   427,     0,   427,
   272,     0,     0,   427,   427,     0,     0,   427,     0,     0,
     0,   265,   427,   427,     0,   388,     0,     0,   394,     0,
   382,     0,     0,   117,     0,   427,   427,    63,    61,     0,
    66,     0,   427,     0,   366,     0,   360,   362,   364,   370,
     0,   427,   368,    57,     0,    20,     0,     0,   234,   209,
   205,   208,   427,     0,     0,   236,   427,   260,   253,   427,
   313,     0,   427,     0,   312,   318,     0,     0,   301,   267,
   427,     0,   427,   427,   264,   240,   296,     0,   297,   289,
   389,     0,   386,   387,   395,   385,   427,   384,   379,     0,
    24,   427,   427,     0,     0,   411,     0,   350,   371,   348,
   427,   373,   335,    17,   242,    60,    87,     0,   427,     0,
     0,   340,     0,   285,     0,     0,   317,   427,     0,   427,
   427,   262,     0,   427,   261,   258,   287,   396,     0,     0,
   392,     0,   378,     0,   107,     0,    65,   127,     0,   374,
    88,   237,     0,   339,   341,   337,   338,   254,   314,     0,
   311,   276,   268,   302,   427,   306,   307,   259,     0,     0,
   393,   380,   427,     0,    34,   372,     0,     0,     0,     0,
   304,   308,   310,   193,   391,   109,   108,   344,   345,     0,
   342,   315,   303,     0,     0,     0,   343,     0,     0,   305,
   309,   110,   346,   316,     0,     0,     0
};

static const short yydefgoto[] = {   905,
     1,     2,    58,   620,   587,   757,   698,   735,   145,   319,
   497,   378,   763,   412,   413,   296,   383,   381,   414,   132,
    62,   675,   874,   896,   209,    63,   670,   341,   354,   268,
   269,   361,   467,   270,   271,   165,   166,   363,   328,   272,
   416,   188,   178,   181,   210,   211,   203,   193,   506,   507,
   704,   761,   762,    64,    65,   621,   518,   819,   302,   723,
    66,   321,   458,   417,   638,   823,   519,   418,   721,   716,
   532,   714,   419,   724,   656,   788,   537,   780,   880,   894,
   832,   868,   774,   775,   300,   390,   391,   392,   126,   833,
   768,   821,   854,   855,   890,    67,   367,   684,   277,   172,
   608,   687,   688,   689,   685,   609,   610,   849,   694,   577,
   733,   842,   665,   578,   796,   569,   840,   728,   797,   136,
   257,   218,   240,   148,   255,   149,   249,   477,   347,   250,
    99,    68,   158,   273
};

static const short yypact[] = {-32768,
-32768,   916,-32768,-32768,-32768,    52,-32768,  1863,-32768,    64,
   408,    29,   136,   200,-32768,-32768,   115,   201,   200,   200,
   231,   328,  1887,-32768,   218,  1747,   266,   281,-32768,-32768,
   283,   122,   122,-32768,-32768,   307,-32768,-32768,   321,   321,
   313,   586,   200,    39,-32768,  1911,  1911,-32768,   351,   363,
   394,   396,   397,   417,   432,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  1702,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   293,-32768,   555,   431,   431,-32768,-32768,-32768,-32768,   567,
-32768,   364,  1911,-32768,-32768,   489,-32768,   491,    50,   187,
   441,-32768,-32768,   554,-32768,-32768,-32768,-32768,-32768,   251,
   499,-32768,-32768,   506,   501,   547,   528,   555,   431,   431,
   530,   534,   549,   557,   567,   569,   364,  1911,   587,    54,
   576,   603,   626,-32768,    54,   628,-32768,-32768,-32768,   393,
   122,-32768,-32768,-32768,   501,-32768,-32768,   217,   633,-32768,
-32768,-32768,-32768,-32768,  1180,    80,   590,   613,   616,-32768,
-32768,-32768,-32768,   620,   622,  1657,   623,-32768,   130,-32768,
    32,   489,   649,   536,   629,   654,    54,   635,   656,-32768,
   662,-32768,-32768,-32768,   431,   431,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   431,   431,-32768,-32768,-32768,-32768,
-32768,   634,   636,-32768,-32768,-32768,   552,   625,   407,   147,
-32768,-32768,   643,-32768,   615,-32768,-32768,-32768,-32768,   618,
   117,   643,   646,-32768,   650,-32768,   551,-32768,-32768,   541,
   652,   655,   663,   667,   668,   682,-32768,   238,   563,   684,
   685,   695,-32768,-32768,   679,-32768,-32768,   683,-32768,   697,
   699,-32768,   279,  1587,  1849,   237,   237,-32768,-32768,-32768,
-32768,  1911,-32768,  1911,-32768,-32768,   700,-32768,   700,   700,
   700,-32768,-32768,   692,-32768,-32768,   703,   706,-32768,-32768,
  1657,   348,-32768,-32768,   735,   739,-32768,-32768,   550,   713,
-32768,-32768,-32768,  1657,-32768,    58,   630,   457,-32768,-32768,
-32768,-32768,-32768,-32768,   152,   718,-32768,   632,   326,   720,
-32768,  1290,  1070,   747,-32768,-32768,   631,-32768,-32768,-32768,
-32768,-32768,  1999,   730,   117,   117,   939,-32768,   422,-32768,
  1421,-32768,-32768,   754,   756,  1657,-32768,   226,-32768,-32768,
   733,  1657,   737,   700,   232,  1911,   399,  1657,-32768,-32768,
   738,   237,   625,   930,-32768,-32768,   734,   746,   616,   764,
   749,   337,  1657,-32768,-32768,-32768,-32768,   409,   536,   152,
-32768,   152,-32768,-32768,   743,   364,   755,-32768,-32768,-32768,
   759,  1657,    36,   226,-32768,-32768,   757,   474,   761,    54,
-32768,-32768,   718,   152,-32768,-32768,-32768,-32768,    28,   762,
-32768,    45,-32768,-32768,   110,  1911,-32768,-32768,-32768,   765,
   763,   444,   489,   731,   736,   740,  1911,  1771,-32768,-32768,
   790,-32768,-32768,-32768,    99,   772,   767,  1290,-32768,   259,
-32768,-32768,-32768,   798,-32768,   555,   431,   431,-32768,-32768,
-32768,-32768,   567,-32768,   364,  1911,   777,   789,   802,  1911,
-32768,-32768,   117,   117,   117,   117,   117,   117,   117,   117,
   117,   468,   799,  1911,-32768,   783,   783,   778,  1290,   248,
-32768,   501,   501,   786,    23,   806,-32768,-32768,  1935,   788,
-32768,-32768,   792,    22,-32768,-32768,   734,-32768,   793,-32768,
   625,-32768,-32768,-32768,   501,-32768,-32768,   706,-32768,-32768,
   152,-32768,-32768,   364,-32768,  1367,   474,   202,   796,   453,
-32768,-32768,-32768,   785,   474,   338,-32768,-32768,   791,   439,
-32768,   801,-32768,   822,   326,-32768,   823,  1290,  1290,  1632,
   828,-32768,   501,   829,   804,-32768,-32768,-32768,    54,   831,
    54,  1510,   811,   649,   336,   475,   771,-32768,   722,    54,
-32768,   595,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   117,-32768,-32768,   813,   588,   602,   660,   523,   523,   517,
   517,-32768,-32768,   373,-32768,    54,   839,  1911,   840,-32768,
   842,-32768,-32768,-32768,   820,   817,-32768,   240,-32768,-32768,
   824,   202,-32768,   847,-32768,-32768,   564,    -5,    54,   202,
  1911,-32768,-32768,  1911,-32768,   826,-32768,-32768,-32768,   827,
-32768,-32768,   830,-32768,-32768,-32768,    54,   825,  1367,   832,
-32768,   385,-32768,   833,-32768,-32768,   202,  1657,-32768,   564,
  1290,-32768,-32768,   474,   850,  1657,-32768,-32768,   835,-32768,
-32768,-32768,   834,-32768,-32768,   836,   838,  1290,-32768,   841,
-32768,   843,   844,   848,-32768,  1657,  1657,-32768,   226,-32768,
-32768,-32768,-32768,-32768,-32768,    54,-32768,-32768,   810,   753,
    15,    -2,-32768,-32768,   240,   852,-32768,-32768,-32768,-32768,
   845,-32768,    54,-32768,-32768,-32768,   853,-32768,   845,   482,
   849,  1657,   855,   265,  1367,   859,  1367,  1367,  1367,   625,
   265,-32768,    54,   856,-32768,   857,  1657,   564,-32768,   845,
   860,-32768,   851,    56,-32768,-32768,   861,  1657,    54,   202,
-32768,   858,   873,   526,  1657,   585,   862,  1657,   863,   866,
   585,-32768,   718,    54,   865,-32768,   868,   287,-32768,   287,
-32768,   869,   248,-32768,   564,   380,  1657,-32768,-32768,  1911,
-32768,   870,  1657,   779,-32768,   881,-32768,-32768,-32768,-32768,
   808,   470,-32768,-32768,   882,-32768,   564,   883,-32768,-32768,
-32768,   939,   718,   884,   887,-32768,   879,-32768,-32768,   501,
   876,   892,   526,   504,-32768,-32768,   896,    54,-32768,-32768,
   202,   897,   202,   202,-32768,-32768,-32768,   899,-32768,-32768,
-32768,   880,-32768,-32768,-32768,-32768,   900,-32768,-32768,   911,
-32768,  1657,   226,   898,   919,-32768,   902,-32768,-32768,-32768,
   832,-32768,-32768,-32768,-32768,-32768,-32768,   920,   718,   948,
   481,-32768,  1290,-32768,    56,   952,-32768,   526,   927,   202,
   409,-32768,   496,   202,-32768,-32768,-32768,-32768,  1657,   955,
-32768,   240,-32768,   932,-32768,   941,-32768,-32768,  1817,-32768,
-32768,-32768,   935,   943,-32768,-32768,-32768,-32768,-32768,   954,
-32768,-32768,-32768,-32768,   226,-32768,-32768,-32768,   494,   956,
-32768,-32768,   202,   564,-32768,-32768,   728,   948,   976,   585,
-32768,-32768,-32768,   532,-32768,-32768,-32768,-32768,   939,   619,
-32768,   945,-32768,   585,   953,   564,-32768,   117,    56,-32768,
-32768,-32768,   939,-32768,   994,  1015,-32768
};

static const short yypgoto[] = {-32768,
-32768,   814,   874,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    20,    25,-32768,-32768,  -370,    19,    -7,
-32768,  -507,-32768,-32768,-32768,  -269,  -448,  -111,   533,  -351,
   751,  -261,  -346,   760,-32768,    88,   -66,   -60,  -283,   289,
   -14,  -101,   -92,    72,  -460,-32768,   -55,  -147,   529,  -487,
-32768,  -806,    -6,   701,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -407,-32768,-32768,-32768,  -314,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -692,-32768,-32768,
  -524,-32768,-32768,  -710,  -282,-32768,   512,   641,    12,  -365,
-32768,-32768,-32768,   154,-32768,-32768,-32768,-32768,-32768,   -40,
  -413,-32768,-32768,-32768,-32768,-32768,   185,-32768,  -714,  -538,
-32768,-32768,-32768,   714,   306,   580,-32768,   375,-32768,  1018,
    71,  -125,   724,  -244,   711,   -42,  -210,-32768,-32768,  -226,
   365,-32768,   709,    -1
};


#define	YYLAST		2119


static const short yytable[] = {     3,
    69,   488,   374,   147,   585,   377,   459,   348,   121,   237,
   541,   489,   511,   224,   280,   221,   340,   623,   859,    90,
    61,    59,   199,   676,   726,   133,    60,   502,   785,   206,
   513,    94,   207,   379,   122,   345,   346,   812,   500,   668,
   212,   190,   143,   376,   267,   144,  -427,   516,   168,   454,
   592,   573,   196,   593,   583,    70,   196,  -142,   307,   480,
   594,   469,   827,   308,   309,   760,   310,   311,   312,   173,
    91,   179,   182,   182,   169,   197,   225,   299,   189,   247,
   173,   313,   243,   372,   194,   248,    95,   386,   387,   388,
   204,   474,   904,   170,   204,   314,   850,   282,   492,   245,
   493,   196,   501,   219,   279,   244,   179,   182,   182,   517,
   633,   634,   702,   189,   164,   173,   171,   861,   278,   307,
   482,   245,   512,   236,   308,   309,   731,   310,   311,   312,
   170,   479,   615,   219,   584,   520,   705,   297,    96,   627,
   100,   678,   313,    69,   251,   744,   183,   256,   258,   258,
   258,   258,   751,   171,   358,   275,   314,   173,   204,   173,
   194,   163,   283,    61,    59,   163,   164,   327,   699,    60,
   164,   315,   371,   182,   182,   316,   198,   342,  -427,   373,
   222,   223,   339,   182,   182,   248,   248,   893,   648,   200,
   756,   246,   276,   245,   800,   692,   164,   597,   301,   130,
     3,   900,    97,   101,   317,   362,    98,   228,   170,   299,
   534,   349,   232,   703,   464,   164,   671,   645,   201,   238,
   470,   259,   260,   261,   679,   327,   329,   801,   170,   600,
   712,   171,   315,   104,   472,   258,   316,   613,   344,   243,
   574,   251,   131,   123,   251,   251,   290,   291,   574,   814,
   595,   700,   239,   213,   286,   525,   292,   293,   835,   836,
   499,   542,   338,   326,   465,   574,   355,   239,   127,   614,
   364,   745,  -427,   747,   748,   749,   368,   214,   245,   245,
  -427,   248,   327,   128,   329,   129,   301,   575,   466,   792,
   543,   576,   491,   364,   382,   575,    89,   490,   437,   576,
   421,    69,   722,   872,   338,   863,   438,   334,   441,   442,
   134,   120,   575,   420,   125,   793,   576,    69,   794,   461,
   245,    61,    59,   135,   439,   495,   468,    60,   385,    69,
   105,   548,   420,   545,   146,   146,   580,   581,   138,   154,
   251,   329,  -427,   155,   767,   251,   680,   256,   536,   795,
   358,   167,   786,   106,   456,   457,   468,   150,   364,   598,
   364,   647,   456,   457,   173,   624,   887,  -427,   157,   151,
   625,   191,   468,   359,   465,   659,   508,   245,   377,   456,
   457,   382,   364,   102,   103,   386,   387,   388,   902,   245,
   389,   170,   816,   173,   549,   234,   505,   639,   466,   154,
   152,   194,   153,   155,   201,   802,   226,   142,   636,   285,
    92,    93,   624,   538,   171,   858,   421,   696,   235,   159,
   327,   245,   156,   895,   179,   182,   182,   475,   157,   420,
   476,   189,   298,   173,   160,   452,   555,   556,   557,   558,
   559,   560,   561,   562,   563,   180,   523,   465,   852,   628,
   245,   453,   803,   601,   570,   570,   845,   421,   579,   360,
   219,   219,   170,   564,   164,   567,   380,   510,   869,   524,
   420,   466,   202,    21,   649,   251,   504,   285,   618,   329,
   245,   619,  -180,   219,   864,   376,   355,   870,  -427,   364,
    24,   192,   173,   195,   611,   508,   616,   693,   546,   547,
   650,   215,   535,   508,    84,    85,   701,   886,   382,   856,
   738,   245,   857,   739,   707,   505,   421,   421,   881,   299,
   740,   219,   882,   505,   866,   883,    87,   867,   771,   420,
   420,   828,   884,   468,   719,   720,   829,   216,   206,   217,
   146,   207,   146,   420,   657,    27,    28,    29,    30,   220,
    32,   681,    33,  -168,   205,  -173,  -193,   322,   323,  -174,
   206,  -193,  -193,   207,  -193,  -193,  -193,   324,   325,   174,
   742,   175,   176,   772,  -171,   177,   579,   304,   369,  -193,
   616,   184,  -175,   185,   186,   755,   173,   187,   616,   227,
   415,   673,   674,  -193,  -172,   727,   765,    27,    28,    29,
    30,   229,    32,   777,    33,   230,   782,   611,   695,   415,
   208,   139,   778,   779,   140,   616,   642,   141,   644,   421,
  -180,   252,   508,   654,   146,   804,   655,   653,   231,   750,
   233,   807,   420,   450,   451,   241,   421,   448,   449,   450,
   451,   253,   505,   254,   824,   897,   898,   468,   262,   420,
   263,   281,   164,   660,   284,   274,   285,   301,   288,   301,
   287,   729,   729,   579,   289,  -199,   245,   294,   295,  -193,
   304,   305,   318,  -193,   306,   327,   677,  -177,   320,   330,
  -169,   333,   579,   611,    89,   611,   611,   611,  -170,   579,
   844,   811,  -176,  -178,   690,   529,   531,   805,   444,   445,
   446,   447,   448,   449,   450,   451,   415,  -179,   616,  -180,
   335,   331,   776,   445,   446,   447,   448,   449,   450,   451,
   332,   382,   789,   336,   550,   337,   351,   350,   554,   352,
   307,   579,   773,   353,   329,   308,   309,   365,   310,   311,
   312,   366,   566,   725,   370,   380,   375,   415,   384,   423,
   695,   393,   424,   313,   888,   440,   462,   589,   463,   338,
   736,   382,   453,   471,   478,   822,   485,   314,   219,   494,
   865,   776,   446,   447,   448,   449,   450,   451,   483,   616,
   752,   616,   616,   486,   607,   498,   496,   509,   503,   515,
   521,   773,   533,   526,   522,   841,   766,   539,   527,   540,
   544,   468,   528,   551,   553,   565,   415,   415,   568,   695,
   572,   787,   582,   586,   590,   552,   626,   382,   591,   596,
   415,   421,   617,   622,   630,   632,   776,   629,   616,   468,
   637,   640,   616,   643,   420,   641,   646,   651,   652,   658,
   579,   661,   663,   315,   664,   666,   773,   316,   667,   672,
   669,   682,   706,   683,   732,   686,   662,   691,   697,   693,
   708,   746,   710,   468,   711,   831,   709,   713,   715,   717,
   889,   616,   299,   718,   373,   770,   734,   741,   737,   146,
   743,   808,   146,   759,   753,   754,   758,   764,   781,   783,
   769,   903,   784,   790,   791,   799,   806,   607,   443,   444,
   445,   446,   447,   448,   449,   450,   451,   809,   813,   415,
   810,   815,   817,   818,   825,    -2,     4,   826,  -427,     5,
     6,     7,   830,   834,   846,   839,   415,   837,   848,     8,
  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
  -427,   820,   838,   843,     9,    10,    11,   847,   851,    12,
   853,  -427,  -427,  -427,   860,   862,    13,   871,   873,    14,
   877,    15,    16,    17,    18,    19,    20,    21,    22,   875,
   878,  -427,   481,   607,  -427,   607,   607,   607,   892,    23,
   879,   901,   885,   899,    24,    25,    26,    27,    28,    29,
    30,    31,    32,   906,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    52,   907,   303,    53,    54,   242,   455,
   599,   356,    55,    56,    57,   612,   631,  -427,   146,   514,
   357,   891,  -427,   876,   460,   798,   571,  -427,   730,   443,
   444,   445,   446,   447,   448,   449,   450,   451,   443,   444,
   445,   446,   447,   448,   449,   450,   451,   137,   473,   484,
   487,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     4,     0,  -427,     5,     6,     7,     0,     0,     0,     0,
     0,     0,     0,     8,  -427,  -427,  -427,  -427,  -427,  -427,
  -427,  -427,  -427,  -427,  -427,     0,     0,     0,     9,    10,
    11,     0,   422,    12,     0,  -427,  -427,  -427,     0,     0,
    13,   415,     0,    14,     0,    15,    16,    17,    18,    19,
    20,    21,    22,     0,     0,  -427,     0,     0,  -427,     0,
     0,     0,     0,    23,     0,     0,     0,   607,    24,    25,
    26,    27,    28,    29,    30,    31,    32,     0,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,     0,     0,
    53,    54,     0,     0,     0,     0,    55,    56,    57,     0,
     4,  -427,  -427,     5,     6,     7,  -427,     0,     0,     0,
     0,  -427,     0,     8,  -427,  -427,  -427,  -427,  -427,  -427,
  -427,  -427,  -427,  -427,  -427,     0,     0,     0,     9,    10,
    11,     0,     0,    12,     0,  -427,  -427,  -427,     0,     0,
    13,     0,     0,    14,     0,    15,    16,    17,    18,    19,
    20,    21,    22,     0,     0,  -427,     0,     0,  -427,     0,
     0,     0,     0,    23,     0,     0,     0,     0,    24,    25,
    26,    27,    28,    29,    30,    31,    32,     0,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,     0,     0,
    53,    54,     0,     0,     0,     0,    55,    56,    57,     0,
   394,  -427,   395,     0,     0,     0,  -427,     0,     0,     0,
     0,  -427,     0,   396,   107,   108,   109,   110,   111,   112,
   113,   114,   115,   116,   117,     0,     0,     0,   397,     0,
     0,     0,  -427,     0,     0,   118,    84,    85,     0,     0,
     0,     0,     0,     0,     0,   398,   399,   400,     0,     0,
     0,   401,   402,     0,     0,   403,     0,     0,    87,   404,
   405,   406,     0,   407,   408,   409,     0,     0,     0,     0,
     0,    27,    28,    29,    30,     0,    32,   602,    33,    71,
     0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
   410,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,     0,     0,     0,     0,     0,     0,     0,  -427,
     0,     0,    83,    84,    85,     0,     0,     0,     0,   411,
     0,    88,     0,     0,   603,     0,     0,     0,     0,     0,
     0,     0,   119,   395,     0,    87,     0,     0,     0,     0,
     0,     0,     0,     0,   396,   107,   108,   109,   110,   111,
   112,   113,   114,   115,   116,   117,     0,     0,     0,   397,
     0,     0,     0,     0,     0,     0,   118,    84,    85,     0,
     0,     0,     0,     0,     0,     0,   398,   399,   400,     0,
   604,   605,   606,   402,     0,     0,   403,     0,     0,    87,
   404,   405,   406,     0,   407,   408,   409,     0,    88,     0,
     0,     0,    27,    28,    29,    30,     0,    32,     0,    33,
     0,     0,     0,    37,    38,     0,     0,     0,     0,     0,
     0,   410,   395,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   396,   107,   108,   109,   110,   111,   112,
   113,   114,   115,   116,   117,   456,   457,     0,   397,     0,
   411,     0,    88,     0,     0,   118,    84,    85,     0,     0,
     0,     0,     0,     0,     0,   398,   399,   400,     0,     0,
     0,     0,   402,     0,     0,   403,     0,     0,    87,   404,
   405,   406,     0,   407,   408,   409,     0,     0,     0,     0,
     0,    27,    28,    29,    30,     0,    32,     0,    33,    71,
     0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
   410,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   264,    83,    84,    85,     0,     0,     0,     0,   411,
     0,    88,     0,     0,    71,     0,     0,     0,     0,     0,
   265,   266,   119,     0,     0,    87,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,     0,     0,    71,
     0,     0,     0,     0,     0,     0,   264,    83,    84,    85,
     0,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,     0,     0,     0,   265,   266,   119,     0,     0,
    87,   264,    83,    84,    85,     0,     0,     0,     0,     0,
     0,     0,     0,   164,   161,     0,     0,     0,    88,     0,
   265,   266,   119,     0,     0,    87,   107,   108,   109,   110,
   111,   112,   113,   114,   115,   116,   117,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   118,    84,    85,
     0,     0,     0,     0,     0,     0,     0,     0,   635,    71,
   124,     0,     0,    88,     0,     0,     0,   162,     0,     0,
    87,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,     0,    71,     0,     0,     0,     0,    88,     0,
     0,     0,    83,    84,    85,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,     0,     0,     0,     0,
     0,     0,   119,     0,     0,    87,    83,    84,    85,     0,
     0,     0,     0,   163,     0,     0,     0,     0,   164,    71,
     0,     0,     0,    88,     0,     0,   119,     0,     0,    87,
     0,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   307,    83,    84,    85,     0,   308,   309,     0,   310,
   311,   312,     0,     0,   603,    71,     0,     0,    88,     0,
     0,     0,   119,     0,   313,    87,     0,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,   314,    71,
   530,   343,    88,     0,     0,     0,     0,     0,    83,    84,
    85,   107,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,     0,    71,     0,     0,     0,     0,    86,     0,
     0,    87,   118,    84,    85,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,     0,   588,    88,     0,
     0,     0,   119,     0,     0,    87,    83,    84,    85,    72,
    73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
     0,     0,     0,     0,   315,     0,   119,     0,   316,    87,
    83,    84,    85,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    88,     0,     0,     0,     0,     0,
   119,     0,     0,    87,     0,     0,     0,     0,     0,     0,
     0,   307,     0,     0,     0,     0,   308,   309,    88,   310,
   311,   312,     0,   425,   426,   427,   428,   429,   430,   431,
   432,   433,   434,   435,   313,     0,     0,     0,     0,     0,
     0,     0,    88,     0,   436,    84,    85,     0,   314,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    88,    87,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   315,     0,     0,     0,   316
};

static const short yycheck[] = {     1,
     2,   353,   286,    46,   465,   288,   321,   252,    23,   135,
   418,   358,   383,   115,   162,   108,   243,   505,   825,     8,
     2,     2,    89,    29,    27,    33,     2,   374,   721,     7,
     3,     3,    10,   295,    23,   246,   247,   752,     3,   578,
    96,    82,     4,    29,   156,     7,    32,     3,    63,   319,
    29,   459,     3,    32,    32,     4,     3,    26,     3,   343,
    39,   331,   773,     8,     9,    10,    11,    12,    13,    71,
     7,    73,    74,    75,    63,    26,   117,    63,    80,   146,
    82,    26,     3,    26,    86,   146,    58,    60,    61,    62,
    92,   336,   899,    99,    96,    40,   811,   164,   360,    42,
   362,     3,    67,   105,   160,    26,   108,   109,   110,    65,
   518,   519,   620,   115,   117,   117,   122,   828,   159,     3,
   347,    42,   384,   131,     8,     9,   665,    11,    12,    13,
    99,   342,   498,   135,   112,    26,   624,   198,     3,   510,
    26,   590,    26,   145,   146,   684,    75,   149,   150,   151,
   152,   153,   691,   122,     3,    26,    40,   159,   160,   161,
   162,   112,   164,   145,   145,   112,   117,   228,   617,   145,
   117,   116,   284,   175,   176,   120,    89,   244,    32,   122,
   109,   110,   243,   185,   186,   246,   247,   880,   535,     3,
   698,   112,    63,    42,   733,   609,   117,   481,   200,    78,
   202,   894,     3,     3,   211,   272,     7,   120,    99,    63,
   112,   254,   125,   621,   326,   117,   582,   532,    32,     3,
   332,   151,   152,   153,   590,   286,   228,   735,    99,   491,
   638,   122,   116,     3,     3,   237,   120,    36,   245,     3,
     1,   243,   121,    26,   246,   247,   175,   176,     1,   757,
   477,   617,    36,     3,   167,   403,   185,   186,   783,   784,
   372,     3,    26,    26,    39,     1,   268,    36,     3,    68,
   272,   685,    33,   687,   688,   689,   278,    27,    42,    42,
    33,   342,   343,     3,   286,     3,   288,    48,    63,     3,
    32,    52,   359,   295,   296,    48,     8,   358,   313,    52,
   302,   303,   649,   842,    26,   830,   313,   237,   315,   316,
     4,    23,    48,   302,    26,    29,    52,   319,    32,   321,
    42,   303,   303,     3,   313,   366,   328,   303,     3,   331,
     3,   433,   321,   426,    46,    47,   462,   463,    26,     3,
   342,   343,   103,     7,   710,   347,   591,   349,   415,    63,
     3,    63,   723,    26,   115,   116,   358,     7,   360,   485,
   362,    26,   115,   116,   366,    28,   874,   103,    32,     7,
    33,    83,   374,    26,    39,     3,   378,    42,   661,   115,
   116,   383,   384,    19,    20,    60,    61,    62,   896,    42,
    65,    99,   763,   395,   435,     3,   378,   523,    63,     3,
     7,   403,     7,     7,    32,    26,   118,    43,   520,     3,
     3,     4,    28,   415,   122,   823,   418,    33,    26,     3,
   481,    42,    26,   884,   426,   427,   428,    29,    32,   418,
    32,   433,    26,   435,     3,    14,   443,   444,   445,   446,
   447,   448,   449,   450,   451,    15,     3,    39,   819,   510,
    42,    30,   736,   494,   456,   457,   803,   459,   460,   112,
   462,   463,    99,   452,   117,   454,    28,   380,   834,    26,
   459,    63,    32,    52,   535,   477,     3,     3,    26,   481,
    42,    29,    26,   485,   831,    29,   488,   839,    32,   491,
    69,     3,   494,     3,   496,   497,   498,    28,   427,   428,
    26,     3,   415,   505,    37,    38,   618,   873,   510,    29,
    29,    42,    32,    32,   626,   497,   518,   519,   865,    63,
    39,   523,    29,   505,    29,    32,    59,    32,     3,   518,
   519,    28,    39,   535,   646,   647,    33,    32,     7,    39,
   252,    10,   254,   532,   551,    72,    73,    74,    75,     3,
    77,   594,    79,    26,     1,    26,     3,     7,     8,    26,
     7,     8,     9,    10,    11,    12,    13,    27,    28,    15,
   682,    17,    18,    48,    26,    21,   578,    28,    29,    26,
   582,    15,    26,    17,    18,   697,   588,    21,   590,     3,
   302,    28,    29,    40,    26,   662,   708,    72,    73,    74,
    75,    26,    77,   715,    79,     3,   718,   609,   610,   321,
    57,    26,    28,    29,    29,   617,   529,    32,   531,   621,
    26,    32,   624,    29,   336,   737,    32,   540,     3,   690,
     3,   743,   621,   117,   118,     3,   638,   115,   116,   117,
   118,    29,   624,    28,   770,    27,    28,   649,    29,   638,
    29,     3,   117,   566,    26,    33,     3,   659,     3,   661,
    26,   663,   664,   665,     3,    32,    42,    32,   117,   116,
    28,    57,    27,   120,    57,   736,   589,    26,    29,   117,
    26,     3,   684,   685,   396,   687,   688,   689,    26,   691,
   802,   752,    26,    26,   607,   407,   408,   740,   111,   112,
   113,   114,   115,   116,   117,   118,   418,    26,   710,    26,
    28,    27,   714,   112,   113,   114,   115,   116,   117,   118,
    26,   723,   724,    27,   436,    27,    35,    28,   440,    27,
     3,   733,   714,    28,   736,     8,     9,     3,    11,    12,
    13,     3,   454,   656,    32,    28,   117,   459,   117,     3,
   752,    32,   122,    26,    27,    26,     3,   469,     3,    26,
   673,   763,    30,    27,    27,   767,     3,    40,   770,    27,
   831,   773,   113,   114,   115,   116,   117,   118,    33,   781,
   693,   783,   784,    35,   496,    27,    32,    27,    32,    28,
    26,   773,     3,    63,    32,   797,   709,    26,    63,    33,
     3,   803,    63,    27,     3,     7,   518,   519,    26,   811,
    33,   724,    27,     8,    27,    27,    26,   819,    27,    27,
   532,   823,    27,    39,     3,     3,   828,    27,   830,   831,
     3,     3,   834,     3,   823,    32,    26,    67,   117,    27,
   842,     3,     3,   116,     3,    26,   828,   120,    32,     3,
    27,    26,     3,    27,     3,    26,   568,    33,    26,    28,
    26,     3,    27,   865,    27,   778,    33,    27,    26,    26,
   877,   873,    63,    26,   122,     3,    32,    29,    26,   591,
    26,   103,   594,    33,    29,    29,    27,    27,    27,    27,
    33,   898,    27,    29,    27,    27,    27,   609,   110,   111,
   112,   113,   114,   115,   116,   117,   118,    27,    27,   621,
   103,    29,    29,    27,    39,     0,     1,    26,     3,     4,
     5,     6,    27,    27,    27,    26,   638,    29,    27,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    63,    63,    33,    29,    30,    31,    29,    29,    34,
     3,    36,    37,    38,     3,    29,    41,     3,    27,    44,
    26,    46,    47,    48,    49,    50,    51,    52,    53,    29,
    28,    56,    43,   685,    59,   687,   688,   689,     3,    64,
    27,    29,    27,    39,    69,    70,    71,    72,    73,    74,
    75,    76,    77,     0,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,    97,    98,     0,   202,   101,   102,   145,   319,
   488,   271,   107,   108,   109,   497,   515,   112,   740,   389,
   271,   878,   117,   849,   321,   730,   457,   122,   664,   110,
   111,   112,   113,   114,   115,   116,   117,   118,   110,   111,
   112,   113,   114,   115,   116,   117,   118,    40,   335,   349,
   352,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    -1,    -1,    -1,    29,    30,
    31,    -1,    33,    34,    -1,    36,    37,    38,    -1,    -1,
    41,   823,    -1,    44,    -1,    46,    47,    48,    49,    50,
    51,    52,    53,    -1,    -1,    56,    -1,    -1,    59,    -1,
    -1,    -1,    -1,    64,    -1,    -1,    -1,   849,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    -1,    79,    80,
    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    -1,    -1,
   101,   102,    -1,    -1,    -1,    -1,   107,   108,   109,    -1,
     1,   112,     3,     4,     5,     6,   117,    -1,    -1,    -1,
    -1,   122,    -1,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    -1,    -1,    -1,    29,    30,
    31,    -1,    -1,    34,    -1,    36,    37,    38,    -1,    -1,
    41,    -1,    -1,    44,    -1,    46,    47,    48,    49,    50,
    51,    52,    53,    -1,    -1,    56,    -1,    -1,    59,    -1,
    -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    -1,    79,    80,
    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,    97,    98,    -1,    -1,
   101,   102,    -1,    -1,    -1,    -1,   107,   108,   109,    -1,
     1,   112,     3,    -1,    -1,    -1,   117,    -1,    -1,    -1,
    -1,   122,    -1,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    -1,    -1,    -1,    29,    -1,
    -1,    -1,    33,    -1,    -1,    36,    37,    38,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,    -1,
    -1,    52,    53,    -1,    -1,    56,    -1,    -1,    59,    60,
    61,    62,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
    -1,    72,    73,    74,    75,    -1,    77,     1,    79,     3,
    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
    91,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
    -1,    -1,    36,    37,    38,    -1,    -1,    -1,    -1,   120,
    -1,   122,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    56,     3,    -1,    59,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,    29,
    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,
   104,   105,   106,    53,    -1,    -1,    56,    -1,    -1,    59,
    60,    61,    62,    -1,    64,    65,    66,    -1,   122,    -1,
    -1,    -1,    72,    73,    74,    75,    -1,    77,    -1,    79,
    -1,    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,
    -1,    91,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,   115,   116,    -1,    29,    -1,
   120,    -1,   122,    -1,    -1,    36,    37,    38,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,    -1,
    -1,    -1,    53,    -1,    -1,    56,    -1,    -1,    59,    60,
    61,    62,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
    -1,    72,    73,    74,    75,    -1,    77,    -1,    79,     3,
    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
    91,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    36,    37,    38,    -1,    -1,    -1,    -1,   120,
    -1,   122,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
    54,    55,    56,    -1,    -1,    59,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    -1,    -1,     3,
    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,
    -1,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    -1,    -1,    -1,    54,    55,    56,    -1,    -1,
    59,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   117,     3,    -1,    -1,    -1,   122,    -1,
    54,    55,    56,    -1,    -1,    59,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,     3,
     4,    -1,    -1,   122,    -1,    -1,    -1,    56,    -1,    -1,
    59,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    -1,     3,    -1,    -1,    -1,    -1,   122,    -1,
    -1,    -1,    36,    37,    38,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,    -1,
    -1,    -1,    56,    -1,    -1,    59,    36,    37,    38,    -1,
    -1,    -1,    -1,   112,    -1,    -1,    -1,    -1,   117,     3,
    -1,    -1,    -1,   122,    -1,    -1,    56,    -1,    -1,    59,
    -1,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,    36,    37,    38,    -1,     8,     9,    -1,    11,
    12,    13,    -1,    -1,    48,     3,    -1,    -1,   122,    -1,
    -1,    -1,    56,    -1,    26,    59,    -1,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    40,     3,
   120,    43,   122,    -1,    -1,    -1,    -1,    -1,    36,    37,
    38,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    -1,     3,    -1,    -1,    -1,    -1,    56,    -1,
    -1,    59,    36,    37,    38,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    -1,     3,   122,    -1,
    -1,    -1,    56,    -1,    -1,    59,    36,    37,    38,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    -1,    -1,    -1,    -1,   116,    -1,    56,    -1,   120,    59,
    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   122,    -1,    -1,    -1,    -1,    -1,
    56,    -1,    -1,    59,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     3,    -1,    -1,    -1,    -1,     8,     9,   122,    11,
    12,    13,    -1,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   122,    -1,    36,    37,    38,    -1,    40,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   122,    59,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   116,    -1,    -1,    -1,   120
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 559 "parser.y"
{ 
                    {
		      int ii;
		      for (ii = 0; ii < 256; ii++) {
			handler_stack[ii] = 0;
		      }
		      handler_stack[0] = comment_handler;
		    }
                    doc_stack[0] = doctitle;
                 ;
    break;}
case 2:
#line 568 "parser.y"
{
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
               ;
    break;}
case 3:
#line 582 "parser.y"
{ 
		     scanner_clear_start();
                     Error = 0;
                ;
    break;}
case 4:
#line 586 "parser.y"
{
	       ;
    break;}
case 5:
#line 590 "parser.y"
{
                  if (allow) {
//		    init_language();
		    doc_entry = 0;
		    // comment_handler->clear();
		    include_file(yyvsp[0].id);
		  }
                ;
    break;}
case 6:
#line 601 "parser.y"
{
		 if (allow) {
		   int oldextern = WrapExtern;
//		   init_language();
		   doc_entry = 0;
		   // comment_handler->clear();
		   WrapExtern = 1;
		   if (include_file(yyvsp[0].id) >= 0) {
		     add_symbol("SWIGEXTERN",0,0);
		   } else {
		     WrapExtern = oldextern;
		   }
		 }
	       ;
    break;}
case 7:
#line 618 "parser.y"
{
		  if (allow) {
		    int oldextern = WrapExtern;
		    init_language();
		    doc_entry = 0;
		    WrapExtern = 1;
		    if (include_file(yyvsp[0].id) >= 0) {
		      add_symbol("SWIGEXTERN",0,0);
		      lang->import(yyvsp[0].id);
		    } else {
		      WrapExtern = oldextern;
		    }
		  }
                ;
    break;}
case 8:
#line 636 "parser.y"
{
                  if (allow) {
                     if ((checkout_file(yyvsp[0].id,yyvsp[0].id)) == 0) {
                       fprintf(stderr,"%s checked out from the SWIG library.\n",yyvsp[0].id);
                      }
                  }
                ;
    break;}
case 9:
#line 646 "parser.y"
{
		 if (allow) {
                  doc_entry = 0;
		  if (Verbose) {
		    fprintf(stderr,"%s : Line %d.  CPP %s ignored.\n", input_file, line_number,yyvsp[0].id);
		  }
		 }
		;
    break;}
case 10:
#line 657 "parser.y"
{
		  if (allow) {
		    init_language();
		    if (Active_type) delete Active_type;
		    Active_type = new DataType(yyvsp[-3].type);
		    Active_extern = yyvsp[-4].ivalue;
		    yyvsp[-3].type->is_pointer += yyvsp[-2].decl.is_pointer;
		    if (yyvsp[-1].ivalue > 0) {
		      yyvsp[-3].type->is_pointer++;
		      yyvsp[-3].type->status = STAT_READONLY;
                      yyvsp[-3].type->arraystr = copy_string(ArrayString);
		    }
		    if (yyvsp[-2].decl.is_reference) {
		      fprintf(stderr,"%s : Line %d. Error. Linkage to C++ reference not allowed.\n", input_file, line_number);
		      FatalError();
		    } else {
		      if (yyvsp[-3].type->qualifier) {
			if ((strcmp(yyvsp[-3].type->qualifier,"const") == 0)) {
			  if (yyvsp[0].dtype.type != T_ERROR)
			    create_constant(yyvsp[-2].decl.id, yyvsp[-3].type, yyvsp[0].dtype.id);
			} else 
			  create_variable(yyvsp[-4].ivalue,yyvsp[-2].decl.id,yyvsp[-3].type);
		      } else
			create_variable(yyvsp[-4].ivalue,yyvsp[-2].decl.id,yyvsp[-3].type);
		    }
		  }
		  delete yyvsp[-3].type;
                ;
    break;}
case 11:
#line 684 "parser.y"
{ ;
    break;}
case 12:
#line 688 "parser.y"
{ 
                   skip_decl();
		   fprintf(stderr,"%s : Line %d. Function pointers not currently supported.\n",
			   input_file, line_number);
		;
    break;}
case 13:
#line 696 "parser.y"
{
		  if (Verbose) {
		    fprintf(stderr,"static variable %s ignored.\n",yyvsp[-2].decl.id);
		  }
		  Active_static = 1;
		  delete yyvsp[-3].type;
		;
    break;}
case 14:
#line 702 "parser.y"
{
		  Active_static = 0;
		;
    break;}
case 15:
#line 708 "parser.y"
{ 
                   skip_decl();
		   fprintf(stderr,"%s : Line %d. Function pointers not currently supported.\n",
			   input_file, line_number);
		;
    break;}
case 16:
#line 717 "parser.y"
{
		  if (allow) {
		    init_language();
		    if (Active_type) delete Active_type;
		    Active_type = new DataType(yyvsp[-5].type);
		    Active_extern = yyvsp[-6].ivalue;
		    yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		    yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		    create_function(yyvsp[-6].ivalue, yyvsp[-4].decl.id, yyvsp[-5].type, yyvsp[-2].pl);
		  }
		  delete yyvsp[-5].type;
		  delete yyvsp[-2].pl;
		;
    break;}
case 17:
#line 729 "parser.y"
{ ;
    break;}
case 18:
#line 733 "parser.y"
{
		  if (allow) {
		    init_language();
		    yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		    yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		    create_function(yyvsp[-6].ivalue, yyvsp[-4].decl.id, yyvsp[-5].type, yyvsp[-2].pl);
		  }
		  delete yyvsp[-5].type;
		  delete yyvsp[-2].pl;
		;
    break;}
case 19:
#line 746 "parser.y"
{ 
		  if (allow) {
                    init_language();
		    DataType *t = new DataType(T_INT);
                    t->is_pointer += yyvsp[-4].decl.is_pointer;
		    t->is_reference = yyvsp[-4].decl.is_reference;
		    create_function(yyvsp[-5].ivalue,yyvsp[-4].decl.id,t,yyvsp[-2].pl);
		    delete t;
		  }
                ;
    break;}
case 20:
#line 755 "parser.y"
{ ;
    break;}
case 21:
#line 759 "parser.y"
{
		  if ((allow) && (Inline)) {
		    if (strlen(CCode.get())) {
		      init_language();
		      yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		      yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		      create_function(0, yyvsp[-4].decl.id, yyvsp[-5].type, yyvsp[-2].pl);
		    }
		  }
		  delete yyvsp[-5].type;
		  delete yyvsp[-2].pl;
		;
    break;}
case 22:
#line 774 "parser.y"
{
		  if (allow) {
		    init_language();
		    yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		    yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		    if (Inline) {
		      fprintf(stderr,"%s : Line %d. Repeated %%inline directive.\n",input_file,line_number);
		      FatalError();
		    } else {
		      if (strlen(CCode.get())) {
			fprintf(f_header,"static ");
			emit_extern_func(yyvsp[-4].decl.id,yyvsp[-5].type,yyvsp[-2].pl,3,f_header);
			fprintf(f_header,"%s\n",CCode.get());
		      }
		      create_function(0, yyvsp[-4].decl.id, yyvsp[-5].type, yyvsp[-2].pl);
		    }
		  }
		  delete yyvsp[-5].type;
		  delete yyvsp[-2].pl;
		;
    break;}
case 23:
#line 797 "parser.y"
{
		  if (allow) {
		    if (Verbose) {
		      fprintf(stderr,"static function %s ignored.\n", yyvsp[-4].decl.id);
		    }
		  }
		  Active_static = 1;
		  delete yyvsp[-5].type;
		  delete yyvsp[-2].pl;
		;
    break;}
case 24:
#line 806 "parser.y"
{
		  Active_static = 0;
		 ;
    break;}
case 25:
#line 812 "parser.y"
{
		  if (allow)
		    Status = Status | STAT_READONLY;
	       ;
    break;}
case 26:
#line 819 "parser.y"
{
		 if (allow)
		   Status = Status & ~STAT_READONLY;
	       ;
    break;}
case 27:
#line 825 "parser.y"
{
		 if (allow) {
                     strcpy(yy_rename,yyvsp[-1].id);
                     Rename_true = 1;
		 }
               ;
    break;}
case 28:
#line 833 "parser.y"
{ 
		 if (name_hash.lookup(yyvsp[-2].id)) {
		   name_hash.remove(yyvsp[-2].id);
		 }
		 name_hash.add(yyvsp[-2].id,copy_string(yyvsp[-1].id));
	       ;
    break;}
case 29:
#line 842 "parser.y"
{
                     NewObject = 1;
                ;
    break;}
case 30:
#line 844 "parser.y"
{
                     NewObject = 0;
               ;
    break;}
case 31:
#line 850 "parser.y"
{
		 if (allow) {
		   fprintf(stderr,"%s : Lind %d. Empty %%name() is no longer supported.\n",
			   input_file, line_number);
		   FatalError();
		 }
	       ;
    break;}
case 32:
#line 856 "parser.y"
{
		 Rename_true = 0;
	       ;
    break;}
case 33:
#line 862 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   init_language();
		   if (add_symbol(yyvsp[-4].id,(DataType *) 0, (char *) 0)) {
		     fprintf(stderr,"%s : Line %d. Name of native function %s conflicts with previous declaration (ignored)\n",
			     input_file, line_number, yyvsp[-4].id);
		   } else {
		     doc_entry = new DocDecl(yyvsp[-4].id,doc_stack[doc_stack_top]);
		     lang->add_native(yyvsp[-4].id,yyvsp[-1].id);
		   }
		 }
	       ;
    break;}
case 34:
#line 874 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   init_language();
		   yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		   if (add_symbol(yyvsp[-8].id,(DataType *) 0, (char *) 0)) {
		     fprintf(stderr,"%s : Line %d. Name of native function %s conflicts with previous declaration (ignored)\n",
			     input_file, line_number, yyvsp[-8].id);
		   } else {
		     if (yyvsp[-6].ivalue) {
		       emit_extern_func(yyvsp[-4].decl.id, yyvsp[-5].type, yyvsp[-2].pl, yyvsp[-6].ivalue, f_header);
		     }
		     doc_entry = new DocDecl(yyvsp[-8].id,doc_stack[doc_stack_top]);
		     lang->add_native(yyvsp[-8].id,yyvsp[-4].decl.id);
		   }
		 }
		 delete yyvsp[-5].type;
		 delete yyvsp[-2].pl;
	       ;
    break;}
case 35:
#line 895 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   if (!title_init) {
		     title_init = 1;
		     doc_init = 1;
		     if (!comment_handler) {
		       comment_handler = new CommentHandler();
		     }
		     { 
		       int ii;
		       for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
			 comment_handler->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		       }
		     }
		     // Create a new title for documentation 
		     {
		       int temp = line_number;
		       line_number = yyvsp[-2].ivalue;
		       if (!doctitle)
			 doctitle = new DocTitle(yyvsp[-1].id,0);
		       else {
			 doctitle->name = copy_string(title);
			 doctitle->line_number = yyvsp[-2].ivalue;
			 doctitle->end_line = yyvsp[-2].ivalue;
		       }
		       line_number = temp;
		     }
		     doctitle->usage = yyvsp[-1].id;
		     doc_entry = doctitle;
		     doc_stack[0] = doc_entry;
		     doc_stack_top = 0;
		     handler_stack[0] = comment_handler;
		     { 
		       int ii;
		       for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
			 doc_stack[doc_stack_top]->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		       }
		     }

		   } else {
		     // Ignore it
		   }
		 }
	       ;
    break;}
case 36:
#line 943 "parser.y"
{
		 if (allow && (!WrapExtern) && (!IgnoreDoc)) {
		   // Copy old comment handler
		   // if (handler_stack[1]) delete handler_stack[1];
		   handler_stack[1] = new CommentHandler(handler_stack[0]);  
		   comment_handler = handler_stack[1];
		   { 
		     int ii;
		     for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
		       comment_handler->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		     }
		   }
		   {
		     int temp = line_number;
		     line_number = yyvsp[-2].ivalue;
		     doc_entry = new DocSection(yyvsp[-1].id,doc_stack[0]);
		     line_number = temp;
		   }
		   doc_stack_top = 1;
		   doc_stack[1] = doc_entry;
		   { 
		     int ii;
		     for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
		       doc_stack[doc_stack_top]->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		     }
		   }
		 }
	       ;
    break;}
case 37:
#line 973 "parser.y"
{
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
		       for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
			 comment_handler->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		       }
		     }
		     {
		       int temp = line_number;
		       line_number = yyvsp[-2].ivalue;
		       doc_entry = new DocSection(yyvsp[-1].id,doc_stack[1]);
		       line_number = temp;
		     }
		     doc_stack_top = 2;
		     doc_stack[2] = doc_entry;
		     { 
		       int ii;
		       for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
			 doc_stack[doc_stack_top]->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		       }
		     }
		   }
		 }
	       ;
    break;}
case 38:
#line 1009 "parser.y"
{
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
		       for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
			 comment_handler->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		       }
		     }
		     {
		       int temp = line_number;
		       line_number = yyvsp[-2].ivalue;
		       doc_entry = new DocSection(yyvsp[-1].id,doc_stack[2]);
		       line_number = temp;
		     }
		     doc_stack_top = 3;
		     doc_stack[3] = doc_entry;
		     { 
		       int ii;
		       for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
			 doc_stack[doc_stack_top]->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		       }
		     }
		   }
		 }
	       ;
    break;}
case 39:
#line 1046 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   fprintf(stderr,"%%alpha directive is obsolete.  Use '%%style sort' instead.\n");
		   handler_stack[0]->style("sort",0);
		   doc_stack[0]->style("sort",0);
		 }
	       ;
    break;}
case 40:
#line 1054 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   fprintf(stderr,"%%raw directive is obsolete. Use '%%style nosort' instead.\n");
		   handler_stack[0]->style("nosort",0);
		   doc_stack[0]->style("nosort",0);
		 }
	       ;
    break;}
case 41:
#line 1062 "parser.y"
{ ;
    break;}
case 42:
#line 1066 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   yyvsp[0].id[strlen(yyvsp[0].id) - 1] = 0;
		   doc_entry = new DocText(yyvsp[0].id,doc_stack[doc_stack_top]);
		   doc_entry = 0;
		 }
	       ;
    break;}
case 43:
#line 1075 "parser.y"
{ ;
    break;}
case 44:
#line 1079 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   init_language();
		   yyvsp[0].id[strlen(yyvsp[0].id) - 1] = 0;
//		   fprintf(f_header,"#line %d \"%s\"\n", start_line, input_file);
		   fprintf(f_header, "%s\n", yyvsp[0].id);
		 }
	       ;
    break;}
case 45:
#line 1090 "parser.y"
{
                 if (allow && (!WrapExtern)) {
		   init_language();
		   yyvsp[0].id[strlen(yyvsp[0].id) - 1] = 0;
		   fprintf(f_wrappers,"%s\n",yyvsp[0].id);
		 }
	       ;
    break;}
case 46:
#line 1100 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   init_language();
		   yyvsp[0].id[strlen(yyvsp[0].id) -1] = 0;
		   fprintf(f_init,"%s\n", yyvsp[0].id);
		 }
	       ;
    break;}
case 47:
#line 1109 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   init_language();
		   yyvsp[0].id[strlen(yyvsp[0].id) - 1] = 0;
		   fprintf(f_header, "%s\n", yyvsp[0].id);
		   start_inline(yyvsp[0].id,start_line);
		 }
	       ;
    break;}
case 48:
#line 1119 "parser.y"
{
		 if (allow && (!WrapExtern)) {
		   fprintf(stderr,"%s\n", yyvsp[0].id);
		 }
	       ;
    break;}
case 49:
#line 1125 "parser.y"
{
                 if (allow && (!WrapExtern)) {
                   fprintf(stderr,"%s\n", yyvsp[0].id);
                 }
               ;
    break;}
case 50:
#line 1132 "parser.y"
{
                   DocOnly = 1;
               ;
    break;}
case 51:
#line 1138 "parser.y"
{
		 if (allow) {
		   if (!module_init) {
		     lang->set_init(yyvsp[-1].id);
		     module_init = 1;
		     init_language();
		   } else {
		     if (Verbose)
		       fprintf(stderr,"%s : Line %d. %%init %s ignored.\n",
			       input_file, line_number, yyvsp[-1].id);
		   }
		   if (yyvsp[0].ilist.count > 0) {
		     fprintf(stderr,"%s : Line %d. Warning. Init list no longer supported.\n",
			     input_file,line_number);
		   }
		 }
		 for (i = 0; i < yyvsp[0].ilist.count; i++)
		   if (yyvsp[0].ilist.names[i]) delete [] yyvsp[0].ilist.names[i];
		 delete [] yyvsp[0].ilist.names;
	       ;
    break;}
case 52:
#line 1160 "parser.y"
{
		 if (allow) {
		   if (yyvsp[0].ilist.count)
		     lang->set_module(yyvsp[-1].id,yyvsp[0].ilist.names);
		   else
		     lang->set_module(yyvsp[-1].id,0);
		   module_init = 1;
		   init_language();
		 }
		 for (i = 0; i < yyvsp[0].ilist.count; i++)
		   if (yyvsp[0].ilist.names[i]) delete [] yyvsp[0].ilist.names[i];
		 delete [] yyvsp[0].ilist.names;
	       ;
    break;}
case 53:
#line 1176 "parser.y"
{
		 if (allow) {
		   if ((yyvsp[0].dtype.type != T_ERROR) && (yyvsp[0].dtype.type != T_SYMBOL)) {
		     init_language();
		     temp_typeptr = new DataType(yyvsp[0].dtype.type);
		     create_constant(yyvsp[-1].id, temp_typeptr, yyvsp[0].dtype.id);
		     delete temp_typeptr;
		   } else if (yyvsp[0].dtype.type == T_SYMBOL) {
		     // Add a symbol to the SWIG symbol table
		     if (add_symbol(yyvsp[-1].id,(DataType *) 0, (char *) 0)) {
		       fprintf(stderr,"%s : Line %d. Warning. Symbol %s already defined.\n", 
			       input_file,line_number, yyvsp[-1].id);
		     }
		   }
		 }
	       ;
    break;}
case 54:
#line 1195 "parser.y"
{
		 if (Verbose) {
		   fprintf(stderr,"%s : Line %d.  CPP Macro ignored.\n", input_file, line_number);
		 }
	       ;
    break;}
case 55:
#line 1202 "parser.y"
{
		 remove_symbol(yyvsp[0].id);
	       ;
    break;}
case 56:
#line 1208 "parser.y"
{ scanner_clear_start(); ;
    break;}
case 57:
#line 1208 "parser.y"
{ 
		 if (allow) {
		   init_language();
		   if (yyvsp[-5].id) {
		     temp_type.type = T_INT;
		     temp_type.is_pointer = 0;
		     temp_type.implicit_ptr = 0;
		     sprintf(temp_type.name,"int");
		     temp_type.typedef_add(yyvsp[-5].id,1);
		   }
		 }
	       ;
    break;}
case 58:
#line 1223 "parser.y"
{ scanner_clear_start(); ;
    break;}
case 59:
#line 1223 "parser.y"
{
		 if (allow) {
		   init_language();
		   temp_type.type = T_INT;
		   temp_type.is_pointer = 0;
		   temp_type.implicit_ptr = 0;
		   sprintf(temp_type.name,"int");
		   Active_typedef = new DataType(&temp_type);
		   temp_type.typedef_add(yyvsp[0].id,1);
		 }
	       ;
    break;}
case 60:
#line 1233 "parser.y"
{ ;
    break;}
case 61:
#line 1243 "parser.y"
{
		   TMParm *p;
                   skip_brace();
		   p = yyvsp[-1].tmparm;
		   while (p) {
		     typemap_register(yyvsp[-3].id,yyvsp[-5].id,p->p->t,p->p->name,CCode,p->args);
		     p = p->next;
                   }
		   delete yyvsp[-5].id;
		   delete yyvsp[-3].id;
	       ;
    break;}
case 62:
#line 1256 "parser.y"
{
		 if (!typemap_lang) {
		   fprintf(stderr,"SWIG internal error. No typemap_lang specified.\n");
		   fprintf(stderr,"typemap on %s : Line %d. will be ignored.\n",input_file,line_number);
		   FatalError();
		 } else {
		   TMParm *p;
		   skip_brace();
		   p = yyvsp[-1].tmparm;
		   while (p) {
		     typemap_register(yyvsp[-3].id,typemap_lang,p->p->t,p->p->name,CCode,p->args);
		     p = p->next;
		   }
		 }
		 delete yyvsp[-3].id;
	       ;
    break;}
case 63:
#line 1275 "parser.y"
{
		 TMParm *p;
		 p = yyvsp[-1].tmparm;
		 while (p) {
                   typemap_clear(yyvsp[-3].id,yyvsp[-5].id,p->p->t,p->p->name);
		   p = p->next;
		 }
		 delete yyvsp[-5].id;
		 delete yyvsp[-3].id;
	       ;
    break;}
case 64:
#line 1287 "parser.y"
{
		 if (!typemap_lang) {
		   fprintf(stderr,"SWIG internal error. No typemap_lang specified.\n");
		   fprintf(stderr,"typemap on %s : Line %d. will be ignored.\n",input_file,line_number);
		   FatalError();
		 } else {
		   TMParm *p;
		   p = yyvsp[-1].tmparm;
		   while (p) {
		     typemap_clear(yyvsp[-3].id,typemap_lang,p->p->t,p->p->name);
		     p = p->next;
		   }
		 }
		 delete yyvsp[-3].id;
	       ;
    break;}
case 65:
#line 1305 "parser.y"
{
                   TMParm *p;
		   p = yyvsp[-3].tmparm;
		   while (p) {
		     typemap_copy(yyvsp[-5].id,yyvsp[-7].id,yyvsp[-1].tmparm->p->t,yyvsp[-1].tmparm->p->name,p->p->t,p->p->name);
		     p = p->next;
		   }
		   delete yyvsp[-7].id;
		   delete yyvsp[-5].id;
		   delete yyvsp[-1].tmparm->p;
		   delete yyvsp[-1].tmparm;
	       ;
    break;}
case 66:
#line 1320 "parser.y"
{
		 if (!typemap_lang) {
		   fprintf(stderr,"SWIG internal error. No typemap_lang specified.\n");
		   fprintf(stderr,"typemap on %s : Line %d. will be ignored.\n",input_file,line_number);
		   FatalError();
		 } else {
                   TMParm *p;
		   p = yyvsp[-3].tmparm;
		   while (p) {
		     typemap_copy(yyvsp[-5].id,typemap_lang,yyvsp[-1].tmparm->p->t,yyvsp[-1].tmparm->p->name,p->p->t,p->p->name);
		     p = p->next;
		   }
		 }
		 delete yyvsp[-5].id;
		 delete yyvsp[-1].tmparm->p;
		 delete yyvsp[-1].tmparm;
	       ;
    break;}
case 67:
#line 1341 "parser.y"
{
		 TMParm *p;
		 p = yyvsp[-1].tmparm;
		 while(p) {
		   typemap_apply(yyvsp[-3].tmparm->p->t,yyvsp[-3].tmparm->p->name,p->p->t,p->p->name);
		   p = p->next;
		 }
		 delete yyvsp[-1].tmparm;
		 delete yyvsp[-3].tmparm->args;
		 delete yyvsp[-3].tmparm;
               ;
    break;}
case 68:
#line 1352 "parser.y"
{
		 TMParm *p;
		 p = yyvsp[-1].tmparm;
		 while (p) {
		   typemap_clear_apply(p->p->t, p->p->name);
		   p = p->next;
		 }
               ;
    break;}
case 69:
#line 1369 "parser.y"
{
                    skip_brace();
                    fragment_register("except",yyvsp[-2].id, CCode);
		    delete yyvsp[-2].id;
	       ;
    break;}
case 70:
#line 1376 "parser.y"
{
                    skip_brace();
                    fragment_register("except",typemap_lang, CCode);
               ;
    break;}
case 71:
#line 1383 "parser.y"
{
                     fragment_clear("except",yyvsp[-2].id);
               ;
    break;}
case 72:
#line 1388 "parser.y"
{
                     fragment_clear("except",typemap_lang);
	       ;
    break;}
case 73:
#line 1394 "parser.y"
{ ;
    break;}
case 74:
#line 1395 "parser.y"
{ ;
    break;}
case 75:
#line 1396 "parser.y"
{ ;
    break;}
case 76:
#line 1397 "parser.y"
{
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
	       ;
    break;}
case 77:
#line 1415 "parser.y"
{ ;
    break;}
case 78:
#line 1416 "parser.y"
{ ;
    break;}
case 79:
#line 1420 "parser.y"
{ ;
    break;}
case 80:
#line 1424 "parser.y"
{
		 { 
		   int ii,jj;
		   for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
		     comment_handler->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		     for (jj = 0; jj < doc_stack_top; jj++) 
		       doc_stack[jj]->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		     if (doctitle)
		       doctitle->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		     doc->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		   }
		 }
	       ;
    break;}
case 81:
#line 1440 "parser.y"
{
		 { 
		   int ii;
		   for (ii = 0; ii < yyvsp[0].dlist.count; ii++) {
		     comment_handler = new CommentHandler(comment_handler);
		     handler_stack[doc_stack_top] = comment_handler;
		     comment_handler->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		     doc_stack[doc_stack_top]->style(yyvsp[0].dlist.names[ii],yyvsp[0].dlist.values[ii]);
		   }
		 }
	       ;
    break;}
case 82:
#line 1453 "parser.y"
{ ;
    break;}
case 83:
#line 1459 "parser.y"
{
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
	       ;
    break;}
case 84:
#line 1473 "parser.y"
{
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
	       ;
    break;}
case 85:
#line 1492 "parser.y"
{
		 if (allow) {
		   init_language();
		   /* Add a new typedef */
		   Active_typedef = new DataType(yyvsp[-1].type);
		   yyvsp[-1].type->is_pointer += yyvsp[0].decl.is_pointer;
		   yyvsp[-1].type->typedef_add(yyvsp[0].decl.id);
		   /* If this is %typedef, add it to the header */
		   if (yyvsp[-2].ivalue) 
		     fprintf(f_header,"typedef %s %s;\n", yyvsp[-1].type->print_full(), yyvsp[0].decl.id);
		   cplus_register_type(yyvsp[0].decl.id);
		 }
	       ;
    break;}
case 86:
#line 1504 "parser.y"
{ ;
    break;}
case 87:
#line 1508 "parser.y"
{
		 if (allow) {
		   init_language();
		   /* Typedef'd pointer */
		   if (yyvsp[-9].ivalue) {
		     sprintf(temp_name,"(*%s)",yyvsp[-5].id);
		     fprintf(f_header,"typedef ");
		     emit_extern_func(temp_name, yyvsp[-8].type,yyvsp[-2].pl,0,f_header);
		   }
		   strcpy(yyvsp[-8].type->name,"<function ptr>");
		   yyvsp[-8].type->type = T_USER;
		   yyvsp[-8].type->is_pointer = 1;
		   yyvsp[-8].type->typedef_add(yyvsp[-5].id,1);
		   cplus_register_type(yyvsp[-5].id);
		 }
		 delete yyvsp[-8].type;
		 delete yyvsp[-5].id;
		 delete yyvsp[-2].pl;
	       ;
    break;}
case 88:
#line 1530 "parser.y"
{
		 if (allow) {
		   init_language();
		   if (yyvsp[-10].ivalue) {
		     yyvsp[-9].type->is_pointer += yyvsp[-8].ivalue;
		     sprintf(temp_name,"(*%s)",yyvsp[-5].id);
		     fprintf(f_header,"typedef ");
		     emit_extern_func(temp_name, yyvsp[-9].type,yyvsp[-2].pl,0,f_header);
		   }

		   /* Typedef'd pointer */
		   strcpy(yyvsp[-9].type->name,"<function ptr>");
		   yyvsp[-9].type->type = T_USER;
		   yyvsp[-9].type->is_pointer = 1;
		   yyvsp[-9].type->typedef_add(yyvsp[-5].id,1);
		   cplus_register_type(yyvsp[-5].id);
		 }
		 delete yyvsp[-9].type;
		 delete yyvsp[-5].id;
		 delete yyvsp[-2].pl;
	       ;
    break;}
case 89:
#line 1554 "parser.y"
{
		 if (allow) {
		   init_language();
		   Active_typedef = new DataType(yyvsp[-2].type);
		   // This datatype is going to be readonly
			
		   yyvsp[-2].type->status = STAT_READONLY | STAT_REPLACETYPE;
		   yyvsp[-2].type->is_pointer += yyvsp[-1].decl.is_pointer;
		   // Turn this into a "pointer" corresponding to the array
		   yyvsp[-2].type->is_pointer++;
		   yyvsp[-2].type->arraystr = copy_string(ArrayString);
		   yyvsp[-2].type->typedef_add(yyvsp[-1].decl.id);
		   fprintf(stderr,"%s : Line %d. Warning. Array type %s will be read-only without a typemap\n",input_file,line_number, yyvsp[-1].decl.id);
		   cplus_register_type(yyvsp[-1].decl.id);

		 }
	       ;
    break;}
case 90:
#line 1570 "parser.y"
{ ;
    break;}
case 91:
#line 1583 "parser.y"
{
                if (allow) {
		  if (Active_typedef) {
		    DataType *t;
		    t = new DataType(Active_typedef);
		    t->is_pointer += yyvsp[-1].decl.is_pointer;
		    t->typedef_add(yyvsp[-1].decl.id);
		    cplus_register_type(yyvsp[-1].decl.id);
		    delete t;
		  }
		}
              ;
    break;}
case 92:
#line 1595 "parser.y"
{
		    DataType *t;
		    t = new DataType(Active_typedef);
		    t->status = STAT_READONLY | STAT_REPLACETYPE;
		    t->is_pointer += yyvsp[-1].decl.is_pointer + 1;
		    t->arraystr = copy_string(ArrayString);
		    t->typedef_add(yyvsp[-1].decl.id);
		    cplus_register_type(yyvsp[-1].decl.id);
		    delete t;
    		    fprintf(stderr,"%s : Line %d. Warning. Array type %s will be read-only without a typemap.\n",input_file,line_number, yyvsp[-1].decl.id);
	      ;
    break;}
case 93:
#line 1606 "parser.y"
{ ;
    break;}
case 94:
#line 1626 "parser.y"
{
		 /* Push old if-then-else status */
		 if_push();
		 /* Look a symbol up in the symbol table */
		 if (lookup_symbol(yyvsp[0].id)) {
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
                ;
    break;}
case 95:
#line 1652 "parser.y"
{
		 if_push();
		 if (lookup_symbol(yyvsp[0].id)) {
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
	       ;
    break;}
case 96:
#line 1675 "parser.y"
{
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
	       ;
    break;}
case 97:
#line 1694 "parser.y"
{
		 if ((!in_then) && (!in_else)) {
		   fprintf(stderr,"%s : Line %d. Misplaced endif\n", input_file, line_number);
		   FatalError();
		 } else {
		   if_pop();
		 }
	       ;
    break;}
case 98:
#line 1704 "parser.y"
{
		 /* Push old if-then-else status */
		 if_push();
		 if (yyvsp[0].ivalue) {
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
	       ;
    break;}
case 99:
#line 1730 "parser.y"
{
		 /* have to pop old if clause off */
		 if_pop();

		 /* Push old if-then-else status */
		 if_push();
		 if (yyvsp[0].ivalue) {
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
	       ;
    break;}
case 100:
#line 1759 "parser.y"
{

                 /* Look ID up in the symbol table */
                    if (lookup_symbol(yyvsp[-1].id)) {
		      yyval.ivalue = 1;
		    } else {
		      yyval.ivalue = 0;
		    }
               ;
    break;}
case 101:
#line 1768 "parser.y"
{
		 if (lookup_symbol(yyvsp[0].id)) {
		   yyval.ivalue = 1;
		 } else {
		   yyval.ivalue = 0;
		 }
	       ;
    break;}
case 102:
#line 1775 "parser.y"
{
                      if (yyvsp[0].ivalue) yyval.ivalue = 0;
		      else yyval.ivalue = 1;
	       ;
    break;}
case 103:
#line 1781 "parser.y"
{
		 if (allow && (!WrapExtern))
		   lang->pragma(yyvsp[-4].id,yyvsp[-2].id,yyvsp[-1].id);
		   fprintf(stderr,"%s : Line %d. Warning. '%%pragma(lang,opt=value)' syntax is obsolete.\n",
			   input_file,line_number);
		   fprintf(stderr,"        Use '%%pragma(lang) opt=value' instead.\n");
	       ;
    break;}
case 104:
#line 1789 "parser.y"
{
                 if (allow && (!WrapExtern)) 
		   swig_pragma(yyvsp[-1].id,yyvsp[0].id);
    	       ;
    break;}
case 105:
#line 1793 "parser.y"
{
		 if (allow && (!WrapExtern))
		   lang->pragma(yyvsp[-3].id,yyvsp[-1].id,yyvsp[0].id);
	       ;
    break;}
case 106:
#line 1801 "parser.y"
{ ;
    break;}
case 107:
#line 1802 "parser.y"
{
		 if (allow) {
		   init_language();
		   temp_typeptr = new DataType(Active_type);
		   temp_typeptr->is_pointer += yyvsp[-2].decl.is_pointer;
		   if (yyvsp[-1].ivalue > 0) {
		     temp_typeptr->is_pointer++;
		     temp_typeptr->status = STAT_READONLY;
		     temp_typeptr->arraystr = copy_string(ArrayString);
		   }
		   if (yyvsp[-2].decl.is_reference) {
		     fprintf(stderr,"%s : Line %d. Error. Linkage to C++ reference not allowed.\n", input_file, line_number);
		     FatalError();
		   } else {
		     if (temp_typeptr->qualifier) {
		       if ((strcmp(temp_typeptr->qualifier,"const") == 0)) {
			 /* Okay.  This is really some sort of C++ constant here. */
			 if (yyvsp[0].dtype.type != T_ERROR)
			   create_constant(yyvsp[-2].decl.id, temp_typeptr, yyvsp[0].dtype.id);
		       } else 
			 create_variable(Active_extern,yyvsp[-2].decl.id, temp_typeptr);
		     } else
		       create_variable(Active_extern, yyvsp[-2].decl.id, temp_typeptr);
		   }
		   delete temp_typeptr;
		 }
	       ;
    break;}
case 108:
#line 1828 "parser.y"
{ ;
    break;}
case 109:
#line 1829 "parser.y"
{
		 if (allow) {
		   init_language();
		   temp_typeptr = new DataType(Active_type);
		   temp_typeptr->is_pointer += yyvsp[-4].decl.is_pointer;
		   temp_typeptr->is_reference = yyvsp[-4].decl.is_reference;
		   create_function(Active_extern, yyvsp[-4].decl.id, temp_typeptr, yyvsp[-2].pl);
		   delete temp_typeptr;
		 }
		 delete yyvsp[-2].pl;
	       ;
    break;}
case 110:
#line 1839 "parser.y"
{ ;
    break;}
case 111:
#line 1842 "parser.y"
{
                   yyval.dtype = yyvsp[-1].dtype;
                 ;
    break;}
case 112:
#line 1845 "parser.y"
{
                   yyval.dtype.type = T_SYMBOL;
	       ;
    break;}
case 113:
#line 1848 "parser.y"
{
		 if (Verbose) 
		   fprintf(stderr,"%s : Line %d.  Warning. Unable to parse #define (ignored)\n", input_file, line_number);
		 yyval.dtype.type = T_ERROR;
	       ;
    break;}
case 114:
#line 1856 "parser.y"
{ yyval.ivalue = 1; ;
    break;}
case 115:
#line 1857 "parser.y"
{yyval.ivalue = 0; ;
    break;}
case 116:
#line 1858 "parser.y"
{
		 if (strcmp(yyvsp[0].id,"C") == 0) {
		   yyval.ivalue = 2;
		 } else {
		   fprintf(stderr,"%s : Line %d.  Unrecognized extern type \"%s\" (ignored).\n", input_file, line_number, yyvsp[0].id);
		   FatalError();
		 }
	       ;
    break;}
case 117:
#line 1870 "parser.y"
{ skip_brace(); ;
    break;}
case 118:
#line 1879 "parser.y"
{
                 if ((yyvsp[-1].p->t->type != T_VOID) || (yyvsp[-1].p->t->is_pointer))
		   yyvsp[0].pl->insert(yyvsp[-1].p,0);
		 yyval.pl = yyvsp[0].pl;
		 delete yyvsp[-1].p;
		;
    break;}
case 119:
#line 1885 "parser.y"
{ yyval.pl = new ParmList;;
    break;}
case 120:
#line 1888 "parser.y"
{
		 yyvsp[0].pl->insert(yyvsp[-1].p,0);
		 yyval.pl = yyvsp[0].pl;
		 delete yyvsp[-1].p;
                ;
    break;}
case 121:
#line 1893 "parser.y"
{ yyval.pl = new ParmList;;
    break;}
case 122:
#line 1896 "parser.y"
{
                  yyval.p = yyvsp[0].p;
		  if (typemap_check("ignore",typemap_lang,yyval.p->t,yyval.p->name))
		    yyval.p->ignore = 1;
               ;
    break;}
case 123:
#line 1901 "parser.y"
{
                  yyval.p = yyvsp[0].p;
                  yyval.p->call_type = yyval.p->call_type | yyvsp[-1].ivalue;
		  if (InArray && (yyval.p->call_type & CALL_VALUE)) {
		     fprintf(stderr,"%s : Line %d. Error. Can't use %%val with an array.\n", input_file, line_number);
		     FatalError();
		  }
		  if (!yyval.p->t->is_pointer) {
		     fprintf(stderr,"%s : Line %d. Error. Can't use %%val or %%out with a non-pointer argument.\n", input_file, line_number);
		     FatalError();
		  } else {
		    yyval.p->t->is_pointer--;
		  }
		;
    break;}
case 124:
#line 1916 "parser.y"
{
		    if (InArray) {
		      yyvsp[-1].type->is_pointer++;
		      if (Verbose) {
			fprintf(stderr,"%s : Line %d. Warning. Array %s", input_file, line_number, yyvsp[-1].type->print_type());
			print_array();
			fprintf(stderr," has been converted to %s.\n", yyvsp[-1].type->print_type());
		      }
		      // Add array string to the type
		      yyvsp[-1].type->arraystr = copy_string(ArrayString.get());
		    } 
		    yyval.p = new Parm(yyvsp[-1].type,yyvsp[0].id);
		    yyval.p->call_type = 0;
		    yyval.p->defvalue = DefArg;
		    if ((yyvsp[-1].type->type == T_USER) && !(yyvsp[-1].type->is_pointer)) {
		      if (Verbose)
			fprintf(stderr,"%s : Line %d. Warning : Parameter of type '%s'\nhas been remapped to '%s *' and will be called using *((%s *) ptr).\n",
				input_file, line_number, yyvsp[-1].type->name, yyvsp[-1].type->name, yyvsp[-1].type->name);

		      yyval.p->call_type = CALL_REFERENCE;
		      yyval.p->t->is_pointer++;
		    }
		    delete yyvsp[-1].type;
		    delete yyvsp[0].id;
                 ;
    break;}
case 125:
#line 1942 "parser.y"
{
		   yyval.p = new Parm(yyvsp[-2].type,yyvsp[0].id);
		   yyval.p->t->is_pointer += yyvsp[-1].ivalue;
		   yyval.p->call_type = 0;
		   yyval.p->defvalue = DefArg;
		   if (InArray) {
		     yyval.p->t->is_pointer++;
		     if (Verbose) {
		       fprintf(stderr,"%s : Line %d. Warning. Array %s", input_file, line_number, yyval.p->t->print_type());
		       print_array();
		       fprintf(stderr," has been converted to %s.\n", yyval.p->t->print_type());
		     }
		     // Add array string to the type
		     yyval.p->t->arraystr = copy_string(ArrayString.get());
		    }
		   delete yyvsp[-2].type;
		   delete yyvsp[0].id;
		;
    break;}
case 126:
#line 1961 "parser.y"
{
		  yyval.p = new Parm(yyvsp[-2].type,yyvsp[0].id);
		  yyval.p->t->is_reference = 1;
		  yyval.p->call_type = 0;
		  yyval.p->t->is_pointer++;
		  yyval.p->defvalue = DefArg;
		  if (!CPlusPlus) {
			fprintf(stderr,"%s : Line %d. Warning.  Use of C++ Reference detected.  Use the -c++ option.\n", input_file, line_number);
		  }
		  delete yyvsp[-2].type;
		  delete yyvsp[0].id;
		;
    break;}
case 127:
#line 1973 "parser.y"
{
                  fprintf(stderr,"%s : Line %d. Error. Function pointer not allowed (remap with typedef).\n", input_file, line_number);
		  FatalError();
		  yyval.p = new Parm(yyvsp[-7].type,yyvsp[-4].id);
		  yyval.p->t->type = T_ERROR;
		  yyval.p->name = copy_string(yyvsp[-4].id);
		  strcpy(yyval.p->t->name,"<function ptr>");
		  delete yyvsp[-7].type;
		  delete yyvsp[-4].id;
		  delete yyvsp[-1].pl;
		;
    break;}
case 128:
#line 1984 "parser.y"
{
                  fprintf(stderr,"%s : Line %d. Variable length arguments not supported (ignored).\n", input_file, line_number);
		  yyval.p = new Parm(new DataType(T_INT),"varargs");
		  yyval.p->t->type = T_ERROR;
		  yyval.p->name = copy_string("varargs");
		  strcpy(yyval.p->t->name,"<varargs>");
		  FatalError();
		;
    break;}
case 129:
#line 1994 "parser.y"
{
                    yyval.id = yyvsp[-1].id; 
                    InArray = 0;
		    if (yyvsp[0].dtype.type == T_CHAR)
		      DefArg = copy_string(ConstChar);
		    else
		      DefArg = copy_string(yyvsp[0].dtype.id);
                    if (yyvsp[0].dtype.id) delete yyvsp[0].dtype.id;
                ;
    break;}
case 130:
#line 2003 "parser.y"
{
                    yyval.id = yyvsp[-1].id; 
                    InArray = yyvsp[0].ivalue; 
                    DefArg = 0;
               ;
    break;}
case 131:
#line 2008 "parser.y"
{
                         yyval.id = new char[1];
                         yyval.id[0] = 0;
                         InArray = yyvsp[0].ivalue;
                         DefArg = 0;
               ;
    break;}
case 132:
#line 2014 "parser.y"
{ yyval.id = new char[1];
	                 yyval.id[0] = 0;
                         InArray = 0;
                         DefArg = 0;
               ;
    break;}
case 133:
#line 2021 "parser.y"
{ yyval.dtype = yyvsp[0].dtype; ;
    break;}
case 134:
#line 2022 "parser.y"
{
		 yyval.dtype.id = new char[strlen(yyvsp[0].id)+2];
		 yyval.dtype.id[0] = '&';
		 strcpy(&yyval.dtype.id[1], yyvsp[0].id);
		 yyval.dtype.type = T_USER;
	       ;
    break;}
case 135:
#line 2028 "parser.y"
{
		 skip_brace();
		 yyval.dtype.id = 0; yyval.dtype.type = T_INT;
	       ;
    break;}
case 136:
#line 2032 "parser.y"
{
               ;
    break;}
case 137:
#line 2034 "parser.y"
{yyval.dtype.id = 0; yyval.dtype.type = T_INT;;
    break;}
case 138:
#line 2037 "parser.y"
{ yyval.ivalue = CALL_VALUE; ;
    break;}
case 139:
#line 2038 "parser.y"
{ yyval.ivalue = CALL_OUTPUT; ;
    break;}
case 140:
#line 2041 "parser.y"
{
                 yyval.ivalue = yyvsp[-1].ivalue | yyvsp[0].ivalue;
               ;
    break;}
case 141:
#line 2044 "parser.y"
{
                 yyval.ivalue = yyvsp[0].ivalue;
	       ;
    break;}
case 142:
#line 2051 "parser.y"
{ yyval.decl.id = yyvsp[0].id;
                      yyval.decl.is_pointer = 0;
		      yyval.decl.is_reference = 0;
                ;
    break;}
case 143:
#line 2055 "parser.y"
{
                      yyval.decl.id = yyvsp[0].id;
		      yyval.decl.is_pointer = yyvsp[-1].ivalue;
		      yyval.decl.is_reference = 0;
	       ;
    break;}
case 144:
#line 2060 "parser.y"
{
		      yyval.decl.id = yyvsp[0].id;
		      yyval.decl.is_pointer = 1;
		      yyval.decl.is_reference = 1;
		      if (!CPlusPlus) {
			fprintf(stderr,"%s : Line %d. Warning.  Use of C++ Reference detected.  Use the -c++ option.\n", input_file, line_number);
		      }
	       ;
    break;}
case 145:
#line 2070 "parser.y"
{ yyval.ivalue = 1; ;
    break;}
case 146:
#line 2071 "parser.y"
{ yyval.ivalue = yyvsp[0].ivalue + 1;;
    break;}
case 147:
#line 2075 "parser.y"
{
		 yyval.ivalue = yyvsp[0].ivalue + 1;
		 "[]" >> ArrayString;
              ;
    break;}
case 148:
#line 2079 "parser.y"
{
                 yyval.ivalue = yyvsp[0].ivalue + 1;
		 "]" >> ArrayString;
		 yyvsp[-2].dtype.id >> ArrayString;
		 "[" >> ArrayString;
              ;
    break;}
case 149:
#line 2086 "parser.y"
{
                 yyval.ivalue = yyvsp[0].ivalue;
              ;
    break;}
case 150:
#line 2089 "parser.y"
{ yyval.ivalue = 0;
                        ArrayString = "";
              ;
    break;}
case 151:
#line 2097 "parser.y"
{
                   yyval.type = yyvsp[0].type;
               ;
    break;}
case 152:
#line 2100 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 153:
#line 2103 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 154:
#line 2106 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 155:
#line 2109 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 156:
#line 2112 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 157:
#line 2115 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 158:
#line 2118 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 159:
#line 2121 "parser.y"
{
                   if (yyvsp[0].type) yyval.type = yyvsp[0].type;
		   else yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 160:
#line 2125 "parser.y"
{
                   if (yyvsp[0].type) yyval.type = yyvsp[0].type;
		   else yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 161:
#line 2129 "parser.y"
{
		 yyval.type = yyvsp[-1].type;
		 if (strlen(yyvsp[0].id) > 0) {
		    if ((strlen(yyvsp[0].id) + strlen(yyval.type->name)) >= MAX_NAME) {
		      fprintf(stderr,"%s : Line %d. Fatal error. Type-name is too long!\n", 
			      input_file, line_number);
		    } else {
		      strcat(yyval.type->name,yyvsp[0].id);
		    }
		  }
	       ;
    break;}
case 162:
#line 2140 "parser.y"
{
		  yyval.type = new DataType;
		  strcpy(yyval.type->name,yyvsp[-1].id);
		  yyval.type->type = T_USER;
		  /* Do a typedef lookup */
		  yyval.type->typedef_resolve();
		  if (strlen(yyvsp[0].id) > 0) {
		    if ((strlen(yyvsp[0].id) + strlen(yyval.type->name)) >= MAX_NAME) {
		      fprintf(stderr,"%s : Line %d. Fatal error. Type-name is too long!\n", 
			      input_file, line_number);
		    } else {
		      strcat(yyval.type->name,yyvsp[0].id);
		    }
		  }
	       ;
    break;}
case 163:
#line 2155 "parser.y"
{
		  yyval.type = yyvsp[0].type;
                  yyval.type->qualifier = new char[6];
		  strcpy(yyval.type->qualifier,"const");
     	       ;
    break;}
case 164:
#line 2160 "parser.y"
{
                  yyval.type = new DataType;
		  sprintf(yyval.type->name,"%s %s",yyvsp[-1].id, yyvsp[0].id);
		  yyval.type->type = T_USER;
	       ;
    break;}
case 165:
#line 2165 "parser.y"
{
                  yyval.type = new DataType;
                  sprintf(yyval.type->name,"%s::%s",yyvsp[-2].id,yyvsp[0].id);
                  yyval.type->type = T_USER;
		  yyval.type->typedef_resolve();
               ;
    break;}
case 166:
#line 2174 "parser.y"
{
                  yyval.type = new DataType;
                  sprintf(yyval.type->name,"%s", yyvsp[0].id);
                  yyval.type->type = T_USER;
                  yyval.type->typedef_resolve(1);
               ;
    break;}
case 167:
#line 2180 "parser.y"
{
                  yyval.type = new DataType;
                  sprintf(yyval.type->name,"enum %s", yyvsp[0].id);
                  yyval.type->type = T_INT;
                  yyval.type->typedef_resolve(1);
               ;
    break;}
case 168:
#line 2190 "parser.y"
{
                   yyval.type = yyvsp[0].type;
               ;
    break;}
case 169:
#line 2193 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 170:
#line 2196 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 171:
#line 2199 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 172:
#line 2202 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 173:
#line 2205 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 174:
#line 2208 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 175:
#line 2211 "parser.y"
{
                   yyval.type = yyvsp[0].type;
	       ;
    break;}
case 176:
#line 2214 "parser.y"
{
                   if (yyvsp[0].type) yyval.type = yyvsp[0].type;
		   else yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 177:
#line 2218 "parser.y"
{
                   if (yyvsp[0].type) yyval.type = yyvsp[0].type;
		   else yyval.type = yyvsp[-1].type;
	       ;
    break;}
case 178:
#line 2222 "parser.y"
{
		   yyval.type = yyvsp[-1].type;
		   strcat(yyval.type->name,yyvsp[0].id);
	       ;
    break;}
case 179:
#line 2226 "parser.y"
{
		  yyval.type = yyvsp[0].type;
                  yyval.type->qualifier = new char[6];
		  strcpy(yyval.type->qualifier,"const");
     	       ;
    break;}
case 180:
#line 2231 "parser.y"
{
                  yyval.type = new DataType;
		  sprintf(yyval.type->name,"%s %s",yyvsp[-1].id, yyvsp[0].id);
		  yyval.type->type = T_USER;
	       ;
    break;}
case 181:
#line 2240 "parser.y"
{
                   yyval.type = (DataType *) 0;
               ;
    break;}
case 182:
#line 2243 "parser.y"
{
                   yyval.type = yyvsp[0].type;
		   yyval.type->type = T_INT;
		   sprintf(temp_name,"signed %s",yyvsp[0].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 183:
#line 2249 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
		   yyval.type->type = T_SHORT;
		   sprintf(temp_name,"signed %s",yyvsp[-1].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 184:
#line 2255 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
		   yyval.type->type = T_LONG;
		   sprintf(temp_name,"signed %s",yyvsp[-1].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 185:
#line 2261 "parser.y"
{
                   yyval.type = yyvsp[0].type;
		   yyval.type->type = T_SCHAR;
		   sprintf(temp_name,"signed %s",yyvsp[0].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 186:
#line 2271 "parser.y"
{
                   yyval.type = (DataType *) 0;
               ;
    break;}
case 187:
#line 2274 "parser.y"
{
                   yyval.type = yyvsp[0].type;
		   yyval.type->type = T_UINT;
		   sprintf(temp_name,"unsigned %s",yyvsp[0].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 188:
#line 2280 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
		   yyval.type->type = T_USHORT;
		   sprintf(temp_name,"unsigned %s",yyvsp[-1].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 189:
#line 2286 "parser.y"
{
                   yyval.type = yyvsp[-1].type;
		   yyval.type->type = T_ULONG;
		   sprintf(temp_name,"unsigned %s",yyvsp[-1].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 190:
#line 2292 "parser.y"
{
                   yyval.type = yyvsp[0].type;
		   yyval.type->type = T_UCHAR;
		   sprintf(temp_name,"unsigned %s",yyvsp[0].type->name);
		   strcpy(yyval.type->name,temp_name);
	       ;
    break;}
case 191:
#line 2300 "parser.y"
{ ;
    break;}
case 192:
#line 2301 "parser.y"
{ ;
    break;}
case 193:
#line 2304 "parser.y"
{ scanner_check_typedef(); ;
    break;}
case 194:
#line 2304 "parser.y"
{
                   yyval.dtype = yyvsp[0].dtype;
		   scanner_ignore_typedef();
		   if (ConstChar) delete ConstChar;
		   ConstChar = 0;
                ;
    break;}
case 195:
#line 2310 "parser.y"
{
                   yyval.dtype.id = yyvsp[0].id;
                   yyval.dtype.type = T_CHAR;
		   if (ConstChar) delete ConstChar;
		   ConstChar = new char[strlen(yyvsp[0].id)+3];
		   sprintf(ConstChar,"\"%s\"",yyvsp[0].id);
		;
    break;}
case 196:
#line 2317 "parser.y"
{
                   yyval.dtype.id = yyvsp[0].id;
		   yyval.dtype.type = T_CHAR;
		   if (ConstChar) delete ConstChar;
		   ConstChar = new char[strlen(yyvsp[0].id)+3];
		   sprintf(ConstChar,"'%s'",yyvsp[0].id);
		 ;
    break;}
case 197:
#line 2329 "parser.y"
{
                 yyval.ilist = yyvsp[-2].ilist;
		 yyval.ilist.names[yyval.ilist.count] = copy_string(yyvsp[0].id);
		 yyval.ilist.count++;
		 yyval.ilist.names[yyval.ilist.count] = (char *) 0;
                ;
    break;}
case 198:
#line 2335 "parser.y"
{
                 yyval.ilist.names = new char *[NI_NAMES];
		 yyval.ilist.count = 0;
		 for (i = 0; i < NI_NAMES; i++) 
		   yyval.ilist.names[i] = (char *) 0;
	       ;
    break;}
case 199:
#line 2345 "parser.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 200:
#line 2346 "parser.y"
{ yyval.id = (char *) 0;;
    break;}
case 201:
#line 2352 "parser.y"
{;
    break;}
case 202:
#line 2353 "parser.y"
{;
    break;}
case 203:
#line 2357 "parser.y"
{
		   temp_typeptr = new DataType(T_INT);
		   create_constant(yyvsp[0].id, temp_typeptr, yyvsp[0].id);
		   delete temp_typeptr;
		 ;
    break;}
case 204:
#line 2362 "parser.y"
{ scanner_check_typedef();;
    break;}
case 205:
#line 2362 "parser.y"
{
		   temp_typeptr = new DataType(yyvsp[0].dtype.type);
// Use enum name instead of value
// OLD		   create_constant($1, temp_typeptr, $4.id);
                   create_constant(yyvsp[-3].id, temp_typeptr, yyvsp[-3].id);
		   delete temp_typeptr;
                 ;
    break;}
case 206:
#line 2369 "parser.y"
{ ;
    break;}
case 207:
#line 2370 "parser.y"
{ ;
    break;}
case 208:
#line 2373 "parser.y"
{
                   yyval.dtype = yyvsp[0].dtype;
		   if ((yyval.dtype.type != T_INT) && (yyval.dtype.type != T_UINT) &&
		       (yyval.dtype.type != T_LONG) && (yyval.dtype.type != T_ULONG) &&
		       (yyval.dtype.type != T_SHORT) && (yyval.dtype.type != T_USHORT) && 
		       (yyval.dtype.type != T_SCHAR) && (yyval.dtype.type != T_UCHAR)) {
		     fprintf(stderr,"%s : Lind %d. Type error. Expecting an int\n",
			     input_file, line_number);
		     FatalError();
		   }

                ;
    break;}
case 209:
#line 2385 "parser.y"
{
                   yyval.dtype.id = yyvsp[0].id;
		   yyval.dtype.type = T_CHAR;
		 ;
    break;}
case 210:
#line 2396 "parser.y"
{ 
                  yyval.dtype.id = yyvsp[0].id;
                  yyval.dtype.type = T_INT;
                 ;
    break;}
case 211:
#line 2400 "parser.y"
{
                  yyval.dtype.id = yyvsp[0].id;
                  yyval.dtype.type = T_DOUBLE;
               ;
    break;}
case 212:
#line 2404 "parser.y"
{
                  yyval.dtype.id = yyvsp[0].id;
		  yyval.dtype.type = T_UINT;
	       ;
    break;}
case 213:
#line 2408 "parser.y"
{
                  yyval.dtype.id = yyvsp[0].id;
		  yyval.dtype.type = T_LONG;
	       ;
    break;}
case 214:
#line 2412 "parser.y"
{
                  yyval.dtype.id = yyvsp[0].id;
		  yyval.dtype.type = T_ULONG;
	       ;
    break;}
case 215:
#line 2416 "parser.y"
{
	          yyval.dtype.id = new char[strlen(yyvsp[-1].type->name)+9];
		  sprintf(yyval.dtype.id,"sizeof(%s)", yyvsp[-1].type->name);
		  yyval.dtype.type = T_INT;
	       ;
    break;}
case 216:
#line 2421 "parser.y"
{
		  yyval.dtype.id = new char[strlen(yyvsp[0].dtype.id)+strlen(yyvsp[-2].type->name)+3];
		  sprintf(yyval.dtype.id,"(%s)%s",yyvsp[-2].type->name,yyvsp[0].dtype.id);
		  yyval.dtype.type = yyvsp[-2].type->type;
	       ;
    break;}
case 217:
#line 2426 "parser.y"
{
		 yyval.dtype.id = lookup_symvalue(yyvsp[0].id);
		 if (yyval.dtype.id == (char *) 0)
		   yyval.dtype.id = yyvsp[0].id;
		 else {
		   yyval.dtype.id = new char[strlen(yyval.dtype.id)+3];
		   sprintf(yyval.dtype.id,"(%s)",lookup_symvalue(yyvsp[0].id));
		 }
		 temp_typeptr = lookup_symtype(yyvsp[0].id);
		 if (temp_typeptr) yyval.dtype.type = temp_typeptr->type;
		 else yyval.dtype.type = T_INT;
               ;
    break;}
case 218:
#line 2438 "parser.y"
{
                  yyval.dtype.id = new char[strlen(yyvsp[-2].id)+strlen(yyvsp[0].id)+3];
		  sprintf(yyval.dtype.id,"%s::%s",yyvsp[-2].id,yyvsp[0].id);
                  yyval.dtype.type = T_INT;
		  delete yyvsp[-2].id;
		  delete yyvsp[0].id;
               ;
    break;}
case 219:
#line 2445 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"+");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;
	       ;
    break;}
case 220:
#line 2451 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"-");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;
	       ;
    break;}
case 221:
#line 2457 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"*");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 222:
#line 2464 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"/");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 223:
#line 2471 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"&");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 if ((yyvsp[-2].dtype.type == T_DOUBLE) || (yyvsp[0].dtype.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 224:
#line 2482 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"|");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 if ((yyvsp[-2].dtype.type == T_DOUBLE) || (yyvsp[0].dtype.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 yyval.dtype.type = T_INT;
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 225:
#line 2494 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"^");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 if ((yyvsp[-2].dtype.type == T_DOUBLE) || (yyvsp[0].dtype.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 yyval.dtype.type = T_INT;
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 226:
#line 2506 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,"<<");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 if ((yyvsp[-2].dtype.type == T_DOUBLE) || (yyvsp[0].dtype.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 yyval.dtype.type = T_INT;
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 227:
#line 2518 "parser.y"
{
	         E_BINARY(yyval.dtype.id,yyvsp[-2].dtype.id,yyvsp[0].dtype.id,">>");
		 yyval.dtype.type = promote(yyvsp[-2].dtype.type,yyvsp[0].dtype.type);
		 if ((yyvsp[-2].dtype.type == T_DOUBLE) || (yyvsp[0].dtype.type == T_DOUBLE)) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		 }
		 yyval.dtype.type = T_INT;
		 delete yyvsp[-2].dtype.id;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 228:
#line 2530 "parser.y"
{
	          yyval.dtype.id = new char[strlen(yyvsp[0].dtype.id)+2];
		  sprintf(yyval.dtype.id,"-%s",yyvsp[0].dtype.id);
		  yyval.dtype.type = yyvsp[0].dtype.type;
		 delete yyvsp[0].dtype.id;

	       ;
    break;}
case 229:
#line 2537 "parser.y"
{
	          yyval.dtype.id = new char[strlen(yyvsp[0].dtype.id)+2];
		  sprintf(yyval.dtype.id,"~%s",yyvsp[0].dtype.id);
		  if (yyvsp[0].dtype.type == T_DOUBLE) {
		   fprintf(stderr,"%s : Line %d. Type error in constant expression (expecting integers).\n", input_file, line_number);
		   FatalError();
		  }
		  yyval.dtype.type = yyvsp[0].dtype.type;
		  delete yyvsp[0].dtype.id;
	       ;
    break;}
case 230:
#line 2547 "parser.y"
{
	          yyval.dtype.id = new char[strlen(yyvsp[-1].dtype.id)+3];
	          sprintf(yyval.dtype.id,"(%s)", yyvsp[-1].dtype.id);
		  yyval.dtype.type = yyvsp[-1].dtype.type;
		  delete yyvsp[-1].dtype.id;
	       ;
    break;}
case 231:
#line 2558 "parser.y"
{ ;
    break;}
case 232:
#line 2559 "parser.y"
{;
    break;}
case 233:
#line 2565 "parser.y"
{
	       char *iname;
	       if (allow) {
		 init_language();
		 DataType::new_scope();

		 sprintf(temp_name,"CPP_CLASS:%s\n",yyvsp[-2].id);
		 if (add_symbol(temp_name, (DataType *) 0, (char *) 0)) {
		   fprintf(stderr,"%s : Line %d. Error. %s %s is multiply defined.\n", input_file, line_number, yyvsp[-3].id, yyvsp[-2].id);
		   FatalError();
		 }
		 if ((!CPlusPlus) && (strcmp(yyvsp[-3].id,"class") == 0))
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);

		 iname = make_name(yyvsp[-2].id);
		 doc_entry = new DocClass(iname, doc_parent());
		 if (iname == yyvsp[-2].id) 
		   cplus_open_class(yyvsp[-2].id, 0, yyvsp[-3].id);
		 else
		   cplus_open_class(yyvsp[-2].id, iname, yyvsp[-3].id);
		 if (strcmp(yyvsp[-3].id,"class") == 0)
		   cplus_mode = CPLUS_PRIVATE;
		 else
		   cplus_mode = CPLUS_PUBLIC;
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
		 nested_list = 0;
		 // Merge in scope from base classes
		 cplus_inherit_scope(yyvsp[-1].ilist.count,yyvsp[-1].ilist.names);
	       }
              ;
    break;}
case 234:
#line 2596 "parser.y"
{
		if (allow) {
		  if (yyvsp[-4].ilist.names) {
		    if (strcmp(yyvsp[-6].id,"union") != 0)
		      cplus_inherit(yyvsp[-4].ilist.count, yyvsp[-4].ilist.names);
		    else {
		      fprintf(stderr,"%s : Line %d.  Inheritance not allowed for unions.\n",input_file, line_number);
		      FatalError();
		    }
		  }
		  // Clean up the inheritance list
		  if (yyvsp[-4].ilist.names) {
		    int j;
		    for (j = 0; j < yyvsp[-4].ilist.count; j++) {
		      if (yyvsp[-4].ilist.names[j]) delete [] yyvsp[-4].ilist.names[j];
		    }
		    delete [] yyvsp[-4].ilist.names;
		  }

		  // Dumped nested declarations (if applicable)
		  dump_nested(yyvsp[-5].id);

		  // Save and collapse current scope
		  cplus_register_scope(DataType::collapse_scope(yyvsp[-5].id));

		  // Restore the original doc entry for this class
		  doc_entry = doc_stack[doc_stack_top];
		  cplus_class_close((char *) 0); 
		  doc_entry = 0;
		  // Bump the documentation stack back down
		  doc_stack_top--;
		  cplus_mode = CPLUS_PUBLIC;
		}
	      ;
    break;}
case 235:
#line 2633 "parser.y"
{
	       if (allow) {
		 char *iname;
		 init_language();
		 DataType::new_scope();

		 sprintf(temp_name,"CPP_CLASS:%s\n",yyvsp[-2].id);
		 if (add_symbol(temp_name, (DataType *) 0, (char *) 0)) {
		   fprintf(stderr,"%s : Line %d. Error. %s %s is multiply defined.\n", input_file, line_number, yyvsp[-3].id, yyvsp[-2].id);
		   FatalError();
		 }
		 if ((!CPlusPlus) && (strcmp(yyvsp[-3].id,"class") == 0))
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);
		 
		 iname = make_name(yyvsp[-2].id);
		 doc_entry = new DocClass(iname, doc_parent());
		 if (yyvsp[-2].id == iname) 
		   cplus_open_class(yyvsp[-2].id, 0, yyvsp[-3].id);
		 else
		   cplus_open_class(yyvsp[-2].id, iname, yyvsp[-3].id);
		 if (strcmp(yyvsp[-3].id,"class") == 0)
		   cplus_mode = CPLUS_PRIVATE;
		 else
		   cplus_mode = CPLUS_PUBLIC;
		 // Create a documentation entry for the class
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
		 nested_list = 0;

		 // Merge in scope from base classes
		 cplus_inherit_scope(yyvsp[-1].ilist.count,yyvsp[-1].ilist.names);

	       }
              ;
    break;}
case 236:
#line 2667 "parser.y"
{
		if (allow) {
		  if (yyvsp[-5].ilist.names) {
		    if (strcmp(yyvsp[-7].id,"union") != 0)
		      cplus_inherit(yyvsp[-5].ilist.count, yyvsp[-5].ilist.names);
		    else {
		      fprintf(stderr,"%s : Line %d.  Inheritance not allowed for unions.\n",input_file, line_number);
		      FatalError();
		    }
		  }
		  // Create a datatype for correctly processing the typedef
		  Active_typedef = new DataType();
		  Active_typedef->type = T_USER;
		  sprintf(Active_typedef->name,"%s %s", yyvsp[-7].id,yyvsp[-6].id);
		  Active_typedef->is_pointer = 0;
		  Active_typedef->implicit_ptr = 0;

		  // Clean up the inheritance list
		  if (yyvsp[-5].ilist.names) {
		    int j;
		    for (j = 0; j < yyvsp[-5].ilist.count; j++) {
		      if (yyvsp[-5].ilist.names[j]) delete [] yyvsp[-5].ilist.names[j];
		    }
		    delete [] yyvsp[-5].ilist.names;
		  }

		  if (yyvsp[0].decl.is_pointer > 0) {
		    fprintf(stderr,"%s : Line %d.  typedef struct { } *id not supported properly. Winging it...\n", input_file, line_number);

		  }
		  // Create dump nested class code
		  if (yyvsp[0].decl.is_pointer > 0) {
		    dump_nested(yyvsp[-6].id);
		  } else {
		    dump_nested(yyvsp[0].decl.id);
		  }
		    
		  // Collapse any datatypes created in the the class

		  cplus_register_scope(DataType::collapse_scope(yyvsp[-6].id));

		  doc_entry = doc_stack[doc_stack_top];
		  if (yyvsp[0].decl.is_pointer > 0) {
		    cplus_class_close(yyvsp[-6].id);
		  } else {
		    cplus_class_close(yyvsp[0].decl.id); 
		  }
		  doc_stack_top--;
		  doc_entry = 0;

		  // Create a typedef in global scope

		  if (yyvsp[0].decl.is_pointer == 0)
		    Active_typedef->typedef_add(yyvsp[0].decl.id);
		  else {
		    DataType *t = new DataType(Active_typedef);
		    t->is_pointer += yyvsp[0].decl.is_pointer;
		    t->typedef_add(yyvsp[0].decl.id);
		    cplus_register_type(yyvsp[0].decl.id);
		    delete t;
		  }
		  cplus_mode = CPLUS_PUBLIC;
		}
	      ;
    break;}
case 237:
#line 2730 "parser.y"
{ ;
    break;}
case 238:
#line 2734 "parser.y"
{
	       char *iname;
	       if (allow) {
		 init_language();
		 DataType::new_scope();
		 if ((!CPlusPlus) && (strcmp(yyvsp[-1].id,"class") == 0))
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);
		 
		 iname = make_name("");
		 doc_entry = new DocClass(iname,doc_parent());
		 if (strlen(iname))
		   cplus_open_class("", iname, yyvsp[-1].id);
		 else
		   cplus_open_class("",0,yyvsp[-1].id);
		 if (strcmp(yyvsp[-1].id,"class") == 0)
		   cplus_mode = CPLUS_PRIVATE;
		 else
		   cplus_mode = CPLUS_PUBLIC;
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
		 nested_list = 0;
	       }
              ;
    break;}
case 239:
#line 2757 "parser.y"
{
		if (allow) {
		  if (yyvsp[0].decl.is_pointer > 0) {
		    fprintf(stderr,"%s : Line %d. typedef %s {} *%s not supported correctly. Will be ignored.\n", input_file, line_number, yyvsp[-5].id, yyvsp[0].decl.id);
		    cplus_abort();
		  } else {
		    sprintf(temp_name,"CPP_CLASS:%s\n",yyvsp[0].decl.id);
		    if (add_symbol(temp_name, (DataType *) 0, (char *) 0)) {
		      fprintf(stderr,"%s : Line %d. Error. %s %s is multiply defined.\n", input_file, line_number, yyvsp[-5].id, yyvsp[0].decl.id);
		      FatalError();
		    }
		  }
		  // Create a datatype for correctly processing the typedef
		  Active_typedef = new DataType();
		  Active_typedef->type = T_USER;
		  sprintf(Active_typedef->name,"%s",yyvsp[0].decl.id);
		  Active_typedef->is_pointer = 0;
		  Active_typedef->implicit_ptr = 0;
		  
		  // Dump nested classes
		  if (yyvsp[0].decl.is_pointer == 0)  
		    dump_nested(yyvsp[0].decl.id);

		  // Go back to previous scope

		  cplus_register_scope(DataType::collapse_scope((char *) 0));
		  
		  doc_entry = doc_stack[doc_stack_top];
		  // Change name of doc_entry
		  doc_entry->name = copy_string(yyvsp[0].decl.id);
		  if (yyvsp[0].decl.is_pointer == 0) 
		    cplus_class_close(yyvsp[0].decl.id); 
		  doc_entry = 0;
		  doc_stack_top--;
		  cplus_mode = CPLUS_PUBLIC;
		}
	      ;
    break;}
case 240:
#line 2793 "parser.y"
{ ;
    break;}
case 241:
#line 2798 "parser.y"
{
	       char *iname;
		 if (allow) {
		   init_language();
		   iname = make_name(yyvsp[-1].id);
		   lang->cpp_class_decl(yyvsp[-1].id,iname,yyvsp[-2].id);
		 }
	     ;
    break;}
case 242:
#line 2809 "parser.y"
{
	       if (allow) {
		 init_language();
		 if (!CPlusPlus) 
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);
		 
		 yyvsp[-7].type->is_pointer += yyvsp[-6].decl.is_pointer;
		 yyvsp[-7].type->is_reference = yyvsp[-6].decl.is_reference;
		 // Fix up the function name
		 sprintf(temp_name,"%s::%s",yyvsp[-6].decl.id,yyvsp[-4].id);
		 if (!Rename_true) {
		   Rename_true = 1;
		   sprintf(yy_rename,"%s_%s",yyvsp[-6].decl.id,yyvsp[-4].id);
		 }
		 create_function(yyvsp[-8].ivalue, temp_name, yyvsp[-7].type, yyvsp[-2].pl);
	       }
	       delete yyvsp[-7].type;
	       delete yyvsp[-2].pl;
	      ;
    break;}
case 243:
#line 2830 "parser.y"
{
	       if (allow) {
		 init_language();
		 if (!CPlusPlus) 
		   fprintf(stderr,"%s : Line %d. *** WARNING ***. C++ mode is disabled (enable using -c++)\n", input_file, line_number);

		 yyvsp[-4].type->is_pointer += yyvsp[-3].decl.is_pointer;
		 // Fix up the function name
		 sprintf(temp_name,"%s::%s",yyvsp[-3].decl.id,yyvsp[-1].id);
		 if (!Rename_true) {
		   Rename_true = 1;
		   sprintf(yy_rename,"%s_%s",yyvsp[-3].decl.id,yyvsp[-1].id);
		 }
		 create_variable(yyvsp[-5].ivalue,temp_name, yyvsp[-4].type);
	       }
	       delete yyvsp[-4].type;
	     ;
    break;}
case 244:
#line 2850 "parser.y"
{
	       fprintf(stderr,"%s : Line %d. Operator overloading not supported (ignored).\n", input_file, line_number);
		skip_decl();
		delete yyvsp[-3].type;
	     ;
    break;}
case 245:
#line 2858 "parser.y"
{
	       fprintf(stderr,"%s : Line %d. Templates not currently supported (ignored).\n",
		       input_file, line_number);
	       skip_decl();
	     ;
    break;}
case 246:
#line 2866 "parser.y"
{
	       cplus_mode = CPLUS_PUBLIC;
               doc_entry = cplus_set_class(yyvsp[-1].id);
	       if (!doc_entry) {
		 doc_entry = new DocClass(yyvsp[-1].id,doc_parent());
	       };
	       doc_stack_top++;
	       doc_stack[doc_stack_top] = doc_entry;
	       scanner_clear_start();
	       AddMethods = 1;
	     ;
    break;}
case 247:
#line 2876 "parser.y"
{
	       cplus_unset_class();
	       doc_entry = 0;
	       doc_stack_top--;
	       AddMethods = 0;
	     ;
    break;}
case 248:
#line 2884 "parser.y"
{ ;
    break;}
case 249:
#line 2885 "parser.y"
{ ;
    break;}
case 250:
#line 2886 "parser.y"
{ ;
    break;}
case 251:
#line 2889 "parser.y"
{;
    break;}
case 252:
#line 2890 "parser.y"
{
	           AddMethods = 1;
	     ;
    break;}
case 253:
#line 2892 "parser.y"
{
	           AddMethods = 0;
	     ;
    break;}
case 254:
#line 2894 "parser.y"
{ ;
    break;}
case 255:
#line 2895 "parser.y"
{
	       skip_decl();
		   {
		     static int last_error_line = -1;
		     if (last_error_line != line_number) {
		       fprintf(stderr,"%s : Line %d. Syntax error in input.\n", input_file, line_number);
		       FatalError();
		       last_error_line = line_number;
		     }
		   }
	     ;
    break;}
case 256:
#line 2905 "parser.y"
{ ;
    break;}
case 257:
#line 2906 "parser.y"
{ ;
    break;}
case 258:
#line 2909 "parser.y"
{
                char *iname;
                if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		    yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping member function : %s\n",yyvsp[-4].decl.id);
		    }
		    iname = make_name(yyvsp[-4].decl.id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == yyvsp[-4].decl.id) iname = 0;
		    cplus_member_func(yyvsp[-4].decl.id, iname, yyvsp[-5].type,yyvsp[-2].pl,0);
		  }
		  scanner_clear_start();
		}
		delete yyvsp[-5].type;
		delete yyvsp[-2].pl;
              ;
    break;}
case 259:
#line 2933 "parser.y"
{
	       char *iname;
	       if (allow) {
		 init_language();
		 if (cplus_mode == CPLUS_PUBLIC) {
		   Stat_func++;
		   yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		   yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		   if (Verbose) {
		     fprintf(stderr,"Wrapping virtual member function : %s\n",yyvsp[-4].decl.id);
		   }
		   iname = make_name(yyvsp[-4].decl.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[-4].decl.id) iname = 0;
		   cplus_member_func(yyvsp[-4].decl.id,iname,yyvsp[-5].type,yyvsp[-2].pl,1);
		 }
		 scanner_clear_start();
	       }
	       delete yyvsp[-5].type;
	       delete yyvsp[-2].pl;
	     ;
    break;}
case 260:
#line 2956 "parser.y"
{
		char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping C++ constructor %s\n", yyvsp[-4].id);
		    }
		    iname = make_name(yyvsp[-4].id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == yyvsp[-4].id) iname = 0;
		    cplus_constructor(yyvsp[-4].id,iname, yyvsp[-2].pl);
		  }
		  scanner_clear_start();
		}
		delete yyvsp[-2].pl;
	      ;
    break;}
case 261:
#line 2977 "parser.y"
{
		char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping C++ destructor %s\n", yyvsp[-4].id);
		    }
		    iname = make_name(yyvsp[-4].id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == yyvsp[-4].id) iname = 0;
		    cplus_destructor(yyvsp[-4].id,iname);
		  }
		}
		scanner_clear_start();
	      ;
    break;}
case 262:
#line 2997 "parser.y"
{
 	        char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    Stat_func++;
		    if (Verbose) {
		      fprintf(stderr,"Wrapping C++ destructor %s\n", yyvsp[-3].id);
		    }
		    iname = make_name(yyvsp[-3].id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == yyvsp[-3].id) iname = 0;
		    cplus_destructor(yyvsp[-3].id,iname);
		  }
		}
		scanner_clear_start();
	      ;
    break;}
case 263:
#line 3017 "parser.y"
{
		if (allow) {
		  char *iname;
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    if (Active_type) delete Active_type;
		    Active_type = new DataType(yyvsp[-2].type);
		    yyvsp[-2].type->is_pointer += yyvsp[-1].decl.is_pointer;
		    yyvsp[-2].type->is_reference = yyvsp[-1].decl.is_reference;
		    if (yyvsp[-2].type->qualifier) {
		      if ((strcmp(yyvsp[-2].type->qualifier,"const") == 0) && (yyvsp[-2].type->is_pointer == 0)) {
			// Okay.  This is really some sort of C++ constant here.
	  	          if (yyvsp[0].dtype.type != T_ERROR) {
			    iname = make_name(yyvsp[-1].decl.id);
			    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
			    if (iname == yyvsp[-1].decl.id) iname = 0;
			    cplus_declare_const(yyvsp[-1].decl.id,iname, yyvsp[-2].type, yyvsp[0].dtype.id);
			  }
		      } else {
			int oldstatus = Status;
			char *tm;
			if (yyvsp[-2].type->status & STAT_READONLY) {
			  if (!(tm = typemap_lookup("memberin",typemap_lang,yyvsp[-2].type,yyvsp[-1].decl.id,"",""))) 
			    Status = Status | STAT_READONLY;
			}
			iname = make_name(yyvsp[-1].decl.id);
			doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
			if (iname == yyvsp[-1].decl.id) iname = 0;
			cplus_variable(yyvsp[-1].decl.id,iname,yyvsp[-2].type);
			Status = oldstatus;
		      }
		    } else {
		      char *tm = 0;
		      int oldstatus = Status;
		      if (yyvsp[-2].type->status & STAT_READONLY) {
			if (!(tm = typemap_lookup("memberin",typemap_lang,yyvsp[-2].type,yyvsp[-1].decl.id,"",""))) 
			  Status = Status | STAT_READONLY;
		      }
		      iname = make_name(yyvsp[-1].decl.id);
		      doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		      if (iname == yyvsp[-1].decl.id) iname = 0;
		      cplus_variable(yyvsp[-1].decl.id,iname,yyvsp[-2].type);
		      Status = oldstatus;
		      if (Verbose) {
			fprintf(stderr,"Wrapping member data %s\n", yyvsp[-1].decl.id);
		      }
		    }
		  }
		  scanner_clear_start();
		}
		delete yyvsp[-2].type;
	      ;
    break;}
case 264:
#line 3068 "parser.y"
{ ;
    break;}
case 265:
#line 3070 "parser.y"
{
		char *iname;
		if (allow) {
		  int oldstatus = Status;
		  char *tm = 0;
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    if (Active_type) delete Active_type;
		    Active_type = new DataType(yyvsp[-3].type);
		    yyvsp[-3].type->is_pointer += yyvsp[-2].decl.is_pointer + 1;
		    yyvsp[-3].type->is_reference = yyvsp[-2].decl.is_reference;
		    yyvsp[-3].type->arraystr = copy_string(ArrayString);
		    if (!(tm = typemap_lookup("memberin",typemap_lang,yyvsp[-3].type,yyvsp[-2].decl.id,"",""))) 
		      Status = STAT_READONLY;

		    iname = make_name(yyvsp[-2].decl.id);
		    doc_entry = new DocDecl(iname, doc_stack[doc_stack_top]);
		    if (iname == yyvsp[-2].decl.id) iname = 0;
		    cplus_variable(yyvsp[-2].decl.id,iname,yyvsp[-3].type);
		    Status = oldstatus;
		    if (!tm)
		      fprintf(stderr,"%s : Line %d. Warning. Array member will be read-only.\n",input_file,line_number);
		  }
		scanner_clear_start();
		}
		delete yyvsp[-3].type;
	      ;
    break;}
case 266:
#line 3101 "parser.y"
{
		char *iname;
		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		    yyvsp[-1].type->is_pointer += yyvsp[0].decl.is_pointer;
		    iname = make_name(yyvsp[0].decl.id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == yyvsp[0].decl.id) iname = 0;
		    cplus_static_var(yyvsp[0].decl.id,iname,yyvsp[-1].type);
		    if (Active_type) delete Active_type;
		    Active_type = new DataType(yyvsp[-1].type);
		    if (Verbose) {
		      fprintf(stderr,"Wrapping static member data %s\n", yyvsp[0].decl.id);
		    }
		  }
		  scanner_clear_start();
		}
		delete yyvsp[-1].type;
	      ;
    break;}
case 267:
#line 3120 "parser.y"
{ ;
    break;}
case 268:
#line 3124 "parser.y"
{
		char *iname;
		if (allow) {
		  yyvsp[-5].type->is_pointer += yyvsp[-4].decl.is_pointer;
		  yyvsp[-5].type->is_reference = yyvsp[-4].decl.is_reference;
		  if (cplus_mode == CPLUS_PUBLIC) {
		    iname = make_name(yyvsp[-4].decl.id);
		    doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		    if (iname == yyvsp[-4].decl.id) iname = 0;
		    cplus_static_func(yyvsp[-4].decl.id, iname, yyvsp[-5].type, yyvsp[-2].pl);
		    if (Verbose)
		      fprintf(stderr,"Wrapping static member function %s\n",yyvsp[-4].decl.id);
		  }
		  scanner_clear_start();
		}
		delete yyvsp[-5].type;
		delete yyvsp[-2].pl;
	      ;
    break;}
case 269:
#line 3144 "parser.y"
{
		if (allow) {
		  cplus_mode = CPLUS_PUBLIC;
		  if (Verbose)
		    fprintf(stderr,"Public mode\n");
		  scanner_clear_start();
		}
	      ;
    break;}
case 270:
#line 3155 "parser.y"
{
		if (allow) {
		  cplus_mode = CPLUS_PRIVATE;
		  if (Verbose)
		    fprintf(stderr,"Private mode\n");
		  scanner_clear_start();
		}
	      ;
    break;}
case 271:
#line 3166 "parser.y"
{
		if (allow) {
		  cplus_mode = CPLUS_PROTECTED;
		  if (Verbose)
		    fprintf(stderr,"Protected mode\n");
		  scanner_clear_start();
		}
	      ;
    break;}
case 272:
#line 3177 "parser.y"
{
	       if (allow) {
		 strcpy(yy_rename,yyvsp[-1].id);
		 Rename_true = 1;
	       }
	     ;
    break;}
case 273:
#line 3185 "parser.y"
{
                 NewObject = 1;
             ;
    break;}
case 274:
#line 3187 "parser.y"
{
                 NewObject = 0;
             ;
    break;}
case 275:
#line 3192 "parser.y"
{scanner_clear_start();;
    break;}
case 276:
#line 3192 "parser.y"
{

		 // if ename was supplied.  Install it as a new integer datatype.

		if (allow) {
		  init_language();
		  if (cplus_mode == CPLUS_PUBLIC) {
		   if (yyvsp[-5].id) {
		     cplus_register_type(yyvsp[-5].id);
		     temp_type.type = T_INT;
		     temp_type.is_pointer = 0;
		     temp_type.implicit_ptr = 0;
		     sprintf(temp_type.name,"int");
		     temp_type.typedef_add(yyvsp[-5].id,1); 
		   }
		 }
	       }
	      ;
    break;}
case 277:
#line 3210 "parser.y"
{
		if (allow)
		  Status = Status | STAT_READONLY;
		scanner_clear_start();
              ;
    break;}
case 278:
#line 3215 "parser.y"
{
		if (allow) 
		  Status = Status & ~(STAT_READONLY);
		scanner_clear_start();
	      ;
    break;}
case 279:
#line 3221 "parser.y"
{
		if (allow)
		  fprintf(stderr,"%s : Line %d. Friends are not allowed--members only! (ignored)\n", input_file, line_number);
		skip_decl();
		scanner_clear_start();
	      ;
    break;}
case 280:
#line 3229 "parser.y"
{
		if (allow)
		  fprintf(stderr,"%s : Line %d. Operator overloading not supported (ignored).\n", input_file, line_number);
		skip_decl();
		scanner_clear_start();
	      ;
    break;}
case 281:
#line 3235 "parser.y"
{ 
		scanner_clear_start();
	      ;
    break;}
case 282:
#line 3240 "parser.y"
{ ;
    break;}
case 283:
#line 3244 "parser.y"
{
	      		scanner_clear_start();
	      ;
    break;}
case 284:
#line 3249 "parser.y"
{
                 if (allow && (!WrapExtern)) { }
    	       ;
    break;}
case 285:
#line 3252 "parser.y"
{
		 if (allow && (!WrapExtern))
                   cplus_add_pragma(yyvsp[-3].id,yyvsp[-1].id,yyvsp[0].id);
	       ;
    break;}
case 286:
#line 3275 "parser.y"
{ start_line = line_number; skip_brace(); 
	      ;
    break;}
case 287:
#line 3276 "parser.y"
{ 

		if (cplus_mode == CPLUS_PUBLIC) {
		  cplus_register_type(yyvsp[-4].id);
		  if (yyvsp[-1].decl.id) {
		    if (strcmp(yyvsp[-5].id,"class") == 0) {
		      fprintf(stderr,"%s : Line %d.  Warning. Nested classes not currently supported (ignored).\n", input_file, line_number);
		      /* Generate some code for a new class */
		    } else {
		      Nested *n = new Nested;
		      n->code << "typedef " << yyvsp[-5].id << " " 
			      << CCode.get() << " $classname_" << yyvsp[-1].decl.id << ";\n";
		      n->name = copy_string(yyvsp[-1].decl.id);
		      n->line = start_line;
		      n->type = new DataType;
		      n->type->type = T_USER;
		      n->type->is_pointer = yyvsp[-1].decl.is_pointer;
		      n->type->is_reference = yyvsp[-1].decl.is_reference;
		      n->next = 0;
		      add_nested(n);
		    }
		  }
		}
	      ;
    break;}
case 288:
#line 3301 "parser.y"
{ start_line = line_number; skip_brace();
              ;
    break;}
case 289:
#line 3302 "parser.y"
{ 
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (strcmp(yyvsp[-4].id,"class") == 0) {
		    fprintf(stderr,"%s : Line %d.  Warning. Nested classes not currently supported (ignored)\n", input_file, line_number);
		    /* Generate some code for a new class */
		  } else {
		    /* Generate some code for a new class */

		    Nested *n = new Nested;
		    n->code << "typedef " << yyvsp[-4].id << " " 
			    << CCode.get() << " $classname_" << yyvsp[-1].decl.id << ";\n";
		    n->name = copy_string(yyvsp[-1].decl.id);
		    n->line = start_line;
		    n->type = new DataType;
		    n->type->type = T_USER;
		    n->type->is_pointer = yyvsp[-1].decl.is_pointer;
		    n->type->is_reference = yyvsp[-1].decl.is_reference;
		    n->next = 0;
		    add_nested(n);

		  }
		}
	      ;
    break;}
case 290:
#line 3326 "parser.y"
{
  		    if (cplus_mode == CPLUS_PUBLIC) {
                       cplus_register_type(yyvsp[-1].id);
                    }
              ;
    break;}
case 291:
#line 3333 "parser.y"
{ 
                     skip_decl();
                     fprintf(stderr,"%s : Line %d. Function pointers not currently supported (ignored).\n", input_file, line_number);
		     
	      ;
    break;}
case 292:
#line 3338 "parser.y"
{
                     skip_decl();
                     fprintf(stderr,"%s : Line %d. Function pointers not currently supported (ignored).\n", input_file, line_number);
		     
	      ;
    break;}
case 293:
#line 3343 "parser.y"
{ 
                     skip_decl();
                     fprintf(stderr,"%s : Line %d. Function pointers not currently supported (ignored).\n", input_file, line_number);
		     
	      ;
    break;}
case 294:
#line 3348 "parser.y"
{ ;
    break;}
case 295:
#line 3349 "parser.y"
{ ;
    break;}
case 296:
#line 3352 "parser.y"
{ yyval.decl = yyvsp[0].decl;;
    break;}
case 297:
#line 3353 "parser.y"
{ yyval.decl.id = 0; ;
    break;}
case 298:
#line 3356 "parser.y"
{;
    break;}
case 299:
#line 3357 "parser.y"
{;
    break;}
case 300:
#line 3358 "parser.y"
{;
    break;}
case 301:
#line 3361 "parser.y"
{ ;
    break;}
case 302:
#line 3362 "parser.y"
{
		 if (allow) {
		   int oldstatus = Status;
		   char *tm;

		   init_language();
		   if (cplus_mode == CPLUS_PUBLIC) {
		     temp_typeptr = new DataType(Active_type);
		     temp_typeptr->is_pointer += yyvsp[-1].decl.is_pointer;
		     if (Verbose) {
		       fprintf(stderr,"Wrapping member variable : %s\n",yyvsp[-1].decl.id);
		     }
		     Stat_var++;
		     doc_entry = new DocDecl(yyvsp[-1].decl.id,doc_stack[doc_stack_top]);
		     if (temp_typeptr->status & STAT_READONLY) {
		       if (!(tm = typemap_lookup("memberin",typemap_lang,temp_typeptr,yyvsp[-1].decl.id,"",""))) 
			 Status = Status | STAT_READONLY;
		     }
		     cplus_variable(yyvsp[-1].decl.id,(char *) 0,temp_typeptr);		
		     Status = oldstatus;
		     delete temp_typeptr;
		   }
		   scanner_clear_start();
		 }
	       ;
    break;}
case 303:
#line 3386 "parser.y"
{ ;
    break;}
case 304:
#line 3387 "parser.y"
{
		 if (allow) {
		   int oldstatus = Status;
		   char *tm;

		   init_language();
		   if (cplus_mode == CPLUS_PUBLIC) {
		     temp_typeptr = new DataType(Active_type);
		     temp_typeptr->is_pointer += yyvsp[-2].decl.is_pointer;
		     if (Verbose) {
		       fprintf(stderr,"Wrapping member variable : %s\n",yyvsp[-2].decl.id);
		     }
		     Stat_var++;
		     if (!(tm = typemap_lookup("memberin",typemap_lang,temp_typeptr,yyvsp[-2].decl.id,"",""))) 
		       Status = Status | STAT_READONLY;
		     doc_entry = new DocDecl(yyvsp[-2].decl.id,doc_stack[doc_stack_top]);
		     if (temp_typeptr->status & STAT_READONLY) Status = Status | STAT_READONLY;
		     cplus_variable(yyvsp[-2].decl.id,(char *) 0,temp_typeptr);		
		     Status = oldstatus;
		     if (!tm)
		       fprintf(stderr,"%s : Line %d. Warning. Array member will be read-only.\n",input_file,line_number);
		     delete temp_typeptr;
		   }
		   scanner_clear_start();
		 }
	       ;
    break;}
case 305:
#line 3412 "parser.y"
{ ;
    break;}
case 306:
#line 3415 "parser.y"
{ 
                    CCode = "";
               ;
    break;}
case 307:
#line 3418 "parser.y"
{ skip_brace(); ;
    break;}
case 308:
#line 3421 "parser.y"
{ CCode = ""; ;
    break;}
case 309:
#line 3422 "parser.y"
{ CCode = ""; ;
    break;}
case 310:
#line 3423 "parser.y"
{ skip_brace(); ;
    break;}
case 311:
#line 3426 "parser.y"
{;
    break;}
case 312:
#line 3427 "parser.y"
{;
    break;}
case 313:
#line 3430 "parser.y"
{
                    if (allow) {
		      if (cplus_mode == CPLUS_PUBLIC) {
			if (Verbose) {
			  fprintf(stderr,"Creating enum value %s\n", yyvsp[0].id);
			}
			Stat_const++;
			temp_typeptr = new DataType(T_INT);
			doc_entry = new DocDecl(yyvsp[0].id,doc_stack[doc_stack_top]);
			cplus_declare_const(yyvsp[0].id, (char *) 0, temp_typeptr, (char *) 0);
			delete temp_typeptr;
			scanner_clear_start();
		      }
		    }
                  ;
    break;}
case 314:
#line 3445 "parser.y"
{
		   if (allow) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       if (Verbose) {
			 fprintf(stderr, "Creating enum value %s = %s\n", yyvsp[-2].id, yyvsp[0].dtype.id);
		       }
		       Stat_const++;
		       temp_typeptr = new DataType(T_INT);
		       doc_entry = new DocDecl(yyvsp[-2].id,doc_stack[doc_stack_top]);
		       cplus_declare_const(yyvsp[-2].id,(char *) 0, temp_typeptr,(char *) 0);
// OLD : Bug with value     cplus_declare_const($1,(char *) 0, temp_typeptr,$3.id);
		       delete temp_typeptr;
		       scanner_clear_start();
		     }
		   }
		 ;
    break;}
case 315:
#line 3461 "parser.y"
{
		   if (allow) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       if (Verbose) {
			 fprintf(stderr,"Creating enum value %s\n", yyvsp[0].id);
		       }
		       Stat_const++;
		       temp_typeptr = new DataType(T_INT);
		       doc_entry = new DocDecl(yyvsp[-2].id,doc_stack[doc_stack_top]);
		       cplus_declare_const(yyvsp[0].id, yyvsp[-2].id, temp_typeptr, (char *) 0);
		       delete temp_typeptr;
		       scanner_clear_start();
		     }
		   }
		 ;
    break;}
case 316:
#line 3476 "parser.y"
{
		   if (allow) {
		     if (cplus_mode == CPLUS_PUBLIC) {
		       if (Verbose) {
			 fprintf(stderr, "Creating enum value %s = %s\n", yyvsp[-2].id, yyvsp[0].dtype.id);
		       }
		       Stat_const++;
		       temp_typeptr = new DataType(T_INT);
		       doc_entry = new DocDecl(yyvsp[-4].id,doc_stack[doc_stack_top]);
		       cplus_declare_const(yyvsp[-2].id,yyvsp[-4].id, temp_typeptr, (char *) 0);
// Old : bug with value	       cplus_declare_const($5,$3, temp_typeptr,$7.id);
		       delete temp_typeptr;
		       scanner_clear_start();
		     }
		   }
		 ;
    break;}
case 317:
#line 3492 "parser.y"
{ ;
    break;}
case 318:
#line 3493 "parser.y"
{ ;
    break;}
case 319:
#line 3496 "parser.y"
{
		   yyval.ilist = yyvsp[0].ilist;
                ;
    break;}
case 320:
#line 3499 "parser.y"
{
                   yyval.ilist.names = (char **) 0;
		   yyval.ilist.count = 0;
                ;
    break;}
case 321:
#line 3505 "parser.y"
{ 
                   int i;
                   yyval.ilist.names = new char *[NI_NAMES];
		   yyval.ilist.count = 0;
		   for (i = 0; i < NI_NAMES; i++){
		     yyval.ilist.names[i] = (char *) 0;
		   }
                   if (yyvsp[0].id) {
                       yyval.ilist.names[yyval.ilist.count] = copy_string(yyvsp[0].id);
                       yyval.ilist.count++;
		   }
               ;
    break;}
case 322:
#line 3518 "parser.y"
{ 
                   yyval.ilist = yyvsp[-2].ilist;
                   if (yyvsp[0].id) {
		     yyval.ilist.names[yyval.ilist.count] = copy_string(yyvsp[0].id);
		     yyval.ilist.count++;
		   }
               ;
    break;}
case 323:
#line 3527 "parser.y"
{     
                  fprintf(stderr,"%s : Line %d. No access specifier given for base class %s (ignored).\n",
			  input_file,line_number,yyvsp[0].id);
		  yyval.id = (char *) 0;
               ;
    break;}
case 324:
#line 3532 "parser.y"
{ 
                  fprintf(stderr,"%s : Line %d. No access specifier given for base class %s (ignored).\n",
			  input_file,line_number,yyvsp[0].id);
		  yyval.id = (char *) 0;
	       ;
    break;}
case 325:
#line 3537 "parser.y"
{
		 if (strcmp(yyvsp[-1].id,"public") == 0) {
		   yyval.id = yyvsp[0].id;
		 } else {
		   fprintf(stderr,"%s : Line %d. %s inheritance not supported (ignored).\n",
			   input_file,line_number,yyvsp[-1].id);
		   yyval.id = (char *) 0;
		 }
               ;
    break;}
case 326:
#line 3546 "parser.y"
{
		 if (strcmp(yyvsp[-1].id,"public") == 0) {
		   yyval.id = yyvsp[0].id;
		 } else {
		   fprintf(stderr,"%s : Line %d. %s inheritance not supported (ignored).\n",
			   input_file,line_number,yyvsp[-1].id);
		   yyval.id = (char *) 0;
		 }
	       ;
    break;}
case 327:
#line 3555 "parser.y"
{
                 if (strcmp(yyvsp[-2].id,"public") == 0) {
		   yyval.id = yyvsp[0].id;
		 } else {
		   fprintf(stderr,"%s : Line %d. %s inheritance not supported (ignored).\n",
			   input_file,line_number,yyvsp[-2].id);
		   yyval.id = (char *) 0;
		 }
               ;
    break;}
case 328:
#line 3566 "parser.y"
{ yyval.id = "public"; ;
    break;}
case 329:
#line 3567 "parser.y"
{ yyval.id = "private"; ;
    break;}
case 330:
#line 3568 "parser.y"
{ yyval.id = "protected"; ;
    break;}
case 331:
#line 3572 "parser.y"
{ yyval.id = "class"; ;
    break;}
case 332:
#line 3573 "parser.y"
{ yyval.id = "struct"; ;
    break;}
case 333:
#line 3574 "parser.y"
{yyval.id = "union"; ;
    break;}
case 334:
#line 3577 "parser.y"
{;
    break;}
case 335:
#line 3578 "parser.y"
{ delete yyvsp[-1].pl;;
    break;}
case 336:
#line 3579 "parser.y"
{;
    break;}
case 337:
#line 3584 "parser.y"
{ 
                    CCode = "";
               ;
    break;}
case 338:
#line 3587 "parser.y"
{ skip_brace(); ;
    break;}
case 339:
#line 3590 "parser.y"
{;
    break;}
case 340:
#line 3591 "parser.y"
{;
    break;}
case 341:
#line 3594 "parser.y"
{ ;
    break;}
case 342:
#line 3595 "parser.y"
{ ;
    break;}
case 343:
#line 3598 "parser.y"
{ ;
    break;}
case 344:
#line 3599 "parser.y"
{ ;
    break;}
case 345:
#line 3602 "parser.y"
{ ;
    break;}
case 346:
#line 3603 "parser.y"
{ ;
    break;}
case 347:
#line 3611 "parser.y"
{ 
                   ObjCClass = 1;
                   init_language();
		   cplus_mode = CPLUS_PROTECTED;
		   sprintf(temp_name,"CPP_CLASS:%s\n",yyvsp[-1].id);
		   if (add_symbol(temp_name,(DataType *) 0, (char *) 0)) {
		     fprintf(stderr,"%s : Line %d.  @interface %s is multiple defined.\n",
			     input_file,line_number,yyvsp[-1].id);
		     FatalError();
		   }
		   // Create a new documentation entry
		   doc_entry = new DocClass(yyvsp[-1].id,doc_parent());
		   doc_stack_top++;
		   doc_stack[doc_stack_top] = doc_entry;
		   scanner_clear_start();
		   cplus_open_class(yyvsp[-1].id, (char *) 0, "");     // Open up a new C++ class
                ;
    break;}
case 348:
#line 3627 "parser.y"
{ 
		  if (yyvsp[-6].id) {
		      char *inames[1];
		      inames[0] = yyvsp[-6].id;
		      cplus_inherit(1,inames);
		  }
		  // Restore original doc entry for this class
		  doc_entry = doc_stack[doc_stack_top];
		  cplus_class_close(yyvsp[-7].id);
		  doc_entry = 0;
		  doc_stack_top--;
		  cplus_mode = CPLUS_PUBLIC;
		  ObjCClass = 0;
		  delete yyvsp[-7].id;
		  delete yyvsp[-6].id;
                ;
    break;}
case 349:
#line 3644 "parser.y"
{
                 ObjCClass = 1;
		 init_language();
                 cplus_mode = CPLUS_PROTECTED;
                 doc_entry = cplus_set_class(yyvsp[-4].id);
		 if (!doc_entry) {
		   doc_entry = new DocClass(yyvsp[-4].id,doc_parent());
		 }
		 doc_stack_top++;
		 doc_stack[doc_stack_top] = doc_entry;
		 scanner_clear_start();
	       ;
    break;}
case 350:
#line 3655 "parser.y"
{
                 cplus_unset_class();
                 doc_entry = 0;
                 doc_stack_top--;
               ;
    break;}
case 351:
#line 3660 "parser.y"
{ skip_to_end(); ;
    break;}
case 352:
#line 3661 "parser.y"
{ skip_to_end(); ;
    break;}
case 353:
#line 3662 "parser.y"
{
		 char *iname = make_name(yyvsp[-2].id);
                 init_language();
                 lang->cpp_class_decl(yyvsp[-2].id,iname,"");
		 for (int i = 0; i <yyvsp[-1].ilist.count; i++) {
		   if (yyvsp[-1].ilist.names[i]) {
		     iname = make_name(yyvsp[-1].ilist.names[i]);
		     lang->cpp_class_decl(yyvsp[-1].ilist.names[i],iname,"");
		     delete [] yyvsp[-1].ilist.names[i];
		   }
		 } 
		 delete [] yyvsp[-1].ilist.names;
	       ;
    break;}
case 354:
#line 3677 "parser.y"
{ yyval.id = yyvsp[-1].id;;
    break;}
case 355:
#line 3678 "parser.y"
{ yyval.id = 0; ;
    break;}
case 356:
#line 3682 "parser.y"
{ skip_template(); 
                   CCode.strip();           // Strip whitespace
		   CCode.replace("<","< ");
		   CCode.replace(">"," >");
                   yyval.id = CCode.get();
                 ;
    break;}
case 357:
#line 3688 "parser.y"
{
                   yyval.id = "";
               ;
    break;}
case 358:
#line 3693 "parser.y"
{ ;
    break;}
case 359:
#line 3694 "parser.y"
{ 
                    cplus_mode = CPLUS_PUBLIC;
                 ;
    break;}
case 360:
#line 3696 "parser.y"
{ ;
    break;}
case 361:
#line 3697 "parser.y"
{
                    cplus_mode = CPLUS_PRIVATE;
                 ;
    break;}
case 362:
#line 3699 "parser.y"
{ ;
    break;}
case 363:
#line 3700 "parser.y"
{ 
                    cplus_mode = CPLUS_PROTECTED;
                 ;
    break;}
case 364:
#line 3702 "parser.y"
{ ;
    break;}
case 365:
#line 3703 "parser.y"
{
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
	       ;
    break;}
case 366:
#line 3716 "parser.y"
{ ;
    break;}
case 367:
#line 3717 "parser.y"
{ ;
    break;}
case 368:
#line 3720 "parser.y"
{
  
                ;
    break;}
case 369:
#line 3727 "parser.y"
{ 
                 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm;
		   char *iname;
		   if (Active_type) delete Active_type;
		   Active_type = new DataType(yyvsp[-1].type);
		   yyvsp[-1].type->is_pointer += yyvsp[0].decl.is_pointer;
		   yyvsp[-1].type->is_reference = yyvsp[0].decl.is_reference;
		   if (yyvsp[-1].type->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,yyvsp[-1].type,yyvsp[0].decl.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name(yyvsp[0].decl.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[0].decl.id) iname = 0;
		   cplus_variable(yyvsp[0].decl.id,iname,yyvsp[-1].type);
		   Status = oldstatus; 
		 }
		 scanner_clear_start();
		 delete yyvsp[-1].type;
               ;
    break;}
case 370:
#line 3749 "parser.y"
{ 
		 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm, *iname;
		   if (Active_type) delete Active_type;
		   Active_type = new DataType(yyvsp[-2].type);
		   yyvsp[-2].type->is_pointer += yyvsp[-1].decl.is_pointer;
		   yyvsp[-2].type->is_reference = yyvsp[-1].decl.is_reference;
		   yyvsp[-2].type->arraystr = copy_string(ArrayString);
		   if (yyvsp[-2].type->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,yyvsp[-2].type,yyvsp[-1].decl.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name(yyvsp[-1].decl.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[-1].decl.id) iname = 0;
		   cplus_variable(yyvsp[-1].decl.id,iname,yyvsp[-2].type);
		   Status = oldstatus; 
		 }
		 scanner_clear_start();
		 delete yyvsp[-2].type;
	       ;
    break;}
case 371:
#line 3771 "parser.y"
{
                    strcpy(yy_rename,yyvsp[-1].id);
                    Rename_true = 1;
	       ;
    break;}
case 372:
#line 3774 "parser.y"
{ ;
    break;}
case 373:
#line 3776 "parser.y"
{ 
                 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm, *iname;
		   DataType *t = new DataType (Active_type);
		   t->is_pointer += yyvsp[-1].decl.is_pointer;
		   t->is_reference = yyvsp[-1].decl.is_reference;
		   if (t->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,t,yyvsp[-1].decl.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name(yyvsp[-1].decl.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[-1].decl.id) iname = 0;
		   cplus_variable(yyvsp[-1].decl.id,iname,t);
		   Status = oldstatus; 
		   delete t;
		 }
		 scanner_clear_start();
               ;
    break;}
case 374:
#line 3796 "parser.y"
{
		 char *iname;
                 if (cplus_mode == CPLUS_PUBLIC) {
		   int oldstatus = Status;
		   char *tm;
		   DataType *t = new DataType (Active_type);
		   t->is_pointer += yyvsp[-2].decl.is_pointer;
		   t->is_reference = yyvsp[-2].decl.is_reference;
		   t->arraystr = copy_string(ArrayString);
		   if (t->status & STAT_READONLY) {
		     if (!(tm = typemap_lookup("memberin",typemap_lang,t,yyvsp[-2].decl.id,"",""))) 
		       Status = Status | STAT_READONLY;
		   }
		   iname = make_name(yyvsp[-2].decl.id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[-2].decl.id) iname = 0;
		   cplus_variable(yyvsp[-2].decl.id,iname,t);
		   Status = oldstatus; 
		   delete t;
		 }
		 scanner_clear_start();
               ;
    break;}
case 375:
#line 3818 "parser.y"
{ ;
    break;}
case 376:
#line 3821 "parser.y"
{ ;
    break;}
case 377:
#line 3822 "parser.y"
{
                   AddMethods = 1;
	       ;
    break;}
case 378:
#line 3824 "parser.y"
{
                   AddMethods = 0;
               ;
    break;}
case 379:
#line 3827 "parser.y"
{
                     strcpy(yy_rename,yyvsp[-1].id);
                     Rename_true = 1;
	       ;
    break;}
case 380:
#line 3830 "parser.y"
{ ;
    break;}
case 381:
#line 3831 "parser.y"
{
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
	       ;
    break;}
case 382:
#line 3844 "parser.y"
{ ;
    break;}
case 383:
#line 3845 "parser.y"
{ ;
    break;}
case 384:
#line 3848 "parser.y"
{
                 char *iname;
                 // An objective-C instance function
                 // This is like a C++ member function

		 if (strcmp(yyvsp[-2].id,objc_destruct) == 0) {
		   // This is an objective C destructor
                   doc_entry = new DocDecl(yyvsp[-2].id,doc_stack[doc_stack_top]);
                   cplus_destructor(yyvsp[-2].id,(char *) 0);
		 } else {
		   iname = make_name(yyvsp[-2].id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[-2].id) iname = 0;
		   cplus_member_func(yyvsp[-2].id,iname,yyvsp[-3].type,yyvsp[-1].pl,0);
		   scanner_clear_start();
		   delete yyvsp[-3].type;
		   delete yyvsp[-2].id;
		   delete yyvsp[-1].pl;
		 }
               ;
    break;}
case 385:
#line 3868 "parser.y"
{ 
                 char *iname;
                 // An objective-C class function
                 // This is like a c++ static member function
                 if (strcmp(yyvsp[-2].id,objc_construct) == 0) {
		   // This is an objective C constructor
		   doc_entry = new DocDecl(yyvsp[-2].id,doc_stack[doc_stack_top]);
                   cplus_constructor(yyvsp[-2].id,0,yyvsp[-1].pl);
		 } else {
		   iname = make_name(yyvsp[-2].id);
		   doc_entry = new DocDecl(iname,doc_stack[doc_stack_top]);
		   if (iname == yyvsp[-2].id) iname = 0;
		   cplus_static_func(yyvsp[-2].id,iname,yyvsp[-3].type,yyvsp[-1].pl);
		 }
                 scanner_clear_start();
                 delete yyvsp[-3].type;
                 delete yyvsp[-2].id;
                 delete yyvsp[-1].pl;
               ;
    break;}
case 386:
#line 3889 "parser.y"
{ CCode = ""; ;
    break;}
case 387:
#line 3890 "parser.y"
{ skip_brace(); ;
    break;}
case 388:
#line 3893 "parser.y"
{ 
                  yyval.type = yyvsp[-1].type;
                ;
    break;}
case 389:
#line 3896 "parser.y"
{ 
                  yyval.type = yyvsp[-2].type;
                  yyval.type->is_pointer += yyvsp[-1].ivalue;
               ;
    break;}
case 390:
#line 3900 "parser.y"
{       /* Empty type means "id" type */
                  yyval.type = new DataType(T_VOID);
		  sprintf(yyval.type->name,"id");
                  yyval.type->is_pointer = 1;
                  yyval.type->implicit_ptr = 1;
               ;
    break;}
case 391:
#line 3908 "parser.y"
{ 
                  yyval.type = new DataType(yyvsp[-1].p->t);
                  delete yyvsp[-1].p;
                 ;
    break;}
case 392:
#line 3912 "parser.y"
{ 
                  yyval.type = new DataType(T_VOID);
		  sprintf(yyval.type->name,"id");
                  yyval.type->is_pointer = 1;
                  yyval.type->implicit_ptr = 1;
               ;
    break;}
case 393:
#line 3920 "parser.y"
{ 
                   Parm *p= new Parm(yyvsp[-1].type,yyvsp[0].id);
		   p->objc_separator = yyvsp[-2].id;
                   yyval.pl = yyvsp[-3].pl;
                   yyval.pl->append(p);
               ;
    break;}
case 394:
#line 3926 "parser.y"
{ 
                 yyval.pl = new ParmList;
               ;
    break;}
case 395:
#line 3931 "parser.y"
{ yyval.id = copy_string(":"); ;
    break;}
case 396:
#line 3932 "parser.y"
{ yyval.id = new char[strlen(yyvsp[-1].id)+2]; 
                    strcpy(yyval.id,yyvsp[-1].id);
		    strcat(yyval.id,":");
		    delete yyvsp[-1].id;
	        ;
    break;}
case 397:
#line 3943 "parser.y"
{
                    yyval.dlist = yyvsp[0].dlist;
		    yyval.dlist.names[yyval.dlist.count] = copy_string(yyvsp[-2].id);
		    yyval.dlist.values[yyval.dlist.count] = copy_string(yyvsp[-1].id);
		    format_string(yyval.dlist.values[yyval.dlist.count]);
		    yyval.dlist.count++;
                 ;
    break;}
case 398:
#line 3953 "parser.y"
{
                    yyval.dlist = yyvsp[-3].dlist;
		    yyval.dlist.names[yyval.dlist.count] = copy_string(yyvsp[-1].id);
		    yyval.dlist.values[yyval.dlist.count] = copy_string(yyvsp[0].id);
		    format_string(yyval.dlist.values[yyval.dlist.count]);
		    yyval.dlist.count++;
                 ;
    break;}
case 399:
#line 3960 "parser.y"
{
                    yyval.dlist.names = new char *[NI_NAMES];
		    yyval.dlist.values = new char *[NI_NAMES];
		    yyval.dlist.count = 0;
	       ;
    break;}
case 400:
#line 3967 "parser.y"
{
                     yyval.id = yyvsp[0].id;
                 ;
    break;}
case 401:
#line 3970 "parser.y"
{
                     yyval.id = yyvsp[0].id;
	       ;
    break;}
case 402:
#line 3973 "parser.y"
{ 
                     yyval.id = 0;
                ;
    break;}
case 403:
#line 3983 "parser.y"
{
                 yyval.id = yyvsp[0].id;
               ;
    break;}
case 404:
#line 3986 "parser.y"
{
                 yyval.id = copy_string("const");
               ;
    break;}
case 405:
#line 3991 "parser.y"
{
                 yyval.tmparm = yyvsp[-1].tmparm;
                 yyval.tmparm->next = yyvsp[0].tmparm;
		;
    break;}
case 406:
#line 3997 "parser.y"
{
                 yyval.tmparm = yyvsp[-1].tmparm;
                 yyval.tmparm->next = yyvsp[0].tmparm;
                ;
    break;}
case 407:
#line 4001 "parser.y"
{ yyval.tmparm = 0;;
    break;}
case 408:
#line 4004 "parser.y"
{
		    if (InArray) {
		      yyvsp[-1].type->is_pointer++;
		      yyvsp[-1].type->arraystr = copy_string(ArrayString);
		    }
		    yyval.tmparm = new TMParm;
                    yyval.tmparm->p = new Parm(yyvsp[-1].type,yyvsp[0].id);
		    yyval.tmparm->p->call_type = 0;
		    yyval.tmparm->args = tm_parm;
		    delete yyvsp[-1].type;
		    delete yyvsp[0].id;
                 ;
    break;}
case 409:
#line 4017 "parser.y"
{
		  yyval.tmparm = new TMParm;
		   yyval.tmparm->p = new Parm(yyvsp[-2].type,yyvsp[0].id);
		   yyval.tmparm->p->t->is_pointer += yyvsp[-1].ivalue;
		   yyval.tmparm->p->call_type = 0;
		   if (InArray) {
		     yyval.tmparm->p->t->is_pointer++;
		     yyval.tmparm->p->t->arraystr = copy_string(ArrayString);
		    }
		   yyval.tmparm->args = tm_parm;
		   delete yyvsp[-2].type;
		   delete yyvsp[0].id;
		;
    break;}
case 410:
#line 4031 "parser.y"
{
                  yyval.tmparm = new TMParm;
		  yyval.tmparm->p = new Parm(yyvsp[-2].type,yyvsp[0].id);
		  yyval.tmparm->p->t->is_reference = 1;
		  yyval.tmparm->p->call_type = 0;
		  yyval.tmparm->p->t->is_pointer++;
		  if (!CPlusPlus) {
			fprintf(stderr,"%s : Line %d. Warning.  Use of C++ Reference detected.  Use the -c++ option.\n", input_file, line_number);
		  }
		  yyval.tmparm->args = tm_parm;
		  delete yyvsp[-2].type;
		  delete yyvsp[0].id;
		;
    break;}
case 411:
#line 4044 "parser.y"
{
                  fprintf(stderr,"%s : Line %d. Error. Function pointer not allowed (remap with typedef).\n", input_file, line_number);
		  FatalError();
                  yyval.tmparm = new TMParm;
		  yyval.tmparm->p = new Parm(yyvsp[-7].type,yyvsp[-4].id);
		  yyval.tmparm->p->t->type = T_ERROR;
		  yyval.tmparm->p->name = copy_string(yyvsp[-4].id);
		  strcpy(yyval.tmparm->p->t->name,"<function ptr>");
		  yyval.tmparm->args = tm_parm;
		  delete yyvsp[-7].type;
		  delete yyvsp[-4].id;
		  delete yyvsp[-1].pl;
		;
    break;}
case 412:
#line 4059 "parser.y"
{
                    yyval.id = yyvsp[-1].id; 
                    InArray = 0;
                ;
    break;}
case 413:
#line 4063 "parser.y"
{ 
                    ArrayBackup = "";
		    ArrayBackup << ArrayString;
                  ;
    break;}
case 414:
#line 4066 "parser.y"
{
                    yyval.id = yyvsp[-3].id;
                    InArray = yyvsp[-2].ivalue;
                    ArrayString = "";
		    ArrayString << ArrayBackup;
                ;
    break;}
case 415:
#line 4072 "parser.y"
{ 
                    ArrayBackup = "";
		    ArrayBackup << ArrayString;
		;
    break;}
case 416:
#line 4075 "parser.y"
{
		    yyval.id = new char[1];
		    yyval.id[0] = 0;
		    InArray = yyvsp[-2].ivalue;
                    ArrayString = "";
                    ArrayString << ArrayBackup;
		;
    break;}
case 417:
#line 4082 "parser.y"
{ yyval.id = new char[1];
  	                  yyval.id[0] = 0;
                          InArray = 0;
                ;
    break;}
case 418:
#line 4088 "parser.y"
{
                  tm_parm = yyvsp[-1].pl;
                ;
    break;}
case 419:
#line 4091 "parser.y"
{
                  tm_parm = 0;
                ;
    break;}
case 420:
#line 4096 "parser.y"
{yyval.id = yyvsp[0].id;;
    break;}
case 421:
#line 4097 "parser.y"
{ yyval.id = yyvsp[0].id;;
    break;}
case 422:
#line 4103 "parser.y"
{ ;
    break;}
case 423:
#line 4104 "parser.y"
{ ;
    break;}
case 424:
#line 4107 "parser.y"
{ ;
    break;}
case 425:
#line 4108 "parser.y"
{ ;
    break;}
case 426:
#line 4109 "parser.y"
{ ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 4143 "parser.y"


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

