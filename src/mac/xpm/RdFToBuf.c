/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
* RdFToBuf.c:                                                                 *
*                                                                             *
*  XPM library                                                                *
*  Copy a file to a malloc'ed buffer, provided as a convenience.              *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

#include "wx/setup.h"

#ifdef macintosh
	#ifdef __std
		#undef __std
		#define __std()
	#endif
	#include <stat.h>
	#include <unix.h>
    #include <unistd.h>
	#include <fcntl.h>
#endif

#include "XpmI.h"
#ifndef macintosh
	#include <sys/stat.h>

	#if !defined(FOR_MSW) && !defined(WIN32)
	#include <unistd.h>
	#endif
	#ifndef VAX11C
	#include <fcntl.h>
	#endif
	#if defined(FOR_MSW) || defined(WIN32)
	#include <io.h>
	#define stat _stat
	#define fstat _fstat
	#define fdopen _fdopen
	#define O_RDONLY _O_RDONLY
	#endif
#endif

int
XpmReadFileToBuffer(filename, buffer_return)
    char *filename;
    char **buffer_return;
{
    int fd, fcheck, len;
    char *ptr;
    struct stat stats;
    FILE *fp;

    *buffer_return = NULL;

#ifndef VAX11C
    fd = open(filename, O_RDONLY);
#else
    fd = open(filename, O_RDONLY, NULL);
#endif
    if (fd < 0)
	return XpmOpenFailed;

    if (fstat(fd, &stats)) {
	close(fd);
	return XpmOpenFailed;
    }
    fp = fdopen(fd, "r");
    if (!fp) {
	close(fd);
	return XpmOpenFailed;
    }
    len = (int) stats.st_size;
    ptr = (char *) XpmMalloc(len + 1);
    if (!ptr) {
	fclose(fp);
	return XpmNoMemory;
    }
    fcheck = fread(ptr, 1, len, fp);
    fclose(fp);
#ifdef VMS
    /* VMS often stores text files in a variable-length record format,
       where there are two bytes of size followed by the record.  fread	
       converts this so it looks like a record followed by a newline.	
       Unfortunately, the size reported by fstat() (and fseek/ftell)	
       counts the two bytes for the record terminator, while fread()	
       counts only one.  So, fread() sees fewer bytes in the file (size	
       minus # of records) and thus when asked to read the amount	
       returned by stat(), it fails.
       The best solution, suggested by DEC, seems to consider the length
       returned from fstat() as an upper bound and call fread() with
       a record length of 1. Then don't check the return value.
       We'll check for 0 for gross error that's all.
    */
    len = fcheck;
    if (fcheck == 0) {
#else
    if (fcheck != len) {
#endif
	XpmFree(ptr);
	return XpmOpenFailed;
    }
    ptr[len] = '\0';
    *buffer_return = ptr;
    return XpmSuccess;
}
