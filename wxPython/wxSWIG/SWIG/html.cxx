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
#include "html.h"

/*******************************************************************************
 * $Header$
 *
 * File : html.cxx
 *
 * HTML specific functions for producing documentation.
 *******************************************************************************/

#define PRE      0
#define FORMAT   1

// -----------------------------------------------------------------------------
// HTML::HTML()
// 
// Constructor.  Creates a new HTML documentation module object.
//
// Inputs : None
//
// Output : HTML Object
//
// Side Effects : None
// -----------------------------------------------------------------------------

HTML::HTML() {
  sect_count = 0;
  last_section = 0;
  
  // Initialize default tags for various parts of the documentation

  tag_body = "<BODY BGCOLOR=\"#ffffff\">:</BODY>";
  tag_title = "<H1>:</H1>";
  tag_contents = "<HR><H1>:</H1>";
  tag_section = "<HR><H2>:</H2>";
  tag_subsection = "<H3>:</H3>";
  tag_subsubsection = "<H4>:</H4>";
  tag_usage = "<P><TT><B>:</B></TT>";
  tag_descrip = "<BLOCKQUOTE>:</BLOCKQUOTE>";
  tag_text = "<P>";
  tag_cinfo = "";
  tag_preformat = "<PRE>:</PRE>";
}

// -----------------------------------------------------------------------------
// char *HTML::start_tag(char *tag)
// 
// Utility function for returning the first part of an HTML tag variable.
// A tag must look like this :
//
//          "<b>:</b>"
//
// The start tag for this would be "<b>"
//
// Inputs : tag   = HTML tag string
//
// Output : Staring portion of the tag variable.
//
// Side Effects : None.
// -----------------------------------------------------------------------------

