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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/*******************************************************************************
 * $Header$
 *
 * File : include.cxx
 *
 * Code for including files into a wrapper file.
 *
 *******************************************************************************/

/* Delimeter used in accessing files and directories */

#ifdef MACSWIG
#define  DELIMETER  ':'
#else
#define  DELIMETER  '/'
#endif

/* Linked list containing search directories */

struct   Dnode {
  char    *dirname;
  Dnode   *next;
};

Dnode   ihead, iz;
int     include_init = 0;

/* Linked list containing included files */

struct  Inode {
  char    *name;
  Inode   *next;
};

Inode  *include_list = 0;

// -----------------------------------------------------------------------------
// void add_directory(char *dirname)
//
// Adds a directory to the SWIG search path.
// 
// Inputs : dirname  = Pathname
//
// Output : None
//
// Side Effects : Adds dirname to linked list of pathnames.
// -----------------------------------------------------------------------------

void add_directory(char *dirname) {
  Dnode  *d;

  if (!include_init) {
    ihead.next = &iz;
    iz.next = &iz;
    iz.dirname = new char[2];
    iz.dirname[0] = 0;
    include_init = 1;
  }

  d = new Dnode;
  d->dirname = new char[strlen(dirname)+1];
  strcpy(d->dirname,dirname);
  d->next = ihead.next;
  ihead.next = d;

}

// -----------------------------------------------------------------------------
// int add_iname(char *name)
// 
// Adds an include file to the list of processed files.  If already present,
// returns 1. 
//
// Inputs : name  = filename
//
// Output : 0 on success, 1 on failure.
//
// Side Effects : Adds name to linked list.
// -----------------------------------------------------------------------------

int add_iname(char *name) {

  Inode *newi, *i;

  //  if (WrapExtern) return 0;        // Still thinking about this patch.
  if (include_list) {
    /* Search list */
    i = include_list;
    while (i) {
      if (strcmp(i->name, name) == 0) return 1;
      i = i->next;
    }
  }

  newi = new Inode;
  newi->name = new char[strlen(name)+1];
  strcpy(newi->name, name);
  newi->next = include_list;
  include_list = newi;
  return 0;
}

// -----------------------------------------------------------------------------
// void check_suffix(char *name)
// 
// Checks the suffix of an include file to see if we need to handle it
// differently.  C and C++ source files need a little extra help.
//
// Inputs : name  = include file name.
//
// Output : None
//
// Side Effects : 
//          Sets ForceExtern status variable if a C/C++ source file
//          is detected.
//
// -----------------------------------------------------------------------------

void check_suffix(char *name) {
  char *c;

  if (!name) return;
  if (strlen(name) == 0) return;
  c = name+strlen(name)-1;
  while (c != name) {
    if (*c == '.') break;
    c--;
  }
  if (c == name) return;

  /* Check suffixes */

  if (strcmp(c,".c") == 0) {
    ForceExtern = 1;
  } else if (strcmp(c,".C") == 0) {
    ForceExtern = 1;
  } else if (strcmp(c,".cc") == 0) {
    ForceExtern = 1;
  } else if (strcmp(c,".cxx") == 0) {
    ForceExtern = 1;
  } else if (strcmp(c,".c++") == 0) {
    ForceExtern = 1;
  } else if (strcmp(c,".cpp") == 0) {
    ForceExtern = 1;
  } else {
    ForceExtern = 0;
  }
}
// -----------------------------------------------------------------------------
// int include_file(char *name)
// 
// Includes a new SWIG wrapper file. Returns -1 if file not found.
//
// Inputs : name  = filename
//
// Output : 0 on success. -1 on failure.
//
// Side Effects : sets scanner to read from new file.
// -----------------------------------------------------------------------------

int include_file(char *name) {

  FILE  *f;
  char   filename[256];
  int    found = 0;
  Dnode  *d;
  extern void scanner_file(FILE *);

  if (!include_init) return -1;    // Not initialized yet
  if (add_iname(name)) {
    if (Verbose) fprintf(stderr,"file %s already included.\n", name);
    return -1;  // Already included this file
  }

  if (Verbose) {
    fprintf(stderr,"Wrapping %s...\n", name);	
    fprintf(stderr,"Looking for ./%s\n", name);
  }

  if ((f = fopen(name,"r")) != NULL) {
    input_file = new char[strlen(name)+1];
    strcpy(input_file,name);
    scanner_file(f);
    check_suffix(name);
    return 0;
  }

  // Now start searching libraries

  d = ihead.next;               // Start of search list
  while (d != &iz) {
    // Look for the wrap file in language directory
    sprintf(filename,"%s%c%s%c%s", d->dirname,DELIMETER,LibDir,DELIMETER,name);
    if (Verbose) fprintf(stderr,"Looking for %s\n", filename);
    if((f = fopen(filename,"r")) != NULL) {
      found = 1;
    } else {
      sprintf(filename,"%s%c%s", d->dirname, DELIMETER,name);
      if (Verbose) fprintf(stderr,"Looking for %s\n", filename);
      if ((f = fopen(filename,"r")) != NULL) {
	found = 1;
      }
    }
    if (found) {
      // Found it, open it up for input
      input_file = new char[strlen(filename)+1];
      strcpy(input_file,filename);
      scanner_file(f);
      check_suffix(name);
      return 0;
    }
    d = d->next;
  }

  if (!found) fprintf(stderr,"%s : Line %d. Unable to find include file %s (ignored).\n",input_file, line_number, name);

  return -1;
}


