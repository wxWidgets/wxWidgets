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
 * File : comment.cxx
 *
 * This is a semi-magical module for associating C/C++ comments with 
 * documentation entries.   While this sounds like it might be easy,
 * there are a number of subtle problems getting things to associate
 * correctly.
 *
 * Here's the general idea :
 *
 *      1.   The parser and scanner feed both C comments and documentation
 *           entries to this class.    These may show up in really bizarre
 *           orders (not necessarily the order seen in an interface file). 
 *
 *      2.   We maintain separate lists of comments and documentation 
 *           entries.
 *
 *      3.   Periodically, we go through the list of documentation entries
 *           and see if we can associate any comments.
 *
 *      4.   Upon completion of parsing, it's critical that we cleanup
 *           the lists using the cleanup() method.
 *
 *******************************************************************************/

// -----------------------------------------------------------------------------
// struct Comment
// 
// Structure used to maintain a linked list of comments for later use.
// -----------------------------------------------------------------------------

class Comment {
public:  
  String          *text;                   // Text of the comment
  int              first_line;             // First line of the comment
  int              last_line;              // Last line of the comment
  int              column;                 // First column of comment
  char            *file;                   // Name of the file that it was in
  Comment         *next;                   // Next comment (when in a linked list)
  Comment         *prev;                   // Previous comment
  static Comment  *comment_list;           // List of all comments

  Comment(char *t, int line, int col, char *f);
  ~Comment();
  static Comment *find(DocEntry *de, CommentHandler *ch);
  void attach(DocEntry *de, CommentHandler *ch);
};


// -----------------------------------------------------------------------
// Create a new comment.  Automatically puts it on the linked list
// -----------------------------------------------------------------------
Comment::Comment(char *t, int line, int col, char *f) {
    int   nlines = 0;
    char *c;

    text = new String(t);
    c = t;
    while (*c) {
      if (*c == '\n') nlines++;
      c++;
    }
    first_line = line;
    column = col;
    last_line = line + nlines - 1;
    file = copy_string(f);
    if (comment_list) {
      comment_list->prev = this;
    }
    next = comment_list;
    comment_list = this;
    prev = 0;
}

// -----------------------------------------------------------------------
// Destroy a comment
// -----------------------------------------------------------------------
Comment::~Comment() {
    delete text;
    delete file;
    // Remove from linked list (if applicable)
    if (prev) {
      prev->next = next;
    }
    if (next) {
      next->prev = prev;
    }
    if (this == comment_list) comment_list = next;
}
// -----------------------------------------------------------------------
// find(DocEntry *de, CommentHandler *ch)
// 
// This function tries to a find a comment matching the search criteria
// of a given comment handler and documentation entry.
// -----------------------------------------------------------------------
  
Comment *Comment::find(DocEntry *de, CommentHandler *ch) {
    Comment *c;

    c = comment_list;

    // Start walking down our list of stored comments

    while (c) {
      //      printf("Searching %x : %s\n", c, c->text->get());
      if (strcmp(de->file,c->file) == 0) {

	// At least comment is in the right file.  Now check line numbers

	if (ch->location == BEFORE) {
	  
	  // Check to see if the last line of the comment is close
	  // enough to our declaration.

	  if ((c->last_line <= de->line_number) && 
	      ((de->line_number - c->last_line) <= ch->skip_lines)) {
	    return c;
	  }
	} else {     // AFTER mode
	  // Check to see if the first line of the comment is close
	  // enough to our declaration.

	  if ((c->first_line >= de->end_line) &&
	      ((c->first_line - de->end_line) <= ch->skip_lines)) {
	    return c;
	  }
	}
	// Check to see if the line numbers are too small.  Comments
	// are processed in order so there's no sense in checking
	// all entries.

	if (c->last_line < de->line_number)
	  return 0;

      }
      c = c->next;
    }
    return 0;
}

