//
// SWIG pointer conversion and utility library
// 
// Dave Beazley
// April 19, 1997
//
// Perl5 specific implementation.   This file is included
// by the file ../pointer.i

%{

#ifdef WIN32
#undef isspace
#define isspace(c) (c == ' ')
#endif

/*------------------------------------------------------------------
  ptrcast(value,type)

  Constructs a new pointer value.   Value may either be a string
  or an integer. Type is a string corresponding to either the
  C datatype or mangled datatype.

  ptrcast(0,"Vector *")
               or
  ptrcast(0,"Vector_p")   
  ------------------------------------------------------------------ */
#ifdef PERL_OBJECT
static SV *_ptrcast(CPerlObj *pPerl, SV *_PTRVALUE, char *type) {
#define ptrcast(a,b)  _ptrcast(pPerl,a,b)
#else
static SV *_ptrcast(SV *_PTRVALUE, char *type) {
#define ptrcast(a,b)  _ptrcast(a,b)
#endif
  char *r,*s;
  void *ptr;
  SV *obj;
  char *typestr,*c;

  /* Produce a "mangled" version of the type string.  */

  typestr = (char *) malloc(strlen(type)+20);

  /* Go through and munge the typestring */
  
  r = typestr;
  c = type;
  while (*c) {
    if (!isspace(*c)) {
      if ((*c == '*') || (*c == '&')) {
	strcpy(r,"Ptr");
	r+=3;
      } else *(r++) = *c;
    } 
    c++;
  }
  *(r++) = 0;
    
  /* Check to see if the input value is an integer */
  if (SvIOK(_PTRVALUE)) {
    ptr = (void *) SvIV(_PTRVALUE);
    /* Received a numerical value. Make a pointer out of it */
    obj = sv_newmortal();
    sv_setref_pv(obj,typestr,ptr);
  } else if (sv_isobject(_PTRVALUE)) {
    /* Have a real pointer value now.  Try to strip out the pointer value */
    /* Now extract the pointer value */
    if (!SWIG_GetPtr(_PTRVALUE,&ptr,0)) {
      obj = sv_newmortal();
      sv_setref_pv(obj,typestr,ptr);
    }
  } else {
    croak("ptrcast(). Not a reference.");
  }
  free(typestr);
  return obj;
}



/*------------------------------------------------------------------
  ptrvalue(ptr,type = 0)

  Attempts to dereference a pointer value.  If type is given, it 
  will try to use that type.  Otherwise, this function will attempt
  to "guess" the proper datatype by checking against all of the 
  builtin C datatypes. 
  ------------------------------------------------------------------ */

#ifdef PERL_OBJECT
static SV *_ptrvalue(CPerlObj *pPerl,SV *_PTRVALUE, int index, char *type) {
#define ptrvalue(a,b,c) _ptrvalue(pPerl,a,b,c)
#else
static SV *_ptrvalue(SV *_PTRVALUE, int index, char *type) {
#define ptrvalue(a,b,c) _ptrvalue(a,b,c)
#endif

  void     *ptr;
  SV       *obj = 0;


  if (SWIG_GetPtr(_PTRVALUE,&ptr,0)) {
    croak("Type error it ptrvalue. Argument is not a valid pointer value.");
  } else {
    /* If no datatype was passed, try a few common datatypes first */
    if (!type) {

      /* No datatype was passed.   Type to figure out if it's a common one */
      
      if (!SWIG_GetPtr(_PTRVALUE,&ptr,"intPtr")) {
	type = "int";
      } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"doublePtr")) {
	type = "double";
      } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"shortPtr")) {
	type = "short";
      } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"longPtr")) {
	type = "long";
      } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"floatPtr")) {
	type = "float";
      } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"charPtr")) {
	type = "char";
      } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"charPtrPtr")) {
	type = "char *";
      } else {
	type = "unknown";
      }
    }
    
    if (!ptr) {
      croak("Unable to dereference NULL pointer.");
      return 0;
    }

    /* Now we have a datatype.  Try to figure out what to do about it */
    if (strcmp(type,"int") == 0) {
      obj = sv_newmortal();
      sv_setiv(obj,(IV) *(((int *) ptr) + index));
    } else if (strcmp(type,"double") == 0) {
      obj = sv_newmortal();
      sv_setnv(obj,(double) *(((double *) ptr)+index));
    } else if (strcmp(type,"short") == 0) {
      obj = sv_newmortal();
      sv_setiv(obj,(IV) *(((short *) ptr) + index));
    } else if (strcmp(type,"long") == 0) {
      obj = sv_newmortal();
      sv_setiv(obj,(IV) *(((long *) ptr) + index));
    } else if (strcmp(type,"float") == 0) {
      obj = sv_newmortal();
      sv_setnv(obj,(double) *(((float *) ptr)+index));
    } else if (strcmp(type,"char") == 0) {
      obj = sv_newmortal();
      sv_setpv(obj,((char *) ptr)+index);
    } else if (strcmp(type,"char *") == 0) {
      char *c = *(((char **) ptr)+index);
      obj = sv_newmortal();
      if (c) 
	sv_setpv(obj,c);
      else 
	sv_setpv(obj,"NULL");
    } else {
      croak("Unable to dereference unsupported datatype.");
      obj = 0;
    }
  }
  return obj;
}

