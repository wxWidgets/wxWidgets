/**************************************************************************
 * $Header$
 *
 * swigtcl8.swg
 *
 * This file provides type-checked pointer support to Tcl 8.0.
 **********************************************************************/

#if defined(_WIN32) || defined(__WIN32__)
#   if defined(_MSC_VER)
#	define SWIGEXPORT(a) __declspec(dllexport) a
#   else
#	if defined(__BORLANDC__)
#	    define SWIGEXPORT(a) a _export 
#	else
#	    define SWIGEXPORT(a) a 
#	endif
#   endif
#else
#   define SWIGEXPORT(a) a 
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SWIG_GLOBAL
#include <tcl.h>
#define SWIGSTATICRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGSTATICRUNTIME(a) static a
#endif

#ifdef SWIG_NOINCLUDE
extern void SWIG_SetPointerObj(Tcl_Obj *, void *, char *);
extern void SWIG_RegisterMapping(char *, char *, void *(*)(void *));
extern char *SWIG_GetPointerObj(Tcl_Interp *, Tcl_Obj *, void **, char *);
extern int  SWIG_MakePtr(char *, const void *, char *);
extern void SWIG_RegisterType();
#else

/* These are internal variables.   Should be static */

typedef struct SwigPtrType {
  char               *name;
  int                 len;
  void               *(*cast)(void *);
  struct SwigPtrType *next;
} SwigPtrType;

/* Pointer cache structure */

typedef struct {
  int                 stat;               /* Status (valid) bit             */
  SwigPtrType        *tp;                 /* Pointer to type structure      */
  char                name[256];          /* Given datatype name            */
  char                mapped[256];        /* Equivalent name                */
} SwigCacheType;

static int SwigPtrMax  = 64;           /* Max entries that can be currently held */
static int SwigPtrN    = 0;            /* Current number of entries              */
static int SwigPtrSort = 0;            /* Status flag indicating sort            */
static int SwigStart[256];             /* Array containing start locations (for searching) */
static SwigPtrType *SwigPtrTable = 0;  /* Table containing pointer equivalences  */

/* Cached values */

#define SWIG_CACHESIZE  8
#define SWIG_CACHEMASK  0x7
static SwigCacheType SwigCache[SWIG_CACHESIZE];  
static int SwigCacheIndex = 0;
static int SwigLastCache = 0;

/* Sort comparison function */
static int swigsort(const void *data1, const void *data2) {
	SwigPtrType *d1 = (SwigPtrType *) data1;
	SwigPtrType *d2 = (SwigPtrType *) data2;
	return strcmp(d1->name,d2->name);
}

/* Binary Search function */
static int swigcmp(const void *key, const void *data) {
  char *k = (char *) key;
  SwigPtrType *d = (SwigPtrType *) data;
  return strncmp(k,d->name,d->len);
}


/*---------------------------------------------------------------------
 * SWIG_RegisterMapping(char *origtype, char *newtype, void *(*cast)(void *))
 *
 * Register a new type-mapping with the type-checking system.
 *---------------------------------------------------------------------*/

SWIGSTATICRUNTIME(void) 
SWIG_RegisterMapping(char *origtype, char *newtype, void *(*cast)(void *)) {

  int i;
  SwigPtrType *t = 0, *t1;

  if (!SwigPtrTable) {     
    SwigPtrTable = (SwigPtrType *) malloc(SwigPtrMax*sizeof(SwigPtrType));
    SwigPtrN = 0;
  }
  if (SwigPtrN >= SwigPtrMax) {
    SwigPtrMax = 2*SwigPtrMax;
    SwigPtrTable = (SwigPtrType *) realloc(SwigPtrTable,SwigPtrMax*sizeof(SwigPtrType));
  }
  for (i = 0; i < SwigPtrN; i++)
    if (strcmp(SwigPtrTable[i].name,origtype) == 0) {
      t = &SwigPtrTable[i];
      break;
    }
  if (!t) {
    t = &SwigPtrTable[SwigPtrN];
    t->name = origtype;
    t->len = strlen(origtype);
    t->cast = 0;
    t->next = 0;
    SwigPtrN++;
  }
  while (t->next) {
    if (strcmp(t->name,newtype) == 0) {
      if (cast) t->cast = cast;
      return;
    }
    t = t->next;
  }
  t1 = (SwigPtrType *) malloc(sizeof(SwigPtrType));
  t1->name = newtype;
  t1->len = strlen(newtype);
  t1->cast = cast;
  t1->next = 0;
  t->next = t1;
  SwigPtrSort = 0;
}


