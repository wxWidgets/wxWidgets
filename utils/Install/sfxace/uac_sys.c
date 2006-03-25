/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Some basic things.                                                  */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include "os.h"

#include <signal.h>  // signal()
#include <stdio.h>   // fprintf() fflush() getch() putc()

#include "globals.h"
#include "uac_sys.h"


void memset16(USHORT * dest, SHORT val, INT len)  // fills short-array with
{                                                 // value
	while (len--)
		*(dest++) = val;
}

INT  cancel(void)               // checks whether to interrupt the program
{
#ifdef DOS
	while (kbhit())
	{
		if (getch() == 27)
			f_err = ERR_USER;
	}
#endif
	return (f_err);
}

/*INT  wrask(CHAR * s)            // prompt-routine
{
   INT  ch;

   fprintf(stderr, "\n %s (Yes,Always,No,Cancel) ", s);
   fflush(stderr);
   do
   {
      ch = getch();
      ch = upcase(ch);
   }
   while (ch != 'Y' && ch != 'A' && ch != 'N' && ch != 'C' && ch != 27);
   fprintf(stderr, "%s", ch == 'Y' ? "Yes" : (ch == 'A' ? "Always" : (ch == 'N' ? "No" : "Cancel")));
   fflush(stderr);
   return (ch == 'Y' ? 0 : (ch == 'A' ? 1 : (ch == 'N' ? 2 : 3)));
} */

void beep(void)                           // makes some noise
{
#ifdef DOS
	sound(800);
	delay(250);
	nosound();
#endif
#ifdef AMIGA
	putc(0x07, stderr);
#endif
}

void my_signalhandler(INT sig_number)     // sets f_err if ctrl+c or ctrl+brk
{
	f_err = ERR_USER;
	error("\nUser break\n");
}

#ifdef DOS                                // handles hardware errors
#ifdef __BORLANDC__
INT harderrhandler(UINT deverr, UINT errc, UINT * devhdr)
#else
INT __far harderrhandler(UINT deverr, UINT errc, UINT * devhdr)
#endif
{
   f_criterr = 'A' + deverr & 0xff;
   f_err = ERR_OTHER;
   return (0x3);
}
#endif

void set_handler(void)                    // initializes handlers
{
#if defined(DOS) && !defined(__BORLANDC__)
	signal(SIGBREAK, my_signalhandler);    // set ctrl-break/-c handlers
#endif
	signal(SIGINT, my_signalhandler);
#if defined(DOS) && !defined(__CONSOLE__) // set hardware error handler
#ifdef __BORLANDC__
	harderr(harderrhandler);
#else
	_harderr(harderrhandler);
#endif
#endif
}

