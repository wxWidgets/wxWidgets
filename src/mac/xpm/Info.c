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
*  Info.c:                                                                    *
*                                                                             *
*  XPM library                                                                *
*  Functions related to the XpmInfo structure.                                *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "XpmI.h"

/*
 * Init returned data to free safely later on
 */
void
xpmInitXpmInfo(info)
    XpmInfo *info;
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
XpmFreeXpmInfo(info)
    XpmInfo *info;
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
xpmSetInfoMask(info, attributes)
    XpmInfo *info;
    XpmAttributes *attributes;
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
xpmSetInfo(info, attributes)
    XpmInfo *info;
    XpmAttributes *attributes;
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
