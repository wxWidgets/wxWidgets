/****************************************************************************
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
 ****************************************************************************/

/**************************************************************************
 * class PERL5
 *
 * A Perl 5 implementation
 **************************************************************************/

class PERL5 : public Language {
private:
  int    export_all;
  char   *package;
  char   *module;
  String cmodule;
  String  vinit;
  FILE   *f_pm;
  String  pm;                      // Package initialization code
  String  magic;                   // Magic variable wrappers

  char   *perl_path;
  int    is_static;
  void   get_pointer(char *iname, char *srcname, char *src, char *dest,
			DataType *t, String &f, char *ret);
  char     *usage_var(char *, DataType *);
  char     *usage_func(char *, DataType *, ParmList *);
  char     *usage_const(char *, DataType *, char *);

  char   *alt_header;   

// The following variables are used to manage Perl5 classes

  int      blessed;                // Enable object oriented features
  Hash     classes;                // A hash table for storing the classes we've seen so far
  int      have_constructor;
  int      have_destructor;
  int      have_data_members;
  char     *class_name;            // Name of the class (what Perl thinks it is)
  char     *class_type;            // Type of class "struct", "class", "union"
  char     *real_classname;        // Real name of C/C++ class
  String   *base_class;            // Base class (if using inheritance)
  String   *pcode;                 // Perl code associated with each class
  String   *blessedmembers;        // Member data associated with each class
  int      member_func;            // Set to 1 when wrapping a member function
  char     *realpackage;           // Name of real module 
  String   func_stubs;             // Function stubs
  String   var_stubs;              // Variable stubs
  String   *member_keys;            // Keys for all member data
  String   exported;               // Exported symbols

public :
  PERL5() {
    package = 0;
    module = 0;
    perl_path = "perl5";
    is_static = 0;
    blessed = 0;
    alt_header = 0;
    member_func = 0;
  };
  void parse_args(int, char *argv[]);
  void parse();
  void create_function(char *, char *, DataType *, ParmList *);
  void link_variable(char *, char *, DataType *);
  void declare_const(char *, char *, DataType *, char *);
  void initialize(void);
  void headers(void);
  void close(void);
  void set_module(char *, char **);
  void set_init(char *);
  void add_native(char *, char *);
  void create_command(char *, char *);
  char *type_mangle(DataType *);

  // Support for blessed perl thingies....

  void cpp_open_class(char *classname, char *rename, char *ctype, int strip);
  void cpp_close_class();
  void cpp_member_func(char *name, char *iname, DataType *t, ParmList *l);
  void cpp_static_func(char *name, char *iname, DataType *t, ParmList *l);
  void cpp_variable(char *name, char *iname, DataType *t);
  void cpp_constructor(char *name, char *iname, ParmList *l);
  void cpp_destructor(char *name, char *newname);
  void cpp_inherit(char **baseclass, int mode = INHERIT_ALL);
  void cpp_declare_const(char *name, char *iname, DataType *type, char *value);
  void cpp_class_decl(char *, char *, char *);
  void add_typedef(DataType *t, char *name);
  void pragma(char *, char *, char *);
  void import(char *filename);
};



