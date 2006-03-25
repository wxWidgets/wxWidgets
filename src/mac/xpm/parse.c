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
* parse.c:                                                                    *
*                                                                             *
*  XPM library                                                                *
*  Parse an XPM file or array and store the found informations                *
*  in the given XpmImage structure.                                           *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

#include "xpmi.h"
#include <ctype.h>

LFUNC(ParsePixels, int, (xpmData *data, unsigned int width,
			 unsigned int height, unsigned int ncolors,
			 unsigned int cpp, XpmColor *colorTable,
			 xpmHashTable *hashtable, unsigned int **pixels));

char *xpmColorKeys[] = {
    "s",				/* key #1: symbol */
    "m",				/* key #2: mono visual */
    "g4",				/* key #3: 4 grays visual */
    "g",				/* key #4: gray visual */
    "c",				/* key #5: color visual */
};

int
xpmParseValues(data, width, height, ncolors, cpp,
	    x_hotspot, y_hotspot, hotspot, extensions)
    xpmData *data;
    unsigned int *width, *height, *ncolors, *cpp;
    unsigned int *x_hotspot, *y_hotspot, *hotspot;
    unsigned int *extensions;
{
    unsigned int l;
    char buf[BUFSIZ];

    if (!data->format) {		/* XPM 2 or 3 */

	/*
	 * read values: width, height, ncolors, chars_per_pixel
	 */
	if (!(xpmNextUI(data, width) && xpmNextUI(data, height)
	      && xpmNextUI(data, ncolors) && xpmNextUI(data, cpp)))
	    return (XpmFileInvalid);

	/*
	 * read optional information (hotspot and/or XPMEXT) if any
	 */
	l = xpmNextWord(data, buf, BUFSIZ);
	if (l) {
	    *extensions = (l == 6 && !strncmp("XPMEXT", buf, 6));
	    if (*extensions)
		*hotspot = (xpmNextUI(data, x_hotspot)
			    && xpmNextUI(data, y_hotspot));
	    else {
		*hotspot = (xpmatoui(buf, l, x_hotspot)
			    && xpmNextUI(data, y_hotspot));
		l = xpmNextWord(data, buf, BUFSIZ);
		*extensions = (l == 6 && !strncmp("XPMEXT", buf, 6));
	    }
	}
    } else {

	/*
	 * XPM 1 file read values: width, height, ncolors, chars_per_pixel
	 */
	int i;
	char *ptr;
	Bool got_one, saw_width = False, saw_height = False;
	Bool saw_ncolors = False, saw_chars_per_pixel = False;

	for (i = 0; i < 4; i++) {
	    l = xpmNextWord(data, buf, BUFSIZ);
	    if (l != 7 || strncmp("#define", buf, 7))
			return (XpmFileInvalid);
	    l = xpmNextWord(data, buf, BUFSIZ);
	    if (!l)
			return (XpmFileInvalid);
	    buf[l] = '\0';
	    ptr = buf;
	    got_one = False;
	    while (!got_one) {
	    #ifdef macintosh // we have a strange parameter problem here
		ptr = strchr(ptr, '_'); // index
	    #else
		ptr = index(ptr, '_'); 
		#endif
		if (!ptr)
		    return (XpmFileInvalid);
		switch (l - (ptr - buf)) {
		case 6:
		    if (saw_width || strncmp("_width", ptr, 6)
			|| !xpmNextUI(data, width))
			return (XpmFileInvalid);
		    else
			saw_width = True;
		    got_one = True;
		    break;
		case 7:
		    if (saw_height || strncmp("_height", ptr, 7)
			|| !xpmNextUI(data, height))
			return (XpmFileInvalid);
		    else
			saw_height = True;
		    got_one = True;
		    break;
		case 8:
		    if (saw_ncolors || strncmp("_ncolors", ptr, 8)
			|| !xpmNextUI(data, ncolors))
			return (XpmFileInvalid);
		    else
			saw_ncolors = True;
		    got_one = True;
		    break;
		case 16:
		    if (saw_chars_per_pixel
			|| strncmp("_chars_per_pixel", ptr, 16)
			|| !xpmNextUI(data, cpp))
			return (XpmFileInvalid);
		    else
			saw_chars_per_pixel = True;
		    got_one = True;
		    break;
		default:
		    ptr++;
		}
	    }
	    /* skip the end of line */
	    xpmNextString(data);
	}
	if (!saw_width || !saw_height || !saw_ncolors || !saw_chars_per_pixel)
	  return (XpmFileInvalid);

	*hotspot = 0;
	*extensions = 0;
    }
    return (XpmSuccess);
}

