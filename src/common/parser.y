 %{
#include "wx/setup.h"
#include <string.h>
#ifdef _MSC_VER
#include <io.h>
#endif
#if defined(__GNUWIN32__) && !defined(__TWIN32__)
#include <sys/unistd.h>
#endif

#include "wx/expr.h"

#ifndef __EXTERN_C__
#define __EXTERN_C__ 1
#endif

#if defined(__cplusplus) || defined(__STDC__)
#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
#endif
int yylex(void);
int yylook(void);
int yywrap(void);
int yyback(int *, int);
void yyerror(char *);

/* You may need to put /DLEX_SCANNER in your makefile
 * if you're using LEX!
 */
void yyoutput(int);

#if defined(__cplusplus) || defined(__STDC__)
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif
#endif
%}

%union {
    char *s;
/*    struct pexpr *expr; */
}


%start commands

%token <s> INTEGER 1
%token <s> WORD 2
%token <s> STRING 3
%token <s> PERIOD 13
%token OPEN 4
%token CLOSE 5
%token COMMA 6
%token NEWLINE 7
%token ERROR 8
%token OPEN_SQUARE 9
%token CLOSE_SQUARE 10
%token EQUALS 11
%token EXP 14

/* %type <expr> command expr arglist arg arg1 */
%type <s> command expr arglist arg arg1

%%

commands :	/* empty */
	|	commands command
	;

command	:       WORD PERIOD
			{process_command(proio_cons(wxmake_word($1), NULL)); free($1);}
        |       expr PERIOD
			{process_command($1);}
	|	error PERIOD
			{syntax_error("Unrecognized command.");}
	;

expr	:	WORD OPEN arglist CLOSE 
			{$$ = proio_cons(wxmake_word($1), $3); free($1);}
	|	OPEN_SQUARE CLOSE_SQUARE
                        {$$ = proio_cons(NULL, NULL);}
	|	OPEN_SQUARE arglist CLOSE_SQUARE
			{$$ = $2; }
	;

arglist	:
			{$$ = NULL;}
	|	arg
			{$$ = proio_cons($1, NULL);}
	|
		arg COMMA arglist
			{$$ = proio_cons($1, $3);}
	;

arg	:	WORD EQUALS arg1
			{$$ = proio_cons(wxmake_word("="), proio_cons(wxmake_word($1), proio_cons($3, NULL)));
                         free($1); }
	|	arg1
			{$$ = $1; }

arg1	:	WORD
			{$$ = wxmake_word($1); free($1);}
	|	STRING
			{$$ = wxmake_string($1); free($1);}
	|	INTEGER
			{$$ = wxmake_integer($1); free($1);}
	|	INTEGER PERIOD INTEGER
			{$$ = wxmake_real($1, $3); free($1); free($3); }
        |       INTEGER EXP INTEGER
                         {$$ = wxmake_exp($1, $3); free($1); free($3); }
        |
              INTEGER PERIOD INTEGER EXP INTEGER
                         {$$ = wxmake_exp2($1, $3, $5); free($1); free($3);
                                                                  free($5); }

	|	expr
			{$$ = $1;}
	;

%%

/* We include lexer.c if we are building for gtk, wine or motif
 * and also whenever we are using configure (marked by __WX_SETUP_H__) for,
 * for example, cross compilation. */
#if (defined(__WXGTK__) || defined(__WXWINE__) || defined(__WXMOTIF__)) || defined(__WX_SETUP_H__) && !defined(NO_CONFIGURE)
#include "lexer.c"
#else
#if (defined(__MWERKS__))
#include "../common/cwlex_yy.c"
#else
#include "../common/lex_yy.c"
#endif
#endif

/*
void yyerror(s)
char *s;
{
  syntax_error(s);
}
*/

/* Ansi prototype. If this doesn't work for you... uncomment
   the above instead.
 */

void yyerror(char *s)
{
  syntax_error(s);
}

/*
 * Unfortunately, my DOS version of FLEX
 * requires yywrap to be #def'ed, whereas
 * the UNIX flex expects a proper function.
 */

/* Not sure if __SC__ is the appropriate thing
 * to test
 */

/* HH: Added test for __WX_SETUP_H__ for gnuwin builds
 * using configure */ 
#if !defined(__SC__) && !defined(__GNUWIN32__) 
#ifdef USE_DEFINE
#ifndef yywrap
#define yywrap() 1
#endif
#else if !defined(__alpha___) && !defined(__alpha) && !defined(__ultrix)
int yywrap() { return 1; }
#endif
#elif defined(__WX_SETUP_H__)
int yywrap() { return 1; }
#endif
