//
// ctype.i
// Dave Beazley
// November 30, 1996
// SWIG file for character tests
//

%module ctype
%{
#include <ctype.h>
%}

%section "Character Class Testing Module",after,info,nosort,pre,chop_left=3,chop_bottom=0,chop_top=0,chop_right=0,skip=1

%text %{
%include ctype.i

This module provides access to a number of functions for testing
characters.   These functions are in the C <ctype.h> library.
Most scripting languages already provide much of this functionality,
but just in case you want to use one of the built-in C functions,
you can use this module.
%}

int isalnum(char c);
/* Returns 1 if isalpha(c) or isdigit(c) is true. */

int isalpha(char c);
/* Returns 1 if isupper(c) or islower(c) is true. */

int iscntrl(char c);
/* Returns 1 if c is a control character. */

int isdigit(char c);
/* Returns 1 if c is a decimal digit. */

int isgraph(char c);
/* Returns 1 if c is a printing character except space. */

int islower(char c);
/* Returns 1 if c is a lower-case letter. */

int isprint(char c);
/* Returns 1 if c is a printing character including space. */

int ispunct(char c);
/* Returns 1 if c is a printing character except space or letter
   or digit. */

int isspace(char c);
/* Returns 1 if c is a space, formfeed, newline, carriage return,
   tab, or vertical tab. */

int isupper(char c);
/* Returns 1 if c is an upper case letter. */

int isxdigit(char c);
/* Returns 1 if c is a hexadecimal digit. */

char tolower(char c);
/* Converts c to lower case */

char toupper(char c);
/* Converts c to upper case */
