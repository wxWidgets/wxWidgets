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
 * latex.c
 *
 * Latex specific functions for producing documentation.
 *
 ***********************************************************************/

#include "swig.h"
#include "latex.h"

// -------------------------------------------------------------------
// LATEX::LATEX()
//
// Create new LaTeX handler
// -------------------------------------------------------------------

LATEX::LATEX() {
  sect_count = 0;

  tag_pagestyle = "\\pagestyle{headings}";
  tag_parindent = "0.0in";
  tag_textwidth = "6.5in";
  tag_documentstyle = "[11pt]{article}";
  tag_oddsidemargin = "0.0in";
  tag_title = "{\\Large \\bf : }";
  tag_preformat = "{\\small \\begin{verbatim}:\\end{verbatim}}";
  tag_usage = "{\\tt \\bf : }";
  tag_descrip = "\\\\\n\\makebox[0.5in]{}\\begin{minipage}[t]{6in}:\n\\end{minipage}\\\\\n";
  tag_text = ":\\\\";
  tag_cinfo = "{\\tt : }\\\\";
  tag_section = "\\section{:}";
  tag_subsection="\\subsection{:}";
  tag_subsubsection="\\subsubsection{:}";
}

// -------------------------------------------------------------------
// char *start_tag(char *tag) {
//
// Returns the start of a tag
// -------------------------------------------------------------------

char *LATEX::start_tag(char *tag) {
  static String stag;
  char  *c;

  stag = "";
  c = tag;
  while ((*c) && (*c != ':')) {
    stag << *c;
    c++;
  }
  return stag.get();
}

// -------------------------------------------------------------------
// char *end_tag(char *tag) {
//
// Returns the end of a tag
// -------------------------------------------------------------------

char *LATEX::end_tag(char *tag) {
  static String etag;
  char  *c;

  etag = "";
  c = tag;
  while ((*c) && (*c != ':')) {
    c++;
  }
  if (*c) {
    c++;
    while (*c) {
      etag << *c;
      c++;
    }
  }
  return etag.get();
}
	
// -------------------------------------------------------------------
// LATEX::print_string(char *s, String &str)
//
// Dumps string s to str, but performs some LaTeX character replacements
// -------------------------------------------------------------------

void LATEX::print_string(char *s, String &str) { 

  char *c;
  c = s;
  while (*c) {
    switch(*c) {
    case '*':
    case '<':
    case '>':
    case '+':
    case '=':
    case '|':
      str << "$" << *c << "$";
      break;
    case '\\':
      str << "\\\\";
      break;
    case '_':
      str << "\\_";
      break;
    case '%':
      str << "\\%";
      break;
    case '$':
      str << "\\$";
      break;
    case '&':
      str << "\\&";
      break;
    case '#':
      str << "\\#";
      break;
    case '\n':
      str << "\\\\\n";
      break;
    default :
      str << *c;
      break;
    }
    c++;
  }
}

// --------------------------------------------------------------
// LATEX::print_decl(DocEntry *)
//
// Print a documentation entry
// --------------------------------------------------------------

void LATEX::print_decl(DocEntry *de) { 

  char *c;

  c = de->usage.get();
  
  if (c) {
    s_doc << start_tag(tag_usage);
    print_string(c,s_doc);
    s_doc << end_tag(tag_usage) << "\n";
  }
  
  // Check to see if there any information available

  if ((strlen(de->cinfo.get()) && de->print_info) || strlen(de->text.get())) {
    
    // There is additional information now.   If we're in preformatting mode,
    // we need to handle things differently

    s_doc << start_tag(tag_descrip) << "\n";
    
    if (!de->format) {
      // Verbatim mode
      s_doc << start_tag(tag_preformat) << "\n";

      // If there is any C annotation, print that
      if (de->print_info) {
	c = de->cinfo.get();
	if (strlen(c) > 0) {
	  s_doc << "[ " << c << " ]\n";
	}
      }
      c = de->text.get();
      if (strlen(c) > 0) {
	s_doc << c;
      }
      s_doc << end_tag(tag_preformat) << "\n";
    } else {
      // We are in format mode now
      // We need to emit some stubs for the description format

      // If there is any C annotation, print that
      if (de->print_info) {
	c = de->cinfo.get();
	if (strlen(c) > 0) {
	  s_doc << start_tag(tag_cinfo) << "[ ";
	  print_string(c,s_doc);
	  s_doc << " ] " << end_tag(tag_cinfo) << "\n";
	}
      }
      // Print out descriptive text (if any).
      c = de->text.get();
      if (strlen(c) > 0) {
	s_doc << c << "\\\\\n";
      }
    }
    s_doc << end_tag(tag_descrip) << "\n";
  } else {
    s_doc << "\\\\\n";        // No description available, move to next line
  }
}

