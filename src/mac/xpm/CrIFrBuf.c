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
*  CrIFrBuf.c:                                                                *
*                                                                             *
*  XPM library                                                                *
*  Parse an Xpm buffer (file in memory) and create the image and possibly its *
*  mask                                                                       *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "XpmI.h"

LFUNC(OpenBuffer, void, (char *buffer, xpmData *mdata));

int
XpmCreateImageFromBuffer(display, buffer, image_return,
			 shapeimage_return, attributes)
    Display *display;
    char *buffer;
    XImage **image_return;
    XImage **shapeimage_return;
    XpmAttributes *attributes;
{
    XpmImage image;
    XpmInfo info;
    int ErrorStatus;
    xpmData mdata;

    xpmInitXpmImage(&image);
    xpmInitXpmInfo(&info);

    /* open buffer to read */
    OpenBuffer(buffer, &mdata);

    /* create the XImage from the XpmData */
    if (attributes) {
	xpmInitAttributes(attributes);
	xpmSetInfoMask(&info, attributes);
	ErrorStatus = xpmParseDataAndCreate(display, &mdata,
					    image_return, shapeimage_return,
					    &image, &info, attributes);
    } else
	ErrorStatus = xpmParseDataAndCreate(display, &mdata,
					    image_return, shapeimage_return,
					    &image, NULL, attributes);
    if (attributes) {
	if (ErrorStatus >= 0)		/* no fatal error */
	    xpmSetAttributes(attributes, &image, &info);
	XpmFreeXpmInfo(&info);
    }

    /* free the XpmImage */
    XpmFreeXpmImage(&image);

    return (ErrorStatus);
}

int
XpmCreateXpmImageFromBuffer(buffer, image, info)
    char *buffer;
    XpmImage *image;
    XpmInfo *info;
{
    xpmData mdata;
    int ErrorStatus;

    /* init returned values */
    xpmInitXpmImage(image);
    xpmInitXpmInfo(info);

    /* open buffer to read */
    OpenBuffer(buffer, &mdata);

    /* create the XpmImage from the XpmData */
    ErrorStatus = xpmParseData(&mdata, image, info);

    return (ErrorStatus);
}

/*
 * open the given buffer to be read or written as an xpmData which is returned
 */
static void
OpenBuffer(buffer, mdata)
    char *buffer;
    xpmData *mdata;
{
    mdata->type = XPMBUFFER;
    mdata->cptr = buffer;
    mdata->CommentLength = 0;
}
