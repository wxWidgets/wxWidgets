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
 * nodoc.h
 *
 * A null documentation header.  Does nothing.
 ***********************************************************************/

class NODOC : public Documentation {
private:
public:
  NODOC() { };
  void parse_args(int, char **) { };
  void title(DocEntry *) { };
  void newsection(DocEntry *, int) { };
  void endsection() { };
  void print_decl(DocEntry *) { };
  void print_text(DocEntry *) { };
  void separator() { };
  void init(char *) { };
  void close(void) { };
  void style(char *, char *) { };
};

    
      



    
    
  






  
  