/*---------------------------------------------------------------------
 * void SWIG_SetPointerObj(Tcl_Obj *objPtr, void *ptr, char *type)
 *
 * Sets a Tcl object to a pointer value.
 *           ptr = void pointer value
 *           type = string representing type
 *
 *---------------------------------------------------------------------*/

SWIGSTATICRUNTIME(void)
SWIG_SetPointerObj(Tcl_Obj *objPtr, void *_ptr, char *type) {
  static char _hex[16] =
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f'};
  unsigned long _p, _s;
  char _result[20], *_r;    /* Note : a 64-bit hex number = 16 digits */
  char _temp[20], *_c;
  _r = _result;
  _p = (unsigned long) _ptr;
  if (_p > 0) {
    while (_p > 0) {
      _s = _p & 0xf;
      *(_r++) = _hex[_s];
      _p = _p >> 4;
    }
    *_r = '_';
    _c = &_temp[0];
    while (_r >= _result)
      *(_c++) = *(_r--);
    *_c = 0;
    Tcl_SetStringObj(objPtr,_temp,-1);
  } else {
    Tcl_SetStringObj(objPtr,"NULL",-1);
  }
  if (_ptr)
    Tcl_AppendToObj(objPtr,type,-1);
}

/* This is for backwards compatibility */

SWIGSTATICRUNTIME(int) 
SWIG_MakePtr(char *_c, const void *_ptr, char *type)
{
  static char _hex[16] =
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f'};
  unsigned long _p, _s;
  char _result[20], *_r;
  int      l = 0;
  _r = _result;
  _p = (unsigned long) _ptr;
  if (_p > 0) {
    while (_p > 0) {
      _s = _p & 0xf;
      *(_r++) = _hex[_s];
      _p = _p >> 4;
      l++;
    }
    *_r = '_';
    l++;
    while (_r >= _result)
      *(_c++) = *(_r--);
    _r = type;
    while (*_r)
      *(_c++) = *(_r++);
    *(_c) = 0;
  } else {
      strcpy (_c, "NULL");
  }
  return l;
}

/*---------------------------------------------------------------------
 * char *SWIG_GetPointerObj(Tcl_Interp *interp, Tcl_Obj *objPtr, void **ptr, char *type)
 *
 * Attempts to extract a pointer value from our pointer type.
 * Upon failure, returns a string corresponding to the actual datatype.
 * Upon success, returns NULL and sets the pointer value in ptr.
 *---------------------------------------------------------------------*/

