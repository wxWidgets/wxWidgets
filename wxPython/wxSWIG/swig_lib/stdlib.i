//
// $Header$
//
// stdlib.i
// Dave Beazley
// March 24, 1996
// SWIG file for some C stdlib functions
//
/* Revision history
 * $Log$
 * Revision 1.1  2002/04/29 19:56:49  RD
 * Since I have made several changes to SWIG over the years to accomodate
 * special cases and other things in wxPython, and since I plan on making
 * several more, I've decided to put the SWIG sources in wxPython's CVS
 * instead of relying on maintaining patches.  This effectivly becomes a
 * fork of an obsolete version of SWIG, :-( but since SWIG 1.3 still
 * doesn't have some things I rely on in 1.1, not to mention that my
 * custom patches would all have to be redone, I felt that this is the
 * easier road to take.
 *
 * Revision 1.1.1.1  1999/02/28 02:00:53  beazley
 * Swig1.1
 *
 * Revision 1.1  1996/05/22 17:27:01  beazley
 * Initial revision
 *
 */

%module stdlib
%{
#include <stdlib.h>
%}

typedef unsigned int size_t;

double atof(const char *s);
int    atoi(const char *s);
long   atol(const char *s);
int    rand();
void   srand(unsigned int seed);
void  *calloc(size_t nobj, size_t size);
void  *malloc(size_t size);
void  *realloc(void *ptr, size_t size);
void   free(void *ptr);
void   abort(void);
int    system(const char *s);
char  *getenv(const char *name);
int    abs(int n);
long   labs(long n);

