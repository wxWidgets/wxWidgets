//
// memory.i
// Dave Beazley
// November 30, 1996
// SWIG file for memory operations
//

%module memory
%{
#include <string.h>
%}

%section "Memory Manipulation Module",after,info,nosort,pre,chop_left=3,chop_bottom=0,chop_top=0,chop_right=0,skip=1

%text %{
%include memory.i

This module provides support for a few memory operations from the C
<string.h> library.  These functions can be used to manipulate binary
data. s and t are of type void *, cs and ct are both of type const void *.
%}

void *memcpy(void *s, const void *ct, int n);
/* Copy n characters from ct to s, and return s */

void *memmove(void *s, const void *ct, int n);
/* Same as memcpy except that it works even if the objects overlap. */

int memcmp(const void *cs, const void *ct, int n);
/* Compare the first n characters of cs with ct.  Returns 0 if
   they are equal, <0 if cs < ct, and >0 if cs > ct. */

void *memchr(const void *cs, char c, int n);
/* Returns pointer to the first occurrence of character c in cs. */

void *memset(void *s, char c, int n);
/* Place character c into first n characters of s, return s */


