/*                    -*- Mode: C -*- 
 * 
 * lacheck.lex - A consistency checker checker for LaTeX documents
 * 
 * Copyright (C) 1991 Kresten Krab Thorup (krab@iesd.auc.dk).
 * 
 * $Locker$
 * $Revision$
 * Author          : Kresten Krab Thorup
 * Created On      : Sun May 26 18:11:58 1991
 * Last Modified By: Kresten Krab Thorup
 * Last Modified On: Thu May 30 02:29:57 1991
 * Update Count    : 16
 * 
 * HISTORY
 * 30-May-1991  (Last Mod: Thu May 30 02:22:33 1991 #15)  Kresten Krab Thorup
 *    Added check for `$${punct}' and `{punct}$' constructions
 * 30-May-1991  (Last Mod: Wed May 29 10:31:35 1991 #6)  Kresten Krab Thorup
 *    Improved (dynamical) stack management from Andreas Stolcke ...
 *                                       <stolcke@ICSI.Berkeley.EDU> 
 * 26-May-1991  Kresten Krab Thorup
 *    Initial distribution version.
 */
%{
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

extern char *realloc();

#ifdef NEED_STRSTR
char *strstr();
#endif

#define GROUP_STACK_SIZE 10
#define INPUT_STACK_SIZE 10

#define PROGNAME "LaCheck"

  /* macros */

#define CG_NAME gstack[gstackp-1].s_name
#define CG_TYPE gstack[gstackp-1].s_type
#define CG_LINE gstack[gstackp-1].s_line
#define CG_FILE gstack[gstackp-1].s_file

char *bg_command();
void pop();
void push();
void g_checkend();
void e_checkend();
void f_checkend();
void input_file();
void print_bad_match();
int check_top_level_end();

  /* global variables */

char returnval[100];
int line_count = 1;
int warn_count = 0;
char *file_name;
char verb_char;

  /* the group stack */

typedef struct tex_group 
 {
    unsigned char *s_name;
    int s_type;
    int s_line;
    char *s_file; 
 } tex_group;

tex_group *gstack;
int gstack_size = GROUP_STACK_SIZE;
int gstackp = 0;

typedef struct input_ 
 {
    YY_BUFFER_STATE stream;
    char *name;
    int linenum;
 } input_;

input_ *istack;
int istack_size = INPUT_STACK_SIZE;
int istackp = 0;

int def_count = 0;

%}

%x B_ENVIRONMENT E_ENVIRONMENT VERBATIM INCLUDE MATH COMMENT VERB DEF
%x AFTER_DISPLAY

b_group ("{"|\\bgroup)
e_group ("}"|\\egroup)

b_math \\\(
e_math \\\)
math \$

b_display \\\[
e_display \\\]
display \$\$

non_par_ws ([ \t]+\n?[ \t]*|[ \t]*\n[ \t]*|[ \t]*\n?[ \t]+)

ws [ \n\t]
space ({ws}|\~|\\space)
hard_space (\~|\\space)

u_letter [A-ZFXE] 
l_letter [a-zfxe] 
punct [\!\.\?]
atoz [a-zA-Z]
letter (u_letter|l_letter)

c_bin ("-"|"+"|"\\cdot"|"\\oplus"|"\\otimes"|"\\times")
l_bin (",")

general_abbrev {letter}+{punct}

non_abbrev {u_letter}{u_letter}+{punct}

font_spec (rm|bf|sl|it|tt|em|mediumseries|normalshape)

primitive \\(above|advance|catcode|chardef|closein|closeout|copy|count|countdef|cr|crcr|csname|delcode|dimendef|dimen|divide|expandafter|font|hskp|vskip|openout)

symbol ("$"("\\"{atoz}+|.)"$"|"\\#"|"\\$"|"\\%"|"\\ref")

%%

"\\\\" { ; }

<DEF,INITIAL>"\\\%" { ; }

<DEF,INITIAL>"%"[^\n]* { ; }

<DEF,INITIAL>\n 	{ line_count++; }

<DEF,INITIAL>"\\\{" { ; }

<DEF,INITIAL>"\\\}" { ; }

"\\\$" { ; }

{b_group} {  push( "{", 0, line_count);}

{e_group} {  g_checkend(0); }