static char  buffer[1024];

// -----------------------------------------------------------------------------
// void copy_data(FILE *f1, FILE *f2)
//
// Copies data from file f1 to file f2.
// 
// Inputs : f1  = FILE 1
//          f2  = FILE 2
//
// Output : None
//
// Side Effects : Closes file f1 upon exit.
// -----------------------------------------------------------------------------

void copy_data(FILE *f1, FILE *f2) {

  while (fgets(buffer,1023,f1)) {
    fputs(buffer, f2);
  }
  fclose(f1);
}

// -----------------------------------------------------------------------------
// void copy_data(FILE *f1, String *s2)
//
// Copies data from file f1 to String s2.
// 
// Inputs : f1  = FILE 1
//          s2  = String
//
// Output : None
//
// Side Effects : Closes file f1 upon exit.
// -----------------------------------------------------------------------------

void copy_data(FILE *f1, String &s2) {

  while (fgets(buffer,1023,f1)) {
    s2 << buffer;
  }
  fclose(f1);
}

// -----------------------------------------------------------------------------
// int insert_file(char *name, FILE *f)
//
// Looks for a file and inserts into file f.
//
// Inputs : name  = filename
//          f     = FILE
//
// Output : 0 on success, -1 on failure.
//
// Side Effects : None
// -----------------------------------------------------------------------------

int insert_file(char *name, FILE *f_out) {

  FILE  *f;
  char   filename[256];
  int    found = 0;
  Dnode  *d;

  if (!include_init) return -1;    // Not initialized yet
  if (add_iname(name)) {
    if (Verbose) fprintf(stderr,"file %s already included.\n", name);
    return -1;  // Already included this file
  }

  if (Verbose) fprintf(stderr,"Looking for ./%s\n", name);
  if ((f = fopen(name,"r")) != NULL) {
      copy_data(f,f_out);
      return 0;
  }

  // Now start searching libraries

  d = ihead.next;               // Start of search list
  while (d != &iz) {
    // Look for the wrap file in language directory
    sprintf(filename,"%s%c%s%c%s", d->dirname,DELIMETER,LibDir,DELIMETER,name);
    if (Verbose) fprintf(stderr,"Looking for %s\n", filename);
    if((f = fopen(filename,"r")) != NULL) {
      found = 1;
    } else {
      sprintf(filename,"%s%c%s", d->dirname, DELIMETER, name);
      if (Verbose) fprintf(stderr,"Looking for %s\n", filename);
      if ((f = fopen(filename,"r")) != NULL) {
	found = 1;
      }
    }
    if (found) {
	copy_data(f,f_out);
	return 0;
    }
    d = d->next;
  }

  if ((!found) && (Verbose)) fprintf(stderr,"unable to find %s. (Ignored)\n",name);
  return -1;
}

// -----------------------------------------------------------------------------
// void swig_append(char *filename, FILE *f)
// 
// Appends the contents of filename to stream f.
//
// Inputs : 
//          filename  = File to append
//          f         = FILE handle 
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void swig_append(char *filename, FILE *f) {

  FILE *in_file;

  if ((in_file = fopen(filename,"r")) == NULL) {
      fprintf(stderr,"** SWIG ERROR ** file %s not found.\n",filename);
      FatalError();
      return;
  }
  while (fgets(buffer,1023,in_file)) {
    fputs(buffer, f);
  }
  fclose(in_file);
}


// -----------------------------------------------------------------------------
// int get_file(char *name, String &str)
//
// Looks for a file and converts it into a String.  
//
// Inputs : name  = filename
//          str   = String
//
// Output : 0 on success, -1 on failure.
//
// Side Effects : None
// -----------------------------------------------------------------------------

