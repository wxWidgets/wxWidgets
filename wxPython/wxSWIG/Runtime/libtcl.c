/*
 * $Header$
 * 
 * swigtcl.swg
 */

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

/*****************************************************************************
 * $Header$
 *
 * swigptr.swg
 *****************************************************************************/

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SWIG_NOINCLUDE
extern void SWIG_MakePtr(char *, void *, char *);
extern void SWIG_RegisterMapping(char *, char *, void *(*)(void *));
extern char *SWIG_GetPtr(char *, void **, char *);
#else

#ifdef SWIG_GLOBAL
#define SWIGSTATICRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGSTATICRUNTIME(a) static a
#endif

/* SWIG pointer structure */
typedef struct SwigPtrType {
  char               *name;               /* Datatype name                  */
  int                 len;                /* Length (used for optimization) */
  void               *(*cast)(void *);    /* Pointer casting function       */
  struct SwigPtrType *next;               /* Linked list pointer            */
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
static int SwigStart[256];             /* Starting positions of types            */
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

/* Register a new datatype with the type-checker */
SWIGSTATICRUNTIME(void) 
SWIG_RegisterMapping(char *origtype, char *newtype, void *(*cast)(void *)) {
  int i;
  SwigPtrType *t = 0,*t1;

  /* Allocate the pointer table if necessary */
  if (!SwigPtrTable) {     
    SwigPtrTable = (SwigPtrType *) malloc(SwigPtrMax*sizeof(SwigPtrType));
  }

  /* Grow the table */
  if (SwigPtrN >= SwigPtrMax) {
    SwigPtrMax = 2*SwigPtrMax;
    SwigPtrTable = (SwigPtrType *) realloc((char *) SwigPtrTable,SwigPtrMax*sizeof(SwigPtrType));
  }
  for (i = 0; i < SwigPtrN; i++) {
    if (strcmp(SwigPtrTable[i].name,origtype) == 0) {
      t = &SwigPtrTable[i];
      break;
    }
  }
  if (!t) {
    t = &SwigPtrTable[SwigPtrN++];
    t->name = origtype;
    t->len = strlen(t->name);
    t->cast = 0;
    t->next = 0;
  }

  /* Check for existing entries */
  while (t->next) {
    if ((strcmp(t->name,newtype) == 0)) {
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
SWIG_MakePtr(char *c, const void *ptr, char *type) {
  static char hex[17] = "0123456789abcdef";
  unsigned long p, s;
  char result[24], *r; 
  r = result;
  p = (unsigned long) ptr;
  if (p > 0) {
    while (p > 0) {
      s = p & 0xf;
      *(r++) = hex[s];
      p = p >> 4;
    }
    *r = '_';
    while (r >= result)
      *(c++) = *(r--);
    strcpy (c, type);
  } else {
    strcpy (c, "NULL");
  }
}

/* Function for getting a pointer value */
SWIGSTATICRUNTIME(char *) 
SWIG_GetPtr(char *c, void **ptr, char *t)
{
  unsigned long p;
  char temp_type[256], *name;
  int  i, len, start, end;
  SwigPtrType *sp,*tp;
  SwigCacheType *cache;
  register int d;

  p = 0;
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) return (char *) 0;
    else c;
  }
  c++;
  /* Extract hex value from pointer */
  while (d = *c) {
    if ((d >= '0') && (d <= '9'))
      p = (p << 4) + (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
      p = (p << 4) + (d - ('a'-10));
    else
      break; 
    c++;
  }
  *ptr = (void *) p;
  if ((!t) || (strcmp(t,c)==0)) return (char *) 0;

  if (!SwigPtrSort) {
    qsort((void *) SwigPtrTable, SwigPtrN, sizeof(SwigPtrType), swigsort); 
    for (i = 0; i < 256; i++) SwigStart[i] = SwigPtrN;
    for (i = SwigPtrN-1; i >= 0; i--) SwigStart[(int) (SwigPtrTable[i].name[1])] = i;
    for (i = 255; i >= 1; i--) {
      if (SwigStart[i-1] > SwigStart[i])
	SwigStart[i-1] = SwigStart[i];
    }
    SwigPtrSort = 1;
    for (i = 0; i < SWIG_CACHESIZE; i++) SwigCache[i].stat = 0;
  }
  /* First check cache for matches.  Uses last cache value as starting point */
  cache = &SwigCache[SwigLastCache];
  for (i = 0; i < SWIG_CACHESIZE; i++) {
    if (cache->stat && (strcmp(t,cache->name) == 0) && (strcmp(c,cache->mapped) == 0)) {
      cache->stat++;
      if (cache->tp->cast) *ptr = (*(cache->tp->cast))(*ptr);
      return (char *) 0;
    }
    SwigLastCache = (SwigLastCache+1) & SWIG_CACHEMASK;
    if (!SwigLastCache) cache = SwigCache;
    else cache++;
  }
  /* Type mismatch.  Look through type-mapping table */
  start = SwigStart[(int) t[1]];
  end = SwigStart[(int) t[1]+1];
  sp = &SwigPtrTable[start];

  /* Try to find a match */
  while (start <= end) {
    if (strncmp(t,sp->name,sp->len) == 0) {
      name = sp->name;
      len = sp->len;
      tp = sp->next;
      /* Try to find entry for our given datatype */
      while(tp) {
	if (tp->len >= 255) {
	  return c;
	}
	strcpy(temp_type,tp->name);
	strncat(temp_type,t+len,255-tp->len);
	if (strcmp(c,temp_type) == 0) {
	  strcpy(SwigCache[SwigCacheIndex].mapped,c);
	  strcpy(SwigCache[SwigCacheIndex].name,t);
	  SwigCache[SwigCacheIndex].stat = 1;
	  SwigCache[SwigCacheIndex].tp = tp;
	  SwigCacheIndex = SwigCacheIndex & SWIG_CACHEMASK;
	  /* Get pointer value */
	  *ptr = (void *) p;
	  if (tp->cast) *ptr = (*(tp->cast))(*ptr);
	  return (char *) 0;
	}
	tp = tp->next;
      }
    }
    sp++;
    start++;
  }
  return c;
} 

#endif
  
#ifdef __cplusplus
}
#endif

