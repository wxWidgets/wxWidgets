#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
/* Definitions for compiling Perl extensions on a variety of machines */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
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

#ifdef PERL_OBJECT
#define MAGIC_PPERL  CPerlObj *pPerl = (CPerlObj *) this;
#define MAGIC_CAST   (int (CPerlObj::*)(SV *, MAGIC *))
#define SWIGCLASS_STATIC 
#else
#define MAGIC_PPERL
#define MAGIC_CAST
#define SWIGCLASS_STATIC static
#endif

#if defined(WIN32) && defined(PERL_OBJECT) && !defined(PerlIO_exportFILE)
#define PerlIO_exportFILE(fh,fl) (FILE*)(fh)
#endif

/* Modifications for newer Perl 5.005 releases */

#if !defined(PERL_REVISION) || ((PERL_REVISION >= 5) && ((PERL_VERSION < 5) || ((PERL_VERSION == 5) && (PERL_SUBVERSION < 50))))
#ifndef PL_sv_yes
#define PL_sv_yes sv_yes
#endif
#ifndef PL_sv_undef
#define PL_sv_undef sv_undef
#endif
#ifndef PL_na
#define PL_na na
#endif
#endif

/******************************************************************************
 * Pointer type-checking code
 *****************************************************************************/

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SWIG_NOINCLUDE
extern void SWIG_MakePtr(char *, void *, char *);
#ifndef PERL_OBJECT
extern void SWIG_RegisterMapping(char *, char *, void *(*)(void *));
#else
#define SWIG_RegisterMapping(a,b,c) _SWIG_RegisterMapping(pPerl,a,b,c);
extern void _SWIG_RegisterMapping(CPerlObj *,char *, char *, void *(*)(void *),int);
#endif
#ifndef PERL_OBJECT
extern char *SWIG_GetPtr(SV *, void **, char *);
#else
extern char *_SWIG_GetPtr(CPerlObj *, SV *, void **, char *);
#define SWIG_GetPtr(a,b,c) _SWIG_GetPtr(pPerl,a,b,c)
#endif

#else

#ifdef SWIG_GLOBAL
#define SWIGSTATICRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGSTATICRUNTIME(a) static a
#endif

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
static SwigPtrType *SwigPtrTable = 0;  /* Table containing pointer equivalences  */
static int SwigStart[256];             /* Table containing starting positions    */

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

/* Register a new datatype with the type-checker */