"\\"[exg]?def[^\{] 	BEGIN(DEF);

<DEF>{b_group} { ++def_count; }

<DEF>{e_group} { --def_count;
		 if(def_count == 0)
		     BEGIN(INITIAL); }

<DEF>. { ; }

{b_math} {
    if(CG_TYPE == 4 || CG_TYPE == 5)
	print_bad_match(yytext,4);
    else
    {
	push( yytext, 4, line_count);
    }}

{e_math} {  g_checkend(4); }

{b_display} {
    if(CG_TYPE == 4 || CG_TYPE == 5)
	print_bad_match(yytext,5);
    else 
    {
	push( yytext, 5, line_count);
    }}


{e_display} {  g_checkend(5);
	       BEGIN(AFTER_DISPLAY);}

<AFTER_DISPLAY>{punct} { 

    printf( "\"%s\", line %d: puctation mark \"%s\" should be placed before end of displaymath\n", 
	   file_name, line_count, yytext); 
    ++warn_count ; 

  BEGIN(INITIAL); }

<AFTER_DISPLAY>. {  BEGIN(INITIAL); }

<AFTER_DISPLAY>\n { 	++line_count; 
			BEGIN(INITIAL); }

{punct}/("\$"|"\\)") { if (CG_TYPE == 4)
       {
	 printf( "\"%s\", line %d: puctation mark \"%s\" should be placed after end of math mode\n", 
		file_name, line_count, yytext); 
	 ++warn_count ;
       }}

{math} {

    if(CG_TYPE == 5)
	print_bad_match(yytext, 4);
    else 

    if(CG_TYPE == 4)
    {
	e_checkend(4, yytext);
    }
    else
    {
	push( yytext, 4, line_count); 
    }}


{display}  {

    if(CG_TYPE == 4)
	print_bad_match(yytext,5);
    else 

    if(CG_TYPE == 5)
    {
	e_checkend(5, yytext);
        BEGIN(AFTER_DISPLAY);
    }
    else
    {
	push( yytext, 5, line_count);
    }}

\\begingroup/[^a-zA-Z]  {
 {
    push((unsigned char *)"\\begingroup", 1, line_count); 
 }}


\\endgroup/[^a-zA-Z]  {
 {
    g_checkend(1);
 }}


\\begin[ \t]*"{" { BEGIN(B_ENVIRONMENT); }

\\begin[ \t]*/\n { 
 {
    
    printf("\"%s\", line %i: {argument} missing for \\begin\n",
	   file_name, line_count) ;
    ++warn_count;
 }}

<B_ENVIRONMENT>[^\}\n]+ { 
 {
    if (strcmp( yytext, "verbatim" ) == 0 )
	{
	 input();
	 BEGIN(VERBATIM);
	}
    else
	{
    	 push(yytext, 2, line_count);
 	 input();
	 BEGIN(INITIAL);
	}
 }}

<VERBATIM>\\end[ \t]*\{verbatim\} { BEGIN(INITIAL); }

<VERBATIM>. { ; }

<VERBATIM>\n { ++line_count; }


\\verb.	{ 
	  sscanf (yytext, "\\verb%c", &verb_char  ); 
	  BEGIN(VERB); 
	}

<VERB>. {
		if ( *yytext == verb_char )
		  BEGIN(INITIAL); 
		if ( *yytext == '\n' )
 		  ++line_count;
	} 


\\end[ \t]*"{" { BEGIN(E_ENVIRONMENT); }

\\end[ \t]*/\n { 
 {
    printf("\"%s\", line %i: {argument} missing for \\end\n",
	   file_name, line_count) ;
    ++warn_count;
 }}


<E_ENVIRONMENT>[^\}\n]+ { 
 {
    e_checkend(2, yytext);
    input();
    
    BEGIN(INITIAL);
 }}


{ws}([a-zfxe]".")*[a-zA-ZfxeFXE]*[a-zfxe]"."/{non_par_ws}+[a-zfxe]    { 
 {
    if ( *yytext == '\n' )
		++line_count; 

    printf( "\"%s\", line %d: missing `\\ ' after \"%s\"\n", 
	   file_name, line_count, ++yytext); 
    ++warn_count ; 
 }}

([a-zfxe]".")*[a-zA-ZfxeFXE]*[a-zfxe]"."/{non_par_ws}+[a-zfxe]    { 
 {
    printf( "\"%s\", line %d: missing `\\ ' after \"%s\"\n", 
	   file_name, line_count, yytext); 
    ++warn_count ; 
 }}

