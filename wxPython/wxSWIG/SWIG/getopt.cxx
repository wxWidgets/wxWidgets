/*******************************************************************************
 * Simplified Wrapper and Interface Generator  (SWIG)
 * 
 * Author : David Beazley
 *
 * Department of Computer Science        
 * University of Chicago
 * 1100 E 58th Street
 * Chicago, IL  60637
 * beazley@cs.uchicago.edu
 *
 * Please read the file LICENSE for the copyright and terms by which SWIG
 * can be used and distributed.
 *******************************************************************************/

#include "internal.h"

/*******************************************************************************
 * $Header$
 *
 * File : getopt.cxx
 *
 * This file defines a few functions for handling command line arguments.
 * C++ makes this really funky---especially since each language module
 * may want to extract it's own command line arguments.
 *
 * My own special version of getopt.   This is a bit weird, because we
 * don't know what the options are in advance (they could be determined
 * by a language module).
 *******************************************************************************/

static char **args;
static int    numargs;
static int   *marked;

// -----------------------------------------------------------------------------
// void init_args(int argc, char **argv)
// 
// Initializes the argument list.
//
// Inputs :
//          argc      = Argument count
//          argv      = Argument array
//
// Output : None
//
// Side Effects : Saves local copy of argc and argv
// -----------------------------------------------------------------------------

void
init_args(int argc, char **argv)
{
  int i;
  numargs = argc;
  args = argv;
  marked = new int[numargs];
  for (i = 0; i < argc; i++) {
    marked[i] = 0;
  }
  marked[0] = 1;
}

// -----------------------------------------------------------------------------
// void mark_arg(int n)
// 
// Marks an argument as being parsed.  All modules should do this whenever they
// parse a command line option.
//
// Inputs : n  =  Argument number
//
// Output : None
//
// Side Effects : Sets a status bit internally
// -----------------------------------------------------------------------------

void
mark_arg(int n) {
  if (marked)
    marked[n] = 1;
}

// -----------------------------------------------------------------------------
// void check_options()
// 
// Checks for unparsed command line options.  If so, issues an error and exits.
//
// Inputs : None
//
// Output : None
//
// Side Effects : exits if there are unparsed options
// -----------------------------------------------------------------------------
 
void check_options() {
 
    int error = 0;
    int i;

    if (!marked) {
      fprintf(stderr,"Must specify an input file.  Use -help for available options.\n");
      SWIG_exit(1);
    }
    for (i = 1; i < numargs-1; i++) {
        if (!marked[i]) {
            fprintf(stderr,"swig error : Unrecognized option %s\n", args[i]);
            error=1;
        }
    }
 
    if (error) {
        fprintf(stderr,"Use 'swig -help' for available options.\n");
        SWIG_exit(1);
    }

    if (marked[numargs-1]) {
      fprintf(stderr,"Must specify an input file. Use -help for available options.\n");
      SWIG_exit(1);
    }
}

// -----------------------------------------------------------------------------
// void arg_error()
// 
// Generates a generic error message and exits.
//
// Inputs : None
//
// Output : None
//
// Side Effects : Exits
// -----------------------------------------------------------------------------

void arg_error() {
  fprintf(stderr,"SWIG : Unable to parse command line options.\n");
  fprintf(stderr,"Use 'swig -help' for available options.\n");
  SWIG_exit(1);
}


	
		
