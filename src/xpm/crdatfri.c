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
*  CrDataFI.c:                                                                *
*                                                                             *
*  XPM library                                                                *
*  Scan an image and possibly its mask and create an XPM array                *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "XpmI.h"

LFUNC(CreateColors, int, (char **dataptr, unsigned int *data_size,
			  XpmColor *colors, unsigned int ncolors,
			  unsigned int cpp));

LFUNC(CreatePixels, void, (char **dataptr, unsigned int width,
			   unsigned int height, unsigned int cpp,
			   unsigned int *pixels, XpmColor *colors));

LFUNC(CountExtensions, void, (XpmExtension *ext, unsigned int num,
			      unsigned int *ext_size,
			      unsigned int *ext_nlines));

LFUNC(CreateExtensions, void, (char **dataptr, unsigned int offset,
			       XpmExtension *ext, unsigned int num,
			       unsigned int ext_nlines));

int
XpmCreateDataFromImage(display, data_return, image, shapeimage, attributes)
    Display *display;
    char ***data_return;
    XImage *image;
    XImage *shapeimage;
    XpmAttributes *attributes;
{
    XpmImage xpmimage;
    XpmInfo info;
    int ErrorStatus;

    /* initialize return value */
    if (data_return)
	*data_return = NULL;

    /* create an XpmImage from the image */
    ErrorStatus = XpmCreateXpmImageFromImage(display, image, shapeimage,
					     &xpmimage, attributes);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /* create the data from the XpmImage */
    if (attributes) {
	xpmSetInfo(&info, attributes);
	ErrorStatus = XpmCreateDataFromXpmImage(data_return, &xpmimage, &info);
    } else
	ErrorStatus = XpmCreateDataFromXpmImage(data_return, &xpmimage, NULL);

    /* free the XpmImage */
    XpmFreeXpmImage(&xpmimage);

    return (ErrorStatus);
}

#undef RETURN
#define RETURN(status) \
{ \
      ErrorStatus = status; \
      goto exit; \
}

int
XpmCreateDataFromXpmImage(data_return, image, info)
    char ***data_return;
    XpmImage *image;
    XpmInfo *info;
{
    /* calculation variables */
    int ErrorStatus;
    char buf[BUFSIZ];
    char **header = NULL, **data, **sptr, **sptr2, *s;
    unsigned int header_size, header_nlines;
    unsigned int data_size, data_nlines;
    unsigned int extensions = 0, ext_size = 0, ext_nlines = 0;
    unsigned int offset, l, n;

    *data_return = NULL;

    extensions = info && (info->valuemask & XpmExtensions)
	&& info->nextensions;

    /* compute the number of extensions lines and size */
    if (extensions)
	CountExtensions(info->extensions, info->nextensions,
			&ext_size, &ext_nlines);

    /*
     * alloc a temporary array of char pointer for the header section which
     * is the hints line + the color table lines
     */
    header_nlines = 1 + image->ncolors;
    header_size = sizeof(char *) * header_nlines;
    header = (char **) XpmCalloc(header_size, sizeof(char *));
    if (!header)
	return (XpmNoMemory);

    /* print the hints line */
    s = buf;
#ifndef VOID_SPRINTF
    s +=
#endif
    sprintf(s, "%d %d %d %d", image->width, image->height,
	    image->ncolors, image->cpp);
#ifdef VOID_SPRINTF
    s += strlen(s);
#endif

    if (info && (info->valuemask & XpmHotspot)) {
#ifndef VOID_SPRINTF
	s +=
#endif
	sprintf(s, " %d %d", info->x_hotspot, info->y_hotspot);
#ifdef VOID_SPRINTF
	s += strlen(s);
#endif
    }
    if (extensions) {
	strcpy(s, " XPMEXT");
	s += 7;
    }
    l = s - buf + 1;
    *header = (char *) XpmMalloc(l);
    if (!*header)
	RETURN(XpmNoMemory);
    header_size += l;
    strcpy(*header, buf);

    /* print colors */
    ErrorStatus = CreateColors(header + 1, &header_size,
			       image->colorTable, image->ncolors, image->cpp);

    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus);

    /* now we know the size needed, alloc the data and copy the header lines */
    offset = image->width * image->cpp + 1;
    data_size = header_size + (image->height + ext_nlines) * sizeof(char *)
	+ image->height * offset + ext_size;

    data = (char **) XpmMalloc(data_size);
    if (!data)
	RETURN(XpmNoMemory);

    data_nlines = header_nlines + image->height + ext_nlines;
    *data = (char *) (data + data_nlines);
    n = image->ncolors;
    for (l = 0, sptr = data, sptr2 = header; l <= n; l++, sptr++, sptr2++) {
	strcpy(*sptr, *sptr2);
	*(sptr + 1) = *sptr + strlen(*sptr2) + 1;
    }

    /* print pixels */
    data[header_nlines] = (char *) data + header_size
	+ (image->height + ext_nlines) * sizeof(char *);

    CreatePixels(data + header_nlines, image->width, image->height,
		 image->cpp, image->data, image->colorTable);

    /* print extensions */
    if (extensions)
	CreateExtensions(data + header_nlines + image->height - 1, offset,
			 info->extensions, info->nextensions,
			 ext_nlines);

    *data_return = data;
    ErrorStatus = XpmSuccess;

