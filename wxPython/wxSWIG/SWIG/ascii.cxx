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

#include "swig.h"
#include "ascii.h"
#include <ctype.h>

/*******************************************************************************
 * $Header$
 *
 * File : ascii.cxx
 *
 * Module for producing ASCII documentation.
 *
 *******************************************************************************/

// -----------------------------------------------------------------------------
// ASCII::ASCII()
//
// Constructor.  Initializes the ASCII module. 
// 
// Inputs : None
//
// Output : Documentation module object   
//
// Side Effects :
//     Sets page-width and indentation.
// -----------------------------------------------------------------------------

ASCII::ASCII() {
  sect_count = 0;
  indent     = 8;
  columns    = 70;
}

// -----------------------------------------------------------------------------
// void ASCII::print_string(char *s, int margin, int mode) 
// 
// Prints a string to the documentation file.  Performs line wrapping and
// other formatting.
//
// Inputs :
//          s      = NULL terminate ASCII string
//          margin = Number of characters to be inserted on left side
//          mode   = If set, text will be reformatted.  Otherwise, it's 
//                   printed verbatim (with indentation).
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void ASCII::print_string(char *s, int margin, int mode) { 

  char *c;
  int i;
  int lbreak = 0;
  int col;

  c = s;

  if (!s) return;
  // Apply indentation

  for (i = 0; i < margin; i++)
    fputc(' ',f_doc);

  col = margin;
  if (mode) {

    // Dump out text in formatted mode

    // Strip leading white-space

    while ((*c) && (isspace(*c))) {
      c++;
    }
    while (*c) {
      switch(*c) {
      case '\n':
      case '\\':
	if (lbreak) {
	  col = margin;
	  fputc('\n',f_doc);
	  for (i = 0; i < margin; i++)
	    fputc(' ',f_doc);
	  lbreak = 0;
	} else {
	  if ((*c) == '\n') {
	    col++;
	  }
	  lbreak++;
	}
	break;
      case ' ':
      case '\t':
      case '\r':
      case '\f':
	if (col > columns) {
	  fputc('\n',f_doc);
	  for (i = 0; i < margin; i++)
	    fputc(' ',f_doc);
	  col = margin;
	} else {
	  fputc(' ',f_doc);
	  col++;
	}
	// Skip over rest of white space found 
	while ((*c) && isspace(*c)) c++;
	c--;
	lbreak = 0;
	break;
      default :
	if (lbreak) fputc(' ',f_doc);
	lbreak = 0;
	fputc(*c,f_doc);
	col++;
	break;
      }
      c++;
    }
  } else {
    // Dump out text in pre-formatted mode
    while (*c) {
      switch(*c) {
      case '\n':
	fputc('\n',f_doc);
	for (i = 0; i < margin; i++)
	  fputc(' ',f_doc);
	break;
      default :
	fputc(*c,f_doc);
	col++;
	break;
      }
      c++;
    }
  } 
}

// -----------------------------------------------------------------------------
// void ASCII::print_decl(DocEntry *de)
// 
// Prints the documentation entry corresponding to a declaration
//
// Inputs : 
//          de = Documentation entry (which should be for a declaration)
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void ASCII::print_decl(DocEntry *de) { 

  int i;
  char *c;

  c = de->usage.get();
  fprintf(f_doc,"%s\n",c);

  // If there is any C annotation, print that
  if (de->print_info) {
    c = de->cinfo.get();
    if (strlen(c) > 0) {
      for (i = 0; i < indent; i++)
	fputc(' ',f_doc);
      fprintf(f_doc,"[ ");
      print_string(c,0,1);
      fprintf(f_doc," ]\n");
    }
  }

  c = de->text.get();
  if (strlen(c) > 0) {
    print_string(c,indent,de->format);
    fprintf(f_doc,"\n");
    if (de->format) fputc('\n',f_doc);
  } else {
    fprintf(f_doc,"\n");
  }
}

// -----------------------------------------------------------------------------
// void ASCII::print_text(DocEntry *de)
//
// Prints the documentation for a block of text.  Will strip any leading white
// space from the text block.
// 
// Inputs : 
//          de = Documentation entry of text
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void ASCII::print_text(DocEntry *de) {
  char *c;
  c = de->text.get();
  if (strlen(c) > 0) {
    while ((*c == '\n')) c++;
    print_string(c,0,de->format);
    fprintf(f_doc,"\n\n");
  }
}

// -----------------------------------------------------------------------------
// void ASCII::title(DocEntry *de)
// 
// Sets the title of the documentation file.
//
// Inputs : 
//          de = Documentation entry of the title.
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void ASCII::title(DocEntry *de) { 
  char *c;

  c = de->usage.get();
  if (strlen(c) > 0) {
    fprintf(f_doc,"%s\n\n",c);
  }

  // If there is any C annotation, print that
  if (de->print_info) {
    c = de->cinfo.get();
    if (strlen(c) > 0) {
      fprintf(f_doc,"[ ");
      print_string(c,0,1);
      fprintf(f_doc," ]\n");
    }
  }

  c = de->text.get();
  if (strlen(c)) {
    print_string(c,0,de->format);
  }
  fprintf(f_doc,"\n\n");
}

