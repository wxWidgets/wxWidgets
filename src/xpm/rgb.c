/*
 * Copyright (C) 1989-94 GROUPE BULL
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

#include "xpm34p.h"
#ifdef VMS
#include "sys$library:ctype.h"
#include "sys$library:string.h"
#else
#include <ctype.h>
#if defined(SYSV) || defined(SVR4)
#include <string.h>
#else
#include <strings.h>
#endif
#endif

#ifndef FOR_MSW				/* normal part first, MSW part at
					 * the end, (huge ifdef!) */
/*
 * Read a rgb text file.  It stores the rgb values (0->65535)
 * and the rgb mnemonics (malloc'ed) into the "rgbn" array.  Returns the
 * number of entries stored.
 */
int
xpmReadRgbNames(char *rgb_fname, xpmRgbName rgbn[])
{
    FILE *rgbf;
    int i, items, red, green, blue;
    char line[512], name[512], *rgbname, *n, *m;
    xpmRgbName *rgb;

    /* Open the rgb text file.  Abort if error. */
    if ((rgbf = fopen(rgb_fname, "r")) == NULL)
	return 0;

    /* Loop reading each line in the file. */
    for (i = 0, rgb = rgbn; fgets(line, sizeof(line), rgbf); i++, rgb++) {

	/* Quit if rgb text file is too large. */
	if (i == MAX_RGBNAMES) {
	    /* Too many entries in rgb text file, give up here */
	    break;
	}
	/* Read the line.  Skip silently if bad. */
	items = sscanf(line, "%d %d %d %[^\n]\n", &red, &green, &blue, name);
	if (items != 4) {
	    i--;
	    continue;
	}

	/*
	 * Make sure rgb values are within 0->255 range. Skip silently if
	 * bad.
	 */
	if (red < 0 || red > 0xFF ||
	    green < 0 || green > 0xFF ||
	    blue < 0 || blue > 0xFF) {
	    i--;
	    continue;
	}
	/* Allocate memory for ascii name. If error give up here. */
	if (!(rgbname = (char *) XpmMalloc(strlen(name) + 1)))
	    break;

	/* Copy string to ascii name and lowercase it. */
	for (n = name, m = rgbname; *n; n++)
	    *m++ = tolower(*n);
	*m = '\0';

	/* Save the rgb values and ascii name in the array. */
	rgb->r = red * 257;		/* 65535/255 = 257 */
	rgb->g = green * 257;
	rgb->b = blue * 257;
	rgb->name = rgbname;
    }

    fclose(rgbf);

    /* Return the number of read rgb names. */
    return i < 0 ? 0 : i;
}

/*
 * Return the color name corresponding to the given rgb values
 */
char *
xpmGetRgbName(xpmRgbName rgbn[], int rgbn_max, int red, int green, int blue)
/*    xpmRgbName rgbn[]; */			/* rgb mnemonics from rgb text file */
/*    int rgbn_max; */			/* number of rgb mnemonics in table */
/*    int red, green, blue; */		/* rgb values */

{
    int i;
    xpmRgbName *rgb;

    /*
     * Just perform a dumb linear search over the rgb values of the color
     * mnemonics.  One could speed things up by sorting the rgb values and
     * using a binary search, or building a hash table, etc...
     */
    for (i = 0, rgb = rgbn; i < rgbn_max; i++, rgb++)
	if (red == rgb->r && green == rgb->g && blue == rgb->b)
	    return rgb->name;

    /* if not found return NULL */
    return NULL;
}

/*
 * Free the strings which have been malloc'ed in xpmReadRgbNames
 */
void
xpmFreeRgbNames(xpmRgbName rgbn[], int rgbn_max)
{
    int i;
    xpmRgbName *rgb;

    for (i = 0, rgb = rgbn; i < rgbn_max; i++, rgb++)
	XpmFree(rgb->name);
}

#else					/* here comes the MSW part, the
					 * second part of the  huge ifdef */

#include "rgbtab.h"			/* hard coded rgb.txt table */

int
xpmReadRgbNames(char *rgb_fname, xpmRgbName rgbn[])
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
char *
xpmGetRgbName(xpmRgbName rgbn[], int rgbn_max, int red, int green, int blue)
/*    xpmRgbName rgbn[]; */			/* rgb mnemonics from rgb text file
					 * not used */
/*    int rgbn_max; */			/* not used */
/*    int red, green, blue; */		/* rgb values */

{
    int i;
    unsigned long rgbVal;

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
xpmGetRGBfromName(char *inname, int *r, int *g, int *b)
{
    int left, right, middle;
    int cmp;
    unsigned long rgbVal;
    char *name;
    char *grey, *p;

    name = strdup(inname);

    /*
     * the table in rgbtab.c has no names with spaces, and no grey, but a
     * lot of gray
     */
    /* so first extract ' ' */
    while (p = strchr(name, ' ')) {
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
    if (grey = strstr(name, "grey"))
	grey[2] = 'a';

    /* binary search */
    left = 0;
    right = numTheRGBRecords - 1;
    do {
	middle = (left + right) / 2;
	cmp = strcasecmp(name, theRGBRecords[middle].name);
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

    *r = 255;
    *g = 0;
    *b = 0;				/* red error pixel */

    free(name);
    return (1);
}

void
xpmFreeRgbNames(xpmRgbName rgbn[], int rgbn_max)
{
    /* nothing to do */
}

#endif					/* MSW part */
