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
* misc.c:                                                                     *
*                                                                             *
*  XPM library                                                                *
*  Miscellaneous utilities                                                    *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "XpmI.h"

#ifdef NEED_STRDUP
/*
 * in case strdup is not provided by the system here is one
 * which does the trick
 */
char *
xpmstrdup(s1)
    char *s1;
{
    char *s2;
    int l = strlen(s1) + 1;

    if (s2 = (char *) XpmMalloc(l))
	strcpy(s2, s1);
    return s2;
}

#endif

unsigned int
xpmatoui(p, l, ui_return)
    register char *p;
    unsigned int l;
    unsigned int *ui_return;
{
    register unsigned int n, i;

    n = 0;
    for (i = 0; i < l; i++)
	if (*p >= '0' && *p <= '9')
	    n = n * 10 + *p++ - '0';
	else
	    break;

    if (i != 0 && i == l) {
	*ui_return = n;
	return 1;
    } else
	return 0;
}

/*
 * Function returning a character string related to an error code.
 */
char *
XpmGetErrorString(errcode)
    int errcode;
{
    switch (errcode) {
    case XpmColorError:
	return ("XpmColorError");
    case XpmSuccess:
	return ("XpmSuccess");
    case XpmOpenFailed:
	return ("XpmOpenFailed");
    case XpmFileInvalid:
	return ("XpmFileInvalid");
    case XpmNoMemory:
	return ("XpmNoMemory");
    case XpmColorFailed:
	return ("XpmColorFailed");
    default:
	return ("Invalid XpmError");
    }
}

/*
 * The following function provides a way to figure out if the linked library is
 * newer or older than the one with which a program has been first compiled.
 */
int
XpmLibraryVersion()
{
    return XpmIncludeVersion;
}


/* The following should help people wanting to use their own functions */
#ifdef XpmFree
#undef XpmFree
#endif

void
XpmFree(ptr)
    void *ptr;
{
    free(ptr);
}
