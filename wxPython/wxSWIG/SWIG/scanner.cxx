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

/*************************************************************************
 * $Header$
 * scanner.c
 *
 * Dave Beazley
 * January 1996
 *
 * Input scanner.   This scanner finds and returns tokens
 * for the wrapper generator.   Since using lex/flex from
 * C++ is so F'ed up, I've written this function to replace
 * them entirely.     It's not as fast, but hopefully it will
 * eliminate alot of compilation problems.
 *
 *************************************************************************/


#include "internal.h"
#include "parser.h"
#include <string.h>
#include <ctype.h>

#define  YYBSIZE  8192

struct InFile {
  FILE   *f;
  int    line_number;
  char  *in_file;
  int    extern_mode;	
  int    force_extern;
  struct InFile *prev;
};

// This structure is used for managing code fragments as
// might be used by the %inline directive and handling of
// nested structures.

struct CodeFragment {
  char          *text;
  int            line_number;
  CodeFragment  *next;
};

InFile  *in_head;

FILE    *LEX_in = NULL;

static String         header;
static String         comment;
       String         CCode;              // String containing C code
static char           *yybuffer;
static int            lex_pos = 0;
static int            lex_len = 0;         
static char           *inline_yybuffer = 0;
static int            inline_lex_pos = 0;
static int            inline_lex_len = 0;
static int            inline_line_number = 0;
static CodeFragment  *fragments = 0;      // Code fragments

static 
char           yytext[YYBSIZE];
static int     yylen = 0;
int            line_number = 1;
int            column = 1;
int            column_start = 1;
char          *input_file;
int            start_line = 0;
static  int    comment_start;
static  int    scan_init  = 0;
static  int    num_brace = 0;
static  int    last_brace = 0;
static  int    in_define = 0;
static  int    define_first_id = 0;   /* Set when looking for first identifier of a define */
extern  int    Error;


/**************************************************************
 * scanner_init()
 *
 * Initialize buffers
 **************************************************************/

void scanner_init() {

  yybuffer = (char *) malloc(YYBSIZE);
  scan_init = 1;
}

/**************************************************************
 * scanner_file(FILE *f)
 *
 * Start reading from new file 
 **************************************************************/
void scanner_file(FILE *f) {
  InFile *in;

  in = new InFile;
  in->f = f;
  in->in_file = input_file;
  in->extern_mode = WrapExtern;	
  in->force_extern = ForceExtern;
  if (in_head) in_head->line_number = line_number+1;
  if (!in_head) in->prev = 0;
  else in->prev = in_head;
  in_head = in;
  LEX_in = f;
  line_number = 1;
}

/**************************************************************
 * scanner_close()
 *
 * Close current input file and go to next 
 **************************************************************/

void scanner_close() {

  InFile *p;
  static int lib_insert = 0;
  fclose(LEX_in);
  if (!in_head) return;
  p = in_head->prev;
  if (p != 0) {
    LEX_in = p->f;
    line_number = p->line_number;
    input_file = p->in_file;
    WrapExtern = p->extern_mode;
    if (!WrapExtern) remove_symbol("SWIGEXTERN");
    ForceExtern = p->force_extern;
  } else {
    LEX_in = NULL;
  }
  delete in_head;
  in_head = p;

  // if LEX_in is NULL it means we're done with the interface file. We're now
  // going to grab all of the library files.

  if ((!LEX_in) && (!lib_insert)) {
    library_insert();
    lib_insert = 1;
  }

}

/**************************************************************
 * char nextchar()
 *
 * gets next character from input.
 * If we're in inlining mode, we actually retrieve a character
 * from inline_yybuffer instead.
 **************************************************************/

char nextchar() {

    char c = 0;

    if (Inline) {
      if (inline_lex_pos >= inline_lex_len) {
	// Done with inlined code.  Check to see if we have any
	// new code fragments.  If so, switch to them.
	delete inline_yybuffer;
	if (fragments) {
	  CodeFragment *f;
	  inline_yybuffer = fragments->text;
	  inline_lex_pos = 1;
	  inline_lex_len = strlen(fragments->text);
	  line_number = fragments->line_number;
	  f = fragments->next;
	  delete fragments;
	  fragments = f;
	  c = inline_yybuffer[0];
	} else {
	  c = 0;
	  Inline = 0;
	  line_number = inline_line_number;       // Restore old line number
	}
      } else {
	inline_lex_pos++;
	c = inline_yybuffer[inline_lex_pos-1];
      }
    }
    if (!Inline) {
      if (lex_pos >= lex_len) {
	if (!LEX_in) {
	  SWIG_exit(1);
	}
	while(fgets(yybuffer, YYBSIZE, LEX_in) == NULL) {
	  scanner_close();        // Close current input file
	  if (!LEX_in) return 0;  // No more, we're outta here
	}
	lex_len = strlen(yybuffer);
	lex_pos = 0;
      }
      
      lex_pos++;
      c = yybuffer[lex_pos-1];
    }
      
    if (yylen >= YYBSIZE) {
      fprintf(stderr,"** FATAL ERROR.  Buffer overflow in scanner.cxx.\nReport this to swig@cs.utah.edu.\n");
      SWIG_exit(1);
    }
    yytext[yylen] = c;
    yylen++;
    if (c == '\n') {
      line_number++;
      column = 1;
    } else {
      column++;
    }
    return(c);
}