{ws}{non_abbrev}/{non_par_ws}{u_letter}   { 
 {
    if ( *yytext == '\n' )
	++line_count;
    printf("\"%s\", line %d: missing `\\\@' before punctation mark in \"%s\"\n", 
	   file_name, line_count, ++yytext); 
    ++warn_count ; 
 }}

{non_abbrev}/{non_par_ws}{u_letter}   { 
 {
    printf("\"%s\", line %d: missing `\\\@' before `.' in \"%s\"\n", 
	   file_name, line_count, yytext); 
    ++warn_count ; 
 }}

({hard_space}{space}|{space}{hard_space})  { 

    printf("\"%s\", line %d: double space at \"%s\"\n",
	   file_name, line_count, yytext); 
    ++warn_count;
  }

{c_bin}{ws}?(\\(\.|\,|\;|\:))*{ws}?\\ldots{ws}?(\\(\.|\,|\;|\:))*{ws}?{c_bin} {
	printf("\"%s\", line %d: \\ldots should be \\cdots in \"%s\"\n",
	   file_name, line_count, yytext); 
	++warn_count;
  }

[^\\]{l_bin}{ws}?(\\(\.|\,|\;|\:))*{ws}?\\cdots{ws}?(\\(\.|\,|\;|\:))*{ws}?[^\\]{l_bin} {
	printf("\"%s\", line %d: \\cdots should be \\ldots in \"%s\"\n",
	   file_name, line_count, yytext); 
	++warn_count;
  }

{c_bin}{ws}?(\\(\.|\,|\;|\:))*{ws}?"."+{ws}?(\\(\.|\,|\;|\:))*{ws}?{c_bin} {
	printf("\"%s\", line %d: Dots should be \\cdots in \"%s\"\n",
	   file_name, line_count, yytext); 
	++warn_count;
  }

[^\\]{l_bin}{ws}?(\\(\.|\,|\;|\:))*{ws}?"."+{ws}?(\\(\.|\,|\;|\:))*{ws}?[^\\]{l_bin} {
	printf("\"%s\", line %d: Dots should be \\ldots in \"%s\"\n",
	   file_name, line_count, yytext); 
	++warn_count;
  }


\.\.\. { 
    printf("\"%s\", line %d: Dots should be ellipsis \"%s\"\n",
	   file_name, line_count, yytext); 
    ++warn_count;
  }

 /*
 *
 *  The `~' one is not too good, perhaps it shoud be an option.
 *
 */
 /*
 {l_letter}" "{symbol}  {

    printf("\"%s\", line %d: perhaps you should insert a `~' before%s\n",
	   file_name, line_count, ++yytext); 
  }
 */
 
 /*
 {primitive}/[^a-zA-Z] {
 {
    printf("\"%s\", line %d: Don't use \"%s\" in LaTeX documents\n", 
	   file_name, line_count, yytext); 
    ++warn_count ; 
 }}    
 */

\\{font_spec}/[ \t]*"{" { 
 {
    printf("\"%s\", line %d: Fontspecifiers don't take arguments. \"%s\"\n", 
	   file_name, line_count, yytext); 
    ++warn_count; 
  /*    (void) input(); */
 }}


\\([a-zA-Z\@]+\@[a-zA-Z\@]*|[a-zA-Z\@]*\@[a-zA-Z\@]+) { 
 {
    printf("\"%s\", line %d: Do not use @ in LaTeX macro names. \"%s\"\n", 
	   file_name, line_count, yytext); 
    ++warn_count; 
 }}


"%"  { BEGIN(COMMENT); }

<COMMENT>\n	{ BEGIN(INITIAL); ++line_count; }

<COMMENT>.	{ ; }


\\(input|include)([ \t]|"{")	{ BEGIN(INCLUDE); }

<INCLUDE>[^\}\n]+	{
 {
	if ( strstr(yytext,"\.sty") == NULL )
	{
		input_file(yytext);
	}
	else
	{
		printf("\"%s\", line %d: Style file \`%s\' omitted.\n",
			file_name,
			line_count,
			yytext);
		input();
	}
	BEGIN(INITIAL);
 }}

<<EOF>> { 
	  if (--istackp < 0)
		  yyterminate(); 

	  else
		{ 
		  fclose(yyin);
	  	  f_checkend(file_name);
		  yy_switch_to_buffer(istack[istackp].stream);
		  free(file_name);
		  line_count = istack[istackp].linenum;
		  file_name = istack[istackp].name;
		  input();
		  BEGIN(INITIAL);
		}    	
	 
	}


