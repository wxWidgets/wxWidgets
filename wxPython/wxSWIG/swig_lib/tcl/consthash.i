// consthash.i
//
// This module changes SWIG to place constant values into a Tcl
// hash table. 


#ifdef AUTODOC
%subsection "Hash Constants",pre
%text %{
%include consthash.i

This module changes SWIG so that constant values are placed into a Tcl
hash table in addition to normal Tcl variables.  When working with systems
involving large numbers of constants, the use of a hash table
simplifies use because it is no longer necessary to declare constants
using the 'global' statement.

This module should generally be included at the top of an interface
file before any declarations appear.   Furthermore, this module changes
the default handling of basic datatypes including integers, floats,
and character strings.

When this module is used, constants are simply accessed by name
without the associated dollar sign. For example :

     #define FOO 42

would be accessed as 'FOO' in Tcl, not '$FOO'.

Note : This module only affects integer, float, and character
constants.  Pointer constants are not currently affected.  This module
should not break existing Tcl scripts that rely on the normal SWIG
constant mechanism.
%}
#endif

%{
static Tcl_HashTable      intHash, doubleHash, charHash;
static Tcl_HashEntry      *entryPtr;
static int                init_dummy;
%}

%init %{
   Tcl_InitHashTable(&intHash, TCL_STRING_KEYS);
   Tcl_InitHashTable(&doubleHash, TCL_STRING_KEYS);
   Tcl_InitHashTable(&charHash, TCL_STRING_KEYS);
%}

%typemap(tcl,const) int SWIG_DEFAULT_TYPE, 
                    unsigned int SWIG_DEFAULT_TYPE,
                    long SWIG_DEFAULT_TYPE,
                    unsigned long SWIG_DEFAULT_TYPE,
                    short SWIG_DEFAULT_TYPE,
                    unsigned short SWIG_DEFAULT_TYPE,
                    unsigned char  SWIG_DEFAULT_TYPE,
                    signed char    SWIG_DEFAULT_TYPE
{
  static int ivalue = (int) $source;
  entryPtr = Tcl_CreateHashEntry(&intHash, "$target", &init_dummy);
  Tcl_SetHashValue(entryPtr, &ivalue);
  Tcl_LinkVar(interp, SWIG_prefix "$target",(char *) &ivalue, TCL_LINK_INT | TCL_LINK_READ_ONLY);
}

%typemap(tcl,const) float  SWIG_DEFAULT_TYPE, 
                    double SWIG_DEFAULT_TYPE
{
  static double dvalue = (double) $source;
  entryPtr = Tcl_CreateHashEntry(&doubleHash, "$target", &init_dummy);
  Tcl_SetHashValue(entryPtr, &dvalue);
  Tcl_LinkVar(interp, SWIG_prefix "$target",(char *) &dvalue, TCL_LINK_DOUBLE | TCL_LINK_READ_ONLY);
}

