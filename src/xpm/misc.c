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
* misc.c:                                                                     *
*                                                                             *
*  XPM library                                                                *
*  Miscellaneous utilities                                                    *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

#include "xpm34p.h"
#ifdef VMS
#include "sys$library:stat.h"
#include "sys$library:fcntl.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#ifdef FOR_MSW
#include <io.h>
#else
#include <unistd.h>
#endif
#endif

/* 3.2 backward compatibility code */
LFUNC(CreateOldColorTable, int, (XpmColor *ct, int ncolors,
				 XpmColor ***oldct));

LFUNC(FreeOldColorTable, void, (XpmColor **colorTable, int ncolors));

/*
 * Create a colortable compatible with the old style colortable
 */
static int
CreateOldColorTable(XpmColor *ct, int ncolors, XpmColor ***oldct)
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
FreeOldColorTable(XpmColor **colorTable, int ncolors)
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
xpmFreeColorTable(XpmColor *colorTable, int ncolors)
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
XpmFreeExtensions(XpmExtension *extensions, int nextensions)
{
    unsigned int i, j, nlines;
    XpmExtension *ext;
    char **sptr;

    if (extensions) {
	for (i = 0, ext = extensions; i < (unsigned int)nextensions; i++, ext++) {
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
xpmInitAttributes(XpmAttributes *attributes)
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
	attributes->extensions = NULL;
	attributes->nextensions = 0;
    }
}

/*
 * Fill in the XpmAttributes with the XpmImage and the XpmInfo
 */
void
xpmSetAttributes(XpmAttributes *attributes, XpmImage *image, XpmInfo *info)
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
XpmFreeAttributes(XpmAttributes *attributes)
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
    attributes->valuemask = 0;
}

/*
 * Init returned data to free safely later on
 */
void
xpmInitXpmImage(XpmImage *image)
{
    image->ncolors = 0;
    image->colorTable = NULL;
    image->data = NULL;
}

/*
 * Free the XpmImage data which have been allocated
 */
void
XpmFreeXpmImage(XpmImage *image)
{
    if (image->colorTable)
	xpmFreeColorTable(image->colorTable, image->ncolors);
    XpmFree(image->data);
    image->data = NULL;
}

/*
 * Init returned data to free safely later on
 */
void
xpmInitXpmInfo(XpmInfo *info)
{
    if (info) {
	info->hints_cmt = NULL;
	info->colors_cmt = NULL;
	info->pixels_cmt = NULL;
	info->extensions = NULL;
	info->nextensions = 0;
    }
}

/*
 * Free the XpmInfo data which have been allocated
 */
void
XpmFreeXpmInfo(XpmInfo *info)
{
    if (info) {
	if (info->valuemask & XpmComments) {
	    if (info->hints_cmt) {
		XpmFree(info->hints_cmt);
		info->hints_cmt = NULL;
	    }
	    if (info->colors_cmt) {
		XpmFree(info->colors_cmt);
		info->colors_cmt = NULL;
	    }
	    if (info->pixels_cmt) {
		XpmFree(info->pixels_cmt);
		info->pixels_cmt = NULL;
	    }
	}
	if (info->valuemask & XpmReturnExtensions && info->nextensions) {
	    XpmFreeExtensions(info->extensions, info->nextensions);
	    info->extensions = NULL;
	    info->nextensions = 0;
	}
	info->valuemask = 0;
    }
}

/*
 * Set the XpmInfo valuemask to retrieve required info
 */
void
xpmSetInfoMask(XpmInfo *info, XpmAttributes *attributes)
{
    info->valuemask = 0;
    if (attributes->valuemask & XpmReturnInfos)
	info->valuemask |= XpmReturnComments;
    if (attributes->valuemask & XpmReturnExtensions)
	info->valuemask |= XpmReturnExtensions;
}

/*
 * Fill in the XpmInfo with the XpmAttributes
 */
void
xpmSetInfo(XpmInfo *info, XpmAttributes *attributes)
{
    info->valuemask = 0;
    if (attributes->valuemask & XpmInfos) {
	info->valuemask |= XpmComments | XpmColorTable;
	info->hints_cmt = attributes->hints_cmt;
	info->colors_cmt = attributes->colors_cmt;
	info->pixels_cmt = attributes->pixels_cmt;
    }
    if (attributes->valuemask & XpmExtensions) {
	info->valuemask |= XpmExtensions;
	info->extensions = attributes->extensions;
	info->nextensions = attributes->nextensions;
    }
    if (attributes->valuemask & XpmHotspot) {
	info->valuemask |= XpmHotspot;
	info->x_hotspot = attributes->x_hotspot;
	info->y_hotspot = attributes->y_hotspot;
    }
}


#ifdef NEED_STRDUP
/*
 * in case strdup is not provided by the system here is one
 * which does the trick
 */
char *
strdup(char *s1)
{
    char *s2;
    int l = strlen(s1) + 1;

    if (s2 = (char *) XpmMalloc(l))
	strncpy(s2, s1, l);
    return s2;
}

#endif

unsigned int
atoui(register char *p, unsigned int l, unsigned int *ui_return)
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
 *  File / Buffer utilities
 */
int
XpmReadFileToBuffer(char *filename, char **buffer_return)
{
    int fd, fcheck, len;
    char *ptr;
    struct stat stats;
    FILE *fp;

    *buffer_return = NULL;

    fd = open(filename, O_RDONLY);
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
    fcheck = fread(ptr, len, 1, fp);
    fclose(fp);
    if (fcheck != 1) {
	XpmFree(ptr);
	return XpmOpenFailed;
    }
    ptr[len] = '\0';
    *buffer_return = ptr;
    return XpmSuccess;
}

int
XpmWriteFileFromBuffer(char *filename, char *buffer)
{
    int fcheck, len;
    FILE *fp = fopen(filename, "w");

    if (!fp)
	return XpmOpenFailed;

    len = strlen(buffer);
    fcheck = fwrite(buffer, len, 1, fp);
    fclose(fp);
    if (fcheck != 1)
	return XpmOpenFailed;

    return XpmSuccess;
}


/*
 * Small utility function
 */
char *
XpmGetErrorString(int errcode)
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


#ifndef FOR_MSW
void
xpmCreatePixmapFromImage(Display *display, Drawable d, XImage *ximage, Pixmap *pixmap_return)
{
    GC gc;

    *pixmap_return = XCreatePixmap(display, d, ximage->width,
				   ximage->height, ximage->depth);
    gc = XCreateGC(display, *pixmap_return, 0, NULL);

    XPutImage(display, *pixmap_return, gc, ximage, 0, 0, 0, 0,
	      ximage->width, ximage->height);

    XFreeGC(display, gc);
}

void
xpmCreateImageFromPixmap(Display *display, Pixmap pixmap, XImage **ximage_return, unsigned int *width, unsigned int *height)
{
    unsigned int dum;
    int dummy;
    Window win;

    if (*width == 0 && *height == 0)
	XGetGeometry(display, pixmap, &win, &dummy, &dummy,
		     width, height, &dum, &dum);

    *ximage_return = XGetImage(display, pixmap, 0, 0, *width, *height,
			       AllPlanes, ZPixmap);
}

#endif /* FOR_MSW */