SWIGSTATICRUNTIME(char *)
SWIG_GetPointerObj(Tcl_Interp *interp, Tcl_Obj *objPtr, void **ptr, char *_t) {
  unsigned long _p;
  char temp_type[256];
  char *name;
  int  i, len;
  SwigPtrType *sp,*tp;
  SwigCacheType *cache;
  int  start, end;
  char *_c;
  _p = 0;

  /* Extract the pointer value as a string */
  _c = Tcl_GetStringFromObj(objPtr, &i);

  /* Pointer values must start with leading underscore */
  if (*_c == '_') {
      _c++;
      /* Extract hex value from pointer */
      while (*_c) {
	  if ((*_c >= '0') && (*_c <= '9'))
	    _p = (_p << 4) + (*_c - '0');
	  else if ((*_c >= 'a') && (*_c <= 'f'))
	    _p = (_p << 4) + ((*_c - 'a') + 10);
	  else
	    break;
	  _c++;
      }

      if (_t) {
	if (strcmp(_t,_c)) { 
	  if (!SwigPtrSort) {
	    qsort((void *) SwigPtrTable, SwigPtrN, sizeof(SwigPtrType), swigsort); 
	    for (i = 0; i < 256; i++) {
	      SwigStart[i] = SwigPtrN;
	    }
	    for (i = SwigPtrN-1; i >= 0; i--) {
	      SwigStart[(int) (SwigPtrTable[i].name[1])] = i;
	    }
	    for (i = 255; i >= 1; i--) {
	      if (SwigStart[i-1] > SwigStart[i])
		SwigStart[i-1] = SwigStart[i];
	    }
	    SwigPtrSort = 1;
	    for (i = 0; i < SWIG_CACHESIZE; i++)  
	      SwigCache[i].stat = 0;
	  }
	  
	  /* First check cache for matches.  Uses last cache value as starting point */
	  cache = &SwigCache[SwigLastCache];
	  for (i = 0; i < SWIG_CACHESIZE; i++) {
	    if (cache->stat) {
	      if (strcmp(_t,cache->name) == 0) {
		if (strcmp(_c,cache->mapped) == 0) {
		  cache->stat++;
		  *ptr = (void *) _p;
		  if (cache->tp->cast) *ptr = (*(cache->tp->cast))(*ptr);
		  return (char *) 0;
		}
	      }
	    }
	    SwigLastCache = (SwigLastCache+1) & SWIG_CACHEMASK;
	    if (!SwigLastCache) cache = SwigCache;
	    else cache++;
	  }
	  /* We have a type mismatch.  Will have to look through our type
	     mapping table to figure out whether or not we can accept this datatype */

	  start = SwigStart[(int) _t[1]];
	  end = SwigStart[(int) _t[1]+1];
	  sp = &SwigPtrTable[start];
	  while (start < end) {
	    if (swigcmp(_t,sp) == 0) break;
	    sp++;
	    start++;
	  }
	  if (start > end) sp = 0;
	  /* Try to find a match for this */
	  while (start <= end) {
	    if (swigcmp(_t,sp) == 0) {
	      name = sp->name;
	      len = sp->len;
	      tp = sp->next;
	      /* Try to find entry for our given datatype */
	      while(tp) {
		if (tp->len >= 255) {
		  return _c;
		}
		strcpy(temp_type,tp->name);
		strncat(temp_type,_t+len,255-tp->len);
		if (strcmp(_c,temp_type) == 0) {
		  
		  strcpy(SwigCache[SwigCacheIndex].mapped,_c);
		  strcpy(SwigCache[SwigCacheIndex].name,_t);
		  SwigCache[SwigCacheIndex].stat = 1;
		  SwigCache[SwigCacheIndex].tp = tp;
		  SwigCacheIndex = SwigCacheIndex & SWIG_CACHEMASK;
		  
		  /* Get pointer value */
		  *ptr = (void *) _p;
		  if (tp->cast) *ptr = (*(tp->cast))(*ptr);
		  return (char *) 0;
		}
		tp = tp->next;
	      }
	    }
	    sp++;
	    start++;
	  }
	  /* Didn't find any sort of match for this data.  
	     Get the pointer value and return the received type */
	  *ptr = (void *) _p;
	  return _c;
	} else {
	  /* Found a match on the first try.  Return pointer value */
	  *ptr = (void *) _p;
	  return (char *) 0;
	}
      } else {
	/* No type specified.  Good luck */
	*ptr = (void *) _p;
	return (char *) 0;
      }
  } else {
    if (strcmp (_c, "NULL") == 0) {
	*ptr = (void *) 0;
	return (char *) 0;
    }
    *ptr = (void *) 0;	
    return _c;
  }
}

/*---------------------------------------------------------------------
 * void SWIG_RegisterType()
 *
 * Registers our new datatype with an interpreter.
 *---------------------------------------------------------------------*/

SWIGSTATICRUNTIME(void)
SWIG_RegisterType() {
  /* Does nothing at the moment */
}

#endif

#ifdef __cplusplus
}
#endif