void retract(int n) {
  int i, j, c;
  
  for (i = 0; i < n; i++) {
    if (Inline) {
      inline_lex_pos--;
      if (inline_lex_pos < 0) {
	fprintf(stderr,"Internal scanner error. inline_lex_pos < 0\n");
	SWIG_exit(1);
      }
    }
    else lex_pos--;
    yylen--;
    column--;
    if (yylen >= 0) {
      if (yytext[yylen] == '\n') {
	line_number--;
	// Figure out what column we're in
	c = yylen-1;
	j = 1;
	while (c >= 0){
	  if (yytext[c] == '\n') break;
	  j++;
	  c--;
	}
	column = j;
      }
    }
  }
  if (yylen < 0) yylen = 0;
}

/**************************************************************
 * start_inline(char *text, int line)
 * 
 * This grabs a chunk of text and tries to inline it into
 * the current file.  (This is kind of wild, but cool when
 * it works).
 *
 * If we're already in inlining mode, we will save the code
 * as a new fragment.
 **************************************************************/

void start_inline(char *text, int line) {

  if (Inline) {

    // Already processing a code fragment, simply hang on
    // to this one for later.

    CodeFragment *f,*f1;

    // Add a new code fragment to our list
    f = new CodeFragment;
    f->text = copy_string(text);
    f->line_number = line;
    f->next = 0;
    if (!fragments) fragments = f;
    else {
      f1 = fragments;
      while (f1->next) f1 = f1->next;
      f1->next = f;
    }
  } else {

    // Switch our scanner over to process text from a string.
    // Save current line number and other information however.

    inline_yybuffer = copy_string(text);
    inline_lex_len = strlen(text);
    inline_lex_pos = 0;
    inline_line_number = line_number;       // Make copy of old line number
    line_number = line;
    Inline = 1;
  }
}

/**************************************************************
 * yycomment(char *, int line)
 *
 * Inserts a comment into a documentation entry.    
 **************************************************************/

void yycomment(char *s, int line, int col) {
  comment_handler->add_comment(s,line,col,input_file);
}

/**************************************************************
 * void skip_brace(void)
 *
 * Found a {.
 * Skip all characters until we find a matching closed }.
 *
 * This function is used to skip over inlined C code and other
 * garbage found in interface files.
 ***************************************************************/

void skip_brace(void) {

  char c;
  CCode = "{";
  while (num_brace > last_brace) {
    if ((c = nextchar()) == 0) {
      fprintf(stderr,"%s : Line %d.  Missing '}'. Reached end of input.\n",
	      input_file, line_number);
      FatalError();
      return;
    }
    CCode << c;
    if (c == '{') num_brace++;
    if (c == '}') num_brace--;
    yylen = 0;
  }
}


/**************************************************************
 * void skip_template(void)
 *
 * Found a <.
 * Skip all characters until we find a matching closed >.
 *
 * This function is used to skip over C++ templated types
 * and objective-C protocols.
 ***************************************************************/

void skip_template(void) {

  char c;
  CCode = "<";
  int  num_lt = 1;
  while (num_lt > 0) {
    if ((c = nextchar()) == 0) {
      fprintf(stderr,"%s : Line %d.  Missing '>'. Reached end of input.\n",
	      input_file, line_number);
      FatalError();
      return;
    }
    CCode << c;
    if (c == '<') num_lt++;
    if (c == '>') num_lt--;
    yylen = 0;
  }
}

/**************************************************************
 * void skip_to_end(void)
 *
 * Skips to the @end directive in a Objective-C definition
 **************************************************************/

void skip_to_end(void) {
  char c;
  int  state = 0;
  yylen = 0;
  while ((c = nextchar())){
    switch(state) {
    case 0:
      if (c == '@') state = 1;
      else yylen = 0;
      break;
    case 1:
      if (isspace(c)) {
	if (strncmp(yytext,"@end",4) == 0) return; 
	else {
	  yylen = 0;
	  state = 0;
	}
      } else {
	state = 1;
      }
      break;
    }
  }
  fprintf(stderr,"%s : EOF. Missing @end. Reached end of input.\n",
	  input_file);
  FatalError();
  return;
}  

