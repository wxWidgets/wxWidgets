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
 * html.h
 *
 * HTML specific functions for producing documentation.
 ***********************************************************************/

class HTML : public Documentation {
private:
  FILE  *f_doc;
  void  print_string(char *s, String &str, int mode);
  char *start_tag(char *);
  char *end_tag(char *);
  int   sect_count;
  int   sect_num[20];
  int   last_section;
  String  s_doc;
  String  s_title;
  String  contents;
  char  *tag_body;
  char  *tag_title;
  char  *tag_contents;
  char  *tag_section;
  char  *tag_subsection;
  char  *tag_subsubsection;
  char  *tag_usage;
  char  *tag_descrip;
  char  *tag_text;
  char  *tag_cinfo;
  char  *tag_preformat;
public:
  HTML();
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

    
      



    
    
  






  
  