int
xpmParseColors(data, ncolors, cpp, colorTablePtr, hashtable)
    xpmData *data;
    unsigned int ncolors;
    unsigned int cpp;
    XpmColor **colorTablePtr;
    xpmHashTable *hashtable;
{
    unsigned int key, l, a, b;
    unsigned int curkey;		/* current color key */
    unsigned int lastwaskey;		/* key read */
    char buf[BUFSIZ];
    char curbuf[BUFSIZ];		/* current buffer */
    char **sptr, *s;
    XpmColor *color;
    XpmColor *colorTable;
    char **defaults;
    int ErrorStatus;

    colorTable = (XpmColor *) XpmCalloc(ncolors, sizeof(XpmColor));
    if (!colorTable)
	return (XpmNoMemory);

    if (!data->format) {		/* XPM 2 or 3 */
	for (a = 0, color = colorTable; a < ncolors; a++, color++) {
	    xpmNextString(data);	/* skip the line */

	    /*
	     * read pixel value
	     */
	    color->string = (char *) XpmMalloc(cpp + 1);
	    if (!color->string) {
		xpmFreeColorTable(colorTable, ncolors);
		return (XpmNoMemory);
	    }
	    for (b = 0, s = color->string; b < cpp; b++, s++)
		*s = xpmGetC(data);
	    *s = '\0';

	    /*
	     * store the string in the hashtable with its color index number
	     */
	    if (USE_HASHTABLE) {
		ErrorStatus =
		    xpmHashIntern(hashtable, color->string, HashAtomData(a));
		if (ErrorStatus != XpmSuccess) {
		    xpmFreeColorTable(colorTable, ncolors);
		    return (ErrorStatus);
		}
	    }

	    /*
	     * read color keys and values
	     */
	    defaults = (char **) color;
	    curkey = 0;
	    lastwaskey = 0;
	    *curbuf = '\0';		/* init curbuf */
	    while ((l = xpmNextWord(data, buf, BUFSIZ))!=NULL ) {
		if (!lastwaskey) {
		    for (key = 0, sptr = xpmColorKeys; key < NKEYS; key++,
			 sptr++)
			if ((strlen(*sptr) == l) && (!strncmp(*sptr, buf, l)))
			    break;
		}
		if (!lastwaskey && key < NKEYS) {	/* open new key */
		    if (curkey) {	/* flush string */
			s = (char *) XpmMalloc(strlen(curbuf) + 1);
			if (!s) {
			    xpmFreeColorTable(colorTable, ncolors);
			    return (XpmNoMemory);
			}
			defaults[curkey] = s;
			strcpy(s, curbuf);
		    }
		    curkey = key + 1;	/* set new key  */
		    *curbuf = '\0';	/* reset curbuf */
		    lastwaskey = 1;
		} else {
		    if (!curkey) {	/* key without value */
			xpmFreeColorTable(colorTable, ncolors);
			return (XpmFileInvalid);
		    }
		    if (!lastwaskey)
			strcat(curbuf, " ");	/* append space */
		    buf[l] = '\0';
		    strcat(curbuf, buf);/* append buf */
		    lastwaskey = 0;
		}
	    }
	    if (!curkey) {		/* key without value */
		xpmFreeColorTable(colorTable, ncolors);
		return (XpmFileInvalid);
	    }
	    s = defaults[curkey] = (char *) XpmMalloc(strlen(curbuf) + 1);
	    if (!s) {
		xpmFreeColorTable(colorTable, ncolors);
		return (XpmNoMemory);
	    }
	    strcpy(s, curbuf);
	}
    } else {				/* XPM 1 */
	/* get to the beginning of the first string */
	data->Bos = '"';
	data->Eos = '\0';
	xpmNextString(data);
	data->Eos = '"';
	for (a = 0, color = colorTable; a < ncolors; a++, color++) {

	    /*
	     * read pixel value
	     */
	    color->string = (char *) XpmMalloc(cpp + 1);
	    if (!color->string) {
		xpmFreeColorTable(colorTable, ncolors);
		return (XpmNoMemory);
	    }
	    for (b = 0, s = color->string; b < cpp; b++, s++)
		*s = xpmGetC(data);
	    *s = '\0';

	    /*
	     * store the string in the hashtable with its color index number
	     */
	    if (USE_HASHTABLE) {
		ErrorStatus =
		    xpmHashIntern(hashtable, color->string, HashAtomData(a));
		if (ErrorStatus != XpmSuccess) {
		    xpmFreeColorTable(colorTable, ncolors);
		    return (ErrorStatus);
		}
	    }

	    /*
	     * read color values
	     */
	    xpmNextString(data);	/* get to the next string */
	    *curbuf = '\0';		/* init curbuf */
	    while ((l = xpmNextWord(data, buf, BUFSIZ))!=NULL ) {
		if (*curbuf != '\0')
		    strcat(curbuf, " ");/* append space */
		buf[l] = '\0';
		strcat(curbuf, buf);	/* append buf */
	    }
	    s = (char *) XpmMalloc(strlen(curbuf) + 1);
	    if (!s) {
		xpmFreeColorTable(colorTable, ncolors);
		return (XpmNoMemory);
	    }
	    strcpy(s, curbuf);
	    color->c_color = s;
	    *curbuf = '\0';		/* reset curbuf */
	    if (a < ncolors - 1)
		xpmNextString(data);	/* get to the next string */
	}
    }
    *colorTablePtr = colorTable;
    return (XpmSuccess);
}

