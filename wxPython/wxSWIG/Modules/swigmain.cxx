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

/***********************************************************************
 * $Header$
 *
 * swigmain.cxx
 *
 * The main program.
 *
 ***********************************************************************/

#include "wrap.h"
#include "swigtcl.h"
#include "tcl8.h"
#include "perl5.h"
#include "python.h"
// #include "pythoncom.h"
#include "guile.h"
#include "debug.h"
#include "ascii.h"
#include "latex.h"
#include "html.h"
#include "nodoc.h"
#include <ctype.h>

static char  *usage = "\
swig <options> filename\n\n\
Target Language Options:\n\
     -tcl            - Generate Tcl wrappers.\n\
     -tcl8           - Generate Tcl 8.0 wrappers.\n\
     -python         - Generate Python wrappers.\n\
     -perl5          - Generate Perl5 wrappers.\n\
     -guile          - Generate Guile wrappers.\n\
     -debug          - Parser debugging module.\n";

#ifdef MACSWIG     
static char *macmessage = "\
Copyright (c) 1995-1997\n\
University of Utah and the Regents of the University of California\n\n\
Enter SWIG processing options and filename below. For example :\n\
\n\
      -tcl -c++ interface.i\n\
\n\
-help displays a list of all available options.\n\
\n\
Note : Macintosh filenames should be enclosed in quotes if they contain whitespace.\n\
\n";

#endif

//-----------------------------------------------------------------
// main()
//
// Main program.    Initializes the files and starts the parser.
//-----------------------------------------------------------------

#ifndef MACSWIG
int main(int argc, char **argv) {
#else
int Mac_main(int argc, char **argv) {
#endif

  int i;

  Language *dl = new SWIG_LANG;
  Documentation *dd = new SWIG_DOC;
  extern int SWIG_main(int, char **, Language *, Documentation *);

  init_args(argc,argv);
  
  // Get options
  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if(strcmp(argv[i],"-tcl") == 0) {
	      dl = new TCL;
	      mark_arg(i);
          } else if (strcmp(argv[i],"-tcl8") == 0) {
	      dl = new TCL8;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-perl5") == 0) {
	      dl = new PERL5;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-python") == 0) {
	      dl = new PYTHON;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-debug") == 0) {
	      dl = new DEBUGLANG;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-guile") == 0) {
	      dl = new GUILE;
	      mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	      fputs(usage,stderr);
	      mark_arg(i);
	  }
      }
  }
  SWIG_main(argc,argv,dl,dd);
  return 0;
}

#ifdef MACSWIG
int MacMainEntry(char *options) {
	static char *_argv[256];
	int i,argc;
	char *c,*s,*t;

	swig_log = fopen("swig_log","w");
	fprintf(swig_log,"SWIG 1.1\n");
	fprintf(swig_log,"Options : %s\n", options);
	fprintf(swig_log,"-----------------------------------------------------\n");

	// Tokenize the user input
	
	_argv[0] = "swig";
	i=1;
	c = options;
	while (*c) {
	    while(isspace(*c)) c++;
	    if (*c) {
	      s = c;             // Starting character
	      while(isgraph(*c)) {
		if (*c == '\"') {
		  c++;
		  while ((*c) && (*c != '\"'))
		    c++;
		  c++;
		} else {
		  c++;
		}
	      }
	      // Found some whitespace 
	      if (*c) {
		*c = 0;
		c++;
	      }
	      _argv[i] = copy_string(s);
	      // Go through and remove quotes (if necessary)
	      
	      t = _argv[i];
	      while(*s) {
		if (*s != '\"') 
		  *(t++) = *s;
		s++;
	      }
	      *t = 0;
	      i++;
	    }
	}
	argc = i;
	_argv[i] = 0;
	return Mac_main(argc,_argv);
}
	
#endif	
