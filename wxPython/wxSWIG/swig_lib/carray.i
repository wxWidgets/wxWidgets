//
// $Header$
// carray.i
// Dave Beazley
// March 24, 1996
//
// This SWIG library file supports C arrays of various datatypes.
// These arrays are probably *not* compatible with scripting languages
// but they are compatible with C functions.
//
/* Revision History
 * -- $Log$
 * -- Revision 1.1  2002/04/29 19:56:49  RD
 * -- Since I have made several changes to SWIG over the years to accomodate
 * -- special cases and other things in wxPython, and since I plan on making
 * -- several more, I've decided to put the SWIG sources in wxPython's CVS
 * -- instead of relying on maintaining patches.  This effectivly becomes a
 * -- fork of an obsolete version of SWIG, :-( but since SWIG 1.3 still
 * -- doesn't have some things I rely on in 1.1, not to mention that my
 * -- custom patches would all have to be redone, I felt that this is the
 * -- easier road to take.
 * --
 * -- Revision 1.1.1.1  1999/02/28 02:00:53  beazley
 * -- Swig1.1
 * --
 * -- Revision 1.1  1996/05/22 17:23:48  beazley
 * -- Initial revision
 * --
 */

%module carray
%{

#include <string.h>

/* Create an integer array of given size */

static int *array_int(int size) {
  return (int *) malloc(size*sizeof(int));
}

static int get_int(int *array_int, int index) {
  if (array_int) 
    return (array_int[index]);
  else 
    return 0;
}

static int set_int(int *array_int, int index, int val) {
  if (array_int)
    return (array_int[index] = val);
  else
    return 0;
}

/* Create double precision arrays */

static double *array_double(int size) {
  return (double *) malloc(size*sizeof(double));
}

static double get_double(double *array_double, int index) {
  if (array_double) 
    return (array_double[index]);
  else 
    return 0;
}

static double set_double(double *array_double, int index, double val) {
  if (array_double)
    return (array_double[index] = val);
  else
    return 0;
}

/* Create byte arrays */

typedef unsigned char byte;

static byte *array_byte(int size) {
  return (byte *) malloc(size*sizeof(byte));
}

static byte get_byte(byte *array_byte, int index) {
  if (array_byte) 
    return (array_byte[index]);
  else 
    return 0;
}

static byte set_byte(byte *array_byte, int index, byte val) {
  if (array_byte)
    return (array_byte[index] = val);
  else
    return 0;
}

/* Create character string arrays */

static char **array_string(int size) {
  char **a;
  int i;

  a = (char **) malloc(size*sizeof(char *));
  for (i = 0; i < size; i++)
    a[i] = 0;
  return a;
}

static char *get_string(char **array_string, int index) {
  if (array_string) 
    return (array_string[index]);
  else 
    return "";
}

static char *set_string(char **array_string, int index, char * val) {
  if (array_string) {
    if (array_string[index]) free(array_string[index]);
    if (strlen(val) > 0) {
      array_string[index] = (char *) malloc(strlen(val)+1);
      strcpy(array_string[index],val);
      return array_string[index];
    } else {
      array_string[index] = 0;
      return val;
    }
  }
  else
    return val;
  }

%}

%section "Array Operations"

int *array_int(int size);
/* Creates an integer array of size elements.  Integers are the same
size as the C int type. */

int get_int(int *array_int, int index) ;
/* Return the integer in array_int[index] */

int set_int(int *array_int, int index, int ival);
/* Sets array_int[index] = ival. Returns it's value so you
can use this function in an expression. */

/* Create double precision arrays */

double *array_double(int size);
/* Creates an array of double precision floats. */

double get_double(double *array_double, int index);
/* Return the double in array_double[index] */

double set_double(double *array_double, int index, double dval);
/* Sets array_double[index] = dval.  Returns it's value */

typedef unsigned char byte;

byte *array_byte(int nbytes);
/* Creates a byte array.  A byte is defined as an unsigned char. */

byte get_byte(byte *array_byte, int index);
/* Returns array_byte[index] */

byte set_byte(byte *array_byte, int index, byte val);
/* Sets array_byte[index] = val.  Returns it's new value */

char **array_string(int size);
/* Creates a string array.  A string is array is the same as char ** in C */

char *get_string(char **array_string, int index);
/* Returns character string in array_string[index]. If that entry is
NULL, returns an empty string */

char *set_string(char **array_string, int index, char * string);
/* Sets array_string[index] = string.  string must be a 0-terminated
ASCII string.  If string is "" then this will create a NULL pointer. */