%typemap(tcl,const) char  *SWIG_DEFAULT_TYPE
{
  static char *cvalue = $source;
  entryPtr = Tcl_CreateHashEntry(&charHash, "$target", &init_dummy);
  Tcl_SetHashValue(entryPtr, &cvalue);
  Tcl_LinkVar(interp, SWIG_prefix "$target",(char *) &cvalue, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
}

// Change input handling to look for names

%typemap(tcl,in) int SWIG_DEFAULT_TYPE, 
                 unsigned int SWIG_DEFAULT_TYPE,
                 long SWIG_DEFAULT_TYPE,
                 unsigned long SWIG_DEFAULT_TYPE,
                 short SWIG_DEFAULT_TYPE,
                 unsigned short SWIG_DEFAULT_TYPE,
                 unsigned char  SWIG_DEFAULT_TYPE,
                 signed char    SWIG_DEFAULT_TYPE
{
   Tcl_HashEntry *entry;
   entry = Tcl_FindHashEntry(&intHash,$source);
   if (entry) {
     $target = ($type) (*((int *) Tcl_GetHashValue(entry)));
   } else {
     int temp;
     if (Tcl_GetInt(interp, $source, &temp) == TCL_ERROR) return TCL_ERROR;
     $target = ($type) temp;
   }
}

%typemap(tcl,in) float SWIG_DEFAULT_TYPE, 
                 double SWIG_DEFAULT_TYPE
{
   Tcl_HashEntry *entry;
   entry = Tcl_FindHashEntry(&doubleHash,$source);
   if (entry) {
     $target = ($type) (*((double *) Tcl_GetHashValue(entry)));
   } else if (entry = Tcl_FindHashEntry(&intHash,$source)) {
     $target = ($type) (*((int *) Tcl_GetHashValue(entry)));
   } else {
     double temp;
     if (Tcl_GetDouble(interp,$source,&temp) == TCL_ERROR) return TCL_ERROR;
     $target = ($type) temp;
   }
}

%typemap(tcl,in) char *SWIG_DEFAULT_TYPE 
{
   Tcl_HashEntry *entry;
   entry = Tcl_FindHashEntry(&charHash,$source);
   if (entry) {
     $target = ($type) (*((char **) Tcl_GetHashValue(entry)));
   } else {
     $target = $source;
   }
}

// ----------------------------------------------------------------------------------
// Tcl 8 Object versions
// ----------------------------------------------------------------------------------

%typemap(tcl8,const) int SWIG_DEFAULT_TYPE, 
                    unsigned int SWIG_DEFAULT_TYPE,
                    long SWIG_DEFAULT_TYPE,
                    unsigned long SWIG_DEFAULT_TYPE,
                    short SWIG_DEFAULT_TYPE,
                    unsigned short SWIG_DEFAULT_TYPE,
                    unsigned char  SWIG_DEFAULT_TYPE,
                    signed char    SWIG_DEFAULT_TYPE
{
  static int ivalue = (int) $source;
  entryPtr = Tcl_CreateHashEntry(&intHash, "$target", &init_dummy);
  Tcl_SetHashValue(entryPtr, &ivalue);
  Tcl_LinkVar(interp, SWIG_prefix "$target",(char *) &ivalue, TCL_LINK_INT | TCL_LINK_READ_ONLY);
}

%typemap(tcl8,const) float  SWIG_DEFAULT_TYPE, 
                    double SWIG_DEFAULT_TYPE
{
  static double dvalue = (double) $source;
  entryPtr = Tcl_CreateHashEntry(&doubleHash, "$target", &init_dummy);
  Tcl_SetHashValue(entryPtr, &dvalue);
  Tcl_LinkVar(interp, SWIG_prefix "$target",(char *) &dvalue, TCL_LINK_DOUBLE | TCL_LINK_READ_ONLY);
}

%typemap(tcl8,const) char  *SWIG_DEFAULT_TYPE
{
  static char *cvalue = $source;
  entryPtr = Tcl_CreateHashEntry(&charHash, "$target", &init_dummy);
  Tcl_SetHashValue(entryPtr, &cvalue);
  Tcl_LinkVar(interp, SWIG_prefix "$target",(char *) &cvalue, TCL_LINK_STRING | TCL_LINK_READ_ONLY);
}

// Change input handling to look for names

%typemap(tcl8,in) int SWIG_DEFAULT_TYPE, 
                 unsigned int SWIG_DEFAULT_TYPE,
                 long SWIG_DEFAULT_TYPE,
                 unsigned long SWIG_DEFAULT_TYPE,
                 short SWIG_DEFAULT_TYPE,
                 unsigned short SWIG_DEFAULT_TYPE,
                 unsigned char  SWIG_DEFAULT_TYPE,
                 signed char    SWIG_DEFAULT_TYPE
{
   Tcl_HashEntry *entry;
   int  _len;
   char *_str = Tcl_GetStringFromObj($source,&_len);
   entry = Tcl_FindHashEntry(&intHash,_str);
   if (entry) {
     $target = ($type) (*((int *) Tcl_GetHashValue(entry)));
   } else {
     int temp;
     if (Tcl_GetIntFromObj(interp, $source, &temp) == TCL_ERROR) return TCL_ERROR;
     $target = ($type) temp;
   }
}

%typemap(tcl8,in) float SWIG_DEFAULT_TYPE, 
                 double SWIG_DEFAULT_TYPE
{
   Tcl_HashEntry *entry;
   int  _len;
   char *_str = Tcl_GetStringFromObj($source,&_len);
   entry = Tcl_FindHashEntry(&doubleHash,_str);
   if (entry) {
     $target = ($type) (*((double *) Tcl_GetHashValue(entry)));
   } else if (entry = Tcl_FindHashEntry(&intHash,_str)) {
     $target = ($type) (*((int *) Tcl_GetHashValue(entry)));
   } else {
     double temp;
     if (Tcl_GetDoubleFromObj(interp,$source,&temp) == TCL_ERROR) return TCL_ERROR;
     $target = ($type) temp;
   }
}

%typemap(tcl8,in) char *SWIG_DEFAULT_TYPE 
{
   Tcl_HashEntry *entry;
   int  _len;
   char *_str = Tcl_GetStringFromObj($source,&_len);
   entry = Tcl_FindHashEntry(&charHash,_str);
   if (entry) {
     $target = ($type) (*((char **) Tcl_GetHashValue(entry)));
   } else {
     $target = _str;
   }
}



   