/**************************************************************
 * void skip_decl(void)
 *
 * This tries to skip over an entire declaration.   For example
 *
 *  friend ostream& operator<<(ostream&, const char *s);
 * 
 * or 
 *  friend ostream& operator<<(ostream&, const char *s) { };
 *
 **************************************************************/

void skip_decl(void) {
  char c;
  int  done = 0;
  while (!done) {
    if ((c = nextchar()) == 0) {
      fprintf(stderr,"%s : Line %d.  Missing semicolon. Reached end of input.\n",
	      input_file, line_number);
      FatalError();
      return;
    }
    if (c == '{') {
      last_brace = num_brace;
      num_brace++;
      break;
    }
    yylen = 0;
    if (c == ';') done = 1;
  }
  if (!done) {
    while (num_brace > last_brace) {
      if ((c = nextchar()) == 0) {
	fprintf(stderr,"%s : Line %d.  Missing '}'. Reached end of input.\n",
		input_file, line_number);
	FatalError();
	return;
      }
      if (c == '{') num_brace++;
      if (c == '}') num_brace--;
      yylen = 0;
    }
  }
}

/**************************************************************
 * void skip_define(void)
 *
 * Skips to the end of a #define statement.
 *
 **************************************************************/

void skip_define(void) {
  char c;
  while (in_define) {
    if ((c = nextchar()) == 0) return;
    if (c == '\\') in_define = 2;
    if (c == '\n') {
      if (in_define == 2) {
	in_define = 1;
      } else if (in_define == 1) {
	in_define = 0;
      }
    }
    yylen = 0;
  }
}

/**************************************************************
 * int skip_cond(int inthen)
 *
 * Skips the false portion of an #ifdef directive.   Looks
 * for either a matching #else or #endif 
 *
 * inthen is 0 or 1 depending on whether or not we're
 * handling the "then" or "else" part of a conditional.
 *
 * Returns 1 if the else part of the #if-#endif block was
 * reached.  Returns 0 otherwise.  Returns -1 on error.
 **************************************************************/

int skip_cond(int inthen) {
  int    level = 0;         /* Used to handled nested if-then-else */
  int    state = 0;
  char   c;
  int    start_line;
  char  *file;
  
  file = copy_string(input_file);
  start_line = line_number;
  yylen = 0;

  while(1) {
    switch(state) {
    case 0 :
      if ((c = nextchar()) == 0) {
	fprintf(stderr,"%s : Line %d.  Unterminated #if-else directive.\n", file, start_line);
	FatalError();
	return -1;     /* Error */
      }
      if ((c == '#') || (c == '%')) {
	state = 1;
      } else if (isspace(c)) {
	yylen =0;
	state = 0;
      } else {
	/* Some non-whitespace character. Look for line end */
	yylen = 0;
	state = 3;
      }
      break;
    case 1:
      /* Beginning of a C preprocessor statement */
      if ((c = nextchar()) == 0) {
	fprintf(stderr,"%s : Line %d.  Unterminated #if-else directive.\n", file, start_line);
	FatalError();
	return -1;     /* Error */
      }
      if (c == '\n') {
	state = 0;
	yylen = 0;
      }
      else if (isspace(c)) {
	state = 1;
	yylen--;
      } else {
	state = 2;
      }
      break;
    case 2:
      /* CPP directive */
      if ((c = nextchar()) == 0)  {
	fprintf(stderr,"%s : Line %d.  Unterminated #if-else directive.\n", file, start_line);
	FatalError();
	return -1;     /* Error */
      }
      if ((c == ' ') || (c == '\t') || (c=='\n')) {
	yytext[yylen-1] = 0;
	if ((strcmp(yytext,"#ifdef") == 0) || (strcmp(yytext,"%ifdef") == 0)) {
	  level++;
	  state = 0;
	} else if ((strcmp(yytext,"#ifndef") == 0) || (strcmp(yytext,"%ifndef") == 0)) {
	  level++;
	  state = 0;
	} else if ((strcmp(yytext,"#if") == 0) || (strcmp(yytext,"%if") == 0)) {
	  level++;
	  state = 0;
	} else if ((strcmp(yytext,"#else") == 0) || (strcmp(yytext,"%else") == 0)) {
	  if (level == 0) {    /* Found matching else.  exit */
	    if (!inthen) {
	      /* Hmmm.  We've got an "extra #else" directive here */
	      fprintf(stderr,"%s : Line %d.  Misplaced #else.\n", input_file, line_number);
	      FatalError();
	      yylen = 0;
	      state = 0;
	    } else {
	      yylen = 0;
	      delete file;
	      return 1;
	    }
	  } else {
	    yylen = 0;
	    state = 0;
	  }
	} else if ((strcmp(yytext,"#endif") == 0) || (strcmp(yytext,"%endif") == 0)) {
	  if (level <= 0) {    /* Found matching endif. exit */
	    yylen = 0;
	    delete file;
	    return 0;
	  } else {
	    state = 0;
	    yylen = 0;
	    level--;
	  }
	} else if ((strcmp(yytext,"#elif") == 0) || (strcmp(yytext,"%elif") == 0)) {
	  if (level <= 0) {
	    // If we come across this, we pop it back onto the input queue and return
	    retract(6);
	    delete file;
	    return 0;
	  } else {
	    yylen = 0;
	    state = 0;
	  }
	} else {
	  yylen = 0;
	  state = 0;
	}
      }
      break;
    case 3:
      /* Non-white space.  Look for line break */
      if ((c = nextchar()) == 0) {
	fprintf(stderr,"%s : Line %d.  Unterminated #if directive.\n", file, start_line);
	FatalError();
	return -1;     /* Error */
      }
      if (c == '\n') {
	yylen = 0;
	state = 0;
      } else {
	yylen = 0;
	state = 3;
      }
      break;
    }
  }
}
      
