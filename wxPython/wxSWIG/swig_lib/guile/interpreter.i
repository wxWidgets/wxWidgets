//
// $Header$
//
// SWIG file for a simple Guile interpreter
//
/* Revision History
 * $Log$
 * Revision 1.1  2002/04/29 19:56:52  RD
 * Since I have made several changes to SWIG over the years to accomodate
 * special cases and other things in wxPython, and since I plan on making
 * several more, I've decided to put the SWIG sources in wxPython's CVS
 * instead of relying on maintaining patches.  This effectivly becomes a
 * fork of an obsolete version of SWIG, :-( but since SWIG 1.3 still
 * doesn't have some things I rely on in 1.1, not to mention that my
 * custom patches would all have to be redone, I felt that this is the
 * easier road to take.
 *
 * Revision 1.1.1.1  1999/02/28 02:00:54  beazley
 * Swig1.1
 *
 * Revision 1.1  1996/05/22 20:02:10  beazley
 * Initial revision
 *
 */

%{

#include <stdio.h>
GSCM_status guile_init();

int main(int argc, char **argv) {
  GSCM_status status;
  GSCM_top_level toplev;
  char *eval_answer;
  char input_str[16384];
  int done;

  /* start a scheme interpreter */
  status = gscm_run_scm(argc, argv, 0, stdout, stderr, guile_init, 0, "#t");
  if (status != GSCM_OK) {
    fputs(gscm_error_msg(status), stderr);
    fputc('\n', stderr);
    printf("Error in startup.\n");
    exit(1);
  }

  /* create the top level environment */
  status = gscm_create_top_level(&toplev);
  if (status != GSCM_OK) {
    fputs(gscm_error_msg(status), stderr);
    fputc('\n', stderr);
    exit(1);
  }

  /* now sit in a scheme eval loop: I input the expressions, have guile
   * evaluate them, and then get another expression.
   */
  done = 0;
  fprintf(stdout,"Guile > ");
  while (!done) {
    if (fgets(input_str,16384,stdin) == NULL) {
      exit(1);
    } else {
      if (strncmp(input_str,"quit",4) == 0) exit(1);
      status = gscm_eval_str(&eval_answer, toplev, input_str);
      fprintf(stdout,"%s\n", eval_answer);
      fprintf(stdout,"Guile > ");
    }
  }

  /* now clean up and quit */
  gscm_destroy_top_level(toplev);
}

%}



