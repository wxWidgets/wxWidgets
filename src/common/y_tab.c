#ifndef lint
static char yysccsid[] = "@(#)yaccpar     1.7 (Berkeley) 09/09/90";
#endif
#define YYBYACC 1
/* #line 2 "parser.y" */
#include "string.h"
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
/* int yywrap(void); */
int yyback(int *, int);

/* You may need to put /DLEX_SCANNER in your makefile
 * if you're using LEX!
	Last change:  JS   13 Jul 97    6:12 pm
 */
#ifdef LEX_SCANNER
/* int yyoutput(int); */
void yyoutput(int);
#else
void yyoutput(int);
#endif

#if defined(__cplusplus) || defined(__STDC__)
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif
#endif
#line 36 "parser.y"
typedef union {
    char *s;
/*    struct pexpr *expr; */
} YYSTYPE;
#line 44 "y_tab.c"
#define INTEGER 1
#define WORD 2
#define STRING 3
#define PERIOD 13
#define OPEN 4
#define CLOSE 5
#define COMMA 6
#define NEWLINE 7
#define ERROR 8
#define OPEN_SQUARE 9
#define CLOSE_SQUARE 10
#define EQUALS 11
#define EXP 14
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    2,    2,    2,    3,    3,
    3,    4,    4,    5,    5,    5,    5,    5,    5,    5,
};
short yylen[] = {                                         2,
    0,    2,    2,    2,    2,    4,    2,    3,    0,    1,
    3,    3,    1,    1,    1,    1,    3,    3,    5,    1,
};
short yydefred[] = {                                      1,
    0,    0,    0,    0,    2,    0,    5,    3,    0,    0,
    0,   15,    7,   20,    0,    0,   13,    4,    0,    0,
    0,    0,    8,    0,    6,    0,   18,    0,   12,   11,
    0,   19,
};
short yydgoto[] = {                                       1,
    5,   14,   15,   16,   17,
};
short yysindex[] = {                                      0,
   -2,    9,    2,    1,    0,   10,    0,    0,   11,   -5,
   17,    0,    0,    0,   14,   -1,    0,    0,   33,   38,
   41,   16,    0,   11,    0,   29,    0,   40,    0,    0,
   44,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,   42,   21,
   24,    0,    0,    0,    0,   30,    0,    0,    0,    0,
    0,    0,    0,   31,    0,   27,    0,   24,    0,    0,
    0,    0,
};
short yygindex[] = {                                      0,
    0,   45,   -8,    0,   26,
};
#define YYTABLESIZE 254
short yytable[] = {                                       3,
   19,   10,   11,   12,   24,    9,    4,   20,   21,    4,
   13,   10,   11,   12,    8,   30,   10,   28,   12,    4,
    9,    7,   18,   23,    4,   16,   16,   22,   14,   14,
   16,   17,   17,   14,   10,    9,   17,   25,   26,   10,
    9,   27,   31,    9,   32,    6,    9,   29,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    2,
};
short yycheck[] = {                                       2,
    9,    1,    2,    3,    6,    4,    9,   13,   14,    9,
   10,    1,    2,    3,   13,   24,    1,    2,    3,    9,
    4,   13,   13,   10,    9,    5,    6,   11,    5,    6,
   10,    5,    6,   10,    5,    5,   10,    5,    1,   10,
   10,    1,   14,    4,    1,    1,    5,   22,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  256,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 14
#if YYDEBUG
char *yyname[] = {
"end-of-file","INTEGER","WORD","STRING","OPEN","CLOSE","COMMA","NEWLINE",
"ERROR","OPEN_SQUARE","CLOSE_SQUARE","EQUALS",0,"PERIOD","EXP",
};
char *yyrule[] = {
"$accept : commands",
"commands :",
"commands : commands command",
"command : WORD PERIOD",
"command : expr PERIOD",
"command : error PERIOD",
"expr : WORD OPEN arglist CLOSE",
"expr : OPEN_SQUARE CLOSE_SQUARE",
"expr : OPEN_SQUARE arglist CLOSE_SQUARE",
"arglist :",
"arglist : arg",
"arglist : arg COMMA arglist",
"arg : WORD EQUALS arg1",
"arg : arg1",
"arg1 : WORD",
"arg1 : STRING",
"arg1 : INTEGER",
"arg1 : INTEGER PERIOD INTEGER",
"arg1 : INTEGER EXP INTEGER",
"arg1 : INTEGER PERIOD INTEGER EXP INTEGER",
"arg1 : expr",
};
#endif
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 600
#define YYMAXDEPTH 600
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 118 "parser.y"

#include "../common/lex_yy.c"

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

#ifndef __SC__
#ifdef USE_DEFINE
#ifndef yywrap
#define yywrap() 1
#endif
#else
/* int yywrap() { return 1; } */
#endif
#endif
#line 247 "y_tab.c"
#define YYABORT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, reading %d (%s)\n", yystate,
                    yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: state %d, shifting to state %d\n",
                    yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: state %d, error recovery shifting\
 to state %d\n", *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: error recovery discarding state %d\n",
                            *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, error recovery discards token %d (%s)\n",
                    yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("yydebug: state %d, reducing by rule %d (%s)\n",
                yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 3:
#line 68 "parser.y"
{process_command(proio_cons(make_word(yyvsp[-1].s), NULL)); free(yyvsp[-1].s);}
break;
case 4:
#line 70 "parser.y"
{process_command(yyvsp[-1].s);}
break;
case 5:
#line 72 "parser.y"
{syntax_error("Unrecognized command.");}
break;
case 6:
#line 76 "parser.y"
{yyval.s = proio_cons(make_word(yyvsp[-3].s), yyvsp[-1].s); free(yyvsp[-3].s);}
break;
case 7:
#line 78 "parser.y"
{yyval.s = proio_cons(NULL, NULL);}
break;
case 8:
#line 80 "parser.y"
{yyval.s = yyvsp[-1].s; }
break;
case 9:
#line 84 "parser.y"
{yyval.s = NULL;}
break;
case 10:
#line 86 "parser.y"
{yyval.s = proio_cons(yyvsp[0].s, NULL);}
break;
case 11:
#line 89 "parser.y"
{yyval.s = proio_cons(yyvsp[-2].s, yyvsp[0].s);}
break;
case 12:
#line 93 "parser.y"
{yyval.s = proio_cons(make_word("="), proio_cons(make_word(yyvsp[-2].s), proio_cons(yyvsp[0].s, NULL)));
                         free(yyvsp[-2].s); }
break;
case 13:
#line 96 "parser.y"
{yyval.s = yyvsp[0].s; }
break;
case 14:
#line 99 "parser.y"
{yyval.s = make_word(yyvsp[0].s); free(yyvsp[0].s);}
break;
case 15:
#line 101 "parser.y"
{yyval.s = make_string(yyvsp[0].s); free(yyvsp[0].s);}
break;
case 16:
#line 103 "parser.y"
{yyval.s = make_integer(yyvsp[0].s); free(yyvsp[0].s);}
break;
case 17:
#line 105 "parser.y"
{yyval.s = make_real(yyvsp[-2].s, yyvsp[0].s); free(yyvsp[-2].s); free(yyvsp[0].s); }
break;
case 18:
#line 107 "parser.y"
{yyval.s = make_exp(yyvsp[-2].s, yyvsp[0].s); free(yyvsp[-2].s); free(yyvsp[0].s); }
break;
case 19:
#line 110 "parser.y"
{yyval.s = make_exp2(yyvsp[-4].s, yyvsp[-2].s, yyvsp[0].s); free(yyvsp[-4].s); free(yyvsp[-2].s);
                                                                  free(yyvsp[0].s); }
break;
case 20:
#line 114 "parser.y"
{yyval.s = yyvsp[0].s;}
break;
#line 461 "y_tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("yydebug: state %d, reading %d (%s)\n",
                        YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("yydebug: after reduction, shifting from state %d \
to state %d\n", *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
