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
 * latex.h
 *
 * Latex specific functions for producing documentation.
 ***********************************************************************/
class LATEX : public Documentation {
private:
  FILE  *f_doc;
  String s_doc;
  char  fn[256];
  char *start_tag(char *);
  char *end_tag(char *);
  void  print_string(char *s, String &str);
  int   sect_count;      // Section counter
  int   sect_num[20];    // Section numbers
  // Style parameters

  char  *tag_parindent;
  char  *tag_textwidth;
  char  *tag_documentstyle;
  char  *tag_oddsidemargin;
  char  *tag_title;
  char  *tag_preformat;
  char  *tag_usage;
  char  *tag_descrip;
  char  *tag_text;
  char  *tag_cinfo;
  char  *tag_pagestyle;
  char  *tag_section;
  char  *tag_subsection;
  char  *tag_subsubsection;

public:
  LATEX();
  void parse_args(int argc, char **argv);
  void title(DocEntry *de);
  void newsection(DocEntry *de, int sectnum);
  void endsection();
  void print_decl(DocEntry *de);
  void print_text(DocEntry *de);
  void separator();
  void init(char *filename);
  void close(void);
  void style(char *name, char *value);
};


       
      



    
    
  






  
  