// --------------------------------------------------------------
// LATEX::print_text(DocEntry *de)
//
// Print out some text.  We use verbatim mode because of formatting
// problems.
// --------------------------------------------------------------

void LATEX::print_text(DocEntry *de) {
  char *c;
  c = de->text.get();
  if (strlen(c) > 0) {
    if (de->format) {
      s_doc << start_tag(tag_text) << "\n";
      s_doc << c;
      s_doc << end_tag(tag_text) << "\n\n";
    } else {
      s_doc << start_tag(tag_preformat) << "\n";
      s_doc << c;
      s_doc << end_tag(tag_preformat) << "\n\n";
    }
  }
}

void LATEX::title(DocEntry *de) { 
  char *c;

  c = de->usage.get();
  if (strlen(c) > 0) {
    s_doc << start_tag(tag_title) << " ";
    print_string(c,s_doc);
    s_doc << end_tag(tag_title) << "\\\\\n";
  }

  // Print out any C annotation and descriptive text 
  // Check to see if there any information available

  if ((strlen(de->cinfo.get()) && de->print_info) || strlen(de->text.get())) {
    
    // There is additional information now.   If we're in preformatting mode,
    // we need to handle things differently

    if (!de->format) {
      // Verbatim mode
      s_doc << start_tag(tag_preformat) << "\n";

      // If there is any C annotation, print that
      if (de->print_info) {
	c = de->cinfo.get();
	if (strlen(c) > 0) {
	  s_doc << "[ " << c << " ]\n";
	}
      }

      c = de->text.get();
      if (strlen(c) > 0) {
	s_doc << c;
      }
      s_doc << end_tag(tag_preformat) << "\n\n";
    } else {
      // We are in format mode now
      // We need to emit some stubs for the description format
      s_doc << start_tag(tag_text);

      // If there is any C annotation, print that
      if (de->print_info) {
	c = de->cinfo.get();
	if (strlen(c) > 0) {
	  s_doc << start_tag(tag_cinfo) << "[ ";
	  print_string(c,s_doc);
	  s_doc << " ] " << end_tag(tag_cinfo) << "\n";
	}
      }
      // Print out descriptive text (if any).
      c = de->text.get();
      if (strlen(c) > 0) {
	s_doc << c;
      }
      s_doc << end_tag(tag_text);
    }
  }
}

void LATEX::newsection(DocEntry *de,int sectnum) {
  char *c;
  char *tag;

  sect_num[sect_count] = sectnum;
  sect_count++;
  switch (sect_count) {
  case 1: /* Section */
    tag = tag_section;
    break;
  case 2: /* Subsection */
    tag = tag_subsection;
    break;
  default: /* subsubsection */
    tag = tag_subsubsection;
    break;
  }

  s_doc << start_tag(tag);
  c = de->usage.get();
  print_string(c,s_doc);
  s_doc << end_tag(tag);


  // Print out any C annotation and descriptive text 
  // Check to see if there any information available

  if ((strlen(de->cinfo.get()) && de->print_info) || strlen(de->text.get())) {
    
    // There is additional information now.   If we're in preformatting mode,
    // we need to handle things differently

    if (!de->format) {
      // Verbatim mode
      s_doc << start_tag(tag_preformat) << "\n";

      // If there is any C annotation, print that
      if (de->print_info) {
	c = de->cinfo.get();
	if (strlen(c) > 0) {
	  s_doc << "[ " << c << " ]\n";
	}
      }

      c = de->text.get();
      if (strlen(c) > 0) {
	s_doc << c;
      }
      s_doc << end_tag(tag_preformat) << "\n\n";
    } else {
      // We are in format mode now
      // We need to emit some stubs for the description format

      s_doc << start_tag(tag_text);
      // If there is any C annotation, print that
      if (de->print_info) {
	c = de->cinfo.get();
	if (strlen(c) > 0) {
	  s_doc << start_tag(tag_cinfo) << "[ ";
	  print_string(c,s_doc);
	  s_doc << " ] " << end_tag(tag_cinfo) << "\n";
	}
      }
      // Print out descriptive text (if any).
      c = de->text.get();
      if (strlen(c) > 0) {
	s_doc << c;
      }
      s_doc << end_tag(tag_text);
    }
  }
}