// -----------------------------------------------------------------------
// void attach(DocEntry *de, CommentHandler *ch)
// 
// This function attachs a comment to a documentation entry and applies
// all of the style information in the comment handler.
// -----------------------------------------------------------------------
void Comment::attach(DocEntry *de, CommentHandler *ch) {
    int    nlines = 0;
    char   **split = 0;
    char   *c;
    int    i,lnum,el;
    if (!de) return;

    // If we're ignoring comments, forget it
    if (ch->ignore) {
      return;
    }

    // If the comment is formatted, no style processing is applied

    if (de->format) {
      de->text << *text;
      return;
    }

    // Untabify the comment

    if (ch->untabify) text->untabify(); 

    // Count how many lines we have

    c = text->get();
    while (*c) {
      if (*c == '\n') nlines++;
      c++;
    }

    if (nlines == 0) return;

    // Tokenize the documentation string into lines

    split = new char*[nlines+1];
    c = text->get();
    i = 0;
    split[i] = c;
    while (*c) {
      if (*c == '\n') {
	*(c++) = 0;
 	split[++i] = c;
      } else c++;
    }
    lnum = 0;

    // Now process the chop_top and chop_bottom values
    // if nlines < (chop_top + chop_bottom), then we do nothing

    if (nlines > (ch->chop_top + ch->chop_bottom)) {
      lnum += ch->chop_top;
      el = nlines-ch->chop_bottom;
    } else {
      el = nlines;
    }

    // Now process in-between lines

    while (lnum < el) {
      /* Chop line */
      if (split[lnum]) {
	if (strlen(split[lnum]) > (unsigned) (ch->chop_left+ch->chop_right)) {
	  if (ch->chop_right > 0) 
	    split[lnum][strlen(split[lnum]) - ch->chop_right] = 0;
	  de->text << &split[lnum][ch->chop_left];
	}
      }
      lnum++;
      de->text << "\n";
    }

    //    printf("*** ATTACHING %s : %s\n", de->usage.get(), de->text.get());
    delete split;
}


CommentHandler     *comment_handler = 0;
Comment            *Comment::comment_list = 0;

// ------------------------------------------------------------------------
// struct DocEntryList
//
// This structure manages a linked list of documentation entries that
// haven't had comments attached to them yet.
//
// As a general rule, this list tends to remain rather short.
// ------------------------------------------------------------------------

struct DocEntryList {
  DocEntry         *de;
  CommentHandler   *ch;
  DocEntryList     *next;
  DocEntryList     *prev;
  static DocEntryList *doc_list;
  
  // -----------------------------------------------------------------------
  // Create a new list entry
  // -----------------------------------------------------------------------
  DocEntryList(DocEntry *d, CommentHandler *c) {

    de = d;
    ch = c;
    next = doc_list;
    prev = 0;
    if (doc_list)
      doc_list->prev = this;
    doc_list = this;

    // Only allow a few doc entries to survive

    if (this->next) {
      if (this->next->next) {
	delete this->next->next;
      }
    }
  }

  // -----------------------------------------------------------------------
  // Destroy a list entry
  // -----------------------------------------------------------------------
  ~DocEntryList() {
    if (prev) {
      prev->next = next;
    }
    if (next) {
      next->prev = prev;
    }
    if (this == doc_list) doc_list = next;
  };
  
  // -----------------------------------------------------------------------
  // static check()
  //
  // Checks the list of documentation entries to see if any can be associated.
  // -----------------------------------------------------------------------

  static void check() {
    
    DocEntryList *dl, *dl_temp;
    Comment      *cmt;

    //    printf ("Checking\n");
    dl = doc_list;
    while (dl) {
      cmt = Comment::find(dl->de,dl->ch);
      if (cmt) {
	// Okay, we found a matching comment. Attach it to this
	// documentation entry.
	cmt->attach(dl->de,dl->ch);

	// Destroy the comment and doc list entry
	delete cmt;

	// Declarations are always coming in order so we're going
	// to blow away all of them past this point

	dl_temp = dl->next;
	delete dl;
	dl = dl_temp;
      } else {
	dl = dl->next;
      }
    }
  }
};


DocEntryList  *DocEntryList::doc_list = 0;

// -----------------------------------------------------------------------------
// CommentHandler::CommentHandler()
//
// Constructor.  Creates a new comment handler.  Sets up some default values
// for comment handling. 
// 
// Inputs : None
//
// Output : New CommentHandler object.
//
// Side Effects : Sets default comment handling parameters.
// -----------------------------------------------------------------------------

CommentHandler::CommentHandler() {
  skip_lines = 1;
  location = AFTER;
  chop_top = 0;
  chop_bottom = 0;
  chop_left = 3;
  chop_right = 0;
  untabify = 1;
  ignore = 0;
}

