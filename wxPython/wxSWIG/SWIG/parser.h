typedef union {         
  char        *id;
  struct Declaration {
    char *id;
    int   is_pointer;
    int   is_reference;
  } decl;
  struct InitList {
    char **names;
    int    count;
  } ilist;
  struct DocList {
    char **names;
    char **values;
    int  count;
  } dlist;
  struct Define {
    char *id;
    int   type;
  } dtype;
  DataType     *type;
  Parm         *p;
  TMParm       *tmparm;
  ParmList     *pl;
  int           ivalue;
} YYSTYPE;
#define	ID	258
#define	HBLOCK	259
#define	WRAPPER	260
#define	POUND	261
#define	STRING	262
#define	NUM_INT	263
#define	NUM_FLOAT	264
#define	CHARCONST	265
#define	NUM_UNSIGNED	266
#define	NUM_LONG	267
#define	NUM_ULONG	268
#define	TYPEDEF	269
#define	TYPE_INT	270
#define	TYPE_UNSIGNED	271
#define	TYPE_SHORT	272
#define	TYPE_LONG	273
#define	TYPE_FLOAT	274
#define	TYPE_DOUBLE	275
#define	TYPE_CHAR	276
#define	TYPE_VOID	277
#define	TYPE_SIGNED	278
#define	TYPE_BOOL	279
#define	TYPE_TYPEDEF	280
#define	LPAREN	281
#define	RPAREN	282
#define	COMMA	283
#define	SEMI	284
#define	EXTERN	285
#define	INIT	286
#define	LBRACE	287
#define	RBRACE	288
#define	DEFINE	289
#define	PERIOD	290
#define	CONST	291
#define	STRUCT	292
#define	UNION	293
#define	EQUAL	294
#define	SIZEOF	295
#define	MODULE	296
#define	LBRACKET	297
#define	RBRACKET	298
#define	WEXTERN	299
#define	ILLEGAL	300
#define	READONLY	301
#define	READWRITE	302
#define	NAME	303
#define	RENAME	304
#define	INCLUDE	305
#define	CHECKOUT	306
#define	ADDMETHODS	307
#define	PRAGMA	308
#define	CVALUE	309
#define	COUT	310
#define	ENUM	311
#define	ENDDEF	312
#define	MACRO	313
#define	CLASS	314
#define	PRIVATE	315
#define	PUBLIC	316
#define	PROTECTED	317
#define	COLON	318
#define	STATIC	319
#define	VIRTUAL	320
#define	FRIEND	321
#define	OPERATOR	322
#define	THROW	323
#define	TEMPLATE	324
#define	NATIVE	325
#define	INLINE	326
#define	IFDEF	327
#define	IFNDEF	328
#define	ENDIF	329
#define	ELSE	330
#define	UNDEF	331
#define	IF	332
#define	DEFINED	333
#define	ELIF	334
#define	RAW_MODE	335
#define	ALPHA_MODE	336
#define	TEXT	337
#define	DOC_DISABLE	338
#define	DOC_ENABLE	339
#define	STYLE	340
#define	LOCALSTYLE	341
#define	TYPEMAP	342
#define	EXCEPT	343
#define	IMPORT	344
#define	ECHO	345
#define	NEW	346
#define	APPLY	347
#define	CLEAR	348
#define	DOCONLY	349
#define	TITLE	350
#define	SECTION	351
#define	SUBSECTION	352
#define	SUBSUBSECTION	353
#define	LESSTHAN	354
#define	GREATERTHAN	355
#define	USERDIRECTIVE	356
#define	OC_INTERFACE	357
#define	OC_END	358
#define	OC_PUBLIC	359
#define	OC_PRIVATE	360
#define	OC_PROTECTED	361
#define	OC_CLASS	362
#define	OC_IMPLEMENT	363
#define	OC_PROTOCOL	364
#define	OR	365
#define	XOR	366
#define	AND	367
#define	LSHIFT	368
#define	RSHIFT	369
#define	PLUS	370
#define	MINUS	371
#define	STAR	372
#define	SLASH	373
#define	UMINUS	374
#define	NOT	375
#define	LNOT	376
#define	DCOLON	377


extern YYSTYPE yylval;
