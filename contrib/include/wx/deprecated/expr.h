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
