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
* Attrib.c:                                                                   *
*                                                                             *
*  XPM library                                                                *
*  Functions related to the XpmAttributes structure                           *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "XpmI.h"

/* 3.2 backward compatibility code */
LFUNC(CreateOldColorTable, int, (XpmColor *ct, int ncolors,
				 XpmColor ***oldct));

LFUNC(FreeOldColorTable, void, (XpmColor **colorTable, int ncolors));

/*
 * Create a colortable compatible with the old style colortable
 */
static int
CreateOldColorTable(ct, ncolors, oldct)
    XpmColor *ct;
    int ncolors;
    XpmColor ***oldct;
{
    XpmColor **colorTable, **color;
    int a;

    colorTable = (XpmColor **) XpmMalloc(ncolors * sizeof(XpmColor *));
    if (!colorTable) {
	*oldct = NULL;
	return (XpmNoMemory);
    }
    for (a = 0, color = colorTable; a < ncolors; a++, color++, ct++)
	*color = ct;
    *oldct = colorTable;
    return (XpmSuccess);
}

static void
FreeOldColorTable(colorTable, ncolors)
    XpmColor **colorTable;
    int ncolors;
{
    int a, b;
    XpmColor **color;
    char **sptr;

    if (colorTable) {
	for (a = 0, color = colorTable; a < ncolors; a++, color++) {
	    for (b = 0, sptr = (char **) *color; b <= NKEYS; b++, sptr++)
		if (*sptr)
		    XpmFree(*sptr);
	}
	XpmFree(*colorTable);
	XpmFree(colorTable);
    }
}

/* end 3.2 bc */

/*
 * Free the computed color table
 */
void
xpmFreeColorTable(colorTable, ncolors)
    XpmColor *colorTable;
    int ncolors;
{
    int a, b;
    XpmColor *color;
    char **sptr;

    if (colorTable) {
	for (a = 0, color = colorTable; a < ncolors; a++, color++) {
	    for (b = 0, sptr = (char **) color; b <= NKEYS; b++, sptr++)
		if (*sptr)
		    XpmFree(*sptr);
	}
	XpmFree(colorTable);
    }
}

/*
 * Free array of extensions
 */
void
XpmFreeExtensions(extensions, nextensions)
    XpmExtension *extensions;
    int nextensions;
{
    unsigned int i, j, nlines;
    XpmExtension *ext;
    char **sptr;

    if (extensions) {
	for (i = 0, ext = extensions; i < nextensions; i++, ext++) {
	    if (ext->name)
		XpmFree(ext->name);
	    nlines = ext->nlines;
	    for (j = 0, sptr = ext->lines; j < nlines; j++, sptr++)
		if (*sptr)
		    XpmFree(*sptr);
	    if (ext->lines)
		XpmFree(ext->lines);
	}
	XpmFree(extensions);
    }
}

/*
 * Return the XpmAttributes structure size
 */

int
XpmAttributesSize()
{
    return sizeof(XpmAttributes);
}

/*
 * Init returned data to free safely later on
 */
void
xpmInitAttributes(attributes)
    XpmAttributes *attributes;
{
    if (attributes) {
	attributes->pixels = NULL;
	attributes->npixels = 0;
	attributes->colorTable = NULL;
	attributes->ncolors = 0;
/* 3.2 backward compatibility code */
	attributes->hints_cmt = NULL;
	attributes->colors_cmt = NULL;
	attributes->pixels_cmt = NULL;
/* end 3.2 bc */
	if (attributes->valuemask & XpmReturnExtensions) {
	    attributes->extensions = NULL;
	    attributes->nextensions = 0;
	}
	if (attributes->valuemask & XpmReturnAllocPixels) {
	    attributes->alloc_pixels = NULL;
	    attributes->nalloc_pixels = 0;
	}
    }
}

/*
 * Fill in the XpmAttributes with the XpmImage and the XpmInfo
 */
