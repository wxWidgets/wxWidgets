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
 * ascii.h
 *
 * ASCII specific functions for producing documentation.   Basically
 * prints things out as 80 column ASCII.
 ***********************************************************************/

class ASCII : public Documentation {
private:
  FILE  *f_doc;
  char  fn[256];
  void  print_string(char *s,int indent,int mode);
  int   indent;          // Indentation (for formatting)
  int   columns;         // Number of columns (for formatting)
  int   sect_count;      // Section counter
  int   sect_num[20];    // Section numbers
  // Style parameters
public:
  ASCII();
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


    
      



    
    
  






  
  