/*------------------------------------------------------------------
  ptrcreate(type,value = 0,numelements = 1)

  Attempts to create a new object of given type.  Type must be
  a basic C datatype.  Will not create complex objects.
  ------------------------------------------------------------------ */
#ifdef PERL_OBJECT
static SV *_ptrcreate(CPerlObj *pPerl, char *type, SV *value, int numelements) {
#define ptrcreate(a,b,c) _ptrcreate(pPerl,a,b,c)
#else
static SV *_ptrcreate(char *type, SV *value, int numelements) {
#define ptrcreate(a,b,c) _ptrcreate(a,b,c)
#endif

  void     *ptr;
  SV       *obj;
  int       sz;
  char     *cast;
  char      temp[40];

  /* Check the type string against a variety of possibilities */

  if (strcmp(type,"int") == 0) {
    sz = sizeof(int)*numelements;
    cast = "intPtr";
  } else if (strcmp(type,"short") == 0) {
    sz = sizeof(short)*numelements;
    cast = "shortPtr";
  } else if (strcmp(type,"long") == 0) {
    sz = sizeof(long)*numelements;
    cast = "longPtr";
  } else if (strcmp(type,"double") == 0) {
    sz = sizeof(double)*numelements;
    cast = "doublePtr";
  } else if (strcmp(type,"float") == 0) {
    sz = sizeof(float)*numelements;
    cast = "floatPtr";
  } else if (strcmp(type,"char") == 0) {
    sz = sizeof(char)*numelements;
    cast = "charPtr";
  } else if (strcmp(type,"char *") == 0) {
    sz = sizeof(char *)*(numelements+1);
    cast = "charPtrPtr";
  } else if (strcmp(type,"void") == 0) {
    sz = numelements;
    cast = "voidPtr";
  } else {
    croak("Unable to create unknown datatype."); 
    return 0;
  }
   
  /* Create the new object */
  
  ptr = (void *) malloc(sz);
  if (!ptr) {
    croak("Out of memory in ptrcreate."); 
    return 0;
  }

  /* Now try to set its default value */

  if (value) {
    if (strcmp(type,"int") == 0) {
      int *ip,i,ivalue;
      ivalue = (int) SvIV(value);
      ip = (int *) ptr;
      for (i = 0; i < numelements; i++)
	ip[i] = ivalue;
    } else if (strcmp(type,"short") == 0) {
      short *ip,ivalue;
      int i;
      ivalue = (short) SvIV(value);
      ip = (short *) ptr;
      for (i = 0; i < numelements; i++)
	ip[i] = ivalue;
    } else if (strcmp(type,"long") == 0) {
      long *ip,ivalue;
      int i;
      ivalue = (long) SvIV(value);
      ip = (long *) ptr;
      for (i = 0; i < numelements; i++)
	ip[i] = ivalue;
    } else if (strcmp(type,"double") == 0) {
      double *ip,ivalue;
      int i;
      ivalue = (double) SvNV(value);
      ip = (double *) ptr;
      for (i = 0; i < numelements; i++)
	ip[i] = ivalue;
    } else if (strcmp(type,"float") == 0) {
      float *ip,ivalue;
      int i;
      ivalue = (float) SvNV(value);
      ip = (float *) ptr;
      for (i = 0; i < numelements; i++)
	ip[i] = ivalue;
    } else if (strcmp(type,"char") == 0) {
      char *ip,*ivalue;
      ivalue = (char *) SvPV(value,PL_na);
      ip = (char *) ptr;
      strncpy(ip,ivalue,numelements-1);
    } else if (strcmp(type,"char *") == 0) {
      char **ip, *ivalue;
      int  i;
      ivalue = (char *) SvPV(value,PL_na);
      ip = (char **) ptr;
      for (i = 0; i < numelements; i++) {
	if (ivalue) {
	  ip[i] = (char *) malloc(strlen(ivalue)+1);
	  strcpy(ip[i],ivalue);
	} else {
	  ip[i] = 0;
	}
      }
      ip[numelements] = 0;
    }
  } 
  /* Create the pointer value */
  
  SWIG_MakePtr(temp,ptr,cast);
  obj = sv_newmortal();
  sv_setref_pv(obj,cast,ptr);
  return obj;
}

/*------------------------------------------------------------------
  ptrset(ptr,value,index = 0,type = 0)

  Attempts to set the value of a pointer variable.  If type is
  given, we will use that type.  Otherwise, we'll guess the datatype.
  ------------------------------------------------------------------ */

#ifdef PERL_OBJECT
static void _ptrset(CPerlObj *pPerl,SV *_PTRVALUE, SV *value, int index, char *type) {
#define ptrset(a,b,c,d) _ptrset(pPerl,a,b,c,d)
#else
static void _ptrset(SV *_PTRVALUE, SV *value, int index, char *type) {
#define ptrset(a,b,c,d) _ptrset(a,b,c,d)
#endif
  void     *ptr;
  SV       *obj;

  if (SWIG_GetPtr(_PTRVALUE,&ptr,0)) {
    croak("Type error in ptrset. Argument is not a valid pointer value.");
    return;
  }

  /* If no datatype was passed, try a few common datatypes first */

  if (!type) {

    /* No datatype was passed.   Type to figure out if it's a common one */

    if (!SWIG_GetPtr(_PTRVALUE,&ptr,"intPtr")) {
      type = "int";
    } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"doublePtr")) {
      type = "double";
    } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"shortPtr")) {
      type = "short";
    } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"longPtr")) {
      type = "long";
    } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"floatPtr")) {
      type = "float";
    } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"charPtr")) {
      type = "char";
    } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"charPtrPtr")) {
      type = "char *";
    } else {
      type = "unknown";
    }
  }

  if (!ptr) {
    croak("Unable to set NULL pointer.");
    return;
  }
  
  /* Now we have a datatype.  Try to figure out what to do about it */
  if (strcmp(type,"int") == 0) {
    *(((int *) ptr)+index) = (int) SvIV(value);
  } else if (strcmp(type,"double") == 0) {
    *(((double *) ptr)+index) = (double) SvNV(value);
  } else if (strcmp(type,"short") == 0) {
    *(((short *) ptr)+index) = (short) SvIV(value);
  } else if (strcmp(type,"long") == 0) {
    *(((long *) ptr)+index) = (long) SvIV(value);
  } else if (strcmp(type,"float") == 0) {
    *(((float *) ptr)+index) = (float) SvNV(value);
  } else if (strcmp(type,"char") == 0) {
    char *c = SvPV(value,PL_na);
    strcpy(((char *) ptr)+index, c);
  } else if (strcmp(type,"char *") == 0) {
    char *c = SvPV(value,PL_na);
    char **ca = (char **) ptr;
    if (ca[index]) free(ca[index]);
    if (strcmp(c,"NULL") == 0) {
      ca[index] = 0;
    } else {
      ca[index] = (char *) malloc(strlen(c)+1);
      strcpy(ca[index],c);
    }
  } else {
    croak("Unable to set unsupported datatype.");
    return;
  }
}