int get_file(char *name, String &str) {

  FILE  *f;
  char   filename[256];
  int    found = 0;
  Dnode  *d;

  if (!include_init) return -1;    // Not initialized yet

  if (Verbose) fprintf(stderr,"Looking for %s\n", name);
  if ((f = fopen(name,"r")) != NULL) {
      copy_data(f,str);
      return 0;
  }

  // Now start searching libraries

  d = ihead.next;               // Start of search list
  while (d != &iz) {
    // Look for the wrap file in language directory
    sprintf(filename,"%s%c%s%c%s", d->dirname,DELIMETER,LibDir,DELIMETER,name);
    if (Verbose) fprintf(stderr,"Looking for %s\n", filename);
    if((f = fopen(filename,"r")) != NULL) {
      found = 1;
    } else {
      sprintf(filename,"%s%c%s", d->dirname, DELIMETER, name);
      if (Verbose) fprintf(stderr,"Looking for %s\n", filename);
      if ((f = fopen(filename,"r")) != NULL) {
	found = 1;
      }
    }
    if (found) {
	copy_data(f,str);
	return 0;
    }
    d = d->next;
  }

  if ((!found)) fprintf(stderr,"SWIG Error. Unable to find %s. Possible installation problem.\n",name);
  FatalError();
  return -1;
}

static char *libs[1000];
static int   nlibs = 0;

// -----------------------------------------------------------------------------
// void library_add(char *name)
//
// Adds a filename to the list of libraries.   This is usually only called by
// the SWIG main program.
//
// Inputs : name  = library name
//
// Outputs: None
//
// Side Effects : Adds the library name to the libs array above
// -----------------------------------------------------------------------------

void library_add(char *name) {
  int i;

  // Check to make sure it's not already added

  if (!(*name)) return;

  for (i = 0; i < nlibs; i++) {
    if (strcmp(libs[i],name) == 0) return;
  }

  libs[nlibs] = copy_string(name);
  nlibs++;
}

// -----------------------------------------------------------------------------
// void library_insert()
// 
// Starts parsing all of the SWIG library files.
// 
// Inputs : None
//
// Output : None
//
// Side Effects : Opens and attaches all of the specified library files to
//                the scanner.
//
// Bugs : Opens all of the files.   Will fail if there are too many open files.
//
// -----------------------------------------------------------------------------

void library_insert() {
  int i;

  i = nlibs-1;
  while (i >= 0) {
    include_file(libs[i]);
    i--;
  }
}

// -----------------------------------------------------------------------------
// int checkout(char *filename,char *dest) 
// 
// Tries to check a file out of the SWIG library. If found, it will save it in
// the current directory.   This is a useful mechanism for using SWIG as a code
// manager and for extracting library files.
//
// Inputs : filename = Library file
//          dest     = Destination file
//
// Output : 0 on success
//         -1 on failure.
//
// Side Effects :  None
// -----------------------------------------------------------------------------

int checkout_file(char *filename,char *dest) {

  FILE *f1;
  char tempn[256];

  // First check to see if the file already exists in current directory
  f1 = fopen(dest,"r");
  if (f1) {
    if (Verbose)
      fprintf(stderr,"Warning. Unable to check-out %s. File already exists.\n", filename);
    fclose(f1);
    return -1;
  } 

  while (!f1) {
    sprintf(tempn,"%s%d",dest,rand());
    f1 = fopen(tempn,"r");
    if (f1) {
      fclose(f1);
      f1 = 0;
    } else {
      f1 = fopen(tempn,"w");
      if (!f1) {
	fprintf(stderr,"Unable to open %s for writing\n", tempn);
	return -1;
      }
    }
  }

  // Now try to insert the library file into the destination file
  if ((insert_file(filename,f1)) == -1) {
    fprintf(stderr,"Unable to check-out '%s'. File does not exist in SWIG library.\n",filename);
    fclose(f1);
    remove(tempn);       // Unsuccessful, remove file we created
    return -1;
  }
  fclose(f1);
  // Now move the file
  rename(tempn,dest);
  return 0;
}


// -----------------------------------------------------------------------------
// int checkin_file(char *dir, char *lang, char *source,char *dest) 
// 
// Attempts to check a file into the SWIG library.  
//
// Inputs : dir      = Location of the SWIG library.
//          lang     = Language specific subdirectory.
//          source   = Source file.
//          dest     = Destination file.
//
// Output : 0 on success
//         -1 on failure.
//
// Side Effects :  None
// -----------------------------------------------------------------------------

int checkin_file(char *dir, char *lang, char *source, char *dest) {

  FILE *f1;
  char tempn[256];
  String s;

  // First check to see if the file exists
  
  f1 = fopen(source,"r");
  if (!f1) return -1;

  copy_data(f1,s);

  // Now try to open the destination file
  sprintf(tempn,"%s/%s/%s", dir,lang,dest);
  f1 = fopen(tempn,"w");
  if (!f1) return -1;
  fprintf(f1,"%s",s.get());
  fclose(f1);
  return 0;
}


     