static int
ParsePixels(data, width, height, ncolors, cpp, colorTable, hashtable, pixels)
    xpmData *data;
    unsigned int width;
    unsigned int height;
    unsigned int ncolors;
    unsigned int cpp;
    XpmColor *colorTable;
    xpmHashTable *hashtable;
    unsigned int **pixels;
{
    unsigned int *iptr, *iptr2;
    unsigned int a, x, y;

#ifndef FOR_MSW
    iptr2 = (unsigned int *) XpmMalloc(sizeof(unsigned int) * width * height);
#else

    /*
     * special treatment to trick DOS malloc(size_t) where size_t is 16 bit!!
     * XpmMalloc is defined to longMalloc(long) and checks the 16 bit boundary
     */
    iptr2 = (unsigned int *)
	XpmMalloc((long) sizeof(unsigned int) * (long) width * (long) height);
#endif
    if (!iptr2)
	return (XpmNoMemory);

    iptr = iptr2;

    switch (cpp) {

    case (1):				/* Optimize for single character
					 * colors */
	{
	    unsigned short colidx[256];

	    bzero((char *)colidx, 256 * sizeof(short));
	    for (a = 0; a < ncolors; a++)
		colidx[(unsigned char)colorTable[a].string[0]] = a + 1;

	    for (y = 0; y < height; y++) {
		xpmNextString(data);
		for (x = 0; x < width; x++, iptr++) {
		    int c = xpmGetC(data);

		    if (c > 0 && c < 256 && colidx[c] != 0)
			*iptr = colidx[c] - 1;
		    else {
			XpmFree(iptr2);
			return (XpmFileInvalid);
		    }
		}
	    }
	}
	break;

    case (2):				/* Optimize for double character
					 * colors */
	{

/* free all allocated pointers at all exits */
#define FREE_CIDX {int f; for (f = 0; f < 256; f++) \
if (cidx[f]) XpmFree(cidx[f]);}

	    /* array of pointers malloced by need */
	    unsigned short *cidx[256];
	    int char1;

	    bzero((char *)cidx, 256 * sizeof(unsigned short *)); /* init */
	    for (a = 0; a < ncolors; a++) {
		char1 = colorTable[a].string[0];
		if (cidx[char1] == NULL) { /* get new memory */
		    cidx[char1] = (unsigned short *)
			XpmCalloc(256, sizeof(unsigned short));
		    if (cidx[char1] == NULL) { /* new block failed */
			FREE_CIDX;
			XpmFree(iptr2);
			return (XpmNoMemory);
		    }
		}
		cidx[char1][(unsigned char)colorTable[a].string[1]] = a + 1;
	    }

	    for (y = 0; y < height; y++) {
		xpmNextString(data);
		for (x = 0; x < width; x++, iptr++) {
		    int cc1 = xpmGetC(data);
		    if (cc1 > 0 && cc1 < 256) {
			int cc2 = xpmGetC(data);
			if (cc2 > 0 && cc2 < 256 &&
			    cidx[cc1] && cidx[cc1][cc2] != 0)
			    *iptr = cidx[cc1][cc2] - 1;
			else {
			    FREE_CIDX;
			    XpmFree(iptr2);
			    return (XpmFileInvalid);
			}
		    } else {
			FREE_CIDX;
			XpmFree(iptr2);
			return (XpmFileInvalid);
		    }
		}
	    }
	    FREE_CIDX;
	}
	break;

    default:				/* Non-optimized case of long color
					 * names */
	{
	    char *s;
	    char buf[BUFSIZ];

	    buf[cpp] = '\0';
	    if (USE_HASHTABLE) {
		xpmHashAtom *slot;

		for (y = 0; y < height; y++) {
		    xpmNextString(data);
		    for (x = 0; x < width; x++, iptr++) {
			for (a = 0, s = buf; a < cpp; a++, s++)
			    *s = xpmGetC(data);
			slot = xpmHashSlot(hashtable, buf);
			if (!*slot) {	/* no color matches */
			    XpmFree(iptr2);
			    return (XpmFileInvalid);
			}
			*iptr = HashColorIndex(slot);
		    }
		}
	    } else {
		for (y = 0; y < height; y++) {
		    xpmNextString(data);
		    for (x = 0; x < width; x++, iptr++) {
			for (a = 0, s = buf; a < cpp; a++, s++)
			    *s = xpmGetC(data);
			for (a = 0; a < ncolors; a++)
			    if (!strcmp(colorTable[a].string, buf))
				break;
			if (a == ncolors) {	/* no color matches */
			    XpmFree(iptr2);
			    return (XpmFileInvalid);
			}
			*iptr = a;
		    }
		}
	    }
	}
	break;
    }
    *pixels = iptr2;
    return (XpmSuccess);
}