. { ; }
%%
int main( argc, argv )
int argc;
char *argv[];
{
    /* allocate initial stacks */
    gstack = (tex_group *)malloc(gstack_size * sizeof(tex_group));
    istack = (input_ *)malloc(istack_size * sizeof(input_));
    if ( gstack == NULL || istack == NULL ) {
	fprintf(stderr, "%s: not enough memory for stacks\n", PROGNAME);
	exit(3);
    }
	
    if(argc > 1)
    {
        if ( (file_name = malloc(strlen(argv[1]) + 5)) == NULL ) {
		fprintf(stderr, "%s: out of memory\n", PROGNAME);
		exit(3);
	}
	
	strcpy(file_name, argv[1]);
	
	if ((yyin = fopen( file_name, "r")) != NULL )
	{
	    push(file_name, 3, 1);
	    yylex();
	    f_checkend(file_name);
	}
	else {   
		 strcat(file_name, ".tex" );
		 if ((yyin = fopen( file_name, "r")) != NULL )
		 {
		     push(file_name, 3, 1);
		     yylex();
		     f_checkend(file_name);
		 }
		 else
		     fprintf(stderr,
				"%s: Could not open : %s\n",PROGNAME, argv[1]);
	     }
    }
    else
    {
	printf("\n* %s *\n\n",PROGNAME);
	printf("\t...a consistency checker for LaTeX documents.\n\n");

	printf("Usage:\n\tlacheck filename[.tex] <return>\n\n\n");

	printf("\tFrom within Emacs:\n\n\t");
	printf("M-x compile <return>\n\tlacheck filename[.tex] <return>");
	printf("\n\n\tUse C-x ` to step through the messages.\n\n");
	printf("\n\tThe found context is displayed in \"double quotes\"\n\n");
	printf("Remark:\n\tAll messages are only warnings!\n\n");
	printf("\tYour document may be right though LaCheck tells\n");
	printf("\tsomthing else.\n\n");
    }
    return(0);
}

#ifdef NEED_STRSTR
char *
strstr(string, substring)
    register char *string;	/* String to search. */
    char *substring;		/* Substring to try to find in string. */
{
    register char *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
    if (*b == 0) {
	return string;
    }
    for ( ; *string != 0; string += 1) {
	if (*string != *b) {
	    continue;
	}
	a = string;
	while (1) {
	    if (*b == 0) {
		return string;
	    }
	    if (*a++ != *b++) {
		break;
	    }
	}
	b = substring;
    }
    return (char *) 0;
}
#endif /* NEED_STRSTR */

void push(p_name, p_type, p_line)
unsigned char *p_name;
int p_type;
int p_line;
{
    if ( gstackp == gstack_size ) {	/* extend stack */
	gstack_size *= 2;
	gstack = (tex_group *)realloc(gstack, gstack_size * sizeof(tex_group));
	if ( gstack == NULL ) {
		fprintf(stderr, "%s: stack out of memory", PROGNAME);
	exit(3);
    }
    }
    
    if ( (gstack[gstackp].s_name =
		(unsigned char *)malloc(strlen(p_name) + 1)) == NULL ||
         (gstack[gstackp].s_file =
		(char *)malloc(strlen(file_name) + 1)) == NULL ) {
	fprintf(stderr, "%s: out of memory\n", PROGNAME);
	exit(3);
    }

    strcpy(gstack[gstackp].s_name,p_name);
    gstack[gstackp].s_type = p_type;
    gstack[gstackp].s_line = p_line;	
    strcpy(gstack[gstackp].s_file,file_name);
    ++gstackp;	

}