// -----------------------------------------------------------------------------
// CommentHandler::CommentHandler(CommentHandler *c) 
// 
// Constructor.  Creates a new comment handler, but copies attributes from 
// another handler.
//
// Inputs : 
//          c   = A different comment handler.
//
// Output : A new CommentHandler object.
//
// Side Effects : None
// -----------------------------------------------------------------------------

CommentHandler::CommentHandler(CommentHandler *c) {
  skip_lines = c->skip_lines;
  location = c->location;
  chop_top = c->chop_top;
  chop_bottom = c->chop_bottom;
  chop_left = c->chop_left;
  chop_right = c->chop_right; 
  untabify = c->untabify;
  ignore = c->ignore;
}

// -----------------------------------------------------------------------------
// CommentHandler::~CommentHandler()
// 
// Destructor.  Destroys a comment handler.  Does nothing interesting at the
// moment.
//
// Inputs : None
//
// Output : None
//
// Side Effects : None
// -----------------------------------------------------------------------------

CommentHandler::~CommentHandler() {
}

// -----------------------------------------------------------------------------
// void CommentHandler::add_comment(char *text, int line_num, int col, char *file)
//
// This function takes a character string as comment text and appends
// it to the current comment string (which is held in Comment::comment_list)
//
//     1.   If two comments appear in successive lines, they are
//          concatenated.   This is to handle C++ style comments like the
//          one surrounding this text.
//
//     2.   If a new comment appears, we simply create a new one
// 
// Inputs :
//          text     = Text of the comment
//          line_num = Starting line number of the comment
//          col      = Starting column of the comment
//          file     = File in which the comment was located.
//
// Output : None
//
// Side Effects :
//          Saves the comment in an internal linked list.
//          If multiple comments appear in succession, some may end up
//          in our comment list permanently (ie. never attached to any
//          particular declaration).
// -----------------------------------------------------------------------------

void CommentHandler::add_comment(char *text, int line_num, int col, char *file) {

  char *c;
  int  nlines = 0;
  Comment *cmt;

  //  printf("line_num = %d, %s\n", line_num,text);

  // Count up how many lines are in this comment

  c = text;
  while (*c) {
    if (*c == '\n') nlines++;
    c++;
  }

  // Check to see if this comment is in a successive line to the last one

  cmt = Comment::comment_list;
  
  if (cmt) {
    
    // Check for column alignment
    if ((cmt->column == col) && (line_num == (cmt->last_line + 1)) &&
	(nlines <= 1)) {
      *(cmt->text) << text;
      cmt->last_line = line_num + nlines - 1;
    } else {
    // This is a new comment, add it to our list
      cmt = new Comment(text,line_num,col,file);
    }
  } else {
    cmt = new Comment(text,line_num,col,file);
  }
}

// -----------------------------------------------------------------------------
// void CommentHanlder::set_entry(DocEntry *d)
//
// This grabs a DocEntry and hangs onto it.
//
// We will place the doc entry into our documentation list and then
// check it to see if any comments are sitting around.
// 
// Inputs : d = Documentation Entry
//
// Output : None
//
// Side Effects :
//          May attach comments to the documentation entry.  In this case,
//          comments and DocEntries may be removed from internal lists.
// -----------------------------------------------------------------------------

void CommentHandler::set_entry(DocEntry *d) {

  //  printf("Set entry : file: %s, line %d, %s\n", d->file, d->line_number, d->usage.get());

  // Create a new list entry and save it

  new DocEntryList(d,this);

  // Check all of the documentation entries to see if they can be placed

  DocEntryList::check();

}

// -----------------------------------------------------------------------------
// static void CommentHandler::cleanup() 
// 
// Checks all documentation entries and sees if there are any comments available.
// If so, they are attached.  This function is usually only called upon completion
// of parsing.
//
// Inputs : None
//
// Output : None
//
// Side Effects : 
//          Removes documentation entries and comments from internal lists.
//          
// -----------------------------------------------------------------------------

void CommentHandler::cleanup() {
  int nc, nd;
  Comment *c;
  DocEntryList *d;

  DocEntryList::check();

  // Figure out how bad we're doing on memory

  nc = 0;
  nd = 0;
  c = Comment::comment_list;
  while (c) {
    nc++;
    c = c->next;
  }
  
  d = DocEntryList::doc_list;
  while(d) {
    nd++;
    d = d->next;
  }

  if (Verbose) {
    printf("%d unprocessed comments, %d unprocessed doc entries.\n",nc,nd);
  }
}

