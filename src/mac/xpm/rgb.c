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
* rgb.c:                                                                      *
*                                                                             *
*  XPM library                                                                *
*  Rgb file utilities                                                         *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

/*
 * Part of this code has been taken from the ppmtoxpm.c file written by Mark
 * W. Snitily but has been modified for my special need
 */

#include "XpmI.h"
#include <ctype.h>

#include "rgbtab.h"			/* hard coded rgb.txt table */

int
xpmReadRgbNames(rgb_fname, rgbn)
    char *rgb_fname;
    xpmRgbName rgbn[];
{
    /*
     * check for consistency???
     * table has to be sorted for calls on strcasecmp
     */
    return (numTheRGBRecords);
}

/*
 * MSW rgb values are made from 3 BYTEs, this is different from X XColor.red,
 * which has something like #0303 for one color
 */
 
/* portable rgb values have 16 bit unsigned value range for each color these are promoted from the table */

char *
xpmGetRgbName(rgbn, rgbn_max, red, green, blue)
    xpmRgbName rgbn[];			/* rgb mnemonics from rgb text file
					 * not used */
    int rgbn_max;			/* not used */
    int red, green, blue;		/* rgb values */

{
    int i;
    RGBColor rgbVal;

    i = 0;
    while (i < numTheRGBRecords) {
	rgbVal = theRGBRecords[i].rgb;
	if (GetRValue(rgbVal) == red &&
	    GetGValue(rgbVal) == green &&
	    GetBValue(rgbVal) == blue)
	    return (theRGBRecords[i].name);
	i++;
    }
    return (NULL);
}

/* used in XParseColor in simx.c */
int
xpmGetRGBfromName(inname, r, g, b)
    char *inname;
    int *r, *g, *b;
{
    int left, right, middle;
    int cmp;
    RGBColor rgbVal;
    char *name;
    char *grey, *p;

    name = xpmstrdup(inname);

    /*
     * the table in rgbtab.c has no names with spaces, and no grey, but a
     * lot of gray
     */
    /* so first extract ' ' */
    while ((p = strchr(name, ' '))!=NULL) {
	while (*(p)) {			/* till eof of string */
	    *p = *(p + 1);		/* copy to the left */
	    p++;
	}
    }
    /* fold to lower case */
    p = name;
    while (*p) {
	*p = tolower(*p);
	p++;
    }

    /*
     * substitute Grey with Gray, else rgbtab.h would have more than 100
     * 'duplicate' entries
     */
    if ((grey = strstr(name, "grey"))!=NULL)
	grey[2] = 'a';

    /* binary search */
    left = 0;
    right = numTheRGBRecords - 1;
    do {
	middle = (left + right) / 2;
	cmp = xpmstrcasecmp(name, theRGBRecords[middle].name);
	if (cmp == 0) {
	    rgbVal = theRGBRecords[middle].rgb;
	    *r = GetRValue(rgbVal);
	    *g = GetGValue(rgbVal);
	    *b = GetBValue(rgbVal);
	    free(name);
	    return (1);
	} else if (cmp < 0) {
	    right = middle - 1;
	} else {			/* > 0 */
	    left = middle + 1;
	}
    } while (left <= right);

    /*
     * I don't like to run in a ColorInvalid error and to see no pixmap at
     * all, so simply return a red pixel. Should be wrapped in an #ifdef
     * HeDu
     */

#ifdef FOR_MSW
    *r = 255;
    *g = 0;
    *b = 0;				/* red error pixel */
#else
    *r = 0xFFFF;
    *g = 0;
    *b = 0;				/* red error pixel */
#endif
    free(name);
    return (1);
}

void
xpmFreeRgbNames(rgbn, rgbn_max)
    xpmRgbName rgbn[];
    int rgbn_max;

{
    /* nothing to do */
}
