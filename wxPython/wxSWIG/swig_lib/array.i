//
// array.i
// Dave Beazley
// November 30, 1996
//
// This SWIG library file provides access to C arrays.

%module carray

%section "SWIG C Array Module",info,after,pre,nosort,skip=1,chop_left=3,chop_right=0,chop_top=0,chop_bottom=0

%text %{
%include array.i

This module provides scripting language access to various kinds of C/C++
arrays. For each datatype, a collection of four functions are created :

   <type>_array(size)              : Create a new array of given size
   <type>_get(array, index)        : Get an element from the array
   <type>_set(array, index, value) : Set an element in the array
   <type>_destroy(array)           : Destroy an array

The functions in this library are only low-level accessor functions
designed to directly access C arrays.  Like C, no bounds checking is
performed so use at your own peril.
%}

// Grab the SWIG exception library

#ifndef AUTODOC
%include exception.i
#endif

// A Typemap used to check input arguments.

%typemap(check) int *, double *, float *, char **, short *, long * {
  if (!$target) {
    SWIG_exception(SWIG_ValueError,"Received a NULL Pointer");
  }
}

%typemap(ret) int *, double *, float *, char **, short *, long * {
  if (!$source) {
    SWIG_exception(SWIG_MemoryError,"Out of memory.");
  }
}

// -----------------------------------------------------------------------
// Integer array support
// -----------------------------------------------------------------------