/* exit point, free only locally allocated variables */
exit:
    if (header) {
	for (l = 0; l < header_nlines; l++)
	    if (header[l])
		XpmFree(header[l]);
		XpmFree(header);
    }
    return(ErrorStatus);
}

static int
CreateColors(dataptr, data_size, colors, ncolors, cpp)
    char **dataptr;
    unsigned int *data_size;
    XpmColor *colors;
    unsigned int ncolors;
    unsigned int cpp;
{
    char buf[BUFSIZ];
    unsigned int a, key, l;
    char *s, *s2;
    char **defaults;

    for (a = 0; a < ncolors; a++, colors++, dataptr++) {

	defaults = (char **) colors;
	strncpy(buf, *defaults++, cpp);
	s = buf + cpp;

	for (key = 1; key <= NKEYS; key++, defaults++) {
	    if (s2 = *defaults) {
#ifndef VOID_SPRINTF
		s +=
#endif
		sprintf(s, "\t%s %s", xpmColorKeys[key - 1], s2);
#ifdef VOID_SPRINTF
		s += strlen(s);
#endif
	    }
	}
	l = s - buf + 1;
	s = (char *) XpmMalloc(l);
	if (!s)
	    return (XpmNoMemory);
	*data_size += l;
	*dataptr = strcpy(s, buf);
    }
    return (XpmSuccess);
}

static void
CreatePixels(dataptr, width, height, cpp, pixels, colors)
    char **dataptr;
    unsigned int width;
    unsigned int height;
    unsigned int cpp;
    unsigned int *pixels;
    XpmColor *colors;
{
    char *s;
    unsigned int x, y, h, offset;

    h = height - 1;
    offset = width * cpp + 1;
    for (y = 0; y < h; y++, dataptr++) {
	s = *dataptr;
	for (x = 0; x < width; x++, pixels++) {
	    strncpy(s, colors[*pixels].string, cpp);
	    s += cpp;
	}
	*s = '\0';
	*(dataptr + 1) = *dataptr + offset;
    }
    /* duplicate some code to avoid a test in the loop */
    s = *dataptr;
    for (x = 0; x < width; x++, pixels++) {
	strncpy(s, colors[*pixels].string, cpp);
	s += cpp;
    }
    *s = '\0';
}

static void
CountExtensions(ext, num, ext_size, ext_nlines)
    XpmExtension *ext;
    unsigned int num;
    unsigned int *ext_size;
    unsigned int *ext_nlines;
{
    unsigned int x, y, a, size, nlines;
    char **line;

    size = 0;
    nlines = 0;
    for (x = 0; x < num; x++, ext++) {
	/* 1 for the name */
	nlines += ext->nlines + 1;
	/* 8 = 7 (for "XPMEXT ") + 1 (for 0) */
	size += strlen(ext->name) + 8;
	a = ext->nlines;
	for (y = 0, line = ext->lines; y < a; y++, line++)
	    size += strlen(*line) + 1;
    }
    /* 10 and 1 are for the ending "XPMENDEXT" */
    *ext_size = size + 10;
    *ext_nlines = nlines + 1;
}

static void
CreateExtensions(dataptr, offset, ext, num, ext_nlines)
    char **dataptr;
    unsigned int offset;
    XpmExtension *ext;
    unsigned int num;
    unsigned int ext_nlines;
{
    unsigned int x, y, a, b;
    char **line;

    *(dataptr + 1) = *dataptr + offset;
    dataptr++;
    a = 0;
    for (x = 0; x < num; x++, ext++) {
	sprintf(*dataptr, "XPMEXT %s", ext->name);
	a++;
	if (a < ext_nlines)
	    *(dataptr + 1) = *dataptr + strlen(ext->name) + 8;
	dataptr++;
	b = ext->nlines;
	for (y = 0, line = ext->lines; y < b; y++, line++) {
	    strcpy(*dataptr, *line);
	    a++;
	    if (a < ext_nlines)
		*(dataptr + 1) = *dataptr + strlen(*line) + 1;
	    dataptr++;
	}
    }
    strcpy(*dataptr, "XPMENDEXT");
}
