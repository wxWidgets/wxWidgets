/* //////////////////////////////////////////////////////////////////////////
// Name:        expr.h
// Purpose:     C helper defines and functions for wxExpr class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
////////////////////////////////////////////////////////////////////////// */

#ifndef _WX_EXPRH__
#define _WX_EXPRH__

#include <math.h>
#if defined(__VISAGECPP__) && __IBMCPP__ >= 400
#  undef __BSEXCPT__
#endif
#include <stdlib.h>

#ifdef ____HPUX__
#define alloca malloc
#endif

/* Rename all YACC/LEX stuff or we'll conflict with other
 * applications
 */

#define yyback PROIO_yyback
#define yylook PROIO_yylook
#define yywrap PROIO_yywrap
#define yyoutput PROIO_yyoutput
#define yylex PROIO_yylex
#define yyerror PROIO_yyerror
#define yyleng PROIO_yyleng
#define yytext PROIO_yytext
#define yymorfg PROIO_yymorfg
#define yylineno PROIO_yylineno
#define yytchar PROIO_yytchar
#define yyin PROIO_yyin
#define yyout PROIO_yyout
#define yysvf PROIO_yysvf
#define yyestate PROIO_yyestate
#define yysvec PROIO_yysvec
#define yybgin PROIO_yybgin
#define yyprevious PROIO_yyprevious
#define yylhs PROIO_yylhs
#define yylen PROIO_yylen
#define yydefred PROIO_yydefred
#define yydgoto PROIO_yydgoto
#define yysindex PROIO_yysindex
#define yyrindex PROIO_yyrindex
#define yygindex PROIO_yygindex
#define yytable PROIO_yytable
#define yycheck PROIO_yycheck
#define yyname PROIO_yyname
#define yyrule PROIO_yyrule
#define yydebug PROIO_yydebug
#define yynerrs PROIO_yynerrs
#define yyerrflag PROIO_yyerrflag
#define yychar PROIO_yychar
#define yyvsp PROIO_yyvsp
#define yyssp PROIO_yyssp
#define yyval PROIO_yyval
#define yylval PROIO_yylval
#define yyss PROIO_yyss
#define yyvs PROIO_yyvs
#define yyparse PROIO_yyparse

/* +++steve162e: more defines necessary */
#define yy_init_buffer PROIO_yy_init_buffer
#define yy_create_buffer PROIO_yy_create_buffer
#define yy_load_buffer_state PROIO_yy_load_buffer_state
#define yyrestart PROIO_yyrestart
#define yy_switch_to_buffer PROIO_yy_switch_to_buffer
#define yy_delete_buffer PROIO_yy_delete_buffer
/* ---steve162e */

/* WG 1/96: still more for flex 2.5 */
#define yy_scan_buffer PROIO_scan_buffer
#define yy_scan_string PROIO_scan_string
#define yy_scan_bytes PROIO_scan_bytes
#define yy_flex_debug PROIO_flex_debug
#define yy_flush_buffer PROIO_flush_buffer
#if !defined(__VISAGECPP__)
/* multiply defined??? */
#define yyleng PROIO_yyleng
#define yytext PROIO_yytext
#endif

#ifdef __cplusplus
extern "C" {
char *proio_cons(char *, char *);
char * wxmake_integer(char *);
char * wxmake_word(char *);
char * wxmake_string(char *);
char * wxmake_real(char *, char *);
char * wxmake_exp(char *, char *);
char * wxmake_exp2(char *, char *, char*);
void add_expr(char *);
void process_command(char *);
void syntax_error(char *);
}
#else
#if defined(__BORLANDC__) || defined(__VISAGECPP__)
char *proio_cons(char *, char *);
char * wxmake_integer(char *);
char * wxmake_word(char *);
char * wxmake_string(char *);
char * wxmake_real(char *, char *);
char * wxmake_exp(char *, char *);
char * wxmake_exp2(char *, char *, char*);
void add_expr(char *);
void process_command(char *);
void syntax_error(char *);
int  lex_input(void);
#else
char *proio_cons();
char * wxmake_integer();
char * wxmake_word();
char * wxmake_string();
char * wxmake_real();
char * wxmake_exp();
char * wxmake_exp2();

void add_expr();
void process_command();
void syntax_error();
#endif
#endif

#endif
	/* _WX_EXPRH__ */
