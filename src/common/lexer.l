SIGN		[+-]
DIGIT		[0-9]
ALPHA		[a-zA-Z_]
ALPHADIGIT	[a-zA-Z_0-9]
STRINGCHAR	[^"\\]
WORDCHAR	[^'\\]

%{
/*
 * File:         lexer.l
 * Description:  Lexical analyser for PROLOGIO; can be used with
 *               either lex and flex.
 */
#include <string.h>
#include <unistd.h>

/* +++steve162e: added, otherwise, PROIO_input will be undefined (at least under LINUX)
             please check, if this is also TRUE under other UNIXes.
 */

#if defined(FLEX_SCANNER) && defined(_LINUX)
#define PROIO_input my_input
#endif
/* ---steve162e */

#include "wx/expr.h"

#ifdef wx_x
extern char *malloc();
#endif
#define Return(x) return x;

#if defined(VMS) && !defined(strdup)
#define strdup(s) (strcpy((char *)malloc(strlen(s)+1), s));
#endif

static size_t lex_buffer_length = 0;
static const char *lex_buffer = NULL;
static size_t lex_string_ptr = 0;
static int lex_read_from_string = 0;

static int my_input(void);
static int my_unput(char);

#ifdef FLEX_SCANNER
#undef YY_INPUT
# define YY_INPUT(buf,result,max_size) \
   if (lex_read_from_string) \
   {  int c = my_input(); result = (c == 0) ? YY_NULL : ((buf)[0]=(c), 1); } \
   else \
	if ( (result = read( fileno(yyin), (char *) buf, max_size )) < 0 ) \
	    YY_FATAL_ERROR( "read() in flex scanner failed" );
#else
# undef unput
# define unput(_c) my_unput(_c)
#endif

%}

%%

{SIGN}?{DIGIT}+			  {yylval.s = strdup(yytext); Return(INTEGER);}

"e"                               Return(EXP);

{ALPHA}{ALPHADIGIT}*		  {yylval.s = strdup(yytext); Return(WORD);}

"'"{WORDCHAR}*"'"	          {int len = strlen(yytext);
                                   yytext[len-1] = 0;
                                   yylval.s = strdup(yytext+1); 
                                   Return(WORD);}

\"({STRINGCHAR}|\\\"|\|\\\\|\\)*\"  {yylval.s = strdup(yytext); Return(STRING);}

"("				  Return(OPEN);

")"				  Return(CLOSE);

","				  Return(COMMA);

"["                               Return(OPEN_SQUARE);

"]"                               Return(CLOSE_SQUARE);

"="                               Return(EQUALS);

"."				  Return(PERIOD);

[ \t]				  ;

\n				  ;

"/*"    {       loop:
#ifdef __cplusplus
                          while (yyinput() != '*');
                          switch (yyinput())
#else
                          while (input() != '*');
                          switch (input())
#endif
                                  {
                                  case '/': break;
                                  case '*': unput('*');
                                  default: goto loop;
                                  }
                          }

.				  Return(ERROR);

%%


#ifdef FLEX_SCANNER
static int lex_input() {
  return input();
}
#else	/* BSD/AT&T lex */
#ifndef input
# error "Sorry, but need either flex or AT&T lex"
#endif
static int lex_input() {
  return input();
}
/* # undef unput
# define unput(_c) my_unput(_c)
*/

# undef input
# define input() my_input()
static int my_unput(char c)
{
  if (lex_read_from_string) {
    /* Make sure we have something */
    if (lex_string_ptr) {
      if (c == '\n') yylineno--;
      lex_string_ptr--;
    }
  } else {
    yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;
/*    unput(c); Causes infinite recursion! */
  }
  return c;
}

#endif

/* Public */ 
void LexFromFile(FILE *fd)
{
  lex_read_from_string = 0;
  yyin = fd;
  /* Don't know why this is necessary, but otherwise
   * lex only works _once_!
   */
#ifdef FLEX_SCANNER
  yyrestart(fd);
  yy_init = 1;
#endif
}

void LexFromString(char *buffer)
{
  lex_read_from_string = 1;
  lex_buffer = buffer;
  lex_buffer_length = strlen(buffer);
  lex_string_ptr = 0;
  /* Don't know why this is necessary, but otherwise
   * lex only works _once_!
   */
#ifdef FLEX_SCANNER
  yy_init = 1;
#endif
}

static int my_input( void )
{
  if (lex_read_from_string) {
    if (lex_string_ptr == lex_buffer_length)
      return 0;
    else {
      char c = lex_buffer[lex_string_ptr++];
#ifndef FLEX_SCANNER
      if (c == '\n') yylineno++;
#endif
      return c;
    }
  } else {
    return lex_input();
  }
}

void wxExprCleanUp()
{
	if (yy_current_buffer)
		yy_delete_buffer(yy_current_buffer);
}