/*------------------------------------------------------------------
  ptradd(ptr,offset)

  Adds a value to an existing pointer value.  Will do a type-dependent
  add for basic datatypes.  For other datatypes, will do a byte-add.
  ------------------------------------------------------------------ */

#ifdef PERL_OBJECT
static SV *_ptradd(CPerlObj *pPerl, SV *_PTRVALUE, int offset) {
#define ptradd(a,b) _ptradd(pPerl,a,b)
#else
static SV *_ptradd(SV *_PTRVALUE, int offset) {
#define ptradd(a,b) _ptradd(a,b)
#endif

  void *ptr,*junk;
  SV   *obj;
  char *type;

  /* Try to handle a few common datatypes first */

  if (!SWIG_GetPtr(_PTRVALUE,&ptr,"intPtr")) {
    ptr = (void *) (((int *) ptr) + offset);
  } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"doublePtr")) {
    ptr = (void *) (((double *) ptr) + offset);
  } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"shortPtr")) {
    ptr = (void *) (((short *) ptr) + offset);
  } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"longPtr")) {
    ptr = (void *) (((long *) ptr) + offset);
  } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"floatPtr")) {
    ptr = (void *) (((float *) ptr) + offset);
  } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,"charPtr")) {
    ptr = (void *) (((char *) ptr) + offset);
  } else if (!SWIG_GetPtr(_PTRVALUE,&ptr,0)) {
    ptr = (void *) (((char *) ptr) + offset);
  } else {
    croak("Type error in ptradd. Argument is not a valid pointer value.");
    return 0;
  }
  type = SWIG_GetPtr(_PTRVALUE,&junk,"INVALID POINTER");
  obj = sv_newmortal();
  sv_setref_pv(obj,type,ptr);
  return obj;
}