void LATEX::endsection() {
  if (sect_count > 0) sect_count--;
}

void LATEX::separator() {
}

void LATEX::init(char *filename) {
  char f[256];

  sprintf(f,"%s.tex",filename);
  sprintf(fn,"%s",filename);
  f_doc = fopen(f,"w");
  if (f_doc == NULL) {
    fprintf(stderr, "Unable to open %s\n", fn);
    SWIG_exit(1);
  }
}

void LATEX::close(void) {

  fprintf(f_doc,"\\documentstyle%s\n",tag_documentstyle);
  fprintf(f_doc,"\\setlength{\\parindent}{%s}\n",tag_parindent);
  fprintf(f_doc,"\\setlength{\\textwidth}{%s}\n",tag_textwidth);
  fprintf(f_doc,"\\setlength{\\oddsidemargin}{%s}\n",tag_oddsidemargin);
  fprintf(f_doc,"%s\n",tag_pagestyle);
  fprintf(f_doc,"\\begin{document}\n");
  fprintf(f_doc,"%s\n",s_doc.get());
  fprintf(f_doc,"\\end{document}\n");
  fclose(f_doc);
  if (Verbose) 
    fprintf(stderr,"Documentation written to %s.tex\n", fn);
}

// -------------------------------------------------------------------
// LATEX::style(char *name, char *value)
//
// Process style parameters
// -------------------------------------------------------------------

void LATEX::style(char *name, char *value) {
  if (strcmp(name,"latex_title") == 0) {
    if (value)
      tag_title = copy_string(value);
  } else if (strcmp(name,"latex_pagestyle") == 0) {
    if (value)
      tag_pagestyle = copy_string(value);
  } else if (strcmp(name,"latex_section") == 0) {
    if (value)
      tag_section = copy_string(value);
  } else if (strcmp(name,"latex_subsection") == 0) {
    if (value)
      tag_subsection = copy_string(value);
  } else if (strcmp(name,"latex_subsubsection") == 0) {
    if (value)
      tag_subsubsection = copy_string(value);
  } else if (strcmp(name,"latex_usage") == 0) {
    if (value)
      tag_usage = copy_string(value);
  } else if (strcmp(name,"latex_descrip") == 0) {
    if (value)
      tag_descrip = copy_string(value);
  } else if (strcmp(name,"latex_text") == 0) {
    if (value)
      tag_text = copy_string(value);
  } else if (strcmp(name,"latex_cinfo") == 0) {
    if (value)
      tag_cinfo = copy_string(value);
  } else if (strcmp(name,"latex_preformat") == 0) {
    if (value)
      tag_preformat = copy_string(value);
  } else if (strcmp(name,"latex_parindent") == 0) {
    if (value)
      tag_parindent = copy_string(value);
  } else if (strcmp(name,"latex_textwidth") == 0) {
    if (value)
      tag_textwidth = copy_string(value);
  } else if (strcmp(name,"latex_documentstyle") == 0) {
    if (value)
      tag_documentstyle = copy_string(value);
  } else if (strcmp(name,"latex_oddsidemargin") == 0) {
    if (value)
      tag_oddsidemargin = copy_string(value);
  }
}

// -------------------------------------------------------------------
// LATEX::parse_args(int argc, char **argv)
//
// Parse command line options 
// -------------------------------------------------------------------

static char *latex_usage = "\
LATEX Documentation Options (available with -dlatex)\n\
     None available.\n\n";

void LATEX::parse_args(int argc, char **argv) {
  int i;

  for (i = 0; i < argc; i++) {
    if (argv[i]) {
      if (strcmp(argv[i],"-help") == 0) {
	fputs(latex_usage,stderr);
      }
    }
  }
}



    
    
  






  
  