%subsection "Integer Arrays"
%text %{
The following functions provide access to integer arrays (mapped
onto the C 'int' datatype. 
%}

%{
#include <limits.h>

/* Create a new integer array */

  static int *int_array(int size) {
#ifdef __cplusplus
    return new int[size];
#else
    return (int *) malloc(size*sizeof(int));
#endif
  }

  /* Destroy an integer array */

  static void int_destroy(int *array) {
    if (array) {
#ifdef __cplusplus
      delete array;
#else
      free(array);
#endif
    }
  }

  /* Return an element */

  static int int_get(int *array, int index) {
    if (array) {
      return array[index];
    } else {
      return INT_MIN;
    }
  }
   
  /* Set an element */
  
  static int int_set(int *array, int index, int value) {
    if (array) {
      return (array[index] = value);
    } else {
      return INT_MIN;
    }
  }

%}

int *int_array(int nitems);
/* Creates a new array of integers. nitems specifies the number of elements.
   The array is created using malloc() in C and new() in C++. */

void int_destroy(int *array);
/* Destroys the given array. */

int int_get(int *array, int index);
/* Returns the value of array[index]. */

int int_set(int *array, int index, int value);
/* Sets array[index] = value.  Returns value. */


// -----------------------------------------------------------------------
// Floating point
// -----------------------------------------------------------------------

%subsection "Floating Point Arrays"
/* The following functions provide access to arrays of floats and doubles. */


%{
  #include <float.h>

  /* Create a new float array */

  static float *float_array(int size) {
#ifdef __cplusplus
    return new float[size];
#else
    return (float *) malloc(size*sizeof(float));
#endif
  }

  /* Destroy an array */

  static void float_destroy(float *array) {
    if (array) {
#ifdef __cplusplus
      delete array;
#else
      free(array);
#endif
    }
  }

  /* Return an element */

  static float float_get(float *array, int index) {
    if (array) {
      return array[index];
    } else {
      return FLT_MIN;
    }
  }
   
  /* Set an element */
  
  static float float_set(float *array, int index, float value) {
    if (array) {
      return (array[index] = value);
    } else {
      return FLT_MIN;
    }
  }

  /* Create a new double array */

  static double *double_array(int size) {
#ifdef __cplusplus
    return new double[size];
#else
    return (double *) malloc(size*sizeof(double));
#endif
  }

  /* Destroy an array */

  static void double_destroy(double *array) {
    if (array) {
#ifdef __cplusplus
      delete array;
#else
      free(array);
#endif
    }
  }

  /* Return an element */

  static double double_get(double *array, int index) {
    if (array) {
      return array[index];
    } else {
      return FLT_MIN;
    }
  }
   
  /* Set an element */
  
  static double double_set(double *array, int index, double value) {
    if (array) {
      return (array[index] = value);
    } else {
      return FLT_MIN;
    }
  }

%}

double *double_array(int nitems);
/* Creates a new array of doubles. nitems specifies the number of elements.
   The array is created using malloc() in C and new() in C++. */

void double_destroy(double *array);
/* Destroys the given array. */

double double_get(double *array, int index);
/* Returns the value of array[index]. */

double double_set(double *array, int index, double value);
/* Sets array[index] = value.  Returns value. */

float *float_array(int nitems);
/* Creates a new array of floats. nitems specifies the number of elements.
   The array is created using malloc() in C and new() in C++. */

void float_destroy(float *array);
/* Destroys the given array. */

float float_get(float *array, int index);
/* Returns the value of array[index]. */

float float_set(float *array, int index, float value);
/* Sets array[index] = value.  Returns value. */

// -----------------------------------------------------------------------
// Character strings
// -----------------------------------------------------------------------

%subsection "String Arrays"

%text %{
The following functions provide support for the 'char **' datatype.   This
is primarily used to handle argument lists and other similar structures that
need to be passed to a C/C++ function.
%}

#if defined(SWIGTCL)
%text %{
To convert from a Tcl list into a 'char **', the following code can be used :

     # $list is a list
     set args [string_array expr {[llength $list] + 1}]
     set i 0
     foreach a $list {
        string_set $args $i $a
        incr i 1
     }
     string_set $i ""
     # $args is now a char ** type
%}
#elif defined(SWIGPERL)

%text %{
To convert from a Perl list into a 'char **', code similar to the following
can be used :

    # @list is a list
    my $l = scalar(@list);
    my $args = string_array($l+1);
    my $i = 0;
    foreach $arg (@list) {
        string_set($args,$i,$arg);
        $i++;
    }   
    string_set($args,$i,"");

(of course, there is always more than one way to do it)
%}
#elif defined(SWIGPYTHON)

%text %{
To convert from a Python list to a 'char **', code similar to the following
can be used :

    # 'list' is a list
    args = string_array(len(list)+1)
    for i in range(0,len(list)):
	string_set(args,i,list[i])
    string_set(args,len(list),"")
%}

#endif

%{
/* Create character string arrays */

static char **string_array(int size) {
  char **a;
  int i;
#ifdef __cplusplus
  a = new char *[size];
#else
  a = (char **) malloc(size*sizeof(char *));
#endif
  for (i = 0; i < size; i++)
    a[i] = 0;
  return a;
}

/* Destroy a string array */

static void string_destroy(char **array) {
  int i = 0;
  if (array) {
      while (array[i]) {
#ifdef __cplusplus
            delete array[i];
#else
            free(array[i]);
#endif
            i++;
      }          	
#ifdef __cplusplus
     delete array;
#else
     free(array);
#endif
   }      
}

/* Get an element */

static char *string_get(char **array_string, int index) {
  if (array_string) 
    if (array_string[index]) return (array_string[index]);
    else return "";
  else 
    return "";
}

/* Set an element */

static char *string_set(char **array_string, int index, char * val) {
  if (array_string) {
    if (array_string[index]) {
#ifdef __cplusplus
	delete array_string[index];
#else
        free(array_string[index]);
#endif
    }	
    if (strlen(val) > 0) {
#ifdef __cplusplus
      array_string[index] = new char[strlen(val)+1];
#else
      array_string[index] = (char *) malloc(strlen(val)+1);
#endif
      strcpy(array_string[index],val);
      return array_string[index];
    } else {
      array_string[index] = 0;
      return val;
    }
  } else return val;
}

%}

char **string_array(int nitems);
/* Creates a new array of strings. nitems specifies the number of elements.
   The array is created using malloc() in C and new() in C++. Each element
   of the array is set to NULL upon initialization. */

void string_destroy(char **array);
/* Destroys the given array. Each element of the array is assumed to be
   a NULL-terminated string allocated with malloc() or new().  All of
   these strings will be destroyed as well. (It is probably only safe to
   use this function on an array created by string_array) */

char *string_get(char **array, int index);
/* Returns the value of array[index]. Returns a string of zero length
   if the corresponding element is NULL. */

char *string_set(char **array, int index, char *value);
/* Sets array[index] = value.  value is assumed to be a NULL-terminated
   string.  A string of zero length is mapped into a NULL value.  When
   setting the value, the value will be copied into a new string allocated
   with malloc() or new().  Any previous value in the array will be
   destroyed. */


%typemap(check) int *, double *, float *, char **, short *, long * = PREVIOUS;
%typemap(out) int *, double *, float *, char **, short *, long * = PREVIOUS;