void
xpmSetAttributes(attributes, image, info)
    XpmAttributes *attributes;
    XpmImage *image;
    XpmInfo *info;
{
    if (attributes->valuemask & XpmReturnColorTable) {
	attributes->colorTable = image->colorTable;
	attributes->ncolors = image->ncolors;

	/* avoid deletion of copied data */
	image->ncolors = 0;
	image->colorTable = NULL;
    }
/* 3.2 backward compatibility code */
    else if (attributes->valuemask & XpmReturnInfos) {
	int ErrorStatus;

	ErrorStatus = CreateOldColorTable(image->colorTable, image->ncolors,
					  (XpmColor ***)
					  &attributes->colorTable);

	/* if error just say we can't return requested data */
	if (ErrorStatus != XpmSuccess) {
	    attributes->valuemask &= ~XpmReturnInfos;
	    if (!(attributes->valuemask & XpmReturnPixels)) {
		XpmFree(attributes->pixels);
		attributes->pixels = NULL;
		attributes->npixels = 0;
	    }
	    attributes->ncolors = 0;
	} else {
	    attributes->ncolors = image->ncolors;
	    attributes->hints_cmt = info->hints_cmt;
	    attributes->colors_cmt = info->colors_cmt;
	    attributes->pixels_cmt = info->pixels_cmt;

	    /* avoid deletion of copied data */
	    image->ncolors = 0;
	    image->colorTable = NULL;
	    info->hints_cmt = NULL;
	    info->colors_cmt = NULL;
	    info->pixels_cmt = NULL;
	}
    }
/* end 3.2 bc */
    if (attributes->valuemask & XpmReturnExtensions) {
	attributes->extensions = info->extensions;
	attributes->nextensions = info->nextensions;

	/* avoid deletion of copied data */
	info->extensions = NULL;
	info->nextensions = 0;
    }
    if (info->valuemask & XpmHotspot) {
	attributes->valuemask |= XpmHotspot;
	attributes->x_hotspot = info->x_hotspot;
	attributes->y_hotspot = info->y_hotspot;
    }
    attributes->valuemask |= XpmCharsPerPixel;
    attributes->cpp = image->cpp;
    attributes->valuemask |= XpmSize;
    attributes->width = image->width;
    attributes->height = image->height;
}

/*
 * Free the XpmAttributes structure members
 * but the structure itself
 */
void
XpmFreeAttributes(attributes)
    XpmAttributes *attributes;
{
    if (attributes->valuemask & XpmReturnPixels && attributes->npixels) {
	XpmFree(attributes->pixels);
	attributes->pixels = NULL;
	attributes->npixels = 0;
    }
    if (attributes->valuemask & XpmReturnColorTable) {
	xpmFreeColorTable(attributes->colorTable, attributes->ncolors);
	attributes->colorTable = NULL;
	attributes->ncolors = 0;
    }
/* 3.2 backward compatibility code */
    else if (attributes->valuemask & XpmInfos) {
	if (attributes->colorTable) {
	    FreeOldColorTable((XpmColor **) attributes->colorTable,
			      attributes->ncolors);
	    attributes->colorTable = NULL;
	    attributes->ncolors = 0;
	}
	if (attributes->hints_cmt) {
	    XpmFree(attributes->hints_cmt);
	    attributes->hints_cmt = NULL;
	}
	if (attributes->colors_cmt) {
	    XpmFree(attributes->colors_cmt);
	    attributes->colors_cmt = NULL;
	}
	if (attributes->pixels_cmt) {
	    XpmFree(attributes->pixels_cmt);
	    attributes->pixels_cmt = NULL;
	}
	if (attributes->pixels) {
	    XpmFree(attributes->pixels);
	    attributes->pixels = NULL;
	    attributes->npixels = 0;
	}
    }
/* end 3.2 bc */
    if (attributes->valuemask & XpmReturnExtensions
	&& attributes->nextensions) {
	XpmFreeExtensions(attributes->extensions, attributes->nextensions);
	attributes->extensions = NULL;
	attributes->nextensions = 0;
    }
    if (attributes->valuemask & XpmReturnAllocPixels
	&& attributes->nalloc_pixels) {
	XpmFree(attributes->alloc_pixels);
	attributes->alloc_pixels = NULL;
	attributes->nalloc_pixels = 0;
    }
    attributes->valuemask = 0;
}