#ifndef PERL_OBJECT
SWIGSTATICRUNTIME(void) 
SWIG_RegisterMapping(char *origtype, char *newtype, void *(*cast)(void *)) {
#else
#define SWIG_RegisterMapping(a,b,c) _SWIG_RegisterMapping(pPerl, a,b,c)
SWIGSTATICRUNTIME(void)
_SWIG_RegisterMapping(CPerlObj *pPerl, char *origtype, char *newtype, void *(*cast)(void *)) {
#endif

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
    t->len = strlen(t->name);
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
  t1->len = strlen(t1->name);
  t1->cast = cast;
  t1->next = 0;
  t->next = t1;
  SwigPtrSort = 0;
}

/* Make a pointer value string */

SWIGSTATICRUNTIME(void) 
SWIG_MakePtr(char *_c, const void *_ptr, char *type) {
  static char _hex[16] =
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f'};
  unsigned long _p, _s;
  char _result[20], *_r;    /* Note : a 64-bit hex number = 16 digits */
  _r = _result;
  _p = (unsigned long) _ptr;
  if (_p > 0) {
    while (_p > 0) {
      _s = _p & 0xf;
      *(_r++) = _hex[_s];
      _p = _p >> 4;
    }
    *_r = '_';
    while (_r >= _result)
      *(_c++) = *(_r--);
  } else {
    strcpy (_c, "NULL");
  }
  if (_ptr)
    strcpy (_c, type);
}

/* Function for getting a pointer value */

#ifndef PERL_OBJECT
SWIGSTATICRUNTIME(char *) 
SWIG_GetPtr(SV *sv, void **ptr, char *_t)
#else
#define SWIG_GetPtr(a,b,c) _SWIG_GetPtr(pPerl,a,b,c)
SWIGSTATICRUNTIME(char *)
_SWIG_GetPtr(CPerlObj *pPerl, SV *sv, void **ptr, char *_t)
#endif
{
  char temp_type[256];
  char *name,*_c;
  int  len,i,start,end;
  IV   tmp;
  SwigPtrType *sp,*tp;
  SwigCacheType *cache;

  /* If magical, apply more magic */

  if (SvGMAGICAL(sv))
    mg_get(sv);

  /* Check to see if this is an object */
  if (sv_isobject(sv)) {
    SV *tsv = (SV*) SvRV(sv);
    if ((SvTYPE(tsv) == SVt_PVHV)) {
      MAGIC *mg;
      if (SvMAGICAL(tsv)) {
	mg = mg_find(tsv,'P');
	if (mg) {
	  SV *rsv = mg->mg_obj;
	  if (sv_isobject(rsv)) {
	    tmp = SvIV((SV*)SvRV(rsv));
	  }
	}
      } else {
	return "Not a valid pointer value";
      }
    } else {
      tmp = SvIV((SV*)SvRV(sv));
    }
    if (!_t) {
      *(ptr) = (void *) tmp;
      return (char *) 0;
    }
  } else if (! SvOK(sv)) {            /* Check for undef */
    *(ptr) = (void *) 0;
    return (char *) 0;
  } else if (SvTYPE(sv) == SVt_RV) {       /* Check for NULL pointer */
    *(ptr) = (void *) 0;
    if (!SvROK(sv)) 
      return (char *) 0;
    else
      return "Not a valid pointer value";
  } else {                                 /* Don't know what it is */
      *(ptr) = (void *) 0;
      return "Not a valid pointer value";
  }
  if (_t) {
    /* Now see if the types match */      

    if (!sv_isa(sv,_t)) {
      _c = HvNAME(SvSTASH(SvRV(sv)));
      if (!SwigPtrSort) {
	qsort((void *) SwigPtrTable, SwigPtrN, sizeof(SwigPtrType), swigsort);  
	for (i = 0; i < 256; i++) {
	  SwigStart[i] = SwigPtrN;
	}
	for (i = SwigPtrN-1; i >= 0; i--) {
	  SwigStart[SwigPtrTable[i].name[0]] = i;
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
	      *ptr = (void *) tmp;
	      if (cache->tp->cast) *ptr = (*(cache->tp->cast))(*ptr);
	      return (char *) 0;
	    }
	  }
	}
	SwigLastCache = (SwigLastCache+1) & SWIG_CACHEMASK;
	if (!SwigLastCache) cache = SwigCache;
	else cache++;
      }

      start = SwigStart[_t[0]];
      end = SwigStart[_t[0]+1];
      sp = &SwigPtrTable[start];
      while (start < end) {
	if (swigcmp(_t,sp) == 0) break;
	sp++;
	start++;
      }
      if (start > end) sp = 0;
      while (start <= end) {
	if (swigcmp(_t,sp) == 0) {
	  name = sp->name;
	  len = sp->len;
	  tp = sp->next;
	  while(tp) {
	    if (tp->len >= 255) {
	      return _c;
	    }
	    strcpy(temp_type,tp->name);
	    strncat(temp_type,_t+len,255-tp->len);
	    if (sv_isa(sv,temp_type)) {
	      /* Get pointer value */
	      *ptr = (void *) tmp;
	      if (tp->cast) *ptr = (*(tp->cast))(*ptr);

	      strcpy(SwigCache[SwigCacheIndex].mapped,_c);
	      strcpy(SwigCache[SwigCacheIndex].name,_t);
	      SwigCache[SwigCacheIndex].stat = 1;
	      SwigCache[SwigCacheIndex].tp = tp;
	      SwigCacheIndex = SwigCacheIndex & SWIG_CACHEMASK;
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
      *ptr = (void *) tmp;
      return _c;
    } else {
      /* Found a match on the first try.  Return pointer value */
      *ptr = (void *) tmp;
      return (char *) 0;
    }
  } 
  *ptr = (void *) tmp;
  return (char *) 0;
}

#endif
#ifdef __cplusplus
}
#endif