char *HTML::start_tag(char *tag) {
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

// -----------------------------------------------------------------------------
// char *HTML::end_tag(char *tag)
// 
// Utility for returning an end-tag.  The counterpart to start_tag().
//
// Inputs : tag   = HTML tag string
//
// Output : Ending portion of tag variable.
//
// Side Effects : None
// -----------------------------------------------------------------------------

char *HTML::end_tag(char *tag) {
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

// -----------------------------------------------------------------------------
// void HTML::print_string(char *s, String &str, int mode)
// 
// Outputs the contents of string s into String str.   If mode is 1, we
// will reformat the text and apply a few common HTML character
// substitutions.
//
// Inputs :  s    =  Documentation text string
//           mode =  Formatting mode (0 = preformat, 1 = formatted)
//
// Output :  str  =  Output is append to str.
//
// Side Effects : None
// -----------------------------------------------------------------------------

void HTML::print_string(char *s, String &str,int mode) { 
  char *c;
  c = s;
  while (*c) {
    switch(*c) {
    case '\"':
      str << "&quot;";
      break;
    case '&':
      str << "&amp;";
      break;
    case '<':
      if (mode == PRE)
	str << "&lt;";
      else
	str << (char) *c;
      break;
    case '>':
      if (mode == PRE)
	str << "&gt;";
      else
	str << (char) *c; 
      break;
    default :
      str << (char ) *c;
      break;
    }
    c++;
  }
}

// -----------------------------------------------------------------------------
// void HTML::print_decl(DocEntry *de)
//
// Generates documentation for a declaration.
// 
// Inputs : de  = Documentation entry
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void HTML::print_decl(DocEntry *de) { 

  char *c;
  c = de->usage.get();
  while ((*c) && ((*c == ' ') || (*c == '\t') || (*c == '\n'))) c++;
  if (c) {
    s_doc << start_tag(tag_usage);
    print_string(c,s_doc,PRE);
    s_doc << end_tag(tag_usage) << "\n";
  } else return;

  // Only print this if there is information

  if ((strlen(de->cinfo.get()) && de->print_info) || strlen(de->text.get())) {
    s_doc << start_tag(tag_descrip);
    if (!de->format)
      s_doc << start_tag(tag_preformat);
  }

  // If there is any C annotation, print that
  if (de->print_info) {
    c = de->cinfo.get();
    if (strlen(c) > 0) {
      s_doc << start_tag(tag_cinfo);
      s_doc << "[ ";
      print_string(c,s_doc,PRE);
      s_doc << " ]" << end_tag(tag_cinfo) << "\n";
      if (de->format) s_doc << "<BR>";
    }
  }

  c = de->text.get();
  if (strlen(c) > 0) {
    print_string(c,s_doc,de->format);
  } 

  if ((strlen(de->cinfo.get()) && de->print_info) || strlen(de->text.get())) {
    if (!de->format) s_doc << end_tag(tag_preformat);
    s_doc << end_tag(tag_descrip) << "\n";
  }

  s_doc << "\n";
}

// -----------------------------------------------------------------------------
// void HTML::print_text(DocEntry *de)
// 
// Generates documentation for a text-block. Strips any leading whitespace.
//
// Inputs : de = Documentation entry
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void HTML::print_text(DocEntry *de) {
  char *c;
  c = de->text.get();
  if (strlen(c) > 0) {
    s_doc << start_tag(tag_text);
    if (!de->format)
      s_doc << start_tag(tag_preformat);
    print_string(c,s_doc,de->format);
    if (!de->format)
      s_doc << end_tag(tag_preformat) << "\n";
    s_doc << end_tag(tag_text) << "\n";
  }
}

// -----------------------------------------------------------------------------
// void HTML::title(DocEntry *de)
// 
// Generates the title for an HTML document.
//
// Inputs : de = Title documentation entry
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void HTML::title(DocEntry *de) { 
  char *c;
  c = de->usage.get();
  if (strlen(c) > 0) {
    s_title << "<HEAD>\n"
	  << "<TITLE>\n";
    print_string(c,s_title,PRE);
    s_title << "</TITLE>\n"
	  << start_tag(tag_body) << "\n";

    s_title << start_tag(tag_title);
    print_string(c,s_title,PRE);
    s_title << end_tag(tag_title) << "\n";
  }

  if (!de->format) 
    s_title << start_tag(tag_preformat);

  // If there is any C annotation, print that
  if (de->print_info) {
    c = de->cinfo.get();
    if (strlen(c) > 0) {
      s_title << start_tag(tag_cinfo) << "[ ";
      print_string(c,s_title,de->format);
      s_title << " ]" << end_tag(tag_cinfo);
      if (de->format)
	s_title << "<BR>\n";
      else
	s_title << "\n";
    }
  }

  c = de->text.get();
  if (strlen(c)) {
    print_string(c,s_title,de->format);
  }
  if (!de->format)
    s_title << end_tag(tag_preformat) << "\n";
}

// -----------------------------------------------------------------------------
// void HTML::newsection(DocEntry *de, int sectnum)
//
// Creates a new section.  sect_count is used to determine the formatting of
// the header.  Also fills in a table of contents
// 
// Inputs : 
//          de       = Documentation Entry
//          sectnum  = Section number
//
// Output : None
//
// Side Effects :
//          Creates a new subsection.  Updates HTML table of contents.
// -----------------------------------------------------------------------------

void HTML::newsection(DocEntry *de,int sectnum) {
  int i,f;
  char *c;

  char *tag;

  sect_num[sect_count] = sectnum;
  sect_count++;
  
  f = sect_count + 1;
  if (f > 5) f = 5;

  // Form table of contents
  // if sect_count > last_section.  We need to indent
  // if sect_count < last_section.  We need to pop out

  if (sect_count > last_section) {
    for (i = 0; i < sect_count - last_section; i++)
      contents << "<UL>";
  } else if (sect_count < last_section) {
    for (i = 0; i < last_section - sect_count; i++)
      contents << "</UL>";
  }

  last_section = sect_count;
  contents << "<LI> <A HREF=\"#s";

  s_doc << "<A name=\"s";
  for (i = 0; i < sect_count; i++) {
    s_doc << sect_num[i] << "_";
    contents << sect_num[i] << "_";
  }
  
  contents << "\">";
  
  // Figure out the tag fields

  switch(f) {
  case 1:
    tag = tag_title;
    break;
  case 2:
    tag = tag_section;
    break;
  case 3:
    tag = tag_subsection;
    break;
  case 4:
    tag = tag_subsubsection;
    break;
  default:
    tag = tag_subsubsection;
  }

  s_doc << "\">\n"
	<< start_tag(tag);

  for (i = 0; i < sect_count; i++) {
    s_doc << sect_num[i] << ".";
    contents << sect_num[i] << ".";
  }
  c = de->usage.get();
  s_doc << " ";
  contents << " ";
  print_string(c,s_doc,PRE);
  print_string(c,contents,PRE);
  s_doc << end_tag(tag) << "</A>\n";
  contents << "</A>\n";

  if (!de->format) 
    s_doc << start_tag(tag_preformat);

  // If there is any C annotation, print that
  if (de->print_info) {
    c = de->cinfo.get();
    if (strlen(c) > 0) {
      s_doc << start_tag(tag_cinfo) << "[ ";
      print_string(c,s_doc,de->format);
      s_doc << " ]" << end_tag(tag_cinfo);
      if (de->format)
	s_doc << "<BR>\n";
      else
	s_doc << "\n";
    }
  }

  // If there is a description text. Print it

  c = de->text.get();
  if (strlen(c) > 0) {
    print_string(c,s_doc,de->format);
    s_doc << "\n";
  }
  if (!de->format)
    s_doc << end_tag(tag_preformat) << "\n";
  
}

// -----------------------------------------------------------------------------
// void HTML::endsection()
//
// Ends a subsection.  It is an error to call this without first calling
// newsection previously.
// 
// Inputs : None
//
// Output : None
//
// Side Effects : Closes current section and goes back to parent.
//          
// -----------------------------------------------------------------------------

void HTML::endsection() {
  if (sect_count > 0) sect_count--;
}

// -----------------------------------------------------------------------------
// void HTML::separator()
// 
// Prints a separator after the declaration of a C++ class.  Currently
// does nothing in HTML mode.
//
// Inputs : None
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

void HTML::separator() {
}

// -----------------------------------------------------------------------------
// void HTML::init(char *filename)
//
// Initializes the HTML module and opens up the documentation file.
// 
// Inputs : filename  = Name of documentation file (without a suffix)
//
// Output : None
//
// Side Effects : Opens documentation file.
// -----------------------------------------------------------------------------

void HTML::init(char *filename) {
  char f[256];

  sprintf(f,"%s.html",filename);
  f_doc = fopen(f,"w");
  if (f_doc == NULL) {
    fprintf(stderr,"Unable to open %s\n",f);
    SWIG_exit(1);
  }
  /* Print a HTML banner */
  fprintf(f_doc,"<HTML>\n");

}

// -----------------------------------------------------------------------------
// void HTML::close(void)
//
// Dumps the table of contents and forms the final documentation file. Closes
// the documentation file upon completion.
// 
// Inputs : None
//
// Output : None
//
// Side Effects : Closes documentation file.
// -----------------------------------------------------------------------------

void HTML::close(void) {

  int i;
  for (i = 0; i < last_section; i++)
    contents << "</UL>\n";

  fprintf(f_doc,"%s\n",s_title.get());
  if (last_section) {
    fprintf(f_doc,"%s Contents %s\n",start_tag(tag_contents),end_tag(tag_contents));
    fprintf(f_doc,"%s\n",contents.get());
  }
  fprintf(f_doc,"%s\n",s_doc.get());
  fprintf(f_doc,"%s\n", end_tag(tag_body));
  fprintf(f_doc,"</HTML>\n");
  fclose(f_doc);
}

// -----------------------------------------------------------------------------
// void HTML::style(char *name, char *value)
// 
// Process parameters given with the %style directive.  Does nothing if an
// unrecognized parameter is given.
//
// Inputs :
//          name   = name of style parameter
//          value  = ASCII string with value of parameter.
//
// Output : None
//
// Side Effects : Updates internal style parameters.
// -----------------------------------------------------------------------------

void HTML::style(char *name, char *value) {
  if (strcmp(name,"html_title") == 0) {
    if (value)
      tag_title = copy_string(value);
  } else if (strcmp(name,"html_contents") == 0) {
    if (value)
      tag_contents = copy_string(value);
  } else if (strcmp(name,"html_section") == 0) {
    if (value)
      tag_section = copy_string(value);
  } else if (strcmp(name,"html_subsection") == 0) {
    if (value)
      tag_subsection = copy_string(value);
  } else if (strcmp(name,"html_subsubsection") == 0) {
    if (value)
      tag_subsubsection = copy_string(value);
  } else if (strcmp(name,"html_usage") == 0) {
    if (value)
      tag_usage = copy_string(value);
  } else if (strcmp(name,"html_descrip") == 0) {
    if (value)
      tag_descrip = copy_string(value);
  } else if (strcmp(name,"html_text") == 0) {
    if (value)
      tag_text = copy_string(value);
  } else if (strcmp(name,"html_cinfo") == 0) {
    if (value)
      tag_cinfo = copy_string(value);
  } else if (strcmp(name,"html_preformat") == 0) {
    if (value)
      tag_preformat = copy_string(value);
  } else if (strcmp(name,"html_body") == 0) {
    if (value)
      tag_body = copy_string(value);
  }
}

// -----------------------------------------------------------------------------
// void HTML::parse_args(int argc, char **argv)
// 
// Parse command line options given on the SWIG command line.
//
// Inputs : 
//          argc     = argument count
//          argv     = argument array
//
// Output : None
//
// Side Effects : Marks arguments as being parsed.
// -----------------------------------------------------------------------------

static char *html_usage = "\
HTML Documentation Options (available with -dhtml)\n\
     None available.\n\n";
 
void HTML::parse_args(int argc, char **argv) {
  int i;

  for (i = 0; i < argc; i++) {
    if (argv[i]) {
      if (strcmp(argv[i],"-help") == 0) {
	fputs(html_usage,stderr);
      }
    }
  }
}

    
    
  






  
  