// -----------------------------------------------------------------------------
// void ASCII::newsection(DocEntry *de, int sectnum) 
// 
// Starts a new section.  Will underline major sections and subsections, but
// not minor subsections.
//
// Inputs : 
//          de      = Documentation entry of the section
//          sectnum = Section number.
//
// Output : None
//
// Side Effects :
//          Forces a new subsection to be created within the ASCII module.
// -----------------------------------------------------------------------------

void ASCII::newsection(DocEntry *de,int sectnum) {
  int i,len = 0;
  char temp[256];
  char *c;

  sect_num[sect_count] = sectnum;
  sect_count++;
  for (i = 0; i < sect_count; i++) {
    sprintf(temp,"%d.",sect_num[i]);
    fprintf(f_doc,"%s",temp);
    len += strlen(temp);
  }
  c = de->usage.get();
  fprintf(f_doc,"  %s\n", c);
  len += strlen(c) + 2;

  // Print an underline if this is a major category

  if (sect_count <= 1) {
    for (i = 0; i < len; i++) 
      fputc('=',f_doc);
    fputc('\n',f_doc);
  } else if (sect_count == 2) {
    for (i = 0; i < len; i++) 
      fputc('-',f_doc);
    fputc('\n',f_doc);
  } else {
    fputc('\n',f_doc);
  }
  
  // If there is any C annotation, print that
  if (de->print_info) {
    c = de->cinfo.get();
    if (strlen(c) > 0) {
      fprintf(f_doc,"[ ");
      print_string(c,0,1);
      fprintf(f_doc," ]\n\n");
    }
  }

  // If there is a description text. Print it

  c = de->text.get();
  if (strlen(c) > 0) {
    print_string(c,0,de->format);
    fprintf(f_doc,"\n");
  }
  fprintf(f_doc,"\n");
}

// -----------------------------------------------------------------------------
// void ASCII::endsection()
// 
// Ends the current section.  It is an error to call this without having first
// called newsection().
// 
// Inputs : None
//
// Output : None
//
// Side Effects : 
//          Pops out of the current section, moving back into the parent section
// -----------------------------------------------------------------------------

void ASCII::endsection() {
  if (sect_count > 0) sect_count--;
}

// -----------------------------------------------------------------------------
// void ASCII::separator()
// 
// Prints a small dashed line that is used to designate the end of C++ class
// subsections.
//
// Inputs : None
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void ASCII::separator() {
  int i;
  for (i = 0; i < 10; i++)
    fputc('-',f_doc);
  fprintf(f_doc,"\n\n");
}

// -----------------------------------------------------------------------------
// void ASCII::init(char *filename)
// 
// Initializes the documentation module and opens up the documentation file.
//
// Inputs : filename = name of documentation file (without suffix)
//
// Output : None
//
// Side Effects : Opens the documentation file.
// -----------------------------------------------------------------------------

void ASCII::init(char *filename) {
  char f[256];

  sprintf(f,"%s.doc",filename);
  sprintf(fn,"%s",filename);
  f_doc = fopen(f,"w");
  if (f_doc == NULL) {
    fprintf(stderr, "Unable to open %s\n", fn);
    SWIG_exit(1);
  }

}

// -----------------------------------------------------------------------------
// void ASCII::close() 
//
// Closes the documentation module.  This function should only be called once
// 
// Inputs : None
//
// Output : None
//
// Side Effects : Closes the documentation file.
// -----------------------------------------------------------------------------

void ASCII::close(void) {

  fclose(f_doc);
  if (Verbose) 
    fprintf(stderr,"Documentation written to %s.doc\n", fn);

}

// -----------------------------------------------------------------------------
// void ASCII::style(char *name, char *value) 
// 
// Looks for style parameters that the user might have supplied using the
// %style directive.   Unrecognized options are simply ignored.
//
// Inputs : 
//          name    = name of the style parameter
//          value   = value of the style parameter (optional)
//
// Output : None
//
// Side Effects : Can change internal settings of 'indent' and 'columns' members.
// -----------------------------------------------------------------------------

void ASCII::style(char *name, char *value) {
  if (strcmp(name,"ascii_indent") == 0) {
    if (value) {
      indent = atoi(value);
    }
  } else if (strcmp(name,"ascii_columns") == 0) {
    if (value) {
      columns = atoi(value);
    }
  }
}

// -----------------------------------------------------------------------------
// void ASCII::parse_args(int argc, char **argv) 
// 
// Function for processing options supplied on the SWIG command line.
//
// Inputs : 
//          argc = Number of arguments
//          argv = Argument strings
//
// Output : None
//
// Side Effects : May set various internal parameters.
// -----------------------------------------------------------------------------

static char *ascii_usage = "\
ASCII Documentation Options (available with -dascii)\n\
     None available.\n\n";

void ASCII::parse_args(int argc, char **argv) {
  int i;

  for (i = 0; i < argc; i++) {
    if (argv[i]) {
      if (strcmp(argv[i],"-help") == 0) {
	fputs(ascii_usage,stderr);
      }
    }
  }
}
  
  