int
xpmParseExtensions(data, extensions, nextensions)
    xpmData *data;
    XpmExtension **extensions;
    unsigned int *nextensions;
{
    XpmExtension *exts = NULL, *ext;
    unsigned int num = 0;
    unsigned int nlines, a, l, notstart, notend = 0;
    int status;
    char *string, *s, *s2, **sp;

    xpmNextString(data);
    exts = (XpmExtension *) XpmMalloc(sizeof(XpmExtension));
    /* get the whole string */
    status = xpmGetString(data, &string, &l);
    if (status != XpmSuccess) {
	XpmFree(exts);
	return (status);
    }
    /* look for the key word XPMEXT, skip lines before this */
    while ((notstart = strncmp("XPMEXT", string, 6))!=NULL 
	   && (notend = strncmp("XPMENDEXT", string, 9))!=NULL ) {
	XpmFree(string);
	xpmNextString(data);
	status = xpmGetString(data, &string, &l);
	if (status != XpmSuccess) {
	    XpmFree(exts);
	    return (status);
	}
    }
    if (!notstart)
	notend = strncmp("XPMENDEXT", string, 9);
    while (!notstart && notend) {
	/* there starts an extension */
	ext = (XpmExtension *)
	    XpmRealloc(exts, (num + 1) * sizeof(XpmExtension));
	if (!ext) {
	    XpmFree(string);
	    XpmFreeExtensions(exts, num);
	    return (XpmNoMemory);
	}
	exts = ext;
	ext += num;
	/* skip whitespace and store its name */
	s2 = s = string + 6;
	while (isspace(*s2))
	    s2++;
	a = s2 - s;
	ext->name = (char *) XpmMalloc(l - a - 6);
	if (!ext->name) {
	    XpmFree(string);
	    ext->lines = NULL;
	    ext->nlines = 0;
	    XpmFreeExtensions(exts, num + 1);
	    return (XpmNoMemory);
	}
	strncpy(ext->name, s + a, l - a - 6);
	XpmFree(string);
	/* now store the related lines */
	xpmNextString(data);
	status = xpmGetString(data, &string, &l);
	if (status != XpmSuccess) {
	    ext->lines = NULL;
	    ext->nlines = 0;
	    XpmFreeExtensions(exts, num + 1);
	    return (status);
	}
	ext->lines = (char **) XpmMalloc(sizeof(char *));
	nlines = 0;
	while ((notstart = strncmp("XPMEXT", string, 6))!=NULL 
	       && (notend = strncmp("XPMENDEXT", string, 9))!=NULL ) {
	    sp = (char **)
		XpmRealloc(ext->lines, (nlines + 1) * sizeof(char *));
	    if (!sp) {
		XpmFree(string);
		ext->nlines = nlines;
		XpmFreeExtensions(exts, num + 1);
		return (XpmNoMemory);
	    }
	    ext->lines = sp;
	    ext->lines[nlines] = string;
	    nlines++;
	    xpmNextString(data);
	    status = xpmGetString(data, &string, &l);
	    if (status != XpmSuccess) {
		ext->nlines = nlines;
		XpmFreeExtensions(exts, num + 1);
		return (status);
	    }
	}
	if (!nlines) {
	    XpmFree(ext->lines);
	    ext->lines = NULL;
	}
	ext->nlines = nlines;
	num++;
    }
    if (!num) {
	XpmFree(string);
	XpmFree(exts);
	exts = NULL;
    } else if (!notend)
	XpmFree(string);
    *nextensions = num;
    *extensions = exts;
    return (XpmSuccess);
}