// -----------------------------------------------------------------------------
// void CommentHandler::style(char *name, char *value)
// 
// Processes comment handling style parameters. The following parameters
// are available :
//
//          after           - Comments appear after a declaration
//          before          - Comments appear before a declaration
//          skip            - Number of blank lines between comment and decl.
//          chop_top        - Number of lines to chop from top of a comment
//          chop_bottom     - Number of lines to chop from bottom of a comment
//          chop_left       - Number of characters to chop from left
//          chop_right      - Number of characters to chop from right
//          tabify          - Leave tabs in comment text
//          untabify        - Strip tabs and convert them into spaces.
//          ignore          - Ignore comments
//          enable          - Enable comments
//
// Inputs :
//          name            - Name of style parameter
//          value           - Optional parameter value
//
// Output : None
//
// Side Effects : Changes style of comment handler object.
//           
// -----------------------------------------------------------------------------

void CommentHandler::style(char *name, char *value) {

  if (strcmp(name,"before") == 0) {
    location = BEFORE;
  } else if (strcmp(name,"after") == 0) {
    location = AFTER;
  } else if (strcmp(name,"skip") == 0) {
    if (value) 
      skip_lines = atoi(value);
  } else if (strcmp(name,"chop_top") == 0) {
    if (value) 
      chop_top = atoi(value);
  } else if (strcmp(name,"chop_bottom") == 0) {
    if (value) 
      chop_bottom = atoi(value);
  } else if (strcmp(name,"chop_left") == 0) {
    if (value) 
      chop_left = atoi(value);
  } else if (strcmp(name,"chop_right") == 0) {
    if (value) 
      chop_right = atoi(value);
  } else if (strcmp(name,"tabify") == 0) {
    untabify = 0;
  } else if (strcmp(name,"untabify") == 0) {
    untabify = 1;
  } else if (strcmp(name,"ignore") == 0) {
    ignore = 1;
  } else if (strcmp(name,"enable") == 0) {
    ignore = 0;
  }
}

// -----------------------------------------------------------------------------
// void CommentHandler::parse_args(int argc, char **argv)
//
// Function for processing command line options given on the SWIG command line.
// See the help string below for available options.
// 
// Inputs : 
//          argc = Argument count
//          argv = Argument strings
//
// Output : None
//
// Side Effects :
//          Changes various style parameters for the top-level CommentHandler.
// -----------------------------------------------------------------------------

static char *comment_usage = "\
Comment Style Options : \n\
     -Safter         - Use comments after a declaration.\n\
     -Sbefore        - Use comments before a declaration.\n\
     -Schop_bottom n - Chop n lines from bottom of comments.\n\
     -Schop_left n   - Chop n characters from left of a comment.\n\
     -Schop_right n  - Chop n characters from right of a comment.\n\
     -Schop_top n    - Chop n lines from top of comments.\n\
     -Signore        - Ignore comments.\n\
     -Sskip n        - Max lines between comment and declaration.\n\
     -Stabify        - Do not convert tabs.\n\
     -Suntabify      - Convert tabs into spaces (the default).\n\n";

void CommentHandler::parse_args(int argc, char **argv) {
  int i;

  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if (strcmp(argv[i],"-Sbefore") == 0) {
	    this->style("before",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Safter") == 0) {
	    this->style("after",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Schop_top") == 0) {
	    if (argv[i+1]) {
	      this->style("chop_top",argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-Schop_bottom") == 0) {
	    if (argv[i+1]) {
	      this->style("chop_bottom",argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-Schop_left") == 0) {
	    if (argv[i+1]) {
	      this->style("chop_left",argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-Schop_right") == 0) {
	    if (argv[i+1]) {
	      this->style("chop_right",argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-Sskip") == 0) {
	    if (argv[i+1]) {
	      this->style("skip",argv[i+1]);
	      mark_arg(i);
	      mark_arg(i+1);
	      i++;
	    } else {
	      arg_error();
	    }
	  } else if (strcmp(argv[i],"-Suntabify") == 0) {
	    this->style("untabify",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Stabify") == 0) {
	    this->style("tabify",0);
	    mark_arg(i);
	  } else if (strcmp(argv[i],"-Signore") == 0) {
	    this->style("ignore",0);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(comment_usage,stderr);
	  }
      }
  }
}