/**************************************************************
 * int yylook()
 *
 * Lexical scanner.
 * See Aho,Sethi, and Ullman,  pg. 106
 **************************************************************/

int yylook(void) {

    int      state;
    char     c = 0;

    state = 0;
    yylen = 0;
    while(1) {

/*	printf("State = %d\n", state);   */
	switch(state) {

	case 0 :
	  if((c = nextchar()) == 0) return(0);

	  /* Process delimeters */

	  if (c == '\n') {
	    state = 0;
	    yylen = 0;
	    if (in_define == 1) {
	      in_define = 0;
	      return(ENDDEF);
	    } else if (in_define == 2) {
	      in_define = 1;
	    }
	  } else if (isspace(c)) {
	    state = 0;
	    yylen = 0;
	  }
	      
	  /* Look for single character symbols */
	      
	  else if (c == '(') return (LPAREN);
	  else if (c == ')') return (RPAREN);
	  else if (c == ';') return (SEMI);
	  else if (c == ',') return (COMMA);
	  else if (c == '*') return (STAR);
	  else if (c == '}') {
	    num_brace--;
	    if (num_brace < 0) {
	      fprintf(stderr,"%s : Line %d. Error. Extraneous '}' (Ignored)\n",
		      input_file, line_number);
	      state = 0;
	      num_brace = 0;
	    } else {
	      return (RBRACE);
	    }
	  }
	  else if (c == '{') {
	    last_brace = num_brace;
	    num_brace++;
	    return (LBRACE);
	  }
	  else if (c == '=') return (EQUAL);
	  else if (c == '+') return (PLUS);
          else if (c == '-') return (MINUS);
	  else if (c == '&') return (AND);
	  else if (c == '|') return (OR);
	  else if (c == '^') return (XOR);
          else if (c == '<') state = 60;
	  else if (c == '>') state = 61;
	  else if (c == '~') return (NOT);
          else if (c == '!') return (LNOT);	                  
	  else if (c == '\\') {
	    if (in_define == 1) {
	      in_define = 2;
	      state = 0;
	    } else 
	      state = 99;
	  }
  	  else if (c == '[') return (LBRACKET);
	  else if (c == ']') return (RBRACKET);

	  /* Look for multi-character sequences */
	  
	  else if (c == '/') state = 1;    // Comment (maybe) 
	  else if (c == '\"') state = 2;   // Possibly a string
	  else if (c == '#') state = 3;    // CPP
	  else if (c == '%') state = 4;    // Directive
	  else if (c == '@') state = 4;    // Objective C keyword
	  else if (c == ':') state = 5;    // maybe double colon
	  else if (c == '0') state = 83;   // An octal or hex value
	  else if (c == '\'') state = 9;   // A character constant
	  else if (c == '.') state = 100;  // Maybe a number, maybe just a period
	  else if (isdigit(c)) state = 8;  // A numerical value
	  else if ((isalpha(c)) || (c == '_') || (c == '$')) state = 7;
	  else state = 99;
	  break;
	case 1:  /*  Comment block */
	  if ((c = nextchar()) == 0) return(0);
	  if (c == '/') {
	    comment_start = line_number;
	    column_start = column;
	    comment = "  ";
	    state = 10;        // C++ style comment
	  } else if (c == '*') {
	    comment_start = line_number;
	    column_start = column;
	    comment = "  ";
	    state = 11;   // C style comment
	  } else {
	    retract(1);
	    return(SLASH);
	  }
	  break;
	case 10:  /* C++ style comment */
	  if ((c = nextchar()) == 0) {
	    fprintf(stderr,"%s : EOF. Unterminated comment detected.\n",input_file);
	    FatalError();
	    return 0;
	  }
	  if (c == '\n') {
	    comment << c;
	    // Add the comment to documentation
	    yycomment(comment.get(),comment_start, column_start);
	    yylen = 0;
	    state = 0;
	    if (in_define == 1) {
	      in_define = 0;
	      return(ENDDEF);
	    }
	  } else {
	    state = 10;
	    comment << c;
	    yylen = 0;
	  }
	  break;
	case 11: /* C style comment block */
	  if ((c = nextchar()) == 0) {
	    fprintf(stderr,"%s : EOF. Unterminated comment detected.\n", input_file);
	    FatalError();
	    return 0;
	  }
	  if (c == '*') {
	    state = 12;
	  } else {
	    comment << c;
	    yylen = 0;
	    state = 11;
	  }
	  break;
	case 12: /* Still in C style comment */
	  if ((c = nextchar()) == 0) {
	    fprintf(stderr,"%s : EOF. Unterminated comment detected.\n", input_file);
	    FatalError();
	    return 0;
	  }
	  if (c == '*') {
	    comment << c;
	    state = 12;
	  } else if (c == '/') {
	    comment << "  \n";
	    yycomment(comment.get(),comment_start,column_start);
	    yylen = 0;
	    state = 0;
	  } else {
	    comment << '*' << c;
	    yylen = 0;
	    state = 11;
	  }
	  break;

	case 2: /* Processing a string */
	  if ((c = nextchar()) == 0) {
	    fprintf(stderr,"%s : EOF. Unterminated string detected.\n", input_file);
	    FatalError();
	    return 0;
	  }
	  if (c == '\"') {
	    yytext[yylen-1] = 0;
	    yylval.id = copy_string(yytext+1);
	    return(STRING);
	  } else if (c == '\\') {
	    state = 21;             /* Possibly an escape sequence. */
	    break;
	  } else state = 2;
	  break;
	case 21: /* An escape sequence. get next character, then go
		    back to processing strings */

	  if ((c = nextchar()) == 0) return 0;
	  state = 2;
	  break;

	case 3: /* a CPP directive */

	  if (( c= nextchar()) == 0) return 0;
	  if (c == '\n') {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = yytext;
	    return(POUND);
	  } else if ((c == ' ') || (c == '\t')) {  // Ignore white space after # symbol
	    yytext[yylen] = 0;
	    yylen--;
	    state = 3;
	  } else {
	    yytext[yylen] = 0;
	    state = 31;
	  }
	  break;
	case 31:
	  if ((c = nextchar()) == 0) return 0;
	  if ((c == ' ') || (c == '\t') || (c=='\n')) {
	    retract(1);
	    yytext[yylen] = 0;
	    if (strcmp(yytext,"#define") == 0) {
	      in_define = 1;
	      define_first_id = 1;
	      return(DEFINE);
	    } else if (strcmp(yytext,"#ifdef") == 0) {
	      return(IFDEF);
	    } else if (strcmp(yytext,"#ifndef") == 0) {
	      return(IFNDEF);
	    } else if (strcmp(yytext,"#else") == 0) {
	      return(ELSE);
	    } else if (strcmp(yytext,"#endif") == 0) {
	      return(ENDIF);
	    } else if (strcmp(yytext,"#undef") == 0) {
	      return(UNDEF);
	    } else if (strcmp(yytext,"#if") == 0) {
	      return(IF);
	    } else if (strcmp(yytext,"#elif") == 0) {
	      return(ELIF);
	    } else {
	      /* Some kind of "unknown CPP directive.  Skip to end of the line */
	      state = 32;
	    }
	  }
	  break;
	case 32:
	  if ((c = nextchar()) == 0) return 0;
	  if (c == '\n') {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = yytext;
	    return(POUND);
	  }
	  state = 32;
	  break;

	case 4: /* A wrapper generator directive (maybe) */
	  if (( c= nextchar()) == 0) return 0;
	  if (c == '{') {
	    state = 40;   /* Include block */
	    header = "";
	    start_line = line_number;
	  }
	  else if ((isalpha(c)) || (c == '_')) state = 7;
	  else {
	    retract(1);
	    state = 99;
	  }
	  break;
	  
	case 40: /* Process an include block */
	  if ((c = nextchar()) == 0) {
	    fprintf(stderr,"%s : EOF. Unterminated include block detected.\n", input_file);
	    FatalError();
	    return 0;
	  }
	  yylen = 0;
	  if (c == '%') state = 41;
	  else {
	    header << c;
	    yylen = 0;
	    state = 40;
	  }
	  break;
	case 41: /* Still processing include block */
	  if ((c = nextchar()) == 0) {
	    fprintf(stderr,"%s : EOF. Unterminated include block detected.\n", input_file);
	    FatalError();
	    return 0;
	  }
	  if (c == '}') {
	    yylval.id = header.get();
	    return(HBLOCK);
	  } else {
	    header << '%';
	    header << c;
	    yylen = 0;
	    state = 40;
	  }
	  break;

	case 5: /* Maybe a double colon */

	  if (( c= nextchar()) == 0) return 0;
	  if ( c == ':') return DCOLON;
	  else {
	    retract(1);
	    return COLON;
	  }


	case 60: /* shift operators */
	  if ((c = nextchar()) == 0) return (0);
	  if (c == '<') return LSHIFT;
	  else {
	    retract(1);
	    return LESSTHAN;
	  }
	  break;
	case 61: 
	  if ((c = nextchar()) == 0) return (0);
	  if (c == '>') return RSHIFT;
	  else {
	    retract(1);
            return GREATERTHAN;
	  }
	  break;
	case 7: /* Identifier */
	  if ((c = nextchar()) == 0) return(0);
	  if (isalnum(c) || (c == '_') || (c == '.') || (c == '$'))
	    //              || (c == '.') || (c == '-'))
	    state = 7;
	  else if (c == '(') {
	    /* We might just be in a CPP macro definition.  Better check */
	    if ((in_define) && (define_first_id)) {
	      /* Yep.  We're going to ignore the rest of it */
	      skip_define();
	      define_first_id = 0;
	      return (MACRO);
	    } else {
	      retract(1);
	      define_first_id = 0;
	      return(ID);
	    }
	  } else {
	    retract(1);
	    define_first_id = 0;
	    return(ID);
	  }
	  break;
	case 8: /* A numerical digit */
	  if ((c = nextchar()) == 0) return(0);
	  if (c == '.') {state = 81;}
	  else if ((c == 'e') || (c == 'E')) {state = 86;}
	  else if ((c == 'f') || (c == 'F')) {
             yytext[yylen] = 0;
	     yylval.id = copy_string(yytext);
	     return(NUM_FLOAT);
	  }
	  else if (isdigit(c)) { state = 8;}
	  else if ((c == 'l') || (c == 'L')) {
	    state = 87;
	  } else if ((c == 'u') || (c == 'U')) {
	    state = 88;
	  } else {
	      retract(1);
	      yytext[yylen] = 0;
	      yylval.id = copy_string(yytext);
	      return(NUM_INT);
	    }
	  break;
	case 81: /* A floating pointer number of some sort */
	  if ((c = nextchar()) == 0) return(0);
	  if (isdigit(c)) state = 81;
	  else if ((c == 'e') || (c == 'E')) state = 82;
          else if ((c == 'f') || (c == 'F') || (c == 'l') || (c == 'L')) {
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_FLOAT);
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_FLOAT);
	  }
	  break;
	case 82:
	  if ((c = nextchar()) == 0) return(0);
	  if ((isdigit(c)) || (c == '-') || (c == '+')) state = 86;
	  else {
	    retract(2);
	    yytext[yylen-1] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_INT);
	  }
	  break;
	case 83:
	  /* Might be a hexidecimal or octal number */
	  if ((c = nextchar()) == 0) return(0);
	  if (isdigit(c)) state = 84;
	  else if ((c == 'x') || (c == 'X')) state = 85;
	  else if (c == '.') state = 81;
	  else if ((c == 'l') || (c == 'L')) {
	    state = 87;
	  } else if ((c == 'u') || (c == 'U')) {
	    state = 88;
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_INT);
	  }
	  break;
	case 84:
	  /* This is an octal number */
	  if ((c = nextchar()) == 0) return (0);
	  if (isdigit(c)) state = 84;
	  else if ((c == 'l') || (c == 'L')) {
	    state = 87;
	  } else if ((c == 'u') || (c == 'U')) {
	    state = 88;
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_INT);
	  }
	  break;
	case 85:
	  /* This is an hex number */
	  if ((c = nextchar()) == 0) return (0);
	  if ((isdigit(c)) || (c=='a') || (c=='b') || (c=='c') ||
	      (c=='d') || (c=='e') || (c=='f') || (c=='A') ||
	      (c=='B') || (c=='C') || (c=='D') || (c=='E') ||
	      (c=='F'))
	    state = 85;
	  else if ((c == 'l') || (c == 'L')) {
	    state = 87;
	  } else if ((c == 'u') || (c == 'U')) {
	    state = 88;
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_INT);
	  }
	  break;

	case 86:
	  /* Rest of floating point number */

	  if ((c = nextchar()) == 0) return (0);
	  if (isdigit(c)) state = 86;
          else if ((c == 'f') || (c == 'F') || (c == 'l') || (c == 'L')) {
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_FLOAT);
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_FLOAT);
	  }
	  /* Parse a character constant. ie. 'a' */
	  break;

	case 87 :
	  /* A long integer of some sort */
	  if ((c = nextchar()) == 0) return (0);
	  if ((c == 'u') || (c == 'U')) {
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_ULONG);
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_LONG);
	  } 

	case 88:
	  /* An unsigned integer of some sort */
	  if ((c = nextchar()) == 0) return (0);
	  if ((c == 'l') || (c == 'L')) {
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_ULONG);
	  } else {
	    retract(1);
	    yytext[yylen] = 0;
	    yylval.id = copy_string(yytext);
	    return(NUM_UNSIGNED);
	  } 

	case 9:
	  if ((c = nextchar()) == 0) return (0);
	  if (c == '\'') {
	    yytext[yylen-1] = 0;
	    yylval.id = copy_string(yytext+1);
	    return(CHARCONST);
	  }
	  break;

	case 100:
	  if ((c = nextchar()) == 0) return (0);
	  if (isdigit(c)) state = 81;
	  else {
	    retract(1);
	    return(PERIOD);
	  }
	  break;
	default:
	  if (!Error) {
	    fprintf(stderr,"%s : Line %d ::Illegal character '%c'=%d.\n",input_file, line_number,c,c);
	    FatalError();
	  }
	  state = 0;	
  	  Error = 1;	
	  return(ILLEGAL);
	}
    }
}

