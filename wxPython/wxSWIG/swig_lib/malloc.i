//
// $Header$
//
// malloc.i
// Dave Beazley
// March 24, 1996
// SWIG file for memory management functions
// (also contained in stdlib.i)
//
/* Revision History
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

%module malloc
%{
#include <stdlib.h>
%}

%section "Memory Allocation Module",
         pre,info,after,nosort,chop_left=3,chop_right=0,chop_top=0,chop_bottom=0,skip=1

%text %{
%include malloc.i

This module provides access to a few basic C memory management functions.
All functions return void pointers, but realloc() and free() will operate
on any sort of pointer.   Sizes should be specified in bytes.
%}

void  *calloc(unsigned nobj, unsigned size);
/* Returns a pointer to a space for an array of nobj objects, each with
   size bytes.   Returns NULL if the request can't be satisfied. 
   Initializes the space to zero bytes. */

void  *malloc(unsigned size);
/* Returns a pointer to space for an object of size bytes.  Returns NULL
   upon failure. */

void  *realloc(void *ptr, unsigned size);
/* Changes the size of the object pointed to by ptr to size bytes. 
   The contents will be unchanged up the minimum of the old and new
   sizes.  Returns a pointer to the new space of NULL upon failure,
   in which case *ptr is unchanged. */

void   free(void *ptr);
/* Deallocates the space pointed to by ptr.  Does nothing if ptr is NULL.
   ptr must be a space previously allocated by calloc, malloc, or realloc. */