void input_file(file_nam)
char *file_nam;
{
    char *tmp_file_name;
    FILE *tmp_yyin;

    if ( (tmp_file_name = malloc(strlen(file_nam) + 5)) == NULL ) {
	fprintf(stderr, "%s: out of memory\n", PROGNAME);
	exit(3);
    }
    strcpy(tmp_file_name,file_nam);

    if (istackp == istack_size) {	/* extend stack */
	istack_size *= 2;
	istack = (input_ *)realloc(istack, istack_size * sizeof(input_));
	if ( istack == NULL ) {
		fprintf(stderr, "%s: \\input stack out of memory\n", PROGNAME);
	exit(3);
        } 
    } 
    	
    istack[istackp].stream = YY_CURRENT_BUFFER;
    istack[istackp].linenum = line_count;
    istack[istackp].name = file_name;
    ++istackp;    

    if ((tmp_yyin = fopen( file_nam, "r")) != NULL )
	{
	  yyin = tmp_yyin;
	  yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	  file_name = tmp_file_name;
	  push(file_name, 3, 1);
          line_count = 1;
	}
    else {
	  (void) strcat(tmp_file_name, ".tex");
	  if ((tmp_yyin = fopen( tmp_file_name , "r")) != NULL )
	    {
		yyin = tmp_yyin;
	   	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
		file_name = tmp_file_name;
		push(file_name, 3, 1);
   	        line_count = 1;
	    }
          else
	  {
	       --istackp;
	       fclose(tmp_yyin);
	       free(tmp_file_name);
	       printf("\"%s\", line %d: Could not open \"%s\"\n", 
			file_name,
			line_count,
			file_nam);
	       input();
	  }
	 }
}

void pop()
{
    if ( gstackp == 0 )
    {
       	fprintf(stderr, "%s: Stack underflow\n", PROGNAME);
	exit(4);
    }
    --gstackp;

    free(gstack[gstackp].s_name);
    free(gstack[gstackp].s_file);
}

char *bg_command(name)
char *name;
{
    
    switch (CG_TYPE) {
	
    case 2:
	(void) strcpy( returnval, "\\begin\{" );
	(void) strcat( returnval, (char *) name);
	(void) strcat( returnval, "}" );
	break;
	
    case 3:
	(void) strcpy( returnval, "beginning of file " );
	(void) strcat( returnval, (char *) name);
	break;
    
    case 4:
	(void) strcpy( returnval, "math begin " );
	(void) strcat( returnval, (char *) name);
	break;
    
    case 5:
	(void) strcpy( returnval, "display math begin " );
	(void) strcat( returnval, (char *) name);
	break;
    
    default:
    	(void) strcpy( returnval, name );
	
    }
    
    return ((char *)returnval);
}

char *eg_command(name,type)
int type;
char *name;
{
    
    switch (type) {
	
    case 2:
	(void) strcpy( returnval, "\\end{" );
	(void) strcat( returnval, (char *) name);
	(void) strcat( returnval, "}" );
	break;
	
    case 3:
	(void) strcpy( returnval, "end of file " );
	(void) strcat( returnval, (char *) name);
	break;
    
    case 4:
	(void) strcpy( returnval, "math end " );
	(void) strcat( returnval, (char *) name);
	break;
    
    case 5:
	(void) strcpy( returnval, "display math end " );
	(void) strcat( returnval, (char *) name);
	break;
    
    default:
    	(void) strcpy( returnval, name );
	break;
    }
    
    return ((char *)returnval);
}


void g_checkend(n)
int n;
{
    if ( check_top_level_end(yytext,n) == 1 ) 
       if (  CG_TYPE != n  )
	 print_bad_match(yytext,n);
       else
	pop();
}

void e_checkend(n, name)
int n;
char *name;
{
   if ( check_top_level_end(name,n) == 1 )
    {
     if (  CG_TYPE != n  ||  strcmp( CG_NAME, name ) != 0 )
    	print_bad_match(name,n);

     pop();

    }
}

void f_checkend(name)
char *name;
{
    if ( check_top_level_end(name,3) == 1 )
     {
       if (  CG_TYPE != 3  ||  strcmp( CG_NAME, name ) != 0 )

    	while( CG_TYPE != 3  )
	{
	  print_bad_match(name,3);
          pop();
        }

         pop();  
     }
}

void print_bad_match(end_command,type)
char *end_command;	      
int type;
{
	  printf("\"%s\", line %i: <- unmatched \"%s\"\n",
	         file_name, 
		 line_count, 
		 eg_command( end_command , type) ) ;

	  printf("\"%s\", line %i: -> unmatched \"%s\"\n",
	         CG_FILE, 
		 CG_LINE, 
		 bg_command( CG_NAME ) ) ;
	  warn_count += 2;
}

int check_top_level_end(end_command,type)
char *end_command;	      
int type;
{
    if ( gstackp == 0 )
	{
	 printf("\"%s\", line %i: \"%s\" found at top level\n",
	       file_name, 
	       line_count, 
	       eg_command( end_command, type )) ;
	 ++warn_count;
         return(0);
	}
    else
    	return(1);
}



