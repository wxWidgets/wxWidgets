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
 * internals.h
 *
 * Contains global variables used in libswig, but which are otherwise
 * inaccessible to the user.
 *
 ***********************************************************************/

#include "swig.h"

// -------------------------------------------------------------------
// class DocTitle : public DocEntry
//
// Top level class for managing documentation.     Prints out a title,
// date, etc...
// -------------------------------------------------------------------

class DocTitle : public DocEntry {
public:
  DocTitle(char *title, DocEntry *_parent);  // Create a new title
  void output(Documentation *d);             // Output documentation
};

// --------------------------------------------------------------------
// class DocSection : public DocEntry
//
// Documentation entry for a section
// --------------------------------------------------------------------

class DocSection : public DocEntry {
public:
  DocSection(char *section, DocEntry *_parent);
  void output(Documentation *d);
};

// --------------------------------------------------------------------
// class DocFunction : public DocEntry
//
// Documentation entry for generic sorts of declarations
// --------------------------------------------------------------------

class DocDecl : public DocEntry {
public:
  DocDecl(char *fname, DocEntry *_parent);
  DocDecl(DocEntry *de, DocEntry *_parent);
  void output(Documentation *d);
};

// --------------------------------------------------------------------
// class DocClass : public DocEntry
//
// Documentation entry for a C++ class or C struct
// --------------------------------------------------------------------

class DocClass : public DocEntry {
public:
  DocClass(char *classname, DocEntry *_parent);
  void output(Documentation *d);
};

// --------------------------------------------------------------------
// class DocText : public DocEntry
//
// Documentation entry for some plain ole text.  Declared using
// the %text %{,%} directive.
// --------------------------------------------------------------------

class DocText : public DocEntry {
public:
  DocText(char *_text, DocEntry *_parent);
  void output(Documentation *d);
};

// --------------------------------------------------------------------
// class CommentHandler
//
// Class for managing comment processing.
// --------------------------------------------------------------------

class CommentHandler {
public:
  CommentHandler();
  CommentHandler(CommentHandler *c);
  ~CommentHandler();
  void        add_comment(char *text, int line_num, int col, char *file); // Add a comment
  void        set_entry(DocEntry *d);                            // Set documentation entry
  static void cleanup();                                         // Clean-up everything before quitting
  void        style(char *name, char *value);
  void        parse_args(int argc, char **argv);                 // Parse command line options

  // Comment handling style parameters
  int           skip_lines;                   // # blank lines before comment is throw away
  int           location;                     // Comment location (BEFORE or AFTER)
  int           chop_top;                     // Lines to chop from the top of a comment
  int           chop_bottom;                  // Lines to chop from the bottom
  int           chop_left;                    // Characters to chop from left
  int           chop_right;                   // Characters to chop from right
  int           untabify;                     // Expand tabs
  int           ignore;                       // Ignore comments
};

#define   BEFORE     0
#define   AFTER      1


extern    int           include_file(char *);         // Insert library file
extern    char          category[256];
extern    char          title[256];
extern    DocEntry      *doc_entry;
extern    DocEntry      *doctitle;               // The very first docentry
extern    DocEntry       *doc_stack[256];             // Stack of documentation entries
extern    CommentHandler *handler_stack[256];         // Stack of comment handlers
extern    int            doc_stack_top;           // Top of stack

extern    Language      *lang;
extern    Documentation *doc;
extern    CommentHandler *comment_handler;      // Comment handling system
extern    void          swig_append(char *, FILE *);
extern    int           Stat_func, Stat_var, Stat_const;
extern    int           IgnoreDoc;
extern    int           ForceExtern;
extern    int           WrapExtern;
extern    String        CCode;
extern    int           GenerateDefault;
extern    int           type_id;
extern    char         *ConfigFile;
extern    char         *objc_construct;
extern    char         *objc_destruct;
extern    int           DocOnly;

// Structure for holding typemap parameters
// A typemap parameter consists of a single parameter (type + name)
// and an optional list of arguments corresponding to local variables.
// Has an optional link for building linked lists of parameter lists

struct TMParm {
  Parm     *p;
  ParmList *args;
  TMParm   *next;
  TMParm() {
    next = 0;
  }
};

/* Global variables.   Needs to be cleaned up */

#ifdef WRAP

    FILE      *f_header;                        // Some commonly used
    FILE      *f_wrappers;                      // FILE pointers
    FILE      *f_init;
    FILE      *f_input;
    char      InitName[256];             
    char      LibDir[512];                      // Library directory
    char     **InitNames = 0;
    int       Status; 
    int       TypeStrict;                       // Type checking strictness
    int       Verbose;
    char       category[256];                   // Variables for documentation
    char       title[256];
    DocEntry  *doc_entry = 0;                   // Current documentation entry
    DocEntry  *doctitle = 0;                    // First doc entry
    DocEntry       *doc_stack[256];             // Stack of documentation entries
    CommentHandler *handler_stack[256];         // Stack of comment handlers
    int            doc_stack_top = 0;           // Top of stack

    Language  *lang;                            // Language method
    Documentation  *doc;                        // Documentation method
    int        Stat_func = 0;
    int        Stat_var = 0;
    int        Stat_const = 0;
    int        CPlusPlus = 0;
    int        ObjC = 0;
    int        ObjCClass = 0;
    int        AddMethods = 0;                  // AddMethods flag
    int        NewObject = 0;                   // NewObject flag
    int        Inline = 0;                      // Inline mode
    int        Stats = 0;
    int        IgnoreDoc = 0;                   // Ignore documentation mode
    int        ForceExtern = 0;                 // Force extern mode
    int        WrapExtern = 0;
    int        GenerateDefault = 0;            // Generate default constructors
    char      *Config = 0;
    int        NoInclude = 0;
    char      *typemap_lang = 0;                // Typemap name
    int        type_id = 0;                     // Type identifier
    int        error_count = 0;                 // Error count
    char      *ConfigFile = 0;
    int        DocOnly = 0;                     // Only produce documentation

#endif

/* Number of initialization names that can be used */

#define NI_NAMES    512

extern void type_undefined_check(void);