/*------------------------------------------------------------------
  ptrmap(type1,type2)

  Allows a mapping between type1 and type2. (Like a typedef)
  ------------------------------------------------------------------ */

#ifdef PERL_OBJECT
static void _ptrmap(CPerlObj *pPerl,char *type1, char *type2) {
#define ptrmap(a,b) _ptrmap(pPerl,a,b)
#else
static void _ptrmap(char *type1, char *type2) {
#define ptrmap(a,b) _ptrmap(a,b)
#endif
  char *typestr1,*typestr2,*c,*r;
  /* Produce a "mangled" version of the type string.  */

  typestr1 = (char *) malloc(strlen(type1)+20);


  /* Go through and munge the typestring */
  
  r = typestr1;
  *(r++) = '_';
  c = type1;

  while (*c) {
    if (!isspace(*c)) {
      if ((*c == '*') || (*c == '&')) {
	strcpy(r,"Ptr");
	r+=3;
      }
      else *(r++) = *c;
    } 
    c++;
  }
  *(r++) = 0;

  typestr2 = (char *) malloc(strlen(type2)+20);

  /* Go through and munge the typestring */
  
  r = typestr2;
  *(r++) = '_';
  c = type2;
  while (*c) {
    if (!isspace(*c)) {
      if ((*c == '*') || (*c == '&')) {
	strcpy(r,"Ptr");
	r+=3;
      }
      else *(r++) = *c;
    }
    c++;
  }
  *(r++) = 0;
  SWIG_RegisterMapping(typestr1,typestr2,0);
  SWIG_RegisterMapping(typestr2,typestr1,0);
}

/*------------------------------------------------------------------
  ptrfree(ptr)

  Destroys a pointer value
  ------------------------------------------------------------------ */
#ifdef PERL_OBJECT
void _ptrfree(CPerlObj *pPerl, SV *_PTRVALUE) {
#define ptrfree(a) _ptrfree(pPerl, a)
#else
void _ptrfree(SV *_PTRVALUE) {
#define ptrfree(a) _ptrfree(a)
#endif

  void *ptr, *junk;

  if (SWIG_GetPtr(_PTRVALUE,&ptr,0)) {
    croak("Type error in ptrfree. Argument is not a valid pointer value.");
    return;
  }

  /* Check to see if this pointer is a char ** */
  if (!SWIG_GetPtr(_PTRVALUE,&junk,"charPtrPtr")) {
    char **c = (char **) ptr;
    if (c) {
      int i = 0;
      while (c[i]) {
	free(c[i]);
	i++;
      }
    }
  } 
  if (ptr)
    free((char *) ptr);
}

%}

%typemap(perl5,in) SV *ptr, SV *value {
  $target = $source;
}


%typemap(perl5,out) SV *ptrcast,
                     SV *ptrvalue,
                     SV *ptrcreate,
                     SV *ptradd
{
  $target = $source;
  argvi++;
}

%typemap(perl5,ret) int ptrset {
  if ($source == -1) return NULL;
}

SV *ptrcast(SV *ptr, char *type);
// Casts a pointer ptr to a new datatype given by the string type.
// type may be either the SWIG generated representation of a datatype
// or the C representation.  For example :
// 
//    ptrcast($ptr,"doublePtr");   # Perl5 representation
//    ptrcast($ptr,"double *");    # C representation
//
// A new pointer value is returned.   ptr may also be an integer
// value in which case the value will be used to set the pointer
// value.  For example :
//
//    $a = ptrcast(0,"VectorPtr");
//
// Will create a NULL pointer of type "VectorPtr"
//
// The casting operation is sensitive to formatting.  As a result,
// "double *" is different than "double*".  As a result of thumb,
// there should always be exactly one space between the C datatype
// and any pointer specifiers (*).

SV *ptrvalue(SV *ptr, int index = 0, char *type = 0);
// Returns the value that a pointer is pointing to (ie. dereferencing).
// The type is automatically inferred by the pointer type--thus, an
// integer pointer will return an integer, a double will return a double,
// and so on.   The index and type fields are optional parameters.  When
// an index is specified, this function returns the value of ptr[index].
// This allows array access.   When a type is specified, it overrides
// the given pointer type.   Examples :
//
//    ptrvalue($a)             #  Returns the value *a
//    ptrvalue($a,10)          #  Returns the value a[10]
//    ptrvalue($a,10,"double") #  Returns a[10] assuming a is a double *


void ptrset(SV *ptr, SV *value, int index = 0, char *type = 0);
// Sets the value pointed to by a pointer.  The type is automatically
// inferred from the pointer type so this function will work for
// integers, floats, doubles, etc...  The index and type fields are
// optional.  When an index is given, it provides array access.  When
// type is specified, it overrides the given pointer type.  Examples :
// 
//   ptrset($a,3)            # Sets the value *a = 3
//   ptrset($a,3,10)         # Sets a[10] = 3
//   ptrset($a,3,10,"int")   # Sets a[10] = 3 assuming a is a int *


SV *ptrcreate(char *type, SV *value = 0, int nitems = 1);
// Creates a new object and returns a pointer to it.  This function 
// can be used to create various kinds of objects for use in C functions.
// type specifies the basic C datatype to create and value is an
// optional parameter that can be used to set the initial value of the
// object.  nitems is an optional parameter that can be used to create
// an array.  This function results in a memory allocation using
// malloc().  Examples :
//
//   $a = ptrcreate("double")     # Create a new double, return pointer
//   $a = ptrcreate("int",7)      # Create an integer, set value to 7
//   $a = ptrcreate("int",0,1000) # Create an integer array with initial
//                                # values all set to zero
//
// This function only recognizes a few common C datatypes as listed below :
//
//        int, short, long, float, double, char, char *, void
//
// All other datatypes will result in an error.  However, other
// datatypes can be created by using the ptrcast function.  For
// example:
//
//  $a = ptrcast(ptrcreate("int",0,100),"unsigned int *")


void ptrfree(SV *ptr);
// Destroys the memory pointed to by ptr.  This function calls free()
// and should only be used with objects created by ptrcreate().  Since
// this function calls free, it may work with other objects, but this
// is generally discouraged unless you absolutely know what you're
// doing.

SV *ptradd(SV *ptr, int offset);
// Adds a value to the current pointer value.  For the C datatypes of
// int, short, long, float, double, and char, the offset value is the
// number of objects and works in exactly the same manner as in C.  For
// example, the following code steps through the elements of an array
//
//  $a = ptrcreate("double",0,100);    # Create an array double a[100]
//  $b = $a;
//  for ($i = 0; $i < 100; $i++) {
//      ptrset($b,0.0025*$i);          # set *b = 0.0025*i
//      $b = ptradd($b,1);             # b++ (go to next double)
//  }
//
// In this case, adding one to b goes to the next double.
// 
// For all other datatypes (including all complex datatypes), the
// offset corresponds to bytes.  This function does not perform any
// bounds checking and negative offsets are perfectly legal.  

void      ptrmap(char *type1, char *type2);
// This is a rarely used function that performs essentially the same
// operation as a C typedef.  To manage datatypes at run-time, SWIG
// modules manage an internal symbol table of type mappings.  This
// table keeps track of which types are equivalent to each other.  The
// ptrmap() function provides a mechanism for scripts to add symbols
// to this table.  For example :
//
//    ptrmap("doublePtr","RealPtr");
//
// would make the types "doublePtr" and "RealPtr" equivalent to each
// other.  Pointers of either type could now be used interchangably.
//
// Normally this function is not needed, but it can be used to
// circumvent SWIG's normal type-checking behavior or to work around
// weird type-handling problems.