static int check_typedef = 0;

void scanner_check_typedef() {
  check_typedef = 1;
}

void scanner_ignore_typedef() {
  check_typedef = 0;
}


/**************************************************************
 * int yylex()
 *
 * Gets the lexene and returns tokens.
 *************************************************************/

extern "C" int yylex(void) {
    
    int   l;

    if (!scan_init) {
      scanner_init();
      //      if (LEX_in == NULL) LEX_in = stdin;
      //      scanner_file(LEX_in);
    }
    
    l = yylook();

    /* We got some sort of non-white space object.  We set the start_line 
       variable unless it has already been set */

    if (!start_line) {
      start_line = line_number;
    }

    /* Copy the lexene */

    yytext[yylen] = 0;

    /* Hack to support ignoring of CPP macros */

    if (l != DEFINE) {
      define_first_id = 0;
    }

    switch(l) {

      case ID:

	/* Look for keywords now */

	if (strcmp(yytext,"int") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_INT;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_INT);
	}
	if (strcmp(yytext,"double") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_DOUBLE;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_DOUBLE);
	}
	if (strcmp(yytext,"void") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_VOID;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_VOID);
	}
	if (strcmp(yytext,"char") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_CHAR;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_CHAR);
	}
	if (strcmp(yytext,"short") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_SHORT;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_SHORT);
	}
	if (strcmp(yytext,"long") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_LONG;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_LONG);
	}
	if (strcmp(yytext,"float") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_FLOAT;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_FLOAT);
	}
	if (strcmp(yytext,"signed") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_SINT;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_SIGNED);
	}
	if (strcmp(yytext,"unsigned") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_UINT;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_UNSIGNED);
	}
	if (strcmp(yytext,"bool") == 0) {
	  yylval.type = new DataType;
	  yylval.type->type = T_BOOL;
	  strcpy(yylval.type->name,yytext);
	  return(TYPE_BOOL);
	}
	// C++ keywords
	
	if (CPlusPlus) {
	  if (strcmp(yytext,"class") == 0) return(CLASS);
	  if (strcmp(yytext,"private") == 0) return(PRIVATE);
	  if (strcmp(yytext,"public") == 0) return(PUBLIC);
	  if (strcmp(yytext,"protected") == 0) return(PROTECTED);
	  if (strcmp(yytext,"friend") == 0) return(FRIEND);
	  if (strcmp(yytext,"virtual") == 0) return(VIRTUAL);
	  if (strcmp(yytext,"operator") == 0) return(OPERATOR);
	  if (strcmp(yytext,"throw") == 0) return(THROW);
	  if (strcmp(yytext,"inline") == 0) return(yylex());
	  if (strcmp(yytext,"template") == 0) return(TEMPLATE);
	}

	// Objective-C keywords
        if (ObjC) {
	  if (strcmp(yytext,"@interface") == 0) return (OC_INTERFACE);
	  if (strcmp(yytext,"@end") == 0) return (OC_END);
	  if (strcmp(yytext,"@public") == 0) return (OC_PUBLIC);
	  if (strcmp(yytext,"@private") == 0) return (OC_PRIVATE);
	  if (strcmp(yytext,"@protected") == 0) return (OC_PROTECTED);
	  if (strcmp(yytext,"@class") == 0) return(OC_CLASS);
	  if (strcmp(yytext,"@implementation") == 0) return(OC_IMPLEMENT);
          if (strcmp(yytext,"@protocol") == 0) return(OC_PROTOCOL);
	}

	// Misc keywords

	if (strcmp(yytext,"static") == 0) return(STATIC);
	if (strcmp(yytext,"extern") == 0) return(EXTERN);
	if (strcmp(yytext,"const") == 0) return(CONST);
	if (strcmp(yytext,"struct") == 0) return(STRUCT);
	if (strcmp(yytext,"union") == 0) return(UNION);
	if (strcmp(yytext,"enum") == 0) return(ENUM);
	if (strcmp(yytext,"sizeof") == 0) return(SIZEOF);
	if (strcmp(yytext,"defined") == 0) return(DEFINED);

	// Ignored keywords 

	if (strcmp(yytext,"volatile") == 0) return(yylex());

	// SWIG directives

	if (strcmp(yytext,"%module") == 0) return(MODULE);
	if (strcmp(yytext,"%init") == 0)  return(INIT);
	if (strcmp(yytext,"%wrapper") == 0) return(WRAPPER);
	if (strcmp(yytext,"%readonly") == 0) return(READONLY);
	if (strcmp(yytext,"%readwrite") == 0) return(READWRITE);
	if (strcmp(yytext,"%name") == 0) return(NAME);
        if (strcmp(yytext,"%rename") == 0) return(RENAME);
	if (strcmp(yytext,"%include") == 0) return(INCLUDE);
	if (strcmp(yytext,"%extern") == 0) return(WEXTERN);
        if (strcmp(yytext,"%checkout") == 0) return(CHECKOUT);
	if (strcmp(yytext,"%val") == 0) return(CVALUE);
	if (strcmp(yytext,"%out") == 0) return(COUT);

	if (strcmp(yytext,"%section") == 0) {
	  yylval.ivalue = line_number;
	  return(SECTION);
	}
	if (strcmp(yytext,"%subsection") == 0) {
	  yylval.ivalue = line_number;
	  return(SUBSECTION);
	}
	if (strcmp(yytext,"%subsubsection") == 0) {
	  yylval.ivalue = line_number;
	  return (SUBSUBSECTION);
	}
	if (strcmp(yytext,"%title") == 0) {
	  yylval.ivalue = line_number;
	  return(TITLE);
	} 
	if (strcmp(yytext,"%style") == 0) return(STYLE);
	if (strcmp(yytext,"%localstyle") == 0) return(LOCALSTYLE);
	if (strcmp(yytext,"%typedef") == 0) {
	  yylval.ivalue = 1;
	  return(TYPEDEF);
	}
	if (strcmp(yytext,"typedef") == 0) {
	  yylval.ivalue = 0;
	  return(TYPEDEF);
	}
	if (strcmp(yytext,"%alpha") == 0) return(ALPHA_MODE);
	if (strcmp(yytext,"%raw") == 0) return(RAW_MODE);
	if (strcmp(yytext,"%text") == 0) return(TEXT);
	if (strcmp(yytext,"%native") == 0) return(NATIVE);
	if (strcmp(yytext,"%disabledoc") == 0) return(DOC_DISABLE);
	if (strcmp(yytext,"%enabledoc") == 0) return(DOC_ENABLE);
	if (strcmp(yytext,"%ifdef") == 0) return(IFDEF);
	if (strcmp(yytext,"%else") == 0) return(ELSE);
	if (strcmp(yytext,"%ifndef") == 0) return(IFNDEF);
	if (strcmp(yytext,"%endif") == 0) return(ENDIF);
	if (strcmp(yytext,"%if") == 0) return(IF);
	if (strcmp(yytext,"%elif") == 0) return(ELIF);
	if (strcmp(yytext,"%pragma") == 0) return(PRAGMA);
	if (strcmp(yytext,"%addmethods") == 0) return(ADDMETHODS);
	if (strcmp(yytext,"%inline") == 0) return(INLINE);
	if (strcmp(yytext,"%typemap") == 0) return(TYPEMAP);
	if (strcmp(yytext,"%except") == 0) return(EXCEPT);
	if (strcmp(yytext,"%import") == 0) return(IMPORT);
	if (strcmp(yytext,"%echo") == 0) return(ECHO);
        if (strcmp(yytext,"%new") == 0) return(NEW);
	if (strcmp(yytext,"%apply") == 0) return(APPLY);
	if (strcmp(yytext,"%clear") == 0) return(CLEAR);
        if (strcmp(yytext,"%doconly") == 0) return(DOCONLY);
 
	// Have an unknown identifier, as a last step, we'll
	// do a typedef lookup on it.

	if (check_typedef) {
	  if (DataType::is_typedef(yytext)) {
	    yylval.type = new DataType;
	    yylval.type->type = T_USER;
	    strcpy(yylval.type->name,yytext);
	    yylval.type->typedef_resolve();
	    return(TYPE_TYPEDEF);
	  }
	}

	yylval.id = copy_string(yytext);
	return(ID);
      default:
	return(l);
    }
}

// --------------------------------------------------------------
// scanner_clear_start()
//
// Clears the start of a declaration
// --------------------------------------------------------------

void scanner_clear_start() {
  start_line = 0;
}