/* function call in case of error */
#undef RETURN
#define RETURN(status) \
{ \
      goto error; \
}

/*
 * This function parses an Xpm file or data and store the found informations
 * in an an XpmImage structure which is returned.
 */
int
xpmParseData(data, image, info)
    xpmData *data;
    XpmImage *image;
    XpmInfo *info;
{
    /* variables to return */
    unsigned int width, height, ncolors, cpp;
    unsigned int x_hotspot, y_hotspot, hotspot = 0, extensions = 0;
    XpmColor *colorTable = NULL;
    unsigned int *pixelindex = NULL;
    char *hints_cmt = NULL;
    char *colors_cmt = NULL;
    char *pixels_cmt = NULL;

    unsigned int cmts;
    int ErrorStatus;
    xpmHashTable hashtable;

    cmts = info && (info->valuemask & XpmReturnComments);

    /*
     * parse the header
     */
    ErrorStatus = xpmParseHeader(data);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /*
     * read values
     */
    ErrorStatus = xpmParseValues(data, &width, &height, &ncolors, &cpp,
				 &x_hotspot, &y_hotspot, &hotspot,
				 &extensions);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /*
     * store the hints comment line
     */
    if (cmts)
	xpmGetCmt(data, &hints_cmt);

    /*
     * init the hastable
     */
    if (USE_HASHTABLE) {
	ErrorStatus = xpmHashTableInit(&hashtable);
	if (ErrorStatus != XpmSuccess)
	    return (ErrorStatus);
    }

    /*
     * read colors
     */
    ErrorStatus = xpmParseColors(data, ncolors, cpp, &colorTable, &hashtable);
    if (ErrorStatus != XpmSuccess) {
	if (USE_HASHTABLE)
	    xpmHashTableFree(&hashtable);
	RETURN(ErrorStatus);
    }

    /*
     * store the colors comment line
     */
    if (cmts)
	xpmGetCmt(data, &colors_cmt);

    /*
     * read pixels and index them on color number
     */
    ErrorStatus = ParsePixels(data, width, height, ncolors, cpp, colorTable,
			      &hashtable, &pixelindex);

    /*
     * free the hastable
     */
    if (USE_HASHTABLE)
	xpmHashTableFree(&hashtable);

    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus);

    /*
     * store the pixels comment line
     */
    if (cmts)
	xpmGetCmt(data, &pixels_cmt);

    /*
     * parse extensions
     */
    if (info && (info->valuemask & XpmReturnExtensions))
	if (extensions) {
	    ErrorStatus = xpmParseExtensions(data, &info->extensions,
					     &info->nextensions);
	    if (ErrorStatus != XpmSuccess)
		RETURN(ErrorStatus);
	} else {
	    info->extensions = NULL;
	    info->nextensions = 0;
	}

    /*
     * store found informations in the XpmImage structure
     */
    image->width = width;
    image->height = height;
    image->cpp = cpp;
    image->ncolors = ncolors;
    image->colorTable = colorTable;
    image->data = pixelindex;

    if (info) {
	if (cmts) {
	    info->hints_cmt = hints_cmt;
	    info->colors_cmt = colors_cmt;
	    info->pixels_cmt = pixels_cmt;
	}
	if (hotspot) {
	    info->x_hotspot = x_hotspot;
	    info->y_hotspot = y_hotspot;
	    info->valuemask |= XpmHotspot;
	}
    }
    return (XpmSuccess);

/* exit point in case of error, free only locally allocated variables */
error:
    if (colorTable)
	xpmFreeColorTable(colorTable, ncolors);
    if (pixelindex)
	XpmFree(pixelindex);
    if (hints_cmt)
	XpmFree(hints_cmt);
    if (colors_cmt)
	XpmFree(colors_cmt);
    if (pixels_cmt)
	XpmFree(pixels_cmt);

    return(ErrorStatus);
}
