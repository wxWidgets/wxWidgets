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
 * class TCL
 *
 * A Simple TCL implementation.
 **************************************************************************/

class TCL : public Language {
private:
  char   interp_name[256];
  char  *prefix;                   // Package prefix
  char  *module;                   // Name of the module
  char  *tcl_path;
  char  *init_name;
  int    Plugin;
  int    nspace;
  char  *safe_name;
  void   get_pointer(char *iname, char *srcname, char *src, char *dest, DataType *t,
		     String &f, char *ret);
  char  *char_result;
  char  *usage_string(char *, DataType *, ParmList *);
  char  *usage_func(char *, DataType *, ParmList *);
  char  *usage_var(char *, DataType *);
  char  *usage_const(char *, DataType *, char *);
  
  // C++ handling

  int    have_constructor;
  int    have_destructor;
  int    have_methods;
  int    have_config;
  int    have_cget;
  String config;
  String cget;
  String methods;
  String options;
  String config_options;
  String methodnames;
  String postinit;
  int    shadow;
  char   *class_name;
  char   *class_type;
  char   *real_classname;
  char   *base_class;
  Hash   hash;
  Hash   repeatcmd;

  // C++ Code generation strings

  String delcmd;
  String methodcmd;
  String objcmd;

public :
    TCL() {
    prefix = 0;
    module = 0;
    init_name = 0;
    nspace = 0;
    shadow = 1;
    char_result = "TCL_VOLATILE";
    tcl_path = "tcl";
    sprintf(interp_name,"interp");
    class_name = 0;
    class_type = 0;
    real_classname = 0;
    base_class = 0;
  };
  void parse_args(int, char *argv[]);
  void parse();
  void create_function(char *, char *, DataType *, ParmList *);
  void link_variable(char *, char *, DataType *);
  void declare_const(char *, char *, DataType *, char *);
  void initialize(void);
  void headers(void);
  void close(void);
  void set_module(char *,char **);
  void set_init(char *);
  void add_native(char *, char *);
  void pragma(char *,char *, char *);
  void create_command(char *, char *);

  // Stubs for processing C++ classes in Tcl

  void cpp_open_class(char *classname, char *rename, char *ctype, int strip);
  void cpp_close_class();
  void cpp_member_func(char *name, char *iname, DataType *t, ParmList *l);
  void cpp_variable(char *name, char *iname, DataType *t);
  void cpp_constructor(char *name, char *iname, ParmList *l);
  void cpp_destructor(char *name, char *newname);
  void cpp_inherit(char **baseclass, int mode = INHERIT_ALL);
  void cpp_declare_const(char *name, char *iname, DataType *type, char *value);
  void cpp_class_decl(char *, char *, char *);
  void add_typedef(DataType *, char *);
};






